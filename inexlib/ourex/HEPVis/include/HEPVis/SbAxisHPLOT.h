//  12/04/2005 : G.Barrand : creation.

#ifndef HEPVis_SbAxisHPLOT_h
#define HEPVis_SbAxisHPLOT_h
 
#include <string>
#include <vector>
#include <Inventor/SbLinear.h>

class SbAxisHPLOT_Text {
public:
  SbAxisHPLOT_Text(double aX,double aY,
                   double aAngle,double aSize,
                   const std::string& aString,
                   short aAlign)
  :fX(aX),fY(aY)
  ,fAngle(aAngle),fSize(aSize)
  ,fString(aString),fAlign(aAlign){}
  SbAxisHPLOT_Text(const SbAxisHPLOT_Text& aFrom)
  :fX(aFrom.fX),fY(aFrom.fY)
  ,fAngle(aFrom.fAngle),fSize(aFrom.fSize)
  ,fString(aFrom.fString)
  ,fAlign(aFrom.fAlign)
  {}
  SbAxisHPLOT_Text& operator=(const SbAxisHPLOT_Text& aFrom){
    fX = aFrom.fX;
    fY = aFrom.fY;
    fAngle = aFrom.fAngle;
    fSize = aFrom.fSize;
    fString = aFrom.fString;
    fAlign = aFrom.fAlign;
    return *this;
  }
public:
  double fX;
  double fY;
  double fAngle; //Degree
  double fSize;
  std::string fString;
  short fAlign;
};


class SbAxisHPLOT {
public:
  SbAxisHPLOT();
  virtual ~SbAxisHPLOT();
  void paint(double xmin,double ymin,double xmax,double ymax,
             double& wmin,double& wmax,int& ndiv,
             const std::string&,
             double gridlength,bool drawGridOnly, 
             std::vector< std::pair<SbVec3f,SbVec3f> >&,
             std::vector< std::pair<SbVec3f,SbVec3f> >&,
             std::vector<SbAxisHPLOT_Text>&);
  void setTitle(const std::string&);
  void setTimeFormat(const std::string&);
  void setTimeOffset(double,bool is_gmt = false);
  void setTickSize(float);
  void setLabelOffset(float);
  void setLabelSize(float);
  void setTitleOffset(float);
  void setTitleSize(float);
private:
  bool testBit(unsigned int);
  static void adjustBinSize(double A1,double A2,int nold,
                            double &BinLow, double &BinHigh, 
                            int &nbins, double &BinWidth);
  static void optimizeLimits(double,double,int,
                             double&,double&,int&,double&,
                             const std::string&);
private:
  int fMaxDigits; //!Number of digits above which the 10>N notation is used 
private:
  //TObject :
  unsigned int fBits;       //bit field status word
  float    fTickSize;            //Size of primary tick mark in NDC
  float    fLabelOffset;         //Offset of label wrt axis
  float    fLabelSize;           //Size of labels in NDC
  float    fTitleOffset;         //Offset of title wrt axis
  float    fTitleSize;           //Size of title in NDC
  int      fLabelFont;           //Font for labels
  std::string fTitle;               //axis title
  std::string fTimeFormat;          //Time format, ex: 09/12/99 12:34:00
};

#endif
