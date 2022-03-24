# code written by John Burkardt available here:
# https://people.sc.fsu.edu/~jburkardt/py_src/quadrule/quadrule.py
# compute gauss-lobatto nodes and weights

# only added __main__ to make available for writing to file

import argparse 

def r8vec_reverse ( n, a1 ):

#*****************************************************************************80
#
## r8vec_reverse() reverses the elements of an R8VEC.
#
#  Example:
#
#    Input:
#
#      N = 5, A = ( 11.0, 12.0, 13.0, 14.0, 15.0 ).
#
#    Output:
#
#      A = ( 15.0, 14.0, 13.0, 12.0, 11.0 ).
#
#  Licensing:
#
#    This code is distributed under the GNU LGPL license.
#
#  Modified:
#
#    09 June 2015
#
#  Author:
#
#    John Burkardt
#
#  Input:
#
#    integer N, the number of entries in the array.
#
#    real A1(N), the array to be reversed.
#
#  Output:
#
#    real A2(N), the reversed array.
#
  import numpy as np

  a2 = np.zeros ( n )
  for i in range ( 0, n ):
    a2[i] = a1[n-1-i]

  return a2


def r8vec_diff_norm_li ( n, a, b ):

#*****************************************************************************80
#
## r8vec_diff_norm_li() returns the L-infinity norm of the difference of R8VEC's.
#
#  Discussion:
#
#    The vector L-infinity norm is defined as:
#
#      value = max ( 1 <= I <= N ) abs ( A(I) ).
#
#  Licensing:
#
#    This code is distributed under the GNU LGPL license.
#
#  Modified:
#
#    09 June 2015
#
#  Author:
#
#    John Burkardt
#
#  Input:
#
#    integer N, the number of entries in A.
#
#    real A(N), B(N), the vectors.
#
#  Output:
#
#    real VALUE, the L-infinity norm of A - B.
#
  value = 0.0
  for i in range ( 0, n ):
    value = max ( value, abs ( a[i] - b[i] ) )

  return value

def lobatto_compute ( n ):

#*****************************************************************************80
#
## lobatto_compute() computes a Lobatto quadrature rule.
#
#  Discussion:
#
#    The integral:
#
#      Integral ( -1 <= X <= 1 ) F(X) dX
#
#    The quadrature rule:
#
#      Sum ( 1 <= I <= N ) W(I) * F ( X(I) )
#
#    The quadrature rule will integrate exactly all polynomials up to
#    X^(2*N-3).
#
#    The Lobatto rule is distinguished by the fact that both endpoints
#    (-1 and 1) are always abscissas of the rule.
#
#  Licensing:
#
#    This code is distributed under the GNU LGPL license.
#
#  Modified:
#
#    01 April 2015
#
#  Author:
#
#    John Burkardt.
#
#  Reference:
#
#    Milton Abramowitz, Irene Stegun,
#    Handbook of Mathematical Functions,
#    National Bureau of Standards, 1964,
#    ISBN: 0-486-61272-4,
#    LC: QA47.A34.
#
#    Claudio Canuto, Yousuff Hussaini, Alfio Quarteroni, Thomas Zang,
#    Spectral Methods in Fluid Dynamics,
#    Springer, 1993,
#    ISNB13: 978-3540522058,
#    LC: QA377.S676.
#
#    Arthur Stroud, Don Secrest,
#    Gaussian Quadrature Formulas,
#    Prentice Hall, 1966,
#    LC: QA299.4G3S7.
#
#    Daniel Zwillinger, editor,
#    CRC Standard Mathematical Tables and Formulae,
#    30th Edition,
#    CRC Press, 1996,
#    ISBN: 0-8493-2479-3.
#
#  Input:
#
#    integer N, the order.  
#
#  Output:
#
#    real X(N), the abscissas.
#
#    real W(N), the weights,
#
  import numpy as np

  x = np.zeros ( n )
  w = np.zeros ( n )

  if ( n == 1 ):
    x[0] = -1.0
    w[0] = 2.0
    return x, w

  tol = 100.0 * np.finfo(float).eps
#
#  Initial estimate for the abscissas is the Chebyshev-Gauss-Lobatto nodes.
#
  for i in range ( 0, n ):
    x[i] = np.cos ( np.pi * float ( i ) / float ( n - 1 ) )

  xold = np.zeros ( n )
  p = np.zeros ( [ n, n ] )

  while ( True  ):

    for i in range ( 0, n ):
      xold[i] = x[i]

    for i in range ( 0, n ):
      p[i,0] = 1.0
      p[i,1] = x[i]

    for j in range ( 2, n ):
      for i in range ( 0, n ):
        p[i,j] = ( float ( 2 * j - 1 ) * x[i] * p[i,j-1]     \
                 + float (   - j + 1 ) *        p[i,j-2] ) \
                 / float (     j     )

    for i in range ( 0, n ):
      x[i] = xold[i] - ( x[i] * p[i,n-1] - p[i,n-2] ) \
        / ( float ( n ) * p[i,n-1] )

    dif = r8vec_diff_norm_li ( n, x, xold )
 
    if ( dif <= tol ):
      break

  x = r8vec_reverse ( n, x )

  for i in range ( 0, n ):
    w[i] = 2.0 / ( float ( n * ( n - 1 ) ) * p[i,n-1] ** 2 )

  return x, w

def to_string(x):
  # write array of data to string for printing
  return ','.join([str(xi) for xi in x])

if __name__ == '__main__':

  parser = argparse.ArgumentParser()
  parser.add_argument('--n', type=int, required=True)
  args = parser.parse_args()

  x, w = lobatto_compute(args.n)
  print(to_string(x))
  print(to_string(w))

  # to get the lines and write to file i use:
  # % for N in 1 2; do
  #     python quadrule.py --n 5 | sed "${N}q;d" 
  #   done
    
