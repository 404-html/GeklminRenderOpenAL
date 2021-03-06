#ifndef TEXTURE_H
#define TEXTURE_H
#include <iostream>
#include <cassert>
#include <string>
#include <cstring>
#include <GL3/gl3w.h>
#include <GL3/gl3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <cstdlib>
// this code doesn't work for some reason. I don't know why.
// #ifndef STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
// #endif
//I'm compiling with GL3W, an old version too.
namespace GeklminRender {
const GLenum HACK_GL_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FE;
const GLenum HACK_GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = 0x84FF;
class Texture
{
	public:
		Texture(const std::string&, bool _enableTransparency, GLenum minfilter = GL_LINEAR, GLenum magfilter = GL_LINEAR, GLenum wrap_mode = GL_REPEAT, float anisotropic_level = 4.0f);
		Texture(){
			if(!isnull)
			{
				if (Permanent_Data_Pointer)
					free(Permanent_Data_Pointer);
				Permanent_Data_Pointer = nullptr;
				glDeleteTextures(1, &m_texture); //Delete the texture upon the asset being destroyed
				m_texture = 0;
			}
			Permanent_Data_Pointer = nullptr;
			isnull = true;
		}
		Texture(int size_w, int size_h, int size_components, unsigned char* data_ptr, GLenum min_filter, GLenum mag_filter, GLenum wrap_mode, float anisotropic_level){
			if(!isnull)
			{
				if (Permanent_Data_Pointer)
					free(Permanent_Data_Pointer);
				glDeleteTextures(1, &m_texture); //Delete the texture upon the asset being destroyed
				m_texture = 0;
			}
			//copy the image to our Permanent_Data_Pointer... Specification by stack overflow... trust? Maybe...
			unsigned int bytesPerPixel = size_components;
			Permanent_Data_Pointer = (unsigned char*)malloc(bytesPerPixel * size_w * size_h);
			memcpy(Permanent_Data_Pointer, data_ptr, bytesPerPixel * size_w * size_h);
			initTexture(size_w, size_h, size_components, Permanent_Data_Pointer, min_filter, mag_filter, wrap_mode, anisotropic_level);
			isnull = false; //It is not null
		}
		Texture(const Texture& other){
			if(!isnull)
			{
				if (Permanent_Data_Pointer)
					free(Permanent_Data_Pointer);
				glDeleteTextures(1, &m_texture); //Delete the texture upon the asset being destroyed
				m_texture = 0;
			}
			if (other.amINull())
			{
				std::cout << "\n !!ERROR! Attempted to construct Texture taking in Null-constructed texture! BAD IDEA!";
				std::abort();
			}
			Permanent_Data_Pointer = (unsigned char*)malloc(other.getMyComponents() * other.getMyWidth() * other.getMyHeight());
			memcpy(Permanent_Data_Pointer, other.getDataPointer(), other.getMyComponents() * other.getMyWidth() * other.getMyHeight());
			initTexture(
			other.getMyWidth(), 
			other.getMyHeight(), 
			other.getMyComponents(), 
			Permanent_Data_Pointer, 
			other.getminfilt(), 
			other.getmaxfilt(), 
			other.getwrapmode(), 
			other.getmyAnisotropicLevel());
			isnull = false; //It is not null
		}
		void Bind(unsigned int unit);
		void BindGeneric();
		void reInitFromDataPointer(bool respecify_wrap_and_filt, bool isSameSize = false){
			if(!isnull)
			{
				//glActiveTexture(GL_TEXTURE0);
				// glDeleteTextures(1, &m_texture); //Delete the texture
				// glGenTextures(1, &m_texture); //Make a new one...
				glBindTexture(GL_TEXTURE_2D, m_texture); //Bind it for editting
				//Set up the texture's properties
				if (respecify_wrap_and_filt)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapmode);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapmode);

					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilt);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxfilt);
				}
				if(isSameSize && !respecify_wrap_and_filt)
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, myWidth, myHeight, GL_RGBA, GL_UNSIGNED_BYTE, Permanent_Data_Pointer);
				else
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myWidth, myHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Permanent_Data_Pointer);
				if (respecify_wrap_and_filt)
				{
					if (
					minfilt == GL_NEAREST_MIPMAP_NEAREST ||
					minfilt == GL_NEAREST_MIPMAP_LINEAR ||
					minfilt == GL_LINEAR_MIPMAP_NEAREST ||
					minfilt == GL_LINEAR_MIPMAP_LINEAR||
					maxfilt == GL_NEAREST_MIPMAP_NEAREST ||
					maxfilt == GL_NEAREST_MIPMAP_LINEAR ||
					maxfilt == GL_LINEAR_MIPMAP_NEAREST ||
					maxfilt == GL_LINEAR_MIPMAP_LINEAR){ //if it in any way has mimapping

						glGenerateMipmap(GL_TEXTURE_2D); //Make those mipmaps son
						GLfloat maxAnisoTropy;
						GLfloat TempAnisoTropics = (GLfloat)myAnisotropicLevel;
						glGetFloatv(HACK_GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisoTropy);
						if (maxAnisoTropy > 0 && TempAnisoTropics < maxAnisoTropy && TempAnisoTropics > 0) //If we can do anisotropy at the specified level AND the level makes sense
						{
							glTexParameterf(GL_TEXTURE_2D, HACK_GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisoTropy);
							//std::cout<<"\nANISOTROPY SUPPORTED!";
						}
					}
					else
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
					}
				}
			} else {
				std::cout << "\nISNULL!!!";
			}
		} //eof reInitFromDataPointer
		static void SetActiveUnit(unsigned int unit);
		virtual ~Texture();
		GLuint getHandle() {return m_texture;} //This was added so that shader.update could be removed
		bool amITransparent(){return transparency_enabled;}
		bool amINull() const {return isnull;}
		static unsigned char* stbi_load_passthrough(char* filename, int* width, int* height, int* numComponents, int something);
		std::string MyName;
		int getMyWidth() const {return myWidth;}
		int getMyHeight() const {return myHeight;}
		int getMyComponents() const {return myComponents;}
		unsigned char* getDataPointer() const {return Permanent_Data_Pointer;}
		unsigned char* getDataPointerNotConst() {return Permanent_Data_Pointer;}
		GLenum getminfilt() const {return minfilt;}
		GLenum getmaxfilt() const {return maxfilt;}
		GLenum getwrapmode() const {return wrapmode;}
		GLenum getmyAnisotropicLevel() const {return myAnisotropicLevel;}
	protected:
	private:
		void initTexture(int size_w, int size_h, int size_components, unsigned char* data_ptr, GLenum min_filter, GLenum mag_filter, GLenum wrap_mode, float anisotropic_level);
		
		//void operator=(const Texture& texture) {}
		bool isnull = true; //By default, it is null.
		int myWidth = 0;
		int myHeight = 0;
		int myComponents = 0;
		int myAnisotropicLevel = 0;
		unsigned char* Permanent_Data_Pointer = nullptr;
		bool transparency_enabled = false;
		GLenum minfilt = GL_LINEAR;
		GLenum maxfilt = GL_LINEAR;
		GLenum wrapmode = GL_REPEAT;
		GLuint m_texture = 0; //The handle of this texture
};





