#ifndef OBJ_LOADER_H_INCLUDED
#define OBJ_LOADER_H_INCLUDED
//(C) DMHSW 2018 All Rights Reserved
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
namespace GeklminRender {
	//An extremely lightweight, easy to read, 100% functional OBJ Loader
	//It does not handle materials, and if presented with a file with multiple objects in it, will only use the first one.
	static const unsigned int GK_RENDER = 1; // Do we render it? This is perhaps the most important flag.
	static const unsigned int GK_TEXTURED = 2; // Do we texture it? if disabled, only the texture will be used. if both this and colored are disabled, the object will be black.
	static const unsigned int GK_COLORED = 4;// Do we color it? if disabled, only the texture will be used. if both this and textured are disabled, the object will be black.
	static const unsigned int GK_FLAT_NORMAL = 8; // Do we use flat normals? If this is set, then the normals output to the fragment shader in the initial opaque pass will use the flat layout qualifier. 
	static const unsigned int GK_FLAT_COLOR = 16; // Do we render flat colors? the final, provoking vertex will be used as the color for the entire triangle.
	static const unsigned int GK_COLOR_IS_BASE = 32; //Use the color as the primary. Uses texture as primary if disabled.
	static const unsigned int GK_TINT = 64; //Does secondary add to primary?
	static const unsigned int GK_DARKEN = 128;//Does secondary subtract from primary?
	static const unsigned int GK_AVERAGE = 256;//Do secondary and primary just get averaged?
	static const unsigned int GK_ENABLE_ALPHA_CULLING = 4096; //Do we use the texture alpha to cull alpha fragments
	static const unsigned int GK_TEXTURE_ALPHA_REPLACE_PRIMARY_COLOR = 8192; //This one is a bit weird. It was made so that if you have a model with per-vertex color, that you could overlay a texture on top of it. Check out FORWARD_MAINSHADER.fs in the shaders folder if you want to check that out.

struct OBJIndex //One of those groups of slashes in an F line. these end up getting optimized.
{
    unsigned int vertexIndex;
    unsigned int uvIndex;
    unsigned int normalIndex;
	unsigned int vertColorIndex; //my custom property
	
