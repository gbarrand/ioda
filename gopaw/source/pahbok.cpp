// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pahbok_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include <inlib/sout>
#include <inlib/touplow>

#include <exlib/KUIP/tools>

void pahbok_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  //std::string cmd_name = ku_name();
  std::string cmd_path = ku_path() ? ku_path() : "";
  if(_sess.verbose_level()) {
    out << "pahbok : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/HISTOGRAM/CREATE/1DHISTO") {

    std::string NAME = ku_gets();
    std::string TITLE = ku_gets();
    int NCX = ku_geti();
    double XMIN = ku_getr();
    double XMAX = ku_getr();
    double VMAX = ku_getr();

    if(_sess.verbose_level()) {
      out << "pahbok : " << cmd_path
          << " NAME= " << inlib::sout(NAME) 
          << " TITLE= " << inlib::sout(TITLE) 
          << " NCX= " << NCX 
          << " XMIN= " << XMIN
          << " XMAX= " << XMAX
          << " VMAX= " << VMAX
          << std::endl;
    }

    
    if((NCX<=0)||(XMAX<=XMIN)) {
      out << "pahbok : bad booking values." << std::endl;
      return;
    }

    _sess.remove_handle(NAME);
    inlib::histo::h1d* histogram = new inlib::histo::h1d(TITLE,NCX,XMIN,XMAX);
    if(!histogram) {
      out << "pahbok : can't create histogram " << inlib::sout(NAME) << std::endl;
      return;
    }
    _sess.add_handle(NAME,histogram);

    VMAX = 0; //FIXME

  } else if(cmd_path=="/HISTOGRAM/CREATE/2DHISTO") {

    std::string NAME = ku_gets();
    std::string TITLE = ku_gets();
    int NCX = ku_geti();
    double XMIN = ku_getr();
    double XMAX = ku_getr();
    int NCY = ku_geti();
    double YMIN = ku_getr();
    double YMAX = ku_getr();
    double VMAX = ku_getr();

    if(_sess.verbose_level()) {
      out << "pahbok : " << cmd_path
          << " NAME= " << inlib::sout(NAME) 
          << " TITLE= " << inlib::sout(TITLE) 
          << " NCX= " << NCX 
          << " XMIN= " << XMIN
          << " XMAX= " << XMAX
          << " YMIN= " << YMIN
          << " YMAX= " << YMAX
          << " VMAX= " << VMAX
          << std::endl;
    }
    
    if((NCX<=0)||(XMAX<=XMIN)) {
      out << "pahbok : bad booking values." << std::endl;
      return;
    }
    if((NCY<=0)||(YMAX<=YMIN)) {
      out << "pahbok : bad booking values." << std::endl;
      return;
    }

    _sess.remove_handle(NAME);
    inlib::histo::h2d* histogram = new inlib::histo::h2d(TITLE,NCX,XMIN,XMAX,NCY,YMIN,YMAX);
    if(!histogram) {
      out << "pahbok : can't create histogram " << inlib::sout(NAME) << std::endl;
      return;
    }
    _sess.add_handle(NAME,histogram);

    VMAX = 0; //FIXME

  } else if(cmd_path=="/HISTOGRAM/CREATE/TITLE_GLOBAL") {

    std::string CHTITLE = ku_gets();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pahbok : " << cmd_path
          << " CHTITLE= " << inlib::sout(CHTITLE) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }
    
    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

  //if(gopaw::session::is_an_option(OPTS,CHOPT,"U")) {
      // CHTITLE is used as a region title.      
      _sess.set_UTIT(CHTITLE);
  //} else {
  //  // CHTITLE is used as a page title.
  //  inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
  //  if(_page) _sess.setPlotterTitle(*_page,CHTITLE);
  //}

  } else {
    out << "pahbok : " << cmd_path << " : dummy." << std::endl;

  }
}
