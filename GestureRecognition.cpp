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
    _fingerCountLast = 0;
    //Serial.println("New Gesture Recognition Object...");
    return 1;
}

void GestureRecognition::addEvent(const struct touchScreenEvent &event)
{
    
    
	_fingerCount = event.numberOfFingers;
	//Serial.printf("Count: %d, Last Count: %d, ", _fingerCount, _fingerCountLast);
	// Add Gesture Event to Stack
	switch (_fingerCount) {
		case 0:
		    //Serial.printlnf("   Case: 0");
		    switch (_fingerCountLast) {
    			case 1:
    			    //Serial.printlnf("       Case: 1");
    			    _finger1.touch = false;
    			    // Debug
                    Serial.printlnf("   Touch: %d, Last Touch: %d",_finger1.touch, _finger1.lastTouch);
    			    _evaluateFinger(_finger1);
    			    break;
    			case 2:
    			    _finger2.touch = false;
    			    _evaluateFinger(_finger2);
        			break;
    			case 3:
    			    _finger3.touch = false;
    			    _evaluateFinger(_finger3);
    			    break;
    			case 4:
    			    _finger4.touch = false;
    			    _evaluateFinger(_finger4);
    			    break;
    			case 5:
    			    _finger5.touch = false;
    			    _evaluateFinger(_finger5);
    			    break;
                default:
                    Serial.printlnf("       defalut: ");
                    Serial.printlnf("           Last Finger Count: %d", _fingerCountLast);
                    break;
		    }
			break;
		case 1:
		    _finger1.touch = true;
			_finger1.x = event.fingerPositions[0].x;
			_finger1.y = event.fingerPositions[0].y;
			_finger1.time = event.timeStamp;
			// Debug
            Serial.printlnf("   Touch: %d, Last Touch: %d",_finger1.touch, _finger1.lastTouch);
			_evaluateFinger(_finger1);
			break;
		case 2:
			_finger1.x = event.fingerPositions[0].x;
			_finger1.y = event.fingerPositions[0].y;
			_finger1.time = event.timeStamp;
			//_evaluateFinger(_finger1);
			_finger2.x = event.fingerPositions[1].x;
			_finger2.y = event.fingerPositions[1].y;
			_finger2.time = event.timeStamp;
			_evaluate2Fingers(_finger2, _finger2);
			break;
		case 3:
		case 4:
		case 5:
			// Furtue
		default:
			break;
	}
	_fingerCountLast = _fingerCount;
}

void GestureRecognition::update(uint32_t now)
{
    if(_finger1.touch == false && _finger1.doubleTapWaiting == true &&  _finger1.tapOK == true)
    {
        _finger1.time = now;
        _evaluateFinger(_finger1);
    }
}

void GestureRecognition::_evaluate2Fingers(struct _finger1 &finger1, struct _finger2 &finger2)
{
    
}

