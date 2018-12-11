#include "mesh.h"
#include "obj_loader.h"
#include <vector>
#include <iostream>
#include <string>
#include <GL3/gl3w.h>
#include <GL3/gl3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

namespace GeklminRender {

/**
Fast version:
Loads the mesh from filebuf
saves it
gets an indexed model
saves it

fiddles around with filenames a bit

then initializes
**/
Mesh::Mesh(std::string filename, bool instanced, bool isstatic, bool assetjaodernein, bool recalculatenormals)
{
	//std::cout<<"\nCurrently loading mesh: " <<filename;
	IndexedModel mmphmodel; //To avoid confusing with the mesh member variable
	OBJModel mdl = OBJModel(filename);
	mmphmodel = mdl.ToIndexedModel();
	if (recalculatenormals)
	{
		std::cout << "\nRecalculating Normals!";
		for (size_t i = 0; i < mmphmodel.normals.size(); i++)
			mmphmodel.normals[i] = glm::vec3(0,0,0);
		mmphmodel.CalcNormals();
	}
	// std::cout<<"\nModel " << filename << " has " << mmphmodel.indices.size() << " Indices or " << mmphmodel.indices.size()/3.0 << " Tris to be rendered every frame.";
	// std::cout << "\nDid it have Normals?" << mdl.hasNormals?" TRUE!":" FALSE!";
	MyName = filename;
	is_instanced = instanced; //Do we have hardware instancing?
	is_static = isstatic;//can we edit the VBOs
	if(isnull)
		InitMesh(mmphmodel, instanced, isstatic); //make the OpenGL Object
	else
		reShapeMesh(mmphmodel);
	is_asset = assetjaodernein; //is the resource manager responsible for deleting this
	
	
	/* 
	Yes, Germany was back then a democracy, before us and we’ve been plundered and squeezed dry. No.. What does Democracy or authoritarian state mean for those international hyenas? They don’t care at all! They are only interested in one thing. Are you willing to be plundered? Yes or No?. Are you stupid enough to keep quite in the process? Yes or No ? And when democracy is stupid enough Not to stand up, Then it’s good! But when an  authoritarian state declares “ You do not plunder our people any longer” Neither from the inside or outside that is bad… In reality money rules in this countries.. They talk about press freedom when in fact all these newspapers have one owner and the owner is in any case, the sponsor.. This press then shapes public opinion. These political parties don’t have any differences at all like before with us. You already know the old political parties. They were all the same. Then people must think that especially in these countries of freedom and wealth.. There should exist a very comfortable life for it’s people but the opposite is the case. In these countries in the so-called “democracies” The people is by no means the main focus of attention. What really matters is the existence of this group of “democracy makers”. That is the existence of a few hundred giant capitalists who own all the factories and shares and who, ultimately, lead the people. They are not interested at all in the great mass of people. J****! they are the only ones who can be addressed as international elements because they conduct their business everywhere. It is a small, rootless, International clique that is turning the people against each other, that does want them to have peace. They can suppress us!, They can kill us, if you like! But we will not capitulate!
	*/
	//std::cout<<"\n My name is " + MyName + "\n";
}






void Mesh::DrawInstancesPhong(GLuint Model, GLuint rflags, GLuint specr, GLuint specd, GLuint diff, GLuint emiss, GLuint enableCubemaps, GLuint enableCubemaps_diffuse, GLuint EnableTransparency, GLuint EnableInstancing, bool Transparency, bool toRenderTarget, int meshmask, bool usePhong, bool donotusemaps, bool force_non_instanced){
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





//Big scary function
//Rundown:
//Generates and Binds the VAO
//Generates the VBOs
//For all the VBOs, it binds them and sends in the data
//This is the function which I imagine will confuse many newbies to OpenGL. the VBO/VAO system is somewhat confusing, although when you do get your head around it, it will become second nature
//DON'T USE IMMEDIATE MODE! There is never EVER a reason to use deprecated OpenGL on a modern computer! Just don't do it, it pisses me off. Steal my code (here) instead, I don't care.
void Mesh::InitMesh(const IndexedModel& model, bool instancing, bool shallwemakeitstatic)
{
	// std::cout <<"\n Friendly neighborhood debugger again. Model is: \"" << MyName << "\"\n and I'm going be debugging the COLOR_VB buffer input.";
	// std::cout <<"\n There are " << model.colors.size() <<" color vectors.\n For comparison, there are " << model.normals.size() << " normal vectors and " << model.positions.size()<<" verts\n";
	// if(MyName == "happy.OBJ") //WOOF!
		// for(int i = 0; i<model.colors.size(); i++)
		// {
			// std::cout << "\nCOLOR " << i << " R:" << model.colors[i].x << " G:" << model.colors[i].y << " B:" << model.colors[i].z;
		// }
	//std::cout << "\n initializing mesh " << model.myFileName<< " has " << model.indices.size() << " Indices or " << model.indices.size()/3.0 << " Tris to be rendered every frame.";;
	if (!isnull){ //WE DONT RE-INITIALIZE IN THIS HOUSE!!!
		std::cerr << "\n ERROR! It is INVALID to call the constructor of an already-instantiated OpenGL Object!\n It is the convention of this library for assets and MOST opengl objects to be non-reinstantiatable." <<
		"\n\n at the current time that includes these classes:" <<
		//"\n Mesh" <<
		//"\n FBO" <<
		//"\n Texture" <<
		"\n Shader" <<
		"\n Cubemap" <<
		"\n If you want to re-instantiate one of these classes, just delete it and make a new one. ";
		std::abort();
		//in case that didn't get them hehe
		int* p = nullptr;
		*p = 0;
	}
	ModelData = model; //Save it.
	isnull = false;
	is_instanced = instancing;
	is_static = shallwemakeitstatic;
	if (model.hadRenderFlagsInFile)
	{
		setFlags(model.renderflags);
	}
	else
		renderflags = GK_RENDER | GK_TEXTURED | GK_TINT; //We want to ensure that users who load ordinary OBJs are fully accomodated. No encumberments.

	MyName = model.myFileName;
	unsigned int numIndices = (unsigned int)model.indices.size();

	m_drawCount = numIndices; //m_ convention is incorrect here.
	unsigned int numVertices = model.positions.size();
	glGenVertexArrays(1, &m_vertexArrayObject);//allocate a vertex array object
	glBindVertexArray(m_vertexArrayObject);//Work with this VAO


	///BUFFERS!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	///BUFFERS!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	///BUFFERS!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);//Make NUM_BUFFERS buffers
	if(is_instanced)
	{
		glGenBuffers(1,&InstanceModelMatrixVBO);
		//std::cout << "\n Created Instance Model Matrix VBO!";
		
			//glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), ContiguousMemory, GL_STATIC_DRAW); //Format of command
		glBindBuffer(GL_ARRAY_BUFFER, InstanceModelMatrixVBO);//Work with the Instanced Model Matrix Buffer
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		//                    #  #floats    type     normalize  how many per cycle?	    			offset
		glVertexAttribPointer(4, 4, 		GL_FLOAT, GL_FALSE, 16 * (int)sizeof(GL_FLOAT), 		0);
		glVertexAttribPointer(5, 4,			GL_FLOAT, GL_FALSE, 16 * (int)sizeof(GL_FLOAT), 		(void*)(4 * sizeof(GL_FLOAT)));
		glVertexAttribPointer(6, 4, 		GL_FLOAT, GL_FALSE, 16 * (int)sizeof(GL_FLOAT), 		(void*)(8 * sizeof(GL_FLOAT)));
		glVertexAttribPointer(7, 4, 		GL_FLOAT, GL_FALSE, 16 * (int)sizeof(GL_FLOAT), 		(void*)(12 * sizeof(GL_FLOAT)));
		
		glVertexAttribDivisor(4,1); //col A
		glVertexAttribDivisor(5,1); //col B
		glVertexAttribDivisor(6,1); //col C
		glVertexAttribDivisor(7,1); //col D
		
	}



	///Buffer 1: Positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);//Work with the Position Buffer

	GLfloat* ContiguousMemory = (GLfloat*)malloc(numVertices * 3 * sizeof(GLfloat)); // we use this again later...
	for (int i = 0; i<numVertices; i++)
	{
		ContiguousMemory[(3*i)] = model.positions[i].x;
		ContiguousMemory[(3*i)+1] = model.positions[i].y;
		ContiguousMemory[(3*i)+2] = model.positions[i].z;
	}
	glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), ContiguousMemory, is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW); //Note to self: Deletes all pre-existing storage for the currently bound buffer, so we could use it for an expensive way of uploading buffer data. EDIT: Which is exactly what we are doing!!!
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	free(ContiguousMemory);




