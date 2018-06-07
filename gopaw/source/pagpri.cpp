// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pagpri_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include <exlib/KUIP/tools>

#include <inlib/touplow>
#include <inlib/mnmx>
#include <inlib/arrout>

#ifdef APP_USE_CFITSIO
#include <exlib/fits_image>
#include <inlib/picmap>
#include "find_object"
#endif

void pagpri_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  //std::string cmd_name = ku_name();
  std::string cmd_path = ku_path() ? ku_path() : "";
  if(_sess.verbose_level()) {
    out << "pagpri :" 
        << " execute " << inlib::sout(cmd_path) 
        << std::endl;
  }

  //opts_t& opts = _sess.opts();
  gopaw::atts_t& atts = _sess.atts();

  //double LWID = atts.value("LWID"); 
  //LWID = MAXIMUM(int(LWID/2.2),1);  //From graflib/higz/ikernel/islwsc.F

  if(cmd_path=="/GRAPHICS/PRIMITIVES/GRAPH") {

    int N = ku_geti();
    std::string X = ku_gets();
    std::string Y = ku_gets();
    std::string CHOPT = ku_gets();

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);
    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path
          << " X= " << inlib::sout(X) 
          << " Y= " << inlib::sout(Y) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    std::string XNAME;
    std::vector< std::pair<int,int> > xdims;
    if(!gopaw::session::parse_name(X,XNAME,xdims,out)) return;
    inlib::touppercase(XNAME);
    gopaw::vector_t* xvec = 0;
    if(!_sess.find_vector(XNAME,xvec)) {
      out << "pagpri :" 
          << " can't find vector " << inlib::sout(XNAME) << "."
          << std::endl;
      return;
    }
    std::vector< std::pair<unsigned int,unsigned int> > xcut;
    if(!array_get_cut(*xvec,xdims,xcut)) {
      out << "pagpri :" 
          << " mismatch dimension in " << inlib::sout(XNAME) 
          << std::endl;
      return;
    }

    std::string YNAME;
    std::vector< std::pair<int,int> > ydims;
    if(!gopaw::session::parse_name(Y,YNAME,ydims,out)) return;
    inlib::touppercase(YNAME);
    gopaw::vector_t* yvec = 0;
    if(!_sess.find_vector(YNAME,yvec)) {
      out << "pagpri :" 
          << " can't find vector " << inlib::sout(YNAME) << "."
          << std::endl;
      return;
    }
    std::vector< std::pair<unsigned int,unsigned int> > ycut;
    if(!array_get_cut(*yvec,ydims,ycut)) {
      out << "pagpri :" 
          << " mismatch dimension in " << inlib::sout(YNAME) 
          << std::endl;
      return;
    }

    //std::string name = VNAME + "_rep";
    int xsize = xvec->size();
    int ysize = yvec->size();
    int number = inlib::mn(xsize,N);
    number = inlib::mn(number,ysize);

    const std::vector<double>& xv = xvec->vector();
    const std::vector<double>& yv = yvec->vector();

    inlib::histo::c2d cloud;

    for(int index=0;index<number;index++) cloud.fill(xv[index],yv[index]);

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    
    _sess.set_plotter_layout(*_page);
    //If A (axes) are required we go in the next region.
    _sess.plotter_next(*_page,!gopaw::session::is_an_option(OPTS,CHOPT,"A"));
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _sess.region_set_layout(_plotter);
    if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) _sess.plot_cloud_2D(_plotter,cloud,"C");
    if(gopaw::session::is_an_option(OPTS,CHOPT,"L")) _sess.plot_cloud_2D(_plotter,cloud,"L");
    if(gopaw::session::is_an_option(OPTS,CHOPT,"P")) _sess.plot_cloud_2D(_plotter,cloud,"P");

  } else if(cmd_path=="/GRAPHICS/PRIMITIVES/TEXT") {

    double X = ku_getr();
    double Y = ku_getr();
    std::string TEXT = ku_gets();
    double SIZE = ku_getr();
    double ANGLE = ku_getr();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path
          << " X= " << X
          << " Y= " << Y
          << " TEXT= " << inlib::sout(TEXT) 
          << " SIZE= " << SIZE
          << " ANGLE= " << ANGLE
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }
    
    int TXCI = (int)atts.value("TXCI");
    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path << " : atts :"
          << " TXCI= " << TXCI
          << std::endl;
    }
    
    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);

    //SIZE is in page coordinate YSIZ.
    char JUST = 'L';
    if(gopaw::session::is_an_option(OPTS,CHOPT,"L")) JUST = 'L';
    if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) JUST = 'C';
    if(gopaw::session::is_an_option(OPTS,CHOPT,"R")) JUST = 'R';

    _plotter.primitives_enforced = true;
    //_plotter.add_primitive(new inlib::sg::plottable_text(TEXT.c_str(),X,Y,SIZE,ANGLE,JUST,"hershey"));

    std::string font;
    double scale;
    bool smoothing,hinting;
    _sess.get_font("TFON",font,scale,smoothing,hinting);

    float TWID = _sess.get_TWID(); //if Hershey. Used in examples/gopaw/lhcb_style.kumac.
    
    _plotter.primitives_enforced = true;
    _plotter.add_primitive(
       new inlib::sg::plottable_text(TEXT.c_str(),float(X),float(Y),float(SIZE),float(ANGLE),JUST,
				     font.c_str(),gopaw::get_COL(TXCI),
				     float(scale),smoothing?true:false,hinting?true:false,TWID));
    
  } else if(cmd_path=="/GRAPHICS/PRIMITIVES/ITX") {

    double X = ku_getr();
    double Y = ku_getr();
    std::string TEXT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path
          << " X= " << X
          << " Y= " << Y
          << " TEXT= " << inlib::sout(TEXT) 
          << std::endl;
    }
    
    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);

    int TXCI = (int)atts.value("TXCI");
    double SIZE = atts.value("CHHE"); 
    //SIZE is in page coordinate YSIZ.    
    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path << " : atts :"
          << " TXCI= " << TXCI
          << " CHHE= " << SIZE
          << std::endl;
    }
   
    std::string font;
    double scale;
    bool smoothing,hinting;
    _sess.get_font("CFON",font,scale,smoothing,hinting);

    float TWID = _sess.get_TWID(); //if Hershey. Used in examples/gopaw/lhcb_style.kumac.

    _plotter.primitives_enforced = true;
    _plotter.add_primitive
      (new inlib::sg::plottable_text
       (TEXT.c_str(),float(X),float(Y),float(SIZE),0,'L',font.c_str(),gopaw::get_COL(TXCI),
	float(scale),smoothing?true:false,hinting?true:false,TWID));

  } else if(cmd_path=="/GRAPHICS/PRIMITIVES/LABELS") {

    int LABNUM = ku_geti();
    int NLABS = ku_geti();
    std::string CHLABS = ku_gets();

    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path
          << " LABNUM= " << LABNUM
          << " NLABS= " << NLABS
          << " CHLABS= " << inlib::sout(CHLABS) 
          << std::endl;
    }
    
    std::vector<std::string> words;
    inlib::words(CHLABS,",",false,words);

    int n = words.size();
    if(NLABS>n) {
      for(int index=n;index<NLABS;index++) words.push_back("");
    } else {
      words.resize(NLABS);
    }

    _sess.set_labels(LABNUM,words);


  } else if(cmd_path=="/GRAPHICS/PRIMITIVES/BOX") {

    double X1 = ku_getr();
    double X2 = ku_getr();
    double Y1 = ku_getr();
    double Y2 = ku_getr();

    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path
          << " X1= " << X1
          << " X2= " << X2
          << " Y1= " << Y1
          << " Y2= " << Y2
          << std::endl;
    }
    
    int FAIS = (int)atts.value("FAIS");
    int FACI = (int)atts.value("FACI");
    int FASI = (int)atts.value("FASI");
    int BORD = (int)atts.value("BORD");
    int PLCI = (int)atts.value("PLCI");

    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path << " : atts :"
          << " FAIS= " << FAIS
          << " FACI= " << FACI
          << " FASI= " << FASI
          << " BORD= " << BORD
          << " PLCI= " << PLCI
          << std::endl;
    }
    
    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);

    _plotter.primitives_enforced = true;
    _plotter.add_primitive
      (new inlib::sg::plottable_box
       (float(X1),float(Y1),float(X2),float(Y2),
	(inlib::sg::plottable_box::fill_area_style)FAIS,gopaw::get_COL(FACI),FASI,BORD?true:false,gopaw::get_COL(PLCI)));

