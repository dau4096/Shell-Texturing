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

#define planetCentre glm::vec3(0.0f, 0.0f, -constants::BODY_RADIUS)
#define planetRadius constants::BODY_RADIUS
#define atmosphereRadius (constants::BODY_RADIUS + constants::ATMOS_THICKNESS)
#define densityFalloff 3.19f
#define numOpticalDepthPoints 20
#define numInScatteringPoints 30
#define dirToSun display::SUN_DIRECTION
#define wavelengths glm::vec3(700, 530, 440) /* R/G/B in nm */
#define scatteringStrength 1.0f
#define intensity 2e-3f /* MAY NEED CHANGING */

float densityAtPoint(glm::vec3 densitySamplePoint) {
	float heightAboveSurface = glm::length(densitySamplePoint - planetCentre) - planetRadius;
	float height01 = glm::clamp(heightAboveSurface / (atmosphereRadius - planetRadius), 0.0f, 1.0f);
	float localDensity = glm::exp(-height01 * densityFalloff) * (1.0f - height01);
	return localDensity;
}


float opticalDepth(glm::vec3 rayOrigin, glm::vec3 rayDir, float rayLength) {
	glm::vec3 densitySamplePoint = rayOrigin;
	float stepSize = rayLength / (numOpticalDepthPoints - 1);
	float opticalDepth = 0;


	for (int i = 0; i < numOpticalDepthPoints; i ++) {
		float localDensity = densityAtPoint(densitySamplePoint);
		opticalDepth += localDensity * stepSize;
		densitySamplePoint += rayDir * stepSize;
	}
	return opticalDepth;
}

float raySphere(
    const glm::vec3& center, float radius,
    const glm::vec3& ro, const glm::vec3& rd
) {
    glm::vec3 oc = ro - center;

    float b = glm::dot(oc, rd);
    float c = glm::dot(oc, oc) - radius * radius;
    float h = b*b - c;

    if (h < 0.0f) return -1.0f;

    h = sqrt(h);
    float t = -b - h;
    if (t < 0.0f) t = -b + h;

    return (t < 0.0f) ? -1.0f : t;
}

bool raySphereFull(
    const glm::vec3& center, float radius,
    const glm::vec3& ro, const glm::vec3& rd,
    float& t0, float& t1
) {
    glm::vec3 oc = ro - center;

    float b = glm::dot(oc, rd);
    float c = glm::dot(oc, oc) - radius * radius;
    float h = b*b - c;

    if (h < 0.0f) return false;

    h = sqrt(h);
    t0 = -b - h;
    t1 = -b + h;

    return true;
}

glm::vec3 calculateLight(glm::vec3 rayOrigin, glm::vec3 rayDir, float rayLength, glm::vec3 originalCol) {
	//Taken from [https://github.com/SebLague/Solar-System/blob/0c60882be69b8e96d6660c28405b9d19caee76d5/Assets/Scripts/Celestial/Shaders/PostProcessing/Atmosphere.shader#L122-L159].
	glm::vec3 inScatterPoint = rayOrigin;
	float stepSize = rayLength / (numInScatteringPoints - 1);
	glm::vec3 inScatteredLight = glm::vec3(0.0f);
	float viewRayOpticalDepth = 0;


	glm::vec3 scatteringCoefficients = glm::vec3(
		glm::pow(400.0f / wavelengths.x, 4),
		glm::pow(400.0f / wavelengths.y, 4),
		glm::pow(400.0f / wavelengths.z, 4)
	) * scatteringStrength;


	for (int i = 0; i < numInScatteringPoints; i ++) {
		float sunRayLength = raySphere(planetCentre, atmosphereRadius, inScatterPoint, dirToSun);
		float sunRayOpticalDepth = opticalDepth(inScatterPoint, dirToSun, sunRayLength);
		float localDensity = densityAtPoint(inScatterPoint);
		viewRayOpticalDepth = opticalDepth(rayOrigin, rayDir, stepSize * i);
		glm::vec3 transmittance = glm::exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatteringCoefficients);
		
		inScatteredLight += localDensity * transmittance;
		inScatterPoint += rayDir * stepSize;
	}
	//inScatteredLight *= scatteringCoefficients * intensity * stepSize / planetRadius;


	// Attenuate brightness of original col (i.e light reflected from planet surfaces)
	// This is a hacky mess, TODO: figure out a proper way to do this
	const float brightnessAdaptionStrength = 0.15;
	const float reflectedLightOutScatterStrength = 3;
	float brightnessAdaption = glm::dot(inScatteredLight, glm::vec3(1)) * brightnessAdaptionStrength;
	float brightnessSum = viewRayOpticalDepth * intensity * reflectedLightOutScatterStrength + brightnessAdaption;
	float reflectedLightStrength = glm::exp(-brightnessSum);
	float hdrStrength = glm::clamp(glm::dot(originalCol, glm::vec3(1))/3-1, 0.0f, 1.0f);
	reflectedLightStrength = glm::mix(reflectedLightStrength, 1.0f, hdrStrength);
	glm::vec3 reflectedLight = originalCol * reflectedLightStrength;


	glm::vec3 finalCol = inScatteredLight;


	
	return finalCol;
}

