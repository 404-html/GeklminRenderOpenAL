#include "font.h"
namespace GeklminRender{
	Font::Font(std::string Directory){ //Must end in slash
		if (!isnull) //REPLACE WITH DESTRUCTOR LATER
		{
			for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					delete iter.second;
			}
			Characters.clear();
			CharacterAABBDimensions.clear();
		}
		isnull = false;
		std::cout << "\nFONT CONSTRUCTOR CALLED!!!";
		size_t char_iter = 0;
		//Load all the Meshes
		//new Mesh("Cube_Test_Low_Poly.obj",true,false,true); //Instanced, Static, Asset
		
		Characters[AllAsciiCharactersString.c_str()[0]] = new Mesh(Directory + "Exclamation Mark.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[1]] = new Mesh(Directory + "Double Quote.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[2]] = new Mesh(Directory + "Pound Sign.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[3]] = new Mesh(Directory + "Dollar Currency Sign.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[4]] = new Mesh(Directory + "Percent Sign.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[5]] = new Mesh(Directory + "Ampersand.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[6]] = new Mesh(Directory + "Single Quote.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[7]] = new Mesh(Directory + "Left Parentheses.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[8]] = new Mesh(Directory + "Right Parentheses.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[9]] = new Mesh(Directory + "Star.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[10]] = new Mesh(Directory + "Plus Sign.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[11]] = new Mesh(Directory + "Comma.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[12]] = new Mesh(Directory + "Minus Sign.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[13]] = new Mesh(Directory + "Period and or Dot.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[14]] = new Mesh(Directory + "Forward Slash.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[15]] = new Mesh(Directory + "0.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[16]] = new Mesh(Directory + "1.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[17]] = new Mesh(Directory + "2.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[18]] = new Mesh(Directory + "3.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[19]] = new Mesh(Directory + "4.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[20]] = new Mesh(Directory + "5.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[21]] = new Mesh(Directory + "6.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[22]] = new Mesh(Directory + "7.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[23]] = new Mesh(Directory + "8.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[24]] = new Mesh(Directory + "9.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[25]] = new Mesh(Directory + "Colon.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[26]] = new Mesh(Directory + "Semicolon.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[27]] = new Mesh(Directory + "Less Than.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[28]] = new Mesh(Directory + "Equal.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[29]] = new Mesh(Directory + "Greater Than.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[30]] = new Mesh(Directory + "Question Mark.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[31]] = new Mesh(Directory + "At Sign.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[32]] = new Mesh(Directory + "Capital A.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[33]] = new Mesh(Directory + "Capital B.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[34]] = new Mesh(Directory + "Capital C.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[35]] = new Mesh(Directory + "Capital D.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[36]] = new Mesh(Directory + "Capital E.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[37]] = new Mesh(Directory + "Capital F.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[38]] = new Mesh(Directory + "Capital G.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[39]] = new Mesh(Directory + "Capital H.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[40]] = new Mesh(Directory + "Capital I.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[41]] = new Mesh(Directory + "Capital J.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[42]] = new Mesh(Directory + "Capital K.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[43]] = new Mesh(Directory + "Capital L.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[44]] = new Mesh(Directory + "Capital M.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[45]] = new Mesh(Directory + "Capital N.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[46]] = new Mesh(Directory + "Capital O.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[47]] = new Mesh(Directory + "Capital P.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[48]] = new Mesh(Directory + "Capital Q.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[49]] = new Mesh(Directory + "Capital R.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[50]] = new Mesh(Directory + "Capital S.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[51]] = new Mesh(Directory + "Capital T.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[52]] = new Mesh(Directory + "Capital U.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[53]] = new Mesh(Directory + "Capital V.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[54]] = new Mesh(Directory + "Capital W.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[55]] = new Mesh(Directory + "Capital X.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[56]] = new Mesh(Directory + "Capital Y.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[57]] = new Mesh(Directory + "Capital Z.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[58]] = new Mesh(Directory + "Left Square Bracket.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[59]] = new Mesh(Directory + "Backslash.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[60]] = new Mesh(Directory + "Right Square Bracket.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[61]] = new Mesh(Directory + "Carot.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[62]] = new Mesh(Directory + "Underscore.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[63]] = new Mesh(Directory + "Grave Accent.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[64]] = new Mesh(Directory + "a.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[65]] = new Mesh(Directory + "b.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[66]] = new Mesh(Directory + "c.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[67]] = new Mesh(Directory + "d.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[68]] = new Mesh(Directory + "e.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[69]] = new Mesh(Directory + "f.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[70]] = new Mesh(Directory + "g.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[71]] = new Mesh(Directory + "h.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[72]] = new Mesh(Directory + "i.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[73]] = new Mesh(Directory + "j.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[74]] = new Mesh(Directory + "k.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[75]] = new Mesh(Directory + "l.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[76]] = new Mesh(Directory + "m.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[77]] = new Mesh(Directory + "n.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[78]] = new Mesh(Directory + "o.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[79]] = new Mesh(Directory + "p.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[80]] = new Mesh(Directory + "q.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[81]] = new Mesh(Directory + "r.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[82]] = new Mesh(Directory + "s.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[83]] = new Mesh(Directory + "t.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[84]] = new Mesh(Directory + "u.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[85]] = new Mesh(Directory + "v.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[86]] = new Mesh(Directory + "w.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[87]] = new Mesh(Directory + "x.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[88]] = new Mesh(Directory + "y.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[89]] = new Mesh(Directory + "z.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[90]] = new Mesh(Directory + "left curly brace.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[91]] = new Mesh(Directory + "vertical bar.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[92]] = new Mesh(Directory + "right curly brace.obj", true, true, true);char_iter++;
		Characters[AllAsciiCharactersString.c_str()[93]] = new Mesh(Directory + "Tilde.obj", true, true, true);char_iter++;
		
