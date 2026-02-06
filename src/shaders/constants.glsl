/* constants.glsl */
//File with CONSTANT values to be shared between all of the "cloud" related shaders.
//Automatically pre-processes "#include <constants>" into the entire source in this file.


/* CONFIG */
//Distance culling
#define MAX_DISTANCE_FROM_CAMERA 32.0f
#define FALLOFF 0.125f
//Distance culling


//Colour
#define COLOUR_SCALING 6.0f
#define COLOUR_A    vec3(0.325f, 0.375f, 0.125f)
#define COLOUR_B    vec3(0.40f, 0.40f, 0.15f)
#define BASE_COLOUR vec3(0.44f, 0.33f, 0.24f)
#define CLOUD_COLOUR vec3(0.8f, 0.8f, 0.8f)
#define SUN_BRIGHTNESS 1.5f
//Colour


//Wind
#define HAS_WIND
//#define DEBUG_WIND
const vec2 WIND_DIRECTION = normalize(vec2(1.0f, 0.25f));
#define WIND_STRENGTH 6.0f
#define WIND_SPEED 0.75f
#define MAX_WIND_DIST 1.0f
//Wind


//Clouds
#define HAS_CLOUD_SHADOWS
//#define DEBUG_CLOUDS
//#define DEBUG_CLOUD_SHADOWS
#define CLOUD_HEIGHT 4.0f
#define CLOUD_QUAD_SIZE MAX_DISTANCE_FROM_CAMERA
#define CLOUD_NOISE_SPEED 1.0f
#define CLOUD_NOISE_SIZE 12.0f
#define CLOUD_MIN_VALUE 0.0f
#define CLOUD_MAX_VALUE 0.5f
//Clouds


//Drawing
#define SHELL_SCALING 256.0f
#define MIN_BLADE_HEIGHT_SCALE 0.325f
float MIN_BLADE_HEIGHT = 0.0f;
const vec3 SUN_DIRECTION = normalize(vec3(0.25f, 0.25f, 1.0f));
#define HAS_CONICAL_SHELLS
#define CONE_RENDER_DIST 1.5f
//Drawing
/* CONFIG */
