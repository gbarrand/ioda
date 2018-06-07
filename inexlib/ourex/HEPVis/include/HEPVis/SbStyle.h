//  20/08/1998 : G.Barrand : creation.
//  30/01/2004 : L.Garnier : addition of methods to modifie hatch parameters


#ifndef HEPVis_SbStyle_h
#define HEPVis_SbStyle_h

#include <Inventor/SbColor.h>
#include <Inventor/SbVec3f.h>

#include <HEPVis/SbStyles.h>
#include <HEPVis/SbColors.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/SbConvert.h>

#include <vector>
#include <cstdio> //sprintf, sscanf

class SbStyleColor : public std::pair<SbString,SbColor> {
public:
  SbStyleColor()
  :std::pair<SbString,SbColor>("",SbColor())
  {}

  SbStyleColor(const SbString& aName,const SbColor& aColor)
  :std::pair<SbString,SbColor>(aName,aColor)
  {}

  SbStyleColor(const SbString& aName,float aR,float aG,float aB)
  :std::pair<SbString,SbColor>(aName,SbColor(aR,aG,aB))
  {}

  virtual ~SbStyleColor(){}
public:
  SbStyleColor(const SbStyleColor& aFrom)
  :std::pair<SbString,SbColor>(aFrom.first,aFrom.second)
  {}
  SbStyleColor& operator=(const SbStyleColor& aFrom){
    first = aFrom.first;
    second = aFrom.second;
    return *this;
  }
};

class SbStyleColormap : public std::vector<SbStyleColor> {
public:
  SbColor getColor(int aIndex) const {
    if((aIndex<0)||(aIndex>=(int)size())) return SbColor(0.5F,0.5F,0.5F);
    return (*this)[aIndex].second;
  }
  SbString getStringColor(int aIndex) const{
    if((aIndex<0)||(aIndex>=(int)size())) return "";
    SbColor c = (*this)[aIndex].second;
    char ss[3 * 32];
    ::sprintf(ss,"%g %g %g",c[0],c[1],c[2]);
    return SbString(ss);  
  }
};

class SbStyleDefaultColormap : public SbStyleColormap {
public:
  SbStyleDefaultColormap() {
    push_back(SbStyleColor("aquamarine",SbColor_aquamarine));
    push_back(SbStyleColor("mediumaquamarine",SbColor_mediumaquamarine));
    push_back(SbStyleColor("black",SbColor_black));
    push_back(SbStyleColor("blue",SbColor_blue));
    push_back(SbStyleColor("cadetblue",SbColor_cadetblue));
    push_back(SbStyleColor("cornflowerblue",SbColor_cornflowerblue));
    push_back(SbStyleColor("darkslateblue",SbColor_darkslateblue));
    push_back(SbStyleColor("lightblue",SbColor_lightblue));
    push_back(SbStyleColor("lightsteelblue",SbColor_lightsteelblue));
    push_back(SbStyleColor("mediumblue",SbColor_mediumblue));

    push_back(SbStyleColor("mediumslateblue",SbColor_mediumslateblue));
    push_back(SbStyleColor("midnightblue",SbColor_midnightblue));
    push_back(SbStyleColor("navyblue",SbColor_navyblue));
    push_back(SbStyleColor("navy",SbColor_navy));
    push_back(SbStyleColor("skyblue",SbColor_skyblue));
    push_back(SbStyleColor("slateblue",SbColor_slateblue));
    push_back(SbStyleColor("steelblue",SbColor_steelblue));
    push_back(SbStyleColor("coral",SbColor_coral));
    push_back(SbStyleColor("cyan",SbColor_cyan));
    push_back(SbStyleColor("firebrick",SbColor_firebrick));
      
    push_back(SbStyleColor("brown",SbColor_brown));
    push_back(SbStyleColor("gold",SbColor_gold));
    push_back(SbStyleColor("goldenrod",SbColor_goldenrod));
    push_back(SbStyleColor("green",SbColor_green));
    push_back(SbStyleColor("darkgreen",SbColor_darkgreen));
    push_back(SbStyleColor("darkolivegreen",SbColor_darkolivegreen));
    push_back(SbStyleColor("forestgreen",SbColor_forestgreen));
    push_back(SbStyleColor("limegreen",SbColor_limegreen));
    push_back(SbStyleColor("mediumseagreen",SbColor_mediumseagreen));
    push_back(SbStyleColor("mediumspringgreen",SbColor_mediumspringgreen));
      
    push_back(SbStyleColor("palegreen",SbColor_palegreen));
    push_back(SbStyleColor("seagreen",SbColor_seagreen));
    push_back(SbStyleColor("springgreen",SbColor_springgreen));
    push_back(SbStyleColor("yellowgreen",SbColor_yellowgreen));
    push_back(SbStyleColor("darkslategrey",SbColor_darkslategrey));
    push_back(SbStyleColor("dimgrey",SbColor_dimgrey));
    push_back(SbStyleColor("lightgrey",SbColor_lightgrey));
    push_back(SbStyleColor("grey",SbColor_grey));
    push_back(SbStyleColor("khaki",SbColor_khaki));
    push_back(SbStyleColor("magenta",SbColor_magenta));
      
    push_back(SbStyleColor("maroon",SbColor_maroon));
    push_back(SbStyleColor("orange",SbColor_orange));
    push_back(SbStyleColor("orchid",SbColor_orchid));
    push_back(SbStyleColor("darkorchid",SbColor_darkorchid));
    push_back(SbStyleColor("mediumorchid",SbColor_mediumorchid));
    push_back(SbStyleColor("pink",SbColor_pink));
    push_back(SbStyleColor("plum",SbColor_plum));
    push_back(SbStyleColor("red",SbColor_red));
    push_back(SbStyleColor("indianred",SbColor_indianred));
    push_back(SbStyleColor("mediumvioletred",SbColor_mediumvioletred));
      
    push_back(SbStyleColor("orangered",SbColor_orangered));
    push_back(SbStyleColor("violetred",SbColor_violetred));
    push_back(SbStyleColor("salmon",SbColor_salmon));
    push_back(SbStyleColor("sienna",SbColor_sienna));
    push_back(SbStyleColor("tan",SbColor_tan));
    push_back(SbStyleColor("thistle",SbColor_thistle));
    push_back(SbStyleColor("turquoise",SbColor_turquoise));
    push_back(SbStyleColor("darkturquoise",SbColor_darkturquoise));
    push_back(SbStyleColor("mediumturquoise",SbColor_mediumturquoise));
    push_back(SbStyleColor("violet",SbColor_violet));
      
    push_back(SbStyleColor("blueviolet",SbColor_blueviolet));
    push_back(SbStyleColor("wheat",SbColor_wheat));
    push_back(SbStyleColor("white",SbColor_white));
    push_back(SbStyleColor("yellow",SbColor_yellow));
    push_back(SbStyleColor("greenyellow",SbColor_greenyellow));
  }
};

