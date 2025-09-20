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
		vertexShader = compileShader(GL_VERTEX_SHADER, "src\\shaders\\"+ name +".vert");
	} else {
		vertexShader = compileShader(GL_VERTEX_SHADER, "src\\shaders\\generic.vert");
	}
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "src\\shaders\\"+ name +".frag");

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




GLuint getVAO() {
	std::array<float, (constants::NUM_LAYERS * 4 * 5)> vertices;
	std::array<GLuint, (constants::NUM_LAYERS * 6)> indices;

	#define START -32.0f
	#define END 32.0f

	int currentIndex = 0;
	std::array<int, 6> iArray = {
		0, 1, 2,
		2, 3, 0
	};
	std::array<glm::vec4, 4> vecArray = {
		//			X 	   Y     U     V
		glm::vec4(START, START, 0.0f, 0.0f),
		glm::vec4(START, END,	0.0f, 1.0f),
		glm::vec4(END,   END,	1.0f, 1.0f),
		glm::vec4(END,   START,	1.0f, 0.0f)
	};
	for (int layerIdx=0; layerIdx<constants::NUM_LAYERS; layerIdx++) {
		int verticesIndex = layerIdx * 4 * 5; //4 vertices, with 5 values each.
		int indicesIndex = layerIdx * 6;
		float layerHeight = float(constants::LAYER_SPACING * layerIdx);

		for (int vIdx=0; vIdx<4; vIdx++) {
			glm::vec4 vecValues = vecArray[vIdx];
			int offset = vIdx * 5;
			vertices[verticesIndex + offset + 0] = vecValues.x;
			vertices[verticesIndex + offset + 1] = vecValues.y;
			vertices[verticesIndex + offset + 2] = layerHeight;
			vertices[verticesIndex + offset + 3] = vecValues.z;
			vertices[verticesIndex + offset + 4] = vecValues.w;
		}

		for (int iIdx=0; iIdx<6; iIdx++) {
			int iVal = iArray[iIdx];
			indices[indicesIndex + iIdx] = currentIndex + iVal;
		}
		currentIndex += 4;
	}

	//Create VAO
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Create VBO
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

	//Create EBO
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

	//Define position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Define UV attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	return VAO;
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


void prepareOpenGL() {
	//OpenGL setup;
	GLIndex::shellVAO = graphics::getVAO();


	//Voxel shader
	GLIndex::shellShader = graphics::createShaderProgram("shell", true);


	glViewport(0, 0, currentWindowResolution.x, currentWindowResolution.y);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glClearDepth(1.0f);
	glEnable(GL_BLEND);
	glClearColor(display::SKY_COLOUR.x, display::SKY_COLOUR.y, display::SKY_COLOUR.z, 1.0f);

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
	glViewport(0, 0, currentRenderResolution.x, currentRenderResolution.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Voxel Shader.
	glUseProgram(GLIndex::shellShader);

	//Uniforms
	uniforms::bindUniformValue(GLIndex::shellShader, "pvmMatrix", pvmMatrix);
	uniforms::bindUniformValue(GLIndex::shellShader, "layerSpacing", constants::LAYER_SPACING);
	uniforms::bindUniformValue(GLIndex::shellShader, "numLayers", constants::NUM_LAYERS);
	uniforms::bindUniformValue(GLIndex::shellShader, "cameraPosition", camera.position);
	uniforms::bindUniformValue(GLIndex::shellShader, "skyColour", display::SKY_COLOUR);

	glBindVertexArray(GLIndex::shellVAO);
	glDrawElements(GL_TRIANGLES, constants::NUM_LAYERS * 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	utils::GLErrorcheck("Voxel Shader", true);
}

}