#include "includes.h"
#include "global.h"
#include "utils.h"
#include "noise.h"
using namespace std;
using namespace utils;






float getNoiseValueForPosition(glm::vec2 position2D) {
	//Get the current noise height value for this position.
	glm::vec2 noisePos = position2D * constants::NOISE_FREQ;
	return noise::heightFunction(noisePos) * constants::SCALE.y;
}





namespace physics {

void cameraMove() {
	//Speed modifier keys
	float cameraSpeed = constants::MOVE_SPEED_BASE;
	float cameraHeight = camera.height;
	if (keyMap[GLFW_KEY_LEFT_CONTROL]) {
		cameraSpeed *= constants::MOVE_SPEED_CROUCH_MULT;
		cameraHeight /= 2.0f;
	} else if (keyMap[GLFW_KEY_LEFT_SHIFT]) {
		cameraSpeed *= constants::MOVE_SPEED_RUN_MULT;
	}


	if constexpr (dev::CAMERA_HAS_PHYSICS && dev::CAMERA_SLOPE_SLOWDOWN) {
		glm::vec3 surfaceNormal = noise::computeNormal(glm::vec2(camera.position));
		float dotUp = glm::max(glm::dot(surfaceNormal, glm::vec3(0.0f, 0.0f, 1.0f)), 0.0f); //0-1 of if slope is climbable.
		cameraSpeed *= dotUp * dotUp;
	}


	//Horizontal movement
	if (keyMap[GLFW_KEY_W]) {
		camera.position.x += cameraSpeed * sin(camera.viewAngle.x);
		camera.position.y += cameraSpeed * cos(camera.viewAngle.x);
	}
	if (keyMap[GLFW_KEY_S]) {
		camera.position.x -= cameraSpeed * sin(camera.viewAngle.x);
		camera.position.y -= cameraSpeed * cos(camera.viewAngle.x);
	}
	if (keyMap[GLFW_KEY_A]) {
		camera.position.x -= cameraSpeed *  cos(camera.viewAngle.x);
		camera.position.y -= cameraSpeed * -sin(camera.viewAngle.x);
	}
	if (keyMap[GLFW_KEY_D]) {
		camera.position.x += cameraSpeed *  cos(camera.viewAngle.x);
		camera.position.y += cameraSpeed * -sin(camera.viewAngle.x);
	}


	if constexpr (dev::CAMERA_HAS_PHYSICS) {
		float zAtPosition = getNoiseValueForPosition(glm::vec2(camera.position));
		camera.position.z = zAtPosition + cameraHeight;

	} else {
		//Vertical movement.
		if (keyMap[GLFW_KEY_E]) {
			camera.position.z += cameraSpeed;
		}
		if (keyMap[GLFW_KEY_Q]) {
			camera.position.z -= cameraSpeed;
		}		
	}
};


}