// this :
#include <HEPVis/nodes/SoTextVtk.h>

//////////////////////////////////////////////////////////////////////////////
//
// Inventor version of the vtk text primitive.
// Code extracted from vtk-2.0/vtk/graphics/vtkVectorText.cxx
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoTextVtk.ic>

#include <string.h>
#include <math.h>
#include <stdlib.h>

#define HEIGHT 1.4F

static float sX = 0.;
static float sY = 0.;

SO_NODE_SOURCE(SoTextVtk)
//////////////////////////////////////////////////////////////////////////////
void SoTextVtk::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_INIT_CLASS(SoTextVtk,SoShape,"Shape");
}
//////////////////////////////////////////////////////////////////////////////
SoTextVtk::SoTextVtk(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoTextVtk);
  SO_NODE_ADD_FIELD(string,(""));
  //
  fLetters[33] = VTK_VECTOR_TEXT_33;
  fLetters[34] = VTK_VECTOR_TEXT_34;
  fLetters[35] = VTK_VECTOR_TEXT_35;
  fLetters[36] = VTK_VECTOR_TEXT_36;
  fLetters[37] = VTK_VECTOR_TEXT_37;
  fLetters[38] = VTK_VECTOR_TEXT_38;
  fLetters[39] = VTK_VECTOR_TEXT_39;
  fLetters[40] = VTK_VECTOR_TEXT_40;
  fLetters[41] = VTK_VECTOR_TEXT_41;
  fLetters[42] = VTK_VECTOR_TEXT_42;
  fLetters[43] = VTK_VECTOR_TEXT_43;
  fLetters[44] = VTK_VECTOR_TEXT_44;
  fLetters[45] = VTK_VECTOR_TEXT_45;
  fLetters[46] = VTK_VECTOR_TEXT_46;
  fLetters[47] = VTK_VECTOR_TEXT_47;
  fLetters[48] = VTK_VECTOR_TEXT_48;
  fLetters[49] = VTK_VECTOR_TEXT_49;
  fLetters[50] = VTK_VECTOR_TEXT_50;
  fLetters[51] = VTK_VECTOR_TEXT_51;
  fLetters[52] = VTK_VECTOR_TEXT_52;
  fLetters[53] = VTK_VECTOR_TEXT_53;
  fLetters[54] = VTK_VECTOR_TEXT_54;
  fLetters[55] = VTK_VECTOR_TEXT_55;
  fLetters[56] = VTK_VECTOR_TEXT_56;
  fLetters[57] = VTK_VECTOR_TEXT_57;
  fLetters[58] = VTK_VECTOR_TEXT_58;
  fLetters[59] = VTK_VECTOR_TEXT_59;
  fLetters[60] = VTK_VECTOR_TEXT_60;
  fLetters[61] = VTK_VECTOR_TEXT_61;
  fLetters[62] = VTK_VECTOR_TEXT_62;
  fLetters[63] = VTK_VECTOR_TEXT_63;
  fLetters[64] = VTK_VECTOR_TEXT_64;
  fLetters[65] = VTK_VECTOR_TEXT_65;
  fLetters[66] = VTK_VECTOR_TEXT_66;
  fLetters[67] = VTK_VECTOR_TEXT_67;
  fLetters[68] = VTK_VECTOR_TEXT_68;
  fLetters[69] = VTK_VECTOR_TEXT_69;
  fLetters[70] = VTK_VECTOR_TEXT_70;
  fLetters[71] = VTK_VECTOR_TEXT_71;
  fLetters[72] = VTK_VECTOR_TEXT_72;
  fLetters[73] = VTK_VECTOR_TEXT_73;
  fLetters[74] = VTK_VECTOR_TEXT_74;
  fLetters[75] = VTK_VECTOR_TEXT_75;
  fLetters[76] = VTK_VECTOR_TEXT_76;
  fLetters[77] = VTK_VECTOR_TEXT_77;
  fLetters[78] = VTK_VECTOR_TEXT_78;
  fLetters[79] = VTK_VECTOR_TEXT_79;
  fLetters[80] = VTK_VECTOR_TEXT_80;
  fLetters[81] = VTK_VECTOR_TEXT_81;
  fLetters[82] = VTK_VECTOR_TEXT_82;
  fLetters[83] = VTK_VECTOR_TEXT_83;
  fLetters[84] = VTK_VECTOR_TEXT_84;
  fLetters[85] = VTK_VECTOR_TEXT_85;
  fLetters[86] = VTK_VECTOR_TEXT_86;
  fLetters[87] = VTK_VECTOR_TEXT_87;
  fLetters[88] = VTK_VECTOR_TEXT_88;
  fLetters[89] = VTK_VECTOR_TEXT_89;
  fLetters[90] = VTK_VECTOR_TEXT_90;
  fLetters[91] = VTK_VECTOR_TEXT_91;
  fLetters[92] = VTK_VECTOR_TEXT_92;
  fLetters[93] = VTK_VECTOR_TEXT_93;
  fLetters[94] = VTK_VECTOR_TEXT_94;
  fLetters[95] = VTK_VECTOR_TEXT_95;
  fLetters[96] = VTK_VECTOR_TEXT_96;
  fLetters[97] = VTK_VECTOR_TEXT_97;
  fLetters[98] = VTK_VECTOR_TEXT_98;
  fLetters[99] = VTK_VECTOR_TEXT_99;
  fLetters[100] = VTK_VECTOR_TEXT_100;
  fLetters[101] = VTK_VECTOR_TEXT_101;
  fLetters[102] = VTK_VECTOR_TEXT_102;
  fLetters[103] = VTK_VECTOR_TEXT_103;
  fLetters[104] = VTK_VECTOR_TEXT_104;
  fLetters[105] = VTK_VECTOR_TEXT_105;
  fLetters[106] = VTK_VECTOR_TEXT_106;
  fLetters[107] = VTK_VECTOR_TEXT_107;
  fLetters[108] = VTK_VECTOR_TEXT_108;
  fLetters[109] = VTK_VECTOR_TEXT_109;
  fLetters[110] = VTK_VECTOR_TEXT_110;
  fLetters[111] = VTK_VECTOR_TEXT_111;
  fLetters[112] = VTK_VECTOR_TEXT_112;
  fLetters[113] = VTK_VECTOR_TEXT_113;
  fLetters[114] = VTK_VECTOR_TEXT_114;
  fLetters[115] = VTK_VECTOR_TEXT_115;
  fLetters[116] = VTK_VECTOR_TEXT_116;
  fLetters[117] = VTK_VECTOR_TEXT_117;
  fLetters[118] = VTK_VECTOR_TEXT_118;
  fLetters[119] = VTK_VECTOR_TEXT_119;
  fLetters[120] = VTK_VECTOR_TEXT_120;
  fLetters[121] = VTK_VECTOR_TEXT_121;
  fLetters[122] = VTK_VECTOR_TEXT_122;
  fLetters[123] = VTK_VECTOR_TEXT_123;
  fLetters[124] = VTK_VECTOR_TEXT_124;
  fLetters[125] = VTK_VECTOR_TEXT_125;
  fLetters[126] = VTK_VECTOR_TEXT_126;
}
//////////////////////////////////////////////////////////////////////////////
SoTextVtk::~SoTextVtk (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
void SoTextVtk::generatePrimitives(
 SoAction* aAction
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int linen = string.getNum();
  const SbString* lines = string.getValues(0);
  sX = 0.;
  sY = 0.;
  for(int count=0;count<linen;count++) {
    renderString(aAction,lines[count].getString());
    sY -= HEIGHT;
    sX = 0.;
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoTextVtk::GLRender (
 SoGLRenderAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoShape::GLRender(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoTextVtk::rayPick(
 SoRayPickAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoShape::rayPick(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoTextVtk::pick(
 SoPickAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoShape::pick(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoTextVtk::computeBBox (
 SoAction* aAction
,SbBox3f& aBox
,SbVec3f& aCenter
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int linen = string.getNum();
  const SbString* lines = string.getValues(0);
  float width = 0.;
  sX = 0.;
  sY = 0.;
  for(int count=0;count<linen;count++) {
    renderString(aAction,lines[count].getString());
    width = SbMaximum(width,sX);
    sY -= HEIGHT;
    sX = 0.;
  }
  //printf("debug : SoTextVtk::computeBBox : %d %g\n",linen,width);
  //
  SbVec3f mn(0.,-(linen-1) * HEIGHT,0.);
  SbVec3f mx(width,HEIGHT,0.01F);
  aBox.setBounds(mn,mx);
  aCenter = aBox.getCenter();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoTextVtk::renderString(
 SoAction* aAction                      
,const char* aString 
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if( (aString==NULL) || (*aString=='\0') ) return;
  for(const char* itema=aString;*itema!='\0';itema++) { 
    renderCharacter(aAction,*itema);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoTextVtk::renderCharacter(
 SoAction* aAction                      
,char aChar 
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aChar==32) {
    sX += 0.4F;
    return;
  }

  // if we have a valid character
  if ((aChar <= 32) || (aChar >= 127)) return;

  //printf("debug : SoTextVtk::renderCharacter : %c\n",aChar);

  // add the result to our output
  const char* Letter = fLetters[(int)aChar];
  char* p = (char*)Letter;
  int ptCount = strtol(p,&p,10);
  float width = (float)strtod(p,&p);

  if(aAction->isOfType(SoGetBoundingBoxAction::getClassTypeId())==FALSE) {

    SoPrimitiveVertex pv;
    SoState* state = aAction->getState();
    SbBool useTexFunction =
      (SoTextureCoordinateElement::getType(state) == 
       SoTextureCoordinateElement::FUNCTION);
    const SoTextureCoordinateElement* tce = NULL;
    SbVec4f texCoord;
    if (useTexFunction) {
      tce = SoTextureCoordinateElement::getInstance(state);
    }
    else {
      texCoord[2] = 0.0;
      texCoord[3] = 1.0;
    }
    
    SbVec3f normal;
    //////////////////////////////////////////
    //----------------------------------------
#define GEN_VERTEX(pv,p,s,t,nx,ny,nz)        \
    normal.setValue(nx,ny,nz);               \
    if (useTexFunction) {                    \
      texCoord=tce->get(p,normal);           \
    } else {                                 \
      texCoord[0]=s;                         \
      texCoord[1]=t;                         \
    }                                        \
    pv.setPoint(p);                          \
    pv.setNormal(normal);                    \
    pv.setTextureCoords(texCoord);           \
    shapeVertex(&pv);
    //----------------------------------------
    //////////////////////////////////////////

    int i;
    float x,y;
    SbVec3f* vecs = new SbVec3f[ptCount];
    for (i = 0; i < ptCount; i++) {
      x = (float)strtod(p,&p);
      y = (float)strtod(p,&p);
      vecs[i].setValue(x+sX,y+sY,0.);
    }
    int triCount = strtol(p,&p,10);
    int j;
    for (i = 0; i < triCount; i++) {
      beginShape(aAction,TRIANGLES);
      j = strtol(p,&p,10);
      GEN_VERTEX(pv,vecs[j], 0.0,0.0, 0.0,0.0,1.0);   
      j = strtol(p,&p,10);
      GEN_VERTEX(pv,vecs[j], 0.0,0.0, 0.0,0.0,1.0);   
      j = strtol(p,&p,10);
      GEN_VERTEX(pv,vecs[j], 0.0,0.0, 0.0,0.0,1.0);   
      endShape();
    }
    delete [] vecs;
  }
  sX += width;
}








