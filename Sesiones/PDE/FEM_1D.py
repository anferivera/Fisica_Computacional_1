# FEM.py 
import numpy as np
import numpy.linalg as la
from scipy import interpolate
import scipy.integrate as integrate

def FEM(N,f,xa,xb,Ua,Ub):
  'Solve a EDO: N nodos, f: function, xa(xb): initial conditions, Ua(Ub): initial(final) boundary condition. Return nodos, solution'

  h = (xb-xa)/(N-1)
  u = np.zeros(N , float);
  b = np.zeros(N, float );

  #steps: elements, malla
  xi = np.zeros(N ,float );
  for i in range(0,N):
      xi[i] = xa + i*h

  # A matrix (integrated analytical)
  A = np.zeros((N, N), float );
  for i in range( 1 , N):
      A[i-1 , i-1 ] = A[ i-1 , i-1 ] + 1/ h
      A[i-1 , i ] = A[ i-1 , i ] - 1/h
      A[i , i-1 ] = A[ i-1 , i ]
      A[i , i ]= A[ i , i ] + 1/ h

  #Initial conditions
  A[0,0] = 1; A[0,1] = 0; A[1,0] = 0;
  A[N-1, N-1] = 1; A[N-1, N-2] = 0; A[N-2, N-1] = 0;

  #phi(x) functions
  def lin1(x, x1, x2):
      return (x-x1)/(x2-x1)

  def lin2(x, x1, x2):
      return (x2-x)/(x2-x1)

  #integrals for bi elements using quad
  def int1(min, max):
      return integrate.quad(lambda x: f(x)*lin1(x, min, max), min, max)[0]

  def int2(min, max):
      return integrate.quad(lambda x: f(x)*lin2(x, min, max), min, max)[0]

  #b matrix (Numerical integration)
  b = np.zeros(N, float);

  for i in range(1 , N):
      b[i-1] = b[i-1] + int2( xi[i-1], xi[i] )
      b[i] = b[i] + int1( xi[i-1], xi[i] )

  #Initial conditions
  b[0] = Ua
  b[N-1] = Ub

  for i in range(0 , N) :
      b[i] = b[i] - b[0]*A[i,0]
      b[i] = b[i] - b[N-1]*A[i-1,N-2] # Warning

  #Solve the system using Gaussian elimination in numpy
  sol = np.linalg.solve(A, b )

  return xi,sol
