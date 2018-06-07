//  Classe Dynamic Variable List (DVList) de PEIDA
//                  R. Ansari  1997
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#include "machdefs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dvlist.h"
#include "strutil.h"

namespace SOPHYA {

//++
// Class	DVList
// Lib		BaseTools
// include	dvlist.h
//
//	Cette classe permet de gérer une ensemble de variables (ou paramètres)
//	pouvant être définies dynamiquement à l'execution. Le nom des 
//	variables ne doit pas contenir de blancs ("<espace>") et est 
//	limité à 64 caractères maximum. Cette classe 
//	offre la possibilité de sauvegarder l'ensemble 
//	des variables (Nom, Type, Valeur) dans un fichier, ou de
//	recréer l'objet DVList et l'ensemble de ses variables à 
//	partir d'un fichier (Objet PPersist). Une zone commentaire (max=320 c.) 
//	est associée à chaque objet DVList, accessible à travers  
//	la méthode "Comment()". Les objets de cette classe sont  
//	en particulier destinés à être inclus dans d'autres objets 
//	PPersist plus complexes. La classe DVList gère des 
//	variables de type entier ("int_8"), réél double précision ("r_8")
//	et de type chaine de caracteres ("string, char*", maxi 30 caracteres ).
//	Une classe intermédiaire (*MuTyV*) est utilisée pour représenter une 
//	variable et fournit les services de conversion entre les différents types.
//--
//--
//++
// Links	Parents
// PPersist
//--


static MuTyV ddvdum(-9.e19);


/*!
   \class DVList
   \ingroup BaseTools
   This class can be used to construct list of values associated with names.
   Variables names should not contain space characters and is limited to 64
   characters. The DVList class uses \b SOPHYA::MuTyV objects to hold values
   of type string, integer (\b int_8) or float (\b r_8) or time/date (TimeStamp). 
   A comment string can be associated with each variable name. A global comment string
   can be attached to the DVList object. DVList objects can conveniently be 
   used to represent FITS headers. The class \b SOPHYA::ObjFileIO<DVList>
   handles serialisation for DVList. (See SOPHYA::PPersist ).

   \sa SOPHYA::ObjFileIO<DVList>
   \sa SOPHYA::MuTyV

   \code
   //  ------- Using DVList objects ------ 
   DVList  dvl;
   dvl("toto") = 14;      // Integer type value (=14) named toto
   dvl("titi") = 25.5;    // float type value (=25.5) named titi
   dvl("tata") = "Bonjour !"; // string type value (="Bonjour !") named tata 
   // Upper and lower case letters are distinguished
   dvl("hello") = 88;
   dvl("Hello") = 77.77;    
   dvl("ToDay") = TimeStamp();    
   dvl.Comment() = "DVList test object, with values named hello, Hello ";
   // Saving the dvl object into a PPF file 
   POutStream os("dvl.ppf");
   os << dvl;
   //  later on ...
   DVList dvlr; 
   PInStream is("dvl.ppf");
   is << dvlr;
   int k = dvlr["toto"] ;     //  k = 14
   r_8 b = dvlr["titi"] ;     //  b = 25.5
   string s =  dvlr["tata"] ; //  s = "Bonjour !"
   int m =  dvlr["hello"] ;   //  m = 88

   \endcode
*/

//++
// Titre	Constructeurs
//--

//++
// DVList()
//	Constructeur par défaut
// DVList(DVList& cfd)
//	Constructeur par copie. Le nouvel objet est une copie complète de "cfd"
// DVList(char* flnm)
//	Constructeur avec initialisation à partir du contenu du fichier (PPF)
//	"flnm". Le fichier doit avoir été créé par la méthode "Write()"
//--

/* --Methode-- */
/*! Default constructor */
DVList::DVList()
{
comment = "";
}

/* --Methode-- */
/*! copy constructor */
DVList::DVList(const DVList& dvl)
{
Merge(dvl);
}

/* --Methode-- */
/*! Copy constructor - Object initialized using the PPF file \b flnm */
DVList::DVList(const char *flnm)
{
PInPersist s(flnm);
ObjFileIO<DVList> fiodvl(this);
fiodvl.Read(s);
}


/* --Methode-- */
DVList::~DVList()
{
}

//++
// Titre	Gestion des variables et opérateurs
//--

//++
// void  Clear()
//	Supprime la définition de toutes les variables de l'objet.
// DVList&  Merge(const DVList& lv)
//	Fusionne l'objet avec la liste des variables de l'objet "lv"
// DVList&  operator= (const DVList& cofr)
//	Remplace la liste des variables de l'objet par celle de l'objet "cofr".   
//--

/* --Methode-- */
/*! Copy operator - Replaces the variables list with the list from \b dvl */
DVList&     DVList::operator= (const DVList& dvl)
{
Clear();
return(Merge(dvl));
}


/* --Methode-- */
/*! Resets the object and clears the variable list and global comment */
void        DVList::Clear()
{
mvlist.erase(mvlist.begin(), mvlist.end());
comment = "";
}

/* --Methode-- */
/*! Appends the values from the object \b dvl to the objects list */
DVList&     DVList::Merge(const DVList& dvl)
{
ValList::const_iterator it;
for(it = dvl.mvlist.begin(); it != dvl.mvlist.end(); it++)
  {
  switch ((*it).second.elval.Type()) 
    {
    case MuTyV::MTVInteger :
      SetI((*it).first, (*it).second.elval.GetIntPart());
      break;
    case  MuTyV::MTVFloat : 
      SetD((*it).first, (*it).second.elval.GetRealPart());
      break;
    case MuTyV::MTVComplex : 
      SetZ((*it).first, complex<r_8>((*it).second.elval.GetRealPart(), (*it).second.elval.GetImagPart()));
      break;
    case MuTyV::MTVString : 
      SetS((*it).first, *((*it).second.elval.GetStringPointer()));
      break;
    default :
      break;
    }
  SetComment((*it).first, (*it).second.elcomm);
  }
comment = comment + "\n" + dvl.comment;
return(*this);
}


//++
// int_8   GetI(string const& key, int_8  def=-1)
// r_8     GetD(string const& key, r_8 def=-9.e19)
// string  GetS(string const& key, const char* def="")
//	Retourne la valeur de la variable de nom "key" et de type entier, réél, 
//	chaine de caracteres.
//	Si la variable n'existe pas, la valeur par défaut "def" est renvoyée.
// string  GetComment(string const& key)
//	Retourne le commentaire associé à la variable de nom "key".
//--

/* --Methode-- */
/*! Returns the value corresponding to name \b key, converted to integer
    Default value \b def is returned if name \b key not found */
int_8       DVList::GetI(string const& key, int_8 def) const
{
ValList::const_iterator it = mvlist.find(key);
if (it == mvlist.end())  return(def);
return( (int_8)((*it).second.elval) );
}

/* --Methode-- */
/*! Returns the value corresponding to name \b key, converted to double
    Default value \b def is returned if name \b key not found */
r_8      DVList::GetD(string const& key, r_8 def) const
{
ValList::const_iterator it = mvlist.find(key);
if (it == mvlist.end())  return(def);
return( (r_8)((*it).second.elval) );
}

/* --Methode-- */
/*! Returns the value corresponding to name \b key, converted to complex
    Default value \b def is returned if name \b key not found */
complex<r_8>       DVList::GetZ(string const& key, complex<r_8> def) const
{
ValList::const_iterator it = mvlist.find(key);
if (it == mvlist.end())  return(def);
/*  
    Appel explicite de l'operateur de conversion 
    suite a la suggestion de M. Reinecke, Reza 31/7/2002 
#if defined(__GNUG__)
complex<r_8> z;
z = (*it).second.elval;
return(z);
#else
return( (complex<r_8>)(*it).second.elval );
#endif
   --- Appel explicite de l'operateur de conversion sur l'objet MuTyV 
*/
return( (*it).second.elval.operator complex<r_8>() );
}

/* --Methode-- */
/*! Returns the value corresponding to name \b key, converted to string
    Default value \b def is returned if name \b key not found */
string      DVList::GetS(string const& key, const char* def) const
{
ValList::const_iterator it = mvlist.find(key);
if (it == mvlist.end())  return(def);
return( (string)((*it).second.elval) );
}

/* --Methode-- */
/*! Returns the comment associated with name \b key */ 
string      DVList::GetComment(string const& key) const
{
ValList::const_iterator it = mvlist.find(key);
if (it == mvlist.end())  return("");
return((*it).second.elcomm);
}

//++
// void  SetI(string const& key, int_8  val)
// void  SetD(string const& key, r_8 val)
// void  SetZ(string const& key, complex<r_8> val)
// void  SetS(string const& key, const char*  val)
// void  SetS(string const& key, string val)
//	Crée la variable de nom "key", de type entier, double, complexe, string et 
//	lui attribue la valeur "val". Si une variable du même nom existe, 
//	sa valeur et eventuellement son type sont modifiés.  Les noms de
//	variables ne doivent pas contenir de caractères spéciaux, 
//	en particulier pas de CR/LF.
// void  SetComment(string const& key, string const& comm)
//	Modifie le commentaire associé à la variable de nom "key", si
//	celle-ci existe. Le texte du commentaire ne doit pas contenir
//	de caractères spéciaux, et en particulier pas de CR/LF. 
//--

/* --Methode-- */
/*! Removes the definition and value associated with the name \b key. 
  Return \c true if the \b key is found in the list, \c false otherwise. */
bool        DVList::DeleteKey(string const& key)
{
ValList::iterator it = mvlist.find(key);
if (it == mvlist.end())  return(false);
mvlist.erase(it);
return(true);
}

/* --Methode-- */
/*!  Return \c true if the \b key is found in the list, \c false otherwise. */
bool        DVList::HasKey(string const& key) const
{
ValList::const_iterator it = mvlist.find(key);
if (it == mvlist.end())  return(false);
return(true);
}


/* --Methode-- */
/*! Appends or sets the integer value \b val in the list with name \b key */
void        DVList::SetI(string const& key, int_8 val)
{
Get(key) = (int_8)val;
}

/* --Methode-- */
void        DVList::SetD(string const& key, r_8 val)
/*! Appends or sets the double value \b val in the list with name \b key */
{
Get(key) = (r_8)val;
}

/* --Methode-- */
void        DVList::SetZ(string const& key, complex<r_8> val)
/*! Appends or sets the complex value \b val in the list with name \b key */
{
Get(key) = val;
}

/* --Methode-- */
/*! Appends or sets the string value \b val in the list with name \b key */
void        DVList::SetS(string const& key, const char * val)
{
MuTyV div(val);
Get(key) = div;
}

/* --Methode-- */
/*! Appends or sets the string value \b val in the list with name \b key */
void        DVList::SetS(string const& key, string const& val)
{
MuTyV div(val);
Get(key) = div;
}

/* --Methode-- */
/*! Appends or sets the TimeStamp value \b val in the list with name \b key */
void        DVList::SetT(string const& key, TimeStamp const& val)
{
MuTyV div(val);
Get(key) = div;
}

/* --Methode-- */
/*! Assigns the comment \b comm with the name \b key . 
    Does nothing if the entry with name is not present in the list   */
void        DVList::SetComment(string const& key, string const& comm)
{
ValList::iterator it = mvlist.find(key);
if (it == mvlist.end())  return;
(*it).second.elcomm = comm;
}

//++
// MuTyV&  Get(string const& key) 
//	Renvoie une référence sur l'objet "MuTyV" de la liste avec le nom "key".
//	Si cet objet (variable) n'existe pas, il est créé.
// MuTyV&     operator()  (string const& key) 
// MuTyV&     operator[]  (string const& key) 
//
//	Renvoie la variable de nom "key". Equivalent à "Get(key)". 
// string&  Comment()
//	Renvoie une référence sur le champ commentaire de l'objet.
//--

/* --Methode-- */
/*!  Return the MuTyV value associated with name \b key .
     Integer 0 is returned if \b key is not present in the list   */
MuTyV       DVList::Get(string const& key) const
{
ValList::const_iterator it = mvlist.find(key);
if (it == mvlist.end()) return(MuTyV( (int_8) 0));
else return((*it).second.elval);
}

/* --Methode-- */
/*!  Return the MuTyV value associated with name \b key .
     Adds an entry of type integer in the list if \b key is not present in the list   */
MuTyV&      DVList::Get(string const& key)
{
size_t l = key.length();
if (l < 1)  return(ddvdum);
// dvlElement xxx = {(int_8)0 , ""};  marche pas sur mac/CW (!) - cf DY
dvlElement xxx; xxx.elval = (int_8)0;  xxx.elcomm = "";
ValList::iterator it = mvlist.find(key);
if (it == mvlist.end()) mvlist[key] = xxx;  
it = mvlist.find(key);
if (it == mvlist.end()) return(ddvdum); 
else return((*it).second.elval);
}

//++
// Titre	Entrée-Sortie
//--

//++
//  void  Print()
//	Imprime (sur "cout") la liste des variables et leurs valeurs.
//  void  Print(ostream& os)
//	Imprime sur le flot "os" la liste des variables et leurs valeurs.
//  ostream&    operator << (ostream& s, DVList& dvl)
//	sortie sur flot "s" (Appel a "Print(s)").
//--

/* --Methode-- */
/*! Prints a brief description of object on on the output stream \b os */
void        DVList::Show(ostream& os) const
{
os << "DVList::Show() - NVar= " << (int)mvlist.size() << "\n";
os << comment << endl;
}

/* --Methode-- */
/*! Prints the list of variables on the output stream \b os */
void        DVList::Print(ostream& os) const
{
os << "DVList::Print() - NVar= " << (int)mvlist.size() << "\n";
if (comment.length() > 0)  os << comment << endl;
char buff[1024];
TimeStamp ts; 
ValList::const_iterator it;
for(it = mvlist.begin(); it != mvlist.end(); it++)  {
  switch ((*it).second.elval.Type()) 
    {
    case MuTyV::MTVInteger :
      sprintf(buff, "%s = %ld (T=long int) /  %s\n", (*it).first.substr(0,64).c_str(), 
	      (long)((*it).second.elval.GetIntPart()), (*it).second.elcomm.substr(0,128).c_str());
      break;
    case MuTyV::MTVFloat : 
      sprintf(buff, "%s = %.20g (T=double) / %s\n", (*it).first.substr(0,64).c_str(), 
	      (*it).second.elval.GetRealPart(), (*it).second.elcomm.substr(0,128).c_str());
      break;
    case MuTyV::MTVComplex : 
      sprintf(buff, "%s = (%.20g,%.20g) (T=complex) / %s\n", (*it).first.substr(0,64).c_str(), 
	      (*it).second.elval.GetRealPart(), (*it).second.elval.GetImagPart(), (*it).second.elcomm.substr(0,128).c_str());
      break;
    case MuTyV::MTVString : 
      sprintf(buff, "%s = %s (T=string) / %s\n", (*it).first.substr(0,64).c_str(), 
	      (*it).second.elval.GetStringPointer()->substr(0,800).c_str(), (*it).second.elcomm.substr(0,128).c_str());
      break;
    case MuTyV::MTVTimeStamp :
      ts.Set((*it).second.elval.GetRealPart());
      sprintf(buff, "%s = %s (T=TimeStamp) %s\n", (*it).first.substr(0,64).c_str(), 
		ts.ToString().c_str(), (*it).second.elcomm.substr(0,128).c_str());
      break;
    default :
      break;
    }
  os << (string)buff;
  }
os << endl;
}


//++
// Titre	Exemples
//	Utilisation des objets *MuTyV* :
//|	MuTyV mvu;         // Declaration d'une variable 
//|	mvu = 60;          // mvu est de type entier (= 60)
//|	mvu = 66.6;        // et double (= 66.6) maintenant ...   
//|	MuTyV mvi(14);     // On construit une variable entiere = 14
//|	r_4 x = mvi;     // x vaut 14.0
//|	MuTyV mvd(44.4);   // Variable double = 44.4
//|	int k = mvd;       // k vaut 44  
//|	MuTyV mvs("Bonjour, Ca va ?");   // Variable chaine de caracteres 
//|	string s = mvs;    // s vaut "Bonjour, Ca va ?"  
//	Utilisation des *DVList* :		 
//|	DVList  dvl;
//|	dvl("toto") = 14;
//|	dvl("titi") = 25.5;
//|	dvl("tata") = "Bonjour, Ca va ?";
//	Majuscules et minuscules sont differenciees pour les noms, pas de blanc ...
//|	dvl("hello") = 88;
//|	dvl("Hello") = 77.77;
//|	dvl.Comment() = "Test d'objet DVList, avec variables hello, Hello ";
//|	dvl.Write("dvlist.ppf");
//	Plus loin, ou dans un autre programme, on relit le fichier fabrique plus haut 
//|	DVList dvlr("dvlist.ppf");
//|	int k = dvlr["toto"] ;     //  k = 14
//|	r_8 b = dvlr["titi"] ;  //  b = 25.5
//|	string s =  dvlr["tata"] ; //  s = "Bonjour, Ca va ?"
//|	r_4 c = dvlr["Hello"] ;  //  c = 77.77
//|	int l =  dvlr["Hello"] ;   //  l = 77
//|	int m =  dvlr["hello"] ;   //  m = 88
//--


//----------------------------------------------------------
// Classe pour la gestion de persistance
// ObjFileIO<DVList>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<DVList>::WriteSelf(POutPersist& s) const
{
if (dobj == NULL)
  throw NullPtrError("ObjFileIO<DVList>::WriteSelf() dobj=NULL");

char buf[1024];
string sfw;
int lc = dobj->Comment().length();
//  itab -  0 : Version,  1 : NVar,  2  : Comment length, 3 reserved
uint_4 itab[4];
itab[0] = 2;  // Version number = 2 
itab[1] = dobj->Size(); 
itab[2] = lc; 
itab[3] = 0;
s.Put(itab, 4);

if (lc > 0) s.PutStr(dobj->Comment());
sfw = "\n----Variable-List---------------\n";  
s.PutStr(sfw);

DVList::ValList::const_iterator it;
for(it = dobj->Begin(); it != dobj->End(); it++)  {
  switch ((*it).second.elval.Type()) {
    case MuTyV::MTVInteger :
      sprintf(buf,"I %s %ld\n", (*it).first.substr(0,64).c_str(), (long)((*it).second.elval.GetIntPart()) );
      sfw = buf;  s.PutStr(sfw);
      break;
    case MuTyV::MTVFloat : 
      sprintf(buf,"F %s %.20g\n", (*it).first.substr(0,64).c_str(), (*it).second.elval.GetRealPart() );
      sfw = buf;  s.PutStr(sfw);
      break;
    case MuTyV::MTVComplex : 
      sprintf(buf,"Z %s %.20g %.20g\n", (*it).first.substr(0,64).c_str(), (*it).second.elval.GetRealPart(), 
                                      (*it).second.elval.GetImagPart());
      sfw = buf;  s.PutStr(sfw);
      break;
    case MuTyV::MTVString : 
      sprintf(buf,"S %s %s\n", (*it).first.substr(0,64).c_str(), (*it).second.elval.GetStringPointer()->substr(0,960).c_str() );
      sfw = buf;  s.PutStr(sfw);
      break;
    case MuTyV::MTVTimeStamp : 
      sprintf(buf,"T %s %.20g\n", (*it).first.substr(0,64).c_str(), (*it).second.elval.GetRealPart() );
      sfw = buf;  s.PutStr(sfw);
      break;
    default :
      break;
  }
// Ecriture eventuelle du commentaire associe
  if ((*it).second.elcomm.length() > 0) {
    sprintf(buf,"# %s", (*it).second.elcomm.substr(0,256).c_str());
    sfw = buf;  s.PutStr(sfw);
  }
}

sfw = "ZZZZZ--End-of-Variable-List------"; s.PutStr(sfw);
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<DVList>::ReadSelf(PInPersist& s)
{
char buf[1024];
string sfr;
int_8 j,iv,k;
r_8 dv, dvi;
bool ok=true;
buf[0] = '\0';
if (dobj == NULL) dobj = new DVList; 
else dobj->Clear();

//  itab -  0 : Version,  1 : NVar,  2  : Comment length, 3 reserved
uint_4 itab[4];
s.Get(itab, 4);
if (itab[2] > 0) {  // Il y a un champ commentaire a lire 
  s.GetStr(sfr);
  dobj->Comment() = sfr;
  }

s.GetStr(sfr);  // Pour lire les "------- "

string key="";
while(ok) {
  s.GetStr(sfr);
  strncpy(buf, sfr.c_str(), 1024);
  buf[1023] = '\0';
  j = strlen(buf)-1;
  if ( (j >= 0) && (buf[j] == '\n') )  buf[j] = '\0';
  if (strncmp(buf,"ZZZZZ",5) == 0)  { ok=false; break; }
  if (buf[0] == '#') {
    dobj->SetComment(key, buf+2);
    continue;
  }
  j = posc(buf+2, ' ')+2;
  buf[j] = '\0';
  switch (buf[0]) {
    case 'I' :
      iv = (int_8)atol(buf+j+1);
      key = buf+2;
      dobj->SetI(key, iv);
      break;
    case 'F' :
      dv = atof(buf+j+1);
      key = buf+2;
      dobj->SetD(key, dv);
      break;
    case 'Z' :
      k = posc(buf+j+1, ' ')+j+1;
      buf[k] = '\0';
      dv = atof(buf+j+1);
      dvi = atof(buf+k+1);
      key = buf+2;
      dobj->SetZ(key, complex<r_8>(dv, dvi));
      break;
    case  'S' :
      key = buf+2;
      dobj->SetS(key, buf+j+1);
      break;
    case 'T' :
      dv = atof(buf+j+1);
      key = buf+2;
      dobj->SetT(key, TimeStamp(dv));
      break;
    default :
      break;
    }
  }
if (dobj->Size() != itab[1])  // Probleme !!!
  throw FileFormatExc("ObjFileIO<DVList>::ReadSelf() Error in Nb. Variables !");
}


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<DVList>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<DVList>;
#endif

} // FIN namespace SOPHYA 

