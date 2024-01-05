#define GYMC_IMPLEMENTATION

#include "./gymC.h"
#include "./holdem.h"

void start_game_impl(void* game) {
    ((Game*)game)->deck = malloc(sizeof(Deck));
    ((Game*)game)->deck->deck_make = &deck_make_impl;
    ((Game*)game)->deck->deck_make((void*)(((Game*)game)->deck));
    ((Game*)game)->num_players = NO_PLAYERS;
    ((Game*)game)->act_players = malloc(NO_PLAYERS * sizeof(size_t));
    ((Game*)game)->players = malloc(sizeof(Player) * ((Game*)game)->num_players);
    FOR_ARRAY(((Game*)game)->num_players, {
        ((Game*)game)->players[i].cards = malloc(sizeof(Card) * NO_CARDS_HAND);
        ((Game*)game)->players[i].fiches = NO_FICHES;
        ((Game*)game)->players[i].is_bblind = 0;
        ((Game*)game)->players[i].is_sblind = 0;
        ((Game*)game)->players[i].cur_bet = 0;
        ((Game*)game)->players[i].bet = FOLD_CHECK;
        ((Game*)game)->players[i].point = HIGH_CARD;
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
    FOR_ARRAY(((Game*)game)->num_players, {
        ((Game*)game)->act_players[i] = i;
    });
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
        ((Game*)game)->players[((Game*)game)->table->dealer].cur_bet = ((Game*)game)->table->sblind;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].is_bblind = 1;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].fiches -= ((Game*)game)->table->bblind;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].cur_bet = ((Game*)game)->table->bblind;
    } else {
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].is_sblind = 1;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].fiches -= ((Game*)game)->table->sblind;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 1) % NO_PLAYERS].cur_bet = ((Game*)game)->table->sblind;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 2) % NO_PLAYERS].is_bblind = 1;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 2) % NO_PLAYERS].fiches -= ((Game*)game)->table->bblind;
        ((Game*)game)->players[(((Game*)game)->table->dealer + 2) % NO_PLAYERS].cur_bet = ((Game*)game)->table->bblind;
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
    ((Game*)game)->bet_round = &bet_round_impl;
    ((Game*)game)->bet_round(game);
}

void check_point_impl(void* game) {
    Point max_point = HIGH_CARD;
    size_t winner = -1;
    Card* all = malloc(sizeof(Card) * (NO_CARDS_TABLE + NO_CARDS_HAND));
    FOR_ARRAY(((Game*)game)->num_players, {
        // THE CYCLE DEPENDS FROM THE DEALER POSITION
        // THE PLAYER BET HAS TO BE SET IN PLAYER STRUCT
        // WE SHOULD CYCLE ONLY ON ACTIVE PLAYERS
        memcpy(all, ((Game*)game)->table->cards, sizeof(Card) * NO_CARDS_TABLE);
        memcpy(&all[NO_CARDS_TABLE], ((Game*)game)->players[i].cards, sizeof(Card) * NO_CARDS_HAND);
        qsort(all, NO_CARDS_TABLE + NO_CARDS_HAND, sizeof(Card), sort_cards_value_impl);

        INFO("--------------------");
        INFO("Sorted Table: %s", print_cards_impl(all, NO_CARDS_TABLE + NO_CARDS_HAND));
        
        Card* test = check_poker_impl(all, NO_CARDS_TABLE + NO_CARDS_HAND);
        SET_POINT(test, ((Game*)game)->players[i], POKER);
        test = check_flush_impl(all, NO_CARDS_TABLE + NO_CARDS_HAND);
        SET_POINT(test, ((Game*)game)->players[i], FLUSH);
        test = check_straight_impl(all, NO_CARDS_TABLE + NO_CARDS_HAND);
        SET_POINT(test, ((Game*)game)->players[i], SIMPLE_STRAIGHT);
        test = check_tris_impl(all, NO_CARDS_TABLE + NO_CARDS_HAND);
        SET_POINT(test, ((Game*)game)->players[i], TRIS);
        test = check_pair_impl(all, NO_CARDS_TABLE + NO_CARDS_HAND);
        SET_POINT(test, ((Game*)game)->players[i], ONE_PAIR);
        test = check_high_card_impl(all, NO_CARDS_TABLE + NO_CARDS_HAND);
        SET_POINT(test, ((Game*)game)->players[i], HIGH_CARD);

    });
    ((Game*)game)->players[winner].fiches += ((Game*)game)->table->plate;
}

