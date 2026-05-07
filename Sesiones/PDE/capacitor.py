# capacitor.py
def capacitor_df(Niter):
  'to compute the grid'
  V0 = 50
  Nmax = 100 # Plate's longitude
  L = 20     # Plate's capacitor
  d = 10     # Plate's whidh
  Lxd = int((d+Nmax)/2)
  import numpy as np
  V_ini = np.zeros((Nmax, Nmax), float) # grid in 0V.

  #Created the initial grid
  for k in range(0,L):
    V_ini[(Lxd-d)+k, Lxd-d] = V0 # let line at -50V
    V_ini[(Lxd-d)+k, Lxd] = -V0

  V = V_ini.copy()
  for iter in range(Niter): # iterations over algorithm. Niter times
    for i in range(1, Nmax-1):
      for j in range(1,Nmax-1):
        if (j == (Lxd-d) or j == Lxd) and ((Lxd-d)<i and i<(Lxd-d)+L): #cut in x
          continue #No toca
        #if i >= (Lxd-d) and i <= (Lxd-d)+L: #cut in x
        #  continue #No toca
        else:
          V[i,j] = 0.25*(V[i+1,j]+V[i-1,j]+V[i,j+1]+V[i,j-1])  

  return V
