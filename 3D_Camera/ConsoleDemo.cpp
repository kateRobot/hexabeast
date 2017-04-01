////////////////////////////////////////////////////////////////////////////////
// SoftKinetic DepthSense SDK
//
// COPYRIGHT AND CONFIDENTIALITY NOTICE - SOFTKINETIC CONFIDENTIAL
// INFORMATION
//
// All rights reserved to SOFTKINETIC SENSORS NV (a
// company incorporated and existing under the laws of Belgium, with
// its principal place of business at Boulevard de la Plainelaan 11,
// 1050 Brussels (Belgium), registered with the Crossroads bank for
// enterprises under company number 0811 341 454 - "Softkinetic
// Sensors").
//
// The source code of the SoftKinetic DepthSense Camera Drivers is
// proprietary and confidential information of Softkinetic Sensors NV.
//
// For any question about terms and conditions, please contact:
// info@softkinetic.com Copyright (c) 2002-2015 Softkinetic Sensors NV
////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#include <windows.h>
#endif

#include <stdio.h>
#include <vector>
#include <exception>

#include <DepthSense.hxx>

#include <SMObject.h>
#include <SMStruct.h>
#include <conio.h>

#define HEIGHT			240
#define WIDTH			320
#define NEW_HEIGHT		120
#define NEW_WIDTH		160

using namespace DepthSense;
using namespace std;

Context g_context;
DepthNode g_dnode;
uint32_t g_dFrames = 0;

bool g_bDeviceFound = false;

ProjectionHelper* g_pProjHelper = NULL;
StereoCameraParameters g_scp;

/*----------------------------------------------------------------------------*/
// New depth sample event handler
void onNewDepthSample(DepthNode node, DepthNode::NewSampleReceivedData data)
{
	SMObject CameraObj(TEXT("CameraData"), sizeof(Camera));
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// Request access to GPS mem block
	CameraObj.SMAccess();
	PMObj.SMAccess();

	//HWND hWnd = GetConsoleWindow();
	//ShowWindow(hWnd, SW_HIDE);// SW_SHOWMINIMIZED);//SW_SHOW, SW_HIDE

	ProcessManagement* PM = (ProcessManagement*)PMObj.pData;
	Camera* pCamera = (Camera*)CameraObj.pData;

	//PM->ShutDown.Flags.Camera = 0;

	/*for (int i = 0; i < HEIGHT*WIDTH; i++) {
	printf("Z#%u: %d\n", g_dFrames, data.depthMap[i]);
	printf("Size = %d\n", sizeof(data.depthMap));
	printf("i = %d\n", i);
	}*/
	int newframe[NEW_HEIGHT][NEW_WIDTH];
	int depthMap2D[HEIGHT][WIDTH];

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
					depthMap2D[i][j] = (int) data.depthMap[j + i * 320];
		}
	}

	printf("%d \n", g_dFrames);

	for (int i = 0; i < NEW_HEIGHT; i++) {
		for (int j = 0; j < NEW_WIDTH; j++) {
			newframe[i][j] = (depthMap2D[i * 2][j * 2] + depthMap2D[i * 2 + 1][j * 2] + depthMap2D[i * 2][j * 2 + 1] + depthMap2D[i * 2 + 1][j * 2 + 1]) / 4;
		}
	}
	
	for (int i = 0; i < NEW_HEIGHT; i++) {
		for (int j = 0; j < NEW_WIDTH; j++) {
			pCamera->depth[i][j] = newframe[i][j]; // from documentation: The depth map in fixed point format. This map represents the cartesian depth of each pixel, expressed in millimeters. Valid values lies in the range [0 - 31999]. Saturated pixels are given the special value 32002.
		}
	}

	pCamera->frame = (double) g_dFrames;

	// Project some 3D points in the Color Frame
	if (!g_pProjHelper)
	{
		g_pProjHelper = new ProjectionHelper(data.stereoCameraParameters);
		g_scp = data.stereoCameraParameters;
	}
	else if (g_scp != data.stereoCameraParameters)
	{
		g_pProjHelper->setStereoCameraParameters(data.stereoCameraParameters);
		g_scp = data.stereoCameraParameters;
	}

	/*int32_t w, h;
	FrameFormat_toResolution(data.captureConfiguration.frameFormat, &w, &h);
	int cx = w / 2;
	int cy = h / 2;

	Vertex p3DPoints[4];

	p3DPoints[0] = data.vertices[(cy-h/4)*w+cx-w/4];
	p3DPoints[1] = data.vertices[(cy-h/4)*w+cx+w/4];
	p3DPoints[2] = data.vertices[(cy+h/4)*w+cx+w/4];
	p3DPoints[3] = data.vertices[(cy+h/4)*w+cx-w/4];
	
	Point2D p2DPoints[4];
	g_pProjHelper->get2DCoordinates ( p3DPoints, p2DPoints, 4, CAMERA_PLANE_COLOR);
	*/

	g_dFrames++;
	Sleep(500); //Transmitted frame rate 2Hz 
	// Quit the main loop after 200 depth frames received
	if (g_dFrames == 200000000000000){// || _kbhit()) {
		PM->HeartBeat.Flags.Arduino = 1;
		g_context.quit();
	}

}

