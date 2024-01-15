#ifndef MCP3425_h
#define MCP3425_h

#include "Wire.h"

#define MCP3425_VOLTAGE_REFERENCE_V					2.048
#define MCP3425_FULL_SCALE_POS_CODE					0x7FFF //2^15
#define MCP3425_FULL_SCALE_NEG_CODE					0x8000 //-(2^15)

#define MCP3425_MIN_I2C_ADDR	0x68
#define MCP3425_MAX_I2C_ADDR	0x6B

#define I2C_GENERAL_CALL_MCP3425_ADDRESS 0x0
#define I2C_GENERAL_CALL_MCP3425_RESET   0x6
#define I2C_GENERAL_CALL_MCP3425_CONV    0x8

//############# REGISTER VALUES ##############

#define MCP3425_GAIN_1			0
#define MCP3425_GAIN_2			1
#define MCP3425_GAIN_4			2
#define MCP3425_GAIN_8			3

#define MCP3425_DR_240SPS		0
#define MCP3425_DR_60SPS		1
#define MCP3425_DR_15SPS		2

#define MCP3425_MODE_SINGLE_SHOT	0
#define MCP3425_MODE_CONTINUOUS		1

#define MCP3425_DRDY_FALSE		1
#define MCP3425_DRDY_TRUE		0

#define MCP3425_DRDY_INIT_CONV	1


typedef union _MCP3425_conf_reg
{
	struct
	{
		uint8_t gain: 2; //bit 0-1
		uint8_t rate: 2; // bit 2-3
		uint8_t mode: 1; // bit 4
		uint8_t ch: 2; // bit 5-6
		uint8_t rdy: 1; // bit 7
	}bits;
	uint8_t conf_byte;
}MCP3425_conf_reg;

class MCP3425
{
	public:
		MCP3425();
	
		void init(uint8_t address, TwoWire *i2c = &Wire);
		void reset();
		
		void set_gain(uint8_t gain);
		void set_rate(uint8_t rate);
		void set_mode(uint8_t mode);
		
		void trigger_conversion();
		
		int16_t trigger_and_get_single_shot_result();
		bool update_conversion_result();
		int16_t get_conversion_result();
		bool get_drdy();
		uint8_t get_conf_byte();

		float measure_voltage(); //uses single-shot mode
		float measure_voltage_continuous(); //uses continuous mode
	
	private:
		void _set_reg_defaults();
		void _write_conf_reg();
		void _write_general_reset();
		uint8_t _wire_request_from(uint8_t num_bytes);

		uint8_t _addr;
		TwoWire *_i2c;
		
		int16_t _conv_result;
		MCP3425_conf_reg	_conf_reg;
	
};

#endif
