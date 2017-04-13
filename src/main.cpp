#include "Program.h"

int main(int argc, char* argv[]) {
	std::cout << "Num of args: " << argc << std::endl;
	Program p;

	if (argc == 1) {
		p = Program();
	}
	else if (argc == 2) {
		p = Program(argv[1]);
	}
	else if (argc == 3) {
		p = Program(argv[1], argv[2]);
	}
	else {
		std::cout << "Invalid number of arguments, starting with default settings" << std::endl;
		p = Program();
	}
	p.start();
	return 0;
}
