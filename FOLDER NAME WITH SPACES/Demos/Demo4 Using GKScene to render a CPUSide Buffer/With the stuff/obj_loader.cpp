#include "obj_loader.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

namespace GeklminRender {

static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b);
static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end);
static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end);
static inline std::vector<std::string> SplitString(const std::string &s, char delim);

OBJModel::OBJModel(const std::string& fileName)
{
	hasUVs = false;
	hasNormals = false;
	hasVertexColors = false;
	smoothshading = true; //this is the default value.
	renderflags = 0;
    std::ifstream file;
    file.open(fileName.c_str());
	int numOs = 0; //Used to avoid 2nd, 3rd objects
	if (file.good())
		myFileName = fileName; //Don't set the filename unless
    std::string line;
    if(file.is_open())
    {
        while(file.good() && numOs <= 1)
        {
            getline(file, line);
            unsigned int lineLength = line.length();

            if(lineLength < 2)
                continue;

            const char* lineCStr = line.c_str();

            switch(lineCStr[0])
            {
                case 'v':
                    if(lineCStr[1] == 't')
						{this->uvs.push_back(ParseOBJVec2(line));hasUVs = true;}
                    else if(lineCStr[1] == 'n')
						{this->normals.push_back(ParseOBJVec3(line));hasNormals = true;}
                    else if(lineCStr[1] == ' ' || lineCStr[1] == '\t')
                        this->vertices.push_back(ParseOBJVec3(line));
					else if (lineCStr[1] == 'c') //Bada bing bada boom
						{this->vertcolors.push_back(ParseOBJVec3(line));hasVertexColors = true;}
					/*std::cout << "\n We are detecting colors in this file...";*/

                break;
                case 'f':
                    CreateOBJFace(line);
                break;
				case 'o': //Now compatible with OBJ files with multiple objects in them!!!
					numOs++;
				break;
				case 's':
					if(lineLength > 3 && lineCStr[3] == 'f') // Prevent a crash
						smoothshading = false;
					else if(lineLength > 3 && lineCStr[3] == 'n')
						smoothshading = true;
				break;
				case '#': //Handle GKMODE comments
					if (lineLength > 7 && lineCStr[1] == 'G' && lineCStr[2] == 'K' && lineCStr[3] == 'M' && lineCStr[4] == 'O' && lineCStr[5] == 'D' && lineCStr[6] == 'E' && lineCStr[7] == ' ')
					{
						std::string gay = line;
						std::vector<std::string> tokens = SplitString(gay, ' '); //Remember: the first token will be #GKMODE_ (space not underscore) and the second will be the renderflag or something.
						if (tokens.size() > 1){
							if (tokens[1] == "GK_RENDER")
							{
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_RENDER;
								//std::cout << "\nGK_RENDER";
							} else if (tokens[1] == "GK_TEXTURED"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_TEXTURED;
								//std::cout << "\nGK_TEXTURED";
							} else if (tokens[1] == "GK_COLORED"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_COLORED;
								//std::cout << "\nGK_COLORED";
							} else if (tokens[1] == "GK_FLAT_NORMAL"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_FLAT_NORMAL;
								//std::cout << "\nGK_FLAT_NORMAL";
							} else if (tokens[1] == "GK_FLAT_COLOR"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_FLAT_COLOR;
								//std::cout << "\nGK_FLAT_COLOR";
							} else if (tokens[1] == "GK_COLOR_IS_BASE"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_COLOR_IS_BASE;
								//std::cout << "\nGK_COLOR_IS_BASE";
							} else if (tokens[1] == "GK_TINT"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_TINT;
								//std::cout << "\nGK_TINT";
							} else if (tokens[1] == "GK_DARKEN"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_DARKEN;
								//std::cout << "\nGK_DARKEN";
							} else if (tokens[1] == "GK_AVERAGE"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_AVERAGE;
								//std::cout << "\nGK_AVERAGE";
							} else if (tokens[1] == "GK_TEXTURE_ALPHA_REPLACE_PRIMARY_COLOR"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_TEXTURE_ALPHA_REPLACE_PRIMARY_COLOR;
								//std::cout << "\nGK_TEXTURE_ALPHA_REPLACE";
							} else if (tokens[1] == "GK_ENABLE_ALPHA_CULLING"){
								hadRenderFlagsInFile = true;
								renderflags = renderflags | GK_ENABLE_ALPHA_CULLING;
								//std::cout << "\nGK_ENABLE_ALPHA_CULLING";
							}
						}
					}
				break; //End of case: comment
                default: break;
            };
        }
    }
    else
    {
        std::cerr << "Unable to load mesh: " << fileName << std::endl;
    }
	// //std::cout << "\nHello! it's your friendly neighborhood debugger, Cout!\nI heard you have some bugs today. Let me get you the report.";
	// //std::cout << "\n\n\nNumber of elements in vertcolors is: " << vertcolors.size();
	// if (vertcolors.size() > 0)
		// for (int i = 0; i<vertcolors.size(); i++)
			// //std::cout << "\nElement " << i << "r value is " << vertcolors[i].x;

	// //std::cout << "\n\n\nNumber of elements in OBJIndices is: " << OBJIndices.size();
	// if (OBJIndices.size() > 0)
		// for (int i = 0; i< OBJIndices.size(); i++)
			// //std::cout << "\nElement " << i << " has a vertColorIndex of " <<OBJIndices[i].vertColorIndex;
	// if (hasVertexColors)
		// //std::cout<<"\n\n\nI'd like to mention that hasVertexColors is true. Might be a problem.";
}

