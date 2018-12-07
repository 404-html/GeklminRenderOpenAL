#include "SceneAPI.h"
#include "geklminrender.h"
//(C) DMHSW 2018 All Rights Reserved


namespace GeklminRender {
	
//Render shadows to rendertarget (Just a depth buffer render
void GkScene::drawShadowPipeline(int meshmask, FBO* CurrentRenderTarget, FBO* RenderTarget_Transparent, Camera* CurrentRenderCamera, bool doFrameBufferChecks, glm::vec4 backgroundColor, glm::vec2 fogRangeDescriptor){
	//Used for error checking
	GLenum communism;
	//Dont render to target unless we have both FBOs
	if (!CurrentRenderTarget)
		{
			std::cout<<"\nYou got kicked out boi 1";
			return;
		}
	if (nextRenderPipelineCallWillBeAfterVsync) //Reset everything if this render call was immediately after the vsync
	{
		// for (int iter = 0; iter < RendertargetCameras.size(); iter++)
			// delete RendertargetCameras[iter];
		// RendertargetCameras.clear();
		nextRenderPipelineCallWillBeAfterVsync = false;
	}
	
	/* Error Check code. Paste where you need it.
	
	// communism = glGetError(); //Ensure there are no errors listed before we start.
	// if (communism != GL_NO_ERROR) //if communism has made an error (which is pretty typical)
	// {
		// std::cout<<"\n OpenGL reports an ERROR!";
		// if (communism == GL_INVALID_ENUM)
			// std::cout<<"\n Invalid enum.";
		// if (communism == GL_INVALID_OPERATION)
			// std::cout<<"\n Invalid operation.";
		// if (communism == GL_INVALID_FRAMEBUFFER_OPERATION)
			// std::cout <<"\n Invalid Framebuffer Operation.";
		// if (communism == GL_OUT_OF_MEMORY)
		// {
			// std::cout <<"\n Out of memory. You've really done it now. I'm so angry, i'm going to close the program. ARE YOU HAPPY NOW, DAVE?!?!";
			// std::abort();
		// }
	// }
	*/
	
	
	
	
	
	if (!ShadowOpaqueMainShader || !ShowTextureShader /*|| !CompositionShader*/){ //if one is not present
		std::cout<<"\nYou got kicked out boi 3";
		return; //gtfo
	}
	
	
	
	
	
	
	
	
	
	glEnable(GL_CULL_FACE); //Enable culling faces
	glCullFace(GL_BACK); //cull faces with clockwise winding
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	FBO* MainFBO = nullptr;
	FBO* TransparencyFBO = nullptr;
	if (CurrentRenderTarget == nullptr) {
		MainFBO = FboArray[FORWARD_BUFFER1];
		//~ TransparencyFBO = FboArray[FORWARD_BUFFER2];
		//FboArray[FORWARD_BUFFER1]->BindRenderTarget();
	} else {
		MainFBO = CurrentRenderTarget;
		//~ TransparencyFBO = RenderTarget_Transparent;
		//CurrentRenderTarget->BindRenderTarget();
	}
	MainFBO->BindRenderTarget();
	FBO::clearTexture(1,1,1,1);
	//std::cout<<"\nShould be clearing the screen";
	/*
	
	Skybox Render
	customMainShaderBinds
	
	*/
	//This draws the skybox in the background
	//~ if(SkyBoxCubemap && SkyboxShader)
	//~ {
		//~ SkyboxShader->Bind();
		//~ if (!haveInitializedSkyboxUniforms)
		//~ {
			//~ SkyboxUniforms[SKYBOX_WORLD2CAMERA] = SkyboxShader->GetUniformLocation("World2Camera");
			//~ SkyboxUniforms[SKYBOX_MODEL2WORLD] = SkyboxShader->GetUniformLocation("Model2World");
			//~ SkyboxUniforms[SKYBOX_VIEWMATRIX] = SkyboxShader->GetUniformLocation("viewMatrix");
			//~ SkyboxUniforms[SKYBOX_PROJECTION] = SkyboxShader->GetUniformLocation("projection");
			//~ SkyboxUniforms[SKYBOX_WORLDAROUNDME] = SkyboxShader->GetUniformLocation("worldaroundme");
			//~ haveInitializedSkyboxUniforms = true; //We've done it boys.
		//~ }
		
		//~ if (CurrentRenderCamera == nullptr)
		//~ {
			//~ glUniformMatrix4fv(SkyboxUniforms[SKYBOX_WORLD2CAMERA], 1, GL_FALSE, &SceneRenderCameraMatrix[0][0]);
			//~ glUniformMatrix4fv(SkyboxUniforms[SKYBOX_VIEWMATRIX], 1, GL_FALSE, &SceneRenderCameraViewMatrix[0][0]);
			//~ glUniformMatrix4fv(SkyboxUniforms[SKYBOX_PROJECTION], 1, GL_FALSE, &SceneRenderCameraProjectionMatrix[0][0]);
		//~ } else {
			//~ //&((*RendertargetCameras[RendertargetCameras.size()-3])[0][0])
			//~ glUniformMatrix4fv(SkyboxUniforms[SKYBOX_WORLD2CAMERA], 1, GL_FALSE, &(CurrentRenderCamera->GetViewProjection()[0][0]));
			//~ glUniformMatrix4fv(SkyboxUniforms[SKYBOX_VIEWMATRIX], 1, GL_FALSE, &(CurrentRenderCamera->GetViewMatrix()[0][0]));
			//~ glUniformMatrix4fv(SkyboxUniforms[SKYBOX_PROJECTION], 1, GL_FALSE, &(CurrentRenderCamera->GetProjection()[0][0]));
		//~ }
		//~ skybox_transform.reTransform(glm::vec3(0,0,0),glm::vec3(0,0,0),glm::vec3(SceneCamera->jafar * 0.5,SceneCamera->jafar * 0.5,SceneCamera->jafar * 0.5));
	
		
	
	
		//~ //Texture::SetActiveUnit(1); //??
		//~ SkyBoxCubemap->Bind(1); //Bind to 1
		//~ glDepthMask(GL_FALSE); //We want it to appear infinitely far away
		//~ // glEnable(GL_CULL_FACE);
		//~ glUniform1i(SkyboxUniforms[SKYBOX_WORLDAROUNDME], 1);
		//~ glEnableVertexAttribArray(0); //Position
		//~ glEnableVertexAttribArray(2); //Normal
			//~ Skybox_Transitional_Transform = skybox_transform.GetModel();
			//~ glUniformMatrix4fv(SkyboxUniforms[SKYBOX_MODEL2WORLD], 1, GL_FALSE, &Skybox_Transitional_Transform[0][0]);
			//~ m_skybox_Mesh->DrawGeneric();
		//~ glDepthMask(GL_TRUE);//We would like depth testing to be done again.
	
	//~ }
	
	//This is where we would render any objects which don't use the mainshader
	//~ if (customRenderingAfterSkyboxBeforeMainShader != nullptr)
		//~ customRenderingAfterSkyboxBeforeMainShader(meshmask, CurrentRenderTarget, RenderTarget_Transparent, CurrentRenderCamera, doFrameBufferChecks, backgroundColor, fogRangeDescriptor);
	/*
		MAINSHADER RENDERING
	*/
	ShadowOpaqueMainShader->Bind(); //Bind the shader!
	
	
	//Runs whenever the window is resized or a shader is reassigned, so that we only need to get uniform locations once. It's not optimized fully yet...
	if(HasntRunYet_Shadows){

		HasntRunYet_Shadows = false;
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_DIFFUSE] = ShadowOpaqueMainShader->GetUniformLocation("diffuse"); //Literal texture unit
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_EMISSIVITY] = ShadowOpaqueMainShader->GetUniformLocation("emissivity"); //Emissivity value
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_WORLD2CAMERA] = ShadowOpaqueMainShader->GetUniformLocation("World2Camera"); //World --> NDC
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_MODEL2WORLD] = ShadowOpaqueMainShader->GetUniformLocation("Model2World"); //Model --> World
		//MainShaderUniforms_SHADOWTEMP[MAINSHADER_AMBIENT] = ShadowOpaqueMainShader->GetUniformLocation("ambient"); //Ambient component of the material
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_SPECREFLECTIVITY] = ShadowOpaqueMainShader->GetUniformLocation("specreflectivity"); //Specular reflectivity
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_SPECDAMP] = ShadowOpaqueMainShader->GetUniformLocation("specdamp"); //Specular dampening
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_DIFFUSIVITY] = ShadowOpaqueMainShader->GetUniformLocation("diffusivity"); //Diffusivity
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_RENDERFLAGS] = ShadowOpaqueMainShader->GetUniformLocation("renderflags"); //Renderflags. Going to be used again b/c we're going to use per-vertex colors again!
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_WORLDAROUNDME] = ShadowOpaqueMainShader->GetUniformLocation("worldaroundme");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_ENABLE_CUBEMAP_REFLECTIONS] = ShadowOpaqueMainShader->GetUniformLocation("enableCubeMapReflections");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_ENABLE_CUBEMAP_DIFFUSION] = ShadowOpaqueMainShader->GetUniformLocation("enableCubeMapDiffusivity");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERAPOS] = ShadowOpaqueMainShader->GetUniformLocation("CameraPos");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_ENABLE_TRANSPARENCY] = ShadowOpaqueMainShader->GetUniformLocation("EnableTransparency");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_NUM_POINTLIGHTS] = ShadowOpaqueMainShader->GetUniformLocation("numpointlights");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_NUM_DIRLIGHTS] = ShadowOpaqueMainShader->GetUniformLocation("numdirlights");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_NUM_AMBLIGHTS] = ShadowOpaqueMainShader->GetUniformLocation("numamblights");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_NUM_CAMLIGHTS] = ShadowOpaqueMainShader->GetUniformLocation("numcamlights");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_IS_INSTANCED] = ShadowOpaqueMainShader->GetUniformLocation("is_instanced");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX1] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex1");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX2] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex2");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX3] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex3");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX4] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex4");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX5] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex5");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX6] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex6");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX7] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex7");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX8] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex8");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX9] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex9");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX10] = ShadowOpaqueMainShader->GetUniformLocation("CameraTex10");
		/*
		MAINSHADER_SKYBOX_CUBEMAP, //The cubemap of the Skybox
		MAINSHADER_BACKGROUND_COLOR, //if a is 0, it uses the skybox cubemap, if a is 1, it uses the background color
		MAINSHADER_FOG_RANGE, //What distance from the camera should fog start at and go to 100% at?
		*/
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_SKYBOX_CUBEMAP] = ShadowOpaqueMainShader->GetUniformLocation("SkyboxCubemap");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_BACKGROUND_COLOR] = ShadowOpaqueMainShader->GetUniformLocation("backgroundColor");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_FOG_RANGE] = ShadowOpaqueMainShader->GetUniformLocation("fogRange");
		//Get the uniform locations for lights
		//m_LightUniformHandles_SHADOWTEMP
		if (m_LightUniformHandles_SHADOWTEMP.size() > 0)
			m_LightUniformHandles_SHADOWTEMP.clear();
		if (m_LightClippingVolumeUniformHandles_SHADOWTEMP.size() > 0)
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.clear();
		for (int i = 0; i < 32; i++) //Point lights
		{
			//access by taking i, multiplying by 4 and adding an offset (0 for position, 1 for color...)
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].position"));
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].color"));
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].range"));
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].dropoff"));
			//AABB info
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp1"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp2"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp3"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp4"));
			//Sphere Info
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere1"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere2"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere3"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere4"));
			//The whitelist uint
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].iswhitelist"));
			//std::cout << "point_lightArray[" + std::to_string(i) + "].dropoff";
		}
		for (int i = 0; i < 2; i++) //Dir Lights
		{
			//access by taking i, multiplying by 2, adding 4 * max point lights, and adding an offset (0 for direction, 1 for color...)
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].direction"));
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].color"));
			
			//AABB info
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp1"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp2"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp3"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp4"));
			//Sphere Info
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere1"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere2"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere3"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere4"));
			//The whitelist uint
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].iswhitelist"));
		}
		
		for (int i = 0; i < 3; i++) //Ambient Lights
		{
			//access by taking i, multiplying by 3, adding 4 * max pointlights + 2 * max dir lights, and adding an offset (0 for position, 1 for color...)
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].position"));
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].color"));
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].range"));
			
			//AABB info
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp1"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp2"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp3"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp4"));
			//Sphere Info
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere1"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere2"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere3"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere4"));
			//The whitelist uint
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].iswhitelist"));
		}
		
		for (int i = 0; i < 5; i++) //Cameralights
		{
			//access by taking i, multiplying by 3, adding 4 * maxpointlights + 2 * maxdirlights + 3 * maxambientlights, and adding an offset (0 for viewproj, 1 for color...)
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].viewproj"));//0
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].position"));//1
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].color"));//2
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].solidColor"));//3
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].range"));//4
			m_LightUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].shadows"));//5
			//AABB info
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].AABBp1"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].AABBp2"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].AABBp3"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].AABBp4"));
			//Sphere Info
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].sphere1"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].sphere2"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].sphere3"));
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].sphere4"));
			//The whitelist uint
			m_LightClippingVolumeUniformHandles_SHADOWTEMP.push_back(ShadowOpaqueMainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].iswhitelist"));
		}
		//debug
		// for (size_t i = 0; i < m_LightUniformHandles_SHADOWTEMP.size(); i++)
		// {
			// std::cout << "\n DEBUG LIGHTUNIFORM HANDLES: " + std::to_string(m_LightUniformHandles_SHADOWTEMP[i]);
		// }
	}
	//Custom Bindings
	//~ if (customMainShaderBinds != nullptr)
		//~ customMainShaderBinds(meshmask, CurrentRenderTarget, RenderTarget_Transparent, CurrentRenderCamera, doFrameBufferChecks, backgroundColor, fogRangeDescriptor); //TODO: Why aren't we passing the shader?
	
	if (CurrentRenderCamera == nullptr)
		glUniform3f(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERAPOS], SceneCamera->pos.x, SceneCamera->pos.y, SceneCamera->pos.z);
	else
		glUniform3f(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERAPOS], CurrentRenderCamera->pos.x, CurrentRenderCamera->pos.y, CurrentRenderCamera->pos.z);
	
	/*
		MAINSHADER_SKYBOX_CUBEMAP, //The cubemap of the Skybox
		MAINSHADER_BACKGROUND_COLOR, //if a is 0, it uses the skybox cubemap, if a is 1, it uses the background color
		MAINSHADER_FOG_RANGE, //What distance from the camera should fog start at and go to 100% at?
	*/
	
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_SKYBOX_CUBEMAP],2);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX1], 3);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX2], 4);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX3], 5);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX4], 6);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX5], 7);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX6], 8);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX7], 9);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX8], 10);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX9], 11);
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERATEX10], 12);
	if (SkyBoxCubemap)
		SkyBoxCubemap->Bind(2);
	//glm::vec4 tempBackgroundColor = glm::vec4(0,0,0,0);
	//glm::vec2 tempFogRange = glm::vec2(500,800);
	glUniform4f(MainShaderUniforms_SHADOWTEMP[MAINSHADER_BACKGROUND_COLOR], 1, 1, 1, 1);
	glUniform2f(MainShaderUniforms_SHADOWTEMP[MAINSHADER_FOG_RANGE], fogRangeDescriptor.x, fogRangeDescriptor.y);
	
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_DIFFUSE],0);
		
	//Do this regardless of the presence of Skyboxcubemap
	glUniform1i(MainShaderUniforms_SHADOWTEMP[MAINSHADER_WORLDAROUNDME], 1);//Cubemap unit 1 is reserved for the cubemap representing the world around the object, for reflections.
	
	//TODO: Change camera reference for sorting lights to the current render target camera (if applicable)

	if (CurrentRenderCamera == nullptr)
		{glUniformMatrix4fv(MainShaderUniforms_SHADOWTEMP[MAINSHADER_WORLD2CAMERA], 1, GL_FALSE, &SceneRenderCameraMatrix[0][0]);}
	else
		{glUniformMatrix4fv(MainShaderUniforms_SHADOWTEMP[MAINSHADER_WORLD2CAMERA], 1, GL_FALSE, &(CurrentRenderCamera->GetViewProjection()[0][0]));}
	// glEnable(GL_CULL_FACE);
	//Now that we have the shader stuff set up, let's get to rendering!
	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); //Texture
	glEnableVertexAttribArray(2); //Normal
	glEnableVertexAttribArray(3); //Color
	//Render Opaque Objects. Note we configured depth masking and blending for this pass much earlier on.
		if (Meshes.size() > 0) //if there are any
			for (size_t i = 0; i < Meshes.size(); i++) //for all of them
				if (Meshes[i]) //don't call methods on nullptrs
				{
					// unsigned int flagerinos = Meshes[i]->getFlags(); //Set flags
					// glUniform1ui(MainShaderUniforms[MAINSHADER_RENDERFLAGS], flagerinos); //Set flags on GPU
					Meshes[i]->DrawInstancesPhong(
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_MODEL2WORLD], 		//Model->World transformation matrix
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_RENDERFLAGS],
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_SPECREFLECTIVITY], 	//Specular reflective material component
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_SPECDAMP], 		//Specular dampening material component
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_DIFFUSIVITY],   //Diffusivity. Reaction to diffuse light.
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_EMISSIVITY],
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_ENABLE_CUBEMAP_REFLECTIONS],
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_ENABLE_CUBEMAP_DIFFUSION],
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_ENABLE_TRANSPARENCY],
						MainShaderUniforms_SHADOWTEMP[MAINSHADER_IS_INSTANCED],
						false,		//NOT transparent
						(CurrentRenderTarget != nullptr)?true:false,		//is it to render target?
						meshmask,
						false, //Phong?
						true, //Do not use maps
						false //Force non-instanced
					);
				}
	//Prep for rendering Transparent Objects
		
	//IF TEST FOR IF WE HAVE THE COMPOSITION SHADER (DON'T ATTEMPT TO DO TRANSPARENCY UNLESS WE HAVE IT)
	
	glDisableVertexAttribArray(0); //Position
	glDisableVertexAttribArray(1); //Texture
	glDisableVertexAttribArray(2); //Normal
	glDisableVertexAttribArray(3); //Color
	
	
	if (CurrentRenderTarget == nullptr) //This part is just for displaying the FBO to the screen... Not used for the FBO stuff.
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		ShowTextureShader->Bind();
		//INTEL GPU FIX
		if(doFrameBufferChecks)
		while(true)
		{
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
				break;
		}
		FBO::unBindRenderTarget(width, height);
		FBO::clearTexture(0,0,0,0);
		FboArray[FORWARD_BUFFER1]->BindasTexture(0, 0);
		// TransparencyFBO->BindasTexture(0,0);
		ScreenquadtoFBO(ShowTextureShader);
	}
	if (CurrentRenderTarget == nullptr)
	{nextRenderPipelineCallWillBeAfterVsync = true;}
	else
	{
		if(doFrameBufferChecks)
			while(true)
			{
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
					break;
			}
		nextRenderPipelineCallWillBeAfterVsync = false;
	}
	
} //eof drawPipeline




