# Jlox-cpp
A modern C++(20) port of the AST-interpreter in Robert Nystrom's [Crafting Interpreters book](https://craftinginterpreters.com/).

## Lox lang
Lox is a imperative, C-like programming language with dynamic typing. The following code is an example of the language which counts the first 10 numbers of the Fibonacci sequence.

```javascript
fn fib(n) {
  if (n < 2) return n;
  return fib(n - 2) + fib(n - 1);
}

for (var i = 1; i <= 10; ++i) {
  print(fib(i));
}

```
Lox also treats functions as first class, which means closures.

```javascript
fn makeGreeter(greeting) 
{
    fn greeter(name) 
    {
        var say = greeting + " " + name;
        print(say);
    }

    return greeter;
}

var hello = makeGreeter("Hello"); 
var terve = makeGreeter("Terve"); 

hello("Foo"); // Prints: Hello Foo
terve("Bar"); // Prints: Terve Bar
```
For more detailed information about the syntax of the language, see [here](http://craftinginterpreters.com/the-lox-language.html).

### Differences from the original implementation
* Added support for `break` and `continue`
* Added support for postfix and prefix `++` and `--` 
* Added support for lists. 
  * Lists can be declared as `var list = [a, b, c];` 
  * List elements can be accessed by index, i.e. `list[0]`
  * Lists are passed by reference only
  * NOTE: Currently there is no support for any list properties, hence they are kinda useless at the moment.
* Made `print` more versatile.
  * Supports '\n', '\t' and empty statements, which will automatically print a newline. 
  * Arguments can be chained, e.g. `print(1, 2, 3, 4, 5); // 1 2 3 4 5`


### TODO and/or in progress
* Add support for REPL
* Pass by reference semantics. Currently implemented implicitly by passing string and list objects by reference. This is not a good solution however and will be reimplemented.
* Add support for lambda expressions
* Add support for accessing the list properties
* *Maybe* implement support for classes
* Some STL features would be nice

# How to build
Dependecies:
* C++20 standard compatible compiler (gcc tested) 
* CMake >= 3.16

You can simply build with CMake:
```cmake
# Creates a build dir for the cmake files
cmake -B build -S .

cmake --build build
```
## Usage
To run the program in REPL: (*in progress*)
```cmake
build/src/main
```

To execute a source file, you can pass it as a CLI argument:
```cmake
build/src/main <filename>
```


## Future work
The AST-interpreter is painfully slow. The next obvious solution would be to write a VM and compile to bytecode instead. However, instead of bytecode, one optimization technique called *Tree rewriting* could be used. This is something that I looked into whilst figuring out ways to optimize the performance of the AST-interpreter. For reference one can look [here](http://lafo.ssw.uni-linz.ac.at/papers/2012_DLS_SelfOptimizingASTInterpreters.pdf).