void IndexedModel::CalcNormals()
{
    for(unsigned int i = 0; i < indices.size(); i += 3)
    {
		int i0;
		int i1;
		int i2;
		
        i0 = indices[i];
        i1 = indices[i + 1];
        i2 = indices[i + 2];
		
        glm::vec3 v1 = positions[i1] - positions[i0];
        glm::vec3 v2 = positions[i2] - positions[i0];

        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }
    for(unsigned int i = 0; i < normals.size(); i++)
        normals[i] = glm::normalize(normals[i]);
}

IndexedModel OBJModel::ToIndexedModel()
{
    IndexedModel result;
    IndexedModel normalModel;

	result.renderflags = renderflags;
	normalModel.renderflags = renderflags;
    result.hadRenderFlagsInFile = hadRenderFlagsInFile; //Why tell a lie? It is the problem of mesh class to handle these trivial matters
	normalModel.hadRenderFlagsInFile = hadRenderFlagsInFile; //We have to do it to both!
	result.myFileName = myFileName;
	normalModel.myFileName = myFileName;
	result.smoothshading = smoothshading;
	normalModel.smoothshading = smoothshading;

	unsigned int numIndices = OBJIndices.size();

    std::vector<OBJIndex*> indexLookup;

    for(unsigned int i = 0; i < numIndices; i++)
        indexLookup.push_back(&OBJIndices[i]);

    std::sort(indexLookup.begin(), indexLookup.end(), CompareOBJIndexPtr);

    std::map<OBJIndex, unsigned int> normalModelIndexMap;
    std::map<unsigned int, unsigned int> indexMap;

    for(unsigned int i = 0; i < numIndices; i++)
    {
        OBJIndex* currentIndex = &OBJIndices[i];

        glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
        glm::vec2 currentTexCoord;
        glm::vec3 currentNormal;
		// the index for the current vertex color is A NEW INDEX SPECIFIED IN THE F LINES OF THE OBJ MODEL
		glm::vec3 currentVertColor;

        if(hasUVs)
            currentTexCoord = uvs[currentIndex->uvIndex];
        else
            currentTexCoord = glm::vec2(0,0);
        if(hasNormals)
            currentNormal = normals[currentIndex->normalIndex];
        else
            currentNormal = glm::vec3(0,0,0);
		
		if (hasVertexColors && vertcolors.size() > currentIndex->vertColorIndex) //Prevent a strange and mysterious crash. hasvertexcolors is being set prematurely somehow
		{
			currentVertColor = vertcolors[currentIndex->vertColorIndex];
			//std::cout << "\n COLOR IDENTIFIED IN CONVERSION!";
		}
		else
			currentVertColor = glm::vec3(0,0,0);

        unsigned int normalModelIndex;
        unsigned int resultModelIndex;

        //Create model to properly generate normals on
        std::map<OBJIndex, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
        if(it == normalModelIndexMap.end())
        {
            normalModelIndex = normalModel.positions.size();

            normalModelIndexMap.insert(std::pair<OBJIndex, unsigned int>(*currentIndex, normalModelIndex));
            normalModel.positions.push_back(currentPosition);
            normalModel.texCoords.push_back(currentTexCoord);
            normalModel.normals.push_back(currentNormal);
			normalModel.colors.push_back(currentVertColor);
        }
        else
            normalModelIndex = it->second;
        //Create model which properly separates texture coordinates
        unsigned int previousVertexLocation = FindLastVertexIndex(indexLookup, currentIndex, result); //This errored once
        if(previousVertexLocation == (unsigned int)-1)
        {
            resultModelIndex = result.positions.size();

            result.positions.push_back(currentPosition);
            result.texCoords.push_back(currentTexCoord);
            result.normals.push_back(currentNormal);
			result.colors.push_back(currentVertColor);
        }
        else
            resultModelIndex = previousVertexLocation;
        normalModel.indices.push_back(normalModelIndex);
        result.indices.push_back(resultModelIndex);
        indexMap.insert(std::pair<unsigned int, unsigned int>(resultModelIndex, normalModelIndex));
    }

    if(!hasNormals) //Smart!
    {
        normalModel.CalcNormals();
        for(unsigned int i = 0; i < result.positions.size(); i++)
            result.normals[i] = normalModel.normals[indexMap[i]];
    }
    return result;
};