void GkScene::drawPipeline(int meshmask, FBO* CurrentRenderTarget, FBO* RenderTarget_Transparent, Camera* CurrentRenderCamera, bool doFrameBufferChecks, glm::vec4 backgroundColor, glm::vec2 fogRangeDescriptor){ //Approaches spaghetti levels of unreadability
	//Used for error checking
	GLenum communism;
	bool doTransparency = true;
	if (CurrentRenderTarget && !RenderTarget_Transparent){
		doTransparency = false;
	}
	//Dont render to target unless we have both FBOs
	//~ if ((CurrentRenderTarget != nullptr && RenderTarget_Transparent == nullptr) || (CurrentRenderTarget == nullptr && RenderTarget_Transparent != nullptr))
		//~ {
			//~ std::cout<<"\nYou got kicked out boi 1";
			//~ return;
		//~ }
	if (nextRenderPipelineCallWillBeAfterVsync) //Reset everything if this render call was immediately after the vsync
	{
		// for (int iter = 0; iter < RendertargetCameras.size(); iter++)
			// delete RendertargetCameras[iter];
		// RendertargetCameras.clear();
		nextRenderPipelineCallWillBeAfterVsync = false;
	}
	
	/* Error Check code. Paste where you need it.
	
	// communism = glGetError(); //Ensure there are no errors listed before we start.
	// if (communism != GL_NO_ERROR) //if communism has made an error (which is pretty typical)
	// {
		// std::cout<<"\n OpenGL reports an ERROR!";
		// if (communism == GL_INVALID_ENUM)
			// std::cout<<"\n Invalid enum.";
		// if (communism == GL_INVALID_OPERATION)
			// std::cout<<"\n Invalid operation.";
		// if (communism == GL_INVALID_FRAMEBUFFER_OPERATION)
			// std::cout <<"\n Invalid Framebuffer Operation.";
		// if (communism == GL_OUT_OF_MEMORY)
		// {
			// std::cout <<"\n Out of memory. You've really done it now. I'm so angry, i'm going to close the program. ARE YOU HAPPY NOW, DAVE?!?!";
			// std::abort();
		// }
	// }
	*/
	
	
	
	
	
	if (!SceneCamera || !MainShader || !MainShaderUniforms || !ShowTextureShader /*|| !CompositionShader*/){ //if one is not present
		std::cout<<"\nYou got kicked out boi 3";
		return; //gtfo
	}
	
	//Setup Camera mat4s so that we can send in the addresses.
	//In the future, we will append these to a vector.
	if (CurrentRenderCamera == nullptr)
	{
		SceneRenderCameraMatrix = SceneCamera->GetViewProjection();
		SceneRenderCameraViewMatrix = SceneCamera->GetViewMatrix();
		SceneRenderCameraProjectionMatrix = SceneCamera->GetProjection();
	} else { //This has to be done to avoid potential issues with the renderer reading bad memory
		// RendertargetCameras.push_back(new glm::mat4(CurrentRenderCamera->GetViewProjection()));
		// RendertargetCameras.push_back(new glm::mat4(CurrentRenderCamera->GetViewMatrix()));
		// RendertargetCameras.push_back(new glm::mat4(CurrentRenderCamera->GetProjection()));
	}
	//In case we need it...
	// InverseViewProjectionMatrix = glm::inverse(SceneRenderCameraMatrix); //Inverse of the Viewprojection Matrix.
	// InverseProjectionMatrix = glm::inverse(SceneRenderCameraProjectionMatrix);//Matrix that undoes perspective transformation
	// InverseViewMatrix = glm::inverse(SceneRenderCameraViewMatrix); 
	
	
	
	
	
	
	
	
	glEnable(GL_CULL_FACE); //Enable culling faces
	glCullFace(GL_BACK); //cull faces with clockwise winding
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	FBO* MainFBO = nullptr;
	FBO* TransparencyFBO = nullptr;
	if (CurrentRenderTarget == nullptr) {
		MainFBO = FboArray[FORWARD_BUFFER1];
		TransparencyFBO = FboArray[FORWARD_BUFFER2];
		//FboArray[FORWARD_BUFFER1]->BindRenderTarget();
	} else {
		MainFBO = CurrentRenderTarget;
		TransparencyFBO = RenderTarget_Transparent;
		//CurrentRenderTarget->BindRenderTarget();
	}
	MainFBO->BindRenderTarget();
	FBO::clearTexture(backgroundColor.x,backgroundColor.y,backgroundColor.z,backgroundColor.w);
	//std::cout<<"\nShould be clearing the screen";
	/*
	
	Skybox Render
	customMainShaderBinds
	
	*/
	//This draws the skybox in the background
	if(SkyBoxCubemap && SkyboxShader)
	{
		SkyboxShader->Bind();
		if (!haveInitializedSkyboxUniforms)
		{
			SkyboxUniforms[SKYBOX_WORLD2CAMERA] = SkyboxShader->GetUniformLocation("World2Camera");
			SkyboxUniforms[SKYBOX_MODEL2WORLD] = SkyboxShader->GetUniformLocation("Model2World");
			SkyboxUniforms[SKYBOX_VIEWMATRIX] = SkyboxShader->GetUniformLocation("viewMatrix");
			SkyboxUniforms[SKYBOX_PROJECTION] = SkyboxShader->GetUniformLocation("projection");
			SkyboxUniforms[SKYBOX_WORLDAROUNDME] = SkyboxShader->GetUniformLocation("worldaroundme");
			haveInitializedSkyboxUniforms = true; //We've done it boys.
		}
		
		if (CurrentRenderCamera == nullptr)
		{
			glUniformMatrix4fv(SkyboxUniforms[SKYBOX_WORLD2CAMERA], 1, GL_FALSE, &SceneRenderCameraMatrix[0][0]);
			glUniformMatrix4fv(SkyboxUniforms[SKYBOX_VIEWMATRIX], 1, GL_FALSE, &SceneRenderCameraViewMatrix[0][0]);
			glUniformMatrix4fv(SkyboxUniforms[SKYBOX_PROJECTION], 1, GL_FALSE, &SceneRenderCameraProjectionMatrix[0][0]);
		} else {
			//&((*RendertargetCameras[RendertargetCameras.size()-3])[0][0])
			glUniformMatrix4fv(SkyboxUniforms[SKYBOX_WORLD2CAMERA], 1, GL_FALSE, &(CurrentRenderCamera->GetViewProjection()[0][0]));
			glUniformMatrix4fv(SkyboxUniforms[SKYBOX_VIEWMATRIX], 1, GL_FALSE, &(CurrentRenderCamera->GetViewMatrix()[0][0]));
			glUniformMatrix4fv(SkyboxUniforms[SKYBOX_PROJECTION], 1, GL_FALSE, &(CurrentRenderCamera->GetProjection()[0][0]));
		}
		skybox_transform.reTransform(glm::vec3(0,0,0),glm::vec3(0,0,0),glm::vec3(SceneCamera->jafar * 0.5,SceneCamera->jafar * 0.5,SceneCamera->jafar * 0.5));
	
		
	
	
		//Texture::SetActiveUnit(1); //??
		SkyBoxCubemap->Bind(1); //Bind to 1
		glDepthMask(GL_FALSE); //We want it to appear infinitely far away
		// glEnable(GL_CULL_FACE);
		glUniform1i(SkyboxUniforms[SKYBOX_WORLDAROUNDME], 1);
		glEnableVertexAttribArray(0); //Position
		glEnableVertexAttribArray(2); //Normal
			Skybox_Transitional_Transform = skybox_transform.GetModel();
			glUniformMatrix4fv(SkyboxUniforms[SKYBOX_MODEL2WORLD], 1, GL_FALSE, &Skybox_Transitional_Transform[0][0]);
			m_skybox_Mesh->DrawGeneric();
		glDepthMask(GL_TRUE);//We would like depth testing to be done again.
	
	}
	
	//This is where we would render any objects which don't use the mainshader
	if (customRenderingAfterSkyboxBeforeMainShader != nullptr)
		customRenderingAfterSkyboxBeforeMainShader(meshmask, CurrentRenderTarget, RenderTarget_Transparent, CurrentRenderCamera, doFrameBufferChecks, backgroundColor, fogRangeDescriptor);
	/*
		MAINSHADER RENDERING
	*/
	MainShader->Bind(); //Bind the shader!
	
	
	//Runs whenever the window is resized or a shader is reassigned, so that we only need to get uniform locations once. It's not optimized fully yet...
	if(HasntRunYet){

		HasntRunYet = false;
		MainShaderUniforms[MAINSHADER_DIFFUSE] = MainShader->GetUniformLocation("diffuse"); //Literal texture unit
		MainShaderUniforms[MAINSHADER_EMISSIVITY] = MainShader->GetUniformLocation("emissivity"); //Emissivity value
		MainShaderUniforms[MAINSHADER_WORLD2CAMERA] = MainShader->GetUniformLocation("World2Camera"); //World --> NDC
		MainShaderUniforms[MAINSHADER_MODEL2WORLD] = MainShader->GetUniformLocation("Model2World"); //Model --> World
		//MainShaderUniforms[MAINSHADER_AMBIENT] = MainShader->GetUniformLocation("ambient"); //Ambient component of the material
		MainShaderUniforms[MAINSHADER_SPECREFLECTIVITY] = MainShader->GetUniformLocation("specreflectivity"); //Specular reflectivity
		MainShaderUniforms[MAINSHADER_SPECDAMP] = MainShader->GetUniformLocation("specdamp"); //Specular dampening
		MainShaderUniforms[MAINSHADER_DIFFUSIVITY] = MainShader->GetUniformLocation("diffusivity"); //Diffusivity
		MainShaderUniforms[MAINSHADER_RENDERFLAGS] = MainShader->GetUniformLocation("renderflags"); //Renderflags. Going to be used again b/c we're going to use per-vertex colors again!
		MainShaderUniforms[MAINSHADER_WORLDAROUNDME] = MainShader->GetUniformLocation("worldaroundme");
		MainShaderUniforms[MAINSHADER_ENABLE_CUBEMAP_REFLECTIONS] = MainShader->GetUniformLocation("enableCubeMapReflections");
		MainShaderUniforms[MAINSHADER_ENABLE_CUBEMAP_DIFFUSION] = MainShader->GetUniformLocation("enableCubeMapDiffusivity");
		MainShaderUniforms[MAINSHADER_CAMERAPOS] = MainShader->GetUniformLocation("CameraPos");
		MainShaderUniforms[MAINSHADER_ENABLE_TRANSPARENCY] = MainShader->GetUniformLocation("EnableTransparency");
		MainShaderUniforms[MAINSHADER_NUM_POINTLIGHTS] = MainShader->GetUniformLocation("numpointlights");
		MainShaderUniforms[MAINSHADER_NUM_DIRLIGHTS] = MainShader->GetUniformLocation("numdirlights");
		MainShaderUniforms[MAINSHADER_NUM_AMBLIGHTS] = MainShader->GetUniformLocation("numamblights");
		MainShaderUniforms[MAINSHADER_NUM_CAMLIGHTS] = MainShader->GetUniformLocation("numcamlights");
		MainShaderUniforms[MAINSHADER_IS_INSTANCED] = MainShader->GetUniformLocation("is_instanced");
		MainShaderUniforms[MAINSHADER_CAMERATEX1] = MainShader->GetUniformLocation("CameraTex1");
		MainShaderUniforms[MAINSHADER_CAMERATEX2] = MainShader->GetUniformLocation("CameraTex2");
		MainShaderUniforms[MAINSHADER_CAMERATEX3] = MainShader->GetUniformLocation("CameraTex3");
		MainShaderUniforms[MAINSHADER_CAMERATEX4] = MainShader->GetUniformLocation("CameraTex4");
		MainShaderUniforms[MAINSHADER_CAMERATEX5] = MainShader->GetUniformLocation("CameraTex5");
		MainShaderUniforms[MAINSHADER_CAMERATEX6] = MainShader->GetUniformLocation("CameraTex6");
		MainShaderUniforms[MAINSHADER_CAMERATEX7] = MainShader->GetUniformLocation("CameraTex7");
		MainShaderUniforms[MAINSHADER_CAMERATEX8] = MainShader->GetUniformLocation("CameraTex8");
		MainShaderUniforms[MAINSHADER_CAMERATEX9] = MainShader->GetUniformLocation("CameraTex9");
		MainShaderUniforms[MAINSHADER_CAMERATEX10] = MainShader->GetUniformLocation("CameraTex10");
		/*
		MAINSHADER_SKYBOX_CUBEMAP, //The cubemap of the Skybox
		MAINSHADER_BACKGROUND_COLOR, //if a is 0, it uses the skybox cubemap, if a is 1, it uses the background color
		MAINSHADER_FOG_RANGE, //What distance from the camera should fog start at and go to 100% at?
		*/
		MainShaderUniforms[MAINSHADER_SKYBOX_CUBEMAP] = MainShader->GetUniformLocation("SkyboxCubemap");
		MainShaderUniforms[MAINSHADER_BACKGROUND_COLOR] = MainShader->GetUniformLocation("backgroundColor");
		MainShaderUniforms[MAINSHADER_FOG_RANGE] = MainShader->GetUniformLocation("fogRange");
		//Get the uniform locations for lights
		//m_LightUniformHandles
		if (m_LightUniformHandles.size() > 0)
			m_LightUniformHandles.clear();
		if (m_LightClippingVolumeUniformHandles.size() > 0)
			m_LightClippingVolumeUniformHandles.clear();
		for (int i = 0; i < 32; i++) //Point lights
		{
			//access by taking i, multiplying by 4 and adding an offset (0 for position, 1 for color...)
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].position"));
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].color"));
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].range"));
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].dropoff"));
			//AABB info
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp1"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp2"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp3"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp4"));
			//Sphere Info
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere1"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere2"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere3"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere4"));
			//The whitelist uint
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].iswhitelist"));
			//std::cout << "point_lightArray[" + std::to_string(i) + "].dropoff";
		}
		for (int i = 0; i < 2; i++) //Dir Lights
		{
			//access by taking i, multiplying by 2, adding 4 * max point lights, and adding an offset (0 for direction, 1 for color...)
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].direction"));
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].color"));
			
			//AABB info
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp1"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp2"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp3"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp4"));
			//Sphere Info
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere1"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere2"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere3"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere4"));
			//The whitelist uint
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].iswhitelist"));
		}
		
		for (int i = 0; i < 3; i++) //Ambient Lights
		{
			//access by taking i, multiplying by 3, adding 4 * max pointlights + 2 * max dir lights, and adding an offset (0 for position, 1 for color...)
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].position"));
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].color"));
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].range"));
			
			//AABB info
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp1"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp2"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp3"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp4"));
			//Sphere Info
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere1"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere2"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere3"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere4"));
			//The whitelist uint
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].iswhitelist"));
		}
		
		for (int i = 0; i < 5; i++) //Cameralights
		{
			//access by taking i, multiplying by 3, adding 4 * maxpointlights + 2 * maxdirlights + 3 * maxambientlights, and adding an offset (0 for viewproj, 1 for color...)
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].viewproj"));//0
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].position"));//1
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].color"));//2
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].solidColor"));//3
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].range"));//4
			m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].shadows"));//5
			//AABB info
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].AABBp1"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].AABBp2"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].AABBp3"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].AABBp4"));
			//Sphere Info
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].sphere1"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].sphere2"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].sphere3"));
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].sphere4"));
			//The whitelist uint
			m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].iswhitelist"));
		}
		//debug
		// for (size_t i = 0; i < m_LightUniformHandles.size(); i++)
		// {
			// std::cout << "\n DEBUG LIGHTUNIFORM HANDLES: " + std::to_string(m_LightUniformHandles[i]);
		// }
	}
	//Custom Bindings
	if (customMainShaderBinds != nullptr)
		customMainShaderBinds(meshmask, CurrentRenderTarget, RenderTarget_Transparent, CurrentRenderCamera, doFrameBufferChecks, backgroundColor, fogRangeDescriptor); //TODO: Why aren't we passing the shader?
	
	if (CurrentRenderCamera == nullptr)
		glUniform3f(MainShaderUniforms[MAINSHADER_CAMERAPOS], SceneCamera->pos.x, SceneCamera->pos.y, SceneCamera->pos.z);
	else
		glUniform3f(MainShaderUniforms[MAINSHADER_CAMERAPOS], CurrentRenderCamera->pos.x, CurrentRenderCamera->pos.y, CurrentRenderCamera->pos.z);
	
	/*
		MAINSHADER_SKYBOX_CUBEMAP, //The cubemap of the Skybox
		MAINSHADER_BACKGROUND_COLOR, //if a is 0, it uses the skybox cubemap, if a is 1, it uses the background color
		MAINSHADER_FOG_RANGE, //What distance from the camera should fog start at and go to 100% at?
	*/
	
	glUniform1i(MainShaderUniforms[MAINSHADER_SKYBOX_CUBEMAP],2);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX1], 3);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX2], 4);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX3], 5);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX4], 6);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX5], 7);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX6], 8);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX7], 9);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX8], 10);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX9], 11);
	glUniform1i(MainShaderUniforms[MAINSHADER_CAMERATEX10], 12);
	if (SkyBoxCubemap)
		SkyBoxCubemap->Bind(2);
	//glm::vec4 tempBackgroundColor = glm::vec4(0,0,0,0);
	//glm::vec2 tempFogRange = glm::vec2(500,800);
	glUniform4f(MainShaderUniforms[MAINSHADER_BACKGROUND_COLOR], backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	glUniform2f(MainShaderUniforms[MAINSHADER_FOG_RANGE], fogRangeDescriptor.x, fogRangeDescriptor.y);
	
	glUniform1i(MainShaderUniforms[MAINSHADER_DIFFUSE],0);
		
	//Do this regardless of the presence of Skyboxcubemap
	glUniform1i(MainShaderUniforms[MAINSHADER_WORLDAROUNDME], 1);//Cubemap unit 1 is reserved for the cubemap representing the world around the object, for reflections.
	
	//TODO: Change camera reference for sorting lights to the current render target camera (if applicable)

	if (CurrentRenderCamera == nullptr)
		{glUniformMatrix4fv(MainShaderUniforms[MAINSHADER_WORLD2CAMERA], 1, GL_FALSE, &SceneRenderCameraMatrix[0][0]);}
	else
		{glUniformMatrix4fv(MainShaderUniforms[MAINSHADER_WORLD2CAMERA], 1, GL_FALSE, &(CurrentRenderCamera->GetViewProjection()[0][0]));}
	//Bind Directional lights.
	if (DirectionalLights.size() > 0)
	{
		for (int i = 0; i < 2 && i < DirectionalLights.size(); i++)
		if(DirectionalLights[i] && DirectionalLights[i]->shouldRender)
		{
			//access by taking i, multiplying by 2, adding 4 * max point lights, and adding an offset (0 for direction, 1 for color...)
			//m_LightUniformHandles
			glUniform3f(m_LightUniformHandles[i * 2 + 4 * 32 + 0], DirectionalLights[i]->myDirection.x,DirectionalLights[i]->myDirection.y, DirectionalLights[i]->myDirection.z);
			glUniform3f(m_LightUniformHandles[i * 2 + 4 * 32 + 1], DirectionalLights[i]->myColor.x,DirectionalLights[i]->myColor.y, DirectionalLights[i]->myColor.z);
			
			//Bind the Light Clipping Volumes
			
			
			//FORMAT OF m_LightClippingVolumeUniformHandles
			//Per Light:
			//0 - 3 AABBp1-4 (4 vec3s)
			//4-7 Spheres 0-3 (4 vec4s)
			//8 whitelist or Whitelist UINT
			
			
			glUniform3f(m_LightClippingVolumeUniformHandles[32*9 + i*9    ], DirectionalLights[i]->AABBp1.x, DirectionalLights[i]->AABBp1.y, DirectionalLights[i]->AABBp1.z); //AABBp1
			glUniform3f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 1], DirectionalLights[i]->AABBp2.x, DirectionalLights[i]->AABBp2.y, DirectionalLights[i]->AABBp2.z); //AABBp2
			glUniform3f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 2], DirectionalLights[i]->AABBp3.x, DirectionalLights[i]->AABBp3.y, DirectionalLights[i]->AABBp3.z); //AABBp3
			glUniform3f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 3], DirectionalLights[i]->AABBp4.x, DirectionalLights[i]->AABBp4.y, DirectionalLights[i]->AABBp4.z); //AABBp4
			
			glUniform4f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 4], DirectionalLights[i]->sphere1.x, DirectionalLights[i]->sphere1.y, DirectionalLights[i]->sphere1.z, DirectionalLights[i]->sphere1.w); //Sphere 1
			glUniform4f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 5], DirectionalLights[i]->sphere2.x, DirectionalLights[i]->sphere2.y, DirectionalLights[i]->sphere2.z, DirectionalLights[i]->sphere2.w); //Sphere 2
			glUniform4f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 6], DirectionalLights[i]->sphere3.x, DirectionalLights[i]->sphere3.y, DirectionalLights[i]->sphere3.z, DirectionalLights[i]->sphere3.w); //Sphere 3
			glUniform4f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 7], DirectionalLights[i]->sphere4.x, DirectionalLights[i]->sphere4.y, DirectionalLights[i]->sphere4.z, DirectionalLights[i]->sphere4.w); //Sphere 4
			
			glUniform1ui(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 8], DirectionalLights[i]->whitelist?1:0);
			if (i == 1 || i == DirectionalLights.size()-1)
			{
				glUniform1i(MainShaderUniforms[MAINSHADER_NUM_DIRLIGHTS], i + 1);
			}
		}
	} else {
		glUniform1i(MainShaderUniforms[MAINSHADER_NUM_DIRLIGHTS], 0);
	}
	
	
	
	//Do point lights
	if (SimplePointLights.size() > 0){
		PointLight* LightsThisFrame[32]; //Array of pointers
		//Grab the first 32
		size_t howmanypointlightshavewedone = 0;
		long long maxindex = -1;
		for (size_t i = 0; i < SimplePointLights.size() && howmanypointlightshavewedone < 32; i++)
		{
			if(SimplePointLights[i]->shouldRender)
			{
				LightsThisFrame[howmanypointlightshavewedone] = SimplePointLights[i];
				howmanypointlightshavewedone++;
				maxindex = i;
			}
		}
		//maxindex++;
		//Compare all of the rest of them
		if(maxindex > -1 && maxindex < SimplePointLights.size()){
			for (size_t i = maxindex + 1; i < SimplePointLights.size(); i++) //for all of them
			{
				if (i < SimplePointLights.size() && SimplePointLights[i]->shouldRender) //don't bother checking if it shouldn't be rendered.
				{
					//Will never be a nullptr
					PointLight* FarthestLight = LightsThisFrame[0];
					size_t indexofFarthestLight = 0;
					for (size_t j = 0; j < 32; j++)
					{
						if (glm::length2(FarthestLight->pos - SceneCamera->pos) < glm::length2(LightsThisFrame[j]->pos - SceneCamera->pos)) //FarthestLight is closer
						{
							FarthestLight = LightsThisFrame[j]; //Swap!
							//break; //I'm hoping this breaks the inner for loop only. If it doesn't- set j to some massively huge value instead
							indexofFarthestLight = j;
						}
					}
					//FarthestLight
					if (glm::length2(SimplePointLights[i]->pos - SceneCamera->pos) < glm::length2(FarthestLight->pos - SceneCamera->pos)) //FarthestLight is farther away than the light
					{
						LightsThisFrame[indexofFarthestLight] = SimplePointLights[i];
					}
				}
			}
		}
		//Bind the lights
		
		glUniform1i(MainShaderUniforms[MAINSHADER_NUM_POINTLIGHTS], howmanypointlightshavewedone);
		//if(maxindex > -1)
			for (size_t i = 0; i < 32 && i < howmanypointlightshavewedone; i++){
				glUniform3f(m_LightUniformHandles[i*4], LightsThisFrame[i]->pos.x, LightsThisFrame[i]->pos.y, LightsThisFrame[i]->pos.z);//pos
				glUniform3f(m_LightUniformHandles[i*4+1], LightsThisFrame[i]->myColor.x, LightsThisFrame[i]->myColor.y, LightsThisFrame[i]->myColor.z);//color
				glUniform1f(m_LightUniformHandles[i*4+2], LightsThisFrame[i]->range);//range
				glUniform1f(m_LightUniformHandles[i*4+3], LightsThisFrame[i]->dropoff);//dropoff
				//std::cout << "\nBOUND POINT LIGHT " << i;
				
				//Bind the Light Clipping Volumes
			
			
				//FORMAT OF m_LightClippingVolumeUniformHandles
				//Per Light:
				//0 - 3 AABBp1-4 (4 vec3s)
				//4-7 Spheres 0-3 (4 vec4s)
				//8 whitelist or Whitelist UINT
				
				
				glUniform3f(m_LightClippingVolumeUniformHandles[i*9], LightsThisFrame[i]->AABBp1.x, LightsThisFrame[i]->AABBp1.y, LightsThisFrame[i]->AABBp1.z); //AABBp1
				glUniform3f(m_LightClippingVolumeUniformHandles[ 1 + i*9], LightsThisFrame[i]->AABBp2.x, LightsThisFrame[i]->AABBp2.y, LightsThisFrame[i]->AABBp2.z); //AABBp2
				glUniform3f(m_LightClippingVolumeUniformHandles[ 2 + i*9], LightsThisFrame[i]->AABBp3.x, LightsThisFrame[i]->AABBp3.y, LightsThisFrame[i]->AABBp3.z); //AABBp3
				glUniform3f(m_LightClippingVolumeUniformHandles[ 3 + i*9], LightsThisFrame[i]->AABBp4.x, LightsThisFrame[i]->AABBp4.y, LightsThisFrame[i]->AABBp4.z); //AABBp4
				
				glUniform4f(m_LightClippingVolumeUniformHandles[ 4 + i*9], LightsThisFrame[i]->sphere1.x, LightsThisFrame[i]->sphere1.y, LightsThisFrame[i]->sphere1.z, LightsThisFrame[i]->sphere1.w); //Sphere 1
				glUniform4f(m_LightClippingVolumeUniformHandles[ 5 + i*9], LightsThisFrame[i]->sphere2.x, LightsThisFrame[i]->sphere2.y, LightsThisFrame[i]->sphere2.z, LightsThisFrame[i]->sphere2.w); //Sphere 2
				glUniform4f(m_LightClippingVolumeUniformHandles[ 6 + i*9], LightsThisFrame[i]->sphere3.x, LightsThisFrame[i]->sphere3.y, LightsThisFrame[i]->sphere3.z, LightsThisFrame[i]->sphere3.w); //Sphere 3
				glUniform4f(m_LightClippingVolumeUniformHandles[ 7 + i*9], LightsThisFrame[i]->sphere4.x, LightsThisFrame[i]->sphere4.y, LightsThisFrame[i]->sphere4.z, LightsThisFrame[i]->sphere4.w); //Sphere 4
				
				glUniform1ui(m_LightClippingVolumeUniformHandles[i*9 + 8], LightsThisFrame[i]->whitelist?1:0);
			}
		//std::cout << "\n DEBUGGING POINTLIGHT BINDINGS: MAXINDEX " << maxindex << " howmanypointlightshavewedone " << howmanypointlightshavewedone << "!" ;
	} else {
		glUniform1i(MainShaderUniforms[MAINSHADER_NUM_POINTLIGHTS], 0);
	}
	
	//Ambient Lights
	if (AmbientLights.size() > 0){
		AmbientLight* LightsThisFrame[3]; //Array of pointers
		//Grab the first 3
		size_t howmanyAmbientlightshavewedone = 0;
		long long maxindex = -1;
		for (size_t i = 0; i < AmbientLights.size() && howmanyAmbientlightshavewedone < 3; i++)
		{
			if(AmbientLights[i]->shouldRender)
			{
				LightsThisFrame[howmanyAmbientlightshavewedone] = AmbientLights[i];
				howmanyAmbientlightshavewedone++;
				maxindex = i;
			}
		}
		maxindex++;
		//Compare all of the rest of them
		if(maxindex > -1 && maxindex != AmbientLights.size()-1){
			for (size_t i = maxindex; i < AmbientLights.size(); i++) //for all of them
			{
				if (AmbientLights[i]->shouldRender) //don't bother checking if it shouldn't be rendered.
				{
					AmbientLight* FarthestLight = LightsThisFrame[0];
					size_t indexofFarthestLight = 0;
					for (size_t j = 0; j < 3; j++)
					{
						if (glm::length2(FarthestLight->myPos - SceneCamera->pos) < glm::length2(LightsThisFrame[j]->myPos - SceneCamera->pos)) //This one is closer
						{
							FarthestLight = LightsThisFrame[j]; //Swap!
							indexofFarthestLight = j;
						}
					}
					if (glm::length2(FarthestLight->myPos - SceneCamera->pos) > glm::length2(AmbientLights[i]->myPos - SceneCamera->pos)) //This one is closer
						LightsThisFrame[indexofFarthestLight] = AmbientLights[i];
				}
			}
		}
		//Bind the lights
		glUniform1i(MainShaderUniforms[MAINSHADER_NUM_AMBLIGHTS], howmanyAmbientlightshavewedone);
		//if(maxindex > -1)
			for (size_t i = 0; i < 32 && i < howmanyAmbientlightshavewedone; i++){
				 glUniform3f(m_LightUniformHandles[4 * 32 + 2 * 2 + i*3], LightsThisFrame[i]->myPos.x, LightsThisFrame[i]->myPos.y, LightsThisFrame[i]->myPos.z);//pos
				 glUniform3f(m_LightUniformHandles[4 * 32 + 2 * 2 + i*3 +1], LightsThisFrame[i]->myColor.x, LightsThisFrame[i]->myColor.y, LightsThisFrame[i]->myColor.z);//color
				 glUniform1f(m_LightUniformHandles[4 * 32 + 2 * 2 + i*3 +2], LightsThisFrame[i]->myRange);//range
				
				//access by taking i, multiplying by 3, adding 4 * max pointlights + 2 * max dir lights, and adding an offset (0 for position, 1 for color...)
				//m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].position"));
				//m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].color"));
				//m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].range"));
				
				//Bind the Light Clipping Volumes
			
			
				//FORMAT OF m_LightClippingVolumeUniformHandles
				//Per Light:
				//0 - 3 AABBp1-4 (4 vec3s)
				//4-7 Spheres 0-3 (4 vec4s)
				//8 whitelist or Whitelist UINT
				
				
				glUniform3f(m_LightClippingVolumeUniformHandles[34*9 + i*9], LightsThisFrame[i]->AABBp1.x, LightsThisFrame[i]->AABBp1.y, LightsThisFrame[i]->AABBp1.z); //AABBp1
				glUniform3f(m_LightClippingVolumeUniformHandles[34*9 + 1 + i*9], LightsThisFrame[i]->AABBp2.x, LightsThisFrame[i]->AABBp2.y, LightsThisFrame[i]->AABBp2.z); //AABBp2
				glUniform3f(m_LightClippingVolumeUniformHandles[34*9 + 2 + i*9], LightsThisFrame[i]->AABBp3.x, LightsThisFrame[i]->AABBp3.y, LightsThisFrame[i]->AABBp3.z); //AABBp3
				glUniform3f(m_LightClippingVolumeUniformHandles[34*9 + 3 + i*9], LightsThisFrame[i]->AABBp4.x, LightsThisFrame[i]->AABBp4.y, LightsThisFrame[i]->AABBp4.z); //AABBp4
				
				glUniform4f(m_LightClippingVolumeUniformHandles[34*9 + 4 + i*9], LightsThisFrame[i]->sphere1.x, LightsThisFrame[i]->sphere1.y, LightsThisFrame[i]->sphere1.z, LightsThisFrame[i]->sphere1.w); //Sphere 1
				glUniform4f(m_LightClippingVolumeUniformHandles[34*9 + 5 + i*9], LightsThisFrame[i]->sphere2.x, LightsThisFrame[i]->sphere2.y, LightsThisFrame[i]->sphere2.z, LightsThisFrame[i]->sphere2.w); //Sphere 2
				glUniform4f(m_LightClippingVolumeUniformHandles[34*9 + 6 + i*9], LightsThisFrame[i]->sphere3.x, LightsThisFrame[i]->sphere3.y, LightsThisFrame[i]->sphere3.z, LightsThisFrame[i]->sphere3.w); //Sphere 3
				glUniform4f(m_LightClippingVolumeUniformHandles[34*9 + 7 + i*9], LightsThisFrame[i]->sphere4.x, LightsThisFrame[i]->sphere4.y, LightsThisFrame[i]->sphere4.z, LightsThisFrame[i]->sphere4.w); //Sphere 4
				
				glUniform1ui(m_LightClippingVolumeUniformHandles[34*9 + 8 + i*9], LightsThisFrame[i]->whitelist?1:0); //whitelist yes or no
				
			}
		//std::cout << "\n DEBUGGING POINTLIGHT BINDINGS: MAXINDEX " << maxindex << " howmanypointlightshavewedone " << howmanypointlightshavewedone << "!" ;
	} else {
		glUniform1i(MainShaderUniforms[MAINSHADER_NUM_AMBLIGHTS], 0);
	}
	
	//Camera Lights
	if (CameraLights.size() > 0){
		int camLightsRegistered = 0;
		size_t currindex = 0;
		//Replace with a system that sorts by distance later
		while (currindex < CameraLights.size() && camLightsRegistered < 5)
		{
			if (CameraLights[currindex] && CameraLights[currindex]->shouldRender)
			{
				//std::cout << "\nBindingCameraLight " << camLightsRegistered;
				//bind
				//grab all the things we need m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + i * 4] //It now has 6...
				CameraLights[currindex]->BindtoUniformCameraLight(
					m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 6],
					m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 6 +1],
					m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 6 +2],
					m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 6 +3],
					m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 6 +4],
					m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 6 +5],
					3+camLightsRegistered
				);
				
				camLightsRegistered++;
			}
			
			currindex++;
		}
		glUniform1i(MainShaderUniforms[MAINSHADER_NUM_CAMLIGHTS], camLightsRegistered);
	} else {
		glUniform1i(MainShaderUniforms[MAINSHADER_NUM_CAMLIGHTS], 0);
	}
	// glEnable(GL_CULL_FACE);
	//Now that we have the shader stuff set up, let's get to rendering!
	glEnableVertexAttribArray(0); //Position
	glEnableVertexAttribArray(1); //Texture
	glEnableVertexAttribArray(2); //Normal
	glEnableVertexAttribArray(3); //Color
	//Render Opaque Objects. Note we configured depth masking and blending for this pass much earlier on.
		if (Meshes.size() > 0) //if there are any
			for (size_t i = 0; i < Meshes.size(); i++) //for all of them
				if (Meshes[i]) //don't call methods on nullptrs
				{
					// unsigned int flagerinos = Meshes[i]->getFlags(); //Set flags
					// glUniform1ui(MainShaderUniforms[MAINSHADER_RENDERFLAGS], flagerinos); //Set flags on GPU
					Meshes[i]->DrawInstancesPhong(
						MainShaderUniforms[MAINSHADER_MODEL2WORLD], 		//Model->World transformation matrix
						MainShaderUniforms[MAINSHADER_RENDERFLAGS],
						MainShaderUniforms[MAINSHADER_SPECREFLECTIVITY], 	//Specular reflective material component
						MainShaderUniforms[MAINSHADER_SPECDAMP], 		//Specular dampening material component
						MainShaderUniforms[MAINSHADER_DIFFUSIVITY],   //Diffusivity. Reaction to diffuse light.
						MainShaderUniforms[MAINSHADER_EMISSIVITY],
						MainShaderUniforms[MAINSHADER_ENABLE_CUBEMAP_REFLECTIONS],
						MainShaderUniforms[MAINSHADER_ENABLE_CUBEMAP_DIFFUSION],
						MainShaderUniforms[MAINSHADER_ENABLE_TRANSPARENCY],
						MainShaderUniforms[MAINSHADER_IS_INSTANCED],
						false,		//NOT transparent
						(CurrentRenderTarget != nullptr)?true:false,		//is it to render target?
						meshmask,
						true, //Phong?
						true, //Do not use maps
						false //Force non-instanced
					);
				}
	//Prep for rendering Transparent Objects
		
	//IF TEST FOR IF WE HAVE THE COMPOSITION SHADER (DON'T ATTEMPT TO DO TRANSPARENCY UNLESS WE HAVE IT)
	if (CompositionShader && doTransparency)
	{
		
			glDepthMask(GL_FALSE); // Transparent objects aren't depth tested against each other but are against opaque objects in the scene.
			glDisable(GL_CULL_FACE); 
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			//Swap these two lines along with vec3(0) and vec3(1) in FORWARD_MAINSHADER.FS to achieve either multiplicative or additive transparency. Both are wrong, but you might prefer one over another.
			if(TransparencyFBO != nullptr) // JUST TO MAKE SURE...
				TransparencyFBO->BindRenderTarget();
				//CLEAR IT BEFORE WE SET THE BLEND FUNCTIONS
				/*
					DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
				}
				glDrawBuffers(numcolorattachments, DrawBuffers);
				*/
				static GLenum DrawBuffers0[1] = {GL_COLOR_ATTACHMENT0};
				static GLenum DrawBuffers1[1] = {GL_COLOR_ATTACHMENT1};
				//GLenum DrawBuffers2[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
				glDrawBuffers(1, DrawBuffers0);
				FBO::clearTexture(0, 0, 0, 0);
				glDrawBuffers(1, DrawBuffers1);
				FBO::clearTexture(1, 1, 1, 1);
				TransparencyFBO->BindDrawBuffers(); //Do the right thing!
			// communism = glGetError(); //Ensure there are no errors listed before we start.
			
			glEnable(GL_BLEND);
			glBlendFunci(0, GL_ONE, GL_ONE); //Additive blending
			glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); //Uh... I don't know
			
			// communism = glGetError(); //Ensure there are no errors listed before we start.
			// if (communism != GL_NO_ERROR) //if communism has made an error (which is pretty typical)
			// {
				// std::cout<<"\n OpenGL reports an ERROR!";
				// if (communism == GL_INVALID_ENUM)
					// std::cout<<"\n Invalid enum.";
				// if (communism == GL_INVALID_OPERATION)
					// std::cout<<"\n Invalid operation.";
				// if (communism == GL_INVALID_FRAMEBUFFER_OPERATION)
					// std::cout <<"\n Invalid Framebuffer Operation.";
				// if (communism == GL_OUT_OF_MEMORY)
				// {
					// std::cout <<"\n Out of memory. You've really done it now. I'm so angry, i'm going to close the program. ARE YOU HAPPY NOW, DAVE?!?!";
					// std::abort();
				// }
			// }
			// glBlendFunc(GL_ZERO, GL_SRC_COLOR); //Multiplicative Blending
		//Render Transparent Objects
			if (Meshes.size() > 0) //if there are any
				for (size_t i = 0; i < Meshes.size(); i++) //for all of them
					if (Meshes[i]) //don't call methods on nullptrs
					{
						unsigned int flagerinos = Meshes[i]->getFlags(); //Set flags
						glUniform1ui(MainShaderUniforms[MAINSHADER_RENDERFLAGS], flagerinos); //Set flags on GPU
						Meshes[i]->DrawInstancesPhong(
							MainShaderUniforms[MAINSHADER_MODEL2WORLD], 		//Model->World transformation matrix
							MainShaderUniforms[MAINSHADER_RENDERFLAGS],
							MainShaderUniforms[MAINSHADER_SPECREFLECTIVITY], 	//Specular reflective material component
							MainShaderUniforms[MAINSHADER_SPECDAMP], 		//Specular dampening material component
							MainShaderUniforms[MAINSHADER_DIFFUSIVITY],   //Diffusivity. Reaction to diffuse light.
							MainShaderUniforms[MAINSHADER_EMISSIVITY],
							MainShaderUniforms[MAINSHADER_ENABLE_CUBEMAP_REFLECTIONS],
							MainShaderUniforms[MAINSHADER_ENABLE_CUBEMAP_DIFFUSION],
							MainShaderUniforms[MAINSHADER_ENABLE_TRANSPARENCY],
							MainShaderUniforms[MAINSHADER_IS_INSTANCED],
							true,		//Transparent.
							(CurrentRenderTarget != nullptr)?true:false,		//is it to render target?
							meshmask,
							true,
							true,
							false
						);
					}
		glDisableVertexAttribArray(0); //Position. NOTE: don't disable if we want to do screenquads later.
		glDisableVertexAttribArray(1); //Texture
		glDisableVertexAttribArray(2); //Normal
		glDisableVertexAttribArray(3); //Color
		
		// This is the part where we composite to the screen with the compositionshader
		if (customRenderingAfterTransparentObjectRendering)
			customRenderingAfterTransparentObjectRendering(meshmask, CurrentRenderTarget, RenderTarget_Transparent, CurrentRenderCamera, doFrameBufferChecks, backgroundColor, fogRangeDescriptor);
		//INTEL GPU FIX
		if(doFrameBufferChecks)
			while(true)
			{
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
					break;
			}
		CompositionShader->Bind();
		if (HasntRunYet_CompositionShader)
		{
			/*
			enum{
					OIT_COMP_1,
					OIT_COMP_2,
					OIT_NUM_OIT_COMP_UNIFORMS
				};
				GLuint CompositionShaderUniforms[OIT_NUM_OIT_COMP_UNIFORMS];
			*/
			CompositionShaderUniforms[OIT_COMP_1] = CompositionShader->GetUniformLocation("diffuse");
			CompositionShaderUniforms[OIT_COMP_2] = CompositionShader->GetUniformLocation("diffuse2");
			HasntRunYet_CompositionShader = false;
		}
		glUniform1i(CompositionShaderUniforms[OIT_COMP_1], 0);
		glUniform1i(CompositionShaderUniforms[OIT_COMP_2], 1);
		MainFBO->BindRenderTarget();
		TransparencyFBO->BindasTexture(0,0);
		TransparencyFBO->BindasTexture(1,1);
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
		ScreenquadtoFBO(CompositionShader);
	
	}else{
		glDisableVertexAttribArray(0); //Position. NOTE: don't disable if we want to do screenquads later.
		glDisableVertexAttribArray(1); //Texture
		glDisableVertexAttribArray(2); //Normal
		glDisableVertexAttribArray(3); //Color
	}
	
	if (CurrentRenderTarget == nullptr) //This part is just for displaying the FBO to the screen... Not used for the FBO stuff.
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		ShowTextureShader->Bind();
		//INTEL GPU FIX
		if(doFrameBufferChecks)
		while(true)
		{
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
				break;
		}
		FBO::unBindRenderTarget(width, height);
		FBO::clearTexture(0,0,0,0);
		FboArray[FORWARD_BUFFER1]->BindasTexture(0, 0);
		// TransparencyFBO->BindasTexture(0,0);
		ScreenquadtoFBO(ShowTextureShader);
	}
	if (CurrentRenderTarget == nullptr)
	{nextRenderPipelineCallWillBeAfterVsync = true;}
	else
	{
		if(doFrameBufferChecks)
			while(true)
			{
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
					break;
			}
		nextRenderPipelineCallWillBeAfterVsync = false;
	}
	
} //eof drawPipeline









