/*
(C) David MHS Webster 2018
GkScene Demo Program 1 main.cpp
*/






//using namespace GeklminRender;



#include "geklminrender.h"
//My AL Utils
#include "GekAL.h"
unsigned int WIDTH = 640;
unsigned int HEIGHT = 480;

//Global Variables
GeklminRender::IODevice* myDevice = new GeklminRender::IODevice();
GeklminRender::GkScene* theScene; //Pointer to the scene






//Keyboard Related
int oldkeystates[51]; //nuff said
double oldmousexy[2]; //Old mouse position
double currentmousexy[2]; //Why query the current mouse position multiple times?


//Rendering Related
GeklminRender::Shader* MainShad = nullptr; //Final Pass Shader
GeklminRender::Shader* SkyboxShad = nullptr; //Skybox shader
GeklminRender::Shader* DisplayTexture = nullptr; //Displays a texture to the screen
GeklminRender::Shader* WBOITCompShader = nullptr; //Composites the WBOIT initial pass onto the opaque framebuffer

GeklminRender::Camera* SceneRenderCamera = nullptr; //SceneRender camera



//OUR OBJECTS
ALuint audiosource1 = 0;
ALuint audiobuffer1 = 0;
GeklminRender::Mesh* CubeMesh = nullptr;
GeklminRender::Mesh* SphereMesh = nullptr;
GeklminRender::MeshInstance myCube = GeklminRender::MeshInstance();
GeklminRender::MeshInstance mySphere = GeklminRender::MeshInstance();
GeklminRender::Texture* CloudsJpeg = nullptr;
GeklminRender::DirectionalLight theSun = GeklminRender::DirectionalLight();
GeklminRender::PointLight thePoint = GeklminRender::PointLight(glm::vec3(10,20,10), glm::vec3(1,1,1));
GeklminRender::Texture* AmigaPng = nullptr;


//Callbacks
void window_size_callback(GLFWwindow* window, int width, int height)
{
	using namespace GeklminRender;
	if (height == 0 || width == 0)
		return;
	if(theScene)
		theScene->resizeSceneViewport(width,height,1);
	WIDTH = width;
	HEIGHT = height;
	if (SceneRenderCamera != nullptr)
		SceneRenderCamera->buildPerspective(70, ((float)WIDTH)/((float)HEIGHT), 1, 1000);
	std::cout<<"\nNEW SIZE: "<< WIDTH <<" x "<<HEIGHT;
}


void init()
{
	//Why didn't I just put this at the top of the file???
	using namespace GeklminRender;
	//Creates the GLFW context. This pretty much has to be the first code we run
	myDevice->initGLFW();
	myDevice->pushWindowCreationHint(GLFW_RESIZABLE, GLFW_TRUE);
	//Set Version to OpenGL 3.3
	myDevice->pushWindowCreationHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	myDevice->pushWindowCreationHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	myDevice->addWindow(WIDTH, HEIGHT, "GkScene API on GeklminRender- Demo Program 2"); 
	// myDevice->addFullScreenWindow(WIDTH, HEIGHT, "GkScene API- Demo Program 2");
	
	myDevice->setWindowSizeCallback(0,window_size_callback);//Self explanatory
	myDevice->setContext(0); //Binds GL context of window 0 to current thread
	myDevice->initGL3W(); //Initialize the OpenGL extension wrangler.
	//myDevice->setKeyCallBack(0, key_callback); //Set a callback function.

	glEnable(GL_CULL_FACE); //Enable culling faces
	glEnable(GL_DEPTH_TEST); //test fragment depth when rendering
	glDepthMask(GL_TRUE);
	glCullFace(GL_BACK); //cull faces with clockwise winding
		
		//Standard error check code
			// GLenum communism = glGetError();
				// if (communism != GL_NO_ERROR)
				// {
					// std::cout<<"\n OpenGL reports an ERROR!";
					// if (communism == GL_INVALID_ENUM)
						// std::cout<<"\n Invalid enum.";
					// if (communism == GL_INVALID_OPERATION)
						// std::cout<<"\n Invalid operation.";
					// if (communism == GL_INVALID_FRAMEBUFFER_OPERATION)
						// std::cout <<"\n Invalid Framebuffer Operation.";
					// if (communism == GL_OUT_OF_MEMORY)
					// {
						// std::cout <<"\n Out of memory. You've really messed up. How could you do this?!?!";
						// std::abort();
					// }
				// }
	//User Input Variables being set to default. Hacky but works.
	for (int i = 0; i<51; i++)
		oldkeystates[i] = 0;
	for (int i = 0; i<2; i++)
		oldmousexy[i] = 0;

	for (int i = 0; i<2; i++)
		currentmousexy[i] = 0;
	theScene = new GkScene(WIDTH, HEIGHT, 1);
	
	//OpenAL Stuff
	//~ OpenALDevice = alcOpenDevice(NULL);
	//~ if (OpenALDevice)
	//~ {
		//~ std::cout << "\nUsing Device: " << alcGetString(OpenALDevice, ALC_DEVICE_SPECIFIER) << "\n";
		//~ OpenALContext = alcCreateContext(OpenALDevice, 0);
		//~ if(alcMakeContextCurrent(OpenALContext))
		//~ {
			//~ std::cout<<"\nSuccessfully Made Context!!!";
		//~ }
	//~ }
	//~ alGetError();
	myDevice->fastInitOpenAL();
}

