#ifndef GK_GLRENDEROBJECT
#define GK_GLRENDEROBJECT

#include "GL3/gl3w.h"
#include "GL3/gl3.h"
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <cstdlib>
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include <iterator>
#include <map>
#include <unordered_map>
//#include <list> //std::list
#include "transform.h"
#include "SafeTexture.h"
#include "FBO.h"
#include "Shader.h"
#include <GLFW/glfw3.h>

namespace GeklminRender{
class GLRenderObject{
public:
	GLRenderObject();
	virtual ~GLRenderObject();
	virtual void Mainshader_Binds(void* args);
	virtual void PreDraw(int meshmask, void* args);
	virtual void Draw(int meshmask, void* args);
	//For custom rendering with custom things
	std::map<std::string, GLuint> GLuint_Dictionary;
	std::map<std::string, GLenum> GLenum_Dictionary;
	std::map<std::string, GLfloat> GLfloat_Dictionary;
	//For custom rendering with conventional things
	std::vector<Shader*> Shaders;
	std::vector<SafeTexture*> Textures;
	std::vector<FBO*> FBOs;
	Transform myTransform;
};


};
#endif
