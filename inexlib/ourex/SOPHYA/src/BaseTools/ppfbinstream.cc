#include "sopnamsp.h"
#include "machdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "ppfbinstream.h"
#include "pexceptions.h"
#include <iostream>


//  strptime n'est pas defini sous Linux avec g++ avant gcc 3.x - Reza Mars 2000
#if defined(Linux) && defined(__GNUG__) && (__GNUC__ < 3) 
extern "C" {
char *strptime(const char *buf, const char *format, struct tm *tm);
}
#endif

#define MAXTAGLEN_V2 255


static inline void bswap16(void* p)
{
  uint_8* p8 = (uint_8*)p;
  uint_8 tmp1 = *(p8);
  uint_8 tmp = *(p8+1);
  *(p8)   = ((tmp >> (7*8)) & 0x000000FF) | 
            ((tmp >> (5*8)) & 0x0000FF00) |
            ((tmp >> (3*8)) & 0x00FF0000) |
            ((tmp >> (1*8)) & 0xFF000000) |
            ((tmp & 0xFF000000) << (1*8))  |
            ((tmp & 0x00FF0000) << (3*8))  |
            ((tmp & 0x0000FF00) << (5*8))  |
            ((tmp & 0x000000FF) << (7*8));
  *(p8+1) = ((tmp1 >> (7*8)) & 0x000000FF) | 
            ((tmp1 >> (5*8)) & 0x0000FF00) |
            ((tmp1 >> (3*8)) & 0x00FF0000) |
            ((tmp1 >> (1*8)) & 0xFF000000) |
            ((tmp1 & 0xFF000000) << (1*8))  |
            ((tmp1 & 0x00FF0000) << (3*8))  |
            ((tmp1 & 0x0000FF00) << (5*8))  |
            ((tmp1 & 0x000000FF) << (7*8));
}

static inline void bswap8(void* p)
{
  uint_8 tmp = *(uint_8*)p;
  *(uint_8*)p = ((tmp >> (7*8)) & 0x000000FF) | 
                ((tmp >> (5*8)) & 0x0000FF00) |
                ((tmp >> (3*8)) & 0x00FF0000) |
                ((tmp >> (1*8)) & 0xFF000000) |
                ((tmp & 0xFF000000) << (1*8))  |
                ((tmp & 0x00FF0000) << (3*8))  |
                ((tmp & 0x0000FF00) << (5*8))  |
                ((tmp & 0x000000FF) << (7*8));
}

static inline void bswap4(void* p)
{
  uint_4 tmp = *(uint_4*)p;
  *(uint_4*)p = ((tmp >> 24) & 0x000000FF) | 
                ((tmp >> 8)  & 0x0000FF00) |
                ((tmp & 0x0000FF00) << 8)  |
                ((tmp & 0x000000FF) << 24);
}

static inline void bswap2(void* p)
{
  uint_2 tmp = *(uint_2*)p;
  *(uint_2*)p = ((tmp >> 8) & 0x00FF) | 
                ((tmp & 0x00FF) << 8);
}

// Convertit la taille de donnees (2 (int_2), 4 (int_4, r_4), 8 (r_8) ...) en taille codee dans le tag
static inline unsigned char datasize2tagsize( unsigned char sz) 
{
  // Taille permises 1,2,4,8,16 
  // Pour les tailles <= 8, c'est directement la taille
  if (sz<=8) return sz;
  else {
    if (sz==16) return 10;
    else return 0;  // signale une erreur 
  }
}
// Convertit la taille codee dans le tag en taille de donnees (2 (int_2), 4 (int_4, r_4), 8 (r_8) ...) en 
static inline unsigned char tagsize2datasize( unsigned char sz) 
{
  // Taille permises 1,2,4,8,16 
  // Pour les tailles <= 8, c'est directement la taille
  if (sz<=8) return sz;
  else {
    if (sz==10) return 10;
    else return 0;  // signale une erreur 
  }
}

//-------------------------------------------------------------------------
//----------------------  Classe PPFBinaryIOStream ------------------------
//-------------------------------------------------------------------------

/*!
   \class SOPHYA::PPFBinaryIOStream
   \ingroup BaseTools
   Base class for SOPHYA PPF input / output stream
*/


PPFBinaryIOStream::PPFBinaryIOStream()
{
  version = 0;     // PPersist(In/Out) version 
  //  creationdate = time(NULL);   // Date de creation du fichier
  _nbpostag = 0;     // Nb de tag de positionnement
  _nbobjs = 0;       // Nb total d'objets
  _nbtlobjs = 0;     // Nb d'objets de niveau 1 
  _nbrefs = 0;       // Nb de reference d'objets
  _maxnestlevel = 0; // Niveau maximum d'objets emboites
}

PPFBinaryIOStream::~PPFBinaryIOStream()
{
}

//! Return the creation date/time of the associated stream as a string 
string 
PPFBinaryIOStream::CreationDateStr()
{
  time_t cdt = CreationDate();
  string cdate = ctime(&cdt);
  return(cdate);
}

//! Return a string with information about the stream (creation-date, version ...)
string 
PPFBinaryIOStream::InfoString()
{
  string rs;
  char buff[256];
  sprintf(buff,"PPFStream Version= %d  CreationDate= ", Version());
  rs += buff;
  rs += CreationDateStr();
  sprintf(buff,"\n NbObjs= %ld NbTopLevObjs= %ld NbRefs= %ld MaxNest= %d ", 
	  (long)NbObjects(), (long)NbTopLevelObjects(), 
	  (long)NbReferences(), MaxNestedObjsLevel());
  rs += buff;
  sprintf(buff,"\n NbPosTag= %ld NbNameTag= %ld ", (long)NbPosTags(), 
	  (long)NbNameTags());
   rs += buff;
   return rs;
}


//! Return the name tag number \b itag
string
PPFBinaryIOStream::GetTagName(int itag)
{
  if (itag<0 || itag >= (int)tags.size()) return "";
  map<string, int_8>::iterator i = tags.begin();
  for (int j=0; j<itag; j++) i++;
  return((*i).first);
}


//! Return a reference to a vector<string> containing all name-tags associated with the stream
static vector<string> * ret_tag_names = NULL;
vector<string> const &
PPFBinaryIOStream::GetNameTags()
{
if (ret_tag_names) delete ret_tag_names;
ret_tag_names = new vector<string> ;
map<string, int_8>::iterator i;
for(i=tags.begin(); i!=tags.end(); i++) ret_tag_names->push_back((*i).first);
return(*ret_tag_names);  
}

//-------------------------------------------------------------------------
//--------------------  Classe PPFBinaryInputStream -----------------------
//-------------------------------------------------------------------------

/*!
   \class SOPHYA::PPFBinaryInputStream
   \ingroup BaseTools
   PPF Input stream implementing read operations
*/

/*! Constructor from a RawInOutStream pointer
  \param is : pointer to RawInOutStream
  \param ad : if true, the RawInOutStream \b is is deleted by the destructor 
  \param scan : if true, try to read the name-tags table from the end of the input stream
*/ 
PPFBinaryInputStream::PPFBinaryInputStream(RawInOutStream * is, bool ad, bool scan)
{
  s = is;
  _ads = ad;
  Init(scan);
}

/*! Constructor 
  \param  flnm : input file name
  \param scan : if true, try to read the name-tags table from the end of the input stream
*/  
PPFBinaryInputStream::PPFBinaryInputStream(string const& flnm, bool scan)
//
//	Constructeur. Ouvre le fichier.
//--
{
  s = new RawInFileStream(flnm.c_str());
  _ads = true;
  Init(scan);
}

PPFBinaryInputStream::~PPFBinaryInputStream()
{
  if (_ads && (s!=NULL) ) delete s;
}

