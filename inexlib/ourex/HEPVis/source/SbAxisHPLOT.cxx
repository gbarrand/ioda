// this :
#include <HEPVis/SbAxisHPLOT.h>

/*
 *
 * Code extracted from ROOT-4.03.02/root/graf/stc/TGaxis.cxx.
 * Itself built from code extracted from HPLOT.
 *
 *   G.Barrand : 12/04/2005.
 */

#include <Inventor/errors/SoDebugError.h>

#include <string>

#include <string.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>

#define MINIMUM(a,b) ((a)<(b)?(a):(b))
#define MAXIMUM(a,b) ((a)>(b)?(a):(b))

#define BIT(n)       (1 << (n))

// TAxis status bits
enum { 
  TAxis_kTickPlus      = BIT(9),
  TAxis_kTickMinus     = BIT(10),
  TAxis_kAxisRange     = BIT(11),
  TAxis_kCenterTitle   = BIT(12),
  TAxis_kCenterLabels  = BIT(14), //bit 13 is used by TObject
  TAxis_kRotateTitle   = BIT(15),
  TAxis_kPalette       = BIT(16),
  TAxis_kNoExponent    = BIT(17),
  TAxis_kLabelsHori    = BIT(18),
  TAxis_kLabelsVert    = BIT(19),
  TAxis_kLabelsDown    = BIT(20),
  TAxis_kLabelsUp      = BIT(21),
  TAxis_kIsInteger     = BIT(22),
  TAxis_kMoreLogLabels = BIT(23),
  TAxis_kDecimals      = BIT(11)
}; //in fBits2

enum {
  kIsOnHeap      = 0x01000000,    // object is on heap
  kNotDeleted    = 0x02000000,    // object has not been deleted
  kZombie        = 0x04000000,    // object ctor failed
  kBitMask       = 0x00ffffff
};

//static void SetBit(unsigned int f) { fBits |= f & kBitMask; }
//static void ResetBit(unsigned int f) { fBits &= ~(f & kBitMask); }

static int GetTextFont() { return 132;}
static void Error(const char* location, const char* fmt,...) {
  va_list args;
  va_start(args,fmt);
  char s[1024];
  vsprintf(s,fmt,args);
  va_end(args);
  SoDebugError::postInfo(location,"%s",s);
}

static double TMath_ATan2(double y, double x) { 
  if (x != 0) return  ::atan2(y, x);
  if (y == 0) return  0;
  if (y >  0) return  M_PI/2;
  else        return -M_PI/2;
}

//static short TMath_Abs(short d)  { return (d >= 0) ? d : -d; }
static int TMath_Abs(int d) { return (d >= 0) ? d : -d; }
//static long TMath_Abs(long d) { return (d >= 0) ? d : -d; }
//static float TMath_Abs(float d) { return (d >= 0) ? d : -d; }
static double TMath_Abs(double d) { return (d >= 0) ? d : -d; }

static void TGaxis_Rotate(
 double X,  double Y,  double CFI, double SFI
,double XT, double YT, double &U,   double &V)
{
  U = CFI*X-SFI*Y+XT;
  V = SFI*X+CFI*Y+YT;
}

SbAxisHPLOT::SbAxisHPLOT()
:fMaxDigits(5)
,fBits(kNotDeleted)
,fTickSize(0.03F)
,fLabelOffset(0.005F)
,fLabelSize(0.04F)
,fTitleOffset(1)
,fTitleSize(0.04F)
,fLabelFont(62)
{}

SbAxisHPLOT::~SbAxisHPLOT(){}

void SbAxisHPLOT::setTitle(const std::string& aTitle) { 
  fTitle = aTitle;
}

bool SbAxisHPLOT::testBit(unsigned int f) { 
  return (bool) ((fBits & f) != 0); 
}

void TGaxis_LabelsLimits(const char *label,int &first,int &last) {
  last = strlen(label)-1;
  for (int i=0; i<=last; i++) {
    if (strchr("1234567890-+.", label[i]) ) { first = i; return; }
  }
  Error("LabelsLimits", "attempt to draw a blank label");
}

