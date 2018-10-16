#include "SafeTexture.h"
#include <iostream>
#include <cassert>
namespace GeklminRender {
SafeTexture::SafeTexture(GLuint texture_object)
{
	m_texture = texture_object;
	DaddyO = nullptr;
	TextureUpdates = false;
}

SafeTexture::SafeTexture(Texture* inTex) //What texture should this be a copy of?
{
	if (inTex)
	{m_texture = inTex->getHandle(); isTransparent = inTex->amITransparent();
	TextureUpdates = true;
	DaddyO = inTex;

	}
	else
	{isnull = true;}
}

void SafeTexture::Bind(unsigned int unit)
{
	if (DaddyO && TextureUpdates)
		m_texture = DaddyO->getHandle();
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void SafeTexture::BindGeneric(){
	if (DaddyO && TextureUpdates)
		m_texture = DaddyO->getHandle();
	glBindTexture(GL_TEXTURE_2D, m_texture);
}
void SafeTexture::SetActiveUnit(unsigned int unit){
	assert(unit >= 0 && unit <= 31);
	glActiveTexture(GL_TEXTURE0 + unit);
}
};