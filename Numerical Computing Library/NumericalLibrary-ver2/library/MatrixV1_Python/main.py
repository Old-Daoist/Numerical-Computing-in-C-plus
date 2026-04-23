#!/usr/bin/env python3
"""MatrixV1 Python — interactive console menu. Mirrors main.cpp from MatrixV1 C++."""

import sys
from matrixv1.core.matrix import Matrix
from matrixv1.operations.divide import divide
from matrixv1.solvers.gauss_elimination import solve as gauss_solve
from matrixv1.solvers.lu_decomposition import solve as lu_solve
from matrixv1.solvers.cholesky import solve as cholesky_solve
from matrixv1.solvers.qr import solve as qr_solve
from matrixv1.solvers.gauss_jacobi import solve as jacobi_solve
from matrixv1.solvers.gauss_seidel import solve as seidel_solve
from matrixv1.numerical.interpolation import lagrange, newton
from matrixv1.numerical.eigen import gershgorin
from matrixv1.numerical.eigen_power import power_method, inverse_method
from matrixv1.numerical.least_squares import fit_line, fit_parabola


# ── I/O helpers ───────────────────────────────────────────────────────────────

def load_matrix() -> Matrix:
    print("\n  1. Input manually\n  2. Input from file")
    choice = input("  Choice: ").strip()
    if choice == "1":
        r = int(input("  Rows: "))
        c = int(input("  Cols: "))
        m = Matrix(r, c)
        print(f"  Enter {r * c} elements row-wise:")
        for i in range(r):
            for j in range(c):
                m.set(i, j, float(input(f"  [{i}][{j}]: ")))
        return m
    else:
        filename = input("  Filename: ").strip()
        m = Matrix.from_file(filename)
        print(f"  Loaded {m.rows}×{m.cols} matrix from {filename}")
        return m


def output_matrix(result: Matrix) -> None:
    print("\n  1. Display  2. Save to file  3. Both")
    choice = input("  Choice: ").strip()

    def show():
        print(str(result))

    def save():
        fn = input("  Filename: ").strip()
        result.to_file(fn)
        print(f"  Saved to {fn}")

    if choice == "1":
        show()
    elif choice == "2":
        save()
    elif choice == "3":
        show()
        save()
    else:
        print("  Invalid choice")


def input_vector(n: int) -> list:
    raw = input(f"  Enter RHS vector ({n} values, space-separated): ")
    return list(map(float, raw.split()))


def output_vector(x: list) -> None:
    print("\n  1. Display  2. Save to file  3. Both")
    choice = input("  Choice: ").strip()

    def show():
        print("  Solution: " + "  ".join(f"{v:.8g}" for v in x))

    def save():
        fn = input("  Filename: ").strip()
        with open(fn, "w") as f:
            for v in x:
                f.write(f"{v}\n")
        print(f"  Saved to {fn}")

    if choice == "1":
        show()
    elif choice == "2":
        save()
    elif choice == "3":
        show()
        save()
    else:
        print("  Invalid choice")


def input_interp_data():
    n = int(input("  Number of data points: "))
    xs = list(map(float, input("  Enter x values (space-separated): ").split()))
    ys = list(map(float, input("  Enter y values (space-separated): ").split()))
    val = float(input("  Interpolate at x = "))
    return xs, ys, val


# ── Menu ──────────────────────────────────────────────────────────────────────

MENU = """
  ╔══════════════════════════════════╗
  ║        MATRIX OPERATIONS         ║
  ╠══════════════════════════════════╣
  ║  1.  Load Matrix A               ║
  ║  2.  Load Matrix B               ║
  ║  3.  Add          (A + B)        ║
  ║  4.  Subtract     (A - B)        ║
  ║  5.  Multiply     (A * B)        ║
  ║  6.  Divide       (A * inv(B))   ║
  ║  7.  Transpose of A              ║
  ║  8.  Determinant of A            ║
  ║  9.  Inverse of A                ║
  ╠══════════════════════════════════╣
  ║        LINEAR SOLVERS            ║
  ╠══════════════════════════════════╣
  ║  10. Gauss Elimination           ║
  ║  11. LU Decomposition            ║
  ║  12. Cholesky Decomposition      ║
  ║  13. QR Decomposition            ║
  ╠══════════════════════════════════╣
  ║        ITERATIVE SOLVERS         ║
  ╠══════════════════════════════════╣
  ║  14. Gauss-Jacobi                ║
  ║  15. Gauss-Seidel                ║
  ╠══════════════════════════════════╣
  ║        NUMERICAL METHODS         ║
  ╠══════════════════════════════════╣
  ║  16. Lagrange Interpolation      ║
  ║  17. Newton Interpolation        ║
  ║  18. Gershgorin Eigenvalue Est.  ║
  ║  19. Power Method (dominant λ)   ║
  ║  20. Inverse Power (smallest λ)  ║
  ╠══════════════════════════════════╣
  ║        CURVE FITTING             ║
  ╠══════════════════════════════════╣
  ║  21. Least Squares — Line        ║
  ║  22. Least Squares — Parabola    ║
  ╠══════════════════════════════════╣
  ║   0. Exit                        ║
  ╚══════════════════════════════════╝"""


