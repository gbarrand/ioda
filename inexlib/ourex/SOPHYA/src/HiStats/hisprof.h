#ifndef HISPROF_SEEN
#define HISPROF_SEEN

#include <stdio.h>
#include "peida.h"
#include "tvector.h"
#include "ppersist.h"
#include "histos.h"

namespace SOPHYA {

template <class T>  class FitsHandler;

//! 1 dimension profile histograms
class HProf : public Histo {
  friend class ObjFileIO<HProf>;
  friend class FitsHandler<Histo>;
public:

  // CREATOR / DESTRUCTOR
  HProf();
  HProf(r_8 xMin, r_8 xMax, int_4 nBin=100, r_8 yMin=1., r_8 yMax=-1.);
  HProf(r_4 xMin, r_4 xMax, int_4 nBin=100, r_4 yMin=1., r_4 yMax=-1.);
  HProf(const HProf& H);
  virtual ~HProf();

  // UPDATING or SETTING
  virtual void UpdateHisto(bool force=false) const;

  void SetErrOpt(bool spread = true);
  void Zero();
  void Add(r_8 x, r_8 y, r_8 w = 1.);
  void AddBin(int_4 numBin, r_8 y, r_8 w = 1.);
  
  // Acces a l information
  //! Retourne l'histogramme de profil.
  inline Histo GetHisto()
               {UpdateHisto(); return (Histo) *this;}
  //! Retourne le contenu de la moyenne dans le vecteur v
  inline void GetValue(TVector<r_8>& v)
              {UpdateHisto(); Histo::GetValue(v);}
  //! Retourne le contenu au carre de la dispersion/erreur dans le vecteur v
  inline void GetError2(TVector<r_8>& v)
              {UpdateHisto(); Histo::GetError2(v);}
  //! Retourne le contenu au carre de la dispersion/erreur dans le vecteur v
  inline void GetError(TVector<r_8>& v)
              {UpdateHisto(); Histo::GetError(v);}
  //! Retourne le contenu du bin i
  inline r_8 operator()(int_4 i) const
               {UpdateHisto(); return mData[i];}
  //! Retourne le carre de la dispersion/erreur du bin i
  inline r_8 Error2(int_4 i) const
                {UpdateHisto(); return mErr2[i];}
  //! Retourne la dispersion/erreur du bin i
  inline r_8 Error(int_4 i) const
               {UpdateHisto();
                return (mErr2[i]>0.) ? sqrt(mErr2[i]) : 0.;}
  //! Retourne le carre de la dispersion/erreur du bin i
  inline r_8 SumW(int_4 i) const {return mSumW[i];}

  // Operators
  HProf& operator = (const HProf& h);
  HProf& operator += (const HProf& a);

  // Info, statistique et calculs sur les histogrammes
  virtual void HRebin(int_4 nbinew);

  virtual void Show(ostream& os) const;
  inline  void Show() const { Show(cout); }
  //! Print, voir detail dans Histo::Print
  inline void Print(int_4 dyn=100,r_8 hmin=1.,r_8 hmax=-1.
                   ,int_4 pflag=0,int_4 il=1,int_4 ih=-1)
         {UpdateHisto(); Histo::Print(dyn,hmin,hmax,pflag,il,ih);}

protected:
  void CreateOrResize(r_8 xMin, r_8 xMax, int_4 nBin, r_8 yMin=1., r_8 yMax=-1.);
  void Delete();
  void updatehisto() const;

  r_8*         mSumY;  //!< somme
  r_8*         mSumY2; //!< somme des carres
  r_8*         mSumW;  //!< somme des poids
  mutable bool mOk;    //!< true if update fait
  r_8          mYMin;  //!< limite minimum Y pour somme
  r_8          mYMax;  //!< limite maximum Y pour somme
  uint_2       mOpt;   //!< options pour les erreurs
};

/*! Prints histogram information on stream \b s (h.Show(s)) */
inline ostream& operator << (ostream& s, HProf const & h)
  {  h.Show(s);  return(s);  }

/*! \ingroup HiStats \fn operator<<(POuttPersist&, HProf)
  \brief Persistance management */
inline POutPersist& operator << (POutPersist& os, HProf & obj)
{ ObjFileIO<HProf> fio(&obj);  fio.Write(os);  return(os); }
/*! \ingroup HiStats \fn operator>>(PInPersist&, HProf)
  \brief Persistance management */
inline PInPersist& operator >> (PInPersist& is, HProf & obj)
{ ObjFileIO<HProf> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }
// Classe pour la gestion de persistance
// ObjFileIO<HProf>

/*! \ingroup HiStats \fn operator+(const HProf&, const HProf&)
  \brief Operateur H = H1 + H2
  \warning Meme commentaire que pour l'operateur +=
*/
inline HProf operator + (const HProf& a, const HProf& b)
{
if (b.NBins()!=a.NBins()) throw SzMismatchError("HProf::operator +");
HProf c(a);
return (c += b);
}

} // Fin du namespace

#endif // HISPROF_SEEN
