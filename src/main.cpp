#include "matrix.h"

// settings for matrix sizes
U AR = 3;
U AC = 5;
U BR = AC; // do not modify
U BC = 4;

// settings for matrix contents
int LB = -8;        // lower bound for data entries
int UB = -LB;       // do not modify
U nDiscards = 100;  // number of discards, for randomisation

void Process_ARGV(int argc, char* argv[])
{
    assert(argc >= 1);

    if (argc == 1)
        return;

    if (!strcmp(argv[1], "-h") || (argc != 4))
    {
        printf("Usage: %s [<AR> <AC|BR> <BC>]\n", argv[0]);
        exit(0);
    }

    assert (argc == 4);

    AR = atoi(argv[1]);
    AC = atoi(argv[2]);
    BR = AC;
    BC = atoi(argv[3]);

    DPRINTF(1)("A needs to be %dx%d; B needs to be %dx%d\n", AR, AC, BR, BC);
}


template<typename T>
void test_equals(Matrix<T>* m1, Matrix<T>* m2, string s1, string s2)
{
    printf("%s %s %s\n----\n",
        s1.c_str(),
        (m1->equals(m2) ? "equals" : "does not equal"),
        s2.c_str());
}

template<typename T>
void test()
{
    Matrix<T>* A = new Matrix<T>(AR, AC);
    A->randomize(LB, UB, nDiscards);
    A->display("A");

    Matrix<T>* A1 = new Matrix<T>(1,1);
    A1->display("A1 #1");
    A1->copyFrom(A);
    A1->display("A1 #2");

    test_equals(A1, A, "A1 #2", "A");

    T orig_23 = A1->get_IJ(2, 3);
    A1->set_IJ(2, 3, A1->get_IJ(3, 2));
    test_equals(A1, A, "A1 #3", "A");
    A1->set_IJ(2, 3, orig_23);
    test_equals(A1, A, "A1 #4", "A");

    Matrix<T>* B = new Matrix<T>(BR, BC);
    // Do not use the same discard count for A and B.
    // If you do, A and B will start off at the same location in the
    // pseudo-random sequence.
    // e.g.: 7, 3, 5, 2, ...
    // A = [ 7 3 5 | 2 4 9 | 8 6 1 ]
    // B = [ 7 3 | 5 2 | 4 9 ]
    B->randomize(LB, UB, nDiscards << 1);
    B->display("B");

    test_equals(A, B, "A", "B");

    Matrix<T>* C = A->multiply(B);
    if (C != nullptr)
        C->display("C");

    Matrix<T>* D = A1->multiply(B);
    if (D != nullptr)
        D->display("D");

    test_equals(C, D, "C", "D");

    Matrix<T>* E = new Matrix<T>(AR+2);
    E->randomize(LB, UB, nDiscards << 2);
    E->display("E AR+2");

    E->multiply(E)->display("E^2");

    Matrix<T>* F = new Matrix<T>(AR, AC);
    F->setToIdentity();
    F->display("F AR/AC negative test for identity");

    F->setToIdentity(AR + 3);
    F->display("F AR+3 identity");
}

int main(int argc, char* argv[])
{
    Process_ARGV(argc, argv);

    //test<int>(); // no need to test `int` for a while

    test<double>();

    return 0;
}
