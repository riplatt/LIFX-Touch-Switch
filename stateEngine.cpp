/*
 *  stateEngine.cpp - State Engine to control actions.
 */
#include "stateEngine.h"

stateEngine::stateEngine()
{

}

void stateEngine::initialise()
{
    _touched = false;
    _released = false;
    _state = -1;
}

void stateEngine::update(uint32_t now)
{
    _now = now;
    lastUpdate = _now;

    //Update 1 Finger Variables
    if ((_state >= 1000) && (_state < 2000))
    {
        _update1();
    }
    //Update 2 Finger Variables
    if ((_state >= 2000) && (_state < 3000))
    {
        _update2();
    }


    // Serial.printlnf("   State: %ld", _state);
	switch (_state)
    {
        case -1:
            // debouce touch
            if (_now - _touchTime > _debounceTime)
            {
                switch (_event.numberOfFingers)
                {
            		case 0:
            		    //_state = 0;
            		    _touched = false;
            		    _released = true;
            		    // Reset
            		    _hold = false;
            		    break;
            		case 1:
            		    _state = 1000;
            		    break;
            		case 2:
            		    _state = 2000;
            			break;
            		case 3:
            		    break;
            		case 4:
            		    break;
            		case 5:
            		    break;
                    default:
                        break;
                }
            }
            break;
		case 1000: //1 Finger Touch
		    // Serial.printlnf("%lu -  State:%ld", _now, _state);
		    /*if(_testHold())
		    {
		        _state = 1100;
		        _hold = true;
		    }*/

		    if(_testRelease())
		    {
		        _state = 1200;
		        _relaseTime = _now;
		    }

		    if(_testMovement())
		    {
		        _state = 1300;
		    }

		    if(_testForSecondFinger())
		    {
		        _state = 2000;
		    }
		    break;
		case 1100: //1 Finger Touch Hold Event
		    // Serial.printlnf("%lu -  State:%ld - Hold...", _now, _state);
		    if(_testRelease())
		    {
		        _hold = false;
		        _state = -1;
		    }
			break;
		case 1200: //1 Finger Touch Tap or Double Event
		    // Serial.printlnf("%lu -  State:%ld", _now, _state);
		    if(_testTouch())
		    {
                _state = 1210;
		    }

		    if(_testWaitTime()) //1 Finger Tap Event
		    {
		        _state = 1220;
		        _tap = true;
		    }
			break;
		case 1210: //1 Finger Double Event
		    // Serial.printlnf("%lu -  State:%ld - Double Tap...", _now, _state);
		    if(_testRelease())
		    {
                _state = 1215;
                _doubleTap = true;
		    }
			break;
		case 1215:
		    // Serial.printlnf("%lu -  State:%ld", _now, _state);
		    if(_doubleTap == false)
		    {
		        _state = -1;
		    }
			break;
		case 1220:
		    // Serial.printlnf("%lu -  State:%ld - Tap...", _now, _state);
		    if(_tap == false)
		    {
		        _state = -1;
		    }
			break;
		case 1300:
		    // Serial.printlnf("%lu -  State:%ld", _now, _state);
		    if(abs(_deltaXPercent) > abs(_deltaYPercent))
		    {
		        _state = 1310;
		        _movingXAxis = true;
		        _movingYAxis = false;
		    } else if (abs(_deltaXPercent) < abs(_deltaYPercent)) {
		        _state = 1320;
		        _movingYAxis = true;
		        _movingXAxis = false;
		    }

		    if(_testRelease())
		    {
                _state = -1;
		    }
			break;
		case 1310: // Moving in the X axis
		    // Serial.printlnf("%lu -  State:%ld - Moving on X Axis...", _now, _state);
		    //Serial.printlnf("%lu -  Delta Precent x:%f", _now, _deltaXPercent);
		    if(_testRelease())
		    {
                _state = -1;
                _moving = false;
                _movingXAxis = false;
		    }
			break;
		case 1320: // Moving in the Y axis
		    // Serial.printlnf("%lu -  State:%ld - Moving on Y Axis...", _now, _state);
		    //Serial.printlnf("%lu -  Delta Precent y:%f", _now, _deltaYPercent);
		    if(_testRelease())
		    {
                _state = -1;
                _moving = false;
                _movingYAxis = false;
		    }
			break;
		case 2000:
		    // Serial.printlnf("%lu -  State:%ld - Two Fingers...", _now, _state);
		    if((abs(_diffXPercent) >= 60.0) && (abs(_diffYPercent) >= 60.0))
		    {
		         _state = 2100;
		         _claim = true;
		    }

			if(_testMovement())
			{
			    _state = 2300;
			}

			if(_testRelease())
		    {
                _state = -1;
		    }
		    break;
		case 2100:
		    // Serial.printlnf("%lu -  State:%ld - Claim....", _now, _state);
		    if(_testRelease() && _claim == false)
		    {
                _state = -1;
		    }
			break;
		case 2300:
		    Serial.printlnf(Time.timeStr() + " - State: %ld - Two Finger Moving...", _state);
		    if(abs(_deltaXPercent) > abs(_deltaYPercent))
		    {
		        _state = 2310;
		        _movingXAxis2 = true;
		        _movingYAxis2 = false;
		    } else if (abs(_deltaXPercent) < abs(_deltaYPercent)) {
		        _state = 2320;
		        _movingYAxis2 = true;
		        _movingXAxis2 = false;
		    }

		    if(_testRelease())
		    {
		        _moving = false;
                _state = -1;
		    }
		    break;
		case 2310: // Moving in the X axis
		    if(_testRelease())
		    {
                _state = -1;
                _moving = false;
                _movingXAxis2 = false;
		    }
		    break;
		case 2320: // Moving in the Y axis
		    if(_testRelease())
		    {
                _state = -1;
                _moving = false;
                _movingYAxis2 = false;
		    }
		    break;
		case 3000:
		    break;
		case 4000:
		    break;
		case 5000:
		    break;
        default:
            break;
    }
    if(_state != _lastState)
    {
        Serial.printlnf(Time.timeStr() + " - State: %ld", _state);
        _lastState = _state;
    }
}

