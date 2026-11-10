#include <agente.hpp>

#include <cmath>

Agente::Agente(int opinion, double receptividad, double influencia)
    : opinion_(opinion), receptividad_(receptividad), influencia_(influencia) {}

Agente Agente::aleatorio(std::mt19937& generador) {
    std::uniform_int_distribution<int> distOpinion(-100, 100);
    std::uniform_real_distribution<double> distReceptividad(0.0, 1.0);
    std::uniform_real_distribution<double> distInfluencia(1.0, 3.0);
    int opinion = distOpinion(generador);
    // Se redondea a dos decimales para que los archivos generados sean legibles.
    double receptividad = std::round(distReceptividad(generador) * 100.0) / 100.0;
    double influencia = std::round(distInfluencia(generador) * 100.0) / 100.0;
    return Agente(opinion, receptividad, influencia);
}

int Agente::getOpinion() const { return opinion_; }
double Agente::getReceptividad() const { return receptividad_; }
double Agente::getInfluencia() const { return influencia_; }
void Agente::setOpinion(int opinion) { opinion_ = opinion; }
void Agente::setReceptividad(double receptividad) { receptividad_ = receptividad; }
void Agente::setInfluencia(double influencia) { influencia_ = influencia; }
