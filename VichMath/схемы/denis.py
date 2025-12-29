import numpy as np
import matplotlib.pyplot as plt

L = 10.0
T_list = [0.5, 1.0, 2.0]
N_list = [100, 1000]
lambda_list = [0.05, 0.1, 0.2, 0.25]  

# f(u) = u^2/2
def flux(u):
    return 0.5 * u**2

# u(x,0) = sin(2*pi*(x-5)/10) + 3
def initial_condition(x):
    return np.sin(2.0 * np.pi * (x - 5.0) / 10.0) + 3.0


# СХЕМА 1: разность вперёд 
def step_forward(u, r):
    f = flux(u)
    f_plus = np.roll(f, -1)
    return u - r * (f_plus - f)

# СХЕМА 2: разность назад
def step_backward(u, r):
    f = flux(u)
    f_minus = np.roll(f, 1)
    return u - r * (f - f_minus)

# СХЕМА 3: Русанов
def step_rusanov(u, r, filter_c=2.5):
    with np.errstate(over="ignore", invalid="ignore"):
        f = flux(u)

        # --- ШАГ 1 ---
        u_p1 = np.roll(u, -1)
        f_p1 = np.roll(f, -1)
        u_half = 0.5 * (u + u_p1) - (r / 3.0) * (f_p1 - f)
        f_half = flux(u_half)

        # --- ШАГ 2 ---
        u_mid = u - (2.0 * r / 3.0) * (f_half - np.roll(f_half, 1))
        f_mid = flux(u_mid)

        # --- ШАГ 3 ---
        f_m1 = np.roll(f, 1)
        f_p2 = np.roll(f, -2)
        f_m2 = np.roll(f, 2)

        term_a = 7.0 * (f_p1 - f_m1) - 2.0 * (f_p2 - f_m2)
        term_b = np.roll(f_mid, -1) - np.roll(f_mid, 1)

        w4 = (
            np.roll(u, -2)
            - 4.0 * np.roll(u, -1)
            + 6.0 * u
            - 4.0 * np.roll(u, 1)
            + np.roll(u, 2)
        )

        eps = filter_c / 24.0  # C/24, где C=2.5

        return (
            u
            - (r / 24.0) * term_a
            - (3.0 * r / 8.0) * term_b
            - eps * w4
        )

schemes = {
    "forward": step_forward,
    "backward": step_backward,
    "rusanov": step_rusanov,
}

scheme_titles = {
    "forward": "Разность вперёд ",
    "backward": "Разность назад ",
    "rusanov": "Русанов ",
}

cfl_safety = 0.9   # можно 1.0, но лучше с запасом
eps = 1e-12

for scheme_name, step_func in schemes.items():
    for N in N_list:
        h = L / N
        x = np.linspace(0.0, L, N, endpoint=False)

        for lam in lambda_list:
            plt.figure(figsize=(8, 4))

            for T in T_list:
                u = initial_condition(x)
                t = 0.0

                while t < T - 1e-15:
                    umax = np.max(np.abs(u))
                    # CFL с листочка: r <= 1/max|u|
                    r_cfl = cfl_safety / (umax + eps)

                    # берём то, что ты хочешь (lam), но не больше CFL
                    r_eff = min(lam, r_cfl)

                    tau = r_eff * h
                    # чтобы ровно не перескочить T
                    if t + tau > T:
                        tau = T - t
                        r_eff = tau / h

                    u = step_func(u, r_eff)
                    t += tau

                print("scheme=",scheme_name," N=",N," lam_cap=",lam,
                      " T=",T," t=",t," max=",np.max(u)," min=",np.min(u))

                plt.plot(x, u, label=f"T={T} (t={t:.4f})")

            plt.xlabel("x")
            plt.ylabel("u(x,t)")
            plt.title(f"{scheme_titles[scheme_name]}, N={N}, lam_cap={lam} + CFL")
            plt.grid(True)
            plt.legend()

            filename = f"{scheme_name}_N{N}_lam{lam:.2f}_CFL.png"
            plt.tight_layout()
            plt.savefig(filename, dpi=150)
            plt.close()
            print(f"Сохранён файл {filename}")
