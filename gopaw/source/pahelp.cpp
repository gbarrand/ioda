// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pahelp_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"

#include <fstream>

void pahelp_(void* a_tag) {
  
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  int lun;
  std::string CHPATL = ku_help(&lun);
 
  if(CHPATL=="SET") {
    //  CALL HPLSET('SHOW',0.)
    char*  fname = ku_uhlp_file();
    std::ofstream file(fname);
    _sess.showATTs(file,gopaw::cat_HPLSET());
    file.close();
  } else if(CHPATL=="OPTION") {
    //  CALL HPLOPT('SHOW',1)
    char*  fname = ku_uhlp_file();
    std::ofstream file(fname);
    _sess.showOPTs(file);
    file.close();
  } else if(CHPATL=="IGSET") {
    //CALL IGSET('SHOW',0.)
    char*  fname = ku_uhlp_file();
    std::ofstream file(fname);
    _sess.showATTs(file,gopaw::cat_IGSET());
    file.close();

//} else if(CHPATL=="SLIDE") {
//  CALL SLIHLP
//} else if(CHPATL=="CALL") {
//  CALL PAWCSH

  } else {
    _sess.out() << "pahelp : CHPATL " << CHPATL << " not handled." << std::endl;
  }

}
