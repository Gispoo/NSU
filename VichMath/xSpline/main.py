import numpy as np
import math
import matplotlib.pyplot as plt

def thomas(a, b, c, d):
    """
    Решает трёхдиагональную систему:
    a[i]*x[i-1] + b[i]*x[i] + c[i]*x[i+1] = d[i], i=0..n-1
    Здесь a[0]=0, c[n-1]=0.
    """
    n = len(b)
    c_ = np.zeros(n-1, dtype=float)
    d_ = np.zeros(n, dtype=float)

    c_[0] = c[0] / b[0]
    d_[0] = d[0] / b[0]
    for i in range(1, n-1):
        denom = b[i] - a[i] * c_[i-1]
        c_[i] = c[i] / denom
        d_[i] = (d[i] - a[i] * d_[i-1]) / denom
    d_[n-1] = (d[n-1] - a[n-1] * d_[n-2]) / (b[n-1] - a[n-1] * c_[n-2])

    x = np.zeros(n, dtype=float)
    x[-1] = d_[n-1]
    for i in range(n-2, -1, -1):
        x[i] = d_[i] - (c_[i] * x[i+1] if i < n-1 else 0.0)
    return x

def build_cubic_spline(x, y):
    """
    Естественный кубический сплайн.
    Находим c_i (вторые производные/2) из трёхдиагональной системы,
    затем a_i=y_i, b_i и d_i по стандартным формулам.
    """
    n = len(x)
    h = np.diff(x)

    if n < 3:
        # вырожденный случай — просто отрезки
        a = y[:-1].copy()
        b = np.diff(y)/h
        c = np.zeros(n)
        d = np.zeros(n-1)
        return a, b, c, d

    m = n - 2  # число внутренних c_i
    # Составляем систему для c[1..n-2]
    sub = np.zeros(m, dtype=float)   # поддиагональ
    main = np.zeros(m, dtype=float)  # главная
    sup  = np.zeros(m, dtype=float)  # наддиагональ
    rhs  = np.zeros(m, dtype=float)

    for i in range(1, n-1):
        k = i - 1
        sub[k]  = h[i-1]
        main[k] = 2*(h[i-1] + h[i])
        sup[k]  = h[i]
        rhs[k]  = 3*((y[i+1]-y[i])/h[i] - (y[i]-y[i-1])/h[i-1])

    # Решаем на внутренние c
    a_for_thomas = np.concatenate(([0.0], sub))[:m]  # a[0] не используется
    c_internal = thomas(a_for_thomas, main, sup, rhs)

    c = np.zeros(n, dtype=float)
    c[1:n-1] = c_internal

    a = y[:-1].copy()
    b = np.zeros(n-1, dtype=float)
    d = np.zeros(n-1, dtype=float)

    for i in range(n-1):
        b[i] = (y[i+1]-y[i])/h[i] - h[i]*(2*c[i] + c[i+1])/3
        d[i] = (c[i+1] - c[i])/(3*h[i])

    return a, b, c, d

def eval_spline(x_nodes, a, b, c, d, xq):
    i = np.searchsorted(x_nodes, xq) - 1
    i = max(0, min(i, len(x_nodes)-2))
    dx = xq - x_nodes[i]
    return a[i] + b[i]*dx + c[i]*dx*dx + d[i]*dx*dx*dx

# === Узлы: равномерно на [-1,1], n=5 ===
n = 5
x_nodes = np.linspace(-1.0, 1.0, n)
y_nodes = np.abs(x_nodes)

# Строим сплайн
a, b, c, d = build_cubic_spline(x_nodes, y_nodes)

# Сохраняем коэффициенты по сегментам
with open("spline_coeffs_n5_-1to1.txt", "w", encoding="utf-8") as f:
    f.write("Кубический естественный сплайн для |x| на [-1,1], n=5\n\n")
    f.write("Узлы:\n")
    for xi, yi in zip(x_nodes, y_nodes):
        f.write(f"x={xi:.2f}, y={yi:.2f}\n")
    f.write("\nКоэффициенты по отрезкам:\n")
    for i in range(n-1):
        f.write(f"[{x_nodes[i]:.2f}, {x_nodes[i+1]:.2f}]: ")
        f.write(f"S(x) = {a[i]:.6f} + {b[i]:.6f}(x-{x_nodes[i]:.2f}) + ")
        f.write(f"{c[i]:.6f}(x-{x_nodes[i]:.2f})^2 + {d[i]:.6f}(x-{x_nodes[i]:.2f})^3\n")

# График и CSV
xx = np.linspace(-1.0, 1.0, 2001)
yy_true = np.abs(xx)
yy_spl  = np.array([eval_spline(x_nodes, a, b, c, d, x) for x in xx])

plt.figure()
plt.plot(xx, yy_true, label='|x|')
plt.plot(xx, yy_spl, label='Кубический сплайн (n=5)')
plt.scatter(x_nodes, y_nodes, s=25, label='Узлы', zorder=3)
plt.title("Интерполяция |x| на [-1,1] кубическим сплайном (n=5)")
plt.xlabel("x"); plt.ylabel("y"); plt.grid(True); plt.legend()
plt.savefig("spline_abs_n5_-1to1.png", dpi=150, bbox_inches="tight")

with open("spline_abs_n5_-1to1.csv", "w", encoding="utf-8") as f:
    f.write("x,y_spline,y_exact\n")
    for x in np.linspace(-1.0, 1.0, 401):
        ys = eval_spline(x_nodes, a, b, c, d, float(x))
        f.write(f"{x:.6f},{ys:.12f},{abs(float(x)):.12f}\n")

# Печать краткой справки
max_err = float(np.max(np.abs(yy_spl - yy_true)))
print("Готово.")
print("Файлы: spline_abs_n5_-1to1.png, spline_abs_n5_-1to1.csv, spline_coeffs_n5_-1to1.txt")
print(f"Максимальная ошибка на плотной сетке: {max_err:.3e}")
