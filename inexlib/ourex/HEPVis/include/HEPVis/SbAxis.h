//  20/08/1998 : G.Barrand : creation.
//  05/01/2004 : L.GARNIER. Remove tick logic that had been displaced
//               on the SoAxis class.

#ifndef HEPVis_SbAxis_h
#define HEPVis_SbAxis_h
 
#include <Inventor/SbBasic.h> 

#include <HEPVis/SbMath.h>

class SbAxis {
public:
  SbAxis()
  :fMinimumValue(0)
  ,fMaximumValue(0)
  ,fNumberOfSteps(0)
  ,fLogScale(FALSE)
  {}
  virtual ~SbAxis(){}
public:
  SbAxis(const SbAxis& aFrom)
  :fMinimumValue(aFrom.fMinimumValue)
  ,fMaximumValue(aFrom.fMaximumValue)
  ,fNumberOfSteps(aFrom.fNumberOfSteps)
  ,fLogScale(aFrom.fLogScale)
  {}
  SbAxis& operator=(const SbAxis& aFrom){
    fMinimumValue = aFrom.fMinimumValue;
    fMaximumValue = aFrom.fMaximumValue;
    fNumberOfSteps = aFrom.fNumberOfSteps;
    fLogScale = aFrom.fLogScale;
    return *this;
  }
public:
  SbBool setLogScale(SbBool aValue){
    if(fLogScale==aValue) return FALSE; 
    fLogScale = aValue;
    return TRUE;
  }
  SbBool setMinimumValue(float aValue) {
    if(fMinimumValue==aValue) return FALSE; 
    fMinimumValue = aValue;
    return TRUE;
  }
  SbBool setMaximumValue(float aValue) {
    if(fMaximumValue==aValue) return FALSE; 
    fMaximumValue = aValue;
    return TRUE;
  }
  float getMinimumValue() const {return fMinimumValue;}
  float getMaximumValue() const {return fMaximumValue;}
  SbBool isLogScale() const {return fLogScale;}

  void adjustAxis () { //from hippodraw.
    int axis = 0;
    float step;
    float mylow, myhigh;
#define N_NICE 4
    static const float nice[N_NICE] = { 1.0,2.0,2.5,5.0 };
  
    if (fMinimumValue > fMaximumValue) {
      float low = fMinimumValue;
      fMinimumValue = fMaximumValue;
      fMaximumValue = low;  
    } else if (fMinimumValue == fMaximumValue) {
      float value = fMinimumValue;  
      fMinimumValue = value - 1;
      fMaximumValue = value + 1;
      return;
    }
  
    //if (fNumberOfSteps <= 0) {
      axis    = 1;
      fNumberOfSteps = 10;
    //}

    /*
     * Round the "bin width" to a nice number.
     * If this is being done for an axis (ie fNumberOfSteps was 0 , then
     * we don't have to go > *fMaximumValue.
     */
    float w = (fMaximumValue - fMinimumValue)/((float)fNumberOfSteps);
    float mag = FFLOOR(FLOG10(w));  
    int i = 0;
    do {
      step   = nice[i] * FPOW(10.0,mag);
    
      mylow  = FFLOOR(fMinimumValue/step) * step;
      myhigh = axis==1 ? FCEIL(fMaximumValue/step) * step : 
                         mylow + step * fNumberOfSteps;
    
      i++;
      if (i>=N_NICE) {
        i = 0;
        mag++;
      }
    }
    while ( ( (axis==1) && myhigh < fMaximumValue) || 
            ( (axis==0) && myhigh <= fMaximumValue) );
  
    float range = myhigh - mylow;
  
    /*
     * we now have decided on a range. Try to move fMinimumValue/fMaximumValue a little
     *  to end up on a nice number.
     *
     * first check if either end is near 0.0
     */
    if ((fLogScale ==FALSE) && (fMinimumValue >= 0.0) && 
        (( (axis==1) && (range>=fMaximumValue) ) || 
         ( (axis==0) && (range>fMaximumValue) )) ) {  
      fMinimumValue = 0.0;
      fMaximumValue = range;
      return;
    }
  
    if ( (( (axis==1) && (fMaximumValue<=0.0) ) || 
          ( (axis==0) && (fMaximumValue<0.0) )) 
         && (-range<=fMinimumValue)) {     
      fMaximumValue = 0.0;
      fMinimumValue = -range;
      return;
    }
 
    /*
     * try to round *fMinimumValue.
     */
    /* correction */
    if( (fLogScale ==TRUE) && (fMinimumValue<=0.0)) {  
      fMinimumValue = 1.0;
    }
    
    i   = N_NICE-1;
    mag = myhigh != 0.0 ? FCEIL(FLOG10(FFABS(myhigh))) : 
                          FCEIL(FLOG10(FFABS(mylow)));
    
    do {
      step   = nice[i] * FPOW(10.0,mag);        
      mylow  = FFLOOR(fMinimumValue/step) * step;
      myhigh = mylow + range;      
      i--;
      if (i<0) {
        i = N_NICE-1;
        mag--;
      }
    } 
    while (( (fLogScale ==TRUE) && (mylow  <= 0.0)     ) || 
           ( (axis==1)  && (myhigh < fMaximumValue)  ) || 
           ( (axis==0)  && (myhigh <= fMaximumValue) ) );
    
    fMinimumValue = mylow;
    fMaximumValue = myhigh;    
  }


private:
  float fMinimumValue;
  float fMaximumValue;
  int fNumberOfSteps;
  SbBool fLogScale;
};

#endif
