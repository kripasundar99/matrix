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

    DPRINTF(1)("m1 needs to be %dx%d; m2 needs to be %dx%d\n", AR, AC, BR, BC);
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
    Matrix<T>* m1 = new Matrix<T>(AR, AC);
    m1->set_to_random(LB, UB, nDiscards);
    m1->display("m1");

    Matrix<T>* m1a = new Matrix<T>(1,1);
    m1a->display("m1a #1");
    m1a->set_to_copy(m1);
    m1a->display("m1a #2");

    test_equals(m1a, m1, "m1a #2", "m1");

    T orig_23 = m1a->get_IJ(2, 3);
    m1a->set_IJ(2, 3, m1a->get_IJ(3, 2));
    test_equals(m1a, m1, "m1a #3", "m1");
    m1a->set_IJ(2, 3, orig_23);
    test_equals(m1a, m1, "m1a #4", "m1");

    Matrix<T>* m2 = new Matrix<T>(BR, BC);
    // Do not use the same discard count for m1 and m2.
    // If you do, m1 and m2 will start off at the same location in the
    // pseudo-random sequence.
    // e.g.: 7, 3, 5, 2, ...
    // m1 = [ 7 3 5 | 2 4 9 | 8 6 1 ]
    // m2 = [ 7 3 | 5 2 | 4 9 ]
    m2->set_to_random(LB, UB, nDiscards << 1);
    m2->display("m2");

    test_equals(m1, m2, "m1", "m2");

    Matrix<T>* m3 = m1->multiply(m2);
    if (m3 != nullptr)
        m3->display("m3");

    Matrix<T>* m4 = m1a->multiply(m2);
    if (m4 != nullptr)
        m4->display("m4");

    test_equals(m3, m4, "m3", "m4");

    Matrix<T>* m5 = new Matrix<T>(AR+2);
    m5->set_to_random(LB, UB, nDiscards << 2);
    m5->display("m5 AR+2");

    m5->multiply(m5)->display("m5^2");

    Matrix<T>* m6 = new Matrix<T>(AR, AC);
    m6->set_to_identity();
    m6->display("m6 AR/AC negative test for identity");

    m6->set_to_identity(AR + 3);
    m6->display("m6 AR+3 identity");
}

int main(int argc, char* argv[])
{
    Process_ARGV(argc, argv);

    //test<int>(); // no need to test `int` for a while

    test<double>();

    return 0;
}
