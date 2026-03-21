#include "App.hpp"

#include <iostream>

int main() {
	try {
		auto app = App{ 1280, 720, "Application" };
		app.run();
	}
	catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}