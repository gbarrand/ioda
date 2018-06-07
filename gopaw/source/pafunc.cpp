// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pafunc_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include <exlib/KUIP/tools>
#include "plot_histogram_2D"
#include "find_object"

void pafunc_(void* a_tag) { 

  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  //std::string cmd_name = ku_name();
  std::string cmd_path = ku_path() ? ku_path() : "";
  if(_sess.verbose_level()) {
    out << "pafunc : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/FUNCTION/PLOT") {

    std::string UFUNC = ku_gets();
    double XLOW = ku_getr();
    double XUP = ku_getr();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pafunc :" 
          << " " << cmd_path
          << " UFUNC= " << inlib::sout(UFUNC) 
          << " XLOW= " << XLOW
          << " XUP= " << XUP
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    std::string func_name;
    _sess.gen_func_name(func_name);

    char* path = ku_macro_path();
    std::string paths = path?path:"";

    // retain_arrays true in case a vector is deleted whilst the plotter still visualize the function.
    // BUT then, the execution of the function can't update the vectors, if some are used by the function code.

    if(!_sess.remove_handle(func_name)) {}
    gopaw::base_func* func = create_function_vector(out,func_name,paths,UFUNC,_sess,true);
    if(!func) {
      out << "pafunc : can't create function " << inlib::sout(UFUNC) << std::endl;
      return;
    }
    _sess.add_handle(func_name,func);

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    
    _sess.set_plotter_layout(*_page);
    bool superpose = gopaw::session::is_an_option(OPTS,CHOPT,"S");
    //if(superpose) func->set_title(""); // If superposed, CERN/PAW ignores the title of the object.
                                         // It should be done on the plottable.
    _sess.plotter_next(*_page,superpose);
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _sess.region_set_layout(_plotter);
	  
    if(func->dimension()!=1) {
      out << "pafunc : dimension mismatch. /FUNCTION/PLOT is for 1D functions"
      << " (" << func->dimension() << " found)." << std::endl;
      return;
    }
    
    if(gopaw::session::is_an_option(OPTS,CHOPT,"P")) {
      if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::gauss>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"P")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::breit_wigner>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"P")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::polynomial>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"P")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::exp_aida>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"P")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::exp_hbook>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"P")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::scripted>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"P")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::compiled>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"P")) {}
      else {
        out << "pafunc : " << inlib::sout(cmd_path) << " : unknown func class " << inlib::sout(func->s_cls()) << std::endl;
      }
    }

    //not } else if() { //see users/fabry_c.kumac with spc options.
	      
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) {
    if(true) {
      if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::gauss>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::breit_wigner>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::polynomial>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::exp_aida>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::exp_hbook>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::scripted>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
      else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::compiled>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
      else {
        out << "pafunc : " << inlib::sout(cmd_path) << " : unknown func class " << inlib::sout(func->s_cls()) << std::endl;
      }
    }

  } else if(cmd_path=="/FUNCTION/FUN2") {

    std::string ID = ku_gets();
    std::string UFUNC = ku_gets();
    int NCX = ku_geti();
    double XMIN = ku_getr();
    double XMAX = ku_getr();
    int NCY = ku_geti();
    double YMIN = ku_getr();
    double YMAX = ku_getr();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pafunc :" 
          << " " << cmd_path
          << " ID= " << inlib::sout(ID) 
          << " UFUNC= " << inlib::sout(UFUNC) 
          << " NCX= " << NCX
          << " XMIN= " << XMIN
          << " XMAX= " << XMAX
          << " NCY= " << NCY
          << " YMIN= " << YMIN
          << " YMAX= " << YMAX
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    if((NCX<=0)||(XMAX<=XMIN)) {
      out << "pafunc : bad booking values." << std::endl;
      return;
    }
    if((NCY<=0)||(YMAX<=YMIN)) {
      out << "pafunc : bad booking values." << std::endl;
      return;
    }

    _sess.remove_handle(ID);
    inlib::histo::h2d* histogram = new inlib::histo::h2d(UFUNC,NCX,XMIN,XMAX,NCY,YMIN,YMAX);
    if(!histogram) {
      out << "pafunc : can't create histogram " << inlib::sout(ID) << std::endl;
      return;
    }
    _sess.add_handle(ID,histogram);

    double dx = (XMAX - XMIN)/NCX;
    double dy = (YMAX - YMIN)/NCY;

    std::string func_name;
    _sess.gen_func_name(func_name);

    char* path = ku_macro_path();
    std::string paths = path?path:"";

    // retain_arrays false, so that the execution of the function can change the vectors.
    gopaw::base_func* func = create_function_vector(out,func_name,paths,UFUNC,_sess,false);
    if(!func) {
      out << "pafunc : can't create function " << inlib::sout(UFUNC) << std::endl;
      return;
    }

    const std::string& codelet  = func->codelet_string();
    if(codelet.empty()) {
      out << "pafunc : " << cmd_path << " : function with no codelet." << std::endl;
    }
    histogram->add_annotation("opaw.codelet",codelet);

    double epsilon = 0;

    std::vector<double> args(2);
    for(int ibin = 0;ibin<NCX;ibin++) {
      double x = XMIN + (ibin + 0.5 + epsilon) * dx;
      args[0] = x;
      for(int jbin = 0;jbin<NCY;jbin++) {
        double y = YMIN + (jbin + 0.5 + epsilon) * dy;
        args[1] = y;
        double value = func->point_value(args);
        histogram->fill(x,y,value);
      }
    }
    delete func;

    if(gopaw::session::is_an_option(OPTS,CHOPT," ")) return;

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
      
    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _sess.region_set_layout(_plotter);
  
    if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF1")) {
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_LEVEL(),out,0,PALNB,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF2")) {
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_LEVEL_NO_EDGE(),out,0,PALNB,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF3")) {
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_CONT(),out,0,PALNB,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF4")) {
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_LIGHT(),out,0,0,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF5")) {
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_COL(),out,0,PALNB,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,gopaw::s_SURF())) {
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF(),out,0,0,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"LEGO1")) {
      plot_histogram_2D(_plotter,*histogram,gopaw::s_LEGO_LIGHT(),out,0,0,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"LEGO2")) {
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_LEGO_LEVEL(),out,0,PALNB,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"LEGO3")) {
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_LEGO_COL(),out,0,PALNB,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,gopaw::s_LEGO())) {
      plot_histogram_2D(_plotter,*histogram,gopaw::s_LEGO(),out,0,0,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"TEXT")) {
      plot_histogram_2D(_plotter,*histogram,"TEXT",out,0,0,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"CONT")) {
      plot_histogram_2D(_plotter,*histogram,"CONT_DMOD",out,10,0,_sess);
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"BOX")) {
      plot_histogram_2D(_plotter,*histogram,"BOX",out,0,0,_sess);
    }
    if(gopaw::session::has_unkown_option(OPTS,CHOPT)) {
      plot_histogram_2D(_plotter,*histogram,"",out,0,0,_sess);
    }

  } else if(cmd_path=="/FUNCTION/FUN1") {

    std::string ID = ku_gets();
    std::string UFUNC = ku_gets();
    int NCX = ku_geti();
    double XMIN = ku_getr();
    double XMAX = ku_getr();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pafunc :" 
          << " " << cmd_path
          << " ID= " << inlib::sout(ID) 
          << " UFUNC= " << inlib::sout(UFUNC) 
          << " NCX= " << NCX
          << " XMIN= " << XMIN
          << " XMAX= " << XMAX
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    if((NCX<=0)||(XMAX<=XMIN)) {
      out << "pafunc :" 
          << " bad booking values."
          << std::endl;
      return;
    }

    _sess.remove_handle(ID);
    inlib::histo::h1d* histogram = new inlib::histo::h1d(UFUNC,NCX,XMIN,XMAX);
    if(!histogram) {
      out << "pafunc : can't create histogram " << inlib::sout(ID) << std::endl;
      return;
    }
    _sess.add_handle(ID,histogram);

    double dx = (XMAX - XMIN)/NCX;

    std::string func_name;
    _sess.gen_func_name(func_name);

    char* path = ku_macro_path();
    std::string paths = path?path:"";

    // retain_arrays false, so that the execution of the function can change the vectors.
    gopaw::base_func* func = create_function_vector(out,func_name,paths,UFUNC,_sess,false);
    if(!func) {
      out << "pafunc : can't create function " << inlib::sout(UFUNC) << std::endl;
      return;
    }

    //AIDA::IAnnotation& annotation = histogram->annotation();
    //annotation.addItem("opaw.codelet",func->codeletString());

    //double epsilon = 0.001; // Trick to avoid zero.
    double epsilon = 0;

    std::vector<double> args(1);
    for(int ibin = 0;ibin<NCX;ibin++) {
      double x = XMIN + (ibin + 0.5 + epsilon) * dx;
      args[0] = x;
      double value = func->point_value(args);
      histogram->fill(x,value);
    }
    delete func;

    if(gopaw::session::is_an_option(OPTS,CHOPT," ")) return;

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
      
    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _sess.region_set_layout(_plotter);
    if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) _sess.plot_histogram_1D(_plotter,*histogram,ID,"C",false); 
    if(gopaw::session::is_an_option(OPTS,CHOPT,"E")) _sess.plot_histogram_1D(_plotter,*histogram,ID,"E",false); 
  
  } else if(cmd_path=="/FUNCTION/POINTS") {

    int NPX = ku_geti();
    int NPY = ku_geti();
    int NPZ = ku_geti();
    _sess.setFunctionPoints(NPX,NPY,NPZ);

  } else if(cmd_path=="/FUNCTION/ANGLE") {

    double THETA = ku_getr(); //in degrees
    double PHI = ku_getr();   //in degrees

    _sess.set_THETA(THETA);
    _sess.set_PHI(PHI);

  } else {
    out << "pafunc :" 
        << " " << cmd_path << " : dummy." << std::endl;

  }
}
