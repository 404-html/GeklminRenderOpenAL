#ifndef PointLight_H
#define PointLight_H

#include <GL3/gl3w.h>
#include <GL3/gl3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "camera.h"
#include "SafeTexture.h"
//Cpu Representation of point light
namespace GeklminRender {
struct PointLight
{
    public:
		PointLight(){
			pos = glm::vec3(0,0,0);
			myColor = glm::vec3(1,1,1);
		}
        PointLight(glm::vec3 newpos, glm::vec3 newcolor){
			pos = newpos;
			myColor = newcolor;
		}
        ~PointLight(){
			//delet this
		}
		
		void setPos(glm::vec3 pos){this->pos = pos;}
		glm::vec3 getPos() {return pos;}
		
		void bindToUniformLight(GLuint position, GLuint color, GLuint m_range, GLuint m_dropoff) const {
			glUniform3f(position,
				pos.x,
				pos.y,
				pos.z);
			glUniform3f(color,
				myColor.x,
				myColor.y,
				myColor.z
			);
			glUniform1f(m_range, this->range);
			glUniform1f(m_dropoff, this->dropoff);
		}
		
		void setRangeProperties(float newrange, float newdropoff){
			range = newrange;
			dropoff = newdropoff;
		}
	glm::vec3 pos;
	glm::vec3 myColor;
	
	glm::vec4 sphere1 = glm::vec4(0);
	glm::vec4 sphere2 = glm::vec4(0);
	glm::vec4 sphere3 = glm::vec4(0);
	glm::vec4 sphere4 = glm::vec4(0);
	
	glm::vec3 AABBp1 = glm::vec3(0);
	glm::vec3 AABBp2 = glm::vec3(0);
	glm::vec3 AABBp3 = glm::vec3(0);
	glm::vec3 AABBp4 = glm::vec3(0);
	
	bool whitelist = false;
	
	float range = 1000; //range
	float dropoff = 1; //no dropoff
	bool shouldRender = true; //Render this point light.
	std::string name = "\0";
};
//Cpu representation of DirectionalLight
struct DirectionalLight {
	public:
	DirectionalLight(){
		myColor = glm::vec3(0.2,0.2,0.2);
		myDirection = glm::vec3(0,0,1);
	}
	void BindToUniformDirectionalLight(GLuint col, GLuint dir) const {
		glUniform3f(col, 
		myColor.x, 
		myColor.y, 
		myColor.z);
		
		glUniform3f(dir, 
		myDirection.x, 
		myDirection.y, 
		myDirection.z);
	}
	glm::vec3 myColor;
	glm::vec3 myDirection;
	
	glm::vec4 sphere1 = glm::vec4(0);
	glm::vec4 sphere2 = glm::vec4(0);
	glm::vec4 sphere3 = glm::vec4(0);
	glm::vec4 sphere4 = glm::vec4(0);
	
	glm::vec3 AABBp1 = glm::vec3(0);
	glm::vec3 AABBp2 = glm::vec3(0);
	glm::vec3 AABBp3 = glm::vec3(0);
	glm::vec3 AABBp4 = glm::vec3(0);
	
	bool shouldRender = true;
	bool whitelist = false;
};

struct CameraLight {
	CameraLight(){
		myColor = glm::vec3(0.2,0.2,0.2);
		myCamera = Camera(
		glm::vec3(0,0,0), //Pos
		70.0, //FOV
		1, //Aspect
		0.1, //zNear 
		1000, //zFar
		glm::vec3(1,0,0),
		glm::vec3(0,1,0)
		);
		Tex2Project = SafeTexture();
	}
	CameraLight(glm::vec3 pos, float fov, float aspect, float zNear, float zFar, glm::vec3 forward, glm::vec3 up, glm::vec3 _color, SafeTexture _tex){
		myCamera = Camera(pos,fov,aspect,zNear,zFar,forward,up);
		myColor = _color;
		Tex2Project = _tex;
	}
	void BindtoUniformCameraLight(GLuint m_matrix, GLuint m_camerapos, GLuint m_color, GLuint m_SolidColorToggle, GLuint m_range, GLuint m_shadows, unsigned int textureunit) {
		MatrixCache = myCamera.GetViewProjection();
		glUniformMatrix4fv(m_matrix, 1, GL_FALSE, &MatrixCache[0][0]);
		//Setup the far and near plane variables
		
		//Setup the position and color
		if (range >= 0)
		{glUniform3f(m_camerapos, myCamera.pos.x, myCamera.pos.y, myCamera.pos.z);}
		else
		{glUniform3f(m_camerapos, myCamera.forward.x, myCamera.forward.y, myCamera.forward.z);}
		glUniform3f(m_color, myColor.x, myColor.y, myColor.z);
		glUniform1f(m_SolidColorToggle, solidColor);
		glUniform1f(m_range, range);
		glUniform1f(m_shadows, isShadowed?1.0f:0.0f);
		
		//Bind texture
		if(solidColor == 0.0f)
			Tex2Project.Bind(textureunit);
	}
	float solidColor = 0.0f; //By default, use a solid color
	glm::vec3 myColor; //the color to use in solid color mode
	float range = 200.0f;
	Camera myCamera; //Contains position and etc.
	glm::mat4 MatrixCache; //Needed to call glUniformMatrix4fv
	bool shouldRender = true;
	bool isShadowed = false;
	SafeTexture Tex2Project; //Overrides myColor in projection mode OR serves for depth in shadow mode
	
};

//Simulates ambient light. Covers a volume.
struct AmbientLight {
	AmbientLight(){
		
	}
	void BindtoUniformAmbientLight(GLuint _pos, GLuint _col, GLuint _range) const {
		glUniform3f(_pos, myPos.x, myPos.y, myPos.z);
		glUniform3f(_col, myColor.x, myColor.y, myColor.z);
		glUniform1f(_range, myRange);
	}
	glm::vec3 myColor;
	glm::vec3 myPos;
	float myRange;
	bool shouldRender = true;
	
	glm::vec4 sphere1 = glm::vec4(0);
	glm::vec4 sphere2 = glm::vec4(0);
	glm::vec4 sphere3 = glm::vec4(0);
	glm::vec4 sphere4 = glm::vec4(0);
	
	glm::vec3 AABBp1 = glm::vec3(0);
	glm::vec3 AABBp2 = glm::vec3(0);
	glm::vec3 AABBp3 = glm::vec3(0);
	glm::vec3 AABBp4 = glm::vec3(0);
	
	bool whitelist = false;
};

// struct Spotlight {
	// SpotLight(){
		
	// }
	// void BindtoUniformSpotLight(GLuint _pos, GLuint _col, GLuint _range, GLuint _innerangle, GLuint _outerangle, GLuint _direction) const {
		// glUniform3f(_pos, myPos.x, myPos.y, myPos.z);
		// glUniform3f(_col, myColor.x, myColor.y, myColor.z);
		// glUniform3f(_direction, myDirection.x, myDirection.y, myDirection.z);
		
		// glUniform1f(_innerangle, innerangle);
		// glUniform1f(_outerangle, outerangle);
		// glUniform1f(_range, myRange);
	// }
	// glm::vec3 myColor;
	// glm::vec3 myPos;
	// glm::vec3 myDirection;
	// float innerangle;
	// float outerangle;
	// float myRange;
	// SafeTexture* Shadows = nullptr;
// };
}; //Eof Namespace
#endif // PointLight_H
