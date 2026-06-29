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



namespace light {


#define Pr constants::BODY_RADIUS * 1e-3f
#define Ar constants::ATMOS_THICKNESS * 1e-3f

#define VIEW_ORIGIN glm::vec3(0.0f, 0.0f, Pr)
#define NUM_IN_SCATTER_SAMPLES 64u
#define NUM_OPTICAL_DEPTH_SAMPLES 32u
#define DENSITY_FALLOFF 8.0f
#define EXTINCTION 1e-3f
#define SCATTERING_STRENGTH 4.0f
#define MIE_STRENGTH 0.04f
#define MIE_ANISOTROPY 0.9f
#define EXPOSURE (1.0f / 64.0f)


const glm::vec3 wavelengths = glm::vec3(700.0f, 530.0f, 440.0f); //R/G/B wavelengths (nm)
const glm::vec3 scatteringCoefficients = glm::vec3(
	std::pow(400.0f / wavelengths.r, 4),
	std::pow(400.0f / wavelengths.g, 4),
	std::pow(400.0f / wavelengths.b, 4)
) * SCATTERING_STRENGTH;


float rayAtmosphereLength(const glm::vec3& origin, const glm::vec3 direction) {
	//Get length of ray in 3D atmosphere, from arbitrary 3D origin.
	//Guarenteed to be inside the sphere.
	#define atmosRadius (Pr + Ar)

	float b = glm::dot(origin, direction);
	float c = glm::dot(origin, origin) - (atmosRadius * atmosRadius);

	float discriminant = (b*b) - c;
	if (discriminant <= 0.0f) {return 0.0f;}
	return -b + sqrt(discriminant);
}


bool rayHitsPlanet(const glm::vec3& origin, const glm::vec3& dir) {
	//Find if a ray hits the planet.
	float b = glm::dot(origin, dir);
	float c = glm::dot(origin, origin) - (Pr * Pr);
	float disc = (b*b) - c;

	if (disc < 0.0f) {return false;}

	float t = -b - sqrt(disc);

	return (t > 0.0f);
}


float opticalDensityAtPoint(const glm::vec3& point) {
	//Uses altitude to determine atmospheric density, thus optical density.
	float altitude = glm::length(point) - Pr; //Distance from origin (centre of planet) - radius of planet = altitude.
	float altScaled = glm::clamp(altitude / Ar, 0.0f, 1.0f);
	return std::exp(-altScaled * DENSITY_FALLOFF);
}



float opticalDepthAlongRay(const glm::vec3& origin, const glm::vec3 direction, const float length) {
	//Determines optical depth of a ray from multiple samples along its length.
	float stepSize = length / (float)(NUM_OPTICAL_DEPTH_SAMPLES - 1u);
	glm::vec3 densitySamplePoint = origin + direction * (0.5f * stepSize);

	float opticalDepth = 0.0f;	
	for (unsigned int i=0u; i<NUM_OPTICAL_DEPTH_SAMPLES; i++) {
		float localDensity = opticalDensityAtPoint(densitySamplePoint);
		opticalDepth += localDensity * stepSize * EXTINCTION;
		densitySamplePoint += direction * stepSize;
	}

	return opticalDepth;
}



glm::vec3 calculateLight(const glm::vec3& viewDir, const float distanceThroughAtmosphere, const glm::vec3& sunDirection) {
	glm::vec3 inScatterPoint = VIEW_ORIGIN;
	float stepSize = distanceThroughAtmosphere / (float)(NUM_IN_SCATTER_SAMPLES - 1u);
	float mu = glm::dot(viewDir, sunDirection);
	
	glm::vec3 inScatteredLight = glm::vec3(0.0f, 0.0f, 0.0f);
	for (unsigned int i=0u; i<NUM_IN_SCATTER_SAMPLES; i++) {
		//Step through atmos _n_ times and evaluate light contrib at each sample point.
		if (rayHitsPlanet(inScatterPoint, sunDirection)) {
			inScatterPoint += viewDir * stepSize;
			continue;
		}

		float sunRayLength = rayAtmosphereLength(inScatterPoint, sunDirection);
		float sunRayOpticalDepth = opticalDepthAlongRay(inScatterPoint, sunDirection, sunRayLength);
		float viewRayOpticalDepth = opticalDepthAlongRay(inScatterPoint, -viewDir, stepSize * (float)(i));

		glm::vec3 transmittance = glm::exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatteringCoefficients);
		float localDensity = opticalDensityAtPoint(inScatterPoint);

		float rayleighPhase = (3.0f / (16.0f * constants::PI)) * (1.0f + mu * mu);
		glm::vec3 rayleigh = scatteringCoefficients * rayleighPhase;

		float mieAniso2 = MIE_ANISOTROPY*MIE_ANISOTROPY;
		float miePhase = (
			(3.0f * (1.0f - mieAniso2) * (1.0f + mu * mu)) /
			(8.0f * constants::PI *
			(2.0f + mieAniso2) *
			pow(1.0f + mieAniso2 - 2.0f * MIE_ANISOTROPY * mu, 1.5f))
		);
		glm::vec3 mie = glm::vec3(MIE_STRENGTH * miePhase);

		inScatteredLight += localDensity * transmittance * (rayleigh + mie) * stepSize;
		inScatterPoint += viewDir * stepSize;
	}

	return inScatteredLight;
}


glm::vec3 calculateSkyColour(const glm::vec3& sampleDirection, const glm::vec3 sunDirection=display::SUN_DIRECTION) {
	glm::vec3 viewDir = glm::normalize(sampleDirection);
	float sinPitch = viewDir.z; //Sin of pitch is vertical component, 0-1.

	//Camera is always on surface, at "pole" (top) of planetary sphere.
	float distanceThroughAtmosphere = (
		-(Pr*sinPitch) + sqrt((Pr*Pr)*(sinPitch*sinPitch) + (2.0f*Pr*Ar) + (Ar*Ar))
	);


	glm::vec3 light = calculateLight(viewDir, distanceThroughAtmosphere, sunDirection);


	return light * EXPOSURE;
}


}

}