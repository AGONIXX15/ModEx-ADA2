#pragma once

#include <red.hpp>
#include <resultado.hpp>

#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QMainWindow>

#include <optional>

class QLabel;
class QPushButton;
class QTableWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void abrir();
    void guardar();
    void terminado();

private:
    using Algoritmo = Resultado (*)(const Red&);

    void ejecutar(Algoritmo algoritmo);
    void habilitar(bool corriendo);

    std::optional<Red> red_;
    std::optional<Resultado> resultado_;
    QElapsedTimer cronometro_;
    QFutureWatcher<Resultado> observador_;
    QLabel* etiquetaRed_;
    QLabel* etiquetaResultado_;
    QTableWidget* tabla_;
    QPushButton* botonAbrir_;
    QPushButton* botonFB_;
    QPushButton* botonVoraz_;
    QPushButton* botonDinamica_;
    QPushButton* botonGuardar_;
};
