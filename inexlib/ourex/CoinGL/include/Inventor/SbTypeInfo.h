#ifndef COIN_SBTYPEINFO_H
#define COIN_SBTYPEINFO_H

/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2007 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See http://www.coin3d.org/ for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  http://www.sim.no/  sales@sim.no  coin-support@coin3d.org
 *
\**************************************************************************/

// SbTypeInfo<> is a Coin type information library.

/*!
  \class SbTypeInfo<T> SbTypeInfo.h Inventor/SbTypeInfo.h
  \brief a type information library for Coin types and their relations.

  SbTypeInfo<> makes it possible to write generic code using Coin types that
  is able to adjust itself to the parameter types used.  Usually you would
  be able to do that quite easily if the Sb-classes had been template classes
  of Type and Dimension arguments.  This type library makes it possible to
  work around that lack of genericity by looking up type relationships instead.
  
  Certain inside types of SbTypeInfo<> are always defined. Others will only
  be defined if the template argument type is of certain characteristics.

  Type is the type the SbTypeInfo<>-specialization is for.  It is always
  defined.

  ParamType is a type deemed to be efficient for function parameter transfer.
  It is always defined.
  
  If the type is a primitive type that has a corresponding class that is a
  two-dimensional vector of that type, then the Vec2Type type will be
  defined.  If there is a three-dimensional vector of that type, then
  Vec3Type will be defined, and similarly for Vec4Type.

  If the type is a primitive type that has a corresponding class that is a
  two-dimensional box class of that type, then the Box2Type type will be
  defined, and similarly for Box3Type.

  [more to come]

  \since Coin 2.5
*/

#include <Inventor/system/inttypes.h>

// *************************************************************************
// Predeclarations

// Sb-classes
class SbVec2b;
class SbVec2ub;
class SbVec2s;
class SbVec2us;
class SbVec2i32;
class SbVec2ui32;
class SbVec2f;
class SbVec2d;
class SbVec3b;
class SbVec3ub;
class SbVec3s;
class SbVec3us;
class SbVec3i32;
class SbVec3ui32;
class SbVec3f;
class SbVec3d;
class SbVec4b;
class SbVec4ub;
class SbVec4s;
class SbVec4us;
class SbVec4i32;
class SbVec4ui32;
class SbVec4f;
class SbVec4d;

class SbBox2s;
class SbBox2i32;
class SbBox2f;
class SbBox2d;
class SbBox3s;
class SbBox3i32;
class SbBox3f;
class SbBox3d;

class SbLine;
class SbDPLine;
class SbPlane;
class SbDPPlane;
class SbMatrix;
class SbDPMatrix;
class SbRotation;
class SbDPRotation;

class SbTime;
class SbName;
class SbColor;
class SbColor4f;
class SbString;

// Field classes
class SoSFBitMask;
class SoSFBool;
class SoSFBox2s;
class SoSFBox2i32;
class SoSFBox2f;
class SoSFBox2d;
class SoSFBox3s;
class SoSFBox3i32;
class SoSFBox3f;
class SoSFBox3d;
class SoSFColor;
class SoSFEngine;
class SoSFEnum;
class SoSFImage3;
class SoSFImage;
class SoSFMatrix;
class SoSFName;
class SoSFNode;
class SoSFPath;
class SoSFPlane;
class SoSFRotation;
class SoSFString;
class SoSFTime;
class SoSFTrigger;

class SoSFShort;
class SoSFUShort;
class SoSFInt32;
class SoSFUInt32;
class SoSFLong;
class SoSFULong;
class SoSFFloat;
class SoSFDouble;

class SoSFVec2b;
class SoSFVec2s;
class SoSFVec2i32;
class SoSFVec2f;
class SoSFVec2d;
class SoSFVec3b;
class SoSFVec3s;
class SoSFVec3i32;
class SoSFVec3f;
class SoSFVec3d;
class SoSFVec4b;
class SoSFVec4ub;
class SoSFVec4s;
class SoSFVec4us;
class SoSFVec4i32;
class SoSFVec4ui32;
class SoSFVec4f;
class SoSFVec4d;

class SoMFBitMask;
class SoMFBool;
class SoMFColor;
class SoMFDouble;
class SoMFEngine;
class SoMFEnum;
class SoMFFloat;
class SoMFInt32;
class SoMFLong;
class SoMFMatrix;
class SoMFName;
class SoMFNode;
class SoMFPath;
class SoMFPlane;
class SoMFRotation;
class SoMFShort;
class SoMFString;
class SoMFTime;
class SoMFUInt32;
class SoMFULong;
class SoMFUShort;
class SoMFVec2b;
class SoMFVec2s;
class SoMFVec2i32;
class SoMFVec2f;
class SoMFVec2d;
class SoMFVec3b;
class SoMFVec3s;
class SoMFVec3i32;
class SoMFVec3f;
class SoMFVec3d;
class SoMFVec4b;
class SoMFVec4ub;
class SoMFVec4s;
class SoMFVec4us;
class SoMFVec4i32;
class SoMFVec4ui32;
class SoMFVec4f;
class SoMFVec4d;

