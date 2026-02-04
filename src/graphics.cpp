#include "includes.h"
#include "global.h"
#include "utils.h"
#include "tiny_obj_loader.h"
using namespace std;
using namespace utils;
using namespace glm;


namespace uniforms {

//Uniforms; [Many overloads]
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, bool value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1i(location, value);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, size_t value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1i(location, value);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, unsigned int value) {bindUniformValue(shaderProgram, uniformName, size_t(value));}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, int value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1i(location, value);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, float value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1f(location, value);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec2 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform2i(location, value.x, value.y);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec2 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform2f(location, value.x, value.y);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec3 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform3i(location, value.x, value.y, value.z);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec3 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform3f(location, value.x, value.y, value.z);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec4 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform4i(location, value.x, value.y, value.z, value.w);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec4 value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
}
inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::mat4& value) {
	GLuint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

}


namespace graphics {

GLFWwindow* initializeWindow(int width, int height, const char* title) {
	if (!glfwInit()) {
		raise("Failed to initialize GLFW");
		return nullptr;
	}


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // Set OpenGL major version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  // Set OpenGL minor version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // Use Core profile


	GLFWwindow* Window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!Window) {
		glfwTerminate();
		raise("Failed to create GLFW window");
		return nullptr;
	}
	glfwMakeContextCurrent(Window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		raise("Failed to initialize GLEW.");
	}

	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return Window;
}



GLuint compileShader(GLenum shaderType, string filePath) {
	std::string source = utils::readFile(filePath);
	const char* src = source.c_str();

	// Create a shader object
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		raise("Error: Failed to create shader.");
		return 0;
	}

	// Attach the shader source code to the shader object
	glShaderSource(shader, 1, &src, nullptr);

	// Compile the shader
	glCompileShader(shader);
	

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		char infolog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infolog);
		raise("Error: Shader compilation failed;\n" + string(infolog));
	}

	return shader;
}


GLuint createShaderProgram(std::string name, bool hasVertexSource=true) {
	GLuint vertexShader;
	if (hasVertexSource) {
		vertexShader = compileShader(GL_VERTEX_SHADER, "src/shaders/"+ name +".vert");
	} else {
		vertexShader = compileShader(GL_VERTEX_SHADER, "src/shaders/generic.vert");
	}
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src/shaders/"+ name +".frag");

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		char infolog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infolog);
		raise("Error: Program linking failed;\n" + string(infolog));
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}



