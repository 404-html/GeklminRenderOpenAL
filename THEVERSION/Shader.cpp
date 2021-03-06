#include "Shader.h"
#include <iostream>
#include <fstream>
//Written by BennyBox... pretty much entirely

namespace GeklminRender {
static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
    GLint success = 0;
    GLchar error[1024] = { 0 };

    if(isProgram)
        glGetProgramiv(shader, flag, &success);
    else
        glGetShaderiv(shader, flag, &success);

    if(success == GL_FALSE)
    {
        if(isProgram)
            glGetProgramInfoLog(shader, sizeof(error), NULL, error);
        else
            glGetShaderInfoLog(shader, sizeof(error), NULL, error);

        std::cerr << errorMessage << ": '" << error << "'" << std::endl;
    }
}

static GLuint CreateShader(const std::string& text, unsigned int type)
{
    GLuint shader = glCreateShader(type);

    if(shader == 0)
		std::cerr << "Error compiling shader type " << type << std::endl;

    const GLchar* p[1];
	GLint lengths[1];

	p[0] = text.c_str();
    lengths[0] = text.length();

    glShaderSource(shader, 1, p, lengths);
    glCompileShader(shader);

    CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

    return shader;
}



static std::string LoadShader(const std::string& fileName)
{
    std::ifstream file;
    file.open((fileName).c_str());

    std::string output;
    std::string line;

    if(file.is_open())
    {
        while(file.good())// you're good... you're good... you're good... stop!
        {
            getline(file, line);
			output.append(line + "\n");
        }
    }
    else
    {
		std::cerr << "Unable to load: " << fileName << std::endl;
    }

    return output;
}

Shader::Shader(const std::string& fileName)
{
	if (!isnull){
		std::cerr << "\n ERROR! It is INVALID to call the constructor of an already-instantiated OpenGL Object!\n It is the convention of this library for assets and most opengl objects to be non-reinstantiatable." <<
		"\n\n at the current time that includes these classes:" <<
		// "\n Mesh" <<
		// "\n FBO" <<
		"\n CubeMap" <<
		"\n Shader";
		std::abort();
		int* p = nullptr;
		*p = 0;
	}
	isnull = false;
	m_program = glCreateProgram();
	m_shaders[0] = CreateShader(LoadShader(fileName + ".vs"), GL_VERTEX_SHADER);
	m_shaders[1] = CreateShader(LoadShader(fileName + ".fs"), GL_FRAGMENT_SHADER);
	//std::cout<<"\nCreated Shaders!";
	for(unsigned int i = 0; i < NUM_SHADERS; i++) //Doesn't need to be size_t. it will never be big.
		glAttachShader(m_program, m_shaders[i]);

	//THIS NEEDS TO BE MOVED!!!
	//Nevermind. It appears not to be necessary. Don't delete these commetns
		//glBindAttribLocation(m_program, 0, "vPosition");
		//glBindAttribLocation(m_program, 1, "intexcoord");
		//glBindAttribLocation(m_program, 2, "Normal");
	//glBindAttribLocation(m_program, 2, "normal");

	glLinkProgram(m_program);
	CheckShaderError(m_program, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(m_program);
	CheckShaderError(m_program, GL_VALIDATE_STATUS, true, "Invalid shader program");

}







Shader::~Shader()
{
	for(unsigned int i = 0; i < NUM_SHADERS; i++) //Doesn't need to be size_t
    {
        glDetachShader(m_program, m_shaders[i]);
        glDeleteShader(m_shaders[i]);
    }

	glDeleteProgram(m_program);
}

void Shader::Bind()
{
	glUseProgram(m_program);
}

GLint Shader::GetUniformLocation(std::string name) //GLint is a handle
{
	return glGetUniformLocation(m_program, name.c_str());
}

//GLuint Shader::getHandle() // Soon to be removed DISGUSTING!
//{
//	return m_program;
//}




//We will need to be adding more uniform variable types as time goes on, especially arrays of data e.g. arrays of vec4s and arrays of vec3s

//Stuff to do with setting uniforms
void Shader::setUniform1f(std::string name, GLfloat value){
	glUniform1f(GetUniformLocation(name), value);
} //1 float

void Shader::setUniformMatrix4fv(std::string name, GLsizei count, GLboolean transpose, GLfloat* dick){
	glUniformMatrix4fv(GetUniformLocation(name), count, transpose, dick);
} //mat4

void Shader::setUniform1i(std::string name, GLint value){
	glUniform1i(GetUniformLocation(name), value);
} //Integer

void Shader::setUniform1ui(std::string name, GLuint value){
	glUniform1ui(GetUniformLocation(name), value);
} //Unsigned Integer
//Deprecated
/*
void Shader::Update(Transform inTransform, Camera thiscamera)
{
	glm::mat4 model = inTransform.GetModel();
	glm::mat4 camshit = thiscamera.GetViewProjection();
	glUniformMatrix4fv(m_uniforms[TRANSFORM_UNIFORM], 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(m_uniforms[MVP_UNIFORM], 1, GL_FALSE, &camshit[0][0]);
}
*/
}; //Eof Namespace