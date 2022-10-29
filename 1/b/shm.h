#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <signal.h>
#include <semaphore.h>

#define SHM_NAME "/fac_shm"
#define SEM_NAME "fac_buffer"
#define MAX_DATA (50)

typedef struct SharedMemory {
    unsigned int state;
    unsigned int data[MAX_DATA];
} shm_t;

int open_shm(int init, int *shm_fd, shm_t **shm_p);

int close_shm(int destroy, int shm_fd, int shm_size);

int open_sem(int init, sem_t **sem_p);

int close_sem(int destroy, sem_t *sem);