static glm::mat4 getModelMat4(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale) {
	glm::mat4 translationMat = glm::mat4(
		1.0f, 	0.0f, 	0.0f, 	pos.x,
		0.0f, 	1.0f, 	0.0f, 	pos.y,
		0.0f, 	0.0f, 	1.0f, 	pos.z,
		0.0f, 	0.0f, 	0.0f, 	1.0f
	);

	float sx = sin(rot.x), cx = cos(rot.x);
	float sy = sin(rot.y), cy = cos(rot.y);
	float sz = sin(rot.z), cz = cos(rot.z);
	glm::mat4 rotationMat = glm::mat4(
		cy*cz, cy*sz, -sy, 0.0f,
		sx*sy*cz-cx*sz, sx*sy*sz+cx*cz, sx*cy, 0.0f,
		cx*sy*cz+sx*sz, cx*sy*sz-sx*cz, cx*cy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 scaleMat = glm::mat4(
		scale.x,	0.0f, 		0.0f,		0.0f, 
		0.0f, 		scale.y,	0.0f, 		0.0f, 
		0.0f, 		0.0f, 		scale.z,	0.0f, 
		0.0f, 		0.0f, 		0.0f, 		1.0f
	);

	return rotationMat * scaleMat * translationMat;
}




static inline glm::mat4 projectionMatrix() {
	float aspectRatio = float(currentRenderResolution.x) / float(currentRenderResolution.y);
	return glm::perspective(camera.FOV, aspectRatio, camera.nearZ, camera.farZ);
}

static inline glm::mat4 viewMatrix() {
	glm::vec3 forward = glm::vec3(
		sin(camera.viewAngle.x)*cos(camera.viewAngle.y),
		cos(camera.viewAngle.x)*cos(camera.viewAngle.y),
		sin(camera.viewAngle.y)
	);

	return glm::lookAt(camera.position, camera.position + forward, glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::mat4 getPVMMatrix() {
	glm::mat4 pMat = projectionMatrix();
	glm::mat4 vMat = viewMatrix();
	glm::mat4 mMat = glm::mat4(1.0f); //Identity matrix.

	return pMat * vMat * mMat;
}


void APIENTRY openGLErrorCallback(
		GLenum source,
		GLenum type, GLuint id,
		GLenum severity,
		GLsizei length, const GLchar* message,
		const void* userParam
	) {
	/*
	Nicely formatted callback from;
	[https://learnopengl.com/In-Practice/Debugging]
	*/
	if(id == 131169 || id == 131185 || id == 131218 || id == 131204) {return;}

	std::cout << "---------------" << std::endl << "Debug message (" << id << ") | " << message << std::endl;

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             {std::cout << "Source: API"; break;}
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   {std::cout << "Source: Window System"; break;}
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {std::cout << "Source: Shader Compiler"; break;}
		case GL_DEBUG_SOURCE_THIRD_PARTY:     {std::cout << "Source: Third Party"; break;}
		case GL_DEBUG_SOURCE_APPLICATION:     {std::cout << "Source: Application"; break;}
		case GL_DEBUG_SOURCE_OTHER:           {std::cout << "Source: Other"; break;}
	} std::cout << std::endl;

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               {std::cout << "Type: Error"; break;}
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {std::cout << "Type: Deprecated Behaviour"; break;}
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  {std::cout << "Type: Undefined Behaviour"; break;} 
		case GL_DEBUG_TYPE_PORTABILITY:         {std::cout << "Type: Portability"; break;}
		case GL_DEBUG_TYPE_PERFORMANCE:         {std::cout << "Type: Performance"; break;}
		case GL_DEBUG_TYPE_MARKER:              {std::cout << "Type: Marker"; break;}
		case GL_DEBUG_TYPE_PUSH_GROUP:          {std::cout << "Type: Push Group"; break;}
		case GL_DEBUG_TYPE_POP_GROUP:           {std::cout << "Type: Pop Group"; break;}
		case GL_DEBUG_TYPE_OTHER:               {std::cout << "Type: Other"; break;}
	} std::cout << std::endl;
	
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         {std::cout << "Severity: high"; break;}
		case GL_DEBUG_SEVERITY_MEDIUM:       {std::cout << "Severity: medium"; break;}
		case GL_DEBUG_SEVERITY_LOW:          {std::cout << "Severity: low"; break;}
		case GL_DEBUG_SEVERITY_NOTIFICATION: {std::cout << "Severity: notification"; break;}
	} std::cout << std::endl;
	std::cout << std::endl;

	utils::pause();
}





