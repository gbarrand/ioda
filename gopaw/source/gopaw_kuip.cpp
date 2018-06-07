// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.


extern "C" {
  int gopaw_kuip(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>

#include "session"

#include <inlib/sys/dir>

#ifdef APP_USE_PYTHON
#include <inlib/args>
#endif

int gopaw_kuip(void* a_tag) {

  gopaw::session& _sess = *((gopaw::session*)a_tag);
  std::ostream& out = _sess.out();
  
  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();

  if(_sess.verbose_level()) out << "gopaw_kuip : execute " << inlib::sout(cmd_path) << std::endl;

  ///////////////////////////////////////////////////////
  /// SESSION ///////////////////////////////////////////
  ///////////////////////////////////////////////////////
  if(cmd_path=="/SESSION/NOW") {
    //std::string NAME = ku_gets();
    _sess.set_atime(inlib::atime::now());
    return gopaw::status_success();
  }
  
  if(cmd_path=="/SESSION/ELAPSED") {
    _sess.out() << "elapsed : " << inlib::atime::elapsed(_sess.atime()) << std::endl;
    return gopaw::status_success();
  }
  
  if(cmd_path=="/SESSION/MOVE") { //used in pawex13.kumac.
    std::string FROM = ku_gets();
    std::string TO = ku_gets();

    if(_sess.verbose_level()) {
      out << "gopaw_kuip : " << cmd_path
          << " FROM= " << inlib::sout(FROM) 
          << " TO= " << inlib::sout(TO) 
          << std::endl;
    }

    if(FROM==TO) return gopaw::status_success();

    if(!inlib::file::copy_bytes(FROM,TO)) {
      out << "gopaw_kuip : " << cmd_path << " inlib::file::copy_bytes(FROM,TO) failed with :" << std::endl;
      out << " FROM = " << inlib::sout(FROM) << std::endl;
      out << "   TO = " << inlib::sout(TO) << std::endl;
      return gopaw::status_failure();
    }
    if(!inlib::file::std_remove(FROM)) {
      out << "gopaw_kuip : " << cmd_path << " inlib::file::std_remove(FROM) failed with :" << std::endl;
      out << " FROM = " << inlib::sout(FROM) << std::endl;
      return gopaw::status_failure();
    }

    return gopaw::status_success();
  }

  if(cmd_path=="/SESSION/LLS") {
    if(!inlib::dir::ls(out,".",false)) {}
    return gopaw::status_success();
  }
 
  if(cmd_path=="/SESSION/PYTHON") {
#ifdef APP_USE_PYTHON
    std::string FILE = ku_gets();

    if(_sess.verbose_level()) {
      out << "gopaw_kuip : " << cmd_path
          << " FILE= " << inlib::sout(FILE) 
          << std::endl;
    }
    
    std::string sfile;
    inlib::file_name(FILE,sfile); //to expand environment variables.

    if(_sess.verbose_level()) {
      out << "gopaw_kuip : file " << inlib::sout(sfile) << ". FILE is " << inlib::sout(FILE) << "." << std::endl;
    }

    gopaw::iwidget* widget = _sess.ui().current_widget();
    if(widget) {
      inlib::args args;bool done;
      _sess.py_opener().open(sfile,widget->viewer(),args,done);
    }
    
    return gopaw::status_success();
#else  
    out << "gopaw_kuip : " << cmd_path << " : gopaw not built with -DAPP_USE_PYTHON." << std::endl;
    return gopaw::status_failure();
#endif
  }
  
  if(cmd_path=="/SESSION/DELETE") {
    std::string ID = ku_gets(); //could contain wildcard.
    _sess.remove_handles(ID);
    return gopaw::status_success();
  }

  ///////////////////////////////////////////////////////
  /// VIEWER ////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  if(cmd_path=="/VIEWER/RENDER") {
    //std::string NAME = ku_gets();
    _sess.ui().render_it();
    return gopaw::status_success();
  }
  
  if(cmd_path=="/VIEWER/SYNCHRONIZE") {
    _sess.ui().synchronize();
    return gopaw::status_success();
  }
  
  if(cmd_path=="/VIEWER/PAGE/SET_LAYOUT") { //use from ioda (exlib::KUIP::opener).
    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(_page) {
      _sess.set_plots_layout(*_page);
      _sess.set_plots_zone(*_page);
    }
    return gopaw::status_success();
  }

  if(cmd_path=="/VIEWER/BACKGROUND_COLOR") {
    double R = ku_getr();
    double G = ku_getr();
    double B = ku_getr();
    double A = ku_getr();
    gopaw::iwidget* widget = _sess.ui().current_widget();
    if(widget) widget->set_background_color(R,G,B,A);
    return gopaw::status_success();
  }
  
#ifdef APP_USE_EXPAT
  if(cmd_path=="/VIEWER/PLOTTER/SET_STYLE") {
    std::string STYLE = ku_gets();
    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(_page) {
      inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
      _sess.style_from_res(STYLE,_plotter,false);
    }
    return gopaw::status_success();
  }
#endif
  
  _sess.out() << "gopaw_kuip : " << cmd_path << " : unknow command." << std::endl;

  return gopaw::status_failure();
}
