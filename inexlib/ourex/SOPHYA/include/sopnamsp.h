#ifndef SOPNAMSP_SEEN
#define SOPNAMSP_SEEN

#ifdef __cplusplus

/*  SOPHYA classes are in namespace SOPHYA:
 sopnamsp.h :
   1-/ DOIT etre mis dans les .cc de sophya
   2-/ NE DOIT JAMAIS etre mis dans un .h de SOPHYA
       disponible pour les utilisateur exterieurs
 Dans un main programme:
   1-/ Si on met sopnamsp.h les classes de SOPHYA
       doivent etre instanciees: TArray<r_8>
   2-/ Si on NE met PAS sopnamsp.h les classes de SOPHYA
       doivent etre instanciees: SOPHYA::TArray<r_8>

*/
namespace SOPHYA {}
using namespace SOPHYA;

#endif

#endif
