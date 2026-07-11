#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <string>

// Simulación Monte Carlo para el modelo de Ising en 2D con campo magnético
// En este modelo consideramos una red cuadrada de espines (LxL) donde cada espín puede ser +1 o -1.
// A diferencia del otro script acá la energía tiene en cuenta la interacción entre vecinos, además del campo magnético externo H.
// dE = 2 * spin * (J * suma_vecinos + mu * H), en teoria este modelo permite ver curvas de histeresis
// El problema es que en un paso montecarlo se ve una transición de primer orden discontinua
// Tambien notar que con el objetivo de practicar defini muchas funciones

// Funcion de inicialización de la red de spins: int L --> matriz_LxL(-1,+1)
std::vector<std::vector<int>> inicializar_red(int L) {
    std::vector<std::vector<int>> red_spin(L, std::vector<int>(L,0));

    // Configuración numeros aleatorios
    std::random_device rd;                                // Semilla basada en el hardaware
    std::mt19937 gen(rd());                               // Generador de números aleatorios Mersenne Twister
    std::uniform_int_distribution<> distribucion(0, 1);   // Distribución uniforme para 0 y 1

    for (int i = 0; i < L; ++i) {
        for (int j = 0; j < L; ++j) {
            if (distribucion(gen) == 1){
                red_spin[i][j] = 1; // Spin hacia arriba
            }
            else{
                red_spin[i][j] = -1; // Spin hacia abajo
            }
        }
    }
    return red_spin;
}

void imprimir_red(const std::vector<std::vector<int>>& red) {
    for (const auto& fila : red) {
        for (const auto& spin : fila) {
            std::cout << (spin == 1 ? "+ " : "- ");
        }
        std::cout << std::endl;
    }
}

void guardar_red_csv(const std::vector<std::vector<int>>& red, int paso) {

    // Creamos un nombre dinámico para el archivo, ej: "red_paso_10.csv"
    std::string nombre_archivo = "red_paso_" + std::to_string(paso) + ".csv";
    
    // Abrimos el flujo de salida hacia el archivo
    std::ofstream archivo(nombre_archivo);
    
    // Verificamos si el archivo se pudo crear/abrir correctamente
    if (archivo.is_open()) {
        for (const auto& fila : red) {
            for (size_t j = 0; j < fila.size(); ++j) {
                archivo << fila[j];
                // Agregamos una coma entre elementos, excepto al final de la fila
                if (j < fila.size() - 1) {
                    archivo << ",";
                }
            }
            archivo << "\n"; 
        }
        archivo.close(); 
    } else {
        std::cerr << "Error al abrir el archivo para guardar la red." << std::endl;
    }
}

void evolucion_montecarlo(std::vector<std::vector<int>>& red, double T, double H) {
    int L = red.size();
    int total_sitios = L*L;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist_coordenada(0, L-1);
    std::uniform_real_distribution<double> dist_aceptacion(0.0, 1.0);

    // Sea K_Boltzman = 1

    for (int intento = 0; intento < total_sitios; ++intento) {
        
        // Selección de coordenada (i,j)
        int i = dist_coordenada(gen);
        int j = dist_coordenada(gen);

        // Condiciones periodicas
        int arriba  = red[(i - 1 + L) % L][j];
        int abajo   = red[(i + 1) % L][j];
        int izquierda = red[i][(j - 1 + L) % L];
        int derecha   = red[i][(j + 1) % L];
        
        // Calculo de energía
        int suma_vecinos = arriba + abajo + izquierda + derecha;
        double delta_E = 2.0 * red[i][j] * (suma_vecinos + H);

        if (delta_E <= 0.0) {
            red[i][j] = -red[i][j]; 
        } 
        else {
            double factor_boltzmann = std::exp(-delta_E / T);
            double r = dist_aceptacion(gen);
            
            if (r < factor_boltzmann) {
                red[i][j] = -red[i][j]; // Aceptado probabilísticamente
            }
        }
    }

}

double calcular_magnetizacion(const std::vector<std::vector<int>>& red) {
    int L = red.size();
    double suma_espines = 0.0;

    // Recorremos la matriz sumando el valor de cada sitio
    for (const auto& fila : red) {
        for (int espin : fila) {
            suma_espines += espin;
        }
    }

    // Dividimos entre el número total de sitios (L * L) para normalizar
    return suma_espines / (L * L);
}


void guardar_historico_csv(int paso, double T, double H, double mag) {
    // Abrimos el archivo en modo "append" (std::ios::app) para añadir líneas al final sin borrar lo anterior
    std::ofstream archivo("curva_magnetizacion.csv", std::ios::app);
    
    if (archivo.is_open()) {
        archivo << paso << "," << T << "," << H << "," << mag << "\n";
        archivo.close();
    }
}

