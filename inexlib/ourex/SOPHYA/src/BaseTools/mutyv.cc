// Classe MuTyV : Variable multi-type numerique
//                  R. Ansari  1997-2000
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA


#include "sopnamsp.h"
#include "mutyv.h"
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string.h>

/*!
   \class SOPHYA::MuTyV
   \ingroup BaseTools
   Simple utility class which can be used to hold values of type
   string, integer (\b int_8), float (\b r_8), complex and 
   date/time (TimeStamp). Date/time values are kept internally 
   as double precision values and are thus less precise than 
   TimeStamp objects. 
   It provides also easy conversion methods between these types. 

   \sa SOPHYA::TimeStamp

   \code
   //  ------- Using MuTyV objects ------- 
   MuTyV mvu;         // MuTyV variable declaration 
   mvu = 60;          // mvu contains the integer value 60
   mvu = 66.6;        // and now the double value 66.6 
   string ds = mvu;   // ds contains the string "66.6"
   MuTyV mvi(14);     // New MuTyV variable containing integer value 14
   r_4 x = mvi;       // x has the value 14.0
   MuTyV mvs("Bonjour !");  // mvs contains the string "Bonjour !"
   string s = mvs;          // s contains "Bonjour !"
   TimeStamp ts;      // Current date/time
   MuTyV mvt = ts;
   s = mvt;           // s contains the current date in string format
   \endcode
*/


static void mutyv_decodestr(string const &, double& r, double& im);
static inline void mutyv_decodestr_cp(const char * si, double& r, double& im)
{
  r = im = 0.;
  string s = si;
  mutyv_decodestr(s, r, im);
}

/* --Methode-- */
MuTyV::MuTyV(MuTyV const & a)
{
  typ = a.typ;  iv = a.iv;  dv = a.dv;  dv_im = a.dv_im; 
  if (typ == MTVString)  strv = new string(*(a.strv));
  else strv = NULL;
}

/* --Methode-- */
MuTyV::~MuTyV()
{
  if (strv) delete strv;
}

/* --Methode-- */
MuTyV::MuTyV(char const* s)
{
  typ = MTVString;
  strv = new string(s);
  mutyv_decodestr(s, dv, dv_im);
  iv = (int_8)dv;
}

/* --Methode-- */
MuTyV::MuTyV(string const& s)
{
  typ = MTVString;
  strv = new string(s);
  mutyv_decodestr(s.c_str(), dv, dv_im);
  iv = (int_8)dv;
}

/* --Methode-- */
MuTyV::MuTyV(TimeStamp const& ts)
{
  typ = MTVTimeStamp;
  dv = ts.ToDays();
  dv_im = 0.;
  iv = (int_8)dv;
  strv = NULL;
}

/* --Methode-- */
MuTyV & MuTyV::operator= (MuTyV const & a)
{
  typ = a.typ;  iv = a.iv;  dv = a.dv;  dv_im = a.dv_im; 
  if (typ == MTVString)  { 
    if (strv) *strv = *(a.strv);  
    else strv = new string(*(a.strv));
  }
  return(*this);
}

/* --Methode-- */
const char * MuTyV::operator= (const char* s)
{
  typ = MTVString;
  if (strv) *strv = s;  
  else strv = new string(s);
  mutyv_decodestr(s, dv, dv_im);
  iv = (int_8)dv;
  return(s);
}

/* --Methode-- */
string const & MuTyV::operator= (string const& s)
{
  typ = MTVString;
  if (strv) *strv = s;  
  else strv = new string(s);
  mutyv_decodestr(s.c_str(), dv, dv_im);
  iv = (int_8)dv;
  return(s);
}

/* --Methode-- */
TimeStamp const & MuTyV::operator= (TimeStamp const& ts)
{
  typ = MTVTimeStamp;
  dv = ts.ToDays();
  dv_im = 0.;
  iv = (int_8)dv;
  return(ts);
}


/* --Methode-- */
MuTyV::operator string() const 
{
  if (typ == MTVString)  return(*strv);
  else if (typ == MTVTimeStamp) { return TimeStamp(dv).ToString(); }
  else {
    char buff[96];
    if (typ == MTVInteger)  sprintf(buff,"%ld", (long)iv);
    else if (typ == MTVFloat) sprintf(buff,"%g", dv);
    else if (typ == MTVComplex) sprintf(buff,"(%g,%g)", dv, dv_im);
    else buff[0] = '\0';
    return(string(buff));
  }
}

/* --Methode-- */
string & MuTyV::Convert(string & x) const 
{
  if (typ == MTVString)  x = *strv;
  else if (typ == MTVTimeStamp) { x = TimeStamp(dv).ToString(); }
  else {
    char buff[96];
    if (typ == MTVInteger)  sprintf(buff,"%ld", (long)iv);
    else if (typ == MTVFloat) sprintf(buff,"%g", dv);
    else if (typ == MTVComplex) sprintf(buff,"(%g,%g)", dv, dv_im);
    else buff[0] = '\0';
    x = buff;
  }
  return x;
}

/* --Methode-- */
MuTyV::operator TimeStamp() const 
{
  return TimeStamp(dv);
}

/* --Methode-- */
TimeStamp& MuTyV::Convert(TimeStamp& x) const 
{
  x = TimeStamp(dv);
  return x;
}

static void mutyv_decodestr(string const & s, double& r, double& im)
  // decodage d'une chaine contenant une ou deux valeurs
{
  r = im = 0.;
  size_t l = s.length();
  size_t p = s.find_first_not_of(" \t",0);
  size_t q,q2;
  if (p >= l) return;
  if (s[p] == '(') {  // C'est un complexe
    if ((q2=s.find(')',p)) >= l)  return; 
    size_t pz = s.find_first_not_of(" \t",p+1);
    size_t qz = q2;
    if ((q=s.find(',',pz)) < q2) qz = q;
    if (isdigit(s[pz]) || !(s[pz] == '+') || (s[pz] == '-') )  
      r = atof(s.substr(pz,qz-pz).c_str());
    else return;
    if (qz == q) {
      pz = s.find_first_not_of(" \t",qz+1);
      if (isdigit(s[pz]) || (s[pz] == '+') || (s[pz] == '-') )  
	im = atof(s.substr(pz,q2-pz).c_str());      
    }
  }

  q = s.find_first_of(" \t",p+1);
  if (!isdigit(s[p]) && !(s[p] == '+') && !(s[p] == '-') )
    return;
  r = atof(s.substr(p,q-p).c_str());
  im = 0.;
}




