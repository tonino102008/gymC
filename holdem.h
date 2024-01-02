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

#define NO_PLAYERS 2

typedef enum {
    PRE_FLOP,
    FLOP,
    TURN,
    RIVER,
} GamePhase;

typedef enum {
    FOLD,
    CHECK,
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
    Card* cards;
    size_t fiches;
    size_t is_bblind;
    size_t is_sblind;
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

#endif // HOLDEM_H_