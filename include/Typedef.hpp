#ifndef BIS_TYPEDEF_HPP
#define BIS_TYPEDEF_HPP
#include <memory>

struct Expr;
struct Stmt;

using unique_expr_ptr = std::unique_ptr<Expr>;
using unique_stmt_ptr = std::unique_ptr<Stmt>;

#endif // BIS_TYPEDEF_HPP