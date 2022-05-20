#include "matrix.h"

// settings for matrix sizes
U AR = 3;   // number of rows in A
U AC = 5;   // number of cols in A; also number of rows in B
U BC = 4;   // number of cols in B

// settings for matrix contents
int LB = -8;        // lower bound for data entries

// do not modify: derived values
U BR = AC;      // do not modify: number of rows in B == number of cols in A
int UB = -LB;   // do not modify: upper bound == - lower bound

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
void test_equals(Matrix<T>* m1, Matrix<T>* m2, string s1, string s2,
    double tolerance = 0)
{
    printf("%s %s %s, with tolerance %f\n----\n",
        s1.c_str(),
        (m1->equals(m2, tolerance) ? "equals" : "does not equal"),
        s2.c_str(),
        tolerance);
}

template<typename T>
void test_basic_ops()
{
    Matrix<T>* m1 = new Matrix<T>(AR, AC);
    m1->set_to_random(LB, UB);
    m1->display("m1");

    Matrix<T>* m1a = new Matrix<T>(1,1);
    m1a->display("m1a #1");
    m1a->set_to_copy(m1);
    m1a->display("m1a #2");

    test_equals(m1a, m1, "m1a #2", "m1");

    T orig_23 = m1a->get_IJ(2, 3);
    m1a->set_IJ(2, 3, m1a->get_IJ(3, 2));
    test_equals(m1a, m1, "m1a #3", "m1");

    m1->add(m1a)->display("m1+m1a");

    m1->subtract(m1a)->display("m1-m1a");

    m1a->set_IJ(2, 3, orig_23);
    test_equals(m1a, m1, "m1a #4", "m1");

    m1a->set_to_negative();

    test_equals(m1a, m1->get_negative(),
        "m1a after set_to_negative", "m1->get_negative");

    m1->add(m1a)->display("m1+m1a");

    Matrix<T>* m2 = new Matrix<T>(BR, BC);
    m2->set_to_random(LB, UB);
    m2->display("m2");

    test_equals(m1, m2, "m1", "m2");

    Matrix<T>* m3 = m1->multiply(m2);
    if (m3 != nullptr)
        m3->display("m3");

    Matrix<T>* m4 = m1a->multiply(m2);
    if (m4 != nullptr)
        m4->display("m4");

    test_equals(m3, m4, "m3", "m4");

    test_equals(m3, m4->get_negative(), "m3", "negative m4");

    Matrix<T>* m5 = new Matrix<T>(AR+2);
    m5->set_to_random(LB, UB);
    m5->display("m5 AR+2");

    m5->multiply(m5)->display("m5^2");

    Matrix<T>* m6 = new Matrix<T>(AR, AC);
    m6->set_to_identity();
    m6->display("m6 AR/AC negative test for identity");

    m6->set_to_identity(AR + 3);
    m6->display("m6 AR+3 identity");

    m1->add(m2);
    DPRINTF(0)("m1+m2 negative test\n----\n");
    m1->subtract(m2);
    DPRINTF(0)("m1-m2 negative test\n----\n");

    m3->add(m4)->display("m3+m4");
} // test_basic_ops()


template<typename T>
void test_multiply_blocks()
{
    U AR1 = AR + 3;

    Matrix<T>* s1 = new Matrix<T>(AR1, AR1);
    s1->set_to_random(LB, UB);
    s1->display("s1");

    Matrix<T>* s2 = new Matrix<T>(AR1, AR1);
    s2->set_to_random(LB, UB);
    s2->display("s2");

    test_equals(s1->multiply_blocks(s2, AR1), s1->multiply(s2),
        "multiply_blocks(AR1)", "multiply()");

    s1->multiply_blocks(s2, AR, 1, 2, 2, 1)
        ->display("multiply_blocks(AR,1,2,2,1)", true);
}

template<typename T>
void test_assemble()
{
    U AR1 = AR + 1;

    auto s1 = new Matrix<T>(AR1, AR1);
    s1->set_to_random(LB, UB);
    s1->display("s1", true);

    auto s2 = new Matrix<T>(AR1, AR1);
    s2->set_to_random(LB, UB);
    s2->display("s2", true);

    auto s3 = new Matrix<T>(AR1, AR1);
    s3->set_to_random(LB, UB);
    s3->display("s3", true);

    auto s4 = new Matrix<T>(AR1, AR1);
    s4->set_to_random(LB, UB);
    s4->display("s4", true);

    auto r1 = assemble<T>(s1, s2, s3, s4);
    r1->display("r1", true);
}

template<typename T>
void test_NBB_multiply()
{
    U AR1 = 4; // need power of 2

    auto s1 = new Matrix<T>(AR1, AR1);
    s1->set_to_random(LB, UB);
    s1->display("s1", true);

    auto s2 = new Matrix<T>(AR1, AR1);
    s2->set_to_random(LB, UB);
    s2->display("s2", true);

    auto p1 = s1->multiply(s2);
    p1->display("p1", true);

    auto p2 = s1->naive_block_based_multiply(s2);
    p2->display("p2", true);

    test_equals(p1, p2, "p1", "p2");

    test_equals(p1, p2, "p1", "p2", 0.000001);
}

int main(int argc, char* argv[])
{
    Process_ARGV(argc, argv);

    // no need to test `int` for a while
    //test_basic_ops<int>();
    //test_multiply_blocks<int>();

    //test_basic_ops<double>();
    //test_multiply_blocks<double>();

    //test_assemble<double>();

    //test_NBB_multiply<int>();
    test_NBB_multiply<double>();

    return 0;
}
