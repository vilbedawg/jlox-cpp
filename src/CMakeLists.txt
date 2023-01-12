add_library(jlox-cpp)

target_include_directories(jlox-cpp
        PUBLIC
        ${SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_sources(jlox-cpp
        PUBLIC
        ExprNode.cpp
        Lexer.cpp
        Logger.cpp
        Parser.cpp
        StmtNode.cpp
        Token.cpp
        Interpreter.cpp
        Environment.cpp
        FunctionType.cpp
        BuiltIn.cpp
        ListType.cpp
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

target_link_libraries(main PRIVATE jlox-cpp)