#include <estrategias/voraz.hpp>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <vector>

// Minimizar Ext(Mod(RS, E)) equivale a maximizar la suma de los beneficios
// b_i = o_i^2 * inf_i de los agentes moderados, con la suma de sus costos
// c_i = ceil(|o_i| * (1 - r_i) * inf_i) <= R_max (mochila 0/1).
//
// Estrategia voraz:
//   1. Ordenar los agentes por beneficio/costo de mayor a menor (costo 0 primero).
//   2. Recorrerlos en ese orden moderando cada uno que aun quepa en R_max.
//   3. Repetir el paso 2 empezando por el agente de mayor beneficio que quepa
//      solo, y quedarse con la mejor de las dos soluciones.
// El paso 3 corrige el caso en que la razon favorece a agentes baratos pero
// poco extremos y deja por fuera a un agente muy extremo.
// Tiempo O(n log n) por el ordenamiento, espacio O(n).

namespace {

struct Candidato {
    std::size_t indice;
    long long costo;
    double beneficio;
    double razon;
};

void llenar(const std::vector<Candidato>& orden, long long rMax, std::vector<int>& E, long long& usado, double& ganancia) {
    for (const Candidato& c : orden) {
        if (E[c.indice] == 0 && usado + c.costo <= rMax) {
            E[c.indice] = 1;
            usado += c.costo;
            ganancia += c.beneficio;
        }
    }
}

}

Resultado modexV(const Red& red) {
    const std::vector<Agente>& agentes = red.getAgentes();
    const std::size_t n = agentes.size();
    const long long rMax = red.getRmax();

    std::vector<Candidato> orden;
    orden.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        long long costo = costoModerar(agentes[i]);
        double beneficio = beneficioModerar(agentes[i]);
        if (beneficio <= 0.0 || costo > rMax) {
            continue;
        }
        double razon = costo == 0 ? std::numeric_limits<double>::infinity() : beneficio / static_cast<double>(costo);
        orden.push_back(Candidato{i, costo, beneficio, razon});
    }
    std::sort(orden.begin(), orden.end(), [](const Candidato& a, const Candidato& b) {
        if (a.razon != b.razon) {
            return a.razon > b.razon;
        }
        return a.beneficio > b.beneficio;
    });

    std::vector<int> E(n, 0);
    long long usado = 0;
    double ganancia = 0.0;
    llenar(orden, rMax, E, usado, ganancia);

    auto mayor = std::max_element(orden.begin(), orden.end(), [](const Candidato& a, const Candidato& b) {
        return a.beneficio < b.beneficio;
    });
    if (mayor != orden.end() && E[mayor->indice] == 0) {
        std::vector<int> E2(n, 0);
        E2[mayor->indice] = 1;
        long long usado2 = mayor->costo;
        double ganancia2 = mayor->beneficio;
        llenar(orden, rMax, E2, usado2, ganancia2);
        if (ganancia2 > ganancia) {
            E = std::move(E2);
        }
    }

    return Resultado{E, esfuerzo(red, E), red.getExtremismo(E)};
}
