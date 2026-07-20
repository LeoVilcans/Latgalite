#include "interpreter.h"
#include "lexer.h"

#include <cctype>
#include <cmath>
#include <memory>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace {
struct Value;
using Array = std::vector<Value>;
using ArrayPtr = std::shared_ptr<Array>;

struct Value {
    std::variant<double, std::string, bool, ArrayPtr> data;
    Value() : data(0.0) {}
    Value(double value) : data(value) {}
    Value(std::string value) : data(std::move(value)) {}
    Value(const char* value) : data(std::string(value)) {}
    Value(bool value) : data(value) {}
    Value(ArrayPtr value) : data(std::move(value)) {}
};

std::string show(const Value& value) {
    if (const auto* text = std::get_if<std::string>(&value.data)) return *text;
    if (const auto* boolean = std::get_if<bool>(&value.data)) return *boolean ? "patiess" : "aplams";
    if (const auto* array = std::get_if<ArrayPtr>(&value.data)) {
        std::string result = "[";
        for (std::size_t i = 0; i < (*array)->size(); ++i) {
            if (i) result += ", ";
            result += show((**array)[i]);
        }
        return result + "]";
    }
    const double number = std::get<double>(value.data);
    if (std::floor(number) == number) return std::to_string(static_cast<long long>(number));
    std::ostringstream out; out << number; return out.str();
}

bool valuesEqual(const Value& left, const Value& right) {
    return show(left) == show(right);
}

struct Expr {
    enum class Kind { Literal, Variable, Binary, Unary, Call, Array, Index } kind;
    Token token{};
    Value literal{};
    std::shared_ptr<Expr> left, right;
    std::vector<std::shared_ptr<Expr>> arguments;
};
using ExprPtr = std::shared_ptr<Expr>;

struct Stmt;
using StmtPtr = std::shared_ptr<Stmt>;

struct SwitchArm {
    ExprPtr expression;
    std::vector<StmtPtr> body;
};

struct Stmt {
    enum class Kind { Variable, Function, Print, While, If, Switch, Return, Assign, AddAssign, Expression } kind;
    Token token{};
    TokenType declaredType{TokenType::Invalid};
    ExprPtr expression, target;
    std::vector<StmtPtr> body, elseBody;
    std::vector<SwitchArm> switchArms;
    std::vector<std::pair<TokenType, Token>> parameters;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}
    std::vector<StmtPtr> parse() {
        std::vector<StmtPtr> result;
        while (!check(TokenType::EndOfFile)) result.push_back(statement());
        return result;
    }