unsigned int OBJModel::FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IndexedModel& result)
{
    unsigned int start = 0;
    unsigned int end = indexLookup.size();
    unsigned int current = (end - start) / 2 + start;
    unsigned int previous = start;
    while(current != previous)
    {
        OBJIndex* testIndex = indexLookup[current];

        if(testIndex->vertexIndex == currentIndex->vertexIndex)
        {
            unsigned int countStart = current;

            for(unsigned int i = 0; i < current; i++)
            {
                OBJIndex* possibleIndex = indexLookup[current - i];

                if(possibleIndex == currentIndex)
                    continue;

                if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
                    break;

                countStart--;
            }
            for(unsigned int i = countStart; i < indexLookup.size() - countStart; i++)
            {
                OBJIndex* possibleIndex = indexLookup[current + i];

                if(possibleIndex == currentIndex)
                    continue;

                if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
                    break;
                else if((!hasUVs || possibleIndex->uvIndex == currentIndex->uvIndex)
                    && (!hasNormals || possibleIndex->normalIndex == currentIndex->normalIndex)
					&& (!hasVertexColors || possibleIndex->vertColorIndex == currentIndex->vertColorIndex))
                {
                    glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
                    glm::vec2 currentTexCoord;
                    glm::vec3 currentNormal;
					glm::vec3 currentVertColor;

                    if(hasUVs)
                        currentTexCoord = uvs[currentIndex->uvIndex];
                    else
                        currentTexCoord = glm::vec2(0,0);

                    if(hasNormals)
                        currentNormal = normals[currentIndex->normalIndex];
                    else
                        currentNormal = glm::vec3(0,0,0);

					//This was previously commented
					if(hasVertexColors /*&& currentIndex->vertColorIndex > vertcolors.size()*/)
						currentVertColor = vertcolors[currentIndex->vertColorIndex];
					else
						currentVertColor = glm::vec3(0,0,0);

                    for(unsigned int j = 0; j < result.positions.size(); j++)
                    {
						//this if test was modified.
                        if(
							currentPosition == result.positions[j]
                            && (
								(!hasUVs || currentTexCoord == result.texCoords[j])
								&& (!hasNormals || currentNormal == result.normals[j])
								&& (!hasVertexColors || currentVertColor == result.colors[j])
							)
						)
                        {
                            return j;
                        }
                    }
                }
            }
            return -1;
        }
        else
        {
            if(testIndex->vertexIndex < currentIndex->vertexIndex)
                start = current;
            else
                end = current;
        }

        previous = current;
        current = (end - start) / 2 + start;
    }

    return -1;
}

