

#include "geklminrender.h"

namespace GeklminRender{
	//To be programmed...
	IndexedModel createSphere(float radius, float longitudinal_complexity, float latitudinal_complexity){
		return IndexedModel();
	}
	IndexedModel createBox(float Xdim, float Ydim, float Zdim){ //Partially complete
		unsigned int index = 0;
		IndexedModel returnval = IndexedModel();
		//std::cout << "\n Made IndexedModel";
		//Side 1, Triangle 1, Facing toward -Y
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, Zdim)); // Upper Right Point
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, Zdim)); // Upper Left Point
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, -Zdim)); // bottom Left Point
		//Push back 3! (UPPER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		//Side 1, Triangle 2, Facing toward -Y
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, -Zdim)); // bottom Left Point
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, -Zdim)); // bottom Right Point
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, Zdim)); // Upper Right Point
		//Push back 3! (LOWER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		
		//The exact same thing but +Ydim instead of -Ydim, and one dimension has to be flipped
		
		//Side 2, Triangle 1, Facing toward +Y
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, Zdim)); // Upper Right Point
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, Zdim)); // Upper Left Point
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, -Zdim)); // bottom Left Point
		//Push back 3! (UPPER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		//Side 2, Triangle 2, Facing toward +Y
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, -Zdim)); // bottom Left Point
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, -Zdim)); // bottom Right Point
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, Zdim)); // Upper Right Point
		//Push back 3! (LOWER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		
		
		
		//Now, we swap Zdim and Ydim for switching around...
		
		
		//Side 3, Triangle 1, Facing toward Z
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, Zdim)); // Upper Right Point
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, Zdim)); // Upper Left Point
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, Zdim)); // bottom Left Point
		//Push back 3! (UPPER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		//Side 1, Triangle 2, Facing toward Z
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, Zdim)); // bottom Left Point
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, Zdim)); // bottom Right Point
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, Zdim)); // Upper Right Point
		//Push back 3! (LOWER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		
		//The exact same thing but +Zdim instead of -Zdim, and one dimension has to be flipped
		
		//Side 3, Triangle 1, Facing toward -Z
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, -Zdim)); // Upper Right Point
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, -Zdim)); // Upper Left Point
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, -Zdim)); // bottom Left Point
		//Push back 3! (UPPER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		//Side 3, Triangle 2, Facing toward -Z
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, -Zdim)); // bottom Left Point
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, -Zdim)); // bottom Right Point
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, -Zdim)); // Upper Right Point
		//Push back 3! (LOWER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		
		//Now, we return Z to before, but X is swapped with Y
		
		//Side 1, Triangle 1, Facing toward -Y
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, Zdim)); // Upper Right Point
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, Zdim)); // Upper Left Point
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, -Zdim)); // bottom Left Point
		//Push back 3! (UPPER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		//Side 1, Triangle 2, Facing toward +X
		returnval.positions.push_back(glm::vec3(Xdim, -Ydim, -Zdim)); // bottom Left Point
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, -Zdim)); // bottom Right Point
		returnval.positions.push_back(glm::vec3(Xdim, Ydim, Zdim)); // Upper Right Point
		//Push back 3! (LOWER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		
		//The exact same thing but +Ydim instead of -Ydim, and one dimension has to be flipped
		
		//Side 6, Triangle 1, Facing toward -X
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, Zdim)); // Upper Right Point
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, Zdim)); // Upper Left Point
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, -Zdim)); // bottom Left Point
		//Push back 3! (UPPER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		//Side 6, Triangle 2, Facing toward -X
		returnval.positions.push_back(glm::vec3(-Xdim, Ydim, -Zdim)); // bottom Left Point
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, -Zdim)); // bottom Right Point
		returnval.positions.push_back(glm::vec3(-Xdim, -Ydim, Zdim)); // Upper Right Point
		//Push back 3! (LOWER TRI)
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(0,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,0));returnval.normals.push_back(glm::vec3(0,0,0));
		returnval.indices.push_back(index);index++;returnval.colors.push_back(glm::vec3(0,0,0));returnval.texCoords.push_back(glm::vec2(1,-1));returnval.normals.push_back(glm::vec3(0,0,0));
		
		
		//Finally, Calcnormals and get out of here!
		//std::cout << "\n Before Calcnormals";
		returnval.CalcNormals();
		//std::cout << "\n Made a Box!!!";
		return returnval;
	};
	
	
	
	glm::quat faceTowardPoint(glm::vec3 pos, glm::vec3 target, glm::vec3 up){
		glm::vec3 defaultfacingvector = glm::vec3(0,0,-1);
		//glm::vec3 facingvector = target-ourpoint;
		return glm::quat(glm::inverse(glm::lookAt(pos, target, up)));
	}//Autoface for Sprites
	
	
}; //Eof Namespace