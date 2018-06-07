#ifndef HEPVis_SbTessContour_h
#define HEPVis_SbTessContour_h

#include <HEPVis/SbGL.h>

// About glu, we must be consistent with the CoinGL build.
// In ourex/CoinGL public includes there is no more glu.
// But glu is used internally to build nurbs related nodes.

#ifdef OUREX_COINGL_INLIB_GL
#include <inlib/gldum/coin_gl.h>
#include <inlib/gldum/glu.h>
#else
#include <inlib/glutess/glutess>
#endif

#include <Inventor/SbVec3f.h>
#include <cstdio> //fprintf
#include <vector>

typedef struct {
  double pointA[3];
  double pointB[3];
  double pointC[3];
} SbTessTriangle;

class SbTessContour {  
public:
  SbTessContour()
  :fVertexNumber(0),fBeginType(GL_TRIANGLES),fError(false){}
  virtual ~SbTessContour(){}
protected:
  SbTessContour(const SbTessContour&){}
  SbTessContour& operator=(const SbTessContour&){return *this;}
public:
  std::vector<SbTessTriangle> getFilledArea(const std::vector<std::vector<SbVec3f> > aContour) {
    fTriangles.clear();
    fCombineTmps.clear();
    fError = false;

    GLUtesselator* tobj = gluNewTess();

    ::gluTessCallback(tobj,(GLenum)GLU_TESS_VERTEX_DATA, (Func)vertexCallback);
    ::gluTessCallback(tobj,(GLenum)GLU_TESS_BEGIN_DATA,  (Func)beginCallback);
    ::gluTessCallback(tobj,(GLenum)GLU_TESS_END_DATA,    (Func)endCallback);
    ::gluTessCallback(tobj,(GLenum)GLU_TESS_ERROR_DATA,  (Func)errorCallback);
    ::gluTessCallback(tobj,(GLenum)GLU_TESS_COMBINE_DATA,(Func)combineCallback);  
    ::gluTessProperty(tobj,(GLenum)GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_ODD);

    for(unsigned int a=0;a<aContour.size();a++) {
      //if(aContour[a][0]!=aContour[a][aContour[a].size()-1]) continue;
      int vecSize = aContour[a].size()-1;
      if(vecSize<=0) continue; //should not happen.
  
      typedef GLdouble point[3];
      point* tab = new point[vecSize];
  
      ::gluTessBeginPolygon(tobj, this);
  
      ::gluTessBeginContour(tobj);
      for(unsigned int b=0;b<aContour[a].size()-1;b++) {
        tab[b][0] = aContour[a][b][0];
        tab[b][1] = aContour[a][b][1];
        tab[b][2] = aContour[a][b][2];
        ::gluTessVertex(tobj, tab[b],tab[b]);
      }
      ::gluTessEndContour(tobj);
  
      ::gluTessEndPolygon(tobj);
  
      delete [] tab;  
    }
  
    ::gluDeleteTess(tobj);
  
    for(unsigned int index=0;index<fCombineTmps.size();index++) {
      delete [] fCombineTmps[index];
    }
    fCombineTmps.clear();

    if(fError) fTriangles.clear();
  
    return fTriangles;
  }

private:  
  void resetVertex() {fVertexNumber = 0;}
  void setBeginType(int aType) {fBeginType = aType;}
  void setError(bool aError) {fError = aError;}
  std::vector<double*>& combineTmps(){return fCombineTmps;}

