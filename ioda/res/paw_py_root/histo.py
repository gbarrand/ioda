
#//////////////////////////////////////////
#//////////////////////////////////////////
#//////////////////////////////////////////
import inlib
h1 = inlib.histo_h1d('The famous normal distribution',100,-5,5)

r = inlib.rgaussd(0,1)
for I in range(0,10000):
  h1.fill(r.shoot(),1)

print h1.entries(),h1.mean(),h1.rms()

h2 = inlib.histo_h2d('Gauss_BW',100,-5,5,100,-2,2)
rg = inlib.rgaussd(1,2)
rbw = inlib.rbwd(0,1)
for I in range(0,10000):
  h2.fill(rg.shoot(),rbw.shoot(),1)

del h1
del h2
