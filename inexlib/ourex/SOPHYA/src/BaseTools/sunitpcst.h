// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Class  BasePhysQty : Base class for units and Physical Constants
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2012
//-----------------------------------------------------------

#ifndef SUNITPCST_H_SEEN
#define SUNITPCST_H_SEEN


#include "machdefs.h"
#include <math.h> 
#include <string> 
#include <vector> 
#include <iostream> 

#include "pqnumber.h"
#include "objfio.h"


namespace SOPHYA {

//--------------------------------------------------------------------------------   
//--------------------------------  Units class  ---------------------------------
//--------------------------------------------------------------------------------   

//! Class representing physical dimensions and units  
class Units : public AnyDataObj {
// gestionnaire PPF
  friend class ObjFileIO<Units>;

public:
  //! Default constructor - creates a dimensionless object = 1 
  Units();
  //! Copy constructor 
  Units(Units const& un);
  //! Base constructor  
  Units(const char* nom, const char* snom, int m, int kg, int s, r_8 val=1., int A=0, int K=0, int mol=0, int cd=0);
  //! Constructor from an existing unit, but with a scaling factor and new name    
  Units(Units const& un, r_8 scale, const char* nom, const char* snom);
  //! Constructur with the object initialized from a string.
  explicit Units(const string & s);

  virtual ~Units();

  //! Copy method
  virtual Units& Set(Units const& un);
  //! Copy (=) operator 
  inline Units& operator = (Units const& un)  { return Set(un); }
  //! return true if the two quantities have the same dimension 
  bool isSameDimension(Units const& un) const;
  // return true if the two objects are equal (same unit). 
  bool isEqual(Units const& un, bool fgstrict=true) const;
  //! return the value in base SI units
  inline r_8 SIValue() const { return mSI_Value_; }
  //! return the unit name 
  inline string const& Name() const { return mName_; }
  //! return the unit short name 
  inline string const& ShortName() const { return mShortName_; }
  //! Prints the object on \b cout (return the cout stream object)
  inline ostream& Print(int lev=2)  const  
  { return Print(cout,lev); }  
  //! Prints the object on stream \b os (return the os stream object)
  virtual ostream& Print(ostream& os,int lev=2)  const;
  //! return a complete representation of the object as a string
  string ToStringFullForm() const;

  //! Return a new Unit corresponding to a rational power of the initial Unit 
  virtual Units power(QNumber q) const;
  //! Return a new Unit corresponding to an integer power of the initial Unit 
  inline  Units power(int p) const 
  { return power(QNumber(p,1)); }
 
  //! Compute a new unit corresponding corresponding to the product of two units 
  static Units Multiply(Units const& u1, Units const& u2);
  //! Compute a new unit corresponding corresponding to the ratio of two units 
  static Units Divide(Units const& u1, Units const& u2);
  
  // Unites multiples et sous-multiples 
  inline Units pico()
  { return this->Multiple(1.e-12,"pico","p"); }
  inline Units nano()
  { return this->Multiple(1.e-9,"nano","n"); }
  inline Units micro()
  { return this->Multiple(1.e-6,"micro","mu"); }
  Units milli()
  { return this->Multiple(1.e-3,"milli","m"); }
  Units centi()
  { return this->Multiple(1.e-2,"centi","c"); }
  Units deci()
  { return this->Multiple(1.e-1,"deci","d"); }
  Units deca()
  { return this->Multiple(1.e1,"deca","da"); }
  Units hecto()
  { return this->Multiple(1.e2,"hecto","h"); }
  Units kilo()
  { return this->Multiple(1.e3,"kilo","k"); }
  Units mega()
  { return this->Multiple(1.e6,"mega","M"); }
  Units giga()
  { return this->Multiple(1.e9,"giga","G"); }

  // Les unites usuelles - Voir definition et convention sur 
  //  http://www.bipm.org/fr/si/   et  http://physics.nist.gov/cuu/Units/
  inline static Units meter()
  { return Units("meter","m",1,0,0); }
  inline static Units kilometer()
  { return Units("kilometer","km",1,0,0,1.e3); }
  inline static Units km()
  { return Units::kilometer(); }
  inline static Units centimeter()
  { return Units("centimeter","cm",1,0,0,1.e-2); }
  inline static Units cm()
  { return Units::centimeter(); }
  inline static Units millimeter()
  { return Units("millimeter","mm",1,0,0,1.e-3); }
  inline static Units mm()
  { return Units::millimeter(); }

  inline static Units kilogram()
  { return Units("kilogram","kg",0,1,0); }
  inline static Units kg()
  { return Units::kilogram(); }
  inline static Units gram()
  { return Units("gram","g",0,1,0,0.001); }

