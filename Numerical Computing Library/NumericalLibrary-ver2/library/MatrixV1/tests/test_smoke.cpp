#include "catch2/catch_amalgamated.hpp"
#include "Matrix.hpp"

// ─── Construction ─────────────────────────────────────────────────────────────

TEST_CASE("Matrix default constructor creates 0x0 matrix", "[matrix][construction]") {
    Matrix m;
    REQUIRE(m.getRows() == 0);
    REQUIRE(m.getCols() == 0);
}

TEST_CASE("Matrix parameterised constructor sets correct dimensions", "[matrix][construction]") {
    Matrix m(3, 4);
    REQUIRE(m.getRows() == 3);
    REQUIRE(m.getCols() == 4);
}

TEST_CASE("Matrix 1x1 constructor works", "[matrix][construction]") {
    Matrix m(1, 1);
    REQUIRE(m.getRows() == 1);
    REQUIRE(m.getCols() == 1);
}

// ─── Default initialisation ───────────────────────────────────────────────────

TEST_CASE("Matrix elements are zero-initialised", "[matrix][construction]") {
    Matrix m(3, 3);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            REQUIRE(m.get(i, j) == 0.0);
}

// ─── get / set ────────────────────────────────────────────────────────────────

TEST_CASE("Matrix set and get roundtrip", "[matrix][access]") {
    Matrix m(2, 2);
    m.set(0, 0, 1.5);
    m.set(0, 1, 2.5);
    m.set(1, 0, 3.5);
    m.set(1, 1, 4.5);

    REQUIRE(m.get(0, 0) == 1.5);
    REQUIRE(m.get(0, 1) == 2.5);
    REQUIRE(m.get(1, 0) == 3.5);
    REQUIRE(m.get(1, 1) == 4.5);
}

TEST_CASE("Matrix set overwrites previous value", "[matrix][access]") {
    Matrix m(2, 2);
    m.set(0, 0, 10.0);
    m.set(0, 0, 99.0);
    REQUIRE(m.get(0, 0) == 99.0);
}
