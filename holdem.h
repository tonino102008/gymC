#ifndef HOLDEM_H_
#define HOLDEM_H_

#include "./cards.h"

#define NO_CARDS_HAND 2
#define NO_CARDS_FLOP 3
#define NO_CARDS_TURN 1
#define NO_CARDS_RIVER 1
#define NO_CARDS_TABLE 5

#define NO_FICHES 1500
#define NO_BBLIND 20
#define NO_SBLIND 10

#define NO_PLAYERS 9

#define SET_PHASE(phase, num)                           \
        ({switch (phase) {                              \
            case PRE_FLOP:                              \
                num = 0;                                \
                break;                                  \
                                                        \
            case FLOP:                                  \
                num = NO_CARDS_FLOP;                    \
                break;                                  \
                                                        \
            case TURN:                                  \
                num = NO_CARDS_FLOP + NO_CARDS_TURN;    \
                break;                                  \
                                                        \
            case RIVER:                                 \
                num = NO_CARDS_TABLE;                   \
                break;                                  \
            }})

#define FOR_PLAYER_ARRAY(dim, dealer, body)                 \
        do {                                                \
            for (size_t l = 0; l < dim; l++) {              \
                size_t i = (dealer + l + 1) % (NO_PLAYERS); \
                body;                                       \
            }                                               \
        } while(0)

typedef enum {
    PRE_FLOP,
    FLOP,
    TURN,
    RIVER,
} GamePhase;

typedef enum {
    FOLD_CHECK,
    CALL,
    RAISE,
} Bet;

typedef enum {
    HIGH_CARD,
    ONE_PAIR,
    TWO_PAIR,
    TRIS,
    SIMPLE_STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    POKER,
    FLUSH_STRAIGHT,
    ROYAL_STRAIGHT,
} Point;

typedef struct {
    size_t* idx;
    size_t dim;
    Value value;
} Counter;

typedef struct {
    Card* cards;
    Card* best_hand;
    size_t fiches;
    size_t is_bblind;
    size_t is_sblind;
    size_t cur_bet;
    Bet bet;
    Point point;
} Player;

typedef struct {
    Card* cards;
    size_t plate;
    size_t bblind;
    size_t sblind;
    size_t dealer;
} Table;

typedef struct {
    size_t num_players;
    size_t* act_players;
    Deck* deck;
    Player* players;
    GamePhase phase;
    Table* table;
    void (*start_game)(void*);
    void (*reset_hand)(void*);
    void (*move_blinds)(void*);
    void (*deal_cards)(void*);
    void (*check_points)(void*);
    void (*bet_round)(void*);
    void (*flop)(void*);
    void (*turn)(void*);
    void (*river)(void*);
    void (*collect_cards)(void*);
} Game;

int sort_counter_impl(const void* ptr1, const void* ptr2);
int sort_counter_by_value_impl(const void* ptr1, const void* ptr2);

void start_game_impl(void* game);
void reset_hand_impl(void* game);
void move_blinds_impl(void* game);
void deal_cards_impl(void* game);
void flop_impl(void* game);
void turn_impl(void* game);
void river_impl(void* game);
void check_point_impl(void* game);
void bet_round_impl(void* game);
void collect_cards_impl(void* game);

size_t check_flush_straight_impl(Counter* count, Card* all, Card* straight);
size_t check_straight_impl(Counter* count, Card* all, Card* straight);

Card* check_counter_impl(Player* pl, Counter* count, Counter* count_s, Card* all);

size_t compare_best_hands_impl(Card* cur_best, Card* new);

void print_table_impl(void* game);

#endif // HOLDEM_H_