glm::vec3 calculateSkyColour(glm::vec3 viewDir) {
	float t0, t1;

	if (!raySphereFull(planetCentre, atmosphereRadius, camera.position, viewDir, t0, t1)) {
	    return glm::vec3(0.0f); // ray misses atmosphere
	}

	// If camera is inside atmosphere:
	float startT = glm::max(t0, 0.0f);
	float endT   = t1;

	if (endT <= startT) return glm::vec3(0.0f);

	glm::vec3 startPoint = camera.position + viewDir * startT;
	float rayLength = endT - startT;

	std::cout << rayLength << std::endl;
	glm::vec3 result = calculateLight(
	    startPoint,
	    viewDir,
	    rayLength,
	    glm::vec3(1.0f)
	);
	std::cout << std::format(
		"Dir: ({}, {}, {}),    Colour: ({}, {}, {})",
		viewDir.x, viewDir.y, viewDir.z,
		result.r, result.g, result.b
	) << std::endl;
	return result;
}

/*
glm::vec3 calculateSkyColour(glm::vec3 viewDir) {
    glm::vec3 up = glm::vec3(0,0,1);
    float mu = glm::dot(viewDir, up);                // cos(view zenith)
    float muS = glm::dot(display::SUN_DIRECTION, up);        // cos(sun zenith)
    float nu = glm::clamp(glm::dot(viewDir, display::SUN_DIRECTION), -1.0f, 1.0f);

    // Distances to top of atmosphere
    float R = constants::BODY_RADIUS + constants::ATMOS_THICKNESS;
    float dA = sqrt(R*R - constants::BODY_RADIUS*constants::BODY_RADIUS*(1 - mu*mu)) - constants::BODY_RADIUS*mu;
    float dSun = sqrt(R*R - constants::BODY_RADIUS*constants::BODY_RADIUS*(1 - muS*muS)) - constants::BODY_RADIUS*muS;

    // Optical depths
    float H_R = 8000.0f;
    float H_M = 1200.0f;
    glm::vec3 betaR = glm::vec3(5.8e-6f,13.5e-6f,33.1e-6f);
    glm::vec3 betaM = glm::vec3(1e-6f);

    float tauR = H_R*(1 - exp(-dA/H_R)) / glm::max(mu, 0.01f)
               + H_R*(1 - exp(-dSun/H_R)) / glm::max(muS, 0.01f);
    float tauM = H_M*(1 - exp(-dA/H_M)) / glm::max(mu, 0.01f)
               + H_M*(1 - exp(-dSun/H_M)) / glm::max(muS, 0.01f);

    // Phase functions
    float phaseR = 0.75f * (1 + nu*nu);  // scaled for visibility
    float g = 0.76f;
    float phaseM = (1-g*g)/(pow(1+g*g-2*g*nu,1.5f));

	// Compute optical depth along view ray (dA) and sun ray (dSun)
	float tauR_view = H_R*(1 - exp(-dA/H_R)) / glm::max(mu, 0.01f);
	float tauM_view = H_M*(1 - exp(-dA/H_M)) / glm::max(mu, 0.01f);

	// Compute extinction along the view ray
	glm::vec3 extinction = glm::exp(-(betaR*tauR_view + betaM*tauM_view));

	// Direct scattering along view ray
	glm::vec3 color = (betaR*phaseR + betaM*phaseM) * extinction;

	// Add simple airlight contribution
	float airlightFactor = 1.0f - exp(-(betaR*tauR_view + betaM*tauM_view).r);
	glm::vec3 airlight = (betaR*phaseR + betaM*phaseM) * airlightFactor;
	color += airlight;

	// Tone mapping / scale
	color *= glm::vec3(1.0f,0.98f,0.9f) * 2e5f;
	return color / (color + glm::vec3(1.0f));
}
*/


}

}