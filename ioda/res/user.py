print 'user.py : 000'

#//////////////////////////////////////////
#//////////////////////////////////////////
#//////////////////////////////////////////
import inlib
h = inlib.histo_h1d('Rand gauss',100,-5,5)

r = inlib.rgaussd(0,1)
for I in range(0,10000):
  h.fill(r.shoot(),1)

print h.entries(),h.mean(),h.rms()

#//////////////////////////////////////////
#//////////////////////////////////////////
#//////////////////////////////////////////
import exlib_sg_view_Python as vp
viewer = inlib.cast_viewer(vp.get_viewer_string_pointer())
printer = inlib.printer(viewer.out())
import sys
sys.stdout = printer
sys.stderr = printer
print 'print done with viewer.out()'

sys.stdout = sys.__stdout__
sys.stderr = sys.__stderr__
print 'print done with sys.stdout'

#//////////////////////////////////////////
#//////////////////////////////////////////
#//////////////////////////////////////////
gui_viewer = inlib.cast_gui_viewer(viewer)
s = 'entries = '+str(h.entries())+', mean = '+str(h.mean())+',rms = '+str(h.rms())
gui_viewer.show_console(s)
gui_viewer.set_console_size(0.5)

