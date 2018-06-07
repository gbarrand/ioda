// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void panntu_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <inlib/histo/c1d>
#include <inlib/sjust>
#include <inlib/tos_deprec>
#include <inlib/vmanip>
#include <inlib/aida_ntuple>

namespace gopaw {
template <class T>
inline std::string sjust(T a_value,unsigned int a_size,inlib::side a_side = inlib::side_left) {
  std::string s = inlib::tos(a_value);
  inlib::justify(s,a_size,a_side);
  return s;
}
}

#include <exlib/yacc/evaluator>

template <class EVALUATOR,class T>
inline bool project(std::ostream& a_out,inlib::aida::ntuple& a_tuple,T& a_fillable,EVALUATOR& a_x,EVALUATOR& a_filter) {
  if(!a_x.is_valid()) {
    a_out << "project : evaluator initialization failed for tuple " << inlib::sout(a_tuple.title()) << "." << std::endl;
    return false;
  }
  if(!a_filter.is_valid()) {
    a_out << "project : filter initialization failed for tuple " << inlib::sout(a_tuple.title()) << "." << std::endl;
    return false;
  }

  bool ok;
  double value;

  a_tuple.start();
  while(a_tuple.next()) {
    if(!a_filter.accept(ok)) return false;
    if(!ok) continue;
    if(!a_x.evaluate_double(value)) return false;    
    a_fillable.fill(value,1);
  }

  return true;
}

template <class EVALUATOR,class T>
inline bool project(std::ostream& a_out,inlib::aida::ntuple& a_tuple,T& a_fillable,
                    EVALUATOR& a_x,EVALUATOR& a_y,EVALUATOR& a_filter) {
  if(!a_x.is_valid()) {
    a_out << "project : evaluator initialization failed for tuple " << inlib::sout(a_tuple.title()) << "." << std::endl;
    return false;
  }
  if(!a_y.is_valid()) {
    a_out << "project : evaluator initialization failed for tuple " << inlib::sout(a_tuple.title()) << "." << std::endl;
    return false;
  }
  if(!a_filter.is_valid()) {
    a_out << "project : filter initialization failed for tuple " << inlib::sout(a_tuple.title()) << "." << std::endl;
    return false;
  }

  bool ok;
  double vx,vy;

  a_tuple.start();
  while(a_tuple.next()) {
    if(!a_filter.accept(ok)) return false;
    if(!ok) continue;
    if(!a_x.evaluate_double(vx)) return false;    
    if(!a_y.evaluate_double(vy)) return false;    
    a_fillable.fill(vx,vy,1);
  }

  return true;
}

typedef exlib::evaluator<inlib::aida::ntuple ,inlib::aida::aida_col<double> > eval_t;

#include <exlib/KUIP/tools>
#include "find_object"
#include "plot_histogram_2D"

//#define SHOW_TIME
#ifdef SHOW_TIME
#include <inlib/sys/atime>
#endif