class SbStyleROOT_Colormap : public SbStyleColormap {
public:
  SbStyleROOT_Colormap() {
    // ROOT-4.00.08/TApplication.cxx/InitializeColors
    resize(51);

    //base/inc/Gtypes.h
    enum EColor {kWhite,kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan};
                 //0    1      2    3      4     5       6        7

    new_TColor(kWhite,1,1,1,"background");
    new_TColor(kBlack,0,0,0,"black");
    new_TColor(kRed,1,0,0,"red");
    new_TColor(kGreen,0,1,0,"green");
    new_TColor(kBlue,0,0,1,"blue");
    new_TColor(kYellow,1,1,0,"yellow");
    new_TColor(kMagenta,1,0,1,"magenta");
    new_TColor(kCyan,0,1,1,"cyan");
    new_TColor(10,0.999,0.999,0.999,"white");
    new_TColor(11,0.754,0.715,0.676,"editcol");

    // The color white above is defined as being nearly white.
    // Sets the associated dark color also to white.
    //new_TColor(110,0.999,0.999,.999,"Color110");

    // Initialize Custom colors
    new_TColor(20,0.8,0.78,0.67,"Color20");
    new_TColor(31,0.54,0.66,0.63,"Color31");
    new_TColor(41,0.83,0.81,0.53,"Color41");
    new_TColor(30,0.52,0.76,0.64,"Color30");
    new_TColor(32,0.51,0.62,0.55,"Color32");
    new_TColor(24,0.70,0.65,0.59,"Color24");
    new_TColor(21,0.8,0.78,0.67,"Color21");
    new_TColor(47,0.67,0.56,0.58,"Color47");
    new_TColor(35,0.46,0.54,0.57,"Color35");
    new_TColor(33,0.68,0.74,0.78,"Color33");
    new_TColor(39,0.5,0.5,0.61,"Color39");
    new_TColor(37,0.43,0.48,0.52,"Color37");
    new_TColor(38,0.49,0.6,0.82,"Color38");
    new_TColor(36,0.41,0.51,0.59,"Color36");
    new_TColor(49,0.58,0.41,0.44,"Color49");
    new_TColor(43,0.74,0.62,0.51,"Color43");
    new_TColor(22,0.76,0.75,0.66,"Color22");
    new_TColor(45,0.75,0.51,0.47,"Color45");
    new_TColor(44,0.78,0.6,0.49,"Color44");
    new_TColor(26,0.68,0.6,0.55,"Color26");
    new_TColor(28,0.53,0.4,0.34,"Color28");
    new_TColor(25,0.72,0.64,0.61,"Color25");
    new_TColor(27,0.61,0.56,0.51,"Color27");
    new_TColor(23,0.73,0.71,0.64,"Color23");
    new_TColor(42,0.87,0.73,0.53,"Color42");
    new_TColor(46,0.81,0.37,0.38,"Color46");
    new_TColor(48,0.65,0.47,0.48,"Color48");
    new_TColor(34,0.48,0.56,0.6,"Color34");
    new_TColor(40,0.67,0.65,0.75,"Color40");
    new_TColor(29,0.69,0.81,0.78,"Color29");

    // Initialize some additional greyish non saturated colors
    new_TColor(8, 0.35,0.83,0.33,"Color8");
    new_TColor(9, 0.35,0.33,0.85,"Color9");
    new_TColor(12,.3,.3,.3,"grey12");
    new_TColor(13,.4,.4,.4,"grey13");
    new_TColor(14,.5,.5,.5,"grey14");
    new_TColor(15,.6,.6,.6,"grey15");
    new_TColor(16,.7,.7,.7,"grey16");
    new_TColor(17,.8,.8,.8,"grey17");
    new_TColor(18,.9,.9,.9,"grey18");
    new_TColor(19,.95,.95,.95,"grey19");
    new_TColor(50, 0.83,0.35,0.33,"Color50");

    // Initialize the Pretty Palette Spectrum Violet->Red
    //   The color model used here is based on the HLS model which
    //   is much more suitable for creating palettes than RGB.
    //   Fixing the saturation and lightness we can scan through the
    //   spectrum of visible light by using "hue" alone.
    //   In Root hue takes values from 0 to 360.
    /*FIXME
    float  saturation = 1;
    float  lightness = 0.5;
    float  MaxHue = 280;
    float  MinHue = 0;
    Int_t  MaxPretty = 50;
    float  hue;

    for (i=0 ; i<MaxPretty ; i++) {
      hue = MaxHue-(i+1)*((MaxHue-MinHue)/MaxPretty);
      TColor::HLStoRGB(hue, lightness, saturation, r, g, b);
      new_TColor(i+51, r, g, b);
    }*/

    // Initialize the Pretty Palette Spectrum Violet->Red
    //   The color model used here is based on the HLS model which
    //   is much more suitable for creating palettes than RGB.
    //   Fixing the saturation and lightness we can scan through the
    //   spectrum of visible light by using "hue" alone.
    //   In Root hue takes values from 0 to 360.
    /*FIXME
    float  saturation = 1;
    float  lightness = 0.5;
    float  MaxHue = 280;
    float  MinHue = 0;
    Int_t  MaxPretty = 50;
    float  hue;

    for (i=0 ; i<MaxPretty ; i++) {
      hue = MaxHue-(i+1)*((MaxHue-MinHue)/MaxPretty);
      TColor::HLStoRGB(hue, lightness, saturation, r, g, b);
      new_TColor(i+51, r, g, b);
    }*/

  }
private:
  void new_TColor(int i,double r,double g,double b,const SbString& n) {
    //it assumes that the vector has the correct size.
    (*this)[i] = SbStyleColor(n,float(r),float(g),float(b));
  }
};

class SbStyle {
public:
  SbStyle()
  :fColor(0,0,0)
  ,fHighlightColor(0,0,0)
  ,fLineWidth(1)
  ,fMarkerSize(1)
  ,fFontSize(10)
  ,fLinePattern(SbLinePattern_solid)
  ,fMarkerStyle(SbMarkerDot)
  ,fAreaStyle(SbAreaSolid)
  ,fModeling(SbModeling_boxes)
  ,fLightModel(SbLightModel_phong)
  ,fTickModeling(SbTickModeling_hippo)
  ,fEncoding(SbEncoding_none)
  ,fSmoothing(FALSE)
  ,fHinting(FALSE)
  ,fCut("")
  ,fPaintingPolicy(SbPaintingUniform)
  ,fHatchingPolicy(SbHatchingNone)
  ,fProjectionType(SbProjectionNone)
  ,fFontName("defaultFont")
  ,fMultiNodeLimit(NoLimit())
  ,fDivisions(510) //Same as ROOT/TAttAxis
  ,fRotationSteps(24) //Same as SbPolyhedron default.
  ,fTransparency(0)
  ,fSpacing(0.05F)
  ,fAngle(FM_PI/4)
  ,fScale(1)
  ,fOffset(0)
  ,fStripWidth(0)
  ,fVisible(TRUE)
  ,fBarOffset(0.25F)
  ,fBarWidth(0.5F)
  ,fEditable(FALSE)
  ,fAutomated(TRUE)
  ,fOptions("")
  ,fColorMapping("")
  ,fEnforced(FALSE)
  ,fTranslation(0,0,0)
  {}
  
