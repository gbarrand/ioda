
import gopaw

def urout(NEV):
  id_100 = 100
  id_110 = 110
  Y = 0.0
  W = 1.0
  #print 'debug : urout : ',NEV
  for i in range(0,int(NEV)+1) :
    X = gopaw.hrndm1(id_100)
    gopaw.hfill(id_110,X,Y,W)