void
PPFBinaryInputStream::Init(bool scan)
{
  // Read and check header

  char rbuf[36];
  GetRawBytes(rbuf, 32);
  if (strncmp(rbuf,"SOS-SOPHYA-PPersistFile", 23) != 0)  {
    throw FileFormatExc("PPFBinaryInputStream::PPFBinaryInputStream  bad header");
  }
  rbuf[32] = '\0';
  version = atoi(rbuf+25);
  if (version < 2) {
    cerr << "PPFBinaryInputStream::PPFBinaryInputStream(" << FileName() << ") Version(=" << version 
	 << ") < 2 not supported !" << endl;
    throw FileFormatExc("PPFBinaryInputStream::PPFBinaryInputStream() - Unsupported (Old) Version");
  }
  // read endianness
  GetRawBytes(rbuf, 32);
  if (strncmp(rbuf,"BIG-ENDIAN",10) == 0)
    bigEndian = true;
  else if (strncmp(rbuf,"LITTLE-ENDIAN",13) == 0)
    bigEndian = false;
  else {
    throw FileFormatExc("PPFBinaryInputStream::PPFBinaryInputStream  bad header - endianness");
  }

  // read creation date
  GetRawBytes(rbuf, 32);
  rbuf[32] = '\0'; 
  struct tm tm;
  strptime(rbuf,"%d/%m/%Y %H:%M:%S GMT",&tm);

  creationdate = mktime(&tm);
  seqread = true;   // To flag non sequential reads
  if (scan && s->isSeekable()) { 
    if (Version() >= 3) ReadNameTagTable();
    else ReadNameTagTableV2();
  }
}


void
PPFBinaryInputStream::ReadNameTagTable()
{
  unsigned char ppstype;
  int_8 debut;
  debut = s->tellg();

  s->seekg(-(sizeof(int_8)+1), ios::end);
  // Lecture position NameTagTable et tag EOF
  int_8 pos;
  GetRawI8(pos);
  GetRawUByte(ppstype);
  if (ppstype != PPS_EOF)
    throw FileFormatExc("PPFBinaryInputStream::ReadNameTagTable()  Corrupted file, no EOF tag at end of file");
  
  // On se positionne au debut du NameTagTable
  s->seekg(pos);
  GetRawUByte(ppstype);
  if (ppstype != PPS_NAMETAG_TABLE)
    throw FileFormatExc("PPFBinaryInputStream::ReadNameTagTable()  Corrupted file PPS_NAMETAG_TABLE not found");
  // Lecture nb de PosTag et nb d'objets dans le flot
  int_8 stats[8] = {0,0,0,0,0,0,0,0};
  GetI8s(stats, 8);
  _nbpostag = stats[0];
  _nbobjs = stats[1];
  _nbtlobjs = stats[2];
  _nbrefs = stats[3];
  _maxnestlevel = stats[4];

  uint_8 ttsz,it;
  // Lecture nombre de NameTag
  GetRawU8(ttsz);
  if (ttsz > 0) {
    for(it=0; it<ttsz; it++) {
      int_8 tpos;
      string tname;
      GetRawI8(tpos);
      GetStr(tname);
      tags[tname] = tpos;
    }
  }
  // On revient au debut du float, juste apres l'entete
  s->seekg(debut);
}

void
PPFBinaryInputStream::ReadNameTagTableV2()
{
  // On cherche la liste des tags, a la fin du fichier

  unsigned char ppstype;
  int_8 debut;
  debut = s->tellg();

  s->seekg(-(sizeof(int_8)+1), ios::end);

  GetRawUByte(ppstype);
  if (ppstype != PPS_EOF)
    throw FileFormatExc("PPFBinaryInputStream::ReadNameTagTableV2() Corrupted file, no eof entry at end of file");

  int_8 pos;
  GetRawI8(pos);
  if (pos < 0) {  // no tags
    s->seekg(debut);
    return;
  }

  char buffer[MAXTAGLEN_V2+1];
  s->seekg(pos);
  while (true) {
    GetRawUByte(ppstype);
    if (ppstype == PPS_EOF) break;
    
    if (ppstype != PPS_NAMETAG_TABLE)
      throw FileFormatExc("PPFBinaryInputStream::ReadNameTagTableV2() Corrupted file, bad tag entry");

    GetRawI8(pos);
    int_4 len;
    GetRawI4(len);
    if (len > MAXTAGLEN_V2) 
      throw FileFormatExc("PPFBinaryInputStream::ReadNameTagTableV2() Corrupted file, tag name too long");
    GetRawBytes(buffer, len);
    buffer[len] = '\0';

    tags[buffer] = pos;
  }
  s->seekg(debut);
}


bool 
PPFBinaryInputStream::GotoPositionTag(int_8 pos)
{
  s->seekg(pos);
  unsigned char ppstag;
  GetRawUByte(ppstag);
  if (ppstag != PPS_POSTAG_MARK)
    throw FileFormatExc("PPFBinaryInputStream::GotoPositionTag() - PPS_POSTAG_MARK not found!");    
  int_8 tpos;
  GetRawI8(tpos);
  if (tpos != pos) 
    throw FileFormatExc("PPFBinaryInputStream::GotoPositionTag() - Wrong tag position!");
  return true;
}

bool 
PPFBinaryInputStream::GotoNameTag(string const& name)
{
  map<string, int_8>::iterator i = tags.find(name);
  if (i == tags.end()) 
    return false;
      //    throw NotFoundExc("PPFBinaryInputStream::GotoNameTag   tag not found");
  s->seekg((*i).second);
  seqread = false;
  //  objList.clear(); $CHECK$ EA 171199 Ne pas faire ? Reza 03/2000 ?
  return(true); 
}


bool
PPFBinaryInputStream::GotoNameTagNum(int itag)
{
  if (itag<0 || itag >= (int)tags.size()) return false;
  map<string, int_8>::iterator i = tags.begin();
  for (int j=0; j<itag; j++) i++;
  s->seekg((*i).second);
  seqread = false;
  // objList.clear();  $CHECK$ EA 171199  Ne pas faire ? Reza 03/2000 ?
  return(true);
}


bool
PPFBinaryInputStream::SkipToNextObject()
{
  if (! s->isSeekable())  return false;
  unsigned char ppstag=0;
  // int kss;
  while ((ppstag != PPS_OBJECT) && (ppstag != PPS_REFERENCE) && (ppstag != PPS_EOF)) {
    // kss++;
    GetRawUByte(ppstag);
    //    cout << " DBG--SkipToNextObject(" << kss << ")" << (int)ppstag << "," 
    //	 << (int)PPS_OBJECT << " fpos=" << s->tellg() << endl;
    if  ((ppstag != PPS_OBJECT) && (ppstag != PPS_REFERENCE) && (ppstag != PPS_EOF))
      SkipItem(false, ppstag);
  }
  s->seekg(-1, ios::cur);
  if (ppstag == PPS_OBJECT) return true;
  else return false;
  
}

bool
PPFBinaryInputStream::SkipNextItem()
{
  if (! s->isSeekable())  return false;
  SkipItem();
  return true;
}

char
PPFBinaryInputStream::NextItemTag(short & datasz, size_t & asz)
{
  int_8 cpos;
  cpos = s->tellg();

  unsigned char ppstag=0;
  unsigned char ppst1,ppst2,ppst3,ppst30;
  GetRawUByte(ppstag);
  ppst1 = ppstag&0x0f;  // bits 0123
  ppst2 = ppstag&0x30;  // bits     45
  ppst3 = ppstag&0xc0;  // bits       67
  ppst30 = ppstag&0xc1; // bits 0     67
  
  datasz = 0;
  asz = 0;

  int_4 i4;
  uint_8 ui8;

  if ((ppst2 == 0) && (ppst3 == 0) ) {
    switch (ppst1) {
    case PPS_STRING :
      GetRawI4(i4);
      datasz = 1;
      asz = i4;
      break;
      
    case PPS_NULL :
    case PPS_OBJECT :
    case PPS_REFERENCE :
    case PPS_NAMETAG_MARK :
    case PPS_POSTAG_MARK :
    case PPS_ENDOBJECT :
    case PPS_POSTAG_TABLE :
    case PPS_NAMETAG_TABLE :
    case PPS_EOF :
      datasz = 0;
      asz = 0;
      break;

    default :
      throw FileFormatExc("PPFBinaryInputStream::NextItemTag() - Unexpected tag value !");
      break;
    }
  }
  else {
    int_4 dsize = ppst1;
    if (ppst30 == PPS_DATATYPE_COMPLEX) { 
      dsize--;
    }
    switch (ppst2) {      
    case PPS_SIMPLE :
      datasz = dsize;
      asz = 1;      
      break;

    case PPS_SIMPLE_ARRAY4 :
      GetRawI4(i4);
      datasz = dsize;
      asz = i4;      
      break;

    case PPS_SIMPLE_ARRAY8 :
      GetRawU8(ui8);
      datasz = dsize;
      asz = ui8;      
      break;
    }
  }

  s->seekg(cpos,ios::beg);
  return(ppstag);
}

