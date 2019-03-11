#ifndef MESH_H
#define MESH_H


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
		void DrawInstancesPhong(GLuint Model, GLuint rflags, GLuint specr, GLuint specd, GLuint diff, GLuint emiss, GLuint enableCubemaps, GLuint enableCubemaps_diffuse, GLuint EnableTransparency, GLuint EnableInstancing, bool Transparency, bool toRenderTarget, int meshmask, bool usePhong = true, bool donotusemaps = false, bool force_non_instanced = false);


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
		std::vector<GLfloat> MatrixDataCacheMalloced; //Std vector will manage resizing for us... NO LONGER MALLOCED SO IT SHOULDNT BE CALLED THAT!!!
		
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
