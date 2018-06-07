#ifndef INCLUDE_ParticleChar
#define INCLUDE_ParticleChar
// ParticleChar.h
// This is a first stab at a particle characteristics database
#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif
#include <functional>
#include <string>
#include <map>


#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/SbBasic.h>

typedef  std::less<std::string> less_s;
class  ParticleChar;
typedef  std::map<std::string, ParticleChar *, less_s> CMapToParticleList;

/*!  \class ParticleChar ParticleChar.h HEPVis/ParticleChar.h
 *   \brief Provides a mini-database for particle characteristics
 *
 *   This class maintains a small database of standard particle names,
 *   masses, charges, Monte Carlo IDs (PYTHIA, ISAJET, GEANT), as well
 *   as the OpenInventor characteristics which could be used to represent
 *   pure particle tracks: material and line type.
 *
 *   Each ParticleChar maintains both a name and a short tag suitable for use
 *   in a display.
 *
 * 
 *   The standard particles loaded into the database are:
 *   unknown, antiunknown, electron, positron, muon, antimuon, proton, antiproton,
 *   neutron, antineutron, pion, antipion, deuteron, tritium, alpha, and he3.
 *   Particles are taken to have negative charge, anti-particles have positive.
 *   Neutrals haven't been included yet.
 *
 */
class ParticleChar {
public:
//! accessor for GEANT code 
    int getGeantCode() const {return geantCode;}
//! accessor for the SoMaterial used to represent the particle
    SoMaterial *getMaterial() const{return particleMaterial;}
//! accessor for the SoDrawStyle used to represent the particle
    SoDrawStyle *getStyle() const {return particleStyle;}
//! constructor
    ParticleChar(int geantCode, int pythiaCode, int isaCode, 
                 const char *name, const char *label, 
                 float mass, float charge);
//! set the SoMaterial used to represent the particle
    void setMaterial(SoMaterial *particleMaterial);
//! set the SoDrawStyle used to represent the particle
    void setStyle(SoDrawStyle *particleStyle);
//! accessor for the name of the particle
    const char *getName() const;
//! accessor for the mass of the particle (GeV/c^2)
    float getMass() const;
//! accessor for the charge of the particle
    float getCharge() const;
//! method to initialize the database with the standard particle set
    static void initParticles();
//! method to remove a particle from the database
    static void deleteParticles();
//! method to look up a ParticleChar by name
    static ParticleChar *getByName(const char *string);
//! method to look up a ParticleChar by GEANT id
    static ParticleChar *getByGeantID(const int p_code);
//! method to look up a ParticleChar by PYTHIA id
    static ParticleChar *getByPythiaID(const int p_code);
//! method to look up a ParticleChar by ISAJET id
    static ParticleChar *getByIsajetID(const int p_code);
private:
    const int geantCode;
    const int pythiaCode;
    const int isajetCode; 
    SoMaterial *particleMaterial;
    SoDrawStyle *particleStyle;
    const SbString name;
    const SbString label;
    static CMapToParticleList *particleList;
    const float mass;
    const float charge;
    static SbBool inited; 
protected:
    virtual ~ParticleChar();
};

#ifdef _WIN32
#include <SoWinEnterScope.h>
#endif
        
#endif
