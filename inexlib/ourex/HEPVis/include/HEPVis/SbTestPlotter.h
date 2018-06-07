
// Dummy classes to test the plotter.

// File used by the Plotter.cxx example.

/////////////////////////////////////////////
// Some dummy classes to test the plotter. //
/////////////////////////////////////////////

#include <Inventor/SbString.h>

#include <HEPVis/SbMath.h>

#include <cstring> //strcmp

class SbTestHistogram1D {
public:
  SbTestHistogram1D():fName("histo_1D"),fTitle("Histo 1D") {
    fBinn[0] = 1; fBinw[0] = 1.0F; fBine[0] = 0.1F;
    fBinn[1] = 1; fBinw[1] = 1.4F; fBine[1] = 0.15F;
    fBinn[2] = 1; fBinw[2] = 2.1F; fBine[2] = 0.1F;
    fBinn[3] = 2; fBinw[3] = 2.7F; fBine[3] = 0.2F;
    fBinn[4] = 3; fBinw[4] = 2.8F; fBine[4] = 0.2F;
    fBinn[5] = 2; fBinw[5] = 3.1F; fBine[5] = 0.3F;
    fBinn[6] = 1; fBinw[6] = 2.4F; fBine[6] = 0.2F;
    fBinn[7] = 1; fBinw[7] = 1.6F; fBine[7] = 0.1F;
    fBinn[8] = 1; fBinw[8] = 0.3F; fBine[8] = 0.05F;
    fBinn[9] = 1; fBinw[9] = 0.1F; fBine[9] = 0.0F;
  }
public:
  const char* name() const { return fName.getString();}
  const char* title () const { return fTitle.getString();}
  int xn() const { return 10; }
  float xmin() const { return 0.; }
  float xmax() const { return 1.; }
  //const char* xtitle() const { 
  //  static char xtitle[] = "x title";
  //  return xtitle; 
  //}
  void valueRange(float& mn,float& mx) const {    
    mn = mx = fBinw[0];
    for(int i=0;i<10;i++) {
      mn = SbMinimum(mn,fBinw[i]);
      mx = SbMaximum(mx,fBinw[i]);
    }
  }
  int binEntries(int i) const { return fBinn[i]; }
  float binWeight(int i) const { return fBinw[i]; }
  float binError(int i) const { return fBine[i]; }
private:
  SbString fName;
  SbString fTitle;
  int fBinn[10];
  float fBinw[10];
  float fBine[10];
};

class SbTestHistogram2D {
public:
  SbTestHistogram2D():fName("histo_2D"),fTitle("Histo 2D") {
    for(int i=0;i<10;i++) {
      for(int j=0;j<10;j++) {
        fBinn[i][j] = 0;
        fBinw[i][j] = 0.;
        fBine[i][j] = 0.;
      }}
    fBinn[0][0] = 1; fBinw[0][0] = 2.1F; fBine[0][0] = 0;
    fBinn[0][9] = 1; fBinw[0][9] = 2.1F; fBine[0][9] = 0;
    fBinn[9][0] = 1; fBinw[9][0] = 2.1F; fBine[9][0] = 0;
    fBinn[9][9] = 1; fBinw[9][9] = 2.1F; fBine[9][9] = 0;
    //  010
    //  242
    //  010
    // black 1 cyan 2 green 4 orange
    fBinn[5][5] = 1; fBinw[5][5] = 4.4F; fBine[5][5] = 0;
    fBinn[6][5] = 1; fBinw[6][5] = 2.3F; fBine[6][5] = 0;
    fBinn[4][5] = 1; fBinw[4][5] = 2.4F; fBine[4][5] = 0;
    fBinn[5][6] = 1; fBinw[5][6] = 1.8F; fBine[5][6] = 0;
    fBinn[5][4] = 1; fBinw[5][4] = 1.9F; fBine[5][4] = 0;
    fBinn[4][4] = 1; fBinw[4][4] = 0.6F; fBine[4][4] = 0;
    fBinn[6][6] = 1; fBinw[6][6] = 0.7F; fBine[6][6] = 0;
    fBinn[6][4] = 1; fBinw[6][4] = 0.6F; fBine[6][4] = 0;
    fBinn[4][6] = 1; fBinw[4][6] = 0.7F; fBine[4][6] = 0;
  }
public:
  const char* name() const { return fName.getString();}
  const char* title () const { return fTitle.getString();}
  int xn() const {return 10;}
  float xmin() const {return 0.;}
  float xmax() const {return 1.;}
  //const char* xtitle() const { 
  //  static char xtitle[] = "x title";
  //  return xtitle; 
  //}
  int yn() const {return 10;}
  float ymin() const {return 0.;}
  float ymax() const {return 1.;}
  const char* ytitle() const { 
    static char ytitle[] = "y title";
    return ytitle; 
  }
  void valueRange(float& mn,float& mx) const {    
    mn = mx = fBinw[0][0];
    for(int i=0;i<10;i++) {
      for(int j=0;j<10;j++) {
        mn = SbMinimum(mn,fBinw[i][j]);
        mx = SbMaximum(mx,fBinw[i][j]);
      }}
  }
  int binEntries(int i,int j) const { return fBinn[i][j]; }
  float binWeight(int i,int j) const { return fBinw[i][j]; }
  float binError(int i,int j) const { return fBine[i][j]; }
private:
  SbString fName;
  SbString fTitle;
  int fBinn[10][10];
  float fBinw[10][10];
  float fBine[10][10];
};

