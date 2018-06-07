// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void papict_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <inlib/sout>
#include <inlib/touplow>
#include <inlib/system>

#include "session"

#include <exlib/KUIP/tools>

void papict_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "papict :" 
        << " execute " << inlib::sout(cmd_path) 
        << std::endl;
  }
  
  gopaw::atts_t& atts = _sess.atts();

  if(cmd_path=="/PICTURE/IGSET") {

    std::string CHATT = ku_gets();
    double VALUE = ku_getr();
    inlib::touppercase(CHATT);

    if(CHATT=="SHOW") {  

      std::ostream& out = _sess.out();
      out << "+------------------------------------------------------------------------------+" << std::endl;
      out << "|                        IGSET : Current values in use                         |" << std::endl;
      _sess.showATTs(out,gopaw::cat_IGSET());

    } else if(CHATT=="*") { 

      atts.set_category_to_default(gopaw::cat_IGSET());

    } else if(!atts.is_key(CHATT)) {

      out << "papict : " << inlib::sout(CHATT) << " is not an attribute." << std::endl;

    } else {
      int category;
      if(!atts.category(CHATT,category)) {}
      if(category==gopaw::cat_IGSET()) {
        // see also pagraf SET command.
        // it seems that the IGSET atts do not conform to the "0=default" logic except
        // CSHI, BARW (existing in HPLSET) and NCOL :
        if((CHATT=="CSHI") ||
           (CHATT=="BARW") ||
           (CHATT=="NCOL") ){
          if(VALUE==0) VALUE = atts.default_value(CHATT); //PAW convention.
        }
        atts.set_value(CHATT,VALUE);
      } else {
        out << "papict : " << inlib::sout(CHATT) << " is not an IGSET attribute." << std::endl;
      }
    }
    
  } else if(cmd_path=="/PICTURE/PRINT") {

    std::string FILE = ku_gets();
    /*int WIDTH =*/ ku_geti();  // used to scale .gif
    /*int HEIGHT =*/ ku_geti();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "papict :" 
          << " " << cmd_path
          << " FILEE= " << inlib::sout(FILE) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }
    
    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    bool sendToPrinter = false;
    if((FILE==" ")||(FILE=="HIGZPRINTER")) {
      FILE = "paw.ps";
      sendToPrinter = true; 
    }

    std::string stype;

    // see also session/session::print_fmt(), and inex/base_viewer::write() methods :
         if(gopaw::session::is_an_option(OPTS,CHOPT,"INZB_PS"))   stype = "INZB_PS";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"INZB_JPEG")) stype = "INZB_JPEG";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"INZB_PNG"))  stype = "INZB_PNG";
  //else if(gopaw::session::is_an_option(OPTS,CHOPT,"GLFB_PS"))   stype = "GLFB_PS";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"GLFB_JPEG")) stype = "GLFB_JPEG";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"GLFB_PNG"))  stype = "GLFB_PNG";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"GL2PS_PS"))  stype = "GL2PS_PS";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"GL2PS_EPS")) stype = "GL2PS_EPS";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"GL2PS_PDF")) stype = "GL2PS_PDF";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"GL2PS_SVG")) stype = "GL2PS_SVG";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"GL2PS_TEX")) stype = "GL2PS_TEX";
    else if(gopaw::session::is_an_option(OPTS,CHOPT,"GL2PS_PGF")) stype = "GL2PS_PGF";
    else stype = _sess.print_fmt(FILE);
	 
    if(_sess.verbose_level()) {
      out << "papict :" 
          << " selected format " << inlib::sout(stype)
          << std::endl;
    }
    
   {inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    gopaw::iwidget* widget = _sess.ui().current_widget();
    if(_page && widget) {
      if(stype=="GLFB_JPEG") {
	widget->set_produce_file(FILE);
	widget->set_produce_jpeg(true);
	_sess.ui().render_it();
      } else if(stype=="GLFB_PNG") {
	widget->set_produce_file(FILE);
	widget->set_produce_png(true);
	_sess.ui().render_it();
      } else {
        widget->write(FILE,stype);
      }
    }}

    if(sendToPrinter) {
      // If defined HIGZPRINTER should be of the form :
      //   csh> setenv HIGZPRINTER 'lp -dprinter_name paw.ps'
      //    sh> HIGZPRINTER='lp -dprinter_name paw.ps';export HIGZPRINTER
      std::string HIGZPRINTER;
      if(inlib::getenv("HIGZPRINTER",HIGZPRINTER)) {
        inlib::std_system(out,HIGZPRINTER);
      }
    }

  } else {
    out << "papict : " << cmd_path << " : dummy." << std::endl;
  }
}
