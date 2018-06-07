// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void hisdef_();
  void fundef_();
  void ntudef_();
  void gradef_();
  void picdef_();
  void obsdef_();
  void zebdef_();
  void fordef_();
  void netdef_();
  void pvedef_();

  // VECTOR :
  void vecdef_();
  int vector_create_C(void*);
  int vector_delete_C(void*);
  int vector_read_C(void*);
  int vector_write_C(void*);
  int vector_input_C(void*);
  int vector_list_C(void*);
  int vector_operation_C(void*);
  int vector_print_C(void*);
  int vector_find_C(const char*,void*);
  int vector_value_C(const char*,double*,void*);

  //SIGMA :
  int sigma_C(const char*,void*);

  //dummies :
  void bugreprt_(void*);
  void mergin_(void*);
  void pafeyn_(void*);
  void pahfit_(void*);
  void palint_(void*);
  void paobs_(void*);
  void papiaf_(void*);
  void pavercmd_(void*);
  void pawork_(void*);
  void pawuwf_(void*);
  void pazdz_(void*);
  void pazfz_(void*);
  void pchain_(void*);
  void pvmem_(void*);
  void qpflags_(void*);
  void rlogin_(void*);
  void rshell_(void*);
  void pahelp_(void*);

  void gopaw_kuip_def_();

  void kuinit_(int);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>
#include <kuip/kout.h>

extern "C" {
  void kufdef_(const char*,const char*,const char*,const char*,SUBRPTR,int,int,int,int);
  void kxdelfuncs();
}

#include <inlib/tos_deprec>
#include <inlib/vmanip>
#include <inlib/fstring>
#include <inlib/sjust>
#include <inlib/get_lines>

#include "../gopaw/version"

#include "session"

extern "C" {
  void gopaw_initialize(std::ostream&,unsigned int,gopaw::session&,gopaw::iui&,bool);
  void gopaw_finalize();
  bool gopaw_kuip_exec(std::ostream&,const std::string&);
  bool gopaw_kuip_exec_no_help(std::ostream&,const std::string&);
  
  inline int gopaw_kuip_disp_flush_C(void* a_UI){
    //::printf("debug : gopaw_kuip_disp_flush\n");
    //iui* ui = (iui*)a_UI;
    //ui->synchronize();
    return 0;
  }
  
  inline int gopaw_kuip_disp_exit_C(void* a_UI){
    gopaw::iui* ui = (gopaw::iui*)a_UI;
    ui->exit();
    return 0;
  }

  inline char* gopaw_kuip_disp_prompt_C(char* a_prompt,void* a_UI){
    // KUIP free the return string ; need to allocate one.
    gopaw::iui* ui = (gopaw::iui*)a_UI;
    std::string value;
    if(!ui->ask(a_prompt,value)){}
#ifdef INLIB_MEM
    return inlib::str_dup(value.c_str(),false);
#else    
    return inlib::str_dup(value.c_str());
#endif    
  }

  // used in ourex/KUIP/source/kedit.c and kuwhat.c :
  void kgl_setwidth() {/*::printf("debug : inex : kgl_setwidth : dummy\n");*/}
  void kgl_config(const char*,int) {/*::printf("debug : inex : kgl_config : dummy\n");*/}
  char* kgl_getline(const char*) {::printf("debug : inex : kgl_getline : dummy\n");return 0;}
  void kgl_histadd(const char*) {::printf("debug : inex : kgl_histadd : dummy\n");}

  void put_va(void* a_tag,char* a_format,va_list a_args) {
    gopaw::session& _sess = *((gopaw::session*)a_tag);
    std::string s;
    inlib::vsprintf(s,1024,a_format,a_args);
    _sess.out() << s;
  }
}

