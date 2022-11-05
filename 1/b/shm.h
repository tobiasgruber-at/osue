#include "graph.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <signal.h>
#include <semaphore.h>

#define SHM_NAME "/fac_shm"
#define CB_SEM "fac_cb"
#define CB_FREE_SEM "fac_cb_read"
#define CB_USED_SEM "fac_cb_used"
#define CB_MAX_LEN (50)
#define FAC_MAX_LEN (8)

/** @brief Map for all semaphores. */
typedef struct SemaphoreMap {
    sem_t *cb; /**< Semaphore for the circular buffer access. */
    sem_t *cb_free; /**< Semaphore for the free space of the circular buffer. */
    sem_t *cb_used; /**< Semaphore for the used space of the circular buffer. */
} sem_map_t;

/** @brief Item of a circular buffer, containing a feedback arc set and infos. */
typedef struct CircularBufferItem {
    int size; /**< Size of the contained feedback arc set. */
    edge_t fas[FAC_MAX_LEN]; /**< Contained feedback arc set */
} cbi_t;

/**
 * @brief Shared Memory structure.
 */
typedef struct SharedMemory {
    unsigned int active; /**< Whether the program is still active. */
    cbi_t cb[CB_MAX_LEN]; /**< Circular buffer containing feedback arc sets. */
    unsigned int wr_i; /**< Current write index of the circular buffer. */
    unsigned int rd_i; /**< Current read index of the circular buffer. */
} shm_t;

/**
 * @brief Opens the shared memory.
 * @param init 1 to initialise and open them, 0 to just open them.
 * @param shm_fd File descriptor of the shared memory.
 * @param shm_p Pointer to the shared memory.
 * @return 0 on success, -1 on error.
 */
int open_shm(int init, int *shm_fd, shm_t **shm_p);

/**
 * @brief Closes the shared memory.
 * @param destroy 1 to close and destroy them, 0 to just close them.
 * @param shm_fd File descriptor of the shared memory.
 * @return 0 on success, -1 on error.
 */
int close_shm(int destroy, int shm_fd);

/**
 * @brief Opens all necessary semaphores.
 * @param init 1 to initialise and open them, 0 to just open them.
 * @param sem_map Pointer to semaphore map that will be updated.
 * @return 0 on success, -1 on error.
 */
int open_all_sem(int init, sem_map_t *sem_map);

/**
 * @brief Closes all semaphores if they are opened.
 * @param destroy 1 to close and unlink them, 0 to just close them.
 * @param sem_map Map of the opened semaphores.
 * @return 0 on success, -1 on error.
 */
int close_all_sem(int destroy, sem_map_t *sem_map);

/**
 * @brief Pushes an item to the circular buffer.
 * @param cbi Item to be added.
 * @param shm Pointer to the shared memory.
 * @param sem_map Pointer to the semaphore map.
 * @return 0 on success, -1 on error.
 */
int push_cb(cbi_t cbi, shm_t *shm, sem_map_t *sem_map);

/**
 * @brief Reads an item from the circular buffer.
 * @details The item on the current index is read.
 * @param shm Pointer to the shared memory.
 * @param sem_map Pointer to the semaphore map.
 * @param dist Pointer to item that will be updated with the result.
 * @return 0 on success, -1 on error.
 */
int read_cb(shm_t *shm, sem_map_t *sem_map, cbi_t *dist);