////////////////////////////////////////////////////////
///////// Histogram interface to SoPlotter /////////////
////////////////////////////////////////////////////////
#include <HEPVis/SbPlottableThings.h>

class SbTestPlottableBins1D : public virtual SbPlottableBins1D {
public: //SbPlottableObject
  virtual bool isValid() const {return true;}
  virtual void* cast(const char* aClass) const {
    if(!::strcmp(aClass,"SbPlottableBins1D")) {
      return (SbPlottableBins1D*)this;
    } else {
      return 0;
    }
  }
  virtual void* nativeObject() const { return 0; }
  virtual int getDimension() const {return 1;}
  virtual const char* getName() {return fHistogram.name();}
  virtual const char* getLabel() {return fHistogram.name();}
  virtual const char* getLegend() {return fHistogram.name();}
  virtual const char* getInfos(const char* aWhat) { 
    if(!::strcmp(aWhat,Sb_plotter)) {
      fInfos = "";
    } else {
      fInfos = "Name\nhdummy\nEntries\n234\nUnder\n1234\nOver\n456"; 
    }
    return fInfos.getString();
  }
public: //SbPlottableBins1D
  virtual void getBinsSumOfWeightsRange(float& mn,float& mx) const {fHistogram.valueRange(mn,mx);}
  virtual int getAxisNumberOfBins() const { return fHistogram.xn(); }
  virtual float getAxisMinimum() const { return fHistogram.xmin(); }
  virtual float getAxisMaximum() const { return fHistogram.xmax(); }
  virtual float getBinLowerEdge(int i) const {
    float dbin = (fHistogram.xmax()-fHistogram.xmin())/fHistogram.xn();
    return fHistogram.xmin() + i * dbin; 
  }
  virtual float getBinUpperEdge(int i) const {
    float dbin = (fHistogram.xmax()-fHistogram.xmin())/fHistogram.xn();
    return fHistogram.xmin() + i * dbin + dbin; 
  }
  virtual bool hasEntriesPerBin() const {return true;}
  virtual int getBinNumberOfEntries(int i) const {return fHistogram.binEntries(i);}
  virtual float getBinSumOfWeights(int i) const {return fHistogram.binWeight(i);}
  virtual float getBinBar(int i) const { return fHistogram.binError(i); }
public:
  SbTestPlottableBins1D(SbTestHistogram1D& aHistogram)
  :fHistogram(aHistogram){}
  virtual ~SbTestPlottableBins1D(){}
private:
  SbTestHistogram1D& fHistogram;
  SbString fInfos;
};

