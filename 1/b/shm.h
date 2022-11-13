/**
 * Shared memory module definitions
 * @brief Covers all necessary macros, types and operations regarding the shared memory and its semaphores.
 * @details Provides operations to initialise, open, close or unlink shared memory and semaphores as well as
 * operations to push or read from the circular buffer.
 * @file shm.h
 * @author Tobias Gruber, 11912367
 * @date 29.10.2022
 **/

#include "graph.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <signal.h>
#include <semaphore.h>

#define PREFIX "/11912367_fac_" /**< Prefix for names of shared memory and semaphores. */
#define SHM PREFIX "shm" /**< Name of the shared memory. */
#define SEM_CB_MUTEX PREFIX "sem_cb_mutex" /**< Name of semaphore for mutual exclusion of the circular buffer. */
#define SEM_CB_FREE PREFIX "sem_cb_read" /**< Name of semaphore indicating the free space of the circular buffer. */
#define SEM_CB_USED PREFIX "sem_cb_used" /**< Name of semaphore indicating the used space of the circular buffer. */
#define CB_MAX_LEN (50) /**< Max length of the circular buffer. */
#define FAC_MAX_LEN (8) /**< Max length of a feedback arc set. */

/** Map covering all semaphores. */
typedef struct SemaphoreMap {
    sem_t *cb_mutex; /**< Semaphore for mutual exclusion of the circular buffer. */
    sem_t *cb_free; /**< Semaphore indicating the free space of the circular buffer. */
    sem_t *cb_used; /**< Semaphore indicating the used space of the circular buffer. */
} sem_map_t;

/** Item of a circular buffer, containing a feedback arc set and infos. */
typedef struct CircularBufferItem {
    int size; /**< Size of the feedback arc set. */
    edge_t fas[FAC_MAX_LEN]; /**< Feedback arc set */
} cbi_t;

/** Shared Memory, containing the circular buffer and important infos. */
typedef struct SharedMemory {
    unsigned int active; /**< Whether the program should still run. */
    cbi_t cb[CB_MAX_LEN]; /**< Circular buffer containing feedback arc sets. */
    unsigned int wr_i; /**< Current write index of the circular buffer. */
    unsigned int rd_i; /**< Current read index of the circular buffer. */
} shm_t;

/**
 * @brief Opens the shared memory.
 * @details Opens or optionally also initialises the shared memory with read and write access and maps it into the memory.
 * @param init 1 to open and initialise, 0 to just open.
 * @param shm_fd Pointer to file descriptor of the shared memory.
 * @param shm_p Pointer to pointer to shared memory that will be opened.
 * @return 0 on success, -1 on error.
 */
int open_shm(int init, int *shm_fd, shm_t **shm_p);

/**
 * @brief Closes the shared memory.
 * @details Closes and optionally unlinks the shared memory and unmap it from the memory.
 * @param unlink 1 to also unlink, 0 otherwise.
 * @param shm_fd File descriptor of the shared memory.
 * @return 0 on success, -1 on error.
 */
int close_shm(int unlink, int shm_fd);

/**
 * @brief Opens all semaphores.
 * @details Opens or optionally also initialises all semaphores.
 * @param init 1 to initialise and open them, 0 to just open them.
 * @param sem_map Pointer to semaphore map that will be updated.
 * @return 0 on success, -1 on error.
 */
int open_all_sem(int init, sem_map_t *sem_map);

/**
 * @brief Closes all semaphores.
 * @details Closes and optionally unlinks them, but only if they were opened.
 * @param unlink 1 to also unlink, 0 otherwise.
 * @param sem_map Pointer to map of the opened semaphores.
 * @return 0 on success, -1 on error.
 */
int close_all_sem(int unlink, sem_map_t *sem_map);

/**
 * @brief Pushes an item to the circular buffer.
 * @details The item on the current index is overwritten.<br>
 * Respects two synchronisations:<ul>
 * <li>Waits for the mutual access semaphore, so that only one write operation at the time happens.</li>
 * <li>Waits for the free space semaphore, so that only free slots in the circular buffers are overwritten.</li></ul><br>
 * Once the end of the buffer is reached, it starts writing from the start again.
 * @param cbi Item to be added.
 * @param shm Pointer to the shared memory.
 * @param sem_map Pointer to the semaphore map.
 * @return 0 on success, -1 on error.
 */
int push_cb(cbi_t cbi, shm_t *shm, sem_map_t *sem_map);

/**
 * @brief Reads an item from the circular buffer.
 * @details The item on the current index is read.<br>
 * Waits for the used space semaphore, so that only used slots in the circular buffer are read.<br>
 * Once the end of the buffer is reached, it starts reading from the start again.<br>
 * If the semaphore is currently waiting or posting and an interrupt happens, the function returns without an error.
 * @param shm Pointer to the shared memory.
 * @param sem_map Pointer to the semaphore map.
 * @param dist Pointer to item that will be updated with the result.
 * @return 0 on success, -1 on error.
 */
int read_cb(shm_t *shm, sem_map_t *sem_map, cbi_t *dist);
