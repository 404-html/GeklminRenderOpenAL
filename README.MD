Welcome to the GeklminRender OpenAL Repo! This will (probably) be where all continued development will occur.

This is licensed under the MiT license.

You have everything you need to make games and demos.

Changes to GeklminRender will now go here.

# GeklminRender Original Readme
# GeklminRender and GkScene

This is the Official Repository for GeklminRender and the GkScene API.

It is in active development and numerous minor features and additions are still being worked on, although it is ready for you to implement in your own projects. To clarify: The project is in a finished state, but it is being improved regularly.

If you see a file named "All Ascii Characters.txt" that file is pointless, Don't bother copying it. Github won't let me upload a folder without having a file in it, so I use that file. I'm too lazy to delete it, so just ignore it.

It is built with the following dependencies:
* GLM (Everything you need comes in the release as a dependency)
* GLFW (Everything you need comes in the release as a dependency)
* GL3 (if you use different GL headers, then go replace the GL3 and GL3W headers wherever you see them, however I do not believe you need different headers. GL3 headers are provided.)
* GL Libs (Usually DLLs) (They are in every copy of windows... and if you're on linux you know what i mean)
* GL3W (Compiled into the DLL and header provided, dependency)
* stb_image (Compiled into the DLL and headers provided for the entire stb suite, which is all public domain)

## What is it?

This is a rendering API built ontop of OpenGL which works to not only provide an easy, intuitive, and object-oriented way to place your 3D meshes in a 3D world, but also to provide a platform for expanding the capabilities of the renderer.

Technically, the library is seperated into 2 parts:
* GeklminRender: the set of classes and functions which are designed to make OpenGL programming much easier to understand in an object-oriented manner
* GkScene: The renderer built on top of GeklminRender which aims to provide the fast-and-easy 3d rendering solution for you.

Even if you want to write your own Scene class, you can easily modify the GeklminRender classes to add the functionality you want to make your scene class work. I archetyped and/or implemented all major OpenGL features I believe people who use the renderer will be interested in (List of features further down)

GeklminRender is probably all you'll ever need for 99% of your 3d Rendering needs, although some features, while definitely compatible with GeklminRender classes, are not actually implemented.

For instance, GeklminRender's Shader class (Credit where credit is due- Thanks BennyBox) does not support Geometry Shaders, for instance, and adding that functionality is definitely possible, but the Mesh class having its draw function called while a geometry shader was active would have unknown effects. The library is written in such a way, however, that it would be trivial to add your own OpenGL code to operate alongside GeklminRender and GkScene, as long as you know the quirks of GeklminRender and GkScene or sufficiently avoid mucking with things that might cause, for instance, a CPU-GPU Sync point.

There is a raytracing demo in the additional OpenGL demos... if you're interested. Check it out. it is the only secondary demo that comes with a compiled binary example

## How do I build it? There isn't a Makefile!
See the "Compiler_Call.bat" file. If you run that in the context of that folder (on windows) then it will compile, but you have to change the -I bits, or remove them. It just points to your include folder, don't worry about it

First you compile the O files, 
Second,you delete Main.O, 
Third, you link the DLL file, and 
Fourth, you compile the EXE with main.cpp and the DLL as a dependency.

That's how I compile and test the API

For the demos, I just drag and drop the DLLs + h files into the demo's folder (same dir as main.cpp) and run an equivalent of the Step 4 which doesn't use appinfo.res
On linux, I don't use appinfo at all, read Compiler_Call.sh (It doesn't actually execute for some reason, but you can copy-paste the commands into the terminal)


## Why is everything in one folder?

Because I find it easier that way. It's easier to understand too.
MAKEFILES ARE CONFUSING AND HARD!!!
BATCH SCRIPTS ARE EASY!!!

...But I'll still convert to makefiles if you really REALLY want me to
And I'd love to get some help with that...

## How do I implement this in my project?

If you're on windows with MinGW-w64 you'll have the best time.

Windows with MinGW-w64:
1) Download the latest Binary Release
2) Put the DLLs which get linked to in a directory the linker/compiler will "see" when compiling your own code
3) Put the includes in your include folder
4) include the includes in your project
5) Begin programming with GeklminRender
And yes, all the DLLs are commercial-application safe. Your graphical applications are NOT getting slammed with the GPL.