	// Texture Coordinate Information
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
	ContiguousMemory = (GLfloat*)malloc(model.texCoords.size() * 2 * sizeof(GLfloat));
	for (int i = 0; i<model.texCoords.size(); i++)
	{
		ContiguousMemory[(2*i)] = model.texCoords[i].x;
		ContiguousMemory[(2*i)+1] = model.texCoords[i].y;
	}
	glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * 2 * sizeof(GLfloat), ContiguousMemory, is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	free(ContiguousMemory);

	// Normal Information
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	ContiguousMemory = (GLfloat*)malloc(model.normals.size() * 3 * sizeof(GLfloat));
	for (int i = 0; i<model.normals.size(); i++)
	{
		ContiguousMemory[(3*i)] = model.normals[i].x;
		ContiguousMemory[(3*i)+1] = model.normals[i].y;
		ContiguousMemory[(3*i)+2] = model.normals[i].z;
	}
	glBufferData(GL_ARRAY_BUFFER, model.normals.size() * 3 * sizeof(GLfloat), ContiguousMemory, is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	free(ContiguousMemory);

	
	
	//std::cout << "\nMy name is: " << MyName << " and I have " << numVertices << " but the vertex array is of size " << model.positions.size() << "and the color buffer is of size " << model.colors.size();
	//Color buffer information
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[COLOR_VB]);
	ContiguousMemory = (GLfloat*)malloc(model.colors.size() * 3 * sizeof(GLfloat)); //UNADDRESSABLE ACCESS according to Dr. Memory (solved??? see below)
	if (model.colors.size() > 0)
		for (int i = 0; i<model.colors.size(); i++)
		{
			ContiguousMemory[(3*i)] = model.colors[i].x;
			ContiguousMemory[(3*i)+1] = model.colors[i].y;
			ContiguousMemory[(3*i)+2] = model.colors[i].z;
		}
	glBufferData(GL_ARRAY_BUFFER, model.colors.size() * 3 * sizeof(GLfloat), ContiguousMemory, is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);//I KNOW WHY DOCTOR MEMORY WAS ANGRY. I was doing the screenquad. The screenquad in the scene class has no color buffer. Silly me! (solved???)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	free(ContiguousMemory);

