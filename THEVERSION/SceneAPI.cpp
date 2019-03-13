#include "SceneAPI.h"
#include "geklminrender.h"
//(C) DMHSW 2018 All Rights Reserved


namespace GeklminRender {
	
//Render shadows to rendertarget (Just a depth buffer render)
void GkScene::drawShadowPipeline(int meshmask, FBO* CurrentRenderTarget, Camera* CurrentRenderCamera, bool doFrameBufferChecks){
	//Used for error checking
	GLenum communism; //communism is a mistake
	if (!CurrentRenderTarget)
		{
			std::cout<<"\nMust have render target to render shadows";
			return;
		}
	if (nextRenderPipelineCallWillBeAfterVsync) //What this does: Prevent the user from accidentally increasing lag exponentially by reshaping a mesh while it's rendering. Also, some other stuff maybe. TODO: Check it out later
	{
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
	
	
	
	
	
	if (!ShadowOpaqueMainShader || !ShowTextureShader /*|| !CompositionShader*/){ //if you don't have one of these, there's no chance we can render shadows
		std::cout<<"\nYou got kicked out boi 3";
		return; //gtfo
	}
	
	
	
	
	
	
	
	
	
	glEnable(GL_CULL_FACE); //Enable culling faces
	glCullFace(GL_BACK); //cull faces with clockwise winding
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	FBO* MainFBO = nullptr;
	FBO* TransparencyFBO = nullptr;
	
		MainFBO = CurrentRenderTarget;
		//~ TransparencyFBO = RenderTarget_Transparent;
		//CurrentRenderTarget->BindRenderTarget();
	
	MainFBO->BindRenderTarget();
	FBO::clearTexture(1,1,1,1);
	ShadowOpaqueMainShader->Bind(); //Bind the shader!
	
	
	//Runs whenever the window is resized or a shader is reassigned, so that we only need to get uniform locations once. It's not optimized fully yet...
	if(HasntRunYet_Shadows){
		HasntRunYet_Shadows = false;
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_WORLD2CAMERA] = ShadowOpaqueMainShader->GetUniformLocation("World2Camera"); //World --> NDC
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_MODEL2WORLD] = ShadowOpaqueMainShader->GetUniformLocation("Model2World"); //Model --> World
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERAPOS] = ShadowOpaqueMainShader->GetUniformLocation("CameraPos");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_ENABLE_TRANSPARENCY] = ShadowOpaqueMainShader->GetUniformLocation("EnableTransparency");
		MainShaderUniforms_SHADOWTEMP[MAINSHADER_IS_INSTANCED] = ShadowOpaqueMainShader->GetUniformLocation("is_instanced");
	}
	
	
	glUniform3f(MainShaderUniforms_SHADOWTEMP[MAINSHADER_CAMERAPOS], CurrentRenderCamera->pos.x, CurrentRenderCamera->pos.y, CurrentRenderCamera->pos.z);

	
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
	
	glDisableVertexAttribArray(0); //Position
	glDisableVertexAttribArray(1); //Texture
	glDisableVertexAttribArray(2); //Normal
	glDisableVertexAttribArray(3); //Color
	
	
	
	{
		if(doFrameBufferChecks)
			while(true)
			{
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
					break;
			}
		nextRenderPipelineCallWillBeAfterVsync = false;
	}
	
} //eof drawShadowPipeline




