#ifdef HEPVIS_HAS_TTF1

// this :
#include <HEPVis/SbTextTTF1.h>

// Inventor :
#include <Inventor/SbBox.h>
#include <Inventor/SbPList.h>
#include <Inventor/errors/SoDebugError.h>

#include <HEPVis/SbString.h>
#include <HEPVis/SbMath.h>

#include <string.h> //strtok
#include <list>

//////////////////////////////////////////////////////////////////////////////
/// Face cache ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class SbTextTTF1Face {
public:
  static TT_Engine* getEngine();
  static void deleteEngine();
public:
  SbTextTTF1Face();
  virtual ~SbTextTTF1Face();
  SbBool loadFont(const SbString&); 
private:
  static TT_Engine* fEngine;
  SbBool fIsValid;
public: //FIXME : private:
  SbString fFontName;  
  TT_Face fFace;
  TT_CharMap fCharMap;
  TT_UShort* fGlyphIndices;
};
TT_Engine* SbTextTTF1Face::fEngine = 0; //singletion = beurk.
//////////////////////////////////////////////////////////////////////////////
TT_Engine* SbTextTTF1Face::getEngine()
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fEngine) return fEngine;
  fEngine = new TT_Engine;
  TT_Error error = TT_Init_FreeType(fEngine);
  if(error) {
    SoDebugError::post("SoTextTTFFace::getEngine",
                       "error (0x%x) : could not initialise FreeType",error);
    delete fEngine;
    fEngine = 0;
  }
  return fEngine;
}
//////////////////////////////////////////////////////////////////////////////
void SbTextTTF1Face::deleteEngine(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!fEngine) return;
  TT_Done_FreeType(*fEngine);
  delete fEngine;
  fEngine = 0;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SbTextTTF1Face::SbTextTTF1Face()
