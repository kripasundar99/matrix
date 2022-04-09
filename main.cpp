#include "matrix.h"

// settings for matrix sizes
U AR = 3;
U AC = 5;
U BR = AC; // do not modify
U BC = 4;

// settings for matrix contents
int LB = -8;      // lower bound for data entries
int UB = -UB;     // do not modify
U nDiscards = 500; // number of discards, for initialisation

void Usage(int argc, char* argv[])
{
    assert(argc >= 1);

    if (argc == 1)
        return;

    if (!strcmp(argv[1], "-h") || (argc != 4))
    {
        printf("Usage: %s [<AR> <AC|BR> <BC>]\n", argv[0]);
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    Usage(argc, argv);

    Matrix_int* A = new Matrix_int(AR, AC);
    A->initialise(LB, UB, nDiscards);
    A->display();

    Matrix_int* B = new Matrix_int(BR, BC);
    B->initialise(LB, UB, nDiscards);
    B->display();

    Matrix_int* C = A->multiply(B);
    if (C == nullptr)
        printf("AAAAAH -- multiply() failed!!!\n");
    else
        C->display();

    return 0;
}
