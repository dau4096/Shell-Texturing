#ifndef PHYSICS_H
#define PHYSICS_H

#include "includes.h"
#include "utils.h"

namespace physics {

	void cameraMove();
	
	namespace light {

		glm::vec3 calculateSkyColour(glm::vec3 sampleDirection);

	}


}

#endif