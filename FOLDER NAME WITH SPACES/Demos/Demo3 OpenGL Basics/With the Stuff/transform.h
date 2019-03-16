#ifndef TRANSFORM_INCLUDED_H
#define TRANSFORM_INCLUDED_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
//Note: Const reference means we don't make copies for the functions.

namespace GeklminRender {
struct Transform
{
public:
	//void CopyTransform(Transform other){
	//	temppos = *other.GetPos();
	//	tempquatrot = *other.GetRotQUAT();
	//	tempscale = *other.GetScale(); // This annoying FUCKING camelcase...
	//}
	Transform(){ //Empty constructor.
	}
	
	Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)
	{
		glm::mat4 posMat = glm::translate(pos);
		glm::mat4 scaleMat = glm::scale(scale);
		glm::mat4 rotMat = glm::toMat4(glm::quat(rot));
		model = posMat * rotMat * scaleMat;
	}
	
	void reTransform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)
	{
		glm::mat4 posMat = glm::translate(pos);
		glm::mat4 scaleMat = glm::scale(scale);
		glm::mat4 rotMat = glm::toMat4(glm::quat(rot));
		model = posMat * rotMat * scaleMat;
		return;
	}
	//The Quaternion Alternative
	Transform(const glm::vec3& pos, const glm::quat& quatrot, const glm::vec3& scale)
	{
		glm::mat4 posMat = glm::translate(pos);
		glm::mat4 scaleMat = glm::scale(scale);
		glm::mat4 rotMat = glm::toMat4(quatrot);
		model = posMat * rotMat * scaleMat;
	}

	
	glm::mat4 GetModel() const //Returns a constant.
	{
		return model;
	}
	void setModel(glm::mat4 newmodel){ //Setting the model straight, e.g. from bullet physics
		model = newmodel;
	}

	 glm::vec3 GetPos() {
		glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		return temppos; 
	 }
	 glm::vec3 GetRot() {
		glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		temp3rot = glm::eulerAngles(tempquatrot); 
		return temp3rot;
	}
	 glm::quat GetRotQUAT() {
		glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		return tempquatrot;
	 }
	 glm::vec3 GetScale() {
		glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		return tempscale; 
	 }

	 void SetPos(glm::vec3 pos) {
		glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		temppos = pos;
		//Note we are getting rid of skew and perspective transformations...
		glm::mat4 posMat = glm::translate(temppos);
		glm::mat4 scaleMat = glm::scale(tempscale);
		glm::mat4 rotMat = glm::toMat4(tempquatrot);
		model = posMat * rotMat * scaleMat;
	 }
	 void SetRot(glm::vec3 rot) {
		glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		tempquatrot = glm::quat(rot);
		//Note we are getting rid of skew and perspective transformations...
		glm::mat4 posMat = glm::translate(temppos);
		glm::mat4 scaleMat = glm::scale(tempscale);
		glm::mat4 rotMat = glm::toMat4(tempquatrot);
		model = posMat * rotMat * scaleMat;
	 }
	 void SetRotMat4(glm::mat4 rot)
	 {
		 glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		tempquatrot = glm::quat(rot);
		//Note we are getting rid of skew and perspective transformations...
		glm::mat4 posMat = glm::translate(temppos);
		glm::mat4 scaleMat = glm::scale(tempscale);
		//glm::mat4 rotMat = rot;
		model = posMat * rot * scaleMat;
	 }
	 void SetRotQuat(glm::quat rot) {
		glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		tempquatrot = rot;
		//Note we are getting rid of skew and perspective transformations...
		glm::mat4 posMat = glm::translate(temppos);
		glm::mat4 scaleMat = glm::scale(tempscale);
		glm::mat4 rotMat = glm::toMat4(tempquatrot);
		model = posMat * rotMat * scaleMat;
	 }
	 void SetScale(glm::vec3 scale) {
		glm::decompose(model, tempscale, tempquatrot, temppos, tempskew, tempperspective);
		tempscale = scale;
		//Note we are getting rid of skew and perspective transformations...
		glm::mat4 posMat = glm::translate(temppos);
		glm::mat4 scaleMat = glm::scale(tempscale);
		glm::mat4 rotMat = glm::toMat4(tempquatrot);
		model = posMat * rotMat * scaleMat;
	 }
protected:
private:
	//These are used merely to temporarily store data
	glm::vec3 temppos = glm::vec3();
	glm::vec3 temp3rot = glm::vec3();
	glm::quat tempquatrot = glm::quat();
	glm::vec3 tempscale = glm::vec3(1,1,1);
	glm::vec3 tempskew = glm::vec3();
	glm::vec4 tempperspective = glm::vec4();
	//This is the actual information
	glm::mat4 model = glm::mat4(); //The model we want to send.
};
}; //Eof Namespace
#endif
