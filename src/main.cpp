#include "Program.h"

int main(int argc, char* argv[]) {
	Program p;

	if (argc == 1) {
		std::cout << "Running with default settings" << std::endl;
		p = Program();
	}
	else if (argc == 2) {
		std::cout << "Running with only OBJ file" << std::endl;
		p = Program(argv[1]);
	}
	else if (argc == 3) {
		std::cout << "Running with OBJ file and explosion file" << std::endl;
		p = Program(argv[1], argv[2]);
	}
	else {
		std::cout << "Invalid number of arguments, running with default settings" << std::endl;
		p = Program();
	}
	p.start();
	return 0;
}
