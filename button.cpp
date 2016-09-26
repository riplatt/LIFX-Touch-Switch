/*
 * button.cpp - button function library.
 */
#include "button.h"

button::button()
{

}

void button::check(bool buttonValue, unsigned long now)
{
  //int event = 0;
  _now = now;
  // Read the state of the button
  _buttonValue = buttonValue;
  // Button pressed down
  if (_buttonValue == false && _buttonLast == true && (_now - _upTime) > _debounce)
  {
    _downTime = _now;
    _ignoreUp = false;
    _waitForUp = false;
    _singleOK = true;
    _holdEventPast = false;
    _longHoldEventPast = false;
    if ((_now - _upTime) < _DCgap && _DConUp == false && _DCwaiting == true){
      _DConUp = true;
    } else {
      _DConUp = false;
    }
    _DCwaiting = false;
  }
  // Button released
  else if (_buttonValue == true && _buttonLast == false && (_now - _downTime) > _debounce)
  {
    if (not _ignoreUp)
    {
      _upTime = _now;
      if (_DConUp == false)
      {
        _DCwaiting = true;
      } else {
        //event = 2;
        setDoubleClick(true);
        _DConUp = false;
        _DCwaiting = false;
        _singleOK = false;
      }
    }
  }
  // Test for normal click event: _DCgap expired
  if ( _buttonValue == true && (_now - _upTime) >= _DCgap && _DCwaiting == true && _DConUp == false && _singleOK == true)
  {
    //event = 1;
    setClick(true);
    _DCwaiting = false;
  }
  // Test for hold
  if (_buttonValue == false && (_now - _downTime) >= _holdTime)
  {
    // Trigger "normal" hold
    if (not _holdEventPast) {
      //event = 3;
      setHold(true);
      _waitForUp = true;
      _ignoreUp = true;
      _DConUp = false;
      _DCwaiting = false;
      _holdEventPast = true;
    }

    // Trigger "long" hold
    if ((_now - _downTime) >= _longHoldTime)
    {
      if (not _longHoldEventPast)
      {
        //event = 4;
        setLongHold(true);
        _longHoldEventPast = true;
      }
    }
  }
  _buttonLast = _buttonValue;
  //return event;
}

void button::setClick(bool state)
{
  _click = state;
}

bool button::getClick()
{
  return _click;
}

void button::setDoubleClick(bool state)
{
  _doubleClick = state;
}

bool button::getDoubleClick()
{
  return _doubleClick;
}

void button::setHold(bool state)
{
  _hold = state;
}

bool button::getHold()
{
  return _hold;
}

void button::setLongHold(bool state)
{
  _longHold = state;
}

bool button::getLongHold()
{
  return _longHold;
}