/*----------------------------------------------------------------------------*/
void configureDepthNode()
{
	g_dnode.newSampleReceivedEvent().connect(&onNewDepthSample);

	DepthNode::Configuration config = g_dnode.getConfiguration();
	config.frameFormat = FRAME_FORMAT_QVGA;
	config.framerate = 25;
	config.mode = DepthNode::CAMERA_MODE_CLOSE_MODE;
	config.saturation = true;

	//g_dnode.setEnableVertices(true);
	g_dnode.setEnableDepthMap(true);

	try
	{
		g_context.requestControl(g_dnode, 0);

		g_dnode.setConfiguration(config);
	}
	catch (ArgumentException& e)
	{
		printf("Argument Exception: %s\n", e.what());
	}
	catch (UnauthorizedAccessException& e)
	{
		printf("Unauthorized Access Exception: %s\n", e.what());
	}
	catch (IOException& e)
	{
		printf("IO Exception: %s\n", e.what());
	}
	catch (InvalidOperationException& e)
	{
		printf("Invalid Operation Exception: %s\n", e.what());
	}
	catch (ConfigurationException& e)
	{
		printf("Configuration Exception: %s\n", e.what());
	}
	catch (StreamingException& e)
	{
		printf("Streaming Exception: %s\n", e.what());
	}
	catch (TimeoutException&)
	{
		printf("TimeoutException\n");
	}

}

/*----------------------------------------------------------------------------*/
void configureNode(Node node)
{
	if ((node.is<DepthNode>()) && (!g_dnode.isSet()))
	{
		g_dnode = node.as<DepthNode>();
		configureDepthNode();
		g_context.registerNode(node);
	}
}

/*----------------------------------------------------------------------------*/
void onNodeConnected(Device device, Device::NodeAddedData data)
{
	configureNode(data.node);
}

/*----------------------------------------------------------------------------*/
void onNodeDisconnected(Device device, Device::NodeRemovedData data)
{
	if (data.node.is<DepthNode>() && (data.node.as<DepthNode>() == g_dnode))
		g_dnode.unset();
	printf("Node disconnected\n");
}

/*----------------------------------------------------------------------------*/
void onDeviceConnected(Context context, Context::DeviceAddedData data)
{
	if (!g_bDeviceFound)
	{
		data.device.nodeAddedEvent().connect(&onNodeConnected);
		data.device.nodeRemovedEvent().connect(&onNodeDisconnected);
		g_bDeviceFound = true;
	}
}

/*----------------------------------------------------------------------------*/
void onDeviceDisconnected(Context context, Context::DeviceRemovedData data)
{
	g_bDeviceFound = false;
	printf("Device disconnected\n");
}

/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	g_context = Context::create("localhost");

	g_context.deviceAddedEvent().connect(&onDeviceConnected);
	g_context.deviceRemovedEvent().connect(&onDeviceDisconnected);

	// Get the list of currently connected devices
	vector<Device> da = g_context.getDevices();

	// We are only interested in the first device
	if (da.size() >= 1)
	{
		g_bDeviceFound = true;

		da[0].nodeAddedEvent().connect(&onNodeConnected);
		da[0].nodeRemovedEvent().connect(&onNodeDisconnected);

		vector<Node> na = da[0].getNodes();

		printf("Found %u nodes\n", na.size());

		for (int n = 0; n < (int)na.size(); n++)
			configureNode(na[n]);
	}

	g_context.startNodes();

	g_context.run();

	g_context.stopNodes();

	if (g_dnode.isSet()) g_context.unregisterNode(g_dnode);

	if (g_pProjHelper)
		delete g_pProjHelper;

	return 0;
}
