/************************************* Iv2POV.c++ **************************************
* (C) 1997 Mark Lasersohn - Cow House Productions


                           Iv2POV V0.12


   This is an alpha executable and source for Irix 5.3 and the 
OpenInventor 2.0 (or better) run-time library. I reserve all rights, 
this executable and source file is the copywritten property of Cow 
House Productions and may not be packaged or resold without written 
permission.
   It is distributed from my web page (http://www.cowhouse.com). 
The source and executable may be distributed freely, as long as
it is done so freely and my copyright notices remain intact.

   Syntax:

            iv2pov [-b] file.iv [ > file.pov]

   The -b flag causes backface removal. This will save memory and
rendering time, but if there is a reflection that reveals the back of an
object, the removal will be evident.

   The mapping between Inventor's texture scheme (UV mapping) and
POV 3.0's is (at best) an estimate - don't expect a perfect fit.

   By the way - it does not convert any .rgb files used by textures into
the .tga format POV wants - you have to do that by hand.

   Iv2Pov will not translate the inventor 2D constructs, such as points,
lines, or 2D text. I have also been informed that it won't do so well
with quads or nurbs.

   To compile the source on SGI with OpenInventor, simple type:

              CC iv2pov.c++ -lInventor -o iv2pov

I don't have any other versions of Inventor, so TGS and Portable
Graphics users are on their own. This is command line utility
without a graphical user interface. Thus it should readily
compile. But if you have any problems, please feel free to
contact me at laser@cowhouse.com. I have happily tracked down
and on occasion fixed bugs found by users. I don't promise to
fix any problems but I will get back to you and I really do
want to hear your ideas, suggestions, and comments.


                                        Mark Lasersohn
                                        Cow House Productions
                                        laser@cowhouse.com
                                        http://www.cowhouse.com



*/ 
#include <Inventor/SoDB.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/SbBox.h>
#include <Inventor/SbLinear.h>

#define	DEGREES (360.0 / 6.28)

namespace HEPVis { //G.Barrand
static FILE* pov_FILE = 0; //G.Barrand

static char	surf_name[10000][20]; //G.Barrand : have static.
static float	surf_ambient_r[10000];
static float	surf_ambient_g[10000];
static float	surf_ambient_b[10000];
static float	surf_diffuse_r[10000];
static float	surf_diffuse_g[10000];
static float	surf_diffuse_b[10000];
static float	surf_specular_r[10000];
static float	surf_specular_g[10000];
static float	surf_specular_b[10000];
static float	surf_transparency[10000];
static float	surf_shininess[10000];
static int	surf_cnt;

static int	backface;

static char		texture_filename[256];
static long int	texture_index_count;

static int		vcnt = 0;
static int		nvcnt = 0;

//G.Barrand static SbVec3f		gvec[1000000];
//G.Barrand static SbVec3f		nvec[1000000];
//G.Barrand static SbVec3f		gvec2[1000000];
//G.Barrand static SbVec3f		nvec2[1000000];
static SbVec3f* gvec = 0; //G.Barrand 
static SbVec3f* nvec = 0; //G.Barrand 

//FILE		*fp1, *fp2;

static float	camera_loc_x = 0.0;
static float	camera_loc_y = 0.0;
static float	camera_loc_z = 0.0;
}
using namespace HEPVis; //G.Barrand

static const SbVec3f &transformPoint(const SbMatrix &m, const SbVec3f &p)
{
static SbVec3f	tp;

	m.multVecMatrix(p, tp);
	return tp;
}

static const SbVec3f &transformNormal(const SbMatrix &m, const SbVec3f &n)
{
static SbVec3f	tn;

	m.multDirMatrix(n, tn);
	tn.normalize();
	return tn;
}

