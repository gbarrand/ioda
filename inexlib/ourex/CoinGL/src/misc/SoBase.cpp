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

/*!
  \class SoBase SoBase.h Inventor/misc/SoBase.h
  \brief The SoBase class is the top-level superclass for a number
  of class-hierarchies.
  \ingroup general

  SoBase provides the basic interfaces and methods for doing reference
  counting, type identification and import/export. All classes in Coin
  which uses these mechanisms are descendent from this class.

  One important issue with SoBase-derived classes is that they should
  \e not be statically allocated, neither in static module memory nor
  on function's stack-frames. SoBase-derived classes must \e always be
  allocated dynamically from the memory heap with the \c new operator.

  This is so because SoBase-derived instances are reference counted,
  and will self-destruct on the approriate time. For this to work,
  they must be explicitly allocated in heap-memory. See the class
  documentation of SoNode for more information.
*/

// *************************************************************************

// FIXME: There's a lot of methods in SoBase used to implement VRML
// support which are missing.
//
// UPDATE 20020217 mortene: is this FIXME still correct?

// FIXME: One more thing missing: detect cases where we should
// instantiate SoUnknownEngine instead of SoUnknownNode.

// *************************************************************************

#include <Inventor/misc/SoBase.h>

#include <assert.h>
#include <string.h>

#include "../upgraders/SoUpgrader.h"
#include <Inventor/C/threads/threadsutilp.h>
#include <Inventor/C/tidbits.h>
#include <Inventor/C/tidbitsp.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/engines/SoEngineOutput.h>
#include <Inventor/engines/SoNodeEngine.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/lists/SoBaseList.h>
#include <Inventor/lists/SoFieldList.h>
#include <Inventor/misc/SoProto.h>
#include <Inventor/misc/SoProtoInstance.h>
#include <Inventor/nodes/SoUnknownNode.h>
#include <Inventor/sensors/SoDataSensor.h>
#include <Inventor/fields/SoGlobalField.h>
#include <Inventor/misc/SbHash.h>
#include "../io/SoInputP.h"
#include "../io/SoWriterefCounter.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

// *************************************************************************

// Note: the following documentation for getTypeId() will also be
// visible for subclasses, so keep it general.
/*!
  \fn SoType SoBase::getTypeId(void) const

  Returns the type identification of an object derived from a class
  inheriting SoBase.  This is used for run-time type checking and
  "downward" casting.

  Usage example:

  \code
  void foo(SoNode * node)
  {
    if (node->getTypeId() == SoFile::getClassTypeId()) {
      SoFile * filenode = (SoFile *)node;  // safe downward cast, knows the type
      /// [then something] ///
    }
    else if (node->getTypeId().isOfType(SoGroup::getClassTypeId())) {
      SoGroup * group = (SoGroup *)node;  // safe downward cast, knows the type
      /// [then something else] ///
    }
  }
  \endcode


  For application programmers wanting to extend the library with new
  nodes, engines, nodekits, draggers or others: this method needs to
  be overridden in \e all subclasses. This is typically done as part
  of setting up the full type system for extension classes, which is
  usually accomplished by using the pre-defined macros available
  through for instance Inventor/nodes/SoSubNode.h (SO_NODE_INIT_CLASS
  and SO_NODE_CONSTRUCTOR for node classes),
  Inventor/engines/SoSubEngine.h (for engine classes) and so on.

  For more information on writing Coin extensions, see the class
  documentation of the toplevel superclasses for the various class
  groups.
*/

// Note: the following documentation for readInstance() will also be
// visible for subclasses, so keep it general.
/*!
  \fn SbBool SoBase::readInstance(SoInput * in, unsigned short flags)

  This method is mainly intended for internal use during file import
  operations.

  It reads a definition of an instance from the input stream \a in.
  The input stream state points to the start of a serialized /
  persistant representation of an instance of this class type.

  \c TRUE or \c FALSE is returned, depending on if the instantiation
  and configuration of the new object of this class type went ok or
  not.  The import process should be robust and handle corrupted input
  streams by returning \c FALSE.

  \a flags is used internally during binary import when reading user
  extension nodes, group nodes or engines.
*/

/*!
  \enum SoBase::BaseFlags
  \COININTERNAL
*/

// *************************************************************************

// FIXME: should implement and use a proper set-abstraction
// datatype. 20050524 mortene.
typedef SbHash<void *, const SoBase *> SoBaseSet;

class SoBaseP {
public:
  static void * mutex;
  static void * name2obj_mutex;
  static void * obj2name_mutex;
  static void * auditor_mutex;
  static void * global_mutex;

  static SbHash<SoAuditorList *, const SoBase *> * auditordict;
  static SbHash<SbPList *, const char *> * name2obj;
  static SbHash<const char *, const SoBase *> * obj2name;

  static void auditordict_cb(const SoBase * const & key, SoAuditorList * const & value, void * closure);
  static void cleanup_auditordict(void);

  static void emptyName2ObjHash(const char * const & n, SbPList * const & l, void * closure);

  static void check_for_leaks(void);
  static SbBool trackbaseobjects;
  static void * allbaseobj_mutex;
  static SoBaseSet * allbaseobj; // maps from SoBase * to NULL
};

void * SoBaseP::mutex = NULL;
void * SoBaseP::name2obj_mutex = NULL;
void * SoBaseP::obj2name_mutex = NULL;
void * SoBaseP::auditor_mutex = NULL;
void * SoBaseP::global_mutex = NULL;

SbHash<SoAuditorList *, const SoBase *> * SoBaseP::auditordict = NULL;

// Only a small number of SoBase derived objects will under usual
// conditions have designated names, so we use a couple of static
// dictionary objects to keep track of them. Since we avoid storing a
// pointer for each and every object, we'll cut down on a decent
// amount of memory use this way (SoBase should be kept as slim as
// possible, as any dead weight is brought along in a lot of objects).
SbHash<SbPList *, const char *> * SoBaseP::name2obj = NULL;
SbHash<const char *, const SoBase *> * SoBaseP::obj2name = NULL;

// OBSOLETE! Just kept around for Coin 2.x ABI compatibility.
SbDict * SoBase::name2obj;
SbDict * SoBase::obj2name;

// This is used for debugging purposes: it stores a pointer to all
// SoBase-derived objects that have been allocated and not
// deallocated.
SbBool SoBaseP::trackbaseobjects = FALSE;
void * SoBaseP::allbaseobj_mutex = NULL;
SoBaseSet * SoBaseP::allbaseobj = NULL; // maps from SoBase * to NULL

void
SoBaseP::auditordict_cb(const SoBase * const &, SoAuditorList * const & l, void *)
{
  delete l;
}

void
SoBaseP::cleanup_auditordict(void)
{
  if (SoBaseP::auditordict) {
    SoBaseP::auditordict->apply(SoBaseP::auditordict_cb, NULL);
    delete SoBaseP::auditordict;
    SoBaseP::auditordict = NULL;
  }
}

void
SoBaseP::check_for_leaks(void)
{
#if COIN_DEBUG
  if (SoBaseP::trackbaseobjects) {
    SbList<const SoBase *> keys;
    SoBaseP::allbaseobj->makeKeyList(keys);
    const unsigned int len = keys.getLength();
    if (len > 0) {
      // Use printf()s, in case SoDebugError has been made defunct by
      // previous coin_atexit() work.
      (void)printf("\nSoBase-derived instances not deallocated:\n");

      for (unsigned int i=0; i < len; i++) {
        const SoBase * base = keys[i];
        base->assertAlive();
        const SbName name = base->getName();
        const SoType t = base->getTypeId();
        SbString s;
        s.sprintf("\"%s\"", name.getString());
        (void)printf("\t%p type==(0x%04x, '%s') name=='%s'\n",
                     base, t.getKey(), t.getName().getString(),
                     name == "" ? "no name" : s.getString());
      }
      (void)printf("\n");
    }
  }
#endif // COIN_DEBUG
}

