#pragma once

#include <agente.hpp>
#include <resultado.hpp>

#include <cstddef>
#include <filesystem>
#include <random>
#include <vector>

class Red {
public:
    Red(std::vector<Agente> agentes, long long rMax);

    const std::vector<Agente>& getAgentes() const;
    std::size_t getNumAgentes() const;
    long long getRmax() const;
    double getExtremismo() const;
    double getExtremismo(const std::vector<int>& estrategia) const;

private:
    std::vector<Agente> agentes_;
    long long rMax_;
};


long long techo(double x);
long long costoModerar(const Agente& agente);
double beneficioModerar(const Agente& agente);
long long esfuerzo(const Red& red, const std::vector<int>& estrategia);
long long esfuerzoTotal(const Red& red);

Red leerRed(const std::filesystem::path& path);
void escribirRed(const std::filesystem::path& path, const Red& red);
void escribirSalida(const std::filesystem::path& path, const Resultado& resultado);
std::vector<Agente> generarAgentes(std::size_t n, std::mt19937& generador);
