// History :
// - 25/09/98, G.Barrand : creation.

#ifndef HEPVis_SbPainterPS_h
#define HEPVis_SbPainterPS_h 

// Inheritance :
#include <HEPVis/SbPainter.h>

#include <stdarg.h>

#include <Inventor/SbViewportRegion.h>

class SbPainterPS : public SbPainter {
public:
  SbPainterPS();
  ~SbPainterPS();
  void beginTraversal();
  void clearColorBuffer(float,float,float);
  void drawPrimitive(SbPrimitiveType,
                     int,float*,float*,float*,
                     const SbPainterContext&);
  void endTraversal();
public:
  void setColorScheme(int);
  void setOrientation(int);
  void setBackgroundDrawn(int);
  void setBitsPerPixel(int);
  void setLineWidth(int);
  void* getStream();
  void setFileName(const char*);
  const char* getFileName() const;
  void openFileForWriting(const char*);
  void closeStream();
  void putPageScaleInStream(float,float);
  void putSaveStateInStream();
  void putRestoreStateInStream();
  void putTranslationInStream(float,float);
  void putScaleInStream(float,float);
  void putBeginPageInStream();
  void putEndPageInStream();
  void putRGB_InStream(float,float,float);
  void putMarkerSizeInStream(float);
  void putMarkerStyleInStream(SbMarkerStyle);
  void putBackgroundInStream(float,float,float,float,float);
  void putFrameInStream(float,float,float,float,float);
  void putRotateInStream(float);
  void putNewPathInStream();
  void putStrokeInStream();
  void putFillInStream();
  void putClosePathInStream();
  void putCapInStream(int);
  void putLineToInStream(float,float);
  void putMoveInStream(float,float);
  void putCircleInStream(float,float,float);
  void putLineWidthInStream(int);
  void putLinePatternInStream(SbLinePattern);
  typedef int(*GetRGB_Function)(unsigned int,unsigned int,
                                double&,double&,double&);
  void putImageInStream(unsigned int,unsigned int,GetRGB_Function);
private:
  void drawPolygon(int,float*,float*,float,float,float,const SbAreaStyle&);
  void drawLines(int,float*,float*,float,float,float,SbLinePattern,int);
  void drawMarkers(int,float*,float*,
                   float,float,float,const SbMarkerStyle&,int);
  enum ColorScheme {
    Color = 0,
    Grey = 1,
    BlackWhite = 2
  };
  struct {
    int shade;
    int portrait;
    int nbit;
    int doBack;
    float lineWidth;
  } fParams;
  float fDeviceWidth;
  float fDeviceHeight;
  int fPageNumber;
  long fPagePos;
  float fMarkerSize;
  FILE* fFile;
  char* fFileName;
  int fGSave;
  int fBufferCount;
  char* fBufferString;
#define METAFILE_RECORD_LENGTH  80
  unsigned char fBufferPointer[METAFILE_RECORD_LENGTH+1];
private:
  void putInStreamF(const char*,...);
  void printFLN(const char*,...);
  void printV(const char*,va_list);
  float convertRGB_ToGrey(float,float,float);
  void writeByte(unsigned char);
};

#endif





