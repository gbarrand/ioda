// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file ioda.license for terms.

#include "../ioda/main"

#include <exlib/app/Windows/main_cpp>

int main(int argc,char** argv) {return exlib_main<ioda::context,ioda::main>("ioda",argc,argv);}

//exlib_build_use inlib expat
//exlib_build_use exlib png jpeg zlib inlib_glutess freetype
//exlib_build_use GL Windows kernel

