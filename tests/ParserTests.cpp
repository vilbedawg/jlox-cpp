#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include <gtest/gtest.h>

TEST(ParserTests, Expression)
{
    const auto test_script = R"(
        2 + 2 == 4;
    )";
    Lexer lexer{test_script};
    Parser parser{lexer.scanTokens()};
    const auto statements = parser.parse();

    ASSERT_EQ(1, statements.size());
    auto* expr_stmt = dynamic_cast<ExprStmt*>(statements[0].get());

    ASSERT_NE(nullptr, expr_stmt);
    auto& expr = *expr_stmt->expression.get();
    EXPECT_NE(nullptr, dynamic_cast<const BinaryExpr*>(&expr));
}
// TODO: Create tests for expression types, statements