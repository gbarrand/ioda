// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

#include "so_2sg_action.h"

SO_ACTION_SOURCE(so_2sg_action)

//NOTE : Windows : it wants the below in the .cpp file.
void so_2sg_action::initClass() { //SoINTERNAL
  SO_ACTION_INIT_CLASS(so_2sg_action,SoCallbackAction);
}

//exlib_build_use inlib CoinGL
