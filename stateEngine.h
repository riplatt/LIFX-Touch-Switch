/*
 * stateEngine.h - State Engine to control actions.
 */
#ifndef stateEngine_h
    #define stateEngine_h
    
    /* includes */
    #include "common.h"
    
        class stateEngine{
        public:
            /* Member Functions */
            stateEngine();
            void initialise();
            void update(uint32_t now);
            void addEvent(const struct touchScreenEvent &event);
            void setFingerCount(uint8_t _fingerCount);
            uint8_t getFingerCount();
            bool getHold();
            bool getMovingXAxis();
            bool getMovingYAxis();
            float getDeltaXPercent();
            float getDeltaYPercent();
            bool getTap();
            void setTap(bool tapState);
            bool getDoubleTap();
            void setDoubleTap(bool doubleTapState);
            int16_t getState();
            bool getClaim();
            void setClaim(bool claimState);
            /* Members */
            uint32_t lastUpdate;

        
        private:
            /* Member Functions */
            void _update1();
            void _update2();
            bool _testHold();
            bool _testTouch();
            bool _testForSecondFinger();
            bool _testRelease();
            bool _testMovement();
            bool _testWaitTime();
            
            /* Members */
            uint32_t _now;
            uint8_t _fingerCount;
            uint32_t _touchTime;
            uint32_t _relaseTime;
            struct touchScreenEvent _event;
            bool _hold;
            bool _tap;
            bool _doubleTap;
		    bool _touched;
		    bool _released;
		    bool _moving;
		    bool _movingYAxis;
		    bool _movingXAxis;
		    bool _claim;
		    int16_t _state;
		    uint16_t _lastX1;
		    uint16_t _lastY1;
		    uint16_t _lastX2;
		    uint16_t _lastY2;
		    uint32_t _lastTimeStamp;
		    int16_t _deltaX;
            int16_t _deltaY;
            int16_t _deltaX1;
            int16_t _deltaY1;
            int16_t _deltaX2;
            int16_t _deltaY2;
            float _deltaXPercent;
            float _deltaYPercent;
            float _deltaTime;
            float _distance;
            float _angle;
            float _velocity;
            float _velocityX;
            float _velocityY;
            /* */
            uint16_t _movementThreshold = 15;
            uint32_t _debounceTime = 25;
            uint32_t _holdTime = 500;
            uint32_t _waitTime = 250;
            uint16_t _resolutionX = 400;
            uint16_t _resolutionY = 800;
            
    };
    
#endif