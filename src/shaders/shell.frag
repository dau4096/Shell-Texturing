/* shell.frag */
#version 460 core

in vec2 positionXY;
in flat int layerIndex;
in flat float layerHeight;
in vec2 layerUV;

out vec4 fragColour;

uniform float layerSpacing;
uniform int numLayers;
uniform vec3 cameraPosition;



/* CONFIG */
#define SCALING 250.0f
#define MAX_DISTANCE 16.0f
#define SHELL_COLOUR vec4(0.3333f, 0.3725f, 0.1765f, 1.0f)
#define BASE_COLOUR vec4(0.4392f, 0.3294f, 0.2431f, 1.0f)
/* CONFIG */



uint pcg_hash(uint seed) {
	/*
	Hash function taken from;
	https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
	*/
    uint state = seed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}


void main() {

	vec2 UV = fract(layerUV);
	vec2 fragPosition = gl_FragCoord.xy;

	float distance = length(cameraPosition.xy - positionXY);
	float distScaling = (1.0f - clamp(distance / MAX_DISTANCE, 0.0f, 1.0f));
	float trueScaling = distScaling * SCALING;

	float maxHeight = float(numLayers) * layerSpacing;
	uint seed = uint(UV.x * trueScaling) * 73856093u ^ uint(UV.y * trueScaling) * 19349663u;
	float randomDecimal = float(pcg_hash(seed)) / (pow(2.0f, 32.0f) - 1.0f);
	float randomHeight = clamp(randomDecimal * maxHeight * distScaling, 0.0f, maxHeight);

	if (layerHeight > randomHeight) {discard;}

	float layerDecimal = float(layerIndex) / float(numLayers);
	vec3 colour = mix(BASE_COLOUR.rgb, SHELL_COLOUR.rgb, layerDecimal);
	fragColour = vec4(colour.rgb * layerDecimal, 1.0f);

}