#include "sopnamsp.h"
#include "machdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
//G.Barrand : #include "sophyainit.h"
#include "timing.h"
#include "cexpre.h"
#include "rpneval.h"
#include "commander.h"

#if defined(__APPLE__) //G.Barrand : to disconnect system().
#include <TargetConditionals.h>
#endif

/* -------------- Programme test de SOPHYA   tcmd.cc  
   Test des classes Timer , CExpressionEvaluator , 
   RPNExpressionEvaluator , Commander  , 
   csh> tcmd commander
   csh> tcmd cexptst      ( check Rc=0 ) 
   csh> tcmd cexptv   or  tcmd cexptv 5000   ( check Rc=0 ) 
   csh> tcmd cexptvp   or  tcmd cexptvp 5000   ( check Rc=0 ) 
   csh> tcmd rpntst   ( check Rc=0 ) 
   csh  tcmd timer
   ------ R. Ansari ---- 2003-2012    --------------- */

size_t NLOOP = 1000;
int commander_inputloop();   // Test de la classe Commander 
int tst_cexpreval();  // Test de CExpressionEvaluator 
int tst_cexpreval_v(bool fgparse=false);  // Test de CExpressionEvaluator avec variables 
int tst_rpneval();    // Test de RPNEvaluator 
int tst_timer();      // Test de Timer

/* --Main-- */
int main(int narg, char *arg[])
{

  if (narg < 2) {
    cout << " Usage: tcmd S=commander/cexptst/rpntst/timer [NLOOP , def=1000] \n" 
	 << "  S= commander: Commader class test \n"
	 << "  S= cexptst: CExpressionEvaluator class test \n"
	 << "  S= cexptv : CExpressionEvaluator class test with variables\n"
	 << "  S= cexptvp : CExpressionEvaluator class test with variables, parse each time \n"
	 << "  S= rpntst: RPNExpressionEvaluator class test \n"
	 << "  S= timer: Timer class test \n" << endl;
    return 0;
  }
  string opt = arg[1];
  if (narg>2)  NLOOP=atoi(arg[2]); 
  //G.Barrand : SophyaInit();
  cout << "NOTE : SophyaInit() not done." << std::endl;
  InitTim();
  int rc = 0;
  cout << " ---- tcmd S= " << opt << " (commander/evaluator test) " << endl;
  try {
    if (opt == "commander") commander_inputloop();
    else if (opt == "cexptst") rc = tst_cexpreval();
    else if (opt == "cexptv") rc = tst_cexpreval_v();
    else if (opt == "cexptvp") rc = tst_cexpreval_v(true);
    else if (opt == "rpntst")   rc = tst_rpneval();
    else if (opt == "timer")   rc = tst_timer();
    else {
      cout << " tcmd/error: Unknown select option: " << opt << endl;
      rc = 9;
    }
  }
 
  catch (PThrowable & exc) {
    cerr << " Catched Exception " << (string)typeid(exc).name()
         << " - Msg= " << exc.Msg() << endl;
  }
  catch (std::exception & e) {
    cerr << " exception catched : e.what()= " << e.what() << endl;
  }
  catch (...) {
    cerr << " some other exception was caught ! " << endl;
  }

  cout << " =========== End of tcmd " << opt << " -> RC= " << rc << " ===========" << endl;
  PrtTim(" End of tcmd ");
  PrtTim("--Fin tcmd ");
  return(rc);
}

// ----- Test of Commander class ------
// ------ Test Executor class 
class TCmdExecutor : public CmdExecutor {
public:
  TCmdExecutor(Commander& cmd);
  virtual ~TCmdExecutor() { }

  virtual int Execute(string& keyw,vector<string>& args, string& toks);
};

/* --Methode-- */
TCmdExecutor::TCmdExecutor(Commander& cmd)
{ 
string hgrp = "TCmdExecutor";
string usage,kw;

kw = "ls";
usage = "ls: Execute /usr/bin/ls \n";
cmd.RegisterCommand(kw, usage, this, hgrp);
kw = "mv";
usage = "mv: Execute /usr/bin/mv \n";
cmd.RegisterCommand(kw, usage, this, hgrp);
kw = "cp";
usage = "cp: Execute /usr/bin/cp \n";
cmd.RegisterCommand(kw, usage, this, hgrp);
}

/* --Methode-- */
int TCmdExecutor::Execute(string& kw, vector<string>& args, string& toks)
{
#if TARGET_OS_IPHONE //G.Barrand :
  cout << "TCmdExecutor::Execute() : iOS : system() not available." << endl;
  return -1;
#else
  if ((kw!="ls")&&(kw!="mv")&&(kw!="cp"))  return 99; 
  string cmd;
  if (kw == "ls")     cmd = "/usr/bin/ls " ; 
  else if (kw == "mv") cmd = "/usr/bin/mv " ;
  else if (kw == "cp") cmd = "/usr/bin/cp " ;
  else cmd = "/usr/bin/echo " ;
  for(int kk=0; kk<args.size(); kk++)  { cmd += args[kk]; cmd += ' '; }
  cout << "TCmdExecutor::Execute() : Executing " << cmd << endl;
  return system(cmd.c_str());
#endif  
}

