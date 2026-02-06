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


//Pre-processor step replaces these with the entire contents of the files "constants.glsl", "noise.glsl" and "clouds.shared.glsl".
//Lets me share values/functions between shaders when necessary, without repeating their code.
#include <constants>
#include <noise>
#include <clouds.shared>




//// SHELL TEXTURING ////
void distanceFromCamera2DCulling(out float distanceFromCamera2D, inout float layerDecimal) {
	distanceFromCamera2D = length(cameraPosition.xy - positionXY);
	float distScaling = (1.0f - clamp(distanceFromCamera2D / MAX_DISTANCE_FROM_CAMERA, 0.0f, 1.0f));
	if (length(cameraPosition - vec3(positionXY.xy, layerHeight)) > MAX_DISTANCE_FROM_CAMERA) {discard;}
}


void heightDiscard(
	vec2 UV, float layerDelta, float maxHeight,
	out float randomDecimal, out float randomHeight
) {
	//Height discard.
	randomDecimal = getRandom(abs(UV), SHELL_SCALING);
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
		vec2 localPos = fract(UV * SHELL_SCALING);
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
	vec2 windOffset = windMult / vec2(SHELL_SCALING, SHELL_SCALING); //Scale to the correct size of the blades.
	return windOffset;
}
//// SHELL TEXTURING ////



float getCloudShadow() {
	vec2 skyPos = positionXY + SUN_DIRECTION.xy * (CLOUD_HEIGHT / SUN_DIRECTION.z);
	float cloudValue = getCloudValueForPosition(skyPos, (frameNumber/frameRate));
	return cloudValue * 0.5f + 0.5f;
}





void main() {

	//Initial setup of values
	vec2 UV = layerUV;
	float cPerlinRandom = cnoise(positionXY);
	float layerDecimal = float(layerIndex) / float(numLayers); //0-1 of lowest layer to highest layer.
	float layerDelta = float(layerIndex) * layerSpacing; //Difference between the current layer and the base.
	float distanceFromCamera2D;
	distanceFromCamera2DCulling(distanceFromCamera2D, layerDecimal); //Reduce number of layers further away.
	float maxHeight = float(numLayers) * layerSpacing;
	MIN_BLADE_HEIGHT = MIN_BLADE_HEIGHT_SCALE * maxHeight;



#ifdef HAS_WIND
	float windDistBlend = distanceFromCamera2D / (MAX_DISTANCE_FROM_CAMERA * MAX_WIND_DIST);
	vec2 windOffset = mix(
		getWindOffset(layerDecimal),
		vec2(0.0f, 0.0f),
		windDistBlend*windDistBlend
	); //Scrolling noise wind offset. Fades at larger //distance.
	UV += windOffset;
	#ifdef DEBUG_WIND
		fragColour = vec4(windOffset.xy*SHELL_SCALING*0.5f+0.5f, 0.0f, 1.0f);
		return;
	#endif
#endif

	
	//Discarding stages
	float randomDecimal, randomHeight;
	heightDiscard(UV, layerDelta, maxHeight, randomDecimal, randomHeight); //Discard layer for being too high on this blade.
	conicalDiscard(UV, layerDelta, distanceFromCamera2D, randomDecimal, randomHeight); //Discard frag for not being cylindrical with its inclusion


#ifdef HAS_CLOUD_SHADOWS
	float cloudEffect = getCloudShadow(); //Scrolling cloud shadow noise over terrain.
	#ifdef DEBUG_CLOUD_SHADOWS
		fragColour = vec4(cloudEffect, (cloudEffect-0.625f)/0.375f, 0.0f, 1.0f);
		return;
	#endif
#endif


	//Combine into final colour
	float lightMultiplier = SUN_BRIGHTNESS * ((layerDecimal * 0.75f) + 0.25f) * dot(SUN_DIRECTION, normal) * cloudEffect;
	vec3 thisColour = mix(
		COLOUR_A, COLOUR_B,	cPerlinRandom
	);
	vec3 shellColour = mix(BASE_COLOUR, thisColour, layerDecimal) * lightMultiplier;

	float distanceDecimal = 1.0f - clamp(distanceFromCamera2D/MAX_DISTANCE_FROM_CAMERA, 0.0f, 1.0f);
	fragColour = vec4(mix(
		skyColour, shellColour, (distanceDecimal) //Cubed to let you see further, but not too far.
	), 1.0f);
}