		//Generate the AABBs
		for (size_t i = 0; i < char_iter; i++){
			float _x = 0;
			float _y = 0;
			float _z = 0;
			bool exists = Characters[AllAsciiCharactersString.c_str()[i]]->getShape().calcAABB(_x,  _y, _z);
			if(exists)
				CharacterAABBDimensions[AllAsciiCharactersString.c_str()[i]] = glm::vec3(_x, _y, _z);
		}
	}
	
	void Font::registerFontToScene(GkScene* inScene){
		if(isnull)
			return;
		for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					inScene->registerMesh(iter.second);
			}
	}
	void Font::deRegisterFontFromScene(GkScene* inScene){
		if(isnull)
			return;
		for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					inScene->deregisterMesh(iter.second);
			}
	}
	
	void Font::pushTextureToAllMeshes(SafeTexture intex){
		for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					(iter.second)->pushTexture(intex);
			}
	}
	void Font::deRegisterInstanceFromMeshes(MeshInstance* Deregme){
		for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					(iter.second)->deRegisterInstance(Deregme);
			}
	}
	void Font::pushCubeMapToAllMeshes(CubeMap* inCubeMap){
		for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					(iter.second)->pushCubeMap(inCubeMap);
			}
	}
	
	void Font::removeTextureFromAllmeshes(GLuint _m_handle){
		//removeTexture(GLuint _tex)
		for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					(iter.second)->removeTexture(_m_handle);
			}
	}
	void Font::removeCubeMapFromAllmeshes(CubeMap* cubey){
		//removeCubeMapByPointer(CubeMap* TheOneTheOnly)
		for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					(iter.second)->removeCubeMapByPointer(cubey);
			}
	}
	
	void Font::pushMaterialPropertiesToAllLetters(Phong_Material Mat, bool instanced_enable_cubemap_reflections, bool instanced_enable_cubemap_diffusion){
		for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
				{
					// Mesh* current = iter.second;
					iter.second->InstancedMaterial = Mat;
					iter.second->instanced_enable_cubemap_reflections = instanced_enable_cubemap_reflections;
					iter.second->instanced_enable_cubemap_diffusion = instanced_enable_cubemap_diffusion;
				}
			}
	}
	
	Font::Font()
	{
		if (!isnull) //Destructor
		{
			for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					delete iter.second;
			}
			Characters.clear();
			CharacterAABBDimensions.clear();
		}
		isnull = true;
	}

	Font::~Font()
	{
		if (!isnull) //REPLACE WITH DESTRUCTOR LATER
		{
			for (auto const& iter : Characters)
			{
				if (iter.second != nullptr)
					delete iter.second;
			}
			Characters.clear();
			CharacterAABBDimensions.clear();
		}
		isnull = true;
	}
};