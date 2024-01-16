#ifndef KUHN_H_
#define KUHN_H_

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_PLAYERS 2
#define NUM_CARDS 3

typedef struct InfoSet InfoSet;
typedef struct Trainer Trainer;

typedef enum {
    PASS,
    BET,
    NUM_ACTIONS
} Action;

struct InfoSet {
    char* infoset;
    double* strategy;
    double* strategy_avg;
    double* strategy_sum;
    double* regret_sum;
    void (*getStrategy)(struct InfoSet*);
    void (*getAvgStrategy)(struct InfoSet*);
    char* (*toString)(struct InfoSet*);
};

struct Trainer {
    size_t it;
    int* cards;
    char* history;
    size_t lenH;
    void (*train)(struct Trainer*);
    double (*cfr)(struct Trainer*, double, double);
};

void printTreeNode(void*, void*);
int compareTreeNode(void*, void*);

void getStrategy(InfoSet*);
void getAvgStrategy(InfoSet*);
char* toString(InfoSet*);

void train(Trainer*);
double cfr(Trainer*, double, double);

#endif // KUHN_H_