// *************************************************************************

// Strings and character tokens used in parsing.
static const char OPEN_BRACE = '{';
static const char CLOSE_BRACE = '}';
static const char END_OF_LINE[] = "\n";
static const char DEF_KEYWORD[] = "DEF";
static const char USE_KEYWORD[] = "USE";
static const char NULL_KEYWORD[] = "NULL";
static const char ROUTE_KEYWORD[] = "ROUTE";

static const char PROTO_KEYWORD[] = "PROTO";
static const char EXTERNPROTO_KEYWORD[] = "EXTERNPROTO";

SbString * SoBase::refwriteprefix = NULL;

SoType SoBase::classTypeId STATIC_SOTYPE_INIT;

SbBool SoBase::tracerefs = FALSE;
uint32_t SoBase::writecounter = 0;

/**********************************************************************/

// This can be any "magic" bitpattern of 4 bits which seems unlikely
// to be randomly assigned to a memory byte upon destruction. I chose
// "1101".
//
// The 4 bits allocated for the "alive" bitpattern is used in
// SoBase::ref() to try to detect when the instance has been
// prematurely destructed.
//
// <mortene@sim.no>
#define ALIVE_PATTERN 0xd


/*!
  Constructor. The initial reference count will be set to zero.
 */
SoBase::SoBase(void)
{
  // It is a common mistake to place e.g. nodes as static member
  // variables, or on the main()-function's stack-frame. This catches
  // some (but not all) of those cases.
  //
  // FIXME: we could probably add in an MSWin-specific extra check
  // here for instances placed dynamically on a stack, using Win32 API
  // functions that can classify a memory pointer as e.g. heap or
  // stack. 20031018 mortene.
  assert((SoBase::classTypeId != SoType::badType()) &&
         "An SoBase-derived class was attempted instantiated *before* Coin initialization. (Have you perhaps placed an SoBase-derived instance (e.g. a scene graph node) in non-heap memory?) See SoBase class documentation for more info.");

  cc_rbptree_init(&this->auditortree);

  this->objdata.referencecount = 0;

  // For debugging -- we try to catch dangling references after
  // premature destruction. See the SoBase::assertAlive() method for
  // further doc.
  this->objdata.alive = ALIVE_PATTERN;

  // For debugging, store a pointer to all SoBase-instances.
#if COIN_DEBUG
  if (SoBaseP::trackbaseobjects) {
    CC_MUTEX_LOCK(SoBaseP::allbaseobj_mutex);
    void * dummy;
    assert(!SoBaseP::allbaseobj->get(this, dummy));
    SoBaseP::allbaseobj->put(this, NULL);
    CC_MUTEX_UNLOCK(SoBaseP::allbaseobj_mutex);
  }
#endif // COIN_DEBUG
}

/*!
  Destructor. There should not be any normal circumstance where you need
  to explicitly delete an object derived from the SoBase class, as the
  reference counting should take care of deallocating unused objects.

  \sa unref(), unrefNoDelete()
 */
SoBase::~SoBase()
{
#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::~SoBase", "%p", this);
#endif // debug

  // Set the 4 bits of bitpattern to anything but the "magic" pattern
  // used to check that we are still alive.
  this->objdata.alive = (~ALIVE_PATTERN) & 0xf;

  if (SoBaseP::auditordict) {
    SoAuditorList * l;
    if (SoBaseP::auditordict->get(this, l)) {
      delete l;
      SoBaseP::auditordict->remove(this);
    }
  }
  cc_rbptree_clean(&this->auditortree);

#if COIN_DEBUG
  if (SoBaseP::trackbaseobjects) {
    CC_MUTEX_LOCK(SoBaseP::allbaseobj_mutex);
    const SbBool ok = SoBaseP::allbaseobj->remove(this);
    assert(ok && "something fishy going on in debug object tracking");
    CC_MUTEX_UNLOCK(SoBaseP::allbaseobj_mutex);
  }
#endif // COIN_DEBUG
}

//
// callback from auditortree that is used to add sensor
// auditors to the list (closure).
//
static void
sobase_sensor_add_cb(void * auditor, void * type, void * closure)
{
  SbList<SoDataSensor *> * auditingsensors =
    (SbList<SoDataSensor*> *) closure;

  // MSVC7 on 64-bit Windows wants to go through this type when
  // casting from void*.
  const uintptr_t tmp = (uintptr_t)type;
  switch ((SoNotRec::Type) tmp) {
  case SoNotRec::SENSOR:
    auditingsensors->append((SoDataSensor *)auditor);
    break;

  case SoNotRec::FIELD:
  case SoNotRec::ENGINE:
  case SoNotRec::CONTAINER:
  case SoNotRec::PARENT:
    // FIXME: should any of these get special treatment? 20000402 mortene.
    break;

  default:
    assert(0 && "Unknown auditor type");
  }
}

/*!
  Cleans up all hanging references to and from this instance, and then
  commits suicide.

  Called automatically when the reference count goes to zero.
*/
void
SoBase::destroy(void)
{
#if COIN_DEBUG && 0 // debug
  SbName dn = this->getName();
  SoType t = this->getTypeId();
  SoDebugError::postInfo("SoBase::destroy", "start -- %p '%s' ('%s')",
                         this, t.getName().getString(), dn.getString());
#endif // debug


#if COIN_DEBUG
  if (SoBase::tracerefs) {
    SoDebugError::postInfo("SoBase::destroy",
                           "%p ('%s')",
                           this, this->getTypeId().getName().getString());
  }
#endif // COIN_DEBUG

  // Find all auditors that they need to cut off their link to this
  // object. I believe this is necessary only for sensors.
  SbList<SoDataSensor *> auditingsensors;
  cc_rbptree_traverse(&this->auditortree, (cc_rbptree_traversecb *)sobase_sensor_add_cb, &auditingsensors);

  // Notify sensors that we're dying.
  for (int j = 0; j < auditingsensors.getLength(); j++)
    auditingsensors[j]->dyingReference();

  // Link out instance name from the list of all SoBase instances.
  SbName n = this->getName();
  if (n.getLength()) this->removeName(this, n.getString());

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::destroy", "delete this %p", this);
#endif // debug

  // Harakiri
  delete this;

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::destroy", "done -- %p '%s' ('%s')",
                         this, t.getName().getString(), dn.getString());
#endif // debug
}

/*!
  Sets up initialization for data common to all instances of this
  class, like submitting necessary information to the Coin type
  system.
 */
void
SoBase::initClass(void)
{
  coin_atexit((coin_atexit_f *)SoBase::cleanClass, CC_ATEXIT_SOBASE);

  // check_for_leaks() goes through the allocation list, and checks if
  // all allocated SoBase-derived instances was deallocated before the
  // atexit-routines were run.
  //
  // Set up to run before other atexit-code, with NORMAL+1 priority,
  // since we depend on misc parts of Coin still up and running (and
  // no other code should depend on the allocation list, so this
  // should be safe).
  //
  // -mortene.
  coin_atexit((coin_atexit_f *)SoBaseP::check_for_leaks, CC_ATEXIT_NORMAL + 1);

  // Avoid multiple attempts at initialization.
  assert(SoBase::classTypeId == SoType::badType());

  SoBase::classTypeId = SoType::createType(SoType::badType(), "Base");

  SoBaseP::name2obj = new SbHash<SbPList *, const char *>;
  SoBaseP::obj2name = new SbHash<const char *, const SoBase *>();
  SoBase::refwriteprefix = new SbString("+");
  SoBaseP::allbaseobj = new SoBaseSet;

  CC_MUTEX_CONSTRUCT(SoBaseP::mutex);
  CC_MUTEX_CONSTRUCT(SoBaseP::obj2name_mutex);
  CC_MUTEX_CONSTRUCT(SoBaseP::name2obj_mutex);
  CC_MUTEX_CONSTRUCT(SoBaseP::allbaseobj_mutex);
  CC_MUTEX_CONSTRUCT(SoBaseP::auditor_mutex);
  CC_MUTEX_CONSTRUCT(SoBaseP::global_mutex);

  // debug
  const char * str = coin_getenv("COIN_DEBUG_TRACK_SOBASE_INSTANCES");
  SoBaseP::trackbaseobjects = str && atoi(str) > 0;  

  SoWriterefCounter::initClass();
}

