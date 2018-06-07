/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoJet                                                   */
/* Description:      Draws a Jet object using energy, theta, phi, thrust     */
/* Author:           T. Paul                                                 */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoJet_h
#define HEPVis_SoJet_h

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoShape.h>

class SoSFNode;
class SoJet:public SoShape {

  SO_NODE_HEADER(SoJet);
 
public:

  SoSFFloat fEnergy;                   // Energy of the jet
  SoSFFloat fTheta;                    // Theta of the jet axis
  SoSFFloat fPhi;                      // Phi of the jet axis
  SoSFFloat fThrust;                   // Thrust of the jet
  SoSFFloat fMaxZ;                     // Half length of cylinder outside of which jet does not extend
  SoSFFloat fMaxR;                     // Half length of cylinder outside of which jet does not extend
  SoSFNode  alternateRep;              // 

  SoJet();

  static void initClass();

  virtual void generateAlternateRep(SoAction*);

  virtual void clearAlternateRep();

  const char* getInfo();

protected:

  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center );

  virtual void generatePrimitives(SoAction *action);

  virtual void GLRender(SoGLRenderAction *action); 

  virtual SoChildList *getChildren() const;

  virtual ~SoJet();

private: 


  void generateChildren();  

  void updateChildren();
  

  SoChildList *children;

  // dimensions and angles for various things in the SoJet object
  void setParameters();

  // private data members
  float theta, phi, bodyRadius, bodyHeight,
        hatRadius, hatHeight;
  float jetX, jetY, jetZ;

  float myEnergy, myTheta, myPhi, myThrust;
  //  int myLabelOption;
  float myMaxZ, myMaxR;

  SbString fInfo;
};

#endif





