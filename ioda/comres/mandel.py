# Copyright (C) 2018, Guy Barrand. All rights reserved.
# See the file gopaw.license for terms.

def mandel(XP,YP):
  NMAX = 30
  X = XP
  Y = YP
  XX = 0
  YY = 0
  for N in range(1,NMAX+1) :  #[1,NMAX]
    TT = XX*XX-YY*YY+X
    YY = 2.0*XX*YY+Y
    XX = TT
    if 4.0 < (XX*XX+YY*YY) :
      break

  #print XP,YP
  #print N

  return (1.0*N)/(1.0*NMAX)

