#include "matrix.h"

// Explicit template instantiation as advised at
// https://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file .
// This allows the method templates to be defined in this .cpp,
// instead of in the .h .
template class Matrix<int>;
template class Matrix<double>;

// initialise the matrix
// Adapt the randomisation logic from
// https://www.cplusplus.com/reference/random/
// FYI: distribution(generator) generates a number in the range lower..upper
template<>
void Matrix<int>::initialise(int lower, int upper, U nDiscards)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(lower, upper);

    // discard the first few generated items
    for (U i = 0; i < nDiscards; i++)
        distribution(generator);

    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            set_IJ(i, j, distribution(generator));
}

template<>
void Matrix<double>::initialise(int lower, int upper, U nDiscards)
{
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(lower, upper);

    // discard the first few generated items
    for (U i = 0; i < nDiscards; i++)
        distribution(generator);

    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            set_IJ(i, j, distribution(generator));
}

// GFS_display = "get format string for Matrix<T>::display()"
template<typename T> inline const char* GFS_display();
template<> inline const char* GFS_display<int>()    { return "%4d%c"; }
template<> inline const char* GFS_display<double>() { return "%16.16f%c"; }

// display the matrix
template<typename T>
void Matrix<T>::display()
{
    printf("nRows = %d; nCols = %d\n", nRows, nCols);
    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            printf(GFS_display<T>(), get_IJ(i, j), ((j == (nCols-1)) ? '\n' : ' '));
    printf("----\n");
}

// GFS_debug = "get format string for Matrix<T>::debug()"
template<typename T> inline const char* GFS_debug();
template<> inline const char* GFS_debug<int>()
{
    return "a = A[%d][%d] = %d; b = B[%d][%d] = %d; a * b = %d; sum = %d\n";
}
template<> inline const char* GFS_debug<double>()
{
    return "a = A[%d][%d] = %f; b = B[%d][%d] = %f; a * b = %f; sum = %f\n";
}

template<typename T>
Matrix<T>* Matrix<T>::multiply(const Matrix<T>* B) const
{
    U AR = nRows;
    U AC = nCols;
    U BR = B->get_nRows();
    U BC = B->get_nCols();

    assert(AC == BR);

    Matrix<T>* C = new Matrix<T>(AR, BC);

    for (U i = 0; i < AR; i++)
    {
        for (U k = 0; k < BC; k++)
        {
            T sum = 0;

            for (U j = 0; j < AC; j++)
            {
                T a = get_IJ(i, j);
                T b = B->get_IJ(j, k);
                T prod = a * b;
                sum += prod;
                DPRINTF(GFS_debug<T>(), i, j, a, j, k, b, prod, sum);
            }

            DPRINTF("----\n");

            C->set_IJ(i, k, sum);
        }
    }

    return C;
}
