#include "machdefs.h"
#include <stdio.h>
#include "pqnumber.h"
#include "pexceptions.h"
#include "thsafeop.h"  //  for ThreadSafe operations (Ref.Count/Share) 
#include <iostream>

namespace SOPHYA {

/*!
   \class PrimeNumbers
   \ingroup BaseTools
   A simple class for class for manipulating prime naumbers. This class is thread safe
   
   \code
   // Create a PrimeNumbers object 
   // Create a PrimeNumbers object 
   PrimeNumbers primes;
   // Print the prime numbers from rank 20 to 25 ...
   cout << " Primes[20...30]: " ; 
   for(size_t k=19; k<30; k++) cout << primes(k) << "  "; 
   // Print all prime numbers less than some max values
   uint_8 p = primes.Next();  uint_8 pmax = 223;
   cout << endl << " Primes < " << pmax << " : " << endl;
   while (p<pmax) {
    cout << p << "  ";
    p = primes.Next();
   }
   cout << endl; 
   // Compute and print prime factor decomposition:
   uint_8 nombre=247890;
   vector<uint_8> pfac=primes.PrimeFactors(nombre, true);
   cout << " PrimeFactors[" << nombre << "]: "; 
   for(size_t i=0; i<pfac.size(); i++) cout << pfac[i] << "   ";   
   cout << endl;
   \endcode
*/


//---------------------  Classe PrimeNumbers   ------------------------
static ThSafeOp* primes_gThsop = NULL;  // Mutex global pour thread-safety
std::vector<uint_8> * PrimeNumbers::prime_list_p_ = NULL;  // global prime number list  

/* --Methode-- */ 
PrimeNumbers::PrimeNumbers()
  : my_prime_idx_(0)
{
  Init();  
}

/* --Methode-- */ 
PrimeNumbers::PrimeNumbers(PrimeNumbers const& p)
  : my_prime_idx_(p.my_prime_idx_)
{
  Init();
}


/* --Methode-- */ 
uint_8 PrimeNumbers::Get(size_t k)  const 
{
  uint_8 rp=2;
  while (k>=prime_list_p_->size()) {
    size_t nxt=k-prime_list_p_->size()+1;
    if ((nxt<5)||(nxt < prime_list_p_->size()/16))  Extend(nxt);
    else { 
      double dnl,dnh;
      if (k<40000) encadre6(k, dnl,dnh);
      else encadre40k(k, dnl,dnh);
      uint_8 n = (uint_8)dnh+1;
      //DBG      cout << " DBG*PrimeNumbers::Get(" << k << ")  dnh=" << dnh << " ->int=" << n << " nxt=" << nxt << endl;
      Extend2(n);
    }
  }
  rp= prime_list_p_->at(k);
  return(rp);
}

/* --Methode-- */ 
bool PrimeNumbers::CheckIfPrim(uint_8 n)
{
  if (n<4) return true;
  // On verifie et etend la liste des nombres premiers si besoin
  uint_8 pe=prime_list_p_->at(prime_list_p_->size()-1);
  while (pe*pe<(n+1)) {
    uint_8 pet=pe;
    while (pet*pet<(n+1))  pet+=pe/4;
    Extend2(pet);
    pe=prime_list_p_->at(prime_list_p_->size()-1);
  }
  // On verifie si n est premier 
  PrimeNumbers primes;
  uint_8 p=primes.Next();
  while (p*p<=n) {
    if (n%p==0)  return false;
    p=primes.Next();
  }
  return true;
}

/* --Methode-- */ 
vector<uint_8> PrimeNumbers::PrimeFactors(uint_8 n, bool fgprt)
{
  PrimeNumbers primes;
  std::vector<uint_8> fact;
  if (n<4) {
    if (n==0) fact.push_back(0);
    else if (n==1) fact.push_back(1);
    else if (n==2) fact.push_back(2);
    else fact.push_back(3);
    if (fgprt) cout << "  PrimeFactors(" << n << ")= " << fact[0] << endl;
    return fact;
  }
  uint_8 n0=n;
  uint_8 p=primes.Next();  
  while(n>1) {
    if (n%p==0) { fact.push_back(p); n/=p; }
    else p=primes.Next();
  }
  if (fgprt) { 
    cout << "  PrimeFactors(" << n0 << ")= " << fact[0]; 
    if (fact.size()>1)  {
      for(size_t k=1; k<fact.size(); k++) {
	cout << " x " << fact[k];
	if (k%20==0)  cout << "\n ...   ";
      }
    }
    cout << endl;
  }
  return fact;
}

/* --Methode-- */ 
void PrimeNumbers::encadre6(size_t nieme,double &nlow,double &nhigh)
// encadrement du N-ieme (>=6) nombre premier
// warning: dump if n==1
{
  double n = (double)nieme, ln = log(n), lln = log(log(n));
  nlow  = n * (ln + lln - 1.);
  nhigh = n * (ln + lln);
  return;
}

/* --Methode-- */ 
void PrimeNumbers::encadre40k(size_t nieme,double &nlow,double &nhigh)
// encadrement du N-ieme (>=40000) nombre premier
//     (rigoureusement nieme>=38691)
// warning: dump if n==1
{
  double n = (double)nieme, ln = log(n), lln = log(log(n));
  nlow =  n * (ln + lln - 1.);
  nhigh = n * (ln + lln - 0.948);
  return;
}

/* --Methode-- */ 
double PrimeNumbers::approx(unsigned int nieme)
// approximation du N-ieme
// warning: dump if n==1
{
  double n = nieme, ln = log(n), lln = log(log(n));
  return n * (ln + lln - 1. + (lln-2.)/ln);
}

/* --Methode-- */ 
void PrimeNumbers::Init()
{
  if (primes_gThsop != NULL)  return;
  primes_gThsop = new ThSafeOp;
  primes_gThsop->lock();
  prime_list_p_ = new std::vector<uint_8>(10,1);
  uint_8 firstp[10]={2,3,5,7,11,13,17,19,23,29};
  for(size_t k=0; k<10; k++)  prime_list_p_->at(k)=firstp[k];
  primes_gThsop->unlock();
  return;
}

/* --Methode-- */ 
void PrimeNumbers::Extend(size_t nxt)
{
  primes_gThsop->lock();
  //DBG  cout << "\n DBG*Extend Sz=" << prime_list_p_->size() << " nxt= " << nxt << endl;

  for(size_t i=0; i<nxt; i++) {
    uint_8 n = prime_list_p_->at(prime_list_p_->size()-1)+2;
    bool fgp=false;  
    while(!fgp) {
      fgp=CheckIfPrim_P(n);
      if (fgp) { prime_list_p_->push_back(n);  break; }
      else n+=2;
    }
  }
  primes_gThsop->unlock();
  return;
}

/* --Methode-- */ 
void PrimeNumbers::Extend2(uint_8 P)
{
  uint_8 P0=prime_list_p_->at(prime_list_p_->size()-1);
  if (P0>=P)  return;
  primes_gThsop->lock();
  //DBG cout << "\n DBG*Extend2 Sz=" << prime_list_p_->size() << " P0=" << P0 << " ->P=" << P;
  size_t npremiers;
  unsigned char* liste=eratosthene(P,npremiers);
  for(uint_8 i=P0+1; i<P; i+=2) 
    if (liste[i])  prime_list_p_->push_back(i+1);
  delete[] liste; 
  //DBG cout << "  NewSz=" << prime_list_p_->size() << " LastP=" << prime_list_p_->at(prime_list_p_->size()-1) << endl;
  primes_gThsop->unlock();
  return;
}


/* --Methode-- */ 
bool PrimeNumbers::CheckIfPrim_P(uint_8 n)
{
  size_t k=0;
  uint_8 p=prime_list_p_->at(k);
  while (p*p<=n) {
    if (n%p==0)  return false;
    k++; p=prime_list_p_->at(k);
  }
  return true;
}


/* --Methode-- */ 
unsigned char* PrimeNumbers::eratosthene(uint_8 P, size_t& npremiers)
/*
 -- Le crible d'Eratosthene --
 Trouver tous les nombres premiers inferieurs a un entier P
 - Methode:
   On ecrit tous les nombres de 1 a P
   On elimine tous ceux qui sont divisibles par 2
   On elimine tous ceux qui sont divisibles par 3
   ...
   Ainsi de suite pour tous les nombres les plus petits qui restent.
   Les nombres successifs qui restent sont des nombres premiers.
 - Implementation: routine eratosthene()
     P : on cherche les nombres premiers <= P
     liste : liste[i]=1 si "i+1" est un nombre premier
                     =0 sinon
     Retourne le nombre de nombres premiers <= P
   Attention "liste" est alloue par la routine eratosthene
     et doit etre detruite par le programme appelant
*/
{
  uint_8 i,j,p,pp;
  npremiers=0;
  unsigned char* liste = new unsigned char[P]; 
  for(i=0;i<P;i++) liste[i]=1;

  npremiers = 1;
  for(i=1;i<P;i++) {
    // On teste la primarite du nombre "p" = "i+1"
    p = i+1;
    /* printf("test i=%llu p=%llu liste[i]=%d\n",i,p,(*liste)[i]); */
    if(liste[i]==0) continue;
    npremiers++; pp=2*p; j=pp-1;
    while(j<P) {
      /* printf("  kill j=%llu pp=%llu\n",j,pp); */
      liste[j]=0;
      pp+=p; j=pp-1;
    }
  }
 return liste;
}

/*!
   \class QNumber
   \ingroup BaseTools
   Class representing rational number (q \in Q). All operations are thread-safe.
   
   \code
   // Create rational numbers from pairs of integers
   QNumber q1(3,4), q2(5,6), q3(6,8);
   // Operations and comparsion
   QNumber q4 = q1+q2; 
   cout << q1 << " + " << q2 << " = " << q4 << endl;
   QNumber qa(q1), qb(q2);
   cout << " Test qa==qb : -> " << qa << " ?== " << qb;
   if (qa == qb)  cout << " -> true " << endl;
   else cout << " -> false " << endl;
   qb = q3;
   cout << " Test qa==qb : -> " << qa << " ?== " << qb;
   if (qa == qb)  cout << " -> true " << endl;
   else cout << " -> false " << endl;
   QNumber qq(3*7*17*5,(-5)*7*11*2);
   cout << " qq= " << qq << "  qq.Simplify() = " << qq.Simplify() << endl;
   \endcode
*/

/* --Methode-- */ 
QNumber QNumber::Simplify() const
{
  if (num_==den_) return QNumber(1);
  if (num_==(-den_))  return QNumber(-1);
  uint_8 m=(num_>0)?(uint_8)num_:(uint_8)(-num_);
  uint_8 n=(den_>0)?(uint_8)den_:(uint_8)(-den_);
  QNumber qs=*this;
  PrimeNumbers primes;
  uint_8 p=primes.Next();
  uint_8 petit=(m<n)?m:n;
  while (p*p<=petit) {
    if ((m%p==0)&&(n%p==0)) { m/=p; n/=p; }
    else p=primes.Next();
  }
  int_8 nu=(int_8)m;
  int_8 de=(int_8)n;
  if (num_<0) nu= -nu;
  if (den_<0) de= -de;
  return QNumber(nu,de);
}

/* --Methode-- */ 
std::string QNumber::ToString()
{
  char buff[128];
  if (den_==1)  sprintf(buff,"%ld",(long)num_);
  else sprintf(buff,"(%ld/%ld)",(long)num_,long(den_));
  return buff;
}

/* --Methode-- */ 
QNumber QNumber::Add(QNumber const & a, QNumber const & b, bool fgsimp)
{
  QNumber rq=QNumber(a.num_*b.den_+b.num_*a.den_, a.den_*b.den_);
  if (fgsimp) return rq.Simplify();
  else return rq;
}

/* --Methode-- */ 
QNumber QNumber::Subtract(QNumber const & a, QNumber const & b, bool fgsimp)
{
  QNumber rq=QNumber(a.num_*b.den_-b.num_*a.den_, a.den_*b.den_);
  if (fgsimp) return rq.Simplify();
  else return rq;
}

/* --Methode-- */ 
QNumber QNumber::Multiply(QNumber const & a, QNumber const & b, bool fgsimp)
{
  QNumber rq=QNumber(a.num_*b.num_, a.den_*b.den_);
  if (fgsimp) return rq.Simplify();
  else return rq;
}

/* --Methode-- */ 
QNumber QNumber::Divide(QNumber const & a, QNumber const & b, bool fgsimp)
{
  if (b.num_==0)  throw MathExc("QNumber::Divide(a,b) b=0->divide by zero");  
  QNumber rq=QNumber(a.num_*b.den_, a.den_*b.num_);
  if (fgsimp) return rq.Simplify();
  else return rq;
}



/* --Methode-- */ 
int QNumber::Compare(QNumber const & a, QNumber const & b)
{
  QNumber diff=a-b;
  if (diff.Numerator()==0)  return 0;
  else if (diff.Numerator()>0)  return 1;
  else return -1;
}


} // FIN namespace SOPHYA 