Re-compile the demo to ensure that you 

Linux 64 bit:
1) Clone the repo into a folder somewhere
2) Get the following packages/dependencies through your package manager, or by grabbing them from their respective websites
* GLFW
* OpenGL
* GLM (Header-only library, grab from glm website, or from dependencies folder in repo)
3) Copy the GL3 folder from Dependencies into your include directory
4) Run Compiler_Call.sh as root (inside the directory you cloned the repo into... it has to be root to copy a .so file to /usr/lib
5) Run the demo
6) Get programming!!!


## Can this be compiled on linux?

YES!
It has been tested working on linux (100%) on 10/12/2018 and appears to be run at approximately the same speed as on windows.

## Why is this so shoddily shoved together? 
It's improving every <period of time>

Ask and you shall... probably... recieve


## Before I ask about features, what's the license?

Short Answer: My code is under the MIT License. This library is AY-OKAY for commercial use. Note that everything in the dependencies folder is under its respective licenses. Not everything in the THEVERSION folder is my code either, and is under its respective license. Again, none of it is copyleft or anything, so it is all good for commercial use.

Long Answer: See Licensing.txt

The Correct Answer: See Licensing.txt

IF YOUR LAWYER OR BOSS REALLY NEEDS TO KNOW: See Licensing.txt
(I am not a lawyer this is not legal advice etc. etc.)
See Licensing.txt

## What are the features?
* some of the fastest OpenGL rendering this side of Scandinavia
* Optimized GLSL shaders for GKScene (But you can easily write your own)
* Few shader program binds per frame (Skybox (optional), Mainshader, WBOITCompositionShader, Showtex (literally just screenquads a texture))
* Per-vertex color
* OBJ Support
* Make your own model in-code (demonstrated for making the screenquad model in the scene class, see SceneAPI.cpp-> the constructor) or model loader with the IndexedModel Struct
* Easy to change a mesh's shape between frames
* Texture Instantiation Options (Do you want texture filtering or mipmapping etc. etc.)
* Trivially expanded pipeline
* Starter classes (Planned for use in future versions... Unused in GkScene although theoretically useful (e.g. Cameralight))
* Some protection against programmer error (For instance, it doesnt let you re-instantiate a mesh twice in a frame, which might cause a CPU-GPU Sync Point, or worse a crash) but still be careful!!!
* Very few, probably only one CPU-GPU sync point in a frame
* Render-to-target capabilities with "masking" to prevent meshes or meshinstances you dont want to be drawn in a particular pass over the scene from drawing (Meshmask)
* Weighted Blended Order Independent Transparency for fast and approximately-correct transparency (EDIT: WORKING ON AMD AND NVIDIA, MOST LIKELY WORKING ON INTEL)
* OpenGL 3.3 Compatible
* Skybox Rendering
* Cubemap Reflections
* Easy-to-setup camera class
* Autoface function for your sprites and models that should always face the camera
* Upload Texture from Data (CPU rendering is possible, I even wrote a poorly optimized CPU raytracer)
* (UPCOMING): 3D Text Rendering
* (UPCOMING): Texture Projection for Planar Mirrors
* (UPCOMING): Quick and Simple Planar Mirror reflection rendering
* (UPCOMING): Shadowed and texture-projector Lights (Cameralight)
And it's all in Nice, MODERN OpenGL.

## ...Quirks?
The rendering API was written to avoid the problem of CPU-GPU Sync at almost any cost, but to allow dynamically re-shaping meshes and uploading of instanced model2world matrices, and other gl calls which require passing in pointers to data to be uploaded to the GPU.

To keep compatibility with opengl distributions which fuck with data upload and CPU halting, I limit reshaping meshes to once per frame.

This might change in a future version, but currently, this means that:
* You can't reshape a mesh twice between vsyncs (I know how to solve this but it might be slow)
* Don't delete a mesh or scene after drawing and before vsync


