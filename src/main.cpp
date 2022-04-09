#include "matrix.h"

// settings for matrix sizes
U AR = 3;
U AC = 5;
U BR = AC; // do not modify
U BC = 4;

// settings for matrix contents
int LB = -8;        // lower bound for data entries
int UB = -LB;       // do not modify
U nDiscards = 100;  // number of discards, for initialisation

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

    DPRINTF("A needs to be %dx%d; B needs to be %dx%d\n", AR, AC, BR, BC);
}

template<typename T>
void test()
{
    Matrix<T>* A = new Matrix<T>(AR, AC);
    A->initialise(LB, UB, nDiscards);
    A->display();

    Matrix<T>* B = new Matrix<T>(BR, BC);
    B->initialise(LB, UB, nDiscards);
    B->display();

    Matrix<T>* C = A->multiply(B);
    if (C == nullptr)
        printf("AAAAAH -- multiply() failed!!!\n");
    else
        C->display();
}

int main(int argc, char* argv[])
{
    Process_ARGV(argc, argv);

    test<int>();
    test<double>();

    return 0;
}
