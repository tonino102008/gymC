#ifndef GYMC_H_
#define GYMC_H_

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FOR_ARRAY(dim, body)                    \
        do {                                    \
            for (size_t i = 0; i < dim; i++) {  \
                body;                           \
            }                                   \
        } while(0)

#define INIT_SPACE(obj, size, arr, set_fcn)     \
        do {                                    \
            obj->set = &set_fcn;                \
            obj->set(obj, size, (void*)arr);    \
        } while(0)

#define INIT_REWARD(obj, set_fcn)               \
        do {                                    \
            obj->set = &set_fcn;                \
            obj->set((void*)obj, (double) 0.0); \
            obj->cum_reward += obj->reward;     \
        } while(0)

#define INIT_ENV(obj, obs, rew, set_fcn)                \
        do {                                            \
            obj->set = &set_fcn;                        \
            obj->set((void*)obj, obs, rew, (int) 0);    \
        } while (0)

#define INIT_ENV_FCN(obj, reset, step, end)             \
        do {                                            \
            obj->reset_env = &reset;                    \
            obj->step_env = &step;                      \
            obj->end_env = &end;                        \
        } while (0)

#define CALL_ENV_FCN(env, fcn) env->fcn((void*)env)

typedef struct {
    size_t dim;
    void* array;
    void (*set)(void*, size_t, void*);
} Space;

typedef struct {
    double reward;
    double cum_reward;
    void (*set)(void*, double);
} Reward;

typedef struct {
    Space* obs_s;
    Reward* rew_r;
    int is_end;
    void (*reset_env)(void*);
    void (*step_env)(void*);
    void (*end_env)(void*);
    void (*set)(void*, Space*, Reward*, int);
} Environment;

void startEpisode(Environment* env);

void vlog(FILE* stream, const char* tag, const char* fmt, va_list args);
void vlog2(FILE* stream, const char* tag, const char* fmt, va_list args);
void INFO(const char* fmt, ...);
void INFO2(const char* fmt, ...);
void WARN(const char* fmt, ...);
void ERRO(const char* fmt, ...);
void PANIC(const char* fmt, ...);

#endif // GYMC_H

#ifdef GYMC_IMPLEMENTATION

void startEpisode(Environment* env) {
    CALL_ENV_FCN(env, reset_env); // sets is_end = 0
    while (!(env->is_end)) {
        CALL_ENV_FCN(env, step_env); // sets is_end = 1 eventually
    }
    CALL_ENV_FCN(env, end_env);
};

void vlog(FILE* stream, const char* tag, const char* fmt, va_list args) {
    fprintf(stream, "[%s] ", tag);
    vfprintf(stream, fmt, args);
    fprintf(stream, "\n");
};

void vlog2(FILE* stream, const char* tag, const char* fmt, va_list args) {
    fprintf(stream, "[%s] ", tag);
    vfprintf(stream, fmt, args);
};

void INFO(const char* fmt, ...) {
    va_list args;
    va_start(args,fmt);
    vlog(stdout, "INFO", fmt, args);
    va_end(args);
};

void INFO2(const char* fmt, ...) {
    va_list args;
    va_start(args,fmt);
    vlog2(stdout, "INFO", fmt, args);
    va_end(args);
};

void WARN(const char* fmt, ...) {
    va_list args;
    va_start(args,fmt);
    vlog(stderr, "WARN", fmt, args);
    va_end(args);
};

void ERRO(const char* fmt, ...) {
    va_list args;
    va_start(args,fmt);
    vlog(stderr, "ERRO", fmt, args);
    va_end(args);
};

void PANIC(const char* fmt, ...) {
    va_list args;
    va_start(args,fmt);
    vlog(stderr, "ERRO", fmt, args);
    va_end(args);
    exit(1);
};

#endif // GYMC_IMPLEMENTATION