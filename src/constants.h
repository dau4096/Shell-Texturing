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


	constexpr int NUM_LAYERS = 64;
	constexpr float LAYER_SPACING = 0.0025f;


	//Invalid returns for vectors and floats.
	constexpr float INVALID = 1e30f;
	constexpr glm::vec2 INVALIDv2 = glm::vec2(INVALID, INVALID);
	constexpr glm::vec3 INVALIDv3 = glm::vec3(INVALID, INVALID, INVALID);
	constexpr glm::vec4 INVALIDv4 = glm::vec4(INVALID, INVALID, INVALID, INVALID);


	constexpr float MOVE_SPEED_BASE = 0.025f;
	constexpr float MOVE_SPEED_CROUCH_MULT = 0.5f;
	constexpr float MOVE_SPEED_RUN_MULT = 3.0f;

	constexpr float CAMERA_TURN_SPEED = 0.125f;
}

namespace display {
	//Resolutions
	constexpr glm::ivec2 INITIAL_SCREEN_RESOLUTION = glm::ivec2(960, 540);
	constexpr glm::ivec2 RENDER_RESOLUTION = glm::ivec2(960, 540);

	constexpr float FOV = 70.0f * constants::TO_RAD;
	constexpr float MAX_FREQ = 60.0f;
}

namespace dev {
	//Assorted DEV/DEBUG constants
	constexpr bool SHOW_FREQ_CONSOLE = false;
	constexpr bool SHOW_VALUES = false;
}