Other quirks not related to Vsync and data upload and CPU-GPU Sync Points:
* Always deregister before deleting. Meshinstances to Meshes, Cubemaps to Meshes, Meshes to GkScene, Shaders to GkScene, Lights to GkScene, etcetera. If you do not, there is a 99% chance of calling methods of invalid pointers. Luckily, I added a function which takes in a void* and/or GLuint to the Scene class which checks the scene class and all meshes for the object in question and deregisters it from anywhere it might be.
* Copy-constructing GeklminRender classes that perform OpenGL interactions is... Bad. Always instantiate them with NEW and only push pointers onto vectors and such. Data classes such as Meshinstance, IndexedModel, and PhongMaterial are totally fine to do anything with, though. Have fun.
* Don't include STB image with any other parts of your project. I wrote a passthrough into the Texture class which allows you to use stbi_load
* SafeTexture is just a shell for the GLuint. Keep that in mind...

## Why use this instead of a different renderer?
This renderer is very fast and has a unique combination of features and performance.

You cannot get the instancing performance of my engine from any of those big-name game engines.

Pros and Cons compared to those big-name engines' renderers:
Pros:
* Very fast (Although there is known room for improvement... See below)
* Easy to expand and modify, even circumvent all or parts of
* (Assuming you have the dependencies installed) Very easy to implement into your project
* Very good for programming practice or experimentation if you want to morph and mold 3d shapes or just display something on the screen
* Varying levels of low-level interaction, from JustDoEverything() to individual OpenGL Calls, with many helper functions that can be used along the way.
* Relatively low CPU Overhead (It took multiple tens of thousands of instances in instanced rendering to eat 1 core in the main demo with a rendertarget active alongside the main render to the screen)
* GeklminRender is almost entirely on top of GLM and GL, the GLFW dependency is easy to supplement (Rework or replace IODevice) so porting to other platforms is simple. Unlike DirectX...
* Easy to customize, replace, or modify the shaders
* Unique and Original OBJ file format extensions while maintaining compatibility with official standards
* Can be implemented to take up 1 thread in a multithreaded game engine (keep your context bound to the right threads...)

Cons:
* It's OpenGL and not Vulkan, so it eats more CPU
* Nvidia just came out with RTX so in about 5 years all rasterization-based renderers might be made out-of-date
* no shadowmapping implemented by default (You can add it with the custom uniform bindings function and some shader edits) or texture projection
* doesn't have multithreaded optimizations like slave threads for generating std vectors of mat4s
* Phong Materials instead of PBR
* very little attempt to reduce glUniform calls (although that doesnt really matter as long as you use instancing, and even then its better than most engines)
* no use of UBOs
* Cubemaps are Static (Planned to be fixed)
* Shaders limited to only VS + FS
* You are responsible for binding the OpenGL context to the thread which the code runs on

## Planned features and improvements
1) Texture Projection (Mainly for Caustics) and Shadowmapping through the Cameralight (FINISHED)
2) Instantiating a Cubemap with safetextures and/or CubemapFBO (DELAYED)
3) upload CPU-side data as texture in similar manner to Mesh Reshaping. (FINISHED- REINITTEXTUREFROMDATAPOINTER)
4) Text renderer (in development) (Partially Finished)
5) Shape Generators, e.g. Sphere Generator (BOX GENERATOR DONE)
6) Point Sprite towards Camera as a GLM extended utility (EDIT: DONE)
7) Sprite Renderer
8) More functions for creative coding usage case




Enjoy the Library.

Contact me about any bugs, questions, requests, etc. either through my email:
gordonfreeman424@gmail.com
or through my Steam:
https://steamcommunity.com/id/Geklmin/
(You will have to add me on steam... my profile is private)



## Special Thanks
* Dark Photon, the most helpful human being on the OpenGL Forums
* Velbud, for help studying the quirks, features, and interesting properties of various pre-existing game engine rendering systems
* BennyBox, for letting me using his code under the MIT license, giving me the option of the BSD 3-Clause License, and teaching me OpenGL.
* Spectre, for Testing compatibility on AMD GPUs
* Hugh, for OpenGL Inspiration, insight, and advice
* My mom, for testing compatibility on Intel GPUs
* Github for hosting the repository despite being a shitload of fuck to use
* whoever teaches me how to convert this project to makefiles successfully gets their name here when that happens
* You, for bothering to read the special thanks
* Haribo for the delicious gummy bears I ate whilst coding
* Winterberg for being the best rubber-ducking friend I ever had
* Sheo for calling me a fat fuck and an idiot every time I open steam and begging me to play half life

