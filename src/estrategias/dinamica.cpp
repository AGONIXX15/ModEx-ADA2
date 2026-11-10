#include <estrategias/dinamica.hpp>

#include <algorithm>
#include <cstddef>
#include <vector>

// Minimizar Ext(Mod(RS, E)) = sqrt(S - sum_{e_i = 1} b_i) / n, con
// S = sum o_i^2 * inf_i, equivale a maximizar la suma de los beneficios
// b_i = o_i^2 * inf_i de los agentes moderados sujeta a
// sum_{e_i = 1} c_i <= R_max, con c_i = ceil(|o_i| * (1 - r_i) * inf_i).
//
// Subproblemas: M(i, j) = maximo beneficio moderando solo agentes de
// a_0, ..., a_(i-1) con esfuerzo a lo sumo j, 0 <= i <= n, 0 <= j <= A, donde
// A = min(R_max, sum c_i) (un presupuesto mayor que moderar a todos no sirve).
//
//   M(0, j) = 0
//   M(i, j) = M(i-1, j)                                    si c_(i-1) > j
//   M(i, j) = max(M(i-1, j), M(i-1, j - c_(i-1)) + b_(i-1)) si c_(i-1) <= j
//
// El costo optimo es Ext = sqrt(S - M(n, A)) / n.
//
// La fila i solo depende de la fila i-1, asi que los valores se guardan en un
// solo arreglo recorrido de j = A hacia abajo. Para reconstruir E se guarda la
// tabla de decisiones moderar[i][j] = 1 si M(i+1, j) se obtuvo moderando a a_i.
// Como c_i <= 300, A <= 300n: tiempo O(n * A) = O(n^2), espacio O(n * A) = O(n^2).

Resultado modexPD(const Red& red) {
    const std::vector<Agente>& agentes = red.getAgentes();
    const std::size_t n = agentes.size();

    std::vector<long long> costo(n);
    std::vector<double> beneficio(n);
    for (std::size_t i = 0; i < n; ++i) {
        costo[i] = costoModerar(agentes[i]);
        beneficio[i] = beneficioModerar(agentes[i]);
    }
    const auto A = static_cast<std::size_t>(std::min(red.getRmax(), esfuerzoTotal(red)));

    std::vector<double> M(A + 1, 0.0);
    std::vector<std::vector<bool>> moderar(n, std::vector<bool>(A + 1, false));
    for (std::size_t i = 0; i < n; ++i) {
        const auto c = static_cast<std::size_t>(costo[i]);
        if (c > A) {
            continue;
        }
        for (std::size_t j = A + 1; j-- > c;) {
            double conAgente = M[j - c] + beneficio[i];
            if (conAgente > M[j]) {
                M[j] = conAgente;
                moderar[i][j] = true;
            }
        }
    }

    std::vector<int> E(n, 0);
    std::size_t j = A;
    for (std::size_t i = n; i-- > 0;) {
        if (moderar[i][j]) {
            E[i] = 1;
            j -= static_cast<std::size_t>(costo[i]);
        }
    }

    return Resultado{E, esfuerzo(red, E), red.getExtremismo(E)};
}
