// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Class  TimeStamp : Date/Time computation 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2005-2013
//-----------------------------------------------------------

#ifndef TIMESTAMP_H_SEEN
#define TIMESTAMP_H_SEEN

// Classe TimeStamp (Date+heure)
// R. Ansari      UPS+LAL IN2P3/CNRS  2005
// Code recupere en partie ds dates.h dates.cc (classe Date) / E. Aubourg 1995-2000


#include "machdefs.h"
#include "objfio.h"
#include <string> 
#include <iostream> 

namespace SOPHYA {
  
//! Class representing date and time
class TimeStamp : public AnyDataObj {
public:
  /*  Pour utilisation ulterieure
  //! GMT / local time enum 
  enum {kGMTTime=0, kLocalTime=1};
  */
  //! Month name enum
  enum Month {month_January=1, month_February, month_March, month_April, month_May, month_June, month_July,
             month_August, month_September, month_October, month_November, month_December};  

  enum StrFmt { FmtPackedDateTime , FmtDateOnly, FmtTimeOnly, FmtDateTime };

  //! Default Constructor. Set to current date and time (GMT) if fsgcur==true, 0 Jan 1901 0h TU otherwise.
  TimeStamp(bool fgscur=false);
  TimeStamp(TimeStamp const & ts);
  //! Constructor with specification of number of days since 0 Jan 1901 0h TU 
  explicit TimeStamp(double days);
  //! Constructor from number of days (since 0/01/1901) and number of seconds 
  TimeStamp(int_8 days, r_8 seconds);
  //! Constructor with specification of day,month,year,hour,minutes,seconds 
  TimeStamp(int year, int month, int day, int hour, int min, double sec);
  //! Constructor with specification of date & time in the format YYYY/MM/DD hh:mm:ss  
  TimeStamp(string const & date, string const & hour);
  //! Constructor with specification of date & time in the format YYYY/MM/DD hh:mm:ss  
  TimeStamp(const char* date, const char* hour);
  //! Constructor with specification of date & time in the format YYYY-MM-DDThh:mm:ss 
  explicit TimeStamp(string& datim);
  //! Constructor with specification of date & time in the format YYYY-MM-DDThh:mm:ss 
  explicit TimeStamp(const char* datim);

  //! Set the value of the time stamp copying from "ts"
  void Set(TimeStamp const & ts);
  //! Sets the value of the time stamp from the number of days since 0 Jan 1901 0h TU 
  void Set(double days);
  //! Sets the value of the time stamp from the number of days and seconds
  void Set(int_8 days, r_8 seconds);
  //! Sets the value of the time stamp from a string in the format YYYY-MM-DDThh:mm:ss 
  void Set(const char * datim);
 //! Sets the value of the time stamp from a string in the format YYYY-MM-DDThh:mm:ss 
  inline void Set(string const & datim)  { Set(datim.c_str()); }
  //! initialize with current date and time (GMT)
  void SetNow();

  //! The equal (set) operator 
  inline TimeStamp& operator= (TimeStamp const & ts) 
    { Set(ts); return(*this); }

  //! Sets the value of the date (days) 
  void SetDate(int year, int month, int day);
  //! Sets the value of the date (format: DD/MM/YYYY) 
  void SetDate(const char* date); 
  //! Sets the value of the date (format: DD/MM/YYYY) 
  inline void SetDate(string const& date) { SetDate(date.c_str()); }

  //! Sets the value of the time of day (hours) 
  void SetHour(int hour, int min, double sec);
  //! Sets the value of the time of day (format: hh:mm:ss[.ss]) 
  void SetHour(const char* hour); 
  //! Sets the value of the time of day (format: hh:mm:ss[.ss]) 
  inline void SetHour(string const& hour) { SetHour(hour.c_str()); }

  //! Return the date (Year, Month, Day)
  void GetDate(int& year, int& month, int& day) const;
  //! Return the time of the day
  void GetHour(int& hour, int& min, double& sec) const;

  //! Return the value of the TimeStamp as days.fraction_days since 0 Jan 1901
  double ToDays() const;
  //! Conversion operator to double - return ToDays() 
  inline operator double() const { return ToDays(); }

  //! Return the timestamp in string format 
  string ToString(StrFmt fmt=FmtPackedDateTime) const;
  