void SbAxisHPLOT::paint(
 double xmin, double ymin
,double xmax, double ymax
,double& wmin,double& wmax
,int& ndiv
,const std::string& aCHOPT
,double gridlength
,bool drawGridOnly
,std::vector< std::pair<SbVec3f,SbVec3f> >& aLinesAxis
,std::vector< std::pair<SbVec3f,SbVec3f> >& aLinesGrid
,std::vector<SbAxisHPLOT_Text>& aTexts
){
  // Control function to draw an axis
  // ================================
  //
  //============> Original authors (O.Couet C.E.Vandoni N.Cremel-Somon)
  //              largely modified and converted to C++ class by Rene Brun
  //
  // _Input parameters:
  //
  //  xmin      : X origin coordinate in WC space.
  //  xmax      : X end axis coordinate in WC space.
  //  ymin      : Y origin coordinate in WC space.
  //  ymax      : Y end axis coordinate in WC space.
  //  wmin      : Lowest value for the tick mark
  //              labels written on the axis.
  //  wmax      : Highest value for the tick mark labels
  //              written on the axis.
  //  ndiv      : Number of divisions.
  //
  //       ndiv=N1 + 100*N2 + 10000*N3
  //       N1=number of 1st divisions.
  //       N2=number of 2nd divisions.
  //       N3=number of 3rd divisions.
  //           e.g.:
  //           nndi=0 --> no tick marks.
  //           nndi=2 --> 2 divisions, one tick mark in the middle
  //                      of the axis.
  //
  //  chopt :  Options (see below).
  //
  //       chopt='G': loGarithmic scale, default is linear.
  //       chopt='B': Blank axis. Useful to superpose axis.
  //
  // Orientation of tick marks on axis.
  // ----------------------------------
  //
  //   Tick marks are normally drawn on the positive side of the axis,
  //   however, if X0=X1, then negative.
  //
  //       chopt='+': tick marks are drawn on Positive side. (default)
  //       chopt='-': tick mark are drawn on the negative side.
  //       i.e: '+-' --> tick marks are drawn on both sides of the axis.
  //       chopt='U': Unlabeled axis, default is labeled.
  //
  // Size of tick marks
  // ------------------
  // By default, tick marks have a length equal to 3 per cent of the
  // axis length.
  // When the option "S" is specified, the length of the tick marks
  // is equal to fTickSize*axis_length, where fTickSize may be set
  // via TGaxis::SetTickSize.
  //
  // Position of labels on axis.
  // ---------------------------
  //
  //   Labels are normally drawn on side opposite to tick marks.
  //   However:
  //
  //       chopt='=': on Equal side
  //
  // Orientation of labels on axis.
  // ------------------------------
  //
  //   Labels are normally drawn parallel to the axis.
  //   However if X0=X1, then Orthogonal
  //           if Y0=Y1, then Parallel
  //
  // Position of labels on tick marks.
  // ---------------------------------
  //
  //   Labels are centered on tick marks.
  //   However , if X0=X1, then they are right adjusted.
  //
  //       chopt='R': labels are Right adjusted on tick mark.
  //                    (default is centered)
  //       chopt='L': labels are Left adjusted on tick mark.
  //       chopt='C': labels are Centered on tick mark.
  //       chopt='M': In the Middle of the divisions.
  //
  // Format of labels.
  // -----------------
  //
  //   Blank characters are stripped, and then the
  //   label is correctly aligned. the dot, if last
  //   character of the string, is also stripped,
  //   unless the option "." (a dot, or period) is specified.
  //   if SetDecimals(true) has been called (bit TAxis_kDecimals set).
  //   all labels have the same number of decimals after the "."
  //   The same is true if gStyle->SetStripDecimals(false) has been called.
  //
  //   In the following, we have some parameters, like
  //   tick marks length and characters height (in percentage
  //   of the length of the axis (WC))
  //   The default values are as follows:
  //
  //   Primary tick marks: 3.0 %
  //   Secondary tick marks: 1.5 %
  //   Third order tick marks: .75 %
  //   Characters height for labels: 4%
  //
  //   Labels offset: 1.0 %
  //
  // Optional grid.
  // --------------
  //
  //       chopt='W': cross-Wire
  //   In case of a log axis, the grid is only drawn for the primary tick marks
  //   if the number of secondary and tertiary divisions is 0.
  //
  // Axis bining optimization.
  // -------------------------
  //
  //   By default the axis bining is optimized .
  //
  //       chopt='N': No bining optimization
  //       chopt='I': Integer labelling
  //
  // Maximum Number of Digits for the axis labels
  // --------------------------------------------
  // See the static function TGaxis::SetMaxDigits
  //
  // Time representation.
  // --------------------
  //
  //   Axis labels may be considered as times, plotted in a defined 
  //   time format.
  //   The format is set with SetTimeFormat().
  //   wmin and wmax are considered as two time values in seconds.
  //   The time axis will be spread around the time offset value (set with
  //   SetTimeOffset() ). Actually it will go from TimeOffset+wmin to
  //   TimeOffset+wmax.
  //   see examples in tutorials timeonaxis.C and timeonaxis2.C
  //
  //       chopt='t': Plot times with a defined format instead of values
  //

   aLinesAxis.clear();
   aLinesGrid.clear();
   aTexts.clear();

   double alfa, beta, ratio1, ratio2, grid_side;
   double axis_lengthN = 0;
   double axis_length0 = 0;
   double axis_length1 = 0;
   double charheight;
   double phil, phi, sinphi, cosphi, asinphi, acosphi;
   double BinLow,  BinLow2,  BinLow3;
   double BinHigh, BinHigh2, BinHigh3;
   double BinWidth, BinWidth2, BinWidth3;
   double xpl1, xpl2, ypl1, ypl2;
   double Xtick = 0;
   double Xtick0, Xtick1, DXtick=0;
   double Ytick, Ytick0, Ytick1;
   double Wlabel, DWlabel;
   double Xlabel, Ylabel;
   double DXlabel;
   double X0, X1, Y0, Y1, XX0, XX1, YY0, YY1;
   XX0 = XX1 = YY0 = YY1 = 0;
   double Xxmin, Xxmax, Yymin, Yymax;
   Xxmin = Xxmax = Yymin = Yymax = 0;
   double XLside,XMside;
   double WW, AF, RNE;
   double XX, YY;
   double Y;
   double Xtwo;
   int i, j, k, l, decade, ltick;
   int Mside, Lside;
   int IF1, IF2, NA, NF, NCH;
   int OptionLog,OptionBlank,OptionVert,OptionPlus,OptionMinus;
   int OptionUnlab,OptionPara;
   int OptionDown,OptionRight,OptionLeft,OptionCent,OptionEqual;
   int OptionDecimals=0,OptionDot;
   int OptionY,OptionText,OptionGrid,OptionSize,OptionNoopt;
   int OptionInt,OptionM,OptionUp,OptionX;
   int OptionTime;
   int first,last;
   int nbins;
   int N1Aold = 0;
   int NN1old = 0;
   int Xalign,Yalign;
   int ndyn;
   char* LABEL;
   char* CHTEMP;
   char CHLABEL[256];
   char kCHTEMP[256];
   double rangeOffset = 0;

   double epsilon   = 1e-5;
   const double kPI = M_PI; //GB
   double textSize = 0.05; //GB
   short textAlign = 11; //GB

   double rwmi = wmin;
   double rwma = wmax;
   CHTEMP = &kCHTEMP[0];
   LABEL  = &CHLABEL[0];

   bool noExponent = testBit(TAxis_kNoExponent);

   // If MoreLogLabels = true more Log Intermediate Labels are drawn.
   bool MoreLogLabels = testBit(TAxis_kMoreLogLabels);

   // the following parameters correspond to the pad range in NDC
   // and the WC coordinates in the pad

   double padh   = 1;//FIXME gPad->GetWh()*gPad->GetAbsHNDC();
   double RWxmin = 0;
   double RWxmax = 1;
   double RWymin = 0;
   double RWymax = 1;

#define SETOPT(aChar,aOpt) aOpt = aCHOPT.find(aChar)!=std::string::npos?1:0;

   SETOPT('G',OptionLog)
   SETOPT('B',OptionBlank)
   SETOPT('V',OptionVert)
   SETOPT('+',OptionPlus)
   SETOPT('-',OptionMinus)
   SETOPT('U',OptionUnlab)
   SETOPT('P',OptionPara)
   SETOPT('O',OptionDown)
   SETOPT('R',OptionRight)
   SETOPT('L',OptionLeft)
   SETOPT('C',OptionCent)
   SETOPT('=',OptionEqual)
   SETOPT('Y',OptionY)
   SETOPT('T',OptionText)
   SETOPT('W',OptionGrid)
   SETOPT('S',OptionSize)
   SETOPT('N',OptionNoopt)
   SETOPT('I',OptionInt)
   SETOPT('M',OptionM)
   SETOPT('0',OptionUp)
   SETOPT('X',OptionX)
   SETOPT('t',OptionTime)
   SETOPT('.',OptionDot)

   if (testBit(TAxis_kTickPlus))     OptionPlus  = 2;
   if (testBit(TAxis_kTickMinus))    OptionMinus = 2;
   if (testBit(TAxis_kCenterLabels)) OptionM     = 1;
   if (testBit(TAxis_kDecimals))     OptionDecimals = 1;
   /*FIXME if (fAxis) {
      if (fAxis->GetLabels()) {
         OptionM    = 1;
         OptionText = 1;
         ndiv = fAxis->GetLast()-fAxis->GetFirst()+1;
      }
   }*/

   // Set the grid length

   if (OptionGrid) {
      if (gridlength == 0) gridlength = 0.8;
/*FIXME
      linegrid = new TLine();
      linegrid->SetLineColor(gStyle->GetGridColor());
      if (linegrid->GetLineColor() == 0) 
        linegrid->SetLineColor(GetLineColor());
      linegrid->SetLineStyle(gStyle->GetGridStyle());
      linegrid->SetLineWidth(gStyle->GetGridWidth());*/
   }

  
   if (OptionTime) {
     //printf("debug : SbAxisHPLOT::paint : fTimeFormat : \"%s\"\n",
     //    fTimeFormat.c_str());
   }

   // Determine time format
   std::string timeformat;
   std::string::size_type IdF = fTimeFormat.find("%F");
   if (IdF!=std::string::npos) {
     timeformat = fTimeFormat.substr(0,IdF);
   } else {
     timeformat = fTimeFormat;
   }

   // determine the time offset and correct for time offset not being integer
   double timeoffset = 0;
   if (OptionTime) {
      if (IdF!=std::string::npos) {
         int LnF = fTimeFormat.size();
         std::string stringtimeoffset = fTimeFormat.substr(IdF+2,LnF-(IdF+2));
         int yy, mm, dd, hh, mi, ss;
         if (::sscanf(stringtimeoffset.c_str(), 
                 "%d-%d-%d %d:%d:%d", &yy, &mm, &dd, &hh, &mi, &ss) == 6) {
            struct tm tp;
            struct tm* tptest;
            time_t timeoffsettest;
            tp.tm_year  = yy-1900;
            tp.tm_mon   = mm-1;
            tp.tm_mday  = dd;
            tp.tm_hour  = hh;
            tp.tm_min   = mi;
            tp.tm_sec   = ss;
            tp.tm_isdst = 0; // daylight saving time is not in effect (see mktime man pages)
            timeoffset  = mktime(&tp);
            // have to correct this time to go back to UTC
            timeoffsettest = (time_t)((long)timeoffset);
            tptest = gmtime(&timeoffsettest);
            timeoffset += timeoffsettest - mktime(tptest);
            // Add the time offset's decimal part if it is there
            std::string::size_type Ids   = stringtimeoffset.find("s");
            if (Ids != std::string::npos) {
               float dp;
               int Lns   = stringtimeoffset.size();
               std::string sdp = stringtimeoffset.substr(Ids+1,Lns-(Ids+1));
               ::sscanf(sdp.c_str(),"%g",&dp);
               timeoffset += dp;
            }
	    // if OptionTime = 2 gmtime will be used instead of localtime
            if (stringtimeoffset.find("GMT")!=std::string::npos) OptionTime =2;
         } else {
            Error("PaintAxis", "Time offset has not the right format");
         }
      } else {
         Error("PaintAxis", "%%F not found in fTimeFormat.");
         //FIXME timeoffset = gStyle->GetTimeOffset();
      }
      wmin += timeoffset - (int)(timeoffset);
      wmax += timeoffset - (int)(timeoffset);
      // correct for time offset at a good limit (min, hour, day, month, year)
      struct tm* tp0;
      time_t timetp = (time_t)((long)(timeoffset));
      double range = wmax - wmin;
      long rangeBase = 60;
      if (range>60)       rangeBase = 60*20;       // minutes
      if (range>3600)     rangeBase = 3600*20;     // hours
      if (range>86400)    rangeBase = 86400*20;    // days
      if (range>2419200)  rangeBase = 31556736;    // months (average # days)
      rangeOffset = (double) ((long)(timeoffset)%rangeBase);
      if (range>31536000) {
         tp0 = gmtime(&timetp);
         tp0->tm_mon   = 0;
         tp0->tm_mday  = 1;
         tp0->tm_hour  = 0;
         tp0->tm_min   = 0;
         tp0->tm_sec   = 0;
         tp0->tm_isdst = 0; // daylight saving time is not in effect (see mktime man pages)
         rangeBase = (timetp-mktime(tp0)); // years
         rangeOffset = (double) (rangeBase);
      }
      wmax += rangeOffset;
      wmin += rangeOffset;
   }

   // Determine number of divisions 1, 2 and 3
   int N1A   = ndiv%100;
   int N2A   = (ndiv%10000 - N1A)/100;
   int N3A   = ndiv/10000;
   int NN3   = MAXIMUM(N3A,1);
   int NN2   = MAXIMUM(N2A,1)*NN3;
   int NN1   = MAXIMUM(N1A,1)*NN2+1;
   int Nticks= NN1;

   // Axis bining optimization is ignored if:
   // - the first and the last label are equal
   // - the number of divisions is 0
   // - less than 1 primary division is requested
   // - logarithmic scale is requested

   if (wmin == wmax || ndiv == 0 || N1A <= 1 || OptionLog) {
      OptionNoopt = 1;
      OptionInt   = 0;
   }

   // Axis bining optimization
   if ( (wmax-wmin) < 1 && OptionInt) {
      Error("PaintAxis", "option I not available");
      OptionInt = 0;
   }
   if (!OptionNoopt || OptionInt ) {

      // Primary divisions optimization
      // When integer labelling is required, Optimize is invoked first
      // and only if the result is not an integer labelling, AdjustBinSize 
      // is invoked.

      optimizeLimits(wmin,wmax,N1A,
                     BinLow,BinHigh,nbins,BinWidth,
                     aCHOPT);
      if (OptionInt) {
         if (BinLow != double(int(BinLow)) || 
             BinWidth != double(int(BinWidth))) {
            adjustBinSize(wmin,wmax,N1A,BinLow,BinHigh,nbins,BinWidth);
         }
      }
      if ((wmin-BinLow)  > epsilon) { BinLow  += BinWidth; nbins--; }
      if ((BinHigh-wmax) > epsilon) { BinHigh -= BinWidth; nbins--; }
      if (xmax == xmin) {
         double rtyw  = (ymax-ymin)/(wmax-wmin);
         Xxmin = xmin;
         Xxmax = xmax;
         Yymin = rtyw*(BinLow-wmin)  + ymin;
         Yymax = rtyw*(BinHigh-wmin) + ymin;
      }
      else {
         double rtxw  = (xmax-xmin)/(wmax-wmin);
         Xxmin = rtxw*(BinLow-wmin)  + xmin;
         Xxmax = rtxw*(BinHigh-wmin) + xmin;
         if (ymax == ymin) {
            Yymin = ymin;
            Yymax = ymax;
         }
         else {
            alfa  = (ymax-ymin)/(xmax-xmin);
            beta  = (ymin*xmax-ymax*xmin)/(xmax-xmin);
            Yymin = alfa*Xxmin + beta;
            Yymax = alfa*Xxmax + beta;
         }
      }
      /*GB if (fFunction) {
         Yymin = ymin;
         Yymax = ymax;
         Xxmin = xmin;
         Xxmax = xmax;
      } else*/ {
         wmin = BinLow;
         wmax = BinHigh;
      }

      // Secondary divisions optimization
      int NB2 = N2A;
      if (!OptionNoopt && N2A > 1 && BinWidth > 0) {
         optimizeLimits(wmin,wmin+BinWidth,N2A,
                        BinLow2,BinHigh2,NB2,BinWidth2,
                        aCHOPT);
      }

      // Tertiary divisions optimization
      int NB3 = N3A;
      if (!OptionNoopt && N3A > 1 && BinWidth2 > 0) {
         optimizeLimits(BinLow2,BinLow2+BinWidth2,N3A,
                        BinLow3,BinHigh3,NB3,BinWidth3,
                        aCHOPT);
      }
      N1Aold = N1A;
      NN1old = NN1;
      N1A    = nbins;
      NN3    = MAXIMUM(NB3,1);
      NN2    = MAXIMUM(NB2,1)*NN3;
      NN1    = MAXIMUM(N1A,1)*NN2+1;
      Nticks = NN1;
   }

   // Coordinates are normalized

   ratio1 = 1/(RWxmax-RWxmin);
   ratio2 = 1/(RWymax-RWymin);
   X0     = ratio1*(xmin-RWxmin);
   X1     = ratio1*(xmax-RWxmin);
   Y0     = ratio2*(ymin-RWymin);
   Y1     = ratio2*(ymax-RWymin);
   if (!OptionNoopt || OptionInt ) {
      XX0 = ratio1*(Xxmin-RWxmin);
      XX1 = ratio1*(Xxmax-RWxmin);
      YY0 = ratio2*(Yymin-RWymin);
      YY1 = ratio2*(Yymax-RWymin);
   }

   if ((X0 == X1) && (Y0 == Y1)) {
      Error("PaintAxis", "length of axis is 0");
      return;
   }

   // Return wmin, wmax and the number of primary divisions
   if (OptionX) {
      ndiv = N1A;
      return;
   }

   int maxDigits = 5;
   //FIXME if (fAxis) maxDigits = fMaxDigits;

/*FIXME
   TLatex *textaxis = new TLatex();
   lineaxis->SetLineColor(GetLineColor());
   lineaxis->SetLineStyle(1);
   lineaxis->SetLineWidth(GetLineWidth());
   textaxis->SetTextColor(GetTextColor());
   textaxis->SetTextFont(GetTextFont());

   if (!gPad->IsBatch()) {
      float chupxvsav, chupyvsav;
      gVirtualX->GetCharacterUp(chupxvsav, chupyvsav);
      gVirtualX->SetClipOFF(gPad->GetCanvasID());
   }
*/

   // Compute length of axis
   double axis_length = ::sqrt((X1-X0)*(X1-X0)+(Y1-Y0)*(Y1-Y0));
   if (axis_length == 0) {
      Error("PaintAxis", "length of axis is 0");
      return; //goto L210;
   }

   if (!OptionNoopt || OptionInt) {
      axis_lengthN = ::sqrt((XX1-XX0)*(XX1-XX0)+(YY1-YY0)*(YY1-YY0));
      axis_length0 = ::sqrt((XX0-X0)*(XX0-X0)+(YY0-Y0)*(YY0-Y0));
      axis_length1 = ::sqrt((X1-XX1)*(X1-XX1)+(Y1-YY1)*(Y1-YY1));
      if (axis_lengthN < epsilon) {
         OptionNoopt = 1;
         OptionInt   = 0;
         wmin        = rwmi;
         wmax        = rwma;
         N1A         = N1Aold;
         NN1         = NN1old;
         Nticks      = NN1;
         if (OptionTime) {
            wmin        += timeoffset - (int)(timeoffset) + rangeOffset;
            wmax        += timeoffset - (int)(timeoffset) + rangeOffset;
         }
      }
   }

   if (X0 == X1) {
      phi  = 0.5*kPI;
      phil = phi;
   } else {
            phi = TMath_ATan2((Y1-Y0),(X1-X0));
      int px0 = 0;//FIXME gPad->UtoPixel(X0);
      int py0 = 0;//FIXME gPad->VtoPixel(Y0);
      int px1 = 0;//FIXME gPad->UtoPixel(X1);
      int py1 = 0;//FIXME gPad->VtoPixel(Y1);
      if (X0 < X1) phil = TMath_ATan2(double(py0-py1), double(px1-px0));
      else         phil = TMath_ATan2(double(py1-py0), double(px0-px1));
   }
   cosphi  = ::cos(phi);
   sinphi  = ::sin(phi);
   acosphi = TMath_Abs(cosphi);
   asinphi = TMath_Abs(sinphi);
   if (acosphi <= epsilon) { acosphi = 0;  cosphi  = 0; }
   if (asinphi <= epsilon) { asinphi = 0;  sinphi  = 0; }

   // Mside positive, tick marks on positive side
   // Mside negative, tick marks on negative side
   // Mside zero, tick marks on both sides
   // Default is positive except for vertical axis

   Mside=1;
   if (X0 == X1 && Y1 > Y0)       Mside = -1;
   if (OptionPlus)                Mside = 1;
   if (OptionMinus)               Mside = -1;
   if (OptionPlus && OptionMinus) Mside = 0;
   XMside = Mside;
   Lside = -Mside;
   if (OptionEqual) Lside = Mside;
   if (OptionPlus && OptionMinus) {
      Lside = -1;
      if (OptionEqual) Lside=1;
   }
   XLside = Lside;

   // Tick marks size
   double tick_side;
   if(XMside >= 0) tick_side = 1;
   else            tick_side = -1;

   double atick[3];
   if (OptionSize) atick[0] = tick_side*axis_length*fTickSize;
   else            atick[0] = tick_side*axis_length*0.03;

   atick[1] = 0.5*atick[0];
   atick[2] = 0.5*atick[1];

   // Set the side of the grid
   if ((X0 == X1) && (Y1 > Y0))  grid_side =-1;
   else                          grid_side = 1;


   // Compute Values if Function is given
   /*GB if(fFunction) {
      rwmi = fFunction->Eval(wmin);
      rwma = fFunction->Eval(wmax);
      if(rwmi > rwma) {
         double t = rwma;
         rwma = rwmi;
         rwmi = t;
      }
   }*/

   // Draw the axis if needed...
   if (!OptionBlank) {
      xpl1 = X0;
      xpl2 = X1;
      ypl1 = Y0;
      ypl2 = Y1;
      aLinesAxis.push_back(std::pair<SbVec3f,SbVec3f>(SbVec3f((float)xpl1,(float)ypl1,0),SbVec3f((float)xpl2,(float)ypl2,0)));
   }

   // No bining

   if (ndiv == 0) return; //goto L210;
   if (wmin == wmax) {
      Error("PaintAxis", "wmin (%f) == wmax (%f)", wmin, wmax);
      return; //goto L210;
   }

   // Draw axis title if it exists
   if (!drawGridOnly && fTitle.size()) {
      textSize = fTitleSize;
      charheight = fTitleSize;
      if ((GetTextFont() % 10) > 2) {
         //FIXME charheight = charheight/gPad->GetWh();
      }
      double toffset = fTitleOffset;
      if (toffset < 0.1) toffset = 1;
      if (X1 == X0) Ylabel = XLside*1.6*charheight*toffset;
      else          Ylabel = XLside*1.3*charheight*toffset;
      if (Y1 == Y0) Ylabel = XLside*1.6*charheight*toffset;
      double axispos;
      if (testBit(TAxis_kCenterTitle)) axispos = 0.5*axis_length;
      else                       axispos = axis_length;
      if (testBit(TAxis_kRotateTitle)) {
         if (X1 >= X0) {
            if (testBit(TAxis_kCenterTitle)) textAlign = 22;
            else                             textAlign = 12;
            TGaxis_Rotate(axispos,Ylabel,cosphi,sinphi,X0,Y0,xpl1,ypl1);
         } else {
           if (testBit(TAxis_kCenterTitle)) textAlign = 22;
           else                             textAlign = 32;
           TGaxis_Rotate(axispos,Ylabel,cosphi,sinphi,X0,Y0,xpl1,ypl1);
         }
         printf("debug : SbAxisHPLOT : texts : dummy : 000\n");
         aTexts.push_back(SbAxisHPLOT_Text(xpl1,ypl1,
                                           phil=(kPI+phil)*180/kPI,
                                           fTitleSize,
                                           fTitle,textAlign));
      } else {
         if (X1 >= X0) {
            if (testBit(TAxis_kCenterTitle)) textAlign = 22;
            else                             textAlign = 32;
            TGaxis_Rotate(axispos,Ylabel,cosphi,sinphi,X0,Y0,xpl1,ypl1);
         } else {
           if (testBit(TAxis_kCenterTitle)) textAlign = 22;
           else                             textAlign = 12;
           TGaxis_Rotate(axispos,Ylabel,cosphi,sinphi,X0,Y0,xpl1,ypl1);
         }
         aTexts.push_back(SbAxisHPLOT_Text(xpl1,ypl1,
                                           phil*180/kPI,fTitleSize,
                                           fTitle,textAlign));
      }
   }

   // Labels preparation:
   // Get character height
   // Compute the labels orientation in case of overlaps
   // with alphanumeric labels for horizontal axis).

   charheight = fLabelSize;
   if (OptionText) charheight *= 0.66666;
   //FIXME textaxis->SetTextFont(fLabelFont);
   //FIXME textaxis->SetTextColor(GetLabelColor());
   textSize = charheight;
   //FIXME textaxis->SetTextAngle(GetTextAngle());
   if (fLabelFont%10 > 2) {
     charheight /= padh;
   }
   if (!OptionUp && !OptionDown && !OptionY) {
      if (!drawGridOnly && OptionText && ((ymin == ymax) || (xmin == xmax))) {
         textAlign = 32;
         OptionText = 2;
         //int nl = 0;//FIXME fAxis->GetLast()-fAxis->GetFirst()+1;
         //double angle     = 0;
         printf("debug : SbAxisHPLOT : FIXME : 000\n");
         /*FIXME
         for (i=fAxis->GetFirst(); i<=fAxis->GetLast(); i++) {
            textaxis->SetText(0,0,fAxis->GetBinLabel(i));
            if (textaxis->GetXsize() < (xmax-xmin)/nl) continue;
            angle = -20;
            break;
         }
         for (i=fAxis->GetFirst(); i<=fAxis->GetLast(); i++) {
            if ((!strcmp(fAxis->GetName(),"xaxis") && !gPad->testBit(kHori))
              ||(!strcmp(fAxis->GetName(),"yaxis") &&  gPad->testBit(kHori))) {
               if (nl > 50) angle = 90;
               if (fAxis->testBit(TAxis_kLabelsHori)) angle = 0;
               if (fAxis->testBit(TAxis_kLabelsVert)) angle = 90;
               if (fAxis->testBit(TAxis_kLabelsUp))   angle = 20;
               if (fAxis->testBit(TAxis_kLabelsDown)) angle =-20;
               if (angle==   0) textAlign = 23;
               if (angle== -20) textAlign = 12;
               printf("debug : SbAxisHPLOT : texts : dummy : 002\n");
               textaxis->PaintLatex(
                 fAxis->GetBinCenter(i),
                 gPad->GetUymin() - 3*fAxis->GetLabelOffset()*
                   (gPad->GetUymax()-gPad->GetUymin()),
                 angle,
                 charheight,
                 fAxis->GetBinLabel(i));
            } else if ((!strcmp(fAxis->GetName(),"yaxis") && !gPad->testBit(kHori))
                    || (!strcmp(fAxis->GetName(),"xaxis") &&  gPad->testBit(kHori))) {
               printf("debug : SbAxisHPLOT : texts : dummy : 003\n");
               textaxis->PaintLatex(
                 gPad->GetUxmin() - 3*fAxis->GetLabelOffset()*
                   (gPad->GetUxmax()-gPad->GetUxmin()),
                 fAxis->GetBinCenter(i),
                 0,
                 charheight,
                 fAxis->GetBinLabel(i));
            } else {
               printf("debug : SbAxisHPLOT : texts : dummy : 004\n");
               textaxis->PaintLatex(
                 xmin - 3*fAxis->GetLabelOffset()*
                   (gPad->GetUxmax()-gPad->GetUxmin()),
                 ymin +(i-0.5)*(ymax-ymin)/nl,
                 0,
                 charheight,
                 fAxis->GetBinLabel(i));
            }
         }*/
      }
   }

   // Now determine orientation of labels on axis
/*FIXME
   if (!gPad->IsBatch()) {
      if (cosphi > 0) gVirtualX->SetCharacterUp(-sinphi,cosphi);
      else            gVirtualX->SetCharacterUp(sinphi,-cosphi);
      if (X0 == X1)   gVirtualX->SetCharacterUp(0,1);
      if (OptionVert) gVirtualX->SetCharacterUp(0,1);
      if (OptionPara) gVirtualX->SetCharacterUp(-sinphi,cosphi);
      if (OptionDown) gVirtualX->SetCharacterUp(cosphi,sinphi);
   }*/

   // Now determine text alignment
   Xalign = 2;
   Yalign = 1;
   if (X0 == X1)    Xalign = 3;
   if (Y0 != Y1)    Yalign = 2;
   if (OptionCent)  Xalign = 2;
   if (OptionRight) Xalign = 3;
   if (OptionLeft)  Xalign = 1;
   if (TMath_Abs(cosphi) > 0.9) {
      Xalign = 2;
   } else {
      if (cosphi*sinphi > 0)  Xalign = 1;
      if (cosphi*sinphi < 0)  Xalign = 3;
   }
   textAlign = 10*Xalign+Yalign;

   // Position of labels in Y
   if (X0 == X1) {
      if (OptionPlus && !OptionMinus) {
         if (OptionEqual) Ylabel =  fLabelOffset/2 + atick[0];
         else             Ylabel = -fLabelOffset;
      } else {
         Ylabel = fLabelOffset;
         if (Lside < 0)  Ylabel += atick[0];
      }
   } else if (Y0 == Y1) {
      if (OptionMinus && !OptionPlus) {
         Ylabel = fLabelOffset+0.5*fLabelSize;
         Ylabel += TMath_Abs(atick[0]);
      } else {
         Ylabel = -fLabelOffset;
         if (Mside <= 0) Ylabel -= TMath_Abs(atick[0]);
      }
      if (OptionLog)  Ylabel -= 0.5*charheight;
   } else {
      if (Mside+Lside >= 0) Ylabel =  fLabelOffset;
      else                  Ylabel = -fLabelOffset;
   }
   if (OptionText) Ylabel /= 2;

   // Draw the linear tick marks if needed...
   if (!OptionLog) {
      if (ndiv) {
         /*GB if (fFunction) {
            if (OptionNoopt && !OptionInt) {
               DXtick=(BinHigh-BinLow)/double(Nticks-1);
            } else {
               DXtick=(BinHigh-BinLow)/double(Nticks-1);
            }
         } else */ {
            if (OptionNoopt && !OptionInt) DXtick=axis_length/double(Nticks-1);
            else                           DXtick=axis_lengthN/double(Nticks-1);
         }
         for (k=0;k<Nticks; k++) {
            ltick = 2;
            if (k%NN3 == 0) ltick = 1;
            if (k%NN2 == 0) ltick = 0;
            /*GB if (fFunction) {
               double xx = BinLow+double(k)*DXtick;
               double zz = fFunction->Eval(xx)-rwmi;
               Xtick = zz* axis_length / TMath_Abs(rwma-rwmi);
            } else */ {
               Xtick = double(k)*DXtick;
            }
            Ytick = 0;
            if (!Mside) Ytick -= atick[ltick];
            if ( OptionNoopt && !OptionInt) {
               TGaxis_Rotate(Xtick,Ytick,cosphi,sinphi,X0,Y0,xpl2,ypl2);
               TGaxis_Rotate(Xtick,atick[ltick],cosphi,sinphi,X0,Y0,xpl1,ypl1);
            }
            else {
               TGaxis_Rotate(Xtick,Ytick,cosphi,sinphi,XX0,YY0,xpl2,ypl2);
               TGaxis_Rotate(Xtick,atick[ltick],cosphi,sinphi,XX0,YY0,xpl1,ypl1);
            }
            if (OptionVert) {
               if ((X0 != X1) && (Y0 != Y1)) {
                  if (Mside) {
                     xpl1 = xpl2;
                     if (cosphi > 0) ypl1 = ypl2 + atick[ltick];
                     else            ypl1 = ypl2 - atick[ltick];
                  }
                  else {
                     xpl1 = 0.5*(xpl1 + xpl2);
                     xpl2 = xpl1;
                     ypl1 = 0.5*(ypl1 + ypl2) + atick[ltick];
                     ypl2 = 0.5*(ypl1 + ypl2) - atick[ltick];
                  }
               }
            }
            if (!drawGridOnly) {
              aLinesAxis.push_back
                (std::pair<SbVec3f,SbVec3f>(SbVec3f((float)xpl1,(float)ypl1,0),
                                          SbVec3f((float)xpl2,(float)ypl2,0)));
            }

            if (OptionGrid) {
               if (ltick == 0) {
                  if (OptionNoopt && !OptionInt) {
                    TGaxis_Rotate(Xtick,0,cosphi,sinphi,X0,Y0 ,xpl2,ypl2);
                    TGaxis_Rotate
                      (Xtick,grid_side*gridlength,cosphi,sinphi,X0,Y0,
                       xpl1,ypl1);
                  } else {
                    TGaxis_Rotate(Xtick,0,cosphi ,sinphi,XX0,YY0,xpl2,ypl2);
                    TGaxis_Rotate
                      (Xtick,grid_side*gridlength ,cosphi,sinphi,XX0,YY0,
                       xpl1,ypl1);
                  }
                  aLinesGrid.push_back
                    (std::pair<SbVec3f,SbVec3f>
                    (SbVec3f((float)xpl1,(float)ypl1,0),
                     SbVec3f((float)xpl2,(float)ypl2,0)));
               }
            }
         }
         Xtick0 = 0;
         Xtick1 = Xtick;

         if ((!OptionNoopt || OptionInt) && axis_length0) {
            int Nticks0;
            /*GB if (fFunction) Nticks0 = int((BinLow-wmin)/DXtick);
            else */          Nticks0 = int(axis_length0/DXtick);
            if (Nticks0 > 1000) Nticks0 = 1000;
            for (k=0; k<=Nticks0; k++) {
               ltick = 2;
               if (k%NN3 == 0) ltick = 1;
               if (k%NN2 == 0) ltick = 0;
               Ytick0 = 0;
               if (!Mside) Ytick0 -= atick[ltick];
               /*GB if (fFunction) {
                  Xtick0 = (fFunction->Eval(BinLow - double(k)*DXtick)-rwmi)
                           * axis_length / TMath_Abs(rwma-rwmi);
               }*/
               TGaxis_Rotate(Xtick0,Ytick0,cosphi,sinphi,XX0,YY0 ,xpl2,ypl2);
               TGaxis_Rotate(Xtick0,atick[ltick],cosphi,sinphi,XX0,YY0 ,xpl1,ypl1);
               if (OptionVert) {
                  if ((X0 != X1) && (Y0 != Y1)) {
                     if (Mside) {
                        xpl1 = xpl2;
                        if (cosphi > 0) ypl1 = ypl2 + atick[ltick];
                        else            ypl1 = ypl2 - atick[ltick];
                     }
                     else {
                        xpl1 = 0.5*(xpl1 + xpl2);
                        xpl2 = xpl1;
                        ypl1 = 0.5*(ypl1 + ypl2) + atick[ltick];
                        ypl2 = 0.5*(ypl1 + ypl2) - atick[ltick];
                     }
                  }
               }
               if(!drawGridOnly) {
                 aLinesAxis.push_back
                   (std::pair<SbVec3f,SbVec3f>
                      (SbVec3f((float)xpl1,(float)ypl1,0),
                       SbVec3f((float)xpl2,(float)ypl2,0)));
               }

               if (OptionGrid) {
                  if (ltick == 0) {
                    TGaxis_Rotate(Xtick0,0,cosphi,sinphi,XX0,YY0,xpl2,ypl2);
                    TGaxis_Rotate
                      (Xtick0,grid_side*gridlength,cosphi,sinphi,XX0,YY0,
                       xpl1,ypl1);
                    aLinesGrid.push_back
                      (std::pair<SbVec3f,SbVec3f>
                         (SbVec3f((float)xpl1,(float)ypl1,0),
                          SbVec3f((float)xpl2,(float)ypl2,0)));
                  }
               }
               Xtick0 -= DXtick;
            }
         }

         if ((!OptionNoopt || OptionInt) && axis_length1) {
            int Nticks1;
            /*GB if (fFunction) Nticks1 = int((wmax-BinHigh)/DXtick);
            else */          Nticks1 = int(axis_length1/DXtick);
            if (Nticks1 > 1000) Nticks1 = 1000;
            for (k=0; k<=Nticks1; k++) {
               ltick = 2;
               if (k%NN3 == 0) ltick = 1;
               if (k%NN2 == 0) ltick = 0;
               Ytick1 = 0;
               if (!Mside) Ytick1 -= atick[ltick];
               /*GB if (fFunction) {
                  Xtick1 = (fFunction->Eval(BinHigh + double(k)*DXtick)-rwmi)
                           * axis_length / TMath_Abs(rwma-rwmi);
               }*/
               TGaxis_Rotate(Xtick1,Ytick1,cosphi,sinphi,XX0,YY0 ,xpl2,ypl2);
               TGaxis_Rotate(Xtick1,atick[ltick],cosphi,sinphi,XX0,YY0 ,xpl1,ypl1);
               if (OptionVert) {
                  if ((X0 != X1) && (Y0 != Y1)) {
                     if (Mside) {
                        xpl1 = xpl2;
                        if (cosphi > 0) ypl1 = ypl2 + atick[ltick];
                        else            ypl1 = ypl2 - atick[ltick];
                     }
                     else {
                        xpl1 = 0.5*(xpl1 + xpl2);
                        xpl2 = xpl1;
                        ypl1 = 0.5*(ypl1 + ypl2) + atick[ltick];
                        ypl2 = 0.5*(ypl1 + ypl2) - atick[ltick];
                     }
                  }
               }
               if(!drawGridOnly) {
                 aLinesAxis.push_back
                   (std::pair<SbVec3f,SbVec3f>
                      (SbVec3f((float)xpl1,(float)ypl1,0),
                       SbVec3f((float)xpl2,(float)ypl2,0)));
               }

               if (OptionGrid) {
                  if (ltick == 0) {
                    TGaxis_Rotate(Xtick1,0,cosphi,sinphi,XX0,YY0 ,xpl2,ypl2);
                    TGaxis_Rotate
                      (Xtick1,grid_side*gridlength,cosphi,sinphi,XX0,YY0,
                       xpl1,ypl1);
                    aLinesGrid.push_back
                      (std::pair<SbVec3f,SbVec3f>
                         (SbVec3f((float)xpl1,(float)ypl1,0),
                          SbVec3f((float)xpl2,(float)ypl2,0)));
                  }
               }
               Xtick1 += DXtick;
            }
         }
      }
   }

   // Draw the numeric labels if needed...
   if (!drawGridOnly && !OptionUnlab) {
      if (!OptionLog) {
         if (N1A) {
            // Spacing of labels
            if ((wmin == wmax) || (ndiv == 0)) {
               Error("PaintAxis", "wmin (%f) == wmax (%f), or ndiv == 0", wmin, wmax);
               return; //goto L210;
            }
            Wlabel  = wmin;
            DWlabel = (wmax-wmin)/double(N1A);
            if (OptionNoopt && !OptionInt) DXlabel = axis_length/double(N1A);
            else                           DXlabel = axis_lengthN/double(N1A);

            char CHCODED[8];
            int NEXE  = 0;
            bool FLEXE = false;
            if (!OptionText && !OptionTime) {

               // We have to decide what format to generate
               // for numeric labels only)
               // Test the magnitude, decide format
               FLEXE  = false;
               NEXE   = 0;
               bool FLEXPO = false;
               bool FLEXNE = false;
               WW  = MAXIMUM(TMath_Abs(wmin),TMath_Abs(wmax));

               // First case : (wmax-wmin)/N1A less than 0.001
               // 0.001 fMaxDigits of 5 (fMaxDigits) characters). 
               //  Then we use x 10 n
               // format. If AF >=0 x10 n cannot be used
               double xmicros = 0.00099;
               if (maxDigits) xmicros = ::pow(10.,-maxDigits);
               if (!noExponent && (TMath_Abs(wmax-wmin)/double(N1A)) < xmicros) {
                  AF    = ::log10(WW) + epsilon;
                  if (AF < 0) {
                     FLEXE   = true;
                     NEXE    = int(AF);
                     int IEXE  = TMath_Abs(NEXE);
                     if (IEXE%3 == 1)     IEXE += 2;
                     else if(IEXE%3 == 2) IEXE += 1;
                     if (NEXE < 0) NEXE = -IEXE;
                     else          NEXE =  IEXE;
                     Wlabel  = Wlabel*::pow(10.,IEXE);
                     DWlabel = DWlabel*::pow(10.,IEXE);
                     IF1     = maxDigits;
                     IF2     = maxDigits-2;
                     goto L110;
                  }
               }
               if (WW >= 1) AF = ::log10(WW);
               else         AF = ::log10(WW*0.0001);
               AF += epsilon;
               NF  = int(AF)+1;
               if (!noExponent && NF > maxDigits)  FLEXPO = true;
               if (!noExponent && NF < -maxDigits) FLEXNE = true;

               // Use x 10 n format. (only powers of 3 allowed)

               if (FLEXPO) {
                  FLEXE = true;
                  while (1) {
                     NEXE++;
                     WW      /= 10;
                     Wlabel  /= 10;
                     DWlabel /= 10;
                     if (NEXE%3 == 0 && WW <= ::pow(10.,maxDigits-1)) break;
                  }
               }

               if (FLEXNE) {
                  FLEXE = true;
                  RNE   = 1/::pow(10.,maxDigits-2);
                  while (1) {
                     NEXE--;
                     WW      *= 10;
                     Wlabel  *= 10;
                     DWlabel *= 10;
                     if (NEXE%3 == 0 && WW >= RNE) break;
                  }
               }

               NA = 0;
               for (i=maxDigits-1; i>0; i--) {
                  if (TMath_Abs(WW) < ::pow(10.,i)) NA = maxDigits-i;
               }
               ndyn = N1A;
               while (ndyn) {
                  double wdyn = TMath_Abs((wmax-wmin)/ndyn);
                  if (wdyn <= 0.999 && NA < maxDigits-2) {
                     NA++;
                     ndyn /= 10;
                  }
                  else break;
               }

               IF2 = NA;
               IF1 = MAXIMUM(NF+NA,maxDigits)+1;
L110:
               if (MINIMUM(wmin,wmax) < 0)IF1 = IF1+1;
               IF1 = MINIMUM(IF1,32);

               // In some cases, IF1 and IF2 are too small....
               while (DWlabel < ::pow(10.,-IF2)) {
                  IF1++;
                  IF2++;
               }
               char* CODED = &CHCODED[0];
               if (IF1 > 14) IF1=14;
               if (IF2 > 14) IF2=14;
               if (IF2) sprintf(CODED,"%%%d.%df",IF1,IF2);
               else     sprintf(CODED,"%%%d.%df",IF1+1,1);
            }

            // We draw labels

            sprintf(CHTEMP,"%g",DWlabel);
            int ndecimals = 0;
            if (OptionDecimals) {
               char *dot = strchr(CHTEMP,'.');
               if (dot) ndecimals = CHTEMP + strlen(CHTEMP) -dot;
            }
            int Nlabels;
            if (OptionM) Nlabels = N1A-1;
            else         Nlabels = N1A;
            double wTimeIni = Wlabel;
            for ( k=0; k<=Nlabels; k++) {
               /*FIXME if (fFunction) {
                  double xx = BinLow+double(k*NN2)*DXtick;
                  double zz = fFunction->Eval(xx)-rwmi;
                  Wlabel = xx;
                  Xlabel = zz* axis_length / TMath_Abs(rwma-rwmi);
               } else */{
                  Xlabel = DXlabel*k;
               }
               if (OptionM)    Xlabel += 0.5*DXlabel;

               if (!OptionText && !OptionTime) {
                  sprintf(LABEL,&CHCODED[0],Wlabel);
                  LABEL[28] = 0;
                  Wlabel += DWlabel;

                  TGaxis_LabelsLimits(LABEL,first,last);  //Eliminate blanks

                  if (LABEL[first] == '.') { //check if '.' is preceeded by a digit
                     strcpy(CHTEMP, "0");
                     strcat(CHTEMP, &LABEL[first]);
                     strcpy(LABEL, CHTEMP);
                     first = 1; last = strlen(LABEL);
                  }
                  if (LABEL[first] == '-' && LABEL[first+1] == '.') {
                     strcpy(CHTEMP, "-0");
                     strcat(CHTEMP, &LABEL[first+1]);
                     strcpy(LABEL, CHTEMP);
                     first = 1; last = strlen(LABEL);
                  }

                  // We eliminate the non significant 0 after '.'
                  if (ndecimals) {
                     char *adot = strchr(LABEL,'.');
                     if (adot) adot[ndecimals] = 0;
                  } else {
                     while (LABEL[last] == '0') { LABEL[last] = 0; last--;}
                  }
                  // We eliminate the dot, unless dot is forced.
                  if (LABEL[last] == '.') {
                     if (!OptionDot) { LABEL[last] = 0; last--;}
                  }
               }

               // Generate the time labels

               if (OptionTime) {
                  double timed = Wlabel + (int)(timeoffset) - rangeOffset;
                  time_t timelabel = (time_t)((long)(timed));
                  struct tm* utctis;
                  if (OptionTime == 1) {
                     utctis = localtime(&timelabel);
                  } else {
                     utctis = gmtime(&timelabel);
                  }
                  std::string timeformattmp;
                  if (timeformat.size() < 220) timeformattmp = timeformat;
                  else timeformattmp = "#splitline{Format}{too long}";

                  // Appends fractionnal part if seconds displayed
                  if (DWlabel<0.9) {
                     double tmpdb;
                     int tmplast;
                     sprintf(LABEL,"%%S%7.5f",modf(timed,&tmpdb));
                     tmplast = strlen(LABEL)-1;

                     // We eliminate the non significiant 0 after '.'
                     while (LABEL[tmplast] == '0') {
                        LABEL[tmplast] = 0; tmplast--;
                     }

                     //FIXME timeformattmp.ReplaceAll("%S",LABEL);
                     // Replace the "0." at the begining by "s"
                     //FIXME timeformattmp.ReplaceAll("%S0.","%Ss");

                  }

                  strftime(LABEL,256,timeformattmp.c_str(),utctis);
                  strcpy(CHTEMP,&LABEL[0]);
                  first = 0; last=strlen(LABEL)-1;
                  Wlabel = wTimeIni + (k+1)*DWlabel;
               }

               // We generate labels (numeric or alphanumeric).

               if (OptionNoopt && !OptionInt)
                        TGaxis_Rotate (Xlabel,Ylabel,cosphi,sinphi,X0,Y0,XX,YY);
               else     TGaxis_Rotate (Xlabel,Ylabel,cosphi,sinphi,XX0,YY0,XX,YY);
               if (Y0 == Y1 && !OptionDown && !OptionUp) {
                  YY -= 0.80*charheight;
               }
               if (OptionVert) {
                  if (X0 != X1 && Y0 != Y1) {
                     if (OptionNoopt && !OptionInt)
                           TGaxis_Rotate (Xlabel,0,cosphi,sinphi,X0,Y0,XX,YY);
                     else  TGaxis_Rotate (Xlabel,0,cosphi,sinphi,XX0,YY0,XX,YY);
                     if (cosphi > 0 ) YY += Ylabel;
                     if (cosphi < 0 ) YY -= Ylabel;
                  }
               }
               if (!OptionY || (X0 == X1)) {
                  if (!OptionText) {
                     if (first > last)  strcpy(CHTEMP, " ");
                     else               strcpy(CHTEMP, &LABEL[first]);
                     aTexts.push_back(SbAxisHPLOT_Text(XX,YY,
                                                       0,textSize,CHTEMP,
                                                       textAlign));
                  }
                  else  {
                     if (OptionText == 1) {
                       printf("debug : SbAxisHPLOT : texts : dummy : 006\n");
                       /*textaxis->PaintLatex
                         (gPad->GetX1() + XX*(gPad->GetX2() - gPad->GetX1()),
                          gPad->GetY1() + YY*(gPad->GetY2() - gPad->GetY1()),
                          0,
                          textaxis->GetTextSize(),
                          fAxis->GetBinLabel(k+fAxis->GetFirst()));*/
                     }
                  }
               }
               else {

                  // Text alignment is down
                  int LNLEN = 0;
                  if (!OptionText)     LNLEN = last-first+1;
                  else {
                     int NHILAB = 0;
                     if (k+1 > NHILAB) LNLEN = 0;
                  }
                  for ( l=1; l<=LNLEN; l++) {
                     if (!OptionText) *CHTEMP = LABEL[first+l-2];
                     else {
                        if (LNLEN == 0) strcpy(CHTEMP, " ");
                        else            strcpy(CHTEMP, "1");
                     }
                     aTexts.push_back(SbAxisHPLOT_Text(XX,YY,
                                                       0,textSize,CHTEMP,
                                                       textAlign));
                     YY -= charheight*1.3;
                  }
               }
            }

            // We use the format x 10 ** n

            if (FLEXE && !OptionText && NEXE)  {
 	       //G.Barrand sprintf(LABEL,"#times10^{%d}", NEXE);
 	       sprintf(LABEL,"x10^%d!", NEXE); //G.Barrand : PAW encoding
               double Xfactor, Yfactor;
               if (X0 != X1) { Xfactor = X1-X0+0.1*charheight; Yfactor = 0; }
               else          { Xfactor = Y1-Y0+0.1*charheight; Yfactor = 0; }
               TGaxis_Rotate (Xfactor,Yfactor,cosphi,sinphi,X0,Y0,XX,YY);
               textAlign = 11;
               aTexts.push_back(SbAxisHPLOT_Text(XX,YY,
                                                 0,textSize,LABEL,
                                                 textAlign));
            }
         }
      }
   }

   // Log axis

   if (OptionLog && ndiv) {
      unsigned int xi1=0,xi2 = 0,wi = 0,yi1=0,yi2,hi = 0;
      bool firstintlab = true, overlap = false;
      if ((wmin == wmax) || (ndiv == 0))  {
         Error("PaintAxis", "wmin (%f) == wmax (%f), or ndiv == 0", wmin, wmax);
         return; //goto L210;
      }
      if (wmin <= 0)   {
         Error("PaintAxis", "negative logarithmic axis");
         return; //goto L210;
      }
      if (wmax <= 0)     {
         Error("PaintAxis", "negative logarithmic axis");
         return; //goto L210;
      }
      double XMNLOG = ::log10(wmin);
      if (XMNLOG > 0) XMNLOG += 1.E-6;
      else            XMNLOG -= 1.E-6;
      double X00    = 0;
      double X11    = axis_length;
      double H2     = ::log10(wmax);
      double H2SAV  = H2;
      if (H2 > 0) H2 += 1.E-6;
      else        H2 -= 1.E-6;
      int IH1    = int(XMNLOG);
      int IH2    = 1+int(H2);
      int NBININ = IH2-IH1+1;
      double AXMUL  = (X11-X00)/(H2SAV-XMNLOG);

      // Plot decade and intermediate tick marks
      decade      = IH1-2;
      int labelnumber = IH1;
      if ( XMNLOG > 0 && (XMNLOG-double(IH1) > 0) ) labelnumber++;
      for (j=1; j<=NBININ; j++) {

         // Plot decade
         firstintlab = true, overlap = false;
         decade++;
         if (X0 == X1 && j == 1) Ylabel += charheight*0.33;
         if (Y0 == Y1 && j == 1) Ylabel -= charheight*0.65;
         double Xone = X00+AXMUL*(double(decade)-XMNLOG);
         //the following statement is a trick to circumvent a gcc bug
         if (j < 0) printf("j=%d\n",j);
         if (X00 > Xone) goto L160;
         if (Xone > X11) break;
         Xtwo = Xone;
         Y    = 0;
         if (!Mside) Y -= atick[0];
         TGaxis_Rotate(Xone,Y,cosphi,sinphi,X0,Y0,xpl2,ypl2);
         TGaxis_Rotate(Xtwo,atick[0],cosphi,sinphi,X0,Y0,xpl1,ypl1);
         if (OptionVert) {
            if ((X0 != X1) && (Y0 != Y1)) {
               if (Mside) {
                  xpl1=xpl2;
                  if (cosphi > 0) ypl1 = ypl2 + atick[0];
                  else            ypl1 = ypl2 - atick[0];
               }
               else {
                  xpl1 = 0.5*(xpl1 + xpl2);
                  xpl2 = xpl1;
                  ypl1 = 0.5*(ypl1 + ypl2) + atick[0];
                  ypl2 = 0.5*(ypl1 + ypl2) - atick[0];
               }
            }
         }
         if (!drawGridOnly) {
           aLinesAxis.push_back
             (std::pair<SbVec3f,SbVec3f>
                (SbVec3f((float)xpl1,(float)ypl1,0),
                 SbVec3f((float)xpl2,(float)ypl2,0)));
         }

         if (OptionGrid) {
           TGaxis_Rotate(Xone,0,cosphi,sinphi,X0,Y0,xpl2,ypl2);
           TGaxis_Rotate(Xone,grid_side*gridlength,cosphi,sinphi,X0,Y0,
                         xpl1,ypl1);
           aLinesGrid.push_back
             (std::pair<SbVec3f,SbVec3f>
                (SbVec3f((float)xpl1,(float)ypl1,0),
                 SbVec3f((float)xpl2,(float)ypl2,0)));
         }

         if (!drawGridOnly && !OptionUnlab)  {

            // We generate labels (numeric only).
            if (noExponent) {
               double rlab = ::pow(10.,labelnumber);
               sprintf(LABEL, "%f", rlab);
               TGaxis_LabelsLimits(LABEL,first,last);
               while (last > first) {
                  if (LABEL[last] != '0') break;
                  LABEL[last] = 0;
                  last--;
               }
               if (LABEL[last] == '.') {LABEL[last] = 0; last--;}
            } else {
               sprintf(LABEL, "%d", labelnumber);
               TGaxis_LabelsLimits(LABEL,first,last);
            }
            TGaxis_Rotate (Xone,Ylabel,cosphi,sinphi,X0,Y0,XX,YY);
            if ((X0 == X1) && !OptionPara) {
               if (Lside < 0) {
                  if (Mside < 0) {
                     if (labelnumber == 0) NCH=1;
                     else                  NCH=2;
                     XX    += NCH*charheight;
                  } else {
                     if (labelnumber >= 0) XX    += 0.25*charheight;
                     else                  XX    += 0.50*charheight;
                  }
               }
               XX += 0.25*charheight;
            }
            if ((Y0 == Y1) && !OptionDown && !OptionUp) {
               if (noExponent) YY += 0.33*charheight;
            }
            if (N1A == 0) return; //goto L210;
            int KMOD = NBININ/N1A;
            if (KMOD == 0) KMOD=1000000;
            if ((NBININ <= N1A) || (j == 1) || (j == NBININ) || ((NBININ > N1A)
            && (j%KMOD == 0))) {
               if (labelnumber == 0) {
                  aTexts.push_back(SbAxisHPLOT_Text(XX,YY,
                                                    0,textSize,"1",
                                                    textAlign));
               } else if (labelnumber == 1) {
                  aTexts.push_back(SbAxisHPLOT_Text(XX,YY,
                                                    0,textSize,"10",
                                                    textAlign));
               } else {
                  if (noExponent) {
                    printf("debug : SbAxisHPLOT : texts : FIXME : 003\n");
                    //FIXME textaxis->PaintTextNDC(XX,YY,&LABEL[first]);
                  } else {
                    //FIXME : support ROOT Latex encoding ?
                    //  sprintf(CHTEMP, "10^{%d}", labelnumber);
                    sprintf(CHTEMP, "10^%d?", labelnumber); //PAW encoding.
                    aTexts.push_back(SbAxisHPLOT_Text(XX,YY,
                                                      0,textSize,CHTEMP,
                                                      textAlign));
                  }
               }
            }
            labelnumber++;
         }
L160:
         for (k=2;k<10;k++) {

            // Plot intermediate tick marks
            double Xone = X00+AXMUL*(::log10(double(k))+double(decade)-XMNLOG);
            if (X00 > Xone) continue;
            if (Xone > X11) goto L200;
            Y = 0;
            if (!Mside)  Y -= atick[1];
            Xtwo = Xone;
            TGaxis_Rotate(Xone,Y,cosphi,sinphi,X0,Y0,xpl2,ypl2);
            TGaxis_Rotate(Xtwo,atick[1],cosphi,sinphi,X0,Y0,xpl1,ypl1);
            if (OptionVert) {
               if ((X0 != X1) && (Y0 != Y1)) {
                  if (Mside) {
                     xpl1 = xpl2;
                     if (cosphi > 0) ypl1 = ypl2 + atick[1];
                     else            ypl1 = ypl2 - atick[1];
                  }
                  else {
                     xpl1 = 0.5*(xpl1+xpl2);
                     xpl2 = xpl1;
                     ypl1 = 0.5*(ypl1+ypl2) + atick[1];
                     ypl2 = 0.5*(ypl1+ypl2) - atick[1];
                  }
               }
            }
            int IDN = N1A*2;
            if ((NBININ <= IDN) || ((NBININ > IDN) && (k == 5))) {
               if(!drawGridOnly) {
                 aLinesAxis.push_back
                   (std::pair<SbVec3f,SbVec3f>
                      (SbVec3f((float)xpl1,(float)ypl1,0),
                       SbVec3f((float)xpl2,(float)ypl2,0)));
               }

               // Draw the intermediate LOG labels if requested

               if (MoreLogLabels && !OptionUnlab && 
                   !drawGridOnly && !overlap) {
                  if (noExponent) {
                     double rlab = double(k)*::pow(10.,labelnumber-1);
                     sprintf(CHTEMP, "%g", rlab);
                  } else {
                     if (labelnumber-1 == 0) {
                        sprintf(CHTEMP, "%d", k);
                     } else if (labelnumber-1 == 1) {
                        sprintf(CHTEMP, "%d", 10*k);
                     } else {
		        //G.Barrand :
                        //sprintf(CHTEMP, "%d#times10^{%d}", k, labelnumber-1);
	                sprintf(CHTEMP,"%dx10^%d!",k,labelnumber-1);//G.Barrand
                     }
                  }
                  TGaxis_Rotate (Xone,Ylabel,cosphi,sinphi,X0,Y0,XX,YY);
                  if ((Y0 == Y1) && !OptionDown && !OptionUp) {
                     if (noExponent) YY += 0.33*charheight;
                  }
                  if (X0 == X1) XX += 0.25*charheight;
                  if (OptionVert) {
                     if ((X0 != X1) && (Y0 != Y1)) {
                        TGaxis_Rotate(Xone,Ylabel,cosphi,sinphi,X0,Y0,XX,YY);
                        if (cosphi > 0) YY += Ylabel;
                        else            YY -= Ylabel;
                     }
                  }
                  //FIXME textaxis->SetTitle(CHTEMP);
                  double u = XX;
                  double v = YY;
                  if (firstintlab) {
                     //FIXME textaxis->GetBoundingBox(wi, hi); wi=(Uint)(wi*1.3); hi*=(Uint)(hi*1.3);
                     xi1 = 0;//FIXME gPad->XtoAbsPixel(u);
                     yi1 = 0;//FIXME gPad->YtoAbsPixel(v);
                     firstintlab = false;
                     printf("debug : SbAxisHPLOT : texts : dummy : 010\n");
                     aTexts.push_back(SbAxisHPLOT_Text(u,v,
                                                       0,textSize,CHTEMP,
                                                       textAlign));
                  } else {
                     xi2 = 0;//FIXME gPad->XtoAbsPixel(u);
                     yi2 = 0;//FIXME gPad->YtoAbsPixel(v);
                     if ((X0 == X1 && yi1-hi <= yi2) || (Y0 == Y1 && xi1+wi >= xi2)){
                        overlap = true;
                     } else {
                        xi1 = xi2;
                        yi1 = yi2;
                        //FIXME textaxis->GetBoundingBox(wi, hi); wi=(Uint)(wi*1.3); hi*=(Uint)(hi*1.3);
                        printf("debug : SbAxisHPLOT : texts : dummy : 011\n");
                        aTexts.push_back(SbAxisHPLOT_Text(u,v,
                                                          0,textSize,CHTEMP,
                                                          textAlign));
                     }
                  }
               }

               // Draw the intermediate LOG grid if only three 
               // decades are requested
               if (OptionGrid && NBININ <= 5 && ndiv > 100) {
                 TGaxis_Rotate(Xone,0,cosphi,sinphi,X0,Y0,xpl2, ypl2);
                 TGaxis_Rotate
                   (Xone,grid_side*gridlength,cosphi,sinphi,X0,Y0,xpl1,ypl1);
                 aLinesGrid.push_back
                   (std::pair<SbVec3f,SbVec3f>
                      (SbVec3f((float)xpl1,(float)ypl1,0),
                       SbVec3f((float)xpl2,(float)ypl2,0)));
               }
            }  //endif ((NBININ <= IDN) ||
         }  //endfor (k=2;k<10;k++)
      } //endfor (j=1; j<=NBININ; j++)
L200:
      int kuku=0; if (kuku) { }
   }  //endif (OptionLog && ndiv)


//L210:
}

