#pragma once

#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#define DEBUG 0
#define DPRINTF if (DEBUG) printf

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

    // get and set the [i][j]'th element in data.
    T get_IJ(U i, U j) const { return data[i * nCols + j]; }
    void set_IJ(U i, U j, T value) const { data[i * nCols + j] = value; }

    Matrix<T>(U nr, U nc)
    {
        nRows = nr;
        nCols = nc;
        data  = new T[nr * nc];
    }

    ~Matrix<T>()
    {
        delete [] data;
    }

    void initialise(int lower, int upper, U nDiscards);
    void display();
    Matrix<T>* multiply(const Matrix<T>* B) const;

private:
    U     nRows;    // number of rows in matrix
    U     nCols;    // number of columns in matrix
    T*    data;     // the actual data of the matrix
};