// Clean up all commonly allocated resources before application
// exit. Only for debugging purposes.
void
SoBase::cleanClass(void)
{
  assert(SoBaseP::name2obj);
  assert(SoBaseP::obj2name);

  // Delete the SbPLists in the dictionaries.
  SoBaseP::name2obj->apply(SoBaseP::emptyName2ObjHash, NULL);

  delete SoBaseP::allbaseobj; SoBaseP::allbaseobj = NULL;

  delete SoBaseP::name2obj; SoBaseP::name2obj = NULL;
  delete SoBaseP::obj2name; SoBaseP::obj2name = NULL;

  delete SoBase::refwriteprefix;

  CC_MUTEX_DESTRUCT(SoBaseP::mutex);
  CC_MUTEX_DESTRUCT(SoBaseP::obj2name_mutex);
  CC_MUTEX_DESTRUCT(SoBaseP::allbaseobj_mutex);
  CC_MUTEX_DESTRUCT(SoBaseP::name2obj_mutex);
  CC_MUTEX_DESTRUCT(SoBaseP::auditor_mutex);
  CC_MUTEX_DESTRUCT(SoBaseP::global_mutex);

  SoBase::tracerefs = FALSE;
  SoBase::writecounter = 0;
}

/*!
  \COININTERNAL

  There are 4 bits allocated for each SoBase-object for a bitpattern
  that indicates the object is still "alive". The pattern is changed
  when the object is destructed. If this method is then called after
  destruction, an assert will hit.

  This is used internally in Coin (in for instance SoBase::ref()) to
  try to detect when the instance has been prematurely
  destructed. This is a very common mistake to make by application
  programmers (letting the refcount dip to zero before it should, that
  is), so the extra piece of assistance through the accompanying
  assert() in this method to detect dangling references to the object,
  with subsequent memory corruption and mysterious crashes, should be
  a Good Thing.

  \COIN_FUNCTION_EXTENSION

  \since Coin 2.0
*/
void
SoBase::assertAlive(void) const
{
  if (this->objdata.alive != ALIVE_PATTERN) {
    SoDebugError::post("SoBase::assertAlive",
                       "Detected an attempt to access an instance (%p) of an "
                       "SoBase-derived class after it was destructed!  "
                       "This is most likely to be the result of some grave "
                       "programming error in the application / client "
                       "code (or less likely: internal library code), "
                       "causing premature destruction of a reference "
                       "counted object instance. This check was called "
                       "from a dangling reference to it.", this);
    assert(FALSE && "SoBase-object no longer alive!");
  }
}

/*!
  Increase the reference count of the object. This might be necessary
  to do explicitly from user code for certain situations (chiefly to
  avoid premature deletion), but is usually called from other instances
  within the Coin library when objects are made dependent on each other.

  See the class documentation of SoNode for more extensive information
  about reference counting.

  \sa unref(), unrefNoDelete()
 */
void
SoBase::ref(void) const
{
  if (COIN_DEBUG) this->assertAlive();

  // Cast away constness.
  SoBase * base = (SoBase *)this;

  CC_MUTEX_LOCK(SoBaseP::mutex);
#if COIN_DEBUG
  int32_t currentrefcount = base->objdata.referencecount;
#endif // COIN_DEBUG
  base->objdata.referencecount++;
  CC_MUTEX_UNLOCK(SoBaseP::mutex);

#if COIN_DEBUG
  if (base->objdata.referencecount < currentrefcount) {
    SoDebugError::post("SoBase::ref",
                       "%p ('%s') - referencecount overflow!: %d -> %d",
                       this, this->getTypeId().getName().getString(),
                       currentrefcount, this->objdata.referencecount);

    // The reference counter is contained within 27 bits of signed
    // integer, which means it can go up to about ~67 million
    // references. It's hard to imagine that this should be too small,
    // so we don't bother to try to handle overflows any better than
    // this.
    //
    // If we should ever revert this decision, look in Coin-1 for how
    // to handle overflows graciously.
    assert(FALSE && "reference count overflow");
  }
#endif // COIN_DEBUG

#if COIN_DEBUG
  if (SoBase::tracerefs) {
    SoDebugError::postInfo("SoBase::ref",
                           "%p ('%s') - referencecount: %d",
                           this, this->getTypeId().getName().getString(),
                           this->objdata.referencecount);
  }
#endif // COIN_DEBUG
}

/*!
  Decrease the reference count of an object. If the reference count
  reaches zero, the object will delete itself. Be careful when
  explicitly calling this method, beware that you usually need to
  match user level calls to ref() with calls to either unref() or
  unrefNoDelete() to avoid memory leaks.

  \sa ref(), unrefNoDelete()
*/
void
SoBase::unref(void) const
{
  if (COIN_DEBUG) this->assertAlive();

  // Cast away constness.
  SoBase * base = (SoBase *)this;
  CC_MUTEX_LOCK(SoBaseP::mutex);
  base->objdata.referencecount--;
  int refcount = base->objdata.referencecount;
 
  CC_MUTEX_UNLOCK(SoBaseP::mutex);

#if COIN_DEBUG
  if (SoBase::tracerefs) {
    SoDebugError::postInfo("SoBase::unref",
                           "%p ('%s') - referencecount: %d",
                           this, this->getTypeId().getName().getString(),
                           this->objdata.referencecount);
  }
  if (refcount < 0) {
    // Do the debug output in two calls, since the getTypeId() might
    // cause a crash, and then there'd be no output at all with a
    // single SoDebugError::postWarning() call.
    SoDebugError::postWarning("SoBase::unref", "ref count less than zero");
    SoDebugError::postWarning("SoBase::unref", "...for %s instance at %p",
                              this->getTypeId().getName().getString(), this);
  }
#endif // COIN_DEBUG
  if (refcount == 0) base->destroy();
}

/*!
  Decrease reference count, but do \e not delete ourself if the count
  reaches zero.

  \sa unref()
 */
void
SoBase::unrefNoDelete(void) const
{
  if (COIN_DEBUG) this->assertAlive();

  // Cast away constness.
  SoBase * base = (SoBase *)this;
  base->objdata.referencecount--;
#if COIN_DEBUG
  if (SoBase::tracerefs) {
    SoDebugError::postInfo("SoBase::unrefNoDelete",
                           "%p ('%s') - referencecount: %d",
                           this, this->getTypeId().getName().getString(),
                           this->objdata.referencecount);
  }
#endif // COIN_DEBUG
}

/*!
  Returns number of objects referring to this object.
*/
int32_t
SoBase::getRefCount(void) const
{
  return this->objdata.referencecount;
}

/*!
  Force an update, in the sense that all objects connected to this
  object as an auditor will have to re-check the values of their
  inter-dependent data.

  This is often used as an effective way of manually triggering a
  redraw by application programmers.
*/
void
SoBase::touch(void)
{
  this->startNotify();
}

/*!
  Returns \c TRUE if the type of this object is either of the same
  type or inherited from \a type.
 */
SbBool
SoBase::isOfType(SoType type) const
{
  return this->getTypeId().isDerivedFrom(type);
}

/*!
  This static method returns the SoType object associated with
  objects of this class.
 */
SoType
SoBase::getClassTypeId(void)
{
  return SoBase::classTypeId;
}

/*!
  Returns name of object. If no name has been given to this object,
  the returned SbName will contain the empty string.
 */
