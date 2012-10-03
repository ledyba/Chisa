//============================================================================
// Name        : Chisa.cpp
// Author      : PSI
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <GL/glfw.h>
#include "../../chisa/tk/Universe.h"
#include "../../chisa/logging/Exception.h"

using namespace std;

namespace chisa {
namespace entrypoint {
namespace pc {

tk::Universe* gUniverse;

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gUniverse->render();
	glfwSwapBuffers();
}

void reshape(int width, int height)
{
	gUniverse->reshape(tk::Box(width, height));
}

int main(int argc, char** argv) {
	if(glfwInit() == GL_FALSE){
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}
	try {
		if(GL_FALSE == glfwOpenWindow(640,480, 8, 8, 8, 8, 8, 8, GLFW_WINDOW)){
			std::cerr << "Failed to open window." << std::endl;
			return -1;
		}

		glfwSetWindowTitle("Chisa");

		logging::Logger log(std::cout, logging::Logger::TRACE_);
		gUniverse = new tk::Universe(log);
		glfwSetWindowSizeCallback(reshape);
		glfwSetWindowRefreshCallback(render);
		bool running=true;
		float last = glfwGetTime();
		float nextFrame = last+(1.0/60);
		while(running){
			gUniverse->idle(1000.0/60);
			const float now = glfwGetTime();
			if(now < nextFrame){
				glfwSleep(nextFrame-now);
				glfwSwapBuffers();
			}
			nextFrame+=(1.0/60);
			running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
		}

	} catch (logging::Exception& e) {
		std::cerr << "Exception caught at " << e.file() << ":" << e.line() << std::endl;
		std::cerr << "<msg>" << e.msg() << std::endl;
		glfwTerminate();
		return -2;
	} catch (...) {
		std::cerr << "caught unknwon error." << std::endl;
		glfwTerminate();
		return -3;
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}

}}}


int main(int argc, char** argv) {
	return chisa::entrypoint::pc::main(argc, argv);
}

