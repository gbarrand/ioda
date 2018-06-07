#ifndef COIN_SOJAVASCRIPTENGINE_H
#define COIN_SOJAVASCRIPTENGINE_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2007 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

/*
  -- PLEASE NOTE! --
  As the spidermonkey support is only experimental for Coin-2, one
  will have to manually enable this code. This is done to not break the
  ABI. Add the following line to enable the header;

   #define COIN_HAVE_JAVASCRIPT 
*/

#ifdef COIN_HAVE_JAVASCRIPT
  
#include <Inventor/misc/SoScriptEngine.h>
#include <Inventor/C/glue/spidermonkey.h>
#include <Inventor/lists/SbList.h>

// *************************************************************************

#define COIN_JAVASCRIPT_RUNTIME_MAXBYTES 4194304

// *************************************************************************

typedef void SoJSfield2jsvalFunc(JSContext *, const SoField *, jsval *);
typedef SbBool SoJSjsval2field2Func(JSContext *, const jsval, SoField *);
typedef JSObject * SoJSWrapperInitFunc(JSContext *, JSObject * obj);

// *************************************************************************

class COIN_DLL_API SoJavaScriptEngine : public SoScriptEngine {

public:
  SoJavaScriptEngine();
  virtual ~SoJavaScriptEngine();

  virtual SbBool executeScript(const SbName & name, 
                               const SbString & script) const;
  virtual SbBool executeFile(const SbName & filename) const;
  virtual SbBool executeFunction(const SbName &name, int argc, 
                                 const SoField * argv, 
                                 SoField * rval = NULL) const;

  virtual SbBool setScriptField(const SbName & name, const SoField * f) const;
  virtual SbBool unsetScriptField(const SbName & name) const;
  virtual SbBool getScriptField(const SbName & name, SoField * f) const;
  virtual SbBool hasScriptField(const SbName & name) const;

  // Everything under here is javascript specific

  static SoJavaScriptEngine *getEngine(JSContext * cx);

  SbBool field2jsval(const SoField * f, jsval * v) const;
  SbBool jsval2field(const jsval v, SoField * f) const;
  void enableAutoNodeUnref(SbBool onoff);
  SbBool getAutoNodeUnrefState(void) const;

  static SbBool init(uint32_t maxbytes = COIN_JAVASCRIPT_RUNTIME_MAXBYTES);
  static void shutdown(void);
  static SbBool debug(void);

  static JSRuntime * getRuntime(void);
  JSContext * getContext(void) const;
  JSObject * getGlobal(void) const;

  void addHandler(const SoType & type, SoJSWrapperInitFunc * init, 
                  SoJSfield2jsvalFunc * field2jsval, 
                  SoJSjsval2field2Func * jsval2field);

protected:
  static void setRuntime(JSRuntime * runtime);
  void setContext(JSContext * context);
  void setGlobal(JSObject * global);

private:
  class SoJavaScriptEngineP * pimpl; 
  friend class SoJavaScriptEngineP; 
};

#endif // !COIN_HAVE_JAVASCRIPT

#endif // !COIN_SOJAVASCRIPTENGINE_H
