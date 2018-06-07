// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pafitv_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include <inlib/sout>
#include <inlib/mnmx>
#include <inlib/arrout>

#include <exlib/KUIP/tools>
#include "fit_object"

void pafitv_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pafitv : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/VECTOR/FIT") {

    std::string X = ku_gets();
    std::string Y = ku_gets();
    std::string EY = ku_gets();
    std::string FUNC = ku_gets();
    std::string CHOPT = ku_gets();
    //int NP = ku_geti();
    //std::string PAR = ku_gets();
    //std::string STEP = ku_gets();
    //std::string PMIN = ku_gets();
    //std::string PMAX = ku_gets();
    //std::string ERRPAR = ku_gets();

    if(_sess.verbose_level()) {
      out << "pafitv :" 
          << " " << cmd_path
          << " X= " << inlib::sout(X) 
          << " Y= " << inlib::sout(Y) 
          << " EY= " << inlib::sout(EY) 
          << " FUNC= " << inlib::sout(FUNC) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    std::string XNAME;
    std::vector< std::pair<int,int> > xdims;
    if(!gopaw::session::parse_name(X,XNAME,xdims,out)) return;
    inlib::touppercase(XNAME);
    gopaw::vector_t* xvec = 0;
    if(!_sess.find_vector(XNAME,xvec)) {
      out << "pafitv : can't find vector " << inlib::sout(XNAME) << "." << std::endl;
      return;
    }
    std::vector< std::pair<unsigned int,unsigned int> > xcut;
    if(!array_get_cut(*xvec,xdims,xcut)) {
      out << "pafitv : mismatch dimension in " << inlib::sout(XNAME) << std::endl;
      return;
    }

    std::string YNAME;
    std::vector< std::pair<int,int> > ydims;
    if(!gopaw::session::parse_name(Y,YNAME,ydims,out)) return;
    inlib::touppercase(YNAME);
    gopaw::vector_t* yvec = 0;
    if(!_sess.find_vector(YNAME,yvec)) {
      out << "pafitv : can't find vector " << inlib::sout(YNAME) << "." << std::endl;
      return;
    }
    std::vector< std::pair<unsigned int,unsigned int> > ycut;
    if(!array_get_cut(*yvec,ydims,ycut)) {
      out << "pafitv : mismatch dimension in " << inlib::sout(YNAME) << std::endl;
      return;
    }

    std::vector<double> vx;
   {const std::vector<double>& v = xvec->vector();
    unsigned int vsize = xvec->size();
    for(unsigned int index = 0;index<vsize;index++) {
      if(xvec->accept(index,xcut)) vx.push_back(v[index]);
    }}

    std::vector<double> vy;
   {const std::vector<double>& v = yvec->vector();
    unsigned int vsize = yvec->size();
    for(unsigned int index = 0;index<vsize;index++) {
      if(yvec->accept(index,ycut)) vy.push_back(v[index]);
    }}

   inlib::histo::dps dps("tmp",2);

   {size_t number = inlib::mn<size_t>(vx.size(),vy.size());
    for(size_t index=0;index<number;index++) {
      inlib::histo::data_point& point = dps.add_point();
      point.coordinate(0).set_value(vx[index]);
      point.coordinate(1).set_value(vy[index]);
    }}

    double XLOW,XUP;
    if(!dps.lower_extent(0,XLOW)) {}
    if(!dps.upper_extent(0,XUP)) {}

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
    if(!_page) {out << "pafitv : inlib::sg::plots not found." << std::endl;return;}

    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));    
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _sess.region_set_layout(_plotter);

    unsigned int degree;

    if( ((KIND=="CLASS") && (REST=="GAUSSIAN")) || (WHAT=="GAUSSIAN") || (WHAT=="G") ) {
      gopaw::gauss func; 
      if(!dps_fit(out,dps,0,1,func)) {
        out << "pafitv : fit failed." << std::endl;
        return;
      }

      _sess.set_func_domain(func,XLOW,XUP);
      std::string name;
      inlib::sg::f1d2plot_cp<gopaw::gauss>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::gauss>(func);
      _plottable->set_name(name);
      _plottable->set_title(FUNC);
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

    } else if( ((KIND=="CLASS") && inlib::polynomial_degree(REST,degree)) 
            || inlib::polynomial_degree(WHAT,degree)  ) {
      std::vector<double> params;
      params.resize(degree+1,0);
      gopaw::polynomial func(params); 
      if(!dps_fit(out,dps,0,1,func)) {
        out << "pafitv : fit failed." << std::endl;
        return;
      }

      _sess.set_func_domain(func,XLOW,XUP);
      std::string name;
      inlib::sg::f1d2plot_cp<gopaw::polynomial>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::polynomial>(func);
      _plottable->set_name(name);
      _plottable->set_title(FUNC);
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

    } else if( ((KIND=="CLASS") && (REST=="EXPAIDA")) || (WHAT=="EXPAIDA") || (WHAT=="E") || (WHAT=="EXPONENTIAL") ) {
      gopaw::exp_aida func; 
      if(!dps_fit(out,dps,0,1,func)) {
        out << "pafitv : fit failed." << std::endl;
        return;
      }

      _sess.set_func_domain(func,XLOW,XUP);
      std::string name;
      inlib::sg::f1d2plot_cp<gopaw::exp_aida>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::exp_aida>(func);
      _plottable->set_name(name);
      _plottable->set_title(FUNC);
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

    } else if( ((KIND=="CLASS") && (REST=="EXPHBOOK")) || (WHAT=="EXPHBOOK") || (WHAT=="EHBOOK") ) {
      gopaw::exp_hbook func; 
      if(!dps_fit(out,dps,0,1,func)) {
        out << "pafitv : fit failed." << std::endl;
        return;
      }

      _sess.set_func_domain(func,XLOW,XUP);
      std::string name;
      inlib::sg::f1d2plot_cp<gopaw::exp_hbook>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::exp_hbook>(func);
      _plottable->set_name(name);
      _plottable->set_title(FUNC);
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

    } else if( ((KIND=="CLASS") && (REST=="BREITWIGNER")) || (WHAT=="BREITWIGNER") || (WHAT=="BW") ) {
      gopaw::breit_wigner func; 
      if(!dps_fit(out,dps,0,1,func)) {
        out << "pafitv : fit failed." << std::endl;
        return;
      }

      _sess.set_func_domain(func,XLOW,XUP);
      std::string name;
      inlib::sg::f1d2plot_cp<gopaw::breit_wigner>* _plottable = new inlib::sg::f1d2plot_cp<gopaw::breit_wigner>(func);
      _plottable->set_name(name);
      _plottable->set_title(FUNC);
      _sess.plot_function_1D(_plotter,XLOW,XUP,gopaw::s_FUN_C(),_plottable);

    } else {
      out << "pafitv : can't create function " << inlib::sout(FUNC) << "." << std::endl;      
    }

  } else {
    out << "pafitv : " << cmd_path << " : dummy." << std::endl;
  }
}
