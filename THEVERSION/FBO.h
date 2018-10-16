#ifndef FBO_H
#define FBO_H
#include "SafeTexture.h"
#include <string>
#include <GL3/gl3w.h>
#include <GL3/gl3.h>
#include <cstdlib>
//If you need more color attachments, change it.
#define MAX_COLOR_ATTACHMENTS 16
//See FBO.CPP for information on the usage and future plans
//This class APPEARS not to have any memory leaks. GJ.

//The first class to be packed into a single header file after not being so before.
namespace GeklminRender {
class FBO
{
	public:
	//Ordinary Constructor
		FBO(int width, int height, unsigned int howmanycolorattachments, GLenum InternalDataType);
	//Special case constructor. Used by the transparency pass to do a depth test.
		FBO(int width, int height, unsigned int howmanycolorattachments, GLenum InternalDataType, GLuint mylittledepthbuffer);


		void BindasTexture(unsigned int unit, unsigned int whichone);
		GLuint getHandle(unsigned int whichone); //Gets handle of a particular color attachment as a texture.
		void BindasTextureGeneric(unsigned int whichone);
		void BindRenderTarget();
		void BindDrawBuffers();
		GLuint getColorAttachmentTextureHandle(unsigned int whichone);
		
		//I DONT KNOW IF THESE FUNCTIONS CAN BE EXPORTED, THEY ARE STATIC...
		//Wait, yes... I think they can...
		static void SetActiveTexUnit(unsigned int unit);
		static void unBindRenderTarget(unsigned int screenwidth, unsigned int screenheight);
		static void clearTexture(float r, float g, float b, float a);
		unsigned int getWidth() const {return width;}
		unsigned int getHeight() const {return height;}
		unsigned int getNumColorAttachments() const {return numcolorattachments;}
		GLenum getmyInternalDataType() const {return myInternalDataType;}
		bool get_was_instantiated_with_Depth_Buffer() const {return was_instantiated_with_Depth_Buffer;}
		virtual ~FBO();
		GLuint getHandle() {return m_FBO;}
		SafeTexture* getTex(int attachment);
		GLuint getDepthBufferHandle() const;
		FBO(const FBO& Other);
	protected:
	private:
		//FBO(const FBO& Other) {} //Copy constructor. We'll make a new FBO using that FBO's specifications.
		void operator=(const FBO& FBO) {}

		GLuint m_FBO = 0; //The handle of this FBO
		bool was_instantiated_with_Depth_Buffer = false;
		GLuint m_DepthBuffer = 0; // The depth buffer handle
		GLuint* m_RenderTex = nullptr; //The pointer to where the render textures are stored
		unsigned int numcolorattachments = 0;
		unsigned int width;	//Self explanatory
		unsigned int height;	//Ditto
		GLenum DrawBuffers[MAX_COLOR_ATTACHMENTS]; //Holds the list of color attachments we will be rendering to, e.g. GL_COLOR_ATTACHMENT2
		GLenum myInternalDataType;
};

}; //Eof namespace

#endif
