#ifndef INCLUDES_H
#define INCLUDES_H


//////// OS SPECIFIC ////////
#if defined(_WIN32) //Windows specific headers
    #define WIN32_LEAN_AND_MEAN //Reduces the amount of things Windows.h will try to add.
    #include <Windows.h>
#elif defined(__linux__) //Linux specific headers
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif
//////// OS SPECIFIC ////////



//////// OPENGL HEADERS ////////
#include <GL/glew.h>
#include <GL/gl.h>
//////// OPENGL HEADERS ////////



//////// GLFW HEADERS ////////
#include <GLFW/glfw3.h>
//////// GLFW HEADERS ////////



//////// GLM HEADERS ////////
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
//////// GLM HEADERS ////////



//////// C STANDARD LIBRARY HEADERS ////////
#include <cassert>
#include <cmath>
#include <csignal>
#include <cstring>
#include <ctime>
//////// C STANDARD LIBRARY HEADERS ////////



//////// C++ STANDARD LIBRARY HEADERS ////////
#include <algorithm>
#include <array>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <unordered_map>
#include <vector>
//////// C++ STANDARD LIBRARY HEADERS ////////


#endif
