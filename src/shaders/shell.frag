/* shell.frag */
#version 460 core

in vec2 positionXY;
in flat int layerIndex;
in float layerHeight;
in vec2 layerUV;
in vec3 normal;

out vec4 fragColour;




uniform float layerSpacing;
uniform int numLayers;
uniform vec3 cameraPosition;
uniform vec3 skyColour;




/* CONFIG */
//Dist culling
#define MAX_DISTANCE 4.0f
#define FALLOFF 0.125f

//Colour
#define COLOUR_SCALING 6.0f
#define COLOUR_A    vec4(0.40f, 0.45f, 0.25f, 1.0f)
#define COLOUR_B    vec4(0.30f, 0.35f, 0.10f, 1.0f)
#define BASE_COLOUR vec4(0.44f, 0.33f, 0.24f, 1.0f)

//Misc
#define SCALING 250.0f
const vec3 SUN_DIRECTION = normalize(vec3(0.25f, 0.25f, 1.0f));
#define USE_CYLINDRICAL_SHELLS
#define CYLINDER_DIST 1.0f
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


float getRandom(vec2 UV, float scale) {
	uint seed = uint(UV.x * scale) * 73856093u ^ uint(UV.y * scale) * 19349663u;
	return float(pcg_hash(seed)) / (pow(2.0f, 32.0f) - 1.0f);
}


float getRandomValueXY(vec2 UV, float scale) {
    vec2 scaledPosition = UV * scale;
    vec2 cell = floor(scaledPosition);
    vec2 local = fract(scaledPosition);

    float v00 = getRandom(cell + vec2(0.0, 0.0), 1.0);
    float v10 = getRandom(cell + vec2(1.0, 0.0), 1.0);
    float v01 = getRandom(cell + vec2(0.0, 1.0), 1.0);
    float v11 = getRandom(cell + vec2(1.0, 1.0), 1.0);

    vec2 w = local * local * (3.0f - (2.0f * local));

    return mix(
        mix(v00, v10, w.x),
        mix(v01, v11, w.x),
        w.y
    );
}


void main() {

	vec2 UV = fract(layerUV);
	vec2 fragPosition = gl_FragCoord.xy;


	float distance = length(cameraPosition.xy - positionXY);
	float distScaling = (1.0f - clamp(distance / MAX_DISTANCE, 0.0f, 1.0f));
	float layerDecimal = float(layerIndex) / float(numLayers);
	if (length(cameraPosition - vec3(positionXY.xy, layerHeight)) > MAX_DISTANCE) {
		int maxIDX = int(floor(numLayers + FALLOFF * numLayers * (MAX_DISTANCE - distance)));
		if (maxIDX < layerIndex) {discard;}
		layerDecimal = float(layerIndex) / float(maxIDX);
	}


	//Height discard
	float maxHeight = float(numLayers) * layerSpacing;
	float randomDecimal = getRandom(UV, SCALING);
	float randomHeight = clamp(randomDecimal * maxHeight, 0.0f, maxHeight);
	float layerDelta = layerIndex * layerSpacing; //Difference between the current layer and the base.
	if (layerDelta > randomHeight) {discard;}
	float thisBladeDecimal = layerDelta / randomHeight;
	float baseHeight = layerHeight - layerDelta;


	//Cylindrical discard
#ifdef USE_CYLINDRICAL_SHELLS
	if ((layerIndex > 0) && (distance < CYLINDER_DIST)) { //Layer 0 must be flat, and it must be only "grass" close to player.
		vec2 localPos = fract(UV * SCALING);
		float cylDistScaling = 1.0f - (distance / CYLINDER_DIST);
		float thisLayerRadius = 1.0f - (thisBladeDecimal * randomDecimal) * cylDistScaling;
		float thisLayerDistance = length(localPos - 0.5f);
		if (thisLayerDistance > thisLayerRadius) {discard;}
	}
#endif


	float lightMultiplier = ((layerDecimal * 0.75f) + 0.25f) * dot(SUN_DIRECTION, normal);

	float rand = getRandomValueXY(fract(layerUV), COLOUR_SCALING);
	vec3 thisColour = mix(
		COLOUR_A.rgb,
		COLOUR_B.rgb,
		rand
	);
	vec3 shellColour = mix(BASE_COLOUR.rgb, thisColour.rgb, layerDecimal) * lightMultiplier;
	fragColour = vec4(shellColour.rg, 0.0f, 1.0f);

}