/*
//______________________________________________________________________________
void TGaxis_SetDecimals(bool dot)
{
// Set the Decimals flag
// By default, blank characters are stripped, and then the
// label is correctly aligned. The dot, if last character of the string,
// is also stripped, unless this option is specified.
// One can disable the option by calling axis.SetDecimals(true).
// Note the bit is set in fBits (as opposed to fBits2 in TAxis!)

   if (dot) SetBit(TAxis_kDecimals);
   else     ResetBit(TAxis_kDecimals);
}

//______________________________________________________________________________
void TGaxis_SetMaxDigits(int maxd)
{
   // static function to set fMaxDigits for axis with the bin content
   // (y axis for 1-d histogram, z axis for 2-d histogram)
   //fMaxDigits is the maximum number of digits permitted for the axis
   //labels above which the notation with 10^N is used.
   //For example, to accept 6 digits number like 900000 on an axis
   //call TGaxis::SetMaxDigits(6). The default value is 5.
   //fMaxDigits must be greater than 0.

   fMaxDigits = maxd;
   if (maxd < 1) fMaxDigits = 1;
}

void TGaxis_SetMoreLogLabels(bool more)
{
// Set the kMoreLogLabels bit flag
// When this option is selected more labels are drawn when in log scale
// and there is a small number of decades  (<3).
// Note that this option is automatically inherited from TAxis

   if (more) SetBit(TAxis_kMoreLogLabels);
   else      ResetBit(TAxis_kMoreLogLabels);
}
void TGaxis_SetNoExponent(bool noExponent)
{
// Set the NoExponent flag
// By default, an exponent of the form 10^N is used when the label values
// are either all very small or very large.
// One can disable the exponent by calling axis.SetNoExponent(true).

   if (noExponent) SetBit(TAxis_kNoExponent);
   else            ResetBit(TAxis_kNoExponent);
}
void TGaxis_SetOption(const std::string& option)
{
   fCHOPT = option;
}
*/

