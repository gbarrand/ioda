//     Utility classes for template numerical arrays
//                     R. Ansari, C.Magneville   03/2000

#include "sopnamsp.h"
#include "machdefs.h"
#include "utilarr.h"
#include "randr48.h"

// Classe utilitaires

Sequence::~Sequence()
{
}

//////////////////////////////////////////////////////////
/*!
  \class SOPHYA::RandomSequence
  \ingroup TArray
  Base class to generate a sequence of random values
*/

/*!
  \brief constructor using a global default random generator ( RandomGeneratorInterface::GetGlobalRandGenP() )
  \param typ : generated random numbers distribution type: C_RND_Flat,C_RND_Gaussian,C_RND_Poisson,C_RND_Exponential
  \param mean : mean value of the distribution 
  \param sigma : standard deviation of the distribution 
 */
RandomSequence::RandomSequence(int typ, double mean, double sigma)
{
  typ_ = (typ == Flat) ? C_RND_Flat: C_RND_Gaussian ;
  mean_ = mean;
  sig_ = sigma;
  rgp_ = RandomGeneratorInterface::GetGlobalRandGenP();
}


/*!
  \brief constructor with the specification of a RandomGenerator object to be used.
  \param rgen : RandomGeneratorInterface object that should stay valid as long as the RandomSequence object exist.
  \param typ : generated random numbers distribution type: C_RND_Flat,C_RND_Gaussian,C_RND_Poisson,C_RND_Exponential
  \param mean : mean value of the distribution 
  \param sigma : standard deviation of the distribution 
 */
RandomSequence::RandomSequence(RandomGeneratorInterface& rgen, RNDTypes rtyp, double mean, double sigma)
{
  typ_ = rtyp;
  mean_ = mean;
  sig_ = sigma;
  rgp_ = &rgen;
}

RandomSequence::~RandomSequence()
{
}

double RandomSequence::Next() const 
{
  switch (typ_) {
    case C_RND_Flat :
      return rgp_->Flatpm1()*sig_ + mean_;
      break; 
    case C_RND_Gaussian :
      return rgp_->Gaussian(sig_, mean_);
      break; 
    case C_RND_Poisson :
      return rgp_->Poisson(mean_);
      break; 
    case C_RND_Exponential :
      return rgp_->Exponential();
      break; 
    default :
      return rgp_->Flat01();
      break; 

  }
}

//!  \brief Return the next random number 
MuTyV & RandomSequence::Value(sa_size_t k) const 
{
  retv_ = Next();
  return retv_;
}


//////////////////////////////////////////////////////////
/*!
  \class SOPHYA::RegularSequence
  \ingroup TArray
  Class to generate a sequence of values
*/

//! Constructor
/*!
  \param start : start value
  \param step : step value
  \param f : pointer to the sequence function (default = NULL, f(x)=x )

  See \ref RegularSequenceOperat "operator()"
 */
RegularSequence::RegularSequence(double start, double step, Arr_DoubleFunctionOfX f)
{
  start_ = start;
  step_ = step;
  myf_ = f;
}

RegularSequence::~RegularSequence()
{
}

//! Get the \b k th value
/*!
  \param k : index of the value
  \anchor RegularSequenceOperat
  
  \return f(start+k*step)

 */

MuTyV & RegularSequence::Value (sa_size_t k) const 
{
  double x = start_+(double)k*step_;
  if (myf_)  x = myf_(x);
  retv_ = x;
  return(retv_);
}

/*!
  \class SOPHYA::EnumeratedSequence
  \ingroup TArray
  Explicitly defined sequence of values. The comma operator has
  been redefined to let an easy definition of sequences.
  
  \code
  // Initializing a sequence 
  EnumeratedSequence es;
  es = 11, 22, 33, 44, 55, 66;
  
  for(int k=0; k<8; k++) 
    cout << " k= " << k << " es(k)= " << es(k) << endl; 

  // Decoding a sequence from a string
  EnumeratedSequence ess;
  int nbad;
  ess.Append("56.5 (1.,-1.) 4 8 16", nbad);
  cout << ess;
  \endcode
*/

//! Default constructor
EnumeratedSequence::EnumeratedSequence()
{
}

//! Copy constructor
EnumeratedSequence::EnumeratedSequence(EnumeratedSequence const & es)
{
  Append(es);
}

EnumeratedSequence::~EnumeratedSequence()
{
}

//! Return the k th value in the sequence (default = 0)
MuTyV & EnumeratedSequence::Value (sa_size_t k) const 
{
  if (k >= (sa_size_t)vecv_.size())  retv_ = 0;
  else retv_ = vecv_[k];
  return(retv_);
}

//! Appends a new value to the sequence
EnumeratedSequence & EnumeratedSequence::operator , (MuTyV const & v)
{
  vecv_.push_back(v);
  return(*this);
}