## VERSION HISTORY
### 106: Initial Release (V1)
  Hotfixes before Upload:
  * Removed Quirk where you couldn't relocate instanced meshes between drawrenderpipeline and the Vsync
  * Removed deletion quirk of MeshInstance would could cause crash if done between drawing and vsync
  * Changed how deletion worked to use Iterators
  * Changed Names of functions and variables from old names related to the defunct deferred rendering pipeline to names which make sense in the forward pipeline that is implemented
  * Added prototype functions for createbox and createSphere, with createBox being incomplete
### 107: Second Release (V2)
  * createBox function finished and demo changed to reflect feature addition
  * Glew32s dependency removed (it was unused)
  * createSphere function prototyped
### 110: Third Release (V3)
  * Major bug fixes
  * DLL library
  * General Improvements
### 111: Fourth Release (V4)
  * More bug fixes...
  * Interface improvements
  * Direct access to the registration vectors
  * More functions in IODevice.h
### 112: Fifth Release (V5)
  * Fixed issue whereby including GeklminRender.h into multiple CPP files would cause linker error "multiple definitions" of functions from GeklminRender.h
### 113: Sixth Release (V6)
  * Removed politically incorrect joke from the comments
### 117: Seventh Release (V7)
  * Major performance Overhaul
  * Bug fixes
  * Copy constructors, null constructors, and re-construction support added for Mesh and FBO (No null constructor for FBO)
### 121: Eighth Release (V8) (Pre)
  * temporary replacement for WBOIT
  * Cubemap Diffusion Added
### 122: Ninth Release (V9)
  * WBOIT working on AMD, NVIDIA, and Intel GPUs (All I had to do was change gl_FragDepth to gl_FragCoord.z)
  * Features of the Eighth Release Repeated
### 124: Tenth Release (V10)
  * Bug fix relating to how light was calculated for Rendertarget Cameras
  * Fog Added
  * Shader Improvements'
### 128: Eleventh Release (V11)
  * Shader Optimizations
  * Fog and Lighting code bug fixes
  * More constructors were added, currently the only classes you still can't copy-construct and/or null construct are Shader and Cubemap. For maximum error avoidance, try not to use the new constructors, as some are totally untested (they compile!)
  * Added a utility function to the fragment shader which finds the squared length of a vec3
  * Delaying plans for per-vertex baked lighting
### 131: Twelfth Release (V12)
  * See Update notes... generally, just Optimization
### 134: Thirteenth Release (V13)
  * Added politically incorrect joke somewhere in the code (I dont remember)
  * Using compiler optimizations and vectorization in the build binaries from now on...
  * No longer compiling binaries with GDDB flag
  * fixed bugs
  * fixed Texture Re-Initialization from Data Pointer (Used for CPU-side Frame Buffer in Demo4)