void OBJModel::CreateOBJFace(const std::string& line)
{
	//Split into the 4 tokens, first being f, second being the first index...
    std::vector<std::string> tokens = SplitString(line, ' ');
	//NOTE EACH TOKEN LOOKS LIKE THIS:
	//v1/vt1/vn1
	//Or if it has vertex colors
	//v1/vt1/vn1/vc1
	//or it could not have Vt, or no Vn, or no vc1, but it always has the relevant spaced / marks
    this->OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->hasUVs, &this->hasNormals, &this->hasVertexColors));
    this->OBJIndices.push_back(ParseOBJIndex(tokens[2], &this->hasUVs, &this->hasNormals, &this->hasVertexColors));
    this->OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->hasUVs, &this->hasNormals, &this->hasVertexColors));

    if((int)tokens.size() > 4) //This appears to check if it is a square...
    {
        this->OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->hasUVs, &this->hasNormals, &this->hasVertexColors));
        this->OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->hasUVs, &this->hasNormals, &this->hasVertexColors));
        this->OBJIndices.push_back(ParseOBJIndex(tokens[4], &this->hasUVs, &this->hasNormals, &this->hasVertexColors));
    }
}

OBJIndex OBJModel::ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals, bool* hasVertColors)
{
    unsigned int tokenLength = token.length();
    const char* tokenString = token.c_str();

    unsigned int vertIndexStart = 0;
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');

    OBJIndex result;
    result.vertexIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
    result.uvIndex = 0;
    result.normalIndex = 0;
	result.vertColorIndex = 0;
	bool hadUV = false;
	bool hadNormal = false;
	bool hadColor = false;
	
	
    if(vertIndexEnd >= tokenLength) //handle the case of v1
        return result;

	// if (myFileName == "Cube_Test_Low_Poly.obj")
	// {
		// std::cout << "\nBefore Texture Coordinate Check:";
		
		// std::cout << "\n TOKEN" << token;
		// std::cout << "\n TOKENLENGTH: " << tokenLength;
		// std::cout << "\n VERTINDEXSTART: " << vertIndexStart;
		// if (vertIndexStart < tokenLength)
			// std::cout << "\n WHICH was: " << tokenString[vertIndexStart];
		// std::cout << "\n VERTINDEXEND: " << vertIndexEnd;
		// if (vertIndexEnd < tokenLength)
			// std::cout << "\n WHICH was: " << tokenString[vertIndexEnd];
	// }
	
	//REGARDING TEXTURE COORDINATES:
    vertIndexStart = vertIndexEnd+1;
	if (tokenString[vertIndexStart]!='/')//Case: No UV
	{
		vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');//This is finding the wrong slash!
	}
	else
	{vertIndexEnd = vertIndexStart;vertIndexStart--;}

	//
	if (tokenString[vertIndexStart] != '/' && vertIndexStart < tokenLength)
	{
		result.uvIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);//Then we have a valid uvIndex
		*hasUVs = true; //Don't set this to true if it's not true
		hadUV = true;
	}
	// if (!hadUV) //Cases: No UVs.
	// {
		// vertIndexEnd--;
	// }
	// if (myFileName == "Cube_Test_Low_Poly.obj")
	// {
		// std::cout << "\nAfter Texture Coordinate Check:";
		
		// std::cout << "\n TOKEN:" << token;
		// std::cout << "\n TOKENLENGTH: " << tokenLength;
		// std::cout << "\n VERTINDEXSTART: " << vertIndexStart;
		// if (vertIndexStart < tokenLength)
			// std::cout << "\n WHICH was: " << tokenString[vertIndexStart];
		// std::cout << "\n VERTINDEXEND: " << vertIndexEnd;
		// if (vertIndexEnd < tokenLength)
			// std::cout << "\n WHICH was: " << tokenString[vertIndexEnd];
	// }
	
    if(vertIndexEnd >= tokenLength)
	{return result;}

	// if (myFileName == "Cube_Test_Low_Poly.obj")
		// std::cout <<"\nTesting 2";

	// if (myFileName == "Cube_Test_Low_Poly.obj")
	// {
		// std::cout << "\n ~~FINISHED UV GRAB~~";
		// std::cout << "\n TOKEN:~" << token;
		// std::cout << "\n TOKENLENGTH: " << tokenLength;
		// std::cout << "\n VERTINDEXSTART: " << vertIndexStart;
		// if (vertIndexStart < tokenLength)
			// std::cout << "\n WHICH was: " << tokenString[vertIndexStart];
		// std::cout << "\n VERTINDEXEND: " << vertIndexEnd;
		// if (vertIndexEnd < tokenLength)
			// std::cout << "\n WHICH was: " << tokenString[vertIndexEnd];
	// }
	
	// if (myFileName == "Cube_Test_Low_Poly.obj")
	// {
		// std::cout << "\nBefore Texture Coordinate Check:";
		
		// std::cout << "\n TOKEN:" << token;
		// std::cout << "\n TOKENLENGTH: " << tokenLength;
		// std::cout << "\n VERTINDEXSTART: " << vertIndexStart;
		// if (vertIndexStart < tokenLength)
			// std::cout << "\n WHICH was: " << tokenString[vertIndexStart];
		// std::cout << "\n VERTINDEXEND: " << vertIndexEnd;
		// if (vertIndexEnd < tokenLength)
			// std::cout << "\n WHICH was: " << tokenString[vertIndexEnd];
	// }

	// if (hadUV)
		// vertIndexEnd++;
	// else
		// vertIndexEnd--;
	
    vertIndexStart = vertIndexEnd+1;
	if (tokenString[vertIndexStart]!='/')//Case: No Normal
	{
		vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');//This is finding the wrong slash!
	}
	else
	{vertIndexEnd = vertIndexStart;vertIndexStart--;}//Case: No Normal
	if (vertIndexStart < tokenLength && tokenString[vertIndexStart] != '/')//handle the v/t1//c1 case
    {
		// if (myFileName == "SPHERE_TEST.OBJ")
			// std::cout << "\n Big Uh oh.";
		result.normalIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
		*hasNormals = true;
		hadNormal = true;
	}
	if(vertIndexEnd >= tokenLength) //Make sure this is not the case: #/#//0 where we are on the second slash.
	{
		return result;
	} 
	// else if (tokenString[vertIndexStart-1] == '/'){ //The special case. We need to shift back vertIndexStart to vertIndexStart-1 and FindNextChar to the next slash.
		// vertIndexStart--;
		// vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
	// }
	//Since the token is longer than that, the last item must be the vertex color index.
	// if (myFileName == "SPHERE_TEST.OBJ")
		// std::cout << "\nLonger than normals but no normals to be found?";

	//if (hadNormal)
		vertIndexEnd++;
	
	vertIndexStart = vertIndexEnd;
	//vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
	vertIndexEnd = tokenLength;
	
	if(vertIndexStart < tokenLength && tokenString[vertIndexStart] != '/'){ //So long as we didn't start on a slash
		*hasVertColors = true;
		result.vertColorIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
		// if (myFileName == "SPHERE_TEST.OBJ")
		// {
			// std::cout << "\n WE HAVE COLORS!";
			// std::cout << "\n TOKEN:~" << token;
			// std::cout << "\n TOKENLENGTH: " << tokenLength;
			// std::cout << "\n VERTINDEXSTART: " << vertIndexStart;
			// std::cout << "\n VERTINDEXEND: " << vertIndexEnd;
			// std::cout << "\n COLOR INDEX: " << result.vertColorIndex;
		// }
		hadColor = true;
	} else {
	}

    return result;
}