int main() {

    // Parámetros fijos de la red
    int L = 20; 
    double T = 1.5; 
    double H_min = -0.31; 
    double H_max = 0.31;
    int puntos_campo = 500; 
    double delta_H = (H_max - H_min) / puntos_campo;

    int pasos_termalizacion = 150; 
    int pasos_medicion = 50;       

    // CORRECCIÓN 3: Inicializar la red completamente ordenada (todos 1) ayuda a estabilizar el ciclo
    std::vector<std::vector<int>> red_espines = inicializar_red(L);
    // std::vector<std::vector<int>> red_espines(L, std::vector<int>(L, 1));

    std::ofstream archivo_curva("curva_histeresis.csv");
    archivo_curva << "campo,magnetizacion_media,direccion\n"; // Añadimos columna de dirección para Python
    archivo_curva.close();

    std::cout << "Iniciando ciclo de histéresis..." << std::endl;


    std::cout << "-> Graficando camino de ida..." << std::endl;
    for (int step = 0; step <= puntos_campo; ++step) {
        double H_actual = H_min + step * delta_H;

        for (int t = 0; t < pasos_termalizacion; ++t) {
            evolucion_montecarlo(red_espines, T, H_actual);
        }

        double suma_magnetizaciones = 0.0;
        for (int m = 0; m < pasos_medicion; ++m) {
            evolucion_montecarlo(red_espines, T, H_actual);
            suma_magnetizaciones += calcular_magnetizacion(red_espines);
        }
        
        double mag_promedio = suma_magnetizaciones / pasos_medicion;

        std::ofstream archivo_append("curva_histeresis.csv", std::ios::app);
        if (archivo_append.is_open()) {
            archivo_append << H_actual << "," << mag_promedio << ",ida\n";
            archivo_append.close();
        }
    }

    std::cout << "-> Graficando camino de regreso..." << std::endl;
    
    for (int step = puntos_campo; step >= 0; --step) {
        double H_actual = H_min + step * delta_H;

        for (int t = 0; t < pasos_termalizacion; ++t) {
            evolucion_montecarlo(red_espines, T, H_actual);
        }

        double suma_magnetizaciones = 0.0;
        for (int m = 0; m < pasos_medicion; ++m) {
            evolucion_montecarlo(red_espines, T, H_actual);
            suma_magnetizaciones += calcular_magnetizacion(red_espines);
        }
        
        double mag_promedio = suma_magnetizaciones / pasos_medicion;

        std::ofstream archivo_append("curva_histeresis.csv", std::ios::app);
        if (archivo_append.is_open()) {
            archivo_append << H_actual << "," << mag_promedio << ",regreso\n";
            archivo_append.close();
        }
    }
    return 0;
}

// int main() {
//     // Parámetros fijos de la red
//     int L = 20; 
//     double T = 2.0; // Temperatura baja para asegurar comportamiento ferromagnético
    
//     // Parámetros del barrido de Campo Magnético H
//     double H_min = -30.0;
//     double H_max = 30.0;
//     int puntos_campo = 100; // Cuántos puntos evaluaremos entre H_min y H_max
//     double delta_H = (H_max - H_min) / puntos_campo;

//     // Tiempos de simulación por cada punto de campo
//     int pasos_termalizacion = 100; // Pasos para que el sistema se adapte al nuevo H
//     int pasos_medicion = 50;       // Pasos donde promediaremos la magnetización

//     std::vector<std::vector<int>> red_espines = inicializar_red(L);

//     // Preparar el archivo histórico para la curva de magnetización
//     std::ofstream archivo_curva("curva_magnetizacion_H.csv");
//     archivo_curva << "campo,magnetizacion_media\n";
//     archivo_curva.close();

//     std::cout << "Iniciando barrido de campo H..." << std::endl;

//     for (int step = 0; step <= puntos_campo; ++step) {
//         double H_actual = H_min + step * delta_H;

//         // Termalización
//         for (int t = 0; t < pasos_termalizacion; ++t) {
//             evolucion_montecarlo(red_espines, T, H_actual);
//         }

//         // Medición: Promediamos la magnetización en varios pasos para mitigar las fluctuaciones
//         double suma_magnetizaciones = 0.0;
//         for (int m = 0; m < pasos_medicion; ++m) {
//             evolucion_montecarlo(red_espines, T, H_actual);
//             suma_magnetizaciones += calcular_magnetizacion(red_espines);
//         }
        
//         // Calculamos el valor promedio real en ese punto de campo
//         double mag_promedio = suma_magnetizaciones / pasos_medicion;

//         // C. Guardar el resultado del punto actual
//         std::ofstream archivo_append("curva_magnetizacion_H.csv", std::ios::app);
//         if (archivo_append.is_open()) {
//             archivo_append << H_actual << "," << mag_promedio << "\n";
//             archivo_append.close();
//         }

//         std::cout << "H = " << H_actual << " -> Magn. Promedio = " << mag_promedio << std::endl;
//     }

//     std::cout << "¡Simulación completada! Datos listos en 'curva_magnetizacion_H.csv'." << std::endl;
//     return 0;
// }