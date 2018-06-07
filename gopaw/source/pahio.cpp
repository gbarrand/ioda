// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pahio_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <exlib/KUIP/tools>
#include "find_object"

#include <inlib/rroot/rall>

namespace gopaw {

inline void read_key(inlib::rroot::file& a_file,inlib::rroot::key& a_key,
		     gopaw::session& a_session,const std::map<std::string,std::string>& a_annotations) {
  inlib::histo::h1d* h1d = inlib::rroot::key_to_h1d(a_file,a_key,false);
  if(h1d) {
    h1d->set_annotations(a_annotations);
    //a_file.out() << "debug : pahio : annotations :" << std::endl;
    //h1d->hprint_annotations(a_file.out());
    a_session.remove_handle(a_key.object_name());
    a_session.add_handle(a_key.object_name(),h1d);
  }
  inlib::histo::h2d* h2d = inlib::rroot::key_to_h2d(a_file,a_key,false);
  if(h2d) {
    h2d->set_annotations(a_annotations);
    //a_file.out() << "debug : pahio : annotations :" << std::endl;
    //h2d->hprint_annotations(a_file.out());
    a_session.remove_handle(a_key.object_name());
    a_session.add_handle(a_key.object_name(),h2d);
  }
}

}
  
#include <inlib/wroot/to>
#include "root_aida"

namespace gopaw {

inline bool write_handle(std::ostream& a_out,inlib::wroot::directory& a_dir,const named_handle& a_named_handle) {
  inlib::base_handle* handle = a_named_handle.second;
  if(handle->object_class()==inlib::histo::h1d::s_class()) {
    inlib::histo::h1d* histo = (inlib::histo::h1d*)handle->object();
    if(!inlib::wroot::to(a_dir,*histo,a_named_handle.first)) {
      a_out << "pahio : can't write " << inlib::sout(a_named_handle.first) << std::endl;
      return false;
    }
    if(histo->annotations().size()) {
      if(!inlib::wroot::to(a_dir,histo->annotations(),a_named_handle.first)) {
        a_out << "pahio : can't write annotations for " << inlib::sout(a_named_handle.first) << std::endl;
        return false;
      }
    }
  } else if(handle->object_class()==inlib::histo::h2d::s_class()) {
    inlib::histo::h2d* histo = (inlib::histo::h2d*)handle->object();
    if(!inlib::wroot::to(a_dir,*histo,a_named_handle.first)) {
      a_out << "pahio : can't write " << inlib::sout(a_named_handle.first) << std::endl;
      return false;
    }
    if(histo->annotations().size()) {
      if(!inlib::wroot::to(a_dir,histo->annotations(),a_named_handle.first)) {
        a_out << "pahio : can't write annotations for " << inlib::sout(a_named_handle.first) << std::endl;
        return false;
      }
    }
  } else if(handle->object_class()==inlib::aida::ntuple::s_class()) {

    inlib::aida::ntuple* aida_tuple = (inlib::aida::ntuple*)handle->object();
    inlib::wroot::ntuple* root_tuple;
    aida2root(a_out,*aida_tuple,a_dir,a_named_handle.first,root_tuple);

  } else if(handle->object_class()==inlib::rroot::file::s_class()) {
  } else if(handle->object_class()==inlib::wroot::file::s_class()) {
  } else {
    a_out << "pahio : can't write " << inlib::sout(a_named_handle.first)
          << ", unknown class " << inlib::sout(handle->object_class()) << "."
          << std::endl;
  }
  return true;
}

}

void pahio_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pahio : execute " << inlib::sout(cmd_path) << std::endl;
  }

  if(cmd_path=="/HISTOGRAM/HIO/PRINT") {

    std::string ID = ku_gets();
    /*std::string CHOPT = */ku_gets();
 
    inlib::histo::h1d* h1d = h1d_find(_sess,ID);
    inlib::histo::h2d* h2d = h2d_find(_sess,ID);
    if(!h1d && !h2d) {
      out << "pahio : can't find histogram " << inlib::sout(ID) << std::endl;
      return;
    }

    if(h1d) {
      h1d->hprint(_sess.out());
      h1d->hprint_annotations(_sess.out());
    }
    if(h2d) {
      h2d->hprint(_sess.out());
      h2d->hprint_annotations(_sess.out());
    }

  } else if(cmd_path=="/HISTOGRAM/HIO/HRIN") {

    std::string ID = ku_gets();
    /*int ICYCLE =*/ ku_geti();
    /*int IOFFSET =*/ ku_geti();

    inlib::rroot::file* root_file;
    inlib::rroot::directory* root_dir;
    if(!_sess.find_current_rroot_file(root_file,root_dir)) {
      out << "pahio : current file not found." << std::endl;
      return;
    }

    const std::vector<inlib::rroot::key*>& keys = root_dir->keys();

    if(ID=="0") ID.clear(); //read all.
    if(ID=="*") ID.clear(); //read all.

    if(ID.empty()) {
      inlib_vforcit(inlib::rroot::key*,keys,it) {
        std::map<std::string,std::string> annotations;
        inlib_vforcit(inlib::rroot::key*,keys,ita) {
          if((*ita)->object_name()=="annotations_"+(*it)->object_name()) {
            if(!inlib::rroot::key_to_annotations(*root_file,*(*ita),annotations,false)){}
          }
        }
        read_key(*root_file,*(*it),_sess,annotations);
      }
    } else {
      inlib_vforcit(inlib::rroot::key*,keys,it) {
        if((*it)->object_name()==ID) {
          std::map<std::string,std::string> annotations;
          inlib_vforcit(inlib::rroot::key*,keys,ita) {
            if((*ita)->object_name()=="annotations_"+ID) {
              if(!inlib::rroot::key_to_annotations(*root_file,*(*ita),annotations,false)){}
            }
          }
          read_key(*root_file,*(*it),_sess,annotations);
          break;
        }
      }
    }

  } else if(cmd_path=="/HISTOGRAM/HIO/HROUT") {

    std::string ID = ku_gets();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pahio :" 
          << " " << cmd_path
          << " ID= " << inlib::sout(ID) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    inlib::wroot::file* root_file;
    if(!_sess.find_current_wroot_file(root_file)) {
      out << "pahio : current file not found." << std::endl;
      return;
    }

    inlib::wroot::directory& dir = root_file->dir();

    if(ID=="0") ID.clear(); //write all.
    if(ID=="*") ID.clear(); //write all.

    if(ID.empty()) {
      inlib_vforcit(gopaw::named_handle,_sess.named_handles(),it) {
        if(!gopaw::write_handle(out,dir,*it)) {}
      }
    } else {
      inlib_vforcit(gopaw::named_handle,_sess.named_handles(),it) {
        if(ID==(*it).first) {
          if(!gopaw::write_handle(out,dir,*it)) {}
          break;
        }
      }
    }

   {unsigned int n;
    if(!root_file->write(n)) {
      out << "pahio : file write failed." << std::endl;
    }}
  
  } else {
    out << "pahio : " << cmd_path << " : dummy." << std::endl;

  }
}
