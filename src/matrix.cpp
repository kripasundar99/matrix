#include "matrix.h"

// Explicit template instantiation as advised at
// https://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file .
// This allows us to define the method templates here in this .cpp,
// instead of in the .h .
template class Matrix<int>;
template class Matrix<double>;

// Set each element of A to a random value.
// Adapt the randomisation logic from
// https://www.cplusplus.com/reference/random/
// FYI: distribution(generator) generates a number in the range lower..upper
template<>
void Matrix<int>::set_to_random(int lower, int upper, U nDiscards)
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
void Matrix<double>::set_to_random(int lower, int upper, U nDiscards)
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


// ----------------------------------------------------

// Here, and below, "GFS" means "get format string"

// GFS_display = GFS for Matrix<T>::display()
template<typename T> inline const char* GFS_display();
template<> inline const char* GFS_display<int>()    { return "%4d%c"; }
template<> inline const char* GFS_display<double>() { return "%8.2f%c"; }

// display the matrix
template<typename T>
void Matrix<T>::display(string label /* = "Matrix" */) const
{
    printf("%s: %d x %d\n", label.c_str(), nRows, nCols);

    // display the full matrix only for debug mode
    if (DEBUG_LEVEL > 0)
    {
        for (U i = 0; i < nRows; i++)
            for (U j = 0; j < nCols; j++)
                printf(GFS_display<T>(), get_IJ(i, j),
                    ((j == (nCols-1)) ? '\n' : ' '));
    }

    printf("----\n");
}


// ----------------------------------------------------


template<typename T>
void Matrix<T>::set_to_zero()
{
    memset(data, 0, nRows * nCols * sizeof(T));
}


// ----------------------------------------------------


template<typename T>
void Matrix<T>::set_to_identity()
{
    try
    {
        if (nRows != nCols)
            throw std::invalid_argument( "set_to_identity(): not a square" );

        set_to_zero();

        for (U i = 0; i < nRows; i++)
            set_IJ(i, i, 1);
    }
    catch(std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
    catch(...)
    {
        std::cerr << "Error: Unknown problem\n";
    }
}


// ----------------------------------------------------


template<typename T>
void Matrix<T>::set_to_identity(U nr)
{
    delete [] data;

    nRows = nCols = nr;
    data  = new T[nRows * nCols];

    set_to_zero();

    for (U i = 0; i < nRows; i++)
        set_IJ(i, i, 1);
}


// ----------------------------------------------------


template<typename T>
void Matrix<T>::set_to_copy(const Matrix<T>* B)
{
    delete [] data;

    nRows = B->get_nRows();
    nCols = B->get_nCols();
    data  = new T[nRows * nCols];

    memcpy(data, B->get_data(), nRows * nCols * sizeof(T));
}


// ----------------------------------------------------

// GFS_equals1 = GFS #1 for Matrix<T>::equals()
template<typename T> inline const char* GFS_equals1();
template<> inline const char* GFS_equals1<int>()
{
    return "i = %d; j = %d; a = %d; b = %d; a - b = %d\n";
}
template<> inline const char* GFS_equals1<double>()
{
    return "i = %d; j = %d; a = %f; b = %f; a - b = %f\n";
}

// GFS_equals2 = GFS #2 for Matrix<T>::equals()
template<typename T> inline const char* GFS_equals2();
template<> inline const char* GFS_equals2<int>()
{
    return "a = m1[%d][%d] = %d; m2 = B[%d][%d] = %d; a - b = %d\n";
}
template<> inline const char* GFS_equals2<double>()
{
    return "a = m1[%d][%d] = %40.40f; b = m2[%d][%d] = %40.40f; a - b = %40.40f\n";
}


template<typename T>
bool Matrix<T>::equals(const Matrix<T>* B) const
{
    U AR = nRows;
    U AC = nCols;
    U BR = B->get_nRows();
    U BC = B->get_nCols();

    if ((AR != BR) || (AC != BC))
    {
        DPRINTF(1)("dimension mismatch: m1[%d,%d] and m2[%d,%d]\n", AR, AC, BR, BC);
        return false;
    }

    for (U i = 0; i < nRows; i++)
    {
        for (U j = 0; j < nCols; j++)
        {
            T a = get_IJ(i, j);
            T b = B->get_IJ(i, j);

            DPRINTF(1)(GFS_equals1<T>(), i, j, a, b, a - b);

            if (a != b)
            {
                DPRINTF(1)(GFS_equals2<T>(), i, j, a, i, j, b, a - b);
                return false;
            }
        }
    }

    return true;
}


// ----------------------------------------------------



// GFS_multiply = GFS for Matrix<T>::multiply()
template<typename T> inline const char* GFS_multiply();
template<> inline const char* GFS_multiply<int>()
{
    return "a = A[%d][%d] = %d; b = B[%d][%d] = %d; a * b = %d; sum = %d\n";
}
template<> inline const char* GFS_multiply<double>()
{
    return "a = A[%d][%d] = %f; b = B[%d][%d] = %f; a * b = %f; sum = %f\n";
}

template<typename T>
Matrix<T>* Matrix<T>::multiply(const Matrix<T>* B) const
{
    try
    {
        U AR = nRows;
        U AC = nCols;
        U BR = B->get_nRows();
        U BC = B->get_nCols();

        if (AC != BR)
            throw std::invalid_argument( "multiply(): dimension mismatch" );

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
                    DPRINTF(2)(GFS_multiply<T>(), i, j, a, j, k, b, prod, sum);
                }

                DPRINTF(2)("----\n");

                C->set_IJ(i, k, sum);
            }
        }

        return C;
    }
    catch(std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return nullptr;
    }
    catch(...)
    {
        std::cerr << "Error: Unknown problem\n";
        return nullptr;
    }
}
