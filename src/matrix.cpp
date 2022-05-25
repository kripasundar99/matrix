#include "matrix.h"

using Mx_int = Matrix<int>;
using Mx_dbl = Matrix<double>;

// Explicit template instantiation as advised at
// https://stackoverflow.com/questions/115703/storing-c-template-function-definitions-in-a-cpp-file .
// This allows us to define the method templates here in this .cpp,
// instead of in the .h .
template class Matrix<int>;
template class Matrix<double>;

// Explicit template instantiation.
template
Mx_int* assemble(Mx_int* m11, Mx_int* m12, Mx_int* m21, Mx_int* m22);
template
Mx_dbl* assemble(Mx_dbl* m11, Mx_dbl* m12, Mx_dbl* m21, Mx_dbl* m22);

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

static U get_num_discards()
{
    static U num_discards = 0;
    static U step_up = 20;

    // If we used zero discards, or the same discard count each time, the
    // contents of each matrix would start off at the same location in the
    // pseudo-random sequence.
    // e.g.: 7, 3, 5, 2, 4, 9, 8, 6, 1, 0, 3, 2, ...
    // m1 = [ 7 3 5 | 2 4 9 | 8 6 1 ]
    // m2 = [ 7 3 | 5 2 | 4 9 ]

    // To prevent this, each time we call get_num_discards(), we step up the
    // number of items we are going to discard.

    num_discards += step_up;
    return num_discards;
}

// Set each element of A to a random value.
// Adapt the randomisation logic from
// https://www.cplusplus.com/reference/random/
// FYI: distribution(generator) generates a number in the range lower..upper
template<>
void Mx_int::set_to_random(int lower, int upper)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(lower, upper);

    // discard the first few generated items
    U nDiscards = get_num_discards();
    for (U i = 0; i < nDiscards; i++)
        distribution(generator);

    for (U i = 0; i < nRows; i++)
        for (U j = 0; j < nCols; j++)
            set_IJ(i, j, distribution(generator));
}

