#ifndef HEPVis_SbColormap_h
#define HEPVis_SbColormap_h

#include <vector>
#include <string>
#include <cfloat> //FLT_MAX

#include <Inventor/SbColor.h>
#include <Inventor/errors/SoDebugError.h>

#include <HEPVis/SbStyle.h>
#include <HEPVis/SbConvert.h>
#include <HEPVis/SbMath.h> //FLOG10

class SbBaseColormap {
public:
  virtual SbColor getColor(float a_ratio) const = 0;
  virtual void* cast(const std::string&) const = 0;
public:
  SbBaseColormap(){}
  SbBaseColormap(const SbBaseColormap& aFrom)
  :fValues(aFrom.fValues),fColors(aFrom.fColors){}
  SbBaseColormap& operator=(const SbBaseColormap& aFrom){ 
    fValues = aFrom.fValues;
    fColors = aFrom.fColors;
    return *this;
  }
  virtual ~SbBaseColormap(){}
public:
  unsigned int colorn() const {return fColors.size();}
  unsigned int valn() const {return fValues.size();}
  SbColor color(unsigned int a_index) const {
    unsigned int n = fColors.size();
    if(a_index>=n) return SbColor(0.5F,0.5F,0.5F);
    return fColors[a_index];
  }
  float value(unsigned int a_index) const {
    unsigned int n = fValues.size();
    if(a_index>=n) return 0;
    return fValues[a_index];
  }
public:
  // helper :
  void setColors(SbColor(*aGet)(float),unsigned int aCelln) {
    fColors.resize(aCelln);
    if(!aCelln) return;
    float d = 1.0F/(aCelln-1);
    for(unsigned int index=0;index<aCelln;index++) {
      fColors[index] = aGet(d*index);
    }
  }

  void set_PAW_coloring() {
    unsigned int valn = fValues.size();
    if(valn==1) {
      fValues[0] = TakeLog(fValues[0]);
    } else if(valn>=2) {
      //CERN/PAW coloring :
      if(fValues[0]==0) fValues[0] = 10e-5F;
      float vmin = TakeLog(fValues[0]);
      float vmax = TakeLog(fValues[valn-1]);
      float dv = (vmax-vmin)/(valn-1);
      for(unsigned int count=0;count<valn;count++) {
        fValues[count] = vmin + dv * count;
      }
    }
  }

private:
  static float TakeLog(float aVal){
    if(aVal<=0) {
      return -FLT_MAX;
    } else {
      return FLOG10(aVal);
    }
  }

protected:
  std::vector<float> fValues;
  std::vector<SbColor> fColors;
};

#define if_Sb_SCast(a__class) if(a_class==#a__class) {return (void*)static_cast<const a__class*>(this);}

