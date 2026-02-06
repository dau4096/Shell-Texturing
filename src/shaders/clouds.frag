#version 460 core

in vec2 fragPosition;
out vec4 fragColour;

uniform int frameNumber;
uniform float frameRate;
uniform vec3 skyColour;
uniform vec3 cameraPosition;

//Pre-processor step replaces these with the entire contents of the files "constants.glsl", "noise.glsl" and "clouds.shared.glsl".
//Lets me share values/functions between shaders when necessary, without repeating their code.
#include <constants>
#include <noise>
#include <clouds.shared>



void main() {
	//fragColour = vec4(1.0f, 1.0f, 1.0f, alpha); //Blend using alpha.
	float distanceFromCamera2D = length(fragPosition.xy - cameraPosition.xy);
	if (distanceFromCamera2D > MAX_DISTANCE_FROM_CAMERA) {discard;}
	float distanceDecimal = 1.0f - clamp(distanceFromCamera2D / MAX_DISTANCE_FROM_CAMERA, 0.0f, 1.0f);

	float alpha = getCloudValueForPosition(fragPosition, (frameNumber / frameRate));
	alpha *= distanceDecimal;


#ifdef DEBUG_CLOUDS
	fragColour = vec4(alpha, alpha/distanceDecimal, 0.0f, 1.0f);
#else
	fragColour = vec4(mix(
			skyColour,
			CLOUD_COLOUR,
			alpha
		),
		1.0f
	);
#endif
}
