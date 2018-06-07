print('user.lua : 000')

--//////////////////////////////////////////
--//////////////////////////////////////////
--//////////////////////////////////////////
h = inlib.histo_h1d('Rand gauss',100,-5,5)
r = inlib.rgaussd(0,1)
for I = 1,10000,1 do
  h:fill(r:shoot(),1)
end
print(h:entries(),h:mean(),h:rms())

--//////////////////////////////////////////
--//////////////////////////////////////////
--//////////////////////////////////////////
viewer = inlib.cast_viewer(get_viewer_string_pointer())
gui_viewer = inlib.cast_gui_viewer(viewer)
s = 'entries = '..h:entries()..', mean = '..h:mean()..',rms = '..h:rms()
gui_viewer:show_console(s)
gui_viewer:set_console_size(0.5)

