#include "sopnamsp.h"
#include "commander.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <signal.h>

#include "strutil.h"
#include "strutilxx.h"
#include "cexpre.h"
#include "rpneval.h"
#include "srandgen.h"
#include "zthread.h"

#if defined(__APPLE__) //G.Barrand : to disconnect system().
#include <TargetConditionals.h>
#endif

namespace SOPHYA {

//--------------------------------------------------
// Pour ecrire une valeur double sous forme de string 
// sans perte de precision, et si possible de maniere 
// lisible (%g, sinon format adapte jusqu'a %.17e)  
static char cval_strbuff_[80];
////1er essai binaire: char * cval_dble2str(double v)
////1er essai binaire: {
////1er essai binaire:   char* str1 = cval_strbuff_;  
////1er essai binaire:   sprintf(str1,"%g",v);
////1er essai binaire:   char* str2 = cval_strbuff_+32;       
////1er essai binaire:   sprintf(str2,"%.17e",v);
////1er essai binaire:   double x1 = atof(str1);
////1er essai binaire:   double x2 = atof(str2);
////1er essai binaire:   double fx = fabs(v);
////1er essai binaire:   if (fx>0. && fabs(x2-x1)/fx>1e-12)  return str2;
////1er essai binaire:   else return str1;
////1er essai binaire: }
char * cval_dble2str(double v)
{
 char* strgood = cval_strbuff_;        sprintf(strgood,"%g",v);
 char* strfull = cval_strbuff_+32;     sprintf(strfull,"%.17e",v);
 char* strform = cval_strbuff_+64;
 double x1 = atof(strfull), fx = fabs(x1);
 if(fx>0.) {
   for(int i=7;i<=17;i+=2) {  // increment de 2 pour aller + vite
     double x2 = atof(strgood);
     if(fabs(x2-x1)/fx<1.e-16) break;
     sprintf(strform,"%%.%de",i);
     sprintf(strgood,strform,v);
   }
 }
 return strgood;
}
//--------------------------------------------------

// Differents code de retour specifiques 
#define CMD_RETURN_RC     99900
#define CMD_BREAK_RC      99990
#define CMD_LOOPBREAK_RC  99910
#define CMD_BREAKALL_RC   99998
#define CMD_BREAKEXE_RC   99999

// ------------------------------------------------------------
//         Bloc de commandes (Foreach, ...)    
//               Classe  CommanderBloc    
// ------------------------------------------------------------
/*!
  \internal
  \class SOPHYA::CommanderBloc
  \ingroup SysTools 
  Class for internal use by class Commander to handle loops
*/
class CommanderBloc {
public:
  enum BType { BT_None, BT_ForeachList, BT_ForeachInt, BT_ForeachFloat, 
	       BT_ForeachLineInFile };

  CommanderBloc(Commander* piac, CommanderBloc* par, string& kw, vector<string>& args); 
  ~CommanderBloc();
  inline CommanderBloc*   Parent() { return(parent); }
  inline bool   CheckOK() { return blkok; }
  inline void	AddLine(string& line)  
    { lines.push_back(line); bloclineid.push_back(lines.size()); }
  void	        AddLine(string& line, string& kw);  
  inline void	AddBloc(CommanderBloc* blk)  
    { blocs.push_back(blk); bloclineid.push_back(-blocs.size()); }

  // Execution complete du bloc (boucle)
  int	        Execute();
  // Execution pour un element de bloc 
  int	        ExecuteOnce(string& lvv);

  inline int&   TestLevel()  { return testlevel; }
  inline int&   LoopLevel()  { return looplevel; }
  inline bool   CheckBloc() 
    { return ((testlevel == 0)&&(looplevel == 0)&&(!scrdef)); }
  
protected:
  Commander* _commander;
  CommanderBloc* parent;
  bool blkok;        // true -> block OK
  BType typ;         // foreach , integer loop, float loop, test 
  string varname;
  string filename;   // forinfile bloc
  vector<string> strlist;
  vector<string> lines;
  vector<CommanderBloc *> blocs;
  vector<int> bloclineid;
  int i1,i2,di;
  float f1,f2,df;
  int testlevel;   // niveau d'imbrication des if 
  int looplevel;   // niveau d'imbrication des for/foreach 
  bool scrdef;     // true -> commande defscript ds for/foreach 
};

/* --Methode-- */
CommanderBloc::CommanderBloc(Commander* piac, CommanderBloc* par, string& kw, vector<string>& args)
{
_commander = piac;
parent = par;
blkok = false;
typ = BT_None;
i1 = 0;  i2 = -1;  di = 1;
f1 = 0.; f2 = -1.; df = 1.;
testlevel = looplevel = 0;
scrdef = false;

if ((args.size() < 2) ||  !isalpha((int)args[0][0]) )  return; 
if ((kw != "foreach") && (kw != "for") && (kw != "forinfile"))  return;
if (!piac->CheckVarName(args[0])) return;
varname = args[0];  

//if (isalpha((int)args[1][0]) ) {  This is a foreach bloc with string list
if (kw == "forinfile") {
  filename = args[1];
  typ = BT_ForeachLineInFile;
  blkok = true;
}
else if (kw == "foreach" ) { // This is a foreach bloc with string list
  if ( (args[1] == "(") && (args[args.size()-1] == ")") ) {
    // foreach varname ( w1 w2 w3 ... ) 
    for(unsigned int kk=2; kk<args.size()-1; kk++) strlist.push_back(args[kk]);
  }
  else {
    // foreach varname WordVectorName 
    if (!piac->GetVar(args[1], strlist))  return;
  }
  if (strlist.size() < 1)  return;
  typ = BT_ForeachList;
  blkok = true;
}
else { // This is an integer or float loop
  size_t l = args[1].length();
  size_t p = args[1].find(':');
  size_t pp = args[1].find('.');
  bool fl = (pp < l) ? true : false;  // Float loop or integer loop
  if (p >= l) return;  // Syntaxe error
  string a1 = args[1].substr(0, p);
  string aa = args[1].substr(p+1);
  p = aa.find(':');
  string a2, a3;
  bool hasa3 = false;
  if (p < aa.length() ) { 
    a2 = aa.substr(0,p);
    a3 = aa.substr(p+1);
    hasa3 = true;
    }
  else  a2 = aa; 
  if (fl) { 
    typ = BT_ForeachFloat;
    blkok = true;
    f1 = atof(a1.c_str());
    f2 = atof(a2.c_str());
    if (hasa3)  df = atof(a3.c_str());
    else df = 1.;
    }
  else { 
    typ = BT_ForeachInt;
    blkok = true;
    i1 = atoi(a1.c_str());
    i2 = atoi(a2.c_str());
    if (hasa3)  di = atoi(a3.c_str());
    else di = 1;
    }
  }
}

/* --Methode-- */
CommanderBloc::~CommanderBloc()
{
for(unsigned int k=0; k<blocs.size(); k++) delete blocs[k];
}

/* --Methode-- */
void CommanderBloc::AddLine(string& line, string& kw)
{
  AddLine(line);
  if (kw == "if")  testlevel++;
  else if (kw == "endif")  testlevel--; 
  // bug trouve par JEC en Mai 2011 forinfile manquant ds la ligne suivante 
  else if ((kw == "for") || (kw == "foreach") || (kw == "forinfile"))  looplevel++;
  else if (kw == "end")  looplevel--; 
  else if (kw == "defscript")  scrdef = true; 
}

/* --Methode-- */
int CommanderBloc::Execute()
{
int k=0;
char buff[32];
int rcc = 0;

int mxloop = _commander->GetMaxLoopLimit();

if (typ == BT_ForeachLineInFile) {  // foreach line in file loop
  ifstream is(filename.c_str());
  string line;
  while (!is.eof()) {
    rcc = 0;
    line = "";
    getline(is,line);
    if (is.good()) { // bug signale par JEC en mai 2011 || is.eof()) {
      rcc = ExecuteOnce(line);
      if ((rcc == CMD_BREAKEXE_RC) || (rcc == CMD_BREAKALL_RC)) return rcc;
      else if (rcc == CMD_BREAK_RC)  break;
    }
  }
  rcc=0;
}
else if (typ == BT_ForeachList) {  // foreach string loop
  for(unsigned int k=0; k<strlist.size(); k++) {
    rcc = ExecuteOnce(strlist[k]);
    if ((rcc == CMD_BREAKEXE_RC) || (rcc == CMD_BREAKALL_RC)) return rcc;
    else if (rcc == CMD_BREAK_RC)  break; 
  }
  rcc = 0;  
}
else if (typ == BT_ForeachInt)  { // Integer loop
  for(int i=i1; i<i2; i+=di) {
    k++;
    if ((mxloop>0) && (k > mxloop)) {
      cout << ">>> Maximum CommanderBloc loop limit ("<< mxloop << ") -> break " << endl;
      break;
      }
    sprintf(buff, "%d", i);
    string lvv = buff;
    rcc = ExecuteOnce(lvv);
    if ((rcc == CMD_BREAKEXE_RC) || (rcc == CMD_BREAKALL_RC)) return rcc;
    else if (rcc == CMD_BREAK_RC)  break; 
  }
  rcc = 0;
}
else if (typ == BT_ForeachFloat)  { // float loop
  for(double f=f1; f<f2; f+=df) {
    k++;
    if ((mxloop>0) && (k > mxloop)) {
      cout << ">>> Maximum CommanderBloc loop limit ("<< mxloop << ") -> break " << endl;
      break;
      }
    string lvv = cval_dble2str(f);
    rcc = ExecuteOnce(lvv);
    if ((rcc == CMD_BREAKEXE_RC) || (rcc == CMD_BREAKALL_RC)) return rcc;
    else if (rcc == CMD_BREAK_RC)  break; 
  }
  rcc = 0;
}
return(rcc); 
}

/* --Methode-- */
int CommanderBloc::ExecuteOnce(string& lvv)
{
  unsigned int kj=0;
  int kk=0;
  int rcc = 0;
  _commander->SetVar(varname, lvv);
  for(kj=0; kj<bloclineid.size(); kj++) {
    rcc = 0;
    kk = bloclineid[kj]; 
    if (kk > 0)   
      rcc = _commander->Interpret(lines[kk-1]);
    else 
      rcc = blocs[-kk-1]->Execute();
    if ((rcc == CMD_BREAKEXE_RC) || (rcc == CMD_BREAKALL_RC)) return rcc;
    if (rcc == CMD_BREAK_RC)  break;    
  }
  return rcc;
}

// ---------------------------------------------------------------
//                  Classe CommanderScript 
//   Definition et execution d'un script de Commander
//   script : Une liste de commande Commander - Lors de l'execution, 
//   les variables-argument $# $0 $1 sont definies. 
// ---------------------------------------------------------------

/*!
  \internal 
  \class SOPHYA::CommanderScript
  \ingroup SysTools 
  Class for internal use by class Commander to handle functions
  or scripts
*/

class CommanderScript {
public:
  CommanderScript(Commander* piac, string const& name, string const& comm);
  virtual ~CommanderScript();

