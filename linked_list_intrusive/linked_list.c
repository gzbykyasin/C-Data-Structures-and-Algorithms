/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


/******************************************************************************
 * Macros & Defines
 ******************************************************************************/

/**
 * @brief Calculates the byte offset of a given member within a struct.
 *
 * A standard-like implementation of the offsetof macro.
 */
#define my_offsetof(type, member) ((size_t)&((type *)0)->member)

/**
 * @brief Casts a member of a structure out to the containing structure.
 *
 * This is the legendary Linux kernel macro that allows pointer arithmetic
 * to retrieve the parent structure given a pointer to one of its embedded
 * members.
 */
#ifdef __GNUC__
#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *mm_ptr = (ptr);                         \
    (type *)((char *)mm_ptr - my_offsetof(type, member));                      \
  })
#else
#define container_of(ptr, type, member)                                        \
  ((type *)((char *)(ptr) - my_offsetof(type, member)))
#endif

/**
 * @brief Iterates over an intrusive list.
 *
 * @param pos  The list_head_t pointer used as a loop cursor.
 * @param head The head of the list to iterate through.
 */
#define list_for_each(pos, head)                                               \
  for (pos = (head)->next; pos != head; pos = pos->next)

/******************************************************************************
 * Structs & Types
 ******************************************************************************/
/**
 * @brief Represents an intrusive list head.
 *
 * Embedded within user-defined structures to link them together.
 */
typedef struct list_head_s {
  struct list_head_s *next, *prev;

} list_head_t;

/**
 * @brief Example user-defined structure incorporating an intrusive list node.
 */
typedef struct sensor_node_s {
  int id;
  float val;
  list_head_t list;

} sensor_node_t;

/******************************************************************************
 * Static / Helper Functions
 ******************************************************************************/
/**
 * @brief Initializes a list head by pointing it to itself.
 *
 * Used to set up an empty circular doubly linked list.
 *
 * @param list Pointer to the list head to be initialized.
 */
static inline void init_list_head(list_head_t *list) {
  list->next = list;
  list->prev = list;
}

/**
 * @brief Adds a new entry to the tail of the list.
 *
 * This performs a tail insertion into the circular doubly linked list.
 *
 * @param new_entry Pointer to the new list element to insert.
 * @param head      Pointer to the list head.
 */
static inline void list_add_tail(list_head_t *new_entry, list_head_t *head) {
  list_head_t *prev = head->prev;
  list_head_t *next = head;

  next->prev = new_entry;
  new_entry->next = next;
  new_entry->prev = prev;
  prev->next = new_entry;
}

/******************************************************************************
 * Main Function
 ******************************************************************************/
/**
 * @brief Entry point demonstrating intrusive linked list usage.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return EXIT_SUCCESS on success.
 */
int main(int argc, char **argv) {
  printf("\n======================================================\n");
  printf("[INFO] STARTING DEMO: Intrusive Linked List\n");
  printf("======================================================\n");

  list_head_t sensor_list_head;
  init_list_head(&sensor_list_head);

  sensor_node_t s1 = {101, 85.5f};
  sensor_node_t s2 = {102, 95.5f};
  sensor_node_t s3 = {103, 105.5f};

  list_add_tail(&s1.list, &sensor_list_head);
  list_add_tail(&s2.list, &sensor_list_head);
  list_add_tail(&s3.list, &sensor_list_head);

  printf("[INFO] Initiating dump of the recorded sensor list:\n");

  list_head_t *ptr;
  sensor_node_t *entry;

  list_for_each(ptr, &sensor_list_head) {
    entry = container_of(ptr, sensor_node_t, list);

    printf("[INFO] Sensor Profile -> ID: %d, Temperature: %.2f\n", entry->id,
           entry->val);
  }

  printf("======================================================\n");
  printf("[INFO] FINISHED DEMO: Intrusive Linked List\n");
  printf("======================================================\n\n");

  return EXIT_SUCCESS;
}
