#include "includes.h"
#include "global.h"
#include "utils.h"
#include "noise.h"
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


static unsigned int lineNumberAt(const std::string& s, size_t pos) {
	//Find [#line] number from position
    return std::count(s.begin(), s.begin() + pos, '\n');
}

std::string preprocessIncludes(const std::string& source, const std::string& currentFile) {
    std::regex includeRegex(R"(^\s*#include\s*<([^>]+)>)", std::regex_constants::multiline);

    std::string result;
    std::sregex_iterator it(source.begin(), source.end(), includeRegex);
    std::sregex_iterator end;

    size_t lastPos = 0;
    for (; it!=end; it++) {
        const std::smatch& match = *it;

        //Copy text before include
        result.append(source.substr(lastPos, match.position() - lastPos));

        std::string includeFile = match[1].str();
        std::string includePath = "src/shaders/" + includeFile + ".glsl";

        std::string includedSource = utils::readFile(includePath);

        unsigned int includeLine = lineNumberAt(source, match.position());

        result += "#line 1 \"src/shaders/"+includeFile+".glsl\"\n"+includedSource+"\n"+"#line "+std::to_string(includeLine+1u)+" \""+currentFile+"\"\n";

        lastPos = match.position() + match.length();
    }

    // Append remaining source
    result.append(source.substr(lastPos));

    return result;
}


GLuint compileShader(GLenum shaderType, string filePath) {
	std::string source = utils::readFile(filePath);
	source = preprocessIncludes(source, filePath);
	const char* src = source.c_str();

	//Create a shader id
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		raise("Error: Failed to create shader.");
		return 0;
	}

	//Attach the shader src
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	

	//Errorcheck
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



GLuint createComputeShader(std::string compShaderName) {
	GLuint computeShader = compileShader(GL_COMPUTE_SHADER, "src/shaders/" + compShaderName + ".comp");

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, computeShader);
	glLinkProgram(shaderProgram);

	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		char infolog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infolog);
		raise("Error: Compute shader program linking failed:\n" + std::string(infolog));
	}

	glDeleteShader(computeShader);

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
	verticalFOV = 2 * atan(tan(camera.FOV * 0.5f) * (float(currentRenderResolution.y) / float(currentRenderResolution.x)));
	return glm::perspective(verticalFOV, aspectRatio, camera.nearZ, camera.farZ);
}

static inline glm::mat4 viewMatrix() {
	glm::vec3 forward = glm::vec3(
		sin(camera.viewAngle.x)*cos(camera.viewAngle.y),
		cos(camera.viewAngle.x)*cos(camera.viewAngle.y),
		sin(camera.viewAngle.y)
	);

	return glm::lookAt(camera.position, camera.position + forward, glm::vec3(0.0f, 0.0f, 1.0f));
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




GLuint createGLImage2D(size_t width, size_t height, GLint internalFormat=GL_RGBA32F, GLint samplingType=GL_NEAREST, GLint edgeSampling=GL_REPEAT) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


GLuint createShaderStorageBufferObject(int binding, size_t bufferSize=0, GLuint glType=GL_DYNAMIC_DRAW) {
	GLuint SSBO;
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, glType);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return SSBO;
}

template<typename T>
void updateShaderStorageBufferObject(
	GLuint SSBO,
	T* data,
	size_t count
) {
	size_t size = sizeof(T) * count;

	if (count > 0) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}


float radicalInverse(uint32_t bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u)  | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u)  | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u)  | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u)  | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10f; //bits / 2^32
}

