// Copyright (C) 2018, Guy Barrand. All rights reserved.
// See the file gopaw.license for terms.

extern "C" {
  void pagksa_(void*);
}

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>
#include <kuip/klink.h>

#include <inlib/colormanip>
#include <inlib/sout>

#include "session"

void pagksa_(void* a_tag) { 
  gopaw::session& _sess = *((gopaw::session*)a_tag);

  std::ostream& out = _sess.out();
  std::string cmd_path = ku_path() ? ku_path() : "";
  //std::string cmd_name = ku_name();
  if(_sess.verbose_level()) {
    out << "pagksa :" 
        << " execute " << inlib::sout(cmd_path) 
        << std::endl;
  }

  //opts_t& opts = _sess.opts();
  gopaw::atts_t& atts = _sess.atts();

  if(cmd_path=="/GRAPHICS/ATTRIBUTES/COLOR_TABLE") {

    int ICOL = ku_geti();
    double RED = ku_getr();
    double GREEN = ku_getr();
    double BLUE = ku_getr();

    _sess.setColor(ICOL,RED,GREEN,BLUE);

  } else if(cmd_path=="/GRAPHICS/ATTRIBUTES/PALETTE") {

    int PALNB = ku_geti();  //CERN/PAW : PALNB is in [0,9] :
    int NEL = ku_geti();
    std::vector<int> LIST;   
   {for(int index=0;index<NEL;index++) {
      LIST.push_back(ku_geti());
    }}

    if(_sess.verbose_level()) {
      out << "pagksa :" 
          << " " << cmd_path
          << " PALNB= " << PALNB
          << " NEL= " << NEL
          << " LIST= ";
      for(unsigned int index=0;index<LIST.size();index++) {
        out << LIST[index] << " ";
      }
      out << std::endl;
    }
    if(PALNB<0) {
      out << "pagksa : " << cmd_path << " PALNB should be >=0 (PALNB=" << PALNB << ")." << std::endl;
      return;
    }

    if(NEL<=0) {
      int NCOL = (int)atts.value("NCOL");
      // If NCOL<=8 elements from the color table in :
      if(NCOL<=8) {
        std::vector<int> pal;
        pal.push_back(0);
        pal.push_back(5);
        pal.push_back(7);
        pal.push_back(3);
        pal.push_back(6);
        pal.push_back(2);
        pal.push_back(4);
        pal.push_back(1);
        pal.resize(NCOL);
        _sess.set_palette(PALNB,gopaw::palette(pal));
      } else {
        std::vector<int> pal;
        // Fill color_table elements [8,NCOL] with 
        // a blue to red scale (from blue to black) :
        int ncol = NCOL-8;
        if(ncol==1) {
          _sess.setColor(9,1,1,1);
          pal.push_back(9);
        } else {
          // Use the hsv colorwheel. We start from
          // blue (240,255,255) and turn around by using h 
          // to reach red (0,255,255) by passing by green (120,255,1)
          for(int index=0;index<ncol;index++) {
            // index 0  -> h =240
            // index ncol-1 -> h = 0
            int h = int(240 * (1 - ((float)index)/((float)(ncol-1))));
            int s = 255;
            int v = 255;
            int r,g,b;
            inlib::hsv2rgb(h,s,v,r,g,b);
            _sess.setColor(8+index,((float)r)/255,((float)g)/255,((float)b)/255);
            pal.push_back(8+index);
          }
        }
        _sess.set_palette(PALNB,gopaw::palette(pal));
      }
    } else {
      for(unsigned int index=0;index<LIST.size();index++) {
        _sess.createColor(LIST[index]);
      }
      _sess.set_palette(PALNB,gopaw::palette(LIST));
    }
    _sess.set_current_palette(PALNB);

  } else {
    out << "pagksa :" 
        << " " << cmd_path << " : dummy." << std::endl;
  }
}