  virtual ~SbStyle(){}
public:
  SbStyle(const SbStyle& aFrom)
  :fColor(aFrom.fColor)
  ,fHighlightColor(aFrom.fHighlightColor)
  ,fLineWidth(aFrom.fLineWidth)
  ,fMarkerSize(aFrom.fMarkerSize)
  ,fFontSize(aFrom.fFontSize)
  ,fLinePattern(aFrom.fLinePattern)
  ,fMarkerStyle(aFrom.fMarkerStyle)
  ,fAreaStyle(aFrom.fAreaStyle)
  ,fModeling(aFrom.fModeling)
  ,fLightModel(aFrom.fLightModel)
  ,fTickModeling(aFrom.fTickModeling)
  ,fEncoding(aFrom.fEncoding)
  ,fSmoothing(aFrom.fSmoothing)
  ,fHinting(aFrom.fHinting)
  ,fCut(aFrom.fCut)
  ,fPaintingPolicy(aFrom.fPaintingPolicy)
  ,fHatchingPolicy(aFrom.fHatchingPolicy)
  ,fProjectionType(aFrom.fProjectionType)
  ,fFontName(aFrom.fFontName)
  ,fMultiNodeLimit(aFrom.fMultiNodeLimit)
  ,fDivisions(aFrom.fDivisions)
  ,fRotationSteps(aFrom.fRotationSteps)
  ,fTransparency(aFrom.fTransparency)
  ,fSpacing(aFrom.fSpacing)
  ,fAngle(aFrom.fAngle)
  ,fScale(aFrom.fScale)
  ,fOffset(aFrom.fOffset)
  ,fStripWidth(aFrom.fStripWidth)
  ,fVisible(aFrom.fVisible)
  ,fBarOffset(aFrom.fBarOffset)
  ,fBarWidth(aFrom.fBarWidth)
  ,fEditable(aFrom.fEditable)
  ,fAutomated(aFrom.fAutomated)
  ,fOptions(aFrom.fOptions)
  ,fColorMapping(aFrom.fColorMapping)
  ,fEnforced(aFrom.fEnforced)
  ,fTranslation(aFrom.fTranslation)
  {}

  SbStyle& operator=(const SbStyle& aFrom){copy(aFrom);return *this;}
public:
  void copy(const SbStyle& aFrom){
    fColor = aFrom.fColor;
    fHighlightColor = aFrom.fHighlightColor;
    fLineWidth = aFrom.fLineWidth;
    fMarkerSize = aFrom.fMarkerSize;
    fFontSize = aFrom.fFontSize;
    fLinePattern = aFrom.fLinePattern;
    fMarkerStyle = aFrom.fMarkerStyle;
    fAreaStyle = aFrom.fAreaStyle;
    fModeling = aFrom.fModeling;
    fLightModel = aFrom.fLightModel;
    fTickModeling = aFrom.fTickModeling;
    fEncoding = aFrom.fEncoding;
    fSmoothing = aFrom.fSmoothing;
    fHinting = aFrom.fHinting;
    fCut = aFrom.fCut;
    fPaintingPolicy = aFrom.fPaintingPolicy;
    fHatchingPolicy = aFrom.fHatchingPolicy;
    fProjectionType = aFrom.fProjectionType;
    fFontName = aFrom.fFontName;
    fMultiNodeLimit = aFrom.fMultiNodeLimit;
    fDivisions = aFrom.fDivisions;
    fRotationSteps = aFrom.fRotationSteps;
    fTransparency = aFrom.fTransparency;
    fSpacing = aFrom.fSpacing;
    fAngle = aFrom.fAngle;
    fScale = aFrom.fScale;
    fOffset = aFrom.fOffset;
    fStripWidth = aFrom.fStripWidth;
    fVisible = aFrom.fVisible;
    fBarOffset = aFrom.fBarOffset;
    fBarWidth = aFrom.fBarWidth;
    fEditable = aFrom.fEditable;
    fAutomated = aFrom.fAutomated;
    fOptions = aFrom.fOptions;
    fColorMapping = aFrom.fColorMapping;
    fEnforced = aFrom.fEnforced;
    fTranslation = aFrom.fTranslation;
  }

  SbBool isEqual(const SbStyle& aFrom){
    if(fLineWidth!=aFrom.fLineWidth) return FALSE;
    if(fMarkerSize!=aFrom.fMarkerSize) return FALSE;
    if(fFontSize!=aFrom.fFontSize) return FALSE;
    if(fLinePattern!=aFrom.fLinePattern) return FALSE;
    if(fMarkerStyle!=aFrom.fMarkerStyle) return FALSE;
    if(fAreaStyle!=aFrom.fAreaStyle) return FALSE;
    if(fSmoothing!=aFrom.fSmoothing) return FALSE;
    if(fHinting!=aFrom.fHinting) return FALSE;
    if(fPaintingPolicy!=aFrom.fPaintingPolicy) return FALSE;
    if(fHatchingPolicy!=aFrom.fHatchingPolicy) return FALSE;
    if(fProjectionType!=aFrom.fProjectionType) return FALSE;
    if(fMultiNodeLimit!=aFrom.fMultiNodeLimit) return FALSE;
    if(fDivisions!=aFrom.fDivisions) return FALSE;
    if(fRotationSteps!=aFrom.fRotationSteps) return FALSE;
    if(fTransparency!=aFrom.fTransparency) return FALSE;
    if(fSpacing!=aFrom.fSpacing) return FALSE;
    if(fAngle!=aFrom.fAngle) return FALSE;
    if(fScale!=aFrom.fScale) return FALSE;
    if(fOffset!=aFrom.fOffset) return FALSE;
    if(fStripWidth!=aFrom.fStripWidth) return FALSE;
    if(fVisible!=aFrom.fVisible) return FALSE;
    if(fBarOffset!=aFrom.fBarOffset) return FALSE;
    if(fBarWidth!=aFrom.fBarWidth) return FALSE;
    if(fEditable!=aFrom.fEditable) return FALSE;
    if(fAutomated!=aFrom.fAutomated) return FALSE;
    if(fEnforced!=aFrom.fEnforced) return FALSE;
  
    //SbColor
    if(fColor!=aFrom.fColor) return FALSE;
    if(fHighlightColor!=aFrom.fHighlightColor) return FALSE;
  
    //SbString
    if(fModeling!=aFrom.fModeling) return FALSE;
    if(fLightModel!=aFrom.fLightModel) return FALSE;
    if(fTickModeling!=aFrom.fTickModeling) return FALSE;
    if(fEncoding!=aFrom.fEncoding) return FALSE;
    if(fCut!=aFrom.fCut) return FALSE;
    if(fFontName!=aFrom.fFontName) return FALSE;
    if(fOptions!=aFrom.fOptions) return FALSE;
    if(fColorMapping!=aFrom.fColorMapping) return FALSE;
  
    if(fTranslation!=aFrom.fTranslation) return FALSE;

    return TRUE;
  }