static //G.Barrand
SoCallbackAction::Response preTextureCB(void *, SoCallbackAction *, const SoNode *nd)
{
char	*cp;

	SoTexture2 *texture = (SoTexture2 *)nd;
	strcpy(texture_filename, texture->filename.getValue().getString());
	cp = texture_filename + strlen(texture_filename);
	while((*cp != '.') && (cp > texture_filename)) cp--;
	*cp = '\0';
	return(SoCallbackAction::CONTINUE);
}

static //G.Barrand
SoCallbackAction::Response preCameraCB(void *, SoCallbackAction *, const SoNode *nd)
{
SbVec3f vec;
// SbVec3f vec_in(0, 0, -1);
SbVec3f vec_in(1.33, 0, 0);
SbVec3f vec_out;

SbMatrix matrix;
SbMat m;

	SoPerspectiveCamera *camera = (SoPerspectiveCamera *)nd;

	::fprintf(pov_FILE,"camera {\n");

	::fprintf(pov_FILE,"\tup <0.0, 1.0, 0.0>\n");
	::fprintf(pov_FILE,"\tright <%g, 0.0, 0.0>\n", camera->aspectRatio.getValue());
	::fprintf(pov_FILE,"\tdirection <0, 0, -1>\n");

	SbRotation rot = camera->orientation.getValue();
	vec_in.setValue(1.33, 0.0, 0.0);
	rot.multVec(vec_in, vec_out);
	rot.getValue(matrix);
	matrix.getValue(m);
	::fprintf(pov_FILE,"\tmatrix <");
	::fprintf(pov_FILE,"  %g, %g, %g\n", m[0][0], m[0][1], m[0][2]);
	::fprintf(pov_FILE,"\t\t, %g, %g, %g\n", m[1][0], m[1][1], m[1][2]);
	::fprintf(pov_FILE,"\t\t, %g, %g, %g\n", m[2][0], m[2][1], m[2][2]);
	::fprintf(pov_FILE,"\t\t, %g, %g, %g >\n", m[3][0], m[3][1], m[3][2]);

	vec = camera->position.getValue();
	::fprintf(pov_FILE,"\tlocation <%g, %g, %g>\n", vec[0], vec[1], vec[2]);
	camera_loc_x = vec[0];
	camera_loc_y = vec[1];
	camera_loc_z = vec[2];

	::fprintf(pov_FILE,"\tangle %g\n", camera->heightAngle.getValue() * DEGREES);

	::fprintf(pov_FILE,"}\n");
	return(SoCallbackAction::CONTINUE);
}

static //G.Barrand
SoCallbackAction::Response preLightCB(void *, SoCallbackAction *, const SoNode *nd)
{
float	x, y, z;
float	x2, y2, z2;
float	r, g, b;
float	angle, falloff;

	if(nd->isOfType(SoDirectionalLight::getClassTypeId()))
	{
		SoDirectionalLight *light = (SoDirectionalLight *)nd;
		light->direction.getValue().getValue(x, y, z);
		light->color.getValue().getValue(r, g, b);

		::fprintf(pov_FILE,"light_source {\n");
		::fprintf(pov_FILE,"\t<%g, %g, %g>\n", camera_loc_x, camera_loc_y, camera_loc_z);
		::fprintf(pov_FILE,"\tcolor rgb <%g, %g, %g>\n", r, g, b);
		::fprintf(pov_FILE,"}\n");
	}
	if(nd->isOfType(SoPointLight::getClassTypeId()))
	{
		SoPointLight *light = (SoPointLight *)nd;
		light->color.getValue().getValue(r, g, b);
		light->location.getValue().getValue(x, y, z);

		::fprintf(pov_FILE,"light_source {\n");
		::fprintf(pov_FILE,"\t<%g, %g, %g>\n", x, y, z);
		::fprintf(pov_FILE,"\tcolor rgb <%g, %g, %g>\n", r, g, b);
		::fprintf(pov_FILE,"}\n");
	}
	if(nd->isOfType(SoSpotLight::getClassTypeId()))
	{
		SoSpotLight *light = (SoSpotLight *)nd;
		light->location.getValue().getValue(x, y, z);
		light->direction.getValue().getValue(x2, y2, z2);
		light->color.getValue().getValue(r, g, b);
		angle = light->cutOffAngle.getValue() * DEGREES;
		falloff = light->dropOffRate.getValue() * DEGREES;

		::fprintf(pov_FILE,"light_source {\n");
		::fprintf(pov_FILE,"\t<%g, %g, %g>\n", x, y, z);
		::fprintf(pov_FILE,"\tcolor rgb <%g, %g, %g>\n", r, g, b);
		::fprintf(pov_FILE,"\tspotlight\n");
		::fprintf(pov_FILE,"\tpoint_at <%g, %g, %g>\n", x + x2, y + y2, z + z2);
		::fprintf(pov_FILE,"\tradius %g\n", angle);
		::fprintf(pov_FILE,"\tfalloff %g\n", falloff);
		::fprintf(pov_FILE,"\ttightness 50\n");
		::fprintf(pov_FILE,"}\n");
	}
	return(SoCallbackAction::CONTINUE);
}

