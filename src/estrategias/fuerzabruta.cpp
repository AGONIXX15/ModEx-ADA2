#include <estrategias/fuerzabruta.hpp>

#include <cmath>
#include <limits>

long long techo(double x) {
    if (x == static_cast<double>(static_cast<long long>(x))) {
        return static_cast<long long>(x);
    }
    if (x > 0) {
        return static_cast<long long>(x) + 1;
    }
    return static_cast<long long>(x);
}

long long esfuerzo(const Red& red, const std::vector<int>& estrategia) {
    long long suma = 0;
    const std::vector<Agente>& agentes = red.getAgentes();
    for (std::size_t i = 0; i < agentes.size(); ++i) {
        double op = agentes[i].getOpinion();
        double opPrima = op * (1 - estrategia[i]);
        double rc = agentes[i].getReceptividad();
        double inf = agentes[i].getInfluencia();
        suma += techo(std::abs(op - opPrima) * (1 - rc) * inf);
    }
    return suma;
}

std::vector<std::vector<int>> generarEstrategias(int n) {
    if (n <= 0) {
        return {{}};
    }
    std::vector<std::vector<int>> estrategias = generarEstrategias(n - 1);
    std::vector<std::vector<int>> resultado;
    resultado.reserve(estrategias.size() * 2);
    for (const std::vector<int>& e : estrategias) {
        std::vector<int> v{1};
        v.insert(v.end(), e.begin(), e.end());
        resultado.push_back(std::move(v));
    }
    for (const std::vector<int>& e : estrategias) {
        std::vector<int> v{0};
        v.insert(v.end(), e.begin(), e.end());
        resultado.push_back(std::move(v));
    }
    return resultado;
}

FuerzaBruta::FuerzaBruta(const Red& red) : red_(red) {}

Resultado FuerzaBruta::solve() const {
    long long esfuerzoMax = red_.getRmax();
    std::vector<std::vector<int>> estrategias = generarEstrategias(static_cast<int>(red_.getNumAgentes()));
    Resultado final{{}, 0, std::numeric_limits<double>::infinity()};
    for (const std::vector<int>& estrategia : estrategias) {
        long long esf = esfuerzo(red_, estrategia);
        double extremismo = red_.getExtremismo(estrategia);
        if (esf <= esfuerzoMax && extremismo < final.extremismo) {
            final.esfuerzo = esf;
            final.extremismo = extremismo;
            final.E = estrategia;
        }
    }
    return final;
}

Resultado modexFB(const Red& red) {
    return FuerzaBruta(red).solve();
}