void SbAxisHPLOT::setTimeFormat(const std::string& a_format)
// Change the format used for time plotting
// ========================================
//  The format string for date and time use the same options as the one used
//  in the standard strftime C function, i.e. :
//    for date :
//      %a abbreviated weekday name
//      %b abbreviated month name
//      %d day of the month (01-31)
//      %m month (01-12)
//      %y year without century
//
//    for time :
//      %H hour (24-hour clock)
//      %I hour (12-hour clock)
//      %p local equivalent of AM or PM
//      %M minute (00-59)
//      %S seconds (00-61)
//      %% %
//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
   if (a_format.find("%F")!=std::string::npos || !a_format.size()) {
      fTimeFormat = a_format;
      //printf("debug : SbAxisHPLOT::setTimeFormat : 000 : \"%s\"\n",
      //  fTimeFormat.c_str());
      return;
   }

   std::string::size_type IdF = fTimeFormat.find("%F");
   if (IdF!=std::string::npos) {
      int LnF = fTimeFormat.size();
      std::string stringtimeoffset = fTimeFormat.substr(IdF,LnF-IdF);
      fTimeFormat = a_format;
      fTimeFormat += stringtimeoffset;
      //printf("debug : SbAxisHPLOT::setTimeFormat : 001 : \"%s\"\n",
      //  fTimeFormat.c_str());
   } else {
      fTimeFormat = a_format;

      // In CERN-ROOT :
      //SetTimeOffset(gStyle->GetTimeOffset());
      //TAxis::fTimeOffset = 788918400; // UTC time at 01/01/95
      //double UTC_time_1995_01_01__00_00_00 = 788918400; //CERN-ROOT
      //setTimeOffset(UTC_time_1995_01_01__00_00_00);

      //Be consistent with SoAxis::timeOffset being 0.
      double UTC_time_1970_01_01__00_00_00 = 0; //UNIX
      setTimeOffset(UTC_time_1970_01_01__00_00_00);

      //printf("debug : SbAxisHPLOT::setTimeFormat : 002 : \"%s\"\n",
      //  fTimeFormat.c_str());
   }
}