  virtual void reset(){ //virtual because of SoGC
    fColor = SbColor_grey;
    fHighlightColor = SbColor_lightgrey;
    fLineWidth = 1;
    fMarkerSize = 1;
    fFontSize = 10;
    fLinePattern = SbLinePattern_solid;
    fMarkerStyle = SbMarkerDot;
    fAreaStyle = SbAreaSolid;
    fModeling = SbModeling_boxes;
    fLightModel = SbLightModel_phong;
    fTickModeling = SbTickModeling_hippo;
    fEncoding = SbEncoding_none;
    fSmoothing = FALSE;
    fHinting = FALSE;
    fCut = "";
    fPaintingPolicy = SbPaintingUniform;
    fHatchingPolicy = SbHatchingNone;
    fProjectionType = SbProjectionNone;
    fFontName = "defaultFont";
    fMultiNodeLimit = NoLimit();
    fTransparency = 0;
    fDivisions = 510;
    fRotationSteps = 24;
    fSpacing = 0.05F;
    fAngle = FM_PI/4;
    fScale = 1;
    fOffset = 0;
    fStripWidth = 0;
    fVisible = TRUE;
    fBarOffset = 0.25F;
    fBarWidth = 0.5F;  
    fEditable = FALSE;
    fAutomated = TRUE;
    fOptions = "";
    fColorMapping = "";
    fEnforced = FALSE;
    fTranslation = SbVec3f(0,0,0);
  }

  SbString getString() const{
    char ss[23 * 32];
    ::sprintf(ss,"\
color %g %g %g\n\
highlightColor %g %g %g\n\
lineWidth %d\n\
markerSize %d\n\
fontSize %d\n\
linePattern %x\n\
multiNodeLimit %d\n\
transparency %g\n\
divisions %d\n\
rotationSteps %d\n\
angle %g\n\
scale %g\n\
offset %g\n\
stripWidth %g\n\
spacing %g\n\
barOffset %g\n\
barWidth %g\n\
translation %g %g %g"
    ,fColor[0],fColor[1],fColor[2]
    ,fHighlightColor[0],fHighlightColor[1],fHighlightColor[2]
    ,fLineWidth
    ,fMarkerSize
    ,fFontSize
    ,fLinePattern
    ,fMultiNodeLimit
    ,fTransparency
    ,fDivisions
    ,fRotationSteps
    ,fAngle
    ,fScale
    ,fOffset
    ,fStripWidth
    ,fSpacing
    ,fBarOffset
    ,fBarWidth
    ,fTranslation[0],fTranslation[1],fTranslation[2]);
    
    SbString lf("\n");
    SbString s(ss);  

    s += lf;  
    s += "smoothing ";
    s += (fSmoothing?"true":"false");

    s += lf;  
    s += "hinting ";
    s += (fHinting?"true":"false");

    s += lf;  
    s += "enforced ";
    s += (fEnforced?"true":"false");

    s += lf;  
    s += "visible ";
    s += (fVisible?"true":"false");

    s += lf;  
    s += "editable ";
    s += (fEditable?"true":"false");

    s += lf;  
    s += "automated ";
    s += (fAutomated?"true":"false");

    s += lf;  
    s += "markerStyle ";
    s += markerStyleToString(fMarkerStyle);

    s += lf;  
    s += "areaStyle ";
    s += areaStyleToString(fAreaStyle);

    s += lf;
    s += "modeling ";
    s += fModeling;

    s += lf;
    s += "lightModel ";
    s += fLightModel;

    s += lf;
    s += "tickModeling ";
    s += fTickModeling;

    s += lf;
    s += "encoding ";
    s += fEncoding;

    s += lf;
    s += "cut ";
    s += fCut;

    s += lf;
    s += "painting ";
    s += paintingPolicyToString(fPaintingPolicy);

    s += lf;
    s += "hatching ";
    s += hatchingPolicyToString(fHatchingPolicy);

    s += lf;
    s += "projection ";
    s += projectionTypeToString(fProjectionType);

    s += lf;
    s += "fontName ";
    s += fFontName;

    s += lf;
    s += "options ";
    s += fOptions;

    s += lf;
    s += "colorMapping ";
    s += fColorMapping;

    return s;
  }

  void setColor(const SbColor& aValue){fColor = aValue;}
  SbColor getColor() const {return fColor;}

  void setHighlightColor(const SbColor& aValue){fHighlightColor = aValue;}
  SbColor getHighlightColor() const {return fHighlightColor;}

  void setLineWidth(int aValue){fLineWidth = aValue;}
  int getLineWidth() const {return fLineWidth;}

  float getTransparency() const {return fTransparency;}
  void setTransparency(float aValue) {fTransparency = aValue;}

  void setMarkerStyle(SbMarkerStyle aValue){fMarkerStyle = aValue;}
  SbMarkerStyle getMarkerStyle() const {return fMarkerStyle;}

  void setMarkerSize(int aValue){fMarkerSize = aValue;}
  int getMarkerSize() const {return fMarkerSize;}

  void setFontSize(int aValue){fFontSize = aValue;}
  int getFontSize() const {return fFontSize;}

  void setAreaStyle(SbAreaStyle aValue){fAreaStyle = aValue;}
  SbAreaStyle getAreaStyle() const {return fAreaStyle;}

  void setModeling(const SbString& aValue){fModeling = aValue;}
  const SbString& getModeling() const {return fModeling;}

  void setLightModel(const SbString& aValue){fLightModel = aValue;}
  const SbString& getLightModel() const {return fLightModel;}

  void setTickModeling(const SbString& aValue){fTickModeling = aValue;}
  const SbString& getTickModeling() const {return fTickModeling;}

  void setEncoding(const SbString& aValue){fEncoding = aValue;}
  const SbString& getEncoding() const {return fEncoding;}

  void setSmoothing(SbBool aValue){fSmoothing = aValue;}
  SbBool getSmoothing() const {return fSmoothing;}

  void setHinting(SbBool aValue){fHinting = aValue;}
  SbBool getHinting() const {return fHinting;}

  SbPaintingPolicy getPaintingPolicy() const {return fPaintingPolicy;}
  void setPaintingPolicy(SbPaintingPolicy aValue){fPaintingPolicy = aValue;}

