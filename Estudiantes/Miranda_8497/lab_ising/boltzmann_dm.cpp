// Ecuacion de Boltzmann para el yield de materia oscura (Lisanti, eq. 3.39)
//   dY/dx = -(lambda / x^2) * (Y^2 - Yeq^2),   x = m/T
//   Yeq(x) = 0.145 (g/g*s) x^{3/2} e^{-x}
// La ecuacion es rigida (lambda >> 1): se usa Euler implicito,
// que en cada paso se reduce a resolver una cuadratica en Y_{n+1}.

#include <iostream>
#include <fstream>
#include <cmath>

const double g = 2.0, gs = 100.0;

double Yeq(double x) { return 0.145 * (g / gs) * std::pow(x, 1.5) * std::exp(-x); }

int main() {
    const double lambdas[4] = {1e7, 1e8, 1e9, 1e10};
    const double x0 = 1.0, xf = 1000.0;
    const int nPasos = 200000;
    const double dlx = std::log(xf / x0) / nPasos;   // paso uniforme en ln(x)

    std::ofstream out("boltzmann.csv");
    out << "lambda,x,Y,Yeq\n";

    for (double lam : lambdas) {
        double x = x0, Y = Yeq(x0);                  // inicio en equilibrio
        for (int n = 0; n < nPasos; ++n) {
            double xn = x * std::exp(dlx);
            double h  = xn - x;
            double a  = h * lam / (xn * xn);
            double ye = Yeq(xn);
            // a*Y^2 + Y - (Y_n + a*Yeq^2) = 0  ->  raiz positiva
            double c  = Y + a * ye * ye;
            Y = (-1.0 + std::sqrt(1.0 + 4.0 * a * c)) / (2.0 * a);
            x = xn;
            if (n % 500 == 0) out << lam << "," << x << "," << Y << "," << ye << "\n";
        }
        std::cout << "lambda = " << lam << "  ->  Y_inf = " << Y << "\n";
    }
    std::cout << "Datos guardados en boltzmann.csv\n";
    return 0;
}
