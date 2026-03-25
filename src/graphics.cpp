#include "includes.h"
#include "global.h"
#include "utils.h"
#include "noise.h"
#include "physics.h"
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

	//glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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



void createSamplesDataset() {
	unsigned int datasetIdx = 0u;

	ringCount = static_cast<int>(sqrt(constants::NUMBER_OF_SAMPLES) / 1.5f);
	samplesDataset[datasetIdx++].direction = glm::vec3(0.0f, 0.0f, 1.0f); //Add top Sample.
	float sumSineThetaI = 0.0f; //Find Σ[sin(Θi)]
	for (int ringIdx = 1; ringIdx <= ringCount; ringIdx++) {
		float t = ringIdx / static_cast<float>(ringCount);
		float thetaI = pow(t, 1.2f) * (constants::PI / 2.0f);
		sumSineThetaI += sin(thetaI);
	}

	float K = (constants::NUMBER_OF_SAMPLES - 1) / sumSineThetaI;
	std::vector<int> ringCounts = std::vector<int>(ringCount);

	int total = 1;
	for (int ringIdx=1; ringIdx<=ringCount; ringIdx++) {
		float t = ringIdx / static_cast<float>(ringCount);
		float thetaI = pow(t, 1.2f) * (constants::PI / 2.0f);
		int nI = static_cast<int>(round(K * sin(thetaI)));
		nI = std::max(nI, 3);
		ringCounts[ringIdx - 1] = nI; //Store to be referenced later.
		total += nI;
	}

	while (total < constants::NUMBER_OF_SAMPLES) {
		ringCounts.back()++; //Add more vertices to the biggest ring until it matches NUMBER_OF_SAMPLES
		total++;
	}
	while (total > constants::NUMBER_OF_SAMPLES) {
		if (ringCounts.back() > 3) {
			ringCounts.back()--;
			total--;
		} else {
			break;
		}
	}

	for (int ringIdx=1; ringIdx<=ringCount; ringIdx++) {
		float t = ringIdx / static_cast<float>(ringCount);
		float thetaI = pow(t, 1.2f) * (constants::PI / 2.0f);
		int nI = ringCounts[ringIdx - 1];

		float z = cos(thetaI);
		float rC = sin(thetaI);

		for (int ptIdx=0; ptIdx<nI; ptIdx++) {
			float phi = constants::PI2 * ptIdx / static_cast<float>(nI);

			samplesDataset[datasetIdx++].direction = glm::normalize(glm::vec3(
				rC * cos(phi), rC * sin(phi), z
			));
		}
	}

	//Add to ring dataset.
	int offset = 0;
	for (int count : ringCounts) {
		ringDataset.push_back(glm::ivec2(
			count, offset //Number of samples in this ring, offset
		));
		offset += count;
	}
}


void createSamplesEBO() {
	//Indices buffer (EBO)
	glGenBuffers(1, &GLIndex::sampleEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLIndex::sampleEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sampleIndices.size() * sizeof(uint32_t), sampleIndices.data(), GL_STATIC_DRAW);

	//VAO to contain EBO.
	glGenVertexArrays(1, &GLIndex::sampleVAO);
	glBindVertexArray(GLIndex::sampleVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLIndex::sampleEBO);
	glBindVertexArray(0);
}


void stitchRings(
	int offsetA, int nA,
	int offsetB, int nB,
	std::vector<int>& indices
) {
	int i = 0; //A index
	int j = 0; //B index

	while ((i < nA) || (j < nB)) {
		float tA = (i + 1) / static_cast<float>(nA);
		float tB = (j + 1) / static_cast<float>(nB);

		int a0 = offsetA + (i % nA);
		int a1 = offsetA + ((i + 1) % nA);

		int b0 = offsetB + (j % nB);
		int b1 = offsetB + ((j + 1) % nB);

		if (tA < tB) {
			//Triangle A was closer.
			indices.push_back(a0+1);
			indices.push_back(b0+1);
			indices.push_back(a1+1);
			i++;
		} else {
			//Triangle B was closer.
			indices.push_back(a0+1);
			indices.push_back(b0+1);
			indices.push_back(b1+1);
			j++;
		}
	}
}

void createSamplesIndices() {
	sampleIndices.clear();

	//For ring 0 (Uses top vertex.)
	for (int i=0; i<ringDataset[0].x; i++) {
		int next = (i + 1) % ringDataset[0].x;

		sampleIndices.push_back(0); //Top vertex.
		sampleIndices.push_back(1 + next);
		sampleIndices.push_back(1 + i);
	}


	//For all other rings.
	for (int r=0; r<ringDataset.size()-1; r++) {
		int nA = ringDataset[r].x;
		int offsetA = ringDataset[r].y;

		int nB = ringDataset[r+1].x;
		int offsetB = ringDataset[r+1].y;

		//Get indices for this ring pair.
		stitchRings(
			offsetA, nA,
			offsetB, nB,
			sampleIndices
		);

		//Fill the gap.
		sampleIndices.push_back(offsetA);
		sampleIndices.push_back(offsetA+nA);
		sampleIndices.push_back(offsetB);
	}


	//Close the final triangle of the final layer.
	int r = ringDataset.size()-1;
	glm::ivec2 A = ringDataset[r]; //Last ring
	glm::ivec2 B = ringDataset[r-1]; //2nd-last ring.

	int aLast = A.y + ((A.x - 1) % A.x);
	int aFirst = A.y;

	int bLast = B.y + ((B.x - 1) % B.x);
	int bFirst = B.y;

	sampleIndices.push_back(aLast);
	sampleIndices.push_back(bLast);
	sampleIndices.push_back(aFirst);

	//Later.
	for (int index=1; index<=A.x; index++) {
		//For all samples in the final ring, join to a fake pseudo-sample in a cone shape.
		sampleIndices.push_back(A.y + index);
		sampleIndices.push_back(A.y + (index+1 % A.x));
		sampleIndices.push_back(sampleIndices.size()); //Fake vertex.
	}
	//Final triangle to close off the lower cone.
	sampleIndices.push_back(A.y + 1);
	sampleIndices.push_back(A.y + A.x);
	sampleIndices.push_back(sampleIndices.size());


	createSamplesEBO();
}

