#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>      
#include <algorithm>
#include <iomanip>

struct Spline {
    std::vector<double> x; // Узлы
    std::vector<double> y; // Значения в узлах
    std::vector<double> h; // Шаги между узлами
    std::vector<double> a; // Коэффициенты a
    std::vector<double> b; // Коэффициенты b
    std::vector<double> c; // Коэффициенты c
    std::vector<double> d; // Коэффициенты d
    std::vector<std::vector<double>> matrix; // Матрица для системы уравнений
    std::vector<double> rhs; // Вектор правых частей
    std::vector<double> result; // Вектор решений
    int n; // Количество узлов
};

bool isTridiagonal(const std::vector<std::vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (std::abs(i - j) > 1 && std::fabs(matrix[i][j]) > 1e-12) {
                return false;
            }
        }
    }
    return true;
}

bool methodProg(std::vector<std::vector<double>>& matrix, std::vector<double>& d,std::vector<double>& result, int n) {
    std::cout << "Вычисляем СЛАУ с помощью метода прогонки" << "\n";
    if (!isTridiagonal(matrix)) {
        std::cerr << "\nОшибка: матрица не является трёхдиагональной!\n";
        std::cerr << "Метод прогонки можно применять только к трёхдиагональным системам.\n";
        return false;
    }

    // --- Метод прогонки ---
    std::vector<double> alpha(n - 1), beta(n);

    // Прямой ход
    alpha[0] = -matrix[0][1] / matrix[0][0];
    beta[0] = d[0] / matrix[0][0];

    for (int i = 1; i < n - 1; ++i) {
        double denom = matrix[i][i] + matrix[i][i - 1] * alpha[i - 1];
        if (std::fabs(denom) < 1e-12) {
            std::cerr << "Ошибка: деление на ноль в строке " << i + 1 << "\n";
            return 1;
        }
        alpha[i] = -matrix[i][i + 1] / denom;
        beta[i] = (d[i] - matrix[i][i - 1] * beta[i - 1]) / denom;
    }

    double denom = matrix[n - 1][n - 1] + matrix[n - 1][n - 2] * alpha[n - 2];
    if (std::fabs(denom) < 1e-12) {
        std::cerr << "Ошибка: деление на ноль в последней строке!\n";
        return 1;
    }
    beta[n - 1] = (d[n - 1] - matrix[n - 1][n - 2] * beta[n - 2]) / denom;

    // Обратный ход
    result[n - 1] = beta[n - 1];
    for (int i = n - 2; i >= 0; --i) {
        result[i] = alpha[i] * result[i + 1] + beta[i];
    }
    std::cout << "Вычисление СЛАУ завершилось успешно" << "\n";
    return true;
}
    
void inputSplineData(Spline& spline) {
    std::cout << "Введите интервал [a, b]: ";
    double a, b;
    std::cin >> a >> b;
    if (a >= b) {
        std::cerr << "Ошибка: неверный интервал.\n";
        exit(1);
    }
   
    std::cout << "Введите количество узлов, включая концы (n >= 2): ";
    std::cin >> spline.n;
    if (spline.n < 2) {
        std::cerr << "Ошибка: количество узлов должно быть не менее 2.\n";
        exit(1);
    }

    spline.x.resize(spline.n);
    spline.y.resize(spline.n);
    spline.a.resize(spline.n);
    spline.b.resize(spline.n - 1);
    spline.c.resize(spline.n);
    spline.d.resize(spline.n - 1);
    spline.matrix.resize(spline.n, std::vector<double>(spline.n, 0.0));
    spline.rhs.resize(spline.n, 0.0);
    spline.result.resize(spline.n, 0.0);

    spline.x[0] = a;
    spline.x[spline.n - 1] = b;
    std::cout << "Введите узлы x (по возрастанию и не учитывая границы):\n";
    for (int i = 1; i < spline.n - 1; ++i) {
        std::cin >> spline.x[i];
        if (i > 0 && spline.x[i - 1] >= spline.x[i]) {
            std::cerr << "Ошибка: узлы должны быть введены в порядке возрастания.\n";
            exit(1);
        }
    }

    std::cout << "Введите значения в узлах (учитывая границы, а также ввод осуществляется по одному элементу):\n";
    for (int i = 0; i < spline.n; ++i) {
        std::cin >> spline.y[i];

    }

    // Заполнение коэффициентов a
    for (int i = 0; i < spline.n; ++i) {
        spline.a[i] = spline.y[i];
    }


    // Получаем h - шаги между узлами
    spline.h.resize(spline.n - 1);
    for (int i = 0; i < spline.n - 1; ++i) {
        spline.h[i] = spline.x[i + 1] - spline.x[i];
    }
}