//! Initialize the sequence with a single value \b v
EnumeratedSequence & EnumeratedSequence::operator = (MuTyV const & v)
{
  vecv_.clear();
  vecv_.push_back(v);
  return(*this);
}

//! Copy operator
EnumeratedSequence & EnumeratedSequence::operator = (EnumeratedSequence const & seq)
{
  Clear();
  Append(seq);
  return(*this);  
}


//! Prints the list to the output stream \b os
void EnumeratedSequence::Print(ostream& os) const 
{
  os << " EnumeratedSequence::Print() - Size()= " << Size() << endl;
  for(size_t k=0; k<vecv_.size(); k++) {
    os << vecv_[k];
    if ((k > 0) && (k%10 == 0))  os << endl;
    else os << " " ;
  }
  os << endl;
  return;
}

//! Append the \b seq to the end of the sequence.
/*!
  \return the number of added elements.
*/ 
sa_size_t EnumeratedSequence::Append(EnumeratedSequence const & seq)
{
  for(size_t k=0; k<seq.vecv_.size(); k++) 
    vecv_.push_back(seq.vecv_[k]);
  return(seq.vecv_.size());
}

//! Decodes the string, appending values to the end of the sequence.
/*!
  \param str : string to be decoded
  \param nbad : number of unmatched quotes or parenthesis (returned value)
  \param sep : word separator in string. Each word is decoded as a MuTyV value.
  \return the number of added elements.
*/ 
sa_size_t EnumeratedSequence::Append(string const & str, int& nbad, const char* sep)
{
  nbad = 0;
  sa_size_t n = 0;
  size_t l = str.length();
  if (l < 1) return(0);
  //  if ((str[0] == '#') || (str[0] == '*')) return(0);
  size_t q = 0;
  size_t p = 0;
  /*
  size_t p = str.find_first_not_of(sep);
  if ((str[p] == '+') || (str[p] == '-')) {
    if (p == l-1)  return(0);
    if (!isdigit(str[p+1])) return(0);
  }
  else if (!isdigit(str[p]) && (str[p] != '\'') && (str[p] != '(') ) return(0);
  */
  while(q < l) {
    p = str.find_first_not_of(sep,q);
    if (p >= l)  break;
    if (str[p] == '\'')    {  // Decodage d'un string
      q = str.find('\'',p+1);
      if (q < l) {
	vecv_.push_back(MuTyV(str.substr(p+1,q-p-1)));
	n++;	q++;
      }
      else nbad++;
    }
    else if (str[p] == '(')    {  // Decodage d'un complex
      q = str.find(')',p);
      if (q < l) {
	q++;
	MuTyV mtv(str.substr(p,q-p));
	complex<r_8> z = mtv.operator complex<r_8>();
	vecv_.push_back(MuTyV(z));
	n++;
      }
      else nbad++;      
    }
    else { 
      q = str.find_first_of(sep,p);
      if ( !isdigit(str[p]) && !(str[p] == '+')
        && !(str[p] == '-') && !(str[p] == '.') ) { // une chaine
	vecv_.push_back(MuTyV(str.substr(p,q-p)));
        n++;
      }
      else {  // C'est un nombre 
	if (str.find_first_of(".eE",p) < q)   { // c'est un flottant
	  r_8 x = atof(str.substr(p,q-p).c_str());
	  vecv_.push_back(MuTyV(x));
	}
	else {   // un entier
	  int_8 l = atol(str.substr(p,q-p).c_str());
	  vecv_.push_back(MuTyV(l));
	}
	n++;
      }
    }
  }
  return (n);
}

//! Decodes the input ASCII stream, creating a sequence of values
/*! \param is : Input ASCII stream
    \param nr : Number of non empty (or comment) lines in stream (return value)
    \param nc : Number of columns (= ntot/nlines) (return value)
    \param clm : Lines starting with clm character are treated as comment lines
    \param sep : word separator in lines
    \return Number of decoded elements
*/
sa_size_t EnumeratedSequence::FillFromFile(istream& is, sa_size_t& nr, sa_size_t& nc,
					   char clm, const char* sep)
{
  nr = 0;
  nc = 0;
  sa_size_t n = 0;
  //char buff[256];
  string line;
  int nbad, nbadtot, nel;
  nbadtot = nbad = 0;
  while (!is.eof()) {
  /* Reza, Juin 2005 : Remplace par getline(istream, string) - plus sur
     is.clear(); 
     is.getline(buff, 256);    line += buff;  nel = 0; */
    line = "";
    getline(is, line);
    if (is.good() || is.eof()) {
      if ((line.length() > 0) && (line[0]!=clm)) {
	nel = Append(line, nbad, sep);
	if (nel > 0)  { 
	  nr++;  n += nel;
	}
	nbadtot += nbad;
      }
    }
  }
/* Reza, Juin 2005 : plus necessaire
  if ((line.length() > 0) && (line[0]!=clm)) {
    nel = Append(line, nbad, sep);
    if (nel > 0)  {       nr++;  n += nel; }
    nbadtot += nbad;    line = ""; }
*/
  if (nbadtot > 0) 
    cout << "EnumeratedSequence::FillFromFile()/Warning " << nbadtot 
	 << " bad match (quotes or parenthesis) in stream " << endl;
  nc = n/nr;
  return (n);
}

