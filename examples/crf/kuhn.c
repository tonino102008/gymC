#define TREEMAP_IMPLEMENTATION
#include "./kuhn.h"
#include "../../treemap.h"

void printTreeNode(void* key, void* value) {
    assert(key);
    assert(value);
    printf("Key %s - Value: %d\n", (char*)key, toString((InfoSet*)value));
}

int compareTreeNode(void* key1, void* key2) {
    assert(key1);
    assert(key2);
    return strcmp((char*)key1, (char*)key2);
}

void getStrategy(InfoSet* info) {
    double norm = 0.0;
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        info->strategy[i] = info->regret_sum[i] > 0.0 ? info->regret_sum[i] : 0.0;
        norm += info->strategy[i];
    }
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        info->strategy[i] = norm > 0.0 ? info->strategy[i] / norm : 1.0 / NUM_ACTIONS;
        info->strategy_sum[i] += info->strategy[i];
    }
}

void getAvgStrategy(InfoSet* info) {
    double norm = 0.0;
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        norm += info->strategy[i];
    }
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        info->strategy_avg[i] = norm > 0.0 ? info->strategy_sum[i] / norm : 1.0 / NUM_ACTIONS;
    }
}

char* toString(InfoSet* info) {
    size_t len = strlen(info->infoset) + 1;
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        char tmp[100];
        sprintf(tmp, " %d", info->strategy_avg[i]);
        len += strlen(tmp);
    }
    char* out = malloc(len * sizeof(char));
    len = 0;
    sprintf(out, "%s", info->infoset);
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        len += strlen(out);
        sprintf(&out[len], " %d", info->strategy_avg[i]);
    }
    return out;
}

double cfr(Trainer* t, double p0, double p1) {
    size_t player = t->lenH % NUM_PLAYERS;
    size_t opponent = 1 - player;
    double out = 0.0;
    
    if (t->lenH > 1) {
        ssize_t terminal_pass = t->history[t->lenH - 1] == 'p';
        ssize_t double_bet = strncmp(t->history, "bb", 2) == 0;
        ssize_t is_higher = t->cards[player] > t->cards[opponent];
        if (terminal_pass) {
            if (strncmp(t->history, "pp", 2) == 0) return is_higher ? 1.0 : -1.0;
            return 1;
        } else if (double_bet) {
            return is_higher ? 2.0 : -2.0;
        }
    }

    char* infoset = malloc((1 + t->lenH) * sizeof(char));
    infoset[0] = t->cards[player] - '0';
    strcat(infoset, t->history);

    

    return out;
}

void train(Trainer* t) {
    double util = 0.0;
    for (size_t i = 0; i < t->it; i++) {
        t->lenH = 1;
        t->history = realloc(t->lenH, sizeof(char));
        t->history[0] = '\0';
        for (size_t j = 0; j < NUM_CARDS; j++) {
            int tmp = t->cards[i];
            int r = rand() % NUM_CARDS;
            t->cards[i] = t->cards[r];
            t->cards[r] = tmp;
        }
        // util += t->cfr(t, 1.0, 1.0); // TO BE ADDRESSED
    }
    printf("Average Game Value: %f\n", util / t->it);
    // travelTreeMap(); // TO BE ADDRESSED
}

int main(int argc, char** argv) {

    unsigned int seed;
    FILE* urandom = fopen("/dev/urandom", "r");
    fread(&seed, sizeof(int), 1, urandom);
    fclose(urandom);

    srand(seed);

    Trainer* trainer = malloc(sizeof(Trainer));
    trainer->cards = malloc(NUM_CARDS * sizeof(int));
    for (int i = 0; i < NUM_CARDS; i++) trainer->cards[i] = i;
    trainer->train = &train;
    trainer->cfr = &cfr;
    trainer->lenH = 1;
    trainer->history = malloc(trainer->lenH * sizeof(char));
    trainer->history[0] = '\0';

    return 0;
}