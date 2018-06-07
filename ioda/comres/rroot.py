
import inlib

import exlib_sg_view_Python as vp
viewer = inlib.cast_viewer(vp.get_viewer_string_pointer())
gv = inlib.cast_gui_viewer(viewer)

EXIT_FAILURE = 1
EXIT_SUCCESS = 0
#//////////////////////////////////////////////////
#//////////////////////////////////////////////////
#//////////////////////////////////////////////////

path = gv.res_dir()+inlib.sep()+'demo.root'

#//////////////////////////////////////////////////
#// open the file and get an histo : //////////////
#//////////////////////////////////////////////////

EXIT_FAILURE = 1
EXIT_SUCCESS = 0

def rroot_main():
  rfile = inlib.rroot_file(inlib.get_cout(),path,False)
  if rfile.is_open() == False :
    print "can't open out.root"
    return EXIT_FAILURE

  keys = rfile.dir().keys()
  #print 'number of keys = ',keys.size()

  dir = inlib.rroot_find_dir(rfile.dir(),'CHARM')
  if dir == None :
    print 'directory not found'
    return EXIT_FAILURE
    
  key = dir.find_key('h7103')
  if key == None :
    print 'histo not found'
    return EXIT_FAILURE
    
  h = inlib.rroot_key_to_h1d(rfile,key)
  if h == None :
    print 'key is not a h1d.'
    return EXIT_FAILURE

  #print h.mean(),h.rms()
  
  rfile.close()

  #//////////////////////////////////////////////////
  #// plot : ////////////////////////////////////////
  #//////////////////////////////////////////////////

  plots = inlib.get_sg_plots(gv)
  if plots == None :
    print 'sg::plots not found.'
    return EXIT_FAILURE

  plots.set_current_plotter(0)

  plotter = plots.current_plotter()
  plotter.clear()
  plotter.plot_cp(h)

  gv.hide_main_menu()

rroot_main()