  void addVertex(const double* vertex) {
    // GL_TRIANGLE_STRIP
    // Draws a connected group of triangles. One triangle is defined for each
    // vertex presented after the first two vertices. For odd n, vertices n,
    // n+1, and n+2 define triangle n. For even n, vertices n+1, n, and n+2
    // define triangle n. N-2 triangles are drawn.
    if (fBeginType == GL_TRIANGLE_STRIP) {
      fTmp.pointC[0] = vertex[0];
      fTmp.pointC[1] = vertex[1];
      fTmp.pointC[2] = vertex[2];
  
      if(fVertexNumber>=2) fTriangles.push_back(fTmp);
  
      int rest = fVertexNumber % 2;
      if(rest==1) {
        fTmp.pointA[0] = vertex[0];
        fTmp.pointA[1] = vertex[1];
        fTmp.pointA[2] = vertex[2];
      } else {
        fTmp.pointB[0] = vertex[0];
        fTmp.pointB[1] = vertex[1];
        fTmp.pointB[2] = vertex[2];
      }
      fVertexNumber++;
    }
  
    // GL_TRIANGLE_FAN
    // Draws a connected group of triangles. One triangle is defined for each
    // vertex presented after the first two vertices. Vertices 1, n+1,
    // and n+2 define triangle n. N-2 triangles are drawn.
    else if (fBeginType == GL_TRIANGLE_FAN) {
      if (fVertexNumber == 0) {
        fTmp.pointA[0] = vertex[0];
        fTmp.pointA[1] = vertex[1];
        fTmp.pointA[2] = vertex[2];
      } else {
        fTmp.pointC[0] = vertex[0];
        fTmp.pointC[1] = vertex[1];
        fTmp.pointC[2] = vertex[2];
  
        if (fVertexNumber >=2 ) {
          fTriangles.push_back(fTmp);
        }
        fTmp.pointB[0] = vertex[0];
        fTmp.pointB[1] = vertex[1];
        fTmp.pointB[2] = vertex[2];
      }
      fVertexNumber++;
    }
  
    // GL_TRIANGLES
    // Treats each triplet of vertices as an independent triangle.
    // Vertices 3n-2, 3n-1, and 3n define triangle n. N/3 triangles are drawn.
    else if (fBeginType == GL_TRIANGLES) {
  
      int rest = fVertexNumber % 3;
  
      if(rest==2) {
        fTmp.pointC[0] = vertex[0];
        fTmp.pointC[1] = vertex[1];
        fTmp.pointC[2] = vertex[2];
  
        fTriangles.push_back(fTmp);
  
      } else if(rest==1) {
        fTmp.pointB[0] = vertex[0];
        fTmp.pointB[1] = vertex[1];
        fTmp.pointB[2] = vertex[2];
  
      } else if(rest==0) {
        fTmp.pointA[0] = vertex[0];
        fTmp.pointA[1] = vertex[1];
        fTmp.pointA[2] = vertex[2];
      }
      fVertexNumber++;
  
    } else {
      // do nothing and should never happend
    }
  }
private:
#ifdef HEPVIS_USE_STDCALL
  typedef GLvoid(__stdcall *Func)();
#else
  typedef GLvoid(*Func)();
#endif

  static void 
#ifdef HEPVIS_USE_STDCALL
  __stdcall  
#endif
  beginCallback(GLenum aWhich,GLvoid * aThis) {
    SbTessContour* This = (SbTessContour*)aThis;
    This->setBeginType(aWhich);
    This->resetVertex();
  }

  static void 
#ifdef HEPVIS_USE_STDCALL
  __stdcall  
#endif
  errorCallback(GLenum aErrorCode,GLvoid * aThis) {
#ifdef HEPVIS_USE_NATIVE_GLUTESS
    const GLubyte* estring = gluErrorString(aErrorCode);
    ::fprintf(stderr, "Tessellation Error: %s\n", estring);
    SbTessContour* This = (SbTessContour*)aThis;
    This->setError(true);
#endif
  }

  static void 
#ifdef HEPVIS_USE_STDCALL
  __stdcall  
#endif
  endCallback(){}

  static void 
#ifdef HEPVIS_USE_STDCALL
  __stdcall  
#endif
  vertexCallback(GLvoid *vertex,GLvoid* aThis) {
    SbTessContour* This = (SbTessContour*)aThis;
    This->addVertex((double*)vertex);
  }

  static void 
#ifdef HEPVIS_USE_STDCALL
  __stdcall  
#endif
  combineCallback(GLdouble coords[3],
                              void* vertex_data[4],
                              GLfloat weight[4],
                              void** dataOut,
                              void* aThis) {
    SbTessContour* This = (SbTessContour*)aThis;
    double* vertex = new double[3];
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    This->combineTmps().push_back(vertex);
    *dataOut = vertex;
  }
  
private:
  SbTessTriangle fTmp;
  unsigned int fVertexNumber;
  int fBeginType;
  bool fError;
  std::vector<SbTessTriangle> fTriangles;
  std::vector<double*> fCombineTmps;
};

#endif
