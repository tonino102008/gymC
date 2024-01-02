#define GYMC_IMPLEMENTATION

#include "./gymC.h"
#include "./holdem.h"

void start_game_impl(void* game) {
    ((Game*)game)->deck = malloc(sizeof(Deck));
    ((Game*)game)->deck->deck_make = &deck_make_impl;
    ((Game*)game)->deck->deck_make((void*)(((Game*)game)->deck));
    ((Game*)game)->num_players = NO_PLAYERS;
    ((Game*)game)->players = malloc(sizeof(Player) * ((Game*)game)->num_players);
    FOR_ARRAY(((Game*)game)->num_players, {
        ((Game*)game)->players[i].cards = malloc(sizeof(Card) * NO_CARDS_HAND);
        ((Game*)game)->players[i].fiches = NO_FICHES;
        ((Game*)game)->players[i].is_bblind = 0;
        ((Game*)game)->players[i].is_sblind = 0;
    });
    ((Game*)game)->table = malloc(sizeof(Table));
    ((Game*)game)->table->cards = malloc(sizeof(Card) * NO_CARDS_TABLE);
    ((Game*)game)->table->plate = 0;
    ((Game*)game)->table->bblind = 0;
    ((Game*)game)->table->sblind = 0;
    ((Game*)game)->table->dealer = rand() % NO_PLAYERS;
    ((Game*)game)->reset_hand = &reset_hand_impl;
    ((Game*)game)->reset_hand(game);
}

void reset_hand_impl(void* game) {
    ((Game*)game)->phase = PRE_FLOP;
    ((Game*)game)->move_blinds = &move_blinds_impl;
    ((Game*)game)->move_blinds(game);
    ((Game*)game)->table->plate += (((Game*)game)->table->sblind + ((Game*)game)->table->bblind);
}

void move_blinds_impl(void* game) {
    ((Game*)game)->table->dealer = (((Game*)game)->table->dealer + 1) % NO_PLAYERS;
    ((Game*)game)->table->sblind = NO_SBLIND;
    ((Game*)game)->table->bblind = NO_BBLIND;
    if (NO_PLAYERS == 2) {
        ((Game*)game)->players[((Game*)game)->table->dealer].is_sblind = 1;
        ((Game*)game)->players[((Game*)game)->table->dealer].fiches -= ((Game*)game)->table->sblind;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].is_bblind = 1;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].fiches -= ((Game*)game)->table->bblind;
    } else {
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].is_sblind = 1;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].fiches -= ((Game*)game)->table->sblind;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 2) % NO_PLAYERS].is_bblind = 1;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 2) % NO_PLAYERS].fiches -= ((Game*)game)->table->bblind;
    }
}

void deal_cards_impl(void* game) {
    ((Game*)game)->deck->deck_shuffle = &deck_shuffle_impl;
    ((Game*)game)->deck->deck_shuffle((void*)(((Game*)game)->deck));
    FOR_ARRAY(((Game*)game)->num_players, {
        ((Game*)game)->deck->dim  -= NO_CARDS_HAND;
        memcpy(((Game*)game)->players[i].cards, &((Game*)game)->deck->cards[((Game*)game)->deck->dim], 
                NO_CARDS_HAND * sizeof(Card));
        ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
                (((Game*)game)->deck->dim + 1) * sizeof(Card));
    });
}

void check_point_impl(void* game) {

}

void bet_round_impl(void* game) {

}

void collect_cards_impl(void* game) {
    FOR_ARRAY(((Game*)game)->num_players, {
        ((Game*)game)->deck->dim  += NO_CARDS_HAND;
        ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
                (((Game*)game)->deck->dim + 1) * sizeof(Card));
        memcpy(&((Game*)game)->deck->cards[((Game*)game)->deck->dim - NO_CARDS_HAND],
                ((Game*)game)->players[i].cards, 
                NO_CARDS_HAND * sizeof(Card));
    });
    if (((Game*)game)->phase > PRE_FLOP) {
        ((Game*)game)->deck->dim  += NO_CARDS_FLOP;
        ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
                (((Game*)game)->deck->dim + 1) * sizeof(Card));
        memcpy(&((Game*)game)->deck->cards[((Game*)game)->deck->dim - NO_CARDS_FLOP],
                ((Game*)game)->table->cards, 
                NO_CARDS_FLOP * sizeof(Card));
    }
    if (((Game*)game)->phase > FLOP) {
        ((Game*)game)->deck->dim  += NO_CARDS_TURN;
        ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
                (((Game*)game)->deck->dim + 1) * sizeof(Card));
        memcpy(&((Game*)game)->deck->cards[((Game*)game)->deck->dim - NO_CARDS_TURN], 
                &((Game*)game)->table->cards[NO_CARDS_FLOP], 
                NO_CARDS_TURN * sizeof(Card));
    }
    if (((Game*)game)->phase > TURN) {
        ((Game*)game)->deck->dim  += NO_CARDS_RIVER;
        ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
                (((Game*)game)->deck->dim + 1) * sizeof(Card));
        memcpy(&((Game*)game)->deck->cards[((Game*)game)->deck->dim - NO_CARDS_RIVER], 
                &((Game*)game)->table->cards[NO_CARDS_FLOP + NO_CARDS_TURN], 
                NO_CARDS_RIVER * sizeof(Card));
    }
}