  SbHatchingPolicy getHatchingPolicy() const {return fHatchingPolicy;}
  void setHatchingPolicy(SbHatchingPolicy aValue) {fHatchingPolicy = aValue;}

  SbProjectionType getProjectionType() const {return fProjectionType;}
  void setProjectionType(SbProjectionType aValue){fProjectionType = aValue;}

  void setLinePattern(SbLinePattern aValue){fLinePattern = aValue;}
  SbLinePattern getLinePattern() const {return fLinePattern;}

  void setFontName(const SbString& aValue){fFontName = aValue;}
  const SbString& getFontName() const {return fFontName;}

  void setDivisions(int aValue){fDivisions = aValue;}
  int getDivisions() const {return fDivisions;}

  void setRotationSteps(int aValue){fRotationSteps = aValue;}
  int getRotationSteps() const {return fRotationSteps;}

  void setOffset(float aValue){fOffset = aValue;}
  float getOffset() const {return fOffset;}

  void setStripWidth(float aValue){fStripWidth = aValue;}
  float getStripWidth() const {return fStripWidth;}

  void setAngle(float aValue){fAngle = aValue;}
  float getAngle() const {return fAngle;}

  void setScale(float aValue){fScale = aValue;}
  float getScale() const {return fScale;}

  void setBarOffset(float aValue){fBarOffset = aValue;}
  float getBarOffset() const {return fBarOffset;}

  void setBarWidth(float aValue){fBarWidth = aValue;}
  float getBarWidth() const {return fBarWidth;}

  void setMultiNodeLimit(int aValue){fMultiNodeLimit = aValue;}
  int getMultiNodeLimit() const {return fMultiNodeLimit;}

  void setSpacing(float aValue){fSpacing = aValue;}
  float getSpacing() const {return fSpacing;}

  void setVisible(SbBool aValue){fVisible = aValue;}
  SbBool getVisible() const{return fVisible;}

  void setEditable(SbBool aValue){fEditable = aValue;}
  SbBool getEditable() const{return fEditable;}

  void setAutomated(SbBool aValue){fAutomated = aValue;}
  SbBool getAutomated() const{return fAutomated;}

  void setCut(const SbString& aValue){fCut = aValue;}
  const SbString& getCut() const {return fCut;}

  void setOptions(const SbString& aValue){fOptions = aValue;}
  const SbString& getOptions() const{return fOptions;}

  void setColorMapping(const SbString& aValue){fColorMapping = aValue;}
  const SbString& getColorMapping() const{return fColorMapping;}

  void setEnforced(SbBool aValue){fEnforced = aValue;}
  SbBool getEnforced() const{return fEnforced;}

  void setTranslation(const SbVec3f& aValue){fTranslation = aValue;}
  SbVec3f getTranslation() const {return fTranslation;}

public:
  static void hls2rgb(float hue, float light, float satur,
                             float &r, float &g, float &b){
    // taken from ROOT/TColor.
    // Static method to compute RGB from HLS. The l and s are between [0,1]
    // and h is between [0,360]. The returned r,g,b triplet is between [0,1].

    float rh, rl, rs, rm1, rm2;
    rh = rl = rs = 0;
    if (hue   > 0) rh = hue;   if (rh > 360) rh = 360;
    if (light > 0) rl = light; if (rl > 1)   rl = 1;
    if (satur > 0) rs = satur; if (rs > 1)   rs = 1;
  
    if (rl <= 0.5F) {
      rm2 = rl*(1 + rs);
    } else {
      rm2 = rl + rs - rl*rs;
    }
    rm1 = 2*rl - rm2;

    if (!rs) { r = rl; g = rl; b = rl; return; }
    r = hls_to_rgb(rm1, rm2, rh+120);
    g = hls_to_rgb(rm1, rm2, rh);
    b = hls_to_rgb(rm1, rm2, rh-120);
  }

  static SbBool stringToColor(const SbString& aString,SbColor& aColor){
    if( (aString.getLength()==7) && (aString.getString()[0]=='#') ) {
      // #RRGGBB format :
      //  1 3 5
      unsigned int rr,gg,bb;
  
     {SbString s = aString.getSubString(1,2);
      if(::sscanf(s.getString(),"%x",&rr)!=1) return FALSE;}
  
     {SbString s = aString.getSubString(3,4);
      if(::sscanf(s.getString(),"%x",&gg)!=1) return FALSE;}
  
     {SbString s = aString.getSubString(5,6);
      if(::sscanf(s.getString(),"%x",&bb)!=1) return FALSE;}
  
      aColor = SbColor(((float)rr)/255,((float)gg)/255,((float)bb)/255);
      return TRUE;
    } 
  
    // Look if three floats :
   {std::vector<SbString> words = SbGetWords(aString," ");
    if(words.size()==3) {
      SbString& word_0 = words[0];
      SbString& word_1 = words[1];
      SbString& word_2 = words[2];
      float r,g,b;
      if( SbConvertToFloat(word_0,r) &&
          SbConvertToFloat(word_1,g) &&
          SbConvertToFloat(word_2,b) ){
        aColor = SbColor(r,g,b);
        return TRUE;
      }
    }}
  
    // Look in the named color table :
   {SbStyleDefaultColormap defaultColormap; //cstor costly
    std::vector<SbStyleColor>::const_iterator it;
    for(it=defaultColormap.begin();it!=defaultColormap.end();++it) {
      if((*it).first==aString) {
        aColor = (*it).second;
        return TRUE;
      }
    }}
  
    return FALSE;
  }

  static SbBool colorToString(const SbColor& aColor,SbString& aString){
    char s[256];
    ::sprintf(s,"%g %g %g",aColor[0],aColor[1],aColor[2]);
    aString = s;
    return TRUE;
  }

  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
private:
  typedef struct {char string[32];SbMarkerStyle value;} MarkerStyle;
  static const MarkerStyle* getMarkerStyleList(unsigned int& a_num){
    static const MarkerStyle list[] = { //read only static, then ok.
      {"dot",         SbMarkerDot},
      {"plus",        SbMarkerPlus},
      {"asterisk",    SbMarkerAsterisk},
      {"cross",       SbMarkerCross},
      {"star",        SbMarkerStar},
      {"circle_line",         SbMarkerCircleLine},
      {"circle_filled",       SbMarkerCircleFilled},
      {"triangle_up_line",    SbMarkerTriangleUpLine},
      {"triangle_up_filled",  SbMarkerTriangleUpFilled},
      {"triangle_down_line",  SbMarkerTriangleDownLine},
      {"triangle_down_filled",SbMarkerTriangleDownFilled},
      {"david_star_line",     SbMarkerDavidStarLine},
      {"david_star_filled",   SbMarkerDavidStarFilled},
      {"swiss_cross_line",    SbMarkerSwissCrossLine},
      {"swiss_cross_filled",  SbMarkerSwissCrossFilled},
      {"diamond_line",        SbMarkerDiamondLine},
      {"diamond_filled",      SbMarkerDiamondFilled},
      {"square_line",         SbMarkerSquareLine},
      {"square_filled",       SbMarkerSquareFilled},
      {"penta_star_line",     SbMarkerPentaStarLine},
      {"penta_star_filled",   SbMarkerPentaStarFilled},
    };  
    a_num = sizeof(list)/sizeof(list[0]);
    return list;
  }
public:
  static SbBool stringToMarkerStyle(const SbString& aString,SbMarkerStyle& aValue){
    unsigned int number;
    const MarkerStyle* list = getMarkerStyleList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aString==list[count].string) {
        aValue = list[count].value;
        return TRUE;
      }
    }
    aValue = SbMarkerDot;
    return FALSE;
  }
  static const char* markerStyleToString(SbMarkerStyle aValue){
    unsigned int number;
    const MarkerStyle* list = getMarkerStyleList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aValue==list[count].value) return list[count].string;
    }
    return 0;
  }

  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