class SbByValueColormap : public SbBaseColormap {
public:
  virtual SbColor getColor(float a_value) const{
    return getByValue(a_value,fValues,fColors);
  }
  virtual void* cast(const std::string& a_class) const {
    if_Sb_SCast(SbByValueColormap)
    else if_Sb_SCast(SbBaseColormap)
    else return 0;
  }
public:
  SbByValueColormap(const SbString& aString){
    setByValue(aString,fValues,fColors);
  }
  SbByValueColormap(const SbByValueColormap& aFrom):SbBaseColormap(aFrom){}
  SbByValueColormap& operator=(const SbByValueColormap& aFrom){ 
    SbBaseColormap::operator=(aFrom);
    return *this;
  }
  virtual ~SbByValueColormap(){}
private:
  static void setByValue(const SbString& aString,std::vector<float>& aValues,std::vector<SbColor>& aColors){
    //  The given string is of the format :
    //      [<color name> <value>] <color name>
    //  or :
    //      [<value> <color name>] <value>
    //  For example :
    //      black 10 cyan 50 green 100 orange 200 blue 300 pink 400 red
    std::vector<SbString> words = SbGetWords(aString," ");
    int wordn = words.size();
    int number = wordn/2;
    if(number<=0) {
      aValues.clear();
      aColors.clear();
    } else if((2*number+1)!=wordn) {
      SoDebugError::postInfo("SbByValueColormap::setByValue",
        "An odd number (%d given) of words is expected in :\n%s\n",
        wordn,aString.getString());
      aValues.clear();
      aColors.clear();
    } else {
      // look if :
      //   <col> <num> <col> ... <num> <col>
      // or :
      //   <num> <col> <num> ... <col> <num>
      // FIXME : case of <col> being three floats ?
      SbString& word = words[0];
      SbColor c;
      if(SbStyle::stringToColor(word,c)==TRUE) {
        // <col> <num> <col> ... <num> <col>
        aValues.resize(number);
        aColors.resize(number+1);
        for(int count=0;count<number;count++) {
         {SbString& word = words[2*count];
          if(SbStyle::stringToColor(word,aColors[count])==FALSE) {
            SoDebugError::postInfo("SbByValueColormap::setByValue",
              "%s: %s not a color.\n",aString.getString(),word.getString());
            aValues.clear();
            aColors.clear();
            return;
          }}
         {SbString& word = words[2*count+1];
          if(SbConvertToFloat(word,aValues[count])==FALSE)  {
            SoDebugError::postInfo("SbByValueColormap::setByValue",
              "%s: %s not a number.\n",aString.getString(),word.getString());
            aValues.clear();
            aColors.clear();
            return;
          }}
        }
        SbString& word = words[wordn-1];
        if(SbStyle::stringToColor(word,aColors[number])==FALSE) {
          SoDebugError::postInfo("SbByValueColormap::setByValue",
            "%s: %s not a color.\n",aString.getString(),word.getString());
          aValues.clear();
          aColors.clear();
        }
      } else {
        // <num> <col> <num> ... <col> <num>
        aValues.resize(number+1);
        aColors.resize(number);
        for(int count=0;count<number;count++) {
         {SbString& word = words[2*count];
          if(SbConvertToFloat(word,aValues[count])==FALSE)  {
            SoDebugError::postInfo("SbByValueColormap::setByValue",
              "%s: %s not a number.\n",aString.getString(),word.getString());
            aValues.clear();
            aColors.clear();
            return;
          }}
         {SbString& word = words[2*count+1];
          if(SbStyle::stringToColor(word,aColors[count])==FALSE) {
            SoDebugError::postInfo("SbByValueColormap::setByValue",
              "%s: %s not a color.\n",aString.getString(),word.getString());
            aValues.clear();
            aColors.clear();
            return;
          }}
        }
       {SbString& word = words[wordn-1];
        if(SbConvertToFloat(word,aValues[number])==FALSE)  {
          SoDebugError::postInfo("SbByValueColormap::setByValue",
            "%s: %s not a number.\n",aString.getString(),word.getString());
          aValues.clear();
          aColors.clear();
          return;
        }}
      }
    } 
  }

  static SbColor getByValue(float aValue,const std::vector<float>& aValues,const std::vector<SbColor>& aColors){
    // There are aValuen (n) entries in aValues and (n+1) aColors
    //  aColors[0] aValues[0] aColors[1] aValues[1]...
    //                 aValues[n-2] aColors[n-1] aValues[n-1] aColors[n]
    //      black 10 cyan 50 green 100 orange 200 blue 300 pink 400 red
    // valuen = 6
    // values[0] 10 
    // values[1] 50 
    // values[2] 100 
    // values[3] 200 
    // values[4] 300 
    // values[5] 400 
    //
    // colors[0] black
    // colors[1] cyan
    // colors[2] green
    // colors[3] orange
    // colors[4] blue
    // colors[5] pink
    // colors[6] red
    unsigned valn = aValues.size();
    if(!valn) return SbColor_black;
    if(aColors.size()==(valn+1)) {
      // col0 val0 col1 val1 col2 val2 col3
      if(aValue<aValues[0]) {
        return aColors[0];
      } else {
        for(unsigned int count=0;count<=(valn-2);count++) {
          if( (aValues[count]<=aValue) && (aValue<aValues[count+1]) ) {
            return aColors[count+1];
          }
        }
        return aColors[valn];
      }
    } else if((aColors.size()+1)==valn) {
      // val0 col0 val1 col1 val2 col2 val3
      for(unsigned int count=0;count<=(valn-2);count++) {
        if( (aValues[count]<=aValue) && (aValue<aValues[count+1]) ) {
          return aColors[count];
        }
      }
      if(aValue<aValues[0]) return aColors[0];
      if(aValue>=aValues[valn-1]) return aColors[aColors.size()-1];
      return SbColor_black;
    } else {
      return SbColor_black;
    }
  }
  
};

//static void setGreyScale(unsigned int aCelln,std::vector<SbColor>& aColors){setColors(getGrey,aCelln,aColors);}