void flop_impl(void* game) {
    ((Game*)game)->phase = FLOP;
    ((Game*)game)->deck->dim  -= NO_CARDS_FLOP;
    memcpy(((Game*)game)->table->cards, &((Game*)game)->deck->cards[((Game*)game)->deck->dim], 
            NO_CARDS_FLOP * sizeof(Card));
    ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
            (((Game*)game)->deck->dim + 1) * sizeof(Card));
}

void turn_impl(void* game) {
    ((Game*)game)->phase = TURN;
    ((Game*)game)->deck->dim  -= NO_CARDS_TURN;
    memcpy(&((Game*)game)->table->cards[NO_CARDS_FLOP], &((Game*)game)->deck->cards[((Game*)game)->deck->dim], 
            NO_CARDS_TURN * sizeof(Card));
    ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
            (((Game*)game)->deck->dim + 1) * sizeof(Card));
}

void river_impl(void* game) {
    ((Game*)game)->phase = RIVER;
    ((Game*)game)->deck->dim  -= NO_CARDS_RIVER;
    memcpy(&((Game*)game)->table->cards[NO_CARDS_FLOP + NO_CARDS_TURN], &((Game*)game)->deck->cards[((Game*)game)->deck->dim], 
            NO_CARDS_RIVER * sizeof(Card));
    ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
            (((Game*)game)->deck->dim + 1) * sizeof(Card));
}

int main(int argc, char** argv) {

    Game* holdem = malloc(sizeof(Game));

    holdem->start_game = &start_game_impl;
    holdem->start_game((void*)holdem);

    holdem->deal_cards = &deal_cards_impl;
    holdem->deal_cards((void*)holdem);

    FOR_ARRAY(holdem->deck->dim, {
        INFO("Shuffled Card: %s %d", print_card_impl(&(holdem->deck->cards[i])), i);
    });

    FOR_ARRAY(holdem->num_players, {
        INFO("--------------------");
        INFO("Player %d: %s %s", i, print_card_impl(&(holdem->players[i].cards[0])), 
                print_card_impl(&(holdem->players[i].cards[1])));
        INFO("Player %d: FICHES %d, SB %d, BB %d", i, holdem->players[i].fiches,
                holdem->players[i].is_sblind, holdem->players[i].is_bblind);
    });

    holdem->bet_round = &bet_round_impl;
    holdem->bet_round((void*)holdem);

    holdem->flop = &flop_impl;
    holdem->flop((void*)holdem);

    INFO("--------------------");
    INFO("Table: %s %s %s", print_card_impl(&(holdem->table->cards[0])), 
                print_card_impl(&(holdem->table->cards[1])),
                print_card_impl(&(holdem->table->cards[2])));
    FOR_ARRAY(holdem->num_players, {
        INFO("--------------------");
        INFO("Player %d: %s %s", i, print_card_impl(&(holdem->players[i].cards[0])), 
                print_card_impl(&(holdem->players[i].cards[1])));
        INFO("Player %d: FICHES %d, SB %d, BB %d", i, holdem->players[i].fiches,
                holdem->players[i].is_sblind, holdem->players[i].is_bblind);
    });

    holdem->bet_round = &bet_round_impl;
    holdem->bet_round((void*)holdem);

    holdem->turn = &turn_impl;
    holdem->turn((void*)holdem);

    INFO("--------------------");
    INFO("Table: %s %s %s %s", print_card_impl(&(holdem->table->cards[0])), 
                print_card_impl(&(holdem->table->cards[1])),
                print_card_impl(&(holdem->table->cards[2])),
                print_card_impl(&(holdem->table->cards[3])));
    FOR_ARRAY(holdem->num_players, {
        INFO("--------------------");
        INFO("Player %d: %s %s", i, print_card_impl(&(holdem->players[i].cards[0])), 
                print_card_impl(&(holdem->players[i].cards[1])));
        INFO("Player %d: FICHES %d, SB %d, BB %d", i, holdem->players[i].fiches,
                holdem->players[i].is_sblind, holdem->players[i].is_bblind);
    });

    holdem->bet_round = &bet_round_impl;
    holdem->bet_round((void*)holdem);

    holdem->river = &river_impl;
    holdem->river((void*)holdem);

    INFO("--------------------");
    INFO("Table: %s %s %s %s %s", print_card_impl(&(holdem->table->cards[0])), 
                print_card_impl(&(holdem->table->cards[1])),
                print_card_impl(&(holdem->table->cards[2])),
                print_card_impl(&(holdem->table->cards[3])),
                print_card_impl(&(holdem->table->cards[4])));
    FOR_ARRAY(holdem->num_players, {
        INFO("--------------------");
        INFO("Player %d: %s %s", i, print_card_impl(&(holdem->players[i].cards[0])), 
                print_card_impl(&(holdem->players[i].cards[1])));
        INFO("Player %d: FICHES %d, SB %d, BB %d", i, holdem->players[i].fiches,
                holdem->players[i].is_sblind, holdem->players[i].is_bblind);
    });

    holdem->bet_round = &bet_round_impl;
    holdem->bet_round((void*)holdem);

    holdem->check_points = &check_point_impl;
    holdem->check_points((void*)holdem);

    holdem->collect_cards = &collect_cards_impl;
    holdem->collect_cards((void*)holdem);

    FOR_ARRAY(holdem->deck->dim, {
        INFO("Shuffled Card: %s %d", print_card_impl(&(holdem->deck->cards[i])), i);
    });
    return 0;
}