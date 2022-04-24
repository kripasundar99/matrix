# Matrix utilities

A few basic utilities for processing matrices.

------------

Implemented:
* modify the whole matrix in-place
  - <code>set_to_zero</code>, <code>set_to_identity</code>
  - <code>set_to_copy</code>, <code>set_to_random</code>

* unary arithmetic operations:
  - <code>get_negative</code>

* binary arithmetic operations:
  - <code>add</code>, <code>subtract</code>, <code>multiply</code>

* binary boolean operations:
  - <code>equals</code>, <code>dimensions_match</code>

------------

Not yet implemented.
* unary operations:
  - <code>get_determinant</code>, <code>get_inverse</code>
* binary operations:
  - <code>mutiply_by_Strassen</code>