void 
PPFBinaryInputStream::SkipItem(bool fgrdt, unsigned char itag)
{
  unsigned char ppstag=0;
  unsigned char ppst1,ppst2,ppst3,ppst30;
  uint_8 ui8;
  int_4 i4;
  int_8 i8;
  
  if (fgrdt)  GetRawUByte(ppstag);
  else ppstag = itag;

  ppst1 = ppstag&0x0f;  // bits 0123
  ppst2 = ppstag&0x30;  // bits     45
  ppst3 = ppstag&0xc0;  // bits       67
  ppst30 = ppstag&0xc1; // bits 0     67
  if ((ppst2 == 0) && (ppst3 == 0) ) {
    switch (ppst1) {  
    case PPS_NULL :
    case PPS_NAMETAG_MARK :
      break;

    case PPS_STRING :
      GetRawI4(i4);
      s->seekg(i4,ios::cur);
      break;

    case PPS_OBJECT :
      GetRawU8(ui8);
      GetRawU8(ui8);
      break;
      
    case PPS_REFERENCE :
      GetRawU8(ui8);
      GetRawI8(i8);
      break;

    case PPS_POSTAG_MARK :
      GetRawI8(i8);	
      break;
     
    case PPS_ENDOBJECT :
      GetRawU8(ui8);
      break;

    case PPS_POSTAG_TABLE :
      GetRawI4(i4);
      //      for(int kkt=0; kkt<i4; kkt++)  GetRawI8(i8);	
      s->seekg((int_8)i4*8,ios::cur);
      break;

    case PPS_NAMETAG_TABLE :
      if (Version() < 3) {
	GetRawI8(i8);
	GetRawI4(i4);
	s->seekg(i4,ios::cur);
	}
      else {
	GetI8(i8);  // nb pos tag
	GetI8(i8);  // nb d'objets
	GetI8(i8);  // nb objets toplevel
	GetU8(ui8); // nb de nametag
	for(uint_8 kt=0; kt<ui8; kt++) {
	  GetRawI4(i4);
	  s->seekg(i4,ios::cur);
	}
	GetI8(i8);  // position debut NameTagTable
      }
      break;
      

    case PPS_EOF :
      if (Version() < 3) GetRawI8(i8);	
      break;
      
    default :
      cerr << "PPFBinaryInputStream::SkipItem() ERROR : Unexpected tag value " 
	   << hex << ppstag << " At position" << s->tellg() << dec << endl;
      throw FileFormatExc("PPFBinaryInputStream::SkipItem() - Unexpected tag value !");
    }
  }
  else {
    string dtype = "???? x";
    int_4 dsize = ppst1;
    int_8 dsizeskip = dsize;
    if (ppst30 == PPS_DATATYPE_COMPLEX) { 
      dsize--;
      dsizeskip = 2*dsize;
    }

    switch (ppst2) {
    case PPS_SIMPLE :
      s->seekg(dsizeskip, ios::cur);
      break;

    case PPS_SIMPLE_ARRAY4 :
      GetRawI4(i4);
      s->seekg(dsizeskip*(int_8)i4, ios::cur);
      break;
      
    case PPS_SIMPLE_ARRAY8 :
      GetRawU8(ui8);
      s->seekg(dsizeskip*ui8, ios::cur);
      break;
    }
  }
return;
}

//++
// void PPFBinaryInputStream::GetByte(char& c)
// void PPFBinaryInputStream::GetBytes(void* ptr, size_t bytes)
// void PPFBinaryInputStream::GetR4   (r_4& result)
// void PPFBinaryInputStream::GetR4s  (r_4* tab, size_t n)
// void PPFBinaryInputStream::GetR8   (r_8& result)
// void PPFBinaryInputStream::GetR8s  (r_8* tab, size_t n)
// void PPFBinaryInputStream::GetI2   (int_2& result)
// void PPFBinaryInputStream::GetI2s  (int_2* tab, size_t n)
// void PPFBinaryInputStream::GetU2   (uint_2& result)
// void PPFBinaryInputStream::GetU2s  (uint_2* tab, size_t n)
// void PPFBinaryInputStream::GetI4   (int_4& result)
// void PPFBinaryInputStream::GetI4s  (int_4* tab, size_t n)
// void PPFBinaryInputStream::GetU4   (uint_4& result)
// void PPFBinaryInputStream::GetU4s  (uint_4* tab, size_t n)
// void PPFBinaryInputStream::GetI8   (int_8& result)
// void PPFBinaryInputStream::GetI8s  (int_8* tab, size_t n)
// void PPFBinaryInputStream::GetU8   (uint_8& result)
// void PPFBinaryInputStream::GetU8s  (uint_8* tab, size_t n)
//	Lecture de données portables depuis le fichier PPersist. Pour chaque type
//	de données, on peut lire une valeur, ou un tableau de valeurs.
// void PPFBinaryInputStream::GetLine(char* ptr, size_t len)
//	Lecture d'une ligne de texte depuis le fichier PPersist.
//--


void
PPFBinaryInputStream::GetTypeTag(unsigned char& c) 
{
  int_8 tpos;
  c = PPS_NAMETAG_MARK;
  while ( (c == PPS_NAMETAG_MARK) || (c == PPS_POSTAG_MARK) ) {
    GetRawUByte(c);
    if (c == PPS_POSTAG_MARK)  GetRawI8(tpos);
  } 
  //  while (c == PPS_NAMETAG_MARK) {    Il ne faut plus faire ca !
  //    objList.clear();             $CHECK$ Reza 03/2000
  //    GetRawByte(c);
  //  }
}


void
PPFBinaryInputStream::GetRawByte(char& c)
{
  GetRawBytes(&c, 1);
}

void
PPFBinaryInputStream::GetRawUByte(unsigned char& c)
{
  GetRawBytes(&c, 1);
}

void
PPFBinaryInputStream::GetRawBytes(void* ptr, size_t bytes)
{
  s->read((char*)ptr, bytes);
}

void
PPFBinaryInputStream::GetRawI2   (int_2& result)
{
  GetRawBytes(&result, sizeof(int_2));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap2(&result);
}

void
PPFBinaryInputStream::GetRawI4   (int_4& result)
{
  GetRawBytes(&result, sizeof(int_4));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap4(&result);
}

void
PPFBinaryInputStream::GetRawI8   (int_8& result)
{
  GetRawBytes(&result, sizeof(int_8));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&result);
}

void
PPFBinaryInputStream::GetRawU8   (uint_8& result)
{
  GetRawBytes(&result, sizeof(uint_8));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&result);
}

void
PPFBinaryInputStream::CheckTag(unsigned char datasz, unsigned char datatype)
// datatype = PPS_DATATYPE_CHAR or PPS_DATATYPE_FLOAT or PPS_DATATYPE_INTEGER or PPS_DATATYPE_UNSIGNED
{
  unsigned char ppstype;
  GetTypeTag(ppstype);
  if (ppstype != PPS_SIMPLE + datasize2tagsize(datasz) + datatype)
    throw FileFormatExc("PPFBinaryInputStream::CheckTag   bad type in ppersist file");
}

