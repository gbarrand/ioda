/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoCrystalHit                                            */
/* Description:      Draws a hit in an individual crystal                    */
/* Author:           L. Taylor                                               */
/*---------------------------------------------------------------------------*/
#ifndef __SoCrystalHit_hh_
#define __SoCrystalHit_hh_
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/SbBasic.h>

class SoSFNode;
class SoCrystalHit:public SoShape {

  SO_NODE_HEADER(SoCrystalHit);
 
public:

  SoSFFloat Energy;            
  SoSFFloat MinimumEnergy;         
  SoSFFloat Scale;            
  SoSFFloat RelativeWidth;  

  SoSFBool  DrawCrystal;
  SoSFBool  DrawHit;
 
  SoSFVec3f Front1;            
  SoSFVec3f Front2;            
  SoSFVec3f Front3;            
  SoSFVec3f Front4;            
                        
  SoSFVec3f Back1;            
  SoSFVec3f Back2;            
  SoSFVec3f Back3;            
  SoSFVec3f Back4;            
                    
                        
  SoSFNode  alternateRep;              

  SoCrystalHit();

  static void initClass();

  virtual void generateAlternateRep(SoAction*);

  virtual void clearAlternateRep();

  const char* getInfo();

protected:

  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center );

  virtual void generatePrimitives(SoAction *action);

  virtual void GLRender(SoGLRenderAction *action); 

  virtual SoChildList *getChildren() const;

  virtual ~SoCrystalHit();

private: 


  void generateChildren();  

  void updateChildren();
  
  void setParameters();

  SoChildList *children;

  // private data members
  // --------------------

  float   myEnergy;
  float   myMinimumEnergy;
  float   myScale;
  float   myRelativeWidth;

  SbBool  myDrawCrystal;
  SbBool  myDrawHit;
  
  SbVec3f myFront1;
  SbVec3f myFront2;
  SbVec3f myFront3;
  SbVec3f myFront4;
        
  SbVec3f myBack1;
  SbVec3f myBack2;
  SbVec3f myBack3;
  SbVec3f myBack4;

private:                         
  SbString fInfo;
};

#endif





