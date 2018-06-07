// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pahope_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <exlib/KUIP/tools>
#include "find_object"

void pahope_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pahope : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if( (cmd_name=="ADD") || 
      (cmd_name=="SUBTRACT") ||
      (cmd_name=="MULTIPLY") ||
      (cmd_name=="DIVIDE") )  {

    //Add histograms: ID3 = C1*ID1 + C2*ID2.

    std::string ID1 = ku_gets();
    std::string ID2 = ku_gets();
    std::string ID3 = ku_gets();
    double C1 = ku_getr();
    double C2 = ku_getr();

    std::string OPTION = ku_gets();
    inlib::touppercase(OPTION);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("OPTION",OPTS);

    inlib::replace(ID3,"//PAWC","/");

    inlib::histo::h1d* h1d_a = h1d_find(_sess,ID1);
    inlib::histo::h1d* h1d_b = h1d_find(_sess,ID2);

    inlib::histo::h2d* h2d_a = h2d_find(_sess,ID1);
    inlib::histo::h2d* h2d_b = h2d_find(_sess,ID2);

    if(h1d_a && h1d_b) {
      inlib::histo::h1d* hres = new inlib::histo::h1d(*h1d_a);
      hres->scale(C1);
      inlib::histo::h1d tmp_b(*h1d_b);
      tmp_b.scale(C2);
           if(cmd_name=="ADD")      hres->add(tmp_b);
      else if(cmd_name=="SUBTRACT") hres->subtract(tmp_b);
      else if(cmd_name=="MULTIPLY") hres->multiply(tmp_b);
      else if(cmd_name=="DIVIDE")   hres->divide(tmp_b);
      _sess.remove_handle(ID3);
      _sess.add_handle(ID3,hres);

    } else if(h2d_a && h2d_b) {
      inlib::histo::h2d* hres = new inlib::histo::h2d(*h2d_a);
      hres->scale(C1);
      inlib::histo::h2d tmp_b(*h2d_b);
      tmp_b.scale(C2);
           if(cmd_name=="ADD")      hres->add(tmp_b);
      else if(cmd_name=="SUBTRACT") hres->subtract(tmp_b);
      else if(cmd_name=="MULTIPLY") hres->multiply(tmp_b);
      else if(cmd_name=="DIVIDE")   hres->divide(tmp_b);
      _sess.remove_handle(ID3);
      _sess.add_handle(ID3,hres);

    } else {
      out << "pahope : some histo not found or histos of different dimenstions." << std::endl;
      return;
    }

  } else {
    out << "pahope : " << cmd_path << " : dummy." << std::endl;
  }
}
