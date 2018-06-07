// this :
#include <HEPVis/SbFITS.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define CDumpF printf
#define CWarn printf
#define CWarnF printf

static void StripString(char*,char,char);
static int FindPosition(char*,char);

#define BUFF_SIZE 4096
#define HEAD_SIZE 2880

short* SbFITS::getImageFromStream(
 FILE* aFile
,int aXorg   
,int aYorg          
,int aWidth         
,int aHeight  
,int aSwap    
){
/* Fonction  READ_FITS  :  Lecture d'un fichier au format fits ds une       */
/* aFname     Nom du fichier image                                         */
/* aXorg     x Offset par rapport a l'origine du fichier                  */
/* aYorg     y Offset par rapport a l'origine du fichier                  */
/* aWidth                                                                  */
/* aHeight                                                                 */
/* aSwap      Swap flag =0 No swap                                         */
/*                                                  R.Ansari  08/90         */
  if(aFile==NULL) return NULL;
  if((aWidth==0)||(aHeight==0)) return NULL;
  rewind(aFile);
  if ((aXorg<0)||(aYorg<0)) {
    CWarnF("SbFITS::getImageFromStream : bad origin value : %d %d\n",
           aXorg,aYorg);
    return NULL;
  }
  short* picmap = (short*) new short[aWidth * aHeight];
  if(picmap==NULL) return NULL;
  char* buff = (char*)new char[BUFF_SIZE];
  if(buff==NULL) {
    delete [] picmap;
    return NULL;
  }
  // Header :
  int x_axe = 0;
  int y_axe = 0;
  int i,j,k;

  size_t len = ::fread(buff,(size_t)(1),(size_t)(HEAD_SIZE),aFile);
  // Header formatted with groups of 80 characters  :
  char* cpt;
  char* mcpt;
  char* argpt;
  char c;
  for(k=0;k<36;k++) {
    cpt = buff + k * 80;
    c = *(cpt+80);
    *(cpt+80) = '\0';
    StripString(cpt,'L',' ');
    i = FindPosition(cpt,' ');
    *(cpt+i) = '\0';
    mcpt = cpt;
    argpt = cpt+i+1;
    i = FindPosition(argpt,'=');
    argpt += i+1;
    StripString(argpt,'L',' ');
    j = FindPosition(argpt,' ');
    *(argpt+j) = '\0';
    *(cpt+80) = c;
    
    // Decodage des longueurs des axes  :
    if (strcmp(mcpt,"NAXIS1")==0) { // Axes X : Nb Pixel/ligne.
      x_axe = atoi(argpt);
    } 
    if (strcmp(mcpt,"NAXIS2")==0) { // Axes Y : Nb de lignes.
      y_axe = atoi(argpt);
    } 
    if (strcmp(mcpt,"END")==0) break;;
  }

  if(x_axe==0) {
    x_axe = 1024;
    CWarn("Axe X prise par defaut = 1024.\n");
  }
  if(y_axe==0) {
    y_axe = 1024;
    CWarn("Axe Y prise par defaut = 1024.\n");
  }

  /*CInfoF("Axes decodes : X=%d, Y=%d.\n",x_axe,y_axe);*/

  int xmx=x_axe-aXorg;
  int ymx=y_axe-aYorg;
  if(xmx>aWidth) xmx=aWidth;
  if(ymx>aHeight) ymx=aHeight;

  if((xmx<0)||(ymx<0)) {
    CWarn("SbFITS::getImageFromStream : bad size value.\n");
    delete [] buff;
    delete [] picmap;
    return NULL;
  }

  long int totr=0;
  // On se positionne au debut de la portion a lire :
  long int lo = (aYorg * x_axe + aXorg) * 2;
  fseek(aFile,lo,SEEK_CUR);
  lo = (x_axe - xmx)*2;

  long int lrd;
  if(aSwap == 0) { // On lit SANS byteSwapper :
    short* sipt;
    for(j=0;j<ymx;j++) {
      sipt = picmap + j * aWidth;
      lrd = fread(sipt,(size_t)sizeof(short),(size_t)(xmx),aFile);
      if(lrd!=xmx) {
        CWarn("SbFITS::getImageFromStream : IO problem\n");
        delete [] buff;
        delete [] picmap;
        return NULL;
      }
      fseek(aFile,lo,SEEK_CUR); // Go to next ligne.
      totr += 2 * lrd;
    }
  } else {
    // byteSwappe :
    char* cpt;
    for(j=0;j<ymx;j++) {
      lrd=fread(buff,(size_t)(2),(size_t)(xmx),aFile);
      if(lrd != xmx) {
        CWarn("SbFITS::getImageFromStream : IO problem\n");
        delete [] buff;
        delete [] picmap;
        return NULL;
      }
      // byteaSwapp :
      cpt=(char*)((picmap)+j * aWidth);
      for(i=0;i<2*xmx;i+=2) {
        *(cpt+i)=buff[i+1];
        *(cpt+i+1)=buff[i];
      }
      fseek(aFile,lo,SEEK_CUR); // begin of next ligne.
      totr=totr+2*lrd;
    }
  }

  /*CInfoF("Fichier lu (total %d bytes).\n",totr);*/

  //  Si la portion de l'image lue est plus petite 
  // que la taille de la structure
  //  On met a zero la partie non utilisee.
  if(ymx<aHeight) {
    for(j=ymx;j<aHeight;j++) {
      for(i=0;i<aWidth;i++)  *(picmap+(j * aWidth+i))='\0';
    }
  }
  if(xmx<aWidth) {
    for(j=0;j<ymx;j++) {
      for(i=xmx;i<aWidth;i++)  *(picmap+(j * aWidth+i))='\0';
    }
  }
  delete [] buff;
  return picmap;
}

