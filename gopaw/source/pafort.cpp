// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pafort_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "find_object"

#include <inlib/dirmanip>
#include <inlib/sjust>
#include <inlib/forit>

void pafort_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pafort : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/FORTRAN/FILE") {

    int iLUN = ku_geti();
    std::string FNAME = ku_gets();
    std::string STATUS = ku_gets();

    if(_sess.verbose_level()) {
      out << "pafort : " << cmd_path 
          << " iLUN= " << iLUN
          << " FNAME = " << inlib::sout(FNAME) 
          << " STATUS = " << inlib::sout(STATUS) 
          << std::endl;
    }

    unsigned int uLUN;
    if(iLUN==0) { // If LUN is 0 the next free logical unit will be used.
      if(!_sess.mx_LUN(uLUN)) uLUN = 1;
    } else {
      uLUN = (unsigned int)iLUN;
    }

    if(_sess.find_LUN(uLUN)) {
      out << "pafort : LUN " << uLUN << " already in used by gopaw." << std::endl;
      return;
    }

    //OLD        Open existing file for reading.
    //APPEND     Open existing file and position at EOF.
    //NEW        Create new file; error if already existing.
    //UNKNOWN    Open existing or create new file.
    //DONTKNOW   Like UNKNOWN except on VMS opens highest cycle.

    std::string sfile;
    inlib::file_name(FNAME,sfile); //to expand environment variables.

    // if running from ioda, so that ~/Documents/ioda be created :
   {std::string _dir = inlib::dir_name(sfile);
    if(_dir.size()) {
      if(!inlib::check_dir(out,_dir)) {
        out << "pafort : check_dir " << inlib::sout(_dir) << " failed." << std::endl;
        return;
      }
    }}

    if(_sess.verbose_level()) {
      out << "pafort : file " << inlib::sout(sfile) << ". FNAME is " << inlib::sout(FNAME) << "." << std::endl;
    }

    FILE* file = ::fopen(sfile.c_str(),"wb");
    if(!file) {
      out << "pafort : can't open " << inlib::sout(sfile) << " in wb mode." << std::endl;
      return;
    }

    _sess.add_LUN_SYS_FILE(uLUN,sfile,file);

  } else if(cmd_path=="/FORTRAN/CLOSE") {

    int iLUN = ku_geti();

    if(_sess.verbose_level()) {
      out << "pafort : " << cmd_path 
          << " iLUN= " << iLUN
          << std::endl;
    }

    unsigned int uLUN = iLUN<0?-iLUN:iLUN;

    if(!uLUN) {
      _sess.close_LUNs();
    } else {
      gopaw::LUN* _lun;
      if(!_sess.find_LUN(uLUN,_lun)) {
        out << "pafort : " << cmd_path << " : unknown unit " << uLUN << std::endl;
        return;
      }
      _sess.close_LUN(*_lun);
      _sess.remove_LUN(uLUN);

      if(_sess.verbose_level()) {
        out << "pafort : LUN " << uLUN << " closed." << std::endl;
      }
    }
      
  } else if(cmd_path=="/FORTRAN/UNITS") {

    const gopaw::LUNs_t& _LUNs = _sess.LUNs();

    std::vector<std::string> vus; //unit id.
    std::string _su("Lu");
    std::string::size_type usmx = _su.size();
   {std::ostringstream strm;
   {inlib_mforcit(unsigned int,gopaw::LUN,_LUNs,it) {
      strm.str("");
      strm << (*it).first;
      vus.push_back(strm.str());
    }}
   {inlib_vforcit(std::string,vus,it) usmx = inlib::mx(usmx,(*it).size());}
   {inlib_vforit(std::string,vus,it) inlib::justify(*it,usmx,inlib::side_left);}}

    std::vector<std::string> vfs; //file names.
    std::string _sf("File name");
    std::string::size_type fsmx = _sf.size();
  {{inlib_mforcit(unsigned int,gopaw::LUN,_LUNs,it) vfs.push_back((*it).second.m_string);}
   {inlib_vforcit(std::string,vfs,it) fsmx = inlib::mx(fsmx,(*it).size());}
   {inlib_vforit(std::string,vfs,it) inlib::justify(*it,fsmx,inlib::side_left);}}

    std::vector<std::string> vts; //unit type.
    std::string _st("File type");
    std::string::size_type tsmx = _st.size();
   {std::string _s;
   {inlib_mforcit(unsigned int,gopaw::LUN,_LUNs,it) {
      if(!gopaw::LUN::type_to_string((*it).second.m_type,_s)) {}
      vts.push_back(_s);
    }}
   {inlib_vforcit(std::string,vts,it) tsmx = inlib::mx(tsmx,(*it).size());}
   {inlib_vforit(std::string,vts,it) inlib::justify(*it,tsmx,inlib::side_left);}}

    std::string  su(usmx,'-');
    std::string  sf(fsmx,'-');
    std::string  st(tsmx,'-');
    out << " +-" << su << "-+-" << sf << "-+-" << st << "-+" << std::endl;
    
    inlib::justify(_su,usmx,inlib::side_left);
    inlib::justify(_sf,fsmx,inlib::side_left);
    inlib::justify(_st,tsmx,inlib::side_left);
    
    out << " | " << _su << " | " << _sf << " | " << _st << " |" << std::endl;

    out << " +-" << su << "-+-" << sf << "-+-" << st << "-+" << std::endl;

    size_t index = 0;
    inlib_mforcit(unsigned int,gopaw::LUN,_LUNs,it) {
      out << " | " << vus[index] << " | " << vfs[index] << " | " << vts[index] << " |" << std::endl;
      index++;
    }
    out << " +-" << su << "-+-" << sf << "-+-" << st << "-+" << std::endl;

  } else if(cmd_path=="/FORTRAN/CALL") {
    std::string UROUT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pafort : " << cmd_path 
          << " UROUT = " << inlib::sout(UROUT) 
          << std::endl;
    }

    // Extract name/parameters :
    std::string name;
    std::vector<std::string> sargs;
    if(!inlib::name_args(UROUT,name,sargs)) {
      out << "pafort :" 
          << " Syntax error in " << inlib::sout(UROUT) 
          << std::endl;
      return;
    }

    std::string sfunc = name;
    inlib::touppercase(sfunc);
    if(sfunc=="HFILL") {
      if(sargs.size()!=4) { 
        out << "pafort :" 
            << " For HFILL, 4 arguments are expected in " << inlib::sout(UROUT) 
            << ". Signature should be (I,R,R,R)"
            << std::endl;
        return;
      }
    /*int arg0;
      if(!inlib::to(sargs[0],arg0)) { //Syntax error.
        out << "pafort :" 
            << " Syntax error in " << inlib::sout(UROUT) 
            << std::endl;
        return;
      }*/
      std::string ID = sargs[0];

      double x;
      if(!inlib::to(sargs[1],x)) { //Syntax error.
        out << "pafort :" 
            << " Syntax error for x in " << inlib::sout(UROUT) 
            << std::endl;
        return;
      }
      double y;
      if(!inlib::to(sargs[2],y)) { //Syntax error.
        out << "pafort :" 
            << " Syntax error for y in " << inlib::sout(UROUT) 
            << std::endl;
        return;
      }
      double w;
      if(!inlib::to(sargs[3],w)) { //Syntax error.
        out << "pafort :" 
            << " Syntax error for w in " << inlib::sout(UROUT) 
            << std::endl;
        return;
      }

      inlib::histo::h1d* h1d = h1d_find(_sess,ID);
      if(h1d) {
        h1d->fill(x,w);
      } else {
        inlib::histo::h2d* h2d = h2d_find(_sess,ID);
        if(h2d) {
          h2d->fill(x,y,w);
        } else {
          out << "pafort : can't find histogram 1D or 2D " << inlib::sout(ID) << std::endl;
          return;
        }
      }
      return;
    }

    // On the fly compilation :
    if(sargs.size()!=1) { 
      out << "pafort : syntax error in " << inlib::sout(UROUT) << ". One argument expected." << std::endl;
      return;
    }

    std::string UFUNC = name;

    std::string funcName;
    _sess.gen_func_name(funcName);

    char* path = ku_macro_path();
    std::string paths = path?path:"";

    // retain_arrays false, so that the execution of the function can change the vectors.
    gopaw::base_func* func = create_function_vector(out,funcName,paths,UFUNC,_sess,false);
    if(!func) {
      out << "pafort : can't get function " << inlib::sout(UFUNC) << std::endl;
      return;
    }

    std::vector<double> fargs(sargs.size());
    for(size_t index=0;index<sargs.size();index++) {
      std::string NUMBER = sargs[index];
      double number;
      if(!inlib::to(NUMBER,number)) { //Syntax error.
        out << "pafort :" 
            << " Syntax error in " << inlib::sout(UROUT) 
            << std::endl;
        delete func;
        return;
      }
      fargs[index] = number;
    } 

    if(_sess.verbose_level()) {
      out << "pafort : " << cmd_path 
          << " UFUNC(" << inlib::sout(UFUNC) << ").point_value() ..."
          << std::endl;
    }
    
    func->point_value(fargs);

    delete func;

  } else {
    out << "pafort : " << cmd_path << " : dummy." << std::endl;
  }
}
