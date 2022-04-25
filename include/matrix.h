#pragma once

/*

Terminology and conventions

------------------------------------------------------------------------

Matrix names:
In matrix.h and matrix.cpp, we use A, B, and C for matrix names.
In client code (main.cpp), we never use the names A, B, and C.

* A: synonym for current matrix (`this` or current object in C++)
* B: the second operand of a binary operation
* C: the destination of a binary operation

------------------------------------------------------------------------

Methods:

* operations that modify the whole of A
  - they have the prefix `set_to_`.
  - they modify `data` in-place.
  - they do *not* modify `nRows` or `nCols`.

------------------------------------------------------------------------

*/

#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#define DEBUG_LEVEL 0
#define DPRINTF(_level) if (DEBUG_LEVEL >= (_level)) printf

using namespace std;

using U = unsigned int;

template<typename T>
class Matrix
{
public:

    // ------------------ constructors and destructor ------------------ //
    Matrix<T>(U nr, U nc)   { construct(nr, nc); }
    Matrix<T>(U n)          { construct(n, n); }
    ~Matrix<T>()            { delete [] data; }

    // ------------------ getters and setters ------------------ //
    U get_nRows() const { return nRows; }
    void set_nRows(U nr) { nRows = nr; }

    U get_nCols() const { return nCols; }
    void set_nCols(U nc) { nCols = nc; }

    // Get and set the [i][j]'th element in data.
    T get_IJ(U i, U j) const { return data[i * nCols + j]; }
    void set_IJ(U i, U j, T value) const { data[i * nCols + j] = value; }

    // Get direct access to the data block.
    T* get_data() const { return data; }
    // We don't want, and we don't need set_data().

    // --------------- methods that modify the whole of A --------------- //
    // Set each element of A to a random value.
    void set_to_random(int lower, int upper, U nDiscards);

    // Set A to zero matrix.  Need not be square.
    void set_to_zero();

    // Set A to identity matrix of existing dimensions.  Must already be square.
    void set_to_identity();

    // Set A to [n x n] identity matrix.  Will be square by construction.
    void set_to_identity(U n);

    // Set A to -A.
    void set_to_negative();

    // Copy data from B into A.
    void set_to_copy(const Matrix<T>* B);

    // ---------------- methods that do not modify A ---------------- //
    // Display A.
    void display(string label = "Matrix", bool always_show_data = false) const;

    // return "A.dimensions == B.dimensions"
    bool dimensions_match(const Matrix<T>* B) const;

    // return A == B
    bool equals(const Matrix<T>* B) const;

    // return -A
    Matrix<T>* get_negative() const;

    // return A + B
    Matrix<T>* add(const Matrix<T>* B) const;

    // return A - B
    Matrix<T>* subtract(const Matrix<T>* B) const;

    // Multiply a square block (sub-matrix) of A and B.
    // The top-left cell is at [init_row, init_col].
    // The bottom-right cell is at [init_row + size - 1, init_col + size - 1].
    // Return the result as a square matrix.
    //
    // For two [n, n] matrices A and B, multiply_block(B, 0, 0, n) should be
    // identical to multiply(B).
    Matrix<T>* multiply_block(const Matrix<T>* B, U init_row, U init_col, U size) const;

    // return A * B
    Matrix<T>* multiply(const Matrix<T>* B) const;

private:
    U     nRows;    // number of rows in matrix
    U     nCols;    // number of columns in matrix
    T*    data;     // the data = the actual contents of the matrix

    void construct(U nr, U nc)
    {
        try
        {
            if (!nr)
                throw std::invalid_argument( "Matrix<T>(): zero rows" );
            if (!nc)
                throw std::invalid_argument( "Matrix<T>(): zero cols" );

            nRows = nr;
            nCols = nc;
            data  = new T[nRows * nCols];
        }
        catch(std::exception &e)
        {
            std::cerr << "Error: " << e.what() << "\n";
            exit(1);
        }
        catch(...)
        {
            std::cerr << "Error: Unknown problem\n";
            exit(1);
        }
    }


    // helper
    Matrix<T>* helper_for_add_sub(bool isAddition, const Matrix<T>* B) const;
};

