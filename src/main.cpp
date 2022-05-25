#include "matrix.h"

// settings for matrix sizes
U AR = 3;   // number of rows in A
U AC = 5;   // number of cols in A; also number of rows in B
U BC = 4;   // number of cols in B

U Mult_AR = 8;  // needs to be a power of 2

// settings for matrix contents
int UB = 8; // upper bound for data entries

// do not modify: derived values
U BR = AC;      // do not edit: number of rows in B == number of cols in A
int LB = -UB;   // do not edit: lower bound == -(upper bound)

// ----------------------------------------------------

static void Print_usage_and_exit(const char* argv[],
    const char* error_msg = nullptr)
{
    if (error_msg != nullptr)
        fprintf(stderr, "Error: %s\n\n", error_msg);

    fprintf(stderr,
        "Usage: %s [-h | <XP> <UB>]\n"
        "Options:\n"
        "* -h = this help message\n"
        "* <XP> = exponent\n"
        "  - must be a positive integer between 1 and 20\n"
        "  - 2**<XP> will be the number of rows/columns in test matrices\n"
        "* <UB> = upper bound\n"
        "  - must be a positive integer between 1 and 1000\n"
        "  - the contents of the test matrices will range from -UB to UB\n",
        argv[0]);

    exit (error_msg == nullptr);
}

// ----------------------------------------------------

static void Process_ARGV(int argc, const char* argv[])
{
    assert(argc >= 1);

    if (argc == 1)
        return;

    if (!strcmp(argv[1], "-h"))
        Print_usage_and_exit(argv,
            (argc == 2) ? nullptr : "Too many arguments");

    if (argc != 3)
        Print_usage_and_exit(argv,
            "Incorrect arguments: Expected to find <XP> and <UB>");

    U exponent = atoi(argv[1]);
    if ((exponent <= 0) || (exponent > 20))
        Print_usage_and_exit(argv,
            "Incorrect argument: <XP> out of range");

    Mult_AR = 1 << exponent;

    UB = atoi(argv[2]);
    if ((UB <= 0) || (UB > 1000))
        Print_usage_and_exit(argv,
            "Incorrect argument: <UB> out of range");

    LB = -UB;

    printf(
        "We will use 2**%d x 2**%d matrices,"
        " with contents ranging from %d to %d.\n",
        exponent, exponent, LB, UB);
}

// ----------------------------------------------------

template<typename T>
bool test_equals(const Matrix<T>* m1, const Matrix<T>* m2, string s1, string s2,
    double tolerance = 0)
{
    if (tolerance > 0)
    {
        // First try out zero tolerance.
        // If that succeeds, we don't need to test with non-zero tolerance.
        if (test_equals(m1, m2, s1, s2))
            return true;
    }

    bool cmp_status = m1->equals(m2, tolerance);
    const char* cmp_status_msg = (cmp_status ? "equals" : "does not equal");

    char tolerance_msg[512] = {};
    if (tolerance > 0)
    {
        sprintf(tolerance_msg, "With tolerance %f, ", tolerance);
    }

    printf("%s%s %s %s.\n----\n", tolerance_msg,
        s1.c_str(), cmp_status_msg, s2.c_str());

    return cmp_status;
}

// ----------------------------------------------------

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

    Matrix<T>* m3 = m1->TB_multiply(m2);
    if (m3 != nullptr)
        m3->display("m3");

    Matrix<T>* m4 = m1a->TB_multiply(m2);
    if (m4 != nullptr)
        m4->display("m4");

    test_equals(m3, m4, "m3", "m4");

    test_equals(m3, m4->get_negative(), "m3", "negative m4");

    Matrix<T>* m5 = new Matrix<T>(AR+2);
    m5->set_to_random(LB, UB);
    m5->display("m5 AR+2");

    m5->TB_multiply(m5)->display("m5^2");

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

// ----------------------------------------------------

template<typename T>
void test_basic_ops_blocks()
{
    U AR1 = AR + 3;

    Matrix<T>* s1 = new Matrix<T>(AR1, AR1);
    s1->set_to_random(LB, UB);
    s1->display("s1", true);

    Matrix<T>* s2 = new Matrix<T>(AR1, AR1);
    s2->set_to_random(LB, UB);
    s2->display("s2", true);

    Matrix<T>* p = nullptr;

    p = s1->add_blocks(s2, AR1);
    //p->display("add_blocks(AR1)", true);
    test_equals(p, s1->add(s2),
        "add_blocks(AR1)", "add()");

    s1->add_blocks(s2, AR, 1, 2, 2, 1)
        ->display("add_blocks(AR,1,2,2,1)", true);

    p = s1->subtract_blocks(s2, AR1);
    //p->display("subtract_blocks(AR1)", true);
    test_equals(p, s1->subtract(s2),
        "subtract_blocks(AR1)", "subtract()");

    s1->subtract_blocks(s2, AR, 1, 2, 2, 1)
        ->display("subtract_blocks(AR,1,2,2,1)", true);

    p = s1->multiply_blocks(s2, AR1);
    //p->display("multiply_blocks(AR1)", true);
    test_equals(p, s1->TB_multiply(s2),
        "multiply_blocks(AR1)", "TB_multiply()");

    s1->multiply_blocks(s2, AR, 1, 2, 2, 1)
        ->display("multiply_blocks(AR,1,2,2,1)", true);
}

// ----------------------------------------------------

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

// ----------------------------------------------------

template<typename T>
void test_BB_multiply()
{
    auto m1 = new Matrix<T>(Mult_AR, Mult_AR);
    m1->set_to_random(LB, UB);
    m1->display("m1", true);

    auto m2 = new Matrix<T>(Mult_AR, Mult_AR);
    m2->set_to_random(LB, UB);
    m2->display("m2", true);

    auto p1 = m1->TB_multiply(m2)->display("p1 (textbook multiply)", true);
    auto p2 = m1->BB_multiply(m2)->display("p2 (block-based multiply)", true);

    test_equals(p1, p2,
        "p1 (textbook multiply)", "p2 (block-based multiply)", 0.000001);
}

// ----------------------------------------------------

template<typename T>
void test_SB_multiply()
{
    auto m1 = new Matrix<T>(Mult_AR, Mult_AR);
    m1->set_to_random(LB, UB);
    m1->display("m1", true);

    auto m2 = new Matrix<T>(Mult_AR, Mult_AR);
    m2->set_to_random(LB, UB);
    m2->display("m2", true);

    auto p1 = m1->TB_multiply(m2)->display("p1 (textbook multiply)", true);
    auto p2 = m1->SB_multiply(m2)->display("p2 (Strassen multiply)", true);

    test_equals(p1, p2,
        "p1 (textbook multiply)", "p2 (Strassen multiply)", 0.000001);
}

// ----------------------------------------------------

int main(int argc, const char* argv[])
{
    Process_ARGV(argc, argv);

    // Tests for operations on `int` matrices.
    //test_basic_ops<int>();
    //test_basic_ops_blocks<int>();
    //test_assemble<int>();
    //test_BB_multiply<int>();
    //test_SB_multiply<int>();

    // Tests for operations on `double` matrices.
    //test_basic_ops<double>();
    //test_basic_ops_blocks<double>();
    //test_assemble<double>();
    //test_BB_multiply<double>();
    test_SB_multiply<double>();

    return 0;
}
