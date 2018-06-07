#include "sopnamsp.h"
#include "machdefs.h"
#include <math.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <typeinfo>

#include "pexceptions.h"

#include "randinterf.h"

namespace SOPHYA {

//-------------------------------------------------------------------------------
// ------ Definition d'interface des classes de generateurs de nombres aleatoires
/*!
   \class RandomGeneratorInterface
   \ingroup BaseTools
   \brief Base class for random number generators

   This class defines the interface for random number generator classes and 
   implements the generation of some specific distributions (Gaussian, Poisson ...) 
   through generation of random number with a flat distribution in the range [0,1[. 

   The sub classes inheriting from this class should implement the Next() method.

   This base class manages also a global instance of a default generator.

   \sa frand01 drand01 frandpm1 drandpm1 
   \sa Gaussian Poisson

*/


RandomGeneratorInterface* RandomGeneratorInterface::gl_rndgen_p = NULL;

/*! 
   \brief: static method to set or change the intance of the global Random Generator object

   This method should be called during initialization, before any call to global 
   functions for random number generation. The rgp object should be created using new.
*/
void RandomGeneratorInterface::SetGlobalRandGenP(RandomGeneratorInterface* rgp)
{
  if (rgp == NULL) return;
  if (gl_rndgen_p) delete gl_rndgen_p;
  gl_rndgen_p = rgp;
  return;
}

RandomGeneratorInterface::RandomGeneratorInterface()
{
  SelectGaussianAlgo();
  SelectPoissonAlgo();
  SelectExponentialAlgo();
}
 

RandomGeneratorInterface::~RandomGeneratorInterface(void) 
{
  // rien a faire 
}

void RandomGeneratorInterface::ShowRandom()
{
  cout << " RandomGeneratorInterface::ShowRandom() typeid(this)=" << typeid(*this).name() << " @ " 
       << hex << (unsigned long)(this) << dec << endl;
  return;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/*
r_8 RandomGeneratorInterface::Next()
{
  printf("RandomGeneratorInterface::Next(): undefined code !!!\n");
  throw MathExc("RandomGeneratorInterface::Next(): undefined code !!!");
}
*/

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void RandomGeneratorInterface::GenerateSeedVector(int nseed,vector<uint_2>& seed,int lp)
// renvoie un vecteur de 3+nseed entiers 16 bits
// [0 - 2] = codage sur 3*16 = 48 bits du nombre (melange) de microsec depuis l'origine
// [3 -> 3+ngene-1] = entiers aleatoires (poor man generator)
//
// L'initialiseur est donne par un codage du nombre de millisecondes
// ecoulees depuis le 0 heure le 1er Janvier 1970 UTC (cf gettimeofday).
// Seuls les 48 bits de poids faible sont retenus.
// Un melange des bits est ensuite effectue pour que les 3 nombres
// (unsigned short) d'initialisation ne soient pas trop semblables.
// Le nombre le plus grand que l'on peut mettre
// dans un entier unsigned de N bits est: 2^N-1
// 48 bits -> 2^48-1 = 281474976710655 musec = 3257.8j = 8.9y
//         -> meme initialisation tous les 8.9 ans a 1 microsec pres !
{
  if(lp>0) cout<<"RandomGeneratorInterface::GenerateSeedVector: nseed="<<nseed<<endl;

  // ---
  // --- les deux premiers mots remplis avec le temps
  // ---
  // On recupere le temps ecoule depuis l'origine code en sec+musec
  struct timeval now;
  gettimeofday(&now,0);
  // Calcul du temps ecoule depuis l'origine en microsecondes
  uint_8 tmicro70 = (uint_8)now.tv_sec*(uint_8)1000000 + (uint_8)now.tv_usec;
  if(lp>1) cout<<".since orig: "<<now.tv_sec<<" sec + "<<now.tv_usec<<" musec = "<<tmicro70<<" musec"<<endl;
  // Remplissage du tableau de 48 bits
  uint_2 b[48]; uint_8 tdum = tmicro70;
  for(int ip=0;ip<48;ip++) {b[ip] = tdum&1; tdum = (tdum>>1);}
  if(lp>2) {
    cout<<"..b= ";
    for(int ip=47;ip>=0;ip--) {cout<<b[ip]; if(ip%32==0 || ip%16==0) cout<<" ";}
    cout<<endl;
  }
  // Melange des bits qui varient vite (poids faible, microsec)
  //   avec ceux variant lentement (poids fort, sec)
  for(int ip=0;ip<16;ip++) {
    if(ip%3==1) swap(b[ip],b[32+ip]);
    else if(ip%3==2) swap(b[ip],b[16+ip]);
  }
  if(lp>2) {
    cout<<"..b= ";
    for(int ip=47;ip>=0;ip--) {cout<<b[ip]; if(ip%32==0 || ip%16==0) cout<<" ";}
    cout<<endl;
  }
  // Remplissage
  seed.resize(0);
  for(int i=0;i<3;i++) {
    seed.push_back(0);
    uint_2 w = 1;
    for(int ip=0;ip<16;ip++) {seed[i] += w*b[i*16+ip]; w *= 2;}
  }
  if(lp>0) cout<<"seed_time[0-2]: "<<seed[0]<<" "<<seed[1]<<" "<<seed[2]<<endl;
  // On recree tmicro70 avec les bits melanges
  tmicro70 = uint_8(seed[0]) | (uint_8(seed[1])<<16) | (uint_8(seed[2])<<32);

  // ---
  // --- generation des nombres aleatoires complementaires (poor man generator)
  // ---
  //----------------------------------------------------------------------------//
  // Ran088: L'Ecuyer's 1996 three-component Tausworthe generator "taus88"
  // Returns an integer random number uniformly distributed within [0,4294967295]
  // The period length is approximately 2^88 (which is 3*10^26). 
  // This generator is very fast and passes all standard statistical tests.
  // Reference:
  //   (1) P. L'Ecuyer, Maximally equidistributed combined Tausworthe generators,
  //       Mathematics of Computation, 65, 203-213 (1996), see Figure 4.
  //   (2) recommended in:
  //       P. L'Ecuyer, Random number generation, chapter 4 of the
  //       Handbook on Simulation, Ed. Jerry Banks, Wiley, 1997.
  //----------------------------------------------------------------------------//
  if(nseed<=0) return;
  // initialize seeds using the given seed value taking care of
  // the requirements. The constants below are arbitrary otherwise
  uint_4 seed0 = uint_4(tmicro70&0xFFFFFFFFULL);
  if(lp>2) cout<<"seed0_time_init_t88: "<<seed0<<endl;
  uint_4 state_s1, state_s2, state_s3;
  state_s1 = 1243598713U ^ seed0; if (state_s1 <  2) state_s1 = 1243598713U;
  state_s2 = 3093459404U ^ seed0; if (state_s2 <  8) state_s2 = 3093459404U;
  state_s3 = 1821928721U ^ seed0; if (state_s3 < 16) state_s3 = 1821928721U;
  int nfill = 0, ico=0;	
  while(nfill<nseed) {
    uint_4 s1 = state_s1, s2 = state_s2, s3 = state_s3;
    // generate a random 32 bit number
    s1 = ((s1 &  -2) << 12) ^ (((s1 << 13) ^  s1) >> 19);
    s2 = ((s2 &  -8) <<  4) ^ (((s2 <<  2) ^  s2) >> 25);
    s3 = ((s3 & -16) << 17) ^ (((s3 <<  3) ^  s3) >> 11);
    state_s1 = s1; state_s2 = s2; state_s3 = s3;
    // le nombre aleatoire sur 32 bits est: s1^s2^s3
    if(ico<15) {ico++; continue;}  // des tirages blancs
    uint_2 s = uint_2( (s1^s2^s3)&0xFFFFU );
    seed.push_back(s);
    if(lp>0) cout<<"seed_t88["<<nfill+3<<"]: "<<seed[3+nfill]<<endl;
    nfill++;
  }

}

void RandomGeneratorInterface::AutoInit(int lp)
{
  printf("RandomGeneratorInterface::AutoInit(): undefined code !!!\n");
  throw MathExc("RandomGeneratorInterface::AutoInit(): undefined code !!!");
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

r_8 RandomGeneratorInterface::Gaussian() 
{
  switch (usegaussian_) {
    case C_Gaussian_BoxMuller : 
      return GaussianBoxMuller();
      break;
    case C_Gaussian_RandLibSNorm :
      return GaussianSNorm();
      break;
    case C_Gaussian_PolarBoxMuller :
      return GaussianPolarBoxMuller();
      break;
    case C_Gaussian_RatioUnif :
      return GaussianRatioUnif();
      break;
    case C_Gaussian_LevaRatioUnif :
      return GaussianLevaRatioUnif();
      break;
    case C_Gaussian_Ziggurat128 :
      return GaussianZiggurat128();
      break;
    default:
      return GaussianBoxMuller();
      break;
  }
}

//--- Generation de nombre aleatoires suivant une distribution gaussienne 
r_8 RandomGeneratorInterface::GaussianBoxMuller() 
{
  r_8 A=Next(); 
  while (A==0.) A=Next();
  return sqrt(-2.*log(A))*cos(2.*M_PI*Next());
}

//-------------------------------------------
// Adapte de ranlib float snorm() 
// http://orion.math.iastate.edu/burkardt/c_src/ranlib/ranlib.c
/*
**********************************************************************
     (STANDARD-)  N O R M A L  DISTRIBUTION
**********************************************************************
                                                                      
     FOR DETAILS SEE:                                                 
                                                                      
               AHRENS, J.H. AND DIETER, U.                            
               EXTENSIONS OF FORSYTHE'S METHOD FOR RANDOM             
               SAMPLING FROM THE NORMAL DISTRIBUTION.                 
               MATH. COMPUT., 27,124 (OCT. 1973), 927 - 937.          
                                                                      
     ALL STATEMENT NUMBERS CORRESPOND TO THE STEPS OF ALGORITHM 'FL'  
     (M=5) IN THE ABOVE PAPER     (SLIGHTLY MODIFIED IMPLEMENTATION)  
                                                                      
     Modified by Barry W. Brown, Feb 3, 1988 to use RANF instead of   
     SUNIF.  The argument IR thus goes away.                          
                                                                      
**********************************************************************
     THE DEFINITIONS OF THE CONSTANTS A(K), D(K), T(K) AND
     H(K) ARE ACCORDING TO THE ABOVEMENTIONED ARTICLE
*/
static double a_snorm[32] = {
    0.0,3.917609E-2,7.841241E-2,0.11777,0.1573107,0.1970991,0.2372021,0.2776904,
    0.3186394,0.36013,0.4022501,0.4450965,0.4887764,0.5334097,0.5791322,
    0.626099,0.6744898,0.7245144,0.7764218,0.8305109,0.8871466,0.9467818,
    1.00999,1.077516,1.150349,1.229859,1.318011,1.417797,1.534121,1.67594,
    1.862732,2.153875
};
static double d_snorm[31] = {
    0.0,0.0,0.0,0.0,0.0,0.2636843,0.2425085,0.2255674,0.2116342,0.1999243,
    0.1899108,0.1812252,0.1736014,0.1668419,0.1607967,0.1553497,0.1504094,
    0.1459026,0.14177,0.1379632,0.1344418,0.1311722,0.128126,0.1252791,
    0.1226109,0.1201036,0.1177417,0.1155119,0.1134023,0.1114027,0.1095039
};
static float t_snorm[31] = {
    7.673828E-4,2.30687E-3,3.860618E-3,5.438454E-3,7.0507E-3,8.708396E-3,
    1.042357E-2,1.220953E-2,1.408125E-2,1.605579E-2,1.81529E-2,2.039573E-2,
    2.281177E-2,2.543407E-2,2.830296E-2,3.146822E-2,3.499233E-2,3.895483E-2,
    4.345878E-2,4.864035E-2,5.468334E-2,6.184222E-2,7.047983E-2,8.113195E-2,
    9.462444E-2,0.1123001,0.136498,0.1716886,0.2276241,0.330498,0.5847031
};
static float h_snorm[31] = {
    3.920617E-2,3.932705E-2,3.951E-2,3.975703E-2,4.007093E-2,4.045533E-2,
    4.091481E-2,4.145507E-2,4.208311E-2,4.280748E-2,4.363863E-2,4.458932E-2,
    4.567523E-2,4.691571E-2,4.833487E-2,4.996298E-2,5.183859E-2,5.401138E-2,
    5.654656E-2,5.95313E-2,6.308489E-2,6.737503E-2,7.264544E-2,7.926471E-2,
    8.781922E-2,9.930398E-2,0.11556,0.1404344,0.1836142,0.2790016,0.7010474
};
r_8 RandomGeneratorInterface::GaussianSNorm()
{
long i;
double snorm,u,s,ustar,aa,w,y,tt;
    u = Next();
    s = 0.0;
    if(u > 0.5) s = 1.0;
    u += (u-s);
    u = 32.0*u;
    i = (long) (u);
    if(i == 32) i = 31;
    if(i == 0) goto S100;
/*
                                START CENTER
*/
    ustar = u-(double)i;
    aa = *(a_snorm+i-1);
S40:
    if(ustar <= *(t_snorm+i-1)) goto S60;
    w = (ustar-*(t_snorm+i-1))**(h_snorm+i-1);
S50:
/*
                                EXIT   (BOTH CASES)
*/
    y = aa+w;
    snorm = y;
    if(s == 1.0) snorm = -y;
    return snorm;
S60:
/*
                                CENTER CONTINUED
*/
    u = Next();
    w = u*(*(a_snorm+i)-aa);
    tt = (0.5*w+aa)*w;
    goto S80;
S70:
    tt = u;
    ustar = Next();
S80:
    if(ustar > tt) goto S50;
    u = Next();
    if(ustar >= u) goto S70;
    ustar = Next();
    goto S40;
S100:
/*
                                START TAIL
*/
    i = 6;
    aa = *(a_snorm+31);
    goto S120;
S110:
    aa += *(d_snorm+i-1);
    i += 1;
S120:
    u += u;
    if(u < 1.0) goto S110;
    u -= 1.0;
S140:
    w = u**(d_snorm+i-1);
    tt = (0.5*w+aa)*w;
    goto S160;
S150:
    tt = u;
S160:
    ustar = Next();
    if(ustar > tt) goto S50;
    u = Next();
    if(ustar >= u) goto S150;
    u = Next();
    goto S140;
}

r_8 RandomGeneratorInterface::GaussianPolarBoxMuller()
{
 double x1,x2,w;
 do {
   x1 = 2.0 * Next() - 1.0;
   x2 = 2.0 * Next() - 1.0;
   w = x1 * x1 + x2 * x2;
 } while ( w >= 1.0 || w==0. );
 return x1 * sqrt(-2.0*log(w)/w);
}

static double s2se_RatioUnif=sqrt(2./M_E) , epm135_RatioUnif=exp(-1.35) , ep1q_RatioUnif=exp(1./4.);
r_8 RandomGeneratorInterface::GaussianRatioUnif()
{
 double u,v,x;
 while(true) {
   do {u = Next();} while ( u == 0. );
   v = (2.0*Next()-1.0)*s2se_RatioUnif;
   x = v/u;
   if(x*x <= 5.0-4.0*ep1q_RatioUnif*u) break;
   if(x*x<4.0*epm135_RatioUnif/u+1.4)
     if(v*v<-4.0*u*u*log(u)) break;
 }
 return x;
}

r_8 RandomGeneratorInterface::GaussianLevaRatioUnif()
{
 double u,v,x,y,q;
 do {
   u = 1.-Next();  // in ]0,1]
   v = Next()-0.5;  // in [-0.5, 0.5[
   v *= 1.7156;
   x = u - 0.449871;
   y = ((v<0)?-v:v) + 0.386595;
   q = x*x + y*(0.19600*y - 0.25472*x);
 } while( q>=0.27597 && (q>0.27846  || v*v>-4.0*u*u*log(u)) );
 return v/u;
}

//-------------------------------------------------------------------------------
// Definition des tableaux pour tirage Gaussien methode Ziggurat N=128 bandes
// G.Marsaglia and W.W.Tsang "The ziggurat method for generating random variables
// D.Thomas et al. ACM Computing Surveys, Vol 39, No 4, Article 11, October 2007
// http://en.wikipedia.org/wiki/Ziggurat_algorithm  (tres bien explique)
// Calcul des tableaux avec programme "cmvziggurat.cc"
//-------------------------------------------------------------------------------
const int N_ZIGGURRAT_GAUSS128 = 128;
static const double X_ZIGGURRAT_GAUSS128[N_ZIGGURRAT_GAUSS128+1] = {
0.0000000000000000, 0.2723208647046734, 0.3628714310284243, 0.4265479863033096, 0.4774378372537916, 
0.5206560387251481, 0.5586921783755209, 0.5929629424419807, 0.6243585973090908, 0.6534786387150446, 
0.6807479186459064, 0.7064796113136101, 0.7309119106218833, 0.7542306644345121, 0.7765839878761502, 
0.7980920606262765, 0.8188539066833194, 0.8389522142812090, 0.8584568431780525, 0.8774274290977171, 
0.8959153525662399, 0.9139652510088031, 0.9316161966013551, 0.9489026254979132, 0.9658550793881319, 
0.9825008035027615, 0.9988642334806447, 1.0149673952393006, 1.0308302360564565, 1.0464709007525812, 
1.0619059636836206, 1.0771506248819389, 1.0922188768965548, 1.1071236475235364, 1.1218769225722551, 
1.1364898520030764, 1.1509728421389769, 1.1653356361550478, 1.1795873846544616, 1.1937367078237728, 
1.2077917504067583, 1.2217602305309634, 1.2356494832544818, 1.2494664995643345, 1.2632179614460288, 
1.2769102735517004, 1.2905495919178738, 1.3041418501204223, 1.3176927832013436, 1.3312079496576772, 
1.3446927517457137, 1.3581524543224235, 1.3715922024197329, 1.3850170377251492, 1.3984319141236070, 
1.4118417124397606, 1.4252512545068619, 1.4386653166774619, 1.4520886428822168, 1.4655259573357950, 
1.4789819769830983, 1.4924614237746157, 1.5059690368565506, 1.5195095847593711, 1.5330878776675565, 
1.5467087798535037, 1.5603772223598409, 1.5740982160167500, 1.5878768648844011, 1.6017183802152775, 
1.6156280950371333, 1.6296114794646788, 1.6436741568569830, 1.6578219209482079, 1.6720607540918526, 
1.6863968467734867, 1.7008366185643014, 1.7153867407081171, 1.7300541605582440, 1.7448461281083769, 
1.7597702248942324, 1.7748343955807697, 1.7900469825946195, 1.8054167642140493, 1.8209529965910054, 
1.8366654602533845, 1.8525645117230873, 1.8686611409895424, 1.8849670357028696, 1.9014946531003181, 
1.9182573008597323, 1.9352692282919006, 1.9525457295488893, 1.9701032608497135, 1.9879595741230611, 
2.0061338699589673, 2.0246469733729340, 2.0435215366506698, 2.0627822745039639, 2.0824562379877247, 
2.1025731351849992, 2.1231657086697902, 2.1442701823562618, 2.1659267937448412, 2.1881804320720208, 
2.2110814088747279, 2.2346863955870573, 2.2590595738653296, 2.2842740596736570, 2.3104136836950024, 
2.3375752413355309, 2.3658713701139877, 2.3954342780074676, 2.4264206455302118, 2.4590181774083506, 
2.4934545220919508, 2.5300096723854670, 2.5690336259216395, 2.6109722484286135, 2.6564064112581929, 
2.7061135731187225, 2.7611693723841539, 2.8231253505459666, 2.8943440070186708, 2.9786962526450171, 
3.0832288582142140, 3.2230849845786187, 3.4426198558966523, 3.7130862467403638
};
static const double Y_ZIGGURRAT_GAUSS128[N_ZIGGURRAT_GAUSS128+1] = {
1.0000000000000000, 0.9635996931557651, 0.9362826817083690, 0.9130436479920363, 0.8922816508023012, 
0.8732430489268521, 0.8555006078850629, 0.8387836053106459, 0.8229072113952607, 0.8077382946961199, 
0.7931770117838580, 0.7791460859417020, 0.7655841739092348, 0.7524415591857027, 0.7396772436833371, 
0.7272569183545049, 0.7151515074204761, 0.7033360990258165, 0.6917891434460349, 0.6804918410064135, 
0.6694276673577053, 0.6585820000586529, 0.6479418211185500, 0.6374954773431442, 0.6272324852578138, 
0.6171433708265618, 0.6072195366326042, 0.5974531509518116, 0.5878370544418199, 0.5783646811267017, 
0.5690299910747210, 0.5598274127106941, 0.5507517931210546, 0.5417983550317235, 0.5329626593899870, 
0.5242405726789923, 0.5156282382498716, 0.5071220510813041, 0.4987186354765838, 0.4904148252893212, 
0.4822076463348383, 0.4740943006982492, 0.4660721526945706, 0.4581387162728716, 0.4502916436869266, 
0.4425287152802462, 0.4348478302546615, 0.4272469983095620, 0.4197243320540379, 0.4122780401070242, 
0.4049064208114880, 0.3976078564980422, 0.3903808082413892, 0.3832238110598833, 0.3761354695144541, 
0.3691144536682749, 0.3621594953730330, 0.3552693848515469, 0.3484429675498723, 0.3416791412350135, 
0.3349768533169710, 0.3283350983761522, 0.3217529158792085, 0.3152293880681574, 0.3087636380092518, 
0.3023548277894796, 0.2960021568498557, 0.2897048604458103, 0.2834622082260124, 0.2772735029218976, 
0.2711380791410251, 0.2650553022581618, 0.2590245673987105, 0.2530452985097656, 0.2471169475146965, 
0.2412389935477511, 0.2354109422657275, 0.2296323252343025, 0.2239026993871337, 0.2182216465563704, 
0.2125887730737359, 0.2070037094418736, 0.2014661100762031, 0.1959756531181102, 0.1905320403209136, 
0.1851349970107133, 0.1797842721249620, 0.1744796383324022, 0.1692208922389246, 0.1640078546849276, 
0.1588403711409350, 0.1537183122095865, 0.1486415742436969, 0.1436100800919329, 0.1386237799858510, 
0.1336826525846476, 0.1287867061971039, 0.1239359802039817, 0.1191305467087185, 0.1143705124498882, 
0.1096560210158177, 0.1049872554103545, 0.1003644410295455, 0.0957878491225781, 0.0912578008276347, 
0.0867746718955429, 0.0823388982429574, 0.0779509825146547, 0.0736115018847548, 0.0693211173941802, 
0.0650805852136318, 0.0608907703485663, 0.0567526634815385, 0.0526674019035031, 0.0486362958602840, 
0.0446608622008724, 0.0407428680747906, 0.0368843887869688, 0.0330878861465051, 0.0293563174402538, 
0.0256932919361496, 0.0221033046161116, 0.0185921027371658, 0.0151672980106720, 0.0118394786579823, 
0.0086244844129305, 0.0055489952208165, 0.0026696290839025, 0.0000000000000000
};


r_8 RandomGeneratorInterface::GaussianZiggurat128()
//--------
// On a "N = 128" bandes horizontales numerotees [0,N-1=127]
// Les tableaux ont une taille "N + 1 = 129"
// On tire un numero de bande dans [0,N-1=127]
//--------
// Pour choisir le signe sans avoir a retirer un aleatoire,
// on utilise un digit du premier tirage qui n'est pas utilise
// dans le choix du numero de bande "I":
// U = [0,1[ , Ai=[0,9] (chiffres)
// U = A1/10 + A2/100 + A3/1000 + A4/10000 + A5/100000 + ...
// 128*U = A1*12.8 + A2*1.28 + A3*0.128 + A4*0.0128 + A5*0.00128
// pour Ai le plus grand possible cad 9
//   128*U = 115.2 + 11.52 + 1.152 + 0.1152 + 0.01152
// On voit que pour Ai = 9
//  1 terme,  A1       : I = int(128*U) = int(115.2) = 115
//  2 termes, A1,2     : I = int(128*U) = int(115.2+11.52) = int(126.72) = 126
//  3 termes, A1,2,3   : I = int(128*U) = int(115.2+11.52+1.152) = int(127.872) = 127
//  4 termes, A1,2,3,4 : I = int(128*U) = int(115.2+11.52+1.152+0.1152) = int(127.9872) = 127
//  ==> le digit A4 ne sert pas dans la determination de "I"
//  On prend un digit de + pour avoir de la marge -> A5 cad le dernier digit de int(U*10^5)
//--------
{
 while(1) {
   double U;
   // -- choix de l'intervalle et de l'abscisse "x"
   int I = N_ZIGGURRAT_GAUSS128;
   while(I>=N_ZIGGURRAT_GAUSS128) {
     U = Next();
     I = int(N_ZIGGURRAT_GAUSS128*U);
   }

   // -- choix du signe (cf blabla ci-dessus)
   double s = ( (int(U*100000)&1) == 0 ) ? -1.: 1.;

   // -- choix de l'abscisse "x" dans l'intervalle
   double x = Next() * X_ZIGGURRAT_GAUSS128[I+1];

   // -- x est dans l'interieur de la bande
   if(x<X_ZIGGURRAT_GAUSS128[I]) return s * x;

   // -- x n'est pas a l'interieur de la bande mais dans la partie a 2 possibilites

   // l'intervalle est celui qui contient la queue a l'infini
   // On s'assure que la partie "rejection sur la gaussienne" ne sera pas appelle
   //   (cad que slim=1. < X[127]) pour eviter les recursions infinies (possibles?)
   if(I==N_ZIGGURRAT_GAUSS128-1)  // cas de la bande de la queue I=127
     return s * GaussianTail(X_ZIGGURRAT_GAUSS128[N_ZIGGURRAT_GAUSS128-1],1.);

   // on tire "y" uniforme a l'interieur des ordonnees de la bande choisie
   // et on regarde si on est en-dessous ou au-dessus de la pdf
   double y = Y_ZIGGURRAT_GAUSS128[I+1]
            + Next()*(Y_ZIGGURRAT_GAUSS128[I]-Y_ZIGGURRAT_GAUSS128[I+1]);
   double pdf = exp(-0.5*x*x);
   if(pdf>y) return s * x;

   // echec, on est au-dessus de la pdf -> on re-essaye
 }
}

r_8 RandomGeneratorInterface::GaussianTail(double s,double slim)
{
  /* Returns a gaussian random variable larger than a
   * This implementation does one-sided upper-tailed deviates.
   */
  if(s < slim) {
    /* For small s, use a direct rejection method. The limit s < 1
       can be adjusted to optimise the overall efficiency */
    double x;
    do {x = Gaussian();} while (x < s);
    return x;
  } else {
    /* Use the "supertail" deviates from the last two steps
     * of Marsaglia's rectangle-wedge-tail method, as described
     * in Knuth, v2, 3rd ed, pp 123-128.  (See also exercise 11, p139,
     * and the solution, p586.)
     */
    double u, v, x;
    do {u = Next();
      do {v = Next();} while (v == 0.0);
      x = sqrt (s * s - 2 * log (v));
    } while (x * u > s);
    return x;
  }
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

uint_8 RandomGeneratorInterface::Poisson(double mu, double mumax)
{
  switch (usepoisson_) {
    case C_Poisson_Simple :
      return PoissonSimple(mu,mumax);
      break;
    case C_Poisson_Ahrens :
      return PoissonAhrens(mu);
      break;
    default:
      return PoissonSimple(mu,mumax);
      break;
  }
}


//--- Generation de nombre aleatoires suivant une distribution de Poisson 
uint_8 RandomGeneratorInterface::PoissonSimple(double mu,double mumax)
{
  double pp,ppi,x;

  if((mumax>0.)&&(mu>=mumax)) {
    pp = sqrt(mu);
    while( (x=pp*Gaussian()) < -mu );
    return (uint_8)(mu+x+0.5);
  }
  else {
    uint_8 n;
    ppi = pp = exp(-mu);
    x = Next();
    n = 0;
    while (x > ppi) {
      n++;
      pp = mu*pp/(double)n;
      ppi += pp;
    }
    return n;
  }
  return 0;  // pas necessaire ?
}


static double a0_poiahr = -0.5;
static double a1_poiahr = 0.3333333;
static double a2_poiahr = -0.2500068;
static double a3_poiahr = 0.2000118;
static double a4_poiahr = -0.1661269;
static double a5_poiahr = 0.1421878;
static double a6_poiahr = -0.1384794;
static double a7_poiahr = 0.125006;
static double fact_poiahr[10] = {
    1.0,1.0,2.0,6.0,24.0,120.0,720.0,5040.0,40320.0,362880.0};
uint_8 RandomGeneratorInterface::PoissonAhrens(double mu)
/*
**********************************************************************
     long ignpoi(float mu)
                    GENerate POIsson random deviate
                              Function
     Generates a single random deviate from a Poisson
     distribution with mean AV.
                              Arguments
     av --> The mean of the Poisson distribution from which
            a random deviate is to be generated.
     genexp <-- The random deviate.
                              Method
     Renames KPOIS from TOMS as slightly modified by BWB to use RANF
     instead of SUNIF.
     For details see:
               Ahrens, J.H. and Dieter, U.
               Computer Generation of Poisson Deviates
               From Modified Normal Distributions.
               ACM Trans. Math. Software, 8, 2
               (June 1982),163-179
**********************************************************************
**********************************************************************
                                                                      
                                                                      
     P O I S S O N  DISTRIBUTION                                      
                                                                      
                                                                      
**********************************************************************
**********************************************************************
                                                                      
     FOR DETAILS SEE:                                                 
                                                                      
               AHRENS, J.H. AND DIETER, U.                            
               COMPUTER GENERATION OF POISSON DEVIATES                
               FROM MODIFIED NORMAL DISTRIBUTIONS.                    
               ACM TRANS. MATH. SOFTWARE, 8,2 (JUNE 1982), 163 - 179. 
                                                                      
     (SLIGHTLY MODIFIED VERSION OF THE PROGRAM IN THE ABOVE ARTICLE)  
                                                                      
**********************************************************************
      INTEGER FUNCTION IGNPOI(IR,MU)
     INPUT:  IR=CURRENT STATE OF BASIC RANDOM NUMBER GENERATOR
             MU=MEAN MU OF THE POISSON DISTRIBUTION
     OUTPUT: IGNPOI=SAMPLE FROM THE POISSON-(MU)-DISTRIBUTION
     MUPREV=PREVIOUS MU, MUOLD=MU AT LAST EXECUTION OF STEP P OR B.
     TABLES: COEFFICIENTS A0-A7 FOR STEP F. FACTORIALS FACT
     COEFFICIENTS A(K) - FOR PX = FK*V*V*SUM(A(K)*V**K)-DEL
     SEPARATION OF CASES A AND B
*/
{
uint_8 ignpoi,j,k,kflag,l,m;
double b1,b2,c,c0,c1,c2,c3,d,del,difmuk,e,fk,fx,fy,g,omega,p,p0,px,py,q,s,
    t,u,v,x,xx,pp[35];

    if(mu < 10.0) goto S120;
/*
     C A S E  A. (RECALCULATION OF S,D,L IF MU HAS CHANGED)
*/
    s = sqrt(mu);
    d = 6.0*mu*mu;
/*
             THE POISSON PROBABILITIES PK EXCEED THE DISCRETE NORMAL
             PROBABILITIES FK WHENEVER K >= M(MU). L=IFIX(MU-1.1484)
             IS AN UPPER BOUND TO M(MU) FOR ALL MU >= 10 .
*/
    l = (uint_8) (mu-1.1484);
/*
     STEP N. NORMAL SAMPLE - SNORM(IR) FOR STANDARD NORMAL DEVIATE
*/
    g = mu+s*Gaussian();
    if(g < 0.0) goto S20;
    ignpoi = (uint_8) (g);
/*
     STEP I. IMMEDIATE ACCEPTANCE IF IGNPOI IS LARGE ENOUGH
*/
    if(ignpoi >= l) return ignpoi;
/*
     STEP S. SQUEEZE ACCEPTANCE - SUNIF(IR) FOR (0,1)-SAMPLE U
*/
    fk = (double)ignpoi;
    difmuk = mu-fk;
    u = Next();
    if(d*u >= difmuk*difmuk*difmuk) return ignpoi;
S20:
/*
     STEP P. PREPARATIONS FOR STEPS Q AND H.
             (RECALCULATIONS OF PARAMETERS IF NECESSARY)
             .3989423=(2*PI)**(-.5)  .416667E-1=1./24.  .1428571=1./7.
             THE QUANTITIES B1, B2, C3, C2, C1, C0 ARE FOR THE HERMITE
             APPROXIMATIONS TO THE DISCRETE NORMAL PROBABILITIES FK.
             C=.1069/MU GUARANTEES MAJORIZATION BY THE 'HAT'-FUNCTION.
*/
    omega = 0.3989423/s;
    b1 = 4.166667E-2/mu;
    b2 = 0.3*b1*b1;
    c3 = 0.1428571*b1*b2;
    c2 = b2-15.0*c3;
    c1 = b1-6.0*b2+45.0*c3;
    c0 = 1.0-b1+3.0*b2-15.0*c3;
    c = 0.1069/mu;
    if(g < 0.0) goto S50;
/*
             'SUBROUTINE' F IS CALLED (KFLAG=0 FOR CORRECT RETURN)
*/
    kflag = 0;
    goto S70;
S40:
/*
     STEP Q. QUOTIENT ACCEPTANCE (RARE CASE)
*/
    if(fy-u*fy <= py*exp(px-fx)) return ignpoi;
S50:
/*
     STEP E. EXPONENTIAL SAMPLE - SEXPO(IR) FOR STANDARD EXPONENTIAL
             DEVIATE E AND SAMPLE T FROM THE LAPLACE 'HAT'
             (IF T <= -.6744 THEN PK < FK FOR ALL MU >= 10.)
*/
    e = Exponential();
    u = Next();
    u += (u-1.0);
    //t = 1.8+fsign(e,u);
    t = 1.8 + (((u>0. && e<0.) || (u<0. && e>0.))?-e:e);
    if(t <= -0.6744) goto S50;
    ignpoi = (uint_8) (mu+s*t);
    fk = (double)ignpoi;
    difmuk = mu-fk;
/*
             'SUBROUTINE' F IS CALLED (KFLAG=1 FOR CORRECT RETURN)
*/
    kflag = 1;
    goto S70;
S60:
/*
     STEP H. HAT ACCEPTANCE (E IS REPEATED ON REJECTION)
*/
    if(c*fabs(u) > py*exp(px+e)-fy*exp(fx+e)) goto S50;
    return ignpoi;
S70:
/*
     STEP F. 'SUBROUTINE' F. CALCULATION OF PX,PY,FX,FY.
             CASE IGNPOI .LT. 10 USES FACTORIALS FROM TABLE FACT
*/
    if(ignpoi >= 10) goto S80;
    px = -mu;
    py = pow(mu,(double)ignpoi)/ *(fact_poiahr+ignpoi);
    goto S110;
S80:
/*
             CASE IGNPOI .GE. 10 USES POLYNOMIAL APPROXIMATION
             A0-A7 FOR ACCURACY WHEN ADVISABLE
             .8333333E-1=1./12.  .3989423=(2*PI)**(-.5)
*/
    del = 8.333333E-2/fk;
    del -= (4.8*del*del*del);
    v = difmuk/fk;
    if(fabs(v) <= 0.25) goto S90;
    px = fk*log(1.0+v)-difmuk-del;
    goto S100;
S90:
    px = fk*v*v*(((((((a7_poiahr*v+a6_poiahr)*v+a5_poiahr)*v+a4_poiahr)*v+a3_poiahr)*v+a2_poiahr)*v+a1_poiahr)*v+a0_poiahr)-del;
S100:
    py = 0.3989423/sqrt(fk);
S110:
    x = (0.5-difmuk)/s;
    xx = x*x;
    fx = -0.5*xx;
    fy = omega*(((c3*xx+c2)*xx+c1)*xx+c0);
    if(kflag <= 0) goto S40;
    goto S60;
S120:
/*
     C A S E  B. (START NEW TABLE AND CALCULATE P0 IF NECESSARY)
*/
//  m = max(1L,(long) (mu));
    m = (1ULL >= (uint_8)mu) ? 1ULL: (uint_8)mu;

    l = 0;
    p = exp(-mu);
    q = p0 = p;
S130:
/*
     STEP U. UNIFORM SAMPLE FOR INVERSION METHOD
*/
    u = Next();
    ignpoi = 0;
    if(u <= p0) return ignpoi;
/*
     STEP T. TABLE COMPARISON UNTIL THE END PP(L) OF THE
             PP-TABLE OF CUMULATIVE POISSON PROBABILITIES
             (0.458=PP(9) FOR MU=10)
*/
    if(l == 0) goto S150;
    j = 1;
//if(u > 0.458) j = min(l,m);
    if(u > 0.458) j = ((l<=m)? l: m);
    for(k=j; k<=l; k++) {
        if(u <= *(pp+k-1)) goto S180;
    }
    if(l == 35) goto S130;
S150:
/*
     STEP C. CREATION OF NEW POISSON PROBABILITIES P
             AND THEIR CUMULATIVES Q=PP(K)
*/
    l += 1;
    for(k=l; k<=35; k++) {
        p = p*mu/(double)k;
        q += p;
        *(pp+k-1) = q;
        if(u <= q) goto S170;
    }
    l = 35;
    goto S130;
S170:
    l = k;
S180:
    ignpoi = k;
    return ignpoi;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

r_8 RandomGeneratorInterface::Exponential() 
{
  switch (useexpo_) {
    case C_Exponential_Simple :
      return ExpoSimple();
      break;
    case C_Exponential_Ahrens :
      return ExpoAhrens();
      break;
    default:
      return ExpoSimple();
      break;
  }
}

r_8 RandomGeneratorInterface::ExpoSimple(void)
{
  return -log(1.-Next());
}


static double q_expo[8] = {
    0.6931472,0.9333737,0.9888778,0.9984959,0.9998293,0.9999833,0.9999986,1.0};
r_8 RandomGeneratorInterface::ExpoAhrens(void)
/*
**********************************************************************
**********************************************************************
     (STANDARD-)  E X P O N E N T I A L   DISTRIBUTION                
**********************************************************************
**********************************************************************
                                                                      
     FOR DETAILS SEE:                                                 
                                                                      
               AHRENS, J.H. AND DIETER, U.                            
               COMPUTER METHODS FOR SAMPLING FROM THE                 
               EXPONENTIAL AND NORMAL DISTRIBUTIONS.                  
               COMM. ACM, 15,10 (OCT. 1972), 873 - 882.               
                                                                      
     ALL STATEMENT NUMBERS CORRESPOND TO THE STEPS OF ALGORITHM       
     'SA' IN THE ABOVE PAPER (SLIGHTLY MODIFIED IMPLEMENTATION)       
                                                                      
     Modified by Barry W. Brown, Feb 3, 1988 to use RANF instead of   
     SUNIF.  The argument IR thus goes away.                          
                                                                      
**********************************************************************
     Q(N) = SUM(ALOG(2.0)**K/K!)    K=1,..,N ,      THE HIGHEST N
     (HERE 8) IS DETERMINED BY Q(N)=1.0 WITHIN STANDARD PRECISION
*/
{
long i;
double sexpo,a,u,ustar,umin;
double *q1 = q_expo;
    a = 0.0;
    while((u=Next())==0.);
    goto S30;
S20:
    a += *q1;
S30:
    u += u;
    if(u <= 1.0) goto S20;
    u -= 1.0;
    if(u > *q1) goto S60;
    sexpo = a+u;
    return sexpo;
S60:
    i = 1;
    ustar = Next();
    umin = ustar;
S70:
    ustar = Next();
    if(ustar < umin) umin = ustar;
    i += 1;
    if(u > *(q_expo+i-1)) goto S70;
    sexpo = a+umin**q1;
    return sexpo;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

int RandomGeneratorInterface::Gaussian2DRho(double &x,double &y,double mx,double my,double sx,double sy,double ro)
/*
++
|	Tirage de 2 nombres aleatoires x et y distribues sur une gaussienne 2D
|	de centre (mx,my), de coefficient de correlation rho (ro) et telle que
|	les sigmas finals des variables x et y soient sx,sy (ce sont
|	les valeurs des distributions marginales des variables aleatoires x et y
|	c'est a dire les sigmas des projections x et y de l'histogramme 2D
|	de la gaussienne). Retourne 0 si ok.
|
| - La densite de probabilite (normalisee a 1) sur laquelle on tire est:
|   N*exp[-0.5*{[(dx/sx)^2-2*ro/(sx*sy)*dx*dy+(dy/sy)^2]/(1-ro^2)}]
|     avec dx = x-mx, dy = y-my et N = 1/[2Pi*sx*sy*sqrt(1-ro^2)]
| - Dans ce cas la distribution marginale est (ex en X):
|   1/(sqrt(2Pi)*sx) * exp[-0.5*{dx^2/sx^2}]
| - La matrice des covariances C des variables x,y est:
|   |   sx^2      ro*sx*sy |
|   |                      |  et det(C) = (1-ro^2)*sx^2*sy^2
|   | ro*sx*sy      sy^2   |
| - La matrice inverse C^(-1) est:
|   |   1/sx^2      -ro/(sx*sy) |
|   |                           | * 1/(1-ro^2)
|   | -ro/(sx*sy)      1/sy^2   |
|
| - Remarque:
| le sigma que l'on obtient quand on fait une coupe de la gaussienne 2D
| en y=0 (ou x=0) est: SX0(y=0) = sx*sqrt(1-ro^2) different de sx
|                      SY0(x=0) = sy*sqrt(1-ro^2) different de sy
| La distribution qui correspond a des sigmas SX0,SY0
| pour les coupes en y=0,x=0 de la gaussienne 2D serait:
|   N*exp[-0.5*{ (dx/SX0)^2-2*ro/(SX0*SY0)*dx*dy+(dy/SY0)^2 }]
| avec N = sqrt(1-ro^2)/(2Pi*SX0*SY0) et les variances
| des variables x,y sont toujours
|  sx=SX0/sqrt(1-ro^2), sy=SY0/sqrt(1-ro^2)
--
*/
{
double a,b,sa;

if( ro <= -1. || ro >= 1. ) return 1;

while( (b=Flat01()) == 0. );
b = sqrt(-2.*log(b));
a = 2.*M_PI * Flat01();
sa = sin(a);

x = mx + sx*b*(sqrt(1.-ro*ro)*cos(a)+ro*sa);
y = my + sy*b*sa;

return 0;
}

void RandomGeneratorInterface::Gaussian2DAng(double &x,double &y,double mx,double my,double sa,double sb,double teta)
/*
++
|	Tirage de 2 nombres aleatoires x et y distribues sur une gaussienne 2D
|	de centre (x=mx,y=my), de sigmas grand axe et petit axe (sa,sb)
|	et dont le grand axe fait un angle teta (radian) avec l'axe des x.
|
| - La densite de probabilite (normalisee a 1) sur laquelle on tire est:
| N*exp[-0.5*{ (A/sa)**2+(C/sc)**2 }],  N=1/(2Pi*sa*sc)
| ou A et B sont les coordonnees selon le grand axe et le petit axe
| et teta = angle(x,A), le resultat subit ensuite une rotation d'angle teta.
| - La matrice des covariances C des variables A,B est:
|   | sa^2   0   |
|   |            |  et det(C) = (1-ro^2)*sa^2*sb^2
|   |  0    sb^2 |
| - La distribution x,y resultante est:
| N*exp[-0.5*{[(dx/sx)^2-2*ro/(sx*sy)*dx*dy+(dy/sy)^2]/(1-ro^2)}]
| ou N est donne dans NormCo et sx,sy,ro sont calcules a partir
| de sa,sc,teta (voir fonctions paramga ou gaparam). La matrice des
| covariances des variables x,y est donnee dans la fonction NormCo.
--
*/
{
double c,s,X,Y;

while( (s = Flat01()) == 0. );
s = sqrt(-2.*log(s));
c = 2.*M_PI * Flat01();

X = sa*s*cos(c);
Y = sb*s*sin(c);

c = cos(teta); s = sin(teta);
x = mx + c*X - s*Y;
y = my + s*X + c*Y;
}

}  /* namespace SOPHYA */



/////////////////////////////////////////////////////////////////
/*
**** Remarques sur complex< r_8 > ComplexGaussian(double sig) ****

--- variables gaussiennes x,y independantes
x gaussien: pdf f(x) = 1/(sqrt(2Pi) Sx) exp(-(x-Mx)^2/(2 Sx^2))
y gaussien: pdf f(y) = 1/(sqrt(2Pi) Sy) exp(-(y-My)^2/(2 Sy^2))
x,y independants --> pdf f(x,y) = f(x) f(y)
On a:
  <x>   = Integrate[x*f(x)]   = Mx
  <x^2> = Integrate[x^2*f(x)] = Mx^2 + Sx^2

--- On cherche la pdf g(r,t) du module et de la phase
  x = r cos(t) ,  y = r sin(t)
  r=sqrt(x^2+y^2 , t=atan2(y,x)
  (r,t) --> (x,y): le Jacobien = r

  g(r,t) = r f(x,y) = r f(x) f(y)
         = r/(2Pi Sx Sy) exp(-(x-Mx)^2/(2 Sx^2)) exp(-(y-My)^2/(2 Sy^2))

- Le cas general est complique
  (cf D.Pelat cours DEA "bruits et signaux" section 4.5)

- Cas ou "Mx = My = 0" et "Sx = Sy = S"
  c'est la pdf du module et de la phase d'un nombre complexe
     dont les parties reelles et imaginaires sont independantes
     et sont distribuees selon des gaussiennes de variance S^2
  g(r,t) = r/(2Pi S^2) exp(-r^2/(2 S^2))
  La distribution de "r" est donc:
    g(r) = Integrate[g(r,t),{t,0,2Pi}]
         = r/S^2 exp(-r^2/(2 S^2))
  La distribution de "t" est donc:
    g(t) = Integrate[g(r,t),{r,0,Infinity}]
         = 1 / 2Pi  (distribution uniforme sur [0,2Pi[)
  Les variables aleatoires r,t sont independantes:
    g(r,t) = g(r) g(t)
On a:
  <r>   = Integrate[r*g(r)]   = sqrt(PI/2)*S
  <r^2> = Integrate[r^2*g(r)] = 2*S^2
  <r^3> = Integrate[r^3*g(r)] = 3*sqrt(PI/2)*S^3
  <r^4> = Integrate[r^4*g(r)] = 8*S^4

- Attention:
La variable complexe "c = x+iy = r*exp(i*t)" ainsi definie verifie:
              <|c|^2> = <c c*> = <x^2+y^2> = <r^2> = 2 S^2
Si on veut generer une variable complexe gaussienne telle que
     <c c*> = s^2 alors il faut prendre S = s/sqrt(2) comme argument

*/
