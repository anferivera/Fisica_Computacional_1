// Modelo de Ising 2D (espines no interactuantes) - Metropolis Monte Carlo
// E = -mu0 * H * sum(s_i),  s_i = +1 / -1
// Unidades reducidas: mu0 = kB = 1  ->  la variable relevante es H/T
// Teoria (distribucion canonica): <M>/N = mu0 * tanh(mu0*H / (kB*T))

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>

int main() {
    const int L = 50;                 // red L x L
    const int N = L * L;              // numero de atomos
    const double mu0 = 1.0, kB = 1.0;
    const int nTerm = 500;            // barridos de termalizacion
    const int nMed  = 2000;           // barridos de medicion (criterio de finalizacion)

    const double Ts[3] = {5.0, 15.0, 20.0};   // K
    const double Hmin = -30.0, Hmax = 30.0, dH = 1.0;  // G

    std::mt19937 gen(12345);
    std::uniform_real_distribution<double> U(0.0, 1.0);
    std::uniform_int_distribution<int> sitio(0, L - 1);

    std::ofstream out("ising.csv");
    out << "T,H,M_mc,M_teo\n";

    for (double T : Ts) {
        for (double H = Hmin; H <= Hmax + 1e-9; H += dH) {

            // 1. Microestado inicial aleatorio
            std::vector<std::vector<int>> s(L, std::vector<int>(L));
            for (auto& fila : s)
                for (int& x : fila) x = (U(gen) < 0.5) ? 1 : -1;

            double sumaM = 0.0;
            for (int paso = 0; paso < nTerm + nMed; ++paso) {
                // Un barrido = N intentos de inversion
                for (int k = 0; k < N; ++k) {
                    // 2. Visitar un punto aleatorio e invertir su espin
                    int i = sitio(gen), j = sitio(gen);
                    double dE = 2.0 * mu0 * H * s[i][j];   // E(x') - E(x)

                    // 3. Aceptacion de Metropolis
                    if (dE <= 0.0 || U(gen) < std::exp(-dE / (kB * T)))
                        s[i][j] = -s[i][j];
                }
                // 4. Medir despues de termalizar
                if (paso >= nTerm) {
                    long M = 0;
                    for (auto& fila : s) for (int x : fila) M += x;
                    sumaM += mu0 * double(M) / N;
                }
            }
            double Mmc  = sumaM / nMed;
            double Mteo = mu0 * std::tanh(mu0 * H / (kB * T));
            out << T << "," << H << "," << Mmc << "," << Mteo << "\n";
        }
        std::cout << "T = " << T << " K listo\n";
    }
    std::cout << "Datos guardados en ising.csv\n";
    return 0;
}
