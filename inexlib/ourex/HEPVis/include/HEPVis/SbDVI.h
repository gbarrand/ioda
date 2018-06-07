#ifndef HEPVis_SbDVI_h
#define HEPVis_SbDVI_h

/*
 * @author Laurent Garnier
 */


#include <Inventor/SbString.h>
#include <string>

// bitmap_info keeps together all the detailed information about the
// bitmap to be written.
class bitmap_info {
public:
    bitmap_info()
	: blur_bitmap(false), crop_bitmap(true),
	  make_transparent(true), bitmap_scale_factor(1) { }
public:
    bool blur_bitmap;
    bool crop_bitmap;
    bool make_transparent;
    int bitmap_scale_factor;
    std::string ofile_pattern;
    std::string ofile_name;
    std::string ofile_type;
};
#define DVI2BITMAPURL "http://www.astro.gla.ac.uk/users/norman/star/dvi2bitmap/"

class DviFile;
class Bitmap;

class SbDVI {
public:
  SbDVI();
  virtual ~SbDVI();

  unsigned char* getImageFromStream(const SbString&,
                            unsigned int&,unsigned int&,
                            int&,double*&,double*&,double*&,int&);
  void setFontGen(bool);
  void setForeground(const SbString&);
  void setBackground(const SbString&);
  void setTransparent(bool r);
  void setBlur(bool r);
  void setBitmapScaleFactor(int r);
  void setResolution(int r);
  
    private:
  bool process (const SbString&);
  bool process_dvi_file (DviFile& dvif);
  bool process_special (DviFile&,const std::string&);

private:
  Bitmap * fBitmap;
  int fResolution;
  double fMagnitude;
  //std::string fMFontMode;
  SbString fBgValue;
  SbString fFgValue;
  bool fFontGen;
  bitmap_info fBm;
  int fOneInch;
};

#endif