private:
  typedef struct {char string[16];SbAreaStyle value;} AreaStyle;
  static const AreaStyle* getAreaStyleList(unsigned int& a_num){
    static const AreaStyle list[] = { //read only static, then ok.
      {"solid",         SbAreaSolid},
      {"hatched",       SbAreaHatched},
      {"checker",       SbAreaChecker},
      {"edged",         SbAreaEdged}
    };  
    a_num = sizeof(list)/sizeof(list[0]);
    return list;
  }
public:
  static SbBool stringToAreaStyle(const SbString& aString,SbAreaStyle& aValue){
    unsigned int number;
    const AreaStyle* list = getAreaStyleList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aString==list[count].string) {
        aValue = list[count].value;
        return TRUE;  
      }
    }
    aValue = SbAreaSolid;
    return FALSE;
  }
  static const char* areaStyleToString(SbAreaStyle aValue){
    unsigned int number;
    const AreaStyle* list = getAreaStyleList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aValue==list[count].value) return list[count].string;
    }
    return 0;
  }

  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
private:
  typedef struct {char string[20];SbPaintingPolicy value;} PaintingPolicy;
  static const PaintingPolicy* getPaintingPolicyList(unsigned int& a_num){
    static const PaintingPolicy list[] = { //read only static, then ok.
      {"uniform",      SbPaintingUniform},
      {"by_value",     SbPaintingByValue},
      {"by_level",     SbPaintingByLevel},
      {"grey_scale",   SbPaintingGreyScale},
      {"violet_to_red",SbPaintingVioletToRed},
      {"grey_scale_inverse",SbPaintingGreyScaleInverse}
    };  
    a_num = sizeof(list)/sizeof(list[0]);
    return list;
  }
public:
  static SbBool stringToPaintingPolicy(const SbString& aString,SbPaintingPolicy& aValue){
    unsigned int number;
    const PaintingPolicy* list = getPaintingPolicyList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aString==list[count].string) {
        aValue = list[count].value;
        return TRUE;
      }
    }
    aValue = SbPaintingUniform;
    return FALSE;
  }
  static const char* paintingPolicyToString(SbPaintingPolicy aValue){
    unsigned int number;
    const PaintingPolicy* list = getPaintingPolicyList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aValue==list[count].value) return list[count].string;
    }
    return 0;
  }


  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
private:
  typedef struct {char string[16];SbHatchingPolicy value;} HatchingPolicy;
  static const HatchingPolicy* getHatchingPolicyList(unsigned int& a_num){
    static const HatchingPolicy list[] = { //read only static, then ok.
      {"none",           SbHatchingNone},
      {"right",          SbHatchingRight},
      {"left",           SbHatchingLeft},
      {"left_and_right", SbHatchingLeftAndRight}
    };  
    a_num = sizeof(list)/sizeof(list[0]);
    return list;
  }
public:
  static SbBool stringToHatchingPolicy(const SbString& aString,SbHatchingPolicy& aValue){
    unsigned int number;
    const HatchingPolicy* list = getHatchingPolicyList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aString==list[count].string) {
        aValue = list[count].value;
        return TRUE;
      }
    }
    aValue = SbHatchingNone;
    return FALSE;
  }

  static const char* hatchingPolicyToString(SbHatchingPolicy aValue){
    unsigned int number;
    const HatchingPolicy* list = getHatchingPolicyList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aValue==list[count].value) return list[count].string;
    }
    return 0;
  }

  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
private:
  typedef struct {char string[16];SbProjectionType value;} ProjectionType;
  static const ProjectionType* getProjectionTypeList(unsigned int& a_num){
    static const ProjectionType list[] = { //read only static, then ok.
      {"none",     SbProjectionNone},
      {"rz",       SbProjectionRZ},
      {"phiz",     SbProjectionPHIZ},
      {"zr",       SbProjectionZR},
      {"zphi",     SbProjectionZPHI}
    };  
    a_num = sizeof(list)/sizeof(list[0]);
    return list;
  }
public:
  static SbBool stringToProjectionType(const SbString& aString,SbProjectionType& aValue){
    unsigned int number;
    const ProjectionType* list = getProjectionTypeList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aString==list[count].string) {
        aValue = list[count].value;
        return TRUE;
      }
    }
    aValue = SbProjectionNone;
    return FALSE;
  }
  static const char* projectionTypeToString(SbProjectionType aValue){
    unsigned int number;
    const ProjectionType* list = getProjectionTypeList(number);
    for(unsigned int count=0;count<number;count++) {
      if(aValue==list[count].value) return list[count].string;
    }
    return 0;
  }

  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////
  static SbBool stringToLinePattern(const SbString& aString,SbLinePattern& aPattern){
    if(aString=="solid") {
      aPattern = SbLinePattern_solid;
    } else if(aString=="dashed") {
      aPattern = SbLinePattern_dashed;
    } else if(aString=="dotted") {
      aPattern = SbLinePattern_dotted;
    } else if(aString=="dash_dotted") {
      aPattern = SbLinePattern_dash_dotted;
    } else {
      aPattern = SbLinePattern_solid;
      return FALSE;
    }
    return TRUE;
  }

  
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  typedef void(*ErrorFunc)(const char*,const char*,...);

#define SbStyle_check_2 \
      if(wordn!=2) {\
        if(aErrorFunc) aErrorFunc("SbStyle::setFromString",\
          "%s: %s has a bad word count. Two expected.",\
          aString.getString(),line.getString());\
        return FALSE;\
      }

#define SbStyle_word1_bool \
      SbString& word1 = words[1];\
      SbBool value;\
      if(SbConvertToBool(word1,value)==FALSE)  {\
        if(aErrorFunc) aErrorFunc("SbStyle::setFromString",\
          "%s: %s not a boolean.",aString.getString(),word1.getString());\
        return FALSE;\
      }

