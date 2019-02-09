#ifndef SAFETEXTURE_H
#define SAFETEXTURE_H
#include "texture.h"
#include <iostream>
#include <cassert>
#include <string>
#include <GL3/gl3w.h>
#include <GL3/gl3.h>
#include <glm/glm.hpp>
#include <cstdlib>

//This class is kind of obvious... All it needs to know about itself is the GLuint handle of the texture for binding...
//this is a stupid class
namespace GeklminRender {
class SafeTexture
{
	public:
		SafeTexture(){ //Default values
			m_texture = 0;
			isnull = true;
			isTransparent = false;
			DaddyO = nullptr;
			TextureUpdates = false;
		}
		SafeTexture(Texture* inTex);
		SafeTexture(GLuint texture_object);
		void Bind(unsigned int unit);
		void BindGeneric();
		bool amINull() const {return isnull;}
		bool amITransparent() const {return isTransparent;}
		static void SetActiveUnit(unsigned int unit);
		~SafeTexture(){
			//What needs to be destroyed? Nothing! NOTHING NEEDS TO BE DESTROYED!
			DaddyO = nullptr;
			TextureUpdates = false;
		};
		SafeTexture(const SafeTexture& other) {
			isTransparent = other.amITransparent();
			m_texture = other.getHandle();
			isnull = false;
			TextureUpdates = other.TextureUpdates;
			DaddyO = other.DaddyO;
		}
		// void operator=(const SafeTexture& other) {
			// isTransparent = other.amITransparent();
			// m_texture = other.getHandle();
			// isnull = false;
			// TextureUpdates = other.TextureUpdates;
			// DaddyO = other.DaddyO;
		// }
		bool operator==(const SafeTexture& other){
			return (other.getHandle() == getHandle());
		}
		GLuint getHandle() const {return m_texture;}
		bool TextureUpdates = false;
		Texture* DaddyO = nullptr;
	protected:
	
	private:
		bool isTransparent = false; //transparency flag. We will later be setting this!
		bool isnull = true; //To keep track of whether or not the texture pointer is null
		GLuint m_texture = 0; //Initialize Everything!
};
}; //Eof Namespace
#endif
