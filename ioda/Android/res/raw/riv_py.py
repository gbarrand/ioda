
import inlib

import exlib_sg_view_Python as vp
viewer = inlib.cast_viewer(vp.get_viewer_string_pointer())
gv = inlib.cast_gui_viewer(viewer)

file = gv.res_dir()+inlib.sep()+'atlas_evt_1.iv'

gv.opener().open(file)
gv.adapt_camera_to_scene()
gv.set_scene_light_on(True)

dir = inlib.std_string()
gv.first_data_dir(dir)
#print 'first data dir :',dir.cast()
path = inlib.std_string()
gv.find_file('atlas_evt_1.iv',path) # look in data dir and then res dir.
#print 'atlas_evt_1.iv found at :',path.cast()