void bet_round_impl(void* game) {
    size_t max_cur_bet = ((Game*)game)->table->bblind;
    size_t is_raise = 1;
    while (is_raise) {
        is_raise = 0;
        FOR_ARRAY(((Game*)game)->num_players, {
            // THE CYCLE DEPENDS FROM THE DEALER POSITION
            // THE PLAYER BET HAS TO BE SET IN PLAYER STRUCT
            // WE SHOULD CYCLE ONLY ON ACTIVE PLAYERS

            // ALL THE BETS SHOULD BE COLLECTED IN TABLE->PLATE
            switch (((Game*)game)->players[i].bet)
            {
            case FOLD_CHECK:
                if (((Game*)game)->players[i].cur_bet < ((Game*)game)->table->bblind) {
                    // FOLD - REMOVE PLAYER FROM ACT_PLAYERS
                    break;
                }
                break;
            
            case CALL: // FAKE CALL LOGIC -> TBD IN PLAYER STRUCT
                ((Game*)game)->players[i].cur_bet += (max_cur_bet - ((Game*)game)->players[i].cur_bet);
                break;

            case RAISE:
                is_raise = 1;
                ((Game*)game)->players[i].cur_bet *= 2; // FAKE RAISE LOGIC -> TBD IN PLAYER STRUCT
                max_cur_bet = ((Game*)game)->players[i].cur_bet;
                break;
            }
        });
    }
}

void flop_impl(void* game) {
    ((Game*)game)->phase = FLOP;
    ((Game*)game)->deck->dim  -= NO_CARDS_FLOP;
    memcpy(((Game*)game)->table->cards, &((Game*)game)->deck->cards[((Game*)game)->deck->dim], 
            NO_CARDS_FLOP * sizeof(Card));
    ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
            (((Game*)game)->deck->dim + 1) * sizeof(Card));
    ((Game*)game)->bet_round = &bet_round_impl;
    ((Game*)game)->bet_round(game);
}

void turn_impl(void* game) {
    ((Game*)game)->phase = TURN;
    ((Game*)game)->deck->dim  -= NO_CARDS_TURN;
    memcpy(&((Game*)game)->table->cards[NO_CARDS_FLOP], &((Game*)game)->deck->cards[((Game*)game)->deck->dim], 
            NO_CARDS_TURN * sizeof(Card));
    ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
            (((Game*)game)->deck->dim + 1) * sizeof(Card));
    ((Game*)game)->bet_round = &bet_round_impl;
    ((Game*)game)->bet_round(game);
}

