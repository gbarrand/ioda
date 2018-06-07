/* Copyright (C) 2018, Guy Barrand. All rights reserved. */
/* See the file gopaw.license for terms. */

#ifndef gopaw_hbook_h
#define gopaw_hbook_h

typedef struct _hbook {
  float (*m_hrndm1)(void*,int);
  void (*m_hfill)(void*,int,float,float,float);
} hbook;

#endif