  void	        AddLine(string& line, string& kw);  
  virtual int   Execute(vector<string>& args);

  inline string& Name() { return mName; }
  inline string& Comment() { return mComm; }
  inline int&   TestLevel()  { return testlevel; }
  inline int&   LoopLevel()  { return looplevel; }
  inline bool   CheckScript() 
    { return ((testlevel == 0)&&(looplevel == 0)&&(!scrdef)&&fgok); }

protected:
  Commander* _commander;
  string mName;
  string mComm;
  vector<string> lines;
  int testlevel;   // niveau d'imbrication des if 
  int looplevel;   // niveau d'imbrication des for/foreach 
  bool scrdef;     // true -> commande defscript ds for/foreach 
  bool fgok;       // Script name OK
  
};

/* --Methode-- */
CommanderScript::CommanderScript(Commander* piac, string const& name, 
			   string const& comm)
{
_commander = piac;
testlevel = looplevel = 0;
scrdef = false;
mName = name;
if (!isalpha(name[0]))  fgok = false;
else fgok = true;
mComm = comm;
}

/* --Methode-- */
CommanderScript::~CommanderScript()
{
}

/* --Methode-- */
void CommanderScript::AddLine(string& line, string& kw)
{
  if (kw == "if")  testlevel++;
  else if (kw == "endif")  testlevel--; 
  // bug trouve par JEC en Mai 2011 forinfile manquant ds la ligne suivante 
  else if ((kw == "for") || (kw == "foreach") || (kw == "forinfile"))  looplevel++;
  else if (kw == "end")  looplevel--; 
  else if (kw == "defscript")  scrdef = true; 
  lines.push_back(line);
}

/* --Methode-- */
int CommanderScript::Execute(vector<string>& args)
{
  int rcc=-2;
  if (!CheckScript()) return(-1);
  cout << " CommanderScript::Execute() - Executing script " << Name() << endl;
  for(unsigned int k=0; k<lines.size(); k++) {
    rcc = _commander->Interpret(lines[k]);
    if ( (rcc == CMD_BREAKEXE_RC) || (rcc == CMD_RETURN_RC) ) break; 
  }
  return(rcc);
}


// ------------------------------------------------------------
//         Classe CommandExeThr
// ------------------------------------------------------------
/*!
  \internal
  \class SOPHYA::CommandExeThr
  \ingroup SysTools 
  Class for internal use by class Commander for command execution in separate threads.
  The command execution is carried in method run() which has its own exception handling bloc.
*/
class CommandExeThr :  public ZThread  {
public:
                 CommandExeThr(uint_8 id, CmdExecutor * cmdex, string& keyw, 
			       vector<string>& args, string& toks);
  virtual void   run();
  inline  uint_8 Id() { return _id; }
  inline  bool   IfDone() { return _fgdone; }
  inline  string& Tokens() { return  _toks; }
  inline  string& Keyword() { return  _keyw; }
protected:
  uint_8 _id;
  CmdExecutor * _cmdex;
  string _keyw, _toks;
  vector<string> _args;
  bool _fgdone;
};

/* --Methode-- */
CommandExeThr::CommandExeThr(uint_8 id, CmdExecutor * cmdex, string& keyw, 
			       vector<string>& args, string& toks)
{
  _id = id;
  _cmdex = cmdex;
  _keyw = keyw;
  _args = args;
  _args.erase(_args.end()-1);
  _toks = toks;
  for(size_t k=_toks.size()-1; k>0; k--) 
    if (_toks[k] == '&') { _toks[k] = ' '; break; }
  _fgdone = false;
}

/* --Methode-- */
void CommandExeThr::run()
{
  int rc = 0;
  try {
    rc = _cmdex->Execute(_keyw, _args, _toks);
  }
  catch (PThrowable& exc) {
    cout << "CommandExeThr::run() Catched Exception Msg()= " 
	 << exc.Msg() << endl;
    rc = -77;
  }
  catch (std::exception& sex) {
    cout << "CommandExeThr::run() Catched std::exception what()= " 
	 << sex.what() << endl;
    rc = -78;
  }
  catch (...) {
    cout << "CommandExeThr::run() Catched unknown (...) exception " << endl; 
    rc = -79;
  }
  _fgdone = true;
  setRC(rc);
}

// ------------------------------------------------------------
//         Classe Cmd_CE_VarList
// ------------------------------------------------------------
/*!
  \internal
  \class SOPHYA::Cmd_CE_VarList
  \ingroup SysTools 
  This class, reserved for internal use by class Commander, enables the CExpressionEvaluator class 
  to access the interpreter variable values.
*/

class Cmd_CE_VarList : public CE_VarListInterface {
public:
  Cmd_CE_VarList(Commander& cmd) : cmd_(cmd) { } 
  virtual void Update();
  virtual double* GetVarPointer(std::string const& name);
private:
  Commander& cmd_;
  std::map<std::string, double> namevals_;
};

/* --Methode-- */
void Cmd_CE_VarList::Update()
{
  bool exist=false;
  string sval;
  std::map<std::string, double>::iterator it;
  for(it=namevals_.begin(); it!=namevals_.end(); it++) {
    exist=cmd_.GetVar((*it).first, sval);
    if (!exist) throw NotFoundExc("Cmd_CE_VarList::Update() missing variable !");
    (*it).second=atof(sval.c_str());
  }
  return;
}

/* --Methode-- */
double* Cmd_CE_VarList::GetVarPointer(std::string const& name)
{
  std::map<std::string, double>::iterator it=namevals_.find(name);
  if (it!=namevals_.end())  return &((*it).second);
  bool exist=false;
  string sval;
  exist=cmd_.GetVar(name, sval);
  if (!exist) return NULL;
  namevals_[name]=atof(sval.c_str());
  it=namevals_.find(name);
  if (it!=namevals_.end())  return &((*it).second);
  return NULL;
}


// ------------------------------------------------------------
//         Classe Commander
// ------------------------------------------------------------
typedef void (* DlModuleInitEndFunction) ();

/*!
  \class Commander
  \ingroup SysTools
  \brief Simple command interpreter

  This Simple command interpreter with c-shell like syntax 
  can be used to add scripting capabilities
  to applications. 

  Although the interpreter has many limitations compared to 
  c-shell, or Tcl , it provides some interesting possibilities:

  - Extended arithmetic operations (c-like and RPN)
  - Simple and vector variables
  - Script definition 
  - Command execution in separate threads
  - Dynamic Load 

  \sa CmdExecutor CExpressionEvaluator RPNExpressionEvaluator

  Usage example:
  \code
  #include "commander.h"
  ...
  Commander cmd;
  char* ss[3] = {"foreach f ( AA bbb CCCC ddddd )", "echo $f" , "end"};
  for(int k=0; k<3; k++) {
    string line = ss[k];
    cmd.Interpret(line);
  }
  \endcode
*/

#define _MAGENTA_ 1

static Commander* cur_commander = NULL;
/* --Methode-- */
/*! 
  \brief Constructor. Initializes variable list and copies \c history.pic to \c hisold.pic
  if \b fgsigstop == true , calls ZThread::ActivateExitOnSignal()
*/
Commander::Commander(bool fgsigzt)
{
#if TARGET_OS_IPHONE //G.Barrand :
#else
system("cp history.pic hisold.pic");
#endif  
hist.open("history.pic");
histon = true;
trace = false;   timing = false;   varcexp = true;
gltimer = NULL;
felevel_ = 0;
SetMaxLoopLimit();
mulinecmd = "";
mulinefg = false;
spromptmul = "Cmd> ";
SetCurrentPrompt(spromptmul);
SetDefaultPrompt(spromptmul);
curscript = NULL;

_xstatus = 0;
 _retstr = "";

// Controle du flot d'execution 
 fgexebrk = false;

curtestresult_ = true;
PushStack();

// Pour la numerotation et l'identification des threads
ThrId = 0;

// Numero de help-groupe courant - Le premier groupe ajoute aura un gid = 1
// gid = 0 n'existe pas : c'est le groupe de toutes les commandes
cmdgrpid = 0;

string grp = "Commander";
string gdesc = "Basic (generic) interpreter (class SOPHYA::Commander) builtin commands";
AddHelpGroup(grp, gdesc);

string kw = "Commander";
string usage;
usage = ">>> (Commander) Interpreter's keywords : \n";
usage += "  > set varname string     # To set a variable, $varname \n";
usage += "  > unset varname          # clear variable definition \n";
usage += "  > rpneval varname RPNExpression  # Reverse Polish Notation evaluation \n";
usage += "  > varname = ArithmeticExpression # C-like Expression evaluation   \n"; 
usage += "  > varname = 'String'     # Set variable vname  \n"; 
usage += "  > var2words varname wordvarname [sep]    # to break varname into words  \n"; 
usage += "  > echo string            # output string \n"; 
usage += "  > echo2file filename string # Append the string to the specified file \n"; 
usage += "  > alias name string      # define a command alias \n"; 
usage += "  > foreach varname ( string-list ) # Loop \n";  
usage += "  > for varname i1:i2[:di]      # Integer loop  \n";  
usage += "  > for varname f1:f2[:df]      # Float loop  \n";
usage += "  > forinfile varname FileName  # Loop over lines in file \n";
usage += "  > end                         # end loops \n"; 
usage += "  > if ( test ) then  # Conditional test : a == != < > <= >= b \n";
usage += "  > else          # Conditional  \n";
usage += "  > endif         # End of conditional if bloc \n"; 
usage += "  > break         # exit from the current loop blocs \n";
usage += "  > breakall      # exit all loop blocs \n";
usage += "  > return        # Stops command execution from a file \n";
usage += "  > defscript endscript # Command script definition \n";
usage += "  > listvars      # List of variable names and values \n";
usage += "  > listalias     # List of alias names and values \n";
usage += "  > listcommands  # List of all known commands \n";
usage += "  > listscripts   # List of all known scripts \n";
usage += "  > clearcript    # Clear a script definition \n";
usage += "  > thrlist       # List of command execution threads (& as the last character) \n";
usage += "  > clearthrlist  # Removes finished threads from the list  \n";
usage += "  > killthr Id    # Try to stop a given thread (ThrId=id) by sending SIGUSR1 \n";
usage += "  > cancelthr Id  # Try to cancel a given thread (ThrId=id)  \n";
usage += "  > waitthr       # Waits until all active threads have finished (join()) \n";
usage += "  > exec filename # Execute commands from file \n";
usage += "  > help <command_name>  # <command_name> usage info \n";   
usage += "  > sleep nsec  # sleep nsec seconds \n";   
usage += "  > readstdin varname      # reads a line from stdin into $varname \n"; 
usage += "  > timingon  timingoff traceon  traceoff varcexpon varcexpoff \n";
RegisterHelp(kw, usage, grp);

kw = "RPNEvaluator";
usage = " Reverse Polish Notation (HP calculator like) expression evaluation \n";
usage += "  >> Stack: \n";
usage += "     ... (4) (3) z=(2) y=(1) x=(0)=Stack.Top() \n";
usage += "  >> Examples:  \n";
usage += "  - sin(PI/6): pi 6 / sin \n";
usage += "  - 1*2*...*5: 1 2 3 4 5 product \n";
usage += "  - x=x+y: x = $x $y * \n";
usage += "  >>> Stack operations : \n";
usage += "      print x<>y pop push (duplicate x) \n";
usage += "  >>> Constants (Cst pushed to stack): \n";
usage += "      pi e \n";
usage += "  >>> Arithmetic operators (x,y) --> x@y \n";
usage += "      + - * / % ( (int)y % (int)x )\n";
usage += "  >>> F(X): x --> F(x) \n";
usage += "      chs sqrt sq log log10 exp  \n"; 
usage += "      fabs  floor ceil \n"; 
usage += "      cos sin tan acos asin atan deg2rad rad2deg \n"; 
usage += "  >>> F(X,Y): (x,y) --> F(x,y) \n";
usage += "      pow atan2 \n"; 
usage += "  >>> F(): random number generators \n";
usage += "      rand (flat 0..1) norand (normal/gaussian) \n"; 
usage += "  >>> Stack sum/product/mean/sigma/sigma^2 \n";
usage += "      sum product mean sigma sigma2 sigmean (y->sigma x->mean) \n"; 
RegisterHelp(kw, usage, grp);

kw = "autoiniranf";
usage  = "> Automatic random number generator initialisation\n";
usage += "   by AutoInitRand(int lp) \n";
usage += "   Usage: autoiniranf";
RegisterCommand(kw, usage, NULL, grp);

kw = "CExpEvaluator";
usage  = "> Evaluation of C-like expression (used in V = C-like-Expression) \n";
usage += "   >>> Arithmetic operators, parenthesis ( + - * / ) \n";
usage += "   >>> Functions : sqrt fabs floor hypot \n";
usage += "   ...  exp log log10 pow  ; sinh cosh tanh \n";
usage += "   ...  sin cos tan asin acos atan atan2  \n";
usage += "   ...  rand01()  randpm1() gaurand() \n";
usage += "   >>> Constants : Pi = M_PI E = M_E \n";
usage += "   Example: x = 5.*(2.+sin(0.3*Pi))";
RegisterCommand(kw, usage, NULL, grp);

kw = "shell execute";
usage  = "> shell  command_string  # Execute  shell command\n";
usage += "> cshell command_string  # Execute cshell command\n";
usage += "---Examples:\n";
usage += "  > shell ls\n";
usage += "  > cshell echo '$LD_LIBRARY_PATH'; map2cl -h; ls\n";
usage += "  > shell myfile.csh [arg1] [arg2] [...]\n";
usage += "    (where the first line of \"myfile.csh\" is \"#!/bin/csh\")\n";
RegisterCommand(kw, usage, NULL, grp);


AddInterpreter(this);
curcmdi = this;
if (fgsigzt) ZThread::ActivateExitOnSignal(SIGUSR1);
}

/* --Methode-- */
Commander::~Commander()
{
hist.close();
if (gltimer) { delete gltimer;  gltimer = NULL; }
Modmap::iterator it;
for(it = modmap.begin(); it != modmap.end(); it++) {
  string name = (*it).first + "_end";
  DlModuleInitEndFunction fend = (*it).second->GetFunction(name);
  if (fend) fend();
  delete (*it).second;
  }

for(ScriptList::iterator sit = mScripts.begin();
    sit != mScripts.end(); sit++) delete (*sit).second; 
 
if (cur_commander == this)  cur_commander = NULL;
}

/* --Methode-- */
Commander* Commander::GetInterpreter()
{
return(cur_commander);
}

/* --Methode-- */
//! Returns the string \c Commander as the interpreter's name.
string Commander::Name()
{
return("Commander");
}

/* --Methode-- */
//! Add the \b grp help group with description \b desc. 
void Commander::AddHelpGroup(string& grp, string& desc)
{
  int gid; 
  CheckHelpGrp(grp, gid, desc);
}

/* --Methode-- */
/*! 
  \brief Register a command executor associated with a given keyword.
  \param keyw : keyword identifying the command
  \param usage : the command help and usage information
  \param ce : CmdExecutor pointer for this a command. The same object can be registered
  multiple time for different commands.
  \param grp : The help group corresponding to this command.
*/
void Commander::RegisterCommand(string& keyw, string& usage, CmdExecutor * ce, string& grp)
{
if (!ce) { 
  RegisterHelp(keyw, usage, grp);
  return;
  }
int gid;
CheckHelpGrp(grp,gid);
cmdex cme;
cme.group = gid;
cme.us = usage;
cme.cex = ce;
cmdexmap[keyw] = cme;
}

/* --Methode-- */
/*! 
  \brief Register a help text.
  \param keyw : help keyword
  \param usage : help text
  \param grp : help group
*/
void Commander::RegisterHelp(string& keyw, string& usage, string& grp)
{
int gid;
CheckHelpGrp(grp,gid);
cmdex cme;
cme.group = gid;
cme.us = usage;
cme.cex = NULL;
helpexmap[keyw] = cme;
}

/* --Methode-- */
bool Commander::CheckHelpGrp(string& grp, int& gid, string& desc)
{
gid = 0;
CmdHGroup::iterator it = cmdhgrp.find(grp);
if (it == cmdhgrp.end()) {
  cmdgrpid++;  gid = cmdgrpid;
  hgrpst hgs;  hgs.gid = gid;  hgs.desc = desc;
  cmdhgrp[grp] = hgs;
  return true;
  }
else { 
  if (desc.length() > 0)  (*it).second.desc = desc; 
  gid = (*it).second.gid;
  return false;
}
}


/* --Methode-- */
/*!
  \brief  Dynamic loader for modules

  A module is a shared library extending the application functionalities.
  Typically, a module adds new commands to the interpreter. Once loaded,
  the module is activated (initialized) by calling a function with the 
  name \b modulename_init . This function should be declared extern C
  to avoid C++ name mangling. A cleanup function \b modulename_end
  is called by the Commander destructor.

  \param fnameso : Shared library name containing the module functions
  and classes.
  \param name : Module name. This string is used to form module 
  initializer and cleanup function name \c name_init \c name_end
*/
void Commander::LoadModule(string& fnameso, string& name)
{
PDynLinkMgr * dynlink = new PDynLinkMgr(fnameso, false);
if (dynlink == NULL) {
  cerr << "Commander/LoadModule_Error: Pb opening SO " << fnameso << endl;
  return;
  }
string fname = name + "_init";
DlModuleInitEndFunction finit = dynlink->GetFunction(fname);
if (!finit) {
  cerr << "Commander/LoadModule_Error: Pb linking " << fname << endl;
  return;
  }
cout << "Commander/LoadModule_Info: Initialisation module" << name 
     << "  " << fname << "() ..." << endl;
finit();
modmap[name] = dynlink;
return;
}

/* --Methode-- */
//! Declare a new interpreter
void Commander::AddInterpreter(CmdInterpreter * cl)
{
if (!cl) return;
interpmap[cl->Name()] = cl;}

/* --Methode-- */
//! Select an interpreter by its name as the current interpreter. 
void Commander::SelInterpreter(string& name)
{
InterpMap::iterator it = interpmap.find(name);
if (it == interpmap.end())   return;
curcmdi = (*it).second;
}



/* Fonction */
static string GetStringFrStdin(Commander* piac)
{
char buff[128];
fgets(buff, 128, stdin);
buff[127] = '\0';
return((string)buff);
}

/* --Methode-- */
/*!
  \brief Method which has to be invoked to interpret a given command line or string.
*/
int Commander::Interpret(string& s)
{
int rc = 0;
ScriptList::iterator sit;

// Si le flag d'arret d'execution a ete positionne on returne avec le code
//  de BREAKEXECUTION
if (fgexebrk) { 
  cout << " ===> Commander::Interpret() - STOP Execution (CMD_BREAKEXE_RC)" << endl; 
  fgexebrk = false; return CMD_BREAKEXE_RC; 
}

// On saute de commandes vides
size_t l;
l = s.length();
if (!mulinefg && (l < 1))  return(0);

// On enregistre les commandes 
if (histon) hist << s << endl;   

if (s[0] == '#') return(0); // si c'est un commentaire

// Logique de gestion des lignes suite
// un \ en derniere position indique la presence d'une ligne suite
size_t lnb = s.find_last_not_of(' ');
if (lnb >= l) return(0); // il n'y a que des blancs
if (s[lnb] == '\\' ) { // Lignes suite ...
  mulinecmd += s.substr(0,lnb);
  if (!mulinefg) {
    spromptmul = GetCurrentPrompt();
    SetCurrentPrompt("...? ");
    mulinefg = true;
  }
  return(0);
}

if (mulinefg) {  // Il y avait des lignes suite
  s = mulinecmd + s;
  l = s.length();
  mulinecmd = "";
  mulinefg = false;
  SetCurrentPrompt(spromptmul);
}

// Removing leading blanks
size_t p,q;

// On enleve le dernier caractere, si celui-ci est \n 
if (s[l-1] == '\n') s[l-1] = '\0';
p=s.find_first_not_of(" \t");
if (p < l) s = s.substr(p);
// >>>> Substitution d'alias (1er mot)
CmdStrList::iterator it;
p = 0;
q = s.find_first_of(" \t");
l = s.length();
string w1 =  (q < l) ? s.substr(p,q-p) : s.substr(p);
it = mAliases.find(w1);
if (it != mAliases.end())  { 
  s =  (q < l) ? ((*it).second + s.substr(q)) : (*it).second ;
  l = s.length();
  p=s.find_first_not_of(" \t");
  if (p < l) s = s.substr(p);
  p = 0;
  q = s.find_first_of(" ");
  }

// >>>> Separating keyword 
string toks,kw;
if (q < l)
  {  kw = s.substr(p,q-p);  toks = s.substr(q, l-q); }
else { kw = s.substr(p,l-p);  toks = ""; }

// les mot-cle end else endif doivent etre le seul mot de la ligne
if ( (kw == "end") || (kw == "else") || (kw == "endif") || (kw == "endscript") ) {
  size_t ltk = toks.length();
  if (toks.find_first_not_of(" \t") < ltk) {
    cerr << "Commander::Interpret()/syntax error near end else endif endscript \n"
	 << "line: " << s << endl;
    _xstatus = 91;
    return(91);
  }
}

// On verifie si on est en train de definir un script
if (curscript) {
  if (kw == "endscript") {
    if (curscript->CheckScript()) {
      sit = mScripts.find(curscript->Name());
      if (sit != mScripts.end()) {
	cout << "Commander::Interpret() replacing script " 
	     << curscript->Name() << endl;
	CommanderScript* scr = mScripts[curscript->Name()];
	mScripts.erase(sit);
	delete scr;
      }
      cout << "Commander::Interpret() Script " << curscript->Name() 
	   << " defined successfully" << endl;
      mScripts[curscript->Name()] = curscript;
      SetCurrentPrompt("Cmd> ");
      curscript = NULL;
      _xstatus = 0;
      return(0);
    }
    else {
      cout << "Commander::Interpret() Error in Script " << curscript->Name() 
	   << " definition " << endl;
      SetCurrentPrompt("Cmd> ");
      curscript = NULL;
      _xstatus = 92;
      return(92);
    }
  }
  else curscript->AddLine(s, kw);
  _xstatus = 0;
  return(0);
}
// On verifie si nous sommes dans un bloc (for , foreach)
if (CmdBlks.top() != NULL)  { // On est dans un bloc
  if ( (kw == "for") || (kw == "foreach") || (kw == "forinfile") ) felevel_++;
  else if (kw == "end") felevel_--;

  int rcbex = 0;
  if (felevel_ == 0) { // Il faut executer le bloc
    CommanderBloc* curb = CmdBlks.top();
    CmdBlks.top() = curb->Parent();
    SetCurrentPrompt("Cmd> ");
    if (!curb->CheckBloc()) {
      cerr << "Commander::Interpret()/syntax error - unbalenced if ... endif"
	   << " within for/foreach/forinfile bloc ! " << endl;
      delete curb;
      _xstatus = 93;
      return(93);  
    }
       //       cout << " *DBG* Executing bloc " << endl;
    bool ohv = histon;
    histon = false;
    if (curtestresult_) { 
      // We push also CommanderBloc and testresult on the stack
      PushStack();
      rcbex = curb->Execute();
      // And then, pop CommanderBloc and TestResult from the corresponding stacks
      PopStack(false);
    }
    SetCurrentPrompt(defprompt);
    delete curb;
    histon = ohv;
  }
  else CmdBlks.top()->AddLine(s, kw); 
  _xstatus = rcbex;
  return(rcbex);
}
else if (kw == "end") {
  cerr << "Commander::Interpret()/syntax error - end outside for/foreach/forinfile bloc \n"
       << "line: " << s << endl;
  _xstatus = 94;
  return(94);  
}

// Sommes-nous dans un bloc de test if then else 
if (TestsStack.top().tstlist.size() > 0) {  // Nous sommes ds un bloc if 
  if (kw == "else") {
    if ((*tresit_) & 2) {
      cerr << "Commander::Interpret()/syntax error - multiple else in if bloc \n"
	   << "line: " << s << endl;
      _xstatus = 95;
      return(95);
    }
    else { 
      const char * npr = ((*tresit_)&1) ? "else-F> " : "else-T> ";
      if ((*tresit_)&1)  curtestresult_ = false;
      SetCurrentPrompt(npr);
      (*tresit_) |= 2;
      _xstatus = 0;
      return(0);
    }
  }
  else if (kw == "endif") {
    list<char>::iterator dbit = tresit_;
    tresit_--;
    TestsStack.top().tstlist.erase(dbit);
    const char * npr = "Cmd> ";
    if (TestsStack.top().tstlist.size() > 1) {
      curtestresult_ = true;
      list<char>::iterator it;
      for(it=TestsStack.top().tstlist.begin(); it!=TestsStack.top().tstlist.end(); it++) {
	// Si on n'est pas ds le else et le if est faux
	if ( !((*it)&2) && !((*it)&1) ) curtestresult_ = false;
	// Si on est ds else et le if etait vrai !
	if ( ((*it)&2) && ((*it)&1) )  curtestresult_ = false;
	if (!curtestresult_)  break;
      }
      
      if (!((*tresit_)&2)) 
	npr = ((*tresit_)&1) ? "if-T> " : "if-F> ";
      else 
	npr = ((*tresit_)&1) ? "else-F> " : "else-T> ";
    }
    else curtestresult_ = true;
    SetCurrentPrompt(npr);
    _xstatus = 0;
    return(0);
  }
} 
else if ((kw == "else") || (kw == "endif")) {
  cerr << "Commander::Interpret()/syntax error - else,endif outside if bloc \n"
       << "line: " << s << endl;
  _xstatus = 91;
  return(91);  
}

bool fgcont = true;
if (TestsStack.top().tstlist.size() > 0) { // Resultat de if ou else
  list<char>::iterator it;
  for(it=TestsStack.top().tstlist.begin(); it!=TestsStack.top().tstlist.end(); it++) {
    // Si on n'est pas ds le else et le if est faux
    if ( !((*it)&2) && !((*it)&1) )  fgcont = false;
    // Si on est ds else et le if etait vrai !
    if ( ((*it)&2) && ((*it)&1) )  fgcont = false;
    if (!fgcont)  break;
  }
} 

if ((!fgcont) && (kw != "if"))  { 
  _xstatus = 0;
  return(0);
}


// Les mots cles break et return peuvent de sortir de boucles/scripts/execfile
if (kw == "break") return CMD_BREAK_RC;
if (kw == "breakall") return CMD_BREAKALL_RC;
else if (kw == "return")  { 
  _retstr = toks; 
  return CMD_RETURN_RC;
}

// Nous ne sommes donc pas dans un bloc ....  Substitution de variables
string s2;
int rcs ;

rcs = SubstituteVars(s, s2);
if (rcs) { 
  cerr << "Commander::Interpret()/syntax error in SubstituteVars() \n"
       << "line: " << s << endl;
  _xstatus = 99;
  return(99);
}
// >>>> Separating keyword and tokens 
vector<string> tokens;
vector<bool> qottoks;
/* decoupage en mots */
LineToWords(s2, kw, tokens, qottoks, toks, true);

// Si c'est un for/foreach, on cree un nouveau bloc
if ((kw == "foreach") || (kw == "for") || (kw == "forinfile") ) {
  //     cout << " *DBG* We got a foreach... " << endl;
  CommanderBloc* bloc = new CommanderBloc(this, CmdBlks.top(), kw, tokens);
  if (!bloc->CheckOK()) {
    cerr << "Commander::Interpret() for/foreach syntax Error ! " << endl;
    delete bloc;
    _xstatus = 91;
    return(91);
    }
  felevel_++;
  if (CmdBlks.top())  CmdBlks.top()->AddBloc(bloc);
  else  SetCurrentPrompt("for...> ");
  CmdBlks.top() = bloc;
  //  cout << " *DBG* New Bloc created ... " << endl;
  return(0);
  }
else if (kw == "if") {  // Un test if
  bool restst = true;
  int rct = EvaluateTest(tokens, s, restst);
  if (rct) {
    cerr << "Commander::Interpret() if syntax Error ! " << "line: " << s << endl;
    _xstatus = 91;
    return(91);
  }
  char res_tst = (restst) ? 1 : 0;
  TestsStack.top().tstlist.push_back(res_tst);
  if (TestsStack.top().tstlist.size() == 1) tresit_ = TestsStack.top().tstlist.begin();
  else tresit_++;
  const char * npr = (restst) ? "if-T> " : "if-F> ";
  SetCurrentPrompt(npr);
}
else if ((tokens.size() > 0) && (tokens[0] == "=")) {  
  // x = Expression 
  if (qottoks[1]) {  // decodage sous forme de chaine 
    SetVariable(kw, tokens[1]);    
  }
  else {
    try {
      double res = 0.;
      string sex=tokens[1];
      if (tokens.size() > 2) 
	for(unsigned int js=2; js<tokens.size(); js++)  sex += tokens[js];
      if (varcexp) {  //--- Evaluation d'expression avec decodage de noms de variables  
	Cmd_CE_VarList cevl(*this);
	CExpressionEvaluator cex(sex,&cevl);
	res = cex.Value();
      }
      else {  //--- Evaluation d'expression SANS decodage des noms de variables  
	CExpressionEvaluator cex(sex);
	res = cex.Value();
      }
      string vv =  cval_dble2str(res);
      SetVariable(kw, vv);
    }
    catch (CExprException& cexerr) {
      cerr << "Commander::Interpret() evaluation Error : \n " << "line: " << s 
	   << " \n Msg=" << cexerr.Msg()  << endl;
      _xstatus = 98;
      return(98);
    }
  }
}
else if (kw == "defscript") {  // definition de script
  if (tokens.size() > 0) {
    if (tokens.size() < 2)  tokens.push_back("");
    curscript = new CommanderScript(this, tokens[0], tokens[1]);
    SetCurrentPrompt("Script...> ");
    return(0);
  }
  else {
    cerr << "Commander::Interpret() No script name in defscript" << "line: " << s << endl;
    _xstatus = 91;
    return(91);
  }
}
else {
  //  Si c'est le nom d'un script 
  sit = mScripts.find(kw);
  if (sit != mScripts.end()) {
    bool ohv = histon;
    histon = false; 
    tokens.insert(tokens.begin(), kw);
    PushStack(tokens);
    (*sit).second->Execute(tokens);
    PopStack(true);
    histon = ohv;
  }
  //  Execution de commandes 
  else rc = ExecuteCommandLine(kw, tokens, toks);
  _xstatus = rc;
  return(rc);
}  
// cout << "Commander::Do() DBG  KeyW= " << kw << " NbArgs= " << tokens.size() << endl;
//  for(int ii=0; ii<tokens.size(); ii++)
//  cout << "arg[ " << ii << " ] : " << tokens[ii] << endl;

return(0);
}

//! Can be called asynchronously (from a separate thread) to break (halt) execution (in loops, scripts ...) 
void  Commander::StopExecution()
{
  fgexebrk = true;
}


/* --Methode-- */
int Commander::LineToWords(string& line, string& kw, vector<string>& tokens, 
			   vector<bool>& qottoks, string& toks, bool uq)
{
if (line.length() < 1)  return(0);
int nw = 1;
size_t p = line.find_first_not_of(" ");
line = line.substr(p);
p = 0;
size_t q = line.find_first_of(" ");
size_t l = line.length();

if (q < l)
  {  kw = line.substr(p,q-p);  toks = line.substr(q, l-q); }
else { kw = line.substr(p,l-p);  toks = ""; }

q = 0;
while (q < l)  {
  bool swq = false;  // true -> chaine delimite par ' ou " 
  p = toks.find_first_not_of(" \t",q+1); // au debut d'un token
  if (p>=l) break;
  if ( uq && ((toks[p] == '\'') || (toks[p] == '"')) ) {
    q = toks.find(toks[p],p+1);
    if (q>=l)  { 
      cerr << "Commander::LineToWords/Syntax Error - Unbalenced quotes " << toks[p] << '.' << endl;
      return(-1);
    }
    p++;  swq = true;  
  }
  else {
    q = toks.find_first_of(" \t",p); // la fin du token;
  }
  string token = toks.substr(p,q-p);
  tokens.push_back(token); 
  qottoks.push_back(swq);
  nw++;
  }

return(nw);
}

/* --Methode-- */
int Commander::SubstituteVars(string & s, string & s2)
//  Variable substitution  
{

int iarr = -1;  // index d'element de tableau
size_t p,q,q2,q3,l;
bool fgvarapp = false;  //  Si true, VarApp en priorite 

s2="";
p = 0;
l = s.length();
string vn, vv;
while (p < l) {
  iarr = -1;
  fgvarapp = false; 
  q = s.find('$',p);
  if (q > l) break;
  q2 = s.find('\'',p);
  if ((q2 < l) && (q2 < q)) {  // On saute la chaine delimitee par ' '
    q2 = s.find('\'',q2+1);
    if (q2 >= l) {
      cerr << " Syntax error - Unbalenced  quotes !!! " << endl;
      return(1);
      }
    s2 += s.substr(p, q2-p+1);
    p = q2+1;  continue;  
  }
  //  cout << "DBG: " << s2 << " p= " << p << " q= " << q << " L= " << l << endl;
  if ((q>0) && (s[q-1] == '\\')) {   // Escape character \$
     s2 += (s.substr(p,q-1-p) + '$') ; p = q+1;
     continue;
     }
  if (q >= l-1) {
      cerr << " Syntax error - line ending with $ !!! " << endl;
      return(2);
      }
  vn = "";
  if ( s[q+1] == '{' ) {  // Variable in the form ${name}
    q2 = s.find('}',q+1);
    if (q2 >= l) {
      cerr << " Syntax error -  Unbalenced  brace {} !!! " << endl;
      return(3);
      }
    vn = s.substr(q+2,q2-q-2);
    q2++;
    fgvarapp = true;
  }
  else if ( s[q+1] == '(' ) {  // Variable in the form $(name)
    q2 = s.find(')',q+1);
    if (q2 >= l) {
      cerr << " Syntax error -  Unbalenced  parenthesis () !!! " << endl;
      return(3);
      }
    vn = s.substr(q+2,q2-q-2);
    q2++;
  }
  else if ( s[q+1] == '[' ) {  // Variable in the form $[varname]  -> This is $$varname
    q2 = s.find(']',q+1);
    if (q2 >= l) {
      cerr << " Syntax error - Unbalenced  brace [] !!! " << endl;
      return(4);
      }
    vn = s.substr(q+2,q2-q-2);
    if (!Var2Str(vn, vv)) return(5);
    vn = vv;
    q2++;
    }
  else { 
    if (s[q+1] == '#' ) q3 = q+2; // Variable in the form $#varname  
    else q3 = q+1;
    q2 = s.find_first_of(" .:+-*/,[](){}&|!$\"'<>^%=#@\\",q3); 
    if (q2 > l) q2 = l;
    q3 = q2;
    vn = s.substr(q+1, q2-q-1); 
    // Si variable de type $varname[index] : element de tableau
    if ((q2 < l) && (s[q2] == '[') ) {
      q3 = s.find_first_of("]",q2+1);
      string sia = s.substr(q2+1, q3-q2-1);
      if (sia.length() < 1) {
	cerr << " Syntax error - in $varname[index] : $"  
	     << vn << "[" << sia <<"]" << endl;
	return(4);
      }
      if (isalpha(sia[0])) {
	string sia2;
	if (!Var2Str(sia, sia2) || (sia2.length() < 1)) {
	  cerr << " Syntax error - in $varname[index] : $"  
	       << vn << "[" << sia <<"]" << endl;
	return(4);	  
	}
	sia = sia2;
      }
      int rcdia = ctoi(sia.c_str(), &iarr);
      if (rcdia < 0) {
	cerr << " Syntax error - in $varname[iarr] : $" 
	     << vn << "[" << sia <<"]" << endl;
	return(4);
      }
    }
  }
  if (fgvarapp) {
    if (!GetVarApp(vn, vv))
      if (!Var2Str(vn, vv)) return(5);
    s2 += (s.substr(p, q-p) + vv);
    p = q2;    
  }
  else if (iarr < 0) {
    if (!Var2Str(vn, vv)) return(5);
    s2 += (s.substr(p, q-p) + vv);
    p = q2;
  }
  else {
    if (! Var2Str(vn, iarr, vv) ) { 
      cerr << " Substitution error - word index out of range in "
	   << "$varname[iarr] : $" << vn << "[" << iarr <<"]" << endl;
      return(4);
    }
    else s2 += (s.substr(p, q-p) + vv);
    p = q3+1;
  }
} 
if (p < l) s2 += s.substr(p);

p = s2.find_first_not_of(" \t");
if (p < l) s2 = s2.substr(p);

return(0);
}

/* --Methode-- */
bool Commander::Var2Str(string const & vn, string & vv)
{
if (vn.length() < 1) {
  cerr << " Commander::Var2Str/Error: length(varname=" << vn << ")<1 !" << endl;
  vv = "";    return(false);
}
// Variable de type $# $0 $1 ... (argument de .pic ou de script)
int ka = 0;
char buff[32];
  
if (vn == "#") {
  if (ArgsStack.empty()) {
    cerr << " Commander::Var2Str/Error: ArgsStack empty ! " 
	 << " ($" << vn << ")" << endl;
    vv = "";  return(false);
  }
  char buff[32];
  long an = ArgsStack.top().size();
  if (an > 0) an--;  // Pour se conformer a l'usage de csh : Nb args sans le $0 
  sprintf(buff,"%ld", an);
  vv = buff;  return(true);
}
else if (vn == "*") {
  if (ArgsStack.empty()) {
    cerr << " Commander::Var2Str/Error: ArgsStack empty ! " 
	 << " ($" << vn << ")" << endl;
    vv = "";  return(false);
  }
  vv = ArgsStack.top()[0];
  for(unsigned int ssk=1; ssk<ArgsStack.top().size(); ssk++)  vv += ArgsStack.top()[ssk];
  return(true);
}
else if (ctoi(vn.c_str(), &ka) > 0) {  // $0 $1 $2 ...
  if (ArgsStack.empty()) {
    cerr << " Commander::Var2Str/Error: ArgsStack empty ! " 
	 << " ($" << vn << ")" << endl;
    vv = ""; return(false);
  }
  if ( (ka < 0) || (ka >= (int)ArgsStack.top().size()) ) {
    cerr << " Commander::Var2Str/Error: ArgsStack index <0 or >=args.size()  ! " 
	 << " ($" << vn << ")" << endl;
    vv = ""; return(false);
  }
  vv = ArgsStack.top()[ka];  return(true);
}
else if (vn[0] == '#') {  // Variable de type $#vname --> size(vname) 
  CmdVarList::iterator it = variables.find(vn.substr(1));
  if (it == variables.end()) {
    cerr << " Commander::Var2Str/Error #vname Undefined variable " 
	 << vn << " ! " << endl;
    vv = "";  return(false);
  }
  sprintf(buff,"%d", (int)(*it).second.size());
  vv = buff; return(true);
} 
else if (vn == "status") {
  sprintf(buff,"%d", _xstatus);
  vv = buff;
  return true;
}
else if ((vn == "retstr") || (vn == "retval")) {
  vv = _retstr;
  return true;
}
else {  // Variable de l'interpreteur, ou de l'environnement application , env. global
  if (GetVar(vn, vv))  return true;
  else if (GetVarApp(vn, vv))  return true;
  else if (GetVarEnv(vn, vv))  return true;
  else { 
    cerr << " Commander::Var2Str/Error  Undefined variable " 
	 << vn << " ! " << endl;
    vv = "";  return false;
  }
}

return false;
}

/* --Methode-- */
bool Commander::SetVariable(string const & vn, string const & vv)
{
  // On verifie si le nom est de type vname[idx]
  size_t p,q,l;
  l = vn.length();
  p = vn.find('[');
  if (p < l) {
    q = vn.find(']');
    if (q != (l-1)) {
      cout << "Commander::Str2Var/SetVar() - Bad varname with []: " 
	   << vn << endl;
      return false;
    }
    string vna = vn.substr(0, p);
    string sia = vn.substr(p+1, q-(p+1));
    if (isalpha(sia[0])) {
      string sia2;
      if (!Var2Str(sia, sia2) || (sia2.length() < 1)) {
	cerr << "Commander::Str2Var/SetVar() Syntax error- varname[index]:"  
	     << vn << endl;
	return false;	  
      }
      sia = sia2;
    }
    int iarr;
    int rcdia = ctoi(sia.c_str(), &iarr);
    if (rcdia < 0) {
      cerr << "Commander::Str2Var/SetVar() Syntax error- varname[iarr]: " 
	   << vn << endl;
      return false;
    }
    return SetVar(vna, iarr, vv);
  }
  else { 
    if (vn == "status") { 
      _xstatus = atoi(vv.c_str());
      return true;
    }
    else if (vn == "retstr") { 
      _retstr = vv;
      return true;
    }
    else return SetVar(vn, vv);
  }
}

/* --Methode-- */
bool Commander::GetVar(string const & vn, string & vv)
{
  CmdVarList::iterator it = variables.find(vn);
  if (it == variables.end()) {
    vv = "";
    return false;
  }
  vv = (*it).second[0]; 
  if ((*it).second.size() > 1) {   
    for(unsigned int k=1; k<(*it).second.size(); k++) {
      vv += ' ';  vv += (*it).second[k];
    }
  }
  return true;
}

/* --Methode-- */
bool Commander::GetVar(string const & vn, int idx, string & vv)
{
  vv = "";
  CmdVarList::iterator it = variables.find(vn);
  if (it == variables.end())  return false;
  if ((idx < 0) || (idx > (int)(*it).second.size()-1))
    return false;
  vv = (*it).second[idx];
  return true;
}

/* --Methode-- */
bool Commander::GetVar(string const & vn, vector<string> & vv)
{
  vv.clear();
  //  vv.erase(vv.begin(),vv.end());
  CmdVarList::iterator it = variables.find(vn);
  if (it == variables.end())  return false;
  vv = (*it).second;
  return true;
}

/* --Methode-- */
bool Commander::SetVar(string const & vn, string const & val)
{
  if ( !CheckVarName(vn) )  {
    cerr << "Commander::SetVar( " << vn << " ...) Bad VarName " << endl;
    return(false);
  }
  bool fg = false;
  vector<string> nouv;
  nouv.push_back(val);
  CmdVarList::iterator it = variables.find(vn);
  if (it == variables.end())  variables[vn] = nouv;
  else { 
    (*it).second = nouv;
    fg = true;
  }
  return fg;
}

/* --Methode-- */
bool Commander::SetVar(string const & vn, int idx, string const & val)
{
  if ( !CheckVarName(vn) )  {
    cerr << "Commander::SetVar( " << vn << " ,idx, ...) Bad VarName " << endl;
    return(false);
  }
  if ((vn == "status") || (vn == "retstr")) {
    cerr << "Commander::SetVar(vn,idx,val) ERROR - special var status/retstr " 
	 << endl;
    return(false);
  }
  if (idx < 0) {
    cout << "Commander::SetVar(vn," << idx << ",...) Error idx < 0" << endl;
    return(false);
  }
  bool fg = false;
  CmdVarList::iterator it = variables.find(vn);
  if (it == variables.end()) {
    vector<string> nouv;
    for(int j=0; j<idx; j++)  nouv.push_back("");
    nouv.push_back(val);
    variables[vn] = nouv;
  }
  else { 
    if (idx >= (int)(*it).second.size())
      for(int j=(*it).second.size(); j<=idx; j++)  (*it).second.push_back("");
    (*it).second[idx] = val;
    fg = true;
  }
  return fg;
}

/* --Methode-- */
bool Commander::SetVar(string const & vn, vector<string> const & val)
{
  if ( !CheckVarName(vn) )  {
    cerr << "Commander::SetVar( " << vn << " ...) Bad VarName " << endl;
    return(false);
  }
  if ((vn == "status") || (vn == "retstr")) {
    cerr << "Commander::SetVar(vn, vector<string>) ERROR - special var status/retstr " 
	 << endl;
    return(false);
  }
  bool fg = false;
  CmdVarList::iterator it = variables.find(vn);
  if (it == variables.end())  variables[vn] = val;
  else { 
    (*it).second = val;
    fg = true;
  }
  return fg;
}

/* --Methode-- */
bool Commander::CheckVarName(string const & vn)
{
  size_t l,k;
  l = vn.length();
  if (l < 1)  return false;
  if (!isalpha(vn[0]))  return false;
  for(k=1; k<l; k++) 
    if ((!isalnum(vn[k])) && (vn[k] != '_'))  return false;
  return true;  
}

/* --Methode-- */
bool Commander::DeleteVar(string const & vn)
{
  CmdVarList::iterator it = variables.find(vn);
  if (it == variables.end())  { 
    cerr << "Commander::DeleteVar() Var " << vn << " undefined!" << endl;
    return false;
  }
  variables.erase(it);
  return true;
}

/* --Methode-- */
void Commander::ListVar()
{
  cout << " ---- Commander::ListVar() List of defined variables ---- " 
       << endl;
  CmdVarList::iterator it;
  for(it = variables.begin(); it != variables.end(); it++) {
    string vn = (*it).first;  
    int vs = (*it).second.size();
    cout << vn << " -> Size= " << vs  << endl;
  }
  cout << "---------------------------------------------------------- " 
       << endl;
}

/* --Methode-- */
bool Commander::GetVarApp(string const & vn, string & vv)
{
  vv = "";
  //  cout << " Commander::GetVarApp() Not available ! " << endl;
  return false;
}

/* --Methode-- */
bool Commander::SetVarApp(string const & vn, string const & vv)
{
  //  cout << " Commander::SetVarApp() Not available ! " << endl;
  return false;  
}

/* --Methode-- */
bool Commander::DeleteVarApp(string const & vn)
{
  //  cout << " Commander::DeleteVarApp() Not available ! " << endl;
  return false;  
}

/* --Methode-- */
void Commander::ListVarApp()
{
  //  cout << " Commander::ListVarApp() Not available ! " << endl;
  return;  
}


/* --Methode-- */
bool Commander::GetVarEnv(string const & vn, string & vv)
{
  char* vev = getenv(vn.c_str());
  if (vev) {
    vv = vev;
    return true;
  }
  else { 
    vv = "";
    return false;
  }
}

/* --Methode-- */
bool Commander::SetVarEnv(string const & vn, string const & vv)
{
  string pev = vn;
  pev += '=';
  pev += vv;
// if defined(Linux) || defined(AIX)
// Reza - 28/04/2004 
// putenv de Linux ne declare pas la variable char *string const
// On ne doit meme pas utiliser une variable automatique  
// J'alloue donc un nouveau tableau - mais qui va le liberer ?
// Idem AIX , Reza Dec 2005 
// Pb apparu avec g++ 4 sur darwin (Mac) - Jan 2006
// Je fais copie pour tout le monde
  char* bev = new char[pev.size()+1];
  strcpy(bev, pev.c_str());
  if (putenv(bev) == 0)  return true;  
// else
//  if (putenv(pev.c_str()) == 0)  return true;
// endif
  else return false;  
}

/* --Methode-- */
bool Commander::DeleteVarEnv(string const & vn)
{
  //  cout << " Commander::DeleteVarEnv() Not available ! " << endl;
  return false;  
}

/* --Methode-- */
void Commander::ListVarEnv()
{
  cout << " Commander::ListVarEnv() Not available ! " << endl;
  return;  
}


/* --Methode-- */
string Commander::GetTmpDir()
{
  return("/tmp");
}

/* --Methode-- */
void Commander::SetCurrentPrompt(const char* pr)
{
  curprompt = pr;
}

/* --Methode-- */
void Commander::ShowMessage(const char * msg, int att)
{
  cout << msg ;
}



/* --Methode-- */
int Commander::EvaluateTest(vector<string> & args, string & line, bool & res)
{
  res = true;
  if ((args.size() != 6) || (args[5] != "then") || 
      (args[0] != "(") || (args[4] != ")") ) return(1);
  if (args[2] == "==") res = (args[1] == args[3]);
  else if (args[2] == "!=") res = (args[1] != args[3]);
  else if (args[2] == "<") 
    res = (atof(args[1].c_str()) < atof(args[3].c_str()));
  else if (args[2] == ">") 
    res = (atof(args[1].c_str()) > atof(args[3].c_str()));
  else if (args[2] == "<=") 
    res = (atof(args[1].c_str()) <= atof(args[3].c_str()));
  else if (args[2] == ">=") 
    res = (atof(args[1].c_str()) >= atof(args[3].c_str()));
  else return(2);
  return(0);
}

  

/* --Methode-- */
void Commander::PushStack(vector<string>* pargs)
{
  // We push the argument list (args) on the stack if not NULL
  if (pargs != NULL) ArgsStack.push(*pargs);
  // We push also CommanderBloc and testresult on the stack
  CmdBlks.push(NULL);
  if (TestsStack.size()>0) {
    TestsStack.top().tstresit=tresit_;
    TestsStack.top().tstcurres=curtestresult_;
  }
  TstStatus tstat;  
  tstat.tstcurres=curtestresult_;
  TestsStack.push(tstat);
}

/* --Methode-- */
void Commander::PopStack(bool psta)
{
  // We remove the argument list (args) from the stack
  if (psta) ArgsStack.pop();
  // And CommanderBloc and TestResult from the corresponding stacks
  CommanderBloc* curb = CmdBlks.top();
  while (curb != NULL) {
    CommanderBloc* parb = curb->Parent();
    delete curb;  curb = parb;
  }
  CmdBlks.pop();
  TestsStack.pop();
  tresit_=TestsStack.top().tstresit;
  curtestresult_=TestsStack.top().tstcurres;
}

/* --Methode-- */
int Commander::ExecuteCommandLine(string & kw, vector<string> & tokens, string & toks)
{
int rc = 0;

// >>>>>>>>>>> Commande d'interpreteur
if (kw == "help") {
  if (tokens.size() > 0) cout << GetUsage(tokens[0]) << endl;
  else {  
    string kwh = "Commander";
    cout << GetUsage(kwh) << endl;
    }
  }
else if (kw == "sleep") {
  if (tokens.size() < 1) { 
    cout << "Commander::Interpret() Usage: sleep nsec " << endl;  
    return(1); 
  }
  int nsec = atoi(tokens[0].c_str());
  cout << "Commander::Interpret() sleep " << nsec << " seconds" << endl;
  sleep(nsec);
}

else if (kw == "set") {
  if (tokens.size() < 2) { 
    cout << "Commander::Interpret() Usage: set varname value or set vecvar ( w1 w2 ... ) " << endl;  
    return(1); 
  }
  
  if (tokens.size() == 2) 
    SetVariable(tokens[0], tokens[1]);
  else {
    if ( (tokens[1] != "(") || (tokens[tokens.size()-1] != ")") ) {
    cout << "Commander::Interpret() Usage: set vecvar ( w1 w2 ... ) " << endl;  
    return(1); 
    }
    string vname = tokens[0];
    vector<string>::iterator vit;
    vit = tokens.begin(); tokens.erase(vit);
    vit = tokens.begin(); tokens.erase(vit);
    tokens.pop_back();
    SetVar(vname, tokens);
  }
  return 0;
}
else if (kw == "var2words") {
  if (tokens.size() < 2) { 
    cout << "Commander::Interpret() Usage: var2words varname wordvarname [sep]" << endl;  
    return(1); 
  }
  char sep = ' ';
  if (tokens.size() > 2) sep = tokens[2][0];
  string vv;
  if (!GetVar(tokens[0], vv)) {
    cout << "Commander::Interpret() var2words/Error No variable with name " << tokens[0] << endl;  
    return 2;
  }
  vector<string> vs;
  FillVStringFrString(vv, vs, sep);
  SetVar(tokens[1], vs); 
}
else if (kw == "alias") {
  if (tokens.size() < 2) { cout << "Commander::Interpret() Usage: alias aliasname string" << endl;  return(0); }
  if ((tokens[0].length() < 1) || !isalpha((int)tokens[0][0]) ) {
    cerr << "Commander::Interpret()/Error alias name should start with alphabetic" << endl;
    return(1);
  }
  string xx = tokens[1];
  for (unsigned int kk=2; kk<tokens.size(); kk++)  xx += (' ' + tokens[kk]);
  mAliases[tokens[0]] = xx;
}

else if ( (kw == "unset") || (kw == "clearvar") ) {
  if (tokens.size() < 1) { 
    cout << "Commander::Interpret() Usage: unset/clearvar varname" << endl;  
    return(1); 
  }
  else DeleteVar(tokens[0]);
}
// Evaluation d'expression en notation polonaise inverse
else if (kw == "rpneval") {
  try {
    RPNExpressionEvaluator rpn(tokens, 1);
    double res = rpn.Value();
    string vv = cval_dble2str(res);
    SetVariable(tokens[0],vv);
    return 0;
  }
  catch (RPNExprException& rpnerr) {
    cerr << " rpneval: Syntax error - Msg=" << rpnerr.Msg() 
	 << " \n Line=" << toks << endl;
    return 98;
  }
}
else if (kw == "echo") {
  for (unsigned int ii=0; ii<tokens.size(); ii++)
    cout << tokens[ii] << " " ;
  cout << endl;
 }
else if (kw == "echo2file") {
  if (tokens.size() < 1) {
    cout << "Commander::Interpret() Usage: echo2file filename [string ] " << endl;  
    return(1);
  }
  ofstream ofs(tokens[0].c_str(), ios::app);
  for (unsigned int ii=1; ii<tokens.size(); ii++)
    ofs << tokens[ii] << " " ;
  ofs << endl;
 }
else if (kw == "readstdin") {
  if (tokens.size() < 1) { cout << "Commander::Interpret() Usage: readstdin varname" << endl;  return(0); }
  if ((tokens[0].length() < 1) || !isalpha((int)tokens[0][0]) ) {
    cerr << "Commander::Interpret()/Error Variable name should start with alphabetic" << endl;
    return(0);
    }
  ShowMessage(">>> Reading From StdIn \n", _MAGENTA_);
  cout << tokens[0] << " ? " << endl;
  SetVar(tokens[0], GetStringFrStdin(this) );
  }

else if (kw == "listvars" || kw == "listvar") ListVar();
else if (kw == "listalias") {
  cout << "Commander::Interpret()  Alias List , AliasName = Value \n";
  CmdStrList::iterator it;
  for(it = mAliases.begin(); it != mAliases.end(); it++)  
    cout << (*it).first << " = " <<  (*it).second << "\n";
  cout << endl;
  }
else if (kw == "listcommands") {
  cout << "---- Commander::Interpret() Command List ----- \n";
  CmdExmap::iterator it;
  int kc = 0;
  for(it = cmdexmap.begin(); it != cmdexmap.end(); it++) {
    cout << (*it).first << "  ";
    kc++;
    if (kc >= 5) { cout << "\n"; kc = 0; }
    }
  cout << endl;
  }
else if (kw == "listscripts") {
  cout << "---- Commander::Interpret() Script List ----- \n";
  for(ScriptList::iterator sit = mScripts.begin(); 
      sit != mScripts.end(); sit++)
    cout << " Script: " << (*sit).second->Name() << " - " 
	 << (*sit).second->Comment() << endl;
}
else if (kw == "clearscript") {
  if (tokens.size() < 1) { 
    cout << "Commander::Interpret() Usage: clearscript scriptname" << endl;  
    return(0); 
  }
  ScriptList::iterator sit = mScripts.find(tokens[0]);
  if (sit == mScripts.end()) {
    cout << "Commander::Interpret() No script with name" << tokens[0] << endl;
    return(0);
  }
  else {
    delete (*sit).second;
    mScripts.erase(sit);
    cout << "Commander::Interpret() script " << tokens[0] << " cleared" << endl;
    return(0);
  }
}
//---------------------------------------------
//---   Commandes de gestion des threads ------
//---------------------------------------------
else if (kw == "thrlist") {
  ListThreads();
  return(0);
}
else if ( (kw == "killthr") || (kw == "cancelthr") )  {
  if (tokens.size() < 1) { 
    cout << "Commander::Interpret() Usage: killthr/cancelthr thrid" << endl;  
    return(0); 
  }
  uint_8  id = atol(tokens[0].c_str());
  bool fgkill = false;
  if (kw == "killthr")  fgkill = true;
  StopThr(id, fgkill);
  return (0);
}
else if (kw == "waitthr") {
  WaitThreads();
  return (0);
}
else if (kw == "cleanthrlist") {
  CleanThrList();
  return (0);
}  

else if (kw == "traceon")  { cout << "Commander::Interpret()  -> Trace ON mode " << endl; trace = true; }
else if (kw == "traceoff") { cout << "Commander::Interpret()  -> Trace OFF mode " << endl; trace = false; }
else if (kw == "timingon") { 
  cout << "Commander::Interpret()  -> Timing ON mode " << endl; 
  if (gltimer)   delete gltimer;   gltimer = new Timer("PIA-CmdInterpreter ");   timing = true; 
  }
else if (kw == "timingoff") { 
  cout << "Commander::Interpret()  -> Timing OFF mode " << endl; 
  if (gltimer)  delete gltimer;  gltimer = NULL;  timing = false; 
  }
else if (kw == "varcexpon")  
  { cout << "Commander::Interpret()  -> Activating variable name decoding in expression evaluation " << endl; varcexp = true; }
else if (kw == "varcexpoff") 
  { cout << "Commander::Interpret()  -> Deactivating variable name decoding in expression evaluation " << endl; varcexp = false; }
else if (kw == "exec") {
  if (tokens.size() < 1) { cout << "Commander::Interpret() Usage: exec filename" << endl;  return(0); }
  ExecFile(tokens[0], tokens);
  }
else if (kw == "autoiniranf") { 
  AutoInitRand(1);
  return(0);
}
else if (kw == "shell") {
  if (tokens.size() < 1) { cout << "Commander::Interpret() Usage: shell cmdline" << endl;  return(0); }
  string cmd;
  for (unsigned int ii=0; ii<tokens.size(); ii++)
    cmd += (tokens[ii] + ' ');
#if TARGET_OS_IPHONE //G.Barrand :
  cout << "Commander::Interpret() : shell : iOS : system() not available." << endl;
#else
  system(cmd.c_str());
#endif  
  }
else if (kw == "cshell") {
  if(tokens.size()<1) {cout<<"Commander::Interpret() Usage: cshell cmdline"<<endl; return(0);}
  string cmd="";
  for(unsigned int ii=0;ii<tokens.size();ii++) cmd+=(tokens[ii]+' ');
  CShellExecute(cmd);
  }

//  Execution d'une commande enregistree
else rc = ExecuteCommand(kw, tokens, toks);

if (timing)  gltimer->Split();
return(rc);
}

/* --Methode-- */
int Commander::ParseLineExecute(string& line, bool qw)
  // Si qw == true, on decoupe entre '' ou "" ou espaces
{
vector<string> tokens;
vector<bool> qottoks;
string kw, toks;
if (line.length() < 1)  return(0);
LineToWords(line, kw, tokens, qottoks, toks, qw);
return(ExecuteCommand(kw, tokens, toks));  
}

/* --Methode-- */
int Commander::ExecuteCommand(string& keyw, vector<string>& args, string& toks)
{
  int rc = -1;
  CmdExmap::iterator it = cmdexmap.find(keyw);
  if (it == cmdexmap.end())  cout << "No such command : " << keyw << " ! " << endl;
  else { 
    if ((*it).second.cex) {
      // Doit-on l'executer sous forme de thread separe ? 
      if ( (args.size()>0) && (args[args.size()-1] == "&") ) { 
	if ((*it).second.cex->IsThreadable(keyw) ) {
	  ThrId++;
	  CommandExeThr * thr = 
	    new CommandExeThr(ThrId, (*it).second.cex, keyw, args, toks);
	  CmdThrExeList.push_back(thr);
	  cout << " Commander::ExecuteCommand() : Thread execution of command " << keyw << endl;
	  thr->start();
	  if (CmdThrExeList.size() > 5)  CleanThrList();
	  rc = 0;
	}
	else {
	  args.erase(args.end()-1);
	  for(size_t k=toks.size()-1; k>0; k--) 
	    if (toks[k] == '&') { toks[k] = ' '; break; }
	  cout << " Commander::ExecuteCommand() : Thread execution NOT available for" << keyw << endl;
	  rc = (*it).second.cex->Execute(keyw, args, toks);
	}
      }
      else  rc = (*it).second.cex->Execute(keyw, args, toks);
    }
    else cout << "Dont know how to execute " << keyw << " ? " << endl;
  }
  return(rc);
}

/* --Methode-- */
int Commander::ExecFile(string& file, vector<string>& args)
{
char line_buff[1024];
FILE *fip;
int rcc = 0;
if ( (fip = fopen(file.c_str(),"r")) == NULL ) {
  if (file.find('.') >= file.length()) {
    cout << "Commander::Exec(): Error opening file " << file << endl;
    file += ".pic";
    cout << "                Trying file " << file << endl;
    fip = fopen(file.c_str(),"r");
    }
  }

if(fip == NULL) {
  cerr << "Commander::Exec() Error opening file " << file << endl;
  hist << "##! Commander::Exec() Error opening file " << file << endl;
  return(0);
  }

// hist << "### Executing commands from " << file << endl;
PushStack(args);
if (trace) { 
  ShowMessage("### Executing commands from ", _MAGENTA_);
  ShowMessage(file.c_str(), _MAGENTA_);
  ShowMessage("\n", _MAGENTA_);
  }

bool ohv = histon;
histon = false; 
while (fgets(line_buff,1023,fip) != NULL)
  {
  if (trace) ShowMessage(line_buff, _MAGENTA_);
  line_buff[strlen(line_buff)-1] = '\0';   /*  LF/CR de la fin */
  string line(line_buff);
  rcc = Interpret(line);
  if ((rcc == CMD_RETURN_RC) || (rcc == CMD_BREAKEXE_RC)) break;
  }
histon = ohv; 

// hist << "### End of Exec( " << file << " ) " << endl;
if (trace) { 
  ShowMessage("### End of Exec( ", _MAGENTA_);
  ShowMessage(file.c_str(), _MAGENTA_);
  ShowMessage(" ) \n", _MAGENTA_);
  }

PopStack(true);

return(0);
}

/* --Methode-- */
void Commander::ListThreads()
{
  cout << "--- Commander::ListThreads()- command execution threads NThread=" 
       <<  CmdThrExeList.size() << " ---" << endl;
  for(list<CommandExeThr *>::iterator tit = CmdThrExeList.begin(); 
      tit != CmdThrExeList.end(); tit++) {
    cout << "Id=" << (*tit)->Id(); 
    if ( (*tit)->IfRunning() )  cout << " Executing";
    else if ( (*tit)->IfDone() )  cout << " Finished , Rc= " << (*tit)->getRC();
    else cout << " Stopped/Canceled";
    cout << " (Cmd= " << (*tit)->Keyword() << " " << (*tit)->Tokens().substr(0,35);
    if ((*tit)->Tokens().length() > 35) cout << "... )" << endl;
    else cout << " )" << endl;
  }
}
/* --Methode-- */
void Commander::StopThr(uint_8 id, bool fgkill)
{
  for(list<CommandExeThr *>::iterator tit = CmdThrExeList.begin(); 
      tit != CmdThrExeList.end(); tit++) {
    if ( ((*tit)->Id() == id) && ((*tit)->IfRunning()) ) { 
      if (fgkill) {
	(*tit)->kill(SIGUSR1);  
	cout << "Commander::StopThr()  Send signal SIGUSR1 to Thread Id= " << id << endl;
      }
      else {
	(*tit)->cancel();
	cout << "Commander::StopThr()  Canceling Thread Id= " << id << endl;
      }
      return;
    }
  }
  cout << "Commander::StopThr()/Error: No active thread with Id= " << id << endl;
}

/* --Methode-- */
void Commander::CleanThrList()
{
  cout << "---- Commander::CleanThrList()  Cleaning thrlist  ----- \n";
  list<CommandExeThr *> thrcopie;
  int ncl = 0;
  for(list<CommandExeThr *>::iterator tit = CmdThrExeList.begin(); 
      tit != CmdThrExeList.end(); tit++) {
    if ( ((*tit)->IfEnded() || (*tit)->IfStopped()) && (ncl < 3) ) { 
      cout << " Thread Id= " << (*tit)->Id() << " rc= " << (*tit)->getRC() << " Cleaned" << endl;
      delete (*tit);
      ncl++;
    }
    else thrcopie.push_back((*tit));
  }
  CmdThrExeList = thrcopie;
  cout << " ... " << CmdThrExeList.size() << " threads still active " << endl;
}

/* --Methode-- */
void Commander::WaitThreads()
{
  cout << "---- Commander::WaitThreads()  Wait/Join command execution threads - NThread=" 
       <<  CmdThrExeList.size() << " ----- " << endl;
  for(list<CommandExeThr *>::iterator tit = CmdThrExeList.begin(); 
      tit != CmdThrExeList.end(); tit++) {
    try {
      if (! (*tit)->IfDone())  (*tit)->join();
    }
    catch (std::exception & e) {
      cout << " Commander::WaitThreads()/Exception msg= " << e.what() << endl;
    }
    cout << " Joined thread Id= " << (*tit)->Id() << " rc= " << (*tit)->getRC() << endl;
    delete (*tit);
  }
  CmdThrExeList.erase(CmdThrExeList.begin(), CmdThrExeList.end());
}

/* --Methode-- */
int Commander::CShellExecute(string cmd)
{
#if TARGET_OS_IPHONE //G.Barrand :
 cout << "Commander::CShellExecute() : iOS : system() not available." << endl;
 return -2;
#else
 if(cmd.size()<=0) return -1;

 string fname = GetTmpDir(); fname += "cshell_exec_pia.csh";

 string cmdrm = "rm -f " + fname;
 system(cmdrm.c_str());
 
 FILE *fip = fopen(fname.c_str(),"w");
 if(fip==NULL)   { 
   cout << "Commander/CShellExecute_Error: fopen("<<fname<<") failed"<<endl; 
   return -2; 
 }
 fprintf(fip,"#!/bin/csh\n\n");
 fprintf(fip,"%s\n",cmd.c_str());
 fprintf(fip,"\nexit 0\n");
 fclose(fip);

 cmd = "csh "; cmd += fname;
 system(cmd.c_str());

 system(cmdrm.c_str());

 return 0;
#endif
}

static string* videstr = NULL;
/* --Methode-- */
string& Commander::GetUsage(const string& kw)
{
bool fndok = false;
CmdExmap::iterator it = cmdexmap.find(kw);
if (it == cmdexmap.end()) {
  it = helpexmap.find(kw);
  if (it != helpexmap.end())  fndok = true;
  }
  else  fndok = true; 
if (fndok)   return( (*it).second.us ); 
// Keyword pas trouve
if (videstr == NULL) videstr = new string("");
*videstr =  "Nothing known about " + kw + " ?? ";
return(*videstr);
 
}


/*  Les definitions suivantes doivent se trouver ds l'en-tete du fichier LaTeX 
  \newcommand{\piacommand}[1]{
    \framebox{\bf \Large #1 } \index{#1} % (Command) 
  }

  \newcommand{\piahelpitem}[1]{
    \framebox{\bf \Large #1 } \index{#1} (Help item)
  }

  \newcommand{\myppageref}[1]{ (p. \pageref{#1} ) }
*/

// Fonction qui remplace tout caractere non alphanumerique en Z
static void check_latex_reflabel(string & prl)
{
  for(unsigned int k=0; k<prl.length(); k++) 
    if (! isalnum(prl[k]) )  prl[k] = 'Z';
}

// Fonction qui remplace _ en \_
static string check_latex_underscore(string const & mot)
{
  string rs;
  for(unsigned int k=0; k<mot.length(); k++) {
    if (mot[k] == '_')  rs += "\\_"; 
    else rs += mot[k];
  }
  return rs;
}

/* --Methode-- */
/*!
  \brief Produces a LaTeX file containing the registered command helps
  The file \b fname is created and can be inserted into a LaTeX document
  in order to produce the list of registered commands and corresponding description
  texts.
  The LaTeX file should contain the following definitions:
\verbatim
  \newcommand{\piacommand}[1]{
    \framebox{\bf \Large #1 } \index{#1} % (Command) 
  }

  \newcommand{\piahelpitem}[1]{
    \framebox{\bf \Large #1 } \index{#1} (Help item)
  }

  \newcommand{\myppageref}[1]{ (p. \pageref{#1} ) }

\endverbatim
*/
void Commander::HelptoLaTeX(string const & fname)
{
FILE *fip;
if ((fip = fopen(fname.c_str(), "w")) == NULL)   { 
  cout << "Commander::HelptoLaTex_Error: fopen( " << fname << endl; 
  return;
  }

fputs("% ----- Liste des groupes de Help ----- \n",fip);
fputs("List of {\\bf piapp} on-line Help groups: \n", fip);
fputs("\\begin{itemize} \n",fip);
string prl;
string mol;
CmdHGroup::iterator it;
for(it = cmdhgrp.begin(); it != cmdhgrp.end(); it++) {
  if ((*it).first == "All") continue;
  prl = (*it).first;  check_latex_reflabel(prl);
  mol = check_latex_underscore((*it).first);
  fprintf(fip,"\\item {\\bf %s }  (p. \\pageref{%s}) \n", 
	  mol.c_str(), prl.c_str());
}

fputs("\\end{itemize} \n",fip);

fputs("\\vspace*{10mm} \n",fip);

CmdExmap::iterator ite;
fputs("% ----- Liste de toutes les commandes et help item ----- \n",fip);
fputs("\\vspace{5mm} \n",fip);
// fputs("\\begin{table}[h!] \n", fip);
fputs("\\begin{center} \n ", fip);
fputs("\\rule{2cm}{1mm} List of {\\bf piapp} Help items \\rule{2cm}{1mm} \\\\ \n", fip);
fputs("\\vspace{3mm} \n",fip);
fputs("\\begin{tabular}{llllll}  \n", fip);
int kt = 0;
for(ite = helpexmap.begin(); ite != helpexmap.end(); ite++) {
  prl = (*ite).first;  check_latex_reflabel(prl);
  mol = check_latex_underscore((*ite).first);
  fprintf(fip,"%s & p. \\pageref{%s} ", mol.c_str(),  prl.c_str() );
  kt++;
  if (kt < 3) fputs(" & ", fip);
  else  { fputs(" \\\\  \n", fip);  kt = 0; }
  }
if (kt == 1) fputs("  &  &  &   \\\\  \n", fip);
else if (kt == 2)  fputs("  &   \\\\  \n", fip);
fputs("\\end{tabular} \n", fip);
fputs("\\end{center} \n", fip);
//fputs("\\end{table} \n", fip);
fputs("\\newpage  \n",fip);

int gid;
for(it = cmdhgrp.begin(); it != cmdhgrp.end(); it++) {
  gid = (*it).second.gid;
  if (gid == 0)  continue;
  //  fputs("\\begin{table}[h!] \n",fip);
  fputs("\\vspace{6mm} \n",fip);
  fputs("\\begin{center} \n ", fip);
  fprintf(fip, "\\rule{2cm}{0.5mm} \\makebox[60mm]{{ \\bf %s } help group} \\rule{2cm}{0.5mm} \\\\ \n", 
	  (*it).first.c_str());
  fputs("\\vspace{3mm} \n",fip);
  fputs("\\begin{tabular}{llllll} \n", fip);
  kt = 0;
  for(ite = helpexmap.begin(); ite != helpexmap.end(); ite++) {
    if ((*ite).second.group != gid)  continue;
    prl = (*ite).first;  check_latex_reflabel(prl);
    mol = check_latex_underscore((*ite).first);
    fprintf(fip,"%s & p. \\pageref{%s} ", mol.c_str(),  prl.c_str() );
    kt++;
    if (kt < 3) fputs(" & ", fip);
    else  { fputs(" \\\\  \n", fip);  kt = 0; }
  }
  for(ite = cmdexmap.begin(); ite != cmdexmap.end(); ite++) {
    if ((*ite).second.group != gid)  continue;
    prl = (*ite).first;  check_latex_reflabel(prl);
    mol = check_latex_underscore((*ite).first);
    fprintf(fip,"%s & p. \\pageref{%s} ", mol.c_str(),  prl.c_str() );
    kt++;
    if (kt < 3) fputs(" & ", fip);
    else  { fputs(" \\\\  \n", fip);  kt = 0; }
  }
  if (kt == 1) fputs("  &  &  &   \\\\  \n", fip);
  else if (kt == 2)  fputs("  &   \\\\  \n", fip);
  fputs("\\end{tabular} \n", fip);
  fputs("\\end{center} \n", fip);
  //  fputs("\\end{table} \n",fip);
  //  fputs("\\vspace{5mm} \n",fip);
}
// fputs("\\newline \n",fip);

fputs("% ----- Liste des commandes dans chaque groupe ----- \n",fip);
fputs("\\newpage \n",fip);

for(it = cmdhgrp.begin(); it != cmdhgrp.end(); it++) {
  gid = (*it).second.gid;
  if (gid == 0)  continue;
  prl = (*it).first;  check_latex_reflabel(prl);
  fprintf(fip,"\\subsection{%s} \\label{%s} \n", 
          (*it).first.c_str(), prl.c_str());
  if ((*it).second.desc.length() > 0) 
    fprintf(fip,"%s \n \\\\[2mm]  ", (*it).second.desc.c_str());
  fprintf(fip,"\\noindent \n");
  for(ite = helpexmap.begin(); ite != helpexmap.end(); ite++) {
    if ((*ite).second.group != gid)  continue;
    prl = (*ite).first;  check_latex_reflabel(prl);
    mol = check_latex_underscore((*ite).first);
    fprintf(fip,"\\piahelpitem{%s} \\label{%s} \n", 
            mol.c_str(), prl.c_str());
    fputs("\\begin{verbatim} \n",fip);
    fprintf(fip,"%s\n", (*ite).second.us.c_str());
    fputs("\\end{verbatim} \n",fip);
    }
  for(ite = cmdexmap.begin(); ite != cmdexmap.end(); ite++) {
    if ((*ite).second.group != gid)  continue;
    prl = (*ite).first;  check_latex_reflabel(prl);
    mol = check_latex_underscore((*ite).first);
    fprintf(fip,"\\piacommand{%s} \\label{%s} \n", 
            mol.c_str(), prl.c_str());
    fputs("\\begin{verbatim} \n",fip);
    fprintf(fip,"%s\n", (*ite).second.us.c_str());
    fputs("\\end{verbatim} \n",fip);
    }
}

fclose(fip);
cout << " Commander::HelptoLaTeX() - LaTeX format help written to file " <<  fname << endl;
 
return;
}


}  // End of namespace SOPHYA

