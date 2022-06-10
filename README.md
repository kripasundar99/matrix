# Simple implementation of matrix multiplication

This repo demonstrates three matrix multiplication algorithms: textbook,
block-based, and Strassen.

The repo provides the three algorithms, some helper methods, and some simple
self-testing.

## Multiplication algorithms

Three matrix multiplication algorithms:

1. `TB_multiply()` - the straightforward textbook definition of matrix multiplication.
2. `BB_multiply()` - a simple block-based divide-and-conquer algorithm.
3. `SB_multiply()` - Strassen's algorithm.

Source for the algorithms:
https://en.wikipedia.org/wiki/Strassen_algorithm#Algorithm

Note: Currently, algorithms #2 and #3 only apply to the top level of the input
matrices.  At lower levels, they revert to textbook multiplication.

## Helper methods

Several helper methods, including:

* methods to initialize matrix contents to zero, identity, or random
* addition/subtraction/multiplication of blocks within matrices

## Simple self-testing

We test `int` and `double` matrices.

We self-test algorithms #2 (block-based) and #3 (Strassen) by comparing their
output with that of algorithm #1 (textbook).

# Usage

```
Usage: bin/matrix [-h | <XP> <UB>]
Options:
* -h = this help message
* <XP> = exponent
  - must be a positive integer between 1 and 20
  - 2**<XP> will be the number of rows/columns in test matrices
* <UB> = upper bound
  - must be a positive integer between 1 and 1000
  - the contents of the test matrices will range from -UB to UB
```

# Example output

```
$ bin/matrix 3 1
We will use 2**3 x 2**3 matrices, with contents ranging from -1 to 1.
----
M1: 8 x 8
    0.97     0.51    -0.85     0.77    -0.13    -0.04    -0.45    -0.67
    0.80    -0.88     0.01    -0.36    -0.01    -0.82    -0.85    -0.23
    0.83    -0.07    -0.90     0.54    -0.75     0.38     0.26     0.45
    0.78    -0.39     0.03     0.69     0.68    -0.17    -0.06    -0.64
    0.14    -0.93    -0.00     0.50     0.78     0.68    -0.57    -0.74
   -0.45    -0.17     0.42    -0.52    -0.36     0.30     0.36    -0.22
   -0.70     0.69     0.91    -0.70    -0.18     0.13    -0.02     0.92
   -0.60     0.26     0.30     0.61    -0.05    -0.59     0.80    -0.72
----
M2: 8 x 8
   -0.75     0.38     0.26     0.45     0.78    -0.39     0.03     0.69
    0.68    -0.17    -0.06    -0.64     0.14    -0.93    -0.00     0.50
    0.78     0.68    -0.57    -0.74    -0.45    -0.17     0.42    -0.52
   -0.36     0.30     0.36    -0.22    -0.70     0.69     0.91    -0.70
   -0.18     0.13    -0.02     0.92    -0.60     0.26     0.30     0.61
   -0.05    -0.59     0.80    -0.72    -0.18     0.77    -0.68    -0.27
   -0.73    -0.09    -0.10     0.86    -0.57     0.82     0.72     0.01
    0.64    -0.08     0.27     0.65     0.40     0.91    -0.42     0.03
----
P1 (Textbook M1 * M2): 8 x 8
   -1.40     0.03     0.82    -0.34     0.74    -1.21     0.31     0.73
   -0.54     0.92    -0.51     0.69     1.29    -1.28    -0.27     0.56
   -1.35    -0.51     1.35     0.52     1.07     0.99    -0.35     0.09
   -1.56     0.83     0.15     0.70    -0.55    -0.06     1.21     0.28
   -1.15     0.16     0.67    -0.19    -0.93     0.75     0.13    -0.45
    0.38    -0.26    -0.38    -0.67    -0.33     0.14    -0.27    -0.55
    2.60    -0.14    -0.65    -0.96     0.11    -0.14    -0.83    -0.24
   -0.36     0.45    -0.87    -0.19    -1.61    -0.11     1.93    -0.75
----
P2 (Strassen M1 * M2): 8 x 8
   -1.40     0.03     0.82    -0.34     0.74    -1.21     0.31     0.73
   -0.54     0.92    -0.51     0.69     1.29    -1.28    -0.27     0.56
   -1.35    -0.51     1.35     0.52     1.07     0.99    -0.35     0.09
   -1.56     0.83     0.15     0.70    -0.55    -0.06     1.21     0.28
   -1.15     0.16     0.67    -0.19    -0.93     0.75     0.13    -0.45
    0.38    -0.26    -0.38    -0.67    -0.33     0.14    -0.27    -0.55
    2.60    -0.14    -0.65    -0.96     0.11    -0.14    -0.83    -0.24
   -0.36     0.45    -0.87    -0.19    -1.61    -0.11     1.93    -0.75
----
* p1 = P1[0][0] = -1.3954467117452666879984235492884181439877;
* p2 = P2[0][0] = -1.3954467117452673541322383243823423981667;
* p1 - p2 = 0.0000000000000006661338147750939242541790
With zero tolerance, P1 (Textbook M1 * M2) does not equal P2 (Strassen M1 * M2).
----
With tolerance 0.0000000001, P1 (Textbook M1 * M2) equals P2 (Strassen M1 * M2).
----
$
```

**Tolerance:**
Because of divergence in floating point calculations, Textbook multiplication
and Strassen multiplication sometimes disagree beyond 15 or 16 decimal places.

Therefore, for floating point matrices, we check for equality twice: first
with zero tolerance, and then with tolerance set to `0.0000000001`.

In the above example, `P1[0][0]` and `P2[0][0]` differ from the 16th decimal
place.

# Source code structure

* `matrix.h` - declares the `Matrix<T>` class template
* `matrix.cpp` - defines most of the `Matrix<T>` methods
* `main.cpp` - tests the implementation

# Future directions

We can expand the functionality, quality, and usability of this repo in several directions.

## Functionality

* make algorithms #2 and #3 recurse to lower levels
* parallelize algorithms #2 and #3
* add arbitrary precision arithmetic
* provide configuration parameters for the user to control
  - recursion depth
  - type of parallelism
  - amount of parallelism
  - etc.
* add more matrix operations (calculating the determinant, the inverse, etc.)

## Quality

* add debugging capabilities
* add better unit tests
* quantify the performance of the algorithms (serial and parallel versions)
* optimize space usage, by re-using memory blocks whenever possible

## Usability

* command-line options for
  - verbosity
  - debugging
  - output precision
  - output format
  - etc.
* improve display formatting (alignment) for large numbers
