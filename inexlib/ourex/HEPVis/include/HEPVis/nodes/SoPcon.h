#ifndef HEPVis_SoPcon_h
#define HEPVis_SoPcon_h

#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoNurbsSurface.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/SbBox.h>


//adding fields 
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif


// Defining Class
#ifdef _WIN32
#include <SoWinLeaveScope.h>
#endif

class SoPcon : public SoShape {
        SO_NODE_HEADER(SoPcon);

public:

// Fields
        SoMFVec3f vtxPts;
        SoSFVec3f center;
        SoSFFloat startAngle;
        SoSFFloat deltaAngle;
        SoSFNode alternateRep;

public:
#if defined(_WIN32) && defined(BUILDING_DLL)
   // When building the node as a DLL under Win32 we must explicitly
   // declare this entry point as visible outside the DLL.  The macro
   // BUILDING_DLL is defined in the node's source file.
      _declspec(dllexport)
#endif
        SoPcon();
        virtual void setPconPts(const int nzee, const float phi, const float del_phi, 
                 const float *zvals, const float *rmin, const float *rmax);
        virtual void generateAlternateRep(SoAction*);
        virtual void clearAlternateRep();
        static void initClass(); 
protected:
        static const int V_DIM;
        static const int V_ORDER;
        static const int U_ORDER;
        virtual SoChildList *getChildren() const;
        virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &TheCenter );
        virtual void generatePrimitives(SoAction *action);
        int nsurf;
        SbBool ends;
        SoMFVec3f vtxPtsLocal;
        SbVec3f centerLocal;
        float startAngleLocal;
        float deltaAngleLocal;
        virtual ~SoPcon();
private:
        void updateChildren();
        void generateChildren();
        SoChildList *children;
};
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif

#endif