:fIsValid(FALSE)
,fGlyphIndices(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
SbTextTTF1Face::~SbTextTTF1Face(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==TRUE) {
    TT_Close_Face(fFace);
    delete [] fGlyphIndices;
  }
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF1Face::loadFont(
 const SbString& aFontName
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==TRUE) {
    TT_Close_Face(fFace);
    delete [] fGlyphIndices;
    fIsValid = FALSE;
  }
  if(!getEngine()) return FALSE;

  const char* filename = aFontName.getString();
  //printf("debug : SbTextTTF1Face::loadFont : \"%s\"\n",filename);

  char* ttf_path = ::getenv("TTFPATH");
  SbString ttfpath = ttf_path==NULL ? "" : ttf_path;
  if(ttfpath=="") {
    SbString fullName = filename;
    if( (strstr(filename,".ttf")==NULL) && (strstr(filename,".TTF")==NULL) )
       fullName += ".ttf";
    TT_Error error = TT_Open_Face(*getEngine(),fullName.getString(),&fFace);
    if(error) {
      SoDebugError::post("SbTextTTF1Face::loadFont",
                         "error (0x%x) : could not find or open file (%s)", 
                         error,filename);
      return FALSE;
    }
  } else {
    char* path = SbSTRDUP(ttfpath.getString());
    SbPList paths;
    char* token = ::strtok(path," ");
    do {
      paths.append(new SbString(token));
    } while( (token = ::strtok(NULL," "))!=NULL);
    SbSTRDEL(path);
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
      TT_Error error = TT_Open_Face(*getEngine(),fullName.getString(),&fFace);
      if(error) continue;
      found = TRUE;
      break;
    }

    for(index=0;index<paths.getLength();index++) 
      delete ((SbString*)paths[index]);

    if(found==FALSE) {
      SoDebugError::post("SbTextTTF1Face::loadFont",
                         "error : could not find or open file (%s)", 
                         filename);
      return FALSE;
    } 
  }

  // Look for a Unicode charmap. From gltt/FTFace.c.
  TT_UShort encodingOffset = 0;
  int n = TT_Get_CharMap_Count(fFace);
  int i;
  for ( i = 0; i < n; i++ ) {
    unsigned short platform;
    unsigned short encoding;
    TT_Get_CharMap_ID( fFace, i, &platform, &encoding );
    //printf("debug : %d (%d) %d encoding %d\n",i,n,platform,encoding);
    if ( (platform == 3 && encoding == 1 )  ||
         (platform == 3 && encoding == 0 )  ||
         //(platform == 1 && encoding == 0 )  ||
         (platform == 0 && encoding == 0 ) ) {
        TT_Get_CharMap( fFace, i, &(fCharMap) );
        // For symbol.ttf and wingding.ttf
        if (platform == 3 && encoding == 0 ) encodingOffset = 0xF000;
        //if (platform == 1 && encoding == 0 ) encodingOffset = 0xF000;
        break;
    } else {
      //SoDebugError::post("SbTextTTF1Face::loadFont",
      //  "for \"%s\", platform %d and encoding %d not taken into account",
      //  filename,platform,encoding);
    }
  }
  if ( i == n ) {
    SoDebugError::post("SbTextTTF1Face::loadFont",
      "This font (%s) doesn't contain any Unicode mapping table",filename);
    TT_Close_Face(fFace);
    return FALSE;
  } else {
    TT_Face_Properties  properties;
    TT_Error error = TT_Get_Face_Properties(fFace, &properties );
    if(error) {
      SoDebugError::post("SbTextTTF1Face::loadFont",
                         "error (0x%x) : could not get face properties",error);
      TT_Close_Face(fFace);
      return FALSE;
    }
    fGlyphIndices = new TT_UShort[256];
    for( int ascii_code=0;ascii_code<256;++ascii_code) {
      int glyph_index = 
        TT_Char_Index( fCharMap, ascii_code+encodingOffset);
      if((glyph_index<0)||(glyph_index>=properties.num_Glyphs)) {
        SoDebugError::post("SbTextTTF1Face::loadFont",
          "Can't get glyph index for ascii char %d in font file \"%s\".",
          ascii_code,filename);
        TT_Close_Face(fFace);
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
static SbTextTTF1Face* findFont(const SbString&);
static std::list<SbTextTTF1Face*>* sFontCache = 0;
//////////////////////////////////////////////////////////////////////////////
SbTextTTF1Face* findFont(
 const SbString& aName
)
//////////////////////////////////////////////////////////////////////////////
// Handle a simple font cache.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!sFontCache) sFontCache = new std::list<SbTextTTF1Face*>();
  std::list<SbTextTTF1Face*>::iterator it;
  for(it=sFontCache->begin();it!=sFontCache->end();++it) {
    SbTextTTF1Face* face = *it;
    if(face->fFontName==aName) return face;
  }
  // Not found create one :
  SbTextTTF1Face* face = new SbTextTTF1Face();
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
SbTextTTF1::SbTextTTF1(
  const SbString& aName
)
:fTTF_Face(0)
,fIsValid(FALSE)
,fAscent(0)
,fDescent(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTTF_Face = findFont(aName);
  if(!fTTF_Face) return;
  TT_Face face = fTTF_Face->fFace;

  TT_Error error = TT_New_Instance( face, &fInstance );
  if(error) {
    SoDebugError::post("SbTextTTF1::SbTextTTF1",
                       "error (0x%x) : could not create instance",error);
    return;
  }

  // Give screen resolution.
  // Device resolution = # of pixels/inch. 
  // Around 96 for screen (300 for printer).
  // 96 = 24 * 4     72 = 18 * 4
  error = TT_Set_Instance_Resolutions(fInstance, 96, 96 );
  //error = TT_Set_Instance_Resolutions(fInstance, 72, 72 );
  if (error) {
    SoDebugError::post("SbTextTTF1::SbTextTTF1Face",
                       "error (0x%x) : Could not set instance resolution",
                       error);
    TT_Done_Instance(fInstance);
    return;
  }
  fIsValid = TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbTextTTF1::~SbTextTTF1(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==TRUE) {
    TT_Done_Instance(fInstance);
  }
  //FIXME : unref fTTFace;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF1::isValid(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fIsValid;
}
//////////////////////////////////////////////////////////////////////////////
int SbTextTTF1::getAscent(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fAscent;
}
//////////////////////////////////////////////////////////////////////////////
int SbTextTTF1::getDescent(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fDescent;
}
//////////////////////////////////////////////////////////////////////////////
SbString SbTextTTF1::getFontName(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!fTTF_Face) return "";
  return fTTF_Face->fFontName;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF1::getStringBounds(
 const SbString& aString
,TT_BBox& aBox
,SbTTF_Skip aFunc
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aBox.xMin = 0;
  aBox.yMin = 0;
  aBox.xMax = 0;
  aBox.yMax = 0;
  if(fIsValid==FALSE) return FALSE;

  TT_Glyph glyph;
  TT_Error error = TT_New_Glyph( fTTF_Face->fFace, &(glyph) );
  if(error) {
    SoDebugError::post("SbTextTTF1::getStringBounds",
                       "error (0x%x) : could not create glyph container",
                       error);
    return FALSE;
  }

  const char* s = aString.getString();

  TT_Pos lineWidth = 0;
  TT_Pos yMax = 0;
  TT_Pos yMin = 0;
  for(const char* itema=s;*itema!='\0';itema++) { 
    SbBool CTRL,CR;
    if(aFunc) {
      aFunc(itema,CTRL,CR);
    } else {
      CTRL = FALSE;
      CR = (*(itema+1)=='\0') ? TRUE : FALSE;
    }
    if(CTRL==TRUE) continue;

    char c = *itema;

    short index = fTTF_Face->fGlyphIndices[c];

    int flags = TTLOAD_DEFAULT;
    error = TT_Load_Glyph( fInstance, glyph, index, flags );
    if (error) {
      SoDebugError::post("SbTextTTF1::getStringBound",
        "%ld : error (0x%x) for char '%c' in \"%s\" index %d",
        this,error,c,s,index);
      return FALSE;
    }
    TT_Glyph_Metrics  metrics;
    error = TT_Get_Glyph_Metrics( glyph, &metrics );
    if (error) {
      SoDebugError::post("SbTextTTF1::getStringBounds",
                         "error (0x%x) : Could not get glyph metrics",error);
      return FALSE;
    }

    yMax = SbMaximum(yMax,metrics.bbox.yMax);
    yMin = SbMinimum(yMin,metrics.bbox.yMin);

    lineWidth += metrics.advance;
  }

  aBox.xMin = 0;
  aBox.yMin = yMin;
  aBox.xMax = lineWidth;
  aBox.yMax = yMax;

  TT_Done_Glyph(glyph);
  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF1::getBounds(
 int aNumber
,const SbString* aLines
,SbBox2s& aBox
,SbTTF_Skip aFunc
) const
//////////////////////////////////////////////////////////////////////////////
// Assume that font is inited.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aBox.makeEmpty();
  if(fIsValid==FALSE) return FALSE;

  int HEIGHT = fAscent+fDescent;
  int Y_ADVANCE = 2 * HEIGHT; //Same as SoTextHershey.

  TT_Pos textUP = 0;
  TT_Pos textWidth = 0;
 
  for(int count=0;count<aNumber;count++) {
    TT_BBox box;
    if(getStringBounds(aLines[count],box,aFunc)==FALSE) return FALSE;

    TT_Pos lineWidth = box.xMax - box.xMin;
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
SbBool SbTextTTF1::setPointSize(
 int aPointSize
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==FALSE) return FALSE;

  // Give size of a character in "pt" :
  // 1 pt = 1/72 inch.
  // If <pointSize> the char will be : pointSize * 96 /72 pixels on screen.
  // Exa : 10 pt gives 10 * 96 /72 = 13 pixels on screen.
  // To have n pixels : pointSize = 72/96 * n = 0.75 * n.
  // 72/96 = 3/4 = 0.75
  // 96/72 = 4/3 = 1.333
  // Default point size is 10 <--> instanceMetrics.pointSize = 640.

  TT_Error error = TT_Set_Instance_PointSize(fInstance,aPointSize);
  if (error) {
    SoDebugError::post("SbTextTTF1::setPointSize",
                       "error (0x%x) : Could not set instance point size",
                       error);
    return FALSE;
  }

  error = TT_Get_Instance_Metrics(fInstance,&fInstanceMetrics);
  if (error) {
    SoDebugError::post("SbTextTTF1::setPointSize",
                       "error (0x%x) : Could not get instance metric",error);
    return FALSE;
  }

  //FIXME : have a better way to get ascent, descent.
 {// ABCDEFGHIJKLMNOPQRSTUVWXYZ
  // abcdefghijklmnopqrstuvwxyz
  // 0123456789
  // !\"#$%&'()*+,-./:;<=>?@[\\]^_`{}~
  // Take some characters with maximum ymax, ymin :
  //SbString ss = "Ay1_{\";
  SbString ss = "_/";
  TT_BBox box;
  getStringBounds(ss,box);
  fAscent = box.yMax/64;
  fDescent = -box.yMin/64;}

  //printf("debug : asc %d desc %d\n",fAscent,fDescent);

  return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
int SbTextTTF1::getPointSize(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fIsValid==FALSE) return 0;
  return fInstanceMetrics.pointSize/64;
}
//////////////////////////////////////////////////////////////////////////////
SbBool SbTextTTF1::getCharacter(
 char aChar 
,SbBool aSmoothing
,SbBool
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

  short index = fTTF_Face->fGlyphIndices[aChar];

  TT_Glyph glyph;
  TT_Error error = TT_New_Glyph( fTTF_Face->fFace, &(glyph) );
  if(error) {
    SoDebugError::post("SbTextTTF1::getCharacter",
                       "error (0x%x) : could not create glyph container",
                       error);
    return FALSE;
  }

  //SoDebugError::postInfo("SbTextTTF1::getCharacter","debug 001");

  int flags = TTLOAD_DEFAULT;
  error = TT_Load_Glyph( fInstance, glyph, index, flags );
  if (error) {
    SoDebugError::post("SbTextTTF1::getCharacter",
                       "error (0x%x)",error);
    TT_Done_Glyph(glyph);
    return FALSE;
  }

  TT_Glyph_Metrics metrics;
  error = TT_Get_Glyph_Metrics( glyph, &metrics );
  if (error) {
    SoDebugError::post("SbTextTTF1::getCharacter",
                       "error (0x%x) : Could not get glyph metrics",error);
    TT_Done_Glyph(glyph);
    return FALSE;
  }

  #define  FLOOR(x)    ((x) & -64)
  #define  CEILING(x)  (((x)+63) & -64)
  TT_BBox bbox;
  bbox.xMin = FLOOR(metrics.bbox.xMin);
  bbox.yMin = FLOOR(metrics.bbox.yMin);
  bbox.xMax = CEILING(metrics.bbox.xMax);
  bbox.yMax = CEILING(metrics.bbox.yMax);
  #undef CEILING
  #undef FLOOR

  //          UL----|-----------UR
  //          |     |           |
  //          |     |    x      |           x
  //          |     |   x x     |          x
  //          |     |  x   x    |         x
  //          |     | x     x   |      | x
  //          |     |x       x  |      |x
  // baseline ------O------------      O2---------
  //          |     |           |
  //          |     |           |
  //          DL----|-----------DR
  // 
  // The origin O is in general at bottom left of the char on the baseline.
  //   bb.xMin = x(DL) is in general 0 or negative.
  //   bb.xMax = x(DR)
  //   bb.yMin = y(DL) is in general 0 or negative.
  //   bb.yMax = y(UL)
  // and : 
  //   metrics.bearingX = bb.xMin.
  //   metrics.bearingY = bb.yMax
  // Do we have ???
  //   advance = x(O2-O)
  // 
  // TT_Get_Glyph_Bitmap, TT_Get_Glyph_Pixmap origin is O.
  // In general we take : 
  //   xoffset = -bb.xMin
  //   yoffset = -bb.yMin
  // to get the whole glyph. Else, for example 'y', would be drawn as 'v'.

  //printf("debug : %c : xMin %ld xMax %ld bX %ld : yMin %ld yMax %ld bY %ld : advance %ld\n",aChar,
  //  bbox.xMin,bbox.xMax,metrics.bearingX,
  //  bbox.yMin,bbox.yMax,metrics.bearingY,
  //  metrics.advance);

  TT_Raster_Map rasterMap;
  int grays = 0;
  if(aSmoothing==TRUE) { //Pixmap
    int W = (bbox.xMax - bbox.xMin)/64;
    int H = (bbox.yMax - bbox.yMin)/64;
    rasterMap.rows = H;
    //printf("debug : W : %d : %d %d\n",W,int((W+3) & ~4),int((W+3) & ~3));
    //gltt-2.5.2    ROOT-3.0.06.
    //rasterMap.cols = (W+3) & ~4;
    //rasterMap.width = W;
    // See freetype.h TT_Raster_Map doc. Who is right ?
    rasterMap.cols = (W+3) & ~3;
    rasterMap.width = rasterMap.cols;
    grays = 5;
  } else { //Bitmap
    int W = (bbox.xMax - bbox.xMin)/64;
    int H = (bbox.yMax - bbox.yMin)/64;
    // See freetype.h TT_Raster_Map doc.
    rasterMap.rows  = H;
    rasterMap.cols = (W+7)/8;
    rasterMap.width = W;
    grays = 1;
  }
  rasterMap.flow = TT_Flow_Up;
  rasterMap.size = long(rasterMap.rows * rasterMap.cols);
  if(rasterMap.size<=0) {
    // This may happen (for example for the space character).
    rasterMap.bitmap = 0;
  } else {
    rasterMap.bitmap = new char[rasterMap.size];
    if(!rasterMap.bitmap) {
      SoDebugError::post("SbTextTTF1::getCharacter",
        "can't alloc bitmap buffer for character '%c'",aChar);
      TT_Done_Glyph(glyph);
      return FALSE;
    }
    ::memset(rasterMap.bitmap,0,rasterMap.size);

    // We must pass multiple of 64 for the offset. See freetype.h doc.
    TT_F26Dot6 rx = 64 * int(bbox.xMin/64);
    TT_F26Dot6 ry = 64 * int(bbox.yMin/64);
    //if(bbox.xMin != rx) {
    //  printf("debug : WARNING : %c : x : %d %d\n",
    //    aChar,(int)bbox.xMin,(int)rx);
    //}
    //if(bbox.yMin != ry) {
    //  printf("debug : WARNING : %c : y : %d %d\n",
    //    aChar,(int)bbox.yMin,(int)ry);
    //}
    if(aSmoothing==TRUE) {
      error = TT_Get_Glyph_Pixmap( glyph, &rasterMap, -rx, -ry);
    } else {
      error = TT_Get_Glyph_Bitmap( glyph, &rasterMap, -rx, -ry);

    /*if((aChar=='a')||(aChar=='o')||(aChar=='L')) {
        printf("bitmap for '%c' : w = %d h = %d : ptsize = %d\n",
          aChar,rasterMap.width,rasterMap.rows,getPointSize());
        for( int row = (rasterMap.rows-1); row >=0; --row ) {
          unsigned char* bline = 
            (unsigned char*)rasterMap.bitmap + row * rasterMap.cols;
          int icol = 0;
          int ibit = 0;
          unsigned char byte = (unsigned char)bline[icol];
          icol++;
          for( int i= 0; i < rasterMap.width; ++i ) {
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
      }*/
    }

    if (error) {
      SoDebugError::post("SbTextTTF1::getCharacter",
                         "error (0x%x) : could not get glyph bitmap",error);
      TT_Done_Glyph(glyph);
      delete [] ((char*)rasterMap.bitmap);
      rasterMap.bitmap = 0;
      return FALSE;
    }
  }

  TT_Done_Glyph(glyph);

  aBox.xMin = int(metrics.bbox.xMin/64);
  aBox.yMin = int(metrics.bbox.yMin/64);
  aBox.xMax = int(metrics.bbox.xMax/64);
  aBox.yMax = int(metrics.bbox.yMax/64);
  aAdvance = int(metrics.advance/64);

/*struct  TT_Raster_Map {
    int    rows;
    int    cols;
    int    width;
    int    flow;
    void*  bitmap;
    long   size;
  };*/

  aRaster.rows = rasterMap.rows;
  aRaster.cols = rasterMap.cols;
  aRaster.width = rasterMap.width;
  aRaster.buffer = rasterMap.bitmap;
  aRaster.size = int(rasterMap.size);
  aRaster.grays = grays;

  return TRUE;

}

#else
char SbTextTTF1_quiet_compiler = 0;
#endif
