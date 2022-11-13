#include "shm.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h> 
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>

/**
 * Opens a semaphore.
 * @param init
 * @param count
 * @param sem_p
 * @param sem_name
 * @return
 */
static int open_sem(int init, int count, sem_t **sem_p, char *sem_name) {
    *sem_p = init == 1 ? sem_open(sem_name, O_CREAT | O_EXCL, 0600, count) : sem_open(sem_name, 0);
    if (*sem_p == SEM_FAILED) return t_err("sem_open");
    return 0;
}

/**
 * Closes a semaphore if it was opened.
 * @param destroy
 * @param sem
 * @param sem_name
 * @return
 */
static int close_sem(int destroy, sem_t *sem, char *sem_name) {
    if (sem == NULL) return 0;
    if (sem_close(sem) < 0) return t_err("sem_close");
    if (destroy == 1) {
        if (sem_unlink(sem_name) < 0) return t_err("sem_unlink");
    }
    return 0;
}

int open_shm(int init, int *shm_fd, shm_t **shm_p) {
    *shm_fd = init == 1 ? shm_open(SHM, O_RDWR | O_CREAT | O_EXCL, 0600) : shm_open(SHM, O_RDWR, 0);
    if (*shm_fd < 0) return t_err("shm_open");
    if (init == 1) {
        if (ftruncate(*shm_fd, sizeof(shm_t)) < 0) return t_err("ftruncate");
    }
    *shm_p = mmap(NULL, sizeof(shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, *shm_fd, 0);
    if (*shm_p == MAP_FAILED) return t_err("mmap");
    if (init == 1) {
        (*shm_p)->active = 1;
        (*shm_p)->rd_i = 0;
        (*shm_p)->wr_i = 0;
        for (int i = 0; i < CB_MAX_LEN; i++) {
            (*shm_p)->cb[i].size = 0;
        }
    }
    return 0;
}

int close_shm(int destroy, int shm_fd) {
    if (close(shm_fd) < 0) return t_err("close");
    if (munmap(NULL, sizeof(shm_t)) < 0) return t_err("munmap");
    if (destroy == 1) {
        if (shm_unlink(SHM) < 0) return t_err("shm_unlink");
    }
    return 0;
}

int open_all_sem(int init, sem_map_t *sem_map) {
    if (open_sem(init, 1, &sem_map->cb, SEM_CB_MUTEX) == -1) return t_err("open_sem");
    if (open_sem(init, CB_MAX_LEN, &sem_map->cb_free, SEM_CB_FREE) == -1) {
        close_all_sem(init, sem_map);
        return t_err("open_sem");
    }
    if (open_sem(init, 0, &sem_map->cb_used, SEM_CB_USED) == -1) {
        close_all_sem(init, sem_map);
        return t_err("open_sem");
    }
    return 0;
}

int close_all_sem(int destroy, sem_map_t *sem_map) {
    if (close_sem(destroy, sem_map->cb, SEM_CB_MUTEX) == -1) {
        close_sem(destroy, sem_map->cb_free, SEM_CB_FREE);
        close_sem(destroy, sem_map->cb_used, SEM_CB_USED);
        return t_err("close_sem");
    }
    if (close_sem(destroy, sem_map->cb_free, SEM_CB_FREE) == -1) {
        close_sem(destroy, sem_map->cb_used, SEM_CB_USED);
        return t_err("close_sem");
    }
    if (close_sem(destroy, sem_map->cb_used, SEM_CB_USED) == -1) return t_err("close_sem");
    return 0;
}

int push_cb(cbi_t cbi, shm_t *shm, sem_map_t *sem_map) {
    if (sem_wait(sem_map->cb) == -1) t_err("sem_wait");
    if (shm->active == 0) {
        sem_post(sem_map->cb);
        return 0;
    }
    if (sem_wait(sem_map->cb_free) == -1) {
        sem_post(sem_map->cb);
        t_err("sem_wait");
    }
    shm->cb[shm->wr_i] = cbi;
    ++shm->wr_i;
    shm->wr_i %= CB_MAX_LEN;
    if (sem_post(sem_map->cb_used) == -1) {
        sem_post(sem_map->cb);
        return t_err("sem_post");
    }
    if (sem_post(sem_map->cb) == -1) return t_err("sem_post");
    return 0;
}

int read_cb(shm_t *shm, sem_map_t *sem_map, cbi_t *dist) {
    if (sem_wait(sem_map->cb_used) == -1) {
        return errno == EINTR ? 0 : t_err("sem_wait");
    }
    *dist = shm->cb[shm->rd_i];
    shm->rd_i++;
    shm->rd_i %= CB_MAX_LEN;
    if (sem_post(sem_map->cb_free) == -1) {
        return errno == EINTR ? 0 : t_err("sem_post");
    }
    return 0;
}
