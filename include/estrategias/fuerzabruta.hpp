#pragma once

#include <red.hpp>
#include <resultado.hpp>

#include <vector>

long long techo(double x);
long long esfuerzo(const Red& red, const std::vector<int>& estrategia);
std::vector<std::vector<int>> generarEstrategias(int n);

class FuerzaBruta {
public:
    explicit FuerzaBruta(const Red& red);
    Resultado solve() const;

private:
    const Red& red_;
};

Resultado modexFB(const Red& red);
