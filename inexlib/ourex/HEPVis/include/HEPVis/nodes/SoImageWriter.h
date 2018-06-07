////////////////////////////////////////////////////////////
//
//  This node is able to produce PostScript, GIF files. 
//
////////////////////////////////////////////////////////////


#ifndef HEPVis_SoImageWriter_h
#define HEPVis_SoImageWriter_h 

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>

class SoImageWriter : public SoNode {
  SO_NODE_HEADER(SoImageWriter);
public:
  enum Format {
    POST_SCRIPT,
    GIF,
    JPEG
  };
  SoSFEnum format;
  SoSFString fileName;
  SoSFInt32 quality;
  SoSFNode  alternateRep;
public:
  SoImageWriter();
  void enable();
  void disable();
  SbBool getStatus() const;
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
protected:
  virtual  ~SoImageWriter();
public: /*SoEXTENDER*/
  virtual void GLRender(SoGLRenderAction*);
  virtual void doAction(SoAction*);
  virtual void write(SoWriteAction*);
public: /*SoINTERNAL*/
  static void initClass();
private:
  SbBool fEnabled;
  SbBool fStatus;
};

#endif

