#ifndef HEPVis_SoPlottableStyles_h
#define HEPVis_SoPlottableStyles_h

// Inheritance :
#include <HEPVis/nodes/SoStyle.h>

class SoBinsStyle : public SoStyle {
  SO_NODE_HEADER(SoBinsStyle);
public:
  SoBinsStyle();
public:
  static void initClass();
protected:
  virtual ~SoBinsStyle();
};

class SoPointsStyle : public SoStyle {
  SO_NODE_HEADER(SoPointsStyle);
public:
  SoPointsStyle();
public:
  static void initClass();
protected:
  virtual ~SoPointsStyle();
};

class SoFunctionStyle : public SoStyle {
  SO_NODE_HEADER(SoFunctionStyle);
public:
  SoFunctionStyle();
public:
  static void initClass();
protected:
  virtual ~SoFunctionStyle();
};

class SoRightHatchStyle : public SoStyle {
  SO_NODE_HEADER(SoRightHatchStyle);
public:
  SoRightHatchStyle();
public:
  static void initClass();
protected:
  virtual ~SoRightHatchStyle();
};

class SoLeftHatchStyle : public SoStyle {
  SO_NODE_HEADER(SoLeftHatchStyle);
public:
  SoLeftHatchStyle();
public:
  static void initClass();
protected:
  virtual ~SoLeftHatchStyle();
};

class SoErrorsStyle : public SoStyle {
  SO_NODE_HEADER(SoErrorsStyle);
public:
  SoErrorsStyle();
public:
  static void initClass();
protected:
  virtual ~SoErrorsStyle();
};

#endif
