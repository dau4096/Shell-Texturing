/* clouds.frag */
#version 460 core

in vec2 fragPosition;
layout(location=0) out vec4 outAlbedo;
layout(location=1) out vec2 outData;

uniform int frameNumber;
uniform float frameRate;
uniform vec3 skyColour;
uniform vec3 cameraPosition;
layout(binding=0) uniform sampler2D prevData;

//Pre-processor step replaces these with the entire contents of the files "constants.glsl", "noise.glsl" and "clouds.shared.glsl".
//Lets me share values/functions between shaders when necessary, without repeating their code.
#include <constants>
#include <noise>
#include <clouds.shared>



void main() {
	//outAlbedo = vec4(1.0f, 1.0f, 1.0f, alpha); //Blend using alpha.

	outData = texture(prevData, (gl_FragCoord.xy)).rg;

	float distanceFromCamera2D = length(fragPosition.xy - cameraPosition.xy);
	if (distanceFromCamera2D > MAX_DISTANCE_FROM_CAMERA) {discard;}
	float distanceDecimal = 1.0f - clamp(distanceFromCamera2D / MAX_DISTANCE_FROM_CAMERA, 0.0f, 1.0f);

	float alpha = getCloudValueForPosition(fragPosition, (frameNumber / frameRate));
	alpha *= distanceDecimal;


#ifdef DEBUG_CLOUDS
	outAlbedo = vec4(alpha, alpha/distanceDecimal, float(alpha <= CLOUD_EPSILON), 1.0f);
#else
	outAlbedo = vec4(mix(
			skyColour,
			CLOUD_COLOUR,
			alpha
		),
		1.0f
	);
#endif

	if (alpha > CLOUD_EPSILON) {
		outData = vec2(T_CLOUD, alpha); //Write that it was a cloud collis.
	}
}