#define SbStyle_word1_int \
      SbString& word1 = words[1];\
      int value;\
      if(SbConvertToInt(word1,value)==FALSE)  {\
        if(aErrorFunc) aErrorFunc("SbStyle::setFromString",\
          "%s: %s not a number.",aString.getString(),word1.getString());\
        return FALSE;\
      }

#define SbStyle_word1_float \
      SbString& word1 = words[1];\
      float value;\
      if(SbConvertToFloat(word1,value)==FALSE)  {\
        if(aErrorFunc) aErrorFunc("SbStyle::setFromString",\
          "%s: %s not a number.",aString.getString(),word1.getString());\
        return FALSE;\
      }

  SbBool setFromString(const SbString& aString,ErrorFunc aErrorFunc = 0){
    // aString = list of "name value" separated by \n
    //printf("debug : SbStyle::setFromString : \"%s\"\n",aString.getString());
    std::vector<SbString> lines = SbGetLines(aString);
    int linen = lines.size();
    for(int count=0;count<linen;count++) {
      SbString& line = lines[count];
      if(!line.getLength()) continue;
      if(line=="setDefaults") {
        reset();
        continue;
      } else if(line=="reset") {
        reset();
        continue;
      }
      std::vector<SbString> words = SbGetWords(line," ");
      int wordn = words.size();
      if(wordn<=0) {
        if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
          "%s: %s has a bad word count (at least two expected).",
          aString.getString(),line.getString());
        return FALSE;
      }
      SbString& word0 = words[0];
      if(word0=="color") {
        if(wordn==2) {
          SbString& word1 = words[1];
          if(stringToColor(word1,fColor)==FALSE) {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a color.",aString.getString(),word1.getString());
            return FALSE;
          } else if( (word1.getString()[0]=='#') && word1.getLength()==7) {
            // #RRGGBB format :
            //  1 3 5
            unsigned int rr,gg,bb;
           {SbString ss;ss += word1.getString()[1];ss += word1.getString()[2];
            if(::sscanf(ss.getString(),"%x",&rr)!=1) {
              if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
                "%s: %s not a number.",aString.getString(),ss.getString());
              return FALSE;
            }}
           {SbString ss;ss += word1.getString()[3];ss += word1.getString()[4];
            if(::sscanf(ss.getString(),"%x",&gg)!=1) {
              if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
                "%s: %s not a number.",aString.getString(),ss.getString());
              return FALSE;
            }}
           {SbString ss;ss += word1.getString()[5];ss += word1.getString()[6];
            if(::sscanf(ss.getString(),"%x",&bb)!=1) {
              if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
                "%s: %s not a number.",aString.getString(),ss.getString());
              return FALSE;
            }}
            fColor.setValue(((float)rr)/255,((float)gg)/255,((float)bb)/255);
          } 
        } else if (wordn==4) {
          SbString& rs = words[1];
          float r;
          if(SbConvertToFloat(rs,r)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),rs.getString());
            return FALSE;
          }
          SbString& gs = words[2];
          float g;
          if(SbConvertToFloat(gs,g)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),gs.getString());
            return FALSE;
          }
          SbString& bs = words[3];
          float b;
          if(SbConvertToFloat(bs,b)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),bs.getString());
            return FALSE;
          }
          fColor.setValue(r,g,b);
        } else {
          if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
            "%s: %s has a bad word count (two or four expected).",
            aString.getString(),line.getString());
          return FALSE;
        }
      } else if(word0=="highlightColor") {
        if(wordn==2) {
          SbString& word1 = words[1];
          if(stringToColor(word1,fHighlightColor)==FALSE) {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a color.",aString.getString(),word1.getString());
            return FALSE;
          } else if( (word1.getString()[0]=='#') && word1.getLength()==7) {
            // #RRGGBB format :
            //  1 3 5
            unsigned int rr,gg,bb;
           {SbString ss;ss += word1.getString()[1];ss += word1.getString()[2];
            if(::sscanf(ss.getString(),"%x",&rr)!=1) {
              if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
                "%s: %s not a number.",aString.getString(),ss.getString());
              return FALSE;
            }}
           {SbString ss;ss += word1.getString()[3];ss += word1.getString()[4];
            if(::sscanf(ss.getString(),"%x",&gg)!=1) {
              if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
                "%s: %s not a number.",aString.getString(),ss.getString());
              return FALSE;
            }}
           {SbString ss;ss += word1.getString()[5];ss += word1.getString()[6];
            if(::sscanf(ss.getString(),"%x",&bb)!=1) {
              if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
                "%s: %s not a number.",aString.getString(),ss.getString());
              return FALSE;
            }}
            fHighlightColor.setValue
              (((float)rr)/255,((float)gg)/255,((float)bb)/255);
          } 
        } else if (wordn==4) {
          SbString& rs = words[1];
          float r;
          if(SbConvertToFloat(rs,r)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),rs.getString());
            return FALSE;
          }
          SbString& gs = words[2];
          float g;
          if(SbConvertToFloat(gs,g)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),gs.getString());
            return FALSE;
          }
          SbString& bs = words[3];
          float b;
          if(SbConvertToFloat(bs,b)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),bs.getString());
            return FALSE;
          }
          fHighlightColor.setValue(r,g,b);
        } else {
          if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
            "%s: %s has a bad word count (two or four expected).",
            aString.getString(),line.getString());
          return FALSE;
        }
      } else if( (word0=="linePattern") || (word0=="lineStyle") ) {
        SbStyle_check_2
        SbString& word1 = words[1];
        unsigned int ul;
        if(stringToLinePattern(word1,fLinePattern)==FALSE) {
          if(::sscanf(word1.getString(),"%u",&ul)!=1) {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString","%s: %s not a line pattern.",aString.getString(),word1.getString());
            return FALSE;
          }
          fLinePattern = (SbLinePattern)ul;
        }
      } else if(word0=="markerStyle") {
        SbStyle_check_2
        SbString& word1 = words[1];
        if(stringToMarkerStyle(word1,fMarkerStyle)==FALSE) {
          if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
            "%s not a SbMarkerStyle.",word1.getString());
          return FALSE;
        }
      } else if(word0=="areaStyle") {
        SbStyle_check_2
        SbString& word1 = words[1];
        if(stringToAreaStyle(word1,fAreaStyle)==FALSE) {
          if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
            "%s not a SbAreaStyle.",word1.getString());
          return FALSE;
        }
      } else if(word0=="modeling") {
        SbStyle_check_2
        SbString& word1 = words[1];
        fModeling = word1;
      } else if(word0=="lightModel") {
        SbStyle_check_2
        SbString& word1 = words[1];
        fLightModel = word1;
      } else if(word0=="tickModeling") {
        SbStyle_check_2
        SbString& word1 = words[1];
        fTickModeling = word1;
      } else if(word0=="encoding") {
        SbStyle_check_2
        SbString& word1 = words[1];
        fEncoding = word1;
      } else if(word0=="smoothing") {
        SbStyle_check_2
        SbStyle_word1_bool
        fSmoothing = value;
      } else if(word0=="hinting") {
        SbStyle_check_2
        SbStyle_word1_bool
        fHinting = value;
      } else if(word0=="enforced") {
        SbStyle_check_2
        SbStyle_word1_bool
        fEnforced = value;
      } else if(word0=="cut") {
        fCut = "";
        for(int wordi=1;wordi<wordn;wordi++) {
          if(wordi!=1) fCut += " ";
          fCut += words[wordi];
        }
      } else if(word0=="options") {
        fOptions = "";
        for(int wordi=1;wordi<wordn;wordi++) {
          if(wordi!=1) fOptions += " ";
          fOptions += words[wordi];
        }
      } else if(word0=="colorMapping") {
        fColorMapping = "";
        for(int wordi=1;wordi<wordn;wordi++) {
          if(wordi!=1) fColorMapping += " ";
          fColorMapping += words[wordi];
        }
      } else if(word0=="painting") {
        SbStyle_check_2
        SbString& word1 = words[1];
        if(stringToPaintingPolicy(word1,fPaintingPolicy)==FALSE) {
          if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
            "%s not a SbPaintingPolicy.",word1.getString());
          return FALSE;
        }
      } else if(word0=="hatching") {
        SbStyle_check_2
        SbString& word1 = words[1];
        if(stringToHatchingPolicy(word1,fHatchingPolicy)==FALSE) {
          if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
            "%s not a SbHatchingPolicy.",word1.getString());
          return FALSE;
        }
      } else if(word0=="projection") {
        SbStyle_check_2
        SbString& word1 = words[1];
        if(stringToProjectionType(word1,fProjectionType)==FALSE) {
          if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
            "%s not a SbProjectionType.",word1.getString());
          return FALSE;
        }   
      } else if(word0=="fontName") {
        SbStyle_check_2
        SbString& word1 = words[1];
        setFontName(word1);
      } else if(word0=="lineWidth") {
        SbStyle_check_2
        SbStyle_word1_int
        fLineWidth = value;
      } else if(word0=="markerSize") {
        SbStyle_check_2
        SbStyle_word1_int
        fMarkerSize = value;
      } else if(word0=="fontSize") {
        SbStyle_check_2
        SbStyle_word1_int
        fFontSize = value;
      } else if(word0=="transparency") {
        SbStyle_check_2
        SbStyle_word1_float
        if(value<0.F) value = 0.F;
        fTransparency = value;
      } else if(word0=="multiNodeLimit") {
        SbStyle_check_2
        SbStyle_word1_int
        fMultiNodeLimit = value;
      } else if(word0=="divisions") {
        SbStyle_check_2
        SbStyle_word1_int
        fDivisions = value;
      } else if(word0=="rotationSteps") {
        SbStyle_check_2
        SbStyle_word1_int
        fRotationSteps = value;
      } else if(word0=="angle") {
        SbStyle_check_2
        SbStyle_word1_float
        fAngle = value;
      } else if(word0=="scale") {
        SbStyle_check_2
        SbStyle_word1_float
        fScale = value;
      } else if(word0=="offset") {
        SbStyle_check_2
        SbStyle_word1_float
        fOffset = value;
      } else if(word0=="stripWidth") {
        SbStyle_check_2
        SbStyle_word1_float
        fStripWidth = value;
      } else if(word0=="spacing") {
        SbStyle_check_2
        SbStyle_word1_float
        fSpacing = value;
      } else if(word0=="visible") {
        SbStyle_check_2
        SbStyle_word1_bool
        fVisible = value;
      } else if(word0=="editable") {
        SbStyle_check_2
        SbStyle_word1_bool
        fEditable = value;
      } else if(word0=="automated") {
        SbStyle_check_2
        SbStyle_word1_bool
        fAutomated = value;
      } else if(word0=="barOffset") {
        SbStyle_check_2
        SbStyle_word1_float
        fBarOffset = value;
      } else if(word0=="barWidth") {
        SbStyle_check_2
        SbStyle_word1_float
        fBarWidth = value;

      } else if(word0=="translation") {
        if (wordn==4) {
          SbString& sx = words[1];
          float x;
          if(SbConvertToFloat(sx,x)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),sx.getString());
            return FALSE;
          }
          SbString& sy = words[2];
          float y;
          if(SbConvertToFloat(sy,y)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),sy.getString());
            return FALSE;
          }
          SbString& sz = words[3];
          float z;
          if(SbConvertToFloat(sz,z)==FALSE)  {
            if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
              "%s: %s not a number.",aString.getString(),sz.getString());
            return FALSE;
          }
          fTranslation.setValue(x,y,z);
        } else {
          if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
            "%s: %s has a bad word count (two or four expected).",
            aString.getString(),line.getString());
          return FALSE;
        }

      } else {
        if(aErrorFunc) aErrorFunc("SbStyle::setFromString",
          "SbStyle::setFromString: bad option %s.",word0.getString());
        return FALSE;
      }      
    } 
    return TRUE;
  }

