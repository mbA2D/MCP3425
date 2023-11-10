#include <MCP3425.h>
#include "Arduino.h"

MCP3425::MCP3425()
{
	_set_reg_defaults();
	_conv_result = 0;
}

void MCP3425::init(uint8_t address, TwoWire *i2c)
{
	_addr = address;
	_i2c = i2c;
	
	_set_reg_defaults();
}

void MCP3425::reset()
{
	_write_general_reset();
	_set_reg_defaults();
}

void MCP3425::set_gain(uint8_t gain)
{
	if (gain <= MCP3425_GAIN_8)
	{
		_conf_reg.bits.gain = gain;
		_write_conf_reg();
	}
}

void MCP3425::set_rate(uint8_t rate)
{
	if (rate <= MCP3425_DR_15SPS)
	{
		_conf_reg.bits.rate = rate;
		_write_conf_reg();
	}
}

void MCP3425::set_mode(uint8_t mode)
{
	if (mode <= MCP3425_MODE_CONTINUOUS)
	{		
		_conf_reg.bits.mode = mode;
		_write_conf_reg();
	}
}

uint8_t MCP3425::get_conf_byte()
{
	return _conf_reg.conf_byte;
}

void MCP3425::trigger_conversion()
{
	_conf_reg.bits.rdy = MCP3425_DRDY_INIT_CONV;
	_write_conf_reg();
}

int16_t MCP3425::trigger_and_get_single_shot_result()
{
	//assuming already in single-shot mode (would also work in continuous mode
	trigger_conversion();
	unsigned long start_time = millis();
	unsigned long current_time = start_time;
	while (!update_conversion_result() && ((current_time - start_time) < 100))
	{
		delayMicroseconds(4200); //max sample rate is 240SPS. add a little headroom for the delay.
		current_time = millis();
	}
	return _conv_result;
}

int16_t MCP3425::get_conversion_result()
{
	return _conv_result;
}

bool MCP3425::update_conversion_result()
{
	int16_t result = 0;

	uint8_t response_length = 3;
	_wire_request_from(response_length);
	
	result = _i2c->read();
	result <<= 8;
	result |= _i2c->read();
	
	_conf_reg.conf_byte = _i2c->read();
	
	if (_conf_reg.bits.rdy == MCP3425_DRDY_FALSE)
	{
		return false;
	}
	else
	{
		if (_conf_reg.bits.rate == MCP3425_DR_60SPS)
		{
			result = result << 2;
		}
		else if (_conf_reg.bits.rate == MCP3425_DR_240SPS)
		{
			result = result << 4;
		}
		_conv_result = result;

		return true;
	}
}

float MCP3425::measure_voltage()
{
	//this function should return the voltage at the pins of the ADC.
	
	int16_t reading = trigger_and_get_single_shot_result();
	
	//gain
	uint8_t gain = 0;
	if (_conf_reg.bits.gain == MCP3425_GAIN_1) gain = 1;
	else if (_conf_reg.bits.gain == MCP3425_GAIN_2) gain = 2;
	else if (_conf_reg.bits.gain == MCP3425_GAIN_4) gain = 4;
	else if (_conf_reg.bits.gain == MCP3425_GAIN_8) gain = 8;
	
	return ((float(reading) / float(MCP3425_FULL_SCALE_POS_CODE)) * (MCP3425_VOLTAGE_REFERENCE_V / gain));

}

void MCP3425::_set_reg_defaults()
{
	//set everything back to defaults
	_conf_reg.conf_byte = 0;
	_conf_reg.bits.gain = MCP3425_GAIN_1;
	_conf_reg.bits.mode = MCP3425_MODE_CONTINUOUS;
	_conf_reg.bits.rate = MCP3425_DR_240SPS;
	_conf_reg.bits.rdy = MCP3425_DRDY_FALSE;
	_conf_reg.bits.ch = 1; //these bits don't matter for MCP3425
}

void MCP3425::_write_conf_reg()
{
	_i2c->beginTransmission(_addr);
	_i2c->write(_conf_reg.conf_byte);
	_i2c->endTransmission(true);
}

void MCP3425::_write_general_reset()
{
	_i2c->beginTransmission(I2C_GENERAL_CALL_ADDRESS);
	_i2c->write(I2C_GENERAL_CALL_RESET);
	_i2c->endTransmission(true);
}

uint8_t MCP3425::_wire_request_from(uint8_t num_bytes)
{
	uint8_t num_bytes_read = 0;
	num_bytes_read = _i2c->requestFrom(_addr, num_bytes);
	//uint8_t timeout_count = 0;
	//while (timeout_count < 10 && _i2c->getWireTimeoutFlag())
	//{
		//if a timeout occurred, and there were less than 10 timeouts, then try again.
		//_i2c->clearWireTimeoutFlag();
		//num_bytes_read = _i2c->requestFrom(_addr, num_bytes);
		//timeout_count++;
	//}
	return num_bytes_read;
}