void river_impl(void* game) {
    ((Game*)game)->phase = RIVER;
    ((Game*)game)->deck->dim  -= NO_CARDS_RIVER;
    memcpy(&((Game*)game)->table->cards[NO_CARDS_FLOP + NO_CARDS_TURN], &((Game*)game)->deck->cards[((Game*)game)->deck->dim], 
            NO_CARDS_RIVER * sizeof(Card));
    ((Game*)game)->deck->cards = realloc(((Game*)game)->deck->cards,
            (((Game*)game)->deck->dim + 1) * sizeof(Card));
    ((Game*)game)->bet_round = &bet_round_impl;
    ((Game*)game)->bet_round(game);
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

Card* check_flush_impl(Card* cards, size_t no_cards) {
    size_t no_fl = 0;
    Card* cards_t = malloc(no_cards * sizeof(Card));
    memcpy(cards_t, cards, no_cards * sizeof(Card));
    qsort(cards_t, no_cards, sizeof(Card), sort_cards_suit_impl);
    FOR_ARRAY(no_cards, {
        if ((cards_t[i - 1].suit != cards_t[i].suit) && (i > 0)) no_fl = 0;
        no_fl += 1;
        if (no_fl == 5) return ret_flush_impl(cards_t, no_cards, cards_t[i].suit);
    });
    return NULL;
}

Card* ret_flush_impl(Card* cards, size_t no_cards, Suit suit) {
    Card* out = malloc((NO_CARDS_TABLE + NO_CARDS_HAND) * sizeof(Card));
    qsort(cards, no_cards, sizeof(Card), sort_cards_value_impl);
    size_t j = 0;
    FOR_ARRAY(no_cards, {
        if (cards[i].suit == suit) {
            memcpy(&out[j], &cards[i], sizeof(Card));
            j += 1;
        }
    });
    out = realloc(out, NO_CARDS_TABLE * sizeof(Card));
    INFO("Flush: %s", print_cards_impl(out, NO_CARDS_TABLE));
    return out;
}

Card* check_straight_impl(Card* cards, size_t no_cards) {
    // CHECK STRAIGHT DETECTION
    size_t no_s = 0;
    FOR_ARRAY(no_cards, {
        if ((cards[i - 1].value == cards[i].value) && (i > 0)) continue;
        if (no_s == 4 && cards[0].value == ACE) return ret_straight_impl(cards, no_cards, no_cards - no_s);
        if ((cards[i - 1].value != (cards[i].value + 1)) && (i > 0)) no_s = 0;
        no_s += 1;
        if (no_s == 5) return ret_straight_impl(cards, no_cards, i - no_s);
    });
    if (no_s == 4 && cards[0].value == ACE) return ret_straight_impl(cards, no_cards, no_cards - no_s);
    return NULL;
}

Card* ret_straight_impl(Card* cards, size_t no_cards, size_t idx) {
    // CHECK CORNER CASES 
    Card* out = malloc(NO_CARDS_TABLE * sizeof(Card));
    FOR_ARRAY(NO_CARDS_TABLE, {
        memcpy(&out[i], &cards[(i + idx) % (NO_CARDS_TABLE + NO_CARDS_HAND)], sizeof(Card));
    });
    INFO("Straight: %s", print_cards_impl(out, NO_CARDS_TABLE));
    return out;
}

Card* check_poker_impl(Card* cards, size_t no_cards) {
    FOR_ARRAY(no_cards, {
        if ((cards[i - 1].value == cards[i].value) &&
            (cards[i - 2].value == cards[i - 1].value) &&
            (cards[i - 3].value == cards[i - 2].value) &&
            (i > 2)) return ret_poker_impl(cards, no_cards, i);
    });
    return NULL;
}

Card* ret_poker_impl(Card* cards, size_t no_cards, size_t idx) {
    Card* out = malloc(NO_CARDS_TABLE * sizeof(Card));
    size_t j = 0;
    memcpy(&out[j], &cards[idx-3], sizeof(Card));
    j += 1;
    memcpy(&out[j], &cards[idx-2], sizeof(Card));
    j += 1;
    memcpy(&out[j], &cards[idx-1], sizeof(Card));
    j += 1;
    memcpy(&out[j], &cards[idx], sizeof(Card));
    j += 1;
    FOR_ARRAY(no_cards, {
        if ((i != idx - 3) || (i != idx - 2) || (i != idx - 1) || (i != idx)) memcpy(&out[j], &cards[i], sizeof(Card));
        if (j == 4) break;
        j += 1;
    });
    INFO("Poker: %s", print_cards_impl(out, NO_CARDS_TABLE));
    return out;
}

Card* check_tris_impl(Card* cards, size_t no_cards) {
    FOR_ARRAY(no_cards, {
        if ((cards[i - 1].value == cards[i].value) &&
            (cards[i - 2].value == cards[i - 1].value) && (i > 1)) return ret_tris_impl(cards, no_cards, i);
    });
    return NULL;
}

Card* ret_tris_impl(Card* cards, size_t no_cards, size_t idx) {
    Card* out = malloc(NO_CARDS_TABLE * sizeof(Card));
    size_t j = 0;
    memcpy(&out[j], &cards[idx-2], sizeof(Card));
    j += 1;
    memcpy(&out[j], &cards[idx-1], sizeof(Card));
    j += 1;
    memcpy(&out[j], &cards[idx], sizeof(Card));
    j += 1;
    FOR_ARRAY(no_cards, {
        if ((i != idx - 2) || (i != idx - 1) || (i != idx)) memcpy(&out[j], &cards[i], sizeof(Card));
        if (j == 4) break;
        j += 1;
    });
    INFO("Tris: %s", print_cards_impl(out, NO_CARDS_TABLE));
    return out;
}

Card* check_pair_impl(Card* cards, size_t no_cards) {
    FOR_ARRAY(no_cards, {
        if ((cards[i - 1].value == cards[i].value) && (i > 0)) return ret_pair_impl(cards, no_cards, i);
    });
    return NULL;
}

Card* ret_pair_impl(Card* cards, size_t no_cards, size_t idx) {
    Card* out = malloc(NO_CARDS_TABLE * sizeof(Card));
    size_t j = 0;
    memcpy(&out[j], &cards[idx-1], sizeof(Card));
    j += 1;
    memcpy(&out[j], &cards[idx], sizeof(Card));
    j += 1;
    FOR_ARRAY(no_cards, {
        if ((i != idx - 1) || (i != idx)) memcpy(&out[j], &cards[i], sizeof(Card));
        if (j == 4) break;
        j += 1;
    });
    INFO("Pair: %s", print_cards_impl(out, NO_CARDS_TABLE));
    return out;
}

Card* check_high_card_impl(Card* cards, size_t no_cards) {
    Card* out = malloc(NO_CARDS_TABLE * sizeof(Card));
    FOR_ARRAY(NO_CARDS_TABLE, {
        memcpy(&out[i], &cards[i], sizeof(Card));
    });
    INFO("High Card: %s", print_cards_impl(out, NO_CARDS_TABLE));
    return out;
}

int main(int argc, char** argv) {

    Game* holdem = malloc(sizeof(Game));

    holdem->start_game = &start_game_impl;
    holdem->start_game((void*)holdem);

    holdem->deal_cards = &deal_cards_impl;
    holdem->deal_cards((void*)holdem);

    // FOR_ARRAY(holdem->deck->dim, {
    //     INFO("Shuffled Card: %s %d", print_card_impl(&(holdem->deck->cards[i])), i);
    // });

    FOR_ARRAY(holdem->num_players, {
        INFO("--------------------");
        INFO("Player %d: %s", i, print_cards_impl(holdem->players[i].cards, NO_CARDS_HAND));
        INFO("Player %d: FICHES %d, SB %d, BB %d", i, holdem->players[i].fiches,
                holdem->players[i].is_sblind, holdem->players[i].is_bblind);
    });

    holdem->flop = &flop_impl;
    holdem->flop((void*)holdem);

    INFO("--------------------");
    INFO("Table: %s", print_cards_impl(holdem->table->cards, NO_CARDS_FLOP));
    FOR_ARRAY(holdem->num_players, {
        INFO("--------------------");
        INFO("Player %d: %s", i, print_cards_impl(holdem->players[i].cards, NO_CARDS_HAND));
        INFO("Player %d: FICHES %d, SB %d, BB %d", i, holdem->players[i].fiches,
                holdem->players[i].is_sblind, holdem->players[i].is_bblind);
    });

    holdem->turn = &turn_impl;
    holdem->turn((void*)holdem);

    INFO("--------------------");
    INFO("Table: %s", print_cards_impl(holdem->table->cards, NO_CARDS_FLOP + NO_CARDS_TURN));
    FOR_ARRAY(holdem->num_players, {
        INFO("--------------------");
        INFO("Player %d: %s", i, print_cards_impl(holdem->players[i].cards, NO_CARDS_HAND));
        INFO("Player %d: FICHES %d, SB %d, BB %d", i, holdem->players[i].fiches,
                holdem->players[i].is_sblind, holdem->players[i].is_bblind);
    });

    holdem->river = &river_impl;
    holdem->river((void*)holdem);

    INFO("--------------------");
    INFO("Table: %s", print_cards_impl(holdem->table->cards, NO_CARDS_TABLE));
    FOR_ARRAY(holdem->num_players, {
        INFO("--------------------");
        INFO("Player %d: %s", i, print_cards_impl(holdem->players[i].cards, NO_CARDS_HAND));
        INFO("Player %d: FICHES %d, SB %d, BB %d", i, holdem->players[i].fiches,
                holdem->players[i].is_sblind, holdem->players[i].is_bblind);
    });

    holdem->check_points = &check_point_impl;
    holdem->check_points((void*)holdem);

    holdem->collect_cards = &collect_cards_impl;
    holdem->collect_cards((void*)holdem);

    // FOR_ARRAY(holdem->deck->dim, {
    //     INFO("Shuffled Card: %s %d", print_card_impl(&(holdem->deck->cards[i])), i);
    // });

    return 0;
}