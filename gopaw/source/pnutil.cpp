// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pnutil_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "find_object"

#include <inlib/sout>
#include <inlib/file>
#include <inlib/sys/atime>
#include <inlib/sjust>
#include <inlib/tos_deprec>

#include <cstdio>

namespace gopaw {

inline std::string sjust(const std::string& a_string,unsigned int a_size,inlib::side a_side = inlib::side_left){
  std::string s(a_string); 
  inlib::justify(s,a_size,a_side);
  return s;
}

inline void hprint(inlib::aida::ntuple& a_tuple,const std::string& a_name,std::ostream& a_out){
//int field = 13; //HBOOK had 17.

  a_out << " ********************************************************" << std::endl;

  a_out << " * NTUPLE NAME= " << a_name << " ENTRIES= " << a_tuple.rows() << " TITLE= " << a_tuple.title() << std::endl;

  a_out << " ********************************************************" << std::endl;
  a_out << " *  Var numb  *   Name    *    Lower     *    Upper     *" << std::endl;
  a_out << " ********************************************************" << std::endl;

  std::vector<std::string> names;
  a_tuple.column_names(names);
  double mn,mx;
    
  int coln = a_tuple.columns();
  for(int coli=0;coli<coln;coli++) {
    inlib::aida::column_min<double>(a_tuple,coli,mn);
    inlib::aida::column_max<double>(a_tuple,coli,mx);

    a_out << " *" 
          << sjust(inlib::tos(coli+1),12,inlib::side_middle) 
          << "* " 
          << sjust(names[coli],10) 
          << "* " 
          << sjust(inlib::tos(mn),13) 
          << "* " 
          << sjust(inlib::tos(mx),13) 
          << "*" 
          << std::endl;
  }

  a_out << " ********************************************************" << std::endl;

/*

 ********************************************************
 * NTUPLE ID=   10  ENTRIES=   3354   CERN Population 
 ********************************************************
 *  Var numb  *   Name    *    Lower     *    Upper     *
 ********************************************************
 *      1     * CATEGORY  * 0.102000E+03 * 0.567000E+03 *
 *      2     * DIVISION  * 0.100000E+01 * 0.130000E+02 *
 *      3     * FLAG      * 0.000000E+00 * 0.310000E+02 *
 *      4     * AGE       * 0.210000E+02 * 0.640000E+02 *
 *      5     * SERVICE   * 0.000000E+00 * 0.350000E+02 *
 *      6     * CHILDREN  * 0.000000E+00 * 0.600000E+01 *
 *      7     * GRADE     * 0.300000E+01 * 0.140000E+02 *
 *      8     * STEP      * 0.000000E+00 * 0.150000E+02 *
 *      9     * NATION    * 0.100000E+01 * 0.150000E+02 *
 *     10     * HRWEEK    * 0.200000E+01 * 0.440000E+02 *
 *     11     * COST      * 0.686000E+03 * 0.188530E+05 *
 ********************************************************

*/

  //hprint(a_tuple.annotation(),a_out);
}

}
  
void pnutil_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pnutil : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/NTUPLE/CREATE") {

    std::string IDN = ku_gets();
    std::string TITLE = ku_gets();
    /*int NVAR =*/ ku_geti();
    std::string CHRZPA = ku_gets();
    /*int NPRIME =*/ ku_geti();
    std::string VARLIST = ku_gets();
 
    _sess.remove_handle(IDN);
    //if(memoryTree->find(IDN)) {
    // memoryTree->rm(IDN); //ok.
    //  out << " *** WARNING : already existing tuple replaced : ID=" << IDN << std::endl;
    //}

    inlib::aida::ntuple* tuple = new inlib::aida::ntuple(out,TITLE);
    if(!tuple) {
      out << "pnutil : can't create tuple " << inlib::sout(IDN) << std::endl;
      return;
    }
    _sess.add_handle(IDN,tuple);

    std::vector<std::string> cols;
    inlib::words(VARLIST,",",false,cols);
    inlib_vforcit(std::string,cols,it) tuple->create_col<double>(*it);

  } else if(cmd_path=="/NTUPLE/READ") {

    std::string IDN = ku_gets();
    std::string FNAME = ku_gets();
    std::string FORMAT = ku_gets();
    std::string OPT = ku_gets();
    /*int NEVENT =*/ ku_geti();
    std::string MATCH = ku_gets();
 
    if(_sess.verbose_level()) {
      out << "pnutil : " << cmd_path
          << " IDN= " << inlib::sout(IDN) 
          << " FNAME= " << inlib::sout(FNAME) 
          << " FORMAT= " << inlib::sout(FORMAT) 
          << " OPT= " << inlib::sout(OPT) 
          << " MATCH= " << inlib::sout(MATCH) 
          << std::endl;
    }

    inlib::aida::ntuple* tuple = tuple_find(_sess,IDN);
    if(!tuple) {
      out << "pnutil : can't find tuple " << inlib::sout(IDN) << "." << std::endl;
      return;
    }

    std::string sfile;
    inlib::file_name(FNAME,sfile); //to expand environment variables.

    if(_sess.verbose_level()) {
      out << "pnutil : " << cmd_path << " file = " << inlib::sout(sfile) << std::endl;
    }

    FILE* file = ::fopen(sfile.c_str(),"rb");
    if(!file) {
      out << "pnutil :"
	  << " can't open file " << inlib::sout(sfile)
	  << ". FNAME is " << inlib::sout(FNAME)
	  << std::endl;
      return;
    }

    const std::vector<inlib::aida::base_col*>& cols = tuple->cols();

    std::vector<double> row(tuple->columns());
    const unsigned int BUFSIZE = 65536;
    char buffer[BUFSIZE+1];
    inlib::atime begin = inlib::atime::now();
    while(true) {
      size_t l;
      if(!inlib::file::read_cstring(file,buffer,BUFSIZE,l)) break; // EOF.
      inlib::cstring_strtods(buffer,row,true); //the last true is important.

      size_t index = 0;
      inlib_vforcit(inlib::aida::base_col*,cols,it) {
	inlib::aida::aida_col<double>* _col = (inlib::aida::aida_col<double>*)(*it);
        _col->fill(row[index]);index++;
      }

      if(!tuple->add_row()) {
        out << "pnutil : problem filling tuple from file " << inlib::sout(FNAME) << "." << std::endl;
        break;
      }
    }
    inlib::atime end = inlib::atime::elapsed(begin);
    ::fclose(file);

    out << " ==>   " << tuple->rows() << " events have been read" 
        << " in " << end.seconds() << " s (elapsed)."
        << std::endl;

  } else if(cmd_path=="/NTUPLE/PRINT") {

    std::string IDN = ku_gets();
 
    inlib::aida::ntuple* tuple = tuple_find(_sess,IDN);
    if(!tuple) {
      out << "pnutil : object " << inlib::sout(IDN) << " not found." << std::endl;
      return;
    }

    gopaw::hprint(*tuple,IDN,out);

  } else {
    out << "pnutil : " << cmd_path << " : dummy." << std::endl;
  }
}
