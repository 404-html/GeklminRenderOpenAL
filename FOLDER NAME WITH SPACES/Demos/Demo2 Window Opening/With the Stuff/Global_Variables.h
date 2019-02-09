#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

//FILE ONLY INCLUDED FOR DEMO!!!
//These things will (probably) exist no matter what program you write
namespace GeklminRender {
IODevice* myDevice = new IODevice();
Resource_Manager* FileResourceManager = new Resource_Manager();
GkScene* theScene; //Pointer to the scene
};
#endif //Global_variables_h
