#include "machdefs.h"
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "timestamp.h"
#include "pexceptions.h"
#include <iostream>

namespace SOPHYA {

/*!
   \class TimeStamp
   \ingroup BaseTools
   A simple class for representing date and time. Simple operations 
   on date/time, such as time shift or date/time difference (subtraction 
   operator are also provided. TimeStamp objects represent and store universal 
   date and time (TU), without any reference to the local time.
   The subtraction operator between two TimeStamp, as well as comparison 
   operator are defined.
   
   
   \code
   // Create a object with the current date and time and prints it to cout
   TimeStamp ts; ts.SetNow();
   // Or TimeStamp ts(true);
   cout << ts << endl;
   // Create an object with a specified date and time 
   TimeStamp ts2("01/01/1905","00:00:00");
   // Get the number of days since 0 Jan 1901
   cout << ts2.ToDays() << endl;
   \endcode
*/

#ifdef Linux 
// La fonction trunc non declaree ds math.h sous Linux
extern "C" { double trunc   (double x); }
#endif
//-------------------------------------------------------------------------
//--------------------------  Classe TimeStamp   --------------------------
//-------------------------------------------------------------------------

enum Jour {jour_Lundi=0, jour_Mardi, jour_Mercredi, jour_Jeudi, jour_Vendredi, jour_Samedi, jour_Dimanche};
enum Mois {mois_Janvier=1, mois_Fevrier, mois_Mars, mois_Avril, mois_Mai, mois_Juin, mois_Juillet,
		   mois_Aout, mois_Septembre, mois_Octobre, mois_Novembre, mois_Decembre};

