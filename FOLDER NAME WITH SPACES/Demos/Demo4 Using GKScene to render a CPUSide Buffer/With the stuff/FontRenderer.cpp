#include "FontRenderer.h"
#include <cstdlib>
namespace GeklminRender{ //Makes things easier
	
	BMPFontRenderer::BMPFontRenderer(std::string filepath, unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor, std::string Shader_location){
		std::cout << "\nEntered constructor of BMPFontRenderer!!!" << std::endl;
		//If it's already been initialized, delete everything
		if(!isNull){
			if(Screen){
				delete Screen; Screen = nullptr;
			}
			if(BMPFont){
				delete BMPFont; BMPFont = nullptr;
			}
			if(Screenquad_Mesh){
				delete Screenquad_Mesh; Screenquad_Mesh = nullptr;
			}
			if(Screenquad_Shader){
				delete Screenquad_Shader; Screenquad_Shader = nullptr;
			}
			BMPFontWidth = 0;
			BMPFontHeight = 0;
			screen_width = 0;
			screen_height = 0;
			my_scaling_factor = 1.0;
			TextBoxes.clear();
			isNull = true;
		}
		//First: Setup the bmp font with the passthrough
		int temp_width;
		int temp_height;
		int temp_components;
		int temp_something = 4; //no idea
		BMPFont = Texture::stbi_load_passthrough((char*)filepath.c_str(), &temp_width, &temp_height, &temp_components, temp_something);
		if(BMPFont != nullptr){
			BMPFontWidth = temp_width;
			BMPFontHeight = temp_height;
			num_components_BMPFont = temp_components;
		} else {
			std::cout << "\nERROR!!! PROBLEM LOADING FILE!!!" << std::endl;
			std::abort();
			std::cout << "\nThe program didn't crash. HOW?!?!" << std::endl;
		}
		//Second: Setup the screen texture
		isNull = false; //we have to do this before resize
		resize(x_screen_width, y_screen_height, scaling_factor);
		
		
		//Third: Prepare to render Screen quads
		if (Screenquad_Mesh == nullptr)
		{
			//Set up the positions
				screenquad_IndexedModel.positions.push_back(glm::vec3(-1,-1,0));
				screenquad_IndexedModel.positions.push_back(glm::vec3(-1,1,0));
				screenquad_IndexedModel.positions.push_back(glm::vec3(1,1,0));
				screenquad_IndexedModel.positions.push_back(glm::vec3(1,-1,0));
			//Set up the index
				//First Triangle
					screenquad_IndexedModel.indices.push_back(0);
					screenquad_IndexedModel.indices.push_back(2);
					screenquad_IndexedModel.indices.push_back(3);
				//Second triangle
					screenquad_IndexedModel.indices.push_back(0);
					screenquad_IndexedModel.indices.push_back(1);
					screenquad_IndexedModel.indices.push_back(2);
			//Put in some bullshit for the texcoords and normals, we don't need them and if this works we'll remove them later
				screenquad_IndexedModel.texCoords.push_back(glm::vec2(0,0));
				screenquad_IndexedModel.texCoords.push_back(glm::vec2(0,0));
				screenquad_IndexedModel.texCoords.push_back(glm::vec2(0,0));
				screenquad_IndexedModel.texCoords.push_back(glm::vec2(0,0));
			//We also need bullshit for the normals
				screenquad_IndexedModel.normals.push_back(glm::vec3(1,-1,0));
				screenquad_IndexedModel.normals.push_back(glm::vec3(2,1,0));
				screenquad_IndexedModel.normals.push_back(glm::vec3(3,1,0));
				screenquad_IndexedModel.normals.push_back(glm::vec3(4,-1,0));

				//std::cout<<"\n We got the indexed model done";
			//Make the mesh
				Screenquad_Mesh = new Mesh(screenquad_IndexedModel, false, true, true);
				//std::cout<<"\n we got the mesh made";
			//Make the camera needed to render the screenquad
				Camera* ScreenquadCamera = nullptr;
				ScreenquadCamera = new Camera();
				ScreenquadCamera->buildOrthogonal(-1, 1, -1, 1, 0, 1);
				ScreenquadCamera->pos = glm::vec3(0,0,0);
				ScreenquadCamera->forward = glm::vec3(0,0,1);
				ScreenquadCamera->up = glm::vec3(0,1,0);
				Screenquad_CameraMatrix = ScreenquadCamera->GetViewProjection();
				delete ScreenquadCamera;
		}
		//fourth: setup the shader
		Screenquad_Shader = new Shader(Shader_location);
		Screenquad_Shader->Bind();
			diffuse_loc = Screenquad_Shader->GetUniformLocation("diffuse");
			cam_loc = Screenquad_Shader->GetUniformLocation("World2Camera");
		isNull = false;
		std::cout << "\nFinished constructor of BMPFontRenderer!!!" << std::endl;
	} //eof constructor that uses a string path
	
