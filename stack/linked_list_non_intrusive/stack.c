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
 * @brief Represents a single node in a generic non-intrusive linked list stack.
 */
typedef struct Node {
  struct Node *next;
  uint8_t *data;
} Node_t;

/**
 * @brief Represents a generic stack utilizing a non-intrusive linked list.
 */
typedef struct {
  Node_t *head;
  size_t elem_size;
  size_t length;
} Stack_t;

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
 * @brief Pushes an element onto the top of the stack.
 *
 * This implementation dynamically allocates a new node and a separate block of
 * memory to store the user's data copy, satisfying the non-intrusive design.
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

  uint8_t *new_data = (uint8_t *)malloc(p_stack->elem_size);
  if (!new_data) {
    return false;
  }

  (void)memcpy(new_data, p_data, p_stack->elem_size);

  Node_t *new_node = (Node_t *)calloc(1, sizeof(Node_t));
  if (!new_node) {
    free(new_data);
    return false;
  }

  new_node->data = new_data;

  new_node->next = p_stack->head;
  p_stack->head = new_node;

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
  if (!p_stack || p_stack->length == 0 || !p_stack->head) {
    return NULL;
  }

  return p_stack->head->data;
}

/**
 * @brief Removes the top element from the stack and frees its resources.
 *
 * @param p_stack Pointer to the stack.
 * @return true if an element was successfully popped, false if empty.
 */
bool stack_pop(Stack_t *p_stack) {
  if (!p_stack || p_stack->length == 0 || !p_stack->head) {
    return false;
  }

  Node_t *target_node = p_stack->head;
  p_stack->head = target_node->next;

  if (target_node->data) {
    free(target_node->data);
  }
  free(target_node);

  p_stack->length--;

  return true;
}

/**
 * @brief Destroys the stack and frees all heap memory (nodes and data).
 *
 * @param p_stack Pointer to the stack to destroy.
 */
void stack_destroy(Stack_t *p_stack) {
  if (!p_stack) {
    return;
  }

  while (stack_pop(p_stack)) {
  }

  p_stack->elem_size = 0;
  p_stack->length = 0;

  free(p_stack);
}

/******************************************************************************
 * Main Function Test
 ******************************************************************************/
/**
 * @brief Example structure representing a function call frame.
 */
typedef struct {
  char function_name[32];
  uint32_t process_id;
} CallFrame_t;

/**
 * @brief Entry point of the program illustrating the usage of the linked list stack.
 */
int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  printf("\n======================================================\n");
  printf("[INFO] STARTING DEMO: Stack (Linked List)\n");
  printf("======================================================\n");

  Stack_t *call_stack = stack_init(sizeof(CallFrame_t));

  if (!call_stack) {
    printf("[ERROR] Failed to initialize the stack.\n");
    return EXIT_FAILURE;
  }

  // Push
  for (int i = 1; i <= 4; ++i) {
    CallFrame_t frame = {0};
    snprintf(frame.function_name, sizeof(frame.function_name), "Function_%d",
             i);
    frame.process_id = 1000 + i;

    if (!stack_push(call_stack, &frame)) {
      printf("[ERROR] Failed to push %s onto the stack.\n",
             frame.function_name);
      break;
    }

    printf("[DEBUG] Pushed %s (PID: %u) | Current stack depth: %zu\n",
           frame.function_name, frame.process_id, call_stack->length);
  }

  const CallFrame_t *top_frame = (const CallFrame_t *)stack_peek(call_stack);
  if (top_frame) {
    printf("\n[INFO] Peeking top element -> %s (PID: %u)\n\n",
           top_frame->function_name, top_frame->process_id);
  } else {
    printf("[ERROR] Failed to peek the top element.\n");
  }

  while (call_stack->length > 0) {
    const CallFrame_t *frame = (const CallFrame_t *)stack_peek(call_stack);

    if (!frame) {
      printf("[ERROR] Failed to peek element during pop phase.\n");
      break;
    }

    printf("[INFO] Popped %s (PID: %u). Remaining depth: %zu.\n",
           frame->function_name, frame->process_id, call_stack->length - 1);

    if (!stack_pop(call_stack)) {
      printf("[ERROR] Failed to pop the top element.\n");
      break;
    }
  }

  if (stack_pop(call_stack)) {
    printf(
        "[ERROR] Stack popped successfully when it should have been empty!\n");
  } else {
    printf("[DEBUG] Attempted to pop from empty stack. Properly returned "
           "false.\n");
  }

  stack_destroy(call_stack);

  printf("======================================================\n");
  printf("[INFO] FINISHED DEMO: Stack (Linked List)\n");
  printf("======================================================\n\n");

  return EXIT_SUCCESS;
}
