#include <mainwindow.hpp>

#include <estrategias/dinamica.hpp>
#include <estrategias/fuerzabruta.hpp>
#include <estrategias/voraz.hpp>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QtConcurrent>

#include <cmath>
#include <algorithm>
#include <exception>
#include <filesystem>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      etiquetaRed_(new QLabel("Sin archivo cargado")),
      etiquetaResultado_(new QLabel),
      tabla_(new QTableWidget(0, 5)),
      botonAbrir_(new QPushButton("Abrir")),
      botonFB_(new QPushButton("Fuerza bruta")),
      botonVoraz_(new QPushButton("Voraz")),
      botonDinamica_(new QPushButton("Dinamica")),
      botonGuardar_(new QPushButton("Guardar salida")) {
    tabla_->setHorizontalHeaderLabels({"opinion", "receptividad", "influencia", "costo", "moderado"});
    tabla_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto* botones = new QHBoxLayout;
    botones->addWidget(botonAbrir_);
    botones->addWidget(botonFB_);
    botones->addWidget(botonVoraz_);
    botones->addWidget(botonDinamica_);
    botones->addWidget(botonGuardar_);

    auto* diseno = new QVBoxLayout;
    diseno->addLayout(botones);
    diseno->addWidget(etiquetaRed_);
    diseno->addWidget(tabla_);
    diseno->addWidget(etiquetaResultado_);

    auto* central = new QWidget;
    central->setLayout(diseno);
    setCentralWidget(central);
    resize(800, 500);

    habilitar(false);

    connect(botonAbrir_, &QPushButton::clicked, this, &MainWindow::abrir);
    connect(botonGuardar_, &QPushButton::clicked, this, &MainWindow::guardar);
    connect(botonFB_, &QPushButton::clicked, this, [this] { ejecutar(modexFB); });
    connect(botonVoraz_, &QPushButton::clicked, this, [this] { ejecutar(modexV); });
    connect(botonDinamica_, &QPushButton::clicked, this, [this] { ejecutar(modexPD); });
    connect(&observador_, &QFutureWatcher<Resultado>::finished, this, &MainWindow::terminado);
}

void MainWindow::habilitar(bool corriendo) {
    bool hayRed = red_.has_value();
    botonAbrir_->setEnabled(!corriendo);
    botonFB_->setEnabled(!corriendo && hayRed && red_->getNumAgentes() <= 20);
    botonVoraz_->setEnabled(!corriendo && hayRed);
    botonDinamica_->setEnabled(!corriendo && hayRed);
    botonGuardar_->setEnabled(!corriendo && resultado_.has_value());
}

void MainWindow::abrir() {
    QString ruta = QFileDialog::getOpenFileName(this, "Abrir archivo de entrada", QString(), "Texto (*.txt);;Todos (*)");
    if (ruta.isEmpty()) {
        return;
    }
    try {
        red_ = leerRed(std::filesystem::path(ruta.toStdWString()));
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        return;
    }
    resultado_.reset();
    etiquetaResultado_->clear();
    etiquetaRed_->setText(QString("n = %1, R_max = %2").arg(red_->getNumAgentes()).arg(red_->getRmax()));
    const std::vector<Agente>& agentes = red_->getAgentes();
    tabla_->setRowCount(static_cast<int>(agentes.size()));
    for (std::size_t i = 0; i < agentes.size(); ++i) {
        int fila = static_cast<int>(i);
        double op = agentes[i].getOpinion();
        long long costo = techo(std::abs(op) * (1 - agentes[i].getReceptividad()) * agentes[i].getInfluencia());
        tabla_->setItem(fila, 0, new QTableWidgetItem(QString::number(agentes[i].getOpinion())));
        tabla_->setItem(fila, 1, new QTableWidgetItem(QString::number(agentes[i].getReceptividad())));
        tabla_->setItem(fila, 2, new QTableWidgetItem(QString::number(agentes[i].getInfluencia())));
        tabla_->setItem(fila, 3, new QTableWidgetItem(QString::number(costo)));
        tabla_->setItem(fila, 4, new QTableWidgetItem);
    }
    habilitar(false);
}

void MainWindow::ejecutar(Algoritmo algoritmo) {
    habilitar(true);
    Red red = *red_;
    cronometro_.start();
    observador_.setFuture(QtConcurrent::run([red, algoritmo] { return algoritmo(red); }));
}

void MainWindow::terminado() {
    qint64 ms = cronometro_.elapsed();
    try {
        resultado_ = observador_.result();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        resultado_.reset();
        habilitar(false);
        return;
    }
    const std::vector<int>& E = resultado_->E;
    if (E.size() != red_->getNumAgentes()) {
        etiquetaResultado_->setText("sin estrategia aplicable");
        resultado_.reset();
        habilitar(false);
        return;
    }
    for (std::size_t i = 0; i < E.size(); ++i) {
        tabla_->item(static_cast<int>(i), 4)->setText(E[i] ? "si" : "no");
    }
    long long esf = esfuerzo(*red_, E);
    double ext = red_->getExtremismo(E);
    bool coincide = esf == resultado_->esfuerzo && std::abs(ext - resultado_->extremismo) <= 1e-9 * std::max(1.0, std::abs(ext));
    bool dentro = resultado_->esfuerzo <= red_->getRmax();
    etiquetaResultado_->setText(
        QString("Ext = %1 | Esf = %2 | Tiempo = %3 ms\nVerificacion: recalculo %4 (Ext = %5, Esf = %6); Esf <= R_max: %7")
            .arg(resultado_->extremismo, 0, 'f', 6)
            .arg(resultado_->esfuerzo)
            .arg(ms)
            .arg(coincide ? "coincide" : "NO coincide")
            .arg(ext, 0, 'f', 6)
            .arg(esf)
            .arg(dentro ? "si" : "no"));
    habilitar(false);
}

void MainWindow::guardar() {
    QString ruta = QFileDialog::getSaveFileName(this, "Guardar salida", "salida.txt", "Texto (*.txt);;Todos (*)");
    if (ruta.isEmpty()) {
        return;
    }
    try {
        escribirSalida(std::filesystem::path(ruta.toStdWString()), *resultado_);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}
