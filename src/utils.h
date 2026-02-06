#ifndef UTILS_H
#define UTILS_H

#include "includes.h"
#include "global.h"
#include "constants.h"

using namespace std;



//Utility functions
namespace utils {


	//Console related functions
	inline void hideConsole() {
	#ifdef _WIN32
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif
	}

	inline void showConsole() {
	#ifdef _WIN32
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	#endif
	}

	inline bool isConsoleVisible() {
	#ifdef _WIN32
		return IsWindowVisible(GetConsoleWindow());
	#elif defined(__linux__)
		return true;
	#endif 
	}


	static inline void print(std::string str) {
		if (isConsoleVisible()) {
			std::cout << str << std::endl;
		}
	}
	static inline void print(glm::vec2 vector, std::string name="") {
		if (isConsoleVisible()) {
			if (name.empty()) {
				std::cout << "(" << vector.x << ", " << vector.y << ")" << std::endl;
			} else {
				std::cout << name << " = (" << vector.x << ", " << vector.y << ")" << std::endl;
			}
		}
	}
	static inline void print(glm::vec3 vector, std::string name="") {
		if (isConsoleVisible()) {
			if (name.empty()) {
				std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
			} else {
				std::cout << name << " = (" << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
			}
		}
	}
	static inline void print(glm::vec4 vector, std::string name="") {
		if (isConsoleVisible()) {
			if (name.empty()) {
				std::cout << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")" << std::endl;
			} else {
				std::cout << name << " = (" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")" << std::endl;
			}
		}
	}
	static inline void print(glm::mat4 matrix, std::string name="") {
		if (isConsoleVisible()) {
			if (name.empty()) {
				std::cout << "[" << std::endl;
			} else {
				std::cout << name << " = [" << std::endl;
			}
			for (size_t x=0; x<4; x++) {
				std::cout << "	";
				for (size_t y=0; y<4; y++) {
					std::cout << matrix[x][y] << ", ";
				}
				std::cout << std::endl;
			}
			std::cout << "]" << std::endl;
		}
	}
	static inline void raise(std::string err) {
		std::cerr << err << std::endl;
		std::string end;
		std::cin >> end;
	}
	static inline void pause() {
		string pause;
		std::cin >> pause;
	}
	static inline void GLErrorcheck(std::string location = "", bool shouldPause = false) {
		GLenum GLError;
		GLError = glGetError();
		if (GLError != GL_NO_ERROR) {
			if (!utils::isConsoleVisible()) {
				utils::showConsole();
			}
			std::cerr << location << " | OpenGL error; " << GLError << std::endl;
			if (shouldPause) {pause();}
		}
	}

	std::string readFile(const std::string& filePath);

	static inline std::string getTimestamp() {
		time_t now = time(nullptr);
		struct tm* timeinfo = localtime(&now);

		std::ostringstream oss;
		oss << std::put_time(timeinfo, "%Y%m%d%H%M%S");

		return oss.str();
	}


	static inline bool replaceStringSingle(std::string& str, const std::string& from, const std::string& to) {
		//From: [https://stackoverflow.com/a/3418285]
	    size_t start_pos = str.find(from);
	    if(start_pos == std::string::npos)
	        return false;
	    str.replace(start_pos, from.length(), to);
	    return true;
	}



	static inline std::string strToLower(const std::string& input) {
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){return std::tolower(c);});
		return result;
	}

	static inline std::string strToUpper(const std::string& input) {
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){return std::toupper(c);});
		return result;
	}

	static inline bool isVec2NaN(glm::vec2 v) {return (std::isnan(v.x) || std::isnan(v.y));}
	static inline bool isVec3NaN(glm::vec3 v) {return (std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z));}


	float determinant(glm::vec2 vecA, glm::vec2 vecB);


	int RNGc(); //Client
	int RNGw(); //World
	void clearRNG(); //Reset both
	

}

#endif