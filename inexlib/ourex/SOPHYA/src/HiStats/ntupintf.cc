
#include "sopnamsp.h"
#include "ntupintf.h"
#include <stdlib.h>
#include <stdio.h>
#include "pexceptions.h"

/*!
   \class SOPHYA::NTupleInterface
   \ingroup HiStats
   Interface class (pure virtual) defining generic operations on NTuple and DataTable
   a 2-dimensional data set with named columns
*/

//++
// Class	NTupleInterface
// Lib	        HiStats
// include	ntupintf.h
//
//	Interface permettant de construire des vues NTuples (tableau lignes-colonnes) 
//	pour differents objets. Toutes les methodes ont une implementation par defaut. 
//--

//++
// NTupleInterface()
//	Constructeur.
// virtual ~NTupleInterface()
//	Destructeur
// uint_4  NbLines() const 
//	Nombre de lignes du tableau
// uint_4  NbColumns() const 
//	Nombre de colonnes du tableau
// r_8 *  GetLineD(int n) const
//	Renvoie un tableau avec le contenu de la ligne "n"
// r_8	GetCell(int n, int k) const
//	Renvoie le contenu de la cellule correspondant a la ligne "n"
//	et colonne "k"
// r_8  GetCell(int n, string const & nom) const
//	Renvoie le contenu de la cellule correspondant a la ligne "n"
//	pour la colonne identifiee par "nom"
// string  GetCelltoString(int n, int k) const
//	Renvoie le contenu de la cellule correspondant a la ligne "n"
//	et colonne "k", sous forme de chaine de caracteres.
// string  GetCell(int n, string const & nom) const
//	Renvoie le contenu de la cellule correspondant a la ligne "n"
//	pour la colonne identifiee par "nom", sous forme de chaine de caracteres.
// void GetMinMax(int k, double& min, double& max)  const
//	Renvoie la valeur mini et maxi de la colonne numero "k"
// void	GetMinMax(string const & nom, double& min, double& max)
//	Renvoie la valeur mini et maxi de la colonne identifiee par "nom"
// int  ColumnIndex(string const & nom)  const
//	Renvoie le numero de colonne identifiee par "nom". Renvoie -1 (negatif)
//	si colonne "nom" n'existe pas.
// string ColumnName(int k) const
//	Renvoie le nom de la colonne numero "k"
// string VarList_C(const char* nomx=NULL) const
//	Renvoie une chaine avec les declarations "C" pour les
//	variables avec les noms de colonnes
// string  LineHeaderToString()
//	Renvoie une chaine avec les noms de colonnes, utilisables pour l'impression
// string  LineToString(int n)
//	Renvoie une chaine avec le contenu de la ligne "n", utilisable pour l'impression
//--

/* --Methode-- */
NTupleInterface::NTupleInterface()
{
}
/* --Methode-- */
NTupleInterface::~NTupleInterface()
{
}

//! Retuns the number of lines (rows) of the data set
/* --Methode-- */
size_t NTupleInterface::NbLines() const
{
return(0);
}

//! Return the number of columns of the data set 
/* --Methode-- */
size_t NTupleInterface::NbColumns() const
{
return(0);
}

//! Return the content of a given line 
/*! 
   An array (double *) containg all cells of a given line, converted to 
   double precision float values is returned
*/
/* --Methode-- */
r_8 * NTupleInterface::GetLineD(size_t ) const
{
  throw NotAvailableOperation("NTupleInterface::GetLineD(size_t ) must be redefined in derived classes");
}

//! Returns the content of a given cell
/*!
    The cell is identified by the line and column number. Its content is
    converted to a floating value.
*/
/* --Methode-- */
r_8 NTupleInterface::GetCell(size_t , size_t ) const
{
return(0.);
}