def main():
    A = Matrix(0, 0)
    B = Matrix(0, 0)

    while True:
        print(MENU)
        try:
            choice = int(input("  Choice: ").strip())
        except ValueError:
            print("  Please enter a number.")
            continue

        try:
            if choice == 0:
                print("  Goodbye.")
                sys.exit(0)

            elif choice == 1:
                A = load_matrix()

            elif choice == 2:
                B = load_matrix()

            elif choice == 3:
                output_matrix(A + B)

            elif choice == 4:
                output_matrix(A - B)

            elif choice == 5:
                output_matrix(A * B)

            elif choice == 6:
                output_matrix(divide(A, B))

            elif choice == 7:
                output_matrix(A.transpose())

            elif choice == 8:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                print(f"  Determinant of A = {A.determinant():.10g}")

            elif choice == 9:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                output_matrix(A.inverse())

            elif choice == 10:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                b = input_vector(A.rows)
                output_vector(gauss_solve(A, b))

            elif choice == 11:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                b = input_vector(A.rows)
                output_vector(lu_solve(A, b))

            elif choice == 12:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                b = input_vector(A.rows)
                output_vector(cholesky_solve(A, b))

            elif choice == 13:
                b = input_vector(A.rows)
                output_vector(qr_solve(A, b))

            elif choice == 14:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                b = input_vector(A.rows)
                x, iters = jacobi_solve(A, b, tol=1e-6, max_iter=1000)
                print(f"  Converged in {iters} iteration(s).")
                output_vector(x)

            elif choice == 15:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                b = input_vector(A.rows)
                x, iters = seidel_solve(A, b, tol=1e-6, max_iter=1000)
                print(f"  Converged in {iters} iteration(s).")
                output_vector(x)

            elif choice == 16:
                xs, ys, val = input_interp_data()
                print(f"  Lagrange result = {lagrange(xs, ys, val):.10g}")

            elif choice == 17:
                xs, ys, val = input_interp_data()
                print(f"  Newton result   = {newton(xs, ys, val):.10g}")

            elif choice == 18:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                gershgorin(A)

            elif choice == 19:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                lam, vec = power_method(A, iterations=200)
                print(f"  Dominant eigenvalue  = {lam:.10g}")
                print("  Eigenvector: " + "  ".join(f"{v:.6g}" for v in vec))

            elif choice == 20:
                if not A.is_square():
                    print("  Matrix A must be square.")
                    continue
                lam, vec, iters = inverse_method(A, tol=1e-9, max_iter=1000)
                print(f"  Smallest eigenvalue  = {lam:.10g}")
                print(f"  Converged in {iters} iteration(s).")
                print("  Eigenvector: " + "  ".join(f"{v:.6g}" for v in vec))

            elif choice == 21:
                n = int(input("  Number of data points: "))
                xs = list(map(float, input("  Enter x values (space-separated): ").split()))
                ys = list(map(float, input("  Enter y values (space-separated): ").split()))
                a, b = fit_line(xs, ys)
                print(f"  Fitted line:  y = {a:.10g} + {b:.10g}x")

            elif choice == 22:
                n = int(input("  Number of data points: "))
                xs = list(map(float, input("  Enter x values (space-separated): ").split()))
                ys = list(map(float, input("  Enter y values (space-separated): ").split()))
                a, b, c = fit_parabola(xs, ys)
                print(f"  Fitted curve: y = {a:.10g} + {b:.10g}x + {c:.10g}x\u00b2")

            else:
                print("  Invalid choice — enter a number from the menu.")

        except Exception as e:
            print(f"  Error: {e}")


if __name__ == "__main__":
    main()
