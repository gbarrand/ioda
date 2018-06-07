

--//////////////////////////////////////////
--/// same as inlib/exa/hrand //////////////
--//////////////////////////////////////////

--//////////////////////////////////////////
--//////////////////////////////////////////
--//////////////////////////////////////////
h1 = inlib.histo_h1d('The famous normal distribution',100,-5,5)

r = inlib.rgaussd(0,1)
for I = 1,10000,1 do
  h1:fill(r:shoot(),1)
end

--//print h1.entries(),h.mean(),h.rms()

h2 = inlib.histo_h2d('Gauss_BW',100,-5,5,100,-2,2)
rg = inlib.rgaussd(1,2)
rbw = inlib.rbwd(0,1)
for I = 1,10000,1 do
  h2:fill(rg:shoot(),rbw:shoot(),1)
end

--//////////////////////////////////////////////////////////
--/// plotting : ///////////////////////////////////////////
--//////////////////////////////////////////////////////////
viewer = inlib.cast_viewer(get_viewer_string_pointer())
gv = inlib.cast_gui_viewer(viewer)

gv:show_console('h1 '..h1:entries()..' '..h1:mean()..' '..h1:rms())

plots = inlib.get_sg_plots(gv)
if plots ~= nil then
  if plots:number() < 2 then
    plots:set_regions(1,2)
  end
  plots:set_current_plotter(0)
  --#plots.view_border.value(False)

  plotter = plots:current_plotter()
  plotter:clear()
  plotter:plot_cp(h1)
  inlib.style_from_res(gv,inlib.gui_viewer.s_default(),plotter)
  inlib.style_from_res(gv,'exa_hrand',plotter)
  plotter:bins_style(0).color:value(inlib.colorf_grey())
  plotter:bins_style(0).modeling:value(inlib.modeling_boxes())
  --#plotter.infos_style().font.value(inlib.font_arialbd_ttf())
  --#plotter.infos_x_margin.value(0.01) #percent of plotter width.
  --#plotter.infos_y_margin.value(0.01) #percent of plotter height.
  
  plots:next()
  plotter = plots:current_plotter()
  plotter:clear()
  plotter:plot_cp(h2)
  inlib.style_from_res(gv,inlib.gui_viewer.s_default(),plotter)
  inlib.style_from_res(gv,'exa_hrand',plotter)

  gv:hide_main_menu()
end

--del h1 # ok if having done plot_cp(h1)
--del h2 # ok if having done plot_cp(h2)
