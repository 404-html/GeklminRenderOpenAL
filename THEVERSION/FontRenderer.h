
#ifndef GEKL_BMP_FONT_RENDER
#define GEKL_BMP_FONT_RENDER

#include "geklminrender.h"
#include <string>
#include <cmath>


namespace GeklminRender{
	struct BMPTextBox{
		std::string text = "Sample Text";
		float x = 0;
		float y = 0;
		float scale = 1.0;
		unsigned char red = 1;
		unsigned char green = 1;
		unsigned char blue = 1;
		unsigned char alpha = 1;
		//Rectangle behind the textbox
		bool doRenderBackground = false;
		bool isEllipse = false; //if it's an ellipse...
		unsigned char redBackground = 1;
		unsigned char greenBackground = 1;
		unsigned char blueBackground = 1;
		unsigned char alphaBackground = 1;
		//rectangle/ellipse width and height
		unsigned int rect_Width = 0;
		unsigned int rect_Height = 0;
	};
	
	
	
	class BMPFontRenderer{ //Great for: Rendering text, Sprite-based games, CPU rendering demos, etc.
		public:
			
			BMPFontRenderer(std::string filepath, unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor = 1.0, std::string Shader_location = "shaders/SHOWTEX_BMPFONT");
			BMPFontRenderer(unsigned char* _BMPFont, unsigned int _BMPFont_Width, unsigned int _BMPFont_Height, unsigned int _num_components_BMPFont, unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor = 1.0, std::string Shader_location = "shaders/SHOWTEX_BMPFONT"); //Use data to init
			virtual ~BMPFontRenderer();
			void resize(unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor = 1.0);
			void clearScreen(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //Clear to this
			void addText(BMPTextBox _texbox){TextBoxes.push_back(_texbox);}
			void clearText() {TextBoxes.clear();} //remove all text
			void writeText(); //write all text in the list of textboxes
			//OPENGL CALLS!!!
			void pushChangestoTexture(); //reInitfromDataPointer
			void Draw(bool useBlending = true); //Draw all the text to the screen, screenquad, use blending
			
			void writePixel(unsigned int x, unsigned int y, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //[(x + width * y)*num_components + component index]
			void writeRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //Draw a rectangle
			void writeCircle(unsigned int x, unsigned int y, float radius, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //Draws a circle
			void writeEllipse(unsigned int x, unsigned int y, float width, float height, float rotation, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //Draws an ellipse
			void writeImage(
				int x, int y, //Where in the buffer shall the bottom left corner be
				unsigned char* Source, unsigned int width, unsigned int height, unsigned int num_components, //Information about source image. if a component (such as alpha) is missing, then it is assumed to be 1
				unsigned int subx1, unsigned int subx2, unsigned int suby1, unsigned int suby2, //Where in the source image?
				unsigned int targwidth, unsigned int targheight, //How wide and how tall in the target?
				bool flip_x = false, bool flip_y = false //Flip?
			);
			void setCharDimensions(unsigned int _char_width, unsigned int _char_height, unsigned int _chars_per_row){
				char_width = _char_width;
				char_height = _char_height;
				chars_per_row = _chars_per_row;
			}
			bool amINull(){return isNull;}
			std::vector<BMPTextBox>* getTextBoxVector(){return &TextBoxes;}
			
		protected:
		private:
			std::vector<BMPTextBox> TextBoxes; //All text boxes. Public so you can access it.
			//You must use new to allocate this
			BMPFontRenderer(const BMPFontRenderer& Other);
			void operator=(const BMPFontRenderer& Other);
			
			//Member variables
			bool isNull = true;
			unsigned char* BMPFont = nullptr; //Loaded with stb_image_passhthrough
			unsigned int num_components_BMPFont = 3; //Default Guess
			unsigned int BMPFontWidth = 0;
			unsigned int BMPFontHeight = 0;
			unsigned int screen_width = 0;
			unsigned int screen_height = 0;
			//Imfo about the particular font in question
			unsigned int char_width = 16;
			unsigned int char_height = 16;
			unsigned int chars_per_row = 8;
			float my_scaling_factor = 1.0;
			Texture* Screen = nullptr; //The screen. IMPORTANT: RGBA! 4 COMPONENTS!
			Mesh* Screenquad_Mesh = nullptr;
			Shader* Screenquad_Shader = nullptr;
			GLuint diffuse_loc = 0;
			GLuint cam_loc = 0;
			IndexedModel screenquad_IndexedModel;
			glm::mat4 Screenquad_CameraMatrix;
	};
};

#endif
