//  20/08/1998 : G.Barrand : creation.

#ifndef HEPVis_SbPlottableThings_h
#define HEPVis_SbPlottableThings_h

// Pure abstract interfaces.

class SbPlottableObject {
public:
  virtual ~SbPlottableObject(){}
public:
  virtual bool isValid() const = 0;
  virtual void* cast(const char*) const = 0;
  virtual const char* getInfos(const char*) = 0;

  virtual const char* getName() = 0;
  virtual const char* getLabel() = 0;
  virtual const char* getLegend() = 0;

  virtual int getDimension() const = 0;

  // no more used :
  //virtual void* nativeObject() const = 0;
  //virtual const char* nativeObjectClass() const = 0;
};

class SbPlottableBins1D : public virtual SbPlottableObject {
public:
  virtual ~SbPlottableBins1D(){}
public:
  virtual void getBinsSumOfWeightsRange(float&,float&) const = 0;
public:
  virtual int getAxisNumberOfBins() const = 0;
  virtual float getAxisMinimum() const = 0;
  virtual float getAxisMaximum() const = 0;
  virtual float getBinLowerEdge(int) const = 0;
  virtual float getBinUpperEdge(int) const = 0;
  virtual bool hasEntriesPerBin() const = 0;
  virtual int getBinNumberOfEntries(int) const = 0;
  virtual float getBinSumOfWeights(int) const = 0;
  virtual float getBinBar(int) const = 0;
};

class SbPlottableBins2D : public virtual SbPlottableObject {
public:
  virtual ~SbPlottableBins2D(){}
public:
  virtual void getBinsSumOfWeightsRange(float&,float&) const = 0;
public:
  virtual int getAxisNumberOfBinsX() const = 0;
  virtual float getAxisMinimumX() const = 0;
  virtual float getAxisMaximumX() const = 0;
  virtual int getAxisNumberOfBinsY() const = 0;
  virtual float getAxisMinimumY() const = 0;
  virtual float getAxisMaximumY() const = 0;
  virtual float getBinLowerEdgeX(int) const = 0;
  virtual float getBinUpperEdgeX(int) const = 0;
  virtual float getBinLowerEdgeY(int) const = 0;
  virtual float getBinUpperEdgeY(int) const = 0;
  virtual bool hasEntriesPerBin() const = 0;
  virtual int getBinNumberOfEntries(int,int) const = 0;
  virtual float getBinSumOfWeights(int,int) const = 0;
  virtual float getBinBar(int,int) const = 0;
};

class SbPlottablePoints2D : public virtual SbPlottableObject {
public:
  virtual ~SbPlottablePoints2D(){}
public:
  virtual float getAxisMinimumX() const = 0;
  virtual float getAxisMaximumX() const = 0;
  virtual float getAxisMinimumY() const = 0;
  virtual float getAxisMaximumY() const = 0;
  virtual int getNumberOfPoints() const = 0;
  virtual void getIthPoint(int,float&,float&) const = 0;
};

class SbPlottablePoints3D : public virtual SbPlottableObject {
public:
  virtual ~SbPlottablePoints3D(){}
public:
  virtual float getAxisMinimumX() const = 0;
  virtual float getAxisMaximumX() const = 0;
  virtual float getAxisMinimumY() const = 0;
  virtual float getAxisMaximumY() const = 0;
  virtual float getAxisMinimumZ() const = 0;
  virtual float getAxisMaximumZ() const = 0;
  virtual int getNumberOfPoints() const = 0;
  virtual void getIthPoint(int,float&,float&,float&) const = 0;
};

class SbPlottableFunction1D : public virtual SbPlottableObject {
public:
  virtual ~SbPlottableFunction1D(){}
public:
  virtual bool value(float,float&) const = 0;
  virtual int getNumberOfStepsX() const = 0;
  virtual float getMinimumX() const = 0;
  virtual float getMaximumX() const = 0;
};

class SbPlottableFunction2D : public virtual SbPlottableObject {
public:
  virtual ~SbPlottableFunction2D(){}
public:
  virtual bool value(float,float,float&) const = 0;
  virtual int getNumberOfStepsX() const = 0;
  virtual float getMinimumX() const = 0;
  virtual float getMaximumX() const = 0;
  virtual int getNumberOfStepsY() const = 0;
  virtual float getMinimumY() const = 0;
  virtual float getMaximumY() const = 0;
  //For "inside" functions :
  virtual int getNumberOfPoints() const = 0;
  virtual void getIthPoint(int,float&,float&,bool&) const = 0;
  virtual bool setIthPoint(int,float,float) = 0;
  virtual bool dragger_update_points() const = 0;
};

class SbPlottablePrimitive {
public:
  virtual ~SbPlottablePrimitive(){}
public:
  virtual void* cast(const char*) const = 0;
};

#define SbPlottableObject_s     "SbPlottableObject"
#define SbPlottableBins1D_s     "SbPlottableBins1D"
#define SbPlottableBins2D_s     "SbPlottableBins2D"
#define SbPlottablePoints2D_s   "SbPlottablePoints2D"
#define SbPlottablePoints3D_s   "SbPlottablePoints3D"
#define SbPlottableFunction1D_s "SbPlottableFunction1D"
#define SbPlottableFunction2D_s "SbPlottableFunction2D"

#define SbPlottablePrimitive_s "SbPlottablePrimitive"

#define Sb_plotter "plotter"
#define Sb_data "data"

#endif


