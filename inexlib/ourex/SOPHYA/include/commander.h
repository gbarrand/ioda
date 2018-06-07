// This may look like C code, but it is really -*- C++ -*-
// Classe interpreteur de commande pour piapp 
// Reza      Aout 97 , Juillet,Aout 98
//           Octobre 2003: de PIext -> SysTools
// LAL-IN2P3/CNRS  DAPNIA/CEA

#ifndef COMMANDER_H_SEEN
#define COMMANDER_H_SEEN

#include "machdefs.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <functional>

#include "pdlmgr.h"
#include "dvlist.h"
#include "ctimer.h"

namespace SOPHYA {

////// Classe definissant l'interface pour un interpreteur de commande

/*!
  \ingroup SysTools
  \brief  Interface definition for a generic command interpreter.
*/
class CmdInterpreter {
public:
  virtual		~CmdInterpreter() {} ;
//! Returns the interpreter's name
  virtual string	Name()=0;
//! Method to be called in order to interpret a line or string.
  virtual int		Interpret(string& line)=0;
};


/////// Classe definissant l'interface pour un executeur de commande

/*!
  \ingroup SysTools
  \brief Interface definition for command executor, to be used with Commander

  A command is defined by a keyword and a number of argument
*/

class CmdExecutor {
public:
  virtual	~CmdExecutor() {} ;
  // keyw : Le mot cle associe , args: Arguments de la commande
  /*! 
    \brief command execution method for a command defined by keyword and its arguments.

    The command is identified by the keyword \b kw , and a set of arguments \b args.
    The string \b toks represent the whole line (or string) parsed by the interpreter, 
    except the keyword itself.
  */
  virtual int	Execute(string& keyw, vector<string>& args, string& toks)=0;
  //! Return true if the command \b keyw is thread compatible (can be executed in a separate thread)
  virtual bool  IsThreadable(string const & keyw) { return false; }
};



class CommanderBloc;    // Bloc de type foreach / for de l'interpreteur Commander
class CommanderScript;  // Script de commandes defini ds l'interpreteur Commander
class CommandExeThr;    // Thread d'execution de commande

//! A simple command interpreter with c-shell like syntax and  dynamic load capability.

class Commander : public CmdInterpreter  {
public:
  static Commander*	GetInterpreter();

			Commander(bool fgsigzt=true);
  virtual		~Commander();
  virtual string	Name();

  virtual void		AddHelpGroup(string& grp, string& desc);
  virtual void		RegisterCommand(string& keyw, string& usage, CmdExecutor * ce, 
                                        string& grp);
  inline  void		RegisterCommand(string& keyw, string& usage, CmdExecutor * ce, 
                                        const char* grp)
                        { string sgrp = grp; RegisterCommand(keyw, usage, ce, sgrp); }

  virtual void		RegisterHelp(string& keyw, string& usage, string& grp);

  virtual void		LoadModule(string& fnameso, string& name);

  virtual void		AddInterpreter(CmdInterpreter * cl);
  virtual void		SelInterpreter(string& name);

  virtual int		Interpret(string& line);
  virtual void		StopExecution();

  virtual int		ExecuteCommand(string& keyw, vector<string>& args, string& toks);
  virtual int		ExecFile(string& file, vector<string>& args);
  virtual int		CShellExecute(string cmd);
  virtual string&	GetUsage(const string& kw);

  inline  void		SetMaxLoopLimit(int_8 lim=0) { maxlooplimit_ = lim; }
  inline  int_8         GetMaxLoopLimit() { return maxlooplimit_; }

  string		GetCurrentPrompt() { return curprompt; }


  virtual void 		HelptoLaTeX(string const & flnm);

  //! return the current selected interpreter (default : this)
  inline  CmdInterpreter* CurrentInterpreter() { return(curcmdi); }

  // ----- Action / gestion des variables propres de l'interpreteur
  // Verifie l'existence de la variable nomme vn et retourne sa valeur ds vv
  // Retourne false si la variable n'existe pas
  virtual bool    GetVar(string const & vn, string & vv);
  virtual bool    GetVar(string const & vn, int idx, string & vv);
  virtual bool    GetVar(string const & vn, vector<string> & vv);
  virtual bool    SetVar(string const & vn, string const & vv);
  virtual bool    SetVar(string const & vn, int idx, string const & vv);
  virtual bool    SetVar(string const & vn, vector<string> const & vv);
  virtual bool    CheckVarName(string const & vn);
  virtual bool    DeleteVar(string const & vn);
  virtual void    ListVar();
  // Variables de l'environnement application 
  virtual bool    GetVarApp(string const & vn, string & vv);
  virtual bool    SetVarApp(string const & vn, string const & vv);
  virtual bool    DeleteVarApp(string const & vn);
  virtual void    ListVarApp();
  // Variables d'environnement globales 
  virtual bool    GetVarEnv(string const & vn, string & vv);
  virtual bool    SetVarEnv(string const & vn, string const & vv);
  virtual bool    DeleteVarEnv(string const & vn);
  virtual void    ListVarEnv();

  //   Utilitaire pour decoupage en mot
  static  int   LineToWords(string& line, string& kw, vector<string>& tokens,
			    vector<bool>& qottoks, string& toks, bool uq=true);
protected:
  virtual int	ParseLineExecute(string& line, bool qw=true);

  virtual int   ExecuteCommandLine(string & keyw, vector<string> & args,
				   string & toks);