SbName
SoBase::getName(void) const
{
  // If this assert fails, SoBase::initClass() has probably not been
  // called, or you have objects on the stack that is destroyed after
  // you have invoked SoDB::cleanup().
  assert(SoBaseP::obj2name);

  const char * value = NULL;
  CC_MUTEX_LOCK(SoBaseP::obj2name_mutex);
  SbBool found = SoBaseP::obj2name->get(this, value);
  CC_MUTEX_UNLOCK(SoBaseP::obj2name_mutex);
  return SbName(found ? value : "");
}

/*!
  Set the name of this object.

  Some characters are invalid to use as parts of names for SoBase
  derived objects, as object names needs to be consistent with the
  syntax of Inventor and VRML files upon file export / import
  operations (so one must for instance avoid using special token
  characters).

  Invalid characters will be automatically replaced by underscore
  characters. If the name \e starts with an invalid character, the new
  name will be \e preceded by an underscore character.

  For the exact definitions of what constitutes legal and illegal
  characters for SoBase names, see the SbName functions listed below.

  \sa getName(), SbName::isBaseNameStartChar(), SbName::isBaseNameChar()
*/
void
SoBase::setName(const SbName & newname)
{
  // remove old name first
  SbName oldName = this->getName();
  if (oldName.getLength()) this->removeName(this, oldName.getString());

  // check for bad characters
  const char * str = newname.getString();
  SbBool isbad = FALSE;

  isbad = (newname.getLength() > 0) && !SbName::isBaseNameStartChar(str[0]);

  int i;
  for (i = 1; i < newname.getLength() && !isbad; i++) {
    isbad = !SbName::isBaseNameChar(str[i]);
  }

  if (isbad) {
    // replace bad characters
    SbString goodstring;

    if (!SbName::isBaseNameStartChar(str[0])) goodstring += '_';

    for (i = 0; i < newname.getLength(); i++) {
      goodstring += SbName::isBaseNameChar(str[i]) ? str[i] : '_';
    }

#if COIN_DEBUG
    SoDebugError::postWarning("SoBase::setName", "Bad characters in "
                              "name '%s'. Replacing with name '%s'",
                              str, goodstring.getString());
#endif // COIN_DEBUG

    this->addName(this, SbName(goodstring.getString()));
  }
  else {
    this->addName(this, newname.getString());
  }
}

/*!
  Adds a name<->object mapping to the global dictionary.
 */
void
SoBase::addName(SoBase * const b, const char * const name)
{
  SbPList * l;
  CC_MUTEX_LOCK(SoBaseP::name2obj_mutex);
  if (!SoBaseP::name2obj->get(name, l)) {
    // name not used before, create new list
    l = new SbPList;
    SoBaseP::name2obj->put(name, l);
  }

  // append this to the list
  l->append(b);
  CC_MUTEX_UNLOCK(SoBaseP::name2obj_mutex);

  CC_MUTEX_LOCK(SoBaseP::obj2name_mutex);
  // set name of object. SbHash::put() will overwrite old name
  (void)SoBaseP::obj2name->put(b, name);
  CC_MUTEX_UNLOCK(SoBaseP::obj2name_mutex);
}

/*!
  Removes a name<->object mapping from the global dictionary.
*/
void
SoBase::removeName(SoBase * const b, const char * const name)
{
  CC_MUTEX_LOCK(SoBaseP::name2obj_mutex);
  SbPList * l = NULL;
  SbBool found = SoBaseP::name2obj->get(name, l);
  assert(found);

  const int i = l->find(b);
  assert(i >= 0);
  l->remove(i);

  CC_MUTEX_UNLOCK(SoBaseP::name2obj_mutex);

  CC_MUTEX_LOCK(SoBaseP::obj2name_mutex);
  SoBaseP::obj2name->remove(b);
  CC_MUTEX_UNLOCK(SoBaseP::obj2name_mutex);
}

/*!
  This is the method which starts the notification sequence
  after changes.

  At the end of a notification sequence, all "immediate" sensors
  (i.e. sensors set up with a zero priority) are triggered.
*/
void
SoBase::startNotify(void)
{
  SoNotList l;
  SoNotRec rec(this);
  l.append(&rec);
  l.setLastType(SoNotRec::CONTAINER);

  SoDB::startNotify();
  this->notify(&l);
  SoDB::endNotify();
}

// only needed for the callback from cc_rbptree_traverse
typedef struct {
  int cnt;
  int total;
  SoNotList * list;
  SoBase * thisp;
  SbList <void*> notified;
} sobase_notify_data;

//
// Callback from cc_rbptree_traverse().
//
void
SoBase::rbptree_notify_cb(void * auditor, void * type, void * closure)
{
  sobase_notify_data * data = (sobase_notify_data*) closure;
  data->cnt--;

  // MSVC7 on 64-bit Windows wants to go through this type when
  // casting from void*.
  const uintptr_t tmptype = (uintptr_t)type;

  if (data->notified.find(auditor) < 0) {
    if (data->cnt == 0) {
      data->thisp->doNotify(data->list, auditor, (SoNotRec::Type) tmptype);
    }
    else {
      assert(data->cnt > 0);
      // use a copy of 'l', since the notification list might change
      // when auditors are notified
      SoNotList listcopy(data->list);
      data->thisp->doNotify(&listcopy, auditor, (SoNotRec::Type) tmptype);
    }
  }
}

/*!
  Notifies all auditors for this instance when changes are made.
*/
void
SoBase::notify(SoNotList * l)
{
  if (COIN_DEBUG) this->assertAlive();

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::notify", "base %p, list %p", this, l);
#endif // debug

  sobase_notify_data notdata;
  notdata.cnt = cc_rbptree_size(&this->auditortree);
  notdata.list = l;
  notdata.thisp = this;

  cc_rbptree_traverse(&this->auditortree, (cc_rbptree_traversecb *)SoBase::rbptree_notify_cb, &notdata);
  assert(notdata.cnt == 0);
}

/*!
  Add an auditor to notify whenever the object changes in any significant
  way.

  \sa removeAuditor()
 */
void
SoBase::addAuditor(void * const auditor, const SoNotRec::Type type)
{
  // MSVC7 on 64-bit Windows wants to go through this type before
  // casting to void*.
  const uintptr_t val = (uintptr_t)type;
  cc_rbptree_insert(&this->auditortree, auditor, (void *)val);
}

/*!
  Remove an auditor from the list. \a auditor will then no longer be
  notified whenever any updates are made to this object.

  \sa addAuditor()
*/
void
SoBase::removeAuditor(void * const auditor, const SoNotRec::Type type)
{
  cc_rbptree_remove(&this->auditortree, auditor);
}


static void
sobase_audlist_add(void * pointer, void * type, void * closure)
{
  SoAuditorList * l = (SoAuditorList*) closure;
  // MSVC7 on 64-bit Windows wants to go through this type before
  // casting to void*.
  const uintptr_t tmp = (uintptr_t)type;
  l->append(pointer, (SoNotRec::Type)tmp);
}

/*!
  Returns list of objects auditing this object.

  \sa addAuditor(), removeAuditor()
*/
const SoAuditorList &
SoBase::getAuditors(void) const
{
  CC_MUTEX_LOCK(SoBaseP::auditor_mutex);

  if (SoBaseP::auditordict == NULL) {
    SoBaseP::auditordict = new SbHash<SoAuditorList *, const SoBase *>();
    coin_atexit((coin_atexit_f*)SoBaseP::cleanup_auditordict, CC_ATEXIT_NORMAL);
  }

  SoAuditorList * l = NULL;
  if (SoBaseP::auditordict->get(this, l)) {
    // empty list before copying in new values
    for (int i = 0; i < l->getLength(); i++) {
      l->remove(i);
    }
  }
  else {
    l = new SoAuditorList;
    SoBaseP::auditordict->put(this, l);
  }
  cc_rbptree_traverse(&this->auditortree, (cc_rbptree_traversecb*)sobase_audlist_add, (void*) l);

  CC_MUTEX_UNLOCK(SoBaseP::auditor_mutex);

  return *l;
}