void updateSamplesDataset() {
	//Updates the dataset of samples.
	for (structs::Sample& sample : samplesDataset) {
		//sample.colour = sample.direction; //DEBUG
		//sample.colour = display::SKY_COLOUR; //TEMPORARY
		sample.colour = physics::light::calculateSkyColour(sample.direction);
	}

	//Update the actual SSBO.
	updateShaderStorageBufferObject(GLIndex::samplesSSBO, samplesDataset, constants::NUMBER_OF_SAMPLES); //Must be dynamic, as it will change with time later.
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
	GLIndex::skyShader = graphics::createShaderProgram("sky", true);
	GLIndex::displayShader = graphics::createShaderProgram("display", false); //No display.vert file


	//Samples SSBO
	GLIndex::samplesSSBO = createShaderStorageBufferObject(
		0, sizeof(structs::Sample) * constants::NUMBER_OF_SAMPLES
	);
	createSamplesDataset();
	createSamplesIndices();
	updateSamplesDataset(); //Add colours.

	//RingData SSBO
	GLIndex::ringDataSSBO = createShaderStorageBufferObject(
		1, sizeof(glm::ivec2) * ringDataset.size()
	);
	updateShaderStorageBufferObject(GLIndex::ringDataSSBO, &(ringDataset[0]), ringDataset.size());


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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	//Debug settings
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLErrorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	utils::GLErrorcheck("Initialisation", true); //Old basic debugging
}




glm::mat4 getSkyModelMatrix() {
	return glm::scale(
		glm::translate(
			glm::mat4(1.0f), //Start with identity.
			camera.position //Inverse camera translation, to centre on camera.
		),
		glm::vec3(camera.farZ) //Scale it up to be very large.
	);
}


}




namespace frame {

void draw() {
	glm::mat4 pMat = graphics::projectionMatrix();
	glm::mat4 vMat = graphics::viewMatrix();
	glm::mat4 cameraPVMmatrix = pMat * vMat; // * glm::mat4(1.0f); //Model is identity matrix. Commented out as the operation does nothing.
	glm::mat4 skyPVMmatrix = cameraPVMmatrix * graphics::getSkyModelMatrix(); //Uses scale & translation.

	//graphics::updateSamplesDataset(); //Update the sky values.

	//Update resolution & clear
	glViewport(0, 0, currentWindowResolution.x, currentWindowResolution.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	//Sky shader
	//Draws a hemisphere of triangles with interpolated colour.
	glUseProgram(GLIndex::skyShader);
	uniforms::bindUniformValue(GLIndex::skyShader, "pvmMatrix", skyPVMmatrix);
	uniforms::bindUniformValue(GLIndex::skyShader, "numberOfSamples", constants::NUMBER_OF_SAMPLES);
	glBindVertexArray(GLIndex::sampleVAO);
	glDrawElements(GL_TRIANGLES, sampleIndices.size(), GL_UNSIGNED_INT, 0);
	utils::GLErrorcheck("Sky Shader", true);



	//Shell Shader.
	//Shell-textured grass shader.
	glUseProgram(GLIndex::shellShader);
	glEnable(GL_CULL_FACE);

	//Uniforms
	uniforms::bindUniformValue(GLIndex::shellShader, "pvmMatrix", cameraPVMmatrix);
	uniforms::bindUniformValue(GLIndex::shellShader, "layerSpacing", constants::LAYER_SPACING);
	uniforms::bindUniformValue(GLIndex::shellShader, "numLayers", constants::NUM_LAYERS);
	uniforms::bindUniformValue(GLIndex::shellShader, "cameraPosition", camera.position);
	uniforms::bindUniformValue(GLIndex::shellShader, "numberOfRings", ringCount);
	uniforms::bindUniformValue(GLIndex::shellShader, "frameNumber", frameNumber);
	uniforms::bindUniformValue(GLIndex::shellShader, "frameRate", display::MAX_FREQ);
	uniforms::bindUniformValue(GLIndex::shellShader, "sunDirection", display::SUN_DIRECTION);

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
	uniforms::bindUniformValue(GLIndex::cloudShader, "pvmMatrix", cameraPVMmatrix);
	uniforms::bindUniformValue(GLIndex::cloudShader, "cameraPosition", camera.position);
	uniforms::bindUniformValue(GLIndex::cloudShader, "frameNumber", frameNumber);
	uniforms::bindUniformValue(GLIndex::cloudShader, "frameRate", display::MAX_FREQ);
	uniforms::bindUniformValue(GLIndex::cloudShader, "skyColour", display::SKY_COLOUR);
	uniforms::bindUniformValue(GLIndex::cloudShader, "cameraPosition", camera.position);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Uses vertices defined in vertex shader.
	utils::GLErrorcheck("Cloud Shader", true);


	
	//Cleanup.
	glBindVertexArray(0);
	glUseProgram(0);

	utils::GLErrorcheck("Display Shader", true);
}

}