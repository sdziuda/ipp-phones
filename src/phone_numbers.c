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
typedef struct PhoneNumber PNumber; /**< A single phone number. */

/**
 * @struct PhoneNumbers phone_forward.h
 * @brief Vector containing an array of phone numbers.
 * @var PhoneNumbers::array
 *      Array of phone numbers.
 * @var PhoneNumbers::size
 *      Number of elements in the array.
 * @var PhoneNumbers::capacity
 *      Maximum number of elements in the array.
 */
struct PhoneNumbers {
    PNumber *array;
    size_t size;
    size_t capacity;
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

size_t phnumGetSize(PhoneNumbers const *pNumbers) {
    return pNumbers->size;
}

bool phnumAdd(PhoneNumbers *pNumbers, char **number) {
    pNumbers->size++;

    if (pNumbers->size == pNumbers->capacity) {
        pNumbers->capacity *= 2;
        PNumber *tmp = realloc(pNumbers->array, pNumbers->capacity * sizeof(PNumber));
        if (tmp == NULL) {
            pNumbers->size--;
            return false;
        }
        pNumbers->array = tmp;
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
    if (pNumbers == NULL) {
        return;
    }

    size_t i;
    for (i = 0; i < pNumbers->size; i++) {
        if (areEqual(pNumbers->array[i].number, num)) {
            free(pNumbers->array[i].number);
            pNumbers->size--;
            break;
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
    if (pNumbers == NULL) {
        return;
    }

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

void phnumRemoveAtIndex(PhoneNumbers *pNumbers, size_t index) {
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
        int const n1 = toDecimalRepresentation(p1->number[i]);
        int const n2 = toDecimalRepresentation(p2->number[i]);

        if (n1 > n2) {
            return 1;
        } else if (n1 < n2) {
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

