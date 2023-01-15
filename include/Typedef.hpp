#ifndef TYPEDEF_HPP
#define TYPEDEF_HPP

#include <any>
#include <memory>

struct Expr;
struct Stmt;

using unique_expr_ptr = std::unique_ptr<Expr>;
using unique_stmt_ptr = std::unique_ptr<Stmt>;
using shared_ptr_any = std::shared_ptr<std::any>;

#endif // TYPEDEF_HPP