  virtual bool	CheckHelpGrp(string& grp, int& gid, string& desc);
  inline  bool	CheckHelpGrp(string& grp, int& gid)
  { string desc=""; return CheckHelpGrp(grp, gid, desc); }

  virtual int   SubstituteVars(string & s, string & s2);  
  int	        EvaluateTest(vector<string> & args, 
			     string & line, bool & res);

  // variable de l'interpreteur = valeur - accepte la syntaxe de type varname[index] 
  virtual bool    SetVariable(string const & vn, string const & vv);
  // Acces aux variables 
  virtual bool    Var2Str(string const & vn, string & vv);
  inline  bool    Var2Str(string const & vn, int idx, string & vv)
  { return GetVar(vn, idx, vv); }
  inline  bool    Var2Str(string const & vn, vector<string> & vv)
  { return GetVar(vn, vv); }

  virtual string  GetTmpDir();

  virtual void  SetCurrentPrompt(const char* pr);
  inline void   SetCurrentPrompt(string const & pr) { SetCurrentPrompt(pr.c_str()); }
  inline void   SetDefaultPrompt(string const & pr) { defprompt = pr; }

  virtual void	ShowMessage(const char * msg, int att);

  void          PushStack(vector<string> * pargs);
  inline void   PushStack() { PushStack(NULL);  return; }
  inline void   PushStack(vector<string> & args) { PushStack(&args);  return; }
  void          PopStack(bool psta=true);

  //   Gestion des threads d'execution de commandes 
  void          ListThreads();
  void          StopThr(uint_8 thrid, bool fgkill=true);
  void          CleanThrList();
  void          WaitThreads();

// ------ Attributs et variables ------ 
  CmdInterpreter* curcmdi;

// Gestion des variables
  typedef map< string,  vector<string>, less<string> > CmdVarList;
  CmdVarList variables;

// Pour enregistrer la liste de commandes et leurs executeurs et le help
/*! \cond   Pour supprimer la documentation par doxygen */
//  Command executor registration - For Commander internal use
  struct cmdex {int group; string us; CmdExecutor * cex; } ;
//  Help text registration - For Commander internal use
  struct hgrpst {int gid; string desc; } ;               // Identification+description d'un groupe de help
  /*! \endcond */

  typedef map<string, hgrpst, less<string> > CmdHGroup;   // Liste des groupes de commandes
  CmdHGroup cmdhgrp;
  int cmdgrpid;                                        // Numero de groupe courant
  typedef map<string, cmdex, less<string> > CmdExmap;  
  CmdExmap cmdexmap;                                   // Liste des commandes et leurs executeurs
  CmdExmap helpexmap;                                  // Pour les helps sans commande
// Pour garder la liste des threads d'execution de commande
  list<CommandExeThr *> CmdThrExeList;
  uint_8 ThrId;

// Pour garder la liste des modules 
  typedef map<string, PDynLinkMgr* , less<string> > Modmap;
  Modmap modmap;

// Pour garder la liste des interpreteur
  typedef map<string, CmdInterpreter*, less<string> > InterpMap;
  InterpMap interpmap;
 
//  Pour stocker les scripts definis ds l'interpreteur
  typedef map<string, CommanderScript*, less<string> > ScriptList;
  ScriptList mScripts;  // Liste des scripts
  CommanderScript* curscript; // Script en cours de definition

  // Code de retour execution commande
  int _xstatus;
  // Valeur de retour (par l'instruction return) - 
  string _retstr;

  //  Pour stocker les alias definies par l'interpreteur
  typedef map<string, string, less<string> > CmdStrList;
  CmdStrList mAliases;  // Liste des alias 

  // Le stack pour les arguments des .pic et des scripts
  stack< vector<string> > ArgsStack;
  // Stack pour les Prompts
  stack<string> PromptStack; 

  // Gestion des blocs de commandes et tests (if) 
  stack< CommanderBloc * > CmdBlks;  // Bloc de commande courant (foreach, ...)
  int felevel_;                    // foreach-for level 
  int_8 maxlooplimit_;            // Limite maximum des boucles
  /*! \cond   Pour supprimer la documentation par doxygen */
  typedef struct {list<char> tstlist; list<char>::iterator tstresit; bool tstcurres; } TstStatus;
  /*! \endcond */
  stack< TstStatus > TestsStack; // Stack des resultats de test
  list<char>::iterator tresit_;       // Test courant
  bool curtestresult_;             // Resultat courant des tests 

  // Controle du flot d'execution 
  bool fgexebrk; 

  // Commande splitees sur plusieurs lignes
  bool mulinefg;            // Bloc multi-lignes (ligne suite)
  string mulinecmd;         // Commande multi-lignes

  // Texte de prompt (attente de commande)
  string spromptmul;        // Prompt console avant multi-ligne
  string curprompt;         // Prompt courant
  string defprompt;         // Prompt par defaut 

  // Gestion d'historique, trace, timing des commandes
  ofstream hist;       //  History file 
  bool histon;        //  True ->  history file
  bool trace;          // Trace flag
  bool timing;         // Display CPU Time
  Timer* gltimer;      // pour Display CPU Time
  bool varcexp;      // true -> decodage nom de variables lors d'evaluation d'expression 
friend class CommanderBloc;  
friend class CommanderScript;  

};

} // namespace SOPHYA

/* end of ifdef COMMANDER_H_SEEN */
#endif  

