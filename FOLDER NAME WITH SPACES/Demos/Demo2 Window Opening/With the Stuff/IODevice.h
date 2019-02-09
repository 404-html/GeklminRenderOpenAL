#ifndef IODEVICE_H
#define IODEVICE_H
#include <iostream>
#include <vector>
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <string>
namespace GeklminRender {
class IODevice //This class exists so that my engine will support fancy window shit, like drawing all the different stages of rendering to different windows. Will help a lot with shader development.
{
    public:
        IODevice(){
			
		};
		~IODevice()
		{
			if(OpenALDevice)
				alcCloseDevice(OpenALDevice);
			for (int i = 0; i<ourWindows.size(); i++)
		   {
			   removeWindow(i);
		   }
		}
		void setWindowCloseCallback (int index, GLFWwindowclosefun cbfun){
			if (ourWindows.size() > index)
				glfwSetWindowCloseCallback(ourWindows[index], cbfun);
		}
		void setDropCallback (int index, GLFWdropfun cbfun)
		{
			if (ourWindows.size() > index)
				glfwSetDropCallback(ourWindows[index], cbfun);
		}
		void setCursorPositionCallback(int index, GLFWcursorposfun cbfun){
			if (ourWindows.size() > index)
				glfwSetCursorPosCallback(ourWindows[index], cbfun);
		}
		int getKey(int index, int key) {
			if (ourWindows.size() > index)
				return glfwGetKey(ourWindows[index], key);
			else
				return -1;
		}
		int getMouseButton(int index, int key){
			if (ourWindows.size() > index)
				return glfwGetMouseButton(ourWindows[index], key);
			else
				return -1;
		}
		void setMouseButtonCallback(int index, GLFWmousebuttonfun func){
			if (ourWindows.size() > index)
				glfwSetMouseButtonCallback(ourWindows[index], func);
		}
		void setKeyCallBack(int index, GLFWkeyfun cbfun){
			if (ourWindows.size() > index)
				glfwSetKeyCallback(ourWindows[index], cbfun);
		}
        void addWindow(int width, int height, const char* title)
		{
			ourWindows.push_back(
				glfwCreateWindow(width, height, title,  NULL, NULL)
			);
			if (!ourWindows[ourWindows.size()-1])
			{
				glfwTerminate();
				std::cout<<"\nFailed to create the window for some reason.";
				exit(EXIT_FAILURE);
			}
		}
		void addFullScreenWindow(int width, int height, const char* title){
			ourWindows.push_back(
				glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), NULL)
			);
			if (!ourWindows[ourWindows.size()-1])
			{
				glfwTerminate();
				std::cout<<"\nFailed to create the window for some reason.";
				exit(EXIT_FAILURE);
			}
		}
		void pushWindowCreationHint(int hint, int value){
			glfwWindowHint(hint, value); // This is where I left off.
		}
		void HintNextWindowResizeable(bool input){
			if (input)
				glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			else
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}
		void DefaultWindowHints(){
			glfwDefaultWindowHints();
		}
		void setWindowSizeCallback(int index, GLFWwindowsizefun cb)
		{
			if (ourWindows.size() > index)
			{
				glfwSetWindowSizeCallback(ourWindows[index],cb);
			}
		}
		void removeWindow(int index)
		{
			if (ourWindows.size() > index)
			{
				glfwDestroyWindow(ourWindows[index]);
				ourWindows.erase(ourWindows.begin() + index);
			}
		}
		void setContext(int index) //This is the command you have to use in Multithreaded programs to bind the GL context to the current thread.
		{
			if (ourWindows.size() > index)
				glfwMakeContextCurrent(ourWindows[index]);
		}
		void swapBuffers(int index)
		{
			if (ourWindows.size() > index)
				glfwSwapBuffers(ourWindows[index]);
		}
		void getCursorPosition(int index, double* x, double* y)
		{
			if (ourWindows.size() > index)
				glfwGetCursorPos(ourWindows[index], x, y);
		}
		void setCursorPosition(int index, double xpos, double ypos){
			if (ourWindows.size() > index)
				glfwSetCursorPos(ourWindows[index], xpos, ypos);
		}
		void setInputMode(int index, int mode, int value){
			if (ourWindows.size() > index)
				glfwSetInputMode(ourWindows[index], mode, value);
		}
		int getInputMode(int index, int mode){
			if (ourWindows.size() > index)
				return glfwGetInputMode(ourWindows[index], mode);
			else
				return 0;
		}
		const char* getClipboardString(int index){
			if (ourWindows.size() > index)
				return glfwGetClipboardString(ourWindows[index]);
			else
				return nullptr;
		}
		void setClipboardString(int index, const char* string){
			if (ourWindows.size() > index)
				glfwSetClipboardString(ourWindows[index],string);
		}
		void getWindowSize(int index, int* width, int* height){
			if (ourWindows.size() > index)
				glfwGetWindowSize(ourWindows[index], width, height);
		}
		void setWindowSize(int index, int width, int height){
			if (ourWindows.size() > index)
				glfwSetWindowSize(ourWindows[index], width, height);
		}
		int shouldClose(int index){
			if (ourWindows.size() > index && !glfwWindowShouldClose(ourWindows[index]))
				return glfwWindowShouldClose(ourWindows[index]);
			else if (ourWindows.size() > index)
				{
					checkWindows(index);
					return 1;
				}
			else
				return 1;
		}
		void checkWindows(int index){
			if (ourWindows.size() > index){
				if (glfwWindowShouldClose(ourWindows[index])){
					removeWindow(index);
				}
			}
		}
		int getWindowAttrib(int index, int attribute){
			if (ourWindows.size() > index)
				return glfwGetWindowAttrib(ourWindows[index], attribute);
			else
				return -99999;
		}
		static void pollevents() {glfwPollEvents();}
		static void SwapInterval(int interval) {glfwSwapInterval(interval);}
		static void initGLFW(){glfwInit();glfwSetErrorCallback(error_callback);}
		static void initGL3W(){gl3wInit();}
		static double getTime() {return glfwGetTime();}
		static void setTime(double newtime) {glfwSetTime(newtime);}
		static int JoyStickPresent(int joy) {
			return glfwJoystickPresent(joy);
		}
		static const float* getJoystickAxes(int joy, int* count){
			return glfwGetJoystickAxes(joy, count);
		}
		static const unsigned char* GetJoystickButtons (int joy, int* count){
			return glfwGetJoystickButtons(joy, count);
		}	
		GLFWwindow* getPointerAtIndex(int input){
			if (ourWindows.size() > input)
				return ourWindows[input];
			else
				return nullptr;
		}
		void SetWindowTitle(int window, const char* title){
			if (ourWindows.size() > window)
				glfwSetWindowTitle(ourWindows[window], title);
		}
		//glfwSetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images);
		void SetWindowIcon(int window, int count, const GLFWimage* images){
			if (ourWindows.size() > window)
				glfwSetWindowIcon(ourWindows[window], count, images);
			//glfwSetWindowTitle(ourWindows[window], title);
		}
		ALCdevice *OpenALDevice = 0;
		ALCcontext *OpenALContext = 0;
		void fastInitOpenAL(){
			if (OpenALDevice)
				{alcCloseDevice(OpenALDevice);OpenALDevice = 0;}
			OpenALDevice = alcOpenDevice(NULL);
			if (OpenALDevice)
			{
				//Enable these printouts for debugging
				//std::cout << "\nUsing Device: " << alcGetString(OpenALDevice, ALC_DEVICE_SPECIFIER) << "\n";
				OpenALContext = alcCreateContext(OpenALDevice, 0);
				if(alcMakeContextCurrent(OpenALContext))
				{
					//std::cout<<"\nSuccessfully Made Context!!!";
				}
			}
			alGetError();
		}
    protected:

    private:
	static void error_callback(int error, const char* description)
	{
		std::cout << "\n GLFW ERROR: \n" << description;
	}
		IODevice(const IODevice& other);
        IODevice& operator=(const IODevice& other);
		std::vector<GLFWwindow*> ourWindows;
};
}; //Eof Namespace
#endif // IODevice_H
