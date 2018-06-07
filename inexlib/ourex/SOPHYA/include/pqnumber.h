// This may look like C code, but it is really -*- C++ -*-
//-----------------------------------------------------------
// Class  PrimesNumbers (prime numbers)
// Class  QNumber (Rational numbers)
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2012
//-----------------------------------------------------------

#ifndef PQNUMBER_H_SEEN
#define PQNUMBER_H_SEEN


#include "machdefs.h"
#include <string> 
#include <vector> 
#include <iostream> 

#include "pexceptions.h"
#include "ppersist.h"


namespace SOPHYA {

//--------------------------------------------------------------------------------   
//------------------------------ PrimeNumbers class ------------------------------
//--------------------------------------------------------------------------------   

//! Class providing list of prime number
class PrimeNumbers {
public:
  //! Default constructor 
  PrimeNumbers();
  //! Copy constructor
  PrimeNumbers(PrimeNumbers const& p);

  //! return the next prime number 
  inline uint_8 Next()
  { uint_8 rp=Get(my_prime_idx_); my_prime_idx_++; return rp; }
  //! rewind / reset prime number index for the Next() method
  inline void Rewind() { my_prime_idx_=0; } 
  //! Reset prime number index for the Next() method
  inline void ResetIndex() { my_prime_idx_=0; } 

  //! return the \b k th prime number 
  uint_8 Get(size_t k) const;
  //! return the \b k th prime number (operator [] overload)
  inline uint_8 operator[](size_t k) const { return Get(k); }
  //! return the \b k th prime number (operator () overload)
  inline uint_8 operator()(size_t k) const { return Get(k); }
  //! return true if the argument \b n is a prime number 
  static bool CheckIfPrim(uint_8 n);
  //! return the prime number factorization of \b n 
  static std::vector<uint_8> PrimeFactors(uint_8 n, bool fgprt=false);

  //! Computes an interval for the n-th (n>=6) prime number 
  static void encadre6(size_t nieme,double &nlow,double &nhigh);
  //! Computes an interval for the n-th (n>=40000) prime number 
  static void encadre40k(size_t nieme,double &nlow,double &nhigh);
  //! Return an approximate value for the n-th prime number 
  static double approx(unsigned int nieme);

private:
  //! Global initialization (Mutex object  and prime number array creation)
  static void Init();
  //! Extends the prime number array by finding the next \b nxt prime numbers 
  static void Extend(size_t nxt);
  //! Extends the prime number array by finding all prime numbers p such as p<=n
  static void Extend2(uint_8 n);

  //! Static private member used by Extend() to check if a number is prime
  static bool CheckIfPrim_P(uint_8 n);
  //! Static private member to find all prime numbers less than P. return the list of flags.
  static unsigned char* eratosthene(uint_8 P, size_t& npremiers);

  size_t my_prime_idx_;   // prime number index for Next() method
  static std::vector<uint_8> * prime_list_p_;   // global prime number array
};

//--------------------------------------------------------------------------------   
//---------------------- QNumber class : rational numbers ------------------------
//--------------------------------------------------------------------------------   

//! Class representing rational numbers : q = m/n with m,n integers)
class QNumber {  // : public AnyDataObj {
public:
  //! Default constructor, with 0 value (= 0 / 1) 
  explicit QNumber()
  { num_=0; den_=1; }
  //! Constructor from two integers, numerator and denominator
  QNumber(int_8 m, int_8 n=1)
  { 
    if (n==0)  throw MathExc("QNumber(m,n) n=0->null denominator");  
    num_=m;  den_=n; 
    if (den_<0)  { num_=-num_; den_=-den_; } 
  }
  //! Copy constructor
  QNumber(QNumber const& q)
  { num_=q.num_;  den_=q.den_; }

  //! copy (equal) operator 
  inline QNumber& operator = (QNumber const & q) 
  { num_=q.num_;  den_=q.den_; return(*this); }   

  //! Return the simplified rational number (division by all common divisors of numerator and denominator)
  QNumber Simplify() const; 

  //! Convert to decimal (double precision) number 
  inline operator double() const { return (double)num_/(double)den_; }
  //! Return the numerator
  inline int_8 Numerator() const { return num_; }
  //! Return the denominator
  inline int_8 Denominator() const { return den_; }

  //! Return the opposite (-q) of the rational number. Does NOT change the object itself.
  inline QNumber OppositeSign() const 
  { return QNumber(-num_, den_); }
  //! Return the inverse (1/q) of the rational number. Does NOT change the object itself.
  inline QNumber Inverse()  const 
  { return QNumber(den_, num_); }