private:
    std::vector<Token> tokens_; std::size_t current_{};
    const Token& peek() const { return tokens_[current_]; }
    const Token& previous() const { return tokens_[current_ - 1]; }
    bool check(TokenType type) const { return peek().type == type; }
    bool checkNext(TokenType type) const { return current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == type; }
    bool match(TokenType type) { if (!check(type)) return false; ++current_; return true; }
    Token consume(TokenType type, const std::string& message) {
        if (check(type)) return tokens_[current_++];
        throw parseError(peek(), message);
    }
    static std::runtime_error parseError(const Token& token, const std::string& message) {
        return std::runtime_error("Parse error at " + std::to_string(token.line) + ":" +
                                  std::to_string(token.column) + ": " + message);
    }
    TokenType typeName() {
        if (match(TokenType::Skaitlis)) return TokenType::Skaitlis;
        if (match(TokenType::Teksts)) return TokenType::Teksts;
        if (match(TokenType::Logisks)) return TokenType::Logisks;
        if (match(TokenType::Masivs)) return TokenType::Masivs;
        throw parseError(peek(), "Expected type 'skaitlis', 'teksts', 'loģisks', or 'masīvs'.");
    }
    StmtPtr statement() {
        if (match(TokenType::Mainigais)) return variableDeclaration();
        if (match(TokenType::Funkcija)) return functionDeclaration();
        if (match(TokenType::Izvadit)) return printStatement();
        if (match(TokenType::Kamer)) return controlStatement(Stmt::Kind::While);
        if (match(TokenType::Ja)) return controlStatement(Stmt::Kind::If);
        if (match(TokenType::Salidzini)) return switchStatement();
        if (match(TokenType::Atgriezt)) {
            auto stmt = std::make_shared<Stmt>(); stmt->kind = Stmt::Kind::Return; stmt->token = previous();
            stmt->expression = expression(); consume(TokenType::Semicolon, "Expected ';' after return value."); return stmt;
        }

        // An assignment target may be either a variable or an indexed array element.
        if (check(TokenType::Identifier)) {
            const auto saved = current_;
            ExprPtr target = postfix();
            if ((target->kind == Expr::Kind::Variable || target->kind == Expr::Kind::Index) &&
                (check(TokenType::Ir) || check(TokenType::Pieskaitit))) {
                auto stmt = std::make_shared<Stmt>(); stmt->target = target; stmt->token = target->token;
                stmt->kind = match(TokenType::Ir) ? Stmt::Kind::Assign : Stmt::Kind::AddAssign;
                if (stmt->kind == Stmt::Kind::AddAssign) consume(TokenType::Pieskaitit, "Expected 'pieskaitīt'.");
                stmt->expression = expression(); consume(TokenType::Semicolon, "Expected ';'."); return stmt;
            }
            current_ = saved;
        }
        auto stmt = std::make_shared<Stmt>(); stmt->kind = Stmt::Kind::Expression;
        stmt->expression = expression(); consume(TokenType::Semicolon, "Expected ';'."); return stmt;
    }
    StmtPtr variableDeclaration() {
        auto stmt = std::make_shared<Stmt>(); stmt->kind = Stmt::Kind::Variable; stmt->declaredType = typeName();
        stmt->token = consume(TokenType::Identifier, "Expected variable name.");
        consume(TokenType::Ir, "Expected 'ir' after variable name."); stmt->expression = expression();
        consume(TokenType::Semicolon, "Expected ';' after declaration."); return stmt;
    }
    StmtPtr functionDeclaration() {
        auto stmt = std::make_shared<Stmt>(); stmt->kind = Stmt::Kind::Function;
        stmt->token = consume(TokenType::Identifier, "Expected function name."); consume(TokenType::LeftParenthesis, "Expected '('.");
        if (!check(TokenType::RightParenthesis)) do {
            auto type = typeName(); stmt->parameters.emplace_back(type, consume(TokenType::Identifier, "Expected parameter name."));
        } while (match(TokenType::Comma));
        consume(TokenType::RightParenthesis, "Expected ')' after parameters."); stmt->body = block(); return stmt;
    }
    StmtPtr printStatement() {
        auto stmt = std::make_shared<Stmt>(); stmt->kind = Stmt::Kind::Print; stmt->token = previous();
        consume(TokenType::LeftParenthesis, "Expected '(' after 'Izvadīt'."); stmt->expression = expression();
        consume(TokenType::RightParenthesis, "Expected ')'."); consume(TokenType::Semicolon, "Expected ';'."); return stmt;
    }
    std::vector<StmtPtr> switchArmBody() {
        std::vector<StmtPtr> result;
        while (!check(TokenType::Sakrit) && !check(TokenType::Citadi) &&
               !check(TokenType::RightBrace) && !check(TokenType::EndOfFile)) {
            result.push_back(statement());
        }
        return result;
    }
    StmtPtr switchStatement() {
        auto stmt = std::make_shared<Stmt>(); stmt->kind = Stmt::Kind::Switch; stmt->token = previous();
        consume(TokenType::LeftParenthesis, "Expected '(' after 'Salīdzini'."); stmt->expression = expression();
        consume(TokenType::RightParenthesis, "Expected ')' after comparison value.");
        consume(TokenType::LeftBrace, "Expected '{' before comparison branches.");

        bool hasBranch = false;
        bool hasDefault = false;
        while (!check(TokenType::RightBrace) && !check(TokenType::EndOfFile)) {
            if (match(TokenType::Sakrit)) {
                if (hasDefault) throw parseError(previous(), "'Citādi' must be the final comparison branch.");
                SwitchArm arm;
                consume(TokenType::Ar, "Expected 'ar' after 'Sakrīt'."); arm.expression = expression();
                consume(TokenType::Colon, "Expected ':' after matching value."); arm.body = switchArmBody();
                stmt->switchArms.push_back(std::move(arm)); hasBranch = true; continue;
            }
            if (match(TokenType::Citadi)) {
                if (hasDefault) throw parseError(previous(), "A comparison can have only one 'Citādi' branch.");
                consume(TokenType::Colon, "Expected ':' after 'Citādi'.");
                stmt->elseBody = switchArmBody(); hasDefault = true; continue;
            }
            throw parseError(peek(), "Expected 'Sakrīt ar', 'Citādi', or '}'.");
        }
        if (!hasBranch && !hasDefault) throw parseError(peek(), "Expected at least one comparison branch.");
        consume(TokenType::RightBrace, "Expected '}' after comparison branches."); return stmt;
    }
    StmtPtr controlStatement(Stmt::Kind kind) {
        auto stmt = std::make_shared<Stmt>(); stmt->kind = kind; stmt->token = previous();
        consume(TokenType::LeftParenthesis, "Expected '('."); stmt->expression = expression();
        consume(TokenType::RightParenthesis, "Expected ')'."); stmt->body = block();
        if (kind == Stmt::Kind::If && check(TokenType::Citadi) && checkNext(TokenType::LeftBrace)) {
            match(TokenType::Citadi); stmt->elseBody = block();
        }
        return stmt;
    }
    std::vector<StmtPtr> block() {
        consume(TokenType::LeftBrace, "Expected '{'."); std::vector<StmtPtr> result;
        while (!check(TokenType::RightBrace) && !check(TokenType::EndOfFile)) result.push_back(statement());
        consume(TokenType::RightBrace, "Expected '}'."); return result;
    }
    ExprPtr expression() { return logicalOr(); }
    ExprPtr binary(ExprPtr left, Token op, ExprPtr right) {
        auto result = std::make_shared<Expr>(); result->kind = Expr::Kind::Binary; result->token = std::move(op);
        result->left = std::move(left); result->right = std::move(right); return result;
    }
    ExprPtr logicalOr() {
        auto expr = logicalAnd();
        while (match(TokenType::Vai)) { Token op = previous(); expr = binary(expr, op, logicalAnd()); }
        return expr;
    }
    ExprPtr logicalAnd() {
        auto expr = comparison();
        while (match(TokenType::Un)) { Token op = previous(); expr = binary(expr, op, comparison()); }
        return expr;
    }
    ExprPtr comparison() {
        auto expr = addition();
        while (check(TokenType::Mazaks) || check(TokenType::Greater) || check(TokenType::Ir)) {
            Token op = tokens_[current_++];
            if (op.type == TokenType::Ir) { consume(TokenType::Vienads, "Expected 'vienāds'."); consume(TokenType::Ar, "Expected 'ar'."); }
            else consume(TokenType::Par, "Expected 'par'.");
            expr = binary(expr, op, addition());
        }
        return expr;
    }
    ExprPtr addition() {
        auto expr = multiplication();
        while (check(TokenType::Plus) || check(TokenType::Minus)) { Token op = tokens_[current_++]; expr = binary(expr, op, multiplication()); }
        return expr;
    }
    ExprPtr multiplication() {
        auto expr = unary();
        while (check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::Atlikums)) {
            Token op = tokens_[current_++]; expr = binary(expr, op, unary());
        }
        return expr;
    }
    ExprPtr unary() {
        if (match(TokenType::Nav) || match(TokenType::Minus)) {
            Token op = previous();
            auto expr = std::make_shared<Expr>(); expr->kind = Expr::Kind::Unary; expr->token = std::move(op); expr->right = unary(); return expr;
        }
        return postfix();
    }
    ExprPtr postfix() {
        auto expr = primary();
        while (match(TokenType::LeftBracket)) {
            auto index = std::make_shared<Expr>(); index->kind = Expr::Kind::Index; index->token = previous();
            index->left = expr; index->right = expression(); consume(TokenType::RightBracket, "Expected ']' after index."); expr = index;
        }
        return expr;
    }
    ExprPtr primary() {
        if (match(TokenType::NumberLiteral)) { auto e = std::make_shared<Expr>(); e->kind = Expr::Kind::Literal; e->token = previous(); e->literal = std::stod(e->token.text); return e; }
        if (match(TokenType::StringLiteral)) { auto e = std::make_shared<Expr>(); e->kind = Expr::Kind::Literal; e->token = previous(); e->literal = e->token.text; return e; }
        if (match(TokenType::BooleanLiteral)) { auto e = std::make_shared<Expr>(); e->kind = Expr::Kind::Literal; e->token = previous(); e->literal = e->token.text == "patiess"; return e; }
        if (match(TokenType::LeftBracket)) {
            auto e = std::make_shared<Expr>(); e->kind = Expr::Kind::Array; e->token = previous();
            if (!check(TokenType::RightBracket)) do { e->arguments.push_back(expression()); } while (match(TokenType::Comma));
            consume(TokenType::RightBracket, "Expected ']' after array elements."); return e;
        }
        if (match(TokenType::Identifier)) {
            auto e = std::make_shared<Expr>(); e->token = previous();
            if (match(TokenType::LeftParenthesis)) {
                e->kind = Expr::Kind::Call;
                if (!check(TokenType::RightParenthesis)) do { e->arguments.push_back(expression()); } while (match(TokenType::Comma));
                consume(TokenType::RightParenthesis, "Expected ')' after arguments.");
            } else e->kind = Expr::Kind::Variable;
            return e;
        }
        if (match(TokenType::LeftParenthesis)) { auto e = expression(); consume(TokenType::RightParenthesis, "Expected ')'."); return e; }
        throw parseError(peek(), "Expected expression.");
    }
};

