// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pazrz_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"
#include <exlib/KUIP/tools>
#include "find_object"

#include <inlib/rroot/file>
#include <inlib/rroot/rall>
#include <inlib/sjust>

#ifdef APP_USE_HDF5
#include <exlib/hdf5/ls_visitor>
#endif

void pazrz_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pazrz : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/ZEBRA/RZ/CDIR") {

    std::string CHPATH = ku_gets();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pazrz : " << cmd_path
          << " CHPATH= " << inlib::sout(CHPATH) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    //inlib::touppercase(CHOPT);
    //std::vector<std::string> OPTS;
    //exlib::KUIP::get_opts("CHOPT",OPTS);

    if(CHPATH.empty()) { //pwd
      gopaw::LUN* _lun;
      if(!_sess.find_current_LUN(_lun)) {
        out << "pazrz : " << cmd_path << " : no current unit." << std::endl;
        return;
      }
      if(_lun->m_rroot_file) {
        out << "pazrz : current working rroot directory = //LUN" << _sess.current_unit()
	    << "/" << std::endl;
        return;
      }
      if(_lun->m_wroot_file) {
        out << "pazrz : current working wroot directory = //LUN" << _sess.current_unit()
	    << "/" << std::endl;
        return;
      }
#ifdef APP_USE_HDF5
      if(_lun->m_hdf5_file) {
        out << "pazrz : current working hdf5 directory = //LUN" << _sess.current_unit()
	    << "/" << std::endl;
        return;
      }
#endif      
#ifdef APP_USE_CFITSIO
      if(_lun->m_fits_file) {
        out << "pazrz : current working hdf5 directory = //LUN" << _sess.current_unit()
	    << "/" << std::endl;
        return;
      }
#endif      
      return;
    } 

    bool is_pawc;
    std::string path;
    if(!gopaw::is_pawc_path(CHPATH,is_pawc,path)) {
      out << "pazrz : " << cmd_path << " : syntax error in " << inlib::sout(CHPATH) << std::endl;
      return;
    }
    if(is_pawc) {
      //mem_tree->cd(path); //NOTE path does not start with "/"
      return;
    }

    bool is_unit;
    int unit;
    if(!gopaw::is_unit_path(CHPATH,is_unit,path,unit)) {
      out << "pazrz : " << cmd_path <<  " : syntax error in " << inlib::sout(CHPATH) << std::endl;
      return;
    }
    if(is_unit) {
      if(_sess.verbose_level()) out << "pazrz : " << cmd_path <<  " : is unit." << std::endl;
      gopaw::LUN* _lun;
      if(!_sess.find_LUN(unit,_lun)) {
        out << "pazrz : " << cmd_path << " : no unit " << unit << "." << std::endl;
        return;
      }
      if(_lun->m_rroot_file) {
        if(_sess.verbose_level()) out << "pazrz : " << cmd_path <<  " : is rroot unit." << std::endl;
        if(path.empty()) {
          if(_lun->m_rroot_cur_dir_owner) delete _lun->m_rroot_cur_dir;
          _lun->m_rroot_cur_dir = &(_lun->m_rroot_file->dir());
          _lun->m_rroot_cur_dir_owner = false;
	} else {
          inlib::rroot::directory* dir = inlib::rroot::find_path_dir(_lun->m_rroot_file->dir(),path);
          if(!dir) {
            out << "pazrz : " << cmd_path << " : directory not found for path " << inlib::sout(path)
  	      << " in unit " << unit << "." << std::endl;
            return;
          }
          if(_sess.verbose_level())
            out << "pazrz : " << cmd_path << " : directory found for path " << inlib::sout(path)
                << " in unit " << unit << "." << std::endl;
          if(_lun->m_rroot_cur_dir_owner) delete _lun->m_rroot_cur_dir;
          _lun->m_rroot_cur_dir = dir;
          _lun->m_rroot_cur_dir_owner = true;
        }
        _sess.set_current_unit(unit);
        return;
      }
#ifdef APP_USE_HDF5
      if(_lun->m_hdf5_file) {
        if(_sess.verbose_level()) out << "pazrz : " << cmd_path <<  " : is hdf5 unit." << std::endl;
        if(path.empty()) {
	  if(_lun->m_hdf5_cur_dir_owner) ::H5Gclose(_lun->m_hdf5_cur_dir);
          _lun->m_hdf5_cur_dir = _lun->m_hdf5_file;
          _lun->m_hdf5_cur_dir_owner = false;
	} else {
          hid_t dir = exlib_H5Gopen(_lun->m_hdf5_file,path.c_str());
          if(dir<0) {
            out << "pazrz : " << cmd_path << " : directory not found for path " << inlib::sout(path)
  	      << " in unit " << unit << " (hdf5)." << std::endl;
            return;
          }
          if(_sess.verbose_level())
            out << "pazrz : " << cmd_path << " : directory found for path " << inlib::sout(path)
                << " in unit " << unit << "." << std::endl;
	  if(_lun->m_hdf5_cur_dir_owner) ::H5Gclose(_lun->m_hdf5_cur_dir);
          _lun->m_hdf5_cur_dir = dir;
          _lun->m_hdf5_cur_dir_owner = true;
        }
        _sess.set_current_unit(unit);
        return;
      }
#endif      
#ifdef APP_USE_CFITSIO
      if(_lun->m_fits_file) {
        if(_sess.verbose_level()) out << "pazrz : " << cmd_path <<  " : is fits unit." << std::endl;
        out << "pazrz : note : there is no directory in a fits file." << std::endl;
        _sess.set_current_unit(unit);
        return;
      }
#endif      
      if(_lun->m_wroot_file) {
        if(_sess.verbose_level()) out << "pazrz : " << cmd_path <<  " : is wroot unit : dummy." << std::endl;
        //inlib::wroot::directory* dir = inlib::wroot::find_path_dir();
        return;
      }
      return;
    } 

    //mem_tree->cd(CHPATH);

  } else if(cmd_path=="/ZEBRA/RZ/MDIR") {

    std::string CHDIR = ku_gets();
    /*int NWKEY =*/ ku_geti();
    /*std::string CHFORM =*/ ku_gets();
    /*std::string CHTAGS =*/ ku_gets();

    out << "pazrz : " << cmd_path << " dummy." << std::endl;
    return;

    /*
    AIDA::ITree* tree = _sess.findCurrentTree();
    if(!tree) {
      out << "pazrz : no current tree." << std::endl;
      return;
    }
    tree->mkdir(CHDIR);
    */

  } else if(cmd_path=="/ZEBRA/RZ/LDIR") {

    std::string CHPATH = ku_gets();
    std::string CHOPT = ku_gets();

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    if(CHPATH=="//") { // List units :
      std::string s = "//PAWC";
      inlib::justify(s,18,inlib::side_left);
      out << " " << s << "memory" << std::endl; 

      const gopaw::LUNs_t& luns = _sess.LUNs();
      gopaw::LUNs_t::const_iterator it;
      for(it=luns.begin();it!=luns.end();++it) {
        std::string s;
        inlib::sprintf(s,32,"//LUN%d",(*it).first);
        inlib::justify(s,18,inlib::side_left);
        out << " " << s << inlib::sout((*it).second.m_string) << std::endl; 
      }

    } else {
     {inlib::rroot::file* root_file;
      inlib::rroot::directory* root_dir;
      if(_sess.find_current_rroot_file(root_file,root_dir)) {
        //FIXME : start from CPATH.
        const std::vector<inlib::rroot::key*>& keys = root_dir->keys();
        bool recursive = gopaw::session::is_an_option(OPTS,CHOPT,"T");
        inlib::rroot::read(out,*root_file,keys,recursive,true,false,0);
	return;
      }}
     
#ifdef APP_USE_HDF5
     {hid_t hdf5_file = (-1);
      hid_t hdf5_dir = (-1);
      if(_sess.find_current_hdf5_file(hdf5_file,hdf5_dir)) {
        if(hdf5_file!=(-1)) {
          exlib::hdf5::ls(out,hdf5_file);
          return;
        }
      }}
#endif //APP_USE_HDF5

#ifdef APP_USE_CFITSIO
     {exlib::cfitsio::file* fits_file = 0;
      if(_sess.find_current_fits_file(fits_file)) {
        if(fits_file) {
          typedef std::pair<unsigned int,std::string> id_t;
          std::vector<id_t> ids;
          if(!exlib::cfitsio::ls(out,*fits_file,ids)) {
            out << "pazrz : exlib::cfitsio::ls() failed." << std::endl;
            return;
	  }
	  inlib_vforcit(id_t,ids,it) {
	    out << "hdu " << (*it).first << ", type " << (*it).second << std::endl;
	  }
          return;
        }
      }}
#endif //APP_USE_CFITSIO

      out << "pazrz : current file not found." << std::endl;
      return;
    }

  } else {
    out << "pazrz : " << cmd_path << " : dummy." << std::endl;
  }
}
