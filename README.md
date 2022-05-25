# Simple implementation of matrix multiplication

This repo is a proof of concept for matrix multiplication.
It provides the following.

## Multiplication algorithms

Straightforward implementations of three matrix multiplication algorithms:

1. Textbook multiplication - the straightforward textbook definition of matrix multiplication.
2. Block-based multiplication - a simple block-based divide-and-conquer algorithm.
3. Strassen multiplication - Strassen's algorithm.

Source for the algorithms:
https://en.wikipedia.org/wiki/Strassen_algorithm#Algorithm

## Helper methods

Several helper methods, including:

* methods to initialize matrix contents to zero, identity, or random
* addition/subtraction/multiplication of blocks within matrices

## Simple self-testing

We test `int` and `double` matrices.

We self-test algorithms #2 (block-based) and #3 (Strassen) by comparing their
output with that of algorithm #1 (textbook).

# Future directions

Here are some directions in which we can expand this repo:

* add debugging capabilities
* add better unit tests
* parallelize algorithms #2 and #3
* quantify the performance of the algorithms (serial and parallel versions)
* add arbitrary precision arithmetic
* add more matrix operations (calculating the determinant, the inverse, etc.)