void loadResources(){
	using namespace GeklminRender;
	//it puts .vs and .fs after the string. The EXE Is the starting folder, but I could probably change that if I tried.
	MainShad = new Shader("shaders/FORWARD_MAINSHADER");
	DisplayTexture = new Shader("shaders/SHOWTEX");
	SkyboxShad = new Shader("shaders/Skybox");
	WBOITCompShader = new Shader("shaders/WBOIT_COMPOSITION_SHADER");
	theScene->setSkyboxShader(SkyboxShad);
	theScene->setMainShader(MainShad);
	theScene->ShowTextureShader = DisplayTexture; //Add a setter later
	//theScene->setWBOITCompositionShader(WBOITCompShader); //Has a setter, and it's been a long time, I should write a setter for the ShowTextureShader
	
	
	//loadWAVintoALBuffer(const char* fn);
	
}

void initGame(){
	using namespace GeklminRender;
	SceneRenderCamera = new Camera(glm::vec3(0,1,-10),            //World Pos
								70.0f,                       //FOV
								(float)WIDTH/HEIGHT,		 //Aspect
								0.01f,                       //Znear
								1000.0f,                     //Zfar
								glm::vec3(0.0f, 0.0f, 1.0f), //forward
								glm::vec3(0.0f, 1.0f, 0.0f));//Up
	theScene->setSceneCamera(SceneRenderCamera);
	//Load lights and register them
	theScene->registerPointLight(&thePoint);
	theScene->RegisterDirLight(&theSun);
}

void checkKeys(){
	
}

void everyFrame(){
	
}

void Draw(){
	theScene->drawPipeline(1, nullptr, nullptr, nullptr, false, glm::vec4(0,0,0.1,0), glm::vec2(800,1000));
	myDevice->pollevents();
	myDevice->swapBuffers(0);
}

void cleanUp(){
	if(MainShad)
		delete MainShad;
	if(DisplayTexture)
		delete DisplayTexture;
	if(SkyboxShad)
		delete SkyboxShad;
	if(WBOITCompShader)
		delete WBOITCompShader;
	if(SceneRenderCamera)
		delete SceneRenderCamera;
	
	if(CubeMesh)
		delete CubeMesh;
	if(SphereMesh)
		delete SphereMesh;
	
	if(CloudsJpeg)
		delete CloudsJpeg;
	if(AmigaPng)
		delete AmigaPng;
	
	
	
	if(theScene)
		delete theScene;
	if(myDevice)
		delete myDevice;
}

int main(){
	init();
	loadResources();
	initGame();
	while (!myDevice->shouldClose(0)) //Main game loop.
	{
		checkKeys();
		everyFrame();
		Draw();
	}
	
	cleanUp();
	return 0;
}
