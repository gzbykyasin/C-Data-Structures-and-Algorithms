cmake_minimum_required(VERSION 3.10)
project(DataStructuresAndAlgorithms C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Enable warnings to catch potential issues (e.g., format warnings)
if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. Dynamic Array
add_executable(vector dynamic_array/vector.c)

# 2. Intrusive Linked List
add_executable(linked_list_intrusive linked_list_intrusive/linked_list.c)

# 3. Non-Intrusive Linked List
add_executable(linked_list_non_intrusive linked_list_non_intrusive/linked_list.c)

# Custom target to explicitly build and run all executables
add_custom_target(run
    COMMAND $<TARGET_FILE:vector>
    COMMAND $<TARGET_FILE:linked_list_intrusive>
    COMMAND $<TARGET_FILE:linked_list_non_intrusive>
    DEPENDS vector linked_list_intrusive linked_list_non_intrusive
    COMMENT "Running all data structure algorithms..."
)
