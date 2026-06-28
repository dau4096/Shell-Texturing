#ifndef PHYSICS_H
#define PHYSICS_H

#include "includes.h"
#include "utils.h"

namespace physics {

	void cameraMove();
	
	namespace light {

		glm::vec3 calculateSkyColour(const glm::vec3& sampleDirection, const glm::vec3 sunDirection=display::SUN_DIRECTION);

	}


}

#endif