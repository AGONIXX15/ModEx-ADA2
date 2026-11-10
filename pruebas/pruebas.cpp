#include <estrategias/dinamica.hpp>
#include <estrategias/fuerzabruta.hpp>
#include <estrategias/voraz.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

struct Caso {
    std::vector<Agente> agentes;
    long long r;
};

int fallas = 0;

void verificar(bool condicion, const std::string& mensaje) {
    if (!condicion) {
        ++fallas;
        std::cout << "FALLA: " << mensaje << '\n';
    }
}

bool iguales(double a, double b) {
    return std::abs(a - b) <= 1e-9 * std::max(1.0, std::abs(b));
}

std::string texto(const std::vector<int>& E) {
    std::string s;
    for (std::size_t j = 0; j < E.size(); ++j) {
        s += (j ? "," : "") + std::to_string(E[j]);
    }
    return s;
}

// Comprueba que el resultado sea aplicable y que Esf y Ext correspondan a E.
void verificarConsistencia(const Red& red, const Resultado& r, const std::string& nombre) {
    verificar(r.E.size() == red.getNumAgentes(), nombre + ": tamano de E");
    if (r.E.size() != red.getNumAgentes()) {
        return;
    }
    verificar(r.esfuerzo == esfuerzo(red, r.E), nombre + ": esfuerzo no coincide con E");
    verificar(iguales(r.extremismo, red.getExtremismo(r.E)), nombre + ": extremismo no coincide con E");
    verificar(r.esfuerzo <= red.getRmax(), nombre + ": estrategia no aplicable");
}

void pruebasCosto() {
    verificar(costoModerar(Agente(10, 0.7, 1)) == 3, "ceil(10 * 0.3 * 1) debe ser 3");
    verificar(costoModerar(Agente(20, 0.1, 2)) == 36, "ceil(20 * 0.9 * 2) debe ser 36");
    verificar(costoModerar(Agente(7, 0.3, 1.5)) == 8, "ceil(7 * 0.7 * 1.5) debe ser 8");
    verificar(costoModerar(Agente(-30, 0.9, 1)) == 3, "ceil(30 * 0.1 * 1) debe ser 3");
    Resultado r = modexFB(Red({Agente(10, 0.7, 1)}, 3));
    verificar(r.E == std::vector<int>{1}, "FB debe moderar al agente (10, 0.7, 1) con R_max = 3");
}

void pruebasEnunciado() {
    Red rs1({Agente(100, 0.5, 1), Agente(100, 0.1, 1), Agente(-10, 0.1, 1)}, 55);
    verificar(esfuerzo(rs1, {0, 0, 1}) == 9, "Ejemplo 1: Esfuerzo(E1) = 9");
    verificar(std::abs(rs1.getExtremismo({0, 0, 1}) - 47.14) < 0.005, "Ejemplo 1: Ext(E1) = 47.14");
    verificar(esfuerzo(rs1, {1, 0, 0}) == 50, "Ejemplo 1: Esfuerzo(E2) = 50");
    verificar(std::abs(rs1.getExtremismo({1, 0, 0}) - 33.5) < 0.005, "Ejemplo 1: Ext(E2) = 33.5");

    Red rs2({Agente(-30, 0.9, 1), Agente(20, 0.1, 2), Agente(50, 0.5, 1)}, 35);
    verificar(esfuerzo(rs2, {0, 1, 0}) == 36, "Ejemplo 2: Esfuerzo(E1) = 36");
    verificar(esfuerzo(rs2, {1, 0, 1}) == 28, "Ejemplo 2: Esfuerzo(E2) = 28");
    verificar(std::abs(rs2.getExtremismo({1, 0, 1}) - 9.43) < 0.005, "Ejemplo 2: Ext(E2) = 9.43");

    for (Resultado (*f)(const Red&) : {modexFB, modexPD}) {
        Resultado a = f(rs1);
        verificar(a.E == std::vector<int>{1, 0, 0}, "Ejemplo 1: el optimo es <1,0,0>");
        Resultado b = f(rs2);
        verificar(b.E == std::vector<int>{1, 0, 1}, "Ejemplo 2: el optimo es <1,0,1>");
    }
}