void SbAxisHPLOT::setTimeOffset(double toffset,bool a_is_gmt) {
   // Change the time offset

   char tmp[20];
   time_t timeoff;
   struct tm* utctis;
   std::string::size_type IdF = fTimeFormat.find("%F");
   if (IdF!=std::string::npos) {
     fTimeFormat = fTimeFormat.substr(0,IdF);
   }
   fTimeFormat += "%F";

   timeoff = (time_t)((long)(toffset));
   utctis = gmtime(&timeoff);

   strftime(tmp,256,"%Y-%m-%d %H:%M:%S",utctis);
   fTimeFormat += tmp;

   // append the decimal part of the time offset
   double ds = toffset-(int)toffset;
   if(ds!= 0) {
      sprintf(tmp,"s%g",ds);
      fTimeFormat += tmp;
   }

   // If the time is GMT, stamp fTimeFormat
   if (a_is_gmt) fTimeFormat += " GMT";

   //printf("debug : SbAxisHPLOT::setTimeOffset : \"%s\"\n",
   //  fTimeFormat.c_str());
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SbAxisHPLOT::optimizeLimits(
 double A1,double A2,int nold
,double &BinLow, double &BinHigh
,int &nbins, double &BinWidth
,const std::string& aCHOPT
){
// static function to compute reasonable axis limits
//
// Input parameters:
//
//  A1,A2 : Old WMIN,WMAX .
//  BinLow,BinHigh : New WMIN,WMAX .
//  nold   : Old NDIV .
//  nbins    : New NDIV .

   int lwid, kwid;
   int ntemp = 0;
   int jlog  = 0;
   double siground = 0;
   double alb, awidth, sigfig;
   double timemulti = 1;
   int roundmode =0;

   int OptionTime;
   SETOPT('t',OptionTime)

   double AL = MINIMUM(A1,A2);
   double AH = MAXIMUM(A1,A2);
   if (AL == AH) AH = AL+1;
   // if nold  ==  -1 , program uses binwidth input from calling routine
   if (nold == -1 && BinWidth > 0 ) goto L90;
   ntemp = MAXIMUM(nold,2);
   if (ntemp < 1) ntemp = 1;

L20:
   awidth = (AH-AL)/double(ntemp);
   timemulti = 1;
   if (awidth >= FLT_MAX) goto LOK;  //in float.h
   if (awidth <= 0)       goto LOK;

//      If time representation, bin width should be rounded to seconds
//      minutes, hours or days

   if (OptionTime && awidth>=60) { // if width in seconds, treat it as normal
      //   width in minutes
      awidth /= 60; timemulti *=60;
      roundmode = 1; // round minutes (60)
      //   width in hours ?
      if (awidth>=60) {
         awidth /= 60; timemulti *= 60;
         roundmode = 2; // round hours (24)
         //   width in days ?
         if (awidth>=24) {
            awidth /= 24; timemulti *= 24;
            roundmode = 3; // round days (30)
            //   width in months ?
            if (awidth>=30.43685) { // Mean month length in 1900.
               awidth /= 30.43685; timemulti *= 30.43685;
               roundmode = 2; // round months (12)
               //   width in years ?
               if (awidth>=12) {
                  awidth /= 12; timemulti *= 12;
                  roundmode = 0; // round years (10)
               }
            }
         }
      }
   }
//      Get nominal bin width in exponential form

   jlog   = int(::log10(awidth));
   if (jlog <-200 || jlog > 200) {
      BinLow   = 0;
      BinHigh  = 1;
      BinWidth = 0.01;
      nbins    = 100;
      return;
   }
   if (awidth <= 1 && (!OptionTime || timemulti==1) ) jlog--;
   sigfig = awidth* ::pow(10.,-jlog) -1e-10;
   //in the above statement, it is important to substract 1e-10
   //to avoid precision problems if the tests below
   
//      Round mantissa

   switch (roundmode) {

//      Round mantissa up to 1, 1.5, 2, 3, or 6 in case of minutes
      case 1: // case 60
         if      (sigfig <= 1)    siground = 1;
         else if (sigfig <= 1.5 && jlog==1)    siground = 1.5;
         else if (sigfig <= 2)    siground = 2;
         else if (sigfig <= 3 && jlog ==1)    siground = 3;
         else if (sigfig <= 5 && sigfig>3 && jlog ==0) siground = 5; //added (Damir in 3.10/02)
         else if (jlog==0)        {siground = 1; jlog++;}
         else                     siground = 6;
         break;
      case 2: // case 12 and 24

//      Round mantissa up to 1, 1.2, 2, 2.4, 3 or 6 in case of hours or months
         if      (sigfig <= 1 && jlog==0)    siground = 1;
         else if (sigfig <= 1.2 && jlog==1)    siground = 1.2;
         else if (sigfig <= 2 && jlog==0)    siground = 2;
         else if (sigfig <= 2.4 && jlog==1)    siground = 2.4;
         else if (sigfig <= 3)    siground = 3;
         else if (sigfig <= 6)    siground = 6;
         else if (jlog==0)        siground = 12;
         else                     siground = 2.4;
         break;

//-      Round mantissa up to 1, 1.4, 2, or 7 in case of days (weeks)
      case 3: // case 30
         if      (sigfig <= 1 && jlog==0)    siground = 1;
         else if (sigfig <= 1.4 && jlog==1)    siground = 1.4;
         else if (sigfig <= 3 && jlog ==1)    siground = 3;
         else                     siground = 7;
         break;
      default :
      
//      Round mantissa up to 1, 2, 2.5, 5, or 10 in case of decimal number
         if      (sigfig <= 1)    siground = 1;
         else if (sigfig <= 2)    siground = 2;
         else if (sigfig <= 5 && (!OptionTime || jlog<1))  siground = 5;
         else if (sigfig <= 6 && OptionTime && jlog==1)    siground = 6;
         else                    {siground = 1;   jlog++; }
         break;
   }

   BinWidth = siground* ::pow(10.,jlog);
   if (OptionTime) BinWidth *= timemulti;

//      Get new bounds from new width BinWidth

L90:
   alb  = AL/BinWidth;
   if (TMath_Abs(alb) > 1e9) {
      BinLow  = AL;
      BinHigh = AH;
      if (nbins > 10*nold && nbins > 10000) nbins = nold;
      return;
   }
   lwid   = int(alb);
   if (alb < 0) lwid--;
   BinLow     = BinWidth*double(lwid);
   alb        = AH/BinWidth + 1.00001;
   kwid = int(alb);
   if (alb < 0) kwid--;
   BinHigh = BinWidth*double(kwid);
   nbins = kwid - lwid;
   if (nold == -1) goto LOK;
   if (nold <= 5) {          //    Request for one bin is difficult case
      if (nold > 1 || nbins == 1)goto LOK;
      BinWidth = BinWidth*2;
      nbins    = 1;
      goto LOK;
   }
   if (2*nbins == nold && !OptionTime) {ntemp++; goto L20; }

LOK:
   double oldBinLow = BinLow;
   double oldBinHigh = BinHigh;
   int oldnbins = nbins;

   double atest = BinWidth*0.0001;
   //if (TMath_Abs(BinLow-A1)  >= atest) { BinLow  += BinWidth;  nbins--; } //replaced by Damir in 3.10/02
   //if (TMath_Abs(BinHigh-A2) >= atest) { BinHigh -= BinWidth;  nbins--; } //by the next two lines
   if (AL-BinLow  >= atest) { BinLow  += BinWidth;  nbins--; }
   if (BinHigh-AH >= atest) { BinHigh -= BinWidth;  nbins--; }
   if (!OptionTime && BinLow >= BinHigh) {
      //this case may happen when nbins <=5
      BinLow = oldBinLow;
      BinHigh = oldBinHigh;
      nbins = oldnbins;
   }
   else if (OptionTime && BinLow>=BinHigh) {
      nbins = 2*oldnbins;
      BinHigh = oldBinHigh;
      BinLow = oldBinLow;
      BinWidth = (oldBinHigh - oldBinLow)/nbins;
      double atest = BinWidth*0.0001;
      if (AL-BinLow  >= atest) { BinLow  += BinWidth;  nbins--; }
      if (BinHigh-AH >= atest) { BinHigh -= BinWidth;  nbins--; }
   }
}

void SbAxisHPLOT::adjustBinSize(
 double A1,double A2,int nold
,double &BinLow, double &BinHigh, int &nbins, double &BinWidth
){
// Axis labels optimisation
// ========================
//
//   This routine adjusts the bining of the axis
//   in order to have integer values for the labels
//
// _Input parameters:
//
//  A1,A2    : Old WMIN,WMAX .
//  BinLow,BinHigh : New WMIN,WMAX .
//  nold     : Old NDIV (primary divisions)
//  nbins    : New NDIV .
//
   BinWidth = TMath_Abs(A2-A1)/double(nold);
   if (BinWidth <= 1) { BinWidth = 1; BinLow = int(A1); }
   else {
      int width = int(BinWidth/5) + 1;
      BinWidth = 5*width;
      BinLow   = int(A1/BinWidth)*BinWidth;

//     We determine BinLow to have one tick mark at 0
//     if there are negative labels.

      if (A1 < 0) {
         for (int ic=0; ic<1000; ic++) {
            double rbl = BinLow/BinWidth;
            int   ibl = int(BinLow/BinWidth);
            if ( (rbl-ibl) == 0 || ic > width) { BinLow -= 5; break;}
         }
      }
   }
   BinHigh     = int(A2);
   nbins       = 0;
   double XB  = BinLow;
   while (XB <= BinHigh) {
      XB += BinWidth;
      nbins++;
   }
   BinHigh = XB - BinWidth;
}
void SbAxisHPLOT::setTickSize(float aValue) { fTickSize = aValue;}
void SbAxisHPLOT::setLabelOffset(float aValue) { fLabelOffset = aValue;}
void SbAxisHPLOT::setLabelSize(float aValue) { fLabelSize = aValue;}
void SbAxisHPLOT::setTitleOffset(float aValue) { fTitleOffset = aValue;}
void SbAxisHPLOT::setTitleSize(float aValue) { fTitleSize = aValue;}