void stateEngine::addEvent(const struct touchScreenEvent &event)
{
    _event = event;

    if ((_touched == false) && (_event.numberOfFingers > 0))
    {
        // Serial.printlnf("%lu -  First Touch...", millis());
        _lastX1 = _event.fingerPositions[0].x;
        _lastY1 = _event.fingerPositions[0].y;
        _lastX2 = _event.fingerPositions[1].x;
        _lastY2 = _event.fingerPositions[1].y;
        _lastTimeStamp = _event.timeStamp;

        //Serial.printlnf("   x1:%d, Last x1:%d, x2:%d, Last x2:%d", _event.fingerPositions[0].x, _lastX1, _event.fingerPositions[1].x, _lastX2);
        //Serial.printlnf("   y1:%d, Last y1:%d, x2:%d, Last x2:%d", _event.fingerPositions[0].y, _lastY1, _event.fingerPositions[1].y, _lastY2);

        _touched = true;
        _released = false;
        _touchTime = millis();

    }

    update(millis());
}

void stateEngine::_update1()
{
    //Serial.printlnf("   Update1...");
    _deltaX = _event.fingerPositions[0].x - _lastX1;
    _deltaY = _event.fingerPositions[0].y - _lastY1;

    _deltaXPercent = ((float)_deltaX / (float)_resolutionX) * 100.00;
    _deltaYPercent = ((float)_deltaY / (float)_resolutionY) * 100.00;
    _deltaTime = (float)_event.timeStamp - (float)_lastTimeStamp;
    _distance = sqrt((_deltaX * _deltaX) + (_deltaY * _deltaY));
    _angle = atan2((float)_deltaY, (float)_deltaX) * 360 / M_PI;
    _velocity = _distance / _deltaTime;
    _velocityX = _deltaX / _deltaTime;
    _velocityY = _deltaY / _deltaTime;

    //Serial.printlnf("   x:%d, Last x:%d, Delta x:%f, Delta Precent x:%f", _event.fingerPositions[0].x, _lastX1, _deltaX, _deltaXPercent);
    //Serial.printlnf("   y:%d, Last y:%d, Delta y:%f, Delta Precent y:%f", _event.fingerPositions[0].y, _lastX1, _deltaY, _deltaYPercent);

    _lastX1 = _event.fingerPositions[0].x;
    _lastY1 = _event.fingerPositions[0].y;
    _lastTimeStamp = _event.timeStamp;
}

