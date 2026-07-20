#include "interpreter.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

namespace {
int failures = 0;

void expectOutput(
    const std::string& name,
    const std::string& source,
    const std::string& input,
    const std::string& expected
) {
    std::istringstream inputStream(input);
    std::ostringstream outputStream;

    try {
        interpret(source, inputStream, outputStream);
        if (outputStream.str() == expected) return;
        std::cerr << name << ": expected output\n" << expected
                  << "but received\n" << outputStream.str();
    } catch (const std::exception& error) {
        std::cerr << name << ": unexpected error: " << error.what() << '\n';
    }
    ++failures;
}

void expectError(
    const std::string& name,
    const std::string& source,
    const std::string& input,
    const std::string& expectedFragment
) {
    std::istringstream inputStream(input);
    std::ostringstream outputStream;

    try {
        interpret(source, inputStream, outputStream);
        std::cerr << name << ": expected an error containing '" << expectedFragment << "'.\n";
    } catch (const std::exception& error) {
        if (std::string(error.what()).find(expectedFragment) != std::string::npos) return;
        std::cerr << name << ": expected an error containing '" << expectedFragment
                  << "', but received: " << error.what() << '\n';
    }
    ++failures;
}
}

int main() {
    expectOutput(
        "numeric input and conversion",
        R"(
Mainīgais skaitlis pirmais ir ievadīt();
Mainīgais skaitlis otrais ir ievadīt_skaitli();
Mainīgais skaitlis trešais ir " .5 ";
Izvadīt(pirmais + otrais + trešais);
pirmais ir "3,5";
Izvadīt(pirmais);
)",
        " 12,5 \n-2.25\n",
        "10.75\n3.5\n"
    );

    expectOutput(
        "text functions",
        R"(
Mainīgais masīvs daļas ir saskaldīt("viens||divi||||", "||");
Izvadīt(garums(daļas));
Izvadīt(daļas[0]);
Izvadīt(daļas[1]);
Izvadīt("<" + daļas[2] + ">");
Izvadīt("<" + daļas[3] + ">");
Izvadīt(savienot(daļas, "/"));
Izvadīt(savienot([1, patiess, "x"], "|"));
Izvadīt(satur("Latgalīte", "galī"));
Izvadīt(satur("Latgalīte", "xyz"));
Izvadīt(atrast("abcabc", "ca"));
Izvadīt(atrast("abcabc", "z"));
Izvadīt(aizstāt("aaaa", "aa", "b"));
Izvadīt(aizstāt("abcabc", "b", "XYZ"));
Mainīgais masīvs unicode ir saskaldīt("viens—divi—", "—");
Izvadīt(garums(unicode));
Izvadīt("<" + unicode[2] + ">");
)",
        "",
        "4\nviens\ndivi\n<>\n<>\nviens/divi//\n1|patiess|x\npatiess\naplams\n2\n-1\nbb\naXYZcaXYZc\n3\n<>\n"
    );

    expectError(
        "invalid numeric input",
        "Mainīgais skaitlis vērtība ir ievadīt();",
        "12abc\n",
        "Cannot convert '12abc' to a number."
    );
    expectError(
        "numeric input at end of file",
        "Mainīgais skaitlis vērtība ir ievadīt_skaitli();",
        "",
        "ievadīt_skaitli reached the end of input."
    );
    expectError(
        "empty split separator",
        "Mainīgais masīvs daļas ir saskaldīt(\"abc\", \"\");",
        "",
        "saskaldīt requires a non-empty separator."
    );
    expectError(
        "empty replacement fragment",
        "Mainīgais teksts vērtība ir aizstāt(\"abc\", \"\", \"x\");",
        "",
        "aizstāt requires a non-empty fragment."
    );

    if (failures != 0) {
        std::cerr << failures << " test(s) failed.\n";
        return 1;
    }
    return 0;
}
