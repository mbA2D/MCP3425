#include <MCP3425.h>
#include <Wire.h>

MCP3425 adc;

void setup()
{
	Serial.begin(115200);
	Serial.println("ADC Starting");
	
	Wire.begin();
	
	adc.init(0x68);
	adc.reset();
	
	//adc.set_rate(MCP3425_DR_60SPS);
	//adc.set_gain(MCP3425_GAIN_2);
	//adc.set_mode(MCP3425_MODE_CONTINUOUS);
}


void loop()
{
	Serial.print("Voltage: ");
	Serial.println(adc.measure_voltage(),5);
	Serial.println("----------------");
	delay(500);
}
