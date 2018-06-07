/* 
 *  Created by Laurent Garnier on Wen May 18 2005.
 *  From dvi2bitmap/dvi2bitmap.cc code.
 */ 

// this :
#include <HEPVis/SbDVI.h>
#include <Inventor/SbString.h>
#include <Inventor/errors/SoDebugError.h>

#ifdef HEPVIS_HAS_DVI2BITMAP
#include <dvi2bitmap/PkFont.h>
#include <dvi2bitmap/Bitmap.h>
#include <dvi2bitmap/DviFile.h>
#include <dvi2bitmap/Util.h>
#endif

#include <math.h>

//////////////////////////////////////////////////////////////////////////////
SbDVI::SbDVI(
)
:fBitmap(0)
,fResolution(300)
,fMagnitude(1.0)
,fBgValue("#000000")
,fFgValue("#FFFFFF")
,fFontGen(false)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fBm.bitmap_scale_factor = 1;
  fBm.make_transparent = true;
  fBm.blur_bitmap = true;
}
//////////////////////////////////////////////////////////////////////////////
SbDVI::~SbDVI (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef HEPVIS_HAS_DVI2BITMAP
  delete fBitmap;
  fBitmap = 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////
unsigned char* SbDVI::getImageFromStream(
 const SbString& aDviname
,unsigned int& aWidth
,unsigned int& aHeight
,int& aColorNumber
,double*& aReds
,double*& aGreens
,double*& aBlues
,int& aTransparent
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef HEPVIS_HAS_DVI2BITMAP
  if (!process(aDviname) || !fBitmap) {
    aWidth = 0;
    aHeight = 0;
    aColorNumber = 0;
    aReds   = 0;
    aGreens = 0;
    aBlues  = 0;
    aTransparent = 0;
    return 0;
  }
  unsigned char* data = 0;

  // build image
  aTransparent = (fBm.make_transparent ?0:-1);

  // get the bitmap
  Bitmap::BitmapColour* rgb;
  fBitmap->getColorTable(rgb,aColorNumber);

  aReds   = new double[aColorNumber];
  aGreens = new double[aColorNumber];
  aBlues  = new double[aColorNumber];

  for (int a=0;a<aColorNumber;a++) {
    aReds[a]   = static_cast<double>(rgb[a].red)/256;
    aGreens[a] = static_cast<double>(rgb[a].green)/256;
    aBlues[a]  = static_cast<double>(rgb[a].blue)/256;
  }
  Byte *by;
  int cropB,cropT,cropL,cropR;
  fBitmap->getBitmap(by,cropB,cropT,cropL,cropR);
  aHeight = cropB-cropT;
  aWidth = cropR-cropL;
  data = new unsigned char[aWidth*aHeight];
  unsigned int indice = 0;
  for(int a=cropT;a<cropB;a++) {
    for(int b=cropL;b<cropR;b++) {
      data[indice] = by[a*fBitmap->getWidth()+b];
      indice ++;
    }
  }

  delete fBitmap;
  fBitmap = 0;

  return data;
#else
  aDviname.getLength(); //to have no warning.
  aWidth = 0;
  aHeight = 0;
  aColorNumber = 0;
  aReds   = 0;
  aGreens = 0;
  aBlues  = 0;
  aTransparent = 0;
  return 0;
#endif
}


//////////////////////////////////////////////////////////////////////////////
bool SbDVI::process (
 const SbString& aDviname
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef HEPVIS_HAS_DVI2BITMAP
  PkFont::setResolution (fResolution);
  int resolution = PkFont::dpiBase();	// one inch, including magnification
  fOneInch = resolution;	// one inch, including magnification

  Bitmap::BitmapColour rgb;
  if (Util::parseRGB(rgb, fBgValue.getString()))
    Bitmap::setDefaultRGB (0, &rgb);
  if (Util::parseRGB(rgb, fFgValue.getString()))
    Bitmap::setDefaultRGB (1, &rgb);
  
  if (fFontGen) {
    PkFont::setFontgen(true);
  } else {
    PkFont::setFontgen(false);
  }

  std::string name = aDviname.getString();
  bool fonts_ok = true;	// are there accumulated font errors?
  bool status = false;
  try
    {
      DviFile *dvif = new DviFile(name,
                                  fResolution, fMagnitude,
                                  true,true);
      
      bool all_fonts_present = true; // are all expected fonts found?
      
      if (dvif->haveReadPostamble()) {
        all_fonts_present = true;
        bool no_font_present = true; // are no expected fonts found?
        
        const DviFile::FontSet* fontset = dvif->getFontSet();
        for (DviFile::FontSet::const_iterator ci = fontset->begin();
             ci != fontset->end();
             ++ci)
          {
            const PkFont *f = *ci;
            if (f->loaded()) {
              no_font_present = false;
            }
            else		// flag at least one missing
              all_fonts_present = false;
          }
        
        // Font loading is deemed `successful' if either all the
        // fonts are loaded, or failing that, if it's not the case
        // that _no_ fonts were loaded.  Note that if there were
        // _no_ fonts listed in the postamble (an odd DVI file,
        // but not impossible), then both all_fonts_present and
        // no_font_present are true, and this expression evaluates
        // to true.
        fonts_ok = all_fonts_present || !no_font_present;
      } else {
        // haven't read postamble
        fonts_ok = true; // don't know any better
      }
      if (fonts_ok) status = process_dvi_file(*dvif);
      delete dvif;
    }
  catch (DviBug& e)
    {
      SoDebugError::post("SbDvi::process",
                         "Build dvi :%s",
                         e.problem().c_str());
      status = false;
    }
  catch (DviError& e)
    {
      SoDebugError::post("SbDvi::process",
                         "Dvi bug :%s",
                         e.problem().c_str());
      status = false;
    }
  
  return status;
#else
  aDviname.getLength(); //to have no warning.
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////////////
void SbDVI::setFontGen(
 bool r
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fFontGen = r;
}


//////////////////////////////////////////////////////////////////////////////
void SbDVI::setTransparent(
 bool r
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fBm.make_transparent = r;
}
//////////////////////////////////////////////////////////////////////////////
void SbDVI::setBlur(
 bool r
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fBm.blur_bitmap = r;
}
//////////////////////////////////////////////////////////////////////////////
void SbDVI::setBackground(
 const SbString& value
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fBgValue = value;
}
//////////////////////////////////////////////////////////////////////////////
void SbDVI::setForeground(
 const SbString& value
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fFgValue = value;
}
//////////////////////////////////////////////////////////////////////////////
void SbDVI::setResolution (
 int res
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef HEPVIS_HAS_DVI2BITMAP
  if (res == 110) {
    fResolution =110;
    PkFont::setMissingFontMode ("ibmvga");
  } else if (res == 300) {
    fResolution =300;
    PkFont::setMissingFontMode ("cx");
  } else if (res == 360) {
    fResolution =360;
    PkFont::setMissingFontMode ("lqhires");
  } else if (res == 180) {
    fResolution =180;
    PkFont::setMissingFontMode ("lqlores");
  } else {
    //  should not arrive, incompatible modes
  }
#else
  res = 0;
#endif
}
//////////////////////////////////////////////////////////////////////////////
void SbDVI::setBitmapScaleFactor(
 int r
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fBm.bitmap_scale_factor = r;
}

//////////////////////////////////////////////////////////////////////////////
bool SbDVI::process_dvi_file (
 DviFile& dvif
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef HEPVIS_HAS_DVI2BITMAP
  DviFileEvent *ev;
  const PkFont *curr_font = 0;
  int pagenum = 0;
  bool end_of_file = false;
  bool initialisedInch = false;
  delete fBitmap;
  fBitmap = 0;
  
  while (! end_of_file) {
    ev = dvif.getEvent();
    
    if (! initialisedInch)
      {
        // can't do this any earlier, as it's set in the preamble
        fOneInch = static_cast<int>(fResolution * dvif.magnification());
        initialisedInch = true;
      }
    
    if (DviFilePage *test = dynamic_cast<DviFilePage*>(ev))
      {
        DviFilePage &page = *test;
        if (page.isStart)
          {
            pagenum++;
            
            // Request a big-enough bitmap; this bitmap is the `page'
            // on which we `print' below.  hSize and vSize are the
            // width and height of the widest and tallest pages,
            // as reported by the DVI file; however, the file doesn't
            // report the ofFsets of these pages.  Add a
            // couple of inches to both and hope for the best.
            // If we haven't read the postamble, then hSize()
            // and vSize() return negative, so make a rough guess.
            if (fBitmap == 0) {
              int bmw, bmh;
              if (dvif.hSize() < 0)
                bmw = 4*fOneInch;
              else
                bmw = dvif.hSize() + 2*fOneInch;
              if (dvif.vSize() < 0)
                bmh = 2*fOneInch;
              else
                bmh = dvif.vSize() + fOneInch;
              fBitmap = new Bitmap(bmw,bmh); // specify 8 Bytes per pixel
            } else {
              fBitmap->clear();
            }
          } else {
            // end of page
            if (fBitmap == 0) 
              return false;//throw DviBug ("bitmap uninitialised at page end");
            else {
              if (fBm.crop_bitmap)
                fBitmap->crop();
              if (fBm.blur_bitmap)
                fBitmap->blur();
              if (fBm.make_transparent) {
                fBitmap->setTransparent(true);
              }
              if (fBm.bitmap_scale_factor != 1)
                fBitmap->scaleDown (fBm.bitmap_scale_factor);

              // bitmap.write....
            }
          }
      }
    else if (DviFileSetChar *test = dynamic_cast<DviFileSetChar*>(ev))
      {
        if (curr_font == 0 || fBitmap == 0)
          return false; //throw DviBug ("font or bitmap not initialised setting char");
        DviFileSetChar& sc = *test;
        PkGlyph& glyph = *curr_font->glyph(sc.charno());
        // calculate glyph positions, taking into account the
        // offsets for the bitmaps, and the (1in,1in)=(72pt,72pt)
        // = (resolution px,resolution px) offset of the TeX origin.
        int x = dvif.currH(DviFile::unit_pixels) + glyph.hoff() + fOneInch;
        int y = dvif.currV(DviFile::unit_pixels) + glyph.voff() + fOneInch;
        fBitmap->paint (x, y,
                        glyph.w(), glyph.h(),
                        glyph.bitmap());
      }
    else if (DviFileSetRule *test = dynamic_cast<DviFileSetRule*>(ev))
      {
        DviFileSetRule& sr = *test;
        int x = dvif.currH() + fOneInch;
        int y = dvif.currV() + fOneInch;
        fBitmap->rule (x,y,sr.w, sr.h);
      }
    else if (DviFileFontChange *test =
             dynamic_cast<DviFileFontChange*>(ev))
      {
        DviFileFontChange& fc = *test;
        const PkFont *f = fc.font;
        if (f->loaded())
          curr_font = f;
        else {
          f = dvif.getFallbackFont(f);
          if (f != 0)
            curr_font = f;
        }
        // curr_font unchanged if font-change was unsuccessful
      }
    else if (DviFileSpecial* test =
             dynamic_cast<DviFileSpecial*>(ev))
      {
        DviFileSpecial& special = *test;
        if (!process_special (dvif,
                              special.specialString)) {
        //            cerr << "Warning: unrecognised special: "
        //                 << special.specialString
        //                 << endl;
        }
      }
    else if (DviFilePostamble *post
             = dynamic_cast<DviFilePostamble*>(ev))
      end_of_file = true;
    
    ev->release();
  }
  return true;
#else
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////////////
bool SbDVI::process_special (
 DviFile& dvif
,const std::string& specialString
 )
//////////////////////////////////////////////////////////////////////////////
// Process the special string, returning true on success.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef HEPVIS_HAS_DVI2BITMAP
    string_list l = Util::tokenise_string (specialString);
    string_list::const_iterator s = l.begin();
    bool stringOK = false;
    bool setDefault = false;
    bool absolute = false;

    // Define units used within specials.  special_unit is used in the
    // file, but if it's set with `default', then
    // default_special_unit is set, too.
    static DviFile::DviUnits default_special_unit = DviFile::unit_pt;
    DviFile::DviUnits special_unit = default_special_unit;

    //LAL : Laurent Garnier
    if (*s == "color") {
      Bitmap::BitmapColour rgb;
      s++;
      if (*s == "push") {
        s++;
        std::string color;
        while (s != l.end()) {
          color += std::string(*s)+" ";
          s++;
        }
        if (Util::parseColor(rgb,color))
          stringOK = fBitmap->pushColor(&rgb);
      } else if (*s == "pop") {
        stringOK = fBitmap->popColor();
      }
    } else if ((*s == "background") ||(*s == "foreground")) {
      bool fg = false;
      if (*s == "foreground") {
        fg =true;
      }
      Bitmap::BitmapColour rgb;
      s++;
      std::string color;
      while (s != l.end()) {
        color += std::string(*s)+" ";
        s++;
      }
      if (Util::parseColor(rgb,color)) {
        fBitmap->setRGB (fg, &rgb);
        stringOK = true;
      }
    //LAL : end
    } else if (*s == "dvi2bitmap") {	// OK
	stringOK = true;
	s++;

	while (s != l.end() && stringOK) {
	    if (*s == "default")
		setDefault = true;

	    else if (*s == "absolute")
		absolute = true;
	    else if (*s == "crop") {
		s++;
		if (s == l.end()) { stringOK = false; break; }
		std::string side_s = *s;
		Bitmap::Margin side = Bitmap::All;
		s++;
		if (s == l.end()) { stringOK = false; break; }
		int dimen = atoi (s->c_str());
		// scale from points to pixels
		double npixels = DviFile::convertUnits(static_cast<double>(dimen),
						       special_unit,
						       DviFile::unit_pixels,
						       &dvif);
// 		double npixels = dimen/72.0    // to inches
// 		    * fOneInch;
		if (absolute)
		{
		    // these dimensions are given w.r.t. an origin one inch
		    // from the left and top of the `paper'.  Add this inch:
		    npixels += fOneInch;
		}
		dimen = static_cast<int>(npixels);

		if (side_s == "left")
		    side = Bitmap::Left;
		else if (side_s == "right")
		    side = Bitmap::Right;
		else if (side_s == "top")
		    side = Bitmap::Top;
		else if (side_s == "bottom")
		    side = Bitmap::Bottom;
		else if (side_s == "all")
		    side = Bitmap::All;
		else
		    stringOK = false;

		if (stringOK)
		    if (side == Bitmap::All)
		    {
			if (setDefault)
			    for (int tside=0; tside<4; tside++)
				Bitmap::cropDefault
				    (static_cast<Bitmap::Margin>(tside),
				     dimen, absolute);
			for (int tside=0; tside<4; tside++)
			    fBitmap->crop
				(static_cast<Bitmap::Margin>(tside),
				 dimen, absolute);
		    }
		    else
		    {
			if (setDefault)
			    Bitmap::cropDefault (side, dimen, absolute);
			fBitmap->crop (side, dimen, absolute);
		    }
	    } else if (*s == "foreground" || *s == "background") {
		bool isfg = (*s == "foreground");
		//Byte r, g, b;
		Bitmap::BitmapColour rgb;
		s++;
		if (s == l.end()) { stringOK = false; break; }
		rgb.red   = static_cast<Byte>(strtol (s->c_str(), 0, 0));
		s++;
		if (s == l.end()) { stringOK = false; break; }
		rgb.green = static_cast<Byte>(strtol (s->c_str(), 0, 0));
		s++;
		if (s == l.end()) { stringOK = false; break; }
		rgb.blue  = static_cast<Byte>(strtol (s->c_str(), 0, 0));

		if (stringOK)
		{
		    if (setDefault)
			Bitmap::setDefaultRGB (isfg, &rgb);
		    else
			fBitmap->setRGB (isfg, &rgb);
		}
	    } else if (*s == "strut") {
		int x = dvif.currH() + fOneInch;
		int y = dvif.currV() + fOneInch;
		int strut_lrtb[4];
		for (int i=0; i<4; i++) {
		    s++;
		    if (s == l.end()) {
			stringOK = false;
			break;
		    }
		    strut_lrtb[i] = static_cast<int>
			    (DviFile::convertUnits(strtod(s->c_str(),0),
						   special_unit,
						   DviFile::unit_pixels,
						   &dvif)
			     + 0.5); // round to nearest pixel
		    if (strut_lrtb[i] < 0) {
                      /*
			if (verbosity > silent)
			    cerr << "Strut must have positive dimensions"
				 << endl;
			stringOK = false;
                      */
		    }
		}

		if (stringOK) {
		    fBitmap->strut (x, y,
				   strut_lrtb[0],
				   strut_lrtb[1],
				   strut_lrtb[2],
				   strut_lrtb[3]);
		}
	    } else if (*s == "unit") {
		s++;
		if (s == l.end()) {
		    stringOK = false;
		    break;	// JUMP OUT of special-processing loop
		}
		special_unit = DviFile::unitType(*s);
		if (setDefault)
		    default_special_unit = special_unit;
            } else if (*s == "mark") {
                // Set the mark at the current position.  This must
                // match the offset calculations we make when setting
                // characters handling DviFileSetChar event above.
                fBitmap->mark
                    (dvif.currH(DviFile::unit_pixels) + fOneInch,
                     dvif.currV(DviFile::unit_pixels) + fOneInch);
                
	    } else
		stringOK = false;

	    s++;
	}
    }
    return stringOK;
#else
  specialString.size();
  return false;
#endif
}
