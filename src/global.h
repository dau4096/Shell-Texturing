#pragma once
#include "includes.h"
#include "constants.h"

inline glm::ivec2 currentWindowResolution;
inline glm::ivec2 desiredRenderResolution;
inline glm::ivec2 currentRenderResolution;

inline std::unordered_map<int, bool> keyMap;

inline float verticalFOV;
inline unsigned int frameNumber;

inline int ringCount;


namespace GLIndex {

inline GLuint shellShader, cloudShader, skyShader, displayShader;
inline GLuint shellVAO;

inline GLuint sampleEBO, sampleVAO;

inline GLuint samplesSSBO, ringDataSSBO;

}



namespace structs {

struct Camera {
	glm::vec3 position;
	glm::vec2 viewAngle;
	float FOV, nearZ, farZ;
	float height, vZ;

	Camera(glm::vec3 position, glm::vec2 angle, float FOV, float nearZ, float farZ, float height)
		: position(position), viewAngle(angle), FOV(FOV), nearZ(nearZ), farZ(farZ), height(height), vZ(0.0f) {}
};


struct Sample {
	alignas(16) glm::vec3 direction;
	alignas(16) glm::vec3 colour;

	Sample() : direction(0.0f, 0.0f, 1.0f), colour(1.0f, 0.0f, 1.0f) {}
	Sample(glm::vec3 d, glm::vec3 c) : direction(d), colour(c) {}
};

}



inline structs::Camera camera = structs::Camera(
	glm::vec3(-2.0f, 0.0f, 1.0f), 	//Position
	glm::vec2(1.57f, -0.7854f), 	//Angle
	display::FOV, 0.1f, 96.0f, 		//FOV, zNear, zFar
	0.1875f							//Camera height over floor
);

inline structs::Sample samplesDataset[constants::NUMBER_OF_SAMPLES]; //Holds all samples.
inline std::vector<int> sampleIndices; //Indices for drawing a mesh using the direction attr of samples.
inline std::vector<glm::ivec2> ringDataset; //Data about each sample ring.
