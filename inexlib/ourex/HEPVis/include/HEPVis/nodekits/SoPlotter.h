/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoPlotter                                               */
/* Description:      To plot histograms, functions.                          */
/* Author:           Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoPlotter_h
#define HEPVis_SoPlotter_h

/*
 * SoPlotter is a nodekit to plot bins, points, functions.
 * 
 *  SoPlotter knows only the abstract classes :
 *   SbPlottableBins[1,2]D, SbPlottablePoints[2,3]D,...
 * These classes permit SoPlotter to retreive
 * informations about things to plot ; bins,
 * function,... The virtuality of the SbPlottable
 * classes permits to plug many histogram packages
 * to SoPlotter. 
 *  See the applications/Plotter example.
 */

/*
 * The modeling is done in a centered [width x height] area.
 * The default for width and height is 1.
 * The origin of the axes (being the lower left corner of the data area)
 * is at :
 *    -XSIZ/2 + XMGR
 *    -YSIZ/2 + YMGL
 * with :
 *    XSIZ = width.getValue();
 *    YSIZ = height.getValue();
 *    XMGL = leftMargin.getValue();
 *    XMGR = rightMargin.getValue();
 *    YMGL = bottomMargin.getValue();
 *    YMGU = topMargin.getValue();
 */

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

//Inventor :
#include <Inventor/SbString.h>
#include <Inventor/SbPList.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFUInt32.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoMFBool.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoSFEnum.h>

#include <HEPVis/SbAxis.h>
#include <HEPVis/SbStyles.h>
#include <HEPVis/SbStyle.h>

#include <vector>

class SbPlottableObject;
class SbPlottableBins1D;
class SbPlottableBins2D;
class SbPlottableFunction1D;
class SbPlottableFunction2D;
class SbPlottablePoints2D;
class SbPlottablePoints3D;
class SbBaseColormap;
class SbPlottablePrimitive;

class SbPlottableText;
class SbPlottableBox;
//namespace HEPVis {class SbMutex;}

namespace inlib {class clist_contour;}

class SbColor;
class SoSeparator;
class SoTransform;
class SoPlotter;
class SoDrawStyle;
class SoCoordinate3;
class SoIndexedFaceSet;
class SoHighlightMaterial;
class SoStyle;
class SoTextStyle;
class SoStyleCache;
class SoAxes2D;
class SoAxes3D;
class SoAxis;
class SoNodeKitListPart;
class SoScale;

class SbRepBin1D {
public:
  SbRepBin1D():fXmin(0),fXmax(0),fVmin(0),fVal(0),fRatio(0),fSeparator(0){}
  SbRepBin1D(float aXmin,float aXmax,float aVmin,float aVal)
  :fXmin(aXmin),fXmax(aXmax),fVmin(aVmin),fVal(aVal),fRatio(0),fSeparator(0){}
public:
  float fXmin;
  float fXmax;
  float fVmin;
  float fVal;
  float fRatio;
  SoSeparator* fSeparator;
};

class SbRepBin2D {
public:
  SbRepBin2D()
  :fXmin(0),fXmax(0),fYmin(0),fYmax(0),fVal(0),fRatio(0),fI(0),fJ(0){}
  SbRepBin2D(float aXmin,float aXmax,float aYmin,float aYmax,float aVal,int aI,int aJ)
  :fXmin(aXmin),fXmax(aXmax)
  ,fYmin(aYmin),fYmax(aYmax)
  ,fVal(aVal)
  ,fRatio(0)
  ,fI(aI),fJ(aJ){}
public:
  float fXmin;
  float fXmax;
  float fYmin;
  float fYmax;
  float fVal;
  float fRatio;
  int fI;
  int fJ;
};

class SbRepTopFace2D {
public:
  SbRepTopFace2D()
  :fXmin(0),fXmax(0),fYmin(0),fYmax(0),fVal1(0),fVal2(0),fVal3(0),fVal4(0){}
  SbRepTopFace2D(float aXmin,float aXmax,float aYmin,float aYmax,float aVal1,float aVal2,float aVal3,float aVal4)
  :fXmin(aXmin),fXmax(aXmax),fYmin(aYmin),fYmax(aYmax),fVal1(aVal1),fVal2(aVal2),fVal3(aVal3),fVal4(aVal4){}
public:
  float fXmin;
  float fXmax;
  float fYmin;
  float fYmax;
  float fVal1;
  float fVal2;
  float fVal3;
  float fVal4;
};

