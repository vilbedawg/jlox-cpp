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

TEST(ParserTests, ForStatement)
{
    const auto test_script = R"(
        for(var i = 0; i < 10; i++) {
            false;
        }
    )";

    const auto statements = initParser(test_script);
    ASSERT_EQ(statements.size(), 1);

    auto stmt = dynamic_cast<ForStmt*>(statements.at(0).get());
    ASSERT_TRUE(stmt);
    auto initializer = dynamic_cast<VarStmt*>(stmt->initializer.get());
    auto condition = dynamic_cast<BinaryExpr*>(stmt->condition.get());
    auto increment = dynamic_cast<IncrementExpr*>(stmt->increment.get());
    auto body = dynamic_cast<BlockStmt*>(stmt->body.get());

    // Initializer
    // var i = 0;
    ASSERT_TRUE(initializer);
    EXPECT_EQ(initializer->identifier.type, TokenType::IDENTIFIER);
    auto initializer_init = dynamic_cast<LiteralExpr*>(initializer->initializer.get());
    ASSERT_TRUE(initializer->initializer && initializer_init &&
                initializer_init->literal.type() == typeid(double));
    EXPECT_EQ(std::any_cast<double>(initializer_init->literal), 0);

    // condition
    // i < 10;
    ASSERT_TRUE(condition);
    EXPECT_EQ(condition->op.type, TokenType::LESS);
    auto left = dynamic_cast<VarExpr*>(condition->left.get());
    auto right = dynamic_cast<LiteralExpr*>(condition->right.get());
    ASSERT_TRUE(left && right);
    EXPECT_EQ(left->identifier.type, TokenType::IDENTIFIER);
    ASSERT_EQ(right->literal.type(), typeid(double));
    EXPECT_EQ(std::any_cast<double>(right->literal), 10);

    // increment
    // i++
    ASSERT_TRUE(increment);
    EXPECT_EQ(increment->identifier.type, TokenType::IDENTIFIER);
    EXPECT_EQ(increment->identifier.lexeme, "i");
    EXPECT_EQ(increment->type, IncrementExpr::Type::POSTFIX);

    // body
    // { false; }
    ASSERT_TRUE(body);
    ASSERT_EQ(body->statements.size(), 1);
    auto block_stmt = dynamic_cast<ExprStmt*>(body->statements.at(0).get());
    ASSERT_TRUE(block_stmt);
    auto literal = dynamic_cast<LiteralExpr*>(block_stmt->expression.get());
    ASSERT_TRUE(literal);
    ASSERT_EQ(literal->literal.type(), typeid(bool));
    EXPECT_FALSE(std::any_cast<bool>(literal->literal));
}

TEST(ParserTests, WhileStatement)
{
    const auto test_script = R"(
        while (x < 10) { x++; }
    )";

    const auto statements = initParser(test_script);
    ASSERT_EQ(statements.size(), 1);

    auto stmt = dynamic_cast<WhileStmt*>(statements.at(0).get());
    auto condition = dynamic_cast<BinaryExpr*>(stmt->condition.get());
    auto body = dynamic_cast<BlockStmt*>(stmt->body.get());

    // Condtition
    ASSERT_TRUE(condition);
    EXPECT_EQ(condition->op.type, TokenType::LESS);
    auto left = dynamic_cast<VarExpr*>(condition->left.get());
    auto right = dynamic_cast<LiteralExpr*>(condition->right.get());
    ASSERT_TRUE(left && right);
    EXPECT_EQ(left->identifier.type, TokenType::IDENTIFIER);
    EXPECT_EQ(left->identifier.lexeme, "x");
    ASSERT_EQ(right->literal.type(), typeid(double));
    EXPECT_EQ(std::any_cast<double>(right->literal), 10);

    // Body
    ASSERT_TRUE(body);
    ASSERT_EQ(body->statements.size(), 1);
    auto block_stmt = dynamic_cast<ExprStmt*>(body->statements.at(0).get());
    ASSERT_TRUE(block_stmt);

    auto increment = dynamic_cast<IncrementExpr*>(block_stmt->expression.get());
    EXPECT_EQ(increment->identifier.type, TokenType::IDENTIFIER);
    EXPECT_EQ(increment->identifier.lexeme, "x");
    EXPECT_EQ(increment->type, IncrementExpr::Type::POSTFIX);
}

bool checkListValue(const std::any& item)
{
    return false;
}

TEST(ParserTests, List)
{
    const auto test_script = R"(
        var a = [1, "string", false, clock()];
    )";

    const auto statements = initParser(test_script);
    ASSERT_EQ(statements.size(), 1);

    auto var = dynamic_cast<VarStmt*>(statements.at(0).get());
    ASSERT_TRUE(var);
    EXPECT_EQ(var->identifier.type, TokenType::IDENTIFIER);
    EXPECT_EQ(var->identifier.lexeme, "a");

    auto list = dynamic_cast<ListExpr*>(var->initializer.get());
    ASSERT_TRUE(list);
    EXPECT_EQ(list->items.size(), 4);
    for (const auto& item : list->items)
    {
        if (auto expr = dynamic_cast<LiteralExpr*>(item.get()))
        {
            if (expr->literal.type() == typeid(bool))
            {
                EXPECT_FALSE(std::any_cast<bool>(expr->literal));
            }

            else if (expr->literal.type() == typeid(double))
            {
                EXPECT_DOUBLE_EQ(std::any_cast<double>(expr->literal), 1);
            }

            else if (expr->literal.type() == typeid(std::string))
            {
                EXPECT_EQ(std::any_cast<std::string>(expr->literal), "string");
            }
        }
        else if (auto call = dynamic_cast<CallExpr*>(item.get()))
        {
            ASSERT_TRUE(call);
            auto caller = dynamic_cast<VarExpr*>(call->callee.get());
            ASSERT_TRUE(caller);
            EXPECT_EQ(caller->identifier.type, TokenType::IDENTIFIER);
            EXPECT_EQ(caller->identifier.lexeme, "clock");
        }
        else
        {
            EXPECT_ANY_THROW("list items dont match with expected values.");
        }
    }
}