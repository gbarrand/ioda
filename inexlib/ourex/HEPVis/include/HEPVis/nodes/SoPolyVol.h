#ifndef HEPVis_SoPolyVol_h
#define HEPVis_SoPolyVol_h

#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>
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

class SoPolyVol : public SoShape {
        SO_NODE_HEADER(SoPolyVol);

public:

// Fields
        SoMFVec3f vtxPts;
        SoSFVec3f center;
        SoSFFloat dZ;
        SoSFNode alternateRep;

public:
#if defined(_WIN32) && defined(BUILDING_DLL)
   // When building the node as a DLL under Win32 we must explicitly
   // declare this entry point as visible outside the DLL.  The macro
   // BUILDING_DLL is defined in the node's source file.
      _declspec(dllexport)
#endif
        SoPolyVol();
        virtual void setPolyVolPts(const int nSides, const float dZhalf, 
                const float *angles, const float *sideHalf);
        virtual void generateAlternateRep(SoAction*);
        virtual void clearAlternateRep();
        static void initClass(); 
protected:
        virtual SoChildList *getChildren() const;
        virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &TheCenter );
        virtual void generatePrimitives(SoAction *action);
        SoMFVec3f vtxPtsLocal;
        SbVec3f centerLocal;
        float dZLocal;
        virtual ~SoPolyVol();
private:
        void updateChildren();
        void generateChildren();
        SoChildList *children;
};
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif

#endif