/*!
  This method is used during the first write pass of a write action to
  count the number of references to this object in the scene graph.
*/
void
SoBase::addWriteReference(SoOutput * out, SbBool isfromfield)
{
  assert(out->getStage() == SoOutput::COUNT_REFS);

  int refcount = SoWriterefCounter::instance(out)->getWriteref(this);

#if COIN_DEBUG
  if (SoWriterefCounter::debugWriterefs()) {
    SoDebugError::postInfo("SoBase::addWriteReference",
                           "%p/%s/'%s': %d -> %d",
                           this, this->getTypeId().getName().getString(),
                           this->getName().getString(),
                           refcount, refcount + 1);
  }
#endif // COIN_DEBUG

  refcount++;

  if (!isfromfield) {
    SoWriterefCounter::instance(out)->setInGraph(this, TRUE);
  }
  SoWriterefCounter::instance(out)->setWriteref(this, refcount);
}

/*!
  Returns \c TRUE if this object should be written out during a write action.
  Will return \c FALSE if no references to this object has been made in the
  scene graph.

  Note that connections from the fields of fieldcontainer objects is not
  alone a valid reason for writing out the object -- there must also be at
  least one reference directly from another SoBase (like a parent/child
  relationship, for instance).

  This method will return a valid result only during the second pass of
  write actions.
*/
SbBool
SoBase::shouldWrite(void)
{
  return SoWriterefCounter::instance(NULL)->shouldWrite(this);
}

/*!
  \COININTERNAL
*/
void
SoBase::incrementCurrentWriteCounter(void)
{
  SoBase::writecounter++;
}

/*!
  \COININTERNAL
*/
void
SoBase::decrementCurrentWriteCounter(void)
{
  SoBase::writecounter--;
}

/*!
  Returns the object of \a type, or derived from \a type, registered
  under \a name. If several has been registered under the same name
  with the same type, returns the \e last one which was registered.

  If no object of a valid type or subtype has been registered with the
  given name, returns \c NULL.
*/
SoBase *
SoBase::getNamedBase(const SbName & name, SoType type)
{
  CC_MUTEX_LOCK(SoBaseP::name2obj_mutex);
  SbPList * l;
  if (SoBaseP::name2obj->get((const char *)name, l)) {
    if (l->getLength()) {
      SoBase * b = (SoBase *)((*l)[l->getLength() - 1]);
      if (b->isOfType(type)) {
        CC_MUTEX_UNLOCK(SoBaseP::name2obj_mutex);
        return b;
      }
    }
  }
  CC_MUTEX_UNLOCK(SoBaseP::name2obj_mutex);
  return NULL;
}

/*!
  Returns the number of objects of type \a type, or derived from \a type,
  registered under \a name.

  All matches will also be appended to \a baselist.
 */
int
SoBase::getNamedBases(const SbName & name, SoBaseList & baselist, SoType type)
{
  CC_MUTEX_LOCK(SoBaseP::name2obj_mutex);

  int matches = 0;

  SbPList * l;
  if (SoBaseP::name2obj->get((const char *)name, l)) {
    for (int i=0; i < l->getLength(); i++) {
      SoBase * b = (SoBase *)((*l)[i]);
      if (b->isOfType(type)) {
        baselist.append(b);
        matches++;
      }
    }
  }
  CC_MUTEX_UNLOCK(SoBaseP::name2obj_mutex);

  return matches;
}

/*!
  Read next SoBase derived instance from the \a in stream, check that
  it is derived from \a expectedtype and place a pointer to the newly
  allocated instance in \a base.

  \c FALSE is returned on read errors, mismatch with the \a
  expectedtype, or if there are attempts at referencing (through the
  \c USE keyword) unknown instances.

  If we return \c TRUE with \a base equal to \c NULL, three things
  might have happened:

  1. End-of-file. Use SoInput::eof() after calling this method to
  detect end-of-file conditions.

  2. \a in didn't have a valid identifier name at the stream for us to
  read. This happens either in the case of errors, or when all child
  nodes of a group has been read. Check if the next character in the
  stream is a '}' to detect the latter case.

  3. A child was given as the \c NULL keyword. This can happen when
  reading the contents of SoSFNode fields (note that NULL is not
  allowed for SoMFNode)

  If \c TRUE is returned and \a base is not \c NULL upon return, the
  instance was allocated and initialized according to what was read
  from the \a in stream.
*/
SbBool
SoBase::read(SoInput * in, SoBase *& base, SoType expectedtype)
{
  // FIXME: the interface design for this function is goddamn _awful_!
  // We need to keep it like this, though, to be compatible with SGI
  // Inventor. What we however /could/ do about it is:
  //
  // First, split out the SoBaseP class definition to a separate
  // interface, which can be accessed internally from Coin lirbary
  // code.
  //
  // Then, in this, write up /sensibly designed/ read()-function(s)
  // which implements the actually needed functionality of this
  // method.
  //
  // Third, make this function use those new functions in SoBaseP (to
  // avoid code duplication) -- and mangle the results so that this
  // function still conforms to the SGI Inventor behavior.
  //
  // Finally, start using the SoBaseP::read()-function(s) from
  // internal Coin code instead, to clean up the messy interaction
  // with this function from everywhere else.
  //
  // 20060202 mortene.

  assert(expectedtype != SoType::badType());
  base = NULL;

  SbName name;
  SbBool result = in->read(name, TRUE);

#if COIN_DEBUG
  if (SoInputP::debug()) {
    // This output is extremely useful when debugging the import code.
    SoDebugError::postInfo("SoBase::read",
                           "SoInput::read(&name, TRUE) => returns %s, name=='%s'",
                           result ? "TRUE" : "FALSE", name.getString());
  }
#endif // COIN_DEBUG

  // read all (vrml97) routes. Do this also for non-vrml97 files,
  // since in Coin we can have a mix of Inventor and VRML97 nodes in
  // the same file.
  while (result && name == ROUTE_KEYWORD) {
    result = SoBase::readRoute(in);
    // read next ROUTE keyword
    if (result ) result = in->read(name, TRUE);
    else return FALSE; // error while reading ROUTE
  }

  // The SoInput stream does not start with a valid base name. Return
  // TRUE with base==NULL.
  if (!result) return TRUE;

  // If no valid name / identifier string is found, the return value
  // from SbInput::read(SbName&,TRUE) _should_ also be FALSE.
  assert(name != "");

  if (name == USE_KEYWORD) result = SoBase::readReference(in, base);
  else if (name == NULL_KEYWORD) return TRUE;
  else result = SoBase::readBase(in, name, base);

  // Check type correctness.
  if (result) {
    assert(base);

    SoType type = base->getTypeId();
    assert(type != SoType::badType());

    if (!type.isDerivedFrom(expectedtype)) {
      SoReadError::post(in, "Type '%s' is not derived from '%s'",
                        type.getName().getString(),
                        expectedtype.getName().getString());
      result = FALSE;
    }
  }

  // Make sure we don't leak memory.
  if (!result && base && (name != USE_KEYWORD)) {
    base->ref();
    base->unref();
  }

#if COIN_DEBUG
  if (SoInputP::debug()) {
    SoDebugError::postInfo("SoBase::read", "done, name=='%s' baseptr==%p, result==%s",
                           name.getString(), base, result ? "TRUE" : "FALSE");
  }
#endif // COIN_DEBUG

  return result;
}

/*!
  Referenced instances of SoBase are written as "DEF NamePrefixNumber" when
  exported. "Name" is the name of the base instance from setName(), "Prefix"
  is common for all objects and can be set by this method (default is "+"),
  and "Number" is a unique id which is necessary if multiple objects have
  the same name.

  If you want the prefix to be something else than "+", use this method.
 */
