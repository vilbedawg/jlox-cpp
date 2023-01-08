add_library(BIS)

target_include_directories(BIS
        PUBLIC
        ${SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_sources(BIS
        PUBLIC
        ExprNode.cpp
        Lexer.cpp
        Logger.cpp
        Parser.cpp
        StmtNode.cpp
        Token.cpp
        Interpreter.cpp
        Environment.cpp
        BisFunction.cpp
        BuiltIn.cpp
        )

add_executable(main main.cpp)

target_include_directories(main
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_sources(main
        PRIVATE
        main.cpp
        )

target_link_libraries(main PRIVATE BIS)