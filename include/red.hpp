#pragma once

#include <agente.hpp>
#include <resultado.hpp>

#include <cstddef>
#include <filesystem>
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

Red leerRed(const std::filesystem::path& path);
void escribirSalida(const std::filesystem::path& path, const Resultado& resultado);