	//This is where you make per-instance buffers if you want.


	//NOTE: PLEASE DO NOT ADD YOUR CUSTOM BUFFER HERE UNLESS YOU ARE PREPARED TO RE-WRITE THE OBJLOADER! Load your bullshit in a separate function. That way, everything works out nicely and neatly

	// Index Buffer Information
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), &model.indices[0], is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}


void Mesh::reShapeMesh(const IndexedModel& model){
	// std::cout <<"\n Friendly neighborhood debugger again. Model is: \"" << MyName << "\"\n and I'm going be debugging the COLOR_VB buffer input.";
	// std::cout <<"\n There are " << model.colors.size() <<" color vectors.\n For comparison, there are " << model.normals.size() << " normal vectors and " << model.positions.size()<<" verts\n";
	// if(MyName == "happy.OBJ") //WOOF!
		// for(int i = 0; i<model.colors.size(); i++)
		// {
			// std::cout << "\nCOLOR " << i << " R:" << model.colors[i].x << " G:" << model.colors[i].y << " B:" << model.colors[i].z;
		// }
	//std::cout << "\n initializing mesh " << model.myFileName<< " has " << model.indices.size() << " Indices or " << model.indices.size()/3.0 << " Tris to be rendered every frame.";;
	
	//If we allowed someone to reshape a mesh multiple times before the vsync, the memory might fuck up. Do not let it happen.
	if (!canbereshaped)
		return;
	if (isnull){ //WE DONT RE-INITIALIZE IN THIS HOUSE!!!
		std::cerr << "ERROR! You tried to Re-Initialize a mesh without first initializing! The engine will crash now. Goodbye." << std::endl;
		std::abort();
		//in case that didn't get them hehe
		int* p = nullptr;
		*p = 0;
	}
	//std::cout << "\n Managed to get past isnull check!!!";
	ModelData = model; //Save it.
	if (model.hadRenderFlagsInFile)
	{
		setFlags(model.renderflags);
	}
	else
		renderflags = GK_RENDER | GK_TEXTURED | GK_TINT; //We want to ensure that users who load ordinary OBJs are fully accomodated. No encumberments.

	MyName = model.myFileName;
	unsigned int numIndices = (unsigned int)model.indices.size();

	m_drawCount = numIndices; //m_ convention is incorrect here.
	unsigned int numVertices = model.positions.size();
	// glGenVertexArrays(1, &m_vertexArrayObject);//allocate a vertex array object
	 glBindVertexArray(m_vertexArrayObject);//Work with this VAO


	///BUFFERS!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	///BUFFERS!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	///BUFFERS!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);//Make NUM_BUFFERS buffers
	// if(is_instanced)
	// {
		// glGenBuffers(1,&InstanceModelMatrixVBO);
		// std::cout << "\n Created Instance Model Matrix VBO!";
		
			// glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), ContiguousMemory, GL_STATIC_DRAW); //Format of command
		// glBindBuffer(GL_ARRAY_BUFFER, InstanceModelMatrixVBO);//Work with the Instanced Model Matrix Buffer
		// glEnableVertexAttribArray(4);
		// glEnableVertexAttribArray(5);
		// glEnableVertexAttribArray(6);
		// glEnableVertexAttribArray(7);
		                   // #  #floats    type     normalize  how many per cycle?	    			offset
		// glVertexAttribPointer(4, 4, 		GL_FLOAT, GL_FALSE, 16 * (int)sizeof(GL_FLOAT), 		0);
		// glVertexAttribPointer(5, 4,			GL_FLOAT, GL_FALSE, 16 * (int)sizeof(GL_FLOAT), 		(void*)(4 * sizeof(GL_FLOAT)));
		// glVertexAttribPointer(6, 4, 		GL_FLOAT, GL_FALSE, 16 * (int)sizeof(GL_FLOAT), 		(void*)(8 * sizeof(GL_FLOAT)));
		// glVertexAttribPointer(7, 4, 		GL_FLOAT, GL_FALSE, 16 * (int)sizeof(GL_FLOAT), 		(void*)(12 * sizeof(GL_FLOAT)));
		
		// glVertexAttribDivisor(4,1); //col A
		// glVertexAttribDivisor(5,1); //col B
		// glVertexAttribDivisor(6,1); //col C
		// glVertexAttribDivisor(7,1); //col D
		
	// }



	///Buffer 1: Positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);//Work with the Position Buffer

	GLfloat* ContiguousMemory = (GLfloat*)malloc(numVertices * 3 * sizeof(GLfloat)); // we use this again later...
	for (int i = 0; i<numVertices; i++)
	{
		ContiguousMemory[(3*i)] = model.positions[i].x;
		ContiguousMemory[(3*i)+1] = model.positions[i].y;
		ContiguousMemory[(3*i)+2] = model.positions[i].z;
	}
	glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(GLfloat), ContiguousMemory, is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW); //Note to self: Deletes all pre-existing storage for the currently bound buffer, so we could use it for an expensive way of uploading buffer data. EDIT: Which is exactly what we are doing!!!
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	free(ContiguousMemory);




	// Texture Coordinate Information
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXCOORD_VB]);
	ContiguousMemory = (GLfloat*)malloc(model.texCoords.size() * 2 * sizeof(GLfloat));
	for (int i = 0; i<model.texCoords.size(); i++)
	{
		ContiguousMemory[(2*i)] = model.texCoords[i].x;
		ContiguousMemory[(2*i)+1] = model.texCoords[i].y;
	}
	glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * 2 * sizeof(GLfloat), ContiguousMemory, is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	free(ContiguousMemory);

	// Normal Information
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	ContiguousMemory = (GLfloat*)malloc(model.normals.size() * 3 * sizeof(GLfloat));
	for (int i = 0; i<model.normals.size(); i++)
	{
		ContiguousMemory[(3*i)] = model.normals[i].x;
		ContiguousMemory[(3*i)+1] = model.normals[i].y;
		ContiguousMemory[(3*i)+2] = model.normals[i].z;
	}
	glBufferData(GL_ARRAY_BUFFER, model.normals.size() * 3 * sizeof(GLfloat), ContiguousMemory, is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	free(ContiguousMemory);

	//m_drawCount
	
	//std::cout << "\nMy name is: " << MyName << " and I have " << numVertices << " but the vertex array is of size " << model.positions.size() << "and the color buffer is of size " << model.colors.size();
	//Color buffer information
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[COLOR_VB]);
	ContiguousMemory = (GLfloat*)malloc(model.colors.size() * 3 * sizeof(GLfloat)); //UNADDRESSABLE ACCESS according to Dr. Memory (solved??? see below)
	if (model.colors.size() > 0)
		for (int i = 0; i<model.colors.size(); i++)
		{
			ContiguousMemory[(3*i)] = model.colors[i].x;
			ContiguousMemory[(3*i)+1] = model.colors[i].y;
			ContiguousMemory[(3*i)+2] = model.colors[i].z;
		}
	glBufferData(GL_ARRAY_BUFFER, model.colors.size() * 3 * sizeof(GLfloat), ContiguousMemory, is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);//I KNOW WHY DOCTOR MEMORY WAS ANGRY. I was doing the screenquad. The screenquad in the scene class has no color buffer. Silly me! (solved???)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	free(ContiguousMemory);

	//This is where you make per-instance buffers if you want.


	//NOTE: PLEASE DO NOT ADD YOUR CUSTOM BUFFER HERE UNLESS YOU ARE PREPARED TO RE-WRITE THE OBJLOADER! Load your bullshit in a separate function. That way, everything works out nicely and neatly

	// Index Buffer Information
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexArrayBuffers[INDEX_VB]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), &model.indices[0], is_static?GL_STATIC_DRAW:GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
	canbereshaped = false;
	//std::cout << "\n Finished Function";
}











