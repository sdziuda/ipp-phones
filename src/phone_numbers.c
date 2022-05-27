/** @file
 * Implementations of functions regarding phone numbers.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "phone_numbers.h"
#include "string_utils.h"

/**
 * @struct PhoneNumber
 * @brief Structure containing a single phone number.
 * @var PhoneNumber::number
 *      String containing a phone number.
 */
struct PhoneNumber {
    char *number;
};

PhoneNumbers *phnumNew(void) {
    PhoneNumbers *numbers = malloc(sizeof(PhoneNumbers));
    if (numbers == NULL) {
        return NULL;
    }

    numbers->array = NULL;
    numbers->size = 0;
    numbers->capacity = 1;
    return numbers;
}

bool phnumAdd(PhoneNumbers *pNumbers, char **number) {
    pNumbers->size++;

    if (pNumbers->size == pNumbers->capacity) {
        pNumbers->capacity *= 2;
        pNumbers->array = realloc(pNumbers->array, pNumbers->capacity * sizeof(PNumber));
        if (pNumbers->array == NULL) {
            pNumbers->size--;
            return false;
        }
    }

    pNumbers->array[pNumbers->size - 1].number = *number;
    return true;
}

void phnumRemove(PhoneNumbers **pNumbersPtr, char const *num) {
    PhoneNumbers *pNumbers = *pNumbersPtr;
    size_t i;
    for (i = 0; i < pNumbers->size; i++) {
        if (areEqual(pNumbers->array[i].number, num)) {
            free(pNumbers->array[i].number);
            pNumbers->size--;
        }
    }

    if (pNumbers->size == 0) {
        free(pNumbers->array);
        free(pNumbers);
        pNumbers = NULL;
        *pNumbersPtr = NULL;
        return;
    }

    for (size_t j = i; j < pNumbers->size; j++) {
        pNumbers->array[j] = pNumbers->array[j + 1];
    }
}

void phnumRemoveWithPrefix(PhoneNumbers **pNumbersPtr, char const *prefix) {
    PhoneNumbers *pNumbers = *pNumbersPtr;
    size_t i = 0;
    while (i < pNumbers->size) {
        if (isPrefix(pNumbers->array[i].number, prefix)) {
            free(pNumbers->array[i].number);
            pNumbers->array[i] = pNumbers->array[pNumbers->size - 1];
            pNumbers->size--;
        } else {
            i++;
        }
    }

    if (pNumbers->size == 0) {
        free(pNumbers->array);
        free(pNumbers);
        pNumbers = NULL;
        *pNumbersPtr = NULL;
        return;
    }
}

void phnumDelete(PhoneNumbers *pn) {
    if (pn == NULL) {
        return;
    }

    for (size_t i = 0; i < pn->size; i++) {
        free(pn->array[i].number);
    }
    free(pn->array);
    free(pn);
}

char const *phnumGet(PhoneNumbers const *pn, size_t idx) {
    if (pn == NULL || idx >= pn->size) {
        return NULL;
    }

    //qsort(pn->array, pn->size, sizeof(PNumber), compareNumbers); -> jednak lepiej w reverse

    return pn->array[idx].number;
}

