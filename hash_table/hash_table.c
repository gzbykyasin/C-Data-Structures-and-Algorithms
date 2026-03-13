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
 * Macros & Defines
 ******************************************************************************/
#define TABLE_SIZE 100U

/******************************************************************************
 * Structs & Types
 ******************************************************************************/

/**
 * @brief Node structure representing an entry in the hash table bucket.
 */
typedef struct Node {
  void *key;
  void *val;
  struct Node *next;
} Node_t;

/**
 * @brief Hash Table structure handling generic keys and values of varying
 * sizes.
 */
typedef struct HashTable {
  Node_t **bucket;
  size_t key_size;
  size_t val_size;
} HashTable_t;

/******************************************************************************
 * Helper Functions
 ******************************************************************************/

/**
 * @brief Computes a hash index for a generic key.
 *
 * @param key Pointer to the data to be hashed.
 * @param key_size Size of the key data in bytes.
 * @return Computed hash index.
 */
static size_t hash_function(const void *key, size_t key_size) {
  const uint8_t *p = (const uint8_t *)key;
  size_t hash = 0;
  for (size_t i = 0; i < key_size; i++) {
    hash = (hash * 31) + p[i];
  }
  return hash % TABLE_SIZE;
}

/**
 * @brief Allocates and initializes a new Node for the hash table.
 *
 * @param key Pointer to the key data to copy.
 * @param val Pointer to the value data to copy.
 * @param key_size Size of the key data in bytes.
 * @param val_size Size of the value data in bytes.
 * @return Pointer to the newly created Node, or NULL on failure.
 */
static Node_t *create_node(void *key, void *val, size_t key_size,
                           size_t val_size) {
  Node_t *new_node = (Node_t *)malloc(sizeof(Node_t));

  if (new_node) {
    new_node->key = malloc(key_size);
    new_node->val = malloc(val_size);
    new_node->next = NULL;

    if (new_node->key && key) {
      memcpy(new_node->key, key, key_size);
    }
    if (new_node->val && val) {
      memcpy(new_node->val, val, val_size);
    }
  }

  return new_node;
}

/******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Initializes and returns a new empty Hash Table.
 *
 * @param key_size Size of each key in the table in bytes.
 * @param val_size Size of each value in the table in bytes.
 * @return Pointer to the initialized HashTable_t, or NULL on failure.
 */
HashTable_t *create_table(size_t key_size, size_t val_size) {
  HashTable_t *table = (HashTable_t *)malloc(sizeof(HashTable_t));

  if (NULL == table) {
    return NULL;
  }

  table->bucket = (Node_t **)calloc(TABLE_SIZE, sizeof(Node_t *));

  if (NULL == table->bucket) {
    free(table);
    return NULL;
  }

  table->key_size = key_size;
  table->val_size = val_size;
  return table;
}

/**
 * @brief Inserts or updates a key-value pair in the hash table.
 *
 * @param table Pointer to the hash table.
 * @param key Pointer to the key data.
 * @param val Pointer to the value data.
 * @return true if successful, false otherwise.
 */
bool set(HashTable_t *table, void *key, void *val) {
  if (NULL == table || NULL == key || NULL == val) {
    return false;
  }

  size_t index = hash_function(key, table->key_size);
  Node_t *current = table->bucket[index];

  while (NULL != current) {
    if (0 == memcmp(current->key, key, table->key_size)) {
      memcpy(current->val, val, table->val_size);
      return true;
    }
    current = current->next;
  }

  Node_t *new_node = create_node(key, val, table->key_size, table->val_size);

  if (new_node) {
    new_node->next = table->bucket[index];
    table->bucket[index] = new_node;
  }

  return (NULL != new_node);
}

/**
 * @brief Retrieves a value from the hash table by its key.
 *
 * @param table Pointer to the hash table.
 * @param key Pointer to the key data to lookup.
 * @return Pointer to the stored value data, or NULL if not found.
 */
