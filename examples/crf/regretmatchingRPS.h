#ifndef REGRETMATCHINGRPS_H_
#define REGRETMATCHINGRPS_H_

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef enum {
    ROCK,
    PAPER,
    SCISSORS,
    NUM_ACTIONS,
} RPS;

extern char* RPS_char[];

typedef struct Trainer Trainer;
typedef struct Trainer2 Trainer2;

struct Trainer {
    double opponent_strategy[3];
    double* regret_sum;
    double* strategy_sum;
    double* strategy;
    void (*train)(Trainer*, size_t);
};

struct Trainer2 {
    double* opponent_regret_sum;
    double* opponent_strategy;
    double* opponent_strategy_sum;
    double* regret_sum;
    double* strategy_sum;
    double* strategy;
    void (*train)(Trainer2*, size_t);
};

void getStrategy(double*, double*, double*);
double* getAverageStrategy(double*);
int getAction(double*);
void train_impl(Trainer*, size_t);

void train2_impl(Trainer2*, size_t);

#endif // REGRETMATCHING_H_