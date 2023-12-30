#define GYMC_IMPLEMENTATION
#include "./gymC.h"

void set_space(void* str, size_t dim, void* array) {
    ((Space*)str)->dim = dim;
    ((Space*)str)->array = array;
}

void set_reward(void* str, double reward) {
    ((Reward*)str)->reward = reward;
}

void set_env(void* str, Space* obs, Reward* rew, int flag) {
    ((Environment*)str)->obs_s = obs;
    ((Environment*)str)->rew_r = rew;
    ((Environment*)str)->is_end = flag;
}

int main(int argc, const char** argv) {

    size_t dim_a = 10;
    int* a = malloc(dim_a * sizeof(int));
    FOR_ARRAY(dim_a, {
        a[i] = i;
    });

    Space* input = malloc(sizeof(Space));
    INIT_SPACE(input, dim_a, a, set_space);
    Space* output = malloc(sizeof(Space));
    INIT_SPACE(output, dim_a, a, set_space);
    Space* obs = malloc(sizeof(Space));
    INIT_SPACE(obs, dim_a, a, set_space);

    Reward* rew = malloc(sizeof(Reward));
    INIT_REWARD(rew, set_reward);

    Environment* env = malloc(sizeof(Environment));
    INIT_ENV(env, obs, rew, set_env);

    FOR_ARRAY(input->dim, {
        INFO("Space Value: %d", ((int*)input->array)[i]);
    });

    INFO("Reward Value: %f", rew->reward);

    FOR_ARRAY(env->obs_s->dim, {
        INFO("Observation Value: %d", ((int*)env->obs_s->array)[i]);
    });

    return 0;
}