void
PPFBinaryInputStream::CheckArrayTag(unsigned char datasz, size_t sz, unsigned char datatype)
// datatype = PPS_DATATYPE_CHAR or PPS_DATATYPE_FLOAT or PPS_DATATYPE_INTEGER or PPS_DATATYPE_UNSIGNED
{
  unsigned char ppstype;
  GetTypeTag(ppstype);
  size_t filesz;
  if (sz <= 0x7fffffff) {
    if (ppstype != PPS_SIMPLE_ARRAY4 + datasize2tagsize(datasz) + datatype)
      throw FileFormatExc("PPFBinaryInputStream::CheckArrayTag   bad type in ppersist file");
    int_4 ff;
    GetRawI4(ff); filesz=ff;
  } else {
    if (ppstype != PPS_SIMPLE_ARRAY8 + datasize2tagsize(datasz) + datatype)
      throw FileFormatExc("PPFBinaryInputStream::CheckArrayTag   bad type in ppersist file");
    uint_8 ff;
    GetRawU8(ff); filesz=ff;
  }
  if (filesz != sz)
    throw FileFormatExc("PPFBinaryInputStream::CheckArrayTag   bad array size in ppersist file");
}

void
PPFBinaryInputStream::GetByte(char& c)
{
  CheckTag(1,PPS_DATATYPE_CHAR);
  GetRawBytes(&c, 1);
}

void
PPFBinaryInputStream::GetBytes(void* ptr, size_t bytes)
{
  CheckArrayTag(1, bytes, PPS_DATATYPE_CHAR);
  GetRawBytes(ptr, bytes);
}

void
PPFBinaryInputStream::GetR4   (r_4& result)
{
  CheckTag(4,PPS_DATATYPE_FLOAT);
  GetRawBytes(&result, sizeof(r_4));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap4(&result);
}


void
PPFBinaryInputStream::GetR4s  (r_4* tab, size_t n)
{
  CheckArrayTag(4,n,PPS_DATATYPE_FLOAT);
  GetRawBytes(tab, n*sizeof(r_4));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap4(tab+i);

  return;
}

void
PPFBinaryInputStream::GetR8   (r_8& result)
{
  CheckTag(8,PPS_DATATYPE_FLOAT);
  GetRawBytes(&result, sizeof(r_8));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&result);
}

void
PPFBinaryInputStream::GetR8s  (r_8* tab, size_t n)
{
  CheckArrayTag(8,n,PPS_DATATYPE_FLOAT);
  GetRawBytes(tab, n*sizeof(r_8));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap8(tab+i);

  return;
}

void
PPFBinaryInputStream::GetI1   (int_1& result)
{
  CheckTag(1,PPS_DATATYPE_INTEGER);
  GetRawBytes(&result, sizeof(int_1));
}

void
PPFBinaryInputStream::GetI1s  (int_1* tab, size_t n)
{
  CheckArrayTag(1,n,PPS_DATATYPE_INTEGER);
  GetRawBytes(tab, n*sizeof(int_1));
}

void
PPFBinaryInputStream::GetU1   (uint_1& result)
{
  CheckTag(1,PPS_DATATYPE_UNSIGNED);
  GetRawBytes(&result, sizeof(uint_1));
}

void
PPFBinaryInputStream::GetU1s  (uint_1* tab, size_t n)
{
  CheckArrayTag(1,n,PPS_DATATYPE_UNSIGNED);
  GetRawBytes(tab, n*sizeof(uint_1));
}

void
PPFBinaryInputStream::GetI2   (int_2& result)
{
  CheckTag(2,PPS_DATATYPE_INTEGER);
  GetRawBytes(&result, sizeof(int_2));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap2(&result);
}

void
PPFBinaryInputStream::GetI2s  (int_2* tab, size_t n)
{
  CheckArrayTag(2,n,PPS_DATATYPE_INTEGER);
  GetRawBytes(tab, n*sizeof(int_2));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap2(tab+i);

  return;
}

void
PPFBinaryInputStream::GetU2   (uint_2& result)
{
  CheckTag(2,PPS_DATATYPE_UNSIGNED);
  GetRawBytes(&result, sizeof(uint_2));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap2(&result);
}

void
PPFBinaryInputStream::GetU2s  (uint_2* tab, size_t n)
{
  CheckArrayTag(2,n,PPS_DATATYPE_UNSIGNED);
  GetRawBytes(tab, n*sizeof(uint_2));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap2(tab+i);

  return;
}

void
PPFBinaryInputStream::GetI4   (int_4& result)
{
  CheckTag(4,PPS_DATATYPE_INTEGER);
  GetRawBytes(&result, sizeof(int_4));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap4(&result);
}

void
PPFBinaryInputStream::GetI4s  (int_4* tab, size_t n)
{
  CheckArrayTag(4,n,PPS_DATATYPE_INTEGER);
  GetRawBytes(tab, n*sizeof(int_4));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap4(tab+i);

  return;
}

void
PPFBinaryInputStream::GetU4   (uint_4& result)
{
  CheckTag(4,PPS_DATATYPE_UNSIGNED);
  GetRawBytes(&result, sizeof(uint_4));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap4(&result);
}

void
PPFBinaryInputStream::GetU4s  (uint_4* tab, size_t n)
{
  CheckArrayTag(4,n,PPS_DATATYPE_UNSIGNED);
  GetRawBytes(tab, n*sizeof(uint_4));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap4(tab+i);

  return;
}


void
PPFBinaryInputStream::GetI8   (int_8& result)
{
  CheckTag(8,PPS_DATATYPE_INTEGER);
  GetRawBytes(&result, sizeof(int_8));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&result);
}

void
PPFBinaryInputStream::GetI8s  (int_8* tab, size_t n)
{
  CheckArrayTag(8,n,PPS_DATATYPE_INTEGER);
  GetRawBytes(tab, n*sizeof(int_8));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap8(tab+i);

  return;
}

void
PPFBinaryInputStream::GetU8   (uint_8& result)
{
  CheckTag(8,PPS_DATATYPE_UNSIGNED);
  GetRawBytes(&result, sizeof(uint_8));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&result);
}

void
PPFBinaryInputStream::GetU8s  (uint_8* tab, size_t n)
{
  CheckArrayTag(8,n,PPS_DATATYPE_UNSIGNED);
  GetRawBytes(tab, n*sizeof(uint_8));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap8(tab+i);

  return;
}


void
PPFBinaryInputStream::GetLine(char* ptr, size_t len)
{
  string str;
  GetStr(str);
  strncpy(ptr, str.c_str(), len);
  ptr[len] = '\0';
}

void
PPFBinaryInputStream::GetStr(string& str)
{
  unsigned char ppstype;
  GetTypeTag(ppstype);
  if (ppstype != PPS_STRING) 
    throw FileFormatExc("PPFBinaryInputStream::GetStr   bad type in ppersist file");
  int_4 len;
  GetRawI4(len);
  char * buff = new char[len+1];
  GetRawBytes(buff, len);
  buff[len] = '\0';
  str = buff;
  delete[] buff;
}

void
PPFBinaryInputStream::GetZ4   (complex<r_4>& result)
{
  CheckTag(4,PPS_DATATYPE_COMPLEX);
  r_4 reim[2];
  GetRawBytes(reim, 2*sizeof(r_4));
  if (bigEndian != IS_BIG_ENDIAN) {
    bswap4(reim);
    bswap4(reim+1);
  }
  result = complex<r_4>(reim[0], reim[1]);
}

void
PPFBinaryInputStream::GetZ4s  (complex<r_4>* tab, size_t n)
{  
  CheckArrayTag(4,n,PPS_DATATYPE_COMPLEX);
  GetRawBytes(tab, n*2*sizeof(r_4));
  if (bigEndian == IS_BIG_ENDIAN) return;

  r_4 * p = (r_4 *)tab;
  for (size_t i=0; i<n; i++) {
    bswap4(p);  p++;
    bswap4(p);  p++;
  }
  return;
}

void
PPFBinaryInputStream::GetZ8   (complex<r_8>& result)
{
  CheckTag(8,PPS_DATATYPE_COMPLEX);
  r_8 reim[2];
  GetRawBytes(reim, 2*sizeof(r_8));
  if (bigEndian != IS_BIG_ENDIAN) {
    bswap8(reim);
    bswap8(reim+1);
  }
  result = complex<r_8>(reim[0], reim[1]);
}

