#ifndef KUHN_H_
#define KUHN_H_

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../treemap.h"

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
    double* strategy;
    double* strategy_avg;
    double* strategy_sum;
    double* regret_sum;
    void (*_getStrategy)(struct InfoSet*, double);
    void (*_getAvgStrategy)(struct InfoSet*);
};

struct Trainer {
    size_t it;
    int* cards;
    char* history;
    size_t lenH;
    TreeMap* tree;
    void (*_train)(struct Trainer*);
    double (*_cfr)(struct Trainer*, char*, double, double);
};

InfoSet* ctorInfoSet();
void dtorInfoSet(InfoSet*);

void printTreeNode(void*, void*);
int compareTreeNode(void*, void*);

void getStrategy(InfoSet*, double);
void getAvgStrategy(InfoSet*);
char* toString(char*, InfoSet*);

void train(Trainer*);
double cfr(Trainer*, char*, double, double);

#endif // KUHN_H_