class SbTestPlottableBins2D : public virtual SbPlottableBins2D {
public: //SbPlottableObject
  virtual bool isValid() const {return true;}
  virtual void* cast(const char* aClass) const {
    if(!::strcmp(aClass,"SbPlottableBins2D")) {
      return (SbPlottableBins2D*)this;
    } else {
      return 0;
    }
  }
  virtual void* nativeObject() const { return 0; }
  virtual int getDimension() const {return 2;}
  virtual const char* getName() {return fHistogram.name();}
  virtual const char* getLabel() {return fHistogram.name();}
  virtual const char* getLegend() {return fHistogram.name();}
  virtual const char* getInfos(const char* aWhat) { 
    if(!::strcmp(aWhat,Sb_plotter)) {
      fInfos = "";
    } else {
      fInfos = "Name\nhdummy\nEntries\n234\nUnder\n1234\nOver\n456"; 
    }
    return fInfos.getString();
  }
public: //SbPlottableBins2D
  virtual void getBinsSumOfWeightsRange(float& mn,float& mx) const { 
    fHistogram.valueRange(mn,mx);}
  virtual int getAxisNumberOfBinsX() const { 
    return fHistogram.xn();}
  virtual float getAxisMinimumX() const { 
    return fHistogram.xmin();}
  virtual float getAxisMaximumX() const { 
    return fHistogram.xmax();}
  virtual int getAxisNumberOfBinsY() const { 
    return fHistogram.yn();}
  virtual float getAxisMinimumY() const { 
    return fHistogram.ymin();}
  virtual float getAxisMaximumY() const { 
    return fHistogram.ymax();}
  virtual float getBinLowerEdgeX(int i) const {
    float dbin = (fHistogram.xmax()-fHistogram.xmin())/fHistogram.xn();
    return fHistogram.xmin() + i * dbin; 
  }
  virtual float getBinUpperEdgeX(int i) const {
    float dbin = (fHistogram.xmax()-fHistogram.xmin())/fHistogram.xn();
    return fHistogram.xmin() + i * dbin + dbin; 
  }
  virtual float getBinLowerEdgeY(int i) const {
    float dbin = (fHistogram.ymax()-fHistogram.ymin())/fHistogram.yn();
    return fHistogram.ymin() + i * dbin; 
  }
  virtual float getBinUpperEdgeY(int i) const {
    float dbin = (fHistogram.ymax()-fHistogram.ymin())/fHistogram.yn();
    return fHistogram.ymin() + i * dbin + dbin; 
  }
  virtual bool hasEntriesPerBin() const {return true;}
  virtual int getBinNumberOfEntries(int i,int j) const {return fHistogram.binEntries(i,j);}
  virtual float getBinSumOfWeights(int i,int j) const {return fHistogram.binWeight(i,j);}
  virtual float getBinBar(int i,int j) const {return fHistogram.binError(i,j);}
public:
  SbTestPlottableBins2D(SbTestHistogram2D& aHistogram)
  :fHistogram(aHistogram){}
  virtual ~SbTestPlottableBins2D(){}
private:
  SbTestHistogram2D& fHistogram;
  SbString fInfos;
};

#include <inlib/geom2>
#include <inlib/vec2f>

class SbTestPlottableFunction2D : public virtual SbPlottableFunction2D {
public: //SbPlottableObject
  virtual bool isValid() const {return true;}
  virtual void* cast(const char* aClass) const {
    if(!::strcmp(aClass,"SbPlottableFunction2D")) {
      return (SbPlottableFunction2D*)this;
    } else {
      return 0;
    }
  }
  virtual void* nativeObject() const { return 0; }
  virtual int getDimension() const {return 2;}
  virtual const char* getName() {return fName.getString();}
  virtual const char* getLabel() {return fName.getString();}
  virtual const char* getLegend() {return fName.getString();}
  virtual const char* getInfos(const char* aWhat) { 
    if(!::strcmp(aWhat,Sb_plotter)) {
      fInfos = "";
    } else {
      fInfos = "Name\nhdummy\nEntries\n234\nUnder\n1234\nOver\n456"; 
    }
    return fInfos.getString();
  }
public: //SbPlottableFunction2D
  virtual bool value(float a_x,float a_y,float& a_value) const {
    std::vector<inlib::vec2f> vs;
    vs.push_back(inlib::vec2f(f_xs[0],f_ys[0]));
    vs.push_back(inlib::vec2f(f_xs[1],f_ys[1]));
    vs.push_back(inlib::vec2f(f_xs[2],f_ys[2]));
    vs.push_back(inlib::vec2f(f_xs[0],f_ys[0]));
    a_value = inlib::is_inside(inlib::vec2f(a_x,a_y),vs)?1:0;
    return true;
  }
  virtual int getNumberOfStepsX() const {return 100;}
  virtual float getMinimumX() const {return -1;}
  virtual float getMaximumX() const {return 1;}
  virtual int getNumberOfStepsY() const {return 100;}
  virtual float getMinimumY() const {return -1;}
  virtual float getMaximumY() const {return 1;}
  //For "inside" functions :
  virtual int getNumberOfPoints() const {return 3;}
  virtual void getIthPoint(int a_index,float& a_x,float& a_y,bool& a_control) const {
    a_x = f_xs[a_index];
    a_y = f_ys[a_index];
    a_control = true;
  }
  virtual bool setIthPoint(int a_index,float a_x,float a_y) {
    f_xs[a_index] = a_x;
    f_ys[a_index] = a_y;
    return true;
  }
  virtual bool dragger_update_points() const {return true;}
public:
  SbTestPlottableFunction2D():fName("func_2D"){
    f_xs[0] = -1;f_ys[0] = -1;
    f_xs[1] =  1;f_ys[1] = -1;
    f_xs[2] =  0;f_ys[2] =  1;
  }
  virtual ~SbTestPlottableFunction2D(){}
private:
  SbString fName;
  SbString fInfos;
  float f_xs[3];
  float f_ys[3];
};
