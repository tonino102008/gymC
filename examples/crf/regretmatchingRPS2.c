#include "./regretmatchingRPS.h"

char* RPS_char[] = {"ROCK", "PAPER", "SCISSORS"};

void getStrategy(double* strategy_sum, double* regret_sum, double* strategy) {
    double norm = 0.0;
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        strategy[i] = regret_sum[i] > 0.0 ? regret_sum[i] : 0.0;
        norm += strategy[i];
    }
    //printf("NORM: %f\n\n", norm);
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        strategy[i] = norm > 0.0 ? strategy[i] / norm : 1.0 / NUM_ACTIONS;
        strategy_sum[i] += strategy[i];
    }
}

int getAction(double* strategy) {
    double r = ((double)rand()) / RAND_MAX;
    double cum_prob = 0.0;
    size_t act = ROCK;
    while (act < NUM_ACTIONS - 1) {
        cum_prob += strategy[act];
        if (r < cum_prob) return act;
        act += 1;
    }   
    return act;
}

void train2_impl(Trainer2* trainer, size_t num) {
    double* action_utility = malloc(NUM_ACTIONS * sizeof(double));
    for (size_t i = 0; i < num; i++) {
        getStrategy(trainer->strategy_sum, trainer->regret_sum, trainer->strategy);
        getStrategy(trainer->opponent_strategy_sum, trainer->opponent_regret_sum, trainer->opponent_strategy);
        int my_act = getAction(trainer->strategy);
        int opp_act = getAction(trainer->opponent_strategy);
        // printf("ACTION: %d %d\n", my_act, opp_act);
        // for (size_t i = 0; i < NUM_ACTIONS; i++) {
        //     printf("%f %f %f\n", trainer->regret_sum[i], trainer->strategy_sum[i], trainer->strategy[i]);
        // }
        // printf("\n");
        action_utility[opp_act] = 0;
        action_utility[opp_act == NUM_ACTIONS - 1 ? 0 : opp_act + 1] = 1.0;
        action_utility[opp_act == 0 ? NUM_ACTIONS - 1 : opp_act - 1] = -1.0;
        for (size_t i = 0; i < NUM_ACTIONS; i++) {
            trainer->regret_sum[i] += action_utility[i] - action_utility[my_act];
        }
        action_utility[my_act] = 0;
        action_utility[my_act == NUM_ACTIONS - 1 ? 0 : my_act + 1] = 1.0;
        action_utility[my_act == 0 ? NUM_ACTIONS - 1 : my_act - 1] = -1.0;
        for (size_t i = 0; i < NUM_ACTIONS; i++) {
            trainer->opponent_regret_sum[i] += action_utility[i] - action_utility[opp_act];
        }
    }
}

double* getAverageStrategy(double* strategy_sum) {
    double* strategy_avg = malloc(NUM_ACTIONS * sizeof(double));
    double norm = 0;
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        norm += strategy_sum[i];
    }
    //printf("NORM AVG: %f\n\n", norm);
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        strategy_avg[i] = norm > 0.0 ? strategy_sum[i] / norm : 1.0 / NUM_ACTIONS;
    }
    return strategy_avg;
}

int main(int argc, char** argv) {

    unsigned int seed;
    FILE* urandom = fopen("/dev/urandom", "r");
    fread(&seed, sizeof(int), 1, urandom);
    fclose(urandom);
    srand(seed);

    Trainer2 t = {
        .opponent_regret_sum = calloc(NUM_ACTIONS, sizeof(double)),
        .opponent_strategy = calloc(NUM_ACTIONS, sizeof(double)),
        .opponent_strategy_sum = calloc(NUM_ACTIONS, sizeof(double)),
        .regret_sum = calloc(NUM_ACTIONS, sizeof(double)),
        .strategy = calloc(NUM_ACTIONS, sizeof(double)),
        .strategy_sum = calloc(NUM_ACTIONS, sizeof(double)),
        .train = &train2_impl
    };

    // for (size_t i = 0; i < NUM_ACTIONS; i++) {
    //     printf("%f %f %f\n", t.regret_sum[i], t.strategy_sum[i], t.strategy[i]);
    // }
    // printf("\n");
    
    t.train(&t, 1000000);

    double* out = getAverageStrategy(t.strategy_sum);
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        printf("%s: %f\n", RPS_char[i], out[i]);
    }
    printf("\n");
    double* out2 = getAverageStrategy(t.opponent_strategy_sum);
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        printf("%s: %f\n", RPS_char[i], out2[i]);
    }

    return 0;
}