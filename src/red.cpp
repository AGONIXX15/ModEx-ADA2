#include <red.hpp>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

Red::Red(std::vector<Agente> agentes, long long rMax)
    : agentes_(std::move(agentes)), rMax_(rMax) {}

const std::vector<Agente>& Red::getAgentes() const { return agentes_; }
std::size_t Red::getNumAgentes() const { return agentes_.size(); }
long long Red::getRmax() const { return rMax_; }

double Red::getExtremismo() const {
    return getExtremismo(std::vector<int>(agentes_.size(), 0));
}

double Red::getExtremismo(const std::vector<int>& estrategia) const {
    double suma = 0.0;
    for (std::size_t i = 0; i < agentes_.size(); ++i) {
        double opPrima = agentes_[i].getOpinion() * (1 - estrategia[i]);
        suma += opPrima * opPrima * agentes_[i].getInfluencia();
    }
    return std::sqrt(suma) / static_cast<double>(agentes_.size());
}

long long techo(double x) {
    constexpr double tolerancia = 1e-9;
    return static_cast<long long>(std::ceil(x - tolerancia));
}

long long costoModerar(const Agente& agente) {
    double op = agente.getOpinion();
    return techo(std::abs(op) * (1 - agente.getReceptividad()) * agente.getInfluencia());
}

double beneficioModerar(const Agente& agente) {
    double op = agente.getOpinion();
    return op * op * agente.getInfluencia();
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

long long esfuerzoTotal(const Red& red) {
    long long suma = 0;
    for (const Agente& agente : red.getAgentes()) {
        suma += costoModerar(agente);
    }
    return suma;
}

namespace {

std::vector<std::string> campos(std::string linea) {
    for (std::size_t i = 0; i < linea.size(); ++i) {
        char c = linea[i];
        bool puntoSeparador = c == '.' && (i + 1 == linea.size() || linea[i + 1] == ' ');
        if (c == ',' || c == ';' || c == '\t' || puntoSeparador) {
            linea[i] = ' ';
        }
    }
    std::vector<std::string> resultado;
    std::istringstream flujo(linea);
    std::string campo;
    while (flujo >> campo) {
        resultado.push_back(campo);
    }
    return resultado;
}

long long entero(const std::string& texto) {
    std::size_t usados = 0;
    long long valor = std::stoll(texto, &usados);
    if (usados != texto.size()) {
        throw std::invalid_argument(texto);
    }
    return valor;
}

double real(const std::string& texto) {
    std::size_t usados = 0;
    double valor = std::stod(texto, &usados);
    if (usados != texto.size() || !std::isfinite(valor)) {
        throw std::invalid_argument(texto);
    }
    return valor;
}

}

Red leerRed(const std::filesystem::path& path) {
    std::ifstream archivo(path);
    if (!archivo) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path.string());
    }
    std::vector<std::pair<std::size_t, std::string>> lineas;
    std::string linea;
    std::size_t numero = 0;
    while (std::getline(archivo, linea)) {
        ++numero;
        if (numero == 1 && linea.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            linea.erase(0, 3);
        }
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }
        if (linea.find_first_not_of(" \t") != std::string::npos) {
            lineas.emplace_back(numero, linea);
        }
    }
    std::size_t actual = 0;
    try {
        if (lineas.empty()) {
            throw std::runtime_error("Archivo vacio");
        }
        actual = lineas[0].first;
        std::vector<std::string> cabecera = campos(lineas[0].second);
        if (cabecera.size() != 1) {
            throw std::runtime_error("Linea " + std::to_string(actual) + ": se esperaba n");
        }
        long long n = entero(cabecera[0]);
        if (n <= 0 || lineas.size() != static_cast<std::size_t>(n) + 2) {
            throw std::runtime_error("Cantidad de lineas inconsistente con n");
        }
        std::vector<Agente> agentes;
        for (std::size_t i = 1; i <= static_cast<std::size_t>(n); ++i) {
            actual = lineas[i].first;
            std::string donde = "Linea " + std::to_string(actual) + ": ";
            std::vector<std::string> c = campos(lineas[i].second);
            if (c.size() != 3) {
                throw std::runtime_error(donde + "se esperaban opinion, receptividad e influencia");
            }
            long long opinion = entero(c[0]);
            double receptividad = real(c[1]);
            double influencia = real(c[2]);
            if (opinion < -100 || opinion > 100) {
                throw std::runtime_error(donde + "opinion fuera de [-100, 100]");
            }
            if (!(receptividad >= 0.0 && receptividad <= 1.0)) {
                throw std::runtime_error(donde + "receptividad fuera de [0, 1]");
            }
            if (!(influencia >= 1.0 && influencia <= 3.0)) {
                throw std::runtime_error(donde + "influencia fuera de [1, 3]");
            }
            agentes.emplace_back(static_cast<int>(opinion), receptividad, influencia);
        }
        actual = lineas.back().first;
        std::vector<std::string> ultima = campos(lineas.back().second);
        if (ultima.size() != 1) {
            throw std::runtime_error("Linea " + std::to_string(actual) + ": se esperaba R_max");
        }
        long long rMax = entero(ultima[0]);
        if (rMax < 0) {
            throw std::runtime_error("Linea " + std::to_string(actual) + ": R_max negativo");
        }
        return Red(std::move(agentes), rMax);
    } catch (const std::logic_error& e) {
        throw std::runtime_error("Linea " + std::to_string(actual) + ": formato invalido (" + e.what() + ")");
    }
}

void escribirRed(const std::filesystem::path& path, const Red& red) {
    std::ofstream archivo(path);
    if (!archivo) {
        throw std::runtime_error("No se pudo escribir el archivo: " + path.string());
    }
    archivo << std::setprecision(15);
    archivo << red.getNumAgentes() << '\n';
    for (const Agente& agente : red.getAgentes()) {
        archivo << agente.getOpinion() << ',' << agente.getReceptividad() << ',' << agente.getInfluencia() << '\n';
    }
    archivo << red.getRmax() << '\n';
}

std::vector<Agente> generarAgentes(std::size_t n, std::mt19937& generador) {
    std::vector<Agente> agentes;
    agentes.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        agentes.push_back(Agente::aleatorio(generador));
    }
    return agentes;
}

void escribirSalida(const std::filesystem::path& path, const Resultado& resultado) {
    std::ofstream archivo(path);
    if (!archivo) {
        throw std::runtime_error("No se pudo escribir el archivo: " + path.string());
    }
    archivo << std::fixed << std::setprecision(6) << resultado.extremismo << '\n';
    archivo << resultado.esfuerzo << '\n';
    for (int e : resultado.E) {
        archivo << e << '\n';
    }
}
