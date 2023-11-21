/**
 * MIT License
 *
 * Copyright (c) 2023 Ochawin A.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef TESTER_H
#define TESTER_H

#include "../core/parser_base.hpp"
#include "token_test.hpp"

namespace test
{

/**
 * An interpreter for the test code.
 */
class Tester : public BaseParser<TestTokenTypeScanner, TestTokenType>
{
  public:
    /**
     * Constructor with file path of test source code.
     */
    [[nodiscard]] explicit Tester(const std::string& file_path)
        : BaseParser<TestTokenTypeScanner, TestTokenType>(file_path)
    {
    }

    /**
     * Default Ctor prohibted.
     */
    constexpr Tester() = delete;

    /**
     * Parse the source code and build the RecipeBuilder.
     */
    [[nodiscard]] auto parse() noexcept -> bool
    {
        advance();

        while (!match(TestTokenType::EndOfFile))
        {
            declaration();
        }

        return !this->has_error;
    }

    /**
     * Parsing functions.
     */
  private:
    auto declaration() noexcept -> void
    {
        if (match(TestTokenType::Load))
        {
            LOAD_statement();
        }
        else if (match(TestTokenType::Test))
        {
            TEST_statement();
        }

        if (this->panic)
        {
            restabilize();
        }
    }

    auto LOAD_statement() noexcept -> void
    {
        log("Parsing LOAD statement.");

        consume(TestTokenType::Identifier, "Expected chip name.");
        const auto& chip_name = previous.lexeme;

        log("Loading chip: " + chip_name);

        expect_semicolon("Expected ';' at the end of LOAD statement.");

        log("Finished parsing LOAD statement.");
    }

    auto parse_quote() noexcept -> std::string
    {
        std::stringstream val{};

        while (match(TestTokenType::Identifier))
        {
            val << previous.lexeme << " ";
        }

        consume(TestTokenType::Quote, "String not terminated.");

        return val.str();
    }

    auto expect_semicolon(const std::string& message) noexcept -> void
    {
        consume(TestTokenType::Semicolon, message);
    }

    auto VAR_statement() noexcept -> void
    {
        log("Parsing VAR statement.");
        consume(TestTokenType::Identifier, "Expected variable name.");
        const auto& varname = previous.lexeme;

        consume(TestTokenType::colon, "Expected ':' after variable name.");

        consume(TestTokenType::Identifier, "Expected variable type.");
        const auto& vartype = previous.lexeme;

        log("Varname: " + varname);
        log("Type: " + vartype);

        expect_semicolon("Expected ';' at the end of VAR statement.");
        log("Finished parsing VAR statement.");
    }

    auto EVAL_statement() noexcept -> void
    {
        // Simulate...

        expect_semicolon("Expected semicolon for EVAL statement.");
    }

    // TODO: Make this return variable information.
    auto parse_variable() noexcept -> std::string
    {
        log("Parsing VARIABLE.");
        if (match(TestTokenType::Number))
        {
            const auto& number = previous.lexeme;
            log("Returning numeric constant: " + number);
            return number;
        }
        else if (match(TestTokenType::Identifier)) 
        {
            const auto& varname = previous.lexeme;

            consume(TestTokenType::Dot, "Expected '.' after variable name to access member.");

            consume(TestTokenType::Identifier, "Expected variable member name.");
            const auto& member = previous.lexeme;

            log("Varname: " + varname);
            log("Member: " + member);
            log("Finished parsing VARIABLE.");
        
            // TODO.
            return member;
        }
        else
        {
          report_error("Expected variable.");
          return "";
        }
    }

    auto SET_statement() noexcept -> void
    {
        log("Parsing SET statement.");

        // Parse variable.
        auto var = parse_variable();

        consume(TestTokenType::Equal, "Expected assignment in SET statement.");

        auto val = parse_variable();

        expect_semicolon("Expected ';' at the end of SET statement.");

        log("Setting " + var + " to " + val + ".");
        log("Finished parsing SET statement.");
    }

    auto parse_condition() noexcept -> void
    {
        auto grouped = (match(TestTokenType::LParen));

        const auto& var = parse_variable();
        log("Var: " + var);

        if (match(TestTokenType::Is))
        {
            log("Comparator: ==");
        }
        else if (match(TestTokenType::Not))
        {
            log("Comparator: !=");
        }
        else
        {
            report_error("Expected comparison operator in condition.");
        }

        const auto& val = parse_variable();
        log("Val: " + val);

        if (grouped)
        {
            consume(TestTokenType::RParen, "Expected ')' at the end of grouped condition.");
        }
    }

    auto REQUIRE_statement() noexcept -> void
    {
        log("Parsing REQUIRE statement.");

        parse_condition();

        while (match(TestTokenType::And))
        {
            parse_condition();
        }

        expect_semicolon("Expected ';' at the end of REQUIRE statement.");

        log("Finished parsing REQUIRE statement.");
    }

    auto parse_TEST_body() noexcept -> void
    {
        log("Parsing TEST body");
        consume(TestTokenType::LBrace, "Expected '{'.");

        while (!match(TestTokenType::RBrace))
        {
            if (match(TestTokenType::Var))
            {
                VAR_statement();
            }
            else if (match(TestTokenType::Set))
            {
                SET_statement();
            }
            else if (match(TestTokenType::Eval))
            {
                EVAL_statement();
            }
            else if (match(TestTokenType::Require))
            {
                REQUIRE_statement();
            }
            else if (match(TestTokenType::EndOfFile))
            {
                report_error("CHIP definition not terminated, expected '}', found '" +
                             current.lexeme + "'.");
                return;
            }
            // TODO: This should not be here.
            else
            {
                restabilize();   
            }
        }
        log("Finished parsing TEST body");
    }

    auto TEST_statement() noexcept -> void
    {
        log("Parsing TEST statement.");

        // Retrieve the test name.
        consume(TestTokenType::Quote, "Expected test name");
        const auto test_name = parse_quote();
        log("Test name: " + test_name);

        // Parsing body.
        parse_TEST_body();

        log("Finished parsing TEST statement.");
    }

    auto restabilize() noexcept -> void
    {
        // Reset panic flag, since now we can report a new error for a different part.
        this->panic = false;

        // log("Restabilizing...");
        std::cout << "Restabilizing...\n";

        while (this->current.type != TestTokenType::EndOfFile)
        {
            auto token = "Token: " + current.lexeme + ", " + std::string(current.type.name());
            log(token);
            if (this->previous.type == TestTokenType::Semicolon)
                return;

            switch (this->current.type)
            {
                case TestTokenType::Test:
                    return;
                default:; // Do nothing...
            }
            this->advance();
        }
    }
};

} /* namespace test */

#endif /* TESTER_H */
