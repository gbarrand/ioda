// this :
#include <HEPVis/SbLUT.h>

#include <string.h>
#include <stdio.h>
#include <math.h>

typedef unsigned char Uchar;

//////////////////////////////////////////////////////////////////////////////
SbLUT::SbLUT(
)
:fType(IDENTITY)
,fMin(0)
,fMax(0)
,fNbin(0)
,fNumber(0)
,fList(NULL)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
SbLUT::SbLUT(
 int aNumber 
)
:fType(IDENTITY)
,fMin(0)
,fMax(0)
,fNbin(0)
,fNumber(0)
,fList(NULL)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aNumber!=0) {
    fList = (Uchar*)new Uchar[aNumber];
    if(fList!=NULL) fNumber = aNumber;
  }
}
//////////////////////////////////////////////////////////////////////////////
SbLUT::~SbLUT(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  delete [] fList;
}
//////////////////////////////////////////////////////////////////////////////
void SbLUT::initialize (
 Type aType 
,int aMin 
,int aMax 
,int aNbin 
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aMin>=aMax) return;
  if(aMin<0) return;
  if(aMax<0) return;
  if( (fList==NULL) || (aMax>fNumber) ) {
    delete [] fList;
    fList = NULL;
    fNumber = 0;
    fList = (Uchar*)new Uchar[aMax];
    if(fList!=NULL) fNumber = aMax;
  }
  if(fList==NULL) return;
  //printf("debug : SbLUT::init %d %d %d\n",aMin,aMax,aNbin);
  int i,j;
  int px,dpx;
  if(aType==IDENTITY) { 
    for(i=0;i<fNumber;i++) fList[i] = (Uchar)i;
  } else if(aType==LINEAR) { 
    if((aNbin<=0)||(aNbin>254)) return;  // aNbin should be in [1,254].
    for(i=0;i<aMin;i++) fList[i] = (Uchar)0;
    px = aMin;
    dpx = (aMax-aMin)/aNbin;
    if(dpx<=0) return;
    for(j=1;j<=aNbin;j++) {
      for(i=px;i<px+dpx;i++) fList[i] = (Uchar)j;
      px += dpx;
    }
    for(i=px;i<fNumber;i++) fList[i] = (Uchar)(aNbin+1);
  } else if(aType==LOG) {
    double fpx,fmulpx;
    if((aNbin<=0)||(aNbin>254)) return;  /* aNbin in [1,254] */
    for(i=0;i<aMin;i++) fList[i] = (Uchar)0;
    px = aMin;
    fpx = (double)(aMax)/(double)(aMin);
    fmulpx = pow( (double)(10.0) , log10(fpx)/(double)(aNbin)  );
    fpx = (double)(aMin);
    for(j=0;j<aNbin;j++) {
      fpx *= fmulpx;
      dpx  = (int)(fpx)-px;
      for(i=px;i<px+dpx;i++) fList[i] = (Uchar)j;
      px  += dpx;
    }
    for(i=px;i<fNumber;i++) fList[i] = (Uchar)(aNbin+1);
  }
  fType = aType;
  fMin = aMin;  
  fMax = aMax;
  fNbin = aNbin;
}
//////////////////////////////////////////////////////////////////////////////
int SbLUT::getCellNumber(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fNumber;
}
//////////////////////////////////////////////////////////////////////////////
unsigned char* SbLUT::getCells(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fList;
}


