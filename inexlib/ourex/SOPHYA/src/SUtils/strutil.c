#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "strutil.h"

/* 
++ 
  Module	Strutil (C)
  Lib	LibsUtil
  include	strutil.h

	Groupe de fonction de gestion et manipulation
	des chaines de caracteres.
--
*/

/*
++
  char* itoc(int in, char* s, int nDigits)
	conversion entier-chaine de caracteres
--
*/
/*
++
  ctoi(const char* cst, int *ipt)
	convertit une chaine de caractere en entier
--
*/
/*
++
  ctol(const char* cst, long int *lpt)
	convertit une chaine de caractere en entier long
--
*/
/*
++
  ctof(const char* cst, double *fpt)
	convertit une chaine de caractere en flottant
--
*/
/*
++
  posc(const char* cst, char sec)
	trouve la position d'un caractere dans une chaine
	(premier charactere rencontre)
--
*/
/*
++
  poslc(const char* cst, char sec)
	trouve la position d'un caractere dans une chaine
	(dernier charactere rencontre)
--
*/
/*
++
  strip(char cst[], char opt, char bc)
	permet d'enlever les blancs au debut ou a la fin d'une
	chaine de caractere
--
*/
/*
++
  padstr(spo,lpa,bc)
	Cette  fonction  rajoute  des blancs a la fin de la chaine `spo'
	pour amener la longueur de la chaine a `lpa'. Les caracteres
	rajoutes a la fin sont `bc'. La fonction retourne le nombre de
	blancs (caracteres bc)  rajoutes a la fin.
--
*/
/*
++
  tomaj(char *cbuff)
	Changement en majuscule d'une chaine de caracteres
--
*/
/*
++
  tomin(char *cbuff)
	Changement en minuscule d'une chaine de caracteres
--
*/
/*
++
  efface(char *cbuff,int lbuff,char c)
	Remplace un caractere avec des blancs
--
*/
/*
++
  csh_parse(const char* x, const char* yp)
	Compare la chaine de caracteres `x' avec la chaine `yp'
	comprenant des regles de substitution type c-shell
	(asterisque et point d'interrogation).
--
*/
/*
  DecArgList()
	Decoder une liste du type a,b,c-d,e,f-g,h...
*/
 

/*            Fonction  itoc()    */
char* itoc(int in, char* s, int nDigits)
{
  int i,j,n;
  n=abs(in);
  for(i=0;i<nDigits;i++)
    s[i]=' ';
  s[nDigits-1]='\0';
  j=nDigits-2;
  for (i=j; i>=0; i--)
   {
    int x = n % 10;
    n /= 10;
    s[i] = '0' + x;
    if(n==0) break;
   }
  if(in < 0 )
   {
    s[i-1]='-';
   } 
 return (s);
 }
 
/*            Fonction  ctoi()    */
int ctoi(const char* cst, int *ipt)
{
int i,j,k,l,ls,isgn;
ls = strlen(cst)-1;
if (ls < 0)  return (-1);
j = -1;
isgn = l = 1;
k = 0;
if(cst[0] == '-')
  {
  j = 0;
  isgn = -1;
  }
if(cst[0] == '+') j = 0;
for(i=ls;i>j;i--)
  {
  if(isdigit(cst[i]) == 0)  return (-1);
  k += (cst[i] - '0')*l;
  l *= 10;
  }
*ipt = k*isgn;
return (1);
}
/*                                                       */
/*            Fonction  ctol()    */
int ctol(const char* cst, long int *lpt)
{
int i,j,ls,isgn;
long k,l;
ls = strlen(cst)-1;
if (ls < 0)  return (-1);
j = -1;
isgn = l = 1;
k = 0;
if(cst[0] == '-')
  {
  j = 0;
  isgn = -1;
  }
if(cst[0] == '+') j = 0;
for(i=ls;i>j;i--)
  {
  if(isdigit(cst[i]) == 0)  return (-1);
  k += (cst[i] - '0')*l;
  l *= 10;
  }
*lpt = k*isgn;
return (1);
}
/*              Fonction  ctof()           */
int ctof(const char* cst, double *fpt)
{
int i,jb,pv,ls,isgn;
double v1,v2;
ls = strlen(cst);
if (ls < 0)  return (-1);
jb = -1;
isgn = 1;
v1 = 0.;
v2 = 1.;
if (cst[0] == '-')
  {
  jb = 0;
  isgn = -1;
  }
if (cst[0] == '+')  jb = 0;
if ((pv=posc(cst,'.')) != -1)
  {
  v2 = 0.1;
  for(i=pv+1;i<ls;i++)
    {
    if(isdigit(cst[i]) == 0)  return (-1);
    v1 += (cst[i]-'0')*v2;
    v2 /= 10.;
    }
  ls = pv;
  v2 = 1.;
  }
for(i=ls-1;i>jb;i--)
  {
  if(isdigit(cst[i]) == 0)  return (-1);
  v1 += (cst[i]-'0')*v2;
  v2 *= 10.;
  }
*fpt = (isgn == 1)? v1 : -v1;
return (1);
}
/*           Fonction  posc()        */
int posc(const char* cst, char sec)
{
int i,ls;
ls = strlen(cst);
for(i=0;i<ls;i++)
  {
  if (cst[i] == sec)  return(i);
  }
return (-1);
}