void SbFITS::getHeaderFromStream(FILE* aFile,int& aSizeX,int& aSizeY){
  aSizeX = 0;
  aSizeY = 0;
  if(aFile==NULL) return;
  rewind(aFile);

  char* buff = (char*)new char[BUFF_SIZE];
  if(buff==NULL) return;

  // Read header :
  int x_axe=0;
  int y_axe=0;

  size_t len = ::fread(buff,(size_t)(1),(size_t)(HEAD_SIZE),aFile);
  // Header formatted with groups of 80 characters  :
  char* cpt;
  char* mcpt;
  char* argpt;
  char c;
  int i,j,k;
  for(k=0;k<35;k++) {
    cpt = buff + k * 80;
    c = *(cpt+80);
    *(cpt+80) = '\0';
    StripString(cpt,'L',' ');
    i = FindPosition(cpt,' ');
    *(cpt+i) = '\0';
    mcpt = cpt;
    argpt = cpt+i+1;
    i = FindPosition(argpt,'=');
    argpt += i+1;
    StripString(argpt,'L',' ');
    j = FindPosition(argpt,' ');
    *(argpt+j) = '\0';
    *(cpt+80) = c;
    
    // Decodage des longueurs des axes :
    if (strcmp(mcpt,"NAXIS1")==0) { // Axes X : Nb Pixel/ligne.
      x_axe = atoi(argpt);
    } 
    if (strcmp(mcpt,"NAXIS2")==0) { // Axes Y : Nb de lignes.
      y_axe = atoi(argpt);
    } 
    if (strcmp(mcpt,"END")==0) break;
  }

  if (x_axe==0) {
    x_axe = 1024;
    CWarn ("Axe X prise par defaut = 1024.\n");
  }
  if (y_axe==0) {
    y_axe = 1024;
    CWarn ("Axe Y prise par defaut = 1024.\n");
  }
  
  /*CInfoF("Axes decodes : X=%d ,Y=%d.\n",x_axe,y_axe);*/

  aSizeX = x_axe;
  aSizeY = y_axe;
  delete [] buff;
}

void SbFITS::dumpHeaderInStream(FILE* aFile){
  if(aFile==NULL) return;
  rewind(aFile);

  char* buff = (char*)new char[BUFF_SIZE];
  if(buff==NULL) return;

  // Lecture entete :
  int x_axe=0;
  int y_axe=0;

  size_t len = ::fread(buff,(size_t)(1),(size_t)(HEAD_SIZE),aFile);
  // Entete formatte sous forme de groupes de 80 caracteres :
  int i,j,k;
  char* cpt;
  char* mcpt;
  char* argpt;
  char c;
  for(k=0;k<35;k++) {
    cpt = buff + k * 80;
    c = *(cpt+80);
    *(cpt+80) = '\0';
    StripString(cpt,'L',' ');
    i = FindPosition(cpt,' ');
    *(cpt+i) = '\0';
    mcpt = cpt;
    argpt = cpt+i+1;
    i = FindPosition(argpt,'=');
    argpt += i+1;
    StripString(argpt,'L',' ');
    j=FindPosition(argpt,' ');
    *(argpt+j) = '\0';
    *(cpt+80) = c;
      
    // Decodage des longueurs des axes :
    CDumpF("%s %s\n",mcpt,argpt);
    
    if (strcmp(mcpt,"NAXIS1")==0) { // Axes X : Nb Pixel/ligne.
      x_axe = atoi(argpt);
    } 
    if (strcmp(mcpt,"NAXIS2")==0) { // Axes Y : Nb de lignes.
      y_axe = atoi(argpt);
    } 
    if(strcmp(mcpt,"END")==0) break;
  }

  if(x_axe==0) {
    x_axe = 1024;
    CWarn ("Axe X prise par defaut = 1024.\n");
  }
  if(y_axe==0) {
    y_axe = 1024;
    CWarn ("Axe Y prise par defaut = 1024.\n");
  }

  /*CInfoF("Axes decodes : X=%d ,Y=%d.\n",x_axe,y_axe);*/

  delete [] buff;
}

