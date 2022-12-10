include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_test)

target_sources(unit_test
    PRIVATE
        LexerTests.cpp
        ParserTests.cpp
        main.cpp
)

target_include_directories(main
    PUBLIC
        ${SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(unit_test
  PUBLIC
    BIS
    gtest_main
)

add_test(
  unit_gtest
  unit_test
)

