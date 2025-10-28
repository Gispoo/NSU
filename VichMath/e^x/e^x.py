import numpy as np, math

def I_e():  # ∫_0^1 e^x dx
    return math.e - 1.0

def trapezoid(n):
    a, b = 0.0, 1.0
    h = (b - a) / n
    x = np.linspace(a, b, n+1)
    y = np.exp(x)
    return h * (0.5 * y[0] + y[1:-1].sum() + 0.5 * y[-1])

def K(n):
    I = I_e()
    Sh   = trapezoid(n)
    Sh2  = trapezoid(2 * n)
    k = math.log(abs(I - Sh) / abs(I - Sh2), 2)
    return I, Sh, Sh2, k

if __name__ == "__main__":
    n = 10
    I, Sh, Sh2, k = K(n)
    print(f"n = {n:3d}  S_h = {Sh:.16f}  S_2h = {Sh2:.16f}  k = {k:.4f}  I_e = {I:.16f}")

    n *= 2
    I, Sh, Sh2, k = K(n)
    print(f"n = {n:3d}  S_h = {Sh:.16f}  S_2h = {Sh2:.16f}  k = {k:.4f}  I_e = {I:.16f}")

    n *= 5
    I, Sh, Sh2, k = K(n)
    print(f"n = {n:3d}  S_h = {Sh:.16f}  S_2h = {Sh2:.16f}  k = {k:.4f}  I_e = {I:.16f}")

    n *= 2
    I, Sh, Sh2, k = K(n)
    print(f"n = {n:3d}  S_h = {Sh:.16f}  S_2h = {Sh2:.16f}  k = {k:.4f}  I_e = {I:.16f}")
    
# n =  10  S_h = 1.7197134913893146  S_2h = 1.7186397889252214  k = 1.9998  I_e = 1.7182818284590451
# n =  20  S_h = 1.7186397889252214  S_2h = 1.7183713213720642  k = 2.0000  I_e = 1.7182818284590451
# n = 100  S_h = 1.7182961474504177  S_2h = 1.7182854082113630  k = 2.0000  I_e = 1.7182818284590451
# n = 200  S_h = 1.7182854082113630  S_2h = 1.7182827233974045  k = 2.0000  I_e = 1.7182818284590451