### FROM NOW ON, GO READ THE DAMN PATCH NOTES IN THE RELEASE SECTION YOURSELF, OK???
## Prerelease Version History (Names and Descriptions where Applicable)
 * Pre-Scene: BennyBox Intro to Modern OpenGL Codebase with modifications
 * 1-5 LOST
 * 6- Range and Brightness 
  * Referring to the Point Light in the code
 * 7- Rendering Benchmark
  * Testing how fast rendering was without batching or instancing or any of those optimizations
 * 8- Commenting Existing Features
 * 9- Seperated Uniform Matrices
  * Not combining mat4s for viewprojmodel
 * 10- Adding Multiple GLuint for multiple color attachments, FBO class
  * Adding capability to have multiple color attachments to FBOs
 * 11- Changing the FBO Creation Call
 * 12- Allocating the Color Attachments for the FBO class
 * 13- Test multiple color attachment rendering and emit safetexture
  * Multiple Color Attachment rendering performed for first time, safetexture system changed
 * 14- Clean HOUSE!
  * Code Cleanup
 * 15- GL 3 3
  * Testing to ensure 3.3 compatibility
 * 16- Screenquad IndexedModel
  * Prepping to add the Screenquad function to the Scene class
 * 17- Cameraspace Rendering Shader and the Screenquad
  * ShowTex Shader Written
 * 18- Render to the Screenquad
 * 19- Move the Screenquad thing to a function and optimize
 * 20- Fullscreen Triangle
 * 21- Toss out the code that uses the custom FBO, render it to me straight doc
  * Minor Refactor
 * 22- Fixed Benny's OBJ Loader
  * First of many fixes to BennyBox's lackluster OBJ Loader to support more in the OBJ format
 * 23- Quick optimizations, changes, updates, etc
  * Trying to keep the code clean and fast
 * 24- Checking Destructors and Constructors
  * Double-checking for memory leaks (Note: Some were found later)
 * 25- Initial Opaque Pass Development (FINALLY)
  * Beginning of the Now-Defunct Deferred Rendering Pipeline development
 * 26- Implementing Renderflags and shitsparency
  * Implemented system to allow the most basic form of transparency, by using discard in the fragment shader (Later removed due to performance hit)
 * 27- GL_SAMPLE_ALPHA_TO_COVERAGE
  * Testing an idea which didn't work to achieve screen door transparency
 * 28- m_NamingConvention
  * Trying to name handles with m_ like all the cool kids. Thinking about it, I should probably start doing that again...
 * 29- Optimizations and Options PART 1
 * 30- Header Files
  * First attempt to pack everything into header files. Later, this would turn out to be a very bad idea, because I need to do the exact opposite to get the library ready for shipping as a DLL
 * 31- Phong Update
  * Something to do with the PHONG lighting/reflection model
 * 32- Initialize Everything
 * 33- Error Checking with OpenGL and GLFW
  * Sniffing around my code for errors with glGetError glfwGetError
 * 34- vertexattribpointers
  * Something to do with vertex attributes, I have no clue
 * 35- Trivial Packing
  * I have no idea
 * 36- Mesh can Draw too
  * Moving draw calls to the mesh class
 * 37- Completely Phasing out the RenderableMesh
  * RenderableMesh phased out in favor of MeshInstance
 * 38- Size t iterators
  * Tried to make all for loops use size_t variables where important instead of int, to increase instance capacity
 * 39- Transparency Flag Update in Texture and Safetexture
  * You can now decide whether or not a Texture is transparent when you load it
 * 40- Safetexture Literal
  * Instead of passing a pointer to a Safetexture, safetextures are now literally passed to meshes
 * 41- Nvidia Error Investigation
  * Doctor memory was used on the demo to investigate an invalid memory read error. Turns out to be coming from glBufferData
 * 42- Renaming the Point Light Class
 * 43- Cubemap Class
  * First addition of the Cubemap Class
 * 44- Cubemap Test Fast
  * For some reason I made this an entire seperate internal version
 * 45- Cubemap Reflections
  * First implementation of Cubemap Reflections
 * 46- FBO 16 bit precision
  * Allows FBOs to be instantiated with custom precision and internal data type
 * 47- Proper Depth
 * 48- PROPER, PROPER DEPTH!
  * I was having trouble with something I was being really stupid about, so I actually went onto the GL forums for help about it. Thanks Dark Photon for the advice on gl_FragDepth. Never would have gotten it fixed without the man.
 * 49- Initial Opaque Shader Cleanup
  * I was still operating under the idea that I would be releasing a deferred rendering engine
 * 50- Skybox Rendering
  * Skybox rendering added
 * 51- Skybox Masking
  * something to do with the deferred rendering pipeline, obviously it doesnt exist anymore
 * 52- Shader Efficiency
  * Testing and ensuring shaders are as fast as possible
 * 53- Per-Mesh Cubemaps
 * 54- Adding a shader to the Scene class (Preparing for point lights)
 * 55- God help me
  * Discovered an impossible bug
 * 56- What does it look like
  * Fixed it
 * 57- Pointlights FINISHED!
  * Pointlight class finished
 * 58- Cubemap Reflection Tweak
 * 59- Perf Benchmark
  * This is where I start to see its going to be a bumpy ride...
 * 60- Specular Dampening in Init Opaque for Cubemap Reflections
 * 61- ResizeSceneViewport
  * Adding Ability to resize the viewport
 * 62- Point Light Optimization
 * 63- Multipointlight shader Loading and Optimizing light code
  * Still deferred...
 * 64- Light Classes
  * Wrote a whole bunch of classes
 * 65- Pointlight Limit 200
  * The advantage of deferred rendering: Lots of lights.
 * 66- Bug Fix in the OBJLoader
  * ANOTHER OBJLOADER BUGFIX
 * 67- Deferred OR Forward Initialization
  * Start planning a forward pipeline
 * 68- Directional Lights in the Deferred Rendering Pipeline
  * But im still going with the deferred pipeline...
 * 69- May 24th and 25th Restructuring
  * On may 24th I realized that Deferred Rendering was not viable (A few months later I found out that I was doing something wrong, and I might go back and try again) and switched to forward rendering
 * 70- May 24th Resolution Part 2 Removing Unused Shaders, FBOs, and the render_deferred boolean
 * 71- May 24th Resolution Part 3
 * 73- Batched Rendering
  * 72 was skipped for some reason. In this version, batched rendering for non-instanced instances was added.
 * 74- Sending Directional Lights to MainShader
 * 75- Instanced Rendering Part 1- Mesh Class
  * Instanced Rendering starts to be added...
 * 76- Sending the closest 32 point lights
  * This is where that came in.
 * 77- Registering Ambient Lights
  * Ambient Lights... You see where this is going? I believed at the time I was very close to the first release. HA!
 * 78- Sending Ambient Lights to the Shader the same way we do point lights
 * 79- Hardware Instancing part 0 fixing up the variables in the mesh class
  * Minor Refactoring
 * 80- Recovering Old Performance
  * This is where I figured out why perf was so bad. It wasn't my rendering code, it was the way i was generating the mat4 transforms of objects.
 * 81- GenBuffer the buffer which will hold the mat4s for instancing
  * Hardware Instancing is being developed...
 * 82- Grabbing and Changing the Shader Variable
 * 83- Creating the MatrixDataCache (UNCANCELLED!)
  * I considered cancelling instancing at times due to how hard it was. I was being an idiot in retrospect :P
 * 84- Getting Familiar with Deferred Rendering Again (CANCELLED)
  * I was going to go back to deferred rendering after I figured out that it wasnt the deferred rendering that was slow it was my mat4 generation. However, I was so invested in forward rendering, and the code did everything I wanted it to, so there was no reason to
 * 85- Uploading the MatrixDataCache to the GPU
  * I think this is where instanced rendering started working
 * 86- Namespace
  * Started to pack things up for first release
 * 87- Cleanup and Commenting
 * 88- Adding Planes to the Light Classes
  * Scrapped version of what I ended up doing in release version 1 to allow for masking a volume from light, thereby simulating a shadowed region. In the end, it was only implemented for directional lights, although the bindings and code are commented out for point lights and others
 * 89- Plane Shader Variables and Plane Default Values
 * 90- Emissivity and Per Vertex Color
  * I re-added per-vertex color because it was so awesome
 * 91- Spheres and boxes for Lights
  * This is the volume culling system I ended up implementing into the final version, although I did not use the sphere system.
 * 92- Changing Shape
  * Allow for meshes to change shape
 * 93- Changing Shape Test
  * Surprise- It worked first try.
 * 94- Optional Render-to-Target
  * Allowed specifying a render-target in the scene class's draw function
 * 95- Cant change meshes more than once in a frame
  * Had a rubber ducking moment with my dad while trying to explain what the engine did. Realized there was a potential crash if the programmer was an idiot.
 * 96- CameraLight Class Refitting
  * This is, as of yet, unimplemented, but there were times when I thought it was definitely going in before first release.
 * 97- Masking Meshes and Meshinstances
  * Meshmask system invented
 * 98- Weighted Blended OIT Attempt 2 Part 1 Switching to FBOs
  * I tried to implement WBOIT twice, the first time resulted in a total failure- Program instantly crashed on startup with no debug output worth mentioning. The second time worked perfectly, but took more versions than I anticipated
 * 99- Weighted Blended OIT Att2 P2 Requiring a Second FBO in Rendertarget Mode
 * 100- WBOIT ATT2 P3 Passing in a second FBO
  * This is a pattern with versions. I implement, then I test, in seperate versions.
 * 101- WBOIT ATT2 P4 Using In-function variables for the pointer to the FBOs we use in renderpipeline
 * 102- WBOIT ATT2 P5 Changing what the Shader Ouputs and FBO bound and blend modes
 * 103- WBOIT ATT2 P6 WBOIT IS COMPLETE
  * I decided, then and there, that I was ready to release version 1
 * 104- Bug Fixes and Cleanup and Stuff
 * 105- Rendertarget Camera Test
  * testing the ability to use a second camera for the rendertarget. It worked.
### then came the hotfixes, and then release
