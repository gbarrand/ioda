#ifdef HEPVIS_HAS_TTF2

// this :
#include <HEPVis/SbTextTTF2.h>

// Inventor :
#include <Inventor/SbBox.h>
#include <Inventor/SbPList.h>
#include <Inventor/errors/SoDebugError.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/SbText.h>

#include <string.h> //strtok
#include <list>

#include FT_GLYPH_H

//////////////////////////////////////////////////////////////////////////////
/// Face cache ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class SbTextTTF2Face {
public:
  static FT_Library* getEngine();
  static void deleteEngine();
public:
  SbTextTTF2Face();
  virtual ~SbTextTTF2Face();
  SbBool loadFont(const SbString&); 
private:
  static FT_Library* fEngine;
  SbBool fIsValid;
public: //FIXME : private:
  SbString fFontName;  
  FT_Face fFace;
  FT_UInt* fGlyphIndices;
};
FT_Library* SbTextTTF2Face::fEngine = 0; //singletion = beurk.
//////////////////////////////////////////////////////////////////////////////
FT_Library* SbTextTTF2Face::getEngine()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fEngine) return fEngine;
  fEngine = new FT_Library;
  FT_Error error = FT_Init_FreeType(fEngine);
  if(error) {
    SoDebugError::post("SoTextTTFFace::getEngine",
                       "error (0x%x) : could not initialise FreeType",error);
    delete fEngine;
    fEngine = 0;
  }
  return fEngine;
}
//////////////////////////////////////////////////////////////////////////////
void SbTextTTF2Face::deleteEngine(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!fEngine) return;
  FT_Done_FreeType(*fEngine);
  delete fEngine;
  fEngine = 0;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SbTextTTF2Face::SbTextTTF2Face()
