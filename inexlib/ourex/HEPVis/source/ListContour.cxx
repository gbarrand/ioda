//
//  Code get on the web at : 
//    http://www.codeproject.com/cpp/contour.asp
//  
//  G.Barrand 
//

// ListContour.cpp: implementation of the CListContour class.
//
//////////////////////////////////////////////////////////////////////

// this :
#include <HEPVis/ListContour.h> //G.Barrand

//G.Barrand 
#include <cstdio>
#include <cmath>
#include <iomanip>

#include <cstdlib> //exit

#define _TT(what) what
//#define TRACE printf
static void TRACE(const char*,...){}

#define PROBLEM(what) ::printf(what)

#define ASSERT(what,cmt) if(!(what)) {::printf("debug : ListContour : assert failure in %s\n",cmt);::exit(0);} //G.Barrand
#define ASSERT_RET(what,cmt) if(!(what)) {::printf("debug : ListContour : assert failure in %s\n",cmt);return false;} //G.Barrand

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListContour::CListContour()
: CContour()
{
}

CListContour::~CListContour()
{
	CleanMemory();
}

void CListContour::Generate()
{
	// generate line strips
	CContour::Generate();
	// compact strips
	CompactStrips();
}

void CListContour::InitMemory()
{
	CContour::InitMemory();
	
	CLineStripList::iterator pos;
	CLineStrip* pStrip;
	
	if (!m_vStripLists.empty())
	{
		UINT i;
		// reseting lists
		ASSERT(m_vStripLists.size() == GetNPlanes(),"CListContour::InitMemory::0");
		for (i=0;i<GetNPlanes();i++)
		{
			for (pos = m_vStripLists[i].begin(); pos!=m_vStripLists[i].end() ; pos++)
			{
				pStrip=(*pos);
				ASSERT(pStrip,"CListContour::InitMemory::1");

				pStrip->clear();
				delete pStrip;
			}
			m_vStripLists[i].clear();
		}
	}
	else
		m_vStripLists.resize(GetNPlanes());
}

void CListContour::CleanMemory()
{
	CContour::CleanMemory();
	
	CLineStripList::iterator pos;
	CLineStrip* pStrip;
	UINT i;

	// reseting lists
	for (i=0;i<m_vStripLists.size();i++) //G.Barrand
	{
		for (pos=m_vStripLists[i].begin(); pos!=m_vStripLists[i].end();pos++)
		{
			pStrip=(*pos);
			ASSERT(pStrip,"CListContour::CleanMemory");
			pStrip->clear();
			delete pStrip;
		}
		m_vStripLists[i].clear();
	}
}

void CListContour::ExportLine(int iPlane,int x1, int y1, int x2, int y2)
{
	ASSERT(iPlane>=0,"CListContour::ExportLine::0");
	ASSERT(iPlane<(int)GetNPlanes(),"CListContour::ExportLine::1");
	
	// check that the two points are not at the beginning or end of the  some line strip
	UINT i1=y1*(m_iColSec+1)+x1;
	UINT i2=y2*(m_iColSec+1)+x2;
	
	CLineStrip* pStrip;
	
	CLineStripList::iterator pos;
	bool added = false;
	for(pos=m_vStripLists[iPlane].begin(); pos!=m_vStripLists[iPlane].end() && !added; pos++)
	{
		pStrip=(*pos);
		ASSERT(pStrip,"CListContour::ExportLine::2");
		// check if points are appendable to this strip
		if (i1==pStrip->front())
		{
			pStrip->insert(pStrip->begin(),i2);
			return;
		}
		if (i1==pStrip->back())
		{
			pStrip->insert(pStrip->end(),i2);
			return;
		}
		if (i2==pStrip->front())
		{
			pStrip->insert(pStrip->begin(),i1);
			return;
		}
		if (i2==pStrip->back())
		{
			pStrip->insert(pStrip->end(),i1);
			return;
		}
	}
	
	// segment was not part of any line strip, creating new one
	pStrip=new CLineStrip;
	pStrip->insert(pStrip->begin(),i1);
	pStrip->insert(pStrip->end(),i2);
	m_vStripLists[iPlane].insert(m_vStripLists[iPlane].begin(),pStrip);
}

