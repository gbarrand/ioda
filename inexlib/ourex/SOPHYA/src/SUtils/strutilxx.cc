#include "sopnamsp.h"
#include "machdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "strutilxx.h"


/* methode */
void FillVStringFrString(string const & s,vector<string>& vs,char sep)
// Use string "s" to fill vector of strings "vs"
// considering char "sep" as a separator.
// Vector is filled from its end (no reset done).
// To write a "sep" char, use \'sep'
// Warning: separator "sep" could not be set to '\'
// Ex: sep=' ': s="aaa   bbb cc d " -> vs=(aaa,bbb,cc,d)
// Ex: sep=';': s="aaa   ;bbb; cc;d " -> vs=(aaa   ,bbb, cc,d )
// Ex: sep=';': s=";aaa\;bbb;;;ccc;ddd" -> vs=(aaa;bbb,ccc,ddd)
// Ex: sep=';': s=";aaa\;bbb;;;ccc;ddd\" -> vs=(aaa;bbb,ccc,ddd\)
{
uint_4 ls = s.size();
if(ls<=0 || sep=='\\') return;
const char* str = s.c_str();
ls = strlen(str); // str[ls-1]==sep cf ci-dessus
string dum = "";
for(uint_4 i=0; i<ls; i++) {
  if(i==0 && str[i]==sep) {
    continue;
  } else if(str[i]=='\\') {
    if(str[i+1]!=sep || i==ls-2) dum += str[i];
  } else if(str[i]!=sep) {
    dum += str[i];
  } else {  // C'est un "sep" mais est-ce vraiment un separateur?
    if(str[i-1]=='\\' && i!=ls-1) dum += str[i];
    else {  // C'est un separateur, ne delimite t-il pas d'autres separateurs?
      if(dum.size()<=0) continue;
      vs.push_back(dum);
      dum = "";
    }
  }
}
// Ajout du dernier mot eventuellement - Pas de separateur a la fin - 
if(dum.size() > 0)  vs.push_back(dum);
}

/* methode */
void SplitStringToVString(string const & s,vector<string>& vs,char separator)
// Split the string "s" into a string vector "vs"
// - Separator is "separator"
// - If the separator is not a space (' '), the space is considered
//   as a dummy character:
// - If ',' is the separator, "1, 2  ,3 ,    4  " == "1,2,3,4"
// - If ',' is the separator, "1, 2 3 ,    4  " == "1,23,4"
// - Fill "vs" at the end: NO RESET IS DONE
// - If ',' is the separator, ",,," is c
// That routine is much more rapid than FillVStringFrString although less general
//           (ex no '\' gestion is done)
{
 uint_4 ls = s.size();
 if(ls<=0) return;
 //bool sep_non_blanc = (separator==' ')? false: true;
 char *str = new char[ls+2];

 uint_4 i=0, lstr=0;
 while(i<ls) {
   // le caractere n'est ni un blanc ni un separateur
   if(s[i]!=separator && s[i]!=' ') {
     str[lstr] = s[i]; lstr++;
   }
   // Condition de remplissage du vecteur
   if(s[i]==separator || i==ls-1) {
     if(lstr>0) {str[lstr]='\0'; vs.push_back(str); lstr=0;}
   }
   i++;
 }

 delete [] str;
 return;
}
