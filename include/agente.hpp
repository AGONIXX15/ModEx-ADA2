#pragma once

#include <random>

class Agente {
public:
    Agente(int opinion, double receptividad, double influencia);
    static Agente aleatorio(std::mt19937& generador);

    int getOpinion() const;
    double getReceptividad() const;
    double getInfluencia() const;
    void setOpinion(int opinion);
    void setReceptividad(double receptividad);
    void setInfluencia(double influencia);

private:
    int opinion_;
    double receptividad_;
    double influencia_;
};
