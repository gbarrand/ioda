// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

#ifndef exlib_iv_so_2sg_action_h
#define exlib_iv_so_2sg_action_h

//NOTE : Android : have .h for this header because of the Android ndk-build tools.
//                 Else the make system does not produce the right build rule for so_2sg_action.cpp,
//                 it attempts to compile with the arm command and not c++.

#include <Inventor/actions/SoCallbackAction.h>

#include <Inventor/nodes/SoShape.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoLinePatternElement.h>
#include <Inventor/elements/SoDrawStyleElement.h>

#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>

//#include <Inventor/elements/SoViewVolumeElement.h>

#include <inlib/sg/atb_vertices>

enum viewport_handling {
  viewport_none,
  viewport_project,
  viewport_transform
};

class so_2sg_action : public SoCallbackAction {
  SO_ACTION_HEADER(so_2sg_action);
  typedef SoCallbackAction parent;
public:
  static void initClass(); //SoINTERNAL
public:
  so_2sg_action(const SbViewportRegion& a_vp,
                inlib::sg::atb_vertices& a_tris,inlib::sg::atb_vertices& a_segs,inlib::sg::atb_vertices& a_pts,
                inlib::sg::atb_vertices& a_atris,inlib::sg::atb_vertices& a_asegs,inlib::sg::atb_vertices& a_apts,
	        viewport_handling a_viewport_handling = viewport_none)
  :parent(a_vp)
  ,m_tris(a_tris),m_segs(a_segs),m_pts(a_pts)
  ,m_atris(a_atris),m_asegs(a_asegs),m_apts(a_apts)
  ,m_viewport_handling(a_viewport_handling)
  {
    SO_ACTION_CONSTRUCTOR(so_2sg_action);

    addPointCallback(SoShape::getClassTypeId(),shapePointCB,NULL);  
    addLineSegmentCallback(SoShape::getClassTypeId(),shapeLineSegmentCB,NULL);
    addTriangleCallback(SoShape::getClassTypeId(),shapeTriangleCB,NULL);
  } 
protected:
  virtual void beginTraversal(SoNode* aNode) {
    //SoViewportRegionElement::set(getState(),fViewportRegion); //CoinGL/SoCallbackAction::beginTraversal does it.

    //const SbVec2s& win = fViewportRegion.getWindowSize();
    //const SbVec2s& siz = fViewportRegion.getViewportSizePixels();

    m_tris.clear();
    m_tris.mode = inlib::gl::triangles();
    m_segs.clear();
    m_segs.mode = inlib::gl::lines();
    m_pts.clear();
    m_pts.mode = inlib::gl::points();

    m_atris.clear();
    m_atris.mode = inlib::gl::triangles();
    m_asegs.clear();
    m_asegs.mode = inlib::gl::lines();
    m_apts.clear();
    m_apts.mode = inlib::gl::points();

    SoCallbackAction::beginTraversal(aNode);
  }
protected:
  void get_matrix_viewport(SbMatrix & a_matrix,float a_viewport[]) const {
    a_matrix = SoModelMatrixElement::get(getState());
    if(m_viewport_handling==viewport_project) {
      a_matrix.multRight(SoViewingMatrixElement::get(getState())); //Yes, multRight and not multLeft.
      a_matrix.multRight(SoProjectionMatrixElement::get(getState()));
      
      const SbViewportRegion& vpr = SoViewportRegionElement::get(getState());
      const short* size = vpr.getViewportSizePixels().getValue();
      const short* orig = vpr.getViewportOriginPixels().getValue();
      
      a_viewport[0] = orig[0];
      a_viewport[1] = orig[1];
      a_viewport[2] = size[0];
      a_viewport[3] = size[1];
      
    } else if(m_viewport_handling==viewport_transform) {

      SbMatrix total = a_matrix;      
      total.multRight(SoViewingMatrixElement::get(getState())); //Yes, multRight and not multLeft.
      total.multRight(SoProjectionMatrixElement::get(getState()));
      if(!total.det4()) return;
           
      SbVec3f nilpoint(0,0,0);
      SbVec3f screenpoint;
      total.multVecMatrix(nilpoint,screenpoint);
      
      const SbViewportRegion& vpr = SoViewportRegionElement::get(getState());
      
      const short* win = vpr.getWindowSize().getValue();
      const short* size = vpr.getViewportSizePixels().getValue();
      const short* orig = vpr.getViewportOriginPixels().getValue();

      // Without viewport placement, [-1,1]x[-1,1] is positionned at :
      //   orig_0_x = (win[0]-size[0])/2
      //   orig_0_y = (win[1]-size[1])/2
      // with size (size[0],size[1]).
      // Then it should be translated in pixel window coordiantes by :
      //   orig[0]-orig_0_x    
      //   orig[1]-orig_0_y
      // then brought back to projected ndc [-1,1]x[-1,1] and at last
      // converted back to world model coord.
      
      float orig_0_x = 0.5f*(win[0]-size[0]);
      float orig_0_y = 0.5f*(win[1]-size[1]);

      float tx = (float(orig[0])-orig_0_x)*2.0f/float(size[0]);
      float ty = (float(orig[1])-orig_0_y)*2.0f/float(size[1]);
      
      SbVec3f trans = screenpoint + SbVec3f(tx,ty,0);

      SbMatrix inv = total.inverse();
      inv.multVecMatrix(trans,trans);
      SbMatrix mtrans;mtrans.setTranslate(trans);
      a_matrix.multLeft(mtrans);      
            
      a_viewport[0] = 0;
      a_viewport[1] = 0;
      a_viewport[2] = 0;
      a_viewport[3] = 0;
    } else {
      a_viewport[0] = 0;
      a_viewport[1] = 0;
      a_viewport[2] = 0;
      a_viewport[3] = 0;
    }
  }

