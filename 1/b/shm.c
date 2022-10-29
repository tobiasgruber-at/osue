#include "shm.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h> 
#include <semaphore.h>
#include <stdio.h>

static int open_sem(int init, int count, sem_t **sem_p, char *sem_name) {
    *sem_p = init == 1 ? sem_open(sem_name, O_CREAT | O_EXCL, 0600, count) : sem_open(sem_name, 0);
    if (*sem_p == SEM_FAILED) return -1;
    return 0;
}

static int close_sem(int destroy, sem_t *sem, char *sem_name) {
    if (sem_close(sem) < 0) return -1;
    if (destroy == 1) {
        if (sem_unlink(sem_name) < 0) return -1;
    }
    return 0;
}

int open_shm(int init, int *shm_fd, shm_t **shm_p) {
    *shm_fd = init == 1 ? shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0600) : shm_open(SHM_NAME, O_RDWR, 0);
    if (*shm_fd < 0) return -1;
    if (init == 1) {
        if (ftruncate(*shm_fd, sizeof(shm_t)) < 0) return -1;
    }
    *shm_p = mmap(NULL, sizeof(**shm_p), PROT_READ | PROT_WRITE, MAP_SHARED, *shm_fd, 0);
    if (*shm_p == MAP_FAILED) return -1;
    if (init == 1) {
        (*shm_p)->active = 1;
        (*shm_p)->rd_i = 0;
        (*shm_p)->wr_i = 0;
    }
    return 0;
}

int close_shm(int destroy, int shm_fd) {
    if (close(shm_fd) < 0) return -1;
    if (munmap(NULL, sizeof(shm_t)) < 0) return -1;
    if (destroy == 1) {
        if (shm_unlink(SHM_NAME) < 0) return -1;
    }
    return 0;
}

int open_all_sem(int init, sem_map_t *sem_map) {
    if (open_sem(init, 1, &sem_map->cb, CB_SEM) == -1) return -1;
    if (open_sem(init, CB_MAX_LEN, &sem_map->cb_free, CB_FREE_SEM) == -1) return -1;
    if (open_sem(init, 0, &sem_map->cb_used, CB_USED_SEM) == -1) return -1;
    return 0;
}

int close_all_sem(int destroy, sem_map_t *sem_map) {
    if (close_sem(destroy, sem_map->cb, CB_SEM) == -1) return -1;
    if (close_sem(destroy, sem_map->cb_free, CB_FREE_SEM) == -1) return -1;
    if (close_sem(destroy, sem_map->cb_used, CB_USED_SEM) == -1) return -1;
    return 0;
}

int push_cb(edge_t fac[FAC_MAX_LEN], shm_t *shm, sem_map_t *sem_map) {
    if (sem_wait(sem_map->cb) == -1) return -1;
    if (sem_wait(sem_map->cb_free) == -1) return -1;
    printf("[%i] wr: %i\n", getpid(), shm->wr_i);
    /*memcyp()
    shm->cb[] = fac;*/
    ++shm->wr_i;
    shm->wr_i %= CB_MAX_LEN;
    if (sem_post(sem_map->cb_used) == -1) return -1;
    if (sem_post(sem_map->cb) == -1) return -1;
    return 0;
}

int read_cb(shm_t *shm, sem_map_t *sem_map, char dist[FAC_MAX_LEN]) {
    if (sem_wait(sem_map->cb_used) == -1) return -1;
    printf("[%i] rd: %i\n", getpid(), shm->rd_i);
    shm->rd_i++;
    shm->rd_i %= CB_MAX_LEN;
    if (sem_post(sem_map->cb_free) == -1) return -1;
    return 0;
}
