#include <estrategias/dinamica.hpp>

Resultado modexPD(const Red& red) {
    return Resultado{std::vector<int>(red.getNumAgentes(), 0), 0, red.getExtremismo()};
}
