#ifndef CARDS_H_
#define CARDS_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES,
} Suit;

typedef enum {
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE,
} Value;

typedef struct {
    Suit suit;
    Value value;
} Card;

typedef struct {
    Card* cards;
    size_t dim;
    void (*deck_make)(void*);
    void (*deck_shuffle)(void*);
} Deck;

void deck_make_impl(void* deck);
void deck_shuffle_impl(void* deck);
int sort_cards_value_impl(const void* ptr1, const void* ptr2);
int sort_cards_suit_impl(const void* ptr1, const void* ptr2);
char* print_card_impl(Card* card);
char* print_cards_impl(Card* cards, size_t no_cards);

#endif // CARDS_H_