namespace noise {

inline float fade(float t) {
	//Smoothstep
	return t*t*t*(t*((t*6)-15)+10);
}


inline float lerp(float a, float b, float t) {
	return a + (t * (b - a));
}


inline float hash(int x, int y) {
	//Deterministic pseudo-random in for range [0.0 → 1.0]
	int h = (x * 374761393u) + (y * 668265263u);
	h = (h ^ (h >> 13u)) * 1274126177u;
	return (h & 0x7FFFFFFFu) / float(0x7FFFFFFFu);
}


float heightFunction(glm::vec2 position2D) {
	int x0 = static_cast<int>(std::floor(position2D.x));
	int y0 = static_cast<int>(std::floor(position2D.y));
	int x1 = x0 + 1; int y1 = y0 + 1;

	float sx = fade(position2D.x - x0);
	float sy = fade(position2D.y - y0);

	float n00 = hash(x0, y0);
	float n10 = hash(x1, y0);
	float n01 = hash(x0, y1);
	float n11 = hash(x1, y1);

	float ix0 = lerp(n00, n10, sx);
	float ix1 = lerp(n01, n11, sx);

	return lerp(ix0, ix1, sy);
}


glm::vec3 computeNormal(
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



std::vector<glm::vec3> singleLayerVertexArray;
std::vector<glm::vec3> singleLayerNormalArray;

void generateGrid(int n) {
	singleLayerVertexArray.clear();     	singleLayerNormalArray.clear();
	singleLayerVertexArray.reserve(n*n);	singleLayerNormalArray.reserve(n*n);

	float step = 2.0f / float(n-1);

	for (int y=0; y<n; y++) {
		for (int x=0; x<n; x++) {
			float posX = -1.0f + (x * step);
			float posY = -1.0f + (y * step);

			glm::vec2 position2D = glm::vec2(posX, posY);
			glm::vec2 noisePos = position2D * constants::SCALE.x * constants::NOISE_FREQ;
			singleLayerVertexArray.emplace_back(glm::vec3(
				position2D, (noise::heightFunction(noisePos) * constants::SCALE.y)
			));
			singleLayerNormalArray.emplace_back(noise::computeNormal(noisePos, 0.5f));
		}
	}
}


void generateGridIndices(int n, std::vector<GLuint>* indices, GLuint baseVertex) {
	for (int y=0; y<(n-1); y++) {
		for (int x=0; x<(n-1); x++) {
			GLuint i0 = baseVertex + ((y * n) + x);
			GLuint i1 = baseVertex + ((y * n) + x + 1);
			GLuint i2 = baseVertex + (((y + 1) * n) + x);
			GLuint i3 = baseVertex + (((y + 1) * n) + x + 1);

			//T1
			indices->emplace_back(i0);
			indices->emplace_back(i1);
			indices->emplace_back(i2);

			//T2
			indices->emplace_back(i1);
			indices->emplace_back(i3);
			indices->emplace_back(i2);
		}
	}
}


std::vector<float> vertices;
std::vector<GLuint> indices;
GLuint getVAO(int n) {
	vertices.clear(); indices.clear();
	vertices.reserve(singleLayerVertexArray.size() * 6);
	graphics::generateGrid(n);

	for (int vIdx = 0; vIdx < singleLayerVertexArray.size(); vIdx++) {
		glm::vec3 position = singleLayerVertexArray[vIdx];
		glm::vec3 normal   = singleLayerNormalArray[vIdx];

		//Position - Z as base layer height.
		vertices.push_back(position.x * constants::SCALE.x);
		vertices.push_back(position.y * constants::SCALE.x);
		vertices.push_back(position.z);

		//Normal
		vertices.push_back(normal.x);
		vertices.push_back(normal.y);
		vertices.push_back(normal.z);
	}
	generateGridIndices(n, &indices, 0);


	//Create VAO
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Create VBO
	GLuint layerVBO;
	glGenBuffers(1, &layerVBO);
	glBindBuffer(GL_ARRAY_BUFFER, layerVBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	//Create EBO
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	//Define position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Define normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


	return VAO;
}


void prepareOpenGL() {
	//OpenGL setup;
	GLIndex::shellVAO = graphics::getVAO(constants::GRID_WIDTH);


	//Voxel shader
	GLIndex::shellShader = graphics::createShaderProgram("shell", true);


	glViewport(0, 0, currentWindowResolution.x, currentWindowResolution.y);

	//Depth and clear.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glClearDepth(1.0f);
	glClearColor(display::SKY_COLOUR.x, display::SKY_COLOUR.y, display::SKY_COLOUR.z, 1.0f);

	//Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Alpha blending
	glDisable(GL_BLEND);

	verticalFOV = 2.0f * atan(tan(display::FOV * 0.5f) * (float(currentRenderResolution.y) / float(currentRenderResolution.x)));
	

	//Debug settings
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLErrorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	utils::GLErrorcheck("Initialisation", true); //Old basic debugging
}


}




namespace frame {

void draw() {
	glm::mat4 pvmMatrix = graphics::getPVMMatrix();

	//Update resolution
	glViewport(0, 0, currentWindowResolution.x, currentWindowResolution.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Voxel Shader.
	glUseProgram(GLIndex::shellShader);

	//Uniforms
	uniforms::bindUniformValue(GLIndex::shellShader, "pvmMatrix", pvmMatrix);
	uniforms::bindUniformValue(GLIndex::shellShader, "layerSpacing", constants::LAYER_SPACING);
	uniforms::bindUniformValue(GLIndex::shellShader, "numLayers", constants::NUM_LAYERS);
	uniforms::bindUniformValue(GLIndex::shellShader, "cameraPosition", camera.position);
	uniforms::bindUniformValue(GLIndex::shellShader, "skyColour", display::SKY_COLOUR);
	uniforms::bindUniformValue(GLIndex::shellShader, "frameNumber", frameNumber);
	uniforms::bindUniformValue(GLIndex::shellShader, "frameRate", display::MAX_FREQ);

	glBindVertexArray(GLIndex::shellVAO);
	glDrawElementsInstanced(
		GL_TRIANGLES, graphics::indices.size(),
		GL_UNSIGNED_INT, nullptr,
		constants::NUM_LAYERS
	);
	glBindVertexArray(0);
	utils::GLErrorcheck("Voxel Shader", true);
}

}