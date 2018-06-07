// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.


#ifdef INLIB_MEM
#include <inlib/mem>
#endif

#include "UI_offscreen"

#ifdef GOPAW_OFFSCREEN
#else
#ifdef _WIN32
#include "UI_Windows"
#else
#include "UI_X11"
#endif
#endif

#include "../gopaw/version"
#include "wrap_gopaw_help"

#include "session"

#include <inlib/args>

extern "C" {
  void gopaw_initialize(std::ostream&,unsigned int,gopaw::session&,gopaw::iui&,bool);
  void gopaw_finalize();
  bool gopaw_kuip_exec(std::ostream&,const std::string&);
  bool gopaw_kuip_exec_no_help(std::ostream&,const std::string&);
}

#ifdef APP_USE_PYTHON
#include <exlib/uzres>
#include <exlib/sg/view_Python>
extern "C" {void initinlib_swig_py();}
#include "session_py"
#include <inlib/S_STRING>
INLIB_GLOBAL_STRING(PYTHONHOME)
INLIB_GLOBAL_STRING(PYTHONPATH)
#endif

#include <inlib/sargs>
#include <inlib/saui>
#include <inlib/app>

#include <iostream>
#include <cstdlib>

inline void run_offscreen(std::ostream& a_out,unsigned int a_ww,unsigned int a_wh,unsigned int a_verbose,
			  const std::string& a_res_dir,const inlib::args& a_args) {
  gopaw::UI_offscreen ui(a_out,a_ww,a_wh); //before the below. gopaw_initialize() needs to find a valid IUI.
#ifdef APP_USE_PYTHON
  exlib::sg::view_Python view_Python(ui.viewer()); //it initializes Python.
  initinlib_swig_py();
#endif
  gopaw::session _sess(a_out,a_verbose,ui,a_res_dir);  
#ifdef APP_USE_PYTHON
  gopaw::session_py _sess_py(_sess);
#endif
  gopaw_initialize(a_out,a_verbose,_sess,ui,true); //true=silent
 {std::vector<std::string> kums;
  a_args.find("-kumac_file",kums);
  a_args.find("-kumac",kums,false);
  inlib_vforcit(std::string,kums,it) gopaw_kuip_exec_no_help(a_out,"/macro/exec "+*it);}
 {std::vector<std::string> cmds;
  a_args.find("-command",cmds);
  inlib_vforcit(std::string,cmds,it) gopaw_kuip_exec_no_help(a_out,*it);}
 {std::string file;
  if(a_args.find("-out",file)) {
    gopaw::iwidget* widget = ui.current_widget();
    if(widget) {
      std::string format;
      a_args.find("-format",format,"INZB_PNG");
      widget->write(file,format);
    }
  }}
}

