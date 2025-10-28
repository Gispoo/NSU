import math

def f(x): 
    return x*x - 7.0

def fp(x):
    return 2.0*x

def newton_sequence(x0, steps=4):
    """Ньютон: x_{k+1} = x_k - f(x_k)/f'(x_k)"""
    xs = [x0]
    for _ in range(steps):
        x = xs[-1]
        xs.append(x - f(x)/fp(x))
    return xs

def one_tangent_sequence(x0, steps=4):
    """Одна касательная
    x_{k+1} = x_k - f(x_k)/f'(x0).
    """
    d0 = fp(x0)
    xs = [x0]
    for _ in range(steps):
        x = xs[-1]
        xs.append(x - f(x)/d0)
    return xs

def secant_sequence(x0, x1, steps=4):
    """Секущие: x_{k+1} = x_k - f(x_k)*(x_k-x_{k-1})/(f(x_k)-f(x_{k-1}))"""
    xs = [x0, x1]
    for _ in range(steps-1):
        xkm1, xk = xs[-2], xs[-1]
        fxkm1, fxk = f(xkm1), f(xk)
        x_next = xk - fxk * (xk - xkm1) / (fxk - fxkm1)
        xs.append(x_next)
    return xs

def print_table(title, xs, true_root):
    w_k   = 3
    w_x   = 26
    w_err = 26
    fmt_num = ".17g"

    print("\n" + title)
    print("-" * (w_k + w_x + w_err + 6))
    header = f"{'k':>{w_k}}  {'x_k':>{w_x}}  {'|x_k - sqrt(7)|':>{w_err}}"
    print(header)
    print("-" * (w_k + w_x + w_err + 6))
    for k, x in enumerate(xs):
        err = abs(x - true_root)
        print(f"{k:>{w_k}d}  {format(x, fmt_num):>{w_x}}  {format(err, fmt_num):>{w_err}}")
    print("-" * (w_k + w_x + w_err + 6))
    print(f"-> x2 = {format(xs[2], fmt_num)}, x3 = {format(xs[3], fmt_num)}, x4 = {format(xs[4], fmt_num)}")
    print(f"|x4 - sqrt(7)| = {format(abs(xs[4]-true_root), fmt_num)}")

def print_summary(errs):
    w_name = 28
    w_err  = 26
    fmt_num = ".17g"
    print("\nСравнение ошибок |x4 - sqrt(7)| (по возрастанию):")
    print("-" * (w_name + w_err + 3))
    print(f"{'Метод':<{w_name}}  {'Ошибка':>{w_err}}")
    print("-" * (w_name + w_err + 3))
    for name, e in errs:
        print(f"{name:<{w_name}}  {format(e, fmt_num):>{w_err}}")
    print("-" * (w_name + w_err + 3))

def main():
    x0 = 2.0
    true_root = math.sqrt(7.0)

    # Метод Ньютона
    xs_newton = newton_sequence(x0, steps=4)

    # Метод одной касательной
    xs_one_tangent = one_tangent_sequence(x0, steps=4)

    # Метод секущих
    x1_for_secant = xs_newton[1]
    xs_secant = secant_sequence(x0, x1_for_secant, steps=4)

    print_table("Ньютон", xs_newton, true_root)
    print_table("Одна касательная (фикс. f'(x0))", xs_one_tangent, true_root)
    print_table("Секущие (x1 как у Ньютона)", xs_secant, true_root)

    errs = [
        ("Ньютон", abs(xs_newton[4] - true_root)),
        ("Секущие", abs(xs_secant[4] - true_root)),
        ("Одна касательная", abs(xs_one_tangent[4] - true_root)),
    ]
    errs_sorted = sorted(errs, key=lambda t: t[1])
    print_summary(errs_sorted)

if __name__ == "__main__":
    main()
