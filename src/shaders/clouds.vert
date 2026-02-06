/* clouds.vert */
#version 460 core

out vec2 fragPosition;
uniform mat4 pvmMatrix;
uniform vec3 cameraPosition;


//Pre-processor step replaces this with the entire contents of the file "constants.glsl".
//Lets me share values/functions between shaders when necessary, without repeating their code.
#include <constants>


//The clouds are always at some position around the camera.
//You cannot move to the edge of them.
const vec2 vertices[4] = {
	vec2(-1.0f, -1.0f),
	vec2( 1.0f, -1.0f),
	vec2(-1.0f,  1.0f),
	vec2( 1.0f,  1.0f)
};

void main() {

	vec3 aPos = vec3(
		(vertices[gl_VertexID].xy * CLOUD_QUAD_SIZE) + cameraPosition.xy, //Scale to the correct size, centre on camera pos.
		CLOUD_HEIGHT
	);
	gl_Position = pvmMatrix * vec4(aPos, 1.0f);
	fragPosition = (aPos - cameraPosition).xy; //Moves horizontally with camera to fake lack of perspective.
}