class SbGreyScaleColormap : public SbBaseColormap {
public:
  virtual SbColor getColor(float a_value) const{ //a_value in [0,1]
    return getGrey(a_value);
  }
  virtual void* cast(const std::string& a_class) const {
    if_Sb_SCast(SbGreyScaleColormap)
    else if_Sb_SCast(SbBaseColormap)
    else return 0;
  }
public:
  SbGreyScaleColormap(float a_min,float a_max,unsigned int aCelln){
    fValues.resize(2);
    fValues[0] = a_min;
    fValues[1] = a_max;
    setColors(getGrey,aCelln);
  }
  SbGreyScaleColormap(const SbGreyScaleColormap& aFrom):SbBaseColormap(aFrom){}
  SbGreyScaleColormap& operator=(const SbGreyScaleColormap& aFrom){ 
    SbBaseColormap::operator=(aFrom);
    return *this;
  }
  virtual ~SbGreyScaleColormap(){}
private:
  static SbColor getGrey(float a_ratio){
    if(a_ratio<0.0F) a_ratio = 0;
    if(a_ratio>1.0F) a_ratio = 1;
    return SbColor(a_ratio,a_ratio,a_ratio);
  }
};

class SbGreyScaleInverseColormap : public SbBaseColormap {
public:
  virtual SbColor getColor(float a_value) const{ //a_value in [0,1]
    return getGreyInverse(a_value);
  }
  virtual void* cast(const std::string& a_class) const {
    if_Sb_SCast(SbGreyScaleInverseColormap)
    else if_Sb_SCast(SbBaseColormap)
    else return 0;
  }
public:
  SbGreyScaleInverseColormap(float a_min,float a_max,unsigned int aCelln){
    fValues.resize(2);
    fValues[0] = a_min;
    fValues[1] = a_max;
    setColors(getGreyInverse,aCelln);
  }
  SbGreyScaleInverseColormap(const SbGreyScaleInverseColormap& aFrom):SbBaseColormap(aFrom){}
  SbGreyScaleInverseColormap& operator=(const SbGreyScaleInverseColormap& aFrom){ 
    SbBaseColormap::operator=(aFrom);
    return *this;
  }
  virtual ~SbGreyScaleInverseColormap(){}
private:
  static SbColor getGreyInverse(float a_ratio){
    if(a_ratio<0.0F) a_ratio = 0;
    if(a_ratio>1.0F) a_ratio = 1;
    a_ratio = 1 - a_ratio;
    return SbColor(a_ratio,a_ratio,a_ratio);
  }
};

//static void setVioletToRed(unsigned int aCelln,std::vector<SbColor>& aColors){setColors(getVioletToRed,aCelln,aColors);}

class SbVioletToRedColormap : public SbBaseColormap {
public:
  virtual SbColor getColor(float a_value) const { //a_value in [0,1]
    return getVioletToRed(a_value);
  }
  virtual void* cast(const std::string& a_class) const {
    if_Sb_SCast(SbVioletToRedColormap)
    else if_Sb_SCast(SbBaseColormap)
    else return 0;
  }
public:
  SbVioletToRedColormap(float a_min,float a_max,unsigned int aCelln){
    fValues.resize(2);
    fValues[0] = a_min;
    fValues[1] = a_max;
    setColors(getVioletToRed,aCelln);
  }
  SbVioletToRedColormap(const SbVioletToRedColormap& aFrom):SbBaseColormap(aFrom){}
  SbVioletToRedColormap& operator=(const SbVioletToRedColormap& aFrom){ 
    SbBaseColormap::operator=(aFrom);
    return *this;
  }
  virtual ~SbVioletToRedColormap(){}
private:
  static SbColor getVioletToRed(float a_ratio){
    if(a_ratio<0.0F) a_ratio = 0;
    if(a_ratio>1.0F) a_ratio = 1;
    // a_ratio in [0,1]
    // From ROOT pretty palette.
    // Initialize with the spectrum Violet->Red
    // The color model used here is based on the HLS model which
    // is much more suitable for creating palettes than RGB.
    // Fixing the saturation and lightness we can scan through the
    // spectrum of visible light by using "hue" alone.
    // In Root hue takes values from 0 to 360.
    float saturation = 1;
    float lightness = 0.5;
    float hue_mn = 0;
    float hue_mx = 280;
    float hue = hue_mx - a_ratio * (hue_mx-hue_mn);
    float r,g,b;
    SbStyle::hls2rgb(hue,lightness,saturation,r,g,b);
    return SbColor(r,g,b);
  }

};

class SbConstColormap : public SbBaseColormap {
public:
  virtual SbColor getColor(float) const {return fColors[0];}
  virtual void* cast(const std::string& a_class) const {
    if_Sb_SCast(SbConstColormap)
    else if_Sb_SCast(SbBaseColormap)
    else return 0;
  }
public:
  SbConstColormap(const SbColor& aColor){fColors.push_back(aColor);}
  SbConstColormap(const SbConstColormap& aFrom):SbBaseColormap(aFrom){}
  SbConstColormap& operator=(const SbConstColormap& aFrom){ 
    SbBaseColormap::operator=(aFrom);
    return *this;
  }
  virtual ~SbConstColormap(){}
};

#undef if_Sb_SCast

#endif
