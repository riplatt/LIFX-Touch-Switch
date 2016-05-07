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
            void update(uint32_t now);

            void setDoubleTap(bool state);
            bool getDoubleTap();
            void setHold(bool state);
            bool getHold();
            void setTap(bool state);
            bool getTap();
            
		private:
            /* Members */
            struct _finger
            {
                bool touch;
                bool lastTouch;
                bool tapOK;
                bool waitForUp;
                bool ignoreUp;
                bool doubleTapOnUp;
                bool doubleTapWaiting;
                bool held;
                bool moving;
                uint16_t xFirst;
                uint16_t x;
                uint16_t xLast ;
                uint16_t yFirst;
                uint16_t y;
                uint16_t yLast ;
                uint16_t firstTime;
                uint32_t time;
                uint32_t lastTime;
                uint32_t startTime;
                float distance;
                float angle;
                float velocity;
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
            // Button timing variables
            uint16_t debounceTime = 15;
            uint16_t doubleTapGapTime = 250;  
            uint16_t holdTime = 750;
            
            /* Members Functions */
            void _checkTap(struct _finger &finger);
            void _decay(struct _finger &finger);
            void _evaluateFinger(struct _finger &finger);
            void _evaluate2Fingers(struct _finger1 &finger1, struct _finger2 &finger2)
            void _recognise();
            void _printFinger(struct _finger &finger);
	};

#endif