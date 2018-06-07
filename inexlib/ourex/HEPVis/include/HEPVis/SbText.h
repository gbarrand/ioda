#ifndef HEPVis_SbText_h
#define HEPVis_SbText_h

#include <Inventor/SbBasic.h>

class SbBox3f;
class SoFont;
class SoType;
class SbString;

#include <vector>

class SbChar {
public:
  SbChar();
  SbChar(const SbChar&);
  SbChar& operator=(const SbChar&);
  enum Font {  
    LATIN,
    GREEK,
    SPECIAL
  };
  enum Move {  
    NONE,
    UP,
    DOWN
  };
public:
  char fChar;
  Font fFont;  //0 Latin, 1 Greek, 2 Special.
  Move fYMove; //O normal, 1 up, 2 down.
  SbBool fBack;
  SbBool fBar;
  SbBool fCR;
};

typedef std::vector<SbChar> SbEncodedString;

class SbText {
public:
  static SbBox3f getBBox(SoFont*,SoType,int,const SbString*);
  static SbString getSubString(SoFont*,SoType,const SbString&,float);
  static void getAscentDescent(SoFont*,SoType,float&,float&,float&);

  static SbEncodedString decodePlain(const SbString&);
  ////////////////////////////////////////////////////////////////////////////
  // PAW control characters :
  //  [ go to greek (roman = default)
  //  ] end of greek
  //  " go to special
  //  # end of special
  //  ! go to normal level of script
  //  ^ go to superscript
  //  ? go to subscript
  //  & backscpace one charachter
  // Extension :
  //  | draw a bar over one character
  static SbEncodedString decodePAW(const SbString&);
};

#endif
