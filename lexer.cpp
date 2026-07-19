//
// Created by leo on 7/19/26.
//

#include "lexer.h"
#include "token.h"

#include <cctype>
#include <stdexcept>
#include <utility>

const std::unordered_map<std::string, TokenType> Lexer::keywords_ = {
    {"Mainīgais", TokenType::Mainigais},
    {"Funkcija", TokenType::Funkcija},
    {"Ja", TokenType::Ja},
    {"Kamēr", TokenType::Kamer},
    {"Atgriezt", TokenType::Atgriezt},
    {"Citādi", TokenType::Citadi},

    {"skaitlis", TokenType::Skaitlis},
    {"teksts", TokenType::Teksts},
    {"loģisks", TokenType::Logisks},
    {"masīvs", TokenType::Masivs},
    {"patiess", TokenType::BooleanLiteral},
    {"aplams", TokenType::BooleanLiteral},

    {"ir", TokenType::Ir},
    {"vienāds", TokenType::Vienads},
    {"mazāks", TokenType::Mazaks},
    {"lielāks", TokenType::Greater},
    {"par", TokenType::Par},
    {"ar", TokenType::Ar},
    {"pieskaitīt", TokenType::Pieskaitit},
    {"un", TokenType::Un},
    {"vai", TokenType::Vai},
    {"nav", TokenType::Nav},
    {"atlikums", TokenType::Atlikums},

    {"Izvadīt", TokenType::Izvadit}
};

Lexer::Lexer(std::string source)
    : source_(std::move(source))
{
}

std::vector<Token> Lexer::tokenize()
{
    tokens_.clear();

    start_ = 0;
    current_ = 0;

    line_ = 1;
    column_ = 1;
    tokenColumn_ = 1;

    while (!isAtEnd()) {
        start_ = current_;
        tokenColumn_ = column_;

        scanToken();
    }

    tokens_.push_back(Token{
        TokenType::EndOfFile,
        "",
        static_cast<int>(line_),
        static_cast<int>(column_)
    });

    return tokens_;
}

void Lexer::scanToken()
{
    const char character = advance();

    switch (character) {
        case '(':
            addToken(TokenType::LeftParenthesis);
            break;

        case ')':
            addToken(TokenType::RightParenthesis);
            break;

        case '{':
            addToken(TokenType::LeftBrace);
            break;

        case '[':
            addToken(TokenType::LeftBracket);
            break;

        case ']':
            addToken(TokenType::RightBracket);
            break;

        case '}':
            addToken(TokenType::RightBrace);
            break;

        case ',':
            addToken(TokenType::Comma);
            break;

        case ';':
            addToken(TokenType::Semicolon);
            break;

        case '+':
            addToken(TokenType::Plus);
            break;

        case '-':
            addToken(TokenType::Minus);
            break;

        case '*':
            addToken(TokenType::Star);
            break;

        case '/':
            if (match('/')) {
                skipLineComment();
            } else if (match('*')) {
                skipBlockComment();
            } else {
                addToken(TokenType::Slash);
            }
            break;

        case '"':
            scanString();
            break;

        case ' ':
        case '\t':
        case '\r':
            // Whitespace is ignored.
            break;

        case '\n':
            ++line_;
            column_ = 1;
            break;

        default:
            if (isAsciiDigit(character)) {
                scanNumber();
            } else if (isIdentifierStart(character)) {
                scanIdentifier();
            } else {
                error(
                    "Unexpected character '" +
                    std::string(1, character) +
                    "'."
                );
            }

            break;
    }
}

void Lexer::scanIdentifier()
{
    while (isIdentifierContinuation(peek())) {
        advance();
    }

    const std::string text = currentText();

    const auto keyword = keywords_.find(text);

    if (keyword != keywords_.end()) {
        addToken(keyword->second, text);
    } else {
        addToken(TokenType::Identifier, text);
    }
}

void Lexer::scanNumber()
{
    while (isAsciiDigit(peek())) {
        advance();
    }

    // Decimal number, for example: 14.5
    if (peek() == '.' && isAsciiDigit(peekNext())) {
        advance();

        while (isAsciiDigit(peek())) {
            advance();
        }
    }

    addToken(TokenType::NumberLiteral);
}

void Lexer::scanString()
{
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            advance();

            ++line_;
            column_ = 1;
        } else {
            advance();
        }
    }

    if (isAtEnd()) {
        error("Unterminated string.");
    }

    // Consume the closing quotation mark.
    advance();

    // Store the string contents without the quotation marks.
    const std::size_t contentStart = start_ + 1;
    const std::size_t contentLength = current_ - start_ - 2;

    addToken(
        TokenType::StringLiteral,
        source_.substr(contentStart, contentLength)
    );
}

void Lexer::skipLineComment()
{
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

void Lexer::skipBlockComment()
{
    while (!isAtEnd()) {
        if (peek() == '*' && peekNext() == '/') {
            advance();
            advance();
            return;
        }

        if (peek() == '\n') {
            advance();

            ++line_;
            column_ = 1;
        } else {
            advance();
        }
    }

    error("Unterminated block comment.");
}

void Lexer::addToken(TokenType type)
{
    addToken(type, currentText());
}

void Lexer::addToken(TokenType type, std::string text)
{
    tokens_.push_back(Token{
        type,
        std::move(text),
        static_cast<int>(line_),
        static_cast<int>(tokenColumn_)
    });
}

bool Lexer::isAtEnd() const noexcept
{
    return current_ >= source_.size();
}

char Lexer::advance()
{
    const char character = source_[current_];

    ++current_;
    ++column_;

    return character;
}

char Lexer::peek() const noexcept
{
    if (isAtEnd()) {
        return '\0';
    }

    return source_[current_];
}

char Lexer::peekNext() const noexcept
{
    if (current_ + 1 >= source_.size()) {
        return '\0';
    }

    return source_[current_ + 1];
}

bool Lexer::match(char expected)
{
    if (isAtEnd() || source_[current_] != expected) {
        return false;
    }

    ++current_;
    ++column_;

    return true;
}

std::string Lexer::currentText() const
{
    return source_.substr(start_, current_ - start_);
}

bool Lexer::isAsciiDigit(char character) noexcept
{
    return character >= '0' && character <= '9';
}

bool Lexer::isIdentifierStart(char character) noexcept
{
    const auto byte = static_cast<unsigned char>(character);

    // ASCII letters, underscore, @, and # may begin identifiers.
    if (
        (character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z') ||
        character == '_' ||
        character == '@' ||
        character == '#'
    ) {
        return true;
    }

    // Any non-ASCII byte is accepted. This allows raw UTF-8 identifiers
    // such as "ābols" without decoding individual Unicode characters.
    return byte >= 0x80;
}

bool Lexer::isIdentifierContinuation(char character) noexcept
{
    return isIdentifierStart(character) || isAsciiDigit(character);
}

[[noreturn]]
void Lexer::error(const std::string& message) const
{
    throw std::runtime_error(
        "Lexer error at line " +
        std::to_string(line_) +
        ", column " +
        std::to_string(tokenColumn_) +
        ": " +
        message
    );
}
