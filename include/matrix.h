#pragma once

#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#define DEBUG 0
#define DPRINTF(_level) if (DEBUG >= (_level)) printf

using namespace std;

using U = unsigned int;

template<typename T>
class Matrix
{
public:
    U get_nRows() const { return nRows; }
    void set_nRows(U nr) { nRows = nr; }

    U get_nCols() const { return nCols; }
    void set_nCols(U nc) { nCols = nc; }

    // Get and set the [i][j]'th element in data.
    T get_IJ(U i, U j) const { return data[i * nCols + j]; }
    void set_IJ(U i, U j, T value) const { data[i * nCols + j] = value; }

    // Get direct access to the data block.
    T* get_data() const { return data; }
    // We don't need set_data().

    Matrix<T>(U nr, U nc)   { construct(nr, nc); }
    Matrix<T>(U n)          { construct(n, n); }
    ~Matrix<T>()            { delete [] data; }

    void randomize(int lower, int upper, U nDiscards);
    void display(string label = "Matrix") const;

    // Set to zero matrix.  Need not be square.
    void set_to_zero();

    // Set to identity matrix of existing dimensions.  Must already be square.
    void set_to_identity();

    // Set to [n x n] identity matrix.  Will be square by construction.
    void set_to_identity(U n);

    // Copy data from B into A.
    void copy_from(const Matrix<T>* B);

    bool equals(const Matrix<T>* B) const;

    // NYI
    //Matrix<T>* negate() const;
    //Matrix<T>* add(const Matrix<T>* B) const;
    //Matrix<T>* subtract(const Matrix<T>* B) const;

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
};