void
PPFBinaryInputStream::GetZ8s  (complex<r_8>* tab, size_t n)
{
  CheckArrayTag(8,n,PPS_DATATYPE_COMPLEX);
  GetRawBytes(tab, n*2*sizeof(r_8));
  if (bigEndian == IS_BIG_ENDIAN) return;

  r_8 * p = (r_8 *)tab;
  for (size_t i=0; i<n; i++) {
    bswap8(p);  p++;
    bswap8(p);  p++;
  }
  return;
}

// Support pour les long double (128 bits floating point numbers)
#ifdef SO_LDBLE128
void
PPFBinaryInputStream::GetR16  (r_16& result)
{
  CheckTag(16,PPS_DATATYPE_FLOAT);
  GetRawBytes(&result, sizeof(r_16));
  if (bigEndian != IS_BIG_ENDIAN)
    bswap16(&result);
}

void
PPFBinaryInputStream::GetR16s  (r_16* tab, size_t n)
{
  CheckArrayTag(16,n,PPS_DATATYPE_FLOAT);
  GetRawBytes(tab, n*sizeof(r_16));
  if (bigEndian == IS_BIG_ENDIAN) return;

  for (size_t i=0; i<n; i++)  bswap16(tab+i);

  return;
}

void
PPFBinaryInputStream::GetZ16   (complex<r_16>& result)
{
  CheckTag(16,PPS_DATATYPE_COMPLEX);
  r_16 reim[2];
  GetRawBytes(reim, 2*sizeof(r_16));
  if (bigEndian != IS_BIG_ENDIAN) {
    bswap16(reim);
    bswap16(reim+1);
  }
  result = complex<r_16>(reim[0], reim[1]);
}

void
PPFBinaryInputStream::GetZ16s  (complex<r_16>* tab, size_t n)
{
  CheckArrayTag(16,n,PPS_DATATYPE_COMPLEX);
  GetRawBytes(tab, n*2*sizeof(r_16));
  if (bigEndian == IS_BIG_ENDIAN) return;

  r_16 * p = (r_16 *)tab;
  for (size_t i=0; i<n; i++) {
    bswap16(p);  p++;
    bswap16(p);  p++;
  }
  return;
}
#endif

void
PPFBinaryInputStream::GetPosTagTable(int_8* ptab, size_t sz)
{
  unsigned char ppstype;
  GetTypeTag(ppstype);
  if (ppstype != PPS_POSTAG_TABLE) 
    throw FileFormatExc("PPFBinaryInputStream::GetPosTagTable  bad type in ppersist stream");
  int_4 tsz;
  GetRawI4(tsz);
  if (tsz != (int_4)sz)
    throw FileFormatExc("PPFBinaryInputStream::GetPosTagTable Size mismatch ");
  for(int kk=0; kk<tsz; kk++)
    GetRawI8(ptab[kk]);
  return;
}

void
PPFBinaryInputStream::GetPosTagTable(vector<int_8>& ptab)
{
  unsigned char ppstype;
  GetTypeTag(ppstype);
  if (ppstype != PPS_POSTAG_TABLE) 
    throw FileFormatExc("PPFBinaryInputStream::GetPosTagTable  bad type in ppersist stream");
  ptab.clear();
  int_4 tsz;
  GetRawI4(tsz);
  int_8 tpos;
  for(int kk=0; kk<tsz; kk++) {
    GetRawI8(tpos);
    ptab.push_back(tpos);
  }
  return;
}

/*! Scans the streams and prints in text format to standard output (cout) the PPF 
   stream content. 
   \param lev : print level (level of details) = 0,1,2,3
*/
void
PPFBinaryInputStream::AnalyseTags(int lev)
{
  unsigned char ppstag=0;
  unsigned char ppst1,ppst2,ppst3,ppst30;
  int_8 cpos,fsize;
  uint_8 ui8,cid,oid;
  int_4 i4;
  int_8 i8;
  char * buff;
  string str;
  // Pour indenter lors de l'impression 
  #define _MXINDENT_ 10 
  const char * indents[_MXINDENT_+1] = {"","  ", "    ", "      ", "        ", "          ",
				    "            ", "              ", "                ",
				    "                  ", "                    "};
  int idxindent = 0;
  int idxind = 0;
  cout << "\n ---------------------------------------------------------- " << endl;
  cout << " PPFBinaryInputStream::AnalyseTags(Level= " << lev << ")" << endl; 


  cpos = s->tellg();
  s->seekg(0,ios::end);
  fsize = s->tellg();
  s->seekg(cpos,ios::beg);

  cout << "   FileName= " << FileName() << endl;
  cout << "   Version= " << Version() << " FileSize= " << fsize 
       << " Creation Date= " << CreationDateStr() <<  endl;
  cout << " NbPosTag=" << NbPosTags() << " NbNameTag=" << tags.size() 
       << " NbObjs=" << NbObjects() << " NbTopLevObjs=" << NbTopLevelObjects()
       << " NbRefs=" << NbReferences() << " MaxNest=" << MaxNestedObjsLevel()
       << endl << endl;

  uint_8 totntags = 0;
  bool eofok = false;

  while ( (ppstag != PPS_EOF) &&  (cpos < fsize) ) {
    cpos = s->tellg();
    GetRawUByte(ppstag);
    totntags++;

    ppst1 = ppstag&0x0f;  // bits 0123
    ppst2 = ppstag&0x30;  // bits     45
    ppst3 = ppstag&0xc0;  // bits       67
    ppst30 = ppstag&0xc1; // bits 0     67
    if ((ppst2 == 0) && (ppst3 == 0) ) {
      switch (ppst1) {

      case PPS_NULL :
	if (lev > 1)  cout << indents[idxindent] 
			   << "<PPS_NULL> tag at position " << hex << cpos << dec << endl;
	break;

      case PPS_STRING :
	GetRawI4(i4);
	if (lev > 1)  cout << indents[idxindent]
			   << "<PPS_STRING> tag at position " << hex << cpos << dec 
			   << " Length=" << i4 << endl;
	s->seekg(i4,ios::cur);
	break;

      case PPS_OBJECT :
	GetRawU8(cid);
	GetRawU8(oid);
	cout << indents[idxindent] 
	     << "<PPS_OBJECT> tag at position " << hex << cpos << " ClassId= " << cid 
	     << "  ObjectId= " << oid << dec << endl;
	idxind++;
	idxindent = (idxind <= _MXINDENT_) ? idxind : _MXINDENT_;
	break;

      case PPS_REFERENCE :
	GetRawU8(oid);
	GetRawI8(i8);
	cout << indents[idxindent] 
	     << "<PPS_REFERENCE> tag at position " << hex << cpos << "  ObjectId= " 
	     << oid << "  OrigPos=" << i8 << dec << endl;
	break;

      case PPS_NAMETAG_MARK :
	cout << indents[idxindent] 
	     << "<PPS_NAMETAG_MARK> tag at position " << hex << cpos << dec << endl; 
	break;

      case PPS_POSTAG_MARK :
	GetRawI8(i8);	
	cout << indents[idxindent] 
	     << "<PPS_POSTAG_MARK> tag at position " << hex << cpos 
	     << " TPos=" << i8 << dec << endl; 
	break;
     
      case PPS_ENDOBJECT :
	GetRawU8(oid);
	idxind--;
	idxindent = (idxind >= 0) ? idxind : 0;
	cout << indents[idxindent] 
	     << "<PPS_ENDOBJECT> tag at position " << hex << cpos << "  ObjectId= " 
	     << oid << dec << endl;
	break;

      case PPS_POSTAG_TABLE :
	GetRawI4(i4);
	for(int kkt=0; kkt<i4; kkt++)  GetRawI8(i8);	
	cout << indents[idxindent]
	     << "<PPS_POSTAG_TABLE> tag at position " << hex << cpos << dec << endl; 
	break;

      case PPS_NAMETAG_TABLE :
	if (Version() < 3) {
	  GetRawI8(i8);
	  GetRawI4(i4);
	  buff = new char[i4+1];
	  GetRawBytes(buff, i4); 
	  buff[i4] = '\0';  str = buff;
	  delete[] buff;
	  cout << indents[idxindent]
	       << "<PPS_NAMETAG_TABLE> tag at position " << hex << cpos << dec 
	       << " Name= " << str << endl; 
	}
	else {
	  cout << indents[idxindent]
	       << "<PPS_NAMETAG_TABLE> tag at position " << hex << cpos << dec << endl;
	  int_8 stats[8];
	  GetI8s(stats,8); 
	  GetRawU8(ui8); // nb de nametag
	  for(uint_8 kt=0; kt<ui8; kt++) {
	    string tname;
	    GetRawI8(i8);
	    GetStr(tname);
	    if (lev > 0)  
	      cout << "<PPS_NAMETAG_ENTRY>  NameTag=" << tname 
		   << " NameTagMark Position=" << hex << i8 << dec << endl; 
	  }
	  GetRawI8(i8);  // position debut NameTagTable
	}
	break;

      case PPS_EOF :
	if (Version() < 3) GetRawI8(i8);	
	cout << indents[idxindent]
	     << "<PPS_EOF> tag at position " << hex << cpos  
	     << " TagPos=" << i8 << dec << endl;
	eofok = true;
	break;

      default :
	cerr << " ERROR : Unexpected tag value " << hex << ppstag 
	     << " At position" << cpos << dec << endl;
	throw FileFormatExc("PPFBinaryInputStream::AnalyseTags() - Unexpected tag value !");
      }
    }
    else {
      string dtype = "???? x";
      int_4 dsize = (int_4)tagsize2datasize(ppst1);
      int_8 dsizeskip = dsize;
      if (ppst30 == PPS_DATATYPE_COMPLEX) {
	dtype = "COMPLEX x";
	dsize = (int_4)tagsize2datasize(ppst1-1);
	dsizeskip = 2*dsize;
      }
      else if (ppst3 == PPS_DATATYPE_CHAR) dtype = "CHAR x";
      else if (ppst3 == PPS_DATATYPE_FLOAT) dtype = "FLOAT x";
      else if (ppst3 == PPS_DATATYPE_INTEGER) dtype = "INTEGER x";
      else if (ppst3 == PPS_DATATYPE_UNSIGNED) dtype = "UNSIGNED x";
      char sb[16];
      sprintf(sb, "%d", dsize);
      dtype += sb;

      switch (ppst2) {

      case PPS_SIMPLE :
	if (lev > 2)  cout << indents[idxindent]
			   << "<PPS_SIMPLE> tag at position " << hex << cpos << dec 
			   << " DataType=" << dtype << endl;
	s->seekg(dsizeskip, ios::cur);
	break;

      case PPS_SIMPLE_ARRAY4 :
	GetRawI4(i4);
	if (lev > 0)  cout << indents[idxindent]
			   << "<PPS_SIMPLE_ARRAY4> tag at position " << hex << cpos << dec 
			   << " DataType=" << dtype << " NElts= " << i4 << endl;
	s->seekg(dsizeskip*(int_8)i4, ios::cur);
	break;

      case PPS_SIMPLE_ARRAY8 :
	GetRawU8(ui8);
	if (lev > 0)  cout << indents[idxindent]
			   << "<PPS_SIMPLE_ARRAY8> tag at position " << hex << cpos << dec 
			   << " DataType=" << dtype << " NElts= " << ui8 << endl;
	s->seekg(dsizeskip*ui8, ios::cur);
	break;
      }
    }
  }
  if (!eofok) 
    throw FileFormatExc("PPFBinaryInputStream::AnalyseTags() - Not found <PPS_EOF> tag ");

  cout << " PPFBinaryInputStream::AnalyseTags() - End - Total Number of Tags= " << totntags << endl; 
  cout << " ---------------------------------------------------------- \n" << endl;
  return;
}


