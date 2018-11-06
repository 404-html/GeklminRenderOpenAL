/*
(C) David MHS Webster 2018
GkScene Demo Program 1 main.cpp
*/





















#include "geklminrender.h"
#include "font.h"
#include "resource_manager.h"
#include "Global_Variables.h" //theScene and FileResourceManager
//My AL Utils
#include "GekAL.h"
//#define GLFW_DLL // Do we need this? //No.
//(C) DMHSW 2018 All Rights Reserved

unsigned int WIDTH = 1920;
unsigned int HEIGHT = 1080;
bool useFBO = false; //Should the next object spawned use the test FBO texture?
bool holdingShift = false; //used for sprinting
double progtime = 0.0;
bool shouldQuit = false;
bool creepilyfacetowardthecamera = true;
bool deRegisterOnButtonPress = true;
//std::vector<RenderableMesh*> RendMeshArray; //TO BE DEPRECATED
std::vector<GeklminRender::PointLight*> PointLightsWithoutShadows;
std::vector<GeklminRender::DirectionalLight*> Dir_Lights;
std::vector<GeklminRender::AmbientLight*> Amb_Lights;
std::vector<GeklminRender::CameraLight*> Cam_Lights;

GeklminRender::Mesh* InstancedMesh = nullptr;
std::vector<GeklminRender::MeshInstance*> ProgramMeshInstances; //Instances of meshes.
std::vector<GeklminRender::FBO*> FBOArray; //FBOs, not including ones used by GkScene
GeklminRender::MeshInstance TerrainInstance;//The terrain meshinstance. To be registered to
GeklminRender::MeshInstance LetterTester;//Meshinstance for testing all letters

GeklminRender::Shader* MainShad = nullptr; //Final Pass Shader
GeklminRender::Shader* SkyboxShad = nullptr; //Skybox shader
GeklminRender::Shader* DisplayTexture = nullptr; //Displays a texture to the screen
GeklminRender::Shader* WBOITCompShader = nullptr; //Composites the WBOIT initial pass onto the opaque framebuffer


GeklminRender::Camera* SceneRenderCamera = nullptr; //SceneRender camera
GeklminRender::Camera RenderTargetCamera; //The render target camera
GeklminRender::Camera RenderTargetCameraShadowMapping; //render target camera for shadowmapping
GeklminRender::CubeMap* SkyboxTex = nullptr; //Skybox texture
GeklminRender::CubeMap* SkyboxTwo = nullptr; //Second skybox texture, for testing per-mesh cubemaps

GeklminRender::Font* myFont = nullptr; //my super special font!
GeklminRender::Mesh* DeleteMeshTest = nullptr;


//OpenAL Variables
//~ ALCdevice *OpenALDevice = 0;
//~ ALCcontext *OpenALContext = 0;
//~ ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
//For the actual audio we're going to play
ALuint audiosource1 = 0;
ALuint audiobuffer1 = 0;



