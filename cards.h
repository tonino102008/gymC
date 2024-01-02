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
    ACE,
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
char* print_card_impl(Card* card);

#endif // CARDS_H_