void GkScene::drawPipeline(int meshmask, FBO* CurrentRenderTarget, FBO* RenderTarget_Transparent, Camera* CurrentRenderCamera, bool doFrameBufferChecks, glm::vec4 backgroundColor, glm::vec2 fogRangeDescriptor, bool Render_Transparent){ //Approaches spaghetti levels of unreadability
	//Used for error checking
	GLenum communism;
	bool doTransparency = !(CurrentRenderTarget && !RenderTarget_Transparent); // new code
	//old code
	//bool doTransparency = true;
	//if (CurrentRenderTarget && !RenderTarget_Transparent){ 
	//	doTransparency = false;
	//}
	
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
	
	
	
	
	
	if (!SceneCamera || !MainShader || !MainShaderUniforms || !ShowTextureShader /*|| !CompositionShader*/){ //if one is not present, no chance
		std::cout<<"\nYou got kicked out boi 3";
		return; //gtfo
	}
	
	//Setup Camera mat4s so that we can send in the addresses.
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

	
	
	
	
	
	
	
	
	//CONFIGURE BLENDING AND CULLING FOR SKYBOX RENDERING
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
	
		
	
	

		SkyBoxCubemap->Bind(1); //Bind to 1
		glDepthMask(GL_FALSE); //We want it to appear infinitely far away

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
		//m_LightUniformHandles no longer necessary because we use a UBO
		//~ if (m_LightUniformHandles.size() > 0)
			//~ m_LightUniformHandles.clear();
		//~ if (m_LightClippingVolumeUniformHandles.size() > 0)
			//~ m_LightClippingVolumeUniformHandles.clear();
		//This code is no longer used since all lighting data except the CameraTex stuff is in the light_data UBO
		//~ for (int i = 0; i < 32; i++) //Point lights
		//~ {
			//~ //access by taking i, multiplying by 4 and adding an offset (0 for position, 1 for color...)
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].position"));
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].color"));
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].range"));
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].dropoff"));
			//~ //AABB info
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp1"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp2"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp3"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].AABBp4"));
			//~ //Sphere Info
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere1"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere2"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere3"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].sphere4"));
			//~ //The whitelist uint
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("point_lightArray[" + std::to_string(i) + "].iswhitelist"));
			//~ //std::cout << "point_lightArray[" + std::to_string(i) + "].dropoff";
		//~ }
		//~ for (int i = 0; i < 2; i++) //Dir Lights
		//~ {
			//~ //access by taking i, multiplying by 2, adding 4 * max point lights, and adding an offset (0 for direction, 1 for color...)
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].direction"));
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].color"));
			
			//~ //AABB info
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp1"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp2"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp3"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].AABBp4"));
			//~ //Sphere Info
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere1"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere2"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere3"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].sphere4"));
			//~ //The whitelist uint
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("dir_lightArray[" + std::to_string(i) + "].iswhitelist"));
		//~ }
		
		//~ for (int i = 0; i < 3; i++) //Ambient Lights
		//~ {
			//~ //access by taking i, multiplying by 3, adding 4 * max pointlights + 2 * max dir lights, and adding an offset (0 for position, 1 for color...)
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].position"));
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].color"));
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].range"));
			
			//~ //AABB info
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp1"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp2"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp3"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].AABBp4"));
			//~ //Sphere Info
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere1"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere2"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere3"));
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].sphere4"));
			//~ //The whitelist uint
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("amb_lightArray[" + std::to_string(i) + "].iswhitelist"));
		//~ }
		
		//~ for (int i = 0; i < 5; i++) //Cameralights
		//~ {
			//~ //access by taking i, multiplying by 3, adding 4 * maxpointlights + 2 * maxdirlights + 3 * maxambientlights, and adding an offset (0 for viewproj, 1 for color...)
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].viewproj"));//0
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].position"));//1
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].color"));//2
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].solidColor"));//3
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].range"));//4
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].shadows"));//5
			//~ m_LightUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].radii"));//6
			//~ //The whitelist uint
			//~ m_LightClippingVolumeUniformHandles.push_back(MainShader->GetUniformLocation("camera_lightArray[" + std::to_string(i) + "].iswhitelist"));
		//~ }
		//~ //debug
		//~ // for (size_t i = 0; i < m_LightUniformHandles.size(); i++)
		//~ // {
			//~ // std::cout << "\n DEBUG LIGHTUNIFORM HANDLES: " + std::to_string(m_LightUniformHandles[i]);
		//~ // }
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
	
	//Make the code significantly prettier by packing all the light binding stuff into a function
	OrganizeUBOforUpload();
	
	
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
	if (CompositionShader && doTransparency && Render_Transparent)
	{
		
			glDepthMask(GL_FALSE); // Transparent objects aren't depth tested against each other but are against opaque objects in the scene.
			glDisable(GL_CULL_FACE); 
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			
			if(TransparencyFBO != nullptr) // JUST TO MAKE SURE...
				TransparencyFBO->BindRenderTarget();
				static GLenum DrawBuffers0[1] = {GL_COLOR_ATTACHMENT0};
				static GLenum DrawBuffers1[1] = {GL_COLOR_ATTACHMENT1};
				//GLenum DrawBuffers2[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
				glDrawBuffers(1, DrawBuffers0);
				FBO::clearTexture(0, 0, 0, 0);
				glDrawBuffers(1, DrawBuffers1);
				FBO::clearTexture(1, 1, 1, 1);
				TransparencyFBO->BindDrawBuffers(); //Do the right thing!
			
			glEnable(GL_BLEND);
			glBlendFunci(0, GL_ONE, GL_ONE); //Additive blending
			glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); //Uh... I don't know
			
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
		//Do additive blending
		if(Render_Transparent){
			glDepthMask(GL_FALSE); // Transparent objects aren't depth tested against each other but are against opaque objects in the scene.
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); //Additive
			glDisable(GL_CULL_FACE); 
			//~ glBlendFunc(GL_ZERO, GL_SRC_COLOR); //Multiplicative Blending
			
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
		}
		
		if (customRenderingAfterTransparentObjectRendering)
			customRenderingAfterTransparentObjectRendering(meshmask, CurrentRenderTarget, RenderTarget_Transparent, CurrentRenderCamera, doFrameBufferChecks, backgroundColor, fogRangeDescriptor);
		
		glDisableVertexAttribArray(0); //Position
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