void pruebasFijas() {
    std::vector<Caso> casos = {
        {{Agente(100, 0.5, 1), Agente(100, 0.1, 1), Agente(-10, 0.1, 1)}, 55},
        {{Agente(-30, 0.9, 1), Agente(20, 0.1, 2), Agente(50, 0.5, 1)}, 35},
        {{Agente(50, 0.5, 1), Agente(-40, 0.2, 2), Agente(10, 0.9, 3)}, 0},
        {{Agente(100, 1, 3), Agente(-100, 1, 3)}, 0},
        {{Agente(40, 0.5, 2), Agente(30, 0.0, 1)}, 40},
        {{Agente(90, 0.0, 1), Agente(-60, 0.5, 3)}, 90},
        {{Agente(7, 0.3, 1.5), Agente(5, 0.8, 1)}, 8},
        {{Agente(7, 0.3, 1.5), Agente(5, 0.8, 1)}, 7},
        {{Agente(-100, 0.2, 3), Agente(80, 0.6, 2), Agente(-20, 0.0, 1.2)}, 500},
        {{Agente(10, 0.7, 1), Agente(-50, 0.5, 1)}, 3},
    };
    std::cout << std::fixed << std::setprecision(4);
    for (std::size_t i = 0; i < casos.size(); ++i) {
        Red red(casos[i].agentes, casos[i].r);
        Resultado fb = modexFB(red);
        Resultado v = modexV(red);
        Resultado pd = modexPD(red);
        std::string nombre = "test" + std::to_string(i + 1);
        verificarConsistencia(red, fb, nombre + " FB");
        verificarConsistencia(red, v, nombre + " V");
        verificarConsistencia(red, pd, nombre + " PD");
        verificar(iguales(fb.extremismo, pd.extremismo), nombre + ": PD no coincide con FB");
        std::cout << nombre << "  FB: Ext=" << fb.extremismo << " Esf=" << fb.esfuerzo << " E=" << texto(fb.E)
                  << "  |  V: Ext=" << v.extremismo << " Esf=" << v.esfuerzo << " E=" << texto(v.E)
                  << "  |  PD: Ext=" << pd.extremismo << " Esf=" << pd.esfuerzo << " E=" << texto(pd.E) << '\n';
    }
}

void pruebasAleatorias() {
    std::mt19937 generador(2026);
    std::uniform_int_distribution<std::size_t> distN(1, 14);
    const int total = 2000;
    int vorazOptimo = 0;
    double peorErrorRelativo = 0.0;
    for (int k = 0; k < total; ++k) {
        std::vector<Agente> agentes = generarAgentes(distN(generador), generador);
        Red base(agentes, 0);
        std::uniform_int_distribution<long long> distR(0, esfuerzoTotal(base));
        Red red(agentes, distR(generador));
        Resultado fb = modexFB(red);
        Resultado v = modexV(red);
        Resultado pd = modexPD(red);
        std::string nombre = "aleatorio " + std::to_string(k);
        verificarConsistencia(red, fb, nombre + " FB");
        verificarConsistencia(red, v, nombre + " V");
        verificarConsistencia(red, pd, nombre + " PD");
        verificar(iguales(fb.extremismo, pd.extremismo), nombre + ": PD no coincide con FB");
        verificar(v.extremismo >= fb.extremismo - 1e-9, nombre + ": el voraz no puede superar al optimo");
        if (iguales(v.extremismo, fb.extremismo)) {
            ++vorazOptimo;
        } else if (fb.extremismo > 0) {
            peorErrorRelativo = std::max(peorErrorRelativo, (v.extremismo - fb.extremismo) / fb.extremismo);
        }
    }
    std::cout << "\nAleatorias (n <= 14): " << total << " casos, PD = FB en todos los que no reportan falla.\n";
    std::cout << "Voraz optimo en " << vorazOptimo << "/" << total << " ("
              << 100.0 * vorazOptimo / total << "%), peor error relativo en Ext: " << 100.0 * peorErrorRelativo << "%\n";
}

void pruebasTiempo() {
    std::mt19937 generador(7);
    std::cout << "\nTiempos (ms):\n";
    for (std::size_t n : std::vector<std::size_t>{10, 15, 20, 22}) {
        std::vector<Agente> agentes = generarAgentes(n, generador);
        Red red(agentes, esfuerzoTotal(Red(agentes, 0)) / 2);
        auto inicio = std::chrono::steady_clock::now();
        modexFB(red);
        std::chrono::duration<double, std::milli> ms = std::chrono::steady_clock::now() - inicio;
        std::cout << "  FB n=" << n << ": " << ms.count() << '\n';
    }
    for (std::size_t n : std::vector<std::size_t>{100, 1000, 3000}) {
        std::vector<Agente> agentes = generarAgentes(n, generador);
        Red red(agentes, esfuerzoTotal(Red(agentes, 0)) / 2);
        auto inicio = std::chrono::steady_clock::now();
        Resultado v = modexV(red);
        std::chrono::duration<double, std::milli> msV = std::chrono::steady_clock::now() - inicio;
        inicio = std::chrono::steady_clock::now();
        Resultado pd = modexPD(red);
        std::chrono::duration<double, std::milli> msPD = std::chrono::steady_clock::now() - inicio;
        verificarConsistencia(red, v, "tiempo V");
        verificarConsistencia(red, pd, "tiempo PD");
        verificar(v.extremismo >= pd.extremismo - 1e-9, "tiempo: el voraz no puede superar a PD");
        std::cout << "  n=" << n << "  V: " << msV.count() << " (Ext=" << v.extremismo << ")  PD: " << msPD.count()
                  << " (Ext=" << pd.extremismo << ")\n";
    }
}

}

int main() {
    pruebasCosto();
    pruebasEnunciado();
    pruebasFijas();
    pruebasAleatorias();
    pruebasTiempo();
    std::cout << '\n' << (fallas == 0 ? "Todas las pruebas pasaron" : std::to_string(fallas) + " fallas") << '\n';
    return fallas == 0 ? 0 : 1;
}
