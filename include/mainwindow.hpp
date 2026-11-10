#pragma once

#include <red.hpp>
#include <resultado.hpp>

#include <QFutureWatcher>
#include <QMainWindow>

#include <array>
#include <cstddef>
#include <optional>
#include <vector>

class QLabel;
class QPushButton;
class QTableWidget;

struct Ejecucion {
    std::size_t algoritmo;
    Resultado resultado;
    double ms;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void abrir();
    void generar();
    void guardar();
    void terminado();

private:
    void cargar(Red red, const QString& nombre);
    void ejecutar(const std::vector<std::size_t>& algoritmos);
    void actualizarResumen();
    void habilitar(bool corriendo);

    std::optional<Red> red_;
    std::array<std::optional<Ejecucion>, 3> resultados_;
    QFutureWatcher<std::vector<Ejecucion>> observador_;
    QLabel* etiquetaRed_;
    QTableWidget* tabla_;
    QTableWidget* resumen_;
    QPushButton* botonAbrir_;
    QPushButton* botonGenerar_;
    QPushButton* botonFB_;
    QPushButton* botonVoraz_;
    QPushButton* botonDinamica_;
    QPushButton* botonComparar_;
    QPushButton* botonGuardar_;
};
