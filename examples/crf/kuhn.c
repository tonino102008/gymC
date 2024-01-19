#define TREEMAP_IMPLEMENTATION
#include "./kuhn.h"

TreeNode* ctorTreeNode(void* key, void* value) {
    TreeNode* out = malloc(sizeof(TreeNode));
    out->key = malloc((1 + strlen((char*)key)) * sizeof(char));
    strcpy((char*)(out->key), (char*)key);
    if (!value) {
        out->value = (void*)ctorInfoSet();
    } else {
        out->value = value;
    }
    out->right = NULL;
    out->left = NULL;
    return out;
}

void dtorTreeNode(TreeNode* node) {
    free(node->key); // HERE IS CHAR* - NO CUSTOM DTOR NEEDED
    dtorInfoSet((InfoSet*)node->value); // CALL INFOSET DTOR
    free(node->value);
    free(node);
}

InfoSet* ctorInfoSet() {
    InfoSet* out = malloc(sizeof(InfoSet));
    out->strategy = calloc(NUM_ACTIONS, sizeof(double));
    out->strategy_sum = calloc(NUM_ACTIONS, sizeof(double));
    out->strategy_avg = calloc(NUM_ACTIONS, sizeof(double));
    out->regret_sum = calloc(NUM_ACTIONS, sizeof(double));
    out->_getStrategy = &getStrategy;
    out->_getAvgStrategy = &getAvgStrategy;
}

void dtorInfoSet(InfoSet* info) {
    free(info->strategy);
    free(info->strategy_avg);
    free(info->strategy_sum);
    free(info->regret_sum);
}

void printTreeNode(void* key, void* value) {
    assert(key);
    assert(value);
    printf("Key %s - Value: %s\n", (char*)key, toString((char*)key, (InfoSet*)value));
}

int compareTreeNode(void* key1, void* key2) {
    assert(key1);
    assert(key2);
    return strcmp((char*)key1, (char*)key2);
}

void getStrategy(InfoSet* info, double real_weight) {
    double norm = 0.0;
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        info->strategy[i] = info->regret_sum[i] > 0.0 ? info->regret_sum[i] : 0.0;
        norm += info->strategy[i];
    }
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        info->strategy[i] = norm > 0.0 ? info->strategy[i] / norm : 1.0 / NUM_ACTIONS;
        info->strategy_sum[i] += real_weight * info->strategy[i];
    }
}

void getAvgStrategy(InfoSet* info) {
    double norm = 0.0;
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        norm += info->strategy_sum[i];
    }
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        info->strategy_avg[i] = norm > 0.0 ? info->strategy_sum[i] / norm : 1.0 / NUM_ACTIONS;
    }
}

char* toString(char* infoset, InfoSet* info) {
    info->_getAvgStrategy(info);
    size_t len = strlen(infoset) + 1;
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        char tmp[100];
        sprintf(tmp, " %f", info->strategy_avg[i]);
        len += strlen(tmp);
    }
    char* out = malloc(len * sizeof(char));
    len = 0;
    sprintf(out, "%s", infoset);
    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        len += strlen(out);
        sprintf(&out[len], " %f", info->strategy_avg[i]);
    }
    return out;
}

double cfr(Trainer* t, char* history, double p0, double p1) {
    size_t player = t->lenH % NUM_PLAYERS;
    size_t opponent = 1 - player;
    
    if (strlen(history) > 1) {
        ssize_t terminal_pass = history[strlen(history) - 1] == 'p';
        ssize_t double_bet = strcmp(&history[strlen(history) - 2], "bb") == 0;
        ssize_t is_higher = t->cards[player] > t->cards[opponent];
        // printf("%s %zd %zd %zd\n", t->history, terminal_pass, double_bet, is_higher);
        // getchar();
        if (terminal_pass) {
            if (strcmp(history, "pp") == 0) return is_higher ? 1.0 : -1.0;
            return 1.0;
        } else if (double_bet) {
            return is_higher ? 2.0 : -2.0;
        }
    }

    char* infoset = malloc((strlen(history) + 1) * sizeof(char));
    infoset[0] = t->cards[player] + '0';
    if (history[0] != '\0') strcpy(&infoset[1], history);
    else infoset[1] = '\0';

    TreeNode* nodeF = findTreeNode(t->tree->head, (void*)infoset, &compareTreeNode);
    if (!nodeF) {
        nodeF = ctorTreeNode((void*)infoset, (void*)NULL);
        insertTreeNode(t->tree, nodeF, &compareTreeNode);
    }

    //printf("%s %s\n", history, infoset);
    //if (t->tree->head) travelTreeMap(t->tree->head, &printTreeNode);

    ((InfoSet*)nodeF->value)->_getStrategy((InfoSet*)nodeF->value, player == 0 ? p0 : p1);
    double* util = calloc(NUM_ACTIONS, sizeof(double));
    double nodeUtil = 0.0;

    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        size_t len;
        if (history[0] != '\0') len = strlen(history) + 1;
        else len = 1;
        char* nextHistory = malloc(len * sizeof(char));
        if (history[0] != '\0') strcpy(&nextHistory[0], history);
        nextHistory[len - 1] = i == 0 ? 'p' : 'b';
        nextHistory[len] = '\0';
        //printf("%s %zu\n", nextHistory, strlen(nextHistory));
        //printf("I: %zu\n", i);
        util[i] = player == 0 ? 
                -t->_cfr(t, nextHistory, p0 * ((InfoSet*)nodeF->value)->strategy[i], p1) :
                -t->_cfr(t, nextHistory, p0, p1 * ((InfoSet*)nodeF->value)->strategy[i]);
        nodeUtil += ((InfoSet*)nodeF->value)->strategy[i] * util[i];
    }

    for (size_t i = 0; i < NUM_ACTIONS; i++) {
        double regret = util[i] - nodeUtil;
        ((InfoSet*)nodeF->value)->regret_sum[i] += (player == 0 ? p1 : p0) * regret;
    }

    return nodeUtil;
}

void train(Trainer* t) {
    double util = 0.0;
    for (size_t i = 0; i < t->it; i++) {
        t->lenH = 1;
        t->history = realloc(t->history, t->lenH * sizeof(char));
        t->history[t->lenH - 1] = '\0';
        for (size_t j = 0; j < NUM_CARDS; j++) {
            int tmp = t->cards[j];
            int r = rand() % NUM_CARDS;
            t->cards[j] = t->cards[r];
            t->cards[r] = tmp;
        }
        char* history = malloc(1 * sizeof(char));
        history = "\0";
        util += t->_cfr(t, history, 1.0, 1.0); // TO BE ADDRESSED
    }
    printf("Average Game Value: %f\n", util / t->it);
    if (t->tree->head) travelTreeMap(t->tree->head, &printTreeNode);
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
    trainer->it = 500000;
    trainer->_train = &train;
    trainer->_cfr = &cfr;
    trainer->lenH = 1;
    trainer->history = malloc(trainer->lenH * sizeof(char));
    trainer->history[0] = '\0';
    trainer->tree = createEmptyTreeMap();

    printf("START\n");
    trainer->_train(trainer);
    printf("END\n");

    return 0;
}