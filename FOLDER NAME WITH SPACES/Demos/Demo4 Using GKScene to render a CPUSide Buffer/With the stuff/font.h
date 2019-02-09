#ifndef Font_H
#define Font_H
#include "geklminrender.h"
#include <map>
#include <unordered_map>
#include <vector>

namespace GeklminRender{
	class Font
	{
		public:
			Font();
			Font(std::string Directory);
			~Font();
			const std::string AllAsciiCharactersString = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
			std::map<char, Mesh*> Characters;
			std::map<char, glm::vec3> CharacterAABBDimensions;
			Mesh* ErrorChar = nullptr;
			
			void registerFontToScene(GkScene* inScene);
			void deRegisterFontFromScene(GkScene* inScene);
			void pushTextureToAllMeshes(SafeTexture intex);
			void removeTextureFromAllmeshes(GLuint handle);
			void removeCubeMapFromAllmeshes(CubeMap* cubey);
			void pushCubeMapToAllMeshes(CubeMap* inCubeMap);
			void pushMaterialPropertiesToAllLetters(Phong_Material Mat, bool instanced_enable_cubemap_reflections = false, bool instanced_enable_cubemap_diffusion = false);
			void deRegisterInstanceFromMeshes(MeshInstance* Deregme);
			bool amINull(){return isnull;}
		protected:

		private:
			Font(const Font& other);
			bool isnull = true;
	};
	
	
	class Textbox {
		public:
			std::string myText = "";
			enum {
				//Horizontal line alignment
				TEXT_CENTER,
				TEXT_LEFT,
				TEXT_RIGHT,
				//Spacing Determination
				TEXT_HORIZONTAL_SPACING_MAX,
				TEXT_HORIZONTAL_SPACING_AVG,
				TEXT_VERTICAL_SPACING_MAX,
				TEXT_VERTICAL_SPACING_AVG,
				TEXT_MODE_COUNT
			};
			int line_alignment_mode = TEXT_LEFT;
			int horiz_spacing_mode = TEXT_HORIZONTAL_SPACING_MAX;
			int vert_spacing_mode = TEXT_VERTICAL_SPACING_MAX;
			Textbox(){
				//Destructor Bit
				if (myFont != nullptr)
				{
					for (auto const& iter : myInstances)
					{
						if ((iter) != nullptr)
							myFont->deRegisterInstanceFromMeshes(iter);
					}
				}
				myFont = nullptr;
			}
			Textbox(Font* _font, std::string _text, Transform _transform = 
				Transform(glm::vec3(0,0,0), glm::quat(), glm::vec3(1,1,1))
			){
				//Destructor Bit
				if (myFont != nullptr)
				{
					for (auto const& iter : myInstances)
					{
						if ((iter) != nullptr)
							myFont->deRegisterInstanceFromMeshes(iter);
					}
				}
				if (_font != nullptr)
					{myFont = _font;}
				myText = _text;
			}
			~Textbox(){
				//Destructor Bit
				if (myFont != nullptr)
				{
					for (auto const& iter : myInstances)
					{
						if ((iter) != nullptr)
							myFont->deRegisterInstanceFromMeshes(iter);
					}
				}
				myFont = nullptr;
			}
			void ReInitialize(Transform _Trans){
				if (myFont == nullptr || myText == "")
					return;
				if (myFont != nullptr)
				{
					for (auto const& iter : myInstances)
					{
						if ((iter) != nullptr)
							myFont->deRegisterInstanceFromMeshes(iter);
					}
				}
				//This is the part where we do kerning
				//Variables for keeping track of the line and position on the line
				float line_number = 0;
				float position_on_line = 0;
				//Variables used in kerning
				float average_character_width;
				float average_character_height; 
				float max_character_width;
				float max_character_height;
			}
		protected:
		private:
			Textbox(const Textbox& other); //NO COPY CONSTRUCTING... for now...
			Font* myFont = nullptr; //This will be nullptr if this is uninitialized
			std::vector<MeshInstance*> myInstances;
	};
}; //EOF namespace
#endif // Font_H