int main(int argc,char** argv){
  if(argc<=0) {
    std::cout << "strange, argc<=0 !" << std::endl;
    return EXIT_SUCCESS;
  }

#ifdef INLIB_MEM
  inlib::mem::set_check_by_class(true);{
#endif //INLIB_MEM

  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  inlib::args args(argc,argv);

  if(args.is_arg(inlib::s_arg_version())) {
    std::cout << GOPAW_VERSION << std::endl;
    return EXIT_SUCCESS;
  }

  if(args.is_arg(inlib::s_arg_help())||args.is_arg("-h")) {
  /*
    unsigned int linen;
    const char** lines = gopaw_help(linen);
    for(unsigned int index=0;index<linen;index++) {
      std::string s(lines[index]);
      inlib::replace(s,"@@double_quote@@","\"");
      inlib::replace(s,"@@back_slash@@","\\");
      std::cout << s << std::endl;
    }
  */
    std::cout << " See the gopaw section at https://gbarrand.github.io." << std::endl;
    return EXIT_SUCCESS;
  }
  
 {std::vector<std::string> files;
  args.files(files);
  inlib_vforcit(std::string,files,it) {
    args.add("-kumac_file",*it,false);
    args.remove(*it);
  }}

  std::string arg0(argv[0]);

  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  std::string ui_file;
  std::string driver;

  std::string exe_path;
  if(!inlib::program_path(arg0,exe_path)) {
    std::cout << "can't get exe directory." << std::endl;
    return EXIT_FAILURE;
  }
  std::string up_exe_path = inlib::dir_name(exe_path);
  std::string fs = inlib::sep();
  std::string res_dir = up_exe_path+fs+"res";
  if(!inlib::env_append_path("EXLIB_FONT_PATH",res_dir+fs+"fonts")) return EXIT_FAILURE;
  
  inlib::put_env("GOPAW_RES_DIR",res_dir);

  std::string tmp_dir;
  if(!inlib::tmpdir(tmp_dir)) {
    std::cout << "inlib::tmpdir() failed." << std::endl;
    return EXIT_FAILURE;
  }

  inlib::put_env("GOPAW_TMP_DIR",tmp_dir);
  
  if(!inlib::isenv("KUIPPSVIEWER")) {
    // override what is in kuip/kuinit.c/Kuinit :
    std::string KUIPPSVIEWER = "echo gopaw_set_env_dummy_psviewer ";
    if(!inlib::putenv("KUIPPSVIEWER",KUIPPSVIEWER)) return EXIT_FAILURE;
  }

#ifdef APP_USE_PYTHON
  std::vector<std::string> tmp_dirs;
 {std::string app_name("gopaw");
  std::string home_dir;
  inlib::dir::home(home_dir);
  if(home_dir.empty()) {
    std::cout << "can't get home directory." << std::endl;
    return EXIT_FAILURE;
  }  
  std::string doc_app_dir = "Documents";
  doc_app_dir += inlib::sep();
  doc_app_dir += app_name;

  std::string doc_dir = home_dir;
  doc_dir += inlib::sep();
  doc_dir += doc_app_dir;

  std::string data_dir = doc_dir;
  std::string out_dir = doc_dir;

  std::string file = res_dir;
  file += inlib::sep();    
  file += "python_2_7_lib_py.zip";    
  if(inlib::device::is_iOS()){
    // no way to cleanup tmp file at exit (because we can't be warned
    // when the app is killed), then we store in app tmp dir but as
    // if done permanently.
    if(!exlib::unfold_res_file_iOS(std::cout,s_PYTHONHOME(),file,"python_2_7",data_dir,tmp_dir)){
      std::cout << "unfold_res_file_iOS() : failed." << std::endl;
      return EXIT_FAILURE;
    }
  } else {
    if(!exlib::unfold_res_file_tmp(std::cout,s_PYTHONHOME(),app_name+"_",file,"python_2_7",data_dir,tmp_dir,tmp_dirs)){
      std::cout << "unfold_res_file_tmp() : failed." << std::endl;
      return EXIT_FAILURE;
    }
  }}
  if(!inlib::is_env(s_PYTHONHOME())) {
    std::cout << "env variable PYTHONHOME not defined." << std::endl;
    return EXIT_FAILURE;
  }
  if(!inlib::env_append_path(s_PYTHONPATH(),res_dir)) { //to find inlib.py
    std::cout << "env_append_path(PYTHONPATH) failed." << std::endl;
    return EXIT_FAILURE;
  }    
#endif
  
  unsigned int ww = 600;
  unsigned int wh = 600;
  if(!inlib::window_size_from_args(args,ww,wh)) {ww = 600;wh = 600;}

  unsigned int verbose = args.is_arg("-verbose")?1:0;
  
#ifdef GOPAW_OFFSCREEN
  if(inlib::file::exists("batch.kumac")) args.add("-kumac_file","batch.kumac",false);
  run_offscreen(std::cout,ww,wh,verbose,res_dir,args);
#else
  if(args.is_arg("-offscreen")) {
    run_offscreen(std::cout,ww,wh,verbose,res_dir,args);
  } else {    
   {
#ifdef _WIN32
    gopaw::UI_Windows ui(std::cout,ww,wh); //before the below. gopaw_initialize() needs to find a valid IUI.
#else
    gopaw::UI_X11 ui(std::cout,ww,wh); //before the below. gopaw_initialize() needs to find a valid IUI.
#endif
#ifdef APP_USE_PYTHON
    //m_opener.add_opener(inlib::file::s_format_py(),new exlib::py_opener());
    exlib::sg::view_Python view_Python(ui.viewer()); //it initializes Python.
    initinlib_swig_py();
#endif
    gopaw::session _sess(std::cout,verbose,ui,res_dir);  
#ifdef APP_USE_PYTHON
    gopaw::session_py _sess_py(_sess);
#endif
    gopaw_initialize(std::cout,verbose,_sess,ui,args.is_arg("-silent"));
    ui.show();
   {std::vector<std::string> kums;
    args.find("-kumac_file",kums);
    args.find("-kumac",kums,false);
    inlib_vforcit(std::string,kums,it) {
      if(gopaw_kuip_exec(std::cout,"/macro/exec "+(*it))) ui.render_it();
    }}
   {std::vector<std::string> cmds;
    args.find("-command",cmds);
    inlib_vforcit(std::string,cmds,it) {
      if(gopaw_kuip_exec(std::cout,(*it))) ui.render_it();
    }}
    if(args.is_arg("-no_term")) {
    } else {
      ui.enable_terminal();
    }
    ui.steer();}
    if(!args.is_arg("-silent")) std::cout << " Exiting from gopaw." << std::endl;
  }
#endif
  
  gopaw_finalize();
  
#ifdef APP_USE_PYTHON
 {inlib_vforcit(std::string,tmp_dirs,it){
    if(!inlib::dir::rmdir(*it)) std::cout << " can't remove directory " << inlib::sout(*it) << std::endl;
  }
  tmp_dirs.clear();}
#endif //APP_USE_PYTHON
 
#ifdef APP_USE_HDF5
 {ssize_t num_open = H5Fget_obj_count(H5F_OBJ_ALL,H5F_OBJ_ALL);
  if(num_open) std::cout << "warning : hdf5 : at exit, num_open " << num_open << std::endl;}
#endif
 
#ifdef INLIB_MEM
  }inlib::mem::balance(std::cout);
  std::cout << "gopaw : exit(mem) ..." << std::endl;
#else  
  if(verbose) std::cout << "gopaw : exit ..." << std::endl;
#endif //INLIB_MEM

  // Some libraries (for exa Coin, gtk) may have declared 
  // C functions with ::atexit. To ease debugging we call
  // explicitly ::exit here.
  //::exit(EXIT_SUCCESS);

  return EXIT_SUCCESS;
}