/*           Fonction  poslc()        */
int poslc(const char* cst, char sec)
{
int i,ls;
ls = strlen(cst);
for(i=ls-1;i>=0;i--)
  {
  if (cst[i] == sec)  return(i);
  }
return (-1);
}

/*                Fonction   strip()             */
int strip(char cst[], char opt, char bc)
{
int i,j,ls;
ls = strlen(cst);
if (opt == 'L' || opt == 'B')
  {
  i = 0;
  while(cst[i] == bc) i++;
  if (i != 0)
    {
    for(j=i;j<ls;j++)  cst[j-i] = cst[j];
    ls -= i;
    cst[ls] = '\0';
    }
  }
if ((opt == 'T' || opt == 'B') && ls > 0)
  {
  i = ls-1;
  while(cst[i] == bc)  i--;
  cst[i+1] = '\0';
  }
return (0);
}

/*                Fonction   padstr(spo,lpa,bc)                      */
/*    Cette  fonction  rajoute  des blancs a la fin de la chaine spo */
/*   pour amener la longueur de la chaine a lpa. les caracteres      */
/*   rajoutes a la fin sont bc. La fonction retourne le nombre de    */ 
/*   blancs (caracteres bc)  rajoutes a la fin.                      */

int padstr(char spo[], int lpa, int bc)
{
int li,i;
li = strlen(spo);
for(i=li;i<lpa;i++)  spo[i] = bc;
spo[lpa] = '\0';
return(lpa-li);
}

/* Eric L. 11-03-94 */
/* quelques fonctions utiles */

void tomaj(char *cbuff)
{
register int i,l;

l=strlen(cbuff);
for(i=0; i< l;i++)
  cbuff[i]=(char) toupper( (int) cbuff[i]);
}

void tomin(char *cbuff)
{
register int i,l;

l=strlen(cbuff);
for(i=0; i< l;i++)
  cbuff[i]=(char) tolower( (int) cbuff[i]);
}

void efface(char *cbuff,int lbuff,char c)
{
register int i;

for(i=0; i< lbuff;i++)
  if(cbuff[i] == c)  cbuff[i] = ' ';
}

char* rep_char(const char *cbuff,char cin,char crep)
{
register int i,l;
char* buff;
buff=(char*) cbuff;
l=strlen(buff);
for(i=0; i< l;i++)
  if(buff[i] == cin) buff[i] = crep;
return(buff);
}

/* Eric L. 23-03-96 */

int csh_parse(const char* x, const char* yp)
{
 int i,lenx,lenyp;
 int ix,iyp;
 int faux;
 char cx, cyp;
 
 lenx=strlen(x);
 lenyp=strlen(yp);
 if(!lenx || !lenyp) return(0);
 faux=0;
 ix=0;iyp=0;
 while(ix < lenx && iyp < lenyp)
  {
   cx=x[ix];
   cyp=yp[iyp];
   if(cx == cyp) 
    {
     ix++; 
     iyp++;
    }
   else
    switch(cyp)
     {
       case '?':
        ix++;
        iyp++;
        break;
       case '*':
        while((cyp=='*' || cyp=='?') && (iyp < lenyp))
         {
          iyp++;
          if(iyp < lenyp) cyp=yp[iyp];
         }
        if(iyp < lenyp)
         {
          faux=1;
          for(i = ix; i < lenx ; i++)
           {
            cx=x[i];
            if(cx == cyp)
             {
              faux=0;
              break;
             }
           }
          ix=i+1;
          if(ix < lenx) cx=x[ix];
          iyp++; 
          if(iyp < lenyp) cyp=yp[iyp];
         }    
        break;
       default:
        faux=1;
     }
    if(faux) return(0);
  }
 
 for( i= iyp; i < lenyp ; i++)
  {
   cyp=yp[i];
   if( cyp != '*' && cyp != '?') return(0);
  }
 
 if(cyp == '*') return(1);
 if((lenx-ix) == 1 && ( cyp == '?' || cx==cyp)) return(1);
 for( i= ix; i < lenx ; i++)
  {
   cx=x[i];
   if( cx != cyp)  return(0);
  }
 return(1);
}
 
/*-------------------------------------------------------------------*/

