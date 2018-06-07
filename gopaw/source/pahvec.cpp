// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pahvec_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <exlib/KUIP/tools>
#include "find_object"

#include <inlib/arrout>

void pahvec_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  //std::string cmd_name = ku_name();
  std::string cmd_path = ku_path() ? ku_path() : "";
  if(_sess.verbose_level()) {
    out << "pahvec : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/HISTOGRAM/PUT_VECT/CONTENTS") {

    std::string ID = ku_gets();
    std::string VNAME = ku_gets();

    inlib::histo::h1d* histogram = h1d_find(_sess,ID);
    if(!histogram) {
      out << "pahvec : can't find histogram " << inlib::sout(ID) << std::endl;
      return;
    }

    std::string NAME;
    std::vector< std::pair<int,int> > dims;
    if(!gopaw::session::parse_name(VNAME,NAME,dims,out)) return;
    inlib::touppercase(NAME);
    gopaw::vector_t* vector = 0;
    if(!_sess.find_vector(NAME,vector)) {
      out << "pahvec : can't find vector " << inlib::sout(NAME) << std::endl;
      return;
    } 
    std::vector< std::pair<unsigned int,unsigned int> > cut;
    if(!array_get_cut(*vector,dims,cut)) {
      out << "pahvec : mismatch dimension in " << inlib::sout(VNAME) << std::endl;
      return;
    }

    histogram->reset();

    const std::vector<double>& v = vector->vector();
    //unsigned int vsize = v.size();

    int binn = histogram->axis().bins();
    for(int bini = 0;bini<binn;bini++) {
      int index = bini;
      if(vector->accept(index,cut)) {
        double x = histogram->axis().bin_lower_edge(bini);
        histogram->fill(x,v[index]);
      }
    }

  } else {
    out << "pahvec : " << cmd_path << " : dummy." << std::endl;
  }
}
