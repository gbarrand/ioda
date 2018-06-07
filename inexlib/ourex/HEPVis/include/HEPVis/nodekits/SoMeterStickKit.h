#ifndef HEPVis_SoMeterStickKit_h
#define HEPVis_SoMeterStickKit_h

#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/SbLinear.h>
#include <vector>
class SoSFNode;
class SoDragger;
// SoMeterStickKit:  a movable meter stick that can be used to measure
// lengths, distances within a scene: 


class SoMeterStickKit:public SoBaseKit {

  //
  //! This is required
  //
  SO_KIT_HEADER(SoMeterStickKit);
  //
  // These three are built by the standard Kit mechanism; however
  // the jack dragger will be extensively modified after its creation
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(meterStickAppearance);
  SO_KIT_CATALOG_ENTRY_HEADER(rulingAppearance);
  SO_KIT_CATALOG_ENTRY_HEADER(jackManip);
 
public:

  // Type:  Enum type starts with smallest measurement device
  // and runs through longtest
  enum Type {RULER=0,
            METER_STICK};

  //
  // A Field representing meter stick types
  //
  SoSFInt32 type;
  SoSFBool  rotationSnap;

  //
  //! the alternate representation, required
  //
  SoSFNode  alternateRep;

  //
  //! Constructor, required
  //
  SoMeterStickKit();

  //
  //! Class Initializer, required
  //
  static void initClass();
  //
  //  This is required:
  //
  virtual SbBool affectsState() const;
  //
  //! Generate AlternateRep, required.  Generating an alternate representation
  //! must be done upon users request.  It allows an Inventor program to read
  //! back the file without requiring *this* code to be dynamically linked. 
  //! If the users expects that *this* code will be dynamically linked, he
  //! need not invoke this method.  
  //
  virtual void generateAlternateRep(SoAction*);

  //
  //! We better be able to clear it, too!
  //
  virtual void clearAlternateRep();

  // 
  //  This callback function is invoked when motion stops:
  //
  static void motionFinished(void *, SoDragger *);

protected:

  //
  //! Destructor, required
  //
  virtual ~SoMeterStickKit();

private: 

  void createInitialMeterStick();
  static const int               _NVALID;
  static std::vector<SbRotation> _validRotations;

};

#endif
