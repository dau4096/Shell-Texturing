
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "src/includes.h"
#include "src/physics.h"
#include "src/graphics.h"
#include "src/utils.h"




GLFWwindow* Window;


void framebufferSizeCallback(GLFWwindow* Window, int width, int height) {
	glViewport(0, 0, width, height);

	currentWindowResolution = glm::ivec2(width, height);
	currentRenderResolution = glm::ivec2(
		glm::min(width, desiredRenderResolution.x),
		glm::min(height, desiredRenderResolution.y)
	);
}





const std::vector<int> monitoredKeys = {
	GLFW_KEY_W, GLFW_KEY_S,
	GLFW_KEY_A, GLFW_KEY_D,
	GLFW_KEY_E, GLFW_KEY_Q,
	GLFW_KEY_SPACE, GLFW_KEY_1,
	GLFW_KEY_LEFT_SHIFT,
	GLFW_KEY_LEFT_CONTROL,
	GLFW_KEY_ESCAPE,
	GLFW_KEY_I, GLFW_KEY_O
};

double cursorXPos, cursorYPos, cursorXPosPrev, cursorYPosPrev;

void handleInputs() {
	glfwPollEvents();

	//Get inputs for this frame
	for (int key : monitoredKeys) {
		int keyState = glfwGetKey(Window, key);
		if (keyState == GLFW_PRESS) {
			keyMap[key] = true;

		} else if (keyState == GLFW_RELEASE) {
			keyMap[key] = false;
		}
	}


	if (keyMap[GLFW_KEY_1]) {
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	} else {
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(Window, &cursorXPos, &cursorYPos);
	}

	if (keyMap[GLFW_KEY_I]) {camera.FOV *= 0.975f; std::cout << (camera.FOV*constants::TO_DEG) << std::endl;}
	if (keyMap[GLFW_KEY_O]) {camera.FOV /= 0.975f; std::cout << (camera.FOV*constants::TO_DEG) << std::endl;}
	camera.FOV = glm::clamp(camera.FOV, 0.05f, constants::PI/2.0f);


	//View rotation
	double cursorXDelta = cursorXPos - cursorXPosPrev;
	double cursorYDelta = cursorYPos - cursorYPosPrev;
	camera.viewAngle.x += cursorXDelta * constants::TO_RAD * constants::CAMERA_TURN_SPEED;
	camera.viewAngle.x = fmodf(camera.viewAngle.x + constants::PI*3.0f, constants::PI2) - constants::PI;
	double dY = cursorYDelta * constants::TO_RAD * constants::CAMERA_TURN_SPEED;
	camera.viewAngle.y = glm::clamp(float(camera.viewAngle.y-dY), -0.499f*constants::PI, 0.499f*constants::PI);
}





float sunAngle = -0.5f;
#ifdef TRACK_SUN
int imgNumber = -1;
void trackSunScreenshots(const glm::vec3& sunDirection) {
	camera.position = glm::vec3(11.3364f, 5.07466f, 2.00475f);
	//print(camera.position); 

	camera.viewAngle = glm::vec2(
		glm::atan(sunDirection.x, sunDirection.y),
		glm::asin(sunDirection.z)
	); //Track the sun.
}
#endif



int main() {
	try { //Catch exceptions
	currentWindowResolution = display::INITIAL_SCREEN_RESOLUTION;
	desiredRenderResolution = display::RENDER_RESOLUTION;
	currentRenderResolution = glm::ivec2(
		glm::min(currentWindowResolution.x, desiredRenderResolution.x),
		glm::min(currentWindowResolution.y, desiredRenderResolution.y)
	);


	Window = graphics::initializeWindow(currentWindowResolution.x, currentWindowResolution.y, "Shell-Texturing/main");
	glfwSetFramebufferSizeCallback(Window, framebufferSizeCallback);
	glfwGetCursorPos(Window, &cursorXPos, &cursorYPos);
	glfwSwapInterval(1);

	cursorXPosPrev = cursorXPos;
	cursorYPosPrev = cursorYPos;
	utils::GLErrorcheck("Window Creation", true);

	//glm::vec3 sunDirection = glm::normalize(glm::vec3(0.25f, 0.25f, 1.0f));
	//glm::vec3 sunDirection = glm::normalize(glm::vec3(1.0f, 0.0f, -0.0625f));
	glm::vec3 sunDirection = glm::normalize(glm::vec3(0.5f, 0.5f, 0.5f));
	graphics::prepareOpenGL(sunDirection);
	double maxFrameTime = 1.0f/display::MAX_FREQ;
	for (int key : monitoredKeys) {
		keyMap[key] = false;
	}


	//Timer queries;
	GLuint timerQuery;
	GLuint64 GPUnanosecs; //Nanoseconds
	glGenQueries(1, &timerQuery);


	frameNumber = 0u;
	while (!glfwWindowShouldClose(Window)) {
		double frameStart = glfwGetTime();
		handleInputs();
		if (keyMap[GLFW_KEY_ESCAPE]) {
			break; //Quit
		}



		glm::vec3 sunDirection = glm::normalize(glm::vec3(
			cos(sunAngle), 1.0f, sin(sunAngle)
		));


		#ifdef TRACK_SUN
		trackSunScreenshots(sunDirection);
		sunAngle += 0.01f;
		if (sunAngle > constants::PI + 0.5f) {break;}
		#else
		sunAngle += 0.0025f;
		if (sunAngle > constants::PI + 0.5f) {sunAngle = -0.5f;}
		#endif

		physics::cameraMove();
		if constexpr (dev::SHOW_VALUES) {
			utils::print(camera.position);
			utils::print(camera.viewAngle);
		}

		glBeginQuery(GL_TIME_ELAPSED, timerQuery);
		frame::draw(sunDirection);
		glEndQuery(GL_TIME_ELAPSED);
		glFinish();


		#ifdef TRACK_SUN
		if (imgNumber == -1) {imgNumber++;}
		else {graphics::saveScreenshot("imgSequence", std::to_string(imgNumber++));}

		#else

		while (glfwGetTime() - frameStart < maxFrameTime) {std::this_thread::yield();}
		if constexpr (dev::SHOW_FREQ_CONSOLE) {
			glGetQueryObjectui64v(timerQuery, GL_QUERY_RESULT, &GPUnanosecs);
			double ms = GPUnanosecs / 1e6;
			std::cout << "Frame #" << frameNumber << " took " << std::setprecision(2) << ms << "ms / Hypothetical framerate: " << static_cast<int>(1000.0d/ms) << endl;
		}

		cursorXPosPrev = cursorXPos;
		cursorYPosPrev = cursorYPos;
		#endif
		glfwSwapBuffers(Window);
		frameNumber++;
	}


	glfwDestroyWindow(Window);
	glfwTerminate();
	return 0;


	//Catch exceptions.
	} catch (const std::exception& e) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		std::cerr << "An exception was thrown: " << e.what() << std::endl;
		utils::pause();
		return -1;
	} catch (...) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		std::cerr << "An unspecified exception was thrown." << std::endl;
		utils::pause();
		return -1;
	}
}