void
SoBase::setInstancePrefix(const SbString & c)
{
  SoWriterefCounter::setInstancePrefix(c);
  (*SoBase::refwriteprefix) = c;
}

/*!
  Set to \c TRUE to activate debugging of reference counting, which
  could aid in finding hard to track down problems with accesses to
  freed memory or memory leaks. Note: this will produce lots of
  debug information in any "normal" running system, so use sensibly.

  The reference tracing functionality will be disabled in "release
  versions" of the Coin library.
 */
void
SoBase::setTraceRefs(SbBool trace)
{
  SoBase::tracerefs = trace;
}

/*!
  Return the status of the reference tracing flag.

  \sa setTraceRefs()
 */
SbBool
SoBase::getTraceRefs(void)
{
  return SoBase::tracerefs;
}

/*!
  Returns \c TRUE if this object will be written more than once upon
  export. Note that the result from this method is only valid during the
  second pass of a write action (and partly during the COUNT_REFS pass).
 */
SbBool
SoBase::hasMultipleWriteRefs(void) const
{
  return SoWriterefCounter::instance(NULL)->getWriteref(this) > 1;
}

// FIXME: temporary bug-workaround needed to test if we are exporting
// a VRML97 or an Inventor file. Implementation in SoOutput.cpp.
// pederb, 2003-03-18
extern SbString SoOutput_getHeaderString(const SoOutputP * out);

/*!
  Write out the header of any SoBase derived object. The header consists
  of the \c DEF keyword and the object name (if the object has a name,
  otherwise these will be skipped), the class name and a left bracket.

  Alternatively, the object representation may be made up of just the
  \c USE keyword plus the object name, if this is the second or subsequent
  reference written to the file.

  If the object has been completed just by writing the header (which will
  be the case if we're writing multiple references of an object),
  we return \c TRUE, otherwise \c FALSE.

  If we return \c FALSE (i.e. there's more to write), we will
  increment the indentation level.

  \sa writeFooter(), SoOutput::indent()
 */
SbBool
SoBase::writeHeader(SoOutput * out, SbBool isgroup, SbBool isengine) const
{
  if (!out->isBinary()) {
    out->write(END_OF_LINE);
    out->indent();
  }

  SbName name = this->getName();
  int refid = out->findReference(this);
  SbBool firstwrite = (refid == SoWriterefCounter::FIRSTWRITE);
  SbBool multiref = SoWriterefCounter::instance(out)->hasMultipleWriteRefs(this);
  SbName writename = SoWriterefCounter::instance(out)->getWriteName(this);

  // Write the node
  if (!firstwrite) {
    out->write(USE_KEYWORD);
    if (!out->isBinary()) out->write(' ');
    out->write(writename.getString());

    if (SoWriterefCounter::debugWriterefs()) {
      SbString tmp;
      tmp.sprintf(" # writeref: %d\n",
                  SoWriterefCounter::instance(out)->getWriteref(this));
      out->write(tmp.getString());
    }
  }
  else {
    if (name.getLength() || multiref) {
      out->write(DEF_KEYWORD);
      if (!out->isBinary()) out->write(' ');

      out->write(writename.getString());
      if (!out->isBinary()) out->write(' ');
    }

    if (this->isOfType(SoNode::getClassTypeId()) &&
        ((SoNode*)this)->getNodeType() == SoNode::VRML2) {
      SbString nodename(this->getFileFormatName());
      if (nodename.getLength() > 4) {
        SbString vrml = nodename.getSubString(0, 3);
        const char vrml2headerprefix[] = "#VRML V2.0 utf8";
        const size_t len = sizeof(vrml2headerprefix) - 1;
        const SbString fullheader = SoOutput_getHeaderString(out->pimpl);
        const SbString fileid = ((size_t)fullheader.getLength() < len) ?
          fullheader : fullheader.getSubString(0, len - 1);
        // FIXME: using a temporary workaround to test if we're
        // exporting a VRML97 file. pederb, 2003-03-18
        //
        // UPDATE 20060207 mortene: a better solution would be to
        // carry along the format information in the SoOutput (or an
        // internal private SoOutputP class?) as an enum or something,
        // methinks.
        if ((vrml == "VRML") && (fileid == vrml2headerprefix)) {
          SbString substring = nodename.getSubString(4);
          out->write(substring.getString());
        }
        else {
          out->write(nodename.getString());
        }
      }
      else {
        out->write(nodename.getString());
      }
    }
    else {
      out->write(this->getFileFormatName());
    }
    if (out->isBinary()) {
      unsigned int flags = 0x0;
      if (isgroup) flags |= SoBase::IS_GROUP;
      if (isengine) flags |= SoBase::IS_ENGINE;
      out->write(flags);
    }
    else {
      out->write(" {");
      if (SoWriterefCounter::debugWriterefs()) {
        SbString tmp;
        tmp.sprintf(" # writeref: %d\n",
                    SoWriterefCounter::instance(out)->getWriteref(this));
        out->write(tmp.getString());
      }
      out->write(END_OF_LINE);
      out->incrementIndent();
    }
  }
  
  int writerefcount = SoWriterefCounter::instance(out)->getWriteref(this);
  
#if COIN_DEBUG
  if (SoWriterefCounter::debugWriterefs()) {
    SoDebugError::postInfo("SoBase::writeHeader",
                           "%p/%s/'%s': %d -> %d",
                           this,
                           this->getTypeId().getName().getString(),
                           this->getName().getString(),
                           writerefcount, writerefcount - 1);
  }
#endif // COIN_DEBUG

  SoBase * thisp = (SoBase *)this;
  writerefcount--;
  SoWriterefCounter::instance(out)->setWriteref(this, writerefcount);

  // Don't need to write out the rest if we are writing anything but
  // the first instance.
  return (firstwrite == FALSE);
}

/*!
  This method will terminate the block representing an SoBase derived
  object.
 */
void
SoBase::writeFooter(SoOutput * out) const
{
  if (!out->isBinary()) {

    // Keep the old ugly-bugly formatting style around, in case
    // someone, for some obscure reason, needs it.
    static int oldstyle = -1;
    if (oldstyle == -1) {
      oldstyle = coin_getenv("COIN_OLDSTYLE_FORMATTING") ? 1 : 0;
    }

    // FIXME: if we last wrote a field, this EOF is superfluous -- so
    // we are getting a lot of empty lines in the files. Should
    // improve output formatting further. 20031223 mortene.
    if (!oldstyle) { out->write(END_OF_LINE); }

    out->decrementIndent();
    out->indent();
    out->write(CLOSE_BRACE);
  }
}

/*!
  Returns the class name this object should be written under.  Default
  string returned is the name of the class from the type system.

  User extensions nodes and engines override this method to return the
  name of the extension (instead of "UnknownNode" or "UnknownEngine").
 */
const char *
SoBase::getFileFormatName(void) const
{
  return this->getTypeId().getName().getString();
}

/*!
  \COININTERNAL
*/
uint32_t
SoBase::getCurrentWriteCounter(void)
{
  return SoBase::writecounter;
}

// Used to free the SbPLists in the name<->object dict.
void
SoBaseP::emptyName2ObjHash(const char * const &, SbPList * const & l, void *)
{
  delete l;
}

// OBSOLETE! Just kept around for Coin 2.x ABI compatibility.
void
SoBase::freeLists(unsigned long, void *)
{
  assert(FALSE);
}

