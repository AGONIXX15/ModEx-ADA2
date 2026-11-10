#include <mainwindow.hpp>

#include <estrategias/dinamica.hpp>
#include <estrategias/fuerzabruta.hpp>
#include <estrategias/voraz.hpp>

#include <QElapsedTimer>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QtConcurrent>

#include <algorithm>
#include <cmath>
#include <exception>
#include <filesystem>
#include <limits>
#include <random>

namespace {

struct InfoAlgoritmo {
    const char* nombre;
    Resultado (*funcion)(const Red&);
};

constexpr std::size_t FB = 0;
constexpr std::size_t VORAZ = 1;
constexpr std::size_t PD = 2;
const std::array<InfoAlgoritmo, 3> ALGORITMOS = {{{"Fuerza bruta", modexFB}, {"Voraz", modexV}, {"Dinamica", modexPD}}};

// Columnas de la tabla de agentes: las 4 primeras son datos, luego una por algoritmo.
constexpr int COLUMNA_PRIMER_ALGORITMO = 4;
// Por encima de este n la fuerza bruta (2^n estrategias) tarda demasiado.
constexpr std::size_t MAX_FUERZA_BRUTA = 25;
constexpr int MAX_GENERAR = 2000;

bool iguales(double a, double b) {
    return std::abs(a - b) <= 1e-9 * std::max(1.0, std::abs(b));
}

QTableWidgetItem* celda(const QString& texto) {
    auto* item = new QTableWidgetItem(texto);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      etiquetaRed_(new QLabel("Sin red cargada")),
      tabla_(new QTableWidget(0, COLUMNA_PRIMER_ALGORITMO + 3)),
      resumen_(new QTableWidget(3, 7)),
      botonAbrir_(new QPushButton("Abrir")),
      botonGenerar_(new QPushButton("Generar caso")),
      botonFB_(new QPushButton("Fuerza bruta")),
      botonVoraz_(new QPushButton("Voraz")),
      botonDinamica_(new QPushButton("Dinamica")),
      botonComparar_(new QPushButton("Comparar")),
      botonGuardar_(new QPushButton("Guardar salida")) {
    setWindowTitle("ModEx");
    tabla_->setHorizontalHeaderLabels({"opinion", "receptividad", "influencia", "costo moderar", "FB", "Voraz", "Dinamica"});
    tabla_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    resumen_->setHorizontalHeaderLabels({"Algoritmo", "Ext", "Esf", "Esf <= R_max", "Recalculo", "Tiempo (ms)", "Optimo (vs PD)"});
    resumen_->verticalHeader()->setVisible(false);
    resumen_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resumen_->setSelectionBehavior(QAbstractItemView::SelectRows);
    resumen_->setSelectionMode(QAbstractItemView::SingleSelection);
    resumen_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (std::size_t a = 0; a < ALGORITMOS.size(); ++a) {
        resumen_->setItem(static_cast<int>(a), 0, celda(ALGORITMOS[a].nombre));
    }
    resumen_->setFixedHeight(resumen_->horizontalHeader()->height() + 3 * resumen_->verticalHeader()->defaultSectionSize() + 4);

    auto* botones = new QHBoxLayout;
    for (QPushButton* b : {botonAbrir_, botonGenerar_, botonFB_, botonVoraz_, botonDinamica_, botonComparar_, botonGuardar_}) {
        botones->addWidget(b);
    }

    auto* diseno = new QVBoxLayout;
    diseno->addLayout(botones);
    diseno->addWidget(etiquetaRed_);
    diseno->addWidget(tabla_);
    diseno->addWidget(new QLabel("Resultados (seleccione una fila para guardar su salida):"));
    diseno->addWidget(resumen_);

    auto* central = new QWidget;
    central->setLayout(diseno);
    setCentralWidget(central);
    resize(1000, 650);

    habilitar(false);

    connect(botonAbrir_, &QPushButton::clicked, this, &MainWindow::abrir);
    connect(botonGenerar_, &QPushButton::clicked, this, &MainWindow::generar);
    connect(botonGuardar_, &QPushButton::clicked, this, &MainWindow::guardar);
    connect(botonFB_, &QPushButton::clicked, this, [this] { ejecutar({FB}); });
    connect(botonVoraz_, &QPushButton::clicked, this, [this] { ejecutar({VORAZ}); });
    connect(botonDinamica_, &QPushButton::clicked, this, [this] { ejecutar({PD}); });
    connect(botonComparar_, &QPushButton::clicked, this, [this] {
        if (red_->getNumAgentes() <= MAX_FUERZA_BRUTA) {
            ejecutar({FB, VORAZ, PD});
        } else {
            ejecutar({VORAZ, PD});
        }
    });
    connect(resumen_, &QTableWidget::itemSelectionChanged, this, [this] { habilitar(observador_.isRunning()); });
    connect(&observador_, &QFutureWatcher<std::vector<Ejecucion>>::finished, this, &MainWindow::terminado);
}

void MainWindow::habilitar(bool corriendo) {
    bool hayRed = red_.has_value();
    botonAbrir_->setEnabled(!corriendo);
    botonGenerar_->setEnabled(!corriendo);
    botonFB_->setEnabled(!corriendo && hayRed && red_->getNumAgentes() <= MAX_FUERZA_BRUTA);
    botonFB_->setToolTip(QString("Disponible para n <= %1").arg(MAX_FUERZA_BRUTA));
    botonVoraz_->setEnabled(!corriendo && hayRed);
    botonDinamica_->setEnabled(!corriendo && hayRed);
    botonComparar_->setEnabled(!corriendo && hayRed);
    int fila = resumen_->currentRow();
    bool haySeleccion = fila >= 0 && !resumen_->selectedItems().isEmpty() && resultados_[static_cast<std::size_t>(fila)].has_value();
    botonGuardar_->setEnabled(!corriendo && haySeleccion);
}

void MainWindow::abrir() {
    QString ruta = QFileDialog::getOpenFileName(this, "Abrir archivo de entrada", QString(), "Texto (*.txt);;Todos (*)");
    if (ruta.isEmpty()) {
        return;
    }
    try {
        cargar(leerRed(std::filesystem::path(ruta.toStdWString())), QFileInfo(ruta).fileName());
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void MainWindow::generar() {
    bool ok = false;
    int n = QInputDialog::getInt(this, "Generar caso de prueba", "Numero de agentes n:", 10, 1, MAX_GENERAR, 1, &ok);
    if (!ok) {
        return;
    }
    std::mt19937 generador(std::random_device{}());
    std::vector<Agente> agentes = generarAgentes(static_cast<std::size_t>(n), generador);
    long long total = esfuerzoTotal(Red(agentes, 0));
    int rMax = QInputDialog::getInt(this, "Generar caso de prueba",
                                    QString("R_max (moderar a todos cuesta %1):").arg(total),
                                    static_cast<int>(total / 2), 0, std::numeric_limits<int>::max(), 1, &ok);
    if (!ok) {
        return;
    }
    Red red(std::move(agentes), rMax);
    QString ruta = QFileDialog::getSaveFileName(this, "Guardar caso generado", QString("caso_n%1.txt").arg(n), "Texto (*.txt);;Todos (*)");
    QString nombre = "(generado, sin guardar)";
    if (!ruta.isEmpty()) {
        try {
            escribirRed(std::filesystem::path(ruta.toStdWString()), red);
            nombre = QFileInfo(ruta).fileName();
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", e.what());
        }
    }
    cargar(std::move(red), nombre);
}

void MainWindow::cargar(Red red, const QString& nombre) {
    red_ = std::move(red);
    for (std::optional<Ejecucion>& r : resultados_) {
        r.reset();
    }
    etiquetaRed_->setText(QString("Red: %1 | n = %2 | R_max = %3 | Esfuerzo de moderar a todos = %4 | Ext inicial = %5")
                              .arg(nombre)
                              .arg(red_->getNumAgentes())
                              .arg(red_->getRmax())
                              .arg(esfuerzoTotal(*red_))
                              .arg(red_->getExtremismo(), 0, 'f', 6));
    const std::vector<Agente>& agentes = red_->getAgentes();
    tabla_->clearContents();
    tabla_->setRowCount(static_cast<int>(agentes.size()));
    for (std::size_t i = 0; i < agentes.size(); ++i) {
        int fila = static_cast<int>(i);
        tabla_->setItem(fila, 0, celda(QString::number(agentes[i].getOpinion())));
        tabla_->setItem(fila, 1, celda(QString::number(agentes[i].getReceptividad())));
        tabla_->setItem(fila, 2, celda(QString::number(agentes[i].getInfluencia())));
        tabla_->setItem(fila, 3, celda(QString::number(costoModerar(agentes[i]))));
        for (std::size_t a = 0; a < ALGORITMOS.size(); ++a) {
            tabla_->setItem(fila, COLUMNA_PRIMER_ALGORITMO + static_cast<int>(a), celda(QString()));
        }
    }
    resumen_->clearSelection();
    actualizarResumen();
    habilitar(false);
}

void MainWindow::ejecutar(const std::vector<std::size_t>& algoritmos) {
    habilitar(true);
    Red red = *red_;
    observador_.setFuture(QtConcurrent::run([red, algoritmos] {
        std::vector<Ejecucion> salida;
        for (std::size_t a : algoritmos) {
            QElapsedTimer cronometro;
            cronometro.start();
            Resultado r = ALGORITMOS[a].funcion(red);
            double ms = static_cast<double>(cronometro.nsecsElapsed()) / 1e6;
            salida.push_back(Ejecucion{a, std::move(r), ms});
        }
        return salida;
    }));
}

void MainWindow::terminado() {
    std::vector<Ejecucion> ejecuciones;
    try {
        ejecuciones = observador_.result();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
        habilitar(false);
        return;
    }
    for (const Ejecucion& e : ejecuciones) {
        const std::vector<int>& E = e.resultado.E;
        if (E.size() != red_->getNumAgentes()) {
            continue;
        }
        resultados_[e.algoritmo] = e;
        int columna = COLUMNA_PRIMER_ALGORITMO + static_cast<int>(e.algoritmo);
        for (std::size_t i = 0; i < E.size(); ++i) {
            tabla_->item(static_cast<int>(i), columna)->setText(E[i] ? "1" : "0");
        }
    }
    actualizarResumen();
    if (!ejecuciones.empty()) {
        resumen_->selectRow(static_cast<int>(ejecuciones.back().algoritmo));
    }
    habilitar(false);
}

void MainWindow::actualizarResumen() {
    const std::optional<Ejecucion>& dinamica = resultados_[PD];
    for (std::size_t a = 0; a < ALGORITMOS.size(); ++a) {
        int fila = static_cast<int>(a);
        const std::optional<Ejecucion>& e = resultados_[a];
        if (!e.has_value()) {
            for (int c = 1; c < resumen_->columnCount(); ++c) {
                resumen_->setItem(fila, c, celda("-"));
            }
            continue;
        }
        const Resultado& r = e->resultado;
        // Se recalculan Esf y Ext a partir de E para comprobar el costo reportado.
        long long esf = esfuerzo(*red_, r.E);
        double ext = red_->getExtremismo(r.E);
        bool coincide = esf == r.esfuerzo && iguales(ext, r.extremismo);
        QString optimo = "-";
        if (dinamica.has_value()) {
            double diferencia = r.extremismo - dinamica->resultado.extremismo;
            optimo = iguales(r.extremismo, dinamica->resultado.extremismo) ? "si" : QString("no (+%1)").arg(diferencia, 0, 'f', 6);
        }
        resumen_->setItem(fila, 1, celda(QString::number(r.extremismo, 'f', 6)));
        resumen_->setItem(fila, 2, celda(QString::number(r.esfuerzo)));
        resumen_->setItem(fila, 3, celda(r.esfuerzo <= red_->getRmax() ? "si" : "NO"));
        resumen_->setItem(fila, 4, celda(coincide ? "coincide" : "NO coincide"));
        resumen_->setItem(fila, 5, celda(QString::number(e->ms, 'f', 3)));
        resumen_->setItem(fila, 6, celda(optimo));
    }
}

void MainWindow::guardar() {
    int fila = resumen_->currentRow();
    if (fila < 0 || !resultados_[static_cast<std::size_t>(fila)].has_value()) {
        return;
    }
    const Ejecucion& e = *resultados_[static_cast<std::size_t>(fila)];
    QString sugerido = QString("salida_%1.txt").arg(QString(ALGORITMOS[e.algoritmo].nombre).replace(' ', '_').toLower());
    QString ruta = QFileDialog::getSaveFileName(this, QString("Guardar salida de %1").arg(ALGORITMOS[e.algoritmo].nombre), sugerido,
                                                "Texto (*.txt);;Todos (*)");
    if (ruta.isEmpty()) {
        return;
    }
    try {
        escribirSalida(std::filesystem::path(ruta.toStdWString()), e.resultado);
    } catch (const std::exception& ex) {
        QMessageBox::critical(this, "Error", ex.what());
    }
}
