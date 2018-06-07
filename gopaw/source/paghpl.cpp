// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void paghpl_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include <exlib/KUIP/tools>

#include <inlib/touplow>

void paghpl_(void* a_tag) { 

  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  //std::string cmd_name = ku_name();
  std::string cmd_path = ku_path() ? ku_path() : "";
  if(_sess.verbose_level()) {
    out << "paghpl :" 
        << " execute " << inlib::sout(cmd_path) 
        << std::endl;
  }

  if(cmd_path=="/GRAPHICS/HPLOT/ATITLE") {

    std::string XTIT = ku_gets();
    std::string YTIT = ku_gets();
    std::string ZTIT = ku_gets();
    int IALIGN = ku_geti();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "paghpl :" 
          << " /GRAPHICS/HPLOT/ATITLE "
          << " XTIT= " << inlib::sout(XTIT) 
          << " YTIT= " << inlib::sout(YTIT) 
          << " ZTIT= " << inlib::sout(ZTIT) 
          << " IALIGN= " << IALIGN
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);

   {float TWID = _sess.get_TWID(); //if Hershey. Used in examples/gopaw/lhcb_style.kumac.
    if(_plotter.x_axis().title_style().font==inlib::sg::font_hershey()) _plotter.x_axis().title_style().line_width = TWID;
    if(_plotter.y_axis().title_style().font==inlib::sg::font_hershey()) _plotter.y_axis().title_style().line_width = TWID;
    if(_plotter.z_axis().title_style().font==inlib::sg::font_hershey()) _plotter.z_axis().title_style().line_width = TWID;}
    
    _plotter.x_axis().title = XTIT;
    _plotter.y_axis().title = YTIT;
    _plotter.z_axis().title = ZTIT;

  } else if(cmd_path=="/GRAPHICS/HPLOT/TICKS") {

    std::string CHOPT = ku_gets();
    /*double XVAL = */ ku_getr();
    /*double YVAL = */ ku_getr();

    if(_sess.verbose_level()) {
      out << "paghpl :" 
          << " /GRAPHICS/HPLOT/TICKS "
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    if(gopaw::session::is_an_option(OPTS,CHOPT," ")) {
      _plotter.right_axis_visible = true;
      _plotter.top_axis_visible = true;
    }

  } else if(cmd_path=="/GRAPHICS/HPLOT/NULL") {

    double XMIN = ku_getr();
    double XMAX = ku_getr();
    double YMIN = ku_getr();
    double YMAX = ku_getr();

    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "paghpl :" 
          << " /GRAPHICS/HPLOT/NULL "
          << " XMIN= " << XMIN 
          << " XMAX= " << XMAX 
          << " YMIN= " << YMIN
          << " YMAX= " << YMAX 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page);
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _plotter.clear();
    //remove_cyclic(_plotter,_sess.ui());
    _sess.region_set_layout(_plotter);
      
    // Enforce plotter axes values :
    _plotter.x_axis_enforced = true;
    _plotter.x_axis_automated = false;
    _plotter.x_axis_min = float(XMIN);
    _plotter.x_axis_max = float(XMAX);
    _plotter.y_axis_enforced = true;
    _plotter.y_axis_automated = false;
    _plotter.y_axis_min = float(YMIN);
    _plotter.y_axis_max = float(YMAX);
  
    _plotter.inner_frame_enforced = true;
    //_plotter.z_axis_enforced","true;
    
  } else if(cmd_path=="/GRAPHICS/HPLOT/ERRORS") {

    std::string X = ku_gets();
    std::string Y = ku_gets();
    std::string EX = ku_gets();
    std::string EY = ku_gets();
    int N = ku_geti();
    int ISYMB = ku_geti();
    double SSIZE = ku_getr();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "paghpl :" 
          << " /GRAPHICS/HPLOT/ERRORS "
          << " X= "  << inlib::sout(X) 
          << " Y= "  << inlib::sout(Y) 
          << " EX= " << inlib::sout(EX) 
          << " EX= " << inlib::sout(EY) 
          << " N= "  << N
          << " ISYMB= " << ISYMB
          << " SSIZE= " << SSIZE
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    inlib::touppercase(X);
    gopaw::vector_t* xvec = 0;
    if(!_sess.find_vector(X,xvec)) {out << "paghpl : can't find vector " << inlib::sout(X) << "." << std::endl;return;}
    if(xvec->dimension()!=1) {out << "paghpl : " << inlib::sout(X) << " not a 1D vector." << std::endl;return;}
    if(N>int(xvec->vector().size())) {
      out << "paghpl : for " << inlib::sout(X) << " N=" << N << " > " << int(xvec->vector().size()) << std::endl;
      return;
    }
    const std::vector<double>& xvecv = xvec->vector();
    
    inlib::touppercase(Y);
    gopaw::vector_t* yvec = 0;
    if(!_sess.find_vector(Y,yvec)) {out << "paghpl : can't find vector " << inlib::sout(Y) << "." << std::endl;return;}
    if(yvec->dimension()!=1) {out << "paghpl : " << inlib::sout(Y) << " not a 1D vector." << std::endl;return;}
    if(N>int(yvec->vector().size())) {
      out << "paghpl : for " << inlib::sout(Y) << " N=" << N << " > " << int(yvec->vector().size()) << std::endl;
      return;
    }
    const std::vector<double>& yvecv = yvec->vector();

    inlib::touppercase(EX);
    gopaw::vector_t* exvec = 0;
    if(!_sess.find_vector(EX,exvec)) {out << "paghpl : can't find vector " << inlib::sout(EX) << "." << std::endl;return;}
    if(exvec->dimension()!=1) {out << "paghpl : " << inlib::sout(EX) << " not a 1D vector." << std::endl;return;}
    if(N>int(exvec->vector().size())) {
      out << "paghpl : for " << inlib::sout(EX) << " N=" << N << " > " << int(exvec->vector().size()) << std::endl;
      return;
    }
    const std::vector<double>& exvecv = exvec->vector();

    inlib::touppercase(EY);
    gopaw::vector_t* eyvec = 0;
    if(!_sess.find_vector(EY,eyvec)) {out << "paghpl : can't find vector " << inlib::sout(EY) << "." << std::endl;return;}    
    if(eyvec->dimension()!=1) {out << "paghpl : " << inlib::sout(EY) << " not a 1D vector." << std::endl;return;}    
    if(N>int(eyvec->vector().size())) {
      out << "paghpl : for " << inlib::sout(EY) << " N=" << N << " > " << int(eyvec->vector().size()) << std::endl;
      return;
    }
    const std::vector<double>& eyvecv = eyvec->vector();
    
      
    // Create a temporary two dimensional IDataPointSet.
    inlib::histo::dps atmp("tmp",2);

    for ( int i = 0; i<N; i++ ) {
      //::printf("debug : %d (%d) : %g %g : %g %g\n",i,N,xvecv[i],yvecv[i],exvecv[i],eyvecv[i]);      
      inlib::histo::data_point& pt = atmp.add_point();
     {inlib::histo::measurement& ms = pt.coordinate(0);
      ms.set_value(xvecv[i]);
      double error = exvecv[i];
      ms.set_error_plus(error);
      ms.set_error_minus(error);}
     {inlib::histo::measurement& ms = pt.coordinate(1);
      ms.set_value(yvecv[i]);
      double error = eyvecv[i];
      ms.set_error_plus(error);
      ms.set_error_minus(error);}
    }

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    
    _sess.set_plotter_layout(*_page);
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _sess.region_set_layout(_plotter);
    _sess.plot_errors_2D(_plotter,atmp,ISYMB,SSIZE);

  } else if(cmd_path=="/GRAPHICS/HPLOT/KEY") {

    double X = ku_getr();
    double Y = ku_getr();
    int IATT = ku_geti();
    std::string TEXT = ku_gets();
    double DX = ku_getr();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "paghpl :" 
          << " /GRAPHICS/HPLOT/KEY "
          << " X= " << X 
          << " Y= " << Y 
          << " IATT= " << IATT
          << " TEXT= " << inlib::sout(TEXT) 
          << " DX= " << DX 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    std::string model = "M";
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"F")) model = "F"
    
    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    
    _sess.set_plotter_layout(*_page);
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    bool superpose = gopaw::session::is_an_option(OPTS,CHOPT,"S");
    _sess.add_legend(_plotter,X,Y,TEXT,model,IATT,DX,superpose);

  } else {
    out << "paghpl : " << cmd_path << " : dummy." << std::endl;
  }
}
