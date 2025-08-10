/* shell.frag */
#version 460 core

in flat int layerIndex;
in flat float layerHeight;
in vec2 layerUV;

out vec4 fragColour;

uniform float layerSpacing;
uniform int numLayers;
#define scaling 100.0f


#define SHELL_COLOUR vec4(1.0f, 1.0f, 1.0f, 1.0f)


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

	vec2 fragPosition = gl_FragCoord.xy;

	float maxHeight = float(numLayers) * layerSpacing;
	uint seed = uint(layerUV.x * scaling) * 73856093u ^ uint(layerUV.y * scaling) * 19349663u;
	float randomDecimal = float(pcg_hash(seed)) / (pow(2.0f, 32.0f) - 1.0f);
	float randomHeight = randomDecimal * maxHeight;

	if (layerHeight > randomHeight) {discard;}

	float layerDecimal = float(layerIndex) / float(numLayers);
	fragColour = vec4(SHELL_COLOUR.rgb * layerDecimal, 1.0f);

}