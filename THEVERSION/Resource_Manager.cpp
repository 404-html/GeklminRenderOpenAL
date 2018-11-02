#include "resource_manager.h"
#include "GekAL.h"
//^^ that file says it all
namespace GeklminRender {
ALuint Resource_Manager::loadSound(const std::string& fileName){
			if (loadedSoundBuffers.count(fileName) > 0)
			{
				return loadedSoundBuffers[fileName];
			}
			else 
			{
				loadedSoundBuffers[fileName] = loadWAVintoALBuffer(fileName.c_str());
				return loadedSoundBuffers[fileName];
			}
		}
	
Resource_Manager::~Resource_Manager()
{
	/*
    for (int i = 0; i< loadedMeshes.size(); i++){
		delete loadedMeshes[i];
	}
	for (int i = 0; i< loadedTextures.size(); i++){
		delete loadedTextures[i];
	}
	*/
	if (loadedMeshes.size() > 0)
		while(loadedMeshes.size() > 0)
		{
			delete loadedMeshes[loadedMeshes.size()-1];
			loadedMeshes.erase(loadedMeshes.begin() + (loadedMeshes.size()-1));
		}
	if (loadedTextures.size() > 0)
		while(loadedTextures.size() > 0)
		{
			delete loadedTextures[loadedTextures.size()-1];
			loadedTextures.erase(loadedTextures.begin() + (loadedTextures.size()-1));
		}
	std::cout <<"\nDELETING SOUNDS...";
	std::map<std::string,ALuint>::iterator it;
	for (it = loadedSoundBuffers.begin(); it != loadedSoundBuffers.end(); it++ )
	{
		alDeleteBuffers(1,&(it->second));
	}
	std::cout <<"\nSOUNDS DELETED!!!!";
}
}; //Eof Namespace
