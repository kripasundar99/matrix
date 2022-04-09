#include "matrix.h"

// initialise the matrix
// Adapt the randomisation logic from
// https://www.cplusplus.com/reference/random/
void Matrix_int::initialise(U lower, U upper, U discard)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(lower, upper);
    // FYI: distribution(generator) generates number in the range lower..upper

    // discard the first few generated items
    for (U d = 0; d < discard; d++)
        distribution(generator);

    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            set_IJ(i, j, distribution(generator));
}

// display the matrix
void Matrix_int::display()
{
    printf("nRows = %d; nCols = %d\n", nRows, nCols);
    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            printf("%4d%c", get_IJ(i, j), ((j == (nCols-1)) ? '\n' : ' '));
    printf("----\n");
}

Matrix_int* Matrix_int::multiply(const Matrix_int* B) const
{
    U AR = nRows;
    U AC = nCols;
    U BR = B->get_nRows();
    U BC = B->get_nCols();

    assert(AC == BR);

    Matrix_int* C = new Matrix_int(AR, BC);

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
