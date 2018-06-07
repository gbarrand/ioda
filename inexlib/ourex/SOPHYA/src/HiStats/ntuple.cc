#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strutil.h"
#include "perrors.h"
#include "ntuple.h"
#include "thsafeop.h"

namespace SOPHYA {

#define LENNAME 8
#define LENNAME1  (LENNAME+1)
#define BADVAL -1.e19

/*!
   \class NTuple
   \ingroup HiStats
   Simple NTuple class (a Table or 2-D data set) with double or 
   single precision floating point value columns. 
   \warning 
   Thread safe fill operation can be activated using the method SetThreadSafe()
   Default mode is NON thread-safe fill.
   \sa SOPHYA::ObjFileIO<NTuple>

   \code
   #include "ntuple.h"
   // ...
   const char * names[3] = {"XPos", "YPos", "Val"};
   // NTuple (Table) creation with 3 columns (double precision)
   NTuple  nt(3, names);
   // Filling the NTuple
   r_8 x[3];
   for(int i=0; i<63; i++) {
     x[0] = (i%9)-4.;  x[1] = (i/9)-3.;  x[2] = x[0]*x[0]+x[1]*x[1];
     nt.Fill(x);
   }
   // Printing table info
   cout << nt ;
   // Saving object into a PPF file
   POutPersist po("nt.ppf");
   po << nt ;
   \endcode
*/

//++
// Class	NTuple
// Lib	Outils++ 
// include	ntuple.h
//
//	Classe de ntuples
//--
//++
// Links        Parents
// PPersist
// NTupleInterface
//--

/* --Methode-- */
//! Default constructor - To be used when reading in an NTuple.
//++
NTuple::NTuple()
//
//	Createur par defaut
//--
{
mNVar = mNEnt = mBlk = mNBlk = 0;
mVar = NULL;
mVarD = NULL;
mFgDouble = true;
mInfo = NULL;
mThS = NULL; 
}


//! Constructor with specification of number of columns and column names
/*!
  \param nvar : Number of columns in the table 
  \param noms : Array of column names
  \param blk : Optional argument specifying number of table lines
  in a given data block
  \param fgdouble : if \b true: internal data kept as double precision values (r_8),
  simple precision (r_4) otherwise
 */
//++
NTuple::NTuple(int nvar, char** noms, int blk, bool fgdouble)
//
//	Createur d'un ntuple de `nvar' variables dont les
//	noms sont dans le tableau de chaines de caracteres `noms'
//	avec `blk' d'evenements par blocks.
//--
{
if (nvar <= 0)  throw ParmError("NTuple::NTuple(nvar<=0) with char** noms"); 
Initialize(nvar,blk,fgdouble);
for(int i=0; i<nvar; i++) mNames.push_back(noms[i]);
return;
}


//! Constructor with specification of number of columns and column names
/*!
  \param nvar : Number of columns in the table 
  \param noms : Array of column names
  \param blk : Optional argument specifying number of table lines
  in a given data block
  \param fgdouble : if \b true: internal data kept as double precision values (r_8),
  simple precision (r_4) otherwise
 */
//++
NTuple::NTuple(int nvar, const char** noms, int blk, bool fgdouble)
//
//	Createur d'un ntuple de `nvar' variables dont les
//	noms sont dans le tableau de chaines de caracteres `noms'
//	avec `blk' d'evenements par blocks.
//--
{
if (nvar <= 0)  throw ParmError("NTuple::NTuple(nvar<=0) with const char** noms"); 
Initialize(nvar,blk,fgdouble);
for(int i=0; i<nvar; i++) mNames.push_back(noms[i]);
return;
}

//! Constructor with specification of number of columns and column names as a string vector
/*!
  \param noms : Array of column names (length(name) < 8 characters) 
  \param blk : Optional argument specifying number of table lines
  in a given data block
  \param fgdouble : if \b true: internal data kept as double precision values (r_8),
  simple precision (r_4) otherwise
 */
NTuple::NTuple(vector<string>& noms, int blk, bool fgdouble)
{
int nvar = noms.size();
if (nvar <= 0)  throw ParmError("NTuple::NTuple(nvar<=0) with vector<string>& noms");
Initialize(nvar,blk,fgdouble);
for(int i=0; i<nvar; i++) mNames.push_back(noms[i]);
return;
}


/* --Methode-- */
/* Initialisation pour Createurs (fonction privee) */
void NTuple::Initialize(int nvar, int blk, bool fgdouble)
{
mNVar = mNEnt = mBlk = mNBlk = 0;
mVar = NULL;
mVarD = NULL;
mInfo = NULL;
mThS = NULL; 
mNVar = nvar;
mVar = new r_4[nvar];
mVarD = new r_8[nvar];
if (blk < 10) blk = 10;
mBlk = blk;

if (fgdouble) {
  r_8* pt = new r_8[nvar*blk];
  mNBlk = 1;
  mPtrD.push_back(pt);
  mFgDouble = true;
}
else { 
  r_4* pt = new r_4[nvar*blk];
  mNBlk = 1;
  mPtr.push_back(pt);
  mFgDouble = false;
}

return;
}

/*! 
  Copy constructor - Copies the table definition and associated data, 
  as well as the tread safety state
*/
//                                       cmv 8/10/99
//++
NTuple::NTuple(const NTuple& NT)
//
//	Createur par copie (clone).
//--
: mNVar(0), mNEnt(0), mBlk(0), mNBlk(0)
 , mVar(NULL), mVarD(NULL), mInfo(NULL),mThS(NULL)
{
if(NT.mNVar<=0) return; // cas ou NT est cree par defaut
mNVar = NT.mNVar;
mBlk = NT.mBlk;
mVar = new r_4[NT.mNVar];
mVarD = new r_8[NT.mNVar];

mNames = NT.mNames;

int i;
mFgDouble = NT.mFgDouble;
if (mFgDouble) {
  r_8* pt = new r_8[mNVar*mBlk];
  mNBlk = 1; mPtrD.push_back(pt);
  if(NT.mNEnt > 0)
    for(i=0;i<NT.mNEnt;i++) {pt=NT.GetVecD(i,NULL); Fill(pt);}
}
else {
  r_4* pt = new r_4[mNVar*mBlk];
  mNBlk = 1; mPtr.push_back(pt);
  for(i=0;i<NT.mNEnt;i++) {pt=NT.GetVec(i,NULL); Fill(pt);}
}

mNames = NT.mNames;

if(NT.mInfo!=NULL) {mInfo = new DVList; *mInfo = *(NT.mInfo);}
mThS = NULL; 
if(NT.mThS!=NULL) SetThreadSafe(true);
return;
}


/* --Methode-- */
NTuple::~NTuple()
{
Clean();
}

/* --Methode-- */ 
/*!
  \brief Activate or deactivate thread-safe \b Fill() operations
  If fg==true, create an ThSafeOp object in order to insure atomic Fill()
  operations. if fg==false, the ThSafeOp object (mThS) of the target NTuple
  is destroyed. 
  \warning The default Fill() operation mode for NTuples is NOT thread-safe. 
  Please note also that the thread-safety state is NOt saved to PPF (or FITS) streams.
*/
void NTuple::SetThreadSafe(bool fg) 
{
if (fg) {
  if (mThS) return;
  else mThS = new ThSafeOp();
}
else {
  if (mThS) delete mThS;
  mThS = NULL;
}
}

/* --Methode-- */
//! Clear the data table definition and deletes the associated data
void NTuple::Clean()
{
if (mVar) delete[] mVar;
if (mVarD) delete[] mVarD;
if (mInfo) delete mInfo;
if (mThS) delete mThS;
int i;
if(mNBlk>0) { 
  if (mFgDouble) for(i=0; i<mNBlk; i++)  delete[] mPtrD[i];
  else for(i=0; i<mNBlk; i++)  delete[] mPtr[i];
}
if (mFgDouble) mPtrD.erase(mPtrD.begin(), mPtrD.end());
else mPtr.erase(mPtr.begin(), mPtr.end());
mNVar = mNEnt = mBlk = mNBlk = 0;
mVar = NULL;
mVarD = NULL;
mInfo = NULL;
mThS = NULL;
return;
}

/* --Methode--        cmv 08/10/99 */
//! = operator, copies the data table definition and its contents
//++
NTuple& NTuple::operator = (const NTuple& NT)
//
//	Operateur egal (clone).
//--
{
if(this == &NT) return *this;
Clean();
if(NT.mNVar<=0) return *this; // cas ou NT est cree par defaut
mNVar = NT.mNVar;
mBlk = NT.mBlk;
mVar = new r_4[NT.mNVar];
mVarD = new r_8[NT.mNVar];

mNames = NT.mNames;

int i;
mFgDouble = NT.mFgDouble; 
if (mFgDouble) {
  r_8* pt = new r_8[mNVar*mBlk];
  mNBlk = 1; mPtrD.push_back(pt);
  if(NT.mNEnt > 0)
    for(i=0;i<NT.mNEnt;i++) {pt=NT.GetVecD(i,NULL); Fill(pt);}
}
else {
  r_4* pt = new r_4[mNVar*mBlk];
  mNBlk = 1; mPtr.push_back(pt);
  for(i=0;i<NT.mNEnt;i++) {pt=NT.GetVec(i,NULL); Fill(pt);}
}

if(NT.mInfo!=NULL) {mInfo = new DVList; *mInfo = *(NT.mInfo);}

return *this;
}

/* --Methode-- */
//! Adds an entry (a line) to the table
/*!
  \param x : content of the line to be appended to the table
 */
//++
void  NTuple::Fill(r_4* x)
//
//	Remplit le ntuple avec le tableau cd reels `x'.
//--
{
if (mThS)  mThS->lock();   // Thread-safe operation 
int numb = mNEnt/mBlk;
if (numb >= mNBlk) {
  if (mFgDouble) {
    r_8* pt = new r_8[mNVar*mBlk];
    mNBlk++;
    mPtrD.push_back(pt);
  }
  else {
    r_4* pt = new r_4[mNVar*mBlk];
    mNBlk++;
    mPtr.push_back(pt);
  }
}

int offb = mNEnt-numb*mBlk;
if (mFgDouble) 
  for(int i=0; i<mNVar; i++) (mPtrD[numb]+offb*mNVar)[i] = x[i];
else memcpy((mPtr[numb]+offb*mNVar), x, mNVar*sizeof(r_4));
mNEnt++;
if (mThS)  mThS->unlock();   // Thread-safe operation 
return;
}

/* --Methode-- */
//! Adds an entry (a line) to the table (double precision)
/*!
  \param x : content of the line to be appended to the table
 */
//++
void  NTuple::Fill(r_8* x)
//
//	Remplit le ntuple avec le tableau double precision `x'.
//--
{
if (mThS)  mThS->lock();   // Thread-safe operation 
int numb = mNEnt/mBlk;
if (numb >= mNBlk) {
  if (mFgDouble) {
    r_8* pt = new r_8[mNVar*mBlk];
    mNBlk++;
    mPtrD.push_back(pt);
  }
  else {
    r_4* pt = new r_4[mNVar*mBlk];
    mNBlk++;
    mPtr.push_back(pt);
  }
}

int offb = mNEnt-numb*mBlk;
if (mFgDouble) 
  memcpy((mPtrD[numb]+offb*mNVar), x, mNVar*sizeof(r_8));
else
  for(int i=0; i<mNVar; i++) (mPtr[numb]+offb*mNVar)[i] = x[i];

mNEnt++;
if (mThS)  mThS->unlock();   // Thread-safe operation 
return;
}


/* --Methode-- */
//++
float NTuple::GetVal(int n, int k)  const
//
//	Retourne la valeur de la variable `k' de l'evenement `n'.
//--
{
if (n >= mNEnt)   return(BADVAL);
if ( (k < 0) || (k >= mNVar) )    return(BADVAL);
int numb = n/mBlk;
int offb = n-numb*mBlk;
if ( mFgDouble) return(*(mPtrD[numb]+offb*mNVar+k));
else return(*(mPtr[numb]+offb*mNVar+k));
}


/* --Methode-- */
//++
int NTuple::IndexNom(const char* nom)  const
//
//	Retourne le numero de la variable de nom `nom'.
//--
{
int i;
string snom = nom;
for(i=0; i<mNVar; i++)  
  if ( mNames[i] == snom)  return(i);
return(-1);
}


static char nomretour[256];
/* --Methode-- */
//++
char* NTuple::NomIndex(int k)  const
//
//	Retourne le nom de la variable numero 'k'
//--
{
nomretour[0] = '\0';
if ((k >= 0) && (k < mNVar))  strncpy(nomretour, mNames[k].c_str(), 255);
return(nomretour);
}

  
/* --Methode-- */
//++
r_4* NTuple::GetVec(int n, r_4* ret)   const
//
//	Retourne l'evenement `n' dans le vecteur `ret'.
//--
{
int i;
if (ret == NULL)   ret = mVar;
if (n >= mNEnt) {
  for(i=0; i<mNVar; i++)   ret[i] = BADVAL;
  return(ret);
}
  
int numb = n/mBlk;
int offb = n-numb*mBlk;
if (mFgDouble) for(i=0; i<mNVar; i++) ret[i] = (mPtrD[numb]+offb*mNVar)[i];
else  memcpy(ret, (mPtr[numb]+offb*mNVar), mNVar*sizeof(r_4));
return(ret);
}

/* --Methode-- */
//++
r_8* NTuple::GetVecD(int n, r_8* ret)   const
//
//	Retourne l'evenement `n' dans le vecteur `ret'.
//--
{
int i;
if (ret == NULL)   ret = mVarD;
if (n >= mNEnt) {
  for(i=0; i<mNVar; i++)   ret[i] = BADVAL;
  return(ret);
}
  
int numb = n/mBlk;
int offb = n-numb*mBlk;
if (mFgDouble) memcpy(ret, (mPtrD[numb]+offb*mNVar), mNVar*sizeof(r_8));
else for(i=0; i<mNVar; i++) ret[i] = (mPtr[numb]+offb*mNVar)[i];
return(ret);
}

/* --Methode-- */
//++
void  NTuple::GetColumn(int k, vector<r_8> v)  const
//
//	Retourne le contenu de la colonne k
//--
{
  if (k >= mNVar)    return;
  v.resize(NEntry());
  size_t i=0;
  if (mFgDouble) {
    for(int_4 jb=0; jb< mNBlk; jb++)
      for(int_4 ib=0; ib< mBlk; ib++) {
	if (i >= mNEnt)  break;
	i++;
	v[i] = *(mPtrD[jb]+ib*mNVar+k);
      }
  }
  else {
    for(int_4 jb=0; jb< mNBlk; jb++)
      for(int_4 ib=0; ib< mBlk; ib++) {
	if (i >= mNEnt)  break;
	i++;
	v[i] = *(mPtr[jb]+ib*mNVar+k);
      }
  }
  return;
}


/* --Methode-- */
//++
DVList&  NTuple::Info()
//
//	Renvoie une référence sur l'objet DVList Associé
//--
{
if (mInfo == NULL)  mInfo = new DVList;
return(*mInfo);
}

/* --Methode-- */
//++
void  NTuple::Print(int num, int nmax)  const
//
//	Imprime `nmax' evenements a partir du numero `num'.
//--
{
int i,j;

printf("Num     ");
for(i=0; i<mNVar; i++)  printf("%8s ", mNames[i].c_str());
putchar('\n');

if (nmax <= 0)  nmax = 1;
if (num < 0)  num = 0;
nmax += num;
if (nmax > mNEnt) nmax = mNEnt;
for(i=num; i<nmax; i++) {
  GetVec(i, NULL); 
  printf("%6d  ", i);  
  for(j=0; j<mNVar; j++)  printf("%8g ", (float)mVar[j]);
  putchar('\n');
}
return;
}

/* --Methode-- */
//! Prints table definition and number of entries 
//++
void  NTuple::Show(ostream& os)  const
//
//	Imprime l'information generale sur le ntuple.
//--
{
const char * tt = "float"; 
if (mFgDouble) tt = "double";
os << "NTuple T=" << tt << " : NVar= " << mNVar << " NEnt=" << mNEnt  
   << " (Blk Sz,Nb= " << mBlk << " ," << mNBlk << ")\n";
os << "            Variables       Min      Max       \n";
int i;
double min, max;
char buff[128];
for(i=0; i<mNVar; i++) {
  GetMinMax(i, min, max);
  sprintf(buff, "%3d  %16s  %10g  %10g \n", i, mNames[i].c_str(), min, max);
  os << (string)buff ;
  }
os << endl;
}


/* --Methode-- */
//! Fills the table, reading lines from an ascii file
/*!
  \param fn : file name
  \param defval : default value for empty cells in the ascii file 
 */
//++
int  NTuple::FillFromASCIIFile(string const& fn, float defval)
//
//	Remplit le ntuple a partir d'un fichier ASCII.
//	Renvoie le nombre de lignes ajoutees.
//--
{
if (NbColumns() < 1)  { 
  cout << "NTuple::FillFromASCIIFile() Ntuple has " << NbColumns() << " columns" << endl;
  return(-1);
  }
FILE * fip = fopen(fn.c_str(), "r");
if (fip == NULL) {
  cout << "NTuple::FillFromASCIIFile() Error opening file " << fn << endl;
  return(-2);
  }

char lineb[4096];
char *line;
char* ccp;
int j,kk;
int postab, posb;
double* xv = new double[NbColumns()];

int nlread = 0;
int nvar = NbColumns();
// On boucle sur toutes les lignes
while (fgets(lineb,4096,fip) != NULL) {
  lineb[4095] = '\0';
  j = 0; line = lineb;
//  On enleve les espaces et tab de debut 
  while ( (line[j] != '\0') && ((line[j] == ' ') || (line[j] == '\t')) )  j++;
  line = lineb+j; 
// Il faut que le premier caractere non-espace soit un digit, ou + ou - ou .
  if (!( isdigit(line[0]) || (line[0] == '+') || (line[0] == '-') || (line[0] == '.') ))  continue;
  ccp = line;
  for(kk=0; kk<nvar; kk++)  xv[kk] = defval;
  for(kk=0; kk<nvar; kk++) {
// Les mots sont separes par des espaces ou des tab
    postab = posc(ccp, '\t' );
    posb = posc(ccp, ' ' );
    if (postab >= 0) { 
       if (posb < 0) posb = postab;
       else if (postab < posb)  posb = postab; 
       } 
    if (posb >= 0)  ccp[posb] = '\0';
    if ( isdigit(line[0]) || (line[0] == '+') || (line[0] == '-') || (line[0] == '.') ) 
      xv[kk] = atof(ccp);
    if (posb < 0)  break;
    ccp += posb+1;   j = 0;
    while ( (ccp[j] != '\0') && ((ccp[j] == ' ') || (ccp[j] == '\t')) )  j++;
    ccp += j;
    }
  Fill(xv);
  nlread++;
  }

delete[] xv;
cout << "NTuple::FillFromASCIIFile( " << fn << ") " << nlread << " fill from file " << endl;
return(nlread);
}


// ------- Implementation de  l interface NTuple  ---------

/* --Methode-- */
size_t NTuple::NbLines() const
{
return(NEntry());
}
/* --Methode-- */
size_t NTuple::NbColumns() const
{
return(NVar());
}

/* --Methode-- */
r_8 * NTuple::GetLineD(size_t n) const
{
return(GetVecD(n));
}

/* --Methode-- */
r_8 NTuple::GetCell(size_t n, size_t k) const
{
return(GetVal(n, k));
}

/* --Methode-- */
r_8 NTuple::GetCell(size_t n, string const & nom) const
{
return(GetVal(n, nom.c_str()));
}

/* --Methode-- */
//++
void  NTuple::GetMinMax(size_t k, double& min, double& max)  const
//
//	Retourne le minimum et le maximum de la variable `k'.
//--
{
min = 9.e19; max = -9.e19;
if (k >= mNVar)    return;
int jb,ib,i;
double x;
i=0;
for(jb=0; jb< mNBlk; jb++)
  for(ib=0; ib< mBlk; ib++) {
    if (i >= mNEnt)  break;
    i++;
    x = (mFgDouble) ? *(mPtrD[jb]+ib*mNVar+k) : *(mPtr[jb]+ib*mNVar+k);
    if(i==1) {min = x; max = x;}
    if (x < min)  min = x;
    if (x > max)  max = x;
  }
return;
}

/* --Methode-- */
void NTuple::GetMinMax(string const & nom, double& min, double& max)   const
{
GetMinMax(IndexNom(nom.c_str()), min, max);
}

/* --Methode-- */
size_t NTuple::ColumnIndex(string const & nom)  const
{
return(IndexNom(nom.c_str()));
}

/* --Methode-- */
string NTuple::ColumnName(size_t k) const
{
return(NomIndex(k));
}

/* --Methode-- */
//++
string NTuple::VarList_C(const char* nomx)  const
//
//	Retourne une chaine de caracteres avec la declaration des noms de 
//	variables. si "nomx!=NULL" , des instructions d'affectation
//	a partir d'un tableau "nomx[i]" sont ajoutees. 
//--
{
string rets="";
int i;
for(i=0; i<mNVar; i++) {
  if ( (i%5 == 0) && (i > 0) )  rets += ";";  
  if (i%5 == 0)   rets += "\ndouble "; 
  else rets += ",";
  rets += mNames[i];
  }
rets += "; \n";
if (nomx) { 
  char buff[256];
  for(i=0; i<mNVar; i++) {
    sprintf(buff,"%s=%s[%d]; ",  mNames[i].c_str(), nomx, i);
    rets += buff;
    if ( (i%3 == 0) && (i > 0) )  rets += "\n"; 
    }
  }

return(rets);
}


/* --Methode-- */
//++
string NTuple::LineHeaderToString() const 
//	Retourne une chaine de caracteres avec la liste des noms de  
//	variables, utilisables pour une impression
//--
{
char buff[32];
string rets=" Num    ";
for(int i=0; i<mNVar; i++) {
  sprintf(buff, "%8s ", mNames[i].c_str());
  rets += buff;
  }
rets += '\n';
return(rets);
}

/* --Methode-- */
//++
string NTuple::LineToString(size_t n) const
//	Retourne une chaine de caracteres avec le contenu de la ligne "n"
//	utilisable pour une impression
//--
{
char buff[32];
double* val;
val = GetLineD(n);
 sprintf(buff,"%7ld: ",(long)n);  
string rets=buff;
int i;
for(i=0; i<mNVar; i++) {
  sprintf(buff, "%8.3g ", val[i]);
  rets += buff;
  }
rets += '\n';
return(rets);
}

/*!
   \class ObjFileIO<NTuple>
   \ingroup HiStats
   Persistence (serialisation) handler for class NTuple
*/

/* --Methode-- */ 
//++
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void   ObjFileIO<NTuple>::WriteSelf(POutPersist& s)  const
//
//	Ecriture ppersist du ntuple.
//--
{
if (dobj == NULL)   return;

// On ecrit cette chaine pour compatibilite avec les anciennes version (1,2) 
string strg = "NTuple";
s.Put(strg);

//  On ecrit 3 uint_4 .... 
//  [0]: Numero de version ;
//  [1] : bit1 non nul -> has info, bit 2 non nul mFgDouble=true 
//  [2] : reserve
uint_4 itab[3];
itab[0] = 3;  // Numero de version a 3
itab[1] = itab[2] = 0;
if (dobj->mInfo)  itab[1] = 1;
if (dobj->mFgDouble) itab[1] += 2;
s.Put(itab, 3);

s.Put(dobj->mNVar);
for(int k=0; k<dobj->mNVar; k++) s.Put(dobj->mNames[k]);
s.Put(dobj->mNEnt);
s.Put(dobj->mBlk);
s.Put(dobj->mNBlk);

if (dobj->mInfo)  s << (*(dobj->mInfo));
int jb;
if (dobj->mFgDouble) 
  for(jb=0; jb<dobj->mNBlk; jb++)
    s.Put(dobj->mPtrD[jb], dobj->mNVar*dobj->mBlk); 
else 
  for(jb=0; jb<dobj->mNBlk; jb++)
    s.Put(dobj->mPtr[jb], dobj->mNVar*dobj->mBlk); 
return;
}

/* --Methode-- */
//++
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void  ObjFileIO<NTuple>::ReadSelf(PInPersist& s)
//
//	Lecture ppersist du ntuple.
//--
{

if (dobj == NULL) dobj = new NTuple;
else dobj->Clean();

bool hadinfo = false;
string strg;
s.Get(strg);
uint_4 itab[3] = {0,0,0};
if (strg == "NTuple") {
  s.Get(itab, 3);
  if ( ((itab[0] < 3) && (itab[1] != 0)) || 
       ((itab[0] >= 3) && ((itab[1]&1) == 1)) ) hadinfo = true;
}
else {
// Ancienne version de PPF NTuple - Pour savoir s'il y avait un DVList Info associe
  char buff[256];
  strcpy(buff, strg.c_str());
  if (strncmp(buff+strlen(buff)-7, "HasInfo", 7) == 0)  hadinfo = true;
}
if (itab[0] < 3) {  // Lecture version anterieures V= 1 , 2 
  s.Get(dobj->mNVar);
  dobj->mVar = new r_4[dobj->mNVar];
  dobj->mVarD = new r_8[dobj->mNVar];
  
  char * names = new char[dobj->mNVar*LENNAME1];
  s.GetBytes(names, dobj->mNVar*LENNAME1);
  for(int k=0; k<dobj->mNVar; k++) dobj->mNames.push_back(names+k*LENNAME1);
  s.Get(dobj->mNEnt);
  s.Get(dobj->mBlk);
  s.Get(dobj->mNBlk);  
  if (hadinfo) {    // Lecture eventuelle du DVList Info
    if (dobj->mInfo == NULL)  dobj->mInfo = new DVList;
    s >> (*(dobj->mInfo));
  }
  // Il n'y avait que des NTuple avec data float pour V < 3
  dobj->mFgDouble = false;   
  int jb; 
  for(jb=0; jb<dobj->mNBlk; jb++) {
    r_4* pt = new r_4[dobj->mNVar*dobj->mBlk];
    dobj->mPtr.push_back(pt);
    s.Get(dobj->mPtr[jb], dobj->mNVar*dobj->mBlk); 
  }
}
else {  // Lecture version V 3 
  s.Get(dobj->mNVar);
  dobj->mVar = new r_4[dobj->mNVar];
  dobj->mVarD = new r_8[dobj->mNVar];
  string nom;
  for(int k=0; k<dobj->mNVar; k++) { 
    s.Get(nom);
    dobj->mNames.push_back(nom);
  }
  s.Get(dobj->mNEnt);
  s.Get(dobj->mBlk);
  s.Get(dobj->mNBlk);  
  if (hadinfo) {    // Lecture eventuelle du DVList Info
    if (dobj->mInfo == NULL)  dobj->mInfo = new DVList;
    s >> (*(dobj->mInfo));
  }
  // Il n'y avait que des NTuple avec data float pour V < 3
  dobj->mFgDouble =  ((itab[1]&2) == 2) ? true : false;   
  int jb; 
  if (dobj->mFgDouble) {
    for(jb=0; jb<dobj->mNBlk; jb++) {
      r_8* pt = new r_8[dobj->mNVar*dobj->mBlk];
      dobj->mPtrD.push_back(pt);
      s.Get(dobj->mPtrD[jb], dobj->mNVar*dobj->mBlk); 
    }
  }
  else {
    for(jb=0; jb<dobj->mNBlk; jb++) {
      r_4* pt = new r_4[dobj->mNVar*dobj->mBlk];
      dobj->mPtr.push_back(pt);
      s.Get(dobj->mPtr[jb], dobj->mNVar*dobj->mBlk); 
    }
  }
}  // Fin lecture V3 

}


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<NTuple>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<NTuple>;
#endif

} // FIN namespace SOPHYA 
