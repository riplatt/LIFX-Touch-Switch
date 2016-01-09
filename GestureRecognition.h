/*
* GestureRecognition.h Gesture Recognition Class Header File
*/
#ifndef _gestureRecognition_h
    #define _gestureRecognition_h
    
    /* includes */
    #include "common.h"

	class GestureRecognition{
		public:
            /* Members */
            bool down;
            uint16_t downSpeed;
            bool up;
            uint16_t upSpeed;
            bool down2;
            uint16_t down2Speed;
            bool up2;
            uint16_t up2Speed;
            bool right;
            uint16_t rightSpeed;
            bool left;
            uint16_t leftSpeed;
            bool hold;
            bool tap;
            bool doubleTap;
            uint32_t lastUpdate;

            /* Members Functions */
            GestureRecognition();
            bool initialise();
            void addEvent(const struct touchScreenEvent &event);
            void update();
            // TODO: Look at doing set get functions for flags??
            /*
            void setTap(bool state);
            void getTap(bool state);
            */
		private:
            /* Members */
            struct _finger
            {
                uint16_t x;
                uint16_t xLast;
                uint16_t y;
                uint16_t yLast;
                uint16_t xSpeed;
                uint16_t xSpeedLast;
                uint16_t ySpeed;
                uint16_t ySpeedLast;
                uint32_t timeStamp;
                uint32_t timeStampLast;
                bool touchBegin;
                bool touchEnd;
                bool tapWaiting;
                bool tap;
                bool doubleTapWaiting;
                bool doubleTap;
                uint32_t touchEndTimeStamp;
                int8_t up;
                int8_t down;
                int8_t right;
                int8_t left;
                int8_t hold;
            };
            bool _singleTapWaiting;
            bool _doubleTapWaiting;
            uint8_t _fingerCount;
            uint8_t _fingerCountLast;
            uint8_t _upperLimit;
            uint8_t _lowerLimit;
            struct _finger _finger1 = _finger();
            struct _finger _finger2 = _finger();
            struct _finger _finger3 = _finger();
            struct _finger _finger4 = _finger();
            struct _finger _finger5 = _finger();
            
            /* Members Functions */
            void _checkTap(struct _finger &finger);
            void _decay(struct _finger &finger);
            void _evaluateFinger(struct _finger &finger);
            void _clearFinger(struct _finger &finger);
            void _recognise();
	};

#endif
