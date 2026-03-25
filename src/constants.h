#pragma once

#include "includes.h"
#include <glm/glm.hpp>



namespace constants {
	//Mathematical Constants
	constexpr float PI = 3.141593f;
	constexpr float PI2 = 2.0f * PI;
	constexpr float EXP = 2.718282f;

	constexpr float TO_RAD = 0.017453f;
	constexpr float TO_DEG = 57.29578f;


	constexpr int GRID_WIDTH = 256;
	constexpr float NOISE_FREQ = 0.5f;
	constexpr int NUM_LAYERS = 64;
	constexpr float MAX_HEIGHT = 0.0625f;
	constexpr float LAYER_SPACING = MAX_HEIGHT / float(NUM_LAYERS);
	constexpr glm::vec2 SCALE = glm::vec2(GRID_WIDTH/4, 2.0f);
	constexpr unsigned int NUMBER_OF_SAMPLES = 128u;
	constexpr float BODY_RADIUS = 6357e3f; //6,357km
	constexpr float ATMOS_THICKNESS = 100e3f; //100km


	//Invalid returns for vectors and floats.
	constexpr float INVALID = 1e30f;
	constexpr glm::vec2 INVALIDv2 = glm::vec2(INVALID, INVALID);
	constexpr glm::vec3 INVALIDv3 = glm::vec3(INVALID, INVALID, INVALID);
	constexpr glm::vec4 INVALIDv4 = glm::vec4(INVALID, INVALID, INVALID, INVALID);


	constexpr float MOVE_SPEED_BASE = 0.0125f;
	constexpr float MOVE_SPEED_CROUCH_MULT = 0.25f;
	constexpr float MOVE_SPEED_RUN_MULT = 3.0f;
	constexpr float CAMERA_TURN_SPEED = 0.125f;
}

namespace display {
	//Resolutions
	constexpr glm::ivec2 INITIAL_SCREEN_RESOLUTION = glm::ivec2(960, 540);
	constexpr glm::ivec2 RENDER_RESOLUTION = glm::ivec2(960, 540);

	constexpr float FOV = 70.0f * constants::TO_RAD;
	constexpr float MAX_FREQ = 60.0f;

	constexpr glm::vec3 SKY_COLOUR = glm::vec3(0.4157f, 0.6039f, 0.7098f);
	const glm::vec3 SUN_DIRECTION = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));//glm::normalize(glm::vec3(0.25f, 0.25f, 1.0f));
}

namespace dev {
	//Assorted DEV/DEBUG constants
	constexpr bool SHOW_FREQ_CONSOLE = false;
	constexpr bool SHOW_VALUES = false;
	constexpr bool CAMERA_HAS_PHYSICS = true;
	constexpr bool CAMERA_SLOPE_SLOWDOWN = true;
}