#include "interpreter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

int main(int argc, char** argv) {
    try {
        std::ostringstream source;
        if (argc == 2) {
            std::ifstream file(argv[1]);
            if (!file) throw std::runtime_error(std::string("Nevar atvērt failu: ") + argv[1]);
            source << file.rdbuf();
        } else if (argc == 1) {
            source << std::cin.rdbuf();
        } else {
            std::cerr << "Lietošana: latgalite [fails.lat]\n";
            return 2;
        }
        interpret(source.str(), std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
