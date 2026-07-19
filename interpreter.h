#ifndef LATGALITE_INTERPRETER_H
#define LATGALITE_INTERPRETER_H

#include <iosfwd>
#include <string>

void interpret(const std::string& source, std::istream& input, std::ostream& output);

#endif
