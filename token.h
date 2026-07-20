//
// Created by leo on 7/19/26.
//

#ifndef LATGALITE_TOKEN_H
#define LATGALITE_TOKEN_H
#include <string>

// https://en.wikipedia.org/wiki/Lexical_analysis

/*
Examples of common tokens:

identifier - Names assigned by the programmer                           x, color, UP
keyword - Reserved words of the language                                if, while, return
separator/punctuator - Punctuation characters and paired delimiters.    }, (, ;
operator - Symbols that operate on arguments and produce results.       +, <, =
literal - Numeric, logical, textual, and reference literals.	        true, 6.02e23, "music"
comment -	Line or block comments. Usually discarded.
whitespace - Groups of non-printable characters. Usually discarded.

Consider this expression in the C programming language:
   x = a + b * 2;
The lexical analysis of this expression yields the following sequence of tokens:
   [(identifier, 'x'), (operator, '='), (identifier, 'a'), (operator, '+'), (identifier, 'b'), (operator, '*'), (literal, '2'), (separator, ';')]
*/

enum class TokenType {
    // literal, identifier
    Identifier,
    NumberLiteral,
    StringLiteral,
    BooleanLiteral,

    // type names
    Skaitlis,
    Teksts,
    Logisks,
    Masivs,

    // keyword
    Mainigais,
    Funkcija,
    Ja,
    Kamer,
    Salidzini,
    Sakrit,
    Atgriezt,
    Ir,
    Mazaks,
    Par,
    Ar,
    Vienads,
    Pieskaitit,
    Izvadit,
    Citadi,
    Un,
    Vai,
    Nav,
    Atlikums,

    // operator
    Plus,
    Minus,
    Star,
    Slash,
    Assign, // =
    Equal, // ==
    Less,
    Greater,

    // separator
    LeftParenthesis, // (
    RightParenthesis, // )
    LeftBracket, // [
    RightBracket, // ]
    LeftBrace, // {
    RightBrace, // }
    Comma, // ,
    Colon, // :
    Semicolon, // ;

    // special
    EndOfFile,
    Invalid
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;
};

#endif //LATGALITE_TOKEN_H
