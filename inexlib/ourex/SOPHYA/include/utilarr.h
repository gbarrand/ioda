// This may look like C code, but it is really -*- C++ -*-
//     Utility classes for template numerical arrays 
//                     R. Ansari, C.Magneville   03/2000

#ifndef UtilArray_SEEN
#define UtilArray_SEEN

#include "machdefs.h"
#include "mutyv.h"
#include "randinterf.h"

#include <stdlib.h>
#include <vector>

namespace SOPHYA {   

/* Quelques utilitaires pour les tableaux (Array) */             

/*! \ingroup TArray
  \typedef Arr_DoubleFunctionOfX
  \brief define a function of double which returns a double
*/
typedef double (* Arr_DoubleFunctionOfX) (double x);
/*! \ingroup TArray
  \typedef Arr_FloatFunctionOfX
  \brief define a function of float which returns a float
*/
typedef float  (* Arr_FloatFunctionOfX)  (float x);

/*! \ingroup TArray
  \typedef Arr_ComplexDoubleFunctionOfX
  \brief define a function of a complex<double> which returns a complex<double>
*/
typedef complex<double> (* Arr_ComplexDoubleFunctionOfX) (complex<double> x);
/*! \ingroup TArray
  \typedef Arr_ComplexFloatFunctionOfX
  \brief define a function of complex<float> which returns a complex<float>
*/
typedef float  (* Arr_ComplexFloatFunctionOfX)  (complex<float> x);

//////////////////////////////////////////////////////////
//! Class to generate a sequence of values
class Sequence {
public:
  virtual ~Sequence();
  virtual MuTyV & Value(sa_size_t k) const = 0;
  inline MuTyV & operator () (sa_size_t k) const { return(Value(k)) ; }
};

class RandomSequence : public Sequence {
public:
  //! to define the generator type
  enum {
    Gaussian = 0, //!< gaussian generator
    Flat = 1      //!< Flat generator
    };

  explicit RandomSequence(int typ = RandomSequence::Gaussian, double m=0., double s=1.);
  explicit RandomSequence(RandomGeneratorInterface& rgen, RNDTypes rtyp=C_RND_Gaussian, double mean=0., double sigma=1.);
  virtual  ~RandomSequence();
  virtual MuTyV & Value(sa_size_t k) const ;
  double   Next() const ;

protected:
  RNDTypes typ_;            //!< random generation type
  double mean_, sig_;  //!< generation parameters mean and sigma (if needed)
  mutable MuTyV retv_;
  mutable RandomGeneratorInterface* rgp_;
};


//////////////////////////////////////////////////////////
//! Class to generate a sequence of values
class RegularSequence : public Sequence {
public:
  explicit RegularSequence (double start=0., double step=1., Arr_DoubleFunctionOfX f=NULL);
  virtual  ~RegularSequence();

  //! return start value of the sequence
  inline double & Start() { return start_; }
  //! return step value of the sequence
  inline double & Step() { return step_; }

  virtual MuTyV & Value(sa_size_t k) const ;

protected:
  double start_;              //!< start value of the sequence
  double step_;               //!< step value of the sequence
  Arr_DoubleFunctionOfX myf_; //!< pointer to the sequence function
  mutable MuTyV retv_;
};

//////////////////////////////////////////////////////////
//! Class for creation and handling of an explicitly defined list of values

class EnumeratedSequence : public Sequence  {
public:
  explicit EnumeratedSequence();
           EnumeratedSequence(EnumeratedSequence const & es);
  virtual ~EnumeratedSequence();
  virtual MuTyV & Value(sa_size_t k) const ;

  inline sa_size_t Size() const { return (sa_size_t)vecv_.size(); } //G.Barrand : cast.

  EnumeratedSequence & operator , (MuTyV const & v);
  EnumeratedSequence & operator = (MuTyV const & v);

  EnumeratedSequence & operator = (EnumeratedSequence const & es);

  inline  void         Clear()  { vecv_.clear(); }      
  void                 Print(ostream& os) const;

  sa_size_t            Append(EnumeratedSequence const & seq);
  sa_size_t            Append(string const & str, int & nbad, const char* sep=" \t");
  sa_size_t            FillFromFile(istream& is, sa_size_t& nr, sa_size_t& nc, 
				    char clm='#', const char* sep=" \t");

private:
  vector<MuTyV> vecv_;
  mutable MuTyV retv_;
};

inline ostream& operator << (ostream& os, const EnumeratedSequence& a)
                            { a.Print(os);    return(os);    }

  //inline EnumeratedSequence operator , (MuTyV const & a, MuTyV const & b)
  //{ EnumeratedSequence seq;   return ((seq,a),b) ; }

//////////////////////////////////////////////////////////
//! Class to define a range of indexes
class Range {
public:
  //! Index range containing a single index = \b start 
  Range(sa_size_t start);
  //! Index range start \<= index \<= end
  Range(sa_size_t start, sa_size_t end); 
  //! Index range start \<= index \<= end with increment step 
  Range(sa_size_t start, sa_size_t end, sa_size_t step);
  //! General Index range specification, using size or start/end and increment
  Range(sa_size_t start, sa_size_t end, sa_size_t size, sa_size_t step);
  //! Return the start index
  inline sa_size_t & Start()  {  return start_; }
  //! Return the last index
  inline sa_size_t & End()    {  return end_; }
  //! Return the size
  inline sa_size_t & Size()   {  return size_; }
  //! Return the step
  inline sa_size_t & Step()   {  return step_; }
  //! return a constant value defining the first element
  inline static sa_size_t firstIndex() { return 0; }
  //! return a constant value as a placeholder for the last valid index 
  inline static sa_size_t lastIndex() { return -1; }
  //! return a Range object specifing all indices, from first to last
  inline static Range all() 
  { return Range(Range::firstIndex() , Range::lastIndex()); }
  //! return a Range object specifing the first index
  inline static Range first()
  { return Range(Range::firstIndex() , Range::firstIndex(), 1, 1); }
  //! return a Range object specifing the last valid index
  inline static Range last()
  { return Range(Range::lastIndex() , Range::lastIndex(), 1, 1); }

  //! For internal TArray module use.
  void Update(sa_size_t osz);

protected:
  sa_size_t start_; //!< start index
  sa_size_t end_;   //!< end index
  sa_size_t size_;  //!< size
  sa_size_t step_;  //!< step
};

//////////////////////////////////////////////////////////
//! Class to define an identity matrix
class IdentityMatrix {
public:
  explicit IdentityMatrix(double diag=1., sa_size_t n=0);
  //! return the size of the identity matrix
  inline sa_size_t Size() { return size_; }
  //! return the value of the diagonal elements
  inline double Diag() { return diag_; }
protected:
  sa_size_t size_; //!< size of the matrix
  double diag_; //!< value of the diagonal elements
};

} // Fin du namespace

#endif
