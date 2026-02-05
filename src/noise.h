#include "includes.h"
#include "global.h"
#include "utils.h"



namespace noise {

//// PERLIN NOISE ////
//Src: [https://github.com/stegu/webgl-noise]
static glm::vec4 mod289(glm::vec4 x)
{
	return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}

static glm::vec4 permute(glm::vec4 x)
{
	return mod289(((x*34.0f)+10.0f)*x);
}

static glm::vec4 taylorInvSqrt(glm::vec4 r)
{
	return 1.79284291400159f - 0.85373472095314f * r;
}

static glm::vec2 fade(glm::vec2 t) {
	return t*t*t*(t*(t*6.0f-15.0f)+10.0f);
}

// Classic Perlin noise
static float cnoise(glm::vec2 P)
{
	glm::vec4 Pi = glm::floor(glm::vec4(P.x, P.y, P.x, P.y)) + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 Pf = glm::fract(glm::vec4(P.x, P.y, P.x, P.y)) - glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	Pi = mod289(Pi); // To avoid truncation effects in permutation
	glm::vec4 ix = glm::vec4(Pi.x, Pi.z, Pi.x, Pi.z);
	glm::vec4 iy = glm::vec4(Pi.y, Pi.y, Pi.w, Pi.w);
	glm::vec4 fx = glm::vec4(Pf.x, Pf.z, Pf.x, Pf.z);
	glm::vec4 fy = glm::vec4(Pf.y, Pf.y, Pf.w, Pf.w);

	glm::vec4 i = permute(permute(ix) + iy);

	glm::vec4 gx = glm::fract(i * (1.0f / 41.0f)) * 2.0f - 1.0f ;
	glm::vec4 gy = glm::abs(gx) - 0.5f ;
	glm::vec4 tx = glm::floor(gx + 0.5f);
	gx = gx - tx;

	glm::vec2 g00 = glm::vec2(gx.x,gy.x);
	glm::vec2 g10 = glm::vec2(gx.y,gy.y);
	glm::vec2 g01 = glm::vec2(gx.z,gy.z);
	glm::vec2 g11 = glm::vec2(gx.w,gy.w);

	glm::vec4 norm = taylorInvSqrt(glm::vec4(glm::dot(g00, g00), glm::dot(g01, g01), glm::dot(g10, g10), glm::dot(g11, g11)));

	float n00 = norm.x * glm::dot(g00, glm::vec2(fx.x, fy.x));
	float n01 = norm.y * glm::dot(g01, glm::vec2(fx.z, fy.z));
	float n10 = norm.z * glm::dot(g10, glm::vec2(fx.y, fy.y));
	float n11 = norm.w * glm::dot(g11, glm::vec2(fx.w, fy.w));

	glm::vec2 fade_xy = fade(glm::vec2(Pf.x, Pf.y));
	glm::vec2 n_x = glm::mix(glm::vec2(n00, n01), glm::vec2(n10, n11), fade_xy.x);
	float n_xy = glm::mix(n_x.x, n_x.y, fade_xy.y);
	return 2.3f * n_xy;
}
//// PERLIN NOISE ////


#define NUMBER_OF_NOISE_OCTAVES 4
#define NOISE_SCALE 0.1f
static float heightFunction(glm::vec2 position2D) {
	float result = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;

	for (int i=0; i<NUMBER_OF_NOISE_OCTAVES; i++) {
		result += amplitude * cnoise(position2D * NOISE_SCALE * frequency);

		frequency *= 2.0f;
		amplitude *= 0.5f;
	}

	return result;
}


static glm::vec3 computeNormal(
	const glm::vec2 position2D,
	float epsilon = 0.01f
) {
	float hL = heightFunction(position2D + glm::vec2(-epsilon,  0.0f));
	float hR = heightFunction(position2D + glm::vec2( epsilon,  0.0f));
	float hD = heightFunction(position2D + glm::vec2( 0.0f,    -epsilon));
	float hU = heightFunction(position2D + glm::vec2( 0.0f,     epsilon));

	float dX = (hR - hL) / (2.0f * epsilon);
	float dY = (hU - hD) / (2.0f * epsilon);

	return glm::normalize(glm::vec3(-dX, -dY, 1.0f));
}

}