void stateEngine::_update2()
{
    // Serial.printlnf("   Update2...");

    _deltaX1 = _event.fingerPositions[0].x - _lastX1;
    _deltaY1 = _event.fingerPositions[0].y - _lastY1;
    _deltaX2 = _event.fingerPositions[1].x - _lastX2;
    _deltaY2 = _event.fingerPositions[1].y - _lastY2;

    _diffX = _event.fingerPositions[0].x - _event.fingerPositions[1].x;
    _diffY = _event.fingerPositions[0].y - _event.fingerPositions[1].y;
    _diffXPercent = ((float)_diffX / (float)_resolutionX) * 100.00;
    _diffYPercent = ((float)_diffY / (float)_resolutionY) * 100.00;

    _deltaX = (_deltaX1 + _deltaX2)/2;
    _deltaY = (_deltaY1 + _deltaY2)/2;
    _deltaXPercent = ((float)_deltaX / (float)_resolutionX) * 100.00;
    _deltaYPercent = ((float)_deltaY / (float)_resolutionY) * 100.00;

    //Serial.printlnf("   x1:%d, x2:%d, Delta x:%f, Delta Precent x:%f", _event.fingerPositions[0].x, _event.fingerPositions[1].x, _deltaX, _deltaXPercent);
    //Serial.printlnf("   y1:%d, y2:%d, Delta y:%f, Delta Precent y:%f", _event.fingerPositions[0].y, _event.fingerPositions[1].y, _deltaY, _deltaYPercent);

    /*_deltaTime = _event.timeStamp - _lastTimeStamp;
    _distance = sqrt((_deltaX * _deltaX) + (_deltaY * _deltaY));
    _angle = atan2(_deltaY, _deltaX) * 360 / M_PI;
    _velocity = _distance / _deltaTime;
    _velocityX = _deltaX / _deltaTime;
    _velocityY = _deltaY / _deltaTime;*/

    _lastX1 = _event.fingerPositions[0].x;
    _lastY1 = _event.fingerPositions[0].y;
    _lastX2 = _event.fingerPositions[1].x;
    _lastY2 = _event.fingerPositions[1].y;
    _lastTimeStamp = _event.timeStamp;
}

bool stateEngine::_testHold()
{
    if((_touchTime + _holdTime) < _now)
    {
        _hold = true;
        return true;
    }
    return false;
}

bool stateEngine::_testWaitTime()
{
    if((_relaseTime + _waitTime) < _now)
    {
        _hold = true;
        return true;
    }
    return false;
}

bool stateEngine::_testRelease()
{
    if(_event.numberOfFingers == 0)
    {
        _touched = false;
        _released = true;
        return true;
    } else {
        _touched = true;
        _released = false;
    }
    return false;
}

bool stateEngine::_testTouch()
{
    if(_event.numberOfFingers > 0)
    {
        _touched = true;
        _released = false;
        return true;
    } else {
        _touched = false;
        _released = true;
    }
    return false;
}

bool stateEngine::_testForSecondFinger()
{
    if(_event.numberOfFingers == 2)
    {
        return true;
    }
    return false;
}

bool stateEngine::_testMovement()
{
    if((abs(_deltaX) > _movementThreshold) || (abs(_deltaY) > _movementThreshold))
    {
        _moving = true;
        return true;
    }
    return false;
}

bool stateEngine::getHold()
{
    return _hold;
}

float stateEngine::getDeltaXPercent()
{
    return _deltaXPercent;
}

float stateEngine::getDeltaYPercent()
{
    return _deltaYPercent;
}

bool stateEngine::getMovingXAxis()
{
    return _movingXAxis;
}

bool stateEngine::getMovingYAxis()
{
    return _movingYAxis;
}

bool stateEngine::getMovingXAxis2()
{
    return _movingXAxis2;
}

bool stateEngine::getMovingYAxis2()
{
    return _movingYAxis2;
}

bool stateEngine::getTap()
{
    return _tap;
}

void stateEngine::setTap(bool tapState)
{
    _tap = tapState;
}

bool stateEngine::getDoubleTap()
{
    return _doubleTap;
}

void stateEngine::setDoubleTap(bool doubleTapState)
{
    _doubleTap = doubleTapState;
}

bool stateEngine::getClaim()
{
    return _claim;
}

void stateEngine::setClaim(bool claimState)
{
    _claim = claimState;
}

int16_t stateEngine::getState()
{
    return _state;
}

void stateEngine::setFingerCount(uint8_t _count)
{
    _fingerCount = _count;
}

uint8_t stateEngine::getFingerCount()
{
    return _fingerCount;
}
