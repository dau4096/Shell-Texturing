/* shell.frag */
#version 460 core

in vec2 positionXY;
in flat int layerIndex;
in flat float layerHeight;
in vec2 layerUV;
in vec3 normal;

out vec4 fragColour;




uniform float layerSpacing;
uniform int numLayers;
uniform vec3 cameraPosition;
uniform vec3 skyColour;




/* CONFIG */
#define SCALING 250.0f
#define MAX_DISTANCE 32.0f
#define SHELL_COLOUR vec4(0.3333f, 0.3725f, 0.1765f, 1.0f)
#define BASE_COLOUR vec4(0.4392f, 0.3294f, 0.2431f, 1.0f)
//#define USE_CYLINDRICAL_SHELLS
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
	float layerDecimal = float(layerIndex) / float(numLayers);


	//Height discard
	float maxHeight = float(numLayers) * layerSpacing;
	uint seed = uint(UV.x * SCALING) * 73856093u ^ uint(UV.y * SCALING) * 19349663u;
	float randomDecimal = float(pcg_hash(seed)) / (pow(2.0f, 32.0f) - 1.0f);
	float randomHeight = clamp(randomDecimal * maxHeight, 0.0f, maxHeight);
	float layerDelta = layerIndex * layerSpacing; //Difference between the current layer and the base.
	if (layerDelta > randomHeight) {discard;}
	float thisBladeDecimal = layerDelta / randomHeight;


	//Cylindrical discard
#ifdef USE_CYLINDRICAL_SHELLS
	if (layerIndex > 0) { //Layer 0 must be flat.
		vec2 localPos = fract(UV * SCALING);
		float thisLayerRadius = 1.0f - (thisBladeDecimal * randomDecimal);
		float thisLayerDistance = length(localPos - 0.5f);
		if (thisLayerDistance > thisLayerRadius) {discard;}
	}
#endif


	float lightMultiplier = (layerDecimal * 0.75f) + 0.25f;
	vec3 shellColour = mix(BASE_COLOUR.rgb, SHELL_COLOUR.rgb, layerDecimal) * lightMultiplier;
	fragColour = vec4(shellColour.rgb, 1.0f);
	//fragColour = vec4(distScaling, 0.0f, 0.0f, 1.0f);

}