// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pavect_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <exlib/KUIP/tools>
#include "find_object"

#include <inlib/arrout>

void pavect_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  //std::string cmd_name = ku_name();
  std::string cmd_path = ku_path() ? ku_path() : "";
  if(_sess.verbose_level()) {
    out << "pavect :" 
        << " execute " << inlib::sout(cmd_path) 
        << std::endl;
  }

  if(cmd_path=="/VECTOR/DRAW") {

    std::string VNAME = ku_gets();
    std::string ID = ku_gets();
    std::string CHOPT = ku_gets();
    if(_sess.verbose_level()) {
      out << "pavect :" 
          << " " << cmd_path
          << " VNAME= " << inlib::sout(VNAME) 
          << " ID= " << inlib::sout(ID) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    std::string NAME;
    std::vector< std::pair<int,int> > dims;
    if(!gopaw::session::parse_name(VNAME,NAME,dims,out)) return;
    inlib::touppercase(NAME);
    gopaw::vector_t* vector = 0;
    if(!_sess.find_vector(NAME,vector)) {
      out << "pavect : can't find vector " << inlib::sout(NAME) << std::endl;
      return;
    } 
    std::vector< std::pair<unsigned int,unsigned int> > cut;
    if(!array_get_cut(*vector,dims,cut)) {
      out << "pavect : mismatch dimension in " << inlib::sout(VNAME) << std::endl;
      return;
    }

    const std::vector<double>& v = vector->vector();
    unsigned int vsize = v.size();

    int n = 0;
   {for(unsigned int index = 0;index<vsize;index++) {
      if(vector->accept(index,cut)) n++;
    }}

    //FIXME : must be able to do a region.plot(gopaw::vector_t).

    inlib::histo::h1d rep(VNAME,n,1,n+1);
    int i = 0;
    for(unsigned int index = 0;index<vsize;index++) {
      if(vector->accept(index,cut)) {
        rep.fill(i+1.5,v[index]);
        i++;
      }
    }

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    _sess.region_set_layout(_plotter);

    bool empty = true;
    if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) {_sess.plot_histogram_1D(_plotter,rep,ID,"C",true);empty = false;}
    if(gopaw::session::is_an_option(OPTS,CHOPT,"L")) {_sess.plot_histogram_1D(_plotter,rep,ID,"L",true);empty = false;}
    if(gopaw::session::is_an_option(OPTS,CHOPT,"E")) {_sess.plot_histogram_1D(_plotter,rep,ID,"E",true);empty = false;}
    if(gopaw::session::is_an_option(OPTS,CHOPT,"P")) {_sess.plot_histogram_1D(_plotter,rep,ID,"P",true);empty = false;}
    if(gopaw::session::is_an_option(OPTS,CHOPT,"*")) {_sess.plot_histogram_1D(_plotter,rep,ID,"*",true);empty = false;}
    if(gopaw::session::is_an_option(OPTS,CHOPT,"B")) {_sess.plot_histogram_1D(_plotter,rep,ID,"B",true);empty = false;}
    if(gopaw::session::is_an_option(OPTS,CHOPT," ")) {_sess.plot_histogram_1D(_plotter,rep,ID,"",true);empty = false;}
    if(empty) _sess.plot_histogram_1D(_plotter,rep,ID,"",true); 

  } else if(cmd_path=="/VECTOR/PLOT") {

    std::string VNAME = ku_gets();
    std::string ID = ku_gets();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pavect :" 
          << " " << cmd_path
          << " VNAME= " << inlib::sout(VNAME) 
          << " ID= " << inlib::sout(ID) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    std::vector<std::string> VNAMEs;
    inlib::words(VNAME,"%",false,VNAMEs);
    if(VNAMEs.size()==2) {
      std::string YNAME = VNAMEs[0];
      inlib::touppercase(YNAME);
      gopaw::vector_t* yvec = 0;
      if(!_sess.find_vector(YNAME,yvec)) {
        out << "pavect : can't find vector " << inlib::sout(YNAME) << std::endl;
        return;
      } 
      std::string XNAME = VNAMEs[1];
      inlib::touppercase(XNAME);
      gopaw::vector_t* xvec = 0;
      if(!_sess.find_vector(XNAME,xvec)) {
        out << "pavect : can't find vector " << inlib::sout(XNAME) << std::endl;
        return;
      } 

      int xsize = xvec->size();
      int ysize = yvec->size();
      if(xsize!=ysize) {
        out << "pavect :" 
            << " size mismatch for vectors " 
            << inlib::sout(XNAME) << " and "
            << inlib::sout(YNAME) << "."
            << std::endl;
        return;
      }

      const std::vector<double>& xv = xvec->vector();
      const std::vector<double>& yv = yvec->vector();

      inlib::histo::c2d cloud;

      for(int index=0;index<xsize;index++) cloud.fill(xv[index],yv[index]);

      inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
      if(_page) {
        _sess.set_plotter_layout(*_page);
        _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
        inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
        _sess.region_set_layout(_plotter);
	  
        if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) _sess.plot_cloud_2D(_plotter,cloud,"C");
        if(gopaw::session::is_an_option(OPTS,CHOPT,"L")) _sess.plot_cloud_2D(_plotter,cloud,"L");
        if(gopaw::session::is_an_option(OPTS,CHOPT,"P")) _sess.plot_cloud_2D(_plotter,cloud,"P");
      }

    } else {

      std::string NAME;
      std::vector< std::pair<int,int> > dims;
      if(!gopaw::session::parse_name(VNAME,NAME,dims,out)) return;
      inlib::touppercase(NAME);
      gopaw::vector_t* vector = 0;
      if(!_sess.find_vector(NAME,vector)) {
        out << "pavect : can't find vector " << inlib::sout(NAME) << std::endl;
        return;
      } 
      std::vector< std::pair<unsigned int,unsigned int> > cut;
      if(!array_get_cut(*vector,dims,cut)) {
        out << "pavect : mismatch dimension in " << inlib::sout(VNAME) << std::endl;
        return;
      }
  
      const std::vector<double>& v = vector->vector();
      unsigned int vsize = v.size();
  
      double xmn = 0;
      double xmx = 0;
      int n = 0;
      double value;
     {for(unsigned int index = 0;index<vsize;index++) {
        if(vector->accept(index,cut)) {
          value = v[index];
          if(n==0) {
            xmx = xmn = value;
          } else {
            xmn = inlib::mn(xmn,value);
            xmx = inlib::mx(xmx,value);
          }
          n++;
        }
      }}
  
      if(n<=0) {
        out << "pavect : vector selection is empty." << inlib::sout(VNAME) << std::endl;
        return;
      }
  
      if(xmx==xmn) {
        double value = xmn;
        xmn = value - 1;
        xmx = value + 1;
      } else { 
        double dx = 0.1 * (xmx - xmn);
        xmn -= dx;
        xmx += dx;
      }
       
      _sess.remove_handle(ID); //ok.

      inlib::histo::h1d* rep = new inlib::histo::h1d(VNAME,100,xmn,xmx);
      if(!rep) {
        out << "pavect : can't create histogram " << inlib::sout(ID) << " in memory tree." << std::endl;
        return;
      }
      _sess.add_handle(ID,rep);
  
      for(unsigned int index = 0;index<vsize;index++) {
        if(vector->accept(index,cut)) {
          rep->fill(v[index]);
        }
      }
  
      inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
      if(_page) {
          _sess.set_plotter_layout(*_page);
          _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
          inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
          _sess.region_set_layout(_plotter);
	  
          bool empty = true;
          if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) {
            _sess.plot_histogram_1D(_plotter,*rep,ID,"C",false);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"L")) {
            _sess.plot_histogram_1D(_plotter,*rep,ID,"L",false); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"E")) {
            _sess.plot_histogram_1D(_plotter,*rep,ID,"E",false); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"P")) {
            _sess.plot_histogram_1D(_plotter,*rep,ID,"P",false); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"*")) {
            _sess.plot_histogram_1D(_plotter,*rep,ID,"*",false); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"B")) {
            _sess.plot_histogram_1D(_plotter,*rep,ID,"B",false); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT," ")) {
            _sess.plot_histogram_1D(_plotter,*rep,ID,"",false); 
            empty = false;
          }
          if(empty) _sess.plot_histogram_1D(_plotter,*rep,ID,"",false); 
      }

    } //end VNAMEs.size() == 2

  } else if(cmd_path=="/VECTOR/HFILL") {

    std::string VNAME = ku_gets();
    std::string ID = ku_gets();

    std::string NAME;
    std::vector< std::pair<int,int> > dims;
    if(!gopaw::session::parse_name(VNAME,NAME,dims,out)) return;
    inlib::touppercase(NAME);
    gopaw::vector_t* vector = 0;
    if(!_sess.find_vector(NAME,vector)) {
      out << "pavect : can't find vector " << inlib::sout(NAME) << std::endl;
      return;
    } 
    std::vector< std::pair<unsigned int,unsigned int> > cut;
    if(!array_get_cut(*vector,dims,cut)) {
      out << "pavect : mismatch dimension in " << inlib::sout(VNAME) << std::endl;
      return;
    }

    inlib::histo::h1d* histogram = h1d_find(_sess,ID);
    if(!histogram) {
      out << "pavect : can't find histogram " << inlib::sout(ID) << std::endl;
      return;
    }

    const std::vector<double>& v = vector->vector();
    unsigned int vsize = v.size();

    for(unsigned int index = 0;index<vsize;index++) {
      if(vector->accept(index,cut)) {
        histogram->fill(v[index]);
      }
    }

  } else {
    out << "pavect : " << cmd_path << " : dummy." << std::endl;

  }
}
