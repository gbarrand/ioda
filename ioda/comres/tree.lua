
viewer = inlib.cast_viewer(get_viewer_string_pointer())
gv = inlib.cast_gui_viewer(viewer)

--//////////////////////////////////////////////////
--//////////////////////////////////////////////////
--//////////////////////////////////////////////////
path = gv:res_dir()..inlib.sep()..'demo.root'

--//gv:show_console(path)

--//////////////////////////////////////////////////
--// open the file and get an histo : //////////////
--//////////////////////////////////////////////////
function tree_main() 

  rfile = inlib.rroot_file(inlib.get_cout(),path,false)
  if rfile:is_open() == false then
    print('cannot open out.root')
    do return end
  end

  keys = rfile:dir():keys()
  --//print('number of keys = ',keys.size())

  dir = inlib.rroot_find_dir(rfile:dir(),'STAFF')
  if dir == nil then
    print('directory not found')
    do return end
  end
    
  key = dir:find_key('h10')
  if key == nil then
    print('tree not found')
    do return end
  end
    
  fac = inlib.rroot_fac(gv:out())
  tree = inlib.rroot_key_to_tree(rfile,fac,key)
  if tree == nil then
    print('key is not a tree.')
    do return end
  end

  --//tree:show(gv:out(),1)
  --//print(tree:entries())
  
  leaf = tree:find_leaf("Age")
  if leaf == nil then
    print('leaf not found.')
    do return end
  end

  --//print('leaf type : ',leaf:s_cls())

  li = inlib.rroot_cast_leaf_int(leaf)
  if leaf == nil then
    print('leaf not a leaf<int>.')
    do return end
  end
  
  branch = tree:find_leaf_branch(leaf)
  if branch == nil then
    print('branch of leaf not found.')
    do return end
  end

  h_age = inlib.histo_h1d('CERN/Age',100,0,100)

  for i = 0,tree:entries()-1,1 do
    if branch:find_entry(rfile,i) == false then
      print('branch:find_entry failed.')
      do return end
    end    
    v = li:value(0)
    --//print('li ',i,' ',v)
    h_age:fill(v,1)    
  end

  rfile:close()

  --//////////////////////////////////////////////////
  --// plot : ////////////////////////////////////////
  --//////////////////////////////////////////////////

  plots = inlib.get_sg_plots(gv)
  if plots == nil then
    print('sg::plots not found.')
    do return end
  end

  plots:set_current_plotter(0)

  plotter = plots:current_plotter()
  plotter:clear()
  plotter:plot_cp(h_age)

  gv:hide_main_menu()

end

tree_main()