	BMPFontRenderer::BMPFontRenderer(unsigned char* _BMPFont, unsigned int _BMPFont_Width, unsigned int _BMPFont_Height, unsigned int _num_components_BMPFont, unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor, std::string Shader_location){
		std::cout << "\nEntered constructor of BMPFontRenderer!!!" << std::endl;
		//If it's already been initialized, delete everything
		if(!isNull){
			if(Screen){
				delete Screen; Screen = nullptr;
			}
			if(BMPFont){
				delete BMPFont; BMPFont = nullptr;
			}
			if(Screenquad_Mesh){
				delete Screenquad_Mesh; Screenquad_Mesh = nullptr;
			}
			if(Screenquad_Shader){
				delete Screenquad_Shader; Screenquad_Shader = nullptr;
			}
			BMPFontWidth = 0;
			BMPFontHeight = 0;
			screen_width = 0;
			screen_height = 0;
			my_scaling_factor = 1.0;
			TextBoxes.clear();
			isNull = true;
		}
		//First: Setup the bmp font with the passthrough
		//~ int temp_width;
		//~ int temp_height;
		//~ int temp_components;
		//~ int temp_something = 4; //no idea
		//~ BMPFont = Texture::stbi_load_passthrough((char*)filepath.c_str(), &temp_width, &temp_height, &temp_components, temp_something);
		BMPFont = _BMPFont;
		if(BMPFont != nullptr){
			BMPFontWidth = _BMPFont_Width;
			BMPFontHeight = _BMPFont_Height;
			num_components_BMPFont = _num_components_BMPFont;
		} else {
			std::cout << "\nERROR!!! PROBLEM LOADING FILE!!!" << std::endl;
			std::abort();
			std::cout << "\nThe program didn't crash. HOW?!?!" << std::endl;
		}
		//Second: Setup the screen texture
		isNull = false; //we have to do this before resize
		resize(x_screen_width, y_screen_height, scaling_factor);
		
		
		//Third: Prepare to render Screen quads
		if (Screenquad_Mesh == nullptr)
		{
			//Set up the positions
				screenquad_IndexedModel.positions.push_back(glm::vec3(-1,-1,0));
				screenquad_IndexedModel.positions.push_back(glm::vec3(-1,1,0));
				screenquad_IndexedModel.positions.push_back(glm::vec3(1,1,0));
				screenquad_IndexedModel.positions.push_back(glm::vec3(1,-1,0));
			//Set up the index
				//First Triangle
					screenquad_IndexedModel.indices.push_back(0);
					screenquad_IndexedModel.indices.push_back(2);
					screenquad_IndexedModel.indices.push_back(3);
				//Second triangle
					screenquad_IndexedModel.indices.push_back(0);
					screenquad_IndexedModel.indices.push_back(1);
					screenquad_IndexedModel.indices.push_back(2);
			//Put in some bullshit for the texcoords and normals, we don't need them and if this works we'll remove them later
				screenquad_IndexedModel.texCoords.push_back(glm::vec2(0,0));
				screenquad_IndexedModel.texCoords.push_back(glm::vec2(0,0));
				screenquad_IndexedModel.texCoords.push_back(glm::vec2(0,0));
				screenquad_IndexedModel.texCoords.push_back(glm::vec2(0,0));
			//We also need bullshit for the normals
				screenquad_IndexedModel.normals.push_back(glm::vec3(1,-1,0));
				screenquad_IndexedModel.normals.push_back(glm::vec3(2,1,0));
				screenquad_IndexedModel.normals.push_back(glm::vec3(3,1,0));
				screenquad_IndexedModel.normals.push_back(glm::vec3(4,-1,0));

				//std::cout<<"\n We got the indexed model done";
			//Make the mesh
				Screenquad_Mesh = new Mesh(screenquad_IndexedModel, false, true, true);
				//std::cout<<"\n we got the mesh made";
			//Make the camera needed to render the screenquad
				Camera* ScreenquadCamera = nullptr;
				ScreenquadCamera = new Camera();
				ScreenquadCamera->buildOrthogonal(-1, 1, -1, 1, 0, 1);
				ScreenquadCamera->pos = glm::vec3(0,0,0);
				ScreenquadCamera->forward = glm::vec3(0,0,1);
				ScreenquadCamera->up = glm::vec3(0,1,0);
				Screenquad_CameraMatrix = ScreenquadCamera->GetViewProjection();
				delete ScreenquadCamera;
		}
		//fourth: setup the shader
		Screenquad_Shader = new Shader(Shader_location);
		Screenquad_Shader->Bind();
			diffuse_loc = Screenquad_Shader->GetUniformLocation("diffuse");
			cam_loc = Screenquad_Shader->GetUniformLocation("World2Camera");
		isNull = false;
		std::cout << "\nFinished constructor of BMPFontRenderer!!!" << std::endl;
	} //eof constructor that uses a pointer
	
	
	//Related to constructing
	void BMPFontRenderer::resize(unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor){
		if (isNull){
			return;
		}
		my_scaling_factor = scaling_factor;
		screen_width = x_screen_width * my_scaling_factor;
		screen_height = y_screen_height * my_scaling_factor;
		//                                                              1              RGBA   width            height
		unsigned char* Temporary_Memory = (unsigned char*)malloc(sizeof(unsigned char) * 4 * screen_width * screen_height);
		//Make it black
		for(int i = 0; i < 4 * screen_width * screen_height; i++){
			//Temporary_Memory[i] = rand()%256; //RANDOM!!!
			Temporary_Memory[i] = 0; //Black
		}
		//Declare the screen
		Screen = new Texture(x_screen_width, y_screen_height, 4, Temporary_Memory, GL_NEAREST, GL_NEAREST, GL_REPEAT, 0.0);
		if(Temporary_Memory)
			free(Temporary_Memory);
	}
	
	
	
	
	
