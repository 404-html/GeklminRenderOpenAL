#ifndef MESH_H
#define MESH_H


#include <GL3/gl3w.h>
#include <GL3/gl3.h>
#define GLM_FORCE_RADIANS
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
#include "obj_loader.h"
#include "transform.h"
#include "SafeTexture.h"
#include <GLFW/glfw3.h>
namespace GeklminRender {


struct Phong_Material
{
    public:
        Phong_Material(float amb, float diff, float specr, float specd, float emiss){
			ambient = amb;
			specreflectivity = specr;
			specdamp = specd;
			emissivity = emiss;
			diffusivity = diff;
		}
		Phong_Material(){} //Default Constructor
	float diffusivity = 1.0;
	float ambient = 1;
	float specreflectivity = 0.4;
	float specdamp = 30;
	float emissivity = 0;
};

//Getting rid of RenderableMesh one step at a time!
struct MeshInstance {
	public:
		MeshInstance(int _texind, Transform _transform){
			tex = _texind;
			myTransform = _transform;
		}
		MeshInstance(){} //Default constructor
		unsigned int tex = 0;
		unsigned int cubeMap = 0;
		float EnableCubemapReflections = 0; //Chrome reflections
		float EnableCubemapDiffusion = 0; //A lightmap (Thanks, Brian Hook Speech)
		
		Transform myTransform = Transform(); //We have a default constructor
		void* userPointer = nullptr; //points to the user's small and effeminate pickle jar
		Phong_Material myPhong = Phong_Material();
		//Transitional is no longer needed because we now use a cache in Mesh.
			//glm::mat4 Transitional = glm::mat4(); //We need this for the renderer in NON-INSTANCED mode.
		int mymeshmask = -1; //Always Render is -1, if not -1, meshmask is valid IF it is divisible by the renderer's mask with no remainder, so modulo comes out as 0
		bool shouldRender = true;
};


class Mesh
{
	public:
		Mesh(std::string fileName, bool instanced, bool is_static, bool assetjaodernein, bool recalculatenormals = false);
		Mesh(){
			if(!isnull)
			{
				glDeleteBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
				if (is_instanced)
					glDeleteBuffers(1, &InstanceModelMatrixVBO);
				glDeleteVertexArrays(1, &m_vertexArrayObject);
				if (MatrixDataCacheMalloced.size() > 0)
					MatrixDataCacheMalloced.clear();
				//TODO: Push this update to the main SceneAPI (In 147)
				InstanceModelMatrixVBO = 0;
				m_vertexArrayObject = 0;
			}
			isnull = true;
		}
		Mesh(const Mesh& Other)
		{
			if (isnull)
				InitMesh(Other.ModelData, Other.is_instanced, Other.is_static);
			else
				reShapeMesh(Other.ModelData);
		}
		Mesh(const IndexedModel& usethisone, bool instancing, bool shallwemakeitstatic, bool assetjaodernein){
			if(isnull)
				InitMesh(usethisone, instancing, shallwemakeitstatic);
			else
				reShapeMesh(usethisone);
			is_asset = assetjaodernein;
		}
		
		void optimizeCacheMemoryUsage(bool ShrinkToFit = true){
			MatrixDataCacheMalloced.clear();
			auto iter = std::find(Instances.begin(), Instances.end(), nullptr);
			while (iter != Instances.end())
			{
				Instances.erase(iter);
				iter = std::find(Instances.begin(), Instances.end(), nullptr);
			}
			if(ShrinkToFit)
				Instances.shrink_to_fit();
		}

		void setFlags(unsigned int input){
			renderflags = input;
		}
		unsigned int getFlags(){
			return renderflags;
		}

