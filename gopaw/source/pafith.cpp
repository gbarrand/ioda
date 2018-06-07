// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pafith_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include "fit_object"
#include <exlib/KUIP/tools>
#include "find_object"

void pafith_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pafith : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/HISTOGRAM/FIT") {

    std::string ID = ku_gets();
    std::string FUNC = ku_gets();
    std::string CHOPT = ku_gets();
    //int NP = ku_geti();
    //std::string PAR = ku_gets();
    //std::string STEP = ku_gets();
    //std::string PMIN = ku_gets();
    //std::string PMAX = ku_gets();
    //std::string ERRPAR = ku_gets();

    if(_sess.verbose_level()) {
      out << "pafith :" 
          << " " << cmd_path
          << " ID= " << inlib::sout(ID) 
          << " FUNC= " << inlib::sout(FUNC) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    inlib::histo::h1d* histogram = h1d_find(_sess,ID);
    if(!histogram) {
      out << "pafith : can't find histogram " << inlib::sout(ID) << std::endl;
      return;
    }
    double XLOW = histogram->axis().lower_edge();
    double XUP = histogram->axis().upper_edge();

    inlib::touppercase(FUNC);
    if(FUNC=="E") FUNC = "EHBOOK";

    std::string KIND;
    std::string REST;
   {std::string::size_type pos = FUNC.find(':');
    if(pos==std::string::npos) {
      REST = FUNC;
    } else {
      KIND = FUNC.substr(0,pos);
      REST = FUNC.substr(pos+1,FUNC.size()-(pos+1));
    }
    inlib::touppercase(KIND);}
    std::string WHAT = FUNC;

    //out << "pafitv : WHAT = " << inlib::sout(WHAT)
    //    << ", KIND = " << inlib::sout(KIND) << ", REST = " << inlib::sout(REST) << "." << std::endl;
  
    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) {out << "pafith : inlib::sg::plots not found." << std::endl;return;}
    
    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _sess.region_set_layout(_plotter);

    // by default the histo and the function are drawn :
   {std::vector< std::pair<int,int> > dims;  
    dims.push_back( std::pair<int,int>(inlib::not_found(),inlib::not_found())); 
    if(gopaw::region_set_limits(_plotter,*histogram,dims,out)) {
      _sess.plot_histogram_1D(_plotter,*histogram,ID,"",false); 
    }}

    std::string func_name;
    _sess.gen_func_name(func_name);

    unsigned int degree;

    if( ((KIND=="CLASS") && (REST=="GAUSSIAN")) || (WHAT=="GAUSSIAN") || (WHAT=="G") ) {
      double h = histogram->max_bin_height() - histogram->min_bin_height();
      double m = histogram->mean();
      double w = histogram->rms();

      gopaw::gauss* func = new gopaw::gauss(h,m,w); 
      if(!hist_fit(out,*histogram,*func)) {
        out << "pafith : fit failed." << std::endl;
        delete func;
	return;
      }

      _sess.set_func_domain(*func,XLOW,XUP);
      inlib::sg::f1d2plot_cp<gopaw::gauss>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::gauss>(*func);
      _plottable->set_name(func_name);
      _plottable->set_title(func_name); //?
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

      _sess.add_handle(func_name,static_cast<gopaw::base_func*>(func));
      histogram->add_annotation("fit.function",func_name);

    } else if( ((KIND=="CLASS") && (REST=="BREITWIGNER")) || (WHAT=="BREITWIGNER") || (WHAT=="BW") ) {
      double h = histogram->max_bin_height() - histogram->min_bin_height();
      double m = histogram->mean();
      double w = histogram->rms();

      gopaw::breit_wigner* func = new gopaw::breit_wigner(h,m,w); 
      if(!hist_fit(out,*histogram,*func)) {
        out << "pafith : fit failed." << std::endl;
        delete func;
        return;
      }

      _sess.set_func_domain(*func,XLOW,XUP);
      inlib::sg::f1d2plot_cp<gopaw::breit_wigner>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::breit_wigner>(*func);
      _plottable->set_name(func_name);
      _plottable->set_title(func_name); //?
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

      _sess.add_handle(func_name,static_cast<gopaw::base_func*>(func));
      histogram->add_annotation("fit.function",func_name);

    } else if( ((KIND=="CLASS") && inlib::polynomial_degree(REST,degree)) 
            || inlib::polynomial_degree(WHAT,degree)  ) {
      double h = histogram->max_bin_height() - histogram->min_bin_height();
      double m = histogram->mean();
      double w = histogram->rms();

      std::vector<double> params;
      params.resize(degree+1,0);
      if(degree==2) {
        params[0] = h;
        params[1] = m;
        params[2] = w;
      } 

      gopaw::polynomial* func = new gopaw::polynomial(params); 
      if(!hist_fit(out,*histogram,*func)) {
        out << "pafith : fit failed." << std::endl;
        delete func;
        return;
      }

      _sess.set_func_domain(*func,XLOW,XUP);
      inlib::sg::f1d2plot_cp<gopaw::polynomial>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::polynomial>(*func);
      _plottable->set_name(func_name);
      _plottable->set_title(func_name); //?
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

      _sess.add_handle(func_name,static_cast<gopaw::base_func*>(func));
      histogram->add_annotation("fit.function",func_name);

    } else if( ((KIND=="CLASS") && (REST=="EXPAIDA")) || (WHAT=="EXPAIDA") || (WHAT=="E") || (WHAT=="EXPONENTIAL") ) {
  
      double xx1,xx2;
      double val1,val2;
      bool status1 = gopaw::half_lower_mean_point(*histogram,xx1,val1);
      bool status2 = gopaw::half_upper_mean_point(*histogram,xx2,val2);
      if( !status1 || !status2 || ((xx2-xx1)==0.) ) {
        out << "pafith : can't compute starting values." << std::endl;
        return;
      }
      if(!val1) {
        out << "pafith : can't compute starting values." << std::endl;
        return;
      }
      double dx = xx2 - xx1;
      if(!dx) {
        out << "pafith : can't compute starting values." << std::endl;
        return;
      }
      // Deduce p0,p1 of p0*exp(p1*x) with the two upper points :
      double p1 = ::log(val2/val1) / dx;
      double p0 = ::exp(p1*xx1);
      if(!p0) {
        out << "pafith : can't compute starting values." << std::endl;
        return;
      }    
      p0 = val1 / p0;
  
      gopaw::exp_aida* func = new gopaw::exp_aida(p0,p1); 
      if(!hist_fit(out,*histogram,*func)) {
        out << "pafith : fit failed." << std::endl;
        delete func;
        return;
      }

      _sess.set_func_domain(*func,XLOW,XUP);
      inlib::sg::f1d2plot_cp<gopaw::exp_aida>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::exp_aida>(*func);
      _plottable->set_name(func_name);
      _plottable->set_title(func_name); //?
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

      _sess.add_handle(func_name,static_cast<gopaw::base_func*>(func));
      histogram->add_annotation("fit.function",func_name);

    } else if( ((KIND=="CLASS") && (REST=="EXPHBOOK")) || (WHAT=="EXPHBOOK") || (WHAT=="EHBOOK") ) {
      double xx1,xx2;
      double val1,val2;
      bool status1 = gopaw::half_lower_mean_point(*histogram,xx1,val1);
      bool status2 = gopaw::half_upper_mean_point(*histogram,xx2,val2);
      if( !status1 || !status2 || ((xx2-xx1)==0.) ) {
        out << "pafith : can't compute starting values." << std::endl;
        return;
      }
      
      // Deduce a,b of exp(a+bx) with the two upper points :
      double b = ::log(val2/val1) / ( xx2 - xx1);
      double a = ::log( val1 / exp ( b * xx1 ) );
  
      gopaw::exp_hbook* func = new gopaw::exp_hbook(a,b); 
      if(!hist_fit(out,*histogram,*func)) {
        out << "pafith : fit failed." << std::endl;
        delete func;
        return;
      }

      _sess.set_func_domain(*func,XLOW,XUP);
      inlib::sg::f1d2plot_cp<gopaw::exp_hbook>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::exp_hbook>(*func);
      _plottable->set_name(func_name);
      _plottable->set_title(func_name); //?
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

      _sess.add_handle(func_name,static_cast<gopaw::base_func*>(func));
      histogram->add_annotation("fit.function",func_name);

    } else {
      out << "pafith : can't create function " << inlib::sout(FUNC) << "." << std::endl;      
    }

  } else {
    out << "pafith : " << cmd_path << " : dummy." << std::endl;
  }
}