//-------------------------------------------------------------------------
//-------------------  Classe PPFBinaryOutputStream -----------------------
//-------------------------------------------------------------------------


/*!
   \class SOPHYA::PPFBinaryInputStream
   \ingroup BaseTools
   PPF output stream implementing write operations. The byte-ordering (endianess) 
   of the output stream can be selected at creation. By default, the native endianness 
   of the machine is used.  
*/

/*! Constructor from a RawInOutStream pointer
  \param os : pointer to RawInOutStream
  \param ad : if true, the RawInOutStream \b os is deleted by the destructor 
  \param endianness : Endianness selector  PPS_NATIVE PPS_LITTLE_ENDIAN PPS_BIG_ENDIAN 
*/ 
PPFBinaryOutputStream::PPFBinaryOutputStream(RawInOutStream* os, bool ad, int endianness)
{
  s = os;
  _ads = ad;
  Init(endianness);
}

/*! Constructor 
  \param flnm : output file name 
  \param endianness : Endianness selector  PPS_NATIVE PPS_LITTLE_ENDIAN PPS_BIG_ENDIAN 
*/ 
PPFBinaryOutputStream::PPFBinaryOutputStream(string const& flnm, int endianness)
{
  // Output stream creation
  s = new RawOutFileStream(flnm.c_str());
  _ads = true;
  Init(endianness);
}

PPFBinaryOutputStream::~PPFBinaryOutputStream()
{
  WriteNameTagTable();
  if (_ads && (s != NULL)) delete s;   // Close the stream
}

void
PPFBinaryOutputStream::Init(int endianness)
{
  if (endianness == -1)
    bigEndian = IS_BIG_ENDIAN;
  else
    bigEndian = endianness;

  version = 3;
  // Header
  PutRawBytes("SOS-SOPHYA-PPersistFile V4               ",32);
  PutRawBytes(bigEndian 
	   ? "BIG-ENDIAN                             "
	   : "LITTLE-ENDIAN                          ",32);

// ---- GMT creation date of the file
  time_t tm = time(NULL);
  creationdate = tm;
  char datestring[33];
  int l=strftime(datestring,32,"%d/%m/%Y %H:%M:%S GMT",gmtime(&tm));
  for(int i=l; i<32; i++)  datestring[i] = ' ';
  datestring[32] = '\0'; 
  PutRawBytes(datestring, 32);
  // we flush the stream to make it possible to open the same file as PPFBinaryInputStream
  getStream().flush();
}

void
PPFBinaryOutputStream::WriteNameTagTable()
{  
  int_8 tagPos;
  tagPos = s->tellp();
  PutRawUByte(PPS_NAMETAG_TABLE);  //   NameTagTable tag
  // Ecriture nb de PosTag et nb d'objets dans le flot
  int_8 stats[8] = {0,0,0,0,0,0,0,0};
  stats[0] = _nbpostag;
  stats[1] = _nbobjs;
  stats[2] = _nbtlobjs;
  stats[3] = _nbrefs;
  stats[4] = _maxnestlevel;
  PutI8s(stats, 8);
  // Ecriture nb de tag et les tags
  PutRawU8((uint_8)tags.size());    // Number of tags
  if (tags.size() > 0) {
    for (map<string,int_8>::iterator i = tags.begin(); i != tags.end(); i++) {
      int_8 pos = (*i).second;
      PutRawI8(pos);
      PutStr((*i).first);
    }
  }
  PutRawI8(tagPos);
  PutRawUByte(PPS_EOF);
  return;
}

void
PPFBinaryOutputStream::WriteNameTagTableV2()
{
  if (tags.size() == 0) {
    PutRawUByte(PPS_EOF);
    PutRawI8(-1);
  } else {
    int_8 tagPos;
    tagPos = s->tellp();
    for (map<string,int_8>::iterator i = tags.begin(); i != tags.end(); i++) {
      string name = (*i).first;
      int_8 pos = (*i).second;
      PutRawUByte(PPS_NAMETAG_TABLE);                       // This is a tag
      PutRawI8(pos);                             // position of previous tag
      PutRawI4(name.length());                   // length of the name
      PutRawBytes(name.c_str(), name.length());  // name, without final "0".
    }
    PutRawUByte(PPS_EOF);
    PutRawI8(tagPos);
  }

}


