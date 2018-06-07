// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pagraf_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include "session"
#include <exlib/KUIP/tools>

#include <inlib/smatch>
#include <inlib/sout>
#include <inlib/touplow>

namespace gopaw {

static bool set_att(gopaw::session& a_session,atts_t& a_atts,const std::string& a_CHATT,double a_VALUE) { 
  if(a_CHATT=="HCOL") { 

    if(a_VALUE==0) a_VALUE = a_atts.default_value(a_CHATT);
    a_atts.set_value(a_CHATT,a_VALUE);

    if(a_VALUE<=0) {
      a_atts.set_value("HTYP",0); //No hatching
      a_session.set_contour_mode(true);         //Lines
      a_atts.set_value("HCOL",1); //Black.
    } else if((a_VALUE>=1)&&(a_VALUE<=999)) {
      a_atts.set_value("HTYP",0); //No hatching.
      a_session.set_contour_mode(true);         //Lines
    } else if((a_VALUE>=1001)&&(a_VALUE<=1999)) {
      a_atts.set_value("HTYP",0); //No hatching.
      a_session.set_contour_mode(false);        //Fill area.
    } else if(a_VALUE==1000) { //??? look to be fill area white.
      a_atts.set_value("HTYP",0); //No hatching.
      a_session.set_contour_mode(false);         //Fill area.
      a_atts.set_value("HCOL",8); //White.
    }

  } else if(a_CHATT=="HTYP") { 

    if(a_VALUE==0) a_VALUE = a_atts.default_value(a_CHATT);
    a_atts.set_value(a_CHATT,a_VALUE);

    a_session.set_contour_mode(true); //Lines
      
  } else if(a_CHATT=="NCOL") { 

    if(a_VALUE==0) a_VALUE = a_atts.default_value(a_CHATT);
    a_atts.set_value(a_CHATT,a_VALUE);

    int NCOL = (int)a_VALUE;
    if(NCOL>8) {
      // Fill color_table elements [8,NCOL] with 
      // a grey scale from white to black :
      int ncol = NCOL-8;
      if(ncol==1) {
        a_session.setColor(9,1,1,1);
      } else {
        for(int index=0;index<ncol;index++) {
          double grey = 1 - ((float)index)/((float)(ncol-1));
          a_session.setColor(8+index,grey,grey,grey);
        }
      }
    }

  } else if(a_atts.is_key(a_CHATT)) {

    double old_VALUE = a_atts.value(a_CHATT);

    int category;
    if(!a_atts.category(a_CHATT,category)) {}
    if(category==gopaw::cat_IGSET()) {
      // see also papict IGSET command.
      // it seems that the IGSET atts do not conform to the "0=default" logic except
      // CSHI, BARW (existing in HPLSET) and NCOL :
      if((a_CHATT=="CSHI") ||
	 (a_CHATT=="BARW") ||
	 (a_CHATT=="NCOL") ){
        if(a_VALUE==0) a_VALUE = a_atts.default_value(a_CHATT); //PAW convention.
      }
    } else {
      if(a_VALUE==0) a_VALUE = a_atts.default_value(a_CHATT); //PAW convention.
    }

    
    a_atts.set_value(a_CHATT,a_VALUE);

    if( (a_CHATT=="XSIZ") ||
        (a_CHATT=="XMGL") ||
        (a_CHATT=="XMGR") ||
        (a_CHATT=="XWIN") ||
        (a_CHATT=="YSIZ") ||
        (a_CHATT=="YMGL") ||
        (a_CHATT=="YMGU") ||
        (a_CHATT=="YWIN") ){
      if(a_VALUE!=old_VALUE) {
        inlib::sg::plots* _page = a_session.find_plots(gopaw::s_current());
        if(_page) a_session.set_plots_layout(*_page);
      }
    }

  } else {
    return false; 
  }

  return true;
}

}
  
