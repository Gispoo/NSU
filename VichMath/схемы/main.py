import numpy as np
import matplotlib.pyplot as plt

# Параметры задачи
LENGTH = 10.0           # длина области
TIMES = [0.5, 1.0, 2.0]  # моменты времени для вывода
CELL_COUNTS = [100, 1000]  # число ячеек
LAMBDA_VALS = [0.05, 0.1, 0.2, 0.25]  # параметры τ/h

# Нелинейный поток: f(u) = u²/2
def flux_func(u):
    return 0.5 * u * u

# Начальное условие: u(x,0) = sin(2π(x-5)/10) + 3
def u_initial(x):
    return np.sin(2.0 * np.pi * (x - 5.0) / 10.0) + 3.0

# Схема 1: явная с разностью вперёд
def explicit_forward(u, r_param):
    f = flux_func(u)
    f_next = np.roll(f, -1)
    return u - r_param * (f_next - f)

# Схема 2: явная с разностью назад
def explicit_backward(u, r_param):
    f = flux_func(u)
    f_prev = np.roll(f, 1)
    return u - r_param * (f - f_prev)

# Схема 3: трёхшаговая схема Русанова
def rusanov_3step(u, r_param, filter_coeff=2.5):
    with np.errstate(over='ignore', invalid='ignore'):
        f = flux_func(u)
        
        # Шаг 1 (предиктор)
        u_next = np.roll(u, -1)
        f_next = np.roll(f, -1)
        u_half = 0.5*(u + u_next) - (r_param/3.0)*(f_next - f)
        f_half = flux_func(u_half)
        
        # Шаг 2 (корректор)
        u_mid = u - (2.0*r_param/3.0)*(f_half - np.roll(f_half, 1))
        f_mid = flux_func(u_mid)
        
        # Шаг 3 (финальный + фильтр)
        f_p1 = np.roll(f, -1)
        f_m1 = np.roll(f, 1)
        f_p2 = np.roll(f, -2)
        f_m2 = np.roll(f, 2)
        
        term1 = 7.0*(f_p1 - f_m1) - 2.0*(f_p2 - f_m2)
        term2 = np.roll(f_mid, -1) - np.roll(f_mid, 1)
        
        # Антидиффузионный фильтр
        filter_term = (
            np.roll(u, -2) - 4.0*np.roll(u, -1) + 6.0*u
            - 4.0*np.roll(u, 1) + np.roll(u, 2)
        )
        eps_filter = filter_coeff / 24.0
        
        return u - (r_param/24.0)*term1 - (3.0*r_param/8.0)*term2 - eps_filter*filter_term

# Словарь схем для удобного перебора
scheme_dict = {
    "forward": explicit_forward,
    "backward": explicit_backward,
    "rusanov": rusanov_3step
}

# Русские названия для графиков
scheme_names_ru = {
    "forward": "Явная схема (разность вперёд)",
    "backward": "Явная схема (разность назад)", 
    "rusanov": "Схема Русанова 3-го порядка"
}

# Коэффициент запаса для CFL
CFL_SAFETY = 0.9
SMALL_NUMBER = 1e-12

# Основной цикл расчётов
for scheme_key, scheme_func in scheme_dict.items():
    for N_cells in CELL_COUNTS:
        dx = LENGTH / N_cells
        x_grid = np.linspace(0.0, LENGTH, N_cells, endpoint=False)
        
        for lam in LAMBDA_VALS:
            fig, ax = plt.subplots(figsize=(9, 5))
            
            for T_end in TIMES:
                u_now = u_initial(x_grid)
                current_time = 0.0
                
                # Цикл по времени до достижения T_end
                while current_time < T_end - 1e-15:
                    u_max = np.max(np.abs(u_now))
                    # Вычисляем допустимый шаг по CFL
                    r_cfl = CFL_SAFETY / (u_max + SMALL_NUMBER)
                    # Берём минимум из желаемого и допустимого
                    r_actual = min(lam, r_cfl)
                    
                    dt = r_actual * dx
                    # Корректируем последний шаг, чтобы точно попасть в T_end
                    if current_time + dt > T_end:
                        dt = T_end - current_time
                        r_actual = dt / dx
                    
                    u_now = scheme_func(u_now, r_actual)
                    current_time += dt
                
                # Вывод информации в консоль
                print(f"Схема: {scheme_key:8} N={N_cells:4} λ={lam:5.2f} "
                      f"T={T_end:4.1f} t={current_time:.4f} "
                      f"max={np.max(u_now):6.3f} min={np.min(u_now):6.3f}")
                
                # Добавляем кривую на график
                ax.plot(x_grid, u_now, linewidth=1.5, label=f"T={T_end} (t={current_time:.4f})")
            
            # Оформление графика
            ax.set_xlabel("Координата x", fontsize=12)
            ax.set_ylabel("Решение u(x,t)", fontsize=12)
            ax.set_title(f"{scheme_names_ru[scheme_key]}\n"
                        f"Число ячеек N={N_cells}, λ={lam} + условие CFL", 
                        fontsize=13)
            ax.grid(True, alpha=0.3)
            ax.legend(loc='best')
            
            # Сохранение графика
            file_name = f"scheme_{scheme_key}_N{N_cells}_lambda{lam:.2f}_CFL.png"
            plt.tight_layout()
            plt.savefig(file_name, dpi=150)
            plt.close()
            print(f"График сохранён: {file_name}\n")