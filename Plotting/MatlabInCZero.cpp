/* Using MATLAB inside a C/C++ programs */
/*
*	MATLABInC.cc
*
*	J.Katupitiya@unsw.edu.au
*	(c)2010 UNSW.
*
*
*/
#define _USE_MATH_DEFINES
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <SMObject.h>
#include <SMStruct.h>


#include "engine.h"
/**** Specify include files from Matlab/extern and also lib files
libmx.lib, libmex.lib and libeng.lib from Matlab/exter/lib/microsoft
*******/
//h = findall(gca, 'type', 'line', 'color', 'b')
//h = findall(gca, 'type', 'line', 'marker', '+') ???
//delete(h)
using namespace std;

int main()
{
	SMObject ArduinoObj(TEXT("ArduinoData"), sizeof(Arduino));
	SMObject CameraObj(TEXT("CameraData"), sizeof(Camera));
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// Request access to GPS mem block
	ArduinoObj.SMAccess();
	CameraObj.SMAccess();
	PMObj.SMAccess();

	//HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);// SW_SHOWMINIMIZED);//SW_SHOW, SW_HIDE

	ProcessManagement* PM = (ProcessManagement*)PMObj.pData;
	Arduino* pArduino = (Arduino*)ArduinoObj.pData;
	Camera* pCamera = (Camera*)CameraObj.pData;

	// Declarations for MATLAB
	Engine *ep;
	mxArray *PINS = NULL, *INSTANCE = NULL, *DEPTHMAP = NULL, *FRAME = NULL;

	int pins[3] = {};
	double instance = 0;
	double depthMap[120][160] = {};
	double frame = 0;


	//Start local MATLAB engine
	if (!(ep = engOpen("\0")))
	{
		printf("\nMATLAB engine failure \n");
		return 1;
	}

	// Make engine window visible or not visible (0 not visible)
	engSetVisible(ep, 1);
	//engEvalString(ep, "axis([-5 5 -5 5]);hold on;");
	//PINS = mxCreateDoubleMatrix(1, 3, mxREAL);
	//INSTANCE = mxCreateDoubleMatrix(1, 1, mxREAL);
	DEPTHMAP = mxCreateDoubleMatrix_700(160, 120, mxREAL);
	FRAME = mxCreateDoubleMatrix(1, 1, mxREAL);

	PM->ShutDown.Flags.Plot = 0;

	while (!PM->ShutDown.Flags.Plot)
	{
		// c Program operations
		/*for (int i = 0; i < 3; i++) {
			pins[i] = pArduino->pin[i];
		}
		instance = pArduino->instance;*/
		for (int i = 0; i < 120; i++) {
			for (int j = 0; j < 160; j++) {
				depthMap[i][j] = (double)(pCamera->depth[i][j]);
			}
		}

		//printf("\n");
		frame = pCamera->frame;

		//MATLAB operations
		//memcpy((void *)mxGetPr(PINS), (void *)&pins, sizeof(pins));
		//memcpy((void *)mxGetPr(INSTANCE), (void *)&instance, sizeof(instance));
		memcpy((void *)mxGetPr(DEPTHMAP), (void *)&depthMap, sizeof(depthMap));
		memcpy((void *)mxGetPr(FRAME), (void *)&frame, sizeof(frame));

		//engPutVariable(ep, "PINS", PINS);
		//engPutVariable(ep, "INSTANCE", INSTANCE);
		engPutVariable(ep, "DEPTHMAP", DEPTHMAP);
		engPutVariable(ep, "FRAME", FRAME);
		//_getch();
		//engEvalString(ep, "h = findall(gca, 'type', 'line', 'color', 'black')");// ???
		//engEvalString(ep, "delete(h)");
		/*engEvalString(ep, "figure(1);");
		engEvalString(ep, "hold on;");
		engEvalString(ep, "grid on;");
		engEvalString(ep, "plot(PINS(:,1),INSTANCE, 'r');");
		engEvalString(ep, "plot(PINS(:,2),INSTANCE, 'g');");
		engEvalString(ep, "plot(PINS(:,3),INSTANCE, 'b');");
		engEvalString(ep, "title('Analog Inputs from Arduino');");
		engEvalString(ep, "xlabel('Instances');");
		engEvalString(ep, "ylabel('Values');");
		engEvalString(ep, "legend('AI 1', 'AI 2', 'AI 3';");*/
		//engEvalString(ep, "x = zeros(120,160);");
		engEvalString(ep, "format long;");
		//engEvalString(ep, "DEPTHMAP = single(DEPTHMAP);");
		//engEvalString(ep, "for i = 1:120 for j = 1:160 DEPTHMAP(i,j)=DEPTHMAP(i,j)/32000; end end");
		engEvalString(ep, "MyGUIHandles.handle1 = imshow(imrotate(flipud(DEPTHMAP(:,:)),-90),[]);");
		//engEvalString(ep, "MyGUIHandles.handle1 = imshow(imrotate(DEPTHMAP,-1),[]);");
		engEvalString(ep, "MyGUIHandles.handle2 = title('');");
		engEvalString(ep, "axis([0,160,0,120]);");
		engEvalString(ep, "xlabel('Width');");
		engEvalString(ep, "ylabel('Height');");
		//engEvalString(ep, "figure();");
		//engEvalString(ep, "hold on;");
		//engEvalString(ep, "grid on;");
		//engEvalString(ep, "set(MyGUIHandles.handle1, 'CData', DEPTHMAP(:,:));");
		//engEvalString(ep, "set(MyGUIHandles.handle1, imrotate(DEPTHMAP(:,:),-1));");
		engEvalString(ep, "s = sprintf('Frame # %d', FRAME);");
		engEvalString(ep, "set(MyGUIHandles.handle2, 'string',s);");
		//engEvalString(ep, "title('3D Camera, Frame: %d', FRAME);");
		//engEvalString(ep, "zlabel('Depth');");
		//engEvalString(ep, "pause(0.5);");

		PM->HeartBeat.Flags.Plot = 1;
		//if (_kbhit())
		//	break;
		Sleep(50);
	}

	/*
	* Free Memory and close the MATLAB engine.
	*/
	printf("Closing!\n");
	mxDestroyArray(PINS);
	mxDestroyArray(INSTANCE);
	mxDestroyArray(DEPTHMAP);
	mxDestroyArray(FRAME);
	engClose(ep);
	
	return 0;
}