const void *get(HashTable_t *table, void *key) {
  if (NULL == table || NULL == key) {
    return NULL;
  }

  size_t index = hash_function(key, table->key_size);
  Node_t *current = table->bucket[index];

  while (NULL != current) {
    if (0 == memcmp(current->key, key, table->key_size)) {
      return current->val;
    }
    current = current->next;
  }

  return NULL;
}

/**
 * @brief Deletes a key-value pair from the hash table.
 *
 * @param table Pointer to the hash table.
 * @param key Pointer to the key data to delete.
 * @return true if successfully deleted, false if the key was not found.
 */
bool delete(HashTable_t *table, void *key) {
  if (NULL == table || NULL == key) {
    return false;
  }

  size_t index = hash_function(key, table->key_size);
  Node_t *current = table->bucket[index];
  Node_t *prev = NULL;

  while (NULL != current) {
    if (0 == memcmp(current->key, key, table->key_size)) {
      if (NULL == prev) {
        table->bucket[index] = current->next;
      } else {
        prev->next = current->next;
      }

      free(current->key);
      free(current->val);
      free(current);

      return true;
    }

    prev = current;
    current = current->next;
  }

  return false;
}

/******************************************************************************
 * Main Function Test
 ******************************************************************************/

/**
 * @brief Prints the hash table structure and elements for debugging.
 *
 * @param table Pointer to the hash table.
 */
void print_table(HashTable_t *table) {
  if (NULL == table)
    return;

  for (int i = 0; i < TABLE_SIZE; i++) {
    if (table->bucket[i] == NULL)
      continue;

    printf("Index [%d]: ", i);
    Node_t *current = table->bucket[i];
    while (NULL != current) {
      if (table->key_size == sizeof(size_t) &&
          table->val_size == sizeof(size_t)) {
        printf("{%zu : %zu} -> ", *(size_t *)current->key,
               *(size_t *)current->val);
      } else {
        printf("{%p : %p} -> ", current->key, current->val);
      }
      current = current->next;
    }
    printf("NULL\n");
  }
}

/**
 * @brief Main entry point testing generic hash table functionality.
 */
int main() {
  printf("======================================================\n");
  printf("[INFO] STARTING DEMO: Hash Table\n");
  printf("======================================================\n");

  HashTable_t *ht = create_table(sizeof(size_t), sizeof(size_t));
  if (!ht) {
    printf("[ERROR] Failed to create hash table.\n");
    return -1;
  }

  size_t k1 = 1, v1 = 100;
  size_t k2 = 11, v2 = 200;
  size_t k3 = 21, v3 = 300;
  size_t k4 = 5, v4 = 500;
  size_t k5 = 99, v5 = 999;

  bool ret = set(ht, &k1, &v1);
  ret &= set(ht, &k2, &v2);
  ret &= set(ht, &k3, &v3);
  ret &= set(ht, &k4, &v4);
  ret &= set(ht, &k5, &v5);

  if (!ret) {
    printf("[ERROR] One of the set operations failed.\n");
  }

  printf("[INFO] --- Hash Table Occupancy Status ---\n");
  print_table(ht);

  // Search
  const void *get11 = get(ht, &k2);
  printf("\n[INFO] Value for key 11: %zu\n",
         get11 ? *(const size_t *)get11 : 0);

  const void *get21 = get(ht, &k3);
  printf("[INFO] Value for key 21: %zu\n", get21 ? *(const size_t *)get21 : 0);

  size_t missing = 999;
  const void *get999 = get(ht, &missing);
  if (get999 == NULL) {
    printf("[INFO] Value for key 999: Not found.\n");
  } else {
    printf("[INFO] Value for key 999: %zu\n", *(const size_t *)get999);
  }

  // Deletion
  printf("\n[INFO] --- Deletion Operation (Key: 11) ---\n");
  delete(ht, &k2);
  print_table(ht);

  printf("======================================================\n");
  printf("[INFO] FINISHED DEMO: Hash Table\n");
  printf("======================================================\n");

  return 0;
}