//Cubemap.h

class CubeMap{
	public:
		CubeMap(std::string& FIRST, std::string& SECOND, std::string& THIRD, std::string& FOURTH, std::string& FIFTH, std::string& SIXTH);
		CubeMap(){
			if (!isnull)
			{
				//Delete the texture, it's the only thing we have to think about.
				glDeleteTextures(1, &m_texture);
			}
			isnull = true;
		}
		void Bind(unsigned int unit) const{
			 assert(unit >= 0 && unit <= 31);
			 glActiveTexture(GL_TEXTURE0 + unit);
			 glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
		}
		void BindGeneric() const{
			 glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
		}
		static void SetActiveUnit(unsigned int unit){
			assert(unit >= 0 && unit <= 31);
			glActiveTexture(GL_TEXTURE0 + unit);
		}
		~CubeMap(){
			if (!isnull)
			{
				//Delete the texture, it's the only thing we have to think about.
				glDeleteTextures(1, &m_texture);
			}
		};
		GLuint getHandle() const {return m_texture;} //This was added so that shader.update could be removed
		bool amITransparent() const {return transparency_enabled;}
		std::string MyName;
	protected:
	private:
		
		
		
		void operator=(const CubeMap& other) {}
		CubeMap(const CubeMap& other){
			std::abort();
			// NO FUCKING AROUND ALRIGHT?!?!
		}
		//Private Member Vars
		GLuint m_texture = 0;
		GLenum minfilt = GL_LINEAR;
		GLenum magfilt = GL_LINEAR;
		//There is no wrap mode.
		//Utils
		bool isnull = true; //By default, it is null.
		bool transparency_enabled = false;
};
}; //Eof Namespace
#endif