/* --Fonction-- */
int  commander_inputloop()
{
  cout << " ====================================================== " << endl;
  cout << " ==============   Test of Commander class  ============ " << endl;
  cout << " ====================================================== " << endl;
  Commander cmd;
  TCmdExecutor cmdex(cmd);

  cout << " tcmd/InputLoop() : Type in your commands, \n" 
       << "     end with a blanck line OR <Cntl>D " << endl;
  int line = 0;
  bool fg = true;
  char buff[1024];
  char * ret;
  while (fg) {
    printf("%d-%s ", line+1, cmd.GetCurrentPrompt().c_str());
    fflush(stdout);
    buff[0] = '\0';
    ret = fgets(buff, 1024, stdin);
    buff[1023] = '\0';
    if (ret && ( (buff[0] != '\0') && (buff[0] != '\n') && (buff[0] != '\r')) ) { 
      buff[strlen(buff)-1] = '\0';
      string cline = buff;
      cmd.Interpret(cline);
      line++;
    }
    else fg = false;
  }
  cout << " \n Total " << line << " lines from stdin -> Commander " << endl;
  if (line > 0) return(0);
  else return(1);
}

/* -- Fonction Test de CExpressionEvaluator -- */
int tst_cexpreval()
{
  double res;
  int nok=0;
  int nerr=0;
  int nerrparse=0;
  int num = 0;
  cout << " ====================================================== " << endl;
  cout << " ============   Test of CExpressionEvaluator ========== " << endl;
  cout << " ====================================================== " << endl;
  try {
    try {
      num++;
      CExpressionEvaluator cex("4.e-1");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << " =? " << ( res=4.e-1 ) << endl;
      if (fabs(cex.Value()-res) > 1.e-19) {
	cout << " ERREUR CALCUL " << cex << endl;
	nerr++;
      } 
      else nok++;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;

    try {
      num++;
      CExpressionEvaluator cex("4*3+8");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << " =? 4*3+8 = " << ( res=4*3+8 ) << endl;
      if (fabs(cex.Value()-res) > 1.e-19) {
	cout << " ERREUR CALCUL " << cex << endl;
	nerr++;
      } 
      else nok++;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;

    try {
      num++;
      CExpressionEvaluator cex("cos(0)+2");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << " =? cos(0)+2 = " << ( res=cos(0.)+2 ) << endl;
      if (fabs(cex.Value()-res) > 1.e-19) {
	cout << " ERREUR CALCUL " << cex << endl;
	nerr++;
      }
      else nok++;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;


    try {
      num++;
      CExpressionEvaluator cex("4+3*8");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << " =? 4+3*8 = " << ( res=4+3*8 ) << endl;
      if (fabs(cex.Value()-res) > 1.e-19) {
	cout << " ERREUR CALCUL " << cex << endl;
	nerr++;
      }
      else nok++;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;


    try {
      num++;
      CExpressionEvaluator cex("4+3*6*2");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << " =? 4+3*6*2 = " << ( res=4+3*6*2 ) << endl;
      if (fabs(cex.Value()-res) > 1.e-19) {
	cout << " ERREUR CALCUL " << cex << endl;
	nerr++;
      }
      else nok++;
    }

    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;

    try {
      num++;
      CExpressionEvaluator cex("(12+3*6*cos(0))");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << " =? (12+3*6*cos(0))  = " 
	   << ( res=(12+3*6*cos(0.)) ) << endl;
      if (fabs(cex.Value()-res) > 1.e-19) {
	cout << " ERREUR CALCUL " << cex << endl;
	nerr++;
      }
      else nok++;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;

    try {
      num++;    
      CExpressionEvaluator cex("(12+3*6*cos(0))-(5*pow(2,2))");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << " =? (12+3*6*cos(0))-(5*pow(2,2))  = " 
	   << ( res=(12+3*6*cos(0.))-(5*pow(2.,2.)) ) << endl;
      if (fabs(cex.Value()-res) > 1.e-19) {
	cout << " ERREUR CALCUL " << cex << endl;
	nerr++;
      }
      else nok++;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;

    try {
      num++;    
      CExpressionEvaluator cex("sin(Pi/6)+E");
      cout << "CExpr=sin(Pi/6)+E " << cex; 
      cout << " -> cex.Value() = " << cex.Value() << " =? sin(M_PI/6)+M_E  = " 
	   << ( res=sin(M_PI/6)+M_E ) << endl;
      if (fabs(cex.Value()-res) > 1.e-19) {
	cout << " ERREUR CALCUL " << cex << endl;
	nerr++;
      }
      else nok++;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;

    // Expression avec erreur de syntaxe 
   cout << " >>>>> Expression avec erreur <<<<<< " << endl;
   try {
      num++;    
      CExpressionEvaluator cex("6**8");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << endl;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;

   try {
      num++;    
      CExpressionEvaluator cex("6*(8+4");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << endl;
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;


   try {
      num++;    
      CExpressionEvaluator cex("6*sin(4,)+12");
      cout << "CExpr=" << cex; 
      cout << " -> cex.Value() = " << cex.Value() << endl; 
    }
    catch (CExprException& err) {
      nerrparse++;
      cout << "Exp[" << num << "]  Exception: " << err.Msg() << endl; 
    }
    cout << "  [" << num << "] CExprBase::NbCreate() = " << CExprBase::NbCreate() 
	 << "CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;

  }
  catch (CExprException& err) {
    cout << " Exception: " << err.Msg() << endl; 
  }

  cout << "-------- CExprBase::NbCreate() = " << CExprBase::NbCreate() << " CExprBase::NbDelete()=" <<  CExprBase::NbDelete() << endl;
  cout << "--- NExpr= " << num << " NOk= " <<  nok << " NErr=" << nerr << " NErrParse=" << nerrparse << endl;
  if ( (nok == 8) && (nerr == 0) && (nerrparse == 3) ) return 0;
  else return 99;
}

/* -- classe de test pour liste de variables a utiliser par CExpressionEvaluator -- */
class CE_TestVarList : public CE_VarListInterface {
public:
  CE_TestVarList(std::vector<std::string> const& names) 
  { 
    for(size_t i=0; i<names.size(); i++) nameids_[names[i] ] = i;
    pvalues_=new double[names.size()];
  }
  ~CE_TestVarList() { delete[] pvalues_; }

  inline double& operator[] (size_t n) { return pvalues_[n]; }
  inline double* getPointer() { return pvalues_; }
  double* GetVarPointer(std::string const& name)
  {
  std::map<std::string, size_t>::iterator it = nameids_.find(name);
  if (it==nameids_.end()) return NULL;
  else return pvalues_+(*it).second;
  }

protected:
  std::map<std::string, size_t> nameids_;
  double* pvalues_; 
};


/* -- Fonction Test de CExpressionEvaluator -- */
int tst_cexpreval_v(bool fgparse)
{
  double res;
  int nok=0;
  int nerr=0;
  int nerrparse=0;
  int num = 0;
  cout << " ============================================================ " << endl;
  cout << " =======   Test of CExpressionEvaluator with variables ====== " << endl;
  if (!fgparse) cout << " ======= (parse once - Loop[evaluate] ) NLoop=" << NLOOP << " ======= " << endl;
  else cout << " ======= (Loop[parse & evaluate] ) NLoop=" << NLOOP << " ======= " << endl;
  cout << " ============================================================ " << endl;
  Timer tm("tcmd::CExpVar");

  try {
    vector<string> names; 
    names.push_back("x");
    names.push_back("y");
    names.push_back("z");
    CE_TestVarList vars(names);
    CExpressionEvaluator cex("sin(0.7*x)+sqrt(y*z)",&vars);
    double x,y,z;
    for(size_t i=0;i<NLOOP; i++) {
      x=vars[0]=(double)i*7.56;  y=vars[1]=vars[0]+986.23;   z=vars[2]=(double)((i+1)*(i+1))*sqrt(0.7*i+3.);
      double res=sin(0.7*x)+sqrt(y*z);
      if (fgparse) {
	CExpressionEvaluator cexp("sin(0.7*x)+sqrt(y*z)",&vars);
	if (fabs(cexp.Value()-res) > 1.e-19)  nerr++; 
      }
      else { 
	if (fabs(cex.Value()-res) > 1.e-19)  nerr++; 
      }
      num++;
    }
  }
  catch (CExprException& err) {
    cout << " Exception: " << err.Msg() << endl; 
  }
  cout << "--- NExpr= " << num << " NErr=" << nerr << endl;
  if (nerr == 0)  return 0;
  else return 99;

}

/* -- Fonction Test de RPNExpressionEvaluator -- */
int tst_rpneval()
{
  double res;
  int nok=0;
  int nerr=0;
  int num = 0;
  cout << " ====================================================== " << endl;
  cout << " ============ Test of RPNExpressionEvaluator ========== " << endl;
  cout << " ====================================================== " << endl;
  try {
    { 
      num++;
      RPNExpressionEvaluator rpn("4 2 print + 3 * ");
      cout << "4 2 + 3 * -> rpn.Value() = " << rpn.Value() << " =? " << (res=(4+2)*3.) << endl;
      if (fabs(rpn.Value()-res)>1.e-19)  nerr++;
    }
    {
      num++;
      RPNExpressionEvaluator rpn("1 2 3 4 5 sum");
      cout << "1 2 3 4 5 sum -> rpn.Value() = " << rpn.Value() << " =? " << (res=(1+2+3+4+5.)) << endl;
      if (fabs(rpn.Value()-res)>1.e-19)  nerr++;
    }
    {
      num++;
      RPNExpressionEvaluator rpn("4 3 pow");
      cout << "4 3 pow -> rpn.Value() = " << rpn.Value() << " =? " << (res=4.*4.*4.) << endl;
      if (fabs(rpn.Value()-res)>1.e-19)  nerr++;
    }
    {
      num++;
      RPNExpressionEvaluator rpn("pi 0.7 * cos 5. log10 /");
      cout << "pi 0.7 * cos 5. log10 / -> rpn.Value() = " << rpn.Value() 
	   << " =? " << (res=cos(0.7*M_PI)/log10(5.)) << endl;
      if (fabs(rpn.Value()-res)>1.e-19)  nerr++;
    }
    {
      num++;
      RPNExpressionEvaluator rpn("9.5 e * log 34.5 7 - sqrt +");
      cout << "9.5 e * log 34.5 7 - sqrt + -> rpn.Value() = " << rpn.Value() 
	   << " =? " << (res=log(9.5*M_E)+sqrt(34.5-7)) << endl;
      if (fabs(rpn.Value()-res)>1.e-19)  nerr++;
    }

  }
  catch (RPNExprException& err) {
    cout << "RPNExp[" << num << "]  Exception: " << err.Msg() << endl; 
  }

  cout << "--- NExpr= " << num << " NOk= " <<  nok << " NErr=" << nerr << endl;

  if (nerr==0) return 0;
  else return 99;
}

/* --Fonction-- */
int  tst_timer() 
{
  TIMEF ;
  cout << "**** tcmd.cc : Test class SOPHYA::Timer() **** " << endl;
  Timer tm("tcmd::Timer", false);

  int N = 10000000;
  double * x = new double[N];
  for(int k=0; k<N; k++) x[k] = k*0.0001;
  for(int k=0; k<N; k++) x[k] = sin(x[k]);
  for(int k=0; k<N; k++) x[k] = cos(x[k]);
  tm.Split();
  cout << " Step1: PARTIAL CPU(s)=" << tm.PartialCPUTime() 
       << " Elapsed (s)= " << tm.PartialElapsedTime()
       << " Elapsed (ms)= " << tm.PartialElapsedTimems() << endl;
  cout << " Step1: TOTAL CPU(s)=" << tm.TotalCPUTime() 
       << "  Elapsed (s)= " << tm.TotalElapsedTime() 
       << "  Elapsed (ms)= " << tm.TotalElapsedTimems() << endl;

  for(int k=0; k<N; k++) x[k] *= 8.5; 
  for(int k=0; k<N; k++) x[k] = tan(x[k]);

  tm.Split();
  cout << " Step2: PARTIAL CPU(s)=" << tm.PartialCPUTime() 
       << " Elapsed (s)= " << tm.PartialElapsedTime()
       << " Elapsed (ms)= " << tm.PartialElapsedTimems() << endl;
  cout << " Step2: TOTAL CPU(s)=" << tm.TotalCPUTime() 
       << "  Elapsed (s)= " << tm.TotalElapsedTime() 
       << "  Elapsed (ms)= " << tm.TotalElapsedTimems() << endl;

  for(int k=0; k<N; k++) x[k] = sin(x[k]);
  for(int k=0; k<N; k++) x[k] = cos(x[k]);
  for(int k=0; k<N; k++) x[k] *= 15.5; 
  for(int k=0; k<N; k++) x[k] = sin(x[k]);

  tm.Split();
  cout << " Step3: PARTIAL CPU(s)=" << tm.PartialCPUTime() 
       << " Elapsed (s)= " << tm.PartialElapsedTime()
       << " Elapsed (ms)= " << tm.PartialElapsedTimems() << endl;
  cout << " Step3: TOTAL CPU(s)=" << tm.TotalCPUTime() 
       << "  Elapsed (s)= " << tm.TotalElapsedTime() 
       << "  Elapsed (ms)= " << tm.TotalElapsedTimems() << endl;

  tm.Split("CheckPrintFmt", true);
  cout << " Waiting 70 sec (sleep(70)) ... " << endl; 
  sleep(100);
  tm.Split("CheckPrintFmt-AfterWait", true);
 
  cout << " Doing delete[] x, sleep(5) ... " << endl;  
  delete[] x;
  sleep(5);
  return 0;
}