// Reads the name of a reference after a "USE" keyword and finds the
// ptr to the object which is being referenced.
SbBool
SoBase::readReference(SoInput * in, SoBase *& base)
{
  SbName refname;
  if (!in->read(refname, FALSE)) {
    SoReadError::post(in, "Premature end of file after \"%s\"", USE_KEYWORD);
    return FALSE;
  }

  // This code to handles cases where USE ref name is
  // immediately followed by a "." and a fieldname, as can occur
  // when reading field-to-field connections.
  if (!in->isBinary()) {
    SbString refstr = refname.getString();

    // NOTE:
    // If the name ends with a }. E.g.
    //
    // USE mesh+0}
    //
    // then we are in trouble, but so is Open Inventor.
    // This is due to the ability for "}" to be a character
    // in the name of a node.
    const size_t index = strcspn(refstr.getString(), ".");
    SbString startstr = refstr.getSubString(0, (int)(index - 1));
    SbString endstr = refstr.getSubString((int)index);
    in->putBack(endstr.getString());

    refname = startstr;
  }

  if ((base = in->findReference(refname)) == NULL) {
    SoReadError::post(in, "Unknown reference \"%s\"", refname.getString());
    return FALSE;
  }

  // when referencing an SoProtoInstance, we need to return the proto
  // instance's root node, not the actual proto instance node.
  if (base->isOfType(SoProtoInstance::getClassTypeId())) {
    base = ((SoProtoInstance*) base)->getRootNode();
  }

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::readReference",
                         "USE: '%s'", refname.getString());
#endif // debug

  return TRUE;
}

// Read the SoBase instance.
SbBool
SoBase::readBase(SoInput * in, SbName & classname, SoBase *& base)
{
  assert(classname != "");

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::readBase", "classname: '%s'",
                         classname.getString());
#endif // debug

  SbBool ret = TRUE;
  base = NULL;

  SbName refname;

  if (in->isFileVRML2()) {
    if (classname == PROTO_KEYWORD ||
        classname == EXTERNPROTO_KEYWORD) { // special case to handle [EXTERN]PROTO definitions
      SoProto * proto = new SoProto(classname == EXTERNPROTO_KEYWORD);
      proto->ref();
      ret = proto->readInstance(in, 0);
      if (ret) {
        proto->unrefNoDelete();
        in->addProto(proto);
      }
      else {
        proto->unref();
        return FALSE;
      }
      base = proto;
      return TRUE;
    }
  }

  if (classname == DEF_KEYWORD) {
    if (!in->read(refname, FALSE) || !in->read(classname, TRUE)) {
      if (in->eof()) {
        SoReadError::post(in, "Premature end of file after %s", DEF_KEYWORD);
      }
      else {
        SoReadError::post(in, "Unable to read identifier after %s keyword", DEF_KEYWORD);
      }
      ret = FALSE;
    }

    if (!refname) {
      SoReadError::post(in, "No name given after %s", DEF_KEYWORD);
      ret = FALSE;
    }

    if (!classname) {
      SoReadError::post(in, "Invalid definition of %s", refname.getString());
      ret = FALSE;
    }
  }

  if (ret) {
    SbBool gotchar = FALSE; // Unnecessary, but kills a compiler warning.
    char c;
    if (!in->isBinary() && (!(gotchar = in->read(c)) || c != OPEN_BRACE)) {
      if (gotchar)
        SoReadError::post(in, "Expected '%c'; got '%c'", OPEN_BRACE, c);
      else
        SoReadError::post(in, "Expected '%c'; got EOF", OPEN_BRACE);
      ret = FALSE;
    }
    else {
      ret = SoBase::readBaseInstance(in, classname, refname, base);

      if (ret && !in->isBinary()) {
        if (!(gotchar = in->read(c)) || c != CLOSE_BRACE) {
          if (gotchar)
            SoReadError::post(in, "Expected '%c'; got '%c' for %s", CLOSE_BRACE, c, classname.getString());
          else
            SoReadError::post(in, "Expected '%c'; got EOF for %s", CLOSE_BRACE, classname.getString());
          ret = FALSE;
        }
      }
    }
  }

  return ret;
}

// Read the SoBase instance.
SbBool
SoBase::readBaseInstance(SoInput * in, const SbName & classname,
                         const SbName & refname, SoBase *& base)
{
  assert(classname != "");

  SbBool needupgrade = FALSE;

  // first, try creating an upgradable node, based on the version of
  // the input file.
  base = SoUpgrader::tryCreateNode(classname, in->getIVVersion());
  if (base) {
    // we need to upgrade the node after reading it
    needupgrade = TRUE;
  }
  else {
    // create normal Coin node
    base = SoBase::createInstance(in, classname);
  }

  if (!base) { goto failed; }

  if (!(!refname)) {
    // Set up new entry in reference hash -- with full name.
    in->addReference(refname, base);

    // Remove reference counter suffix, if any (i.e. "goldsphere+2"
    // becomes "goldsphere").
    SbString instancename = refname.getString();
    const char * strp = instancename.getString();
    const char * occ = strstr(strp, SoBase::refwriteprefix->getString());

    if (occ != strp) { // They will be equal if the name is only a refcount.
      const ptrdiff_t offset = occ - strp - 1;
      if (occ) instancename = instancename.getSubString(0, (int)offset);
      // Set name identifier for newly created SoBase instance.
      base->setName(instancename);
    }
  }

  // The "flags" argument to readInstance is only checked during
  // import from binary format files.
  {
    unsigned short flags = 0;
    if (in->isBinary() && (in->getIVVersion() > 2.0f)) {
      const SbBool ok = in->read(flags);
      if (!ok) { goto failed; }
    }

    const SbBool ok = base->readInstance(in, flags);
    if (!ok) { goto failed; }
  }

  // Make sure global fields are unique
  if (base->isOfType(SoGlobalField::getClassTypeId())) {
    SoGlobalField * globalfield = (SoGlobalField *)base;

    // The global field is removed from the global field list
    // because we have to check if there is already a global field
    // in the list with the same name.  This is because
    // SoGlobalField's constructor automatically adds itself to the
    // list of global fields without checking if the field already
    // exists.
    globalfield->ref(); // increase refcount to 1, so the next call will not destruct the node
    SoGlobalField::removeGlobalFieldContainer(globalfield);
    globalfield->unrefNoDelete(); // corrects ref count back to zero

    // A read-error sanity check should have been done in
    // SoGlobalField::readInstance().
    assert(globalfield->getFieldData()->getNumFields() == 1);

    // Now, see if the global field is in the database already.
    SoField * f = SoDB::getGlobalField(globalfield->getName());
    if (f) {
      SoField * basefield = globalfield->getFieldData()->getField(globalfield, 0);
      assert(basefield && "base (SoGlobalField) does not appear to have a field");

      if (!f->isOfType(basefield->getClassTypeId())) {
        SoReadError::post(in, "Types of equally named global fields do not match: existing: %s, new: %s",
                          f->getTypeId().getName().getString(), basefield->getTypeId().getName().getString());
        goto failed;
      }

      SoGlobalField * container = (SoGlobalField *)f->getContainer();

      // Copy new field values into the existing field. Open Inventor
      // apparently does not copy the new values into the old field,
      // but it seems logical to do so.
      SoFieldContainer::initCopyDict();
      container->copyFieldValues(globalfield, TRUE); // Assign new global field values to old global field
      SoFieldContainer::copyDone();

      // Make sure to update the mapping in SoInput if necessary
      if (!(!refname)) {
        // Set up new entry in reference hash -- with full name.
        in->removeReference(refname);
        in->addReference(refname, container);
      }

      // Remove newly made SoGlobalField, use the existing one instead.
      // Add it to the global field list before deleting it (we
      // manually removed it earlier to test it the field was already
      // in the database)
      SoGlobalField::addGlobalFieldContainer((SoGlobalField*) base);
      base->ref(); base->unref(); // this will delete the global field, and remove it from the database
      base = container;
      container->getFieldData()->getField(container, 0)->touch();
    }
    else {
      // The global field was first removed to check the existence
      // of an equal named item. If no such global field exists, the
      // removed global field has to be added again, which is done
      // by this code:
      SoGlobalField::addGlobalFieldContainer(globalfield);
    }
  }

  if (needupgrade) {
    SoBase * oldbase = base;
    oldbase->ref();
    base = SoUpgrader::createUpgrade(oldbase);
    assert(base && "should never happen (since needupgrade == TRUE)");
    oldbase->unref();
  }

  if (base->isOfType(SoProtoInstance::getClassTypeId())) {
    base = ((SoProtoInstance*) base)->getRootNode();
  }

  return TRUE;

failed:
  if (base) {
    if (!(!refname)) { in->removeReference(refname); }

    base->ref();
    base->unref();
    base = NULL;
  }

  return FALSE;
}

