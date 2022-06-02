/** @file
 * Implementations of the functions declared in string_utils.h.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include "string_utils.h"

#define DECIMAL_STAR_REPRESENTATION 10 /**< The digit which '*' represents. */
#define DECIMAL_HASH_REPRESENTATION 11 /**< The digit which '#' represents. */

bool isValidDigit(char const c) {
    return isdigit(c) || c == '*' || c == '#';
}

bool isNumber(char const *number) {
    if (number == NULL) {
        return false;
    }

    size_t i = 0;
    while (isValidDigit(number[i])) {
        i++;
    }

    if (i == 0 || number[i] != '\0') {
        return false;
    }
    return true;
}

bool areEqual(char const *num1, char const *num2) {
    size_t i = 0;

    while (isValidDigit(num1[i]) && isValidDigit(num2[i])) {
        if (num1[i] != num2[i]) {
            return false;
        }
        i++;
    }

    return num1[i] == '\0' && num2[i] == '\0';
}

bool isPrefix(char const *num, char const *prefix) {
    size_t i = 0;

    while (isValidDigit(num[i]) && isValidDigit(prefix[i])) {
        if (num[i] != prefix[i]) {
            return false;
        }
        i++;
    }

    return prefix[i] == '\0';
}

bool checkNumbers(char const *num1, char const *num2) {
    if (num1 == NULL || num2 == NULL) {
        return false;
    }

    size_t i = 0;
    bool equal = true;

    while (isValidDigit(num1[i]) && isValidDigit(num2[i])) {
        if (num1[i] != num2[i]) {
            equal = false;
        }
        i++;
    }

    if (i == 0) {
        return false;
    }
    size_t j = i;

    while (isValidDigit(num1[i])) {
        i++;
        equal = false;
    }
    if (num1[i] != '\0') {
        return false;
    }

    while (isValidDigit(num2[j])) {
        j++;
        equal = false;
    }
    if (num2[j] != '\0') {
        return false;
    }

    if (equal) {
        return false;
    }
    return true;
}

size_t length(const char *num) {
    size_t len = 0;
    while (isValidDigit(num[len])) {
        len++;
    }
    return len;
}

int toDecimalRepresentation(char const c) {
    if (c == '*') {
        return DECIMAL_STAR_REPRESENTATION;
    } else if (c == '#') {
        return DECIMAL_HASH_REPRESENTATION;
    }
    return c - '0';
}

bool copyNumber(char const *num, char **numberPtr) {
    char *result = NULL;
    result = malloc(sizeof(char) * (length(num) + 1));
    if (result == NULL) {
        return false;
    }
    size_t i = 0;

    while (isValidDigit(num[i])) {
        result[i] = num[i];
        i++;
    }
    result[i] = '\0';

    *numberPtr = result;
    return true;
}

bool copyParts(char const *num, char const *newPrefix, size_t lenOfOriginalPrefix, char **numberPtr) {
    char *result = NULL;
    result = malloc(sizeof(char) * (length(newPrefix) - lenOfOriginalPrefix + length(num) + 1));
    if (result == NULL) {
        return false;
    }
    size_t i = 0;

    while (isValidDigit(newPrefix[i])) {
        result[i] = newPrefix[i];
        i++;
    }

    size_t j = lenOfOriginalPrefix;
    while (isValidDigit(num[j])) {
        result[i] = num[j];
        i++;
        j++;
    }
    result[i] = '\0';

    *numberPtr = result;
    return true;
}
