/*
 * button.h - button function library.
 */
#ifndef _button_h
  #define _button_h

  /* includes */
  //#include "common.h"

  class button
  {
    public:
      /* Member Functions */
      button();
      void check(bool buttonValue, unsigned long now);
      void setClick(bool state);
      bool getClick();
      void setDoubleClick(bool state);
      bool getDoubleClick();
      void setHold(bool state);
      bool getHold();
      void setLongHold(bool state);
      bool getLongHold();
      
    private:
      /* Members */
      // Button timing variables
      int _debounce = 20;              // ms _debounce period to prevent flickering when pressing or releasing the button
      int _DCgap = 250;                // max ms between clicks for a double click event
      int _holdTime = 1500;            // ms hold period: how long to wait for press+hold event
      int _longHoldTime = 5000;         // ms long hold period: how long to wait for press+hold event

      // Other button variables
      bool _buttonValue = true;        // value read from button
      bool _buttonLast = true;         // buffered value of the button's previous state
      bool _DCwaiting = false;         // whether we're waiting for a double click (down)
      bool _DConUp = false;            // whether to register a double click on next release, or whether to wait and click
      bool _singleOK = true;           // whether it's OK to do a single click
      unsigned long _downTime = 0;     // time the button was pressed down
      unsigned long _upTime = 0;       // time the button was released
      unsigned long _now = 0;
      bool _ignoreUp = false;          // whether to ignore the button release because the click+hold was triggered
      bool _waitForUp = false;         // when held, whether to wait for the up event
      bool _holdEventPast = false;     // whether or not the hold event happened already
      bool _longHoldEventPast = false; // whether or not the long hold event happened already

      // Button Flags
      bool _click;
      bool _doubleClick;
      bool _hold;
      bool _longHold;
  };

#endif