	//the destructor
	BMPFontRenderer::~BMPFontRenderer(){
		//The destructing part of the destructor
		if(!isNull){
			if(Screen){
				delete Screen; Screen = nullptr;
			}
			if(BMPFont){
				delete BMPFont; BMPFont = nullptr;
			}
			if(Screenquad_Mesh){
				delete Screenquad_Mesh; Screenquad_Mesh = nullptr;
			}
			if(Screenquad_Shader){
				delete Screenquad_Shader; Screenquad_Shader = nullptr;
			}
			BMPFontWidth = 0;
			BMPFontHeight = 0;
			screen_width = 0;
			screen_height = 0;
			my_scaling_factor = 1.0;
			TextBoxes.clear();
			isNull = true;
		}
	}
	
	
	//Other functions
	
	void BMPFontRenderer::Draw(bool  useBlending){
		Screenquad_Shader->Bind();
		Screen->Bind(0);
		glUniformMatrix4fv(cam_loc, 1, GL_FALSE, &Screenquad_CameraMatrix[0][0]);
		glUniform1i(diffuse_loc, 0);
		GLuint m_handle = Screenquad_Mesh->getVAOHandle();
		glEnableVertexAttribArray(0);
			glBindVertexArray(m_handle);
				glDisable(GL_DEPTH_TEST);
				if(useBlending){
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				if(useBlending){
					glDisable(GL_BLEND);
				}
				glEnable(GL_DEPTH_TEST);
			glBindVertexArray(0);
		glDisableVertexAttribArray(0);
	}
	void BMPFontRenderer::pushChangestoTexture(){
		Screen->reInitFromDataPointer(false, true);
	}
};
