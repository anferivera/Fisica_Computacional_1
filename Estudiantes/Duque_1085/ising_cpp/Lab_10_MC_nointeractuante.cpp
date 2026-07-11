// Simulación Monte Carlo para el modelo de Ising en 2D con campo magnético
// En este modelo consideramos una red cuadrada de espines (LxL) donde cada espín puede ser +1 o -1.
// Pero los espines son no interactuantes entre sí, solo sienten el efecto del campo magnético externo H.
// Es una traducción a c++ del laboratio 8

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <iomanip>

double coth(double x) {
    return 1.0 / std::tanh(x);
}

double metropolis_mc(int L, double T, double H, double mu = 1.0, double kb = 1.0, int steps = 100000) {

    std::vector<std::vector<int>> grid(L, std::vector<int>(L, 0)); // grid de spins LxL
    
    std::random_device rd;                                       // Semilla de número aleatorio
    std::mt19937 gen(rd());                                      // Generador mersenne twister
    std::uniform_int_distribution<int> dist_spin(0, 1);          // Distribución para asignar espines (0 o 1)
    std::uniform_int_distribution<int> dist_coord(0, L - 1);     // Distribución para seleccionar coordenadas [0, L-1]
    std::uniform_real_distribution<double> dist_rand(0.0, 1.0);  // Distribución para aceptación probabilística

    // asignación aleatoria de espines (1 o -1) al grid
    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            grid[i][j] = (dist_spin(gen) == 1) ? 1 : -1;
        }
    }

    int N = L * L;
    int termalization_steps = static_cast<int>(0.1 * steps);  // static_cast me permite castear un double a int
    
    double magnetization_sum = 0.0;
    long long samples = 0;  // Evitar overflow usando long long para contar muestras

    // Algortimo Monte Carlo

    for (int step = 0; step < steps; ++step) {
        
        int i = dist_coord(gen);
        int j = dist_coord(gen);
        int spin = grid[i][j];

        // Definición del dE
        double dE = 2.0 * mu * H * spin;

        // Criterio de Aceptación de Metrópolis
        if (dE < 0.0) {
            grid[i][j] = -spin;
        }

        else {
            double r = dist_rand(gen);
            if (r < std::exp(-dE / (kb * T))) {
                grid[i][j] = -spin;
            }
        }

        // Tomar medidas después de la termalización
        if (step > termalization_steps) {
            double current_mag = 0.0;
            for (const auto& fila : grid) {
                for (int s : fila) {
                    current_mag += s;
                }
            }
            magnetization_sum += current_mag;
            samples++;
        }
    }

    // Retorna la magnetización media por partícula
    return magnetization_sum / (samples * N);
}

int main() {

    int L = 20; 
    std::vector<double> T_values = {5.0, 15.0, 20.0};
    
    // Linspace para el campo magnético H
    int num_puntos_H = 40;
    double H_min = -30.0;
    double H_max = 30.0;
    std::vector<double> H_values(num_puntos_H);
    double delta_H = (H_max - H_min) / (num_puntos_H - 1);
    for (int i = 0; i < num_puntos_H; ++i) {
        H_values[i] = H_min + i * delta_H;
    }

    // Guardar las curvas de magnetización para cada temperatura
    std::ofstream archivo("resultados_paramagneto.csv");
    if (!archivo.is_open()) {
        std::cerr << "Error al crear el archivo de resultados." << std::endl;
        return 1;
    }

    // Encabezado del archivo CSV
    archivo << "T,H,M_MC,M_Analitica,M_Clasica\n";

    for (double T : T_values) {
        std::cout << "Procesando Temperatura: " << T << " K" << std::endl;
        for (double H : H_values) {
            
            // 1. Simulación Monte Carlo (50k pasos por punto)
            double m_mc = metropolis_mc(L, T, H, 1.0, 1.0, 50000);

            // 2. Resultado Analítico Cuántico (Spin 1/2)
            double m_analitica = std::tanh(H / T);

            // 3. Resultado Clásico (Función de Langevin)
            double m_clasica = 0.0;
            double x = H / T;
            if (x != 0.0) {
                m_clasica = coth(x) - (1.0 / x);
            }

            // Escribir datos en el CSV
            archivo << T << "," << H << "," << m_mc << "," << m_analitica << "," << m_clasica << "\n";
        }
    }

    archivo.close();
    std::cout << "¡Simulación completada con éxito! Archivo 'resultados_paramagneto.csv' generado." << std::endl;

    return 0;
}