void panntu_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "panntu :" 
        << " execute " << inlib::sout(cmd_path) 
        << std::endl;
  }

  gopaw::opts_t& opts = _sess.opts();

  if(cmd_path=="/NTUPLE/PROJECT") {

    std::string IDH = ku_gets();
    std::string IDN = ku_gets();
    std::string UWFUNC = ku_gets();
    //int NEVENT = 
    ku_geti();
    //int IFIRST =
    ku_geti();
 
    std::string PATH;
    std::vector<std::string> vars;
    if(!_sess.parse_tuple_name(IDN,PATH,vars)) return;

    inlib::aida::ntuple* tuple = tuple_find(_sess,PATH);
    if(!tuple) {
      out << "panntu : " << cmd_path << " : can't find tuple " << inlib::sout(PATH) << "." << std::endl;
      return;
    }

    std::string sfilter;
    if(!_sess.filter_cuts(UWFUNC,sfilter)) {
      out << "panntu : problem treating " << inlib::sout(UWFUNC) << std::endl;
      return;
    }
    if(sfilter=="1.") sfilter = "";

    if(vars.size()==1) {

      inlib::histo::h1d* histogram = h1d_find(_sess,IDH);
      if(!histogram) {
        out << "panntu : can't find histogram " << inlib::sout(IDH) << std::endl;
        return;
      }

      eval_t evalX(out,*tuple,vars[0],false); //false=case insensitive
      eval_t filter(out,*tuple,sfilter,false);
      if(!project(out,*tuple,*histogram,evalX,filter)){}

    } else if(vars.size()==2) {

      inlib::histo::h2d* histogram = h2d_find(_sess,IDH);
      if(!histogram) {
        out << "panntu : can't find histogram " << inlib::sout(IDH) << std::endl;
        return;
      }

      eval_t evalX(out,*tuple,vars[1],false); //false=case insensitive
      eval_t evalY(out,*tuple,vars[0],false);
      eval_t filter(out,*tuple,sfilter,false);

      if(!project(out,*tuple,*histogram,evalX,evalY,filter)) {}
    }

  } else if(cmd_path=="/NTUPLE/PLOT") {

    std::string IDN = ku_gets();
    std::string UWFUNC = ku_gets();
    int NEVENT = ku_geti();
    int IFIRST = ku_geti();
    int NUPD = ku_geti();
    std::string OPTION = ku_gets();
    int IDH = ku_geti();

    if(_sess.verbose_level()) {
      out << "panntu :" 
          << " " << cmd_path
          << " IDN= " << inlib::sout(IDN)
          << " UWFUNC= " << inlib::sout(UWFUNC)
          << " NEVENT= " << NEVENT
          << " IFIRST= " << IFIRST
          << " NUPD= " << NUPD
          << " OPTION= " << inlib::sout(OPTION)
          << " IDH= " << IDH
          << std::endl;
    }

    if(NEVENT<0) IDH = -NEVENT;
    if(IFIRST<0) IDH = -IFIRST;
    if(NUPD<0) IDH = -NUPD;

    inlib::touppercase(OPTION);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("OPTION",OPTS);
    std::string CHOPT = OPTION;

    std::string PATH;
    std::vector<std::string> vars;
    if(!_sess.parse_tuple_name(IDN,PATH,vars)) return;

    inlib::aida::ntuple* tuple = tuple_find(_sess,PATH);
    if(!tuple) {
      out << "panntu : " << cmd_path << " : can't find tuple " << inlib::sout(PATH) << "." << std::endl;
      return;
    }

    std::string sfilter;
    if(!_sess.filter_cuts(UWFUNC,sfilter)) {
      out << "panntu : problem treating " << inlib::sout(UWFUNC) << std::endl;
      return;
    }
    if(sfilter=="1.") sfilter = "";

    if(vars.size()==1) {

      eval_t evalX(out,*tuple,vars[0],false); //false=case insensitive
      eval_t filter(out,*tuple,sfilter,false);

      std::string ID = inlib::tos(IDH);
      if(IDH==1000000) _sess.remove_handle(ID);

#ifdef SHOW_TIME
      inlib::atime begin = inlib::atime::now();
#endif

      inlib::histo::h1d* rep = h1d_find(_sess,ID);
      if(rep) {

        if(gopaw::session::is_an_option(OPTS,CHOPT,"S")) {
          // Look if ID already plotted.
          // If so we have to create another histo.

          inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
          if(_page) {
              inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
              std::vector<std::string> names;
              _plotter.plotted_object_names(names);
              size_t namen = names.size();
              std::string sID(ID.c_str());
              for(size_t index=0;index<namen;index++) {
                if(sID==names[index]) {
                  //int n = _plotter->numberOfPlottedObject.getValue();
                  //ID = "tmp_" + ID + "_" + inlib::tos(n);
                  ID = "tmp_" + ID + "_" + inlib::tos((unsigned int)namen);
                  rep = new inlib::histo::h1d(*rep);
                  _sess.add_handle(ID,rep);
                  break;
                }
	      }
          }
        }      

        rep->reset();

        if(!project(out,*tuple,*rep,evalX,filter)){}

      } else {

        inlib::histo::c1d cloud;
        if(!project(out,*tuple,cloud,evalX,filter)) return;

        double xmin = cloud.lower_edge();
        double xmax = cloud.upper_edge();
        if(xmax<xmin) {
          out << "panntu : xmin>xmax." << std::endl;
          return;
        } else if(xmax==xmin) {
          xmax = xmin + 1;
        } else { // Tilt xmax in order that max value is in the histo :
          xmax = xmax + (xmax-xmin) * 0.01; //FIXME : try to avoid that.
        }

        std::string title = vars[0];

        rep = new inlib::histo::h1d(title,100,xmin,xmax);
        _sess.add_handle(ID,rep);

        if(!rep) {
          out << "panntu : can't create histogram " << inlib::sout(ID) << " in memory tree." << std::endl;
          return;
        }

        cloud.fill_histogram(*rep);

      }

#ifdef SHOW_TIME
      inlib::atime end = inlib::atime::elapsed(begin);
      out << "panntu :" << " elapsed : " << end.seconds() << " s" << std::endl;
#endif

      if(_sess.verbose_level()) {
        out << "panntu : projection : " << std::endl;
        rep->hprint(_sess.out());
      }

      inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
      if(!_page) return;
      _sess.set_plotter_layout(*_page);
      _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
      inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
      _sess.region_set_layout(_plotter);

      if(opts.value("NBAR")=="BAR") CHOPT += "B";

      bool empty = true;
      if(gopaw::session::is_an_option(OPTS,CHOPT,"C")) {
        _sess.plot_histogram_1D(_plotter,*rep,ID,"C",false); 
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"L")) {
        _sess.plot_histogram_1D(_plotter,*rep,ID,"L",false); 
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"E")) {
        _sess.plot_histogram_1D(_plotter,*rep,ID,"E",false); 
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"P")) {
        _sess.plot_histogram_1D(_plotter,*rep,ID,"P",false); 
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"*")) {
        _sess.plot_histogram_1D(_plotter,*rep,ID,"*",false); 
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"B")) {
        _sess.plot_histogram_1D(_plotter,*rep,ID,"B",false); 
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT," ")) {
        _sess.plot_histogram_1D(_plotter,*rep,ID,"",false); 
        empty = false;
      }
  
      if(empty) _sess.plot_histogram_1D(_plotter,*rep,ID,"",false); 

    } else if(vars.size()==2) {

      eval_t evalX(out,*tuple,vars[1],false); //false=case insensitive
      eval_t evalY(out,*tuple,vars[0],false);
      eval_t filter(out,*tuple,sfilter,false);

      std::string ID = inlib::tos(IDH);
      if(IDH==1000000) _sess.remove_handle(ID);

#ifdef SHOW_TIME
      inlib::atime begin = inlib::atime::now();
#endif

      inlib::histo::h2d* rep = h2d_find(_sess,ID);
      if(rep) {

        // Look if ID already plotted.
        // If so we have to create another histo.
        if(gopaw::session::is_an_option(OPTS,CHOPT,"S")) {

        inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
        if(_page) {
              inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
              std::vector<std::string> names;
              _plotter.plotted_object_names(names);
              size_t namen = names.size();
              std::string sID(ID.c_str());
              for(size_t index=0;index<namen;index++) {
                if(sID==names[index]) {
                  //int n = _plotter->numberOfPlottedObject.getValue();
                  //ID = "tmp_" + ID + "_" + inlib::tos(n);
                  ID = "tmp_" + ID + "_" + inlib::tos((unsigned int)namen);
                  rep = new inlib::histo::h2d(*rep);
                  _sess.add_handle(ID,rep);
                  break;
                }
	      }
            }
        }

        rep->reset();

	if(!project(out,*tuple,*rep,evalX,evalY,filter)) {}

      } else {

        inlib::histo::c2d cloud;
	if(!project(out,*tuple,cloud,evalX,evalY,filter)) return;

        double xmin = cloud.lower_edge_x();
        double xmax = cloud.upper_edge_x();
        if(xmax<xmin) {
          out << "panntu : xmin>xmax." << std::endl;
          return;
        } else if(xmax==xmin) {
          xmax = xmin + 1;
        } else { // Tilt xmax in order that max value is in the histo :
          xmax = xmax + (xmax-xmin) * 0.01; //FIXME : try to avoid that.
         }

        double ymin = cloud.lower_edge_y();
        double ymax = cloud.upper_edge_y();
        if(ymax<ymin) {
          out << "panntu : ymin>ymax." << std::endl;
          return;
        } else if(ymax==ymin) {
          ymax = ymin + 1;
        } else { // Tilt ymax in order that max value is in the histo :
          ymax = ymax + (ymax-ymin) * 0.01; //FIXME : try to avoid that.
        }

        std::string title = vars[0]+" VS. "+vars[1];

        rep = new inlib::histo::h2d(title,100,xmin,xmax,100,ymin,ymax);
        _sess.add_handle(ID,rep);

        if(!rep) {
          out << "panntu : can't create histogram " << inlib::sout(ID) << " in memory tree." << std::endl;
          return;
        }

        cloud.fill_histogram(*rep);

      }

#ifdef SHOW_TIME
      inlib::atime end = inlib::atime::elapsed(begin);
      out << "panntu :" << " elapsed : " << end.seconds() << " s" << std::endl;
#endif

      inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
      if(!_page) return;
      
      _sess.set_plotter_layout(*_page);
      _sess.plotter_next(*_page,gopaw::session::is_an_option(OPTS,CHOPT,"S"));
      inlib::sg::plotter& _plotter = _sess.page_plotter(*_page);
      _sess.region_set_layout(_plotter);

      bool empty = true;
      if(gopaw::session::is_an_option(OPTS,CHOPT,"SURF")) {
        plot_histogram_2D(_plotter,*rep,"SURF",out,0,0,_sess);
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"LEGO")) {
        plot_histogram_2D(_plotter,*rep,"LEGO",out,0,0,_sess);
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"TEXT")) {
        plot_histogram_2D(_plotter,*rep,"TEXT",out,0,0,_sess);
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"CONT")) {
        plot_histogram_2D(_plotter,*rep,"CONT_DMOD",out,10,0,_sess);
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT,"BOX")) {
        plot_histogram_2D(_plotter,*rep,"BOX",out,0,0,_sess);
        empty = false;
      }
      if(gopaw::session::is_an_option(OPTS,CHOPT," ")){
        plot_histogram_2D(_plotter,*rep,"",out,0,0,_sess); 
        empty = false;
      }
      if(empty) plot_histogram_2D(_plotter,*rep,"",out,0,0,_sess); 

    }

  } else if(cmd_path=="/NTUPLE/SCAN") {

    std::string IDN = ku_gets();
    std::string UWFUNC = ku_gets();
    int NEVENT = ku_geti();
    int IFIRST = ku_geti();
    std::string OPTION = ku_gets();
    std::string VARLIST = ku_gets();

    if(_sess.verbose_level()) {
      out << "panntu :" 
          << " " << cmd_path
          << " IDN= " << inlib::sout(IDN)
          << " UWFUNC= " << inlib::sout(UWFUNC)
          << " NEVENT= " << NEVENT
          << " IFIRST= " << IFIRST
          << " OPTION= " << inlib::sout(OPTION)
          << " VARLIST = " << inlib::sout(VARLIST)
          << std::endl;
    }

    inlib::touppercase(OPTION);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("OPTION",OPTS);
    std::string CHOPT = OPTION;

    inlib::aida::ntuple* tuple = tuple_find(_sess,IDN);
    if(!tuple) {
      out << "panntu : " << cmd_path << " : can't find tuple " << inlib::sout(IDN) << "." << std::endl;
      return;
    }

    std::vector<std::string> vars;
    inlib::words(VARLIST,",",false,vars);

    size_t varn = vars.size();
    std::vector<inlib::aida::aida_base_col*> varcs(varn);
    std::vector<std::string> varns(varn);
    size_t vari;
   {for(vari=0;vari<varn;vari++) {
      inlib::aida::aida_base_col* col = tuple->find_aida_base_column(vars[vari],false); //false = case insensitive.
      if(!col) {
        out << "panntu : " << cmd_path << " : "
            << " variable " << inlib::sout(vars[vari]) << " not a column of tuple " << inlib::sout(IDN) << std::endl;
        return;
      }
      varcs[vari] = col;
      varns[vari] = vars[vari];
    }}

    std::string sfilter;
    if(!_sess.filter_cuts(UWFUNC,sfilter)) {
      out << "panntu : " << cmd_path << " : problem treating " << inlib::sout(UWFUNC) << std::endl;
      return;
    }
    if(sfilter=="1.") sfilter = "";

    eval_t filter(out,*tuple,sfilter,false); //false=case_insensitive
    if(!filter.is_valid()) {
      out << "panntu : " << cmd_path << " : filter initialization failed." << std::endl;
      return;
    }

    out << "+-------+";
    for(vari=0;vari<varn;vari++) out << "--------------+";
    out << std::endl;

    out << "| Event |";
    for(vari=0;vari<varn;vari++) out << gopaw::sjust(varns[vari],14,inlib::side_middle) << "|";
    out << std::endl;
 
    out << "+-------+";
    for(vari=0;vari<varn;vari++) out << "--------------+";
    out << std::endl;

    int eventn = 0; 
    int rowi = 1; 
    bool ok;
    std::string sval;
    tuple->start();
    while(tuple->next()) {
      if(rowi>=IFIRST) {
        if((NEVENT!=99999999)&&(rowi>NEVENT)) break;       
        if(!filter.accept(ok)) {
          out << "panntu : " << cmd_path << " : filter.accept() failed." << std::endl;
          return;
	}
        if(ok) {
          out << "|" << gopaw::sjust(rowi,7,inlib::side_middle) << "|";
          for(vari=0;vari<varn;vari++) {
            if(!varcs[vari]->s_value(sval)) {}
            out << gopaw::sjust(sval,14,inlib::side_middle);
            out << "|";
          }
          out << std::endl;
          eventn++;
        }
      }
      rowi++;
    }

    out << "+-------+";
    for(vari=0;vari<varn;vari++) out << "--------------+";
    out << std::endl;

    out << "==> " << eventn << " events satisfied the imposed cuts" << std::endl;

  } else if(cmd_path=="/NTUPLE/LOOP") {

    std::string IDN = ku_gets();
    std::string UWFUNC = ku_gets();
    int NEVENT = ku_geti();
    int IFIRST = ku_geti();

    if(_sess.verbose_level()) {
      out << "panntu :" 
          << " " << cmd_path
          << " IDN= " << inlib::sout(IDN)
          << " UWFUNC= " << inlib::sout(UWFUNC)
          << " NEVENT= " << NEVENT
          << " IFIRST= " << IFIRST
          << std::endl;
    }

    std::string mask;
    int ibit = 0;

    std::string::size_type pos = UWFUNC.find(">>");
    if(pos!=std::string::npos) {
      std::string smask = UWFUNC.substr(pos+2,UWFUNC.size()-(pos+2)); 
      std::string::size_type lpar = smask.rfind('(');
      if(lpar==std::string::npos) { //Syntax error.
        out << "panntu : syntax error in " << inlib::sout(UWFUNC) << std::endl;
        return;
      }
      mask = smask.substr(0,lpar);
      inlib::strip(mask);

      // Argument :
      std::string::size_type rpar = smask.find(')',lpar);
      if(rpar==std::string::npos) { //Syntax error
        out << "panntu : syntax error in " << inlib::sout(UWFUNC) << std::endl;
        return;
      }
      std::string sarg = smask.substr(lpar+1,rpar-lpar-1);
      if(!inlib::to(sarg,ibit)) { //Syntax error.
        out << "panntu : syntax error in " << inlib::sout(UWFUNC) << std::endl;
        return;
      }
      if((ibit<1)||(ibit>32)) {
        out << "panntu :" 
            << " bad bit value " << ibit
            << ". Should be in [1,32]."
            << std::endl;
        return;
      }

      UWFUNC = UWFUNC.substr(0,pos);      
    }

    FILE* file = 0;
    if(mask.size()) {
      gopaw::LUN* _lun;
      if(!_sess.find_LUN(mask,_lun) || (_lun->m_type!=gopaw::LUN::SYS_FILE) || !_lun->m_FILE) {
        out << "panntu : mask file " << inlib::sout(mask) << " not opened." << std::endl;
        return;
      }
      file = _lun->m_FILE;
    }

    inlib::aida::ntuple* tuple = tuple_find(_sess,IDN);
    if(!tuple) {
      out << "panntu : " << cmd_path << " : can't find tuple " << inlib::sout(IDN) << "."
          << std::endl;
      return;
    }

    std::string sfilter;
    if(!_sess.filter_cuts(UWFUNC,sfilter)) {
      out << "panntu : problem treating " << inlib::sout(UWFUNC) << std::endl;
      return;
    }
    if(sfilter=="1.") sfilter = "";

    eval_t filter(out,*tuple,sfilter,false); //false=case_insensitive
    if(!filter.is_valid()) {
      out << "panntu : filter initialization failed." << std::endl;
      return;
    }

    int eventn = 0; 
    long end = 0L;
    if(file) {
      ::fseek(file,0L,SEEK_END);
      end = ::ftell(file);
      ::rewind(file);
    }

    bool ok;
    int rowi = 1; 
    tuple->start();
    while(tuple->next()) {
      if(rowi>=IFIRST) {
        if((NEVENT!=99999999)&&(rowi>NEVENT)) break;
        if(!filter.accept(ok)) {
          out << "panntu : filter.accept() failed." << std::endl;
          return;
        }
        if(ok) eventn++;
        if(file) {    
          if(::ftell(file)==end) {
            unsigned int i = 0;
            char* buff = (char*)&i;
            if(ok) i = (1 << (ibit-1));
            int nitem = ::fwrite(buff,4,1,file);
            if(nitem!=1) {
              out << "panntu : IO problem 1 : " << nitem << std::endl;
              return;
            }
            end = ::ftell(file);
          } else {
            unsigned int i = 0;
            char* buff = (char*)&i;
            int nitem = ::fread(buff,4,1,file);
            if(nitem!=1) {
              out << "panntu : IO problem 2 : " << nitem << std::endl;
              return;
            }
            unsigned int j = (1 << (ibit-1));
            if(ok) i |= j;
            else   i &= ~j;
            if(::fseek(file,-4L, SEEK_CUR)){
              out << "panntu : IO problem 3." << std::endl;
              return;
            }
            nitem = ::fwrite(buff,4,1,file);
            if(nitem!=1) {
              out << "panntu : IO problem 4." << std::endl;
              return;
            }
	    ::fflush(file); //WIN32 : seems needed.
          }
        }
      }
      rowi++;
    }

    if(mask!="") {
      out << " bit " << ibit << ": "
          << gopaw::sjust(eventn,9,inlib::side_right)
          << " " << UWFUNC
          << std::endl;
    }

  } else {
    out << "panntu : " << cmd_path << " : dummy." << std::endl;
  }

}
