#include "matrix.h"

// Explicit template instantiation
// as advised at https://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file
template class Matrix<int>;
template class Matrix<double>;

// initialise the matrix
// Adapt the randomisation logic from
// https://www.cplusplus.com/reference/random/
template<typename T>
void Matrix<T>::initialise(U lower, U upper, U discard)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(lower, upper);
    // FYI: distribution(generator) generates number in the range lower..upper

    // [Sat/2022/04/09, 4:19:42 PM]
    // TODO:
    // I need to handle `int` distribution and `double` distribution here.

    // discard the first few generated items
    for (U d = 0; d < discard; d++)
        distribution(generator);

    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            set_IJ(i, j, distribution(generator));
}

// display the matrix
template<typename T>
void Matrix<T>::display()
{
    printf("nRows = %d; nCols = %d\n", nRows, nCols);
    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            cout << get_IJ(i, j) << ((j == (nCols-1)) ? '\n' : ' ');
    printf("----\n");
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
            int sum = 0;

            for (U j = 0; j < AC; j++)
            {
                int a = get_IJ(i, j);
                int b = B->get_IJ(j, k);
                int prod = a * b;
                sum += prod;
                DPRINTF("a = A[%d][%d] = %d; b = B[%d][%d] = %d; a * b = %d; sum = %d\n",
                    i, j, a,
                    j, k, b,
                    prod,
                    sum
                );
            }

            DPRINTF("----\n");

            C->set_IJ(i, k, sum);
        }
    }

    return C;
}
