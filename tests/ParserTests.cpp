#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"

#include <gtest/gtest.h>

std::vector<unique_stmt_ptr> initParser(const std::string& test_script)
{
    Lexer lexer{test_script};
    Parser parser{lexer.scanTokens()};
    return parser.parse();
}

TEST(ParserTests, Expression)
{
    const auto test_script = R"(
        2 + 2 == 4;
    )";

    const auto statements = initParser(test_script);
    ASSERT_EQ(1, statements.size());
    auto expr_stmt = dynamic_cast<ExprStmt*>(statements[0].get());

    ASSERT_TRUE(expr_stmt);
    auto const& expr = *expr_stmt->expression.get();
    ASSERT_TRUE(dynamic_cast<const BinaryExpr*>(&expr));
}

TEST(ParserTests, Associativity)
{
    /* clang-format off */

    // -2 * 2 + 2 * 2 > 2 == true;
    // should be parsed like this((((-2) * 2) + 2) > 2) == true

    //                     ==
    //                    /  \
    //                   >    true
    //                  /  \
    //                 +    2
    //               /   \
    //              *     *
    //             / \   / \
    //            -   2 2   2
    //           /
    //          2

    /* clang-format on */

    const auto test_script = R"(
    -2 * 2 + 2 * 2 > 2 == true;
    )";

    const auto statements = initParser(test_script);

    auto expr_stmt = dynamic_cast<ExprStmt*>(statements.at(0).get());
    ASSERT_TRUE(expr_stmt);

    // Evaluating nodes in pre-order traversal

    // The root of the tree
    auto binary_equal = dynamic_cast<BinaryExpr*>(expr_stmt->expression.get());
    ASSERT_TRUE(binary_equal);
    EXPECT_EQ(binary_equal->op.type, TokenType::EQUAL_EQUAL);

    // Left child of root '=' node => '>'
    auto binary_greater = dynamic_cast<BinaryExpr*>(binary_equal->left.get());
    ASSERT_TRUE(binary_greater);
    EXPECT_EQ(binary_greater->op.type, binary_greater->op.type);

    // Left child of '>' node => '+'
    auto binary_add = dynamic_cast<BinaryExpr*>(binary_greater->left.get());
    ASSERT_TRUE(binary_add);
    EXPECT_EQ(binary_add->op.type, TokenType::PLUS);

    // Left child of '+' node => '*'
    auto binary_multiply_left = dynamic_cast<BinaryExpr*>(binary_add->left.get());
    ASSERT_TRUE(binary_multiply_left);
    EXPECT_EQ(binary_multiply_left->op.type, TokenType::STAR);

    // Left child of '*' node => '- unary'
    auto unary = dynamic_cast<UnaryExpr*>(binary_multiply_left->left.get());
    ASSERT_TRUE(unary);
    EXPECT_EQ(unary->op.type, TokenType::MINUS);

    // Child of '- unary' node => 2, leaf node
    auto leaf = dynamic_cast<LiteralExpr*>(unary->right.get());
    ASSERT_TRUE(leaf);
    EXPECT_EQ(std::any_cast<double>(leaf->literal), 2);

    // Right child of '*' node => 2, leaf node
    auto right_child = dynamic_cast<LiteralExpr*>(binary_multiply_left->right.get());
    ASSERT_TRUE(right_child);
    EXPECT_EQ(std::any_cast<double>(right_child->literal), 2);

    // Right child of '+' node => '*'
    auto binary_multiply_right = dynamic_cast<BinaryExpr*>(binary_add->right.get());
    ASSERT_TRUE(binary_multiply_right);
    EXPECT_EQ(binary_multiply_right->op.type, TokenType::STAR);

    // Leaf nodes of '*' nodes => 2 and 2, leaf nodes
    auto left_child = dynamic_cast<LiteralExpr*>(binary_multiply_right->right.get());
    right_child = dynamic_cast<LiteralExpr*>(binary_multiply_right->left.get());
    ASSERT_TRUE(left_child);
    ASSERT_TRUE(right_child);
    EXPECT_EQ(std::any_cast<double>(left_child->literal), 2);
    EXPECT_EQ(std::any_cast<double>(right_child->literal), 2);

    // Right child of '>' node => 2, leaf node
    auto binary_greater_right = dynamic_cast<LiteralExpr*>(binary_greater->right.get());
    ASSERT_TRUE(binary_greater_right);
    EXPECT_EQ(std::any_cast<double>(binary_greater_right->literal), 2);

    // Right child of root '=' node => true
    auto literal_true = dynamic_cast<LiteralExpr*>(binary_equal->right.get());
    ASSERT_TRUE(literal_true);
    EXPECT_TRUE(std::any_cast<bool>(literal_true->literal));
}

