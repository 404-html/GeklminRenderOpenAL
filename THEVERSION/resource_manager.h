#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include "mesh.h"
#include "texture.h"
#include "SafeTexture.h"
#include "obj_loader.h"
//~ #include "GekAL.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <map>

//THIS FILE ONLY INCLUDED FOR THIS DEMO, YOU CAN REMOVE IT IN YOUR OWN PROGRAMS AND IN FACT THIS METHOD OF LOADING FILES IS ONLY PARTIALLY DEVELOPED SO IT IS RECOMMENDED YOU MANAGE RESOURCES ON YOUR OWN.


//Manages anything and everything that is on the GPU or loaded from disk so that it never has to be loaded from disk again
//It should be --more than simple-- for anybody, even someone who has never coded in C++ before, to extend this class to all assets and things which someone may want to load.
//You need to create an instance of it.
namespace GeklminRender {
class Resource_Manager
{
    public:
        Resource_Manager(){
			Models_from_file.reserve(1000); //Make caching files a bit faster
		}
        virtual ~Resource_Manager();
		//REMEMBER: This is the resource/asset manager. If you want a custom, dynamic mesh for your scene, then you are not going to use this class.
		Mesh* loadMesh(const std::string& fileName, bool instanced, bool is_static){
			std::string str = fileName;
			std::transform(str.begin(), str.end(),str.begin(), ::toupper);
			//it is guaranteed that we are loading an asset
			for (size_t i = 0; i<loadedMeshes.size(); i++)
			{
				if(loadedMeshes[i]->MyName == str && loadedMeshes[i]->is_instanced == instanced && loadedMeshes[i]->is_static == is_static){
					//std::cout << "\n Match found.";
					return loadedMeshes[i];
				}
			}
			//Search to see if we've cached the file alraedy
			for (size_t i = 0; i < Models_from_file.size(); i++)
			{
				if (Models_from_file[i].myFileName == str) //If this is the correct file..
				{
					loadedMeshes.push_back( //Make a new mesh
					new Mesh(
						Models_from_file[i],
						instanced, 
						is_static, 
						true) //Always an asset.
					);
					//and give it to them.
					return loadedMeshes[loadedMeshes.size()-1];
				}
			}
			//If we couldn't find the mesh already loaded and we couldn't find the mesh cached, then we have to load it. Load it, cache it, and instance the mesh.
			 std::cout <<"\nWe had to load " << str;
			
			//I get a weird error upon program ending if I don't dynamically allocate. Sorry!
			OBJModel* henry = new OBJModel(str);
			Models_from_file.push_back(henry->ToIndexedModel());
			loadedMeshes.push_back(
				new Mesh(
					Models_from_file[Models_from_file.size()-1],
					instanced, 
					is_static, 
					true)
			);
			delete henry;
			// std::cout << "\nHis name is: " << loadedMeshes[loadedMeshes.size()-1]->MyName;
			return loadedMeshes[loadedMeshes.size()-1];
		}
		
		
		// I really don't think anyone will want to load a texture without it being static, and even if they did, it's trivial to do so with a simple shader.
		//NOTE TO SELF: Make this return a literal. Pointers are less efficient. We'll do it after we make Mesh the primary draw-er
		SafeTexture loadTexture(const std::string& fileName, bool enableTransparency){ //The object is responsible for deleting what it gets
			std::string str = fileName;
			std::transform(str.begin(), str.end(),str.begin(), ::toupper);
			for (size_t i = 0; i<loadedTextures.size(); i++)
			{
				if(loadedTextures[i]->MyName == str && loadedTextures[i]->amITransparent() == enableTransparency){
					SafeTexture temp = SafeTexture(loadedTextures[i]);
					return temp;
				}
			}
			std::cout << "\n we had to load texture: " << str << " from file";
			loadedTextures.push_back(new Texture(str,enableTransparency));
			SafeTexture temp = SafeTexture(loadedTextures[loadedTextures.size()-1]); //No longer a memory leak thanks to doctor memory!
			return temp;
		}
		
		ALuint loadSound(const std::string& fileName);

    protected:
	
    private:
	std::vector<Mesh*> loadedMeshes; //SUSPECT FOR MEMORY LEAK!
	std::map<std::string,ALuint> loadedSoundBuffers;
	std::vector<IndexedModel> Models_from_file; //SUSPECT FOR MEMORY LEAK!
	std::vector<Texture*> loadedTextures; //SUSPECT FOR MEMORY LEAK!
};
};
#endif // Resource_Manager_H
