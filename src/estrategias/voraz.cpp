#include <estrategias/voraz.hpp>

Resultado modexV(const Red& red) {
    return Resultado{std::vector<int>(red.getNumAgentes(), 0), 0, red.getExtremismo()};
}