// *************************************************************************

template <typename T>
struct SbTypeInfo {
  typedef T Type;
  typedef const T & ParamType;
}; // fallback

template <>
struct SbTypeInfo<float> {
  typedef float Type;
  typedef Type ParamType;

  typedef SbVec2f Vec2Type;
  typedef SbVec3f Vec3Type;
  typedef SbVec4f Vec4Type;
  typedef SbBox2f Box2Type;
  typedef SbBox3f Box3Type;

  typedef SoSFFloat SFieldType;
  typedef SoMFFloat MFieldType;

  typedef SbPlane PlaneType;
  typedef SbLine LineType;
  typedef SbMatrix MatrixType;
  typedef SbRotation RotationType;
};

template <>
struct SbTypeInfo<double> {
  typedef double Type;
  typedef Type ParamType;

  typedef SbVec2d Vec2Type;
  typedef SbVec3d Vec3Type;
  typedef SbVec4d Vec4Type;
  typedef SbBox2d Box2Type;
  typedef SbBox3d Box3Type;

  typedef SoSFDouble SFieldType;
  typedef SoMFDouble MFieldType;

  typedef SbDPPlane PlaneType;
  typedef SbDPLine LineType;
  typedef SbDPMatrix MatrixType;
  typedef SbDPRotation RotationType;
};

template <>
struct SbTypeInfo<int8_t> {
  typedef int8_t Type;
  typedef Type ParamType;

  typedef SbVec2b Vec2Type;
  typedef SbVec3b Vec3Type;
  typedef SbVec4b Vec4Type;
};

template <>
struct SbTypeInfo<uint8_t> {
  typedef uint8_t Type;
  typedef Type ParamType;

  typedef SbVec2ub Vec2Type;
  typedef SbVec3ub Vec3Type;
  typedef SbVec4ub Vec4Type;
};

template <>
struct SbTypeInfo<short> {
  typedef short Type;
  typedef Type ParamType;

  typedef SbVec2s Vec2Type;
  typedef SbVec3s Vec3Type;
  typedef SbVec4s Vec4Type;
  typedef SbBox2s Box2Type;
  typedef SbBox3s Box3Type;

  typedef SoSFShort SFieldType;
  typedef SoMFShort MFieldType;
};

template <>
struct SbTypeInfo<unsigned short> {
  typedef unsigned short Type;
  typedef Type ParamType;

  typedef SbVec2us Vec2Type;
  typedef SbVec3us Vec3Type;
  typedef SbVec4us Vec4Type;

  typedef SoSFUShort SFieldType;
  typedef SoMFUShort MFieldType;
};

template <>
struct SbTypeInfo<int32_t> {
  typedef int32_t Type;
  typedef Type ParamType;

  typedef SbVec2i32 Vec2Type;
  typedef SbVec3i32 Vec3Type;
  typedef SbVec4i32 Vec4Type;
  typedef SbBox2i32 Box2Type;
  typedef SbBox3i32 Box3Type;

  typedef SoSFInt32 SFieldType;
  typedef SoMFInt32 MFieldType;
};

template <>
struct SbTypeInfo<uint32_t> {
  typedef uint32_t Type;
  typedef Type ParamType;

  typedef SbVec2ui32 Vec2Type;
  typedef SbVec3ui32 Vec3Type;
  typedef SbVec4ui32 Vec4Type;

  typedef SoSFUInt32 SFieldType;
  typedef SoMFUInt32 MFieldType;
};


template <>
struct SbTypeInfo<SbVec2f> {
  typedef SbVec2f Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;
  enum { Dimensions = 2 };

  typedef SbBox2f BoxType;

  typedef SoSFVec2f SFieldType;
  typedef SoMFVec2f MFieldType;
};

template <>
struct SbTypeInfo<SbVec2d> {
  typedef SbVec2d Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
  enum { Dimensions = 2 };

  typedef SbBox2d BoxType;

  typedef SoSFVec2d SFieldType;
  typedef SoMFVec2d MFieldType;
};

template <>
struct SbTypeInfo<SbVec2b> {
  typedef SbVec2b Type;
  typedef Type ParamType;

  typedef int8_t PrimitiveType;
  enum { Dimensions = 2 };

  typedef SoSFVec2b SFieldType;
  typedef SoMFVec2b MFieldType;
};

