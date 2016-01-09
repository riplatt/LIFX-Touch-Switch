/*
* gsl1680.h - library for talking to the GSL1680 800x480 panel.
*/
#ifndef _gsl1680_h
	#define _gsl1680_h

	/* defontions */
	#define GSLX680_I2C_ADDR 	0x40
	#define GSL_DATA_REG 		0x80
	#define GSL_STATUS_REG 		0xe0
	#define GSL_PAGE_REG 		0xf0
    
    #define delayus delayMicroseconds
    
    /* includes */
    #include "common.h"
    #include "gslX680firmware.h"

	class gsl1680{
		public:
		/* Members */
		struct touchScreenEvent event;
		/* Member Functions */
		gsl1680();
		bool initialise(uint8_t pinWake);
		bool readData();

		private:
		/* Member Function */
		bool _i2c_write(uint8_t reg, uint8_t *buf, int cnt);
		bool _i2c_read(uint8_t reg, uint8_t *buf, int cnt);
		void _init_chip();
		void _clr_reg();
		void _reset_chip();
		void _load_fw();
		void _startup_chip();

		/* Members */
		uint8_t _WAKE;

	};
#endif