void GestureRecognition::_evaluateFinger(struct _finger &finger)
{

	float deltaX;
	float deltaY;
	float distance;
	float angle;
	float velocity;
	float velocityX;
	float velocityY;
	int32_t deltaTime;
    
    // Check for first touch
    if(finger.touch == true && finger.lastTouch == false && (finger.time - finger.lastTime) > debounceTime)
    {
        finger.startTime = finger.time;
        finger.ignoreUp = false;
        finger.waitForUp = false;
        finger.tapOK = true;
        // Enable Double Tap
        if((finger.time-finger.lastTime) < doubleTapGapTime && finger.doubleTapOnUp == false && finger.doubleTapWaiting == true)
        {
            Serial.printlnf("       Second touch...");
            finger.doubleTapOnUp = true;
        } else {
            Serial.printlnf("       First touch...");
            finger.doubleTapOnUp = false;
        }
        finger.doubleTapWaiting = false;
        
        finger.xLast = finger.x;
        finger.yLast = finger.y;
        
    // Screen released
    } else if (finger.touch == false && finger.lastTouch == true && (finger.time - finger.startTime) > debounceTime) {
        if (finger.ignoreUp == false)
        {
            finger.lastTime = finger.time;
            if(finger.doubleTapOnUp == false)
            {
                Serial.printlnf("       Waiting for a second tap...");
                finger.doubleTapWaiting = true;
            } else {
                Serial.printlnf("       Setting double tap...");
                setDoubleTap(true);
                finger.doubleTapOnUp = false;
                finger.doubleTapWaiting = false;
                finger.tapOK = false;
            }
        }
    }
    
    
    deltaX = finger.x - finger.xLast;
	deltaY = finger.y - finger.yLast;
	deltaTime = finger.time - finger.startTime;
	
	distance = sqrt((deltaX * deltaX) + (deltaY * deltaY));
	angle = atan2(deltaY, deltaX) * 360 / M_PI;
    velocity = distance / deltaTime;
    velocityX = deltaX / deltaTime;
    velocityY = deltaY / deltaTime;
    
    //Serial.printlnf("Delta X: %f, Delta Y: %f, Delta Time: %d, Distance: %f, Angle: %f, Velocity: %f, VelocityX: %f, VelocityY: %f", deltaX, deltaY, deltaTime, distance, angle, velocity, velocityX, velocityY);
    
    // Test for movement
    if(finger.touch == true && finger.distance > 0.5 && finger.moving == false)
    {
        Serial.println("        Setting Moving...");
        finger.moving = true;
    }
    
    // Test for tap
    if(finger.touch == false && (finger.time-finger.lastTime) >= doubleTapGapTime && finger.doubleTapWaiting == true && finger.doubleTapOnUp == false && finger.tapOK == true)
    {
        Serial.println("        Setting Tap...");
        setTap(true);
        finger.doubleTapWaiting = false;
    }
    
    // Test for Hold
    if(finger.touch == true && hold == false && (finger.time - finger.startTime) > holdTime)
    {
        Serial.println("        Setting Hold...");
        setHold(true);
        finger.waitForUp = true;
        finger.ignoreUp = true;
        finger.doubleTapOnUp = false;
        finger.doubleTapWaiting = false;
        //finger.held = true;
    }
    
    // Set last states
    finger.lastTouch = finger.touch;
    finger.xLast = finger.x;
    finger.yLast = finger.y;

}

void GestureRecognition::_printFinger(struct _finger &finger)
{
    Serial.printlnf("   touch: %s", finger.touch ? "true" : "false");
    Serial.printlnf("   lastTouch: %s", finger.lastTouch ? "true" : "false");
    Serial.printlnf("   tapOK: %s", finger.tapOK ? "true" : "false");
    Serial.printlnf("   waitForUp: %s", finger.waitForUp ? "true" : "false");
    Serial.printlnf("   ignoreUp: %s", finger.ignoreUp ? "true" : "false");
    Serial.printlnf("   doubleTapOnUp: %s", finger.doubleTapOnUp ? "true" : "false");
    Serial.printlnf("   doubleTapWaiting: %s", finger.doubleTapWaiting ? "true" : "false");
    Serial.printlnf("   held: %s", finger.held ? "true" : "false");
    Serial.printlnf("   moving: %s", finger.moving ? "true" : "false");
    Serial.printlnf("   xFirst: %d", finger.xFirst);
    Serial.printlnf("   x: %d", finger.x);
    Serial.printlnf("   xLast: %d", finger.xLast);
    Serial.printlnf("   yFirst: %d", finger.yFirst);
    Serial.printlnf("   y: %d", finger.y);
    Serial.printlnf("   yLast: %d", finger.yLast);
    Serial.printlnf("   firstTime: %d", finger.firstTime);
    Serial.printlnf("   time: %d", finger.time);
    Serial.printlnf("   lastTime: %d", finger.lastTime);
    Serial.printlnf("   startTime: %d", finger.startTime);
    Serial.printlnf("   distance: %f", finger.distance);
    Serial.printlnf("   angle: %f", finger.angle);
    Serial.printlnf("   velocity: %f", finger.velocity);
}

void GestureRecognition::setTap(bool state)
{
    tap = state;
}

bool GestureRecognition::getTap()
{
    return tap;
}

void GestureRecognition::setDoubleTap(bool state)
{
    doubleTap = state;
}

bool GestureRecognition::getDoubleTap()
{
    return doubleTap;
}

void GestureRecognition::setHold(bool state)
{
    hold = state;
}

bool GestureRecognition::getHold()
{
    return hold;
}