#undef SbStyle_check_2
#undef SbStyle_word1_bool
#undef SbStyle_word1_int
#undef SbStyle_word1_float


private:
  static int NoLimit() {return (-1);}

private:
  static float hls_to_rgb(float rn1, float rn2, float huei) {
    // taken from ROOT/TColor.
    float hue = huei;
    if (hue > 360) hue = hue - 360;
    if (hue < 0)   hue = hue + 360;
    if (hue < 60 ) return rn1 + (rn2-rn1)*hue/60;
    if (hue < 180) return rn2;
    if (hue < 240) return rn1 + (rn2-rn1)*(240-hue)/60;
    return rn1;
  }
private: 
  SbColor fColor;
  SbColor fHighlightColor;
  int fLineWidth;
  int fMarkerSize;
  int fFontSize;
  SbLinePattern fLinePattern;
  SbMarkerStyle fMarkerStyle;
  SbAreaStyle fAreaStyle;
  SbString fModeling; 
  SbString fLightModel; 
  SbString fTickModeling; 
  SbString fEncoding; 
  SbBool fSmoothing;
  SbBool fHinting;
  SbString fCut;
  SbPaintingPolicy fPaintingPolicy;
  SbHatchingPolicy fHatchingPolicy;
  SbProjectionType fProjectionType;
  SbString fFontName;
  int fMultiNodeLimit;
  int fDivisions;
  int fRotationSteps;
  float fTransparency;
  float fSpacing;
  float fAngle;
  float fScale;
  float fOffset;
  float fStripWidth;
  SbBool fVisible;
  float fBarOffset;
  float fBarWidth;
  SbBool fEditable;
  SbBool fAutomated;
  SbString fOptions;
  SbString fColorMapping;
  SbBool fEnforced;
  SbVec3f fTranslation;
};

#endif
