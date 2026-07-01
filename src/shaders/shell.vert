/* shell.vert */
#version 460 core

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;

out vec2 positionXY;
out flat int layerIndex;
out float layerHeight;
out float baseHeight;
out vec2 layerUV;
out vec3 normal;

uniform mat4 pvmMatrix;
uniform int numLayers;




layout(std430, binding=2) buffer shellOffsetSSBO {
	float offsets[];
};


/*
#define LAYER_HEIGHT_SCALING 4.0f
float layerOffsetFunc(const int i) {
	//Uses (i / (n-1)) ^ 1/s
	//For the ith layer of n, and scaling-power s
	float maxHeight = numLayers * layerSpacing;
	return pow(
		i / float(numLayers - 1),
		1.0f / LAYER_HEIGHT_SCALING
	) * maxHeight;
}
*/



void main() {

	positionXY = aPosition.xy;
	layerIndex = int(gl_InstanceID);
	float zOffset = offsets[layerIndex]; //layerOffsetFunc(layerIndex);
	layerHeight = zOffset + aPosition.z;
	baseHeight = aPosition.z;
	gl_Position = pvmMatrix * vec4(positionXY, layerHeight, 1.0f);
	layerUV = aPosition.xy;
	normal = normalize(aNormal);

}