#include "geklminrender.h"
#include "TracerDemo.h"
using namespace GeklminRender;//For convenience... don't do this in your programs normally



//This allows us to easily interface with your computer!
IODevice* myDevice = new IODevice();

//Only used for Screenquadding function.
GkScene* theScene = nullptr;

//Screenquads a texture to the screen
Shader* showTextureShader = nullptr;

//Our Frame Buffer... of sorts.
Texture* CPUFrameBuffer = nullptr;

//We will grab this pointer from the texture after initializing it to black
unsigned char* CPUFrameBufferData = nullptr;

//Variables used for creating the window
int WIDTH = 1000;
int HEIGHT = 1000;



//The CPU FrameBuffer can be a different resolution than the window, but that isn't important, so i'll just make it the same size.
const int CPUWIDTH = 500;
const int CPUHEIGHT = 500;
const int CPUCOMPONENTS = 4; //RGBA... don't change this unless you KNOW what you are doing




//Demo for the Tracer
size_t FrameIncrementedVariable = 0;
TracerWorld theWorld;
float screenDistance = 0.6;
glm::vec3 eye = glm::vec3(0,0,-screenDistance);
glm::vec3 eyedir = glm::vec3(0,0,1); //PRE-NORMALIZD
glm::vec3 PreComputedPoints[CPUWIDTH][CPUHEIGHT]; //Pre-computed ray directions
std::vector<TraceableShape*> TraceableShapes;