glm::vec3 OBJModel::ParseOBJVec3(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();

    unsigned int vertIndexStart = 2;

    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }

    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

    float z = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

    return glm::vec3(x,y,z);

    //glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()))
}

glm::vec2 OBJModel::ParseOBJVec2(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();

    unsigned int vertIndexStart = 3;

    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }

    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

    return glm::vec2(x,y);
}

static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b)
{
    return a->vertexIndex < b->vertexIndex;
}

static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
{
    unsigned int result = start;
    while(result < length) //Length actually has to be the maximum index size... if we gave the traditional idea of length (E.g. array[3] which is 0,1,2) then we would be bamboozleed
    {
        result++;
        if(str[result] == token)
            break;
    }

    return result;
}

static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atoi(token.substr(start, end - start).c_str()) - 1;
}

static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atof(token.substr(start, end - start).c_str());
}

static inline std::vector<std::string> SplitString(const std::string &s, char delim)
{
    std::vector<std::string> elems;

    const char* cstr = s.c_str();
    unsigned int strLength = s.length();
    unsigned int start = 0;
    unsigned int end = 0;

    while(end <= strLength)
    {
        while(end <= strLength)
        {
            if(cstr[end] == delim)
                break;
            end++;
        }

        elems.push_back(s.substr(start, end - start));
        start = end + 1;
        end = start;
    }

    return elems;
}
}; //Eof Namespace