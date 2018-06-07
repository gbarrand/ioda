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
  \class SoKeyboardEvent SoKeyboardEvent.h Inventor/events/SoKeyboardEvent.h
  \brief The SoKeyboardEvent class contains information about
  keyboard interaction.
  \ingroup events

  When the user presses any keys on the keyboard, these will be translated
  from a system specific event into a Coin event and sent to the
  scenegraph by using instances of this class.

  \sa SoEvent, SoButtonEvent, SoMouseButtonEvent, SoSpaceballButtonEvent
  \sa SoEventCallback, SoHandleEventAction
*/

// *************************************************************************

#include <Inventor/events/SoKeyboardEvent.h>

#include <Inventor/C/tidbitsp.h>
#include <Inventor/misc/SbHash.h>
#include <Inventor/SbName.h>
#include <assert.h>

// *************************************************************************

// Avoid problem with Microsoft Win32 API headers (yes, they actually
// #define DELETE somewhere in their header files).
#undef DELETE


/*
  FIXME: The keycode handling in SoKeyboardEvent is really terrible --
  could we improve it while still keeping Open Inventor API
  compatibility?  -mortene.
*/



/*!
  \enum SoKeyboardEvent::Key
  This enum contains all keys detected by Coin.
 */

/*!
  \def SO_KEY_PRESS_EVENT(EVENT, KEY)
  This macro evaluates to \c TRUE iff the \c EVENT represents a press on the
  given \c KEY.
*/
/*!
  \def SO_KEY_RELEASE_EVENT(EVENT, KEY)
  This macro evaluates to \c TRUE iff the \c EVENT represents a release of the
  given \c KEY.
*/

static SbHash<char, int> * converttoprintable = NULL;
static SbHash<char, int> * converttoprintable_shift = NULL;

static void
sokeyboardevent_cleanup(void)
{
  delete converttoprintable;
  converttoprintable = NULL;
  delete converttoprintable_shift;
  converttoprintable_shift = NULL;
}

static void
build_convert_dicts(void)
{
  int i;
  converttoprintable = new SbHash<char, int>();
  converttoprintable_shift = new SbHash<char, int>();
  coin_atexit((coin_atexit_f *)sokeyboardevent_cleanup, CC_ATEXIT_NORMAL);

#undef ADD_KEY // just in case
#define ADD_KEY(x,y) d->put(SoKeyboardEvent::x, y)

  // shift not down
  SbHash<char, int> * d = converttoprintable;
  ADD_KEY(NUMBER_0, '0');
  ADD_KEY(NUMBER_1, '1');
  ADD_KEY(NUMBER_2, '2');
  ADD_KEY(NUMBER_3, '3');
  ADD_KEY(NUMBER_4, '4');
  ADD_KEY(NUMBER_5, '5');
  ADD_KEY(NUMBER_6, '6');
  ADD_KEY(NUMBER_7, '7');
  ADD_KEY(NUMBER_8, '8');
  ADD_KEY(NUMBER_9, '9');

  ADD_KEY(PAD_0, '0');
  ADD_KEY(PAD_1, '1');
  ADD_KEY(PAD_2, '2');
  ADD_KEY(PAD_3, '3');
  ADD_KEY(PAD_4, '4');
  ADD_KEY(PAD_5, '5');
  ADD_KEY(PAD_6, '6');
  ADD_KEY(PAD_7, '7');
  ADD_KEY(PAD_8, '8');
  ADD_KEY(PAD_9, '9');

  ADD_KEY(PAD_ADD, '+');
  ADD_KEY(PAD_SUBTRACT, '-');
  ADD_KEY(PAD_MULTIPLY, '*');
  ADD_KEY(PAD_DIVIDE, '/');
  ADD_KEY(PAD_SPACE, ' ');

  ADD_KEY(SPACE, ' ');
  ADD_KEY(APOSTROPHE, '\'');
  ADD_KEY(COMMA, ',');
  ADD_KEY(MINUS, '-');
  ADD_KEY(PERIOD, '.');
  ADD_KEY(SLASH, '/');
  ADD_KEY(SEMICOLON, ';');
  ADD_KEY(EQUAL, '=');
  ADD_KEY(BRACKETLEFT, '[');
  ADD_KEY(BACKSLASH, '\\');
  ADD_KEY(BRACKETRIGHT,']');
  ADD_KEY(GRAVE,'`');

  for (i = (int) SoKeyboardEvent::A; i <= (int) SoKeyboardEvent::Z; i++) {
    d->put(i, ('a' + i - (int) SoKeyboardEvent::A));
  }

  // shift down
  d = converttoprintable_shift;
  ADD_KEY(NUMBER_0, ')');
  ADD_KEY(NUMBER_1, '!');
  ADD_KEY(NUMBER_2, '@');
  ADD_KEY(NUMBER_3, '#');
  ADD_KEY(NUMBER_4, '$');
  ADD_KEY(NUMBER_5, '%');
  ADD_KEY(NUMBER_6, '^');
  ADD_KEY(NUMBER_7, '&');
  ADD_KEY(NUMBER_8, '*');
  ADD_KEY(NUMBER_9, '(');

  ADD_KEY(PAD_0, '0');
  ADD_KEY(PAD_1, '1');
  ADD_KEY(PAD_2, '2');
  ADD_KEY(PAD_3, '3');
  ADD_KEY(PAD_4, '4');
  ADD_KEY(PAD_5, '5');
  ADD_KEY(PAD_6, '6');
  ADD_KEY(PAD_7, '7');
  ADD_KEY(PAD_8, '8');
  ADD_KEY(PAD_9, '9');

  ADD_KEY(PAD_ADD, '+');
  ADD_KEY(PAD_SUBTRACT, '-');
  ADD_KEY(PAD_MULTIPLY, '*');
  ADD_KEY(PAD_DIVIDE, '/');
  ADD_KEY(PAD_SPACE, ' ');

  ADD_KEY(SPACE, ' ');
  ADD_KEY(APOSTROPHE, '\"');
  ADD_KEY(COMMA, '<');
  ADD_KEY(MINUS, '_');
  ADD_KEY(PERIOD, '>');
  ADD_KEY(SLASH, '?');
  ADD_KEY(SEMICOLON, ':');
  ADD_KEY(EQUAL, '+');
  ADD_KEY(BRACKETLEFT, '{');
  ADD_KEY(BACKSLASH, '|');
  ADD_KEY(BRACKETRIGHT,'}');
  ADD_KEY(GRAVE,'~');

  for (i = (int) SoKeyboardEvent::A; i <= (int) SoKeyboardEvent::Z; i++) {
    d->put(i, ('A' + i - (int) SoKeyboardEvent::A));
  }
#undef ADD_KEY
}

