#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef ARDUINO_H
#define ARDUINO_H

//#include <EthernetClient.h>

#pragma pack(1)

struct ArduinoData
{
	int pin[3] = {};
	double instance = 0;
};

class Microprocessor
{
public:
	int pin[3] = {};
	double instance;
	Microprocessor();
	~Microprocessor();
	void Get_Data();
	bool connection;
	Serial* SP;
};
#endif