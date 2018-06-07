#include "machdefs.h"
#include "sunitpcst.h"
#include <sstream>
#include <iomanip>

namespace SOPHYA {

/*!
   \class Units
   \ingroup BaseTools
   This class represents physical dimension and units. Static methods return Units 
   objects corresponding to the usual units, such as meter or watt. Check the 
   following web sites for definition and conventions:
   http://www.bipm.org/fr/si/   or  http://physics.nist.gov/cuu/Units/
   
   \code
   Units w=Units::watt(); 
   cout << " Power unit (watt) : " << w.Name() << " (" << w << ")" << endl; 
   // define the speed SI unit (m/s) : 
   Units mos = Units::meter()/Units::second() ;
   cout << " SI speed unit : " << mos << "  -> "; mos.Print() ;  
   // mutiple of an existing unit :
   Units mw=Units::watt().mega(); 
   cout << " Megawatt: " << mw.Name() << " (" << mw << ")  -> "; mw.Print() ; 
   \endcode
*/

//-------------------------------------------------------------------------
//-------------------------  Classe Units  --------------------------
//-------------------------------------------------------------------------

/* --Methode-- */
Units::Units()
  : mLength_(0), mMass_(0), mTime_(0), mCurr_(0), 
    mTemp_(0), mSubst_(0), mLumInt_(0), mSI_Value_(1.)
{
}

/* --Methode-- */
/*!
  The Units object is created by specifying its name and short name, and the integer powers 
  for the seven SI base physical quantities (length, mass, time \ldots), and the value of the 
  unit in term of the SI unit.
*/
Units::Units(const char* nom, const char* snom, int m, int kg, int s, r_8 val, int A, int K, int mol, int cd)
  : mLength_(m), mMass_(kg), mTime_(s), mCurr_(A), 
    mTemp_(K), mSubst_(mol), mLumInt_(cd), mSI_Value_(val),
    mName_(nom), mShortName_(snom)
{
}


/* --Methode-- */
Units::Units(Units const& un)
{
  Set(un);
}

/* --Methode-- */
Units::Units(Units const& un, r_8 scale, const char* nom, const char* snom)
{
  Set(un);
  mSI_Value_*=scale;
  mName_=nom;
  mShortName_=snom;
}

/*!
  string should have space separated words corresponding to all the Units object field, with 
  the same format as the strings returned by ToStringFullForm() which are are suitable argument 
  for this constructor.
*/
/* --Methode-- */
Units::Units(string const& s)
  : mLength_(0), mMass_(0), mTime_(0), mCurr_(0), 
    mTemp_(0), mSubst_(0), mLumInt_(0), mSI_Value_(0.)
{
  istringstream ss(s);
  // _ separated values, except for the two strings which are space separated 
  char csp;
  ss >> mLength_ >> csp >>  mMass_ >> csp >> mTime_ >> csp  >> mCurr_ 
     >> csp >> mTemp_ >> csp >> mSubst_ >> csp >> mLumInt_ >> csp >> mSI_Value_
     >> csp >> mName_ >> mShortName_;
}

/* --Methode-- */
Units::~Units()
{
}

/* --Methode-- */
Units& Units::Set(Units const& un)
{
  mLength_ = un.mLength_;
  mMass_=un.mMass_;
  mTime_=un.mTime_; 
  mCurr_=un.mCurr_; 
  mTemp_=un.mTemp_; 
  mSubst_=un.mSubst_;
  mLumInt_=un.mLumInt_;
  mSI_Value_=un.mSI_Value_;
  mName_=un.mName_;
  mShortName_=un.mShortName_;
  return (*this);
}

/* --Methode-- */
bool Units::isSameDimension(Units const& un)  const
{
  if ( (mLength_==un.mLength_) && (mMass_==un.mMass_) && 
       (mTime_==un.mTime_) && (mCurr_==un.mCurr_) && 
       (mTemp_==un.mTemp_) && (mSubst_==un.mSubst_) && (mLumInt_==un.mLumInt_) ) return true;
  else return false;
}

/* --Methode-- */
/*!
  \brief return true if the two objects are equal (same unit).

  if fgstrict is true, requires match on all fields, including the name and short name string fields,
  otherwise, dimensional and SI value equality is checked.
*/
bool Units::isEqual(Units const& un, bool fgstrict)  const
{
  if ( (mLength_!=un.mLength_) || (mMass_!=un.mMass_) ||
       (mTime_!=un.mTime_) || (mCurr_!=un.mCurr_) || 
       (mTemp_!=un.mTemp_) || (mSubst_!=un.mSubst_) || (mLumInt_!=un.mLumInt_) ) return false;
  if (fabs(mSI_Value_-un.mSI_Value_)>1.e-12*mSI_Value_)  return false;
  if (!fgstrict) return true;
  if (mName_!=un.mName_)  return false;
  if (mShortName_!=un.mShortName_)  return false;
  return true;
}

/* --Methode-- */
ostream& Units::Print(ostream& os, int lev)  const
{
  if (lev < 2) {
    if (lev==0) os << mShortName_;
    else  os << mName_;
    return os; 
  }
  os << mName_ << " (" << mShortName_ << ") SI_Value=" << mSI_Value_ << " [ ";
  if (!mLength_.isZero()) os << "L" << '^' << mLength_ << ' ';
  if (!(mMass_.isZero())) os << "M" << '^' << mMass_ << ' ';
  if (!(mTime_.isZero())) os << "T" << '^' << mTime_ << ' ';
  if (!mCurr_.isZero()) os << "I" << '^' << mCurr_ << ' ';
  if (!mTemp_.isZero()) os << "K" << '^' << mTemp_ << ' ';
  if (!mSubst_.isZero()) os << "Mol" << '^' << mSubst_ << ' ';
  if (!mLumInt_.isZero()) os << "Lum" << '^' << mLumInt_ << ' ';
  cout << " ]" << endl;
  return os;
}

/* --Methode-- */
/*!
  The string contain the dimensions as space separated QNumbers for all base SI quantities,
  L M T I K Mol Lum , followed by SI-value, two strings, the name and the short name. 
*/
string Units::ToStringFullForm() const
{
  ostringstream ss;
  //  ss << (r_8)998877.33 << " ToStringFullForm";
  // _ separated values, except for the two strings
  char csp = '_';
  ss << mLength_ << csp <<  mMass_ << csp << mTime_ << csp  << mCurr_ 
     << csp << mTemp_ << csp << mSubst_ << csp << mLumInt_ << csp << mSI_Value_ 
     << csp << mName_ << ' ' << mShortName_;
  return ss.str();
}


/* --Methode-- */
Units Units::power(QNumber q)  const
{
  Units ru;
  ru.mLength_=mLength_*q; 
  ru.mMass_=mMass_*q;
  ru.mTime_=mTime_*q;
  ru.mCurr_=mCurr_*q;
  ru.mTemp_=mTemp_*q;
  ru.mSubst_=mSubst_*q;
  ru.mLumInt_=mLumInt_*q;
  ru.mSI_Value_=pow(mSI_Value_,(double)q);
  ru.mName_= '(' + mName_ + ')' + '^' + q.ToString();
  ru.mShortName_= '(' + mShortName_ + ')' + '^' + q.ToString();
  return ru;
}

/* --Methode-- */
Units Units::Multiply(Units const& u1, Units const& u2)
{
  Units ru;
  ru.mLength_=u1.mLength_+u2.mLength_;
  ru.mMass_=u1.mMass_+u2.mMass_;
  ru.mTime_=u1.mTime_+u2.mTime_;
  ru.mCurr_=u1.mCurr_+u2.mCurr_;
  ru.mTemp_=u1.mTemp_+u2.mTemp_;
  ru.mSubst_=u1.mSubst_+u2.mSubst_;
  ru.mLumInt_=u1.mLumInt_+u2.mLumInt_;
  ru.mSI_Value_=u1.mSI_Value_*u2.mSI_Value_;
  ru.mName_=u1.mName_ + '*' + u2.mName_;
  ru.mShortName_=u1.mShortName_ + '*' + u2.mShortName_;
  return ru;
}

/* --Methode-- */
Units Units::Divide(Units const& u1, Units const& u2)
{
  Units ru;
  ru.mLength_=u1.mLength_-u2.mLength_;
  ru.mMass_=u1.mMass_-u2.mMass_;
  ru.mTime_=u1.mTime_-u2.mTime_;
  ru.mCurr_=u1.mCurr_-u2.mCurr_;
  ru.mTemp_=u1.mTemp_-u2.mTemp_;
  ru.mSubst_=u1.mSubst_-u2.mSubst_;
  ru.mLumInt_=u1.mLumInt_-u2.mLumInt_;
  ru.mSI_Value_=u1.mSI_Value_/u2.mSI_Value_;
  ru.mName_=u1.mName_ + '/' + u2.mName_;
  ru.mShortName_=u1.mShortName_ + '/' + u2.mShortName_;
  return ru;
}


//----------------------------------------------------------
// Classe pour la gestion de persistance de Units
// ObjFileIO<Units>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<Units>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<Units>::WriteSelf() dobj=NULL");
  int_4 ver;
  ver = 1;
  s.Put(ver);   // ecriture numero de version PPF
  s << dobj->mLength_ << dobj->mMass_ << dobj->mTime_ << dobj->mCurr_;
  s << dobj->mTemp_ << dobj->mSubst_ << dobj->mLumInt_;
  s << dobj->mSI_Value_;
  s << dobj->mName_ << dobj->mShortName_;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<Units>::ReadSelf(PInPersist& s)
{
  int_4 ver;
  s.Get(ver);   // Lecture numero de version PPF

  if (dobj == NULL) dobj = new Units();
  s >> dobj->mLength_ >> dobj->mMass_ >> dobj->mTime_ >> dobj->mCurr_;
  s >> dobj->mTemp_ >> dobj->mSubst_ >> dobj->mLumInt_;
  s >> dobj->mSI_Value_;
  s >> dobj->mName_ >> dobj->mShortName_;
}

/*!
   \class PhysQty
   \ingroup BaseTools
   Class for representing physical quantities and constants. A physical quantity 
   has a unit and a value, and optionally a precision and/or a name. A number of
   physical constants and standard quantities can be obtained as PhysQty objects 
   through static methods.
   Algebric operations on PhysQty operations are defined and perform error propagation.
   In case of binary operations, the error computation is done assuming independent 
   quantities. If x is a PhysQty, x.power(2) and x*x will give slightly different errors.
   
   \sa SOPHYA::Units 

   \code
   cout <<  " k_Boltzmann: " << PhysQty::k() << endl;
   cout <<  " m_e: " << PhysQty::electron_mass() << endl;
   // Conversion of a quantity to a different unit 
   PhysQty aj(Units::joule(), 25.); 
   PhysQty aev=aj.ConvertTo(Units::electronvolt().giga());
   cout <<  " A : " << aj << " -> " << aev << endl;
   \endcode
*/

//-------------------------------------------------------------------------
//-------------------------  Classe PhysQty  --------------------------
//-------------------------------------------------------------------------

/* --Methode-- */
//! Default constructor: zero valued dimensionless quantity 
PhysQty::PhysQty()
  : unit_(), val_(0.), prec_(0.)
{
}

/* --Methode-- */
/*! 
  \brief standard constructor
  The object is constructed through the specification of the unit \b u, the value \b val, 
  and optional relative precision \b prec (=delta val/val) and an optional name.
*/
PhysQty::PhysQty(Units const & u, r_8 val, r_8 prec, const char* nom)
  : unit_(u), val_(val), prec_(prec), name_((nom==NULL)?"":nom)
{
}

/* --Methode-- */
//! Copy constructor
PhysQty::PhysQty(PhysQty const & qty)
  : unit_(qty.unit_), val_(qty.val_), prec_(qty.prec_), name_(qty.name_)
{
}

/* --Methode-- */
PhysQty::~PhysQty()
{
}

/* --Methode-- */
PhysQty& PhysQty::Set(PhysQty const& qty)
{
  unit_=qty.unit_;
  val_=qty.val_;
  prec_=qty.prec_;
  name_=qty.name_;
  return (*this);
}

/* --Methode-- */
PhysQty PhysQty::ConvertTo(Units const & u) const 
{
  if (! unit_.isSameDimension(u) )
    throw ParmError("PhysQty::ConvertTo() - incompatible dimensions !");
  return PhysQty(u, Value()*unit_.SIValue()/u.SIValue(), RelativePrecision(),Name().c_str());
}

/* --Methode-- */
/*!
  \brief return true if the two objects are equal (same unit and same value).

  if fgstrict is true, requires match on all fields, including the name and short name string fields,
  and precision, otherwise, units and value equality is checked.
*/
bool PhysQty::isEqual(PhysQty const& qty, bool fgstrict)  const
{
  bool fgsu=unit_.isEqual(qty.unit_,fgstrict);
  if (!fgsu)  return false;
  if (fabs(val_-qty.val_)>1.e-12*val_)  return false;
  if (!fgstrict) return true;
  if (fabs(prec_-qty.prec_)>1.e-12*prec_)  return false;
  return true;
}

/* --Methode-- */
ostream& PhysQty::Print(ostream& os,int lev) const 
{
  if (lev==0) {
    os << val_ << ' ' << unit_.ShortName();
    return os; 
  }
  char relp[16];
  if (prec_>1.e-3) sprintf(relp,"%4.1f",100.*prec_);
  else sprintf(relp,"%6.1e",100.*prec_);
  if (name_.length()>0) os << name_ << ": ";
  if (RelativePrecision()>0.) 
    os << val_ << " +/- " << AbsolutePrecision() << " (" << relp << " %) " << unit_.Name();
  else os << val_ << " " << unit_.Name();
  if (lev > 1) { os << " Unit: "; unit_.Print(os, lev); }
  return os;
}


/* --Methode-- */
PhysQty PhysQty::power(QNumber q)  const
{
  Units ru=unit_.power(q);
  double p=(double)q;
  r_8 val=pow(val_,p);
  r_8 prec=prec_*p;
  return PhysQty(ru,val,prec);
}


/* --Methode-- */
/*!
  if fgsum==true, compute the sum r=q1+q2 compute the difference otherwise (fgsum=false) r=q1-q2.
  The resulting quantity Error propagation 
  \warning The two quantities q1 and q2 should have the same unit
*/
PhysQty PhysQty::AddSubtract(PhysQty const& q1, PhysQty const& q2, bool fgsum)
{
  if (! q1.unit_.isEqual(q2.unit_, false))  
    throw ParmError("PhysQty::AddSubtract(q1, q2) q1,q2 do not have the same unit");
  r_8 val;
  if (fgsum) val=q1.val_+q2.val_;
  else val=q1.val_-q2.val_;
  r_8 ap1=q1.AbsolutePrecision();
  r_8 ap2=q2.AbsolutePrecision();
  r_8 abprec=sqrt(ap1*ap1+ap2*ap2);
  r_8 relprec=1.;
  if (fabs(val)>1.e-39)  relprec=abprec/val; 
  return PhysQty(q1.unit_, val, relprec);
}

/* --Methode-- */
/*!
  if fgprod==true, compute the product r=q1*q2 compute the ratio otherwise (fgprod=false) r=q1/q2
*/
  PhysQty PhysQty::MultiplyDivide(PhysQty const& q1, PhysQty const& q2, bool fgprod)
{
  Units ru = q1.unit_ * q2.unit_ ; 
  r_8 val;
  if (fgprod) val=q1.val_*q2.val_;
  else val=q1.val_/q2.val_;
  r_8 rp1=q1.RelativePrecision();
  r_8 rp2=q2.RelativePrecision();
  r_8 relprec=sqrt(rp1*rp1+rp2*rp2);
  return PhysQty(ru, val, relprec);
}

//----------------------------------------------------------
// Classe pour la gestion de persistance de PhysQty
// ObjFileIO<PhysQty>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<PhysQty>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<PhysQty>::WriteSelf() dobj=NULL");
  int_4 ver;
  ver = 1;
  s.Put(ver);   // ecriture numero de version PPF
  // ecriture de l'objet Units et des champs valeurs associes 
  s<< dobj->unit_ << dobj->val_ << dobj->prec_ << dobj->name_;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<PhysQty>::ReadSelf(PInPersist& s)
{
  int_4 ver;
  s.Get(ver);   // Lecture numero de version PPF
  if (dobj == NULL) dobj = new PhysQty();
  // ecriture de l'objet Units et des champs valeurs associes 
  s >> dobj->unit_ >> dobj->val_ >> dobj->prec_ >> dobj->name_;
}

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<Units>
#pragma define_template ObjFileIO<PhysQty>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<Units>;
template class ObjFileIO<PhysQty>;
#endif

} // FIN namespace SOPHYA 
