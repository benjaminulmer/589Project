#include "Program.h"

int main(int argc, char* argv[]) {
	std::cout << "Num of args: " << argc << std::endl;
	Program p;
	if (argc == 1) {
		p = Program();
	}
	p.start();
	return 0;
}