// *************************************************************************

SO_EVENT_SOURCE(SoKeyboardEvent);

// *************************************************************************

/*!
  Initialize the type information data.
 */
void
SoKeyboardEvent::initClass(void)
{
  SO_EVENT_INIT_CLASS(SoKeyboardEvent, SoButtonEvent);
}

/*!
  Constructor.
 */
SoKeyboardEvent::SoKeyboardEvent(void)
{
  this->key = SoKeyboardEvent::ANY;
  this->isprintableset = 0;
}

/*!
  Destructor.
*/
SoKeyboardEvent::~SoKeyboardEvent()
{
}

/*!
  Set the value of the key which the user interacted with.

  This method is used from the window specific device classes when
  translating events to the generic Coin library.

  \sa getKey()
 */
void
SoKeyboardEvent::setKey(SoKeyboardEvent::Key keyarg)
{
  this->key = keyarg;
  this->isprintableset = 0;
}

/*!
  Returns the value of the key which was pressed or released.

  Coin adds a new key value called UNDEFINED. This is needed to
  support GUI toolkits where it's not possible to find exactly which
  key is pressed, and/or to support non-US keyboards. The Open
  Inventor design for this class is flawed, since it assumes everybody
  uses a US keyboard. We recommend using getPrintableCharacter() to
  find which key is pressed/released, at least for printable
  non-alphanumerical characters.

  \sa getPrintableCharacter(), getState()
  \sa wasShiftDown(), wasCtrlDown(), wasAltDown(), getPosition(), getTime()
*/
SoKeyboardEvent::Key
SoKeyboardEvent::getKey(void) const
{
  return this->key;
}

/*!
  Convenience method for quickly checking if the given event is a
  keypress on the given key, \c whichKey.

  \sa isKeyReleaseEvent(), isOfType(), getKey(), getState()
 */
SbBool
SoKeyboardEvent::isKeyPressEvent(const SoEvent * e,
                                 SoKeyboardEvent::Key whichKey)
{
  return (e->isOfType(SoKeyboardEvent::getClassTypeId()) &&
          (whichKey == SoKeyboardEvent::ANY ||
           ((SoKeyboardEvent *)e)->getKey() == whichKey) &&
          ((SoButtonEvent *)e)->getState() == SoButtonEvent::DOWN);
}

/*!
  Convenience method for quickly checking if the given event is a
  key release of the given key, \c whichKey.

  \sa isKeyPressEvent(), isOfType(), getKey(), getState()
 */
SbBool
SoKeyboardEvent::isKeyReleaseEvent(const SoEvent * e,
                                   SoKeyboardEvent::Key whichKey)
{
  return (e->isOfType(SoKeyboardEvent::getClassTypeId()) &&
          (whichKey == SoKeyboardEvent::ANY ||
           ((SoKeyboardEvent *)e)->getKey() == whichKey) &&
          ((SoButtonEvent *)e)->getState() == SoButtonEvent::UP);
}

/*!
  Sets the printable character for this keyboard event. If this method
  is not called when creating an event, getPrintableCharacter() will
  convert the SoKeyboardEvent::Key value into a printable character.
  This conversion does not work on non-US keyboards, so we recommend
  that you set the printable character using this method instead.

  This printable character is cleared each time setKey() is called.

  This method is an extension versus the Open Inventor API.

  \sa getPrintableCharacter()
*/
void
SoKeyboardEvent::setPrintableCharacter(const char c)
{
  this->printable = c;
  this->isprintableset = 1;
}

/*!
  Return ASCII value which would be generated by the key and
  modifier combination.

  NB! If setPrintableCharacter() hasn't been called, this function
  does not always work as expected, particularly not on non-US
  keyboards. The Coin GUI toolkits (SoGtk/SoQt/SoWin/SoXt/Sc21) will
  set the printable character correctly.

  \sa getKey(), wasShiftDown(), wasCtrlDown(), wasAltDown(), setPrintableCharacter()
*/
char
SoKeyboardEvent::getPrintableCharacter(void) const
{
  if (this->isprintableset) return this->printable;

  if (converttoprintable == NULL) {
    build_convert_dicts();
  }

  SbHash<char, int> * dict =
    this->wasShiftDown() ? converttoprintable_shift : converttoprintable;
  char value;
  if (dict->get(this->getKey(), value)) { return value; }
  return '.';
}
