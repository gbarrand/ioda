
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

def tree_main():
  rfile = inlib.rroot_file(inlib.get_cout(),path,False)
  if rfile.is_open() == False :
    print "can't open out.root"
    return EXIT_FAILURE

  keys = rfile.dir().keys()
  #print 'number of keys = ',keys.size()

  dir = inlib.rroot_find_dir(rfile.dir(),'STAFF')
  if dir == None :
    print 'directory not found'
    return EXIT_FAILURE
    
  key = dir.find_key('h10')
  if key == None :
    print 'tree not found'
    return EXIT_FAILURE
    
  fac = inlib.rroot_fac(gv.out())
  tree = inlib.rroot_key_to_tree(rfile,fac,key)
  if tree == None :
    print 'key is not a tree.'
    return EXIT_FAILURE

  #tree.show(gv.out(),1)
  #print tree.entries()
  
  leaf = tree.find_leaf("Age")
  if leaf == None :
    print 'leaf not found.'
    return EXIT_FAILURE

  #print 'leaf type : ',leaf.s_cls()

  li = inlib.rroot_cast_leaf_int(leaf)
  if leaf == None :
    print 'leaf not a leaf<int>.'
    return EXIT_FAILURE
  
  branch = tree.find_leaf_branch(leaf)
  if branch == None :
    print 'branch of leaf not found.'
    return EXIT_FAILURE


  h_age = inlib.histo_h1d('CERN/Age',100,0,100)

  for i in range(0,tree.entries()):
    if branch.find_entry(rfile,i) == False :
      print 'branch.find_entry failed.'
      return EXIT_FAILURE
    v = li.value(0)
    #print 'li ',i,' ',v
    h_age.fill(v,1)    

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
  plotter.plot_cp(h_age)

  gv.hide_main_menu()

tree_main()