void pagraf_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();

  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pagraf : execute " << inlib::sout(cmd_path) << std::endl;
  }

  gopaw::opts_t& opts = _sess.opts();
  gopaw::atts_t& atts = _sess.atts();

  if(cmd_path=="/GRAPHICS/VIEWING/ZONE") {

    int NX = ku_geti();
    int NY = ku_geti();
    int IFIRST = ku_geti();
    std::string CHOPT = ku_gets();

    if(_sess.verbose_level()) {
      out << "pagraf : " << cmd_path
          << " NX= " << NX 
          << " NY= " << NY 
          << " CHOPT= " << inlib::sout(CHOPT)
          << std::endl;
    }

    inlib::touppercase(CHOPT);
    std::vector<std::string> OPTS;
    exlib::KUIP::get_opts("CHOPT",OPTS);

    _sess.set_ZONE(NX,NY,IFIRST,gopaw::session::is_an_option(OPTS,CHOPT,"S"));

    _sess.set_do_zone();

  } else if(cmd_path=="/GRAPHICS/SET") {

    std::string CHATT = ku_gets();
    double VALUE = ku_getr();

    if(_sess.verbose_level()) {
      out << "pagraf : " << cmd_path
          << " CHATT= " << inlib::sout(CHATT)
          << " VALUE= " << VALUE
          << std::endl;
    }
    
    inlib::touppercase(CHATT);
    if(CHATT.size()>4) CHATT.resize(4);

    if(CHATT=="SHOW") {  
      std::ostream& out = _sess.out();
      out << "+------------------------------------------------------------------------------+" << std::endl;
      out << "|                       HPLSET : Current values in use                         |" << std::endl;
      _sess.showATTs(out,gopaw::cat_HPLSET());

      out << "+------------------------------------------------------------------------------+" << std::endl;
      out << "|                        IGSET : Current values in use                         |" << std::endl;
      _sess.showATTs(out,gopaw::cat_IGSET());

      out << "+------------------------------------------------------------------------------+" << std::endl;
      out << "|                        GOPAW : Current values in use                         |" << std::endl;
      _sess.showATTs(out,gopaw::cat_GOPAW_SET());

    } else if(CHATT=="*") { 

      atts.set_all_to_default();
      
      inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
      if(_page) _sess.set_plots_layout(*_page);

      _sess.set_ZONE(1,1,1,false);
      _sess.set_do_zone();

    } else if(CHATT.find('*')!=std::string::npos) { 

      // Find matching attributes :
      const gopaw::atts_t::list_t& atts_list = atts.list();
      gopaw::atts_t::list_t::const_iterator it;
      for(it=atts_list.begin();it!=atts_list.end();++it) {
        if(inlib::match((*it).key(),CHATT)) {
          set_att(_sess,atts,(*it).key(),VALUE);
        }
      }
      
    } else if(!set_att(_sess,atts,CHATT,VALUE)) {

      out << "pagraf : " << inlib::sout(CHATT) << " is not an attribute." << std::endl;

    }

  } else if(cmd_path=="/GRAPHICS/OPTION") {

    std::string CHOPTN = ku_gets();

    if(_sess.verbose_level()) {
      out << "pagraf : " << cmd_path
          << " CHOPTN= " << inlib::sout(CHOPTN)
          << std::endl;
    }
    
    inlib::touppercase(CHOPTN);
    if(CHOPTN.size()>4) CHOPTN.resize(4);

    if(CHOPTN=="SHOW") {

      std::ostream& out = _sess.out();
      out << "+-----------------------------------------------------------------------------+" << std::endl;
      out << "|                      PAW                                                    |" << std::endl;
      _sess.showOPTs(out,gopaw::cat_HPLOPT());

      out << "+-----------------------------------------------------------------------------+" << std::endl;
      out << "|                      GOPAW                                                  |" << std::endl;
      _sess.showOPTs(out,gopaw::cat_GOPAW_OPT());


    } else if(CHOPTN=="*") {

      opts.set_all_to_default();

    } else {
      if(CHOPTN=="STAT") CHOPTN = "STA"; //STAT accepted by PAW !
      else if(CHOPTN=="NSTAT") CHOPTN = "NSTA"; //NSTAT accepted by PAW !

      std::string key = opts.key_boolean(CHOPTN);
      if(key==opts.key_not_found()) {
        out << "pagraf :" 
            << " " << inlib::sout(CHOPTN) << " is not an option." 
            << std::endl;
      } else {

        opts.set_value(key,CHOPTN);
 
               if( (CHOPTN=="NGRI") || (CHOPTN=="GRID")) {
        } else if( (CHOPTN=="BOX") || (CHOPTN=="NBOX")) {
        } else if( (CHOPTN=="HTIT") || (CHOPTN=="UTIT")) {
        } else if( (CHOPTN=="NSTA") || (CHOPTN=="STA")) {
        } else if( (CHOPTN=="NBAR") || (CHOPTN=="BAR")) {
        } else if( (CHOPTN=="LINX") || (CHOPTN=="LOGX")) {
        } else if( (CHOPTN=="LINY") || (CHOPTN=="LOGY")) {
        } else if( (CHOPTN=="LINZ") || (CHOPTN=="LOGZ")) {
        } else if( (CHOPTN=="NFIT") || (CHOPTN=="FIT")) {

        //gopaw :
        } else if( (CHOPTN=="COLL") || (CHOPTN=="COLV")) {
        } else if( (CHOPTN=="PTIT") || (CHOPTN=="RTIT")) {
        } else if( (CHOPTN=="PSTA") || (CHOPTN=="RSTA")) {
        } else if( (CHOPTN=="NBAC") || (CHOPTN=="BACK")) {
        } else if( (CHOPTN=="BGRD") || (CHOPTN=="NBGR")) {
 
        } else {
          out << "pagraf :" 
              << " option " << inlib::sout(CHOPTN) 
              << " is a valid PAW one, but not yet supported in gopaw." 
              << std::endl;
        }

      }
    }

  } else if(cmd_path=="/GRAPHICS/MISC/CLR") {

    inlib::sg::plots* _page = _sess.find_plots(gopaw::s_current());
    if(_page) _page->clear();

  } else if(cmd_path=="/GRAPHICS/METAFILE") {

    int iLUN = ku_geti();
    int METAFL = ku_geti();

    if(_sess.verbose_level()) {
      out << "pagraf : " << cmd_path
          << " LUN= " << iLUN
          << " METAFL= " << METAFL
          << std::endl;
    }
    
    //METAFL= 4 Appendix E GKS. 
    //METAFL=-111 HIGZ/PostScript (Portrait). 
    //METAFL=-112 HIGZ/PostScript (Landscape). 
    //METAFL=-113 HIGZ/Encapsulated PostScript. 
    //METAFL=-114 HIGZ/PostScript Colour (Portrait). 
    //METAFL=-115 HIGZ/PostScript Colour (Landscape). 
    //METAFL=-777 HIGZ/LaTex Encapsulated. 
    //METAFL=-778 HIGZ/LaTex.

    if( (METAFL!=-111) &&
        (METAFL!=-112) &&
        (METAFL!=-113) &&
        (METAFL!=-114) &&
        (METAFL!=-115) ){
      out << "pagraf :" 
          << " METAFL " << METAFL
          << " is a valid PAW one, but not yet supported in gopaw." 
          << std::endl;
      return;
    }

    //LUN =-10 output only on metafile opened on unit 10;
    //LUN =  0 output only on screen;
    //LUN = 10 output on both screen and metafile opened on unit 10;

    unsigned int uLUN = iLUN<0?-iLUN:iLUN;

    gopaw::LUN* _lun;
    if(!_sess.find_LUN(uLUN,_lun)) {
      out << "pagraf : unknown unit " << uLUN << std::endl;
      return;
    }

    if((_lun->m_type==gopaw::LUN::SYS_FILE) && _lun->m_FILE ) {
      _lun->m_is_meta = true; //see pafort/CLOSE.
    }
    
    _sess.warn_metaf_ps();

  } else {
    out << "pagraf : " << cmd_path << " : dummy." << std::endl;
  }
}