bool CListContour::ForceMerge(CLineStrip* pStrip1, CLineStrip* pStrip2,double aHeight) 
{

	CLineStrip::iterator pos;
	CLineStrip::reverse_iterator rpos;
	
	if (pStrip2->empty())
		return false;
	
	double x[4], y[4], weldDist;
	int index;
	index = pStrip1->front();
	x[0] = GetXi(index);
	y[0] = GetYi(index);
	index = pStrip1->back();
	x[1] = GetXi(index);
	y[1] = GetYi(index);
	index = pStrip2->front();
	x[2] = GetXi(index);
	y[2] = GetYi(index);
	index = pStrip2->back();
	x[3] = GetXi(index);
	y[3] = GetYi(index);
	
	weldDist = 10*(m_dDx*m_dDx+m_dDy*m_dDy);
	
	if (((x[1]-x[2])*(x[1]-x[2])+(y[1]-y[2])*(y[1]-y[2])< weldDist)
            || SpecialCompactStripCase(x[1],x[2],y[1],y[2],aHeight)) // L.Garnier
          
    {
		for (pos=pStrip2->begin(); pos!=pStrip2->end();pos++)
		{
			index=(*pos);
			ASSERT_RET(index>=0,"CListContour::ForceMerge::0");
			pStrip1->insert(pStrip1->end(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if (((x[3]-x[0])*(x[3]-x[0])+(y[3]-y[0])*(y[3]-y[0])< weldDist)
            || SpecialCompactStripCase(x[3],x[0],y[3],y[0],aHeight)) // L.Garnier
    {
		for (rpos=pStrip2->rbegin(); rpos!=pStrip2->rend();rpos++)
		{
			index=(*rpos);
			ASSERT_RET(index>=0,"CListContour::ForceMerge::1");
			pStrip1->insert(pStrip1->begin(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if (((x[1]-x[3])*(x[1]-x[3])+(y[1]-y[3])*(y[1]-y[3])< weldDist)
            || SpecialCompactStripCase(x[1],x[3],y[1],y[3],aHeight)) // L.Garnier
    {
		for (rpos=pStrip2->rbegin(); rpos!=pStrip2->rend();rpos++)
		{
			index=(*rpos);
			ASSERT_RET(index>=0,"CListContour::ForceMerge::2");
			pStrip1->insert(pStrip1->end(),index);
		}
		pStrip2->clear();
		return true;
    }

	if (((x[0]-x[2])*(x[0]-x[2])+(y[0]-y[2])*(y[0]-y[2])< weldDist)
            || SpecialCompactStripCase(x[0],x[2],y[0],y[2],aHeight)) // L.Garnier
    {
		for (pos=pStrip2->begin(); pos!=pStrip2->end();pos++)
		{
			index=(*pos);
			ASSERT_RET(index>=0,"CListContour::ForceMerge::3");
			pStrip1->insert(pStrip1->begin(),index);
		}
		pStrip2->clear();
		return true;
    }

	return false;
}

#define ASSERT_MERGE_RET(what,cmt) if(!(what)) {::printf("debug : ListContour : assert failure in %s\n",cmt);pStrip2->clear();return false;} //G.Barrand

bool CListContour::MergeStrips(CLineStrip* pStrip1, CLineStrip* pStrip2)
{
	CLineStrip::iterator pos;
	CLineStrip::reverse_iterator rpos;
	if (pStrip2->empty())
		return false;
	
	int index;

	// debugging stuff
	if (pStrip2->front()==pStrip1->front())
    {
		// retreiving first element
		pStrip2->pop_front();
		// adding the rest to strip1
		for (pos=pStrip2->begin(); pos!=pStrip2->end();pos++)
		{
			index=(*pos);
			ASSERT_MERGE_RET(index>=0,"CListContour::MergeStrips::0");
			pStrip1->insert(pStrip1->begin(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if (pStrip2->front()==pStrip1->back())
    {
		pStrip2->pop_front();
		// adding the rest to strip1
		for (pos=pStrip2->begin(); pos!=pStrip2->end();pos++)
		{
			index=(*pos);
			ASSERT_MERGE_RET(index>=0,"CListContour::MergeStrips::1");
			pStrip1->insert(pStrip1->end(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if (pStrip2->back()==pStrip1->front())
    {
		pStrip2->pop_back();
		// adding the rest to strip1
		for (rpos=pStrip2->rbegin(); rpos!=pStrip2->rend();rpos++)
		{
			index=(*rpos);
			ASSERT_MERGE_RET(index>=0,"CListContour::MergeStrips::2");
			pStrip1->insert(pStrip1->begin(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	if (pStrip2->back()==pStrip1->back())
    {
		pStrip2->pop_back();
		// adding the rest to strip1
		for (rpos=pStrip2->rbegin(); rpos!=pStrip2->rend();rpos++)
		{
			index=(*rpos);
			ASSERT_MERGE_RET(index>=0,"CListContour::MergeStrips::3");
			pStrip1->insert(pStrip1->end(),index);
		}
		pStrip2->clear();
		return true;
    }
	
	return false;
}

bool CListContour::CompactStrips()
{
	CLineStrip* pStrip;
	CLineStrip* pStripBase;
	UINT i;
	CLineStripList::iterator pos,pos2;
	CLineStripList newList;
	bool again, changed;	
	
	const double weldDist = 10*(m_dDx*m_dDx+m_dDy*m_dDy);

	ASSERT_RET(m_vStripLists.size() == GetNPlanes(),"CListContour::CompactStrips::0");
	for (i=0;i<GetNPlanes();i++)
	{
		double planeHeight = GetPlane(i);
		again=true;
		while(again)
		{
			// REPEAT COMPACT PROCESS UNTILL LAST PROCESS MAKES NO CHANGE
			
			again=false;
			// building compacted list
			ASSERT_RET(newList.empty(),"CListContour::CompactStrips::1");
			for (pos=m_vStripLists[i].begin(); pos!=m_vStripLists[i].end();pos++)
			{
				pStrip=(*pos);
				for (pos2=newList.begin(); pos2!=newList.end();pos2++)
				{
					pStripBase=(*pos2);
					changed=MergeStrips(pStripBase,pStrip);
					if (changed)
						again=true;
					if (pStrip->empty())
						break;
				}
				if (pStrip->empty())
					delete pStrip;
				else
					newList.insert(newList.begin(),pStrip);
			}
			

			// deleting old list
			m_vStripLists[i].clear();
			CLineStrip* pStrip2;
			// Copying all
			for (pos2=newList.begin(); pos2 != newList.end(); pos2++)
			{
				pStrip2=(*pos2);
				CLineStrip::iterator pos1 = pStrip2->begin(),pos3;
				while (pos1!=pStrip2->end()) 
				{
					pos3 = pos1;
					pos3++;
					if (pos3==pStrip2->end()) break; //G.Barrand
					if ( (*pos1) == (*pos3))
					{
					  /*G.Barrand : we can't compare with pStrip2->end() content.
						if ( (*pos3) != (*pStrip2->end()))
							pStrip2->erase(pos3);
						else
						{
							pStrip2->erase(pos3);
							break;
						}
					  */
   					  pStrip2->erase(pos3); //G.Barrand.
					}
					else
						pos1++;
				}
				
				//if (!(pStrip2->front()==pStrip2->back() && pStrip2->size()==2))
				if (pStrip2->size()!=1) 	
					m_vStripLists[i].insert(m_vStripLists[i].begin(),pStrip2 );
				else 
					delete pStrip2;
			}
			// emptying temp list
			newList.clear();
			
		} // OF WHILE(AGAIN) (LAST COMPACT PROCESS MADE NO CHANGES)

	
		if (m_vStripLists[i].empty())
			continue;
		///////////////////////////////////////////////////////////////////////
		// compact more
		int Nstrip,j,index,count;

		Nstrip = m_vStripLists[i].size(); 
		std::vector<bool> closed(Nstrip);
		double x,y;

		// First let's find the open and closed lists in m_vStripLists
		for(pos2 = m_vStripLists[i].begin(), j=0, count=0; pos2 != m_vStripLists[i].end(); pos2++, j++) 
		{
			pStrip = (*pos2);

			// is it open ?
			if (pStrip->front() != pStrip->back()) 
			{
				index = pStrip->front();
				x = GetXi(index); y = GetYi(index);
				index = pStrip->back();
				x -= GetXi(index); y -= GetYi(index);
				
				// is it "almost closed" ?
				if ((x*x+y*y < weldDist) ||
                                    SpecialCompactStripCase(GetXi(pStrip->front()), // L.Garnier
                                                            GetXi(pStrip->back()),
                                                            GetYi(pStrip->front()),
                                                            GetYi(pStrip->back()),
                                                            planeHeight))
                                {
                                        closed[j] = true; 
                                        // close it !!! Added by L.Garnier
                                        pStrip->push_back(pStrip->front());
                                        //
                                }
				else 
				{ 
					closed[j] = false; 
					// updating not closed counter...
					count ++; 
				}
			} 
			else 
				closed[j] = true;
		}
		
		// is there any open strip ?
		if (count > 1) 
		{ 
			// Merge the open strips into NewList
			pos = m_vStripLists[i].begin();
			for(j=0;j<Nstrip;j++) 
			{
				if (closed[j] == false ) 
				{
					pStrip = (*pos);
					newList.insert(newList.begin(),pStrip);	
					pos = m_vStripLists[i].erase(pos);
				}
				else 
					pos ++;
			}
			
			// are they open strips to process ?
			while(newList.size()>1) 
			{
				pStripBase = newList.front();
				
				// merge the rest to pStripBase
				again = true;
				while (again) 
				{
					again = false;
					pos = newList.begin(); 
					for(pos++; pos!=newList.end();) 
					{
						pStrip = (*pos);
						changed = ForceMerge(pStripBase,pStrip,planeHeight);
						if (changed) 
						{ 
							again = true;
							delete pStrip;
							pos = newList.erase(pos);
						} 
						else 
							pos ++;
					}		      
				} // while(again)

				index = pStripBase->front();
				x = GetXi(index); y = GetYi(index);
				index = pStripBase->back();
				x -= GetXi(index); y -= GetYi(index);
				// if pStripBase is closed or not

				if ((x*x+y*y < weldDist) ||
                                    SpecialCompactStripCase(GetXi(pStripBase->front()), // L.Garnier
                                                            GetXi(pStripBase->back()),
                                                            GetYi(pStripBase->front()),
                                                            GetYi(pStripBase->back()),
                                                            planeHeight))
                                {

                                  // close it !!! Added by L.Garnier
                                  if ((x!=0) || (y!=0)) {
                                    pStripBase->push_back(pStripBase->front());
                                  }
                                  //
                                  m_vStripLists[i].insert(m_vStripLists[i].begin(),pStripBase);
                                  newList.pop_front();
				}
				else 
				{			
					if (OnBoundary(pStripBase)) 
					{
						TRACE(_TT("# open strip ends on boundary, continue.\n"));
						m_vStripLists[i].insert(m_vStripLists[i].begin(),pStripBase);
						newList.pop_front();
					}
					else 
					{
						PROBLEM(_TT("unpaird open strip at 1!\n"));
						//exit(0);
						return false;
					}
				}
			} // while(newList.size()>1);


			if (newList.size() ==1) 
			{
				pStripBase = newList.front();
				index = pStripBase->front(); // L.Garnier
				x = GetXi(index); y = GetYi(index); // L.Garnier
				index = pStripBase->back(); // L.Garnier
				x -= GetXi(index); y -= GetYi(index); // L.Garnier
				
				// is it "almost closed", give last chance...5*weldDist
				if (x*x+y*y < 3*weldDist) // L.Garnier
				{
					m_vStripLists[i].insert(m_vStripLists[i].begin(),pStripBase);
					newList.pop_front();
				}
				else if (OnBoundary(pStripBase)) 
				{
					TRACE(_TT("# open strip ends on boundary, continue.\n"));
					m_vStripLists[i].insert(m_vStripLists[i].begin(),pStripBase);
					newList.pop_front();
				}
				else 
				{
					PROBLEM(_TT("unpaird open strip at 2!\n"));
					DumpPlane(i);
					//exit(0);
					return false;
				}
			}
			
			newList.clear();
			
		} 
		else if (count == 1) 
		{
			pos = m_vStripLists[i].begin();
			for(j=0;j<Nstrip;j++) 
			{
				if (closed[j] == false ) 
				{
					pStripBase = (*pos);
					break;
				}
				pos ++;
			}
			index = pStripBase->front(); // L.Garnier
			x = GetXi(index); y = GetYi(index); // L.Garnier
			index = pStripBase->back(); // L.Garnier
			x -= GetXi(index); y -= GetYi(index); // L.Garnier
       			
			// is it "almost closed", give last chance...5*weldDist
			if (x*x+y*y < 2*weldDist) // L.Garnier
			{
			  //close it!!
			  pStripBase->push_back(pStripBase->front()); // L.Garnier
			}
			else if (OnBoundary(pStripBase)) 
			{
				TRACE(_TT("# open strip ends on boundary, continue.\n"));
				pStripBase->push_back(pStripBase->front()); // L.Garnier
			} 
			else 
			{
				TRACE(_TT("unpaird open strip at 3!"));
				DumpPlane(i);
				return false;  // L.Garnier
				//exit(0);
			}
			newList.clear();  // L.Garnier
		}
	}
        return true;
}


bool CListContour::OnBoundary(CLineStrip* pStrip) 
{
	bool e1,e2;

	int index = pStrip->front();
	double x = GetXi(index), y = GetYi(index);
	if (x==m_pLimits[0] || x == m_pLimits[1] || 
		y == m_pLimits[2] || y == m_pLimits[3])
		e1 = true;
	else 
		e1 = false;
	
	index = pStrip->back();
	x = GetXi(index); y = GetYi(index);
	if (x==m_pLimits[0] || x == m_pLimits[1] || 
		y == m_pLimits[2] || y == m_pLimits[3])
		e2 = true;
	else 
		e2 = false;
	
	return (e1 && e2);
}

void CListContour::DumpPlane(UINT iPlane) const
{
	CLineStripList::const_iterator pos;
	UINT i;
	CLineStrip* pStrip;
	
	/*ASSERT(iPlane>=0,"CListContour::DumpPlane");*/
	ASSERT(iPlane<GetNPlanes(),"CListContour::DumpPlane::0");
	TRACE(_TT("Level : %d"),GetPlane(iPlane));
	
	TRACE(_TT("Number of strips : %d\r\n"),m_vStripLists[iPlane].size());
	TRACE(_TT("i np start end xstart ystart xend yend\r\n"));
	for (pos = m_vStripLists[iPlane].begin(), i=0; pos != m_vStripLists[iPlane].end(); pos++, i++)
	{
		pStrip=*pos;
		ASSERT(pStrip,"CListContour::DumpPlane::1");
		TRACE(_TT("%d %d %d %d %g %g %g %g\r\n"),i,pStrip->size(),pStrip->front(),pStrip->back(),
			GetXi(pStrip->front()),GetYi(pStrip->front()),
			GetXi(pStrip->back()),GetYi(pStrip->back()) );
	}
}

double CListContour::Area(CLineStrip* Line) 
{
	// if Line is not closed, return 0;
	
	double Ar = 0, x, y, x0,y0,x1, y1;
	int index;
	
	CLineStrip::iterator pos;
	pos = Line->begin();
	index = (*pos);
	x0 = x =  GetXi(index);
	y0 = y =  GetYi(index);
	
	pos ++;
	
	for(;pos!=Line->end();pos++) 
	{
		index = (*pos);  
		x1 = GetXi(index);
		y1 = GetYi(index);
		// Ar += (x1-x)*(y1+y);
		Ar += (y1-y)*(x1+x)-(x1-x)*(y1+y);
		x = x1;
		y = y1;  
		
	}
	
	
	//Ar += (x0-x)*(y0+y);
	Ar += (y0-y)*(x0+x)-(x0-x)*(y0+y);
	// if not closed curve, return 0;
	if ((x0-x)*(x0-x) + (y0-y)*(y0-y)>20*(m_dDx*m_dDx+m_dDy*m_dDy)) 
	{
		Ar = 0;
		TRACE(_TT("# open curve!\n"));
	}
	//else   Ar /= -2;
	else Ar/=4;
	// result is \int ydex/2 alone the implicit direction.
	return Ar;
}

double CListContour:: EdgeWeight(CLineStrip* pLine, double R) 
{
	CLineStrip::iterator pos;
	int count = 0,index;
	double x,y;
	for(pos = pLine->begin(); pos!=pLine->end(); pos++) 
	{
		index = (*pos);
		x = GetXi(index); 
		y = GetYi(index);
		if (fabs(x) > R || fabs(y) > R) 
			count ++;
	}
	return (double)count/pLine->size();
}

bool CListContour::PrintContour(std::ostream& a_out) 
{
        std::streamsize old_prec = a_out.precision(3);
	a_out << std::setprecision(10);
	
	UINT i, index;
	CLineStrip* pStrip;
	CLineStrip::iterator pos2;
	CLineStripList::iterator pos;
	
	for(i=0;i<GetNPlanes();i++) {
		for(pos = m_vStripLists[i].begin();pos!=m_vStripLists[i].end();pos++) 
		{
			pStrip = (*pos);
			for(pos2 = pStrip->begin();pos2!=pStrip->end();pos2++) 
			{
			  index = (*pos2);
  			  a_out << GetXi(index)<<"\t"<<GetYi(index)<<"\n";
			}
			a_out<<"\n";
		}
	}
        a_out.precision(old_prec);
	return true;
	
}
//G.Barrand : from .h to .cxx to avoid ASSERT in .h
CLineStripList* CListContour::GetLines(unsigned int iPlane)	{	/*ASSERT(iPlane>=0);*/ ASSERT(iPlane<GetNPlanes(),"CListContour::GetLines");	return &m_vStripLists[iPlane];}

// Added by L.Garnier
bool CListContour::SpecialCompactStripCase(double aXfront,double aXback,double aYfront,double aYback,double actualHeight) 
{
  // To solve the case of a list of strips 
  // which appeared to be open but should correspond to a closed
  // contour.
  //  With the today Generate() algorithm, it appears that we fall
  // on this case when the begin and end points
  // are on a horizontal or vertical line.
  // (case where front()->x == back()->x or front()->y == back()->y).
  //  We try to detect in this method this situation and decide to close
  // or not the line. To do that we check the heigth of intermediate 
  // points to see if there are on the same contour; if so we close
  // the line (and return true), if not we do nothing (and return false).

  // check if we could realy close it
  float marge = 1; // *m_dDy or *m_dDx. 1 seems to be good and normal, but why
                   // not 2 in certain cases???

  double distToNext =0;
  // try to get the correct hight
  if (GetPlane(0)>= actualHeight) {
    return false;
  }
  if (GetNPlanes() >1){
    distToNext = GetPlane(1)-GetPlane(0);
  } else {
    return false;
  }

  if ((aYback-aYfront) == 0) {
    double temp;
    double av;
    double eg;
    double ap;

    if (aXfront==m_pLimits[0] && aXback == m_pLimits[1]) return false;
    if (aXfront==m_pLimits[1] && aXback == m_pLimits[0]) return false;

    if (aXfront > aXback ) {
      temp = aXfront;
      aXfront = aXback;
      aXback = temp;
    }
    for(double check=aXfront+m_dDx;
        check<aXback;
        check+=m_dDx) {
      av = ((*m_pFieldFcn)(check,aYback-marge*m_dDy,m_pFieldFcnData)-actualHeight);
      eg = ((*m_pFieldFcn)(check,aYback,m_pFieldFcnData)-actualHeight);
      ap = ((*m_pFieldFcn)(check,aYback+marge*m_dDy,m_pFieldFcnData)-actualHeight);

      if ((av>distToNext) && (ap>distToNext) && (eg>distToNext)) {
        return false;
      } else if ((av<0) && (ap<0) && (eg<0)) {
        return false;
      }
    }
    return true;
  } else if ((aXback-aXfront) == 0) {
    double temp;
    double av;
    double eg;
    double ap;
    if (aYfront==m_pLimits[3] && aYback == m_pLimits[2]) return false;
    if (aYfront==m_pLimits[2] && aYback == m_pLimits[3]) return false;

    if (aYfront > aYback ) {
      temp = aYfront;
      aYfront = aYback;
      aYback = temp;
    }

    for(double check=aYfront+m_dDy;
        check<aYback;
        check+=m_dDy) {
      av = ((*m_pFieldFcn)(aXback-marge*m_dDx,check,m_pFieldFcnData)-actualHeight);
      eg = ((*m_pFieldFcn)(aXback,check,m_pFieldFcnData)-actualHeight);
      ap = ((*m_pFieldFcn)(aXback+marge*m_dDx,check,m_pFieldFcnData)-actualHeight);
      if ((av>distToNext) && (ap>distToNext) && (eg>distToNext)) {
        return false;
      } else if ((av<0) && (ap<0) && (eg<0)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

#undef _TT
//#undef TRACE
#undef PROBLEM
#undef ASSERT
