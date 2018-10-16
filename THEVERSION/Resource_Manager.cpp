#include "resource_manager.h"
//^^ that file says it all
namespace GeklminRender {
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
}
}; //Eof Namespace