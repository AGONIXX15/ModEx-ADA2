#include <estrategias/fuerzabruta.hpp>

#include <limits>

// Genera las 2^n estrategias E = <e0, ..., e(n-1)> fijando e_i = 0 y luego
// e_i = 1 para cada agente i. Cada estrategia completa se evalua apenas se
// genera (no se guardan todas en memoria) y se conserva la mejor aplicable.
// Tiempo O(n * 2^n), espacio O(n).

FuerzaBruta::FuerzaBruta(const Red& red) : red_(red) {}

void FuerzaBruta::explorar(std::size_t i, std::vector<int>& estrategia, Resultado& mejor) const {
    if (i == estrategia.size()) {
        long long esf = esfuerzo(red_, estrategia);
        if (esf > red_.getRmax()) {
            return;
        }
        double ext = red_.getExtremismo(estrategia);
        bool mejora = ext < mejor.extremismo || (ext == mejor.extremismo && esf < mejor.esfuerzo);
        if (mejora) {
            mejor = Resultado{estrategia, esf, ext};
        }
        return;
    }
    estrategia[i] = 0;
    explorar(i + 1, estrategia, mejor);
    estrategia[i] = 1;
    explorar(i + 1, estrategia, mejor);
    estrategia[i] = 0;
}

Resultado FuerzaBruta::solve() const {
    std::vector<int> estrategia(red_.getNumAgentes(), 0);
    Resultado mejor{{}, 0, std::numeric_limits<double>::infinity()};
    explorar(0, estrategia, mejor);
    return mejor;
}

Resultado modexFB(const Red& red) {
    return FuerzaBruta(red).solve();
}
