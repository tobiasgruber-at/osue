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

typedef struct SemaphoreMap {
    sem_t *cb;
    sem_t *cb_used;
    sem_t *cb_free;
} sem_map_t;

typedef struct CircularBufferItem {
    int size;
    edge_t fas[FAC_MAX_LEN];
} cbi_t;

typedef struct SharedMemory {
    unsigned int active;
    unsigned int wr_i;
    unsigned int rd_i;
    cbi_t cb[CB_MAX_LEN]; /**< Circular buffer. */
} shm_t;

int open_shm(int init, int *shm_fd, shm_t **shm_p);

int close_shm(int destroy, int shm_fd);

int open_all_sem(int init, sem_map_t *sem_map);

int close_all_sem(int destroy, sem_map_t *sem_map);

int push_cb(cbi_t cbi, shm_t *shm, sem_map_t *sem_map);

int read_cb(shm_t *shm, sem_map_t *sem_map, cbi_t *dist);