TEST(ParserTests, FunctionDeclaration)
{
    const auto test_script = R"(
        fn foo(first, second) { return first + second; }
    )";

    const auto statements = initParser(test_script);
    ASSERT_EQ(statements.size(), 1);

    auto fn_stmt = dynamic_cast<FnStmt*>(statements.at(0).get());
    ASSERT_TRUE(fn_stmt);

    // Parameters
    EXPECT_EQ(fn_stmt->params.size(), 2);
    EXPECT_EQ(fn_stmt->params.at(0).type, TokenType::IDENTIFIER);
    EXPECT_EQ(fn_stmt->params.at(0).lexeme, "first");
    EXPECT_EQ(fn_stmt->params.at(1).type, TokenType::IDENTIFIER);
    EXPECT_EQ(fn_stmt->params.at(1).lexeme, "second");

    // body
    ASSERT_EQ(fn_stmt->body.size(), 1);
    const auto return_stmt = dynamic_cast<ReturnStmt*>(fn_stmt->body.at(0).get());
    ASSERT_TRUE(return_stmt);
    EXPECT_EQ(return_stmt->keyword.type, TokenType::RETURN);

    const auto return_value = dynamic_cast<BinaryExpr*>(return_stmt->expression.get());
    ASSERT_TRUE(return_value);
    EXPECT_EQ(return_value->op.type, TokenType::PLUS);

    const auto left = dynamic_cast<VarExpr*>(return_value->left.get());
    ASSERT_TRUE(left);
    EXPECT_EQ(left->identifier.lexeme, "first");
    EXPECT_EQ(left->identifier.type, TokenType::IDENTIFIER);

    const auto right = dynamic_cast<VarExpr*>(return_value->right.get());
    ASSERT_TRUE(right);
    EXPECT_EQ(right->identifier.lexeme, "second");
    EXPECT_EQ(right->identifier.type, TokenType::IDENTIFIER);
}

TEST(ParserTests, VariableDeclaration)
{
    const auto test_script = R"(
        var x = 10;
        var y = true == false;
        var z = y;
    )";

    const auto statements = initParser(test_script);
    ASSERT_EQ(statements.size(), 3);

    const auto x = dynamic_cast<VarStmt*>(statements.at(0).get());
    const auto y = dynamic_cast<VarStmt*>(statements.at(1).get());
    const auto z = dynamic_cast<VarStmt*>(statements.at(2).get());
    ASSERT_TRUE(x);
    ASSERT_TRUE(y);
    ASSERT_TRUE(z);

    EXPECT_EQ(x->identifier.lexeme, "x");
    EXPECT_EQ(y->identifier.lexeme, "y");
    EXPECT_EQ(z->identifier.lexeme, "z");

    EXPECT_EQ(x->identifier.type, TokenType::IDENTIFIER);
    EXPECT_EQ(y->identifier.type, TokenType::IDENTIFIER);
    EXPECT_EQ(z->identifier.type, TokenType::IDENTIFIER);

    const auto x_assign_val = dynamic_cast<LiteralExpr*>(x->initializer.get());
    const auto y_assign_val = dynamic_cast<BinaryExpr*>(y->initializer.get());
    const auto z_assign_val = dynamic_cast<VarExpr*>(z->initializer.get());

    ASSERT_TRUE(x_assign_val);
    ASSERT_TRUE(y_assign_val);
    ASSERT_TRUE(z_assign_val);

    // var x = 10;
    EXPECT_EQ(std::any_cast<double>(x_assign_val->literal), 10);

    // var y = true == false;
    EXPECT_EQ(y_assign_val->op.type, TokenType::EQUAL_EQUAL);
    const auto left = dynamic_cast<LiteralExpr*>(y_assign_val->left.get());
    const auto right = dynamic_cast<LiteralExpr*>(y_assign_val->right.get());
    ASSERT_TRUE(left);
    ASSERT_TRUE(right);
    EXPECT_TRUE(std::any_cast<bool>(left->literal));
    EXPECT_FALSE(std::any_cast<bool>(right->literal));

    // var z = y;
    const auto var_expr = dynamic_cast<VarExpr*>(z->initializer.get());
    ASSERT_TRUE(var_expr);
    EXPECT_EQ(var_expr->identifier.lexeme, y->identifier.lexeme);
}

