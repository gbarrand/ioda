#ifndef HEPVis_SoTestKit_h
#define HEPVis_SoTestKit_h

/*
 *  SoTestKit.h
 *  
 *
 *  Created by Guy Barrand to test / debug VRML2 convertion of nodekits.
 *    12 Oct 2006.
 *
 */

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFNode.h>

class SoTestKit : public SoBaseKit {
  SO_KIT_HEADER(SoTestKit);
  
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
public :
  SoSFNode alternateRep;
public:
  static void initClass();
public:
  SoTestKit();
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
protected:
  virtual ~SoTestKit();
private :
  void initialize();
};

#endif
