/*                                                                          */
/*   Ce fichier contient quelques fonctions de manipulation de caracteres   */
/*   et autres bricoles utiles                                              */
/*     # char* itoc() : conversion entier-chaine de caracteres              */ 
/*     # ctoi()   : convertit une chaine de caractere en entier             */
/*     # ctol()   : convertit une chaine de caractere en entier long        */
/*     # ctof()   : convertit une chaine de caractere en flottant           */
/*     # posc()    : trouve la position d'un caractere dans une chaine      */
/*     # strip()  : permet d'enlever les blancs au debut ou a la fin d'une  */
/*                  chaine de caractere                                     */ 
/*     # padstr() : permet d'ajouter des caracteres a la fin d'une chaine   */
/*                  de caracteres pour amener la longueur de cette chaine   */
/*                  a une valeur donnee.                                    */
/*     # tomaj()  : Changement en majuscule d'une chaine de caracteres      */
/*     # efface() : Remplace un caractere avec des blancs                   */
/*     # rep_char() : Remplace un caractere par un autre                    */ 
/*                                                                          */
/* Derniere modif: csh_parse, E. Lesquoy, le 23-03-96                       */
/* DecArgList: Decoder une liste du type a,b,c-d,e,f-g,h...(cmv 7/8/97)     */
/* ExtName: eclatement des nom de fichiers unix (cmv 24/4/98)               */

#ifndef STRUTIL_H_SEEN
#define STRUTIL_H_SEEN

#ifdef __cplusplus
extern "C" {
#endif

/*    Manipulation de chaines de caracteres     */
/*                    Reza - 92/93              */
char* itoc(int in, char* s,int nDigits);
int ctoi(const char* cst, int *ipt);
int ctol(const char* cst, long int *lpt);
int ctof(const char* cst, double *fpt);
/* retour ctox : 1: OK, -1 erreur */
int posc(const char* cst, char sec);
int poslc(const char* cst, char sec);
int strip(char* cst, char opt, char bc);
int padstr(char* spo, int lpa, int bc);

/* Eric L. 11-03-94 */
void tomaj(char *cbuff);
void tomin(char *cbuff);
void efface(char *cbuff,int lbuff,char c);
/* Attention rep_char(const char *cbuff ...) c'est declare const cbuff, mais ca modifie cbuff - Reza 2/09/98  */
char* rep_char(const char *cbuff,char cin,char crep);  
/* Eric L. 23-03-96 */
int csh_parse(const char* x, const char* yp);
/* cmv 7/8/97 */
int DecArgList(char *listin, int *Vec, int nVec);
char *ExtName(char *cin,char *cout,char t);

#ifdef __cplusplus
}
#endif

#endif