//////////////////////////////////////////////////////////
/*!
  \class SOPHYA::Range
  \ingroup TArray
  This class can be used to define a range of indices. Range objects are used to extract 
  sub-arrays and slices, from arrays and matrices. 
  \sa SOPHYA::TArray SOPHYA::TMatrix
*/

/*!
  Constructor defining a range corresponding to the single index \b start
  \param start : start=end index 
*/
Range::Range(sa_size_t start)
{
  start_ = start;
  end_ = start;
  size_ = 1;
  step_ = 1;
}

/*!
  Constructor defining defining the range of indices, from \b start to \b end
  \param start : start index (inclusive)
  \param end : end index (inclusive)
*/
Range::Range(sa_size_t start, sa_size_t end)
{
  start_ = start;
  end_ = end;
  if (end >= start)   size_ = end-start+1;
  else size_ = 0;
  step_ = 1;
}

/*!
  Constructor defining defining the range of indices, from \b start to \b end
  \param start : start index (inclusive)
  \param end : end index (inclusive)
  \param step : step (or stride) = index increment
*/
Range::Range(sa_size_t start, sa_size_t end, sa_size_t step)
{
  start_ = start;
  end_ = end;
  step_ = (step > 0) ? step : 1;
  if (step < 1) step = 1;
  if (end >= start) 
    size_ = (end-start)/step_+1;
  else size_ = 0;
}

/*!
  Define a range of indices
  \param start : start index (inclusive)
  \param end : end index (inclusive, used if size \<= 0 and end \>= start)
  \param size : size (number of elements, used if \>= 1 )
  \param step : step (or stride) = index increment

  \warning If \b size \>= 1 , \b end index computed automatically. 
  If \b size \< 1 and \b end < \b start , equivalent to \b end = Range()::lastIndex() 
 */
Range::Range(sa_size_t start, sa_size_t end, sa_size_t size, sa_size_t step)
{
  start_ = start;
  step_ = (step > 0) ? step : 1;
  if (size > 0) {  // Nb d'elements fixe
    size_ = size;
    if ( (start == end) && (end == Range::lastIndex()) ) start_ = end_ = end;
    else end_ = start_+(size_-1)*step_;    
  }
  else {
    if (end >= start) {  // Indice de fin fixe 
      end_ = end;
      size_ = (end-start)/step_+1;
    }
    else {  // rien fixe
      size_ = 0;
      end_ = Range::lastIndex();
    }
  }
}

/*
Range::Range(Range const& a)
{
  start_ = a.start_;
  end_ = a.end_;
  size_ = a.size_;
  step_ = a.step_;
}
*/

/*!
  This method is called to recompute index ranges, specifying the original array size
  by the TArray<T> (or derived classes) sub array extraction methods
*/ 
void Range::Update(sa_size_t osz)
{
  if (end_ >= 0)  return; 
  if (osz == 0) {
    start_ = end_ = 0;
    size_ = step_ = 1;
    return;
  }
  if (end_ == start_) {
    end_ = osz-1; 
    if ((size_ > 0) && (size_ <= osz/step_)) 
      start_ = end_ - (size_-1)*step_;
    else {
      start_ = end_; 
      size_ = 1;
    }
  }
  else {
    end_ = osz-1; 
    size_ = (end_-start_)/step_+1;
  }
  //DBG  cout << ">>> DBG/Update start=" << start_ << " end=" << end_ 
  //DBG     << " size=" << size_ << " step=" << step_ << endl;
  return;
}

/*
Range & Range::operator = (sa_size_t start)
{
  start_ = start;
  size_ = 1;
  step_ = 1;
  return (*this);
}
*/


//////////////////////////////////////////////////////////
/*!
  \class SOPHYA::IdentityMatrix
  \ingroup TArray
  Class to define an identity matrix. This class is mainly intented for 
  initializing TMatrix<T> objects, proportional to the identity matrix. 
*/

//! Constructor representing a square matrix (\b n x \b n) with value \b diag on the diagonal
IdentityMatrix::IdentityMatrix(double diag, sa_size_t n)
{
  size_ = n;
  diag_ = diag;
}