//Собираем матрицу и вектор правых частей
void buildMatrixAndRHS(Spline& spline) {
    int n = spline.n;
    // Заполнение матрицы и вектора правых частей для сплайна
    spline.matrix[0][0] = 1.0; // Естественные граничные условия
    spline.matrix[n - 1][n - 1] = 1.0; // Естественные граничные условия
    spline.rhs[0] = 0.0;
    spline.rhs[n-1] = 0.0;
    for (int i = 1; i < n - 1; ++i) {
        spline.matrix[i][i - 1] = spline.h[i - 1];
        spline.matrix[i][i] = 2.0 * (spline.h[i - 1] + spline.h[i]);
        spline.matrix[i][i + 1] = spline.h[i];
        spline.rhs[i] = 3.0 * (spline.y[i + 1] - spline.y[i]) / spline.h[i] - 3.0 * (spline.y[i] - spline.y[i - 1]) / spline.h[i - 1];
    }
}

//оценка сплайна в точке
double evalSplineAt(const Spline& s, double xq) {
    if (xq <= s.x.front()) return s.y.front();
    if (xq >= s.x.back())  return s.y.back();
    int i = int(std::upper_bound(s.x.begin(), s.x.end(), xq) - s.x.begin()) - 1;
    double t = xq - s.x[i];
    return s.a[i] + s.b[i]*t + s.c[i]*t*t + s.d[i]*t*t*t;
}

//экспорт в CSV
void exportSplineCSV(const Spline& s,const std::string& curvePath = "spline.csv", const std::string& nodesPath = "nodes.csv", int ptsPerSegment = 300) {
    std::ofstream fcurve(curvePath), fnodes(nodesPath);
    if (!fcurve || !fnodes) {
        std::cerr << "Не удалось открыть файлы для записи CSV.\n";
        return;
    }
    fcurve << "x,y\n";
    fnodes << "x,y\n";

    // узлы
    for (int i = 0; i < s.n; ++i)
        fnodes << std::setprecision(17) << s.x[i] << "," << s.y[i] << "\n";

    // кривая
    for (int seg = 0; seg < s.n - 1; ++seg) {
        double L = s.x[seg], R = s.x[seg + 1];
        for (int k = 0; k <= ptsPerSegment; ++k) {
            double xq = L + (R - L) * (double)k / ptsPerSegment;
            double yq = evalSplineAt(s, xq);
            fcurve << std::setprecision(17) << xq << "," << yq << "\n";
        }
    }
    std::cout << "CSV: spline.csv (кривая), nodes.csv (узлы)\n";
}

//текстовый отчёт
void writeReport(const Spline& s, const std::string& path = "report.txt") {
    std::ofstream out(path);
    if (!out) { std::cerr << "Не удалось открыть report.txt\n"; return; }
    out.setf(std::ios::fixed); out << std::setprecision(10);

    out << "Кубический сплайн (натуральные края)\n\n";
    out << "Узлы (x_i, y_i):\n";
    for (int i = 0; i < s.n; ++i)
        out << "  i=" << i << ": x=" << s.x[i] << ", y=" << s.y[i] << "\n";

    out << "\nКоэффициенты сегментов (локальная форма):\n";
    for (int i = 0; i < s.n - 1; ++i) {
        out << "Сегмент " << i
            << " на [" << s.x[i] << ", " << s.x[i+1] << "]\n";
        out << "  a=" << s.a[i]
            << ", b=" << s.b[i]
            << ", c=" << s.c[i]
            << ", d=" << s.d[i] << "\n";
        out << "  S_" << i << "(x) = "
            << s.a[i] << " + " << s.b[i] << "*(x-" << s.x[i] << ")"
            << " + " << s.c[i] << "*(x-" << s.x[i] << ")^2"
            << " + " << s.d[i] << "*(x-" << s.x[i] << ")^3\n\n";
    }

    std::cout << "Отчёт: report.txt\n";
}

int main() {
    setlocale(LC_ALL, "ru");
    Spline spline;
    inputSplineData(spline);
    buildMatrixAndRHS(spline);
    if (!methodProg(spline.matrix, spline.rhs, spline.result, spline.n)) {
        return 1;
    }
    spline.c = spline.result;

    // Заполнение коэффициентов b, c, d
    for (int i = 0; i < spline.n - 1; ++i) {
        spline.b[i] = (spline.a[i + 1] - spline.a[i]) / spline.h[i] - spline.h[i] * (2.0 * spline.c[i] + spline.c[i + 1]) / 3.0;
        spline.d[i] = (spline.c[i + 1] - spline.c[i]) / (3.0 * spline.h[i]);
    }


    std::cout << "Коэффициенты сплайна:\n";
    for (int i = 0; i < spline.n - 1; ++i) {
        std::cout << "----------------------------------------\n";
        std::cout << "Сегмент " << i + 1 << ": a = " << spline.a[i] << ", b = " << spline.b[i] << ", c = " << spline.c[i] << ", d = " << spline.d[i] << "\n";
        std::cout << "Уравнение на отрезке [" << spline.x[i] << ", " << spline.x[i + 1] << "]: S(x) = " 
                  << spline.a[i] << " + " 
                  << spline.b[i] << "(x - " << spline.x[i] << ") + " 
                  << spline.c[i] << "(x - " << spline.x[i] << ")^2 + " 
                  << spline.d[i] << "(x - " << spline.x[i] << ")^3\n";
    }

    exportSplineCSV(spline, "spline.csv", "nodes.csv", 100);
    writeReport(spline, "report.txt");

    return 0;
}
