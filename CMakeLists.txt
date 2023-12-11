add_executable (Goal
  Main.cpp
  CodeGen.cpp
  Lexer.cpp
  Parser.cpp
  Sema.cpp
  )
target_link_libraries(Goal PRIVATE ${llvm_libs})