static void gopaw_HPLQSE(void* a_CHOPT,float* a_R,int a_len_CHOPT,void* a_tag) {
  // Fortran signature is :
  //      SUBROUTINE HPLQSE(CHOPT,R)
  //      CHARACTER*(*) CHOPT
  //      CHARACTER*8   CHOPTN
  gopaw::session& _sess = *((gopaw::session*)a_tag);
  gopaw::atts_t& atts = _sess.atts();

  inlib::fstring CHOPT(a_CHOPT,a_len_CHOPT);
  std::string sCHOPT;
  CHOPT.std_string(sCHOPT);
  if(!sCHOPT.size()) return;
  inlib::strip(sCHOPT);
  if(sCHOPT[0]=='?') {
    *a_R = (float)atts.value(sCHOPT.substr(1,sCHOPT.size()-1)); 
  } else {
    atts.set_value(sCHOPT,(double)*a_R);
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void gopaw_initialize(std::ostream& a_out,unsigned int a_verbose,gopaw::session& a_session,gopaw::iui& a_ui,bool a_silent) {
  if(a_silent) a_session.set_warn_metaf_ps();
  
  int NWORDS=15000;
  ::kuinit_(NWORDS);
  ::ku_exec("/KUIP/SET_SHOW/RECALL_STYLE 'NONE'"); //ifdef _WIN32
  ::ku_exec("/KUIP/SET_SHOW/BREAK OFF");
  ::ku_action_data(&a_session); 

  hisdef_();
  fundef_();
  ntudef_();
  gradef_();
  picdef_();
  obsdef_();
  zebdef_();
  fordef_();
  netdef_();
  pvedef_();

  // Getline :
  ku_xgl_setwidth(kgl_setwidth);
  ku_xgl_config(kgl_config);
  ku_xgl_getline(kgl_getline);
  ku_xgl_histadd(kgl_histadd);
  
  ::k_set_out_func(put_va,&a_session);

  // VECTOR :
  vecdef_();
  ku_xvcreate(vector_create_C,&a_session); 
  ku_xvdelete(vector_delete_C,&a_session); 
  ku_xvfind(vector_find_C,&a_session); 
  ku_xvvalue(vector_value_C,&a_session); 

  ku_xvread(vector_read_C,&a_session); 
  ku_xvwrite(vector_write_C,&a_session); 
  ku_xvinput(vector_input_C,&a_session); 
  ku_xvlist(vector_list_C,&a_session); 
  ku_xvoperation(vector_operation_C,&a_session); 
  ku_xvprint(vector_print_C,&a_session); 

  //SIGMA 
  ku_sigm(sigma_C,&a_session);

  gopaw_kuip_def_();
  
  //PAW KUFDEFs found in CERNLIB/src/pawlib/paw/code/pawdef.F.
  std::string arg1("$GRAFINFO('attr')");
  std::string arg2("HPLOT/HIGZ attributes (see HELP SET for valid names)");
  std::string arg3("x=$GRAFINFO(u)");
  std::string arg4("-=HPLQSE(u,x)");
  kufdef_(arg1.c_str(),arg2.c_str(),arg3.c_str(),arg4.c_str(),
          (SUBRPTR)gopaw_HPLQSE,
          arg1.size(),arg2.size(),arg3.size(),arg4.size());

  std::string line_version("Version ");
  line_version += std::string(GOPAW_VERSION);
  inlib::justify(line_version,52,inlib::side_middle);

  if(!a_silent) {
  //        0        1         2         3         4         5   
  //        1234567890123456789012345678901234567890123456789012
  a_out << " ******************************************************"<<std::endl;
  a_out << " *                                                    *"<<std::endl;
  a_out << " *    W E L C O M E  to  G O P A W                    *"<<std::endl;
  a_out << " *                                                    *"<<std::endl;
  a_out << " *" << line_version                              << "*"<<std::endl;
  a_out << " *                                                    *"<<std::endl;
  a_out << " ******************************************************"<<std::endl;
  }
  
  ::ku_prompt(gopaw_kuip_disp_prompt_C,&a_ui); 
  ::ku_flush(gopaw_kuip_disp_flush_C,&a_ui); 
  ::ku_exit(gopaw_kuip_disp_exit_C,&a_ui); 
  ::ku_quit(gopaw_kuip_disp_exit_C,&a_ui);

  inlib::sg::plots* _page = a_session.find_plots(gopaw::s_current());
  if(_page) {
    //page_set_title(*_page,"");
    a_session.set_plots_layout(*_page);
    a_session.set_plots_zone(*_page);
  }
}

void gopaw_finalize() {
  ::k_set_out_func(0,0);
  ::kxdelfuncs();
}

bool gopaw_kuip_exec(std::ostream& a_out,const std::string& a_string){
  //NOTE : if looping, have to take into account a ::ku_stop() from the GUI.
  //std::vector<std::string> text;
  //inlib::words(a_string,"\\n\\",false,text);
  //for(unsigned int index=0;index<text.size();index++) {
  //  if(::ku_exec((char*)text[index].c_str())) return false;
  //}
  //return true;
  if(a_string.empty()) return true;
  //int status =
  ::ku_exec(a_string.c_str());
  //if(status!=0) { //status not the same on Macos, Windows, Android.
    //a_out << "gopaw_kuip_exec : ku_exec(" << inlib::sout(a_string) << ")a. status " << status << "." << std::endl;
    //return false;
  //}
  return true;
}

bool gopaw_kuip_exec_no_help(std::ostream& a_out,const std::string& a_string){
  //NOTE : if looping, have to take into account a ::ku_stop() from the GUI.
  //std::vector<std::string> text;
  //inlib::words(a_string,"\\n\\",false,text);
  //for(unsigned int index=0;index<text.size();index++) {
  //  if(::ku_exec_no_help((char*)text[index].c_str())) return false; //if kuip_disp_prompt_C() not implemented.
  //}
  //return true;
  //int status =
  if(a_string.empty()) return true;
  ::ku_exec_no_help(a_string.c_str());
  //if(status!=0) { //status not the same on Macos, Windows, Android.
  //  a_out << "gopaw_kuip_exec : ku_exec_no_help(" << inlib::sout(a_string) << "). status " << status << "." << std::endl;
  //  return false;
  //}
  return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void dummy(const std::string& a_func,void* a_tag) {
  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(!a_tag) {
    // With : 
    //    OPAW> HELP IGSET
    // we pass here with a null tag !
    ::printf("%s : %s : dummy.\n",a_func.c_str(),cmd_path.c_str());
  } else {
    gopaw::session& _sess = *((gopaw::session*)a_tag);
    _sess.out() << a_func << " : " << cmd_path << " : dummy." << std::endl; 
  }
}
void bugreprt_(void* a_tag) { dummy("bugreprt",a_tag);}
void mergin_(void* a_tag) { dummy("mergin",a_tag);}
void pafeyn_(void* a_tag) { dummy("pafeyn",a_tag);}
void pahfit_(void* a_tag) { dummy("pahfit",a_tag);}
void palint_(void* a_tag) { dummy("palint",a_tag);}
void paobs_(void* a_tag) { dummy("paobs",a_tag);}
void papiaf_(void* a_tag) { dummy("papiaf",a_tag);}
void pavercmd_(void* a_tag) { dummy("pavercmd",a_tag);}
void pawork_(void* a_tag) { dummy("pawork",a_tag);}
void pawuwf_(void* a_tag) { dummy("pawuwf",a_tag);}
void pazdz_(void* a_tag) { dummy("pazdz",a_tag);}
void pazfz_(void* a_tag) { dummy("pazfz",a_tag);}
void pchain_(void* a_tag) { dummy("pchain",a_tag);}
void pvmem_(void* a_tag) { dummy("pvmem",a_tag);}
void qpflags_(void* a_tag) { dummy("qpflags",a_tag);}
void rlogin_(void* a_tag) { dummy("rlogin",a_tag);}
void rshell_(void* a_tag) { dummy("rshell",a_tag);}

#include "pahelp.cpp"
#include "vector_C.cpp"
#include "pagpri.cpp"
#include "pahbok.cpp"
#include "panmas.cpp"
#include "pazrz.cpp"
#include "pnutil.cpp"
#include "papict.cpp"
#include "pafort.cpp"
#include "paghpl.cpp"
#include "pagksa.cpp"
#include "pasigm.cpp"
#include "pancut.cpp"
#include "pagraf.cpp"
#include "pafitv.cpp"
#include "pafith.cpp"
#include "pahope.cpp"
#include "pahset.cpp"
#include "pahio.cpp"
#include "pahvec.cpp"
#include "panntu.cpp"
#include "pafunc.cpp"
#include "pahist.cpp"
#include "pavect.cpp"
#include "gopaw_kuip.cpp"
