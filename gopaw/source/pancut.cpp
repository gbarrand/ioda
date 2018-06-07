// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pancut_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include <inlib/sout>
#include <inlib/touplow>

void pancut_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pancut :" 
        << " execute " << inlib::sout(cmd_path) 
        << std::endl;
  }

  inlib::params<int,std::string>& cuts = _sess.cuts();

  if(cmd_path=="/NTUPLE/CUTS") {

    std::string CUTID = ku_gets();
    std::string OPTION = ku_gets();
    /*std::string FNAME = */ku_gets();
    /*int WKID = */ku_geti();

    if(!CUTID.size()) return;
    std::string scutid = CUTID;
    if(CUTID[0]=='$') {
      scutid = CUTID.substr(1,CUTID.size()-1);
    }
    int cutid;
    if(!inlib::to(scutid,cutid)) { //Syntax error.
      out << "pancut : syntax error in " << inlib::sout(CUTID) << std::endl;
      return;
    }

    std::string CHOPT = OPTION;
    inlib::touppercase(CHOPT);
    if(CHOPT=="P") {
      out << "pancut : " << cmd_path << " P is a valid option, but not yet supported in gopaw." << std::endl;
    } else if(CHOPT=="-") {
      out << "pancut : " << cmd_path << " - is a valid option, but not yet supported in gopaw." << std::endl;
    } else if(CHOPT=="R") {
      out << "pancut : " << cmd_path << " R is a valid option, but not yet supported in gopaw." << std::endl;
    } else if(CHOPT=="W") {
      out << "pancut : " << cmd_path << " W is a valid option, but not yet supported in gopaw." << std::endl;
    } else if(CHOPT=="D") {
      out << "pancut : " << cmd_path << " D is a valid option, but not yet supported in gopaw." << std::endl;
    } else {

      if(cuts.is_key(cutid)) {
        out << "pancut : CUTID " << CUTID << " already in used by gopaw." << std::endl;
        return;
      }

      cuts.add(0,cutid,"",OPTION,"","");

    }

  } else {
    out << "pancut : " << cmd_path << " : dummy." << std::endl;

  }
}
