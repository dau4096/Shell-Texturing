/* shell.vert */
#version 460 core

layout(location=0) in vec4 aPos;

out vec2 positionXY;
out flat int layerIndex;
out flat float layerHeight;
out vec2 layerUV;
out vec3 normal;

uniform mat4 pvmMatrix;
uniform float layerSpacing;

void main() {

	positionXY = aPos.xy;
	layerHeight = aPos.z;
	layerIndex = int(aPos.w);
	gl_Position = pvmMatrix * vec4(aPos.xyz, 1.0f);
	layerUV = aPos.xy;
	normal = vec3(0.0f, 0.0f, 1.0f);

}