/*
User Input Variables and Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
bool cameralock; //Should the mouse be locked and enable first person mouse control?
int oldkeystates[51]; //nuff said
double oldmousexy[2]; //Old mouse position
double currentmousexy[2]; //Why query the current mouse position multiple times?

//KEYBOARD CALLBACK FUNCTIONS~~~~~~~~~~~~~
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        return;
}

//MOUSE BUTTON CALLBACK FUNCTIONS~~~~~~~~~~~~~~~~
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    return;
}



//Callback for custom Mainshader rendering and uniform binding
void MainshaderUniformFunctionDemo(int meshmask, GeklminRender::FBO* CurrentRenderTarget, GeklminRender::FBO* RenderTarget_Transparent, GeklminRender::Camera* CurrentRenderCamera, bool doFrameBufferChecks, glm::vec4 backgroundColor, glm::vec2 fogRangeDescriptor)
{
	// std::cout<<"The Uniform Binding Function works, replace the code here with your uniform binds!!!"<<std::endl;
	// Put your own code here for binding uniform variables... Keep in mind you have to get their locations and save them yourself!
}

void CustomRenderingFunction(int meshmask, GeklminRender::FBO* CurrentRenderTarget, GeklminRender::FBO* RenderTarget_Transparent, GeklminRender::Camera* CurrentRenderCamera, bool doFrameBufferChecks, glm::vec4 backgroundColor, glm::vec2 fogRangeDescriptor)
{
	// std::cout<<"The Custom Rendering Function works, replace the code here with your rendering code."<<std::endl;
	//Put your custom rendering code in here. Make sure you reset the GL state settings (e.g. glEnable(GL_DEPTH_TEST)) if you change them at any point. You can bind shaders as you please. This function takes place between rendering the skybox and rendering the objects in the scene, before MainShader is bound.
}
//A stupid way to do key checking but it WORKS!!!
void checkKeys(){
	using namespace GeklminRender;
	static int state;

	//Keyboard Keys
		state = myDevice->getKey(0, GLFW_KEY_W);
			if (state == GLFW_PRESS)
			{
				if (!holdingShift)
					SceneRenderCamera->MoveForward(0.20);
				else
					SceneRenderCamera->MoveForward(10.0);
			}
			oldkeystates[0] = state;
		state = myDevice->getKey(0, GLFW_KEY_A);
			if (state == GLFW_PRESS)
				SceneRenderCamera->MoveRight(0.20);
			oldkeystates[1] = state;
		state = myDevice->getKey(0, GLFW_KEY_S);
			if (state == GLFW_PRESS)
			{
				if (!holdingShift)
					SceneRenderCamera->MoveForward(-0.20);
				else
					SceneRenderCamera->MoveForward(-10.0);
			}
			oldkeystates[2] = state;
		state = myDevice->getKey(0, GLFW_KEY_D);
			if (state == GLFW_PRESS)
				SceneRenderCamera->MoveRight(-0.20);
			oldkeystates[3] = state;
		state = myDevice->getKey(0, GLFW_KEY_C);
			if (state == GLFW_PRESS && oldkeystates[4] != GLFW_PRESS)
			{cameralock = !cameralock;
			std::cout << "\nKey works";
			}
			oldkeystates[4] = state;
		state = myDevice->getKey(0, GLFW_KEY_J); //Spawn a mesh
			if (state == GLFW_PRESS && oldkeystates[5] != GLFW_PRESS)
			for (int i = 0; i < 1000; i++)
			{
				//Test out the Meshinstance registration feature
				ProgramMeshInstances.push_back(new MeshInstance(
					(rand()%100 > 50) ? 1 : 0, //Texture, demo of inline if. It's either 1 or 0.
					Transform(SceneRenderCamera->pos + glm::vec3(rand()%200-100 , rand()%200-100, rand()%200-100), glm::vec3(rand()%200-100 , rand()%200-100, rand()%200-100), glm::vec3(1,1,1))
				));
				//Randomly decide whether or not to use Cubemap Reflections
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->EnableCubemapReflections = (rand()%100 > 25) ? 1 : 0; //Note this does nothing because instancing is used...
				// ProgramMeshInstances[ProgramMeshInstances.size()-1]->EnableCubemapReflections = 1;
				//Randomly decide which cubemap to use.
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->cubeMap = 0;
				
				//Random Specular Dampening
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.specdamp = rand()%128 + 1.0;
				
				
				//Random Specular Reflectivity
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.specreflectivity = rand()%100/100.0;
				
				//Use specr to determine diffusivity
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.diffusivity = 1-ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.specreflectivity;
				
				
				InstancedMesh->RegisterInstance(
					ProgramMeshInstances[ProgramMeshInstances.size()-1]
				);
			}
			oldkeystates[5] = state;
		state = myDevice->getKey(0, GLFW_KEY_K);
			if (state == GLFW_PRESS && oldkeystates[6] != GLFW_PRESS)
			for (int i = 0; i < 1000; i++)
			{
				if (ProgramMeshInstances.size() > 0 && InstancedMesh->NumInstances() > 0) //if there are any to deregister
				{
					int haveDeRegistered = 0;
					int i = ProgramMeshInstances.size()-1;
					while (haveDeRegistered < 1 && i > -1) //While we might be able to deregister
					{
						bool donedeed = InstancedMesh->deRegisterInstance(ProgramMeshInstances[i]);
						if (donedeed)
						{
							delete ProgramMeshInstances[i];
							ProgramMeshInstances.erase(ProgramMeshInstances.begin() + i);
							haveDeRegistered++;
						}
						i--;
					}
					if (haveDeRegistered < 1)
						std::cout <<"\n ERROR! Unable to delete";
				}
			}
			oldkeystates[6] = state;
		state = myDevice->getKey(0, GLFW_KEY_H);
			if (state == GLFW_PRESS && oldkeystates[7] != GLFW_PRESS){
				std::cout << "\n BEGUN DELETION PROCESS~~~ There are" << PointLightsWithoutShadows.size() << " point lights right now";
				if (PointLightsWithoutShadows.size() > 0)
				{
					theScene->deRegisterPointLight(PointLightsWithoutShadows[PointLightsWithoutShadows.size()-1]);
					delete PointLightsWithoutShadows[PointLightsWithoutShadows.size()-1];
					PointLightsWithoutShadows.erase(PointLightsWithoutShadows.begin() +PointLightsWithoutShadows.size()-1);
				}
				std::cout << "\n FINISHED DELETION PROCESS~~~ There are" << PointLightsWithoutShadows.size() << " point lights right now";
			}
			oldkeystates[7] = state;
		state = myDevice->getKey(0, GLFW_KEY_G);
			if (state == GLFW_PRESS && oldkeystates[8] != GLFW_PRESS){
				//Make a light
				float tempr = (rand()%600)/900.0 + 0.33333333333333333333;
				float tempg = (rand()%600)/900.0 + 0.33333333333333333333;
				float tempb = (rand()%600)/900.0 + 0.33333333333333333333;
				// float tempr = 1;
				// float tempg = 1;
				// float tempb = 1;
				PointLightsWithoutShadows.push_back(
					new PointLight(SceneRenderCamera->pos, glm::vec3(tempr,tempg,tempb))
				);
				PointLightsWithoutShadows[PointLightsWithoutShadows.size()-1]->range = rand()%100 + 100;
				PointLightsWithoutShadows[PointLightsWithoutShadows.size()-1]->dropoff = 1;
				PointLightsWithoutShadows[PointLightsWithoutShadows.size()-1]->setPos(SceneRenderCamera->pos);
				theScene->registerPointLight(PointLightsWithoutShadows[PointLightsWithoutShadows.size()-1]);
			}
			oldkeystates[8] = state;
		state = myDevice->getKey(0, GLFW_KEY_LEFT_SHIFT);
			if (state == GLFW_PRESS) //We are testing for holding...
				holdingShift = true;
			else
				holdingShift = false;
			oldkeystates[9] = state;
		state = myDevice->getKey(0, GLFW_KEY_B);
			if (state == GLFW_PRESS && oldkeystates[10] != GLFW_PRESS)
			{
				std::cout << "\nCamera is located at X: " << SceneRenderCamera->pos.x << " Y: " << SceneRenderCamera->pos.y << " Z: " << SceneRenderCamera->pos.z;

				std::cout << "\n We have " << ProgramMeshInstances.size() << " Mesh Instances being rendered every frame.";
				
				std::cout << "\n We have " << PointLightsWithoutShadows.size() << " Point Lights being rendered every frame.";
				
				std::cout << "\nMAIN: " << ProgramMeshInstances.size() << 
				" Instanced Model Instances: " << InstancedMesh->NumInstances() << 
				" Sphere Model Instances: " << FileResourceManager->loadMesh("sphere_test.obj",false,true)->NumInstances();
				
				std::cout << "\n WIDTH: " << WIDTH << "\n HEIGHT: " << HEIGHT;
				RenderTargetCamera = *SceneRenderCamera;
				std::cout << "\n The Rendertarget Camera has been moved to the Camera's location";
				//Test FBO Information
				//FBOArray[0]->printcolorattachments();
				creepilyfacetowardthecamera = !creepilyfacetowardthecamera;
			}
			oldkeystates[10] = state;
		state = myDevice->getKey(0, GLFW_KEY_N);
			if (state == GLFW_PRESS && oldkeystates[11] != GLFW_PRESS)
			{
				useFBO = !useFBO;
			}
			oldkeystates[11] = state;
		state = myDevice->getKey(0, GLFW_KEY_Y);
			if (state == GLFW_PRESS && oldkeystates[12] != GLFW_PRESS)
			{
				if (PointLightsWithoutShadows.size() > 0)
				PointLightsWithoutShadows[0]->setPos(SceneRenderCamera->pos);
			}
			oldkeystates[12] = state;
		state = myDevice->getKey(0, GLFW_KEY_ESCAPE);
			if (state == GLFW_PRESS && oldkeystates[13] != GLFW_PRESS)
			{
				shouldQuit = true;
			}
			oldkeystates[13] = state;
		state = myDevice->getKey(0, GLFW_KEY_E);
			if (state == GLFW_PRESS && oldkeystates[14] != GLFW_PRESS)
			for(int i = 0; i < 25; i++)
			{
				//Test out the Meshinstance registration feature
				ProgramMeshInstances.push_back(new MeshInstance(
					(rand()%100 > 50) ? 1 : 0, //Texture, demo of inline if. It's either 1 or 0.
					Transform(SceneRenderCamera->pos+glm::vec3(rand()%100-50 , rand()%100-50, rand()%100-50), glm::vec3(rand()%100-50 , rand()%100-50, rand()%100-50), glm::vec3(1,1,1))
				));
				//Randomly decide whether or not to use Cubemap Reflections
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->EnableCubemapDiffusion = 0;
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->EnableCubemapReflections = (rand()%100 > 25) ? 1 : 0;
				//~ ProgramMeshInstances[ProgramMeshInstances.size()-1]->EnableCubemapReflections = 0;
				
				
				//Randomly decide which cubemap to use.
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->cubeMap = (rand()%100 > 50) ? 1 : 0;
				
				//Random Specular Dampening
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.specdamp = rand()%127 + 1;
				
				//Random Emissivity
				//ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.emissivity = false?(float(rand()%128)/128):0;
				
				//Random Specular Reflectivity
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.specreflectivity = rand()%100/100.0;
				
				//Random diffusivity
				//~ ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.diffusivity = 1-ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.specreflectivity;
				ProgramMeshInstances[ProgramMeshInstances.size()-1]->myPhong.diffusivity = rand()%100/100.0;
				
				
				FileResourceManager->loadMesh("sphere_test.obj",false,true)->RegisterInstance(
					ProgramMeshInstances[ProgramMeshInstances.size()-1]
				);

				// std::cout << "\n CREATED A " <<name;
			}
			oldkeystates[14] = state;
		state = myDevice->getKey(0, GLFW_KEY_Q);
			if (state == GLFW_PRESS && oldkeystates[15] != GLFW_PRESS)
			for (int i = 0; i < 10; i++)
			{
				if (ProgramMeshInstances.size() > 0 && FileResourceManager->loadMesh("sphere_test.obj",false,true)->NumInstances() > 0) //if there are any to deregister
				{
					int haveDeRegistered = 0;
					int i = ProgramMeshInstances.size()-1;
					while (haveDeRegistered < 1 && i > -1) //While we might be able to deregister
					{
						bool donedeed = FileResourceManager->loadMesh("sphere_test.obj",false,true)->deRegisterInstance(ProgramMeshInstances[i]);
						if (donedeed)
						{
							delete ProgramMeshInstances[i];
							ProgramMeshInstances.erase(ProgramMeshInstances.begin() + i);
							haveDeRegistered++;
						}
						i--;
					}
					if (haveDeRegistered < 1)
						std::cout <<"\n ERROR! Unable to delete";
				}
			}
			oldkeystates[15] = state;
		state = myDevice->getKey(0, GLFW_KEY_I);
			if (state == GLFW_PRESS && oldkeystates[16] != GLFW_PRESS)
			{
				if(deRegisterOnButtonPress)
					theScene->FastAndSafeDeregistration(nullptr, FileResourceManager->loadTexture("Amiga.png",true).getHandle());
				else
				{
					FileResourceManager->loadMesh("sphere_test.obj",false,true)->pushTexture(FileResourceManager->loadTexture("Amiga.png",true));
				}
				deRegisterOnButtonPress = !deRegisterOnButtonPress;
			}
		oldkeystates[16] = state;
		state = myDevice->getKey(0, GLFW_KEY_SPACE);
			if (state == GLFW_PRESS && oldkeystates[17] != GLFW_PRESS)
			{
				alSourcePlay(audiosource1);
			}
		oldkeystates[17] = state;
			
			
			
			
	//Mouse Keys. Yes, I know this is shitty and inefficient
		state = myDevice->getMouseButton(0, GLFW_MOUSE_BUTTON_LEFT);
			if (state == GLFW_PRESS && (oldkeystates[49] != GLFW_PRESS))
			{
				double mousepos[2];
				myDevice->getCursorPosition(0, &mousepos[0], &mousepos[1]);
				std::cout << "\n X: " << (mousepos[0]/WIDTH);
				std::cout << "\n Y: " << (mousepos[1]/HEIGHT);
				std::cout << "\nsizeof int pointer:" << sizeof(int*);
				InstancedMesh->optimizeCacheMemoryUsage();
				FileResourceManager->loadMesh("sphere_test.obj",false,true)->optimizeCacheMemoryUsage();
				//std::cout << "\nALL ASCII CHARACTERS:\n" <<"!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" << "\n";
				//std::cout << "\nCAPITAL I AABB DIMENSIONS:"<<
				//"\nX: " << myFont->CharacterAABBDimensions['I'].x <<
				//"\nY: " << myFont->CharacterAABBDimensions['I'].y <<
				//"\nZ: " << myFont->CharacterAABBDimensions['I'].z;
				if(DeleteMeshTest)
				{
					theScene->deregisterMesh(DeleteMeshTest);
					delete DeleteMeshTest;
					DeleteMeshTest = nullptr;
				}
				else
				{
					std::cout << "\nMaking Mesh";
					DeleteMeshTest = new GeklminRender::Mesh("SPHERE_TEST.OBJ", false, true, true);
					theScene->registerMesh(DeleteMeshTest);
					DeleteMeshTest->RegisterInstance(&LetterTester);
					DeleteMeshTest->pushTexture(FileResourceManager->loadTexture("AMIGA.PNG",true));
				}
			}
		oldkeystates[49] = state;
		state = myDevice->getMouseButton(0, GLFW_MOUSE_BUTTON_RIGHT);
			if (state == GLFW_PRESS && oldkeystates[50] != GLFW_PRESS)
			{
				// std::cout <<"\nProgrammed in C++ and OpenGL to test the functionality of GkScene rendering API middleware.\nSpecial Thanks:";
				// std::cout <<"\n DMHSW- Programming and Design";
				// std::cout <<"\n BennyBox- Teaching me OpenGL";
				// std::cout <<"\n GLM- Mathematics Libraries";
				// std::cout <<"\n GLFW- Window creation, OpenGL Context creation, and Device interface";
				// std::cout <<"\n OpenGL- API for graphics";
				// std::cout <<"\n (Your OpenGL Vendor)- Stunning hardware-accelerated 3D graphics.";
				// std::cout <<"\n\n This demo is licensed to you under the MiT License (https://mit-license.org/). Have fun.";
				// std::cout <<"\n\n if you find any bugs in the software (E.g. crashing) report them to gordonfreeman424@gmail.com";
				IndexedModel Boxy = createBox(1.0f,4.0f,3.0f);
				std::string OBJExportedBoxy = Boxy.exportToString(true);
				std::cout << "\n BEGIN PRINTOUT:\n";
				std::cout << OBJExportedBoxy;
			}
		oldkeystates[50] = state;
}





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
}


// init function. Should initialize all globals
void init()
{
	//Why didn't I just put this at the top of the file???
	using namespace GeklminRender;
	//Creates the GLFW context. This pretty much has to be the first code we run
	myDevice->initGLFW();
	//GkScene has a function for handling resizing, but we will work on that later. We have to set a resizefunc if we enable resizing
	myDevice->pushWindowCreationHint(GLFW_RESIZABLE, GLFW_TRUE);
	//Set Version to OpenGL 3.3
	myDevice->pushWindowCreationHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	myDevice->pushWindowCreationHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	myDevice->addWindow(WIDTH, HEIGHT, "GkScene API on GeklminRender Alpha- Demo Program 1"); 
	// myDevice->addFullScreenWindow(WIDTH, HEIGHT, "GkScene API Alpha- Demo Program 1");
	
	myDevice->setWindowSizeCallback(0,window_size_callback);//Self explanatory
	myDevice->setContext(0); //Binds GL context of window 0 to current thread
	myDevice->initGL3W(); //Initialize the OpenGL extension wrangler.
	myDevice->setKeyCallBack(0, key_callback); //Set a callback function.

	//Default Enabled OpenGL Options (NOTE: I'm going to add these sorts of things to the pipeline of GkScene and add toggles for the different steps. Maximize rendering options and whatnot.
	//I don't actually think we need to execute these commands here... WHATEVS!!!
		glEnable(GL_CULL_FACE); //Enable culling faces
		glEnable(GL_DEPTH_TEST); //test fragment depth when rendering
		glCullFace(GL_BACK); //cull faces with clockwise winding
		//glEnable(GL_ARB_conservative_depth); //conservative depth.
		
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
	cameralock = false;
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

//Load the resources from file for the demo.
void LoadResources()
{
	using namespace GeklminRender;
	//it puts .vs and .fs after the string. The EXE Is the starting folder, but I could probably change that if I tried.
	MainShad = new Shader("shaders/FORWARD_MAINSHADER");
	DisplayTexture = new Shader("shaders/SHOWTEX");
	SkyboxShad = new Shader("shaders/Skybox");
	WBOITCompShader = new Shader("shaders/WBOIT_COMPOSITION_SHADER");
	theScene->setSkyboxShader(SkyboxShad);
	theScene->setMainShader(MainShad);
	theScene->ShowTextureShader = DisplayTexture; //Add a setter later
	theScene->setWBOITCompositionShader(WBOITCompShader); //Has a setter, and it's been a long time, I should write a setter for the ShowTextureShader
	FBOArray.push_back(new FBO(640,480, 1, GL_RGBA8)); //0, the test FBO render target
	FBOArray.push_back(new FBO(640,480, 2, GL_RGBA16F, FBOArray[0]->getDepthBufferHandle())); //1, the FBO necessary for Weighted Blended OIT
	FBOArray.push_back(new FBO(640,480, 1, GL_RGBA32F)); //2, the FBO for rendering Shadowmaps
	theScene->registerCustomFBO(FBOArray[0]); //I was planning to use this to do reflections in the future, so I added a custom FBO registration feature.
	
	
	std::string cubemapfilenames[6] = {
			"Cubemap/Skybox_Water10_128_right.jpg", //right
			"Cubemap/Skybox_Water10_128_left.jpg", //left
			"Cubemap/Skybox_Water10_128_top.jpg", //up
			"Cubemap/Skybox_Water10_128_base.jpg", //down
			"Cubemap/Skybox_Water10_128_back.jpg", //back
			"Cubemap/Skybox_Water10_128_front.jpg" //front
	};
	SkyboxTex = new CubeMap(cubemapfilenames[0],cubemapfilenames[1],cubemapfilenames[2],cubemapfilenames[3],cubemapfilenames[4],cubemapfilenames[5]);
	
	
	
	std::string secondCubeMapFileNames[6] = {
		"Cubemap/2desert_RIGHT.bmp",
		"Cubemap/2desert_LEFT.bmp",
		"Cubemap/2desert_UP.bmp",
		"Cubemap/2desert_DOWN.bmp",
		"Cubemap/2desert_BACK.bmp",
		"Cubemap/2desert_FRONT.bmp"
	};
	SkyboxTwo = new CubeMap(secondCubeMapFileNames[0],secondCubeMapFileNames[1],secondCubeMapFileNames[2],secondCubeMapFileNames[3],secondCubeMapFileNames[4],secondCubeMapFileNames[5]);
	theScene->SetSkyBoxCubemap(SkyboxTex);
	
	//myFont->pushCubeMapToAllMeshes(SkyboxTex);//Cubemap reflections for chrome letters!
	/*
		Resources are things loaded FROM FILE
		AND/OR
		into GPU memory

		In a proper game, you'd actually load assets based on the current level

		and we would call a total unload upon the file resource manager
	*/
	//File Resources
	//FileResourceManager->loadMesh("Cube_Test_Low_Poly.obj",false,true);
	FileResourceManager->loadMesh("sphere_test.obj",false,true);
	FileResourceManager->loadMesh("First_Terrain_With_Caves.obj",false,true);
	InstancedMesh = new Mesh("Cube_Test_Low_Poly.obj",true,false,true); //Instanced, Static, Asset
	
	FileResourceManager->loadTexture("Amiga.png",true);
	FileResourceManager->loadTexture("Art.jpg",false);
	
	//MESH REGISTRATION
	theScene->registerMesh(FileResourceManager->loadMesh("sphere_test.obj",false,true));
	//theScene->registerMesh(FileResourceManager->loadMesh("Cube_Test_Low_Poly.obj",false,true));
	theScene->registerMesh(FileResourceManager->loadMesh("First_Terrain_With_Caves.obj",false,true));
	//Register the instanced
	theScene->registerMesh(InstancedMesh);
	
	//NotPointedTextureTest = Texture("Amiga.png", true);
	
	//Sphere_test Available Textures.
	FileResourceManager->loadMesh("sphere_test.obj",false,true)->pushTexture(FileResourceManager->loadTexture("Amiga.png",true)); //0
	//FileResourceManager->loadMesh("sphere_test.obj",false,true)->pushTexture(FileResourceManager->loadTexture("Art.jpg",false)); //1
	FileResourceManager->loadMesh("sphere_test.obj",false,true)->pushTexture(FileResourceManager->loadTexture("clouds.jpg",false)); //1
	
	
	//Non-instanced Mesh Available Textures.
	//FileResourceManager->loadMesh("Cube_Test_Low_Poly.obj",false,true)->pushTexture(FileResourceManager->loadTexture("Amiga.png",true)); //0
	//FileResourceManager->loadMesh("Cube_Test_Low_Poly.obj",false,true)->pushTexture(FileResourceManager->loadTexture("Art.jpg",false)); //1
	//NotPointedTextureTest = Texture()
	//Instanced Mesh texture and cubemap
	//InstancedMesh->pushTexture(FileResourceManager->loadTexture("clouds.jpg",false));
	InstancedMesh->pushTexture(*FBOArray[0]->getTex(0));
	InstancedMesh->pushCubeMap(SkyboxTex);
	
	//Sphere_test Cubemaps
	FileResourceManager->loadMesh("sphere_test.obj",false,true)->pushCubeMap(SkyboxTwo); //0
	FileResourceManager->loadMesh("sphere_test.obj",false,true)->pushCubeMap(SkyboxTex); //1
	
	//More Cubemaps
	//FileResourceManager->loadMesh("Cube_Test_Low_Poly.obj",false,true)->pushCubeMap(SkyboxTwo);//0
	//FileResourceManager->loadMesh("Cube_Test_Low_Poly.obj",false,true)->pushCubeMap(SkyboxTex);//1
	//Testing...
	theScene->customMainShaderBinds = &MainshaderUniformFunctionDemo;
	theScene->customRenderingAfterSkyboxBeforeMainShader = &CustomRenderingFunction; //Draw to your heart's content!
	
	//See GekAL.h for how this is done
	//~ audiobuffer1 = loadWAVintoALBuffer("SOUNDS/TONE.WAV");
	//Can we cache a sound?
	FileResourceManager->loadSound("SOUNDS/TONE.WAV");
	audiobuffer1 = FileResourceManager->loadSound("SOUNDS/TONE.WAV");
}


