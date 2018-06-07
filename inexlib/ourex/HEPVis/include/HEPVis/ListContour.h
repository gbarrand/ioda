
// The listContour code is prefixed by Lib_ in order
// to avoid clashes with other listContour code that may come at link
// time from other channels.

#ifndef HEPVis_ListContour_h
#define HEPVis_ListContour_h

// Inheritance :
#include <HEPVis/Contour.h> //G.Barrand

#define CListContour HEPVis_CListContour
#define CLineStrip HEPVis_CLineStrip
#define CLineStripList HEPVis_CLineStripList
#define CLineStripListVector HEPVis_CLineStripListVector

//G.Barrand #include "stdafx.h"
#include <list>
#include <ostream>

// a list of point index referring to the secondary grid
// Let i the index of a point,
typedef std::list<unsigned int> CLineStrip;
typedef std::list<CLineStrip*> CLineStripList;
typedef std::vector<CLineStripList> CLineStripListVector;

class CListContour : public CContour  
{
public:
	CListContour();
	virtual ~CListContour();

	// retreiving list of line strip for the i-th contour
	CLineStripList* GetLines(unsigned int iPlane);

	// Initializing memory
	virtual void InitMemory();
	// Cleaning memory and line strips
	virtual void CleanMemory();
	// Generate contour strips
	virtual void Generate();

	// Adding segment to line strips
	// See CContour::ExportLine for further details
	void ExportLine(int iPlane,int x1, int y1, int x2, int y2);

	// Basic algorithm to concatanate line strip. Not optimized at all !
	bool CompactStrips();
	/// debuggin
	void DumpPlane(unsigned int iPlane) const;

	// Area given by this function can be positive or negative depending on the winding direction of the contour.
	double Area(CLineStrip* Line);

	double EdgeWeight(CLineStrip* pLine, double R);
	bool   PrintContour(std::ostream& a_out);
protected:
	// Merges pStrip1 with pStrip2 if they have a common end point
	bool MergeStrips(CLineStrip* pStrip1, CLineStrip* pStrip2);
	// Merges the two strips with a welding threshold.
	bool ForceMerge(CLineStrip* pStrip1, CLineStrip* pStrip2,double);
	// returns true if contour is touching boundary
	bool OnBoundary(CLineStrip* pStrip);
        // L.Garnier : check specials case for CompactStrip
        bool SpecialCompactStripCase(double,double,double,double,double); 

private:
	// array of line strips
	CLineStripListVector m_vStripLists; 
        typedef unsigned int UINT;
};


#endif