void GkScene::OrganizeUBOforUpload(){
	//Do all the stuff to sort the lights out and place them in their spots
	GLenum communism;
	float* temp_ptr;
	
	
	//Setting up the UBO
	/*
	 * 
	unsigned char LightingDataUBOData[16000];//maximum of 16k in size to be compatible with old systems
	GLuint LightingDataUBO = 0; //handle for the lighting data UBO
	bool haveCreatedLightingUBO = false; //Have we created it? determines whether or not we destroy it and whether or not we have to make it again
	 * 
	 * */
	if(!haveCreatedLightingUBO){
		glGetError();
		for(int i = 0; i < 16000; i++)
		{
			LightingDataUBOData[i] = 0;
		}
		glGenBuffers(1, &LightingDataUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, LightingDataUBO);//Work with this UBO
		glBufferData(GL_UNIFORM_BUFFER, 8500, LightingDataUBOData, GL_DYNAMIC_DRAW); //Use dynamic draw
		//from now on we will use glBufferSubData to update the VBO
		haveCreatedLightingUBO = true;
		 
	}
	if(!haveFoundLocationOfLightingUBO){
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, LightingDataUBO);
		LightingDataUBOLocation = MainShader->GetUniformBlockIndex("light_data");
		MainShader->UniformBlockBinding(LightingDataUBOLocation, 0);
		haveFoundLocationOfLightingUBO = true;
	}
	
	
	if (communism != GL_NO_ERROR) //if communism has made an error (which is pretty typical)
	{
		std::cout<<"\n OpenGL reports an ERROR! (AFTER CREATION)";
		if (communism == GL_INVALID_ENUM)
			std::cout<<"\n Invalid enum.";
		if (communism == GL_INVALID_OPERATION)
			std::cout<<"\n Invalid operation.";
		if (communism == GL_INVALID_FRAMEBUFFER_OPERATION)
			std::cout <<"\n Invalid Framebuffer Operation.";
		if (communism == GL_OUT_OF_MEMORY)
		{
			std::cout <<"\n Out of memory. You've really done it now. I'm so angry, i'm going to close the program. ARE YOU HAPPY NOW, DAVE?!?!";
			std::abort();
		}
	}
	
	if (DirectionalLights.size() > 0)
	{
		for (int i = 0; i < 2 && i < DirectionalLights.size(); i++)
		if(DirectionalLights[i] && DirectionalLights[i]->shouldRender)
		{
			//access by taking i, multiplying by 2, adding 4 * max point lights, and adding an offset (0 for direction, 1 for color...)
			//m_LightUniformHandles
			//glUniform3f(m_LightUniformHandles[i * 2 + 4 * 32 + 0], DirectionalLights[i]->myDirection.x,DirectionalLights[i]->myDirection.y, DirectionalLights[i]->myDirection.z); //STARTS AT 0
			temp_ptr = (float*)(&(LightingDataUBOData[0 + i * 16])); //dirlight direction (STARTS AT 0, VEC4S)
			memcpy(temp_ptr, &(DirectionalLights[i]->myDirection[0]), 12); //copy 3 floats of size 4 each 
			
			//glUniform3f(m_LightUniformHandles[i * 2 + 4 * 32 + 1], DirectionalLights[i]->myColor.x,DirectionalLights[i]->myColor.y, DirectionalLights[i]->myColor.z);
			temp_ptr = (float*)(&(LightingDataUBOData[32 + i * 16])); //dirlight color
			memcpy(temp_ptr, &(DirectionalLights[i]->myColor[0]), 12); //copy 3 floats of size 4 each 
			//Bind the Light Clipping Volumes
			
			
			//FORMAT OF m_LightClippingVolumeUniformHandles
			//Per Light:
			//0 - 3 AABBp1-4 (4 vec3s)
			//4-7 Spheres 0-3 (4 vec4s)
			//8 whitelist or Whitelist UINT
			
			
			//glUniform4f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 4], DirectionalLights[i]->sphere1.x, DirectionalLights[i]->sphere1.y, DirectionalLights[i]->sphere1.z, DirectionalLights[i]->sphere1.w); //Sphere 1
			temp_ptr = (float*)(&(LightingDataUBOData[64 + i * 16])); //dirlight sphere1
			memcpy(temp_ptr, &(DirectionalLights[i]->sphere1[0]), 16); //copy 4 floats of size 4 each 
			//glUniform4f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 5], DirectionalLights[i]->sphere2.x, DirectionalLights[i]->sphere2.y, DirectionalLights[i]->sphere2.z, DirectionalLights[i]->sphere2.w); //Sphere 2
			temp_ptr = (float*)(&(LightingDataUBOData[32 * 3 + i * 16])); //dirlight sphere2
			memcpy(temp_ptr, &(DirectionalLights[i]->sphere2[0]), 16); //copy 4 floats of size 4 each 
			//glUniform4f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 6], DirectionalLights[i]->sphere3.x, DirectionalLights[i]->sphere3.y, DirectionalLights[i]->sphere3.z, DirectionalLights[i]->sphere3.w); //Sphere 3
			temp_ptr = (float*)(&(LightingDataUBOData[32 * 4 + i * 16])); //dirlight sphere3
			memcpy(temp_ptr, &(DirectionalLights[i]->sphere3[0]), 16); //copy 4 floats of size 4 each 
			//glUniform4f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 7], DirectionalLights[i]->sphere4.x, DirectionalLights[i]->sphere4.y, DirectionalLights[i]->sphere4.z, DirectionalLights[i]->sphere4.w); //Sphere 4
			temp_ptr = (float*)(&(LightingDataUBOData[32 * 5 + i * 16])); //dirlight sphere4
			memcpy(temp_ptr, &(DirectionalLights[i]->sphere4[0]), 16); //copy 4 floats of size 4 each 
			
			
			//glUniform3f(m_LightClippingVolumeUniformHandles[32*9 + i*9    ], DirectionalLights[i]->AABBp1.x, DirectionalLights[i]->AABBp1.y, DirectionalLights[i]->AABBp1.z); //AABBp1
			temp_ptr = (float*)(&(LightingDataUBOData[32 * 6 + i * 16])); //dirlight AABBp1
			memcpy(temp_ptr, &(DirectionalLights[i]->AABBp1[0]), 12); //copy 3 floats of size 4 each 
			//glUniform3f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 1], DirectionalLights[i]->AABBp2.x, DirectionalLights[i]->AABBp2.y, DirectionalLights[i]->AABBp2.z); //AABBp2
			temp_ptr = (float*)(&(LightingDataUBOData[32 * 7 + i * 16])); //dirlight AABBp2
			memcpy(temp_ptr, &(DirectionalLights[i]->AABBp2[0]), 12); //copy 3 floats of size 4 each 
			//glUniform3f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 2], DirectionalLights[i]->AABBp3.x, DirectionalLights[i]->AABBp3.y, DirectionalLights[i]->AABBp3.z); //AABBp3
			temp_ptr = (float*)(&(LightingDataUBOData[32 * 8 + i * 16])); //dirlight AABBp3
			memcpy(temp_ptr, &(DirectionalLights[i]->AABBp3[0]), 12); //copy 3 floats of size 4 each 
			//glUniform3f(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 3], DirectionalLights[i]->AABBp4.x, DirectionalLights[i]->AABBp4.y, DirectionalLights[i]->AABBp4.z); //AABBp4
			temp_ptr = (float*)(&(LightingDataUBOData[32 * 9 + i * 16])); //dirlight AABBp4
			memcpy(temp_ptr, &(DirectionalLights[i]->AABBp4[0]), 12); //copy 3 floats of size 4 each 
			
			
			//glUniform1ui(m_LightClippingVolumeUniformHandles[32*9 + i*9 + 8], DirectionalLights[i]->whitelist?1:0);
			if (i == 1 || i == DirectionalLights.size()-1)
			{
				glUniform1i(MainShaderUniforms[MAINSHADER_NUM_DIRLIGHTS], i + 1);
				*((int*)(&(LightingDataUBOData[8388]))) = i + 1; //num dir lights 
			}
		}
	} else {
		//glUniform1i(MainShaderUniforms[MAINSHADER_NUM_DIRLIGHTS], 0);
		//Assign the value in the UBO data cpu-side
		*((int*)(&(LightingDataUBOData[8388]))) = 0; //num dir lights
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
						if (glm::length2(FarthestLight->myPos - SceneCamera->pos) < glm::length2(LightsThisFrame[j]->myPos - SceneCamera->pos)) //FarthestLight is closer
						{
							FarthestLight = LightsThisFrame[j]; //Swap!
							//break; //I'm hoping this breaks the inner for loop only. If it doesn't- set j to some massively huge value instead
							indexofFarthestLight = j;
						}
					}
					//FarthestLight
					if (glm::length2(SimplePointLights[i]->myPos - SceneCamera->pos) < glm::length2(FarthestLight->myPos - SceneCamera->pos)) //FarthestLight is farther away than the light
					{
						LightsThisFrame[indexofFarthestLight] = SimplePointLights[i];
					}
				}
			}
		}
		//Bind the lights
		
		//glUniform1i(MainShaderUniforms[MAINSHADER_NUM_POINTLIGHTS], howmanypointlightshavewedone);
		*((int*)(&(LightingDataUBOData[8384]))) = howmanypointlightshavewedone;
		//if(maxindex > -1)
			for (size_t i = 0; i < 32 && i < howmanypointlightshavewedone; i++){
				
				//glUniform3f(m_LightUniformHandles[i*4], LightsThisFrame[i]->pos.x, LightsThisFrame[i]->pos.y, LightsThisFrame[i]->pos.z);//pos
				temp_ptr = (float*)(&(LightingDataUBOData[320 + i*16])); //pointlight position
				memcpy(temp_ptr, &(LightsThisFrame[i]->myPos[0]), 12); //copy 3 floats of size 4 each 
				//glUniform3f(m_LightUniformHandles[i*4+1], LightsThisFrame[i]->myColor.x, LightsThisFrame[i]->myColor.y, LightsThisFrame[i]->myColor.z);//color
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 32 * 1 + 320 + i*16])); //pointlight color
				memcpy(temp_ptr, &(LightsThisFrame[i]->myColor[0]), 12); //copy 3 floats of size 4 each 
				//glUniform1f(m_LightUniformHandles[i*4+2], LightsThisFrame[i]->range);//range
				temp_ptr = (float*)(&(LightingDataUBOData[6480 + i*16])); //pointlight range
				memcpy(temp_ptr, &(LightsThisFrame[i]->range), 4); //copy 1 floats of size 4 each 
				//glUniform1f(m_LightUniformHandles[i*4+3], LightsThisFrame[i]->dropoff);//dropoff
				temp_ptr = (float*)(&(LightingDataUBOData[6992 + i*16])); //pointlight dropoff
				memcpy(temp_ptr, &(LightsThisFrame[i]->dropoff), 4); //copy 1 floats of size 4 each 
				//std::cout << "\nBOUND POINT LIGHT " << i;
				
				//Bind the Light Clipping Volumes
			
			
				//FORMAT OF m_LightClippingVolumeUniformHandles
				//Per Light:
				//0 - 3 AABBp1-4 (4 vec3s)
				//4-7 Spheres 0-3 (4 vec4s)
				//8 whitelist or Whitelist UINT
				
				//TODO: write the memcpy lines for this stuff
				//~ glUniform3f(m_LightClippingVolumeUniformHandles[i*9], LightsThisFrame[i]->AABBp1.x, LightsThisFrame[i]->AABBp1.y, LightsThisFrame[i]->AABBp1.z); //AABBp1
				//~ glUniform3f(m_LightClippingVolumeUniformHandles[ 1 + i*9], LightsThisFrame[i]->AABBp2.x, LightsThisFrame[i]->AABBp2.y, LightsThisFrame[i]->AABBp2.z); //AABBp2
				//~ glUniform3f(m_LightClippingVolumeUniformHandles[ 2 + i*9], LightsThisFrame[i]->AABBp3.x, LightsThisFrame[i]->AABBp3.y, LightsThisFrame[i]->AABBp3.z); //AABBp3
				//~ glUniform3f(m_LightClippingVolumeUniformHandles[ 3 + i*9], LightsThisFrame[i]->AABBp4.x, LightsThisFrame[i]->AABBp4.y, LightsThisFrame[i]->AABBp4.z); //AABBp4
				
				//~ glUniform4f(m_LightClippingVolumeUniformHandles[ 4 + i*9], LightsThisFrame[i]->sphere1.x, LightsThisFrame[i]->sphere1.y, LightsThisFrame[i]->sphere1.z, LightsThisFrame[i]->sphere1.w); //Sphere 1
				//~ glUniform4f(m_LightClippingVolumeUniformHandles[ 5 + i*9], LightsThisFrame[i]->sphere2.x, LightsThisFrame[i]->sphere2.y, LightsThisFrame[i]->sphere2.z, LightsThisFrame[i]->sphere2.w); //Sphere 2
				//~ glUniform4f(m_LightClippingVolumeUniformHandles[ 6 + i*9], LightsThisFrame[i]->sphere3.x, LightsThisFrame[i]->sphere3.y, LightsThisFrame[i]->sphere3.z, LightsThisFrame[i]->sphere3.w); //Sphere 3
				//~ glUniform4f(m_LightClippingVolumeUniformHandles[ 7 + i*9], LightsThisFrame[i]->sphere4.x, LightsThisFrame[i]->sphere4.y, LightsThisFrame[i]->sphere4.z, LightsThisFrame[i]->sphere4.w); //Sphere 4
				
				//~ glUniform1ui(m_LightClippingVolumeUniformHandles[i*9 + 8], LightsThisFrame[i]->whitelist?1:0);
			}
		//std::cout << "\n DEBUGGING POINTLIGHT BINDINGS: MAXINDEX " << maxindex << " howmanypointlightshavewedone " << howmanypointlightshavewedone << "!" ;
	} else {
		//glUniform1i(MainShaderUniforms[MAINSHADER_NUM_POINTLIGHTS], 0);
		*((int*)(&(LightingDataUBOData[8384]))) = 0;
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
		//glUniform1i(MainShaderUniforms[MAINSHADER_NUM_AMBLIGHTS], howmanyAmbientlightshavewedone);
		*((int*)(&(LightingDataUBOData[8392]))) = howmanyAmbientlightshavewedone;
		//if(maxindex > -1)
			for (size_t i = 0; i < 3 && i < howmanyAmbientlightshavewedone; i++){
				 //glUniform3f(m_LightUniformHandles[4 * 32 + 2 * 2 + i*3], LightsThisFrame[i]->myPos.x, LightsThisFrame[i]->myPos.y, LightsThisFrame[i]->myPos.z);//pos
				 temp_ptr = (float*)(&(LightingDataUBOData[5440 + i*16])); //Ambient light position
				 memcpy(temp_ptr, &(LightsThisFrame[i]->myPos[0]), 12); //copy 3 floats of size 4 each 
				 //glUniform3f(m_LightUniformHandles[4 * 32 + 2 * 2 + i*3 +1], LightsThisFrame[i]->myColor.x, LightsThisFrame[i]->myColor.y, LightsThisFrame[i]->myColor.z);//color
				 temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 1 + 5440+ i*16])); //Ambient light color
				 memcpy(temp_ptr, &(LightsThisFrame[i]->myColor[0]), 12); //copy 3 floats of size 4 each 
				 
				 //glUniform1f(m_LightUniformHandles[4 * 32 + 2 * 2 + i*3 +2], LightsThisFrame[i]->myRange);//range
				 temp_ptr = (float*)(&(LightingDataUBOData[7504+ i*16])); //Ambient light color
				 memcpy(temp_ptr, &(LightsThisFrame[i]->myRange), 4); //copy 1 floats of size 4 each 
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
				
				//glUniform4f(m_LightClippingVolumeUniformHandles[34*9 + 4 + i*9], LightsThisFrame[i]->sphere1.x, LightsThisFrame[i]->sphere1.y, LightsThisFrame[i]->sphere1.z, LightsThisFrame[i]->sphere1.w); //Sphere 1
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 2 + 5440+ i*16])); //Ambient light sphere1
				memcpy(temp_ptr, &(LightsThisFrame[i]->sphere1[0]), 16); //copy 4 floats of size 4 each 
				//glUniform4f(m_LightClippingVolumeUniformHandles[34*9 + 5 + i*9], LightsThisFrame[i]->sphere2.x, LightsThisFrame[i]->sphere2.y, LightsThisFrame[i]->sphere2.z, LightsThisFrame[i]->sphere2.w); //Sphere 2
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 3 + 5440+ i*16])); //Ambient light sphere2
				memcpy(temp_ptr, &(LightsThisFrame[i]->sphere2[0]), 16); //copy 4 floats of size 4 each 
				
				//glUniform4f(m_LightClippingVolumeUniformHandles[34*9 + 6 + i*9], LightsThisFrame[i]->sphere3.x, LightsThisFrame[i]->sphere3.y, LightsThisFrame[i]->sphere3.z, LightsThisFrame[i]->sphere3.w); //Sphere 3
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 4 + 5440+ i*16])); //Ambient light sphere3
				memcpy(temp_ptr, &(LightsThisFrame[i]->sphere3[0]), 16); //copy 4 floats of size 4 each 
				
				//glUniform4f(m_LightClippingVolumeUniformHandles[34*9 + 7 + i*9], LightsThisFrame[i]->sphere4.x, LightsThisFrame[i]->sphere4.y, LightsThisFrame[i]->sphere4.z, LightsThisFrame[i]->sphere4.w); //Sphere 4
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 5 + 5440+ i*16])); //Ambient light sphere4
				memcpy(temp_ptr, &(LightsThisFrame[i]->sphere4[0]), 16); //copy 4 floats of size 4 each 
				
				
				//glUniform3f(m_LightClippingVolumeUniformHandles[34*9 + i*9], LightsThisFrame[i]->AABBp1.x, LightsThisFrame[i]->AABBp1.y, LightsThisFrame[i]->AABBp1.z); //AABBp1
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 6 + 5440+ i*16])); //Ambient light AABBp1
				memcpy(temp_ptr, &(LightsThisFrame[i]->AABBp1[0]), 12); //copy 3 floats of size 4 each 
				
				//glUniform3f(m_LightClippingVolumeUniformHandles[34*9 + 1 + i*9], LightsThisFrame[i]->AABBp2.x, LightsThisFrame[i]->AABBp2.y, LightsThisFrame[i]->AABBp2.z); //AABBp2
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 7 + 5440+ i*16])); //Ambient light AABBp2
				memcpy(temp_ptr, &(LightsThisFrame[i]->AABBp2[0]), 12); //copy 3 floats of size 4 each 
				
				//glUniform3f(m_LightClippingVolumeUniformHandles[34*9 + 2 + i*9], LightsThisFrame[i]->AABBp3.x, LightsThisFrame[i]->AABBp3.y, LightsThisFrame[i]->AABBp3.z); //AABBp3
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 8 + 5440+ i*16])); //Ambient light AABBp3
				memcpy(temp_ptr, &(LightsThisFrame[i]->AABBp3[0]), 12); //copy 3 floats of size 4 each 
				
				//glUniform3f(m_LightClippingVolumeUniformHandles[34*9 + 3 + i*9], LightsThisFrame[i]->AABBp4.x, LightsThisFrame[i]->AABBp4.y, LightsThisFrame[i]->AABBp4.z); //AABBp4
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 3 * 9 + 5440+ i*16])); //Ambient light AABBp4
				memcpy(temp_ptr, &(LightsThisFrame[i]->AABBp4[0]), 12); //copy 3 floats of size 4 each 
				
				//glUniform1ui(m_LightClippingVolumeUniformHandles[34*9 + 8 + i*9], LightsThisFrame[i]->whitelist?1:0); //whitelist yes or no
				temp_ptr = (float*)(&(LightingDataUBOData[8336+ i*16])); //Ambient light whitelist var
				*((GLuint*)(temp_ptr)) = LightsThisFrame[i]->whitelist?1:0;
				
			}
		//std::cout << "\n DEBUGGING POINTLIGHT BINDINGS: MAXINDEX " << maxindex << " howmanypointlightshavewedone " << howmanypointlightshavewedone << "!" ;
	} else {
		//glUniform1i(MainShaderUniforms[MAINSHADER_NUM_AMBLIGHTS], 0);
		*((int*)(&(LightingDataUBOData[8392]))) = 0;
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
				const int i = camLightsRegistered; //BAM! 
				//std::cout << "\nBindingCameraLight " << camLightsRegistered;
				//bind
				//grab all the things we need m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + i * 4] //It now has 6...
				//~ CameraLights[currindex]->BindtoUniformCameraLight(
					//~ m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 7],
					//~ m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 7 +1],
					//~ m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 7 +2],
					//~ m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 7 +3],
					//~ m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 7 +4],
					//~ m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 7 +5],
					//~ m_LightUniformHandles[4 * 32 + 2 * 2 + 3*3 + camLightsRegistered * 7 +6], //Radii
					//~ 3+camLightsRegistered
				//~ );
				glm::mat4 m_matrix; //done
				glm::vec3 m_camerapos; //done
				glm::vec3 m_color;  //done
				GLfloat m_SolidColorToggle; //done
				GLfloat m_range; //done
				GLfloat m_shadows; //done
				glm::vec2 m_radii; //done
				CameraLights[currindex]->BindtoUniformBufferCameraLight(&m_matrix,
																		&m_camerapos, 
																		&m_color, 
																		&m_SolidColorToggle, 
																		&m_range, 
																		&m_shadows, 
																		&m_radii, 
																		3 + camLightsRegistered);
				//BINDING INFO
				
				//CAMERAPOS
				temp_ptr = (float*)(&(LightingDataUBOData[             5920 + 16 * i])); //Cameralight Position (4 floats (padded), 4 bytes per float)
				memcpy(temp_ptr, &(m_camerapos[0]), 12); //copy 3 floats of size 4 each
				//COLOR
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 5 * 1 + 5920 + 16 * i])); //Cameralight Color    (4 floats (padded), 4 bytes per float)
				memcpy(temp_ptr, &(m_color[0]), 12); //copy 3 floats of size 4 each
				//RADII
				temp_ptr = (float*)(&(LightingDataUBOData[6400 + 16 * i])); //Cameralight RADII                 (4 floats (padded), 4 bytes per float)
				memcpy(temp_ptr, &(m_radii[0]), 8); //copy 2 floats of size 4 each
				//SOLID COLOR TOGGLE
				temp_ptr = (float*)(&(LightingDataUBOData[7552 + 16 * i])); //Cameralight SOLID COLOR           (4 floats (padded), 4 bytes per float)
				memcpy(temp_ptr, &(m_SolidColorToggle), 4); //copy 1 floats of size 4 each
				//RANGE
				temp_ptr = (float*)(&(LightingDataUBOData[7632 + 16 * i])); //Cameralight RANGE                 (4 floats (padded), 4 bytes per float)
				memcpy(temp_ptr, &(m_range), 4); //copy 1 floats of size 4 each
				//RANGE
				temp_ptr = (float*)(&(LightingDataUBOData[7712 + 16 * i])); //Cameralight SHADOWS               (4 floats (padded), 4 bytes per float)
				memcpy(temp_ptr, &(m_shadows), 4); //copy 1 floats of size 4 each
				//MATRIX
				temp_ptr = (float*)(&(LightingDataUBOData[16 * 5 * 2 + 5920 + 64 * i])); //Cameralight Matrix   (16 floats, 4 bytes per float)
				memcpy(temp_ptr, &(m_matrix[0][0]), 64); //copy 16 floats of size 4 each
				camLightsRegistered++;
			}
			
			currindex++;
		}
		//glUniform1i(MainShaderUniforms[MAINSHADER_NUM_CAMLIGHTS], camLightsRegistered);
		*((int*)(&(LightingDataUBOData[8396]))) = camLightsRegistered; //TODO fix this
	} else {
		//glUniform1i(MainShaderUniforms[MAINSHADER_NUM_CAMLIGHTS], 0);
		*((int*)(&(LightingDataUBOData[8396]))) = 0;
	}
	glGetError();
	
	//Update the buffer
	glBindBuffer(GL_UNIFORM_BUFFER, LightingDataUBO);//Work with this UBO
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 8450, LightingDataUBOData); //Nothing more to do
	communism = glGetError(); //Ensure there are no errors listed before we start.
	if (communism != GL_NO_ERROR) //communism is a mistake
	{
		std::cout<<"\n OpenGL reports an ERROR! (AFTER EVERYTHING)";
		if (communism == GL_INVALID_ENUM)
			std::cout<<"\n Invalid enum.";
		if (communism == GL_INVALID_OPERATION)
			std::cout<<"\n Invalid operation.";
		if (communism == GL_INVALID_FRAMEBUFFER_OPERATION)
			std::cout <<"\n Invalid Framebuffer Operation.";
		if (communism == GL_OUT_OF_MEMORY)
		{
			std::cout <<"\n Out of memory. You've really done it now. I'm so angry, i'm going to close the program. ARE YOU HAPPY NOW, DAVE?!?!";
			std::abort();
		}
	}
}




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
		//Put in some bullshit for the texcoords and normals
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
			const GLfloat canada[] = { //Team Canada: Etho, Pause, Beef
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
	//Handle deleting the lighting UBO
	if(haveCreatedLightingUBO)
		glDeleteBuffers(1, &LightingDataUBO);
}

}; //Eof Namespace
