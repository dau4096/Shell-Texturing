/* clouds.frag */
#version 460 core

in vec2 fragPosition;
out vec4 fragColour;

uniform int frameNumber;
uniform float frameRate;
uniform vec3 skyColour;
uniform vec3 cameraPosition;
uniform int numberOfRings;

//Pre-processor step replaces these with the entire contents of the files "constants.glsl", "noise.glsl" and "clouds.shared.glsl".
//Lets me share values/functions between shaders when necessary, without repeating their code.
#include <constants>
#include <noise>
#include <clouds.shared>
#define SAMPLE_FUNCS
#include <sample>



void main() {

#ifndef HAS_CLOUDS
	return;
#endif

	float distanceFromCamera2D = length(fragPosition.xy - cameraPosition.xy);
	if (distanceFromCamera2D > MAX_DISTANCE_FROM_CAMERA) {discard;}
	float distanceDecimal = 1.0f - clamp(distanceFromCamera2D / MAX_DISTANCE_FROM_CAMERA, 0.0f, 1.0f);

	float alpha = getCloudValueForPosition(fragPosition - cameraPosition.xy, (frameNumber / frameRate));
	alpha *= distanceDecimal;


#ifdef DEBUG_CLOUDS
	fragColour = vec4(alpha, alpha/distanceDecimal, float(alpha <= CLOUD_EPSILON), 1.0f);
#else

	vec3 cloudViewDirection = normalize(vec3((fragPosition.xy - cameraPosition.xy), CLOUD_HEIGHT));
	vec3 skyColour = sampleHemisphereHorizontal(cloudViewDirection);
	vec3 cloudColour = mix(
		skyColour, CLOUD_COLOUR, dot(cloudViewDirection, vec3(0.0f, 0.0f, 1.0f)) //As it approaches vertical, get more white.
	);

	fragColour = vec4(cloudColour.rgb, alpha);
#endif
}
