#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "object.h"
#include "eval.h"
int main(int argc, char **argv) {
    // Check if we want the REPL or to parse a program
    if (argc == 2) {
        std::string input = read_file(argv[1]);
        auto out = eval_program(input);
        std::cout << inspect(out.value()) << std::endl;
        return 0;
    }

    std::shared_ptr<Environment> env = std::make_shared<Environment>(Environment());
    ObjectResult result = Null();
    std::cout << "Welcome to the Monke REPL!" << std::endl;
    while (true) {
        std::cout << ">> ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") break;

        // TODO: Add inputs for help, clear, etc
        auto *l = new Lexer(input);
        Parser p = Parser(l);
        Program program = p.parse_program();
        for (auto &s: program.statements) {
            result = eval(s, env);
            if (!result.has_value()) {
                std::cout << "Error: " << get_msg(result.error()) << std::endl;
                continue;
            };
            if (!std::holds_alternative<Null>(result.value())) {
                std::cout << inspect(result.value()) << std::endl;
            }
        }
    }
    return 0;
}
