// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pahset_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <exlib/KUIP/tools>
#include "find_object"

void pahset_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  //std::string cmd_name = ku_name();
  std::string cmd_path = ku_path() ? ku_path() : "";
  if(_sess.verbose_level()) {
    out << "pahset : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/HISTOGRAM/SET/MAXIMUM") {

    std::string NAME = ku_gets();
    double VALUE = ku_getr();

    inlib::histo::h1d* h1d = h1d_find(_sess,NAME);
    inlib::histo::h2d* h2d = h2d_find(_sess,NAME);
    if(!h1d && !h2d) {
      out << "pahist : can't find histogram " << inlib::sout(NAME) << std::endl;
      return;
    }
    inlib::histo::h1d::base_histo_t* bh = 0;
    if(h1d) bh = static_cast<inlib::histo::h1d::base_histo_t*>(h1d);
    if(h2d) bh = static_cast<inlib::histo::h2d::base_histo_t*>(h2d);

    std::string smn;
    if(!bh->annotation("opaw.value.minimum",smn)) {
      bh->add_annotation("opaw.value.automated","true");
      bh->add_annotation("opaw.value.minimum","0");
      bh->add_annotation("opaw.value.maximum","1");
      if(!bh->annotation("opaw.value.minimum",smn)) {}
    }

    double mn;
    if(!inlib::to(smn,mn)) {
      out << "pahist : can't convert " << inlib::sout(smn) << " to a number." << std::endl;
    } else {
      if(VALUE<=mn) { //PAW convention : set automatic
        bh->add_annotation("opaw.value.automated","true");
      } else {
        bh->add_annotation("opaw.value.automated","false");
        bh->add_annotation("opaw.value.maximum",inlib::tos(VALUE));
      }
    }

  } else if(cmd_path=="/HISTOGRAM/SET/MINIMUM") {

    std::string NAME = ku_gets();
    double VALUE = ku_getr();

    inlib::histo::h1d* h1d = h1d_find(_sess,NAME);
    inlib::histo::h2d* h2d = h2d_find(_sess,NAME);
    if(!h1d && !h2d) {
      out << "pahist : can't find histogram " << inlib::sout(NAME) << std::endl;
      return;
    }
    inlib::histo::h1d::base_histo_t* bh = 0;
    if(h1d) bh = static_cast<inlib::histo::h1d::base_histo_t*>(h1d);
    if(h2d) bh = static_cast<inlib::histo::h2d::base_histo_t*>(h2d);

    std::string smx;
    if(!bh->annotation("opaw.value.maximum",smx)) {
      bh->add_annotation("opaw.value.automated","true");
      bh->add_annotation("opaw.value.minimum","0");
      bh->add_annotation("opaw.value.maximum","1");
      if(!bh->annotation("opaw.value.maximum",smx)) {}
    }

    double mx;
    if(!inlib::to(smx,mx)) {
      out << "pahist : can't convert " << inlib::sout(smx) << " to a number." << std::endl;
    } else {    
      if(VALUE>=mx) { //PAW convention : set automatic
        bh->add_annotation("opaw.value.automated","true");
      } else {
        bh->add_annotation("opaw.value.automated","false");
        bh->add_annotation("opaw.value.minimum",inlib::tos(VALUE));
      }
    }

  } else {
    out << "pahset : " << cmd_path << " : dummy." << std::endl;
  }
}