void updateSamplesDataset() {
	//Updates the dataset of samples.
	for (int i=0; i<constants::NUMBER_OF_SAMPLES; i++) {
		float u1 = float(i) / float(constants::NUMBER_OF_SAMPLES);
		float u2 = radicalInverse(i);

		float r = sqrt(u1);
		float theta = constants::PI2 * u2;

		float x = r * cos(theta);
		float y = r * sin(theta);
		float z = sqrt(1.0f - u1);

		samplesDataset[i] = structs::Sample(
			glm::vec3(x, y, z), //Direction
			glm::vec3(x, y, z)  //Set colour to dir too for now.
		);
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



GLenum shellDrawBuffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
GLenum cloudDrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT2};
void prepareOpenGL() {
	//OpenGL setup;
	GLIndex::shellVAO = graphics::getVAO(constants::GRID_WIDTH);


	//Shaders
	GLIndex::shellShader = graphics::createShaderProgram("shell", true);
	GLIndex::cloudShader = graphics::createShaderProgram("clouds", true);
	GLIndex::atmosShader = graphics::createComputeShader("atmos"); //Compute shader.
	GLIndex::displayShader = graphics::createShaderProgram("display", false); //No display.vert file

	//Lighting data image
	GLIndex::frameAlbedo = createGLImage2D(currentRenderResolution.x, currentRenderResolution.y); //Frame colour values.
	GLIndex::frameNormal = createGLImage2D(currentRenderResolution.x, currentRenderResolution.y); //Frame normal values.
	GLIndex::framePXData = createGLImage2D(currentRenderResolution.x, currentRenderResolution.y, GL_RG32F); //Only 2 components. Type (int) and opacity for clouds.


	//Samples SSBO
	GLIndex::samplesSSBO = createShaderStorageBufferObject(
		0, sizeof(structs::Sample) * constants::NUMBER_OF_SAMPLES
	);
	updateSamplesDataset();
	updateShaderStorageBufferObject(GLIndex::samplesSSBO, samplesDataset, constants::NUMBER_OF_SAMPLES); //Must be dynamic, as it will change with time later.



	//Frame FBO
	glCreateFramebuffers(1, &GLIndex::FBO);
	glCreateRenderbuffers(1, &GLIndex::frameDepth);
	glNamedFramebufferTexture(GLIndex::FBO, GL_COLOR_ATTACHMENT0, GLIndex::frameAlbedo, 0);
	glNamedFramebufferTexture(GLIndex::FBO, GL_COLOR_ATTACHMENT1, GLIndex::frameNormal, 0);
	glNamedFramebufferTexture(GLIndex::FBO, GL_COLOR_ATTACHMENT2, GLIndex::framePXData, 0);
	glNamedRenderbufferStorage(GLIndex::frameDepth, GL_DEPTH_COMPONENT24, currentRenderResolution.x, currentRenderResolution.y);
	glNamedFramebufferRenderbuffer(GLIndex::FBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, GLIndex::frameDepth);

	glNamedFramebufferDrawBuffers(GLIndex::FBO, 3, shellDrawBuffers);
	GLenum status = glCheckNamedFramebufferStatus(GLIndex::FBO, GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FBO incomplete! Status = 0x%x\n", status);
	}


	//Depth and clear.
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearDepth(1.0f);
	glClearColor(display::SKY_COLOUR.r, display::SKY_COLOUR.g, display::SKY_COLOUR.b, 1.0f);

	//Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Alpha blending
	glDisable(GL_BLEND);
	

	//Debug settings
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLErrorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	utils::GLErrorcheck("Initialisation", true); //Old basic debugging
}




void clearFrameAlbedo() {
	const float clearValue[4] = {display::SKY_COLOUR.r, display::SKY_COLOUR.g, display::SKY_COLOUR.b, 1.0f};
	glClearTexImage(
		GLIndex::frameAlbedo, 0, GL_RGBA, GL_FLOAT, clearValue
	);
}
void clearFrameNormal() {
	const float clearValue[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	glClearTexImage(
		GLIndex::frameNormal, 0, GL_RGBA, GL_FLOAT, clearValue
	);
}
void clearFramePXData() {
	const float clearValue[2] = {2.0f, 0.0f};
	glClearTexImage(
		GLIndex::framePXData, 0, GL_RG, GL_FLOAT, clearValue
	);
}



}




namespace frame {

void draw() {
	glm::mat4 pMat = graphics::projectionMatrix();
	glm::mat4 vMat = graphics::viewMatrix();
	glm::mat4 mMat = glm::mat4(1.0f); //Identity matrix.
	glm::mat4 pvmMatrix = pMat * vMat * mMat;

	glm::mat4 invProj = glm::inverse(pMat);
	glm::mat4 invView = glm::inverse(vMat);



	//Update resolution
	glViewport(0, 0, currentRenderResolution.x, currentRenderResolution.y);


	//Clear lighting data texture, and start using frame FBO.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLIndex::FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	graphics::clearFrameAlbedo();
	graphics::clearFrameNormal();
	graphics::clearFramePXData();

	//Shell Shader.
	//Shell-textured grass shader.
	glUseProgram(GLIndex::shellShader);
	glEnable(GL_CULL_FACE);
	glNamedFramebufferDrawBuffers(GLIndex::FBO, 3, graphics::shellDrawBuffers);

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
	utils::GLErrorcheck("Shell Shader", true);




	//Clouds shader
	//Renders flat plane of noise at fixed dist above camera to mimic clouds.
	glDisable(GL_CULL_FACE);
	glUseProgram(GLIndex::cloudShader);
	glNamedFramebufferDrawBuffers(GLIndex::FBO, 2, graphics::cloudDrawBuffers);
	uniforms::bindUniformValue(GLIndex::cloudShader, "pvmMatrix", pvmMatrix);
	uniforms::bindUniformValue(GLIndex::cloudShader, "cameraPosition", camera.position);
	uniforms::bindUniformValue(GLIndex::cloudShader, "frameNumber", frameNumber);
	uniforms::bindUniformValue(GLIndex::cloudShader, "frameRate", display::MAX_FREQ);
	uniforms::bindUniformValue(GLIndex::cloudShader, "skyColour", display::SKY_COLOUR);
	uniforms::bindUniformValue(GLIndex::cloudShader, "cameraPosition", camera.position);
	glBindTextureUnit(0, GLIndex::framePXData); //Previous pass's framePXData

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Uses vertices defined in vertex shader.
	utils::GLErrorcheck("Cloud Shader", true);


	//Atmos shader
	//Applies interpolated atmospheric light to mimic true atmospheric scattering.
	const glm::uvec3 ATMOS_LOCAL_SIZE = glm::uvec3(16u, 16u, 1u);
	glUseProgram(GLIndex::atmosShader);
	uniforms::bindUniformValue(GLIndex::atmosShader, "resolution", currentRenderResolution);
	uniforms::bindUniformValue(GLIndex::atmosShader, "invProjMatrix", invProj);
	uniforms::bindUniformValue(GLIndex::atmosShader, "invViewMatrix", invView);
	glBindImageTexture(0, GLIndex::frameAlbedo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);	//Modifies frame data.
	glBindTextureUnit(1, GLIndex::frameNormal);
	glBindTextureUnit(2, GLIndex::framePXData);
	glDispatchCompute(
		(currentRenderResolution.x + ATMOS_LOCAL_SIZE.x - 1u) / ATMOS_LOCAL_SIZE.x,
		(currentRenderResolution.y + ATMOS_LOCAL_SIZE.y - 1u) / ATMOS_LOCAL_SIZE.y,
		1
	);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);


	//Display shader
	//Displays frame to the screen.
	glViewport(0, 0, currentWindowResolution.x, currentWindowResolution.y);
	glUseProgram(GLIndex::displayShader);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //Back to default FBO.
	glBindTextureUnit(0, GLIndex::frameAlbedo); //Has since been modified by "atmos" shader.
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Screenspace
	
	//Cleanup.
	glBindVertexArray(0);
	glUseProgram(0);

	utils::GLErrorcheck("Display Shader", true);
}

}