template<>
void Mx_dbl::set_to_random(int lower, int upper)
{
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(lower, upper);

    // discard the first few generated items
    U nDiscards = get_num_discards();
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

// display a block of the matrix
template<typename T>
void Matrix<T>::display_block(string label, U size,
    U init_row /* = 0 */, U init_col /* = 0 */) const
{
    if (DEBUG_LEVEL <= 0)
        return;

    assert(nRows >= (init_row + size));
    assert(nCols >= (init_col + size));

    printf("%s: %d x %d; size = %d; init = [%d, %d]\n",
        label.c_str(), nRows, nCols, size, init_row, init_col);

    for (U i = 0; i < size; i++)
        for (U j = 0; j < size; j++)
            printf(GFS_display<T>(), get_IJ(init_row + i, init_col + j),
                ((j == (size-1)) ? '\n' : ' '));

    printf("----\n");
}

// display the matrix
template<typename T>
const Matrix<T>* Matrix<T>::display(string label /* = "{unknown matrix}" */,
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

    // For convenience, return `this`.
    // This enables the idiom shown in code fragment F2 below.
    //
    // For better or worse, my desire for F2 makes me propagate the `const`
    // property to the LHS.
    //
    // Specifically, f1 is a non-const pointer, while f2 is a const pointer.
    // For now, I consider that a good thing.
    //
    // Code fragments:
    // F1 = { auto f1 = multiply(B); }
    // F2 = { auto f2 = multiply(B)->display("product"); }
    return this;
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
void Matrix<T>::set_to_identity(U n)
{
    delete [] data;

    nRows = nCols = n;
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
void Matrix<T>::set_block_to_copy(const Matrix<T>* B, U size,
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
// * if `T` is `int`, then we should assert that `tolerance` is zero.
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
Matrix<T>* Matrix<T>::helper_for_add_sub_blocks(bool isAddition,
    const Matrix<T>* B, U size,
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
        {
            string msg =
                (string(isAddition ? "add" : "sub")
                    + string("_blocks(): sub-matrix doesn't fit"));
            throw std::invalid_argument(msg);
        }

        const auto A = this;
        A->display_block("X", size, init_row_A, init_col_A);
        B->display_block("Y", size, init_row_B, init_col_B);

        Matrix<T>* C = new Matrix<T>(size, size);

        for (U i = 0; i < size; i++)
        {
            for (U j = 0; j < size; j++)
            {
                T a = get_IJ(init_row_A + i, init_col_A + j);
                T b = B->get_IJ(init_row_B + i, init_col_B + j);
                T sum = (isAddition ? (a + b) : (a - b));
                C->set_IJ(i, j, sum);
            }
        }

        C->display_block((isAddition ? "X+Y" : "X-Y"), size);
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
Matrix<T>* Matrix<T>::add_blocks(const Matrix<T>* B, U size,
    U init_row_A /* = 0 */, U init_col_A /* = 0 */,
    U init_row_B /* = 0 */, U init_col_B /* = 0 */) const
{
    return helper_for_add_sub_blocks(true, B, size,
        init_row_A, init_col_A, init_row_B, init_col_B);
}

template<typename T>
Matrix<T>* Matrix<T>::subtract_blocks(const Matrix<T>* B, U size,
    U init_row_A /* = 0 */, U init_col_A /* = 0 */,
    U init_row_B /* = 0 */, U init_col_B /* = 0 */) const
{
    return helper_for_add_sub_blocks(false, B, size,
        init_row_A, init_col_A, init_row_B, init_col_B);
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
            throw std::invalid_argument( "multiply_blocks(): sub-matrix doesn't fit" );

        const auto A = this;
        A->display_block("X", size, init_row_A, init_col_A);
        B->display_block("Y", size, init_row_B, init_col_B);

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

        C->display_block("X*Y", size);
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
    return TB_multiply(B);
}

// ----------------------------------------------------

template<typename T>
Matrix<T>* Matrix<T>::TB_multiply(const Matrix<T>* B) const
{
    try
    {
        U AR = nRows;
        U AC = nCols;
        U BR = B->get_nRows();
        U BC = B->get_nCols();

        if (AC != BR)
            throw std::invalid_argument( "TB_multiply(): dimension mismatch" );

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

// Source for this simple block-based divide-and-conquer multiply:
// https://en.wikipedia.org/wiki/Strassen_algorithm
template<typename T>
Matrix<T>* Matrix<T>::BB_multiply(const Matrix<T>* B) const
{
    U size = get_nRows();
    assert(size == get_nCols());
    assert(size == B->get_nRows());
    assert(size == B->get_nCols());

    assert(is_power_of_2(size));

    U s2 = size / 2;

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

// Source for this Strassen-based multiply:
// https://en.wikipedia.org/wiki/Strassen_algorithm
template<typename T>
Matrix<T>* Matrix<T>::SB_multiply(const Matrix<T>* B) const
{
    U size = get_nRows();
    assert(size == get_nCols());
    assert(size == B->get_nRows());
    assert(size == B->get_nCols());

    assert(is_power_of_2(size));

    U s2 = size / 2;

    const auto A = this;

    auto M1A = A->add_blocks(A, s2, 0, 0, s2, s2);        // A11 + A22
    auto M1B = B->add_blocks(B, s2, 0, 0, s2, s2);        // B11 + B22
    auto M1 = M1A->multiply_blocks(M1B, s2);

    auto M2A = A->add_blocks(A, s2, s2, 0, s2, s2);       // A21 + A22
    auto M2 = M2A->multiply_blocks(B, s2);                // M2A * B11

    auto M3B = B->subtract_blocks(B, s2, 0, s2, s2, s2);  // B12 - B22
    auto M3 = A->multiply_blocks(M3B, s2);                // A11 * M3B

    auto M4B = B->subtract_blocks(B, s2, s2, 0, 0, 0);    // B21 - B11
    auto M4 = A->multiply_blocks(M4B, s2, s2, s2);        // A22 * M4B

    auto M5A = A->add_blocks(A, s2, 0, 0, 0, s2);         // A11 + A12
    auto M5 = M5A->multiply_blocks(B, s2, 0, 0, s2, s2);  // M5A * B22

    auto M6A = A->subtract_blocks(A, s2, s2, 0, 0, 0);    // A21 - A11
    auto M6B = B->add_blocks(B, s2, 0, 0, 0, s2);         // B11 + B12
    auto M6 = M6A->multiply_blocks(M6B, s2);

    auto M7A = A->subtract_blocks(A, s2, 0, s2, s2, s2);  // A12 - A22
    auto M7B = B->add_blocks(B, s2, s2, 0, s2, s2);       // B21 + B22
    auto M7 = M7A->multiply_blocks(M7B, s2);

    auto C11 = M1->add(M4)->subtract(M5)->add(M7);
    auto C12 = M3->add(M5);
    auto C21 = M2->add(M4);
    auto C22 = M1->subtract(M2)->add(M3)->add(M6);

    return assemble(C11, C12, C21, C22);
}

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
    C->set_block_to_copy(m11, size);
    C->set_block_to_copy(m12, size, 0, size);
    C->set_block_to_copy(m21, size, size, 0);
    C->set_block_to_copy(m22, size, size, size);
    return C;
}