//Constructor and Destructor
GkScene::GkScene(unsigned int newwidth, unsigned int newheight, float approxFactor)
{
	//NOTE: we will be glEnabling and glDisabling alot in the draw function when we get to stuff like transparency.
	glEnable(GL_CULL_FACE); //Enable culling faces
	glEnable(GL_DEPTH_TEST); //test fragment depth when rendering.
	glCullFace(GL_BACK); //cull faces with clockwise winding
	glEnable(GL_ARB_conservative_depth); //conservative depth.
		// glEnable(GL_EXT_conservative_depth); //Also needed?
		glGetError(); //the arb conservative depth thing (I think) creates an error, so i'm just voiding it with this...
	//Does the FBO stuff and of course sets the width and height
	resizeSceneViewport(newwidth, newheight, approxFactor); //setup the viewport and do the FBO dimensions
	
	if (m_screenquad_Mesh == nullptr)
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
			m_screenquad_Mesh = new Mesh(screenquad_IndexedModel, false, true, true);
			//std::cout<<"\n we got the mesh made";
		//Make the camera needed to render the screenquad
			if(ScreenquadCamera){
				delete ScreenquadCamera;
				ScreenquadCamera = nullptr;
			}
			ScreenquadCamera = new Camera();
			ScreenquadCamera->buildOrthogonal(-1, 1, -1, 1, 0, 1);
			ScreenquadCamera->pos = glm::vec3(0,0,0);
			ScreenquadCamera->forward = glm::vec3(0,0,1);
			ScreenquadCamera->up = glm::vec3(0,1,0);
			Screenquad_CameraMatrix = ScreenquadCamera->GetViewProjection();
	}
	
	if (m_skybox_Mesh == nullptr)//if this has not been instantiated
	{
		//std::cout << "\n Making the skybox mesh";
			const GLfloat canada[] = { //Team Canada 
				//0-5
				-1,  1, -1,
				-1, -1, -1,
				1, -1, -1,
				
				 1, -1, -1,
				 1,  1, -1,
				-1,  1, -1,
				//6-11
				-1, -1,  1,
				-1, -1, -1,
				-1,  1, -1,
				
				-1,  1, -1,
				-1,  1,  1,
				-1, -1,  1,
				//12-17
				 1, -1, -1,
				 1, -1,  1,
				 1,  1,  1,
				 
				 1,  1,  1,
				 1,  1, -1,
				 1, -1, -1,
				//18-23
				-1, -1,  1,
				-1,  1,  1,
				 1,  1,  1,
				 
				 1,  1,  1,
				 1, -1,  1,
				-1, -1,  1,
				//24-29
				-1,  1, -1,
				 1,  1, -1,
				 1,  1,  1,
				 
				 1,  1,  1,
				-1,  1,  1,
				-1,  1, -1,
				//30-35
				-1, -1, -1,
				-1, -1,  1,
				 1, -1, -1,
				 
				 1, -1, -1,
				-1, -1,  1,
				 1, -1,  1
			}; //36 vertices. 
		for (unsigned int i = 0; i < 36; i++)
		{
			skyboxModel.positions.push_back(
				glm::vec3(canada[i*3],canada[i*3+1],canada[i*3+2])
			);
			skyboxModel.indices.push_back(i);
			skyboxModel.texCoords.push_back(glm::vec2(0,0));
			skyboxModel.colors.push_back(glm::vec3(0,0,0));
			skyboxModel.normals.push_back(glm::vec3(canada[i*3],canada[i*3+1],canada[i*3+2]));
		}
		//std::cout << "\nPushed back all vert data.";
		skyboxModel.myFileName = "/aux/DEBUG_FILENAME_SKYBOX_MESH";
		m_skybox_Mesh = new Mesh(skyboxModel,false, true, true);
	}
}

GkScene::~GkScene()
{
	if (FboArray.size() > 0){ //if there are FBOs...
		for (size_t i = 0; i<FboArray.size(); i++) //for each
		{
			if (FboArray[i]) //if it is not null
			{
				delete FboArray[i]; FboArray[i] = nullptr; //delete it and set it to nullptr. the opengl de-allocation stuff is handled by the destructor which is supposedly called by delete
			}
		}
		while (FboArray.size() > 0) //while there are still nullptrs in the fboarray...
		{
			FboArray.erase( //erase one
				FboArray.begin() + FboArray.size()-1
			); //this line is weird as fuck. Blame GCC...
		}
	}
	
	
	
	if (m_screenquad_Mesh)
		delete m_screenquad_Mesh; //Calls the destructor... or so I hear...
	if (ScreenquadCamera) //Thanks, Doctor Memory!
		delete ScreenquadCamera;
	if (m_skybox_Mesh)
		delete m_skybox_Mesh;
	
}

}; //Eof Namespace
