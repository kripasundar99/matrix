#pragma once

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#include <random>
#include <assert.h>

#define DEBUG 0
#define DPRINTF if (DEBUG) printf

using namespace std;

using U = unsigned int;

class Matrix_int
{
public:
    U get_nRows() const { return nRows; }
    void set_nRows(U nr) { nRows = nr; }

    U get_nCols() const { return nCols; }
    void set_nCols(U nc) { nCols = nc; }

    // get and set the [i][j]'th element in data.
    int get_IJ(U i, U j) const { return data[i * nCols + j]; }
    void set_IJ(U i, U j, int value) const { data[i * nCols + j] = value; }

    Matrix_int(U nr, U nc)
    {
        nRows = nr;
        nCols = nc;
        data  = new int[nr * nc];
    }

    ~Matrix_int()
    {
        delete [] data;
    }

    void initialise(U lower, U upper, U discard);
    void display();
    Matrix_int* multiply(const Matrix_int* B) const;

private:
    U     nRows;    // number of rows in matrix
    U     nCols;    // number of columns in matrix
    int*  data;     // the actual data of the matrix
};