//This code is only here as a memory... it was my first functional, custom draw code in modern OpenGL. Toot your horns in HONOR!!!
//void Mesh::Draw(){
//	glBindVertexArray(m_vertexArrayObject);
//	//std::cout<<"\nBoundVertexArray!!!";
//	//glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
//	//std::cout<<"\nDREWARRAYS!";
//	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
//	//std::cout<<"\nStumped.";
//}

Mesh::~Mesh()
{
	//Handles the deletion of our stuff.
	glDeleteBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	if (is_instanced)
		glDeleteBuffers(1, &InstanceModelMatrixVBO);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
	if (MatrixDataCacheMalloced.size() > 0)
		MatrixDataCacheMalloced.clear();
		// for (size_t i = 0; i < MatrixDataCacheMalloced.size(); i++)
		// {
			// if(MatrixDataCacheMalloced[i] != nullptr)
			// {free(MatrixDataCacheMalloced[i]);}
		// }
	// if (MatrixDataCache.size() > 0)
		// for (size_t i = 0; i < MatrixDataCache.size(); i++)
		// {
			// if(MatrixDataCache[i] != nullptr)
			// {delete MatrixDataCache[i];}
		// }
	
	isnull = true;
	// if (MyTextures.size() > 0)
		// for(int i = MyTextures.size() -1; i > -1; i--)
			// delete MyTextures[i]; //Safetextures are safe to delete
	//Note that if we load any custom VBOs we also have to destroy them.
}
}; //Eof Namespace