		void RegisterInstance(MeshInstance* Doggo){ //This has to be FAST, No checking.
			//Instances.push_back(Doggo);
			
			auto iter = std::find(Instances.begin(), Instances.end(), nullptr);
			if (iter != Instances.end())
			{
				size_t whichone = std::distance(Instances.begin(), iter);
				Instances[whichone] = Doggo;
				//Instances.erase(iter);
				//return true;
			} else {
				Instances.push_back(Doggo);
			}
		}
		bool deRegisterInstance(MeshInstance* Doggo){
			// if (Instances.size() > 0)
				// for (int i = 0; i < Instances.size(); i++)
					// if (Doggo == Instances[i]) // This is not working for some reason
					// {
						// std::cout<<"\n!!!Deregistering... Pre:" <<Instances.size();
						// Instances.erase(Instances.begin() + i);
						// std::cout<<"\nPost:" <<Instances.size();
						// return true;
					// }
			
			auto iter = std::find(Instances.begin(), Instances.end(), Doggo);
			if (iter != Instances.end())
			{
				size_t whichone = std::distance(Instances.begin(), iter);
				Instances[whichone] = nullptr;
				//Instances.erase(iter);
				return true;
			}
			return false;
		}

		void DrawGeneric(){
			glBindVertexArray(m_vertexArrayObject);
			glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		//RenderableMesh is now nothing more than a bad dream.
		void DrawInstancesPhong(GLuint Model, GLuint rflags, GLuint specr, GLuint specd, GLuint diff, GLuint emiss, GLuint enableCubemaps, GLuint enableCubemaps_diffuse, GLuint EnableTransparency, GLuint EnableInstancing, bool Transparency, bool toRenderTarget, int meshmask, bool usePhong = true, bool donotusemaps = false, bool force_non_instanced = false){
			if (Instances.size() <= 0 || mesh_meshmask == 0 || (meshmask != 0 && mesh_meshmask%meshmask != 0) )
				return;
			
			
			
			//std::map<unsigned int, std::vector<MeshInstance*>> Batches_texture;
			if ((!is_instanced || force_non_instanced) && !donotusemaps){
				glUniform1f(EnableInstancing, 0);
				//GENERATE THE BATCHES
				std::map<unsigned int, std::map<unsigned int, std::vector<MeshInstance*>>> Batches_cubemap_texture;
				
				
				for (size_t i = 0; i < Instances.size(); i++) //for each thing
				{
					if (!Instances[i] || (Instances[i] && !Instances[i]->shouldRender) || (meshmask != 0 && Instances[i]->mymeshmask%meshmask != 0))
						continue;
					if (Instances[i] && MyTextures.size() > Instances[i]->tex && MyTextures[Instances[i]->tex].amITransparent() == Transparency)
					{
						if (Instances[i]->cubeMap < myCubeMaps.size())
							Batches_cubemap_texture[Instances[i]->cubeMap][Instances[i]->tex].push_back(Instances[i]);
						else
							Batches_cubemap_texture[0][Instances[i]->tex].push_back(Instances[i]);
					} else if (Instances[i] && MyTextures.size() > Instances[i]->tex && MyTextures[Instances[i]->tex].amITransparent() != Transparency) {
						continue;
					} else if (Instances[i] && MyTextures.size() > 0 && MyTextures[0].amITransparent() == Transparency){
						if (Instances[i]->cubeMap < myCubeMaps.size())
							Batches_cubemap_texture[Instances[i]->cubeMap][0].push_back(Instances[i]);
						else
							Batches_cubemap_texture[0][0].push_back(Instances[i]);
					} else if (Instances[i] && MyTextures.size() > 0 && MyTextures[0].amITransparent() != Transparency) {
						continue;
					} else if (Instances[i] && MyTextures.size() == 0){
						if (Instances[i]->cubeMap < myCubeMaps.size())
							Batches_cubemap_texture[Instances[i]->cubeMap][0].push_back(Instances[i]);
						else
							Batches_cubemap_texture[0][0].push_back(Instances[i]);
					}
				}
				
				glUniform1ui(rflags, renderflags);
				glBindVertexArray(m_vertexArrayObject);
					//Draw the batches, but first, we have to talk about that MatrixDataCache from last frame...
				// if (shouldremakeinstancedmodelvector){
					// for (size_t i = 0; i < MatrixDataCache.size(); i++)
					// {
						// if(MatrixDataCache[i] != nullptr)
						// {delete MatrixDataCache[i];}
					// }
						
					// MatrixDataCache.clear();
								
				// }
				//MatrixDataCacheMalloced.reserve(16 * Instances.size());
				size_t howmanyhavewedone = 0;
					for (unsigned int j = 0; j < Batches_cubemap_texture.size(); j++) //for each cubemap
					{
						if (myCubeMaps.size() > j)
							myCubeMaps[j]->Bind(1);
						for (unsigned int i = 0; i < Batches_cubemap_texture[j].size(); i++) //for each texture
						{
							if(MyTextures.size() > i)
								MyTextures[i].Bind(0);
							for (unsigned int k = 0; k < Batches_cubemap_texture[j][i].size(); k++)
							{
								
								//memcpy(&MatrixDataCacheMalloced[howmanyhavewedone * 16], &(Batches_cubemap_texture[j][i][k]->myTransform.GetModel()[0][0]), sizeof(GLfloat) * 16);
								//glUniformMatrix4fv(Model, 1, GL_FALSE, &MatrixDataCacheMalloced[howmanyhavewedone * 16]);
								glUniformMatrix4fv(Model, 1, GL_FALSE, &(Batches_cubemap_texture[j][i][k]->myTransform.GetModel()[0][0]));
								//howmanyhavewedone++;
								//Phong Properties
								//Of note: this barely affects performance, maybe restricting the number of low poly meshes by a thousand or so
								if (usePhong)
								{
									glUniform1f(specr, Batches_cubemap_texture[j][i][k]->myPhong.specreflectivity); //Specr
									glUniform1f(specd, Batches_cubemap_texture[j][i][k]->myPhong.specdamp); //specular dampening
									glUniform1f(diff, Batches_cubemap_texture[j][i][k]->myPhong.diffusivity);//Diffusivity of the material
									glUniform1f(emiss, Batches_cubemap_texture[j][i][k]->myPhong.emissivity);//Emissivity of the material
									glUniform1f(enableCubemaps, Batches_cubemap_texture[j][i][k]->EnableCubemapReflections);//show cubemap in reflections?
									glUniform1f(enableCubemaps_diffuse, Batches_cubemap_texture[j][i][k]->EnableCubemapDiffusion);
								}
								//glUniform1f(m_instanced_float_loc, 0); //We are NOT instanced, use the modelmatrix VBO.
								if (Transparency)
									glUniform1f(EnableTransparency, 1.0f);
								else
									glUniform1f(EnableTransparency, 0.0f);
								
								//Drawing
								glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
							}
						}
					}
				glBindVertexArray(0);
			//NOT INSTANCED, NO MAPS
			} else if ((!is_instanced || force_non_instanced) && donotusemaps){
				glUniform1f(EnableInstancing, 0);
				//GENERATE THE BATCHES
				
				glBindVertexArray(m_vertexArrayObject);
				glUniform1ui(rflags, renderflags);
				for (size_t i = 0; i < Instances.size(); i++) //for each thing
				{
					if (!Instances[i] || (Instances[i] && !Instances[i]->shouldRender) || (meshmask != 0 && Instances[i]->mymeshmask%meshmask != 0))
						continue;
					if (Instances[i]->cubeMap < myCubeMaps.size() && Instances[i]->cubeMap >= 0 && myCubeMaps[Instances[i]->cubeMap])
						myCubeMaps[Instances[i]->cubeMap]->Bind(1);
					
					if (Instances[i] && MyTextures.size() > Instances[i]->tex && MyTextures[Instances[i]->tex].amITransparent() != Transparency)
						continue;
					else if (Instances[i] && MyTextures.size() > Instances[i]->tex && MyTextures[Instances[i]->tex].amITransparent() == Transparency)
						MyTextures[Instances[i]->tex].Bind(0);
						
					
					glUniformMatrix4fv(Model, 1, GL_FALSE, &(Instances[i]->myTransform.GetModel()[0][0]));
					//howmanyhavewedone++;
					//Phong Properties
					//Of note: this barely affects performance, maybe restricting the number of low poly meshes by a thousand or so
					if (usePhong)
					{
						glUniform1f(specr, Instances[i]->myPhong.specreflectivity); //Specr
						glUniform1f(specd, Instances[i]->myPhong.specdamp); //specular dampening
						glUniform1f(diff, Instances[i]->myPhong.diffusivity);//Diffusivity of the material
						glUniform1f(emiss, Instances[i]->myPhong.emissivity);//Emissivity of the material
						glUniform1f(enableCubemaps, Instances[i]->EnableCubemapReflections);//show cubemap in reflections?
						glUniform1f(enableCubemaps_diffuse, Instances[i]->EnableCubemapDiffusion);
					}
					//glUniform1f(m_instanced_float_loc, 0); //We are NOT instanced, use the modelmatrix VBO.
					if (Transparency)
						glUniform1f(EnableTransparency, 1.0f);
					else
						glUniform1f(EnableTransparency, 0.0f);
					
					//Drawing
					glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
					//~ if (Instances[i] && MyTextures.size() > Instances[i]->tex && MyTextures[Instances[i]->tex].amITransparent() == Transparency)
					//~ {
						//~ if (Instances[i]->cubeMap < myCubeMaps.size())
							//~ Batches_cubemap_texture[Instances[i]->cubeMap][Instances[i]->tex].push_back(Instances[i]);
						//~ else
							//~ Batches_cubemap_texture[0][Instances[i]->tex].push_back(Instances[i]);
					//~ } else if (Instances[i] && MyTextures.size() > Instances[i]->tex && MyTextures[Instances[i]->tex].amITransparent() != Transparency) {
						//~ continue;
					//~ } else if (Instances[i] && MyTextures.size() > 0 && MyTextures[0].amITransparent() == Transparency){
						//~ if (Instances[i]->cubeMap < myCubeMaps.size())
							//~ Batches_cubemap_texture[Instances[i]->cubeMap][0].push_back(Instances[i]);
						//~ else
							//~ Batches_cubemap_texture[0][0].push_back(Instances[i]);
					//~ } else if (Instances[i] && MyTextures.size() > 0 && MyTextures[0].amITransparent() != Transparency) {
						//~ continue;
					//~ } else if (Instances[i] && MyTextures.size() == 0){
						//~ if (Instances[i]->cubeMap < myCubeMaps.size())
							//~ Batches_cubemap_texture[Instances[i]->cubeMap][0].push_back(Instances[i]);
						//~ else
							//~ Batches_cubemap_texture[0][0].push_back(Instances[i]);
					//~ }
				}
				
				glUniform1ui(rflags, renderflags);
				glBindVertexArray(0);
	//INSTANCING ELSE
			}else { //Is instanced	
				//Perform testing to ensure that there is a texture to render with, and that its transparency flag matches the transparency state.
				if ((MyTextures.size() > 0 && MyTextures[0].amITransparent() == Transparency) || (MyTextures.size() == 0))
				{
					
					//glGetError(); //Clear out errors. Error checking demands it!
					//std::cout << "\n ATTEMPTING INSTANCED RENDERING!";
					
					//Almost forgot this part!
					glEnableVertexAttribArray(4);
					glEnableVertexAttribArray(5);
					glEnableVertexAttribArray(6);
					glEnableVertexAttribArray(7);
						
					glBindVertexArray(m_vertexArrayObject);
					
					
					//Set Instancing Variable
					glUniform1f(EnableInstancing, 1);
					
					
					// if (true){
							
						MatrixDataCacheMalloced.reserve(Instances.size() * 16);
						
					// }
						m_instanceDrawCount = 0; //reset instance count
						//MatrixDataCache.clear(); //NOTE: If we do shadows, we have to add a list of matrixdatacaches that each get reset at the beginning of each frame. MatrixDataCache MIGHT be being read asyncronously.
					//Draw instanced
						//First, pack the MatrixDataCache
						//int whichfloat = 0;
						// if (MatrixDataCacheMalloced != nullptr)
							// delete MatrixDataCacheMalloced;
						//MatrixDataCacheMalloced.push_back((GLfloat*)malloc(sizeof(GLfloat) * 16 * Instances.size()));
						for (size_t i = 0; i < Instances.size(); i++) //for each thing
						{
							//Complicated check to ensure that no null ptrs have their methods called
							if (!Instances[i] || (Instances[i] && !Instances[i]->shouldRender))
								continue;
							//get the value pointer
							//glm::mat4 biggus_dickus = Instances[i]->myTransform.GetModel();
							//Push on the floats for the Matrix.
							// for	(int j = 0; j < 16; j++)
							// {
								//column major? I don't know! We'll find out if this works.
								//MatrixDataCache.push_back(biggus_dickus[j/4][j%4]);
								// if (i == 0)
								// {
									// std::cout << "\n ROW: "<<j/4;
									// std::cout << "\n COL: "<<j%4;
								// }
								// MatrixDataCacheMalloced[m_instanceDrawCount * 16 + j] = (Instances[i]->myTransform.GetModel()[j/4][j%4]);
							// }
							memcpy(&MatrixDataCacheMalloced[m_instanceDrawCount * 16], &(Instances[i]->myTransform.GetModel()[0][0]), sizeof(GLfloat) * 16);
							//add 1 to the number of instances
							m_instanceDrawCount+=1;
							
						}
						// std::cout <<"\n DRAW COUNT IS: " << m_instanceDrawCount;
						// std::cout <<"\n MATRIXDATACACHE IS OF SIZE: " << MatrixDataCache.size();
						
						
						
						
						
						//Second, send to GPU (?DONE?)
							//Setup to send in stuff
							glBindBuffer(GL_ARRAY_BUFFER, InstanceModelMatrixVBO);//Work with the Instancing Buffer
							
							//	                     #  #floats    type     normalize   Skip                       offset
							// glVertexAttribPointer(4, 4, 		GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT), 		0);
							// glVertexAttribPointer(5, 4,		GL_FLOAT, GL_FALSE, 12 * (int)sizeof(GL_FLOAT), (void*)(4 * sizeof(GL_FLOAT)));
							// glVertexAttribPointer(6, 4, 		GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT), 		(void*)(8 * sizeof(GL_FLOAT)));
							// glVertexAttribPointer(7, 4, 		GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT), 		(void*)(12 * sizeof(GL_FLOAT)));
							
							// glVertexAttribDivisor(4,1); //col A
							// glVertexAttribDivisor(5,1); //col B
							// glVertexAttribDivisor(6,1); //col C
							// glVertexAttribDivisor(7,1); //col D
							//Send it in!
							//glGetError();
							//glBufferData(GL_ARRAY_BUFFER, MatrixDataCache.size() * sizeof(GLfloat), &MatrixDataCache[0], GL_STATIC_DRAW);
							glBufferData(GL_ARRAY_BUFFER, m_instanceDrawCount * 16 * sizeof(GLfloat), &MatrixDataCacheMalloced[0], GL_STATIC_DRAW); //This does not rely on the Vsync and does not require storing
							//send in the uniforms
								//Needs to be copied from the non-instanced part.
							//GLenum communism;
							
							 shouldremakeinstancedmodelvector = false;
					//}//Eof shouldremakeinstancedmodelvector in the case where we limit the user to changing the positions of instances only before rendering anything
						
						//Third, Set uniform variables for per-instance stuff (E.g. material)
						if (usePhong)
						{
							glUniform1f(specr, InstancedMaterial.specreflectivity); //Specr
							glUniform1f(specd, InstancedMaterial.specdamp); //specular dampening
							glUniform1f(diff, InstancedMaterial.diffusivity);//Diffusivity of the material
							glUniform1f(emiss, InstancedMaterial.emissivity);//Emissivity of the material
							glUniform1f(enableCubemaps, instanced_enable_cubemap_reflections ? 1.0f : 0.0f);//show cubemap in reflections?
							glUniform1f(enableCubemaps_diffuse, instanced_enable_cubemap_diffusion ? 1.0f : 0.0f);
						}
						//glUniform1f(m_instanced_float_loc, 0); //We are NOT instanced, use the modelmatrix VBO.
						if (Transparency)
							glUniform1f(EnableTransparency, 1.0f);
						else
							glUniform1f(EnableTransparency, 0.0f);
						
						//Fourth, Bind texture
						if(MyTextures.size() > 0)
							MyTextures[0].Bind(0);
						//FOURTH, BIND TEXTURES
						if(myCubeMaps.size() > 0)
							myCubeMaps[0]->Bind(1);
						glUniform1ui(rflags, renderflags);
						// if (MyName == "GeklminRender_Font/Capital A.obj")
						// {
							// std::cout << "\nHAS INSTANCES!!! IS BEING DRAWN RIGHT BELOW (SUPPOSEDLY?)";
							// std::cout << "\nNumber of Indices: " << ModelData.indices.size();
						// }
						//Fifth, Draw!
						// /*
						glDrawElementsInstanced( GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceDrawCount);
						// */
						//Disable vertex attribute arrays
						glBindVertexArray(0);
						glDisableVertexAttribArray(4);
						glDisableVertexAttribArray(5);
						glDisableVertexAttribArray(6);
						glDisableVertexAttribArray(7);
						//std::cout << "\nDREW INSTANCED!!!" << std::endl;
					
				} //eof (if mytextures dot size...)
				if(!toRenderTarget)
				{
					shouldremakeinstancedmodelvector = true;
					canbereshaped = true;
				}
			} //eof is instanced
		} //Eof DrawInstancesPhong

		void pushTexture(SafeTexture _tex){ //CHECKED NO PROBLEMS WITH ITER
			if (MyTextures.size() > 0)
				for (auto iter = MyTextures.begin(); iter < MyTextures.end(); ++iter)
				{
					if (*iter == _tex)
						return;
				}
			MyTextures.push_back(_tex);
		}
		
		bool removeTexture(GLuint _tex){
			bool returnval = false;
			if (MyTextures.size() < 1) //Just in case...
				return false;
			for (auto i = MyTextures.begin(); i < MyTextures.end(); ++i){ 
				//if (i >= MyTextures.begin() && i < MyTextures.end())
					if ((*i).getHandle() == _tex)
					{
						MyTextures.erase(i);
						returnval = true;
					}
			}
			return returnval;
		}
		
		void pushCubeMap(CubeMap* _cube)
		{
			if (myCubeMaps.size() > 0)
				for (auto iter = myCubeMaps.begin(); iter < myCubeMaps.end(); ++iter)
				{
					if (*iter == _cube)
						return;
				}
			myCubeMaps.push_back(_cube);
		}
		
		bool removeCubeMap(GLuint _cube){
			bool returnval = false;
			if (myCubeMaps.size() < 1) //Just in case...
				return false;
			for (auto i = myCubeMaps.begin(); i < myCubeMaps.end(); ++i){ 
				//if (i >= myCubeMaps.begin() && i < myCubeMaps.end())
					if ((*i)->getHandle() == _cube)
					{
						myCubeMaps.erase(i);
						returnval = true;
					}
			}
			return returnval;
		}
		
		bool removeCubeMapByPointer(CubeMap* TheOneTheOnly)
		{
			auto iter = std::find(myCubeMaps.begin(), myCubeMaps.end(), TheOneTheOnly);
			if (iter != myCubeMaps.end())
			{
				myCubeMaps.erase(iter);
				return true;
			}
			return false;
		}
		
		
		//for manually testing the vector to see which element is what texture. You would essentially want to use this to debug how many safetextures you have, and pick suitable elements.
		std::vector<SafeTexture>* GetTextureVectorPtr(){
			return &MyTextures;
		}
		std::vector<CubeMap*>* GetCubeMapVectorPtr(){
			return &myCubeMaps;
		}
		std::vector<MeshInstance*>* GetInstanceVectorPtr(){
			return &Instances;
		}
		int NumTextures(){return MyTextures.size();}
		int NumInstances(){return Instances.size();}

		GLuint getVAOHandle(){return m_vertexArrayObject;}
		unsigned int getDrawCount(){return m_drawCount;}

		virtual ~Mesh();
		std::string MyName = ""; //Stores the filename. There's a small problem though... Capitalization... Yeah. It's big.

		bool is_instanced = false; // default is false.
		bool is_static = true; //default is true.
		bool is_asset = true; //default is true.
		
		Phong_Material InstancedMaterial; //Only used if this mesh has instancing enabled
		bool instanced_enable_cubemap_reflections = false; //Only used if this mesh has instancing enabled.
		bool instanced_enable_cubemap_diffusion = false;
		IndexedModel getShape(){
			return ModelData;
		}
		void reShapeMesh(const IndexedModel& model);
		int mesh_meshmask = -1; //See explanation in the Instance class
	protected:
	private:
		void InitMesh(const IndexedModel& model, bool instancing, bool shallwemakeitstatic);
		
		enum {
			POSITION_VB,
			TEXCOORD_VB,
			INDEX_VB,
			NORMAL_VB,
			COLOR_VB,
			//INSTANCELOC_VB, //Uncomment these before doing instancing.
			//INSTANCECUSTOM_VB,
			NUM_BUFFERS
		};

		GLuint m_vertexArrayObject = 0;
		GLuint m_vertexArrayBuffers[NUM_BUFFERS];
		//used for instancing. Only the first texture and cubemap are used in instancing mode.
		
		GLuint InstanceModelMatrixVBO;
		//Indicate the length of the VBO, in 16 * sizeof(float) and vec4 respectively. Transparency toggle does not need to be set, because all instances will use the same texture and therefore will all either be transparent or not.
		// size_t lengthof_InstanceModelMatrixVBO = 0; //Default 0
		//Measured in number of floats
		
		
		//Used for sending in per-instance info. Note that we will only expand this when we need to, no clear calls should be necessary. Send in as columns!
		//This one is used for Non-Instanced mode to store the Mat4s
		//std::vector<glm::mat4*> MatrixDataCache; //When we used this for instancing, it was GLfloat.
		std::vector<GLfloat> MatrixDataCacheMalloced; //Std vector will manage resizing for us...
		
		bool shouldremakeinstancedmodelvector = true;
		bool canbereshaped = true;
		unsigned int m_instanceDrawCount = 0; //Number of instances. Used for instancing!
		unsigned int m_drawCount = 0;
		unsigned int renderflags = 0; //Flags for this mesh by default.
		bool isnull = true;
		//The other booleans are up above, in public.
		IndexedModel ModelData;
		
		
		
		std::vector<MeshInstance*> Instances; //Vector of meshinstance pointers. To be used when we get rid of that DARN renderablemesh class. If this is a dynamic, non-static mesh then the typical use-case will be to only have one instance.
		std::vector<SafeTexture> MyTextures; //To be used in the initial opaque shader. Maximum 8 for instanced situations.
		std::vector<CubeMap*> myCubeMaps; //To be used in the initial opaque shader. Maximum 8 for instanced situations.
};
}; //Eof Namespace
#endif // Mesh_H