void init(){
	//Bare Minimum to open a window...
	myDevice->initGLFW();
	myDevice->pushWindowCreationHint(GLFW_RESIZABLE, GLFW_FALSE);//The next window will be created with the hints we push on! In this case, we do not want the window to be resizeable, so we make it un-resizeable
	myDevice->addWindow(WIDTH, HEIGHT, "CPU Rendering Demo");//Create the window!
	myDevice->setContext(0); //Binds GL context of window 0 to current thread
	myDevice->initGL3W(); //Initialize the OpenGL extension wrangler.
	//At this point the OpenGL Context as been established and we are ready to use OpenGL Functions!
	
	//Again, only used for screenquadding
	theScene = new GkScene(WIDTH, HEIGHT, 1);
	
	//Shows Texture to Screen
	showTextureShader = new Shader("shaders/SHOWTEX");
	
	//Registering the shader that screenquads to the screen
	theScene->ShowTextureShader = showTextureShader; //Don't get confused lol
	
	//Initialize the CPUFrameBuffer with an empty framebuffer that's all black
	std::vector<unsigned char> data_init(CPUWIDTH * CPUHEIGHT * CPUCOMPONENTS, (unsigned char)128);
	//                                                V that part has to change if you want a different number of components
	CPUFrameBuffer = new Texture(CPUWIDTH, CPUHEIGHT, CPUCOMPONENTS, &data_init[0], GL_NEAREST, GL_NEAREST, GL_REPEAT, 0.0f);
	
	//We will be copying it over every frame.
	//CPUFrameBufferData = (unsigned char*)malloc(CPUWIDTH * CPUHEIGHT * CPUCOMPONENTS);
	
	//Or using the one it gives us...
	CPUFrameBufferData = CPUFrameBuffer->getDataPointerNotConst();
	
	//We don't actually NEED these but I like to play it safe
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
}
void initObjects(){
	
}
void checkInput(){
	
}
void setup(){
	
	//Demo Tracer Code
	FrameIncrementedVariable = 0;
	//PreComputedPoints[CPUWIDTH][CPUHEIGHT];
	float widthum = (float)CPUWIDTH;
	float heightum = (float)CPUHEIGHT;
	for (int w_iter = 0; w_iter < CPUWIDTH; w_iter++)
			for(int h_iter = 0; h_iter < CPUHEIGHT; h_iter++)
			{
				
				PreComputedPoints[w_iter][h_iter] = glm::vec3(
					(float)w_iter/widthum - 0.5,
					(float)h_iter/heightum - 0.5,
					0
				);
				if (h_iter == 3 && false)
					std::cout << "PRE-COMPUTED POSITION:\nW: "<<w_iter<<"\nH: "<<h_iter<<"\nis: \nX: " <<PreComputedPoints[w_iter][h_iter].x <<"\nY: "<<PreComputedPoints[w_iter][h_iter].y;
			}
	//Put some lights in the scene
	theWorld.TracerLights.push_back(TraceablePointLight());
	theWorld.TracerLights[0].position = glm::vec3(0,1,1.5);
	theWorld.TracerLights[0].color = glm::vec3(0.8,0.8,0.8);
	//Generate some sphere and place them some interesting places
	glm::vec3 pos1 = glm::vec3(-1,0,2);
	glm::vec3 pos2 = glm::vec3(1,0,2);
	glm::vec3 pos3 = glm::vec3(0,1.5,3);
	glm::vec3 color1 = glm::vec3(1,0,0);
	glm::vec3 color2 = glm::vec3(0,1,0);
	glm::vec3 color3 = glm::vec3(0.5,0.5,0);
	float specdamp3 = 5;
	float reflectivity3 = 0.4;
	float radii = 1;
	TraceableShapes.push_back(new Sphere(pos1, color1, radii));
	TraceableShapes[TraceableShapes.size()-1]->reflectivity = 0.5;
	theWorld.TracerObjects.push_back(TraceableShapes[TraceableShapes.size()-1]);
	
	TraceableShapes.push_back(new Sphere(pos2, color2, radii));
	TraceableShapes[TraceableShapes.size()-1]->reflectivity = 0.9;
	theWorld.TracerObjects.push_back(TraceableShapes[TraceableShapes.size()-1]);
	
	TraceableShapes.push_back(new Sphere(pos3, color3, 0.3));
	TraceableShapes[TraceableShapes.size()-1]->reflectivity = reflectivity3;
	TraceableShapes[TraceableShapes.size()-1]->specularDampening = specdamp3;
	theWorld.TracerObjects.push_back(TraceableShapes[TraceableShapes.size()-1]);
	//Generate some spheres and place them some places
	if (true)
		for (int i = 0; i < 10; i++)
		{
			glm::vec3 pos = glm::vec3(
			((float)(rand()%1000) - 500.0f)/500.0f * 10.0f,
			((float)(rand()%1000) - 500.0f)/500.0f * 10.0f,
			((float)(rand()%1000) - 500.0f)/500.0f * 10.0f + 10.0f
			);
			if (i==0)
				pos = glm::vec3(
				0,
				0,
				100.0f
				);
			glm::vec3 color = glm::vec3(
			((float)(rand()%1000))/1000.0f,
			((float)(rand()%1000))/1000.0f,
			((float)(rand()%1000))/1000.0f
			);
			if(i==0)
			color = glm::vec3(
			0,
			0.8,
			0
			);
			TraceableShapes.push_back(new Sphere(pos, color, 0.1f+((float)(rand()%1000))/1000.0f));
			theWorld.TracerObjects.push_back(TraceableShapes[TraceableShapes.size()-1]);
		}
}
void Update(){
	//Game code I guess
	FrameIncrementedVariable++; //This will fail eventually. Are you going to live that long though? 9,512,937,595.13 years is a long time... and that's a conservative estimate
}
void WritePixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b){
	// if (x > CPUWIDTH || y > CPUHEIGHT || CPUCOMPONENTS < 3 || CPUFrameBufferData ==  nullptr)
		// return;
	
	// if (CPUCOMPONENTS > 0)
		CPUFrameBufferData[(y * CPUWIDTH + x) * CPUCOMPONENTS + 0] = r;
	//  index              (pixel           ) * bytes/pixel   + offset
	// if (CPUCOMPONENTS > 1)
		CPUFrameBufferData[(y * CPUWIDTH + x) * CPUCOMPONENTS + 1] = g;
	//  index              (pixel           ) * bytes/pixel   + offset
	// if (CPUCOMPONENTS > 2)
		CPUFrameBufferData[(y * CPUWIDTH + x) * CPUCOMPONENTS + 2] = b;
	//  index              (pixel           ) * bytes/pixel   + offset
	// if (CPUCOMPONENTS > 3)
		// CPUFrameBufferData[(y * CPUWIDTH + x) * CPUCOMPONENTS + 3] = a;
	//  // index              (pixel           ) * bytes/pixel   + offset
}
void DrawScene(){
	//Write your drawing to the CPU frame buffer here
	
	//This is my example
		for (int w_iter = 0; w_iter < CPUWIDTH; w_iter++)
			for(int h_iter = 0; h_iter < CPUHEIGHT; h_iter++)
			{
				glm::vec3 RayDirection;
				RayDirection.x = PreComputedPoints[w_iter][h_iter].x - eye.x;
				RayDirection.y = PreComputedPoints[w_iter][h_iter].y - eye.y;
				RayDirection.z = PreComputedPoints[w_iter][h_iter].z - eye.z;
				// RayDirection = glm::normalize(RayDirection);
				//std::cout << "\nRAYDIRECTION \nX: " << RayDirection.x << "\nY: " << RayDirection.y << "\nZ: " << RayDirection.z;
				glm::vec3 pixColor = theWorld.TraceRay(glm::normalize(RayDirection), eye, 3, 300.0f);
				int red = (int)(pixColor.x * 255.0f)%256;
				int green = (int)(pixColor.y * 255.0f)%256;
				int blue = (int)(pixColor.z * 255.0f)%256;
				WritePixel(w_iter, h_iter, (unsigned char)(red), (unsigned char)(green), (unsigned char)(blue));
			}
	//And finally...
	//delete CPUFrameBuffer;
	//CPUFrameBuffer = new Texture(CPUWIDTH, CPUHEIGHT, CPUCOMPONENTS, CPUFrameBufferData, GL_LINEAR, GL_LINEAR, GL_REPEAT, 0.0f);
	CPUFrameBuffer->reInitFromDataPointer(true,true);
}
void DrawToScreen(){
	
	FBO::unBindRenderTarget(WIDTH,HEIGHT);
	FBO::clearTexture(0,0,0,1); 
	
	//Bind our Shader
	showTextureShader->Bind();
	
	//Bind CPUFrameBuffer for screenquadding
	CPUFrameBuffer->Bind(0);
	
	//Display to Screen
	theScene->ScreenquadtoFBO(showTextureShader);
}

