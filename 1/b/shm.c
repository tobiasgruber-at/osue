#include "shm.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h> 
#include <semaphore.h>

int open_shm(int init, int *shm_fd, shm_t **shm_p) {
    *shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (*shm_fd < 0) return -1;
    if (ftruncate(*shm_fd, sizeof(shm_t)) < 0) return -1;
    *shm_p = mmap(NULL, sizeof(**shm_p), PROT_READ | PROT_WRITE, MAP_SHARED, *shm_fd, 0);
    if (*shm_p == MAP_FAILED) return -1;
    return 0;
}

int close_shm(int destroy, int shm_fd, int shm_size) {
    if (close(shm_fd) < 0) return -1;
    if (munmap(NULL, shm_size) < 0) return -1;
    if (shm_unlink(SHM_NAME) < 0) return -1;
    return 0;
}

int open_sem(int init, sem_t **sem_p) {
    *sem_p = init == 1 ? sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 1) : sem_open(SEM_NAME, 0);
    if (*sem_p == SEM_FAILED) return -1;
    return 0;
}

int close_sem(int destroy, sem_t *sem) {
    if (sem_close(sem) < 0) return -1;
    if (destroy == 1) {
        if (sem_unlink(SEM_NAME) < 0) return -1;
    }
    return 0;
}