int_8
PPFBinaryOutputStream::WritePositionTag()
{
  int_8 tagpos;
  tagpos = s->tellp();
  PutRawByte(PPS_POSTAG_MARK);
  PutRawI8(tagpos);
  _nbpostag++;  // Compteur de nombre de tags 
  return(tagpos);
}

void
PPFBinaryOutputStream::WriteNameTag(string const& name)
{
  //  if (name.length() > MAXTAGLEN_V2) 
  //  throw ParmError("PPFBinaryOutputStream::WriteNameTag   tag name too long");

  if (tags.find(name) != tags.end())
    throw DuplicateIdExc("PPFBinaryOutputStream::WriteNameTag   duplicate tag name");

  // Get current file position
  int_8 tagPos;
  tagPos = s->tellp();

  tags[name] = tagPos;
  PutRawUByte(PPS_NAMETAG_MARK);                       // This is a tag
  //  objList.clear(); // $CHECK$ EA 171199  - Ne pas faire ? Reza 03/2000
}


//++
// void PPFBinaryOutputStream::PutByte(char& c)
// void PPFBinaryOutputStream::PutBytes(void const* ptr, size_t bytes)
// void PPFBinaryOutputStream::PutR4   (r_4 result)
// void PPFBinaryOutputStream::PutR4s  (r_4 const* tab, size_t n)
// void PPFBinaryOutputStream::PutR8   (r_8 result)
// void PPFBinaryOutputStream::PutR8s  (r_8 const* tab, size_t n)
// void PPFBinaryOutputStream::PutI2   (int_2 result)
// void PPFBinaryOutputStream::PutI2s  (int_2 const* tab, size_t n)
// void PPFBinaryOutputStream::PutU2   (uint_2 result)
// void PPFBinaryOutputStream::PutU2s  (uint_2 const* tab, size_t n)
// void PPFBinaryOutputStream::PutI4   (int_4 result)
// void PPFBinaryOutputStream::PutI4s  (int_4 const* tab, size_t n)
// void PPFBinaryOutputStream::PutU4   (uint_4 result)
// void PPFBinaryOutputStream::PutU4s  (uint_4 const* tab, size_t n)
// void PPFBinaryOutputStream::PutI8   (int_8 result)
// void PPFBinaryOutputStream::PutI8s  (int_8 const* tab, size_t n)
// void PPFBinaryOutputStream::PutU8   (uint_8 result)
// void PPFBinaryOutputStream::PutU8s  (uint_8 const* tab, size_t n)
// void PPFBinaryOutputStream::PutStr  (string const&)
//	Ecriture de données portables.. Pour chaque type
//	de données, on peut écrire une valeur, ou un tableau de valeurs.
// void PPFBinaryOutputStream::PutLine(char const* ptr, size_t len)
//	Ecriture d'une ligne de texte dans le fichier PPersist.
//--




void
PPFBinaryOutputStream::PutRawBytes(void const* ptr, size_t bytes)
{
  s->write((char const*)ptr, bytes);
}

void
PPFBinaryOutputStream::PutRawByte(char c)
{
  PutRawBytes(&c, 1);
}

void
PPFBinaryOutputStream::PutRawUByte(unsigned char c)
{
  PutRawBytes(&c, 1);
}

void
PPFBinaryOutputStream::PutRawI2   (int_2 val)
{
  if (bigEndian != IS_BIG_ENDIAN)
    bswap2(&val);

  PutRawBytes(&val, sizeof(int_2));
}

void
PPFBinaryOutputStream::PutRawI4   (int_4 val)
{
  if (bigEndian != IS_BIG_ENDIAN)
    bswap4(&val);

  PutRawBytes(&val, sizeof(int_4));
}

void
PPFBinaryOutputStream::PutRawI8   (int_8 val)
{
  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&val);

  PutRawBytes(&val, sizeof(int_8));
}

void
PPFBinaryOutputStream::PutRawU8   (uint_8 val)
{
  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&val);

  PutRawBytes(&val, sizeof(uint_8));
}

void
PPFBinaryOutputStream::PutArrayTag(unsigned char datasz, size_t sz, unsigned char datatype) 
// datatype = PPS_DATATYPE_CHAR or PPS_DATATYPE_FLOAT or PPS_DATATYPE_INTEGER or PPS_DATATYPE_UNSIGNED
{
  if (sz <= 0x7fffffff) {
    PutRawUByte(PPS_SIMPLE_ARRAY4 + datasize2tagsize(datasz) + datatype);
    PutRawI4(sz);
  } else {
    PutRawUByte(PPS_SIMPLE_ARRAY8 + datasize2tagsize(datasz) + datatype);
    PutRawU8(sz);
  } 
}

void
PPFBinaryOutputStream::PutByte(char c)
{
  PutRawByte(PPS_SIMPLE + datasize2tagsize(1) + PPS_DATATYPE_CHAR);
  PutRawBytes(&c, 1);
}



void
PPFBinaryOutputStream::PutBytes(void const* ptr, size_t bytes)
{
  PutArrayTag(1, bytes, PPS_DATATYPE_CHAR);
  PutRawBytes(ptr, bytes);
}

void
PPFBinaryOutputStream::PutR4   (r_4 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(4) + PPS_DATATYPE_FLOAT);
  
  if (bigEndian != IS_BIG_ENDIAN)
    bswap4(&val);

  PutRawBytes(&val, sizeof(r_4));
}

void
PPFBinaryOutputStream::PutR4s  (r_4 const* tab, size_t n)
{
  PutArrayTag(4, n, PPS_DATATYPE_FLOAT);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(r_4));
  } else {
    for (size_t i=0; i<n; i++) {
      r_4 val = tab[i];
      bswap4(&val);
      PutRawBytes(&val, sizeof(r_4));
    }
  }
}

void
PPFBinaryOutputStream::PutR8   (r_8 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(8) + PPS_DATATYPE_FLOAT);

  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&val);

  PutRawBytes(&val, sizeof(r_8));
}

void
PPFBinaryOutputStream::PutR8s  (r_8 const* tab, size_t n)
{
  PutArrayTag(8, n, PPS_DATATYPE_FLOAT);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(r_8));
  } else {
    for (size_t i=0; i<n; i++) {
      r_8 val = tab[i];
      bswap8(&val);
      PutRawBytes(&val, sizeof(r_8));
    }
  }
}

void
PPFBinaryOutputStream::PutI1   (int_1 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(1) + PPS_DATATYPE_INTEGER);
  PutRawBytes(&val, sizeof(int_1));
}

void
PPFBinaryOutputStream::PutI1s  (int_1 const* tab, size_t n)
{
  PutArrayTag(1, n, PPS_DATATYPE_INTEGER);
  PutRawBytes(tab, n*sizeof(int_1));
}

void
PPFBinaryOutputStream::PutU1   (uint_1 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(1) + PPS_DATATYPE_UNSIGNED);
  PutRawBytes(&val, sizeof(uint_1));
}

void
PPFBinaryOutputStream::PutU1s  (uint_1 const* tab, size_t n)
{
  PutArrayTag(1, n, PPS_DATATYPE_UNSIGNED);
  PutRawBytes(tab, n*sizeof(uint_1));
}

void
PPFBinaryOutputStream::PutI2   (int_2 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(2) + PPS_DATATYPE_INTEGER);

  if (bigEndian != IS_BIG_ENDIAN)
    bswap2(&val);

  PutRawBytes(&val, sizeof(int_2));
}

void
PPFBinaryOutputStream::PutI2s  (int_2 const* tab, size_t n)
{
  PutArrayTag(2, n, PPS_DATATYPE_INTEGER);
  
  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(int_2));
  } else {
    for (size_t i=0; i<n; i++) {
      int_2 val = tab[i];
      bswap2(&val);
      PutRawBytes(&val, sizeof(int_2));
    }
  }
}

