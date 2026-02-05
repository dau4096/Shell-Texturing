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
uniform int frameNumber;
uniform float frameRate;




/* CONFIG */
//Distance culling
#define MAX_DISTANCE_FROM_CAMERA 32.0f
#define FALLOFF 0.125f
//Distance culling

//Colour
#define COLOUR_SCALING 6.0f
#define COLOUR_A    vec4(0.325f, 0.375f, 0.125f, 1.0f)
#define COLOUR_B    vec4(0.40f, 0.40f, 0.15f, 1.0f)
#define BASE_COLOUR vec4(0.44f, 0.33f, 0.24f, 1.0f)
//Colour

//Wind
#define HAS_WIND
//#define DEBUG_WIND
const vec2 WIND_DIRECTION = normalize(vec2(1.0f, 0.25f));
#define WIND_STRENGTH 6.0f
#define WIND_SPEED 0.75f
//Wind

//Drawing
#define SCALING 250.0f
#define MIN_BLADE_HEIGHT_SCALE 0.325f
float MIN_BLADE_HEIGHT = 0.0f;
const vec3 SUN_DIRECTION = normalize(vec3(0.25f, 0.25f, 1.0f));
#define HAS_CONICAL_SHELLS
#define CONE_RENDER_DIST 1.0f
//Drawing
/* CONFIG */







//// PERLIN NOISE ////
//Src: [https://github.com/stegu/webgl-noise]
vec4 mod289(vec4 x)
{
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
	return mod289(((x*34.0)+10.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
	return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec2 P)
{
	vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
	vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
	Pi = mod289(Pi); // To avoid truncation effects in permutation
	vec4 ix = Pi.xzxz;
	vec4 iy = Pi.yyww;
	vec4 fx = Pf.xzxz;
	vec4 fy = Pf.yyww;

	vec4 i = permute(permute(ix) + iy);

	vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
	vec4 gy = abs(gx) - 0.5 ;
	vec4 tx = floor(gx + 0.5);
	gx = gx - tx;

	vec2 g00 = vec2(gx.x,gy.x);
	vec2 g10 = vec2(gx.y,gy.y);
	vec2 g01 = vec2(gx.z,gy.z);
	vec2 g11 = vec2(gx.w,gy.w);

	vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));

	float n00 = norm.x * dot(g00, vec2(fx.x, fy.x));
	float n01 = norm.y * dot(g01, vec2(fx.z, fy.z));
	float n10 = norm.z * dot(g10, vec2(fx.y, fy.y));
	float n11 = norm.w * dot(g11, vec2(fx.w, fy.w));

	vec2 fade_xy = fade(Pf.xy);
	vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
	float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
	return 2.3 * n_xy;
}
//// PERLIN NOISE ////




//// RANDOM NOISE ////
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
//// RANDOM NOISE ////







//// SHELL TEXTURING ////
void distanceFromCamera2DCulling(out float distanceFromCamera2D, inout float layerDecimal) {
	distanceFromCamera2D = length(cameraPosition.xy - positionXY);
	float distScaling = (1.0f - clamp(distanceFromCamera2D / MAX_DISTANCE_FROM_CAMERA, 0.0f, 1.0f));
	if (length(cameraPosition - vec3(positionXY.xy, layerHeight)) > MAX_DISTANCE_FROM_CAMERA) {
		int maxIDX = int(floor(numLayers + FALLOFF * numLayers * (MAX_DISTANCE_FROM_CAMERA - distanceFromCamera2D)));
		if (maxIDX < layerIndex) {discard;}
		layerDecimal = float(layerIndex) / float(maxIDX);
	}
}


void heightDiscard(
	vec2 UV, float layerDelta, float maxHeight,
	out float randomDecimal, out float randomHeight
) {
	//Height discard.
	randomDecimal = getRandom(abs(UV), SCALING);
	randomHeight = clamp(randomDecimal * maxHeight, 0.0f, maxHeight);
	if (layerDelta > randomHeight) {discard; /* Shell is above top of blade of grass. */}
	if ((randomHeight < MIN_BLADE_HEIGHT) && (layerIndex > 0)) {discard; /* Blade too short, ignore. */}
}


void conicalDiscard(
	vec2 UV, float layerDelta, float distanceFromCamera2D,
	float randomDecimal, float randomHeight
) {
#ifdef HAS_CONICAL_SHELLS
	if ((layerIndex > 0) && (distanceFromCamera2D < CONE_RENDER_DIST)) { //Layer 0 must be flat, and it must be only "grass" close to player.
		vec2 localPos = fract(UV * SCALING);
		float cylDistScaling = 1.0f - (distanceFromCamera2D / CONE_RENDER_DIST);
		float thisBladeDecimal = layerDelta / randomHeight;
		float thisLayerRadius = 1.0f - (thisBladeDecimal * cylDistScaling);
		float thisLayerdistanceFromCamera2D = length(localPos - 0.5f);
		if (thisLayerdistanceFromCamera2D > thisLayerRadius) {discard;}
	}
#endif
}


vec2 getWindOffset(float layerDecimal) {
	float lengthScaling = layerDecimal*layerDecimal; //Defines how intense the wind's effect is, depending on where on the blade it is.
	float windMult = lengthScaling * WIND_STRENGTH * (
		//Controls the wind waving of the grass. Uses more perlin noise, but this instance is influenced by time.
		cnoise(positionXY+ WIND_DIRECTION*WIND_SPEED*(frameNumber/frameRate))
	);
	vec2 windOffset = windMult / vec2(SCALING, SCALING); //Scale to the correct size of the blades.
	return windOffset;
}
//// SHELL TEXTURING ////







void main() {

	vec2 UV = layerUV;
	float cPerlinRandom = cnoise(positionXY);
	float layerDecimal = float(layerIndex) / float(numLayers); //0-1 of lowest layer to highest layer.
	float layerDelta = float(layerIndex) * layerSpacing; //Difference between the current layer and the base.
	float distanceFromCamera2D;
	distanceFromCamera2DCulling(distanceFromCamera2D, layerDecimal);
	float maxHeight = float(numLayers) * layerSpacing;
	MIN_BLADE_HEIGHT = MIN_BLADE_HEIGHT_SCALE * maxHeight;


#ifdef HAS_WIND
	vec2 windOffset = getWindOffset(layerDecimal);
	UV += windOffset;
#endif


	float randomDecimal, randomHeight;
	heightDiscard(UV, layerDelta, maxHeight, randomDecimal, randomHeight);
	conicalDiscard(UV, layerDelta, distanceFromCamera2D, randomDecimal, randomHeight);


	float lightMultiplier = ((layerDecimal * 0.75f) + 0.25f) * dot(SUN_DIRECTION, normal);

	vec3 thisColour = mix(
		COLOUR_A.rgb, COLOUR_B.rgb,	cPerlinRandom
	);
	vec3 shellColour = mix(BASE_COLOUR.rgb, thisColour.rgb, layerDecimal) * lightMultiplier;


#if defined(HAS_WIND) && defined(DEBUG_WIND)
	fragColour = vec4(windOffset.xy*SCALING*0.5f+0.5f, 0.0f, 1.0f);
#else
	float distanceDecimal = distanceFromCamera2D/MAX_DISTANCE_FROM_CAMERA;
	fragColour = vec4(mix(
		shellColour.rgb, skyColour.rgb, distanceDecimal*distanceDecimal
	), 1.0f);
#endif
}