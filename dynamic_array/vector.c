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
 * @brief Represents a dynamic array (vector) that can grow in capacity.
 *
 * Stores generic elements of a fixed size contiguously in memory.
 */
typedef struct {
  uint8_t *data;
  size_t capacity;
  size_t elem_size;
  size_t length;

} Vector_t;

/******************************************************************************
 * Static / Helper Functions
 ******************************************************************************/
/**
 * @brief Grows the internal capacity of the vector.
 *
 * Doubles the current capacity, or initializes it to 4 if the vector is empty.
 * Uses realloc() to resize the underlying memory.
 *
 * @param p_vec Pointer to the vector instance.
 * @return true if memory reallocation was successful, false otherwise.
 */
static bool vector_grow(Vector_t *p_vec) {
  if (!p_vec) {
    return false;
  }

  size_t new_capacity;

  if (p_vec->capacity == 0) {
    new_capacity = 4;
  } else {
    if (p_vec->capacity > (SIZE_MAX / 2)) {
      return false;
    }

    new_capacity = p_vec->capacity * 2;
  }

  if (new_capacity > (SIZE_MAX / p_vec->elem_size)) {
    return false;
  }

  size_t total_bytes = new_capacity * p_vec->elem_size;

  void *temp_ptr = malloc(total_bytes);

  if (!temp_ptr) {
    return false;
  }

  if (p_vec->data && p_vec->length > 0) {
    (void)memcpy(temp_ptr, p_vec->data, p_vec->length * p_vec->elem_size);
    free(p_vec->data);
  }

  p_vec->data = (uint8_t *)temp_ptr;
  p_vec->capacity = new_capacity;

  return true;
}

/******************************************************************************
 * Public / Interface Functions
 ******************************************************************************/
/**
 * @brief Initializes a vector and sets its baseline parameters.
 *
 * @param elem_size The size of each element in bytes (e.g., sizeof(int)).
 * @return The vector pointer if successful, NULL otherwise.
 */
Vector_t *vector_init(size_t elem_size) {
  if (elem_size == 0) {
    return NULL;
  }

  Vector_t *p_vec = (Vector_t *)calloc(1, sizeof(Vector_t));
  if (p_vec) {
    p_vec->elem_size = elem_size;
  }

  return p_vec;
}

/**
 * @brief Destroys the vector and frees its reserved heap memory.
 *
 * @param p_vec Pointer to the vector to destroy.
 */
void vector_destroy(Vector_t *p_vec) {
  if (!p_vec) {
    return;
  }

  if (p_vec->data) {
    free(p_vec->data);
    p_vec->data = NULL;
  }

  p_vec->elem_size = 0;
  p_vec->length = 0;
  p_vec->capacity = 0;
}

/**
 * @brief Appends an element to the end of the vector.
 *
 * This operation runs in amortized O(1) time complexity. If the internal
 * array is full, it will trigger a capacity expansion.
 *
 * @param p_vec Pointer to the vector.
 * @param p_data Pointer to the data to be copied into the vector.
 * @return true on success, false on memory allocation failure or invalid
 * inputs.
 */
bool vector_push_back(Vector_t *p_vec, const void *const p_data) {
  if (!p_vec || !p_data) {
    return false;
  }

  if (p_vec->length == p_vec->capacity) {
    if (!vector_grow(p_vec)) {
      printf("[ERROR] Failed to expand the vector's capacity.\n");
      return false;
    }
  }

  size_t offset = p_vec->elem_size * p_vec->length;
  uint8_t *target = p_vec->data + offset;

  (void)memcpy(target, p_data, p_vec->elem_size);

  p_vec->length++;

  return true;
}

/**
 * @brief Safely retrieves an element from the vector by its index.
 *
 * Performs bounds checking to ensure the index is within the valid range.
 *
 * @param p_vec Pointer to the vector.
 * @param index The 0-based index of the element to retrieve.
 * @return A constant pointer to the element, or NULL if out-of-bounds.
 */
const void *const vector_get(Vector_t *p_vec, size_t index) {
  if (!p_vec || (index >= p_vec->length)) {
    return NULL;
  }

  return p_vec->data + (index * p_vec->elem_size);
}

/**
 * @brief Removes the last element from the vector.
 *
 * Note that this does not shrink the allocated memory, but merely
 * decrements the length counter, making it an O(1) operation.
 *
 * @param p_vec Pointer to the vector.
 * @return true if an element was removed, false if the vector is empty.
 */
bool vector_pop_back(Vector_t *p_vec) {
  if (!p_vec || (p_vec->length == 0)) {
    return false;
  }

  p_vec->length--;

  return true;
}

/******************************************************************************
 * Example Usage Structs & Callbacks
 ******************************************************************************/
/**
 * @brief Example structure representing a sensor reading.
 */
typedef struct {
  int id;
  float value;

} SensorReading_t;

/******************************************************************************
 * Main Function
 ******************************************************************************/
/**
 * @brief Entry point of the program illustrating the usage of Vector_t.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int main(int argc, char **argv) {
  printf("\n======================================================\n");
  printf("[INFO] STARTING DEMO: Dynamic Array (Vector)\n");
  printf("======================================================\n");

  Vector_t *sensors = vector_init(sizeof(SensorReading_t));

  if (!sensors) {
    printf("[ERROR] Failed to initialize the vector.\n");

    return EXIT_FAILURE;
  }

  for (int i = 0; i < 10; ++i) {
    SensorReading_t s = {.id = i, .value = 1.55f * i};

    if (!vector_push_back(sensors, &s)) {
      printf("[ERROR] Failed to push back the element at index %d.\n", i);
      break;
    }

    printf(
        "[DEBUG] Successfully appended element. Current vector length: %zu.\n",
        sensors->length);
  }

  for (size_t i = sensors->length; i-- > 0;) {
    const SensorReading_t *const s =
        (const SensorReading_t *const)vector_get(sensors, i);

    if (!s) {
      printf("[ERROR] Failed to retrieve the element at index %zu.\n", i);
      break;
    }

    if (!vector_pop_back(sensors)) {
      printf("[ERROR] Failed to pop the last element at index %zu.\n", i);
      break;
    }

    printf("[INFO] Successfully retrieved element at index %zu -> ID: %d, "
           "Value: %.2f. Remaining length: %zu.\n",
           i, s->id, s->value, sensors->length);
  }

  vector_destroy(sensors);

  printf("======================================================\n");
  printf("[INFO] FINISHED DEMO: Dynamic Array (Vector)\n");
  printf("======================================================\n\n");

  return EXIT_SUCCESS;
}
