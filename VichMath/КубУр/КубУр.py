import math
from typing import List, Tuple, Callable, Optional

class Cubic:
    def __init__(self, a: float, b: float, c: float, eps: float, delta: float):
        self.a = a
        self.b = b
        self.c = c
        self.eps = eps
        self.delta = delta
    
    def f(self, x: float) -> float:
        return ((x + self.a) * x + self.b) * x + self.c
    
    def fmod(self, x: float) -> float:
        return abs(self.f(x))
    
    def crit(self) -> Tuple[float, float, bool]:
        D = 4 * (self.a * self.a - 3 * self.b)
        if D <= 0:
            return 0, 0, False
        s = math.sqrt(D)
        alpha = (-2 * self.a - s) / 6.0
        beta = (-2 * self.a + s) / 6.0
        return alpha, beta, True

def almost_zero(x: float, eps: float) -> bool:
    return abs(x) <= eps

def bisect(f: Callable[[float], float], L: float, R: float, eps: float) -> float:
    fL, fR = f(L), f(R)
    if fL == 0:
        return L
    if fR == 0:
        return R
    
    for i in range(200):
        m = 0.5 * (L + R)
        fm = f(m)
        if abs(fm) <= eps or abs(R - L) <= 2 * eps:
            return m
        if fL * fm <= 0:
            R, fR = m, fm
        else:
            L, fL = m, fm
    return 0.5 * (L + R)

def bracket_right_from(f: Callable[[float], float], pivot: float, delta: float) -> Optional[Tuple[float, float]]:
    L, fL = pivot, f(pivot)
    step = delta
    for i in range(100):
        R = L + step
        fR = f(R)
        if fL * fR <= 0:
            return L, R
        step *= 2
    return None

def bracket_left_from(f: Callable[[float], float], pivot: float, delta: float) -> Optional[Tuple[float, float]]:
    R, fR = pivot, f(pivot)
    step = delta
    for i in range(100):
        L = R - step
        fL = f(L)
        if fL * fR <= 0:
            return L, R
        step *= 2
    return None

def solve_real_roots(eps: float, delta: float, a: float, b: float, c: float) -> List[float]:
    cb = Cubic(a, b, c, eps, delta)
    roots = []

    alpha, beta, two = cb.crit()
    
    if not two:
        f0 = cb.f(0)
        if almost_zero(f0, eps):
            return [0.0]
        
        if f0 < -eps:
            bracket_result = bracket_right_from(cb.f, 0 + delta, delta)
            if bracket_result is not None:
                L, R = bracket_result
                return [bisect(cb.f, L, R, eps)]
            else:
                return [bisect(cb.f, 0, delta, eps)]
        
        bracket_result = bracket_left_from(cb.f, 0 - delta, delta)
        if bracket_result is not None:
            L, R = bracket_result
            return [bisect(cb.f, L, R, eps)]
        else:
            return [bisect(cb.f, -delta, 0, eps)]

    fa = cb.f(alpha)
    fb = cb.f(beta)

    if almost_zero(fa, eps) and almost_zero(fb, eps):
        return [0.5 * (alpha + beta)]

    if almost_zero(fa, eps) and fb < -eps:
        roots = [alpha, alpha]
        bracket_result = bracket_right_from(cb.f, beta + delta, delta)
        if bracket_result is not None:
            L, R = bracket_result
            roots.append(bisect(cb.f, L, R, eps))
        roots.sort()
        return roots

    if almost_zero(fb, eps) and fa > eps:
        roots = [beta, beta]
        bracket_result = bracket_left_from(cb.f, alpha - delta, delta)
        if bracket_result is not None:
            L, R = bracket_result
            roots.append(bisect(cb.f, L, R, eps))
        roots.sort()
        return roots

    if fa > eps and fb > eps:
        bracket_result = bracket_left_from(cb.f, alpha, delta)
        if bracket_result is not None:
            L, R = bracket_result
            return [bisect(cb.f, L, R, eps)]

    if fa < -eps and fb < -eps:
        bracket_result = bracket_right_from(cb.f, beta, delta)
        if bracket_result is not None:
            L, R = bracket_result
            return [bisect(cb.f, L, R, eps)]

    if fa >= eps and fb <= -eps:
        bracket_result_left = bracket_left_from(cb.f, alpha, delta)
        if bracket_result_left is not None:
            L, R = bracket_result_left
            roots.append(bisect(cb.f, L, R, eps))
        
        roots.append(bisect(cb.f, alpha, beta, eps))
        
        bracket_result_right = bracket_right_from(cb.f, beta, delta)
        if bracket_result_right is not None:
            L, R = bracket_result_right
            roots.append(bisect(cb.f, L, R, eps))
        
        roots.sort()
        return roots

    roots.append(bisect(cb.f, alpha, beta, eps))
    roots.sort()
    return roots

def main():
    data = [
        (10865.01, -1076608.65, 10765, 9),
        (2119.25, 200369.5846, -155541.54, 6),
        (10664.99, -1076606.65, 10765, 9),
        (-107.02, 702.14, -14, 6),
        (101014, 404044, 404040, 7.77),
        (101006, -404036, 404040, 7.77),
        (-10009, 60027, -90027, 1.111),
        (-99997.78, -221998.7679, -123210, 101),
        (-333333, 0, 0, 107),
        (6.6, 10.89, 0, 0.23),
        (-6.6, 10.89, 0, 0.23),
        (29.7, 294.03, 970.299, 200),
        (-21.9, 159.87, -389.017, 200),
        (0, 12345, 0, 200),
        (-1.777, 100.23, -178.10871, 1000),
        (1.111, 100, 111.1, 1000),
        (0, 0, 0, 1000)
    ]

    test_data = [
        (-0.000001, 1, -0.000001, 1000),
        (0, -2, 0, 3)
    ]

    epsilons = [1e-7, 1e-4, 0.1, 1.0]

    print("Тесты с eps = 1e-5")
    eps = 1e-5
    for idx, (a, b, c, delta) in enumerate(data, 1):
        cb = Cubic(a, b, c, eps, delta)
        roots = solve_real_roots(eps, delta, a, b, c)
        print(f"Случай {idx}:")
        print(f"a={a:.8g}, b={b:.8g}, c={c:.8g}, delta={delta:.8g}")
        print(f"1) Количество корней: {len(roots)}")
        for i, r in enumerate(roots, 1):
            print(f"2.{i}) x{i} = {r:.12f}")
            print(f"3.{i}) |f(x{i})| = {cb.fmod(r):.12e}")
        print()

    print("Проверка EPS")
    for eps in epsilons:
        print(f"\nEPS = {eps:.7f}")
        for i, (a, b, c, delta) in enumerate(test_data, 1):
            cb = Cubic(a, b, c, eps, delta)
            roots = solve_real_roots(eps, delta, a, b, c)
            print(f"{i}) a={a:.8g}, b={b:.8g}, c={c:.8g}, delta={delta:.8g}")
            print(f"1) Количество корней: {len(roots)}")
            for j, r in enumerate(roots, 1):
                print(f"2.{j}) x{j} = {r:.12f}")
                print(f"3.{j}) |f(x{j})| = {cb.fmod(r):.12e}")
            print()

if __name__ == "__main__":
    main()