void SbFITS::putImageInStream (
 FILE* aFile
,short* aData
,int aWidth
,int aHeight
,int aSwap        
){
/* Fonction  WRITE_FITS  :  Ecritue d'un fichier au format fits depuis une  */
/* structure image.                                                         */
/* aSwap Swap flag =0 Noswap                                               */
  if(aFile==NULL) return;
  if(aData==NULL) return;
  if( (aWidth<=0) || (aHeight<=0) ) return;
  short* picmap = aData;
  char* buff = (char*)new char[BUFF_SIZE];
  if(buff==NULL)  return;

  // Header :
  sprintf(buff,"SIMPLE  =                    T   / Format standard %40s"," ");
  sprintf(buff+80,"BITPIX  =                   16   / 2 Byte per pixel %40s"," ");
  sprintf(buff+160,"NAXIS   =                    2   / 2 axes  %40s"," ");
  sprintf(buff+240,"NAXIS1  = %20u   / Nb de colonnes %40s",aWidth," ");
  sprintf(buff+320,"NAXIS2  = %20u   / Nb de lignes %40s"  ,aHeight," ");
  sprintf(buff+400,"ORIGIN  = 'Image             '  / HEPVis              %40s"," ");
  sprintf(buff+480,"OBJECT  = 'Image Traite      '  / Resultat traitement %40s"," ");
  sprintf(buff+560,"CRVAL1  =         .1000000E+01  / Pour                %40s"," ");
  sprintf(buff+640,"CRPIX1  =                  1.0  / faire               %40s"," ");
  sprintf(buff+720,"CDELT1  =         .1000000E+01  / plaisir             %40s"," ");
  sprintf(buff+800,"CRVAL2  =         .1000000E+01  / a                   %40s"," ");
  sprintf(buff+880,"CRPIX2  =                  1.0  / Luciano             %40s"," ");
  sprintf(buff+960,"CDELT2  =         .1000000E+01  / et Jim              %40s"," ");
  sprintf(buff+1040,"END    ");
  long int i;
  for(i=1045;i<HEAD_SIZE;i++)  *(buff+i)=' ';
  fwrite(buff,(size_t)(1),(size_t)(HEAD_SIZE),aFile);

/*   Taille total image   */
  int TotNbPix = aWidth * aHeight;
        
  if(aSwap == 0) {     /*  On ecrit SANS byteSwapper : */
    fwrite(picmap,(size_t)(sizeof(short int)),(size_t)(TotNbPix),aFile);
  } else {
    long int j,imx;
    char* cpt;
    for(j=0;j<TotNbPix;j+=BUFF_SIZE/2) {       /*  On byteSwappe : */
                                               /* j : index en short ints */
      imx = TotNbPix-j;
      if (imx > BUFF_SIZE/2)  
        imx = BUFF_SIZE/2;  /* imx : en nombre de short ints  */
      cpt=(char *)((picmap)+j);              /* picmap est un short int * */
      for(i=0;i<2*imx;i+=2) {
        buff[i+1]=(*(cpt+i));
        buff[i]=(*(cpt+i+1));
      }
      fwrite(buff,(size_t)(sizeof(short int)),(size_t)(imx),aFile);
    }
  }

  /*CInfoF("Fichier Ecrit (total %d pixels, %d bytes).\n",TotNbPix,TotNbPix*2+HEAD_SIZE);*/

  delete [] buff;
}

void StripString(char* aCst,char aOpt,char aBc){
  int ls = strlen(aCst);
  if (aOpt == 'L' || aOpt == 'B') {
    int i = 0;
    while(aCst[i] == aBc) i++;
    if (i != 0) {
      for(int j=i;j<ls;j++)  aCst[j-i] = aCst[j];
      ls -= i;
      aCst[ls] = '\0';
    }
  }
  if ((aOpt == 'T' || aOpt == 'B') && ls > 0) {
    int i = ls-1;
    while(aCst[i] == aBc)  i--;
    aCst[i+1] = '\0';
  }
}

int FindPosition(char* aCst,char aSec){
  int ls = strlen(aCst);
  for(int i=0;i<ls;i++) {
    if (aCst[i] == aSec)  return(i);
  }
  return (-1);
}




