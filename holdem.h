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

#define SET_POINT(test, player, point_val)  \
        ({if (test) {                       \
            player.point = point_val;       \
            free(test);                     \
            continue;                       \
        }})

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
    Card* cards;
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

Card* check_flush_impl(Card* cards, size_t no_cards);
Card* ret_flush_impl(Card* cards, size_t no_cards, Suit suit);
Card* check_straight_impl(Card* cards, size_t no_cards);
Card* ret_straight_impl(Card* cards, size_t no_cards, size_t idx);
Card* check_poker_impl(Card* cards, size_t no_cards);
Card* ret_poker_impl(Card* cards, size_t no_cards, size_t idx);
Card* check_tris_impl(Card* cards, size_t no_cards);
Card* ret_tris_impl(Card* cards, size_t no_cards, size_t idx);
Card* check_pair_impl(Card* cards, size_t no_cards);
Card* ret_pair_impl(Card* cards, size_t no_cards, size_t idx);
Card* check_high_card_impl(Card* cards, size_t no_cards);

#endif // HOLDEM_H_