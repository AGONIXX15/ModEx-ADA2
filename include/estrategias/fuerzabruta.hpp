#pragma once

#include <red.hpp>
#include <resultado.hpp>

#include <cstddef>
#include <vector>

class FuerzaBruta {
public:
    explicit FuerzaBruta(const Red& red);
    Resultado solve() const;

private:
    void explorar(std::size_t i, std::vector<int>& estrategia, Resultado& mejor) const;

    const Red& red_;
};

Resultado modexFB(const Red& red);
