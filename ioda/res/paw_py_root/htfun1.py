import math

def htfun1(X):
  C1 = 1.
  C2 = 0.5
  XM1 = 0.3
  XM2 = 0.7
  XS1 = 0.07
  XS2 = 0.12

  A1 = -0.5 * math.pow((X-XM1)/XS1,2)
  A2 = -0.5 * math.pow((X-XM2)/XS2,2)
  X1 = C1
  X2 = C2
  if math.fabs(A1)>0.0001 : X1 = C1 * math.exp(A1)
  if math.fabs(A2)>0.0001 : X2 = C2 * math.exp(A2)
  return X1+X2

def HTFUN2(X,Y):
  return 100*htfun1(X)*htfun1(Y)
