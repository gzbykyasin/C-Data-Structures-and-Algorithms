# C Data Structures and Algorithms

Welcome to this repository! This project features from-scratch implementations of fundamental data structures and algorithms in C. It's designed to be highly educational, giving you a hands-on look at how these structures work under the hood.

Here, you will find both the high-performance techniques commonly used in embedded systems and the Linux kernel, as well as the classic approaches taught in standard computer science curricula.

## Overview

Currently, this repository includes three core data structure implementations:

### 1. Dynamic Array (Vector)

Located in `dynamic_array/vector.c`, this is a standard Vector implementation.

- It dynamically resizes itself (using `malloc` and `memcpy`) when it runs out of capacity, ensuring continuous memory allocation.
- It is fully generic! Thanks to the use of `uint8_t*` and tracking element sizes, it can store any data type.

### 2. Intrusive Linked List

Located in `linked_list_intrusive/linked_list.c`, this showcases the famous Linux kernel-style "intrusive" linked list design.

- **How it works:** Instead of allocating a separate node structure to hold your data, the linked list pointers (`next` and `prev`) are embedded *directly* inside your custom data structure!
- **Benefits:** This entirely eliminates the need for extra `malloc` calls per node, drastically improving cache locality and overall performance.
- It uses the legendary `container_of` macro to calculate the parent structure's memory address from its embedded list pointer.

### 3. Non-Intrusive Linked List

Located in `linked_list_non_intrusive/linked_list.c`, this is the classic Doubly Linked List taught in most computer science classes.

- **How it works:** The list managing logic is completely unaware of the actual data. Whenever you add an item, the list internally calls `calloc` to create a new `Node_t` and `malloc` to hold a copy of your data.
- **Pros & Cons:** It is much more beginner-friendly and easier to interface with. However, because it requires separate heap allocations (`malloc` & `calloc`) for every single element, it's generally slower and can lead to memory fragmentation.

## Building and Running

This project uses `CMake` as its build system. To compile the code on your machine, simply run:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

To compile and **automatically run** all the examples in one step, use the `run` target:

```bash
cmake --build . --target run
```

*(Note: If you are at the project root directory, you can instead use `cmake --build build --target run`)*

After building, you will find the executable files (`vector`, `linked_list_intrusive`, and `linked_list_non_intrusive`) generated in your build directory (or inside a `Debug` / `Release` subfolder if using MSVC on Windows).

Feel free to explore the `main` functions inside each source file to see real examples of how these data structures are utilized. Happy coding!