//! Returns the string representation of a given cell
/*!
    The cell is identified by the line and column number. Its content is
    converted to a string.
*/
string NTupleInterface::GetCelltoString(size_t n, size_t k) const
{
char strg[64];
//sprintf(strg,"C%d= %g\n", k, GetCell(n, k));
sprintf(strg,"%g\n", GetCell(n, k));
return(strg);
}

//! Returns the minimum and maximum values for a given column
/* --Methode-- */
void NTupleInterface::GetMinMax(size_t , double& min, double& max)   const
{
min = max = 0.;
}

//! Return the sum of values and values-squared (Sum[v], Sum[v^2])  for a given column
/* --Methode-- */
void NTupleInterface::GetSum(size_t k, double& sum, double& sumsq)   const
{
sum = sumsq = 0.;
}

/* --Methode-- */
//! Returns the column index given the column name
size_t NTupleInterface::ColumnIndex(string const & nom)  const
{
return(-1);
}

/* --Methode-- */
//! Returns the column name corresponding to a column index.
string NTupleInterface::ColumnName(size_t k) const
{
return("");
}

/* --Methode-- */
//! Return a string with C language style declaration of double typed variables with column names  
string NTupleInterface::VarList_C(const char*) const
{
return("");
}

/* --Methode-- */
//! Return a string with column names, usable as header line for printing  
string NTupleInterface::LineHeaderToString() const
{
return("");
}

/* --Methode-- */
//! Return a string with the content of a given row of the table, usable for printing 
string NTupleInterface::LineToString(size_t ) const
{
return("");
}

/* --Methode-- */
/*!
  \brief Return true if the class implements the extended interface GetLineMTP() and VarListMTP_C() methods 
  
  Default implementation return false. Derived classes which implement the extended interface  
  ( GetLineMTP() and VarListMTP_C() methods) should also reimplement this method and set the return value to true.
*/
bool NTupleInterface::ImplementsExtendedInterface() const
{
  return false; 
}

/* --Methode-- */
//! Return a pointer to data cell contents of row \b n of the table 
NTupMTPointer* NTupleInterface::GetLineMTP(size_t n) const
{
  return NULL; 
}

/* --Methode-- */
/*! 
  \brief Return a string with C language style declaration of variables with column names and corresponding 
  data types, initialzed from an array of NTupMTPointer, as the one that is returned by GetLineMTP()
*/ 
string NTupleInterface::VarListMTP_C(const char* nomx) const
{
  return "";
};


/* --Methode-- */
/*! 
  \brief return the necessary C language declarations for the extended interface (the NTupMTPointer structure)
*/ 
string NTupleInterface::MTP_C_Declarations() 
{
  string rs;
  rs += "struct z_cmplx_f4 {  float real;  float imag; }; \n";
  rs += "struct z_cmplx_f8 {  double real;  double imag; }; \n";
  rs += "union NTupMTPtr { \n";
  rs += "  const int * i4;    /* int_4 */ \n";
  rs += "  const long long * i8;  /*  int_8 */ \n";
  rs += "  const float * f4; \n";
  rs += "  const double * f8; \n";  
  rs += "  const struct z_cmplx_f4 * z4; \n"; 
  rs += "  const struct z_cmplx_f8 * z8; \n";
  rs += "  const char * s; \n";
  rs += "  const char * * sp; \n";  
  rs += "  const void * vp; \n";   
  rs += "}; \n";
  // Definition de fonctions utile pour les complexes 
  rs += "#define arg(z) (atan2((double)z.imag,(double)z.real)) \n";
  rs += "#define norm(z) ((double)z.real*(double)z.real+(double)z.imag*(double)z.imag) \n";
  rs += "#define phas(z) (atan2((double)z.imag,(double)z.real)) \n";
  rs += "#define module(z) sqrt((double)z.real*(double)z.real+(double)z.imag*(double)z.imag) \n";
  rs += "#define module2(z) ((double)z.real*(double)z.real+(double)z.imag*(double)z.imag) \n";

  return rs;
}