#ifdef APP_USE_CFITSIO
  } else if(cmd_path=="/GRAPHICS/PRIMITIVES/IMAGE_HDU") {

    //std::string FNAME = ku_gets();
    int ihdu = ku_geti();
    int islice = ku_geti();
    double LMIN = ku_getr();
    double LMAX = ku_getr();
    std::string CMAP = ku_gets();
    
    double X = ku_getr();
    double Y = ku_getr();
    double WIDTH = ku_getr();
    double HEIGHT = ku_getr();
    double THETA = ku_getr();
    double PHI = ku_getr();

    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path
	//<< " LUN= " << iLUN
        //<< " FNAME= " << inlib::sout(FNAME) 
          << " HDU= " << ihdu
          << " SLICE= " << islice
          << " LMIN= " << LMIN
          << " LMAX= " << LMAX
          << " CMAP= " << inlib::sout(CMAP)
          << " X= " << X
          << " Y= " << Y
          << " WIDTH= " << WIDTH
          << " HEIGHT= " << HEIGHT
          << " THETA= " << THETA
          << " PHI= " << PHI
          << std::endl;
    }
    
  //std::string sfile;
  //inlib::file_name(FNAME,sfile); //to expand environment variables.

    unsigned int hdu = ihdu<0?1:ihdu;
    unsigned int slice = islice<0?0:islice;

    std::string shdu;
    if(!inlib::num2s(hdu,shdu)) {}
    exlib::fits_image::img* fimg = image_find(_sess,shdu,slice);
    if(!fimg) {
      out << "pagpri : (hdu " << hdu << ", slice " << slice << ") not found." << std::endl;
      return;
    }

    inlib::SOPI::cmap cmap(0);
    if(!inlib::SOPI::stocmap(CMAP,cmap)) {
      out << "pagpri : " << cmd_path << " :"
          << " inlib::SOPI::stocmap(" << inlib::sout(CMAP) << ") failed."
          << std::endl;
      return;
    }
    if(_sess.verbose_level()) {
      out << "pagpri : " << cmd_path << " : lut :"
          << " LMIN= " << LMIN
          << " LMAX= " << LMAX
          << " NUM= " << cmap.size()
          << std::endl;
    }

    inlib::lut<double> lut(LMIN,LMAX,cmap.size());
    
    inlib::img<inlib::byte> img;
    bool yswap = false;
  
  //if(!exlib::fits_image::read_slice_to_img(out,sfile,hdu,slice,yswap,lut,cmap,img)) {
    if(!exlib::fits_image::buffer2img(out,fimg->buffer(),fimg->width(),fimg->height(),fimg->bitpix(),yswap,lut,cmap,img)) {
      out << "pagpri : " << cmd_path << " :"
          << " exlib::fits_image::read_slice() failed." << std::endl;
      return;
    }

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);

    _plotter.primitives_enforced = true;
    _plotter.add_primitive
       (new inlib::sg::plottable_img(img,float(X),float(Y),float(WIDTH),float(HEIGHT),float(THETA),float(PHI)));    
#endif

  } else {
    out << "pagpri :" 
        << " " << cmd_path << " : dummy." << std::endl;

  }
}
