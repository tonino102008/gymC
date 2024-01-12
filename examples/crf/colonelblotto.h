#ifndef COLONELBLOTTO_H_
#define COLONELBLOTTO_H_

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_PLAYERS 2
#define SOLDIERS 5
#define BATTLEFIELDS 3
#define NUM_ACTIONS 21

extern size_t actions[NUM_ACTIONS][BATTLEFIELDS];

typedef struct Action Action;
typedef struct Player Player;
typedef struct Trainer Trainer;

struct Player {
    double* strategy;
    double* strategy_sum;
    double* strategy_avg;
    double* regret_sum;
    size_t action;
};

struct Trainer {
    Player* p1;
    Player* p2;
    size_t it;
    void (*train)(struct Trainer*);
};

double checkActions(size_t*, size_t*);
void getStrategy(Player*);
void getAction(Player*);
void getAvgStrategy(Player*);
void train_impl(Trainer*);

#endif // COLONELBLOTTO_H_