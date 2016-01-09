/*
* GestureRecognition.cpp Gesture Recognition Class File
*/

/* includes */
#include "GestureRecognition.h"

GestureRecognition::GestureRecognition()
{

}

bool GestureRecognition::initialise()
{
    _upperLimit = 5;
    _lowerLimit = 1;
    //Serial.println("New Gesture Recognition Object...");
    return 1;
    }

void GestureRecognition::addEvent(const struct touchScreenEvent &event)
{

	_fingerCount = event.numberOfFingers;
	if (_fingerCount != _fingerCountLast)
	{
		_clearFinger(_finger1);
		//_clearFinger(_finger2);
        // Furtue
        /*_clearFinger(_finger3);
		_clearFinger(_finger4);
		_clearFinger(_finger5);*/
		_fingerCountLast = _fingerCount;
		Serial.println("");
	}
	// Add Gesture Event to Stack
	switch (_fingerCount){
		case 0:
			// TODO: Do we need to do something here???
			// Clear flags
			/*up = 0;
			down = 0;
			right = 0;
			left = 0;
			up2 = 0;
			down2 = 0;
			hold = 0;*/
			break;
		case 1:
			_finger1.x = event.fingerPositions[0].x;
			_finger1.y = event.fingerPositions[0].y;
			_finger1.timeStamp = event.timeStamp;
			_evaluateFinger(_finger1);
			break;
		case 2:
			_finger1.x = event.fingerPositions[0].x;
			_finger1.y = event.fingerPositions[0].y;
			_finger1.timeStamp = event.timeStamp;
			_evaluateFinger(_finger1);
			_finger2.x = event.fingerPositions[1].x;
			_finger2.y = event.fingerPositions[1].y;
			_finger2.timeStamp = event.timeStamp;
			_evaluateFinger(_finger2);
			break;
		case 3:
		case 4:
		case 5:
			// Furtue
		default:
			break;
	}

	// Try to Recognise Gesture
	_recognise();
};

void GestureRecognition::update()
{
    //uint32_t now = millis();
    _decay(_finger1);
    _decay(_finger2);
    // Furtue
    /*_decay(_finger3);
    _decay(_finger4);
    _decay(_finger5);*/
    
    //TODO: Check for tap/click and double tap/click
    _checkTap(_finger1);
    
    if (_finger1.tap)
    {
        _finger1.tap = 0;
        tap = 1;
    }
    
    if (_finger1.doubleTap)
    {
        _finger1.doubleTap = 0;
        doubleTap = 1;
    }
    
    lastUpdate = millis();
}

void GestureRecognition::_checkTap(struct _finger &finger)
{
    uint32_t now = millis();
    // Set Tap or Double Tap
    if (now - 250 > finger.touchEndTimeStamp)
    {
        if(finger.tapWaiting)
        {
            Serial.println("Set Tap...");
            finger.tap = 1;
            finger.tapWaiting = 0;
        } else if (finger.doubleTapWaiting)
        {
            Serial.println("Set Double Tap...");
            finger.doubleTap = 1;
            finger.doubleTapWaiting = 0;
        }
    }
    
}

void GestureRecognition::_decay(struct _finger &finger)
{
    uint32_t now = millis();
    
    if((now - 100) > finger.timeStampLast)
    {
        finger.up--;
        finger.down--;
        finger.right--;
        finger.left--;
        finger.hold--;
    }
    
    if (finger.up > _upperLimit) {finger.up = _upperLimit;}
    if (finger.down > _upperLimit) {finger.down = _upperLimit;}
    if (finger.right > _upperLimit) {finger.right = _upperLimit;}
    if (finger.left > _upperLimit) {finger.left = _upperLimit;}
    if (finger.hold > _upperLimit) {finger.hold = _upperLimit;}
    
    if (finger.up < _lowerLimit) {finger.up = _lowerLimit;}
    if (finger.down < _lowerLimit) {finger.down = _lowerLimit;}
    if (finger.right < _lowerLimit) {finger.right = _lowerLimit;}
    if (finger.left < _lowerLimit) {finger.left = _lowerLimit;}
    if (finger.hold < _lowerLimit) {finger.hold = _lowerLimit;}

}

