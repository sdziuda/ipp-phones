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

bool phnumAddAllCopiedParts(PhoneNumbers *from, PhoneNumbers *to, char const *originalNumber, size_t prefixLength) {
    for (size_t i = 0; i < from->size; i++) {
        char *result = NULL;
        if (!copyParts(originalNumber, from->array[i].number, prefixLength, &result)) {
            return false;
        }
        if (!phnumAdd(to, &result)) {
            free(result);
            return false;
        }
    }
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

/**
 * @brief Removes phone number at the given index.
 * This function will free the memory allocated for the phone number at the given index and then shift all other
 * phone numbers in the array to the left.
 * @param [in ,out] pNumbers - pointer to the phone numbers structure.
 * @param [in] index - index of the phone number to be removed.
 */
static void phnumRemoveAtIndex(PhoneNumbers *pNumbers, size_t const index) {
    free(pNumbers->array[index].number);
    pNumbers->size--;

    for (size_t i = index; i < pNumbers->size; i++) {
        pNumbers->array[i] = pNumbers->array[i + 1];
    }
}

/**
 * @brief Compares two phone numbers according to their lexicographical order.
 * @param [in] a - first phone number.
 * @param [in] b - second phone number.
 * @return Value @p 1 if a is greater than b.
 *         Value @p -1 if a is less than b.
 *         Value @p 0 if a is equal to b.
 */
static int comparePhoneNumbers(void const *a, void const *b) {
    PNumber const *p1 = a;
    PNumber const *p2 = b;

    size_t i = 0;
    while (isValidDigit(p1->number[i]) && isValidDigit(p2->number[i])) {
        if (p1->number[i] > p2->number[i]) {
            return 1;
        } else if (p1->number[i] < p2->number[i]) {
            return -1;
        }
        i++;
    }

    if (isValidDigit(p1->number[i])) {
        return 1;
    } else if (isValidDigit(p2->number[i])) {
        return -1;
    }

    return 0;
}


void sortPhoneNumbers(PhoneNumbers *pNumbers) {
    qsort(pNumbers->array, pNumbers->size, sizeof(PNumber), comparePhoneNumbers);
}

void removeDuplicates(PhoneNumbers *pNumbers) {
    size_t i = 0;
    while (i < pNumbers->size - 1) {
        if (areEqual(pNumbers->array[i].number, pNumbers->array[i + 1].number)) {
            phnumRemoveAtIndex(pNumbers, i + 1);
        } else {
            i++;
        }
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

    return pn->array[idx].number;
}