struct Variable { TokenType type; Value value; };
struct Environment {
    std::unordered_map<std::string, Variable> values; std::shared_ptr<Environment> parent;
    Variable& get(const Token& name) {
        if (auto found = values.find(name.text); found != values.end()) return found->second;
        if (parent) return parent->get(name);
        throw std::runtime_error("Undefined variable '" + name.text + "' at " + std::to_string(name.line) + ":" + std::to_string(name.column) + ".");
    }
};
struct Function { std::vector<std::pair<TokenType, Token>> parameters; std::vector<StmtPtr> body; std::shared_ptr<Environment> closure; };
struct ReturnSignal { Value value; };

class Runtime {
public:
    Runtime(std::istream& input, std::ostream& output)
        : input_(input), output_(output), global_(std::make_shared<Environment>()) {}
    void run(const std::vector<StmtPtr>& statements) { executeBlock(statements, global_); }
private:
    std::istream& input_; std::ostream& output_; std::shared_ptr<Environment> global_; std::unordered_map<std::string, Function> functions_;
    [[noreturn]] static void runtimeError(const Token& token, const std::string& message) {
        throw std::runtime_error("Runtime error at " + std::to_string(token.line) + ":" + std::to_string(token.column) + ": " + message);
    }
    static bool truthy(const Value& value) {
        if (const auto* b = std::get_if<bool>(&value.data)) return *b;
        if (const auto* n = std::get_if<double>(&value.data)) return *n != 0;
        if (const auto* s = std::get_if<std::string>(&value.data)) return !s->empty();
        return !std::get<ArrayPtr>(value.data)->empty();
    }
    static double number(const Value& value, const Token& token) {
        if (const auto* n = std::get_if<double>(&value.data)) return *n;
        runtimeError(token, "Expected a number.");
    }
    static std::string text(const Value& value, const Token& token, const std::string& functionName) {
        if (const auto* result = std::get_if<std::string>(&value.data)) return *result;
        runtimeError(token, functionName + " expects text arguments.");
    }
    static double parseNumberText(const std::string& input, const Token& token) {
        std::size_t begin = 0;
        while (begin < input.size() && std::isspace(static_cast<unsigned char>(input[begin]))) ++begin;
        std::size_t end = input.size();
        while (end > begin && std::isspace(static_cast<unsigned char>(input[end - 1]))) --end;

        std::string normalized = input.substr(begin, end - begin);
        std::size_t position = 0;
        if (position < normalized.size() && (normalized[position] == '+' || normalized[position] == '-')) ++position;

        bool hasDigit = false;
        while (position < normalized.size() && std::isdigit(static_cast<unsigned char>(normalized[position]))) {
            hasDigit = true; ++position;
        }
        if (position < normalized.size() && (normalized[position] == '.' || normalized[position] == ',')) {
            normalized[position] = '.'; ++position;
            while (position < normalized.size() && std::isdigit(static_cast<unsigned char>(normalized[position]))) {
                hasDigit = true; ++position;
            }
        }
        if (!hasDigit || position != normalized.size()) {
            runtimeError(token, "Cannot convert '" + input + "' to a number.");
        }

        try {
            double result = std::stod(normalized);
            if (!std::isfinite(result)) runtimeError(token, "The entered number must be finite.");
            return result;
        } catch (const std::invalid_argument&) {
            runtimeError(token, "Cannot convert '" + input + "' to a number.");
        } catch (const std::out_of_range&) {
            runtimeError(token, "The entered number is outside the supported range.");
        }
    }
    static ArrayPtr array(const Value& value, const Token& token) {
        if (const auto* a = std::get_if<ArrayPtr>(&value.data)) return *a;
        runtimeError(token, "Expected an array.");
    }
    static std::size_t index(const Value& value, std::size_t size, const Token& token) {
        double raw = number(value, token);
        if (raw < 0 || std::floor(raw) != raw || raw >= static_cast<double>(size)) runtimeError(token, "Array index is out of bounds.");
        return static_cast<std::size_t>(raw);
    }
    static Value convert(TokenType type, const Value& value, const Token& token) {
        if (type == TokenType::Teksts) return show(value);
        if (type == TokenType::Skaitlis) {
            if (std::holds_alternative<double>(value.data)) return value;
            if (const auto* input = std::get_if<std::string>(&value.data)) return parseNumberText(*input, token);
            runtimeError(token, "A 'skaitlis' variable requires a number or numeric text.");
        }
        if (type == TokenType::Logisks && !std::holds_alternative<bool>(value.data)) runtimeError(token, "A 'loģisks' variable requires 'patiess' or 'aplams'.");
        if (type == TokenType::Masivs && !std::holds_alternative<ArrayPtr>(value.data)) runtimeError(token, "A 'masīvs' variable requires an array.");
        return value;
    }
    Value evaluateCall(const ExprPtr& expr, const std::shared_ptr<Environment>& env) {
        if (expr->token.text == "ievadīt") {
            if (!expr->arguments.empty()) runtimeError(expr->token, "ievadīt expects no arguments.");
            std::string line;
            if (!std::getline(input_, line)) return std::string("");
            return line;
        }
        if (expr->token.text == "ievadīt_skaitli") {
            if (!expr->arguments.empty()) runtimeError(expr->token, "ievadīt_skaitli expects no arguments.");
            std::string line;
            if (!std::getline(input_, line)) runtimeError(expr->token, "ievadīt_skaitli reached the end of input.");
            return parseNumberText(line, expr->token);
        }
        if (expr->token.text == "saskaldīt") {
            if (expr->arguments.size() != 2) runtimeError(expr->token, "saskaldīt expects text and a separator.");
            Value sourceValue = evaluate(expr->arguments[0], env);
            Value separatorValue = evaluate(expr->arguments[1], env);
            const std::string source = text(sourceValue, expr->token, "saskaldīt");
            const std::string separator = text(separatorValue, expr->token, "saskaldīt");
            if (separator.empty()) runtimeError(expr->token, "saskaldīt requires a non-empty separator.");

            auto result = std::make_shared<Array>();
            std::size_t begin = 0;
            while (true) {
                const std::size_t found = source.find(separator, begin);
                if (found == std::string::npos) { result->push_back(source.substr(begin)); break; }
                result->push_back(source.substr(begin, found - begin)); begin = found + separator.size();
            }
            return result;
        }
        if (expr->token.text == "savienot") {
            if (expr->arguments.size() != 2) runtimeError(expr->token, "savienot expects an array and a separator.");
            ArrayPtr values = array(evaluate(expr->arguments[0], env), expr->token);
            Value separatorValue = evaluate(expr->arguments[1], env);
            const std::string separator = text(separatorValue, expr->token, "savienot");
            std::string result;
            for (std::size_t i = 0; i < values->size(); ++i) {
                if (i) result += separator;
                result += show((*values)[i]);
            }
            return result;
        }
        if (expr->token.text == "satur") {
            if (expr->arguments.size() != 2) runtimeError(expr->token, "satur expects text and a fragment.");
            Value sourceValue = evaluate(expr->arguments[0], env);
            Value fragmentValue = evaluate(expr->arguments[1], env);
            const std::string source = text(sourceValue, expr->token, "satur");
            const std::string fragment = text(fragmentValue, expr->token, "satur");
            return source.find(fragment) != std::string::npos;
        }
        if (expr->token.text == "atrast") {
            if (expr->arguments.size() != 2) runtimeError(expr->token, "atrast expects text and a fragment.");
            Value sourceValue = evaluate(expr->arguments[0], env);
            Value fragmentValue = evaluate(expr->arguments[1], env);
            const std::string source = text(sourceValue, expr->token, "atrast");
            const std::string fragment = text(fragmentValue, expr->token, "atrast");
            const std::size_t found = source.find(fragment);
            return found == std::string::npos ? -1.0 : static_cast<double>(found);
        }
        if (expr->token.text == "aizstāt") {
            if (expr->arguments.size() != 3) runtimeError(expr->token, "aizstāt expects text, a fragment, and its replacement.");
            Value sourceValue = evaluate(expr->arguments[0], env);
            Value fragmentValue = evaluate(expr->arguments[1], env);
            Value replacementValue = evaluate(expr->arguments[2], env);
            const std::string source = text(sourceValue, expr->token, "aizstāt");
            const std::string fragment = text(fragmentValue, expr->token, "aizstāt");
            const std::string replacement = text(replacementValue, expr->token, "aizstāt");
            if (fragment.empty()) runtimeError(expr->token, "aizstāt requires a non-empty fragment.");

            std::string result;
            std::size_t begin = 0;
            while (true) {
                const std::size_t found = source.find(fragment, begin);
                if (found == std::string::npos) { result += source.substr(begin); break; }
                result += source.substr(begin, found - begin); result += replacement; begin = found + fragment.size();
            }
            return result;
        }
        if (expr->token.text == "garums") {
            if (expr->arguments.size() != 1) runtimeError(expr->token, "garums expects one argument.");
            Value value = evaluate(expr->arguments[0], env);
            if (const auto* a = std::get_if<ArrayPtr>(&value.data)) return static_cast<double>((*a)->size());
            if (const auto* s = std::get_if<std::string>(&value.data)) return static_cast<double>(s->size());
            runtimeError(expr->token, "garums expects an array or text.");
        }
        if (expr->token.text == "pievienot") {
            if (expr->arguments.size() != 2) runtimeError(expr->token, "pievienot expects an array and a value.");
            auto target = array(evaluate(expr->arguments[0], env), expr->token);
            target->push_back(evaluate(expr->arguments[1], env)); return static_cast<double>(target->size());
        }
        if (expr->token.text == "noņemt_pēdējo") {
            if (expr->arguments.size() != 1) runtimeError(expr->token, "noņemt_pēdējo expects one array.");
            auto target = array(evaluate(expr->arguments[0], env), expr->token);
            if (target->empty()) runtimeError(expr->token, "Cannot remove from an empty array.");
            Value value = target->back(); target->pop_back(); return value;
        }
        if (expr->token.text == "kods") {
            if (expr->arguments.size() != 1) runtimeError(expr->token, "kods expects one character.");
            Value value = evaluate(expr->arguments[0], env);
            const auto* text = std::get_if<std::string>(&value.data);
            if (!text || text->size() != 1) runtimeError(expr->token, "kods expects one single-byte character.");
            return static_cast<double>(static_cast<unsigned char>((*text)[0]));
        }
        if (expr->token.text == "rakstzīme") {
            if (expr->arguments.size() != 1) runtimeError(expr->token, "rakstzīme expects one character code.");
            double raw = number(evaluate(expr->arguments[0], env), expr->token);
            if (raw < 0 || raw > 255 || std::floor(raw) != raw) runtimeError(expr->token, "Character code must be an integer from 0 to 255.");
            return std::string(1, static_cast<char>(static_cast<unsigned char>(raw)));
        }
        auto found = functions_.find(expr->token.text);
        if (found == functions_.end()) runtimeError(expr->token, "Undefined function '" + expr->token.text + "'.");
        auto& function = found->second;
        if (expr->arguments.size() != function.parameters.size()) runtimeError(expr->token, "Wrong number of arguments.");
        auto local = std::make_shared<Environment>(); local->parent = function.closure;
        for (std::size_t i = 0; i < expr->arguments.size(); ++i) {
            Value value = evaluate(expr->arguments[i], env); const auto& parameter = function.parameters[i];
            local->values.emplace(parameter.second.text, Variable{parameter.first, convert(parameter.first, value, parameter.second)});
        }
        try { executeBlock(function.body, local); } catch (const ReturnSignal& signal) { return signal.value; }
        return 0.0;
    }
    Value evaluate(const ExprPtr& expr, const std::shared_ptr<Environment>& env) {
        switch (expr->kind) {
            case Expr::Kind::Literal: return expr->literal;
            case Expr::Kind::Variable: return env->get(expr->token).value;
            case Expr::Kind::Array: { auto result = std::make_shared<Array>(); for (const auto& e : expr->arguments) result->push_back(evaluate(e, env)); return result; }
            case Expr::Kind::Index: {
                Value targetValue = evaluate(expr->left, env);
                if (const auto* text = std::get_if<std::string>(&targetValue.data)) {
                    auto position = index(evaluate(expr->right, env), text->size(), expr->token);
                    return std::string(1, (*text)[position]);
                }
                auto target = array(targetValue, expr->token);
                return (*target)[index(evaluate(expr->right, env), target->size(), expr->token)];
            }
            case Expr::Kind::Call: return evaluateCall(expr, env);
            case Expr::Kind::Unary: {
                Value right = evaluate(expr->right, env);
                if (expr->token.type == TokenType::Nav) return !truthy(right);
                return -number(right, expr->token);
            }
            case Expr::Kind::Binary: break;
        }
        if (expr->token.type == TokenType::Un) { Value left = evaluate(expr->left, env); return truthy(left) && truthy(evaluate(expr->right, env)); }
        if (expr->token.type == TokenType::Vai) { Value left = evaluate(expr->left, env); return truthy(left) || truthy(evaluate(expr->right, env)); }
        Value left = evaluate(expr->left, env), right = evaluate(expr->right, env);
        switch (expr->token.type) {
            case TokenType::Plus:
                if (std::holds_alternative<std::string>(left.data) || std::holds_alternative<std::string>(right.data)) return show(left) + show(right);
                return number(left, expr->token) + number(right, expr->token);
            case TokenType::Minus: return number(left, expr->token) - number(right, expr->token);
            case TokenType::Star: return number(left, expr->token) * number(right, expr->token);
            case TokenType::Slash: { double divisor = number(right, expr->token); if (divisor == 0) runtimeError(expr->token, "Division by zero."); return number(left, expr->token) / divisor; }
            case TokenType::Atlikums: { double divisor = number(right, expr->token); if (divisor == 0) runtimeError(expr->token, "Division by zero."); return std::fmod(number(left, expr->token), divisor); }
            case TokenType::Mazaks: return number(left, expr->token) < number(right, expr->token);
            case TokenType::Greater: return number(left, expr->token) > number(right, expr->token);
            case TokenType::Ir: return valuesEqual(left, right);
            default: runtimeError(expr->token, "Unknown operator '" + expr->token.text + "'.");
        }
    }
    Value& assignmentTarget(const ExprPtr& target, const std::shared_ptr<Environment>& env) {
        if (target->kind == Expr::Kind::Variable) return env->get(target->token).value;
        auto targetArray = array(evaluate(target->left, env), target->token);
        return (*targetArray)[index(evaluate(target->right, env), targetArray->size(), target->token)];
    }
    void executeBlock(const std::vector<StmtPtr>& statements, const std::shared_ptr<Environment>& env) { for (const auto& stmt : statements) execute(stmt, env); }
    void execute(const StmtPtr& stmt, const std::shared_ptr<Environment>& env) {
        switch (stmt->kind) {
            case Stmt::Kind::Variable: { Value value = evaluate(stmt->expression, env); env->values[stmt->token.text] = {stmt->declaredType, convert(stmt->declaredType, value, stmt->token)}; break; }
            case Stmt::Kind::Function: functions_[stmt->token.text] = {stmt->parameters, stmt->body, env}; break;
            case Stmt::Kind::Print: output_ << show(evaluate(stmt->expression, env)) << '\n'; break;
            case Stmt::Kind::While: while (truthy(evaluate(stmt->expression, env))) { auto child = std::make_shared<Environment>(); child->parent = env; executeBlock(stmt->body, child); } break;
            case Stmt::Kind::If: { auto child = std::make_shared<Environment>(); child->parent = env; executeBlock(truthy(evaluate(stmt->expression, env)) ? stmt->body : stmt->elseBody, child); break; }
            case Stmt::Kind::Switch: {
                Value compared = evaluate(stmt->expression, env);
                const std::vector<StmtPtr>* selected = &stmt->elseBody;
                for (const auto& arm : stmt->switchArms) {
                    if (valuesEqual(compared, evaluate(arm.expression, env))) { selected = &arm.body; break; }
                }
                auto child = std::make_shared<Environment>(); child->parent = env; executeBlock(*selected, child); break;
            }
            case Stmt::Kind::Return: throw ReturnSignal{evaluate(stmt->expression, env)};
            case Stmt::Kind::Assign: {
                Value value = evaluate(stmt->expression, env);
                if (stmt->target->kind == Expr::Kind::Variable) { auto& variable = env->get(stmt->target->token); variable.value = convert(variable.type, value, stmt->token); }
                else assignmentTarget(stmt->target, env) = value;
                break;
            }
            case Stmt::Kind::AddAssign: {
                Value& target = assignmentTarget(stmt->target, env); Value right = evaluate(stmt->expression, env);
                if (std::holds_alternative<std::string>(target.data)) target = show(target) + show(right);
                else target = number(target, stmt->token) + number(right, stmt->token);
                break;
            }
            case Stmt::Kind::Expression: (void)evaluate(stmt->expression, env); break;
        }
    }
};
}

void interpret(const std::string& source, std::istream& input, std::ostream& output) {
    Parser parser(Lexer(source).tokenize()); Runtime runtime(input, output); runtime.run(parser.parse());
}