  //! Return the integral number of days since 0 Jan 1901
  inline int_8 DaysPart() const { return mDays; }
  //! Return the fractional number of days in seconds ( 0 < nsec < 86400. )
  inline r_8 SecondsPart() const { return mSeconds; }
  //! Changes the date/time by shifting it the specified number of days and seconds
  TimeStamp& Shift(int_8 sdays, r_8 ssec);
  //! Changes the date/time by shifting it the specified number of days
  inline TimeStamp& ShiftDays(int_8 sdays)
  { return Shift(sdays,0.); }
  //! Changes the date/time by shifting it the specified number of seconds
  inline TimeStamp& ShiftSeconds(r_8 ssec)
  { return Shift(0,ssec); }
  //! Changes the date/time by shifting it the specified number of hours
  inline TimeStamp& ShiftHours(r_8 shours)
  { return Shift(0,shours*3600.); }
 
  //! Prints the date/time in string format on \b cout 
  inline  void    Print(StrFmt fmt=FmtDateTime) const  
     { Print(cout, fmt); }  
  //! Prints the date/time in string format on stream \b os 
  virtual void    Print(ostream& os, StrFmt fmt=FmtDateTime)  const;

  //! Number of days in a given month  
  static int   MonthDays(int year, int month);
  //! Number of days in a given year
  static int YearDays(int annee);
  //! Number of  days since 0 janvier 1901 0h TU
  static int_8 ConvertToDays(int year, int month, int day);  
  //! return the time difference in seconds between the two dates tm1-tm2 
  static double TimeDifferenceSeconds(TimeStamp const& tm1, TimeStamp const& tm2);
  //! return the time difference in days between the two dates tm1-tm2 
  static double TimeDifferenceDays(TimeStamp const& tm1, TimeStamp const& tm2);

protected:
  r_8 mSeconds;     // Number of seconds since 00:00:00 
  int_8 mDays;      // Number of days since 0 Jan 1901
};

/*! operator << overloading - Prints date/time in string format on \b os*/
inline ostream& operator << (ostream& s, TimeStamp const & ts)
  {  ts.Print(s);  return(s);  }
/*! computes the time difference in seconds between the two dates tm1-tm2 */
inline double operator - (TimeStamp const& tm1, TimeStamp const& tm2) 
{ return TimeStamp::TimeDifferenceSeconds(tm1,tm2); }

//! \brief equality comparison operator (warning within 1e-12 s (1 ps) precision) 
inline bool operator == (TimeStamp const& tm1, TimeStamp const& tm2)
{ return (fabs(TimeStamp::TimeDifferenceSeconds(tm1,tm2))<1.e-12); }
//! \brief unequality comparison operator (warning difference > 1e-12 s (1 ps)) 
inline bool operator != (TimeStamp const& tm1, TimeStamp const& tm2)
{ return (fabs(TimeStamp::TimeDifferenceSeconds(tm1,tm2))>1.e-12); }
//! \brief Comparison (less than) operator (warning within 1e-12 s precision) 
inline bool operator < (TimeStamp const& tm1, TimeStamp const& tm2)
{ return (TimeStamp::TimeDifferenceSeconds(tm1,tm2)<0.); }
//! \brief Comparison (greater than) operator  
inline bool operator > (TimeStamp const& tm1, TimeStamp const& tm2)
{ return (TimeStamp::TimeDifferenceSeconds(tm1,tm2)>0.); }
//! \brief Comparison (less or equal than) operator 
inline bool operator <= (TimeStamp const& tm1, TimeStamp const& tm2)
{ return (TimeStamp::TimeDifferenceSeconds(tm1,tm2)<=0.); }
//! \brief Comparison (greater or equal than) operator  
inline bool operator >= (TimeStamp const& tm1, TimeStamp const& tm2)
{ return (TimeStamp::TimeDifferenceSeconds(tm1,tm2)>=0.); }

/*! Writes the object in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, TimeStamp const & obj)
{ ObjFileIO<TimeStamp> fio(const_cast<TimeStamp *>(&obj));  fio.Write(os);  return(os); }
/*! Reads the object from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, TimeStamp & obj)
{ ObjFileIO<TimeStamp> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} // namespace SOPHYA

#endif