  //! Return true if the rational number is equal to zero , false otherwise
  inline bool isZero()  const
  { return ( (num_==0)?true:false ); }
  //! Return true if the rational number represents an integer , false otherwise
  inline bool isInteger()  const
  { return ((Simplify().Denominator()==1)?true:false); }

  //! Return the rational number in string format (num/den) 
  std::string ToString(); 
  //! Prints the rational number on \b cout  (return the cout stream object)
  inline ostream&  Print()  const  
  { return Print(cout); }  
  //! Prints the rational number on stream \b os (return the os stream object)
  inline ostream& Print(ostream& os)  const
  { 
    os << '(' << num_ << '/' << den_ << ')'; 
    return os;
  } 

  //! Add two QNumbers (simplify the result if fgsimp true)
  static QNumber  Add(QNumber const & a, QNumber const & b, bool fgsimp=true);
  //! Subtract two QNumbers (simplify the result if fgsimp true)
  static QNumber  Subtract(QNumber const & a, QNumber const & b, bool fgsimp=true);
  //! Multiply two QNumbers (simplify the result if fgsimp true)
  static QNumber  Multiply(QNumber const & a, QNumber const & b, bool fgsimp=true);
  //! Divide two QNumbers (simplify the result if fgsimp true)
  static QNumber  Divide(QNumber const & a, QNumber const & b, bool fgsimp=true);
  //! Compare two QNumbers: return -1 (a<b), 0 (a==b), 1 (a>b)  
  static int Compare(QNumber const & a, QNumber const & b);

protected:
  int_8 num_,den_;      // numerateur et denominateur 
};


/*! operator << overloading - prints the QNumber q on \b s*/
inline ostream& operator << (ostream& s, QNumber const& q)
  {  return q.Print(s);  }
/*! operator >> overloading - reads the QNumber q from the input stream \b s as a pair of
    ascii formatted, / separated integer numbers, enclosed in parenthesis, without spaces. */
inline istream& operator >> (istream& s, QNumber& q)
{  
  int_8 n,d; char c1,c2,c3; 
  s >> c1 >> n >> c2 >> d >> c3;
  if ((c1=='(')&&(c2=='/')&&(c3==')')) q=QNumber(n,d);  
  return(s);  
}

/*! Sum operator definition for QNumbers : r = a + b  */
inline QNumber operator + (QNumber a, QNumber b)
  { return QNumber::Add(a,b); }

/*! Difference (subtraction) operator definition for QNumbers : r = a - b  */
inline QNumber operator - (QNumber a, QNumber b)
  { return QNumber::Subtract(a,b); }

/*! Product (multiplication) operator definition for QNumbers : r = a * b  */
inline QNumber operator * (QNumber a, QNumber b)
  { return QNumber::Multiply(a,b); }

/*! Divide operator definition for QNumbers : r = a / b  */
inline QNumber operator / (QNumber a, QNumber b)
  { return QNumber::Divide(a,b); }

/*! Equality comparison operator == for QNumbers : return true if (a == b)  */
inline bool operator == (QNumber a, QNumber b)
{ return ( QNumber::Compare(a,b)==0); }

/*! Not equal comparison operator != for QNumbers : return true if (a != b)  */
inline bool operator != (QNumber a, QNumber b)
{ return ( QNumber::Compare(a,b)!=0); }

/*! Comparison operator < for QNumbers : return true if (a < b)  */
inline bool operator < (QNumber a, QNumber b)
{ return ( QNumber::Compare(a,b)<0); }

/*! Comparison operator > for QNumbers : return true if (a > b)  */
inline bool operator > (QNumber a, QNumber b)
{ return ( QNumber::Compare(a,b)>0); }

/*! Comparison operator <= for QNumbers : return true if (a <= b)  */
inline bool operator <= (QNumber a, QNumber b)
{ return ( QNumber::Compare(a,b)<=0); }

/*! Comparison operator >= for QNumbers : return true if (a >= b)  */
inline bool operator >= (QNumber a, QNumber b)
{ return ( QNumber::Compare(a,b)>=0); }

/*! Writes the two integers representing a QNumber in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, QNumber const& q)
{ os << '(' << q.Numerator() << '/' << q.Denominator() << ')'; return(os); }
/*! Reads the object from the PInPersist stream \b is  */
inline PInPersist& operator >> (PInPersist& is, QNumber& q)
{ int_8 num,den; char c; is >> c >> num >> c >> den >> c; q = QNumber(num,den);  return(is); }


} // namespace SOPHYA

#endif
