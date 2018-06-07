/*
 * Created by G.Barrand. January 2008.
 * 
 * Node to have :
 *  - the SoSearchAction working with coin3.d
 *  - support web URL.
 * 
 */

#ifndef HEPVis_SoFile_h
#define HEPVis_SoFile_h

// Inheritance :
#include <Inventor/nodes/SoFile.h>

class HEPVis_SoFile : public SoFile {
  SO_NODE_HEADER(HEPVis_SoFile);
public:
  HEPVis_SoFile();
protected:
  virtual ~HEPVis_SoFile();
public:/*SoINTERNAL*/ 
  static void initClass();
public: /*SoEXTENDER*/ 
#if((COIN_MAJOR_VERSION>=2)&&(COIN_MINOR_VERSION>=5))
#else
  virtual void search(SoSearchAction* action);
#endif
  virtual SbBool readNamedFile(SoInput* in);
private:
  static SbBool curl_file(const SbString&,const SbString&);

  // to avoid a curl dependency for OSC-16.2 :
public:
  typedef SbBool (*CurlFunction)(const SbString&,const SbString&); //FIXME
  static void setCurlFunction(CurlFunction aFunc); //FIXME
private:
  static CurlFunction fCurlFunction; //to be removed.
};

#endif
