/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************
 * Structs & Types
 ******************************************************************************/
/**
 * @brief Represents a generic stack utilizing a dynamic array.
 *
 * Stores elements of a generic type contiguously in memory.
 * Grows its capacity automatically when full.
 */
typedef struct {
  uint8_t *data;
  size_t capacity;
  size_t elem_size;
  size_t length;
} Stack_t;

/******************************************************************************
 * Static / Helper Functions
 ******************************************************************************/
/**
 * @brief Grows the internal capacity of the stack.
 *
 * Doubles the current capacity, or initializes it to 4 if the stack is empty.
 * Uses malloc() and copies the existing data into the new block.
 *
 * @param p_stack Pointer to the stack instance.
 * @return true if memory allocation was successful, false otherwise.
 */
static bool stack_grow(Stack_t *p_stack) {
  if (!p_stack) {
    return false;
  }

  size_t new_capacity;

  if (p_stack->capacity == 0) {
    new_capacity = 4;
  } else {
    /* Check for integer overflow in capacity duplication */
    if (p_stack->capacity > (SIZE_MAX / 2)) {
      return false;
    }
    new_capacity = p_stack->capacity * 2;
  }

  /* Check for integer overflow when multiplying by element size */
  if (new_capacity > (SIZE_MAX / p_stack->elem_size)) {
    return false;
  }

  size_t total_bytes = new_capacity * p_stack->elem_size;
  void *temp_ptr = malloc(total_bytes);

  if (!temp_ptr) {
    return false;
  }

  /* Copy existing data if there are any elements */
  if (p_stack->data && p_stack->length > 0) {
    (void)memcpy(temp_ptr, p_stack->data, p_stack->length * p_stack->elem_size);
    free(p_stack->data);
  }

  p_stack->data = (uint8_t *)temp_ptr;
  p_stack->capacity = new_capacity;

  return true;
}

/******************************************************************************
 * Public / Interface Functions
 ******************************************************************************/
/**
 * @brief Initializes a stack and sets its baseline parameters.
 *
 * @param elem_size The size of each element in bytes (e.g., sizeof(int)).
 * @return The stack pointer if successful, NULL otherwise.
 */
Stack_t *stack_init(size_t elem_size) {
  if (elem_size == 0) {
    return NULL;
  }

  Stack_t *p_stack = (Stack_t *)calloc(1, sizeof(Stack_t));
  if (p_stack) {
    p_stack->elem_size = elem_size;
  }

  return p_stack;
}

/**
 * @brief Destroys the stack and frees its reserved heap memory.
 *
 * @param p_stack Pointer to the stack to destroy.
 */
void stack_destroy(Stack_t *p_stack) {
  if (!p_stack) {
    return;
  }

  if (p_stack->data) {
    free(p_stack->data);
    p_stack->data = NULL;
  }

  p_stack->elem_size = 0;
  p_stack->length = 0;
  p_stack->capacity = 0;

  free(p_stack);
}

/**
 * @brief Pushes an element onto the top of the stack.
 *
 * This operation runs in amortized O(1) time complexity. If the internal
 * array is full, it will trigger a capacity expansion.
 *
 * @param p_stack Pointer to the stack.
 * @param p_data Pointer to the data to be copied into the stack.
 * @return true on success, false on memory allocation failure or invalid
 * inputs.
 */
bool stack_push(Stack_t *p_stack, const void *const p_data) {
  if (!p_stack || !p_data) {
    return false;
  }

  if (p_stack->length == p_stack->capacity) {
    if (!stack_grow(p_stack)) {
      printf("[ERROR] Failed to expand the stack's capacity.\n");
      return false;
    }
  }

  size_t offset = p_stack->elem_size * p_stack->length;
  uint8_t *target = p_stack->data + offset;

  (void)memcpy(target, p_data, p_stack->elem_size);

  p_stack->length++;

  return true;
}

/**
 * @brief Safely retrieves the top element from the stack without removing it.
 *
 * @param p_stack Pointer to the stack.
 * @return A constant pointer to the element, or NULL if the stack is empty.
 */
const void *const stack_peek(Stack_t *p_stack) {
  if (!p_stack || (p_stack->length == 0)) {
    return NULL;
  }

  size_t index = p_stack->length - 1;
  return p_stack->data + (index * p_stack->elem_size);
}

/**
 * @brief Removes the top element from the stack.
 *
 * Note that this does not shrink the allocated memory, but merely
 * decrements the length counter, making it a strict O(1) operation.
 *
 * @param p_stack Pointer to the stack.
 * @return true if an element was successfully popped, false if the stack is
 * empty.
 */
bool stack_pop(Stack_t *p_stack) {
  if (!p_stack || (p_stack->length == 0)) {
    return false;
  }

  p_stack->length--;

  return true;
}

/******************************************************************************
 * Main Function Test
 ******************************************************************************/
/**
 * @brief Example structure representing a web browser history entry.
 */
typedef struct {
  int page_id;
  float load_time;
} WebPage_t;

/**
 * @brief Entry point of the program illustrating the usage of the dynamic array
 * stack.
 */
int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  printf("\n======================================================\n");
  printf("[INFO] STARTING DEMO: Stack (Dynamic Array)\n");
  printf("======================================================\n");

  Stack_t *history = stack_init(sizeof(WebPage_t));

  if (!history) {
    printf("[ERROR] Failed to initialize the stack.\n");
    return EXIT_FAILURE;
  }

  // Push
  for (int i = 1; i <= 5; ++i) {
    WebPage_t page = {.page_id = i * 100, .load_time = 0.5f * i};

    if (!stack_push(history, &page)) {
      printf("[ERROR] Failed to push page ID %d onto the stack.\n",
             page.page_id);
      break;
    }

    printf("[DEBUG] Pushed Page ID: %d | Current stack depth: %zu\n",
           page.page_id, history->length);
  }

  const WebPage_t *top_page = (const WebPage_t *)stack_peek(history);
  if (top_page) {
    printf(
        "\n[INFO] Peeking top element -> Page ID: %d, Load Time: %.2f sec\n\n",
        top_page->page_id, top_page->load_time);
  } else {
    printf("[ERROR] Failed to peek the top element.\n");
  }

  while (history->length > 0) {
    const WebPage_t *page = (const WebPage_t *)stack_peek(history);

    if (!page) {
      printf("[ERROR] Failed to peek element during pop phase.\n");
      break;
    }

    printf("[INFO] Popped Page ID: %d, Load Time: %.2f sec. Remaining depth: "
           "%zu.\n",
           page->page_id, page->load_time, history->length - 1);

    if (!stack_pop(history)) {
      printf("[ERROR] Failed to pop the top element.\n");
      break;
    }
  }

  if (stack_pop(history)) {
    printf(
        "[ERROR] Stack popped successfully when it should have been empty!\n");
  } else {
    printf("[DEBUG] Attempted to pop from empty stack. Properly returned "
           "false.\n");
  }

  stack_destroy(history);

  printf("======================================================\n");
  printf("[INFO] FINISHED DEMO: Stack (Dynamic Array)\n");
  printf("======================================================\n\n");

  return EXIT_SUCCESS;
}
