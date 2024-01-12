#include "./colonelblotto.h"

size_t actions[NUM_ACTIONS][BATTLEFIELDS] = {{5, 0, 0}, {0, 5, 0}, {0, 0, 5},
                                             {4, 1, 0}, {4, 0, 1}, {1, 4, 0}, {1, 0, 4}, {0, 4, 1}, {0, 1, 4},
                                             {3, 2, 0}, {3, 0, 2}, {2, 3, 0}, {2, 0, 3}, {0, 3, 2}, {0, 2, 3},
                                             {3, 1, 1}, {1, 3, 1}, {1, 1, 3},
                                             {2, 2, 1}, {2, 1, 2}, {1, 2, 2}};

double checkActions(size_t* act1, size_t* act2) {
    int sum = 0;
    for (size_t i = 0; i < BATTLEFIELDS; i++) {
        if (act1[i] > act2[i]) sum += 1.0;
        if (act1[i] < act2[i]) sum -= 1.0;
    }
    if (sum > 1.0) return 1.0;
    if (sum < -1.0) return -1.0;
    return sum;
}

void getStrategy(Player* p) {
    double norm = 0.0;
    for (size_t act = 0; act < NUM_ACTIONS; act++) {
        p->strategy[act] = p->regret_sum[act] > 0.0 ? p->regret_sum[act] : 0.0;
        norm += p->strategy[act];
    }
    for (size_t act = 0; act < NUM_ACTIONS; act++) {
        p->strategy[act] = norm > 0.0 ? p->strategy[act] / norm : 1.0 / NUM_ACTIONS;
        p->strategy_sum[act] += p->strategy[act];
    }
}

void getAvgStrategy(Player* p) {
    double norm = 0.0;
    for (size_t act = 0; act < NUM_ACTIONS; act++) {
        norm += p->strategy_sum[act];
    }
    for (size_t act = 0; act < NUM_ACTIONS; act++) {
        p->strategy_avg[act] = norm > 0.0 ? p->strategy_sum[act] / norm : 1.0 / NUM_ACTIONS;
    }
}

void getAction(Player* p) {
    double r = ((double)rand()) / RAND_MAX;
    double cum_sum = 0.0;
    size_t act = 0;
    while (act < NUM_ACTIONS - 1)
    {
        cum_sum += p->strategy[act];
        if (r < cum_sum) {
            p->action = act;
            return;
        }
        act++;
    }
    p->action = act;
}

void train_impl(Trainer* t) {
    double* action_utility = calloc(NUM_ACTIONS, sizeof(double));
    for (size_t it = 0; it < t->it; it++) {
        getStrategy(t->p1);
        getAction(t->p1);
        getStrategy(t->p2);
        getAction(t->p2);
        for (size_t act = 0; act < NUM_ACTIONS; act++) {
            action_utility[act] = checkActions(actions[act], actions[t->p2->action]);
        }
        for (size_t act = 0; act < NUM_ACTIONS; act++)
            t->p1->regret_sum[act] += action_utility[act] - action_utility[t->p1->action];
        for (size_t act = 0; act < NUM_ACTIONS; act++) {
            action_utility[act] = checkActions(actions[act], actions[t->p1->action]);
        }
        for (size_t act = 0; act < NUM_ACTIONS; act++)
            t->p2->regret_sum[act] += action_utility[act] - action_utility[t->p2->action];
    }
}

int main(int argc, char** argv) {

    unsigned int seed;
    FILE* urandom = fopen("/dev/urandom", "r");
    fread(&seed, sizeof(int), 1, urandom);
    fclose(urandom);
    
    srand(seed);

    Player* p1 = malloc(sizeof(Player));
    p1->strategy = calloc(NUM_ACTIONS, sizeof(double));
    p1->strategy_sum = calloc(NUM_ACTIONS, sizeof(double));
    p1->strategy_avg = calloc(NUM_ACTIONS, sizeof(double));
    p1->regret_sum = calloc(NUM_ACTIONS, sizeof(double));
    p1->action = 0;

    Player* p2 = malloc(sizeof(Player));
    p2->strategy = calloc(NUM_ACTIONS, sizeof(double));
    p2->strategy_sum = calloc(NUM_ACTIONS, sizeof(double));
    p2->strategy_avg = calloc(NUM_ACTIONS, sizeof(double));
    p2->regret_sum = calloc(NUM_ACTIONS, sizeof(double));
    p2->action = 0;

    Trainer* trainer = malloc(sizeof(Trainer));
    trainer->it = 1000000;
    trainer->p1 = p1;
    trainer->p2 = p2;
    trainer->train = &train_impl;

    trainer->train(trainer);
    getAvgStrategy(trainer->p1);
    getAvgStrategy(trainer->p2);

    for (size_t i = 0; i < NUM_ACTIONS; i++)
        printf("0: %f --- 1: %f\n", trainer->p1->strategy_avg[i], trainer->p2->strategy_avg[i]);

    return 0;
}