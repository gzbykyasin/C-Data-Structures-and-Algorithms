/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/******************************************************************************
 * Structs & Types
 ******************************************************************************/
/**
 * @brief Represents a single node in a doubly linked list.
 */
typedef struct Node_s {
  void *data;
  struct Node_s *next;
  struct Node_s *prev;
} Node_t;

/**
 * @brief Represents the non-intrusive linked list structure.
 *
 * Manages the head, tail, and size of the dynamically allocated data.
 */
typedef struct LinkedList_s {
  size_t data_size;
  size_t size;
  Node_t *head;
  Node_t *tail;
} LinkedList_t;

/******************************************************************************
 * Static / Helper Functions
 ******************************************************************************/
/**
 * @brief A helper function to create a new linked list node.
 *
 * Performs generic dynamic memory allocation to store a copy of the provided
 * data within a newly created node structure.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data payload to be stored.
 * @return A correctly initialized Node_t pointer, or NULL on allocation
 * failure.
 */
static Node_t *create_node_helper(LinkedList_t *list, const void *data) {
  Node_t *new_node = (Node_t *)calloc(1, sizeof(Node_t));

  if (new_node) {
    new_node->data = (void *)malloc(list->data_size);

    if (!new_node->data) {
      free(new_node);
      return NULL;
    }

    memcpy(new_node->data, data, list->data_size);
  }

  return new_node;
}

/******************************************************************************
 * Public / Interface Functions
 ******************************************************************************/
/**
 * @brief Creates and initializes a new non-intrusive linked list.
 *
 * Sets up the initial list management interfaces. The head, tail, and size 
 * are implicitly initialized to NULL/0 via calloc.
 *
 * @param data_size Size of the generic data elements the list will hold (in
 * bytes).
 * @return A pointer to the newly allocated LinkedList_t, or NULL on failure.
 */
LinkedList_t *create_linked_list(size_t data_size) {
  LinkedList_t *list = (LinkedList_t *) calloc(1, sizeof(LinkedList_t));

  if (list) {
    list->data_size = data_size;
  }

  return list;
}

/**
 * @brief Adds an element to the tail of the linked list.
 *
 * Allocates a new node and inserts it at the end of the sequence.
 *
 * @param list Pointer to the linked list.
 * @param data Pointer to the data payload to insert.
 * @return true if the element was added successfully, false otherwise.
 */
bool push_back(LinkedList_t *list, const void *data) {
  if (!list || !data) {
    return false;
  }

  Node_t *new_node = create_node_helper(list, data);

  if (new_node) {
    if (!list->head) {
      list->head = new_node;
      list->tail = new_node;
    } else {
      list->tail->next = new_node;
      new_node->prev = list->tail;
      list->tail = new_node;
    }

    list->size++;
  }

  return (NULL != new_node);
}

/**
 * @brief Removes the last element from the linked list.
 *
 * Safely unlinks the tail node and frees the dynamically allocated memory
 * for both the data payload and the node itself.
 *
 * @param list Pointer to the linked list.
 */
void pop_back(LinkedList_t *list) {
  if (NULL == list || NULL == list->tail) {
    return;
  }

  Node_t *node_to_delete = list->tail;

  if (NULL != node_to_delete->prev) {
    list->tail = node_to_delete->prev;
    list->tail->next = NULL;
  } else {
    list->head = NULL;
    list->tail = NULL;
  }

  if (node_to_delete->data) {
    free(node_to_delete->data);
  }

  free(node_to_delete);
  list->size--;
}

/**
 * @brief Searches the list using a generic function pointer callback.
 *
 * Traverses the entire list invoking the `match_func` on each node's data.
 *
 * @param list       Pointer to the linked list.
 * @param key        An integer key used as search criteria.
 * @param match_func A callback function that assesses if a given node's data
 * matches the key.
 * @return A pointer to the matching element's data, or NULL if no match is
 * found.
 */
void *find_by_key(LinkedList_t *list, int key,
                  bool (*match_func)(void *data, int key)) {
  if (NULL == list || NULL == list->head) {
    return NULL;
  }

  Node_t *current = list->head;
  while (NULL != current) {
    if (match_func(current->data, key)) {
      return current->data;
    }
    current = current->next;
  }

  return NULL;
}

/**
 * @brief Prints the nodes of the linked list for debugging.
 *
 * @param list Pointer to the linked list.
 */
void print_list(LinkedList_t *list) {
  if (NULL == list) {
    return;
  }

  Node_t *current = list->head;
  while (NULL != current) {
    printf("[DEBUG] Node Address: %p | Next: %p | Prev: %p\n", (void *)current,
           (void *)current->next, (void *)current->prev);
    current = current->next;
  }
}

/******************************************************************************
 * Example Usage Structs & Callbacks
 ******************************************************************************/
/**
 * @brief Example structure representing sensor data payload.
 */
typedef struct SensorData_s {
  int key;
  float val;
} SensorData_t;

/**
 * @brief Callback function to match a sensor data payload by its key.
 *
 * @param data Pointer to the sensor data element.
 * @param key  The key to search for.
 * @return true if the keys match, false otherwise.
 */
bool compare_sensor_key(void *data, int key) {
  SensorData_t *sensor = (SensorData_t *)data;
  return sensor->key == key;
}

/******************************************************************************
 * Main Function
 ******************************************************************************/
/**
 * @brief Entry point demonstrating non-intrusive linked list usage.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return 0 on success, -1 on failure.
 */
int main(int argc, char **argv) {
  printf("\n======================================================\n");
  printf("[INFO] STARTING DEMO: Non-Intrusive Linked List\n");
  printf("======================================================\n");

  LinkedList_t *list = create_linked_list(sizeof(SensorData_t));

  if (NULL == list) {
    printf("[ERROR] Failed to create the linked list.\n");
    return EXIT_FAILURE;
  }

  SensorData_t data1 = {.key = 0x1, .val = 10.0f};
  SensorData_t data2 = {.key = 0x2, .val = 11.0f};
  SensorData_t data3 = {.key = 0x3, .val = 12.0f};
  SensorData_t data4 = {.key = 0x4, .val = 13.0f};
  SensorData_t data5 = {.key = 0x5, .val = 14.0f};

  bool ret = push_back(list, &data1);
  ret &= push_back(list, &data2);
  ret &= push_back(list, &data3);
  ret &= push_back(list, &data4);
  ret &= push_back(list, &data5);

  if (!ret) {
    printf("[ERROR] Failed to populate the linked list with initial data.\n");
    return EXIT_FAILURE;
  }

  print_list(list);

  int search_key = 0x3;
  SensorData_t *found =
      (SensorData_t *)find_by_key(list, search_key, compare_sensor_key);

  if (found) {
    printf("\n[INFO] Successfully located the element with key %d. Its value "
           "is %.2f.\n\n",
           search_key, found->val);
  } else {
    printf("\n[WARNING] Could not find any element matching the key %d.\n\n",
           search_key);
  }

  while (list->size > 0) {
    pop_back(list);
    print_list(list);
  }

  free(list);

  printf("======================================================\n");
  printf("[INFO] FINISHED DEMO: Non-Intrusive Linked List\n");
  printf("======================================================\n\n");

  return EXIT_SUCCESS;
}