static void addTriangleCB(void *, SoCallbackAction *cba,
        const SoPrimitiveVertex *v1,
        const SoPrimitiveVertex *v2,
        const SoPrimitiveVertex *v3)
{
  if(!gvec) gvec = new SbVec3f[1000000];//G.Barrand
  if(!nvec) nvec = new SbVec3f[1000000];//G.Barrand

static	int	flag = 0;

	if((vcnt < 999999) && (nvcnt < 999999))
	{
		flag = 0;
		if(backface == 1)
		{
       			const SbMatrix &mm = cba->getModelMatrix();
       			SbMatrix nm = mm.inverse().transpose();
	
			SbVec3f vec1 = transformNormal(nm, v1->getNormal());
			SbVec3f vec2 = transformNormal(nm, v2->getNormal());
			SbVec3f vec3 = transformNormal(nm, v3->getNormal());
			if((vec1[2] >= 0.0) || (vec2[2] >= 0.0) || (vec3[2] >= 0.0))
			{
				gvec[vcnt++] = v1->getPoint();
				nvec[nvcnt++] = v1->getNormal();
				gvec[vcnt++] = v2->getPoint();
				nvec[nvcnt++] = v2->getNormal();
				gvec[vcnt++] = v3->getPoint();
				nvec[nvcnt++] = v3->getNormal();
			}
		}
		else
		{
			gvec[vcnt++] = v1->getPoint();
			nvec[nvcnt++] = v1->getNormal();
			gvec[vcnt++] = v2->getPoint();
			nvec[nvcnt++] = v2->getNormal();
			gvec[vcnt++] = v3->getPoint();
			nvec[nvcnt++] = v3->getNormal();
		}
	}
	else
	{
		if(flag == 0)
		{
			::fprintf(stderr, "\n\nError: Too many triangles ( > 1000000) in a single shape\n"); 
			::fprintf(stderr, "further triangles in this shape will be ignored\n\n");
		}
		flag = 1;
	}
}