template <>
struct SbTypeInfo<SbVec2ub> {
  typedef SbVec2ub Type;
  typedef Type ParamType;

  typedef uint8_t PrimitiveType;
  enum { Dimensions = 2 };
};

template <>
struct SbTypeInfo<SbVec2s> {
  typedef SbVec2s Type;
  typedef Type ParamType;

  typedef short PrimitiveType;
  enum { Dimensions = 2 };

  typedef SbBox2s BoxType;

  typedef SoSFVec2s SFieldType;
  typedef SoMFVec2s MFieldType;
};

template <>
struct SbTypeInfo<SbVec2us> {
  typedef SbVec2us Type;
  typedef Type ParamType;

  typedef unsigned short PrimitiveType;
  enum { Dimensions = 2 };
};

template <>
struct SbTypeInfo<SbVec2i32> {
  typedef SbVec2i32 Type;
  typedef const Type & ParamType;

  typedef int32_t PrimitiveType;
  enum { Dimensions = 2 };

  typedef SbBox2i32 BoxType;

  typedef SoSFVec2i32 SFieldType;
  typedef SoMFVec2i32 MFieldType;
};

template <>
struct SbTypeInfo<SbVec2ui32> {
  typedef SbVec2ui32 Type;
  typedef const Type & ParamType;

  typedef uint32_t PrimitiveType;
  enum { Dimensions = 2 };
};

template <>
struct SbTypeInfo<SbVec3f> {
  typedef SbVec3f Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;
  enum { Dimensions = 3 };

  typedef SbBox3f BoxType;

  typedef SoSFVec3f SFieldType;
  typedef SoMFVec3f MFieldType;
};

template <>
struct SbTypeInfo<SbVec3d> {
  typedef SbVec3d Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
  enum { Dimensions = 3 };

  typedef SbBox3d BoxType;

  typedef SoSFVec3d SFieldType;
  typedef SoMFVec3d MFieldType;
};

template <>
struct SbTypeInfo<SbVec3b> {
  typedef SbVec3b Type;
  typedef Type ParamType;

  typedef int8_t PrimitiveType;
  enum { Dimensions = 3 };

  typedef SoSFVec3b SFieldType;
  typedef SoMFVec3b MFieldType;
};

template <>
struct SbTypeInfo<SbVec3ub> {
  typedef SbVec3ub Type;
  typedef Type ParamType;

  typedef uint8_t PrimitiveType;
  enum { Dimensions = 3 };
};

template <>
struct SbTypeInfo<SbVec3s> {
  typedef SbVec3s Type;
  typedef const Type & ParamType;

  typedef short PrimitiveType;
  enum { Dimensions = 3 };

  typedef SbBox3s BoxType;

  typedef SoSFVec3s SFieldType;
  typedef SoMFVec3s MFieldType;
};

template <>
struct SbTypeInfo<SbVec3us> {
  typedef SbVec3us Type;
  typedef const Type & ParamType;

  typedef unsigned short PrimitiveType;
  enum { Dimensions = 3 };
};

template <>
struct SbTypeInfo<SbVec3i32> {
  typedef SbVec3i32 Type;
  typedef const Type & ParamType;

  typedef int32_t PrimitiveType;
  enum { Dimensions = 3 };

  typedef SbBox3i32 BoxType;

  typedef SoSFVec3i32 SFieldType;
  typedef SoMFVec3i32 MFieldType;
};

template <>
struct SbTypeInfo<SbVec3ui32> {
  typedef SbVec3ui32 Type;
  typedef const Type & ParamType;

  typedef uint32_t PrimitiveType;
  enum { Dimensions = 3 };
};

template <>
struct SbTypeInfo<SbVec4f> {
  typedef SbVec4f Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;
  enum { Dimensions = 4 };

  typedef SoSFVec4f SFieldType;
  typedef SoMFVec4f MFieldType;
};

template <>
struct SbTypeInfo<SbVec4d> {
  typedef SbVec4d Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
  enum { Dimensions = 4 };

  typedef SoSFVec4d SFieldType;
  typedef SoMFVec4d MFieldType;
};

template <>
struct SbTypeInfo<SbVec4b> {
  typedef SbVec4b Type;
  typedef Type ParamType;

  typedef int8_t PrimitiveType;
  enum { Dimensions = 4 };

  typedef SoSFVec4b SFieldType;
  typedef SoMFVec4b MFieldType;
};

template <>
struct SbTypeInfo<SbVec4ub> {
  typedef SbVec4ub Type;
  typedef Type ParamType;

  typedef uint8_t PrimitiveType;
  enum { Dimensions = 4 };

  typedef SoSFVec4ub SFieldType;
  typedef SoMFVec4ub MFieldType;
};

template <>
struct SbTypeInfo<SbVec4s> {
  typedef SbVec4s Type;
  typedef const Type & ParamType;

