/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoCrystalHit                                            */
/* Description:      Draws a hit in an individual crystal                    */
/* Author:           J. Boudreau based on SoCrystalHit by L. Taylor          */
/*---------------------------------------------------------------------------*/
#ifndef __SoHadEmCrystalHit_hh_
#define __SoHadEmCrystalHit_hh_
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/SbBasic.h>

class SoSFNode;
class SoHadEmCrystalHit:public SoShape {

  SO_NODE_HEADER(SoHadEmCrystalHit);
 
public:

  SoSFFloat EnergyHad;
  SoSFFloat EnergyEm;    
  SoSFFloat MinimumEnergyTot;         
  SoSFFloat Scale;            
  SoSFFloat RelativeWidth;  

  SoSFVec3f Front1;            
  SoSFVec3f Front2;            
  SoSFVec3f Front3;            
  SoSFVec3f Front4;            
                        
                        
  SoSFNode  alternateRep;              

  SoHadEmCrystalHit();

  static void initClass();

  virtual void generateAlternateRep(SoAction*);

  virtual void clearAlternateRep();

protected:

  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center );

  virtual void generatePrimitives(SoAction *action);

  virtual void GLRender(SoGLRenderAction *action); 

  virtual SoChildList *getChildren() const;

  virtual ~SoHadEmCrystalHit();

private: 


  void generateChildren();  

  void updateChildren();
  
  SoChildList *children;

  // private data members
  // --------------------

  float   myEnergyHad;
  float   myEnergyEm;
  float   myMinimumEnergyTot;
  float   myScale;
  float   myRelativeWidth;

  
  SbVec3f myFront1;
  SbVec3f myFront2;
  SbVec3f myFront3;
  SbVec3f myFront4;

  static const int32_t shapeIndicesHad[5*6];
  static const int32_t shapeIndicesEm[5*5];
  static const int32_t normalIndicesHad[6];
  static const int32_t normalIndicesEm[5];

  SbVec3f shapeVertices[16];
  SbVec3f normalVertices[6];

        
                         
};

#endif