static //G.Barrand
SoCallbackAction::Response countMaterialCB(void *, SoCallbackAction *, const SoNode *)
{
static	int	cnt = 0;

	cnt++;
	::fprintf(stderr, "\r shape nodes: %d", cnt);
	return(SoCallbackAction::CONTINUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Callback used to add triangle info to scene graph being built.
//
static //G.Barrand
SoCallbackAction::Response addMaterialCB(void *, SoCallbackAction *cba, const SoNode *nd)
{
SbColor 	ambient; 
SbColor 	diffuse; 
SbColor 	specular; 
SbColor 	emission; 
float 		shininess; 
float 		transparency; 
float 		r, g, b;
char		geometry_file[256];
static int	file_counter = 0;
SbVec2f		vec2;
SbVec3f		vec3;
int		loop;
float		vx1, vy1, vz1;
float		vx2, vy2, vz2;
float		vx3, vy3, vz3;
float		nx1, ny1, nz1;
float		nx2, ny2, nz2;
float		nx3, ny3, nz3;
int		flag;
SbVec3f         translation;
SbRotation      rotation;
SbVec3f         scale;
SbRotation      scale_orientation;
SbVec2s 	size;
int 		numComps;
int 		texturing = 0;
char		node_name[1024];
int		surf_index;
int		cnt;

	cba->getMaterial(ambient, diffuse, specular, emission, shininess, transparency);

	::sprintf(geometry_file, "geo%04d", file_counter);
	::fprintf(stderr, "\r (%d) ", file_counter);
	file_counter++;


	texture_index_count = cba->getNumTextureCoordinates();
	node_name[0] = '\0';
	strcpy(node_name, nd->getName().getString());

	flag = -1;
	for(loop = 0;loop < surf_cnt;loop++)
	{
		ambient.getValue(r, g, b);
		if((r == surf_ambient_r[loop])
		&& (g == surf_ambient_g[loop])
		&& (b == surf_ambient_b[loop]))
		{
			diffuse.getValue(r, g, b);
			if((r == surf_diffuse_r[loop])
			&& (g == surf_diffuse_g[loop])
			&& (b == surf_diffuse_b[loop]))
			{
				specular.getValue(r, g, b);
				if((r == surf_specular_r[loop])
				&& (g == surf_specular_g[loop])
				&& (b == surf_specular_b[loop]))
				{
					if(transparency == surf_transparency[loop])
					{
						if(shininess == surf_shininess[loop])
						{
							if(strlen(node_name) > 0)
							{
								::fprintf(pov_FILE,"/* %s */\n", node_name);
							}
							flag = loop;
							surf_index = loop;
						}
					}
				}
			}
		}
	}
	if(flag == -1)
	{
		strcpy(surf_name[surf_cnt], geometry_file);
		surf_index = surf_cnt;

		ambient.getValue(r, g, b);
		surf_ambient_r[surf_cnt] = r;
		surf_ambient_g[surf_cnt] = g;
		surf_ambient_b[surf_cnt] = b;

		diffuse.getValue(r, g, b);
		surf_diffuse_r[surf_cnt] = r;
		surf_diffuse_g[surf_cnt] = g;
		surf_diffuse_b[surf_cnt] = b;

		specular.getValue(r, g, b);
		surf_specular_r[surf_cnt] = r;
		surf_specular_g[surf_cnt] = g;
		surf_specular_b[surf_cnt] = b;

		surf_transparency[surf_cnt] = transparency;
		surf_shininess[surf_cnt] = shininess;
		surf_cnt++;

		ambient.getValue(r, g, b);
		diffuse.getValue(r, g, b);
		specular.getValue(r, g, b);
		diffuse.getValue(r, g, b);
		if(strlen(node_name) > 0)
		{
			::fprintf(pov_FILE,"/* %s */\n", node_name);
		}

		::fprintf(pov_FILE,"#declare %s = texture {\n", surf_name[surf_index]);
		if(cba->getTextureImage(size, numComps) != NULL)
		{
			texturing = 1;
		}
		if((texture_index_count > 0) || (texturing == 1))
		{
			::fprintf(pov_FILE,"\tpigment { image_map { tga \"%s.tga\" } }\n", texture_filename);
		}
		else
		{
			diffuse.getValue(r, g, b);
			::fprintf(pov_FILE,"\tpigment { color rgbf <%g, %g, %g, %g> }\n", r, g, b, transparency);
		}
		::fprintf(pov_FILE,"\tfinish {diffuse %g ", 1.0);
		ambient.getValue(r, g, b);
		::fprintf(pov_FILE,"ambient %g ", (r + b + g) / 3.0);
		specular.getValue(r, g, b);
		::fprintf(pov_FILE,"specular %g ", (r + b + g) / 3.0);
		::fprintf(pov_FILE,"reflection %g }\n", shininess);
		::fprintf(pov_FILE,"}\n");
	}
	SoNode *node = (SoNode *)nd;

       	const SbMatrix &mm = cba->getModelMatrix();
       	mm.getTransform(translation, rotation, scale, scale_orientation);

	if(vcnt != 0)
	{
                SbVec3f* gvec2 = new SbVec3f[vcnt]; //G.Barrand
                SbVec3f* nvec2 = new SbVec3f[vcnt]; //G.Barrand

		cnt = 0;
       		SbMatrix nm = mm.inverse().transpose();
		for(loop = 0;loop < vcnt;loop++)
		{
			gvec2[loop] = transformPoint(mm, gvec[loop]);
			nvec2[loop] = transformNormal(nm, nvec[loop]);
		}
		for(loop = 0;loop < vcnt;loop += 3)
		{
			vx1 = gvec2[loop][0];
			vy1 = gvec2[loop][1];
			vz1 = gvec2[loop][2];
	
			vx2 = gvec2[loop + 1][0];
			vy2 = gvec2[loop + 1][1];
			vz2 = gvec2[loop + 1][2];

			vx3 = gvec2[loop + 2][0];
			vy3 = gvec2[loop + 2][1];
			vz3 = gvec2[loop + 2][2];

			nx1 = nvec2[loop][0];
			ny1 = nvec2[loop][1];
			nz1 = nvec2[loop][2];

			nx2 = nvec2[loop + 1][0];
			ny2 = nvec2[loop + 1][1];
			nz2 = nvec2[loop + 1][2];

			nx3 = nvec2[loop + 2][0];
			ny3 = nvec2[loop + 2][1];
			nz3 = nvec2[loop + 2][2];
			if(((vx1 == vx2) && (vx1 == vx3) && (vy1 == vy2) && (vy1 == vy3))
			|| ((vx1 == vx2) && (vx1 == vx3) && (vz1 == vz2) && (vz1 == vz3))
			|| ((vy1 == vy2) && (vy1 == vy3) && (vz1 == vz2) && (vz1 == vz3)))
			{
			}
			else
			{
				if(((vx1 == vx2) && (vy1 == vy2) && (vz1 == vz2))
				|| ((vx1 == vx3) && (vy1 == vy3) && (vz1 == vz3))
				|| ((vx2 == vx3) && (vy2 == vy3) && (vz2 == vz3)))
				{
				}
				else
				{
					cnt += 3;
				}
			}
		}
		if(cnt < vcnt)
		{
			::fprintf(stderr, "\nWarning: Mesh contains illegal triangles\n");
		}
		if(cnt > 0)
		{
			::fprintf(pov_FILE,"mesh {\n");
			for(loop = 0;loop < vcnt;loop += 3)
			{
				vx1 = gvec2[loop][0];
				vy1 = gvec2[loop][1];
				vz1 = gvec2[loop][2];
		
				vx2 = gvec2[loop + 1][0];
				vy2 = gvec2[loop + 1][1];
				vz2 = gvec2[loop + 1][2];
	
				vx3 = gvec2[loop + 2][0];
				vy3 = gvec2[loop + 2][1];
				vz3 = gvec2[loop + 2][2];
	
				nx1 = nvec2[loop][0];
				ny1 = nvec2[loop][1];
				nz1 = nvec2[loop][2];
	
				nx2 = nvec2[loop + 1][0];
				ny2 = nvec2[loop + 1][1];
				nz2 = nvec2[loop + 1][2];
	
				nx3 = nvec2[loop + 2][0];
				ny3 = nvec2[loop + 2][1];
				nz3 = nvec2[loop + 2][2];

				::fprintf(pov_FILE,"smooth_triangle { <%g, %g, %g>, <%g, %g, %g>, <%g, %g, %g>, <%g, %g, %g>, <%g, %g, %g>, <%g, %g, %g> }\n"
					, vx1, vy1, vz1, nx1, ny1, nz1
					, vx2, vy2, vz2, nx2, ny2, nz2
					, vx3, vy3, vz3, nx3, ny3, nz3);
			}
			::fprintf(pov_FILE,"\ttexture { %s }\n", surf_name[surf_index]);
			::fprintf(pov_FILE,"}\n");
		}
		else
		{
			::fprintf(stderr, "\nError: Mesh contained no legal triangles - omitted\n");
		}

                delete [] gvec2; //G.Barrand
                delete [] nvec2; //G.Barrand

	}
	else
	{
                //G.Barrand : dump type.
                ::fprintf(stderr, "\nError: Unsupported shape type - probably 2D (Point, Line, or Text2) node : type was %s\n",node->getTypeId().getName().getString());
                ::fprintf(pov_FILE,"/* Error: Unsupported shape type - probably 2D (Point, Line, or Text2) node  : type was %s*/\n",node->getTypeId().getName().getString());
	}
	vcnt = 0;
	nvcnt = 0;
	return(SoCallbackAction::CONTINUE);
}

static //G.Barrand
void	evalGraph(SoGroup *root)
{
	// Set up callbacks to add to scene graph, then apply
	SoType shapeType = SoShape::getClassTypeId();
	SoType lightType = SoLight::getClassTypeId();
	SoType textureType = SoTexture2::getClassTypeId();
	SoType cameraType = SoCamera::getClassTypeId();

	SoCallbackAction	ca;
	ca.addTriangleCallback(shapeType, addTriangleCB, NULL);
	ca.addPostCallback(shapeType, addMaterialCB, NULL);
	ca.addPreCallback(lightType, preLightCB, NULL);
	ca.addPreCallback(cameraType, preCameraCB, NULL);
	ca.addPreCallback(textureType, preTextureCB, NULL);
	ca.apply(root);

        delete [] gvec; //G.Barrand
        gvec = 0; //G.Barrand
        delete [] nvec; //G.Barrand
        nvec = 0; //G.Barrand

	::fprintf(stderr, "\rdone                         \n");
}

static //G.Barrand
void	countGraph(SoGroup *root)
{
	// Set up callbacks to add to scene graph, then apply
	SoType shapeType = SoShape::getClassTypeId();

	SoCallbackAction	ca;
	ca.addPreCallback(shapeType, countMaterialCB, NULL);
	ca.apply(root);
	::fprintf(pov_FILE,"\n");
}

static //G.Barrand
void	hunt_text(SoGroup *nd)
{
int	loop;
SoNode	*child;

	for(loop = 0;loop < nd->getNumChildren();loop++)
	{
		child = nd->getChild(loop);
		if(child->isOfType(SoText2::getClassTypeId()))
		{
			SoText2 *text2 = (SoText2 *)child;
			SoText3 *text3 = new SoText3;
			text3->justification = text2->justification;
			text3->string = text2->string;
			text3->spacing = text2->spacing;
			text3->parts = SoText3::FRONT;
			nd->replaceChild(text2, text3);
		}
		else
		{
			if(child->isOfType(SoGroup::getClassTypeId()))
			{
				hunt_text((SoGroup *)child);
			}
		}
	}
}

#include "iv2pov.h"

SbBool HEPVis_iv2pov(
 SoGroup* a_node
,SbBool aHuntText
,SbBool aRmBack
,const SbString& aFile
) {
  pov_FILE = ::fopen(aFile.getString(),"wb");
  if(pov_FILE==NULL) return FALSE;

  backface = (aRmBack==TRUE?1:0);

  vcnt = 0;
  nvcnt = 0;

  if(aHuntText==TRUE) hunt_text(a_node);

  countGraph(a_node);

  evalGraph(a_node);

  ::fclose(pov_FILE);
  pov_FILE = NULL;

  return TRUE;
}