  inline static Units second()
  { return Units("second","s",0,0,1);  }
  inline static Units hour()
  { return Units("hour","h",0,0,1,3600.);  }
  inline static Units minute()
  { return Units("minute","mn",0,0,1,60.);  }

  inline static Units liter()
  { return Units("liter","L",3,0,0);  }

  inline static Units ampere()
  { return Units("ampere","A",0,0,0,1.,1);  }
  inline static Units kelvin()
  { return Units("kelvin","K",0,0,0,1.,0,1);  }
  inline static Units mole()
  { return Units("mole","mol",0,0,0,1.,0,0,1);  }
  inline static Units candela()
  { return Units("candela","cd",0,0,0,1.,0,0,0,1);  }

  inline static Units newton()
  { return Units("newton","N",1,1,-2);  }

  inline static Units _pascal() //G.Barrand : iOS : have _
  { return Units("pascal","Pa",-1,1,-2);  }

  inline static Units joule()
  { return Units("joule","J",2,1,-2);  }
  inline static Units electronvolt()
  { return Units("electronvolt","eV",2,1,-2,1.602176e-19); }
  inline static Units watthour()
  { return Units("watt.hour","Wh",2,1,-2,3600.);  }

  inline static Units watt()
  { return Units("watt","W",2,1,-3);  }


  inline static Units hertz()
  { return Units("hertz","Hz",0,0,-1);  }

  inline static Units coulomb()
  { return Units("coulomb","C",0,0,1,1.,1);  }
  inline static Units volt()
  { return Units("volt","V",2,1,-3,1.,-1);  }
  inline static Units tesla()
  { return Units("tesla","T",0,1,-2,1.,-1);  }
  inline static Units ohm()
  { return Units("Ohm","Ohm",2,1,-3,1.,-2);  }
  inline static Units henry()
  { return Units("henry","H",2,1,-2,1.,-2);  }
  inline static Units farad()
  { return Units("farad","F",-2,-1,4,1.,2);  }

  inline static Units radian()
  { return Units("radian","rad",0,0,0,1);  }
  inline static Units degree()
  { return Units("degree","deg",0,0,0,M_PI/180.);  }
  inline static Units steradian()
  { return Units("steradian","sr",0,0,0,1);  }

  //----  Specialized units 
  // parsec, lightyear ... Units OR PhysConstants ?
  //! lightyear :  Distance travelled by light in one year 
  inline static Units lightyear()
  { return Units("light-year","ly",1,0,0,0.9461e16); }
  //! parsec :  parallax 1 arcsec ( 1 AU / 1 arcsec)  
  inline static Units parsec()
  { return Units("parsec","pc",1,0,0,3.0856775807e16); }
  //! jansky: 10^-26 W/m^2/Hz 
  inline static Units jansky()
  { return Units("jansky","Jy",0,1,-2,1.e-26); }


protected:
  inline void SetName(string const& nom, string const& snom)
  { mName_=nom; mShortName_=snom; }
  inline Units Multiple(r_8 fact, const char* pre, const char* spre) 
  { 
    Units ru(*this); ru.mSI_Value_*=fact; 
    ru.mName_=pre+mName_; ru.mShortName_=spre+mShortName_; 
    return ru; 
  }

