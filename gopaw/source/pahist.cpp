// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pahist_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <exlib/KUIP/tools>
#include "find_object"
#include "plot_histogram_2D"
#include "funcs"
#include "read_hippo"

#include <inlib/sys/plugin>
#include <inlib/mnmx>

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
#include <inlib/rroot/file>
#include <inlib/wroot/file>

#include <exlib/zlib>
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

void pahist_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  //std::string cmd_name = ku_name();
  std::string cmd_path = ku_path() ? ku_path() : "";
  
  if(_sess.verbose_level()) out << "pahist : execute " << inlib::sout(cmd_path) << std::endl;

  gopaw::opts_t& opts = _sess.opts();
//gopaw::atts_t& atts = _sess.atts();

  std::string s_Z("Z");
  
  if(cmd_path=="/HISTOGRAM/PLOT") {

    std::string NAME = ku_gets();
    std::string CHOPT = ku_gets();

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    if(_sess.verbose_level()) {
      out << "pahist : " << cmd_path
          << " NAME= " << inlib::sout(NAME) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    std::string ID;
    std::vector< std::pair<int,int> > dims;  
    if(!gopaw::session::parse_name(NAME,ID,dims,out)) return;

    if(_sess.verbose_level()) {
      out << "pahist : " << cmd_path
          << " NAME= " << inlib::sout(NAME) 
          << " ID= " << inlib::sout(ID) 
          << std::endl;
    }

    inlib::histo::h1d* h1d = h1d_find(_sess,ID);
    inlib::histo::h2d* h2d = h2d_find(_sess,ID);
    if(!h1d && !h2d) {
      out << "pahist : can't find histogram " << inlib::sout(ID) << std::endl;
      return;
    }

      inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
      if(_page) {
        _sess.set_plotter_layout(*_page);
        bool superpose = false;
        if(gopaw::session::is_an_option(OPTS,CHOPT,"S") || gopaw::session::is_an_option(OPTS,CHOPT,"+") ) superpose = true;
        _sess.plotter_next(*_page,superpose);

        inlib::sg::plotter* region = &_sess.page_plotter(*_page);
        if(gopaw::session::is_an_option(OPTS,CHOPT,"K")) _sess.set_ZONE_K(region); //pawex24.
        bool add_hist = false;
        if(gopaw::session::is_an_option(OPTS,CHOPT,"+")) {
       	  region = _sess.ZONE_K();
          if(!region) {out << "pahist : option + : ZONE 'K' not set." << std::endl;return;}
	  add_hist = true;
	}
  	inlib::sg::plotter& _plotter = *region;
        _sess.region_set_layout(_plotter);

	bool add_plottable = true;
        if(add_hist) {
          const std::vector<inlib::sg::plottable*>& plottables = _plotter.plottables();
	  if(plottables.size()) {
	    const inlib::sg::plottable* _plottable = plottables.front();
	    const std::string& prev_ID = _plottable->name();
            inlib::histo::h1d* prev_h = h1d_find(_sess,prev_ID);
	    if(!prev_h) {
              out << "pahist : option + : but histo with ID " << prev_ID << " not found." << std::endl;
            } else {
	      if(h1d) {
		inlib::histo::h1d* new_h1d = new inlib::histo::h1d(*prev_h);
		new_h1d->add(*h1d);
                ID = prev_ID+"_add_"+ID;
                _sess.add_handle(ID,new_h1d);
		h1d = new_h1d;
		//::printf("debug : %lu : new y %s %g\n",plottables.size(),ID.c_str(),new_h1d->max_bin_height());
 	        add_plottable = false; //prep_plottable()
              }
            }
	  }
	}
	
        if(h1d) {
          if(_sess.verbose_level()) {
            out << "pahist : " << cmd_path
                << " h1d= " << inlib::sout(h1d->title()) 
                << std::endl;
          }
          if(!gopaw::region_set_limits(_plotter,*h1d,dims,out)) return;

          bool empty = true;
          if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) {
            _sess.plot_histogram_1D(_plotter,*h1d,ID,"C",false,add_plottable); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"L")) {
            _sess.plot_histogram_1D(_plotter,*h1d,ID,"L",false,add_plottable); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"E")) {
            _sess.plot_histogram_1D(_plotter,*h1d,ID,"E",false,add_plottable); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"P")) {
            _sess.plot_histogram_1D(_plotter,*h1d,ID,"P",false,add_plottable); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"*")) {
            _sess.plot_histogram_1D(_plotter,*h1d,ID,"*",false,add_plottable); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"B")) {
            _sess.plot_histogram_1D(_plotter,*h1d,ID,"B",false,add_plottable); 
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT," ")) {
            _sess.plot_histogram_1D(_plotter,*h1d,ID,"",false,add_plottable); 
            empty = false;
          }

          if(empty) _sess.plot_histogram_1D(_plotter,*h1d,ID,"",false,add_plottable); 

          // If histogram is fitted (with /HISTOGRAM/FIT) pafith stores the name
          // of the function on the histogram annotation "fit.function".

	  std::string func_name;
          if(h1d->annotation("fit.function",func_name) && func_name.size() ) {
            gopaw::base_func* func;
            _sess.find_func(func_name,func);
            if(!func) {
              out << "pahist : function of histo fit.function " << inlib::sout(func_name) << " not found." << std::endl;
            } else {
              std::string UFUNC = func_name; //?
		
              int xbinn = h1d->axis().bins();
              double XLOW = h1d->axis().bin_lower_edge(0);
              double XUP = h1d->axis().bin_upper_edge(xbinn-1);

	      if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::gauss>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
              else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::breit_wigner>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
              else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::polynomial>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
              else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::exp_aida>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
              else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::exp_hbook>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
              else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::scripted>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
              else if(_sess._PLOT_FUNC<gopaw::base_func,gopaw::compiled>(_plotter,*func,func_name,UFUNC,XLOW,XUP,"C")) {}
              else {
		out << "pahisto : " << inlib::sout(cmd_path) << " : unknown func class " << inlib::sout(func->s_cls()) << std::endl;
	      }
            }
          }

          return;
        }

        if(h2d) {	  
          if(_sess.verbose_level()) {
            out << "pahist : " << cmd_path
                << " h2d= " << inlib::sout(h2d->title()) 
                << std::endl;
          }
          if(!gopaw::region_set_limits(_plotter,*h2d,dims,out)) return;

          bool empty = true;

          // SURF <h2d> ! ! [1,2]
          //is the same than :
          // H/PLOT <h2d> SURF[1,2]  
          if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF1")) {
            if(_sess.verbose_level()) out << "pahist : h2 : SURF1." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            size_t PALNB;
            _sess.palette_from_HCOL(PALNB);
	    plot_histogram_2D(_plotter,*h2d,gopaw::s_SURF_LEVEL(),out,0,PALNB,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF2")) {
            if(_sess.verbose_level()) out << "pahist : h2 : SURF2." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            size_t PALNB;
            _sess.palette_from_HCOL(PALNB);
            plot_histogram_2D(_plotter,*h2d,gopaw::s_SURF_LEVEL_NO_EDGE(),out,0,PALNB,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF3")) {
            if(_sess.verbose_level()) out << "pahist : h2 : SURF3." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            size_t PALNB;
            _sess.palette_from_HCOL(PALNB);
            plot_histogram_2D(_plotter,*h2d,gopaw::s_SURF_CONT(),out,0,PALNB,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF4")) {
            if(_sess.verbose_level()) out << "pahist : h2 : SURF4." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            plot_histogram_2D(_plotter,*h2d,gopaw::s_SURF_LIGHT(),out,0,0,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF5")) {
            if(_sess.verbose_level()) out << "pahist : h2 : SURF5." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            size_t PALNB;
            _sess.palette_from_HCOL(PALNB);
            plot_histogram_2D(_plotter,*h2d,gopaw::s_SURF_COL(),out,0,PALNB,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,gopaw::s_SURF())) {
            if(_sess.verbose_level()) out << "pahist : h2 : SURF." << std::endl;
            plot_histogram_2D(_plotter,*h2d,gopaw::s_SURF(),out,0,0,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"LEGO1")) {
            if(_sess.verbose_level()) out << "pahist : h2 : LEGO1." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            plot_histogram_2D(_plotter,*h2d,gopaw::s_LEGO_LIGHT(),out,0,0,_sess);
            empty = false;
          } 
          if(gopaw::session::is_an_option(OPTS,CHOPT,"LEGO2")) {
            if(_sess.verbose_level()) out << "pahist : h2 : LEGO2." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            size_t PALNB;
            _sess.palette_from_HCOL(PALNB);
            plot_histogram_2D(_plotter,*h2d,gopaw::s_LEGO_LEVEL(),out,0,PALNB,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"LEGO3")) {
            if(_sess.verbose_level()) out << "pahist : h2 : LEGO3." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            size_t PALNB;
            _sess.palette_from_HCOL(PALNB);
            plot_histogram_2D(_plotter,*h2d,gopaw::s_LEGO_COL(),out,0,PALNB,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,gopaw::s_LEGO())) {
            if(_sess.verbose_level()) out << "pahist : h2 : LEGO." << std::endl;
            plot_histogram_2D(_plotter,*h2d,gopaw::s_LEGO(),out,0,0,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"TEXT")) {
            if(_sess.verbose_level()) out << "pahist : h2 : TEXT." << std::endl;
            plot_histogram_2D(_plotter,*h2d,"TEXT",out,0,0,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"CONT")) {
            if(_sess.verbose_level()) out << "pahist : h2 : CONT." << std::endl;
            plot_histogram_2D(_plotter,*h2d,"CONT_DMOD",out,10,0,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"BOX")) {
            if(_sess.verbose_level()) out << "pahist : h2 : BOX." << std::endl;
            plot_histogram_2D(_plotter,*h2d,"BOX",out,0,0,_sess);
            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT,"COL")) {
            if(_sess.verbose_level()) out << "pahist : h2 : COL." << std::endl;
            if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
            // HCOL 0.<p> does not seem to be taken into account.
            // But only the last PALETTE <p>.
            // And if no PALETTE command had been issued,
            // then the PALETTE_HISTOGRAM_PLOT_COL is taken.

            size_t PALNB = _sess.current_palette();
            plot_histogram_2D(_plotter,*h2d,"COL",out,0,PALNB,_sess);

            empty = false;
          }
          if(gopaw::session::is_an_option(OPTS,CHOPT," ")) {
            if(_sess.verbose_level()) out << "pahist : h2 : OPT ' '." << std::endl;
            plot_histogram_2D(_plotter,*h2d,"",out,0,0,_sess); 
            empty = false;
          }

          if(empty) {
            if(_sess.verbose_level()) out << "pahist : h2 : default." << std::endl;
            plot_histogram_2D(_plotter,*h2d,"",out,0,0,_sess); 
          }
	    
        }

      }

  } else if(cmd_path=="/HISTOGRAM/FILE") {

    int iLUN = ku_geti();
    std::string FNAME = ku_gets();
    /*int LRECL =*/ ku_geti();
    std::string CHOPT = ku_gets();

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    if(_sess.verbose_level()) {
      out << "pahist : " << cmd_path
          << " LUN= " << iLUN
          << " FNAME= " << inlib::sout(FNAME) 
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    unsigned int uLUN;
    if(iLUN==0) { // If LUN is 0 the next free logical unit will be used.
      if(!_sess.mx_LUN(uLUN)) uLUN = 1;
    } else {
      uLUN = (unsigned int)iLUN;
    }

    if(_sess.find_LUN(uLUN)) {
      out << "pahist : LUN " << uLUN << " already in used by gopaw." << std::endl;
      return;
    }

    bool read = true;
    if(gopaw::session::is_an_option(OPTS,CHOPT," ")) read = true; // Existing file is opened (read mode only).      
    if(gopaw::session::is_an_option(OPTS,CHOPT,"N")) read = false;  // A new file is opened.
    if(gopaw::session::is_an_option(OPTS,CHOPT,"U")) { // Existing file is opened to be modified.
      out << "pahist : " << cmd_path << " : update mode not handled." << std::endl;
      return;
    }
    if(gopaw::session::is_an_option(OPTS,CHOPT,"D")) {} // Reset lock.

    std::string sfile;
    inlib::file_name(FNAME,sfile); //to expand environment variables.

    if(_sess.verbose_level()) {
      out << "pahist : " << cmd_path << " file = " << inlib::sout(sfile) << std::endl;
    }
    
    bool verbose = false;

    if(read) {
     {bool is;
      inlib::file::is_root(sfile,is);
      if(is) {
        inlib::rroot::file* root_file = new inlib::rroot::file(out,sfile,verbose);
        root_file->add_unziper('Z',exlib::decompress_buffer);
        if(!root_file->is_open()) {
          out << "pahist : can't open " << inlib::sout(sfile) << std::endl;
          delete root_file;
          return;
        } 
        if(_sess.verbose_level()) {
          out << "pahist : " << cmd_path << " file " << inlib::sout(sfile) << " is a root file." << std::endl;
        }
        _sess.add_LUN_rroot_file(uLUN,root_file);
        _sess.set_current_unit(uLUN);
        return;
      }}
     
     {bool is;
      inlib::file::is_hdf(sfile,is);
      if(is) {
#ifdef APP_USE_HDF5
        hid_t hdf5_file = H5Fopen(sfile.c_str(),H5F_ACC_RDONLY, H5P_DEFAULT);
        if(hdf5_file<0) {
          out << "pahist : can't open " << inlib::sout(sfile) << std::endl;
          return;
        }
        _sess.add_LUN_hdf5_file(uLUN,sfile,hdf5_file,hdf5_file,false);
        _sess.set_current_unit(uLUN);
        if(_sess.verbose_level()) {
          out << "pahist : " << cmd_path << " file " << inlib::sout(sfile) << " is an hdf5 file." << std::endl;
        }
        return;
#else	
        out << "pahist : can't open " << inlib::sout(sfile)
            << ". File is an hdf5 file, but gopaw not compiled with -DAPP_USE_HDF5." << std::endl;
        return;
#endif //APP_USE_HDF5
      }}
     
     {bool is;
      inlib::file::is_fits(sfile,is);
      if(is) {
#ifdef APP_USE_CFITSIO
	exlib::cfitsio::file* ffile = new exlib::cfitsio::file(out);
        if(!ffile->open(sfile)) {
          out << "pahist : can't open " << inlib::sout(sfile) << std::endl;
	  delete ffile;
          return;
        }
        _sess.add_LUN_fits_file(uLUN,sfile,ffile);
        _sess.set_current_unit(uLUN);
        if(_sess.verbose_level()) {
          out << "pahist : " << cmd_path << " file " << inlib::sout(sfile) << " is a fits file." << std::endl;
        }
        return;
#else	
        out << "pahist : can't open " << inlib::sout(sfile)
            << ". File is a fits file, but gopaw not compiled with -DAPP_USE_CFITSIO." << std::endl;
        return;
#endif //APP_USE_CFITSIO
      }}
     
      inlib::aida::ntuple* aida_tuple;
      if(gopaw::read_hippo(out,sfile,aida_tuple)) {
	std::string IDN;
       {std::string path,suffix;
        if(!inlib::path_name_suffix(sfile,path,IDN,suffix)) {
          out << "pahist : problem getting path,name,suffix in " << inlib::sout(sfile) << std::endl;
	  delete aida_tuple;
          return;
        }
        inlib::tolowercase(suffix);
        if(suffix=="gz") inlib::nosuffix(IDN,IDN);
        inlib::nosuffix(IDN,IDN);}

        _sess.add_handle(IDN,aida_tuple);

        _sess.add_LUN_SYS_FILE(uLUN,sfile,0); //declare anyway a unit to quiet /FORTRAN/CLOSE.
        _sess.set_current_unit(uLUN);
	
        if(_sess.verbose_level()) {
          out << "pahist : " << cmd_path << " file " << inlib::sout(sfile) << " is an hippodraw file." << std::endl;
          out << "pahist :  ntuple title " << inlib::sout(aida_tuple->title()) << "." << std::endl;
          out << "pahist :  #columns " << aida_tuple->columns() << std::endl;
        }
        return;
      }

      out << "pahist : unknwon file format for file " << inlib::sout(sfile) << "." << std::endl;

      
    } else {
      inlib::wroot::file* root_file = new inlib::wroot::file(out,sfile);
      root_file->add_ziper('Z',exlib::compress_buffer);
      root_file->set_compression(9);

      _sess.add_LUN_wroot_file(uLUN,root_file);
      _sess.set_current_unit(uLUN);
    }

  } else if(cmd_path=="/HISTOGRAM/LIST") {

    std::string CHOPT = ku_gets();

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    //FIXME : dump currentTree ! (then a .root file).
    _sess.dump_handles(out);

  } else if(cmd_path=="/HISTOGRAM/DELETE") {

    std::string ID = ku_gets();

    // The H/DELETE of PAW admit an ID, 0 or * as argument.
    // gopaw extends that to the same logic as VECTOR, that is to say
    // an "HLIST". An HLIST being names, that may have wildcards,
    // separated by colon.

    std::string HLIST = ID;
    //inlib::touppercase(HLIST);
    if(HLIST=="0") HLIST = "*";

    if(HLIST.find('*')==std::string::npos){
      std::string NAME = HLIST;
      if(!_sess.remove_handle(NAME)) { //ok
        out << "pahist : unable to remove " << inlib::sout(NAME) << std::endl;
      }
    } else { //wildcard.
      _sess.remove_handle_of_class(inlib::histo::h1d::s_class(),HLIST);
      _sess.remove_handle_of_class(inlib::histo::h2d::s_class(),HLIST);
    }

  } else if(cmd_path=="/HISTOGRAM/2D_PLOT/CONTOUR") {

    std::string ID = ku_gets();
    int NLEVEL = ku_geti();
    std::string CHOPT = ku_gets();
    if(_sess.verbose_level()) {
      out << "pavect :" 
          << " " << cmd_path
          << " ID= " << inlib::sout(ID) 
          << " NLEVEL= " << NLEVEL
          << " CHOPT= " << inlib::sout(CHOPT) 
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    //std::string PARAM = ku_gets(); //Vector

    std::string NAME;
    std::vector< std::pair<int,int> > dims;  
    if(!gopaw::session::parse_name(ID,NAME,dims,out)) return;

    inlib::histo::h2d* histogram = h2d_find(_sess,NAME);
    if(!histogram) {
      out << "pahist : can't find histogram 2D " << inlib::sout(NAME) << std::endl;
      return;
    }

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    if(!gopaw::region_set_limits(_plotter,*histogram,dims,out)) return;
    
    _sess.region_set_layout(_plotter);

    if(gopaw::session::is_an_option(OPTS,CHOPT,"3")) {
      //-3 The contour is drawn with filled colour levels.
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB); //FIXME : ok ?
      plot_histogram_2D(_plotter,*histogram,"CONT_COL_FILLED",out,NLEVEL,PALNB,_sess);

    } else {
      // wire frame :
      //CHOPT 0 has priority over 1.
      if(gopaw::session::is_an_option(OPTS,CHOPT,"0")) {
        //-0 Use colour to distinguish contours.

        // In CERN/PAW The palette seems to be hard coded 
        // to one with 4 elements (1,2,3,4)
        size_t PALNB = gopaw::session::PALETTE_HISTOGRAM_2D_PLOT_CONTOUR_OPT_0();
	      
        std::string old_COLL = opts.value("COLL");
        opts.set_value("COLL","COLV");
        plot_histogram_2D(_plotter,*histogram,"CONT_COL",out,NLEVEL,PALNB,_sess);
        opts.set_value("COLL",old_COLL);

      } else { //CHOPT 1,2
        //-1 Use line style to distinguish contours.
        //FIXME -2 Line style and colour are the same for all contours.
        plot_histogram_2D(_plotter,*histogram,"CONT_DMOD",out,NLEVEL,0,_sess);
      }

    }

  } else if(cmd_path=="/HISTOGRAM/2D_PLOT/SURFACE") {

    std::string ID = ku_gets();
    double THETA = ku_getr();
    double PHI = ku_getr();
    std::string CHOPT = ku_gets();

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    std::string NAME;
    std::vector< std::pair<int,int> > dims;  
    if(!gopaw::session::parse_name(ID,NAME,dims,out)) return;

    inlib::histo::h2d* histogram = h2d_find(_sess,NAME);
    if(!histogram) {
      out << "pahist : can't find histogram 2D " << inlib::sout(NAME) << std::endl;
      return;
    }

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    if(!gopaw::region_set_limits(_plotter,*histogram,dims,out)) return;
    
    _sess.region_set_layout(_plotter);

    double old_THETA = _sess.THETA();
    double old_PHI = _sess.PHI();
    _sess.set_THETA(THETA);
    _sess.set_PHI(PHI);

    //if(gopaw::session::is_an_option(OPTS,CHOPT," "))
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"1"))
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"2"))
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"3"))
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"4"))

    // SURF <h2d> ! ! [1,2]
    //is the same than :
    // H/PLOT <h2d> SURF[1,2]
  
    //FIXME : -1 show bin edgeds. Do we want that ?
    if(gopaw::session::is_an_option(OPTS,CHOPT,"1")) {
      if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
      //CERN/PAW doc :
      //-1 Hidden surface algorithm is used and each cell is filled_
      //   with a colour corresponding to the Z value.
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_LEVEL(),out,0,PALNB,_sess);
  
    } else if(gopaw::session::is_an_option(OPTS,CHOPT,"2")) {
      if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_LEVEL_NO_EDGE(),out,0,PALNB,_sess);
  
    } else if(gopaw::session::is_an_option(OPTS,CHOPT,"3")) {
      if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_CONT(),out,0,PALNB,_sess);
  
    } else if(gopaw::session::is_an_option(OPTS,CHOPT,"4")) {
      if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_LIGHT(),out,0,0,_sess);
      
    } else if(gopaw::session::is_an_option(OPTS,CHOPT,"5")) {
      if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF_COL(),out,0,PALNB,_sess);
  
    } else {
      plot_histogram_2D(_plotter,*histogram,gopaw::s_SURF(),out,0,0,_sess);
    }
  
    _sess.set_THETA(old_THETA);
    _sess.set_PHI(old_PHI);

  } else if(cmd_path=="/HISTOGRAM/2D_PLOT/LEGO") {

    std::string ID = ku_gets();
    double THETA = ku_getr();
    double PHI = ku_getr();
    std::string CHOPT = ku_gets();

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    std::string NAME;
    std::vector< std::pair<int,int> > dims;  
    if(!gopaw::session::parse_name(ID,NAME,dims,out)) return;

    inlib::histo::h2d* histogram = h2d_find(_sess,NAME);
    if(!histogram) {
      out << "pahist : can't find histogram 2D " << inlib::sout(NAME) << std::endl;
      return;
    }

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(!_page) return;
    
    _sess.set_plotter_layout(*_page);
    _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
    inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
    if(!gopaw::region_set_limits(_plotter,*histogram,dims,out)) return;
    
    _sess.region_set_layout(_plotter);
	
    double old_THETA = _sess.THETA();
    double old_PHI = _sess.PHI();
    _sess.set_THETA(THETA);
    _sess.set_PHI(PHI);

    //if(gopaw::session::is_an_option(OPTS,CHOPT," "))
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"1"))
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"2"))
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"3"))
    //if(gopaw::session::is_an_option(OPTS,CHOPT,"4"))
  
    if(gopaw::session::is_an_option(OPTS,CHOPT,"1")) {
      if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
      plot_histogram_2D(_plotter,*histogram,gopaw::s_LEGO_LIGHT(),out,0,0,_sess);

    } else if(gopaw::session::is_an_option(OPTS,CHOPT,"2")) {
      if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_LEGO_LEVEL(),out,0,PALNB,_sess);

    } else if(gopaw::session::is_an_option(OPTS,CHOPT,"3")) {
      if(gopaw::session::is_an_option(OPTS,CHOPT,s_Z)) _plotter.colormap_visible = true;
      size_t PALNB;
      _sess.palette_from_HCOL(PALNB);
      plot_histogram_2D(_plotter,*histogram,gopaw::s_LEGO_COL(),out,0,PALNB,_sess);

    } else {
      plot_histogram_2D(_plotter,*histogram,gopaw::s_LEGO(),out,0,0,_sess);
    }
  
    _sess.set_THETA(old_THETA);
    _sess.set_PHI(old_PHI);

  } else {
    out << "pahist : " << cmd_path << " : dummy." << std::endl;
  }
}