  static void mul_vec_mtx(const SbMat& mat,const SbVec3f& src,SbVec3f& dst) {
    float W = src[0]*mat[0][3]+src[1]*mat[1][3]+src[2]*mat[2][3]+mat[3][3];
    dst[0] =(src[0]*mat[0][0]+src[1]*mat[1][0]+src[2]*mat[2][0]+mat[3][0])/W;
    dst[1] =(src[0]*mat[0][1]+src[1]*mat[1][1]+src[2]*mat[2][1]+mat[3][1])/W;
    dst[2] =(src[0]*mat[0][2]+src[1]*mat[1][2]+src[2]*mat[2][2]+mat[3][2])/W;
  }

  static void mul_dir_mtx(const SbMat& mat,const SbVec3f& src,SbVec3f & dst) {
    dst[0] = src[0]*mat[0][0] + src[1]*mat[1][0] + src[2]*mat[2][0];
    dst[1] = src[0]*mat[0][1] + src[1]*mat[1][1] + src[2]*mat[2][1];
    dst[2] = src[0]*mat[0][2] + src[1]*mat[1][2] + src[2]*mat[2][2];
  }

  static void to_viewport(float viewport[4],const SbVec3f& src,SbVec3f& dst) {
    // here src is in [-1,1]x[-1,1]x?
    // dst is going to be in [vp[0],vp[0]+vp[2]]x[vp[1],vp[1]+vp[3]]x?
    dst[0] = viewport[0] + (1 + src[0]) * viewport[2]/2;
    dst[1] = viewport[1] + (1 + src[1]) * viewport[3]/2;
    dst[2] = - (1+src[2])/2; //GB : in [0,1]  //Mesa : out[2] = (1+out[2])/2;
  }

  static void shapePointCB(void*,SoCallbackAction* a_this,const SoPrimitiveVertex* a_1) {
    so_2sg_action& self = *((so_2sg_action*)a_this);

    const SbColor& color = SoLazyElement::getDiffuse(self.getState(),0);
    float alpha = 1.0f-SoLazyElement::getTransparency(self.getState(),0);
    SbVec3f& out = self.m_opt_out;

    self.get_matrix_viewport(self.m_opt_matrix,self.m_opt_viewport);
    const SbMat& mm = self.m_opt_matrix.getValue();

    mul_vec_mtx(mm,a_1->getPoint(),out);
    if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out,out);

    if(alpha!=1.0f) {
      self.m_apts.add_pos_color(out[0],out[1],out[2],color[0],color[1],color[2],alpha);
    } else {
      self.m_pts.add_pos_color(out[0],out[1],out[2],color[0],color[1],color[2],1);
    }
  }

  static void shapeLineSegmentCB(void*,SoCallbackAction* a_this,
                                 const SoPrimitiveVertex* a_1,
                                 const SoPrimitiveVertex* a_2) {
    so_2sg_action& self = *((so_2sg_action*)a_this);

    const SbColor& color = SoLazyElement::getDiffuse(self.getState(),0);
    float transp = SoLazyElement::getTransparency(self.getState(),0);
    unsigned short linePattern = (unsigned short)SoLinePatternElement::get(self.getState());

    float alpha = 1.0f-transp;
    SbVec3f& out1 = self.m_opt_out;
    SbVec3f& out2 = self.m_opt_out_2;

    self.get_matrix_viewport(self.m_opt_matrix,self.m_opt_viewport);
    const SbMat& mm = self.m_opt_matrix.getValue();

    mul_vec_mtx(mm,a_1->getPoint(),out1);

    //::printf("debug : shapeLine : point 1 : %g %g %g, out : %g %g %g\n",
    //    a_1->getPoint()[0],a_1->getPoint()[1],a_1->getPoint()[2],out1[0],out1[1],out1[2]);    
    
    if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out1,out1);
    mul_vec_mtx(mm,a_2->getPoint(),out2);
    if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out2,out2);

    if(linePattern!=0xFFFF) {
      //opaw :
      //#define linePattern_SOLID       0xFFFF
      //#define linePattern_DASHED      0x00FF
      //#define linePattern_DOTTED      0x0101
      //#define linePattern_DASH_DOTTED 0x1C47
      //::printf("debug : linePattern %x\n",linePattern);

      unsigned int num_dash = 100; //FIXME : should be adapted according size of primitive versus size of viewport.
      if(alpha!=1.0f) {
        self.m_asegs.add_dashed_line_rgba(out1[0],out1[1],out1[2],
					  out2[0],out2[1],out2[2],num_dash,
					  color[0],color[1],color[2],alpha);
      } else {
        self.m_segs.add_dashed_line_rgba(out1[0],out1[1],out1[2],
	                                 out2[0],out2[1],out2[2],num_dash,
                                         color[0],color[1],color[2],1);
      }

    } else {    
      //::printf("debug : vp %g %g %g %g\n",
      //    self.m_opt_viewport[0],self.m_opt_viewport[1],self.m_opt_viewport[2],self.m_opt_viewport[3]);
    
      if(alpha!=1.0f) {
        self.m_asegs.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],alpha);
        self.m_asegs.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],alpha);
      } else {
        self.m_segs.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],1);
        self.m_segs.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],1);
      }

    }
  }

  static void shapeTriangleCB(void*,SoCallbackAction* a_this,
                              const SoPrimitiveVertex* a_1,
                              const SoPrimitiveVertex* a_2,
                              const SoPrimitiveVertex* a_3) {
    so_2sg_action& self = *((so_2sg_action*)a_this);

    SoDrawStyleElement::Style style = SoDrawStyleElement::get(self.getState());
    if(style==SoDrawStyleElement::INVISIBLE) return;

    const SbColor& color = SoLazyElement::getDiffuse(self.getState(),0);
    float transp = SoLazyElement::getTransparency(self.getState(),0);

    float alpha = 1.0f-transp;

    SbVec3f& out1 = self.m_opt_out;
    SbVec3f& out2 = self.m_opt_out_2;
    SbVec3f& out3 = self.m_opt_out_3;

    SbVec3f& out = self.m_opt_out;

    self.get_matrix_viewport(self.m_opt_matrix,self.m_opt_viewport);
    const SbMat& mm = self.m_opt_matrix.getValue();

    mul_vec_mtx(mm,a_1->getPoint(),out1);
    if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out1,out1);
    mul_vec_mtx(mm,a_2->getPoint(),out2);
    if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out2,out2);
    mul_vec_mtx(mm,a_3->getPoint(),out3);
    if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out3,out3);

    if(style==SoDrawStyleElement::POINTS) {

      if(alpha!=1.0f) {
        self.m_apts.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],alpha);
        self.m_apts.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],alpha);
        self.m_apts.add_pos_color(out3[0],out3[1],out3[2],color[0],color[1],color[2],alpha);
      } else {
        self.m_pts.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],1);
        self.m_pts.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],1);
        self.m_pts.add_pos_color(out3[0],out3[1],out3[2],color[0],color[1],color[2],1);
      }

    } else if(style==SoDrawStyleElement::LINES) {

      if(alpha!=1.0f) {
        self.m_asegs.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],alpha);
        self.m_asegs.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],alpha);

        self.m_asegs.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],alpha);
        self.m_asegs.add_pos_color(out3[0],out3[1],out3[2],color[0],color[1],color[2],alpha);

        self.m_asegs.add_pos_color(out3[0],out3[1],out3[2],color[0],color[1],color[2],alpha);
        self.m_asegs.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],alpha);

      } else {
        self.m_segs.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],1);
        self.m_segs.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],1);

        self.m_segs.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],1);
        self.m_segs.add_pos_color(out3[0],out3[1],out3[2],color[0],color[1],color[2],1);

        self.m_segs.add_pos_color(out3[0],out3[1],out3[2],color[0],color[1],color[2],1);
        self.m_segs.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],1);
      }

    } else if(style==SoDrawStyleElement::FILLED) {
    
      if(alpha!=1.0f) {
        //if(style==SoDrawStyleElement::LINES) self.m_atris.draw_edges = true;

        self.m_atris.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],alpha);
        mul_dir_mtx(mm,a_1->getNormal(),out);
        //if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out,out);
        self.m_atris.add_normal(out[0],out[1],out[2]);

        self.m_atris.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],alpha);
        mul_dir_mtx(mm,a_2->getNormal(),out);
        //if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out,out);
        self.m_atris.add_normal(out[0],out[1],out[2]);
  
        self.m_atris.add_pos_color(out3[0],out3[1],out3[2],color[0],color[1],color[2],alpha);
        mul_dir_mtx(mm,a_3->getNormal(),out);
        //if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out,out);
        self.m_atris.add_normal(out[0],out[1],out[2]);
      } else {
        //if(style==SoDrawStyleElement::LINES) self.m_tris.draw_edges = true;

        self.m_tris.add_pos_color(out1[0],out1[1],out1[2],color[0],color[1],color[2],1);
        mul_dir_mtx(mm,a_1->getNormal(),out);
        //if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out,out);
        self.m_tris.add_normal(out[0],out[1],out[2]);
  
        self.m_tris.add_pos_color(out2[0],out2[1],out2[2],color[0],color[1],color[2],1);
        mul_dir_mtx(mm,a_2->getNormal(),out);
        //if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out,out);
        self.m_tris.add_normal(out[0],out[1],out[2]);
  
        self.m_tris.add_pos_color(out3[0],out3[1],out3[2],color[0],color[1],color[2],1);
        mul_dir_mtx(mm,a_3->getNormal(),out);
        //if(self.m_viewport_handling==viewport_project) to_viewport(self.m_opt_viewport,out,out);
        self.m_tris.add_normal(out[0],out[1],out[2]);
      }

    }
  }

protected:
  inlib::sg::atb_vertices& m_tris;
  inlib::sg::atb_vertices& m_segs;
  inlib::sg::atb_vertices& m_pts;
  // with transparency :
  inlib::sg::atb_vertices& m_atris;
  inlib::sg::atb_vertices& m_asegs;
  inlib::sg::atb_vertices& m_apts;
  viewport_handling m_viewport_handling;
  //optimize
  SbVec3f m_opt_out;
  SbVec3f m_opt_out_2;
  SbVec3f m_opt_out_3;
  SbMatrix m_opt_matrix;
  float m_opt_viewport[4];
};

//exlib_build_use CoinGL inlib

#endif