void
PPFBinaryOutputStream::PutU2   (uint_2 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(2) + PPS_DATATYPE_UNSIGNED);

  if (bigEndian != IS_BIG_ENDIAN)
    bswap2(&val);

  PutRawBytes(&val, sizeof(uint_2));
}

void
PPFBinaryOutputStream::PutU2s  (uint_2 const* tab, size_t n)
{
  PutArrayTag(2, n, PPS_DATATYPE_UNSIGNED);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(uint_2));
  } else {
    for (size_t i=0; i<n; i++) {
      uint_2 val = tab[i];
      bswap2(&val);
      PutRawBytes(&val, sizeof(uint_2));
    }
  }
}

void
PPFBinaryOutputStream::PutI4   (int_4 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(4) + PPS_DATATYPE_INTEGER);

  if (bigEndian != IS_BIG_ENDIAN)
    bswap4(&val);

  PutRawBytes(&val, sizeof(int_4));
}

void
PPFBinaryOutputStream::PutI4s  (int_4 const* tab, size_t n)
{
  PutArrayTag(4, n, PPS_DATATYPE_INTEGER);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(int_4));
  } else {
    for (size_t i=0; i<n; i++) {
      int_4 val = tab[i];
      bswap4(&val);
      PutRawBytes(&val, sizeof(int_4));
    }
  }
}

void
PPFBinaryOutputStream::PutU4   (uint_4 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(4) + PPS_DATATYPE_UNSIGNED);

  if (bigEndian != IS_BIG_ENDIAN)
    bswap4(&val);

  PutRawBytes(&val, sizeof(uint_4));
}

void
PPFBinaryOutputStream::PutU4s  (uint_4 const* tab, size_t n)
{
  PutArrayTag(4, n, PPS_DATATYPE_UNSIGNED);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(uint_4));
  } else {
    for (size_t i=0; i<n; i++) {
      uint_4 val = tab[i];
      bswap4(&val);
      PutRawBytes(&val, sizeof(uint_4));
    }
  }
}

void
PPFBinaryOutputStream::PutI8   (int_8 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(8) + PPS_DATATYPE_INTEGER);

  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&val);

  PutRawBytes(&val, sizeof(int_8));
}

void
PPFBinaryOutputStream::PutI8s  (int_8 const* tab, size_t n)
{
  PutArrayTag(8, n, PPS_DATATYPE_INTEGER);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(int_8));
  } else {
    for (size_t i=0; i<n; i++) {
      int_8 val = tab[i];
      bswap8(&val);
      PutRawBytes(&val, sizeof(int_8));
    }
  }
}

void
PPFBinaryOutputStream::PutU8   (uint_8 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(8) + PPS_DATATYPE_UNSIGNED);

  if (bigEndian != IS_BIG_ENDIAN)
    bswap8(&val);

  PutRawBytes(&val, sizeof(uint_8));
}

void
PPFBinaryOutputStream::PutU8s  (uint_8 const* tab, size_t n)
{
  PutArrayTag(8, n, PPS_DATATYPE_UNSIGNED);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(uint_8));
  } else {
    for (size_t i=0; i<n; i++) {
      uint_8 val = tab[i];
      bswap8(&val);
      PutRawBytes(&val, sizeof(uint_8));
    }
  }
}

void
PPFBinaryOutputStream::PutStr(string const& str) 
{
  PutRawUByte(PPS_STRING);
  PutRawI4(str.length());
  PutRawBytes(str.c_str(), str.length());
}

void
PPFBinaryOutputStream::PutLine(char const* ptr, size_t len)
{
  string str = ptr;
  PutStr(str);
}


void
PPFBinaryOutputStream::PutZ4   (complex<r_4> val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(4) + PPS_DATATYPE_COMPLEX);
  r_4 reim[2];
  reim[0] = val.real();
  reim[1] = val.imag();
  if (bigEndian != IS_BIG_ENDIAN) {
    bswap4(reim);
    bswap4(reim+1);
  }
  PutRawBytes(reim, 2*sizeof(r_4));
}

void
PPFBinaryOutputStream::PutZ4s  (complex<r_4> const* tab, size_t n)
{
  PutArrayTag(4, n, PPS_DATATYPE_COMPLEX);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*2*sizeof(r_4));
  } else {
    for (size_t i=0; i<n; i++) {
      r_4 reim[2];
      reim[0] = tab[i].real();
      reim[1] = tab[i].imag();
      bswap4(reim);
      bswap4(reim+1);
      PutRawBytes(reim, 2*sizeof(r_4));
    }
  }
}

void
PPFBinaryOutputStream::PutZ8   (complex<r_8> val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(8) + PPS_DATATYPE_COMPLEX);
  r_8 reim[2];
  reim[0] = val.real();
  reim[1] = val.imag();
  if (bigEndian != IS_BIG_ENDIAN) {
    bswap8(reim);
    bswap8(reim+1);
  }
  PutRawBytes(reim, 2*sizeof(r_8));
}

void
PPFBinaryOutputStream::PutZ8s  (complex<r_8> const* tab, size_t n)
{
  PutArrayTag(8, n, PPS_DATATYPE_COMPLEX);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*2*sizeof(r_8));
  } else {
    for (size_t i=0; i<n; i++) {
      r_8 reim[2];
      reim[0] = tab[i].real();
      reim[1] = tab[i].imag();
      bswap8(reim);
      bswap8(reim+1);
      PutRawBytes(reim, 2*sizeof(r_8));
    }
  }
}

// Support pour les long double (128 bits floating point numbers)
#ifdef SO_LDBLE128
void
PPFBinaryOutputStream::PutR16   (r_16 val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(16) + PPS_DATATYPE_FLOAT);

  if (bigEndian != IS_BIG_ENDIAN)
    bswap16(&val);

  PutRawBytes(&val, sizeof(r_16));
}

void
PPFBinaryOutputStream::PutR16s  (r_16 const* tab, size_t n)
{
  PutArrayTag(16, n, PPS_DATATYPE_FLOAT);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*sizeof(r_16));
  } else {
    for (size_t i=0; i<n; i++) {
      r_16 val = tab[i];
      bswap16(&val);
      PutRawBytes(&val, sizeof(r_16));
    }
  }
}

void
PPFBinaryOutputStream::PutZ16   (complex<r_16> val)
{
  PutRawUByte(PPS_SIMPLE + datasize2tagsize(16) + PPS_DATATYPE_COMPLEX);
  r_16 reim[2];
  reim[0] = val.real();
  reim[1] = val.imag();
  if (bigEndian != IS_BIG_ENDIAN) {
    bswap16(reim);
    bswap16(reim+1);
  }
  PutRawBytes(reim, 2*sizeof(r_16));
}

void
PPFBinaryOutputStream::PutZ16s  (complex<r_16> const* tab, size_t n)
{
  PutArrayTag(16, n, PPS_DATATYPE_COMPLEX);

  if (bigEndian == IS_BIG_ENDIAN) {
    PutRawBytes(tab, n*2*sizeof(r_16));
  } else {
    for (size_t i=0; i<n; i++) {
      r_16 reim[2];
      reim[0] = tab[i].real();
      reim[1] = tab[i].imag();
      bswap16(reim);
      bswap16(reim+1);
      PutRawBytes(reim, 2*sizeof(r_16));
    }
  }
}
#endif

void
PPFBinaryOutputStream::PutPosTagTable(int_8 const * ptab, size_t sz)
{
  PutRawUByte(PPS_POSTAG_TABLE);
  int_4 tsz = sz;
  PutRawI4(tsz);
  for(int kk=0; kk<tsz; kk++)
    PutRawI8(ptab[kk]);
  return;
}

void
PPFBinaryOutputStream::PutPosTagTable(vector<int_8> const& ptab)
{
  PutRawUByte(PPS_POSTAG_TABLE);
  int_4 tsz = ptab.size();
  PutRawI4(tsz);
  for(int kk=0; kk<tsz; kk++)
    PutRawI8(ptab[kk]);
  return;
}