void cleanUp(){
	while (TraceableShapes.size() > 0)
	{
		delete (TraceableShapes[0]);
		TraceableShapes.erase(TraceableShapes.begin());
	}
}

int main(){
	init();
	myDevice->setContext(0);
	std::cout << "Hello World!" << std::endl;
	GLenum communism;// = glGetError();
	
	
	
	setup();
	
	
	
	//Standard error check code
	communism = glGetError(); //Ensure there are no errors listed before we start.
	if (communism != GL_NO_ERROR) //if communism has made an error (which is pretty typical)
	{
		std::cout<<"\n OpenGL reports an ERROR!";
		if (communism == GL_INVALID_ENUM)
			std::cout<<"\n Invalid enum.";
		if (communism == GL_INVALID_OPERATION)
			std::cout<<"\n Invalid operation.";
		if (communism == GL_INVALID_FRAMEBUFFER_OPERATION)
			std::cout <<"\n Invalid Framebuffer Operation.";
		if (communism == GL_OUT_OF_MEMORY)
		{
			std::cout <<"\n Out of memory. You've really done it now. I'm so angry, i'm going to close the program. ARE YOU HAPPY NOW, DAVE?!?!";
			std::abort();
		}
	}
	
	
	//FBO::unBindRenderTarget(WIDTH,HEIGHT); //Ensure nothing is bound... I mean... if you WANT to edit this demo...
	while (!myDevice->shouldClose(0))
	{
		//Technical stuff
		myDevice->pollevents();
		myDevice->swapBuffers(0);
		//All the wonderful code you wrote!
		Update();
		DrawScene();
		DrawToScreen();
		
		if (communism != GL_NO_ERROR) //if communism has made an error (which is pretty typical)
		{
			std::cout<<"\n OpenGL reports an ERROR!";
			if (communism == GL_INVALID_ENUM)
				std::cout<<"\n Invalid enum.";
			if (communism == GL_INVALID_OPERATION)
				std::cout<<"\n Invalid operation.";
			if (communism == GL_INVALID_FRAMEBUFFER_OPERATION)
				std::cout <<"\n Invalid Framebuffer Operation.";
			if (communism == GL_OUT_OF_MEMORY)
			{
				std::cout <<"\n Out of memory. You've really done it now. I'm so angry, i'm going to close the program. ARE YOU HAPPY NOW, DAVE?!?!";
				std::abort();
			}
		}
	}
	cleanUp();
	delete myDevice;
	return 0;
}
