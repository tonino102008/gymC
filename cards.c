//#define GYMC_IMPLEMENTATION
#include "./gymC.h"
#include "./cards.h"

const char* SuitNames[] = {"♣", "♦", "♥", "♠"};
const char* ValueNames[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

void deck_make_impl(void* deck) {
    ((Deck*)deck)->dim = (KING + 1) * (SPADES + 1);
    ((Deck*)deck)->cards = malloc(sizeof(Card) * ((Deck*)deck)->dim);
    for (Value value = ACE; value < KING + 1; value++) {
        for (Suit suit = CLUBS; suit < SPADES + 1; suit++) {
            Card card = {
                .value = value,
                .suit = suit,
            };
            ((Deck*)deck)->cards[value * (SPADES + 1) + suit] = card;
        }
    }
}

void deck_shuffle_impl(void* deck) {
    assert(((Deck*)deck)->dim > 0);
    srand(time(NULL));
    FOR_ARRAY(((Deck*)deck)->dim, {
        int from = rand() % ((Deck*)deck)->dim;
        int to = rand() % ((Deck*)deck)->dim;
        Card tmp = ((Deck*)deck)->cards[from];
        ((Deck*)deck)->cards[from] = ((Deck*)deck)->cards[to];
        ((Deck*)deck)->cards[to] = tmp;
    });
}

char* print_card_impl(Card* card) {
    size_t dim = strlen(ValueNames[card->value]) + strlen(SuitNames[card->suit]) + 1;
    char* card_name = malloc(dim * sizeof(char));
    strcpy(card_name, ValueNames[card->value]);
    strcat(card_name, SuitNames[card->suit]);
    return card_name;
}

// int main(int argc, char** argv) {
//     Deck* deck = malloc(sizeof(Deck));
//     (*deck).deck_make = &deck_make_impl;
//     (*deck).deck_make(deck);
//     FOR_ARRAY(deck->dim, {
//         INFO("Card: %d %d %d", deck->cards[i].value, deck->cards[i].suit, i);
//     });
//     (*deck).deck_shuffle = &deck_shuffle_impl;
//     (*deck).deck_shuffle(deck);
//     FOR_ARRAY(deck->dim, {
//         INFO("Shuffled Card: %d %d %d", deck->cards[i].value, deck->cards[i].suit, i);
//     });
//     return 0;
// }