// Create a new instance of the "classname" type.
SoBase *
SoBase::createInstance(SoInput * in, const SbName & classname)
{
  assert(classname != "");

  SoType type = SoType::badType();
  if (in->isFileVRML2()) {
    SbString newname;
    newname.sprintf("VRML%s", classname.getString());
    type = SoType::fromName(SbName(newname.getString()));
#if COIN_DEBUG && 0 // debug
    if (type != SoType::badType()) {
      SoDebugError::postInfo("SoBase::createInstance",
                             "Created VRML V2.0 type: %s",
                             type.getName().getString());
    }
#endif // debug
  }

  // search for PROTO in current SoInput instance
  SoProto * proto = in->findProto(classname);
  if (!proto) {
    // search in global PROTO list
    proto = SoProto::findProto(classname);
  }
  if (proto) return proto->createProtoInstance();

  if (type == SoType::badType())
    type = SoType::fromName(classname);

  SoBase * instance = NULL;

  if (type == SoType::badType() ||
      type == SoUnknownNode::getClassTypeId()) {
    // Default to SoUnknownNode for now.. FIXME: what if we're dealing
    // with an unknown engine? 20000105 mortene.
    SoUnknownNode * unknownnode = new SoUnknownNode;
    unknownnode->setNodeClassName(classname);
    instance = unknownnode;
    if (SoInputP::debug()) {
      SoDebugError::postInfo("SoBase::createInstance",
                             "created SoUnknownNode for '%s'",
                             classname.getString());
    }
  }
  else if (!type.canCreateInstance()) {
    SoReadError::post(in, "Class \"%s\" is abstract", classname.getString());
  }
  else {
    instance = (SoBase *)type.createInstance();
  }

  return instance;
}

// Hmm.
void
SoBase::flushInput(SoInput * in)
{
#if 0 // FIXME: obsoleted, see comment at the end of SoBase::readBase(). 20020531 mortene.
  assert(FALSE);
#else // obsoleted
  assert(!in->isBinary());

  int nestlevel = 1;
  char c;

  while (nestlevel > 0 && in->read(c)) {
    if (c == CLOSE_BRACE) nestlevel--;
    else if (c == OPEN_BRACE) nestlevel++;
  }
#endif // obsoleted
}

/*!
  Connect a route from the node named \a fromnodename's field \a
  fromfieldname to the node named \a tonodename's field \a
  tofieldname. This method will consider the fields types (event in,
  event out, etc) when connecting.

  \COIN_FUNCTION_EXTENSION

  \since Coin 2.0
*/
SbBool
SoBase::connectRoute(SoInput * in,
                     const SbName & fromnodename, const SbName & fromfieldname,
                     const SbName & tonodename, const SbName & tofieldname)
{
  SoNode * fromnode = SoNode::getByName(fromnodename);
  SoNode * tonode = SoNode::getByName(tonodename);
  if (fromnode && tonode) {
    SoDB::createRoute(fromnode, fromfieldname.getString(), 
                      tonode, tofieldname.getString());
    return TRUE;
  }
  return FALSE;
}

/*!
  \COININTERNAL

  Reads a (VRML97) ROUTE. We decided to also add support for routes in
  Coin, as a generic feature, since we think it is nicer than setting
  up field connections inside the nodes.

*/
SbBool
SoBase::readRoute(SoInput * in)
{
  SbString fromstring, tostring;

  SbName fromnodename;
  SbName fromfieldname;
  SbName toname;
  SbName tonodename;
  SbName tofieldname;
  SbBool ok;

  ok =
    in->read(fromstring) &&
    in->read(toname) &&
    in->read(tostring);

  if (ok) ok = (toname == SbName("TO"));

  if (ok) {
    ok = FALSE;

    // parse from-string
    char * str1 = (char*) fromstring.getString();
    char * str2 = str1 ? (char*) strchr(str1, '.') : NULL;
    if (str1 && str2) {
      *str2++ = 0;

      // now parse to-string
      fromnodename = str1;
      fromfieldname = str2;
      str1 = (char*) tostring.getString();
      str2 = str1 ? strchr(str1, '.') : NULL;
      if (str1 && str2) {
        *str2++ = 0;
        tonodename = str1;
        tofieldname = str2;

        ok = TRUE;
      }
    }
  }

#if COIN_DEBUG && 0 // debug
  SoDebugError::postInfo("SoBase::readRoute",
                         "%s.%s %s %s.%s",
                         fromnodename.getString(),
                         fromfieldname.getString(),
                         toname.getString(),
                         tonodename.getString(),
                         tofieldname.getString());
#endif // debug

  if (!ok) SoReadError::post(in, "Error parsing ROUTE keyword");
  else {
    SoProto * proto = in->getCurrentProto();
    if (proto) {
      proto->addRoute(fromnodename, fromfieldname, tonodename, tofieldname);
    }
    else {
      SoNode * fromnode = SoNode::getByName(fromnodename);
      SoNode * tonode = SoNode::getByName(tonodename);

      if (!fromnode || !tonode) {
        SoReadError::post(in,
                          "Unable to create ROUTE from %s.%s to %s.%s. "
                          "Delaying.",
                          fromnodename.getString(), fromfieldname.getString(),
                          tonodename.getString(), tofieldname.getString());
        in->addRoute(fromnodename, fromfieldname, tonodename, tofieldname);
      }

      (void)SoBase::connectRoute(in, fromnodename, fromfieldname,
                                 tonodename, tofieldname);
    }
  }
  return ok;
}

//
// private method that sends a notify to auditor based on type
//
void
SoBase::doNotify(SoNotList * l, const void * auditor, const SoNotRec::Type type)
{
  l->setLastType(type);
  switch (type) {
  case SoNotRec::CONTAINER:
  case SoNotRec::PARENT:
    {
      SoFieldContainer * obj = (SoFieldContainer *)auditor;
      obj->notify(l);
    }
    break;

  case SoNotRec::SENSOR:
    {
      SoDataSensor * obj = (SoDataSensor *)auditor;
#if COIN_DEBUG && 0 // debug
      SoDebugError::postInfo("SoAuditorList::notify",
                             "notify and schedule sensor: %p", obj);
#endif // debug
      // don't schedule the sensor here. The sensor instance will do
      // that in notify() (it might also choose _not_ to schedule),
      obj->notify(l);
    }
    break;

  case SoNotRec::FIELD:
  case SoNotRec::ENGINE:
    {
      // We used to check whether or not the fields was already
      // dirty before we transmitted the notification
      // message. This is _not_ correct (the dirty flag is
      // conceptually only relevant for whether or not to do
      // re-evaluation), so don't try to "optimize" the
      // notification mechanism by re-introducing that "feature".
      // :^/
      ((SoField *)auditor)->notify(l);
    }
    break;

  default:
    assert(0 && "Unknown auditor type");
  }
}

/*!
  \internal
  \since Coin 2.3
*/
void
SoBase::staticDataLock(void)
{
  CC_MUTEX_LOCK(SoBaseP::global_mutex);
}

/*!
  \internal
  \since Coin 2.3
*/
void
SoBase::staticDataUnlock(void)
{
  CC_MUTEX_UNLOCK(SoBaseP::global_mutex);
}