  typedef short PrimitiveType;
  enum { Dimensions = 4 };

  typedef SoSFVec4s SFieldType;
  typedef SoMFVec4s MFieldType;
};

template <>
struct SbTypeInfo<SbVec4us> {
  typedef SbVec4us Type;
  typedef const Type & ParamType;

  typedef unsigned short PrimitiveType;
  enum { Dimensions = 4 };

  typedef SoSFVec4us SFieldType;
  typedef SoMFVec4us MFieldType;
};

template <>
struct SbTypeInfo<SbVec4i32> {
  typedef SbVec4i32 Type;
  typedef const Type & ParamType;

  typedef int32_t PrimitiveType;
  enum { Dimensions = 4 };

  typedef SoSFVec4i32 SFieldType;
  typedef SoMFVec4i32 MFieldType;
};

template <>
struct SbTypeInfo<SbVec4ui32> {
  typedef SbVec4ui32 Type;
  typedef const Type & ParamType;

  typedef uint32_t PrimitiveType;
  enum { Dimensions = 4 };

  typedef SoSFVec4ui32 SFieldType;
  typedef SoMFVec4ui32 MFieldType;
};

template <>
struct SbTypeInfo<SbBox2f> {
  typedef SbBox2f Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;
  enum { Dimensions = 2 };

  typedef SbVec2f VecType;

  typedef SoSFBox2f SFieldType;
};

template <>
struct SbTypeInfo<SbBox2d> {
  typedef SbBox2d Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
  enum { Dimensions = 2 };

  typedef SbVec2d VecType;

  typedef SoSFBox2d SFieldType;
};

template <>
struct SbTypeInfo<SbBox2s> {
  typedef SbBox2s Type;
  typedef const Type & ParamType;

  typedef short PrimitiveType;
  enum { Dimensions = 2 };

  typedef SbVec2s VecType;

  typedef SoSFBox2s SFieldType;
};

template <>
struct SbTypeInfo<SbBox2i32> {
  typedef SbBox2i32 Type;
  typedef const Type & ParamType;

  typedef int32_t PrimitiveType;
  enum { Dimensions = 2 };

  typedef SbVec2i32 VecType;

  typedef SoSFBox2i32 SFieldType;
};

template <>
struct SbTypeInfo<SbBox3f> {
  typedef SbBox3f Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;
  enum { Dimensions = 3 };

  typedef SbVec3f VecType;

  typedef SoSFBox3f SFieldType;
};

template <>
struct SbTypeInfo<SbBox3d> {
  typedef SbBox3d Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
  enum { Dimensions = 3 };

  typedef SbVec3d VecType;

  typedef SoSFBox3d SFieldType;
};

template <>
struct SbTypeInfo<SbBox3s> {
  typedef SbBox3s Type;
  typedef const Type & ParamType;

  typedef short PrimitiveType;
  enum { Dimensions = 3 };

  typedef SbVec3s VecType;

  typedef SoSFBox3s SFieldType;
};

template <>
struct SbTypeInfo<SbBox3i32> {
  typedef SbBox3i32 Type;
  typedef const Type & ParamType;

  typedef int32_t PrimitiveType;
  enum { Dimensions = 3 };

  typedef SbVec3i32 VecType;

  typedef SoSFBox3i32 SFieldType;
};

template <>
struct SbTypeInfo<SbPlane> {
  typedef SbPlane Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;
  enum { Dimensions = 3 };

  typedef SoSFPlane SFieldType;
  typedef SoMFPlane MFieldType;
};

template <>
struct SbTypeInfo<SbDPPlane> {
  typedef SbDPPlane Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
  enum { Dimensions = 3 };
};

template <>
struct SbTypeInfo<SbLine> {
  typedef SbLine Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;
  enum { Dimensions = 3 };
};

template <>
struct SbTypeInfo<SbDPLine> {
  typedef SbDPLine Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
  enum { Dimensions = 3 };
};

template <>
struct SbTypeInfo<SbRotation> {
  typedef SbRotation Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;

  typedef SoSFRotation SFieldType;
  typedef SoMFRotation MFieldType;
};

template <>
struct SbTypeInfo<SbDPRotation> {
  typedef SbDPRotation Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
};

template <>
struct SbTypeInfo<SbMatrix> {
  typedef SbMatrix Type;
  typedef const Type & ParamType;

  typedef float PrimitiveType;

  typedef SoSFMatrix SFieldType;
  typedef SoMFMatrix MFieldType;
};

template <>
struct SbTypeInfo<SbDPMatrix> {
  typedef SbDPMatrix Type;
  typedef const Type & ParamType;

  typedef double PrimitiveType;
};

// *************************************************************************

#endif // !COIN_SBTYPEINFO_H
