#define GYMC_IMPLEMENTATION

#include "./gymC.h"
#include "./holdem.h"

int sort_counter_impl(const void* ptr1, const void* ptr2) {
    Counter* c1 = (Counter*)ptr1;
    Counter* c2 = (Counter*)ptr2;
    int biggest = 0;
    if (c1->dim > c2->dim) biggest = 1;
    return biggest;
}

int sort_counter_by_value_impl(const void* ptr1, const void* ptr2) {
    Counter* c1 = (Counter*)ptr1;
    Counter* c2 = (Counter*)ptr2;
    int biggest = 0;
    if (c1->value > c2->value) biggest = 1;
    return biggest;
}

void start_game_impl(void* game) {
    ((Game*)game)->deck = malloc(sizeof(Deck));
    ((Game*)game)->deck->deck_make = &deck_make_impl;
    ((Game*)game)->deck->deck_make((void*)(((Game*)game)->deck));
    ((Game*)game)->num_players = NO_PLAYERS;
    ((Game*)game)->act_players = malloc(NO_PLAYERS * sizeof(size_t));
    ((Game*)game)->players = malloc(sizeof(Player) * ((Game*)game)->num_players);
    FOR_ARRAY(((Game*)game)->num_players, {
        ((Game*)game)->players[i].cards = malloc(sizeof(Card) * NO_CARDS_HAND);
        ((Game*)game)->players[i].best_hand = malloc(sizeof(Card) * NO_CARDS_TABLE);
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
    Counter* winners = malloc(sizeof(Counter));
    winners->dim = 0;
    winners->idx = malloc(1 * sizeof(size_t));
    winners->value = HIGH_CARD;
    Card* all = malloc(sizeof(Card) * (NO_CARDS_TABLE + NO_CARDS_HAND));
    FOR_ARRAY(((Game*)game)->num_players, {
        // THE CYCLE DEPENDS FROM THE DEALER POSITION
        // THE PLAYER BET HAS TO BE SET IN PLAYER STRUCT
        // WE SHOULD CYCLE ONLY ON ACTIVE PLAYERS
        memcpy(all, ((Game*)game)->table->cards, sizeof(Card) * NO_CARDS_TABLE);
        memcpy(&all[NO_CARDS_TABLE], ((Game*)game)->players[i].cards, sizeof(Card) * NO_CARDS_HAND);
        qsort(all, NO_CARDS_TABLE + NO_CARDS_HAND, sizeof(Card), sort_cards_value_impl);

        Counter* count = malloc((ACE + 1) * sizeof(Counter));
        Counter* count_s = malloc((SPADES + 1) * sizeof(Counter));
        FOR_ARRAY(ACE + 1, {
            count[i].dim = 0;
            count[i].idx = malloc(1 * sizeof(size_t));
            count[i].value = i;
        });
        FOR_ARRAY(SPADES + 1, {
            count_s[i].dim = 0;
            count_s[i].idx = malloc(1 * sizeof(size_t));
            count_s[i].value = i;
        });
        FOR_ARRAY(NO_CARDS_TABLE + NO_CARDS_HAND, {
            count[all[i].value].dim += 1;
            count[all[i].value].idx = realloc(count[all[i].value].idx, count[all[i].value].dim * sizeof(size_t));
            count[all[i].value].idx[count[all[i].value].dim - 1] = i;

            count_s[all[i].suit].dim += 1;
            count_s[all[i].suit].idx = realloc(count_s[all[i].suit].idx, count_s[all[i].suit].dim * sizeof(size_t));
            count_s[all[i].suit].idx[count_s[all[i].suit].dim - 1] = i;
        });
        qsort(count, ACE + 1, sizeof(Counter), sort_counter_impl);
        qsort(count_s, SPADES + 1, sizeof(Counter), sort_counter_impl);
        ((Game*)game)->players[i].best_hand = check_counter_impl(&((Game*)game)->players[i], count, count_s, all);
        FOR_ARRAY(SPADES + 1, {
            free(count_s[i].idx);
        });
        FOR_ARRAY(ACE + 1, {
            free(count[i].idx);
        });
        free(count_s);
        free(count);

        if (((Game*)game)->players[i].point == winners->value) {
            if (winners->dim > 0) {
                size_t best = compare_best_hands_impl(((Game*)game)->players[winners->idx[0]].best_hand,
                            ((Game*)game)->players[i].best_hand);
                switch (best)
                {
                    case 0:
                        break;
                    
                    case 1:
                        winners->dim = 1;
                        winners->idx = realloc(winners->idx, winners->dim * sizeof(size_t));
                        winners->idx[winners->dim - 1] = i;
                        winners->value = ((Game*)game)->players[i].point;
                        break;

                    case 2:
                        winners->dim += 1;
                        winners->idx = realloc(winners->idx, winners->dim * sizeof(size_t));
                        winners->idx[winners->dim - 1] = i;
                        winners->value = ((Game*)game)->players[i].point;
                        break;
                }
            } else {
                winners->dim = 1;
                winners->idx = realloc(winners->idx, winners->dim * sizeof(size_t));
                winners->idx[winners->dim - 1] = i;
                winners->value = ((Game*)game)->players[i].point;
            }
        }
        if (((Game*)game)->players[i].point > winners->value) {
            winners->dim = 1;
            winners->idx = realloc(winners->idx, winners->dim * sizeof(size_t));
            winners->idx[winners->dim - 1] = i;
            winners->value = ((Game*)game)->players[i].point;
        }

    });
    FOR_ARRAY(winners->dim, {
        ((Game*)game)->players[winners->idx[i]].fiches += (((Game*)game)->table->plate / (winners->dim));
    });
    free(winners);
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

size_t check_flush_straight_impl(Counter* count, Card* all, Card* straight) {
    size_t is_straight = 0;
    size_t j = ACE;
    Card* tmp = malloc(count->dim * sizeof(Card));
    FOR_ARRAY(count->dim, {
        memcpy(&tmp[i], &all[count->idx[i]], sizeof(Card));
    });
    Counter* count2 = malloc((ACE + 1) * sizeof(Counter));
    FOR_ARRAY(ACE + 1, {
        count2[i].dim = 0;
        count2[i].idx = malloc(1 * sizeof(size_t));
        count2[i].value = i;
    });
    FOR_ARRAY(count->dim, {
        count2[tmp[i].value].dim += 1;
        count2[tmp[i].value].idx = realloc(count2[tmp[i].value].idx, count2[tmp[i].value].dim * sizeof(size_t));
        count2[tmp[i].value].idx[count2[tmp[i].value].dim - 1] = i;
    });
    FOR_ARRAY(ACE + 1, {
        if (count2[j].dim) {
            memcpy(&straight[is_straight], &tmp[count2[j].idx[0]], sizeof(Card));
            is_straight += 1;
        } else {
            is_straight = 0;
        }
        if (is_straight == 5) break;
        j -= 1;
    });
    if (straight[0].value == ACE && is_straight == 5) is_straight += 1;
    if (is_straight == 4 && count2[ACE].dim) {
        memcpy(&straight[is_straight], &tmp[count2[ACE].idx[0]], sizeof(Card));
        is_straight += 1;
    }
    free(count2);
    free(tmp);
    return is_straight;
}

size_t check_straight_impl(Counter* count, Card* all, Card* straight) {
    size_t is_straight = 0;
    size_t j = ACE;
    Counter* count2 = malloc((ACE + 1) * sizeof(Counter));
    memcpy(count2, count, (ACE + 1) * sizeof(Counter));
    qsort(count2, ACE + 1, sizeof(Counter), sort_counter_by_value_impl);
    FOR_ARRAY(ACE + 1, {
        if (count2[j].dim) {
            memcpy(&straight[is_straight], &all[count2[j].idx[0]], sizeof(Card));
            is_straight += 1;
        } else {
            is_straight = 0;
        }
        if (is_straight == 5) break;
        j -= 1;
    });
    if (is_straight == 4 && count2[ACE].dim) {
        memcpy(&straight[is_straight], &all[count2[ACE].idx[0]], sizeof(Card));
        is_straight += 1;
    }
    free(count2);
    return is_straight;
}

Card* check_counter_impl(Player* pl, Counter* count, Counter* count_s, Card* all) {
    Card* out = malloc(NO_CARDS_TABLE * sizeof(Card));
    Card* straight = malloc(NO_CARDS_TABLE * sizeof(Card));
    size_t j = ACE;
    size_t n = 0;
    Card* last = NULL;
    FOR_ARRAY(NO_CARDS_TABLE, {
        if (n < count[j].dim) {
            memcpy(&out[i], &all[count[j].idx[n]], sizeof(Card));
            n += 1;
        } else {
            n = 0;
            j -= 1;
            memcpy(&out[i], &all[count[j].idx[n]], sizeof(Card));
            n += 1;
        }
    });
    FOR_ARRAY(ACE + 1, {
        if (count[ACE - i].dim == 1) {
            last = malloc(sizeof(Card));
            memcpy(last, &all[count[ACE - i].idx[0]], sizeof(Card));
            break;
        }
    });
    j = ACE;
    if (count_s[SPADES].dim >= 5 && check_flush_straight_impl(&count_s[SPADES], all, straight) == 6) {
        pl->point = ROYAL_STRAIGHT;
        return straight;
    }
    if (count_s[SPADES].dim >= 5 && check_flush_straight_impl(&count_s[SPADES], all, straight) == 5) {
        pl->point = FLUSH_STRAIGHT;
        return straight;
    }
    if (count[j].dim == 4) {
        pl->point = POKER;
        if (last && last->value > out[NO_CARDS_TABLE - 1].value) memcpy(&out[NO_CARDS_TABLE - 1], last, sizeof(Card));
        free(last);
        return out;
    }
    if (count[j].dim == 3 && count[j - 1].dim >= 2) {
        pl->point = FULL_HOUSE;
        return out;
    }
    if (count_s[SPADES].dim >= 5) {
        pl->point = FLUSH;
        FOR_ARRAY(NO_CARDS_TABLE, {
            memcpy(&out[i], &all[count_s[SPADES].idx[i]], sizeof(Card));
        });
        free(last);
        return out;
    }
    if (check_straight_impl(count, all, straight) == 5) {
        pl->point = SIMPLE_STRAIGHT;
        return straight;
    }
    if (count[j].dim == 3) {
        pl->point = TRIS;
        return out;
    }
    if (count[j].dim == 2 && count[j - 1].dim == 2) {
        pl->point = TWO_PAIR;
        if (last && last->value > out[NO_CARDS_TABLE - 1].value) memcpy(&out[NO_CARDS_TABLE - 1], last, sizeof(Card));
        return out;
    }
    if (count[j].dim == 2) {
        pl->point = ONE_PAIR;
        return out;
    }
    pl->point = HIGH_CARD;
    return out;
}

size_t compare_best_hands_impl(Card* cur_best, Card* new) {
    FOR_ARRAY(NO_CARDS_TABLE, {
        if (cur_best[i].value > new[i].value) return 0;
        if (cur_best[i].value < new[i].value) return 1;
    });
    return 2;
}

void print_table_impl(void* game) {
    INFO("");
    INFO("-----------------------------------------------------------------------------------------------");
    INFO("");
    FOR_ARRAY(4, {
        if (i < ((Game*)game)->num_players) {
            INFO2("P. %d: %s\t", i, print_cards_impl(((Game*)game)->players[i].cards, NO_CARDS_HAND));
        } else {
            INFO2("P. %d: -- -- \t", i);
        }
    });
    INFO("");
    FOR_ARRAY(4, {
        if (i < ((Game*)game)->num_players) {
            INFO2("SB %d, %d, BB %d\t", ((Game*)game)->players[i].is_sblind,
                ((Game*)game)->players[i].fiches, ((Game*)game)->players[i].is_bblind);
        } else {
            INFO2("----------------\t");
        }
    });
    INFO("");
    INFO("");
    size_t no_cards;
    SET_PHASE(((Game*)game)->phase, no_cards);
    if (8 < ((Game*)game)->num_players) {
        INFO2("P. %d: %s\t", 8, print_cards_impl(((Game*)game)->players[8].cards, NO_CARDS_HAND));
        INFO("\t\t\tTable: %s\t", print_cards_impl(((Game*)game)->table->cards, no_cards));
        INFO("SB %d, %d, BB %d\t", ((Game*)game)->players[8].is_sblind,
                ((Game*)game)->players[8].fiches, ((Game*)game)->players[8].is_bblind);
    } else {
        INFO2("P. %d: -- -- \t", 8);
        INFO("\t\t\tTable: %s\t", print_cards_impl(((Game*)game)->table->cards, no_cards));
        INFO("----------------\t");
    }
    INFO("");
    FOR_ARRAY(4, {
        if (7 - i < ((Game*)game)->num_players) {
            INFO2("P. %d: %s\t", 7 - i, print_cards_impl(((Game*)game)->players[7 - i].cards, NO_CARDS_HAND));
        } else {
            INFO2("P. %d: -- -- \t", 7 - i);
        }
    });
    INFO("");
    FOR_ARRAY(4, {
        if (7 - i < ((Game*)game)->num_players) {
            INFO2("SB %d, %d, BB %d\t", ((Game*)game)->players[7 - i].is_sblind,
                ((Game*)game)->players[7 - i].fiches, ((Game*)game)->players[7 - i].is_bblind);
        } else {
            INFO2("----------------\t");
        }
    });
    INFO("");
    INFO("");
    INFO("-----------------------------------------------------------------------------------------------");
}

int main(int argc, char** argv) {

    unsigned int seed;
    FILE* urandom = fopen("/dev/urandom", "r");
    fread(&seed, sizeof(int), 1, urandom);
    fclose(urandom);
    srand(seed);

    Game* holdem = malloc(sizeof(Game));

    holdem->start_game = &start_game_impl;
    holdem->start_game((void*)holdem);

    holdem->deal_cards = &deal_cards_impl;
    holdem->deal_cards((void*)holdem);

    // FOR_ARRAY(holdem->deck->dim, {
    //     INFO("Shuffled Card: %s %d", print_card_impl(&(holdem->deck->cards[i])), i);
    // });

    //print_table_impl((void*)holdem);

    holdem->flop = &flop_impl;
    holdem->flop((void*)holdem);

    //print_table_impl((void*)holdem);

    holdem->turn = &turn_impl;
    holdem->turn((void*)holdem);

    //print_table_impl((void*)holdem);

    holdem->river = &river_impl;
    holdem->river((void*)holdem);
    
    holdem->check_points = &check_point_impl;
    holdem->check_points((void*)holdem);

    print_table_impl((void*)holdem);

    holdem->collect_cards = &collect_cards_impl;
    holdem->collect_cards((void*)holdem);

    // FOR_ARRAY(holdem->deck->dim, {
    //     INFO("Shuffled Card: %s %d", print_card_impl(&(holdem->deck->cards[i])), i);
    // });

    free(holdem);

    return 0;
}