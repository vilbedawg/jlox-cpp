add_library(BIS)

target_include_directories(BIS
    PUBLIC
        ${SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(BIS
    PUBLIC
        AstPrinter.cpp
        Expr.cpp
        Lexer.cpp
        Logger.cpp
        Parser.cpp
        Stmt.cpp
        Token.cpp
)

add_executable(main main.cpp)

target_include_directories(main
    PUBLIC
        ${SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(main
    PRIVATE
       main.cpp
)

target_link_libraries(main PRIVATE BIS)