/*
++
  int DecArgList(char *listin, int *Vec, int nVec)
	Pour decoder une liste de numeros a,b,c-d,e,f-g,h...
	avec a,b... entiers
| Vec est rempli jusqu'a concurrence de nVec elts.
|     Si Vec==NULL ou nVec<=0 le tableau n'est pas rempli
|         et seul le nombre d'elements est compte
| Return: nombre d'elements rempli
--
*/
/*                            (cmv 7/8/97)      */
int DecArgList(char *listin, int *Vec, int nVec)
{
int i,k, num, min, max;
char *list = NULL, *listfin, *s;

if(strlen(listin) <= 0) return(0);
list = (char *) malloc(strlen(listin)+1);
if(list==NULL) return(0);
strcpy(list,listin); strip(list,'B',' ');
if( (k=strlen(list)) <= 0 ) {free(list); return(0);}
for(i=0; i<k; i++) if(list[i] == ',') list[i] = ' ';
listfin = list + k; *listfin = '\0';

s = list; num = 0; k = 0;
while(s < listfin) {
  if ( (k=posc(s,' ')) < 0)  k = strlen(s);
  s[k] = '\0'; strip(s,'B',' ');
  sscanf(s,"%d-%d",&min,&max);
  max = min - 1;
  sscanf(s,"%d-%d",&min,&max);
  if(max<min) max = min;
  for(i=min; i<=max; i++) {
    if( Vec!=NULL && nVec>0 ) {
      if( num >= nVec )
        {printf("DecArgList_Erreur: pas assez de place dans le vecteur (%d)\n"
               ,nVec); free(list); return(num);}
      Vec[num] = i;
    }
    num++;
  }
  s = s+k+1;
}

free(list);
return(num);
}
 
/*-------------------------------------------------------------------*/

/*
++
  char *ExtName(char *cout,char *cin,char t)
	Pour separer les composantes d'un nom de fichier de type unix.
| cout   = chaine en sortie
| cin    = chaine en entree
| t      = type de racine a extraire
| Return = *cout
| - Remarque: pas de verification de longeur de la chaine cout!
| - Exemple: /home/toto.cc  /home/ /home/.cc  .cc  toto.cc
|    t='d' : /home          /home  /home      ""   ""
|    t='n' : toto.cc        ""     .cc        .cc  toto.cc
|    t='r' : toto           ""     ""         ""   toto
|    t='t' : cc             ""     cc         cc   cc
--
*/
/*                            (cmv 24/4/98)      */
char *ExtName(char *cout,char *cin,char t)
{
int is=-1,ip=-1,i,j;
int len = strlen(cin);
/* cas ou le nom se termine par un / qui n'est pas au debut*/
if(len>1) if(cin[len-1]=='/') len--;

for(i=0;i<len;i++) {
  if(cin[i]=='/') is=i;         /* position du dernier / */
  if(cin[i]=='.' && ip<0) ip=i; /* position du 1er point */
  cout[i]=cin[i];
}
cout[len]='\0';

/* mauvais nom de fichier */
if(is>=0&&ip>=0&&ip<=is) {cout[0]='\0'; return cout;}

/* traitement des differents cas */
switch(t) {
  case 'd':
    if(is==0) {                    /* cas /... */
      if(ip<0) ip=len-1;             /* cas /home */
      else ip=0;                     /* cas /toto.c */
    } else if(is<0) {              /* cas toto.c */
      is=ip=-1;
    } else {                       /* cas /home/.../... */
      ip=is-1; is=0;
    }
    break;
  case 'n':
    if(is==0) {                        /* cas /... */
      if(ip<0) ip=is=-1;                 /* cas /home */
      else {is=1; ip=len-1;}             /* cas /toto.c */
    } else if(is>0) {                  /* cas /home/toto.c */
      is++; ip=len-1;
    } else {                           /* cas toto.c */
      is=0; ip=len-1;
    }
    break;
  case 'r':
    if(is==0) {                          /* cas /... */
      if(ip<0) ip=is=-1;                   /* cas /home */
      else if(ip==1) is=ip=-1;             /* cas /.c */
      else {is=1; ip--;}                   /* cas /toto.c */
    } else if(is>0) {                    /* cas /home/... */
      if(ip<0) {is++;ip=len-1;}            /* cas /home/toto */
      else if(ip==is+1) is=ip=-1;          /* cas /home/.c */
      else {is++; ip--;}                   /* cas /home/toto.c */
    } else if(is<0) {                    /* cas toto.c */
      if(ip<0) {is=0;ip=len-1;}            /* cas toto */
      else if(ip==0) is=ip=-1;             /* cas .c */
      else {is=0; ip--;}                   /* cas toto.c */
    }
    break;
  case 't':
    if(ip<0) is=ip=-1;             /* cas sans '.' dans le nom */
    else {is=ip+1,ip=len-1;}       /* cas blablabla.c */
    break;
  default:
    cout[0]='\0';
    return cout;
    break;
}

j=0;
if(is>=0&&ip>=is) for(i=is,j=0;i<=ip;i++,j++) cout[j]=cin[i];
cout[j]='\0';
/* printf("........ is=%d ip=%d :%s:\n",is,ip,cout); */
return cout;
}
