"""Core Matrix class — mirrors MatrixV1 C++ Matrix interface."""

from __future__ import annotations

import math
import copy
from typing import List


class Matrix:
    """2-D matrix of floats backed by a list-of-lists.

    Indexing is 0-based throughout. All mutating operations return a new
    Matrix; the original is never modified.
    """

    # ── Construction ─────────────────────────────────────────────────────────

    def __init__(self, rows: int = 0, cols: int = 0) -> None:
        if rows < 0 or cols < 0:
            raise ValueError(f"Dimensions must be non-negative, got ({rows}, {cols})")
        self._rows = rows
        self._cols = cols
        self._data: List[List[float]] = [
            [0.0] * cols for _ in range(rows)
        ]

    @classmethod
    def from_list(cls, data: List[List[float]]) -> "Matrix":
        """Build a Matrix from a nested list (no copy of outer list, but rows are copied)."""
        if not data:
            return cls(0, 0)
        rows = len(data)
        cols = len(data[0])
        m = cls(rows, cols)
        for i, row in enumerate(data):
            if len(row) != cols:
                raise ValueError("All rows must have the same length")
            for j, val in enumerate(row):
                m._data[i][j] = float(val)
        return m

    @classmethod
    def from_file(cls, filename: str) -> "Matrix":
        """Load from file. Format: first line is 'rows cols', then data row-by-row."""
        with open(filename) as f:
            rows, cols = map(int, f.readline().split())
            m = cls(rows, cols)
            for i in range(rows):
                values = list(map(float, f.readline().split()))
                for j, v in enumerate(values):
                    m._data[i][j] = v
        return m

    # ── Accessors ─────────────────────────────────────────────────────────────

    @property
    def rows(self) -> int:
        return self._rows

    @property
    def cols(self) -> int:
        return self._cols

    def _check_bounds(self, i: int, j: int) -> None:
        if i < 0 or i >= self._rows or j < 0 or j >= self._cols:
            raise IndexError(
                f"Matrix index ({i}, {j}) out of range [{self._rows}x{self._cols}]"
            )

    def get(self, i: int, j: int) -> float:
        self._check_bounds(i, j)
        return self._data[i][j]

    def set(self, i: int, j: int, val: float) -> None:
        self._check_bounds(i, j)
        self._data[i][j] = float(val)

    # ── Predicates ────────────────────────────────────────────────────────────

    def is_square(self) -> bool:
        return self._rows == self._cols

    def is_symmetric(self, tol: float = 1e-9) -> bool:
        if not self.is_square():
            return False
        for i in range(self._rows):
            for j in range(i + 1, self._cols):
                if abs(self._data[i][j] - self._data[j][i]) > tol:
                    return False
        return True

    # ── Utilities ─────────────────────────────────────────────────────────────

    def transpose(self) -> "Matrix":
        result = Matrix(self._cols, self._rows)
        for i in range(self._rows):
            for j in range(self._cols):
                result._data[j][i] = self._data[i][j]
        return result

    def _submatrix(self, skip_row: int, skip_col: int) -> "Matrix":
        """Return (n-1)×(n-1) matrix with row skip_row and col skip_col removed."""
        result = Matrix(self._rows - 1, self._cols - 1)
        ri = 0
        for i in range(self._rows):
            if i == skip_row:
                continue
            rj = 0
            for j in range(self._cols):
                if j == skip_col:
                    continue
                result._data[ri][rj] = self._data[i][j]
                rj += 1
            ri += 1
        return result

    def _cofactor_det(self) -> float:
        """Recursive cofactor expansion — O(n!), for small matrices / educational use."""
        if self._rows == 1:
            return self._data[0][0]
        if self._rows == 2:
            return (self._data[0][0] * self._data[1][1]
                    - self._data[0][1] * self._data[1][0])
        det = 0.0
        for j in range(self._cols):
            sign = 1.0 if j % 2 == 0 else -1.0
            det += sign * self._data[0][j] * self._submatrix(0, j)._cofactor_det()
        return det

    def determinant(self, method: str = "lu") -> float:
        """Compute determinant via 'lu' (default, O(n³)) or 'cofactor' (O(n!))."""
        if not self.is_square():
            raise ValueError("determinant() requires a square matrix")
        if method == "cofactor":
            return self._cofactor_det()
        # LU-based: partial pivoting, count swaps for sign
        n = self._rows
        A = copy.deepcopy(self._data)
        swaps = 0
        for i in range(n):
            pivot = max(range(i, n), key=lambda p: abs(A[p][i]))
            if abs(A[pivot][i]) < 1e-12:
                return 0.0
            if pivot != i:
                A[i], A[pivot] = A[pivot], A[i]
                swaps += 1
            for j in range(i + 1, n):
                factor = A[j][i] / A[i][i]
                for k in range(i, n):
                    A[j][k] -= factor * A[i][k]
        det = -1.0 if swaps % 2 else 1.0
        for i in range(n):
            det *= A[i][i]
        return det

    def inverse(self) -> "Matrix":
        """Gauss-Jordan elimination on [A | I]. Raises ValueError if singular or non-square."""
        if not self.is_square():
            raise ValueError("inverse() requires a square matrix")
        n = self._rows
        # Build augmented [A | I]
        aug = [[0.0] * (2 * n) for _ in range(n)]
        for i in range(n):
            for j in range(n):
                aug[i][j] = self._data[i][j]
            aug[i][n + i] = 1.0
        # Forward elimination with partial pivoting
        for i in range(n):
            pivot = max(range(i, n), key=lambda p: abs(aug[p][i]))
            if abs(aug[pivot][i]) < 1e-12:
                raise ValueError("Matrix is singular — inverse does not exist")
            if pivot != i:
                aug[i], aug[pivot] = aug[pivot], aug[i]
            scale = aug[i][i]
            for j in range(2 * n):
                aug[i][j] /= scale
            for k in range(n):
                if k == i:
                    continue
                factor = aug[k][i]
                for j in range(2 * n):
                    aug[k][j] -= factor * aug[i][j]
        result = Matrix(n, n)
        for i in range(n):
            for j in range(n):
                result._data[i][j] = aug[i][n + j]
        return result

    # ── I/O ───────────────────────────────────────────────────────────────────

    def to_file(self, filename: str) -> None:
        with open(filename, "w") as f:
            f.write(f"{self._rows} {self._cols}\n")
            for row in self._data:
                f.write(" ".join(str(v) for v in row) + "\n")

    def input_from_console(self) -> None:
        print(f"  Enter {self._rows * self._cols} elements row-wise:")
        for i in range(self._rows):
            for j in range(self._cols):
                self._data[i][j] = float(input(f"  [{i}][{j}]: "))

    # ── Operators ─────────────────────────────────────────────────────────────

    def __add__(self, other: "Matrix") -> "Matrix":
        from matrixv1.operations.add import add
        return add(self, other)

    def __sub__(self, other: "Matrix") -> "Matrix":
        from matrixv1.operations.subtract import subtract
        return subtract(self, other)

    def __mul__(self, other: "Matrix") -> "Matrix":
        from matrixv1.operations.multiply import multiply
        return multiply(self, other)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Matrix):
            return NotImplemented
        if self._rows != other._rows or self._cols != other._cols:
            return False
        tol = 1e-9
        for i in range(self._rows):
            for j in range(self._cols):
                if abs(self._data[i][j] - other._data[i][j]) > tol:
                    return False
        return True

    def __ne__(self, other: object) -> bool:
        result = self.__eq__(other)
        if result is NotImplemented:
            return result
        return not result

    def __repr__(self) -> str:
        return f"Matrix({self._rows}x{self._cols})"

    def __str__(self) -> str:
        lines = []
        for row in self._data:
            lines.append("  " + "  ".join(f"{v:10.6g}" for v in row))
        return "\n".join(lines)

    def display(self) -> None:
        print(str(self))

    # ── Internal helpers (used by solvers) ────────────────────────────────────

    def _raw(self) -> List[List[float]]:
        """Return a deep copy of the internal data list."""
        return copy.deepcopy(self._data)
