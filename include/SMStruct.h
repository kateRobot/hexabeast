#ifndef SMSTRUCT_H
#define SMSTRUCT_H

struct Arduino
{
	int pin[3] = {};
	double instance;
};

struct Camera
{
	short verticesX[76800] = {};
	short verticesY[76800] = {};
	short verticesZ[76800] = {};
};


struct UnitFlags
{
	unsigned short 
		PM : 1,
		SM : 1,
		Camera : 1,
		Arduino : 1,
		Plot : 1,
		Unused : 13;
};

union ExecFlags
{
	UnitFlags Flags;
	unsigned short Status;
};

struct ProcessManagement
{
	ExecFlags HeartBeat;
	ExecFlags ShutDown;
};

#endif