  // International system of Units (SI) - http://physics.nist.gov/cuu/Units/bibliography.html
  QNumber mLength_, mMass_, mTime_, mCurr_;     // Dimensions : length, mass, time and current 
  QNumber mTemp_, mSubst_, mLumInt_;     // Dimensions : Temperature, Amount of substance, LuminousIntensity
  r_8 mSI_Value_;  // Value in SI units (m, kg, s, A, Kelvin, mole, Candela) 
  string mName_, mShortName_;   // Name and short name 
};

//! operator << overloading - Prints Units in string format on \b os
inline ostream& operator << (ostream& s, Units const & un)
{  un.Print(s,0);  return(s);  }

//! Equality operator (==) definition for two Units objects   
inline bool operator == (Units const & a, Units const & b)
{ return a.isEqual(b); }

//! Non-Equality operator (!=) definition for two Units objects   
inline bool operator != (Units const & a, Units const & b)
{ return !(a.isEqual(b)); }

//! Multiply operator definition for two Units objects  
inline Units operator * (Units const & a, Units const & b)
  { return Units::Multiply(a,b); }
//! Divide operator definition for two Units objects 
inline Units operator / (Units const & a, Units const & b)
  { return Units::Divide(a,b); }


//! Writes the Units object in the POutPersist stream \b os
inline POutPersist& operator << (POutPersist& os, Units & obj)
{ ObjFileIO<Units> fio(&obj);  fio.Write(os);  return(os); }
//! Reads the Units object from the PInPersist stream \b is 
inline PInPersist& operator >> (PInPersist& is, Units & obj)
{ ObjFileIO<Units> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

//--------------------------------------------------------------------------------   
//-------------------------------  PhysQty class  --------------------------------
//--------------------------------------------------------------------------------   

//! Class representing physical quantities and constants   
class PhysQty : public AnyDataObj {
// gestionnaire PPF
  friend class ObjFileIO<PhysQty>;

public:
  PhysQty();
  PhysQty(Units const & u, r_8 val, r_8 prec=0., const char* nom=NULL);
  PhysQty(PhysQty const & qty);
  virtual ~PhysQty();

  //! Copy method
  virtual PhysQty& Set(PhysQty const& qty);
  //! Copy (=) operator 
  inline PhysQty& operator = (PhysQty const& qty)  { return Set(qty); }

  //! Return a new physical quantity object after unit conversion   
  PhysQty ConvertTo(Units const & u) const ;

  //! Return the physical quantity value in SI units 
  inline r_8 SIValue()  const { return val_*unit_.SIValue(); }
  //! Return the associated unit 
  inline Units getUnit()  {return unit_; }
  //! Return the associated value
  inline r_8 Value()  const { return val_; }
  //! Change the value of the physical quantity 
  inline void setValue(r_8 val)  { val_=val; }
  //! Return the relative precision 
  inline r_8 RelativePrecision()  const { return prec_; }
  //! Change the relative precision 
  inline void setRelativePrecision(r_8 prec) { prec_=prec; }
  //! Return the absolute precision 
  inline r_8 AbsolutePrecision() const { return prec_*fabs(val_); }
  //! Return the associoated name / description
  inline string const& Name() const { return name_; }
  //! Change the associated name / description
  inline void setNameDesc(string const& name) { name_=name; }

  // return true if the two objects are equal (same unit). 
  bool isEqual(PhysQty const& qty, bool fgstrict=true) const;

  //! Prints the object on \b cout (return the cout stream object)
  inline ostream& Print(int lev=0)  const  
  { return Print(cout, lev); }  
  //! Prints the object on stream \b os (return the os stream object)
  virtual ostream& Print(ostream& os,int lev=2)  const;

  //! Return a new physical quantity corresponding to the initial quantity scaled by f  
  inline PhysQty scale(r_8 f) const
  { return PhysQty(unit_,val_*f,prec_); }
  //! Return a new physical quantity corresponding to a rational power of the initial quantity 
  virtual PhysQty power(QNumber q) const;
  //! Return a new physical quantity corresponding to an integer power of the initial quantity 
  inline  PhysQty power(int p) const 
  { return power(QNumber(p,1)); }

  //! Compute a new physical quantity corresponding  to the sum (fgsum=true) or difference of two quantities
  static PhysQty AddSubtract(PhysQty const& q1, PhysQty const& q2, bool fgsum=true);
  //! Compute a new physical quantity corresponding to the product (fgprod=true) or the ratio of two quantities
  static PhysQty MultiplyDivide(PhysQty const& q1, PhysQty const& q2, bool fgprod=true);

  //---- Constantes physiques - source NIST http://physics.nist.gov/
  //! Return the speed of light in m/s
  inline static PhysQty SpeedofLight() 
  { return PhysQty(Units::meter()/Units::second(), 299792458., 0., "Speed of light (c)"); }
  //! Return the speed of light in m/s
  inline static PhysQty c() 
  { return PhysQty::SpeedofLight(); }

  //! Return the Newtonian constant of gravitation in m^3 kg^-1 s^-2 
  inline static PhysQty G_Newton() 
  { return PhysQty(Units::meter().power(3)*Units::kilogram().power(-1)*Units::second().power(-2), 
		   6.67384e-11, 1.2e-4, "Newtonian constant of gravitation (G)"); }
  //! Return the Newtonian constant of gravitation in m^3 kg^-1 s^-2 
  inline static PhysQty G() 
  { return PhysQty::G_Newton(); }

  //! Return the Planck constant in joule.s
  inline static PhysQty PlanckConstant() 
  { return PhysQty(Units::joule()*Units::second(), 6.62606957e-34, 4.4e-8, "Planck constant (h)"); }
  //! Return the Planck constant in joule.s
  inline static PhysQty h() 
  { return PhysQty::PlanckConstant(); }
  //! Return the Planck constant divided by 2 pi (hbar = h/2pi) in joule.s
  inline static PhysQty hbar() 
  { return PhysQty(Units::joule()*Units::second(), 6.62606957e-34/2./M_PI, 4.4e-8, "hbar (h/2pi)"); }

  //! Return the Boltzmann constant in joule/K
  inline static PhysQty BoltzmannConstant() 
  { return PhysQty(Units::joule()/Units::kelvin(), 1.3806488e-23, 9.1e-7, "Boltzmann constant (k)"); }
  //! Return the Boltzmann constant in joule/K
  inline static PhysQty k() 
  { return PhysQty::BoltzmannConstant(); }
  //! Return the molar gas constant (R) in joule/mol/K
  inline static PhysQty MolarGasConstant() 
  { return PhysQty(Units::joule()/Units::mole()/Units::kelvin(), 8.3144621, 9.1e-7, "Molar gas constant (R)"); }
  //! Return the molar gas constant (R) in joule/mol/K
  inline static PhysQty R() 
  { return PhysQty::MolarGasConstant(); }

  //! Return the Avogadro number (units mol^-1)
  inline static PhysQty N_Avogradro() 
  { return PhysQty(Units::mole().power(-1), 6.02214129e23, 4.4e-8, "Avogadro constant (N_A)"); }
  //! Return the Avogadro number (units mol^-1)
  inline static PhysQty N_A() 
  { return PhysQty::N_Avogradro(); }

  //! Return the electric constant in Farad/m 
  inline static PhysQty epsilon0() 
  { return PhysQty(Units::farad()/Units::meter(), 8.854187817e-12, 0., "Electric constant (epsilon0)"); }
  //! Return the magnetic constant in N / A^2 
  inline static PhysQty mu0() 
  { return PhysQty(Units::newton()/Units::ampere().power(2), 4.*M_PI*1e-7, 0., "Magnetic constant (mu0)"); }

  //! Return the electron mass in Kg 
  inline static PhysQty electron_mass()
  {  return PhysQty(Units::kilogram(), 9.10938291e-31, 4.4e-8, "Electron mass (m_e)"); }
  //! Return the proton mass in Kg 
  inline static PhysQty proton_mass()
  {  return PhysQty(Units::kilogram(), 1.672621777e-27, 4.4e-8, "Proton mass (m_p)"); }

  //----- Astrophysicial quantities 
  //! Return the solar mass in Kg 
  inline static PhysQty solar_mass()
  {  return PhysQty(Units::kilogram(),  1.98844e+30, 1.5e-4, "Solar mass (M_sol)"); }
  //! Return the earth mass in Kg 
  inline static PhysQty earth_mass()
  {  return PhysQty(Units::kilogram(), 5.9723e+30, 1.5e-4, "Earth mass (M_earth)"); }
  //! Return the solar luminosity in watt 
  inline static PhysQty solar_luminosity()
  {  return PhysQty(Units::watt(), 3.846e+26, 2.e-3, "Solar luminosity (L_sol)"); }

 
protected:
  Units unit_; // Unite de la grandeur physique 
  r_8 val_;   // Valeur de la grandeur physique dans l'unite unit_
  r_8 prec_;  // precision relative (1 sigma)
  string name_;  // Name of the quantity / constant 
};

//! operator << overloading - Prints PhysQty on \b os
inline ostream& operator << (ostream& s, PhysQty const & qty)
{  qty.Print(s,1);  return(s);  }

//! Equality operator (==) definition for two PhysQty objects  
inline bool operator == (PhysQty const & a, PhysQty const & b)
{ return a.isEqual(b); }

//! Non-Equality operator (!=) definition for two PhysQty objects 
inline bool operator != (PhysQty const & a, PhysQty const & b)
{ return !(a.isEqual(b)); }

//! multiplication by a real constant (scaling)
inline PhysQty operator * (r_8 f, PhysQty const & a)
{ return a.scale(f); }
//! multiplication by a real constant (scaling)
inline PhysQty operator * (PhysQty const & a, r_8 f)
{ return a.scale(f); }

//! Addition operator definition for two PhysQty objects res=a+b 
inline PhysQty operator + (PhysQty const & a, PhysQty const & b)
{ return PhysQty::AddSubtract(a,b,true); }
//! Subtraction operator definition for two PhysQty objects res=a-b
inline PhysQty operator - (PhysQty const & a, PhysQty const & b)
{ return PhysQty::AddSubtract(a,b,false); }
//! Multiplication operator definition for two PhysQty objects  res=a*b
inline PhysQty operator * (PhysQty const & a, PhysQty const & b)
{ return PhysQty::MultiplyDivide(a,b,true); }
//! Division operator definition for two PhysQty objects  res=a/b
inline PhysQty operator / (PhysQty const & a, PhysQty const & b)
{ return PhysQty::MultiplyDivide(a,b,false); }

//! Writes the PhysQty object in the POutPersist stream \b os
inline POutPersist& operator << (POutPersist& os, PhysQty & obj)
{ ObjFileIO<PhysQty> fio(&obj);  fio.Write(os);  return(os); }
//! Reads the PhysQty object from the PInPersist stream \b is 
inline PInPersist& operator >> (PInPersist& is, PhysQty & obj)
{ ObjFileIO<PhysQty> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }


} // namespace SOPHYA

#endif