  /*
  //! Day of the week enum
  enum WeekDay {day_Monday=0, day_Tuesday, day_Wednesday, day_Thursday, day_Friday, day_Saturday, day_Sunday};
  */
 
TimeStamp::TimeStamp(bool fgscur)
{
  mSeconds = 0.;
  mDays = 0;
  if (fgscur) SetNow();
}

TimeStamp::TimeStamp(TimeStamp const & ts)
{
  Set(ts);
  mSeconds = ts.mSeconds;
  mDays = ts.mDays;
}

TimeStamp::TimeStamp(int year, int month, int day, int hour, int min, double sec)
{
  SetDate(year, month, day);
  SetHour(hour, min, sec);
}


TimeStamp::TimeStamp(double days)
{
  Set(days);
}

TimeStamp::TimeStamp(int_8 days, r_8 seconds)
{
  Set(days, seconds);
}

TimeStamp::TimeStamp(string const & date, string const & hour)
{
  SetDate(date);
  SetHour(hour);
}

TimeStamp::TimeStamp(const char* date, const char* hour)
{
  SetDate(date);
  SetHour(hour);
}

TimeStamp::TimeStamp(string& datim)
{
  Set(datim);
}

TimeStamp::TimeStamp(const char* datim)
{
  Set(datim);
}

void TimeStamp::Set(TimeStamp const & ts)
{
  mSeconds = ts.mSeconds;
  mDays = ts.mDays;
}

void TimeStamp::Set(double days)
{
  if (days >= 0.) {
    mDays = (int_8)trunc(days);
    mSeconds = (days-trunc(days))*86400.;
  }
  else {
    if ( (trunc(days)-days) > 0.) {
      mDays = (int_8)trunc(days)-1;
      mSeconds = (days-mDays)*86400.;
    }
    else {
      mDays = (int_8)trunc(days);
      mSeconds = 0.;
    }
  }
}

void TimeStamp::Set(int_8 days, r_8 seconds)
{
  if ( (seconds < 0.) || (seconds > 86400.) ) 
    throw ParmError("TimeStamp::Set(int_8, r_8) seconds<0 or seconds>86400.");
  mDays = days;
  mSeconds = seconds;
}

void TimeStamp::Set(const char * datim)
{
  int year, month, day;
  int hour, min;
  double sec;
  sscanf(datim,"%d-%d-%dT%d:%d:%lf", &year, &month, &day,
	 &hour, &min, &sec);
  SetDate(year, month, day);
  SetHour(hour, min, sec);
}


void TimeStamp::SetNow()
{
  time_t t = time(NULL);
  struct tm* TM = gmtime(&t);

  int JJ,MM,AA;
  int hh,mm;
  double ss; 
  
  AA = TM->tm_year + 1900;
  MM = TM->tm_mon+1;
  JJ = TM->tm_mday;
  hh = TM->tm_hour;
  mm = TM->tm_min;
  ss = TM->tm_sec;
  SetDate(AA,MM,JJ);
  SetHour(hh,mm,ss);
}

void TimeStamp::SetDate(int year, int month, int day)
{
  mDays = ConvertToDays(year, month, day);
}

void TimeStamp::SetDate(const char* date)
{
  int day,month,year;
  sscanf(date,"%d/%d/%d", &day, &month, &year);
  mDays = ConvertToDays(year, month, day);
}

void TimeStamp::SetHour(int hour, int min, double sec)
{
  mSeconds = hour*3600.+min*60+sec;
}

void TimeStamp::SetHour(const char* shour)
{
  int hour, min;
  double sec;
  sscanf(shour,"%d:%d:%lf",&hour, &min, &sec);
  mSeconds = hour*3600.+min*60+sec;

}

void TimeStamp::GetDate(int& year, int& month, int& day) const
{
  int_8 jours = mDays;
  // Recherche de l'annee
  if (jours < 0) {
    year = 1901;
    while(jours < 0)  { 
      year--;
      jours += YearDays(year); 
    }
  }
  else {
    year = 1901;
    while(jours > YearDays(year))  { 
      jours -= YearDays(year); 
      year++;
    }
  }
  // Recherche du mois
  month = 1;
  while(jours > MonthDays(year, month) )  { 
    jours -= MonthDays(year, month);
    month++;
  }
  day = jours;
}

void TimeStamp::GetHour(int& hour, int& min, double& sec) const
{
  double seconds = mSeconds;
  hour = (int)trunc(seconds/3600.);
  seconds -= hour*3600;
  min = (int)trunc(seconds/60.);
  while (min >= 60) { hour++; min -= 60; }
  sec = seconds-min*60;
  while (sec >= 60.) { min++; sec -= 60.; }
}

double TimeStamp::ToDays() const
{
  return((double)mDays + mSeconds/86400.);
}

/*!
\param fmt : String format for the date 
  - FmtPackedDateTime : YYYY-MM-DDThh:mm:ss.s
  - FmtDateOnly : dd/mm/yyyy
  - FmtTimeOnly : hh:mm:ss.s
  - FmtDateTime : dd/mm/yyyy hh:mm:ss.s UT
*/
string TimeStamp::ToString(StrFmt fmt) const
{
  char buff[128];
  int aa, mm, jj;
  int hh, min;
  double sec;
  GetDate(aa, mm, jj);
  GetHour(hh, min, sec);
  if (fmt == FmtPackedDateTime) 
    sprintf(buff,"%04d-%02d-%02dT%02d:%02d:%02.1f", aa,mm,jj, hh,min,sec);
  else {
    if (fmt == FmtDateOnly) 
      sprintf(buff,"%02d/%02d/%04d ", jj,mm,aa);
    else if (fmt == FmtTimeOnly) 
      sprintf(buff,"%02d:%02d:%02.3f ", hh,min,sec);
    else 
      sprintf(buff,"%02d/%02d/%04d %02d:%02d:%02.1f UT", jj,mm,aa,hh,min,sec);
  }
  return buff;
}

/*!
\param fmt : String format for the date 
  - FmtPackedDateTime : YYYY-MM-DDThh:mm:ss.s
  - FmtDateOnly : dd/mm/yyyy
  - FmtTimeOnly : hh:mm:ss.s
  - FmtDateTime : dd/mm/yyyy hh:mm:ss.s UT
*/
void TimeStamp::Print(ostream& os, StrFmt fmt)  const
{
  os << " " << ToString(fmt) << " "; 
}

int TimeStamp::MonthDays(int annee, int mois) 
{
  if (mois<1 || mois>12) throw ParmError("TimeStamp::MonthDays month out of range");
  
  switch(mois) {
    case mois_Janvier: 
    case mois_Mars:
    case mois_Mai:
    case mois_Juillet:
    case mois_Aout:
    case mois_Octobre:
    case mois_Decembre:
      return 31;
    case mois_Avril:
    case mois_Juin:
    case mois_Septembre:
    case mois_Novembre:
      return 30;
    case mois_Fevrier:
     return (((annee%4 == 0) && (annee%100 != 0)) || (annee%400 == 0)) ? 29 : 28;
  }
  return -1;
}

int TimeStamp::YearDays(int annee)
//	Retourne le nombre de jours dans l'année
{
  return (((annee%4 == 0) && (annee%100 != 0)) || (annee%400 == 0)) ? 366 : 365;
}

int_8 TimeStamp::ConvertToDays(int AA, int MM, int JJ)
{
  int_8 t = 0;
  //   if (!UndetDate()) {
  int nban = AA-1901;
  if (nban >= 0)  
    t = nban*365 + (nban/4) - (nban/100) + ((nban+300)/400);
  else 
    t = nban*365 + (nban/4) - (nban/100) + ((nban-100)/400);
  for (int i=1; i<MM; i++)  t += TimeStamp::MonthDays(AA, i);
  t += JJ;
  // }
  return t;
}

TimeStamp& TimeStamp::Shift(int_8 days, r_8 sec)
{
  //DBG  cout << " *DBG*TimeStamp::Shift(" << days <<","<<sec<<")" << endl;
  sec+=mSeconds;
  days+=mDays;
  //DBG  cout << " *DBG*TimeStamp::Shift() mDays,mSeconds=" << mDays<<","<<mSeconds
  //DBG     << " -> days,sec= " << days <<","<<sec<< endl;
  while (sec<0.) {
    sec+=86400.;  days--;
  }
  while (sec>86400.) {
    sec-=86400.;  days++;
  }
  //DBG  cout << " *DBG*TimeStamp::Shift() - Set(" << days <<","<<sec<<")" << endl;
  Set(days,sec);
  return *this;
}


double TimeStamp::TimeDifferenceSeconds(TimeStamp const& tm1, TimeStamp const& tm2)
{
  int_8 ddays=tm1.mDays-tm2.mDays;
  r_8 dsec=tm1.mSeconds-tm2.mSeconds;
  return ((double)ddays*86400.+dsec);
}

double TimeStamp::TimeDifferenceDays(TimeStamp const& tm1, TimeStamp const& tm2)
{
  int_8 ddays=tm1.mDays-tm2.mDays;
  r_8 dsec=tm1.mSeconds-tm2.mSeconds;
  return ((double)ddays+dsec/86400.);
}



//----------------------------------------------------------
// Classe pour la gestion de persistance
// ObjFileIO<TimeStamp>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<TimeStamp>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<TimeStamp>::WriteSelf() dobj=NULL");
  int_4 ver;
  ver = 1;
  s.Put(ver);   // Lecture numero de version PPF
  s.Put(dobj->DaysPart());
  s.Put(dobj->SecondsPart());
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<TimeStamp>::ReadSelf(PInPersist& s)
{
  int_4 ver;
  s.Get(ver);   // Lecture numero de version PPF
  r_8 seconds;
  int_8 days;
  s.Get(days);
  s.Get(seconds);

  if (dobj == NULL) dobj = new TimeStamp(days, seconds);
  else dobj->Set(days, seconds);
}


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<TimeStamp>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<TimeStamp>;
#endif

} // FIN namespace SOPHYA 
