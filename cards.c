//#define GYMC_IMPLEMENTATION
#include "./gymC.h"
#include "./cards.h"

const char* SuitNames[] = {"♣", "♦", "♥", "♠"};
const char* ValueNames[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

void deck_make_impl(void* deck) {
    ((Deck*)deck)->dim = (ACE + 1) * (SPADES + 1);
    ((Deck*)deck)->cards = malloc(sizeof(Card) * ((Deck*)deck)->dim);
    for (Value value = TWO; value < ACE + 1; value++) {
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
    FOR_ARRAY(((Deck*)deck)->dim, {
        int from = rand() % ((Deck*)deck)->dim;
        int to = rand() % ((Deck*)deck)->dim;
        Card tmp = ((Deck*)deck)->cards[from];
        ((Deck*)deck)->cards[from] = ((Deck*)deck)->cards[to];
        ((Deck*)deck)->cards[to] = tmp;
    });
}

int sort_cards_value_impl(const void* ptr1, const void* ptr2) {
    Card* c1 = (Card*)ptr1;
    Card* c2 = (Card*)ptr2;
    int biggest = 0;
    if (c1->value < c2->value) biggest = 1;
    return biggest;
}

int sort_cards_suit_impl(const void* ptr1, const void* ptr2) {
    Card* c1 = (Card*)ptr1;
    Card* c2 = (Card*)ptr2;
    int biggest = 0;
    if (c1->suit < c2->suit) biggest = 1;
    return biggest;
}

char* print_card_impl(Card* card) {
    size_t dim = strlen(ValueNames[card->value]) + strlen(SuitNames[card->suit]) + 1;
    char* card_name = malloc(dim * sizeof(char));
    strcpy(card_name, ValueNames[card->value]);
    strcat(card_name, SuitNames[card->suit]);
    return card_name;
}

char* print_cards_impl(Card* cards, size_t no_cards) {
    if (no_cards == 0) return "-- -- -- -- -- ";
    char* tmp = print_card_impl(&cards[0]);
    size_t dim = strlen(tmp) + 2;
    char* out = malloc(dim * sizeof(char));
    strcpy(out, tmp);
    strcat(out, " \0");
    FOR_ARRAY(no_cards - 1, {
        char* tmp = print_card_impl(&cards[i + 1]);
        dim += strlen(tmp) + 2;
        out = realloc(out, dim * sizeof(char));
        strcat(out, tmp);
        strcat(out, " \0");
    });
    return out;
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