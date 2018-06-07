// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

// NOTE from Apple :
// This class wraps the CAEAGLLayer from CoreAnimation
// into a convenient UIView subclass.
// The view content is basically an EAGL surface you render
// your OpenGL scene into.
// Note that setting the view non-opaque will only work
// if the EAGL surface has an alpha channel.

@class view_controller_delegate;
@class app_delegate;

//
#define USE_DISPLAY_LINK

//
#define EXLIB_IOS_ANTIALIASING 

@interface GLView : UIView {    
@private
  view_controller_delegate* m_view_controller_delegate;

  EAGLContext* m_context;
  GLuint m_frame_buffer;
  GLuint m_color_buffer;
  GLuint m_depth_buffer;

#ifdef EXLIB_IOS_ANTIALIASING 
  GLuint m_aa_frame_buffer;
  GLuint m_aa_color_buffer;
  GLuint m_aa_depth_buffer;
#endif

  app_delegate* m_app_delegate;

  //unsigned m_draw_count;
  bool m_disable_GL;

  float m_pinch_began;

#ifdef USE_DISPLAY_LINK
  CADisplayLink* m_display_link;
#else
  NSTimer* m_timer;
#endif
}

- (void)set_disable_GL:(bool)value;
- (void)window_render;
- (void)stop_timer;
- (view_controller_delegate*)get_view_controller_delegate;

@end
