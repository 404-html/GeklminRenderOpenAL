Welcome to the GeklminRender OpenAL Repo! This will (probably) be where all continued development will occur.

This is licensed under the MiT license.

You have everything you need to make games and demos.

Changes to GeklminRender will now go here.

## GeklminRender + GekAL (OpenAL wrapper)
### What is it?
GeklminRender is an OpenGL based toolkit for 3d rendering and visual effects. It contains numerous helper classes to assist programming as well as a fully pre-made 3D model renderer capable of loading models and displaying them with lighting, shadows, and cubemap reflections. 

The OpenAL part of the library, which I have dubbed GekAL, is a small wrapper/helper library that I wrote to add the ability to load and play WAV files.

At time of writing (February 25 2019) there is currently a text renderer in development (which will also be capable of CPU rendering for all those fancy CPU graphics demos i'm sure you're eager to write) which you can use to display menus and text.

GeklminRender was designed for speed and its aesthetics are chosen based on my personal preference. I am currently writing a manual for GeklminRender and the latest version of it is in the top level of the repository.

## How do I build it?
Requirements:
* Microsoft Windows with MinGW 64 bit installed or Linux 64 bit with a fairly recent version of G++
* OpenGL 3.3 supported graphics hardware
* The dependencies listed in step 1

First off, you must have the following dependencies installed:
* OpenGL
* GL3 headers in include (so that you can say #include<GL3/gl3.h>)
* GLM (Math library) installed in Include
* GLFW (Windowing library)

Secondly, after you've unzipped/cloned the repo somewhere you must go into the THEVERSION folder and run the Compiler_Call bat or sh file, depending on your platform. IF YOU ARE ON WINDOWS, PLEASE REMOVE THE -I ARGUMENTS AS THEY POINT TO THE LOCATION OF THE INCLUDE FOLDER FOR MY MINGW INSTALLATION ON MY COMPUTER FROM THE BAT FILE. Thank you.

Thirdly, to run the main demo, run the Prog.bin with ./Prog.bin or Prog2.bin with ./Prog2.bin. On windows, EXEs are generated instead of .bin files and there's even little icons on them.

## Why this?

The primary advantages of using my code over other peoples is as follows
### it's pretty fast- Here's some benchmark figures on a GTX 960 SSC at 1080p
CONDITIONS:
1) Only one mesh- 5000 triangles, some rendered transparent and some not, some with a very high resolution texture and some with a fairly low resolution one.
2) Cubemap reflections randomly enabled/disabled for each one (no effect on performance)
3) Only 1 "cameralight" which projects a particular color like a spotlight, has shadows
4) shadows are enabled/disabled by manually commenting out shader code
5) "shadows being rendered" refers to whether or not the shadowmap is being rendered- resolution is 640x480
6) There is a 100,000 polygon mesh being neglected, which constitutes the terrain (Only part of it is in view at a time, so it is not factored into the benchmark) and a model of a deer with an unknown number of polygons that I made
7) Number of point lights is number registered to the GkScene and not related to commenting things in the shader
8) Transparency is disabled by failing to register a composition shader
### Number of 5000 poly meshes able to be rendered in the view at approximately 60 fps when spawned randomly via 'E' in the demo:
### With Shadows being rendered, shadows enabled, and 32 point lights: ~600 - 800 (~3.5 mil tris)
### With Shadows being rendered, shadows disabled, and 32 point lights: ~1200 - 1400 (~6.5 mil tris)
### With Shadows being rendered, shadows disabled, and no point lights: ~1400 - 1600 (~7.5 mil tris)
### With Shadows not rendering, shadows enabled, and 32 point lights: ~800 - 1000 (~4.5 mil tris) 
### With Shadows not rendering, shadows disabled, and 32 point lights: ~1300 - 1600 (~7.25 mil tris)
### With Shadows not rendering, shadows disabled, and no point lights: ~1800 - 2000 (~9.5 mil tris)
### Super-optimized with no transparency and no point lights: ~2000 (~10 mil tris)
The 5,000 polygon mesh is a fairly decent analogue to a typical decently-high poly model in a modern video game. As an example, I found an extracted 3d player model of Michael De Santa from GTA V and its triangle count was 27,456. According to my benchmarks, with shadows and the maximum number of point lights, at 1080p, allowing approximately half of the models to be transparent (transparent objects render slower) I could render ~120 of them at 60 frames per second. With the maximum potential rendering capacity while still having transparency, I could render ~350 of them. 
### it's very easy to integrate your own OpenGL code with it
There are callbacks in the main renderer that allow you to add your own rendering code and draw on any buffer in any stage of the pipeline. Helper classes make writing GL code a lot easier and more fun. If you want to draw menus/overlays then you can use the BMPFontRenderer or merely do all that AFTER the main renderPipeLine of GkScene.
### It's multiplatform
It's been tested on Intel, AMD, and NVidia graphics chips, Microsoft Windows and Linux. It has only been compiled as 64 bit, but it is presumed that it would work as a 32 bit program if the code were altered slightly to allow that.
### Features?
* 3D (duh!)
* Phong lighting
* Point lights, camera lights, directional lights, and ambient lights
* static cubemap reflections (as well as cubemap diffusion, my own invention)
* Weighted Blended Order Independent Transparency
* 
TODO: Finish README.MD
## Is it ready for implementation?
No. Previous versions were, however the implementation has changed as new features are required for my projects (This is the rendering library I use for my C++ hobby/game programming).
Before I can declare this library finally ready for implementation:
1) I have to finish the BMPFontRenderer
2) I have to finish version 1 of the manual
3) I have to re-test every single feature and rigorously test for memory leaks
4) I have to re-test on AMD and Intel graphics chips
5) I have to re-test on Windows
6) Massive code cleanup- Consistency improvements by renaming things, removing old comments, adding in new comments, setters/getters need to be added, more stupid-proofing needs to be done.
7) Code reorganization- Function declarations need to be moved out of header files into cpp files, some header files need to be renamed, some cpp files need to be renamed.


## Special Thanks
* Dark Photon, the most helpful human being on the OpenGL Forums
* Velbud, for help studying the quirks, features, and interesting properties of various pre-existing game engine rendering systems
* BennyBox, for letting me using his code under the MIT license, giving me the option of the BSD 3-Clause License, and teaching me OpenGL.
* Spectre, for Testing compatibility on AMD GPUs
* Hugh, for OpenGL Inspiration, insight, and advice
* My mom, for testing compatibility on Intel GPUs
* Github for hosting the repository despite being a shitload of fuck to use
* You, for bothering to read the special thanks
* Haribo for the delicious gummy bears I ate whilst coding
* Winterberg for being the best rubber-ducking friend I ever had
* Sheo for calling me a fat fuck and an idiot every time I open steam and begging me to play half life

## Prerelease Version History (Names and Descriptions where Applicable) of GeklminRender before OpenAL
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