void initObjects()
{ //Default existing objects
	//Instance Instantiation
		using namespace GeklminRender;

		
		FileResourceManager->loadMesh("First_Terrain_With_Caves.obj",false,true)->RegisterInstance(&TerrainInstance);
		FileResourceManager->loadMesh("First_Terrain_With_Caves.obj",false,true)->pushTexture(FileResourceManager->loadTexture("CLOUDS.JPG",false)); //Note: we are Loading a texture here... should probably move to LoadResources
		
		
		TerrainInstance.myTransform = Transform(glm::vec3(0,0,0), glm::vec3(0 , 0, 0), glm::vec3(20,20,20));
		TerrainInstance.myPhong.specreflectivity = 0;
		TerrainInstance.myPhong.diffusivity = 1;
		TerrainInstance.mymeshmask = 4; //It wont render on a 3 renderpipeline call
		//~~~~~~~~~~~~~~~~~~~~~~~~~
		//Lights...
		PointLightsWithoutShadows.push_back(
			new PointLight(glm::vec3(10,20,10), glm::vec3(1,1,1))
		); //0 LIGHT
		PointLightsWithoutShadows[0]->range = 1000;
		PointLightsWithoutShadows[0]->dropoff = 1;
		theScene->registerPointLight(PointLightsWithoutShadows[0]);
		
		Dir_Lights.push_back(
			new DirectionalLight()
		);//0
		if(Dir_Lights.size() > 0)
		{
			Dir_Lights[0]->myDirection = glm::vec3(0,-1,0);
			Dir_Lights[0]->myColor = glm::vec3(0.3,0.3,0.3);
			Dir_Lights[0]->sphere1 = glm::vec4(0, 0, 0, 50 * 50);
			Dir_Lights[0]->sphere2 = glm::vec4(438, -8, 459, 80 * 80);
			Dir_Lights[0]->AABBp1 = glm::vec3(-200,-200,-200);
			Dir_Lights[0]->AABBp2 = glm::vec3(200,200,200);
			theScene->RegisterDirLight(Dir_Lights[0]);
		}
		
		
		//Register an Ambient Light
		Amb_Lights.push_back(
			new AmbientLight()
		); // 0
		// Amb_Lights.push_back(
			// new AmbientLight()
		// ); // 1
		// Amb_Lights.push_back(
			// new AmbientLight()
		// ); // 2
		// Amb_Lights.push_back(
			// new AmbientLight()
		// ); // 3
		// Amb_Lights.push_back(
			// new AmbientLight()
		// ); // 4
		if (Amb_Lights.size() > 0)
		{
			Amb_Lights[0]->myColor = glm::vec3(0.2,0.2,0.2);
			Amb_Lights[0]->myPos = glm::vec3(0,50,0);
			Amb_Lights[0]->myRange = 5000; //Note: This creates a harsh cutoff. Fix later?
			theScene->RegisterAmbLight(Amb_Lights[0]);
			Amb_Lights[0]->sphere1 = glm::vec4(0, 0, 0, 300 * 300);
		}
		if (Amb_Lights.size() > 1)
		{
			Amb_Lights[1]->myColor = glm::vec3(0,0.2,0.3);
			Amb_Lights[1]->myPos = glm::vec3(100,20,0);
			Amb_Lights[1]->myRange = 100;
			theScene->RegisterAmbLight(Amb_Lights[1]);
		}
		if (Amb_Lights.size() > 2)
		{
			Amb_Lights[2]->myColor = glm::vec3(0,0,0.2);
			Amb_Lights[2]->myPos = glm::vec3(-100,20,0);
			Amb_Lights[2]->myRange = 100;
			theScene->RegisterAmbLight(Amb_Lights[2]);
		}
		if (Amb_Lights.size() > 3)
		{
			Amb_Lights[3]->myColor = glm::vec3(0.2,0.2,0);
			Amb_Lights[3]->myPos = glm::vec3(0,20,100);
			Amb_Lights[3]->myRange = 100;
			theScene->RegisterAmbLight(Amb_Lights[3]);
		}
		if (Amb_Lights.size() > 4)
		{
			Amb_Lights[4]->myColor = glm::vec3(0.2,0.2,0.5);
			Amb_Lights[4]->myPos = glm::vec3(0,20,-100);
			Amb_Lights[4]->myRange = 100;
			theScene->RegisterAmbLight(Amb_Lights[4]);
		}
		// the SceneRender camera
		SceneRenderCamera = new Camera(glm::vec3(0,1,-10),            //World Pos
								70.0f,                       //FOV
								(float)WIDTH/HEIGHT,		 //Aspect
								0.01f,                       //Znear
								1000.0f,                     //Zfar
								glm::vec3(0.0f, 0.0f, 1.0f), //forward
								glm::vec3(0.0f, 1.0f, 0.0f));//Up
		RenderTargetCamera = *SceneRenderCamera;
		theScene->setSceneCamera(SceneRenderCamera);
		Cam_Lights.push_back(new CameraLight());
		Cam_Lights[0]->Tex2Project = FileResourceManager->loadTexture("Art.jpg",false);
		Cam_Lights[0]->myCamera = *SceneRenderCamera;
		Cam_Lights[0]->range = 300;
		theScene->RegisterCamLight(Cam_Lights[0]);
		LetterTester.myTransform.SetPos(glm::vec3(0,100,0));
		LetterTester.myTransform.SetScale(glm::vec3(10,10,10));
		//myFont->Characters['B']->RegisterInstance(&LetterTester);
		
		//OpenAL Source Generation
		alGenSources(1,&audiosource1);
		alSourcef(audiosource1, AL_GAIN, 1);
		alSourcef(audiosource1, AL_PITCH, 1);
		//Params
		alSource3f(audiosource1, AL_POSITION, 0, 0, 0);
		alSource3f(audiosource1, AL_VELOCITY, 0, 0, 0);
		alSourcei(audiosource1, AL_LOOPING, AL_FALSE);
		//What sound?
		alSourcei(audiosource1, AL_BUFFER, audiobuffer1);
}

