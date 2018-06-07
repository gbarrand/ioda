#ifndef HEPVis_SoPrimitive_h
#define HEPVis_SoPrimitive_h

#include <Inventor/SbBox.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFBool.h>

class SoFieldSensor;
class SoSensor;

typedef void SoCB(void*,SoNode*);

/**
 * SoPrimitive is an Inventor implementation of the Motif XmPrimitive widget.
 * It handles, through an SoBackPrimitive, the 'background' of other nodekits 
 * like SoPushButton, SoList,... Main Motif XmPrimitive resources are 
 * transcripted in Inventor fields.
 */

class SoPrimitive : public SoBaseKit {
  SO_KIT_HEADER(SoPrimitive);
  SO_KIT_CATALOG_ENTRY_HEADER(backSeparator);
   SO_KIT_CATALOG_ENTRY_HEADER(backTransform);
   SO_KIT_CATALOG_ENTRY_HEADER(backMaterial);
   SO_KIT_CATALOG_ENTRY_HEADER(backTrd);
  SO_KIT_CATALOG_ENTRY_HEADER(frontSeparator);
   SO_KIT_CATALOG_ENTRY_HEADER(frontTransform);
   SO_KIT_CATALOG_ENTRY_HEADER(frontMaterial);
   SO_KIT_CATALOG_ENTRY_HEADER(frontGroup);
public:
  SoSFColor background;
  SoSFColor foreground;
  SoSFFloat shadowThickness;
  SoSFFloat marginWidth;
  SoSFFloat marginHeight;
  SoSFFloat width;
  SoSFFloat height;
  // Attachment :
  enum Attachment {
    ATTACH_NONE,
    ATTACH_NODE,
    ATTACH_OPPOSITE_NODE
  };
  SoSFEnum leftAttachment;
  SoSFEnum rightAttachment;
  SoSFEnum topAttachment;
  SoSFEnum bottomAttachment;
  SoSFNode leftNode;
  SoSFNode rightNode;
  SoSFNode topNode;
  SoSFNode bottomNode;
public:
  SoPrimitive();
  void compellSize(float,float);
public:
  static void initClass();
protected:
  virtual ~SoPrimitive();
  SoSFBool requireUpdateSceneGraph;
  virtual void updateSceneGraph(); 
  virtual SbBox3f getFrontBBox(); 
  void setBackground(const SbColor&);
  void setForeground(const SbColor&);
private:
  void initialize();
  void updateBack(SbBool); 
  float fObjectWidth;
  float fObjectHeight;
  SoFieldSensor* fUpdateSceneGraphSensor;
  static void sensorCB(void*,SoSensor*);
};


#endif

