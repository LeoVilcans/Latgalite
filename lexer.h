//
// Created by leo on 7/19/26.
//

#ifndef LATGALITE_LEXER_H
#define LATGALITE_LEXER_H

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "token.h"

class Lexer {
public:
    explicit Lexer(std::string source);

    [[nodiscard]]
    std::vector<Token> tokenize();

private:
    std::string source_;

    std::size_t start_ = 0; // byte index where the current token begins.
    std::size_t current_ = 0; // byte index currently being examined.

    std::size_t line_ = 1;
    std::size_t column_ = 1;
    std::size_t tokenColumn_ = 1; // stores where the token began, since column_ changes while scanning it.

    std::vector<Token> tokens_;

    // Maps keyword text such as "Ja" to its corresponding token type.
    static const std::unordered_map<std::string, TokenType> keywords_;

    void scanToken();

    void scanIdentifier();
    void scanNumber();
    void scanString();

    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();

    void addToken(TokenType type);
    void addToken(TokenType type, std::string text);

    [[nodiscard]]
    bool isAtEnd() const noexcept;

    char advance();

    [[nodiscard]]
    char peek() const noexcept;

    [[nodiscard]]
    char peekNext() const noexcept;

    bool match(char expected);

    [[nodiscard]]
    std::string currentText() const;

    [[nodiscard]] static bool isIdentifierStart(char c) noexcept;
    [[nodiscard]] static bool isIdentifierContinuation(char c) noexcept;
    [[nodiscard]] static bool isAsciiDigit(char c) noexcept;

    [[noreturn]]
    void error(const std::string& message) const;
};

#endif // LATGALITE_LEXER_H