int main()
{
	using namespace GeklminRender; //I didn't know that you could do "using namespace X" at the beginning of a file, I swear!
	init(); //Sets up all the OpenGL setup and stuff
	LoadResources(); //Load models and shaders and stuff
	initObjects(); //Create the initial objects we place into the scene
	Mesh BadDeer = Mesh("Bad Deer.obj", false, true, true);
	Texture NotPointedTextureTest = Texture("AMIGA.PNG", true);
	BadDeer.pushTexture(SafeTexture(&NotPointedTextureTest));//0
	BadDeer.pushCubeMap(SkyboxTex);//0
	theScene->registerMesh(&BadDeer);
	MeshInstance MyVeryBadDeer = MeshInstance();
	MyVeryBadDeer.tex = 0;
	MyVeryBadDeer.EnableCubemapReflections = 1;
	MyVeryBadDeer.myPhong.diffusivity = 0.6;
	MyVeryBadDeer.myPhong.specreflectivity = 0.4;
	MyVeryBadDeer.myTransform.SetScale(glm::vec3(10,10,10));
	BadDeer.RegisterInstance(&MyVeryBadDeer);
	//PRE-MAIN FIDDLINGS
		// glClearColor(0,0,0.3,0); //What color shall the backdrop be?
		float ordinarycounter = 0.0f; // Used to achieve the trigonometry wave effects
		//progtime = IODevice::getTime();

	while (!myDevice->shouldClose(0) && !shouldQuit) //Main game loop.
    {


		//COLLECT INFORMATION ABOUT THE SYSTEM
		progtime = IODevice::getTime();
		ordinarycounter += 0.1f;
			if (ordinarycounter > 1000.0f){
				ordinarycounter = 0.0f;
			}


		//INPUT DETECTION
		//We would re-route all information from input detection directly into other classes. E.g. we might have a player class and we would pass it the distance that the player moved their mouse. That would calculate what the new camera orientation would be.
		checkKeys();


		//GAME CODE | Demonstrates using transforms to set the rotation and position of an object in the scene. Quaternions can be used instead of euler angles to avoid gimbal lock.
		//It also demonstrates a first-person camera, which was figured out by BennyBox. Thanks for that.
		{
				//Changing shape demo
				// /*
					//IndexedModel tempshape = createBox(1, 1, 1);
					// for (size_t i = 0; i < tempshape.colors.size(); i++){
						// tempshape.colors[i] = glm::vec3((rand()%100)/100.0, (rand()%100)/100.0, (rand()%100)/100.0);
					// }
					//InstancedMesh->reShapeMesh(createBox(3, 3, 7));
					//NotPointedTextureTest.reInitFromDataPointer(true);
				// */
					// LetterTester.myTransform.SetRot(glm::vec3(sinf(ordinarycounter/10.0) * 5,sinf(ordinarycounter/10.0)*3,sinf(ordinarycounter/11.2)*2));
				for (size_t communists_killed = 0; communists_killed < ProgramMeshInstances.size() && communists_killed < 100; communists_killed++)
				{
					//faceTowardPoint(glm::vec3 pos, glm::vec3 target, glm::vec3 up)
					if (!creepilyfacetowardthecamera)
						ProgramMeshInstances[communists_killed]->myTransform.SetRot(glm::vec3(sinf(ordinarycounter/10.0) * 5,sinf(ordinarycounter/10.0)*3,sinf(ordinarycounter/11.2)*2));
					if (creepilyfacetowardthecamera)
						ProgramMeshInstances[communists_killed]->myTransform.SetRotQuat(
							GeklminRender::faceTowardPoint(
								ProgramMeshInstances[communists_killed]->myTransform.GetPos(), 
								SceneRenderCamera->pos, 
								glm::vec3(0,1,0)
							)
						);
					
					//*/
					// ProgramMeshInstances[communists_killed]->myPhong.specreflectivity = (1+sinf(ordinarycounter/100.0f))/2.0;
				}
				//Camera Positioning
				if (cameralock)
				{
					myDevice->getCursorPosition(0, &currentmousexy[0], &currentmousexy[1]);
					SceneRenderCamera->Pitch((float)(currentmousexy[1] - oldmousexy[1]) * 0.001f);
					SceneRenderCamera->RotateY((float)(currentmousexy[0] - oldmousexy[0]) * -0.001f);
					myDevice->setInputMode(0, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					myDevice->getCursorPosition(0, &oldmousexy[0], &oldmousexy[1]);

				} else {
					//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					myDevice->setInputMode(0, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				//glfwGetCursorPos(window, &oldmousexy[0], &oldmousexy[1]);
				myDevice->getCursorPosition(0, &oldmousexy[0], &oldmousexy[1]);
		}
		//Eof Game Code
		syncCameraStateToALListener(SceneRenderCamera);
		
		//theScene->drawPipeline(1, FBOArray[0], FBOArray[1], &RenderTargetCamera, false, glm::vec4(1.0,0,0,1.0), glm::vec2(300,500)); //Don't draw the ground
		theScene->drawPipeline(1, nullptr, nullptr, nullptr, false, glm::vec4(0,0,0,0), glm::vec2(800,1000));
		myDevice->pollevents();
		myDevice->swapBuffers(0);
	} //EOF game loop


								
	//We should delete stuff... if we want to be good programmers :<
	std::cout << "\n BEGINNING DELETION...";
	delete InstancedMesh; //TODO: Push this to 147
	delete FileResourceManager;
	if (MainShad)
		delete MainShad; //Initial Opaque shader, passed to the Scene class.
	if (SkyboxShad)
		delete SkyboxShad; //Skybox shader, passed to the scene class.
	if (DisplayTexture)
		delete DisplayTexture;
	if(WBOITCompShader) {delete WBOITCompShader;}
	if(SceneRenderCamera){delete SceneRenderCamera;}
	// delete DisplayTexture; //Displays a texture to the screen
	delete SkyboxTex;
	delete SkyboxTwo;
	theScene->deregisterMesh(&BadDeer);
	//myFont->deRegisterFontFromScene(theScene);
	delete theScene;
	//if (myFont)
	//	delete myFont;
	
	std::cout <<"\n DELETING MESHES...";
	while (ProgramMeshInstances.size() > 0)
		{
			delete ProgramMeshInstances[ProgramMeshInstances.size()-1]; //Note: I didn't check for null here. Bad practice?
			ProgramMeshInstances.erase(ProgramMeshInstances.end()-1);
		}
	if (PointLightsWithoutShadows.size()>0)
		for (size_t i = 0; i<PointLightsWithoutShadows.size(); i++)
		{
			if (PointLightsWithoutShadows[i]) //Returns true if it's not null. if it's not null, we want to delete it.
				delete PointLightsWithoutShadows[i];
		}
	if(Dir_Lights.size()>0)
		for (size_t i = 0; i<Dir_Lights.size(); i++)
		{
			if (Dir_Lights[i]) //Returns true if it's not null. if it's not null, we want to delete it.
				delete Dir_Lights[i];
		}
	if (Amb_Lights.size()>0)
		for (size_t i = 0; i < Amb_Lights.size(); i++)
		{
			if (Amb_Lights[i])
				delete Amb_Lights[i];
		}
	if(Cam_Lights.size() > 0) //TODO: Push this to 147
		for (size_t i = 0; i < Cam_Lights.size(); i++)
		{
			if (Cam_Lights[i])
				delete Cam_Lights[i];
		}
	std::cout<<"\n\n DELETED ALL LIGHTS!";
	
	if (FBOArray.size()>0)
	{
		for (size_t i = 0; i<FBOArray.size(); i++)
		{
			std::cout<<"\n\n DELETING FBO " << i;
			if (FBOArray[i]) //Returns true if it's not null. if it's not null, we want to delete it.
				delete FBOArray[i];
		}
	}
	//~ if(OpenALDevice)
		//~ alcCloseDevice(OpenALDevice);
	std::cout<<"\n\n DELETED THE FBOS";
	myDevice->removeWindow(0);
	std::cout << "\n DELETED THE WINDOW";
	delete myDevice;
	std::cout << "\n Peace!";
}