    bool operator<(const OBJIndex& r) const { return vertexIndex < r.vertexIndex; }
};

class IndexedModel //A model that has been completely loaded from file and optimized.
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colors;
	std::vector<unsigned int> indices;
    unsigned int renderflags = 0; //NOTE TO SELF: Initialization matters
	bool hadRenderFlagsInFile = false;
	bool smoothshading = true;
    void CalcNormals();
	float calcRadius(){ //Unscaled
		if (positions.size() < 1)
			return 0.0;
		float LargestRadius = glm::length(positions[0]);
		for (size_t i = 0; i < positions.size(); i++)
		{
			float rad = glm::length(positions[i]);
			if (rad > LargestRadius)
				LargestRadius = rad;
				
		}
		return LargestRadius;
	}
	bool calcAABB(float& _x, float& _y, float& _z){
		if (positions.size() < 1)
			return false;
		float out_x = positions[0].x * positions[0].x;
		float out_y = positions[0].y * positions[0].y;
		float out_z = positions[0].z * positions[0].z;
		
		//find the largest of each 
		for (size_t i = 0; i < positions.size(); i++)
		{
			float curr_x = positions[i].x * positions[i].x;
			float curr_y = positions[i].y * positions[i].y;
			float curr_z = positions[i].z * positions[i].z;
			if(curr_x > out_x)
				out_x = curr_x;
			if(curr_y > out_y)
				out_y = curr_y;
			if(curr_z > out_z)
				out_z = curr_z;
		}
		_x = 2*(float)sqrt(out_x);
		_y = 2*(float)sqrt(out_y);
		_z = 2*(float)sqrt(out_z);
		return true;
	}
	std::string exportToString(bool exportColors){
		std::string Result = "#Exported by GeklminRender (C) David MHS Webster 2018\n";
		Result.append("o ExportedIndexedModel\n");
		//First, export the GKMODE flags
			if((renderflags & GK_RENDER) > 0)
				Result.append("#GKMODE GK_RENDER\n");
			if((renderflags & GK_TEXTURED) > 0)
				Result.append("#GKMODE GK_TEXTURED\n");
			if((renderflags & GK_COLORED) > 0)
				Result.append("#GKMODE GK_COLORED\n");
			if((renderflags & GK_FLAT_NORMAL) > 0)
				Result.append("#GKMODE GK_FLAT_NORMAL\n");
			if((renderflags & GK_FLAT_COLOR) > 0)
				Result.append("#GKMODE GK_FLAT_COLOR\n");
			if((renderflags & GK_COLOR_IS_BASE) > 0)
				Result.append("#GKMODE GK_COLOR_IS_BASE\n");
			if((renderflags & GK_TINT) > 0)
				Result.append("#GKMODE GK_TINT\n");
			if((renderflags & GK_DARKEN) > 0)
				Result.append("#GKMODE GK_DARKEN\n");
			if((renderflags & GK_AVERAGE) > 0)
				Result.append("#GKMODE GK_AVERAGE\n");
			if((renderflags & GK_ENABLE_ALPHA_CULLING) > 0)
				Result.append("#GKMODE GK_ENABLE_ALPHA_CULLING\n");
			if((renderflags & GK_TEXTURE_ALPHA_REPLACE_PRIMARY_COLOR) > 0)
				Result.append("#GKMODE GK_TEXTURE_ALPHA_REPLACE_PRIMARY_COLOR\n");
		//Next, export positions, making no attempt at optimization, so that exporting the f lines will be super simple
			for (size_t i = 0; i < positions.size(); i++)
			{
				Result.append("v ");
				Result.append(std::to_string(positions[i].x)); //X
				Result.append(" ");
				Result.append(std::to_string(positions[i].y)); //Y
				Result.append(" ");
				Result.append(std::to_string(positions[i].z)); //Z
				Result.append("\n"); //NEWLINE
			}
		//Export texture coordinates... vt lines 
			for(size_t i = 0; i < texCoords.size(); i++){
				Result.append("vt ");
				Result.append(std::to_string(texCoords[i].x)); //X
				Result.append(" ");
				Result.append(std::to_string(texCoords[i].y)); //Y
				Result.append("\n"); //NEWLINE
			}
		//Export normals... vn lines
			for(size_t i = 0; i < normals.size(); i++)
			{
				Result.append("vn ");
				Result.append(std::to_string(normals[i].x)); //X
				Result.append(" ");
				Result.append(std::to_string(normals[i].y)); //Y
				Result.append(" ");
				Result.append(std::to_string(normals[i].z)); //Z
				Result.append("\n"); //NEWLINE
			}
		//Export Colors but only if we are doing that
		if (exportColors)
			for(size_t i = 0; i < colors.size(); i++)
			{
				Result.append("vc ");
				Result.append(std::to_string(colors[i].x)); //X
				Result.append(" ");
				Result.append(std::to_string(colors[i].y)); //Y
				Result.append(" ");
				Result.append(std::to_string(colors[i].z)); //Z
				Result.append("\n"); //NEWLINE
			}
		//Export Indices
		size_t indices_I = 0;
		while (indices_I < indices.size())
		{
			size_t human_indices_index = indices_I + 1;
			if (human_indices_index%3 == 1)
				Result.append("f ");
			//Make the slash thingie
				Result.append(std::to_string(indices[indices_I] + 1)); //POSITION
				Result.append("/");
				Result.append(std::to_string(indices[indices_I] + 1)); //TEXCOORD
				Result.append("/");
				Result.append(std::to_string(indices[indices_I] + 1)); //NORMAL
				if(exportColors){
					Result.append("/");
					Result.append(std::to_string(indices[indices_I] + 1)); //COLOR
				}
				Result.append(" ");
			//Increment indices_I
			indices_I++;
			//But if the human indices index (which HAS NOT CHANGED) is divisible by three, we must end the line
			if(human_indices_index%3 == 0)
				Result.append("\n");
		}
		return Result;
	}
	std::string myFileName = "/Aux/?"; //Impossible path on windows
};

class OBJModel //An unoptimized model that can make optimized models.
{
public:
    std::vector<OBJIndex> OBJIndices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
	std::vector<glm::vec3> vertcolors;
    bool hasUVs = false;
    bool hasNormals = false;
	bool hasVertexColors = false;
	bool smoothshading = true;
    unsigned int renderflags = 0; //NOTE TO SELF: Initialization matters.
	bool hadRenderFlagsInFile = false;
	std::string myFileName = "N/A";
    OBJModel(const std::string& fileName);
    
    IndexedModel ToIndexedModel();
private:
    unsigned int FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IndexedModel& result);
    void CreateOBJFace(const std::string& line);
    
    glm::vec2 ParseOBJVec2(const std::string& line);
    glm::vec3 ParseOBJVec3(const std::string& line);
    OBJIndex ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals, bool* hasVertColors);
};
};
#endif // OBJ_LOADER_H_INCLUDED