TEST(ParserTests, IfStatement)
{
    const auto test_script = R"(
        if (false) { 
            false; 
        } elif (true and true) { 
            true; 
        } else { 
            false; 
        }
    )";

    const auto statements = initParser(test_script);
    ASSERT_EQ(statements.size(), 1);

    const auto if_stmt = dynamic_cast<IfStmt*>(statements.at(0).get());
    const auto main_branch = std::move(if_stmt->main_branch);
    // .. false
    const auto main_condition = dynamic_cast<LiteralExpr*>(main_branch.condition.get());
    // .. false
    const auto main_statement = dynamic_cast<BlockStmt*>(main_branch.statement.get());
    ASSERT_TRUE(main_condition);
    ASSERT_TRUE(main_statement);

    EXPECT_FALSE(std::any_cast<bool>(main_condition->literal));
    EXPECT_EQ(main_statement->statements.size(), 1);

    const auto main_block_stmt = dynamic_cast<ExprStmt*>(main_statement->statements.at(0).get());
    ASSERT_TRUE(main_block_stmt);
    EXPECT_FALSE(std::any_cast<bool>(
        dynamic_cast<LiteralExpr*>(main_block_stmt->expression.get())->literal));

    // Elif
    const auto elif_branch = std::move(if_stmt->elif_branches.at(0));
    
    // elif condition => true and true
    const auto elif_condition = dynamic_cast<LogicalExpr*>(elif_branch.condition.get());
    ASSERT_TRUE(elif_condition);

    // .. and
    EXPECT_EQ(elif_condition->op.type, TokenType::AND);
    EXPECT_EQ(elif_condition->op.lexeme, "and");

    // .. true
    const auto left = dynamic_cast<LiteralExpr*>(elif_condition->left.get());
    ASSERT_TRUE(left);
    ASSERT_TRUE(std::any_cast<bool>(left->literal));

    // .. false
    const auto right = dynamic_cast<LiteralExpr*>(elif_condition->right.get());
    ASSERT_TRUE(right);
    ASSERT_TRUE(std::any_cast<bool>(right->literal));

    // elif block => true;
    const auto elif_block = dynamic_cast<BlockStmt*>(elif_branch.statement.get());
    ASSERT_TRUE(elif_block);

    EXPECT_EQ(elif_block->statements.size(), 1);

    const auto elif_block_stmt = dynamic_cast<ExprStmt*>(elif_block->statements.at(0).get());
    ASSERT_TRUE(elif_block_stmt);
    EXPECT_TRUE(std::any_cast<bool>(
        dynamic_cast<LiteralExpr*>(elif_block_stmt->expression.get())->literal));

    // else branch
    const auto else_branch = std::move(if_stmt->else_branch);
    ASSERT_TRUE(else_branch);

    // else block => false
    const auto else_block = dynamic_cast<BlockStmt*>(else_branch.get());
    ASSERT_TRUE(else_block);

    EXPECT_EQ(else_block->statements.size(), 1);

    // .. false
    const auto else_block_stmt = dynamic_cast<ExprStmt*>(else_block->statements.at(0).get());
    ASSERT_TRUE(else_block_stmt);
    EXPECT_FALSE(std::any_cast<bool>(
        dynamic_cast<LiteralExpr*>(else_block_stmt->expression.get())->literal));
}

// Todo: Tests for while and for loops, blocks and for lists when those are done.
// TEST(ParserTest, TestFor Statement) {
// const std::string input = "for (var i = 0; i < 10; i++) { x += i; }";
// Parser p(input);
// EXPECT_TRUE(p.ParseForStatement());
//}

// TEST(ParserTest, TestWhile Statement) {
// const std::string input = "while (x < 10) { x++; }";
// Parser p(input);
// EXPECT_TRUE(p.ParseWhile Statement());
//}

// TEST(ParserTest, TestBlock) {
// const std::string input = "{ var x = 10; y = 20; }";
// Parser p(input);
// EXPECT_TRUE(p.ParseBlock());
//}
