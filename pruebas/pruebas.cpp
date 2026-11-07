#include <estrategias/fuerzabruta.hpp>

#include <iostream>

namespace {

struct Caso {
    std::vector<Agente> agentes;
    long long r;
};

}

int main() {
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
    };
    for (std::size_t i = 0; i < casos.size(); ++i) {
        Resultado r = modexFB(Red(casos[i].agentes, casos[i].r));
        std::cout << "test" << i + 1 << " Ext=" << r.extremismo << " Esf=" << r.esfuerzo << " E=";
        for (std::size_t j = 0; j < r.E.size(); ++j) {
            std::cout << (j ? "," : "") << r.E[j];
        }
        std::cout << '\n';
    }
    return 0;
}
