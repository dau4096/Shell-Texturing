#pragma once
#include "includes.h"
#include "constants.h"

inline glm::ivec2 currentWindowResolution;
inline glm::ivec2 desiredRenderResolution;
inline glm::ivec2 currentRenderResolution;

inline std::unordered_map<int, bool> keyMap;

inline float verticalFOV;
inline unsigned int frameNumber;


namespace GLIndex {

inline GLuint shellShader;
inline GLuint shellVAO;

}



namespace structs {

struct Camera {
	glm::vec3 position;
	glm::vec2 viewAngle;
	float FOV, nearZ, farZ;

	Camera(glm::vec3 position, glm::vec2 angle, float FOV, float nearZ, float farZ)
		: position(position), viewAngle(angle), FOV(FOV), nearZ(nearZ), farZ(farZ) {}
};

}



inline structs::Camera camera = structs::Camera(
	glm::vec3(-2.0f, 0.0f, 1.0f), 	//Position
	glm::vec2(1.57f, -0.7854f), 	//Angle
	display::FOV, 0.1f, 32.0f 		//FOV, zNear, zFar
);