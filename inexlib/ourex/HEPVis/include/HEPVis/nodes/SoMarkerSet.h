#ifndef HEPVis_SoMarkerSet_h
#define HEPVis_SoMarkerSet_h 

// Inheritance :
#include <Inventor/nodes/SoPointSet.h>

#include <Inventor/fields/SoMFInt32.h>
#include <Inventor/fields/SoSFNode.h>

#include <HEPVis/SbStyles.h>

class HEPVis_SoMarkerSet : public SoPointSet {
  SO_NODE_HEADER(HEPVis_SoMarkerSet);
public:
  enum MarkerType {
    //************ 5x5 markers
    PLUS_5_5 = 500,
    ASTERISK_5_5,
    CROSS_5_5,
    STAR_5_5,
    CIRCLE_LINE_5_5,
    CIRCLE_FILLED_5_5,
    TRIANGLE_UP_LINE_5_5,
    TRIANGLE_UP_FILLED_5_5,
    TRIANGLE_DOWN_LINE_5_5,
    TRIANGLE_DOWN_FILLED_5_5,
    DAVID_STAR_LINE_5_5,
    DAVID_STAR_FILLED_5_5,
    SWISS_CROSS_LINE_5_5,
    SWISS_CROSS_FILLED_5_5,
    DIAMOND_LINE_5_5,
    DIAMOND_FILLED_5_5,
    SQUARE_LINE_5_5,
    SQUARE_FILLED_5_5,
    PENTA_STAR_LINE_5_5,
    PENTA_STAR_FILLED_5_5,

    //************ 7x7 markers
    PLUS_7_7 = 700,
    ASTERISK_7_7,
    CROSS_7_7,
    STAR_7_7,
    CIRCLE_LINE_7_7,
    CIRCLE_FILLED_7_7,
    TRIANGLE_UP_LINE_7_7,
    TRIANGLE_UP_FILLED_7_7,
    TRIANGLE_DOWN_LINE_7_7,
    TRIANGLE_DOWN_FILLED_7_7,
    DAVID_STAR_LINE_7_7,
    DAVID_STAR_FILLED_7_7,
    SWISS_CROSS_LINE_7_7,
    SWISS_CROSS_FILLED_7_7,
    DIAMOND_LINE_7_7,
    DIAMOND_FILLED_7_7,
    SQUARE_LINE_7_7,
    SQUARE_FILLED_7_7,
    PENTA_STAR_LINE_7_7,
    PENTA_STAR_FILLED_7_7,

    //************ 9x9 markers
    PLUS_9_9 = 900,
    ASTERISK_9_9,
    CROSS_9_9,
    STAR_9_9,
    CIRCLE_LINE_9_9,
    CIRCLE_FILLED_9_9,
    TRIANGLE_UP_LINE_9_9,
    TRIANGLE_UP_FILLED_9_9,
    TRIANGLE_DOWN_LINE_9_9,
    TRIANGLE_DOWN_FILLED_9_9,
    DAVID_STAR_LINE_9_9,
    DAVID_STAR_FILLED_9_9,
    SWISS_CROSS_LINE_9_9,
    SWISS_CROSS_FILLED_9_9,
    DIAMOND_LINE_9_9,
    DIAMOND_FILLED_9_9,
    SQUARE_LINE_9_9,
    SQUARE_FILLED_9_9,
    PENTA_STAR_LINE_9_9,
    PENTA_STAR_FILLED_9_9
  } ;

  SoMFInt32 markerIndex;
  SoSFNode alternateRep;
public:
  HEPVis_SoMarkerSet();
public: /*SoEXTENDER*/
  virtual void GLRender(SoGLRenderAction*);
  virtual void doAction(SoAction*);
  virtual void write(SoWriteAction*);
public: /*SoINTERNAL*/
  static void initClass();
  static int getMarkerIndex(SbMarkerStyle,int);
  static void getMarkerStyleSize(int,SbMarkerStyle&,int&);
protected:
  virtual ~HEPVis_SoMarkerSet();
private:
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
};

#endif