void GestureRecognition::_evaluateFinger(struct _finger &finger)
{
	// Serial.println("Evaluating a Finger...");
	#if _DEBUG > 2
	    Serial.printf("Evaluate Finger... Up: %d, Down: %d, Right: %d, Left: %d, Hold: %d, ", finger.up, finger.down, finger.right, finger.left, finger.hold);
	#endif
	int32_t xDelta;
	int32_t yDelta;
	int32_t timeDelta;

	xDelta = finger.x - finger.xLast;
	yDelta = finger.y - finger.yLast;
	timeDelta = finger.timeStamp - finger.timeStampLast;
	finger.touchEndTimeStamp = millis();
	
	#if _DEBUG > 2
	    Serial.printlnf("xDelta: %d, yDelta: %d, timeDelta: %d, x: %d, y: %d", xDelta, yDelta, timeDelta, finger.x, finger.y);
	#endif

    if (abs(xDelta) > abs(yDelta))
    {
        // Evaluate the X directions
        if (xDelta > 0)
        {
            // We are moving down
            finger.left++;
            // Hold & Up decay
            finger.hold--;
            finger.right--;
        }else{
            // We are moving up
            finger.right++;
            // Hold & down decay
            finger.hold--;
            finger.left--;
        }

        // Calculate speed of the X direction pixels/s
        if (timeDelta !=0){
            finger.xSpeed = abs(xDelta * 1000 / timeDelta);
        }
    }else if (abs(xDelta) < abs(yDelta)) {
      // Evaluate the Y directions
      	if (yDelta > 0)
      	{
      		// We are moving right
      		finger.down++;
      		// Hold & left decay
      	    finger.hold--;
      		finger.up--;
      	}else{
      		// We are moving left
      		finger.up++;
      		// Hold & right decay
      		finger.hold--;
      		finger.down--;
      	}

        // Calculate speed of the Y direction pixels/s
        if (timeDelta !=0){
            finger.ySpeed = abs(yDelta * 1000 / timeDelta);
        }
    }

    // Evaluate a hold
    if (abs(xDelta) < 1 && abs(yDelta) < 1)
    {
        finger.hold++;
        finger.up--;
        finger.down--;
        finger.right--;
        finger.left--;
    } else {
        finger.hold--;
    }
    
    // Set upper 
    if (finger.down > _upperLimit) {finger.down = _upperLimit;}
    if (finger.up > _upperLimit) {finger.up = _upperLimit;}
    if (finger.right > _upperLimit) {finger.right = _upperLimit;}
    if (finger.left > _upperLimit) {finger.left = _upperLimit;}
    if (finger.hold > _upperLimit) {finger.hold = _upperLimit;}
    // Set lower limits
    if (finger.down < _lowerLimit) {finger.down = _lowerLimit;}
    if (finger.up < _lowerLimit) {finger.up = _lowerLimit;}
    if (finger.right < _lowerLimit) {finger.right = _lowerLimit;}
    if (finger.left < _lowerLimit) {finger.left = _lowerLimit;}
    if (finger.hold < _lowerLimit) {finger.hold = _lowerLimit;}
    
    // Update Last
    finger.xLast = finger.x;
    finger.yLast = finger.y;
    finger.xSpeedLast = finger.xSpeed;
    finger.ySpeedLast = finger.ySpeed;
    finger.timeStampLast = finger.timeStamp;
    
}

void GestureRecognition::_clearFinger(struct _finger &finger)
{
    uint32_t now = millis();
    
	finger.x = 0;
	finger.xLast = 0;
	finger.y = 0;
	finger.yLast = 0;
	finger.xSpeed = 0;
	finger.xSpeedLast = 0;
	finger.ySpeed = 0;
	finger.ySpeedLast = 0;
	finger.timeStamp = 0;
	finger.timeStampLast = 0;
	finger.touchBegin = 0;
	finger.touchEnd = 1;
	
	if (now - 150 > finger.touchEndTimeStamp && finger.tapWaiting)
	{
	    Serial.println("Double Tap Waiting Set...");
	    finger.tapWaiting = 0;
	    finger.doubleTapWaiting = 1;
	} else {
	    Serial.println("Tap Waiting Set...");
	    finger.tapWaiting = 1;
	    finger.doubleTapWaiting = 0;
	}
	
    //finger.touchEndTimeStamp = millis();
	finger.up = _lowerLimit;
	finger.down = _lowerLimit;
	finger.right = _lowerLimit;
	finger.left = _lowerLimit;
	finger.hold = _lowerLimit;
}

void GestureRecognition::_recognise()
{
	// TODO:
	// flick left or right
	// Tap
	// Double Tap

    uint8_t thresholdHigh = 4;
    uint8_t thresholdLow = 2;

	// How many finger gesture is this
	switch (_fingerCount)
	{
		case 0:
			// TODO:
			// What just ended? Check Flags and see if we justed Tap and could be going for a double tap
			break;
		case 1:
			// Are we swiping down
			if (_finger1.down > thresholdHigh)
			{
				down = 1;
				downSpeed = _finger1.ySpeed;
			}else if(_finger1.down < thresholdLow)
			{
				down = 0;
				downSpeed = 0;
			}
			// Are we swiping up
			if (_finger1.up > thresholdHigh)
			{
				up = 1;
				upSpeed = _finger1.ySpeed;
			}else if(_finger1.up < thresholdLow)
			{
				up = 0;
				upSpeed = 0;
			}
			// Are we swiping left
			if (_finger1.left > thresholdHigh)
			{
				left = 1;
				leftSpeed = _finger1.xSpeed;
			}else if(_finger1.left < thresholdLow)
			{
				left = 0;
				leftSpeed = 0;
			}
			// Are we swiping right
			if (_finger1.right > thresholdHigh)
			{
				right = 1;
				rightSpeed = _finger1.xSpeed;
			}else if(_finger1.right < thresholdLow)
			{
				right = 0;
				rightSpeed = 0;
			}
			// TODO:
			// Are we doing a single tap
			// TODO:
			// Are we doing a double tap
			// TODO:
			// Are we just holding
            if (_finger1.hold > thresholdHigh)
            {
                hold = 1;
            }else if(_finger1.hold < thresholdLow)
            {
                hold = 0;
            }
			break;
		case 2:
			// Are we swiping down
			if (_finger1.down > thresholdHigh && _finger2.down > thresholdHigh)
			{
				down2 = 1;
				down2Speed = (_finger1.ySpeed + _finger2.ySpeed) / 2;
			}else if(_finger1.down < thresholdLow || _finger2.down < thresholdLow)
			{
				down2 = 0;
				down2Speed = 0;
			}
			// Are we swiping up
			if (_finger1.up > thresholdHigh && _finger2.up > thresholdHigh)
			{
				up2 = 1;
				up2Speed = (_finger1.ySpeed + _finger2.ySpeed) / 2;
			}else if(_finger1.up < thresholdLow || _finger2.up < thresholdLow)
			{
				up2 = 0;
				up2Speed = 0;
			}
			// TODO:
			// Are we touching adjacent corners
			// TODO:
			// Are we just holding
			break;
		default:
			// Don't really care for now
			break;
	}
};