class SbRepBox {
public:
  SbRepBox(float aPos,float aWidth,SbBool aLog)
  :fPos(aPos),fWidth(aWidth),fLog(aLog){}
  SbRepBox(const SbRepBox& aFrom)
  :fPos(aFrom.fPos),fWidth(aFrom.fWidth),fLog(aFrom.fLog){}
  SbRepBox& operator=(const SbRepBox& aFrom) {
    fPos = aFrom.fPos;
    fWidth = aFrom.fWidth;
    fLog = aFrom.fLog;
    return *this;
  }
public:
  float fPos;
  float fWidth;
  SbBool fLog;
};

class SoPlotter : public SoBaseKit {
  SO_KIT_HEADER(SoPlotter);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(titleSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(layout);
  SO_KIT_CATALOG_ENTRY_HEADER(gridSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(wallSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(xAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(xAxisTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(xAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxisTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(yAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(zAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(zAxisTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(zAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(topAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(rightAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(etcSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(dataSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(dataScale);
  SO_KIT_CATALOG_ENTRY_HEADER(binsSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(pointsSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(functionSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(errorsSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(innerFrameSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(colorMappingSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(colorMappingTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(colorMappingCellsSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(colorMappingAxisSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(colorMappingAxisTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(colorMappingAxis);
  SO_KIT_CATALOG_ENTRY_HEADER(etcDataSeparator);
  // Styles :
  SO_KIT_CATALOG_ENTRY_HEADER(styleSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(wallStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(gridStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(innerFrameStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(infosStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(titleStyle);
  //
  SO_KIT_CATALOG_ENTRY_HEADER(binsStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(pointsStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(functionStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(rightHatchStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(leftHatchStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(errorsStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(legendStyle); //output
public:
  SoSFNode alternateRep;
  SoSFBool verbose;

  SoSFFloat width;         //PAW XSIZ
  SoSFFloat height;        //PAW YSIZ
  SoSFFloat leftMargin;    //PAW XMGL
  SoSFFloat rightMargin;   //PAW XMGR
  SoSFFloat bottomMargin;  //PAW YMGL
  SoSFFloat topMargin;     //PAW YMGU

  SoSFFloat depth;
  SoSFFloat downMargin;    
  SoSFFloat upMargin;

  SoSFBool titleUp;  
  SoSFFloat titleToAxis;  
  SoSFFloat titleHeight;  
  SoSFBool titleAutomated;  
  SoSFString title; //Output
  SoSFBool superposeBins;
  SoSFBool colorMappingVisible;
  enum ColorMappingAxisLabeling {
    CELLS = 0,
    MIN_MAX
  };
  SoSFEnum colorMappingAxisLabeling;
  SoSFBool colorMappingAttached;

  SoSFBool wallEnforced;
  SoSFBool gridEnforced;
  SoSFBool primitivesEnforced;
  SoSFBool innerFrameEnforced;

  // Wanted axes parameters. 
  // They are not necessary realized on the SoAxis nodes.
  SoSFBool xAxisEnforced;
  SoSFBool xAxisAutomated;
  SoSFFloat xAxisMinimum;
  SoSFFloat xAxisMaximum;
  SoSFBool xAxisLogScale;

  SoSFBool yAxisEnforced;
  SoSFBool yAxisAutomated;
  SoSFFloat yAxisMinimum;
  SoSFFloat yAxisMaximum;
  SoSFBool yAxisLogScale;

  SoSFBool zAxisEnforced;
  SoSFBool zAxisAutomated;
  SoSFFloat zAxisMinimum;
  SoSFFloat zAxisMaximum;
  SoSFBool zAxisLogScale;

  SoSFFloat valueTopMargin;

  SoSFBool topAxisVisible;  
  SoSFBool rightAxisVisible;  

  // For contours :
  SoSFInt32 numberOfLevels;
  SoMFFloat levels;

  SoSFBool shapeAutomated;

  SoSFBool frozen;

  SoSFFloat ttfScale;  

  // used with shape XYZ
  SoSFFloat theta;  //in degrees.
  SoSFFloat phi;    //in degrees.

  enum Shape{
    XY = 0,
    XYZ
  };
  SoSFEnum shape; //Output if shapeAutomated == TRUE.
  SoMFString infos; //Output
  SoMFString legend; //Output
  SoSFInt32 numberOfPlottedObject; //Output
  SoSFInt32 numberOfPlottedBins1D; //Output
  SoSFInt32 numberOfPlottedBins2D; //Output
  SoSFInt32 numberOfPlottedPoints2D; //Output
  SoSFInt32 numberOfPlottedPoints3D; //Output
  SoSFInt32 numberOfPlottedFunction1D; //Output
  SoSFInt32 numberOfPlottedFunction2D; //Output
  SoMFString plottedObjectNames; //Output

public:
  SoPlotter();
  void setPlottableObject(SbPlottableObject*);
  void addPlottableObject(SbPlottableObject*);
  void removePlottableObject(SbPlottableObject*);
  void removePlottableObjectsOfClass(const SbString&);
  SbBool isEmpty() const;
  enum UpdateFlag{
    // globals :
    NONE,
    ALL,
    RESET,
    // per scene component :
    SHAPE,
    BINS,
    POINTS,
    FUNCTION,
    WALL,
    GRID,
    PRIMITIVES,
    INNER_FRAME,
    TITLE,
    X_AXIS,
    Y_AXIS,
    Z_AXIS,
    COLOR_MAPPING,
    LAYOUT,
    TTF_SCALE
  };
  void raiseUpdateFlag(UpdateFlag);
  void raiseAllUpdateFlags();
  void resetAllUpdateFlags();
  SbBool isUpdateFlagRaised(UpdateFlag);
  SbBool someFlagRaised() const;
  void updateChildren();
  Shape getShape() const;
  const SbPList& getPlottableObjects() const;
  SoTransform* getLayout() const;
  SoScale* getDataScale() const;
  SoSeparator* getTopSeparator() const;
  SoSeparator* getEtcSeparator() const;
  SoSeparator* getEtcDataSeparator() const;
  SoAxis* getXAxis() const;
  SoAxis* getYAxis() const;
  SoAxis* getZAxis() const;
  SoAxis* getColorMappingAxis() const;

  SoNodeKitListPart* getLegendStyleList() const;
  SoStyle* getWallStyle() const;
  SoStyle* getGridStyle() const;
  SoStyle* getInnerFrameStyle() const;
  SoStyle* getInfosStyle() const;
  SoTextStyle* getTitleStyle() const;

  SoStyle* getBinsStyle(int);
  SoStyle* getPointsStyle(int);
  SoStyle* getFunctionStyle(int);
  SoStyle* getRightHatchStyle(int);
  SoStyle* getLeftHatchStyle(int);
  SoStyle* getErrorsStyle(int);

  SoStyleCache* getStyleCache() const;

  // Style helpers :
  void resetStyles();
  void setAxesModeling(const SbString&);
  void setAxesColor(const SbColor&);
  void setAxesTextScale(float);
  void setAxesLinePattern(unsigned short);
  void setAxesLineWidth(int);
  void setAxesTickLength(float);
  void setAxesLabelHeight(float);
  void setAxesTitleHeight(float);
  void setFont(const SbString&,SbBool,SbBool);
  void setPAW_AxesTextSize(float,float);
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();

  SbBool dataFrame2vp(const SbVec3f&,SbVec3f&) const;
  SbBool vp2dataFrame(const SbVec3f&,SbVec3f&) const;
  SbBool dataFrame2axis(const SbVec3f&,SbVec3f&) const;
  SbBool axis2dataFrame(const SbVec3f&,SbVec3f&) const;
  SbBool axis2vp(const SbVec3f&,SbVec3f&) const;
  SbBool vp2axis(const SbVec3f&,SbVec3f&) const;
private:
  SbBool xx2yy(const SbVec3f&,SbVec3f&) const;
public:

  //SbBool hasMutex() const;
  //void setMutex(HEPVis::SbMutex*);
  //SbBool lock() const;
  //SbBool unlock() const;

  void addPlottablePrimitive(SbPlottablePrimitive*);
  void clearPlottablePrimitives();

  void clear();

public: /*SoINTERNAL*/
  static void initClass();
public: /*SoEXTENDER*/
  virtual void doAction(SoAction*);
  virtual void callback(SoCallbackAction*);
  virtual void GLRender(SoGLRenderAction*);
  virtual void getBoundingBox(SoGetBoundingBoxAction*);
  virtual void getMatrix(SoGetMatrixAction*);
  virtual void handleEvent(SoHandleEventAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void search(SoSearchAction*);
  virtual void write(SoWriteAction*);
  virtual void pick(SoPickAction*);
protected: 
  virtual ~SoPlotter();
private:
  SbBool isBinsStyleChanged();
  SbBool isPointsStyleChanged();
  SbBool isFunctionStyleChanged();
  SbBool isRightHatchStyleChanged();
  SbBool isLeftHatchStyleChanged();
  SbBool isErrorsStyleChanged();

  void setOldBinsStyle();
  void setOldPointsStyle();
  void setOldFunctionStyle();
  void setOldRightHatchStyle();
  void setOldLeftHatchStyle();
  void setOldErrorsStyle();

  SoStyle* mergeBinsStyle(int,SbPlottableObject&);
  SoStyle* mergePointsStyle(int,SbPlottableObject&);
  SoStyle* mergeFunctionStyle(int,SbPlottableObject&);

  //void updateStyles();
  void updateWhat();
  void updateShape();
  void updateSbAxes();
  void updateLayout();
  void updateXAxis2D(SbBool = TRUE); 
  void updateYAxis2D(SbBool = TRUE); 
  void updateZAxis2D(SbBool = TRUE); 
  void updateXAxis3D(SbBool = TRUE); 
  void updateYAxis3D(SbBool = TRUE); 
  void updateZAxis3D(SbBool = TRUE); 
  static void updateAxis(SoAxis*,SbAxis&,SbBool = TRUE);
  void updateLegend();

  void clearPlottableObjects();
  void setPlottableObjects(const SbPList&);
  void set_lists(SbBool);
  Shape get_shape() const;
  SbString getTitle();
  SbString getInfos();
  SoDrawStyle* getLineStyle(const SbStyle&);
  SoDrawStyle* getLineStyle(SoStyle&);
  void updateTopAxis(SbBool = TRUE); 
  void updateRightAxis(SbBool = TRUE); 
  void updateWall2D();
  void updateWall3D();
  void updateGrid2D();
  void updateGrid3D();
  void updateBins1D_XY(const SbPlottableBins1D&,int,SoStyle&,
                       SoStyle&,SoStyle&,SoStyle&,
                       int,const std::vector<SbPlottableBins1D*>&,
                       const SbPList&,SbBool,
                       const SbRepBox&,const SbRepBox&,float);
  void updateBins2D_XY(const SbPlottableBins2D&,int,SoStyle&,
                       const SbRepBox&,const SbRepBox&,const SbRepBox&,float);
  void updatePoints2D_XY(const SbPlottablePoints2D&,SoStyle&,
                         const SbRepBox&,const SbRepBox&,float);
  void updateFunction_XY(const SbPlottableFunction1D&,SoStyle&,
                         const SbRepBox&,const SbRepBox&,const SbRepBox&,
                         float);
  void updateFunction_XY(const SbPlottableFunction2D&,int,SoStyle&,
                         const SbRepBox&,const SbRepBox&,const SbRepBox&,
                         float);
  void updateBins2D_XYZ(const SbPlottableBins2D&,int,
                        SoStyle&,
                        const SbRepBox&,const SbRepBox&,const SbRepBox&);
  void updatePoints3D_XYZ(const SbPlottablePoints3D&,SoStyle&,
                          const SbRepBox&,const SbRepBox&,const SbRepBox&);
  void updateFunction_XYZ(const SbPlottableFunction2D&,int,SoStyle&,
                          const SbRepBox&,const SbRepBox&,const SbRepBox&);
  void updateTitle();
  void updateInnerFrame_XY();
  void updateInnerFrame_XYZ();
  void updateColorMapping(const SbBaseColormap&);

  void updateText(SoSeparator&,const SbPlottableText&);
  void updateBox(SoSeparator&,const SbPlottableBox&);
  void rep_box_hatch(SoSeparator&,
                     float spacing,float angle,float stripWidth,
                     float xx,float yy,
                     float xe,float ye,
                     float aZZ);

  // reps xy
  void repBins1D_xy_top_lines(SoSeparator*, 
                 SoStyle&,
                 SbPaintingPolicy,const SbBaseColormap&,
                 std::vector<SbRepBin1D>&,
                 const SbRepBox&,const SbRepBox&,
                 float,const SbString&);
  void repBins1D_xy_points(SoSeparator*, 
                 SoStyle&,
                 SbPaintingPolicy,const SbBaseColormap&,
                 std::vector<SbRepBin1D>&,
                 const SbRepBox&,const SbRepBox&,
                 float,const SbString&);
  void repBins1D_xy_boxes(SoSeparator*, 
                 SoStyle&,
                 SbPaintingPolicy,const SbBaseColormap&,
                 std::vector<SbRepBin1D>&,
                 const SbRepBox&,const SbRepBox&,
                 float,const SbString&);
  void repBins1D_xy_wire_boxes(SoSeparator*, 
                 SoStyle&,
                 SbBool,
                 SbPaintingPolicy,const SbBaseColormap&,
                 std::vector<SbRepBin1D>&,
                 const SbRepBox&,const SbRepBox&,
                 float,const SbString&);
  void repBins1D_xy_top_lines_one(SoSeparator*, 
                     SoStyle&,
                     const std::vector<SbRepBin1D>&,
                     const SbRepBox&,const SbRepBox&,float,const SbString&);
  void repBins1D_xy_curve_one(SoSeparator*, 
                     SoStyle&,
                     const std::vector<SbRepBin1D>&,
                     const SbRepBox&,const SbRepBox&,float,const SbString&);
  void repBins1D_xy_lines_one(SoSeparator*, 
                     SoStyle&,
                     const std::vector<SbRepBin1D>&,
                     const SbRepBox&,const SbRepBox&,float,const SbString&);
  void repBins1D_xy_points_one(SoSeparator*, 
                     SoStyle&,
                     const std::vector<SbRepBin1D>&,
                     const SbRepBox&,const SbRepBox&,float,const SbString&);
  void repBins1D_xy_boxes_one(SoSeparator*, 
                     SoStyle&,
                     const std::vector<SbRepBin1D>&,
                     const SbRepBox&,const SbRepBox&,float,const SbString&);
  void repBins1D_xy_wire_boxes_one(SoSeparator*, 
                     SoStyle&,
                     SbBool,
                     const std::vector<SbRepBin1D>&,
                     const SbRepBox&,const SbRepBox&,float,const SbString&);
  void repBins2D_xy_random_one(SoSeparator*,
                     SoStyle&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,
                     float,float,const SbString&);
  void repBins2D_xy_box(SoSeparator*,
                     SoStyle&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,
                     float,float,const SbString&);
  void repBins2D_xy_box_one(SoSeparator*,
                     SoStyle&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,
                     float,float,const SbString&);
  void repBins2D_xy_wire_box(SoSeparator*,
                     SoStyle&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,
                     float,float,const SbString&);
  void repBins2D_xy_wire_box_one(SoSeparator*,
                     SoStyle&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,
                     float,float,const SbString&);
  void repBins2D_xy_solid(SoSeparator*,
                     SoStyle&,
                     SbPaintingPolicy,const SbBaseColormap&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,const SbRepBox&,
                     const SbString&);
  void repBins2D_xy_solid_one(SoSeparator*,
                     SoStyle&,
                     SbPaintingPolicy,const SbBaseColormap&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,const SbRepBox&,
                     const SbString&);
  void repBins2D_xy_text(SoSeparator*,
                     SoStyle&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,
                     const SbString&);
  void repBins2D_xy_text_one(SoSeparator*,
                     SoStyle&,
                     const std::vector<SbRepBin2D>&,
                     const SbRepBox&,const SbRepBox&,const SbString&);
  void repContour_xy(SoSeparator*,SoStyle&,
                     SbPaintingPolicy,const SbBaseColormap&,
                     inlib::clist_contour&,
                     const SbRepBox&,const SbRepBox&,const SbRepBox&,float,
                     const SbString&);
  void repContour_xy_filled(SoSeparator*,SoStyle&,
                     SbPaintingPolicy,const SbBaseColormap&,
 		     inlib::clist_contour&,
                     const SbRepBox&,const SbRepBox&,const SbRepBox&,float,
                     const SbString&);
  void repErrorsI_xy(SoSeparator*,SoStyle&,
                  const std::vector<SbRepBin1D>&,
                  const SbRepBox&,const SbRepBox&,
                  const std::vector<float>&,float,const SbString&);
  void repErrorsPlus_xy(SoSeparator*,SoStyle&,
                     const std::vector<SbRepBin1D>&,
                     const SbRepBox&,const SbRepBox&,
                     const std::vector<float>&,float,const SbString&);
  void repPoints2D_xy(SoSeparator*,SoStyle&,
                   const std::vector<SbVec3f>&,
                   const SbRepBox&,const SbRepBox&,float,const SbString&);
  void repHatch1D_xy(SoSeparator*,SoStyle&,SbBool,
                  const std::vector<SbRepBin1D>&,
                  const SbRepBox&,const SbRepBox&,float);
  void repHatch1D_xy_one(SoSeparator*,SoStyle&,SbBool,
                      const std::vector<SbRepBin1D>&,
                      const SbRepBox&,const SbRepBox&,float,const SbString&);

  // reps xyz
  void repBins2D_xyz_boxes(SoSeparator*,
                 SoStyle&,
                 SbPaintingPolicy,const SbBaseColormap&,
                 const std::vector<SbRepBin2D>&,
                 const SbRepBox&,const SbRepBox&,const SbRepBox&,
                 float,float,const SbString&);
  void repBins2D_xyz_boxes_one(SoSeparator*,
                     SoStyle&,
                     const std::vector<SbRepBin2D>&,
			   const SbRepBox&,const SbRepBox&,const SbRepBox&,
                     float,float,const SbString&);
  void repPoints3D_xyz(SoSeparator*,SoStyle&,
                   const std::vector<SbVec3f>&,
                   const SbRepBox&,const SbRepBox&,const SbRepBox&,
                   const SbString&);
  void repTopFaces2D_xyz_line(SoSeparator*,SoStyle&,
                              const std::vector<SbRepTopFace2D>&,
                              const SbRepBox&,const SbRepBox&,const SbRepBox&,
                              const SbString&);
  void repTopFaces2D_xyz(SoSeparator*,SoStyle&,
                     SbPaintingPolicy,const SbBaseColormap&,
                     const std::vector<SbRepTopFace2D>&,
                     const SbRepBox&,const SbRepBox&,const SbRepBox&,
                     const SbString&);
  void repTopFaces2D_xyz_by_level(SoSeparator*,SoStyle&,
                     SbPaintingPolicy,const SbBaseColormap&,
                     const std::vector<SbRepTopFace2D>&,
                     const SbRepBox&,const SbRepBox&,const SbRepBox&,
                     float,float,const SbString&);
  void repTopFaces2D_xyz_one(SoSeparator*,SoStyle&,
                     const std::vector<SbRepTopFace2D>&,
                     const SbRepBox&,const SbRepBox&,const SbRepBox&,
                     const SbString&);

public:
  static void project2D(std::vector<SbVec3f>&,const SbRepBox&,const SbRepBox&);
private:
  static void PAW_coloring(std::vector<float>&);
  static SbVec3f* clipPoints2D(const std::vector<SbVec3f>&,
                     const SbRepBox&,const SbRepBox&,float,int&);
  static SbVec3f* clipPoints3D(const std::vector<SbVec3f>&,
                     const SbRepBox&,const SbRepBox&,const SbRepBox&,int&);
  static SbVec3f* clipPolyLine2D(const std::vector<SbVec3f>&,
                                 const SbRepBox&,const SbRepBox&,int&);
private:
  SoStyle* fDefault__wallStyle;
  SoStyle* fDefault__gridStyle;
  SoStyle* fDefault__innerFrameStyle;
  SoStyle* fDefault__infosStyle;
  SoTextStyle* fDefault__titleStyle;

  SoStyleCache* fStyleCache;

  SbPList fPlottableObjects;
  std::vector<SbPlottablePrimitive*> f_primitives;

  //optimization:
  std::vector<SbPlottableObject*> f_binsList;
  std::vector<SbPlottableObject*> f_pointsList;
  std::vector<SbPlottableObject*> f_functionList;

  std::vector<SbPlottableBins1D*> f_bins1DList;
  std::vector<SbPlottableBins2D*> f_bins2DList;
  std::vector<SbPlottablePoints2D*> f_points2DList;
  std::vector<SbPlottablePoints3D*> f_points3DList;
  std::vector<SbPlottableFunction1D*> f_function1DList;
  std::vector<SbPlottableFunction2D*> f_function2DList;

  int fShape;

  SbAxis f_xDataAxis;
  SbAxis f_yDataAxis;
  SbAxis f_zDataAxis;

  std::vector<SbBaseColormap*> f_bins_cmaps;
  std::vector<SbBaseColormap*> f_points_cmaps;
  std::vector<SbBaseColormap*> f_function_cmaps;

  SbBool f_shape_Update;
  SbBool f_bins_Update;
  SbBool f_points_Update;
  SbBool f_function_Update;
  SbBool f_wall_Update;
  SbBool f_grid_Update;
  SbBool f_primitives_Update;
  SbBool f_innerFrame_Update;
  SbBool f_title_Update;
  SbBool f_xAxis_Update;
  SbBool f_yAxis_Update;
  SbBool f_zAxis_Update;
  SbBool f_colorMapping_Update;
  SbBool f_layout_Update;
  SbBool f_ttfScale_Update;  

  // Olds of the SoPlotter itself :
  float fOld_width;
  float fOld_height;
  float fOld_depth;
  float fOld_leftMargin;
  float fOld_rightMargin;
  float fOld_bottomMargin;
  float fOld_topMargin;
  float fOld_downMargin;
  float fOld_upMargin;
  SbBool fOld_titleUp;  
  float fOld_titleToAxis;  
  float fOld_titleHeight;  
  SbBool fOld_titleAutomated;  
  SbBool fOld_superposeBins;
  SbBool fOld_colorMappingVisible;
  int fOld_colorMappingAxisLabeling;
  SbBool fOld_colorMappingAttached;

  SbBool fOld_wallEnforced;
  SbBool fOld_gridEnforced;
  SbBool fOld_primitivesEnforced;
  SbBool fOld_innerFrameEnforced;

  SbBool fOld_xAxisEnforced;
  SbBool fOld_xAxisAutomated;
  float fOld_xAxisMinimum;
  float fOld_xAxisMaximum;
  SbBool fOld_xAxisLogScale;
  SbBool fOld_yAxisEnforced;
  SbBool fOld_yAxisAutomated;
  float fOld_yAxisMinimum;
  float fOld_yAxisMaximum;
  SbBool fOld_yAxisLogScale;
  SbBool fOld_zAxisEnforced;
  SbBool fOld_zAxisAutomated;
  float fOld_zAxisMinimum;
  float fOld_zAxisMaximum;
  SbBool fOld_zAxisLogScale;
  float fOld_valueTopMargin;
  SbBool fOld_topAxisVisible;  
  SbBool fOld_rightAxisVisible;  
  int fOld_numberOfLevels;
  SoMFFloat* fOld_levels;
  SbBool fOld_shapeAutomated;
  int fOld_shape;
  float fOld_ttfScale;  
  float fOld_theta;  
  float fOld_phi;  

  // Olds for the sub nodes :
  std::vector<SbStyle> fOld_binsStyle;
  std::vector<SbStyle> fOld_pointsStyle;
  std::vector<SbStyle> fOld_functionStyle;
  std::vector<SbStyle> fOld_rightHatchStyle;
  std::vector<SbStyle> fOld_leftHatchStyle;
  std::vector<SbStyle> fOld_errorsStyle;

  SbStyle fOld_wallStyle;
  SbStyle fOld_gridStyle;
  SbStyle fOld_innerFrameStyle;
  SbStyle fOld_infosStyle;
  SbStyle fOld_titleStyle;

  SbStyle fOld_xAxis_lineStyle;
  SbStyle fOld_xAxis_ticksStyle;
  SbStyle fOld_xAxis_labelsStyle;
  SbStyle fOld_xAxis_titleStyle;
  SbStyle fOld_xAxis_magStyle;
  SbString fOld_xAxis_modeling;

  SbStyle fOld_yAxis_lineStyle;
  SbStyle fOld_yAxis_ticksStyle;
  SbStyle fOld_yAxis_labelsStyle;
  SbStyle fOld_yAxis_titleStyle;
  SbStyle fOld_yAxis_magStyle;
  SbString fOld_yAxis_modeling;

  SbStyle fOld_zAxis_lineStyle;
  SbStyle fOld_zAxis_ticksStyle;
  SbStyle fOld_zAxis_labelsStyle;
  SbStyle fOld_zAxis_titleStyle;
  SbStyle fOld_zAxis_magStyle;
  SbString fOld_zAxis_modeling;

  //HEPVis::SbMutex* fMutex;
};

#endif
