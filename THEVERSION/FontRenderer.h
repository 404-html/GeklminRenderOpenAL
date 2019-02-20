
#ifndef GEKL_BMP_FONT_RENDER
#define GEKL_BMP_FONT_RENDER

#include "geklminrender.h"
#include <string>


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
		unsigned char redBackground = 1;
		unsigned char greenBackground = 1;
		unsigned char blueBackground = 1;
		unsigned char alphaBackground = 1;
		unsigned int rect_Width = 0;
		unsigned int rect_Height = 0;
	};
	
	class BMPFontRenderer{ //Great for: Rendering text, Sprite-based games, CPU rendering demos, etc.
		public:
			
			BMPFontRenderer(std::string filepath, unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor = 1.0);
			BMPFontRenderer(unsigned char* _BMPFont, unsigned int _BMPFont_Width, unsigned int _BMPFont_Height, unsigned int _num_components_BMPFont, unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor = 1.0); //Use data to init
			
			void resize(unsigned int x_screen_width, unsigned int y_screen_height, float scaling_factor = 1.0);
			void clearscreen(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //black with 0 alpha
			void clearText(); //remove all text from screen
			//OPENGL CALLS!!!
			void pushChangestoTexture(); //reInitfromDataPointer
			void Draw(bool useBlending = true); //Draw all the text to the screen, screenquad, use blending
			
			void writePixel(unsigned int x, unsigned int y, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //[(x + width * y)*num_components + component index]
			void writeRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //Draw a rectangle
			void writeCircle(unsigned int x, unsigned int y, float radius, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //Draws a circle
			void writeEllipse(unsigned int x, unsigned int y, float width, float height, float rotation, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha); //Draws an ellipse
			void writeImage(
				unsigned int x, unsigned int y, //Where in the buffer shall the top left corner be
				unsigned char* Source, unsigned int width, unsigned int height, unsigned int num_components, //Information about source image. if a component (such as alpha) is missing, then it is assumed to be 1
				unsigned int subx1, unsigned int subx2, unsigned int suby1, unsigned int suby2, //Where in the source image?
				int xscale, int yscale, //Scaling information
				bool Blend //Do we blend? Do we simply overwrite?
			);
			std::vector<BMPTextBox> TextBoxes; //All text boxes. Public so you can access it.
		protected:
		private:
			//You must use new to allocate this
			BMPFontRenderer(const BMPFontRenderer& Other);
			void operator=(const BMPFontRenderer& Other);
			
			//Member variables
			bool HasBeenInitialized = false;
			unsigned char* BMPFont = nullptr; //Loaded with stb_image_passhthrough
			unsigned int num_components_BMPFont = 3; //Default Guess
			unsigned int screen_width = 0;
			unsigned int screen_height = 0;
			Texture* Screen = nullptr; //The screen. IMPORTANT: RGBA! 4 COMPONENTS!
	};
};

#endif
