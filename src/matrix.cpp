#include "matrix.h"

// Explicit template instantiation as advised at
// https://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file .
// This allows us to define the method templates here in this .cpp,
// instead of in the .h .
template class Matrix<int>;
template class Matrix<double>;

using M_int = Matrix<int>;
using M_dbl = Matrix<double>;

// Yet another experiment
template
Matrix<int>* assemble(Matrix<int>* m11, Matrix<int>* m12,
    Matrix<int>* m21, Matrix<int>* m22);

template
Matrix<double>* assemble(Matrix<double>* m11, Matrix<double>* m12,
    Matrix<double>* m21, Matrix<double>* m22);

// ----------------------------------------------------

template<typename T>
void Matrix<T>::construct(U nr, U nc)
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

// ----------------------------------------------------

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
void Matrix<T>::display(string label /* = "Matrix" */,
    bool always_show_data /* = false */) const
{
    printf("%s: %d x %d\n", label.c_str(), nRows, nCols);

    // display the full matrix only for debug mode
    if (always_show_data || (DEBUG_LEVEL > 0))
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

template<typename T>
void Matrix<T>::copy_block(const Matrix<T>* B, U size,
    U init_row_A /* = 0 */, U init_col_A /* = 0 */,
    U init_row_B /* = 0 */, U init_col_B /* = 0 */)
{
    assert(get_nRows() >= (init_row_A + size));
    assert(get_nCols() >= (init_col_A + size));

    assert(B->get_nRows() >= (init_row_B + size));
    assert(B->get_nCols() >= (init_col_B + size));

    for (U i = 0; i < size; i++)
        for (U j = 0; j < size; j++)
            set_IJ(init_row_A + i, init_col_A + j,
                B->get_IJ(init_row_B + i, init_col_B + j));
}

// ----------------------------------------------------

template<typename T>
bool Matrix<T>::dimensions_match(const Matrix<T>* B) const
{
    U AR = nRows;
    U AC = nCols;
    U BR = B->get_nRows();
    U BC = B->get_nCols();

    if ((AR != BR) || (AC != BC))
    {
        DPRINTF(1)("dimension mismatch: A[%d,%d] and B[%d,%d]\n", AR, AC, BR, BC);
        return false;
    }

    return true;
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

// TODO:
// Review this design decision!
//
// Currently, `tolerance` is always a double, irrespective of `T`.
//
// Arguably:
// * if `T` is `int`, then we should not support non-zero `tolerance`.
// * if `T` is `float` or `double`, `tolerance` should be a double.
//
// Separate topic:
// What I really want is an *unsigned* double.  But C++ doesn't have native
// support for that.
template<typename T>
bool Matrix<T>::equals(const Matrix<T>* B, double tolerance /* = 0 */) const
{
    if (!dimensions_match(B))
        return false;

    for (U i = 0; i < nRows; i++)
    {
        for (U j = 0; j < nCols; j++)
        {
            T a = get_IJ(i, j);
            T b = B->get_IJ(i, j);

            DPRINTF(1)(GFS_equals1<T>(), i, j, a, b, a - b);

            if (abs(a - b) > tolerance)
            {
                DPRINTF(1)(GFS_equals2<T>(), i, j, a, i, j, b, a - b);
                return false;
            }
        }
    }

    return true;
}

// ----------------------------------------------------

template<typename T>
void Matrix<T>::set_to_negative()
{
    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            set_IJ(i, j, -get_IJ(i, j));
}

// ----------------------------------------------------

template<typename T>
Matrix<T>* Matrix<T>::get_negative() const
{
    Matrix<T>* C = new Matrix<T>(nRows, nCols);

    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            C->set_IJ(i, j, -get_IJ(i, j));

    return C;
}

// ----------------------------------------------------

template<typename T>
Matrix<T>* Matrix<T>::helper_for_add_sub(bool isAddition, const Matrix<T>* B) const
{
    try
    {
        if (!dimensions_match(B))
        {
            string msg =
                (string(isAddition ? "add" : "sub")
                    + string("(): dimension mismatch"));
            throw std::invalid_argument(msg);
        }

        Matrix<T>* C = new Matrix<T>(nRows, nCols);

        for (U i = 0; i < nRows; i++)
        {
            for (U j = 0; j < nCols; j++)
            {
                T a = get_IJ(i, j);
                T b = B->get_IJ(i, j);
                T sum = (isAddition ? (a + b) : (a - b));
                C->set_IJ(i, j, sum);
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

template<typename T>
Matrix<T>* Matrix<T>::add(const Matrix<T>* B) const
{
    return helper_for_add_sub(true, B);
}

template<typename T>
Matrix<T>* Matrix<T>::subtract(const Matrix<T>* B) const
{
    return helper_for_add_sub(false, B);
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
Matrix<T>* Matrix<T>::multiply_blocks(const Matrix<T>* B, U size,
    U init_row_A /* = 0 */, U init_col_A /* = 0 */,
    U init_row_B /* = 0 */, U init_col_B /* = 0 */) const
{
    try
    {
        U AR = nRows;
        U AC = nCols;
        U BR = B->get_nRows();
        U BC = B->get_nCols();

        if ((AR < (init_row_A + size)) || (AC < (init_col_A + size)) ||
            (BR < (init_row_B + size)) || (BC < (init_col_B + size)))
            throw std::invalid_argument( "multiply(): sub-matrix doesn't fit" );

        Matrix<T>* C = new Matrix<T>(size, size);

        for (U i = 0; i < size; i++)
        {
            for (U k = 0; k < size; k++)
            {
                T sum = 0;

                for (U j = 0; j < size; j++)
                {
                    T a = get_IJ(init_row_A + i, init_col_A + j);
                    T b = B->get_IJ(init_row_B + j, init_col_B + k);
                    T prod = a * b;
                    sum += prod;
                    DPRINTF(2)(GFS_multiply<T>(),
                        init_row_A + i, init_col_A + j, a,
                        init_row_B + j, init_col_B + k, b,
                        prod, sum);
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

// ----------------------------------------------------

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

// ----------------------------------------------------

bool is_power_of_2(U n)
{
    // Source: http://www.graphics.stanford.edu/~seander/bithacks.html
    return (n && !(n & (n - 1)));
}

template<typename T>
Matrix<T>* Matrix<T>::naive_block_based_multiply(const Matrix<T>* B) const
{
    U size = get_nRows();
    assert(size == get_nCols());
    assert(size == B->get_nRows());
    assert(size == B->get_nCols());

    assert(is_power_of_2(size));

    U s2 = size / 2;

    // Source for this naive block-based multiply:
    // https://en.wikipedia.org/wiki/Strassen_algorithm

    auto A11B11 = multiply_blocks(B, s2);
    auto A12B21 = multiply_blocks(B, s2, 0, s2, s2, 0);
    auto C11 = A11B11->add(A12B21);

    auto A11B12 = multiply_blocks(B, s2, 0, 0, 0, s2);
    auto A12B22 = multiply_blocks(B, s2, 0, s2, s2, s2);
    auto C12 = A11B12->add(A12B22);

    auto A21B11 = multiply_blocks(B, s2, s2, 0);
    auto A22B21 = multiply_blocks(B, s2, s2, s2, s2, 0);
    auto C21 = A21B11->add(A22B21);

    auto A21B12 = multiply_blocks(B, s2, s2, 0, 0, s2);
    auto A22B22 = multiply_blocks(B, s2, s2, s2, s2, s2);
    auto C22 = A21B12->add(A22B22);

    return assemble(C11, C12, C21, C22);
}


// ----------------------------------------------------

/*
template<typename T>
Matrix<T>* Matrix<T>::Strassen_multiply(const Matrix<T>* B) const
{
    assert(0);
    return nullptr;
}
*/

// ----------------------------------------------------

// Assemble the four input square matrices into a single large square matrix.
template<typename T>
Matrix<T>* assemble(Matrix<T>* m11, Matrix<T>* m12,
    Matrix<T>* m21, Matrix<T>* m22)
{
    U size = m11->get_nRows();

    if (size != m11->get_nCols())
    {
        printf("assemble(): m11 is not square\n");
        return nullptr;
    }

    for (const auto& m : { m12, m21, m22 })
    {
        if ((size != m->get_nRows()) || (size != m->get_nCols()))
        {
            printf("assemble(): size mismatch\n");
            return nullptr;
        }
    }

    Matrix<T>* C = new Matrix<T>(size * 2, size * 2);
    C->copy_block(m11, size);
    C->copy_block(m12, size, 0, size);
    C->copy_block(m21, size, size, 0);
    C->copy_block(m22, size, size, size);
    return C;
}

