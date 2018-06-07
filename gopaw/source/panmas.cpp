// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void panmas_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include <inlib/sout>

void panmas_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "panmas : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/NTUPLE/MASK/FILE") {

    out << "panmas :" 
        << " /NTUPLE/MASK is an obsolete feature in CERN/PAW."
        << "  Bypass the /NTUPLE/MASK/FILE command."
        << std::endl;
    return;

  } else if(cmd_path=="/NTUPLE/MASK/CLOSE") {

    out << "panmas :" 
        << " /NTUPLE/MASK is an obsolete feature in CERN/PAW."
        << "  Bypass the /NTUPLE/MASK/CLOSE command."
        << std::endl;
    return;

  } else if(cmd_path=="/NTUPLE/MASK/LIST") {

    out << "panmas :" 
        << " /NTUPLE/MASK is an obsolete feature in CERN/PAW."
        << "  Bypass the /NTUPLE/MASK/LIST command."
        << std::endl;
    return;

  } else {
    out << "panmas : " << cmd_path << " : dummy." << std::endl;
  }
}
