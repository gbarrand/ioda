#ifndef HEPVis_SoTools_h
#define HEPVis_SoTools_h

/*
 * A helper class with some common usefull functions.
 */

#include <Inventor/SbString.h>
#include <Inventor/draggers/SoDragger.h> //for SoDraggerCB typedef

#include <HEPVis/SbStyles.h>
#include <HEPVis/SbColors.h>

class SbVec3f;
class SoPathList;
class SoNode;
class SoType;
class SoSeparator;
class SoBaseKit;
class SoFullPath;
class SoCamera;
class SoTransform;
class SoCoordinate3;
class SoTranslate2Dragger;

class SbVisitor {
public:
  virtual ~SbVisitor(){}
public:
  virtual SbBool visit(SoFullPath&) = 0;
};

class SoTools {
public:
  enum HorizontalJustification {
    LEFT   = 0x00,
    CENTER = 0x01,
    RIGHT  = 0x02
  };
  enum VerticalJustification {
    BOTTOM = 0x00,
    MIDDLE = 0x01,
    TOP    = 0x02
  };

  static SoPathList* getSceneGraphs(SoNode*);

  static SoNode* searchNode(SoNode*,SoType);
  static SoNode* searchNode(SoNode*,const SbString&);
  static SoNode* searchNode(SoNode*,const SbString&,SoType);
  static SbBool isChildOf(SoNode*,SoNode*);

  static void addPolygonToNode(SoSeparator*,int,SbVec3f*);
  static void addPolygonToNode(SoSeparator*,int,SbVec3f*,const SbVec3f&);
  static void addBoxCoords(SoSeparator&,float,float,float);

  static void addLinesToNode(SoSeparator*,
                             int,SbVec3f*);
  static void addSegmentsToNode(SoSeparator*,
                                int,SbVec3f*);
  static void addNurbsCurveToNode(SoSeparator*,
                                   int,SbVec3f*);
  static void addSplineCurveToNode(SoSeparator*,
                                    int,SbVec3f*);
  static void addLagrangePolynomialCurveToNode(SoSeparator*,
                                               int,SbVec3f*);
  static void addPointsToNode(SoSeparator*,
                              int,SbVec3f*,
                              SbMarkerStyle,int);
  static void addAlignedStringToNode(SoSeparator*,
                                     const SbString&,
                                     const SbString&,
                                     SbBool,
                                     const SbString&,
                                     const SbVec3f&,
                                     const SbVec3f&,
                                     const SbVec3f&,
                                     float,
                                     HorizontalJustification,
                                     VerticalJustification,
                                     SbBool = FALSE);

  //optimization
  static SbBool setKitPart(SoBaseKit*,const char*,const char*);
  static SbBool getKitPart(SoBaseKit*,const char*,SbString&);
  static SbBool toggleKitPart(SoBaseKit*,const char*);

  //deprecated, use the uppers.
  static SbBool setKitPartValue(SoBaseKit*,const SbString&,const SbString&);
  static SbBool getKitPartValue(SoBaseKit*,const SbString&,SbString&);

  static SbBool toggleKitPartValue(SoBaseKit*,const SbString&);
  static void resetHighlight(SoNode*);
  static void highlight(SoFullPath&);
public:
  static const SbName& highlightName();
  //static void dumpPath(SoFullPath&);

  static void pointAt(SoCamera&,const SbVec3f&,const SbVec3f&);
  static void lookAt(SoCamera&,const SbVec3f&,const SbVec3f&);

  static SbBool write(SoNode& scene_graph,
                      const SbString& file,
                      const SbString& format,
                      SbBool binary,
                      SbBool generateAltRep);

  static SbBool write(SoNode& scene_graph,
                      const SbString& file,
                      const SbString& format,
                      SbBool generateAltRep,
                      SbBool binary,
                      const SbString& compresser,
                      float compression_level);

  static SbBool setManip(SoNode*,SoTransform*);
  static void removeManips(SoNode&);
  static void setManips(SoNode&);

  static SbBool setDraggerColor(SoTranslate2Dragger&,const SbColor&);

  static void addDraggersToPolygon(SoSeparator&,SoCoordinate3&,
                                   const SbColor& color = SbColor_red,
                                   float scale = 0.05,
                                   SoDraggerCB* = 0,void* = 0);

  static SbBool visit(SoNode&,SoType,SbVisitor&);

  static SbBool detectIntersection(SoNode&,SbBool stopAtFirst);

  static void count(SoNode& aNode,unsigned int& aTriangles,
                           unsigned int& aLineSegments,
                           unsigned int& aPoints,
                           unsigned int& aNodes,
                           unsigned int& aShapes);

};

#endif