:fIsValid(FALSE)
,fGlyphIndices(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
SbTextTTF2Face::~SbTextTTF2Face(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==TRUE) {
    FT_Done_Face(fFace);
    delete [] fGlyphIndices;
  }
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF2Face::loadFont(
 const SbString& aFontName
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==TRUE) {
    FT_Done_Face(fFace);
    delete [] fGlyphIndices;
    fIsValid = FALSE;
  }
  if(!getEngine()) return FALSE;

  const char* filename = aFontName.getString();
  //printf("debug : SbTextTTF2Face::loadFont : \"%s\"\n",filename);

  char* ttf_path = ::getenv("TTFPATH");
  SbString ttfpath = ttf_path==NULL ? "" : ttf_path;
  //SoDebugError::post
  //  ("SbTextTTF2Face::loadFont","TTFPATH \"%s\"",ttfpath.getString());
  if(ttfpath=="") {
    SbString fullName = filename;
    if( (strstr(filename,".ttf")==NULL) && (strstr(filename,".TTF")==NULL) )
       fullName += ".ttf";
    FT_Error error = FT_New_Face(*getEngine(),fullName.getString(),0,&fFace);
    if(error) {
      SoDebugError::post("SbTextTTF2Face::loadFont",
                         "error (0x%x) : could not find or open file \"%s\"", 
                         error,filename);
      return FALSE;
    }
  } else {
    char* path = new char[ttfpath.getLength()+1];
    path = ::strcpy(path,ttfpath.getString());
    SbPList paths;
#ifdef _WIN32
    char* token = ::strtok(path,";");
    do {
      paths.append(new SbString(token));
    } while( (token = ::strtok(NULL,";"))!=NULL);
#else
    char* token = ::strtok(path,":");
    do {
      paths.append(new SbString(token));
    } while( (token = ::strtok(NULL,":"))!=NULL);
#endif
    delete [] path;
    SbBool found = FALSE;

    int index;
    for(index=0;index<paths.getLength();index++) {
      SbString fullName(*((SbString*)paths[index]));
#ifdef _WIN32
      fullName += "\\";
#else
      fullName += "/";
#endif
      fullName += filename;
      if( (strstr(filename,".ttf")==NULL) && 
          (strstr(filename,".TTF")==NULL) )
        fullName += ".ttf";
      FT_Error error = FT_New_Face(*getEngine(),fullName.getString(),0,&fFace);
      if(error) continue;
      found = TRUE;
      break;
    }

    for(index=0;index<paths.getLength();index++) 
      delete ((SbString*)paths[index]);

    if(found==FALSE) {
      SoDebugError::post("SbTextTTF2Face::loadFont",
        "error : could not find or open file \"%s\" in TTFPATH \"%s\"", 
        filename,ttfpath.getString());
      return FALSE;
    } 
  }

  unsigned short encodingOffset = 0;
  // Look for a Unicode charmap :
  FT_Int n = fFace->num_charmaps;
  FT_Int i;
  for ( i = 0; i < n; i++ ) {
    FT_CharMap charmap = fFace->charmaps[i];
    unsigned short platform = charmap->platform_id;
    unsigned short encoding = charmap->encoding_id;
    if ( (platform == 3 && encoding == 1 )  ||
         (platform == 3 && encoding == 0 )  ||
         //(platform == 1 && encoding == 0 )  ||
         (platform == 0 && encoding == 0 ) ) {
        FT_Error error = FT_Set_Charmap( fFace, charmap);
        if(error) {
          SoDebugError::post("SbTextTTF2Face::loadFont",
                         "error (0x%x) : can't set charmap for font \"%s\"", 
                         error,filename);
          return FALSE;
        }
        // For symbol.ttf and wingding.ttf
        if (platform == 3 && encoding == 0 ) encodingOffset = 0xF000;
        //if (platform == 1 && encoding == 0 ) encodingOffset = 0xF000;
        break;
    } else {
      //SoDebugError::post("SbTextTTF2Face::loadFont",
      //  "for \"%s\", platform %d and encoding %d not taken into account",
      //  filename,platform,encoding);
    }
  }
  if ( i == n ) {
    SoDebugError::post("SbTextTTF2Face::loadFont",
                      "This font doesn't contain any Unicode mapping table");
    FT_Done_Face(fFace);
    return FALSE;
  } else {
    fGlyphIndices = new FT_UInt[256];
    for( int ascii_code=0;ascii_code<256;++ascii_code) {
      int glyph_index = 
        FT_Get_Char_Index( fFace, ascii_code+encodingOffset);
      if((glyph_index<0)||(glyph_index>=fFace->num_glyphs)) {
        SoDebugError::post("SbTextTTF2Face::loadFont",
          "Can't get glyph index for ascii char %d in font file \"%s\".",
          ascii_code,filename);
        FT_Done_Face(fFace);
        delete [] fGlyphIndices;
        return FALSE;
      }
      fGlyphIndices[ascii_code]= glyph_index;
    }
  }

  fFontName = aFontName;

  fIsValid = TRUE;
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
static SbTextTTF2Face* findFont(const SbString&);
static std::list<SbTextTTF2Face*>* sFontCache = 0;
//////////////////////////////////////////////////////////////////////////////
SbTextTTF2Face* findFont(
 const SbString& aName
)
//////////////////////////////////////////////////////////////////////////////
// Handle a simple font cache.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!sFontCache) sFontCache = new std::list<SbTextTTF2Face*>();
  std::list<SbTextTTF2Face*>::iterator it;
  for(it=sFontCache->begin();it!=sFontCache->end();++it) {
    SbTextTTF2Face* face = *it;
    if(face->fFontName==aName) return face;
  }
  // Not found create one :
  SbTextTTF2Face* face = new SbTextTTF2Face();
  if(face->loadFont(aName)==FALSE) {
    delete face;
    return 0;
  } else {
    sFontCache->push_back(face);
    return face;
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SbTextTTF2::SbTextTTF2(
  const SbString& aName
)
:fTTF_Face(0)
,fIsValid(FALSE)
,fResolution(72)
,fPointSize(0)
,fAscent(0)
,fDescent(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTTF_Face = findFont(aName);
  if(!fTTF_Face) return;
  fIsValid = TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbTextTTF2::~SbTextTTF2(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //FIXME : unref fTTF_Face;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF2::isValid(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fIsValid;
}
//////////////////////////////////////////////////////////////////////////////
int SbTextTTF2::getAscent(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAscent;
}
//////////////////////////////////////////////////////////////////////////////
int SbTextTTF2::getDescent(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fDescent;
}
//////////////////////////////////////////////////////////////////////////////
SbString SbTextTTF2::getFontName(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!fTTF_Face) return "";
  return fTTF_Face->fFontName;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF2::getStringBounds(
 const SbString& aString
,const SbString& aEncoding
,FT_BBox& aBox
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aBox.xMin = 0;
  aBox.yMin = 0;
  aBox.xMax = 0;
  aBox.yMax = 0;
  if(fIsValid==FALSE) return FALSE;

  FT_Face face = fTTF_Face->fFace;

  FT_Error error = 
    FT_Set_Char_Size(face,fPointSize*64,fPointSize*64,
                          fResolution,fResolution);
  if (error) {
    SoDebugError::post("SbTextTTF2::getStringBound",
                       "error (0x%x) : Could not set char size",
                       error);
    return FALSE;
  }
  
  FT_Pos lineWidth = 0;
  FT_Pos yMax = 0;
  FT_Pos yMin = 0;

  SbEncodedString sbs = aEncoding=="PAW" ? 
    SbText::decodePAW(aString) : SbText::decodePlain(aString);
  unsigned int n = sbs.size();
  for(unsigned int index=0;index<n;index++) {
    const SbChar& sbChar = sbs[index];
    char c = sbChar.fChar;

    FT_UInt ft_index = fTTF_Face->fGlyphIndices[c];

    int flags = FT_LOAD_DEFAULT;
    flags |= FT_LOAD_NO_HINTING;
    error = FT_Load_Glyph(face,ft_index,flags);
    if (error) {
      SoDebugError::post("SbTextTTF2::getStringBound",
        "%ld : error (0x%x) for char '%c' in \"%s\" index %d",
        this,error,c,aString.getString(),ft_index);
      return FALSE;
    }

    FT_Glyph glyph;
    error = FT_Get_Glyph (face->glyph, &glyph);
    if (error) {
      SoDebugError::post("SbTextTTF2::getStringBound",
                         "error (0x%x) : could not get glyph",error);
      return FALSE;
    }

    FT_BBox bbox;
    FT_Glyph_Get_CBox(glyph, ft_glyph_bbox_pixels, &bbox);

    if(sbChar.fBack==FALSE)
      lineWidth += face->glyph->advance.x;

    FT_Done_Glyph(glyph);

    //FIXME : handle sbChar.fYMove.

    yMax = SbMaximum(yMax,bbox.yMax*64);
    yMin = SbMinimum(yMin,bbox.yMin*64);

  }


  aBox.xMin = 0;
  aBox.yMin = yMin;
  aBox.xMax = lineWidth;
  aBox.yMax = yMax;

  //printf("debug : %s : %d %d %d %d\n",
    //aString.getString(),aBox.xMin,aBox.yMin,aBox.xMax,aBox.yMax);

  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF2::getBounds(
 int aNumber
,const SbString* aLines
,const SbString& aEncoding
,SbBox2s& aBox
) const
//////////////////////////////////////////////////////////////////////////////
// Assume that font is inited.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aBox.makeEmpty();
  if(fIsValid==FALSE) return FALSE;

  int HEIGHT = fAscent+fDescent;
  int Y_ADVANCE = 2 * HEIGHT; //Same as SoTextHershey.

  FT_Pos textUP = 0;
  FT_Pos textWidth = 0;
 
  for(int count=0;count<aNumber;count++) {
    FT_BBox box;
    if(getStringBounds(aLines[count],aEncoding,box)==FALSE) return FALSE;

    FT_Pos lineWidth = box.xMax - box.xMin;
    textWidth = SbMaximum(textWidth,lineWidth);

    if(count==0) textUP = box.yMax; // First line.
  }
  aBox.setBounds(0,                        //xmin
                 -(aNumber-1) * Y_ADVANCE, //ymin
                 short(textWidth/64),      //xmax
                 short(textUP/64));        //ymax

  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF2::setPointSize(
 int aPointSize
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==FALSE) return FALSE;

  fPointSize = aPointSize;

  //FIXME : have a better way to get ascent, descent.
  // ABCDEFGHIJKLMNOPQRSTUVWXYZ
  // abcdefghijklmnopqrstuvwxyz
  // 0123456789
  // !\"#$%&'()*+,-./:;<=>?@[\\]^_`{}~
  // Take some characters with maximum ymax, ymin :
  //SbString ss = "Ay1_{\";
  SbString ss = "_/";
  FT_BBox box;
  getStringBounds(ss,"",box);

  fAscent = box.yMax/64;
  fDescent = -box.yMin/64;

  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
int SbTextTTF2::getPointSize(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==FALSE) return 0;
  return fPointSize;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF2::getCharacter(
 char aChar 
,SbBool aSmoothing
,SbBool aHinting
,SbTTF_BBox& aBox
,int& aAdvance
,SbTTF_Raster_Map& aRaster
) const
//////////////////////////////////////////////////////////////////////////////
// Convert an ASCII string to a string of glyph indexes.                    //
//                                                                          //
// IMPORTANT NOTE:                                                          //
//                                                                          //
// There is no portable way to convert from any system's char. code         //
// to Unicode.  This function simply takes a char. string as argument       //
// and "interprets" each character as a Unicode char. index with no         //
// further check.                                                           //
//                                                                          //
// This mapping is only valid for the ASCII character set (i.e.,            //
// codes 32 to 127); all other codes (like accentuated characters)          //
// will produce more or less random results, depending on the system        //
// being run.                                                               //
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aBox.xMin = 0;
  aBox.yMin = 0;
  aBox.xMax = 0;
  aBox.yMax = 0;
  aAdvance = 0;

  if(fIsValid==FALSE) return FALSE;
  if(fPointSize<=0) return FALSE;

  FT_Face face = fTTF_Face->fFace;

  FT_Error error = FT_Set_Char_Size(face,fPointSize*64,fPointSize*64,
                                         fResolution,fResolution);
  if (error) {
    SoDebugError::post("SbTextTTF2::getCharacter",
                       "error (0x%x) : Could not set char size",
                       error);
    return FALSE;
  }
  
  FT_UInt ft_index = fTTF_Face->fGlyphIndices[aChar];

  //SoDebugError::postInfo("SbTextTTF2::getCharacter","debug 001");

  int flags = FT_LOAD_DEFAULT;
  if(aHinting==FALSE) flags |= FT_LOAD_NO_HINTING;
  error = FT_Load_Glyph( face, ft_index, flags );
  if (error) {
    SoDebugError::post("SbTextTTF2::getCharacter",
                       "error (0x%x)",error);
    return FALSE;
  }

  FT_Glyph glyph;
  error = FT_Get_Glyph (face->glyph, &glyph);
  if (error) {
    SoDebugError::post("SbTextTTF2::getCharacter",
                       "error (0x%x) : could not get glyph",error);
    return FALSE;
  }

  error = FT_Glyph_To_Bitmap( &glyph, 
    (aSmoothing==TRUE ? ft_render_mode_normal : ft_render_mode_mono),
    0,1);
  if (error) {
    const char* filename = getFontName().getString();
    SoDebugError::post("SbTextTTF2::getCharacter",
      "error (0x%x) : could not get glyph bitmap for character '%c' of font \"%s\" and point size %d",
      error,aChar,filename,fPointSize);
    FT_Done_Glyph(glyph);
    return FALSE;
  }

/*typedef struct  FT_Bitmap_
  {
    int             rows;
    int             width;
    int             pitch;
    unsigned char*  buffer;
    short           num_grays;
    char            pixel_mode;
    char            palette_mode;
    void*           palette;
  } FT_Bitmap;*/

  FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyph;

  /*printf("debug : pixmap : %c : r %d  w %d p %d : grays %d\n",
      aChar,
      bitmap->bitmap.rows,bitmap->bitmap.width,bitmap->bitmap.pitch,
      bitmap->bitmap.num_grays);*/

  if( (bitmap->bitmap.pixel_mode!=ft_pixel_mode_mono) &&
      (bitmap->bitmap.pixel_mode!=ft_pixel_mode_grays) ){
    SoDebugError::post("SbTextTTF2::getCharacter",
      "not a mono or grays pixmap for character '%c' (mode %d)",
      aChar,bitmap->bitmap.pixel_mode);
    FT_Done_Glyph(glyph);
    return FALSE;
  } 

  if(bitmap->bitmap.pitch<0) {
    SoDebugError::post("SbTextTTF2::getCharacter",
      "negative bitmap pitch for character '%c' (mode %d)",
      aChar,bitmap->bitmap.pixel_mode);
    FT_Done_Glyph(glyph);
    return FALSE;
  } 

  if(bitmap->bitmap.pixel_mode==ft_pixel_mode_mono) {
    aRaster.rows = bitmap->bitmap.rows;
    aRaster.width = bitmap->bitmap.width;
    //WARNING : bitmap->bitmap.pitch != int((bitmap->bitmap.width+7)/8) !!!
    // OpenGL wants the below for cols.
    aRaster.cols = (aRaster.width+7)/8;
    aRaster.size = aRaster.rows * aRaster.cols;
    aRaster.grays = 1;
  } else { //ft_pixel_mode_grays

    if(bitmap->bitmap.width!=bitmap->bitmap.pitch) {
      SoDebugError::post("SbTextTTF2::getCharacter",
        "bitmap pitch (%d) != width (%d) for character '%c'",
        aChar,bitmap->bitmap.pitch,bitmap->bitmap.width);
      FT_Done_Glyph(glyph);
      return FALSE;
    } 

    aRaster.rows = bitmap->bitmap.rows;
    aRaster.width = bitmap->bitmap.width;
    aRaster.cols = aRaster.width;
    aRaster.size = aRaster.rows * aRaster.cols;
    aRaster.grays = bitmap->bitmap.num_grays;
  }

  if(aRaster.size<=0) {
    // This may happen (for example for the space character).
    aRaster.buffer = 0;
  } else {
    aRaster.buffer = new char[aRaster.size];
    if(!aRaster.buffer) {
      SoDebugError::post("SbTextTTF2::getCharacter",
        "can't alloc bitmap buffer for character '%c'",aChar);
      FT_Done_Glyph(glyph);
      return FALSE;
    } 
    // The bitmap is upside down for OpenGL.
    for(int row=0;row<aRaster.rows;++row) {
      unsigned char* from = (unsigned char*)bitmap->bitmap.buffer + 
        (bitmap->bitmap.rows-row-1)*bitmap->bitmap.pitch;
      unsigned char* to = 
        (unsigned char*)aRaster.buffer + row * aRaster.cols;
      for( int col=0;col<aRaster.cols;++col) to[col] = from[col];
    }
  }

  FT_BBox bbox;
  FT_Glyph_Get_CBox(glyph,ft_glyph_bbox_pixels,&bbox);

  aBox.xMin = int(bbox.xMin);
  aBox.yMin = int(bbox.yMin);
  aBox.xMax = int(bbox.xMax);
  aBox.yMax = int(bbox.yMax);
  aAdvance = int(face->glyph->advance.x/64);

  FT_Done_Glyph(glyph);

/*
  if(bitmap->bitmap.pixel_mode==ft_pixel_mode_mono) {
    if((aChar=='T')||(aChar=='a')||(aChar=='o')||(aChar=='L')) {
      printf("bitmap for '%c' : w = %d h = %d cols %d : ptsize = %d\n",
        aChar,aRaster.width,aRaster.rows,aRaster.cols,getPointSize());
      for( int row = (aRaster.rows-1); row >=0; --row ) {
        unsigned char* bline = 
          (unsigned char*)aRaster.buffer + row * aRaster.cols;
        int icol = 0;
        int ibit = 0;
        unsigned char byte = (unsigned char)bline[icol];
        icol++;
        for( int i= 0; i < aRaster.width; ++i ) {
          unsigned char v =  (byte & (1<<(7-ibit)));
          printf("%c",(v?'x':' '));
          ibit++;
          if(ibit==8) {
            ibit = 0;
            byte = (unsigned char)bline[icol];
            icol++;
          }
        }
        printf("\n");
      }
    }
  } else {
    if(aChar=='E') {
      printf
      ("bitmap for '%c' : w = %d h = %d cols %d grays %d : ptsize = %d\n",
        aChar,
        aRaster.width,aRaster.rows,aRaster.cols,aRaster.grays,
        getPointSize());
      unsigned char* p_buffer = (unsigned char*)aRaster.buffer;
      for(int j=0;j<aRaster.rows;++j) {
        for(int i=0;i<aRaster.width;++i) {
          unsigned char k = p_buffer[i]; //[0,grays-1]
          if(k>=128) {
            printf("x");
          } else if(k>=64) {
            printf("o");
          } else {
            printf(" ");
          }
        }
        p_buffer += aRaster.cols;
        printf("\n");
      }
    }
  }*/

  return TRUE;

}


#else
char SbTextTTF2_quiet_compiler = 0;
#endif
