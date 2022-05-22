/** @file
 * Implementations of phone forwarding functions.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include "phone_forward.h"

#define NUMBER_OF_DIGITS 12 /**< Number of different digits. */
#define DECIMAL_STAR_REPRESENTATION 10 /**< The number which '*' represents. */
#define DECIMAL_HASH_REPRESENTATION 11 /**< The number which '#' represents. */

/**
 * @struct Node
 * @brief Node of the tree of phone forwarding.
 * @var Node::forwardedNumber
 *      String of digits containing the number to which the route from root to the current node is forwarded.
 * @var Node::parent
 *      Pointer to the parent node.
 * @var Node::next
 *      Array of pointers to the 12 children nodes in the tree.
 */
struct Node {
    char *forwardedNumber;
    struct Node *parent;
    struct Node *next[NUMBER_OF_DIGITS];
};
typedef struct Node DNode; /**< Node of the decimal tree. */

/**
 * @struct PhoneForward phone_forward.h
 * @brief Structure containing the root of the tree of phone forwarding.
 * @var PhoneForward::root
 *      Pointer to the root of the tree.
 */
struct PhoneForward {
    DNode *root;
};

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
 * @brief Structure containing an array of phone numbers.
 * @var PhoneNumbers::array
 *      Array of phone numbers.
 * @var PhoneNumbers::size
 *      Number of elements in the array.
 */
struct PhoneNumbers {
    PNumber *array;
    size_t size;
};

PhoneForward *phfwdNew(void) {
    PhoneForward *pf = malloc(sizeof(PhoneForward));
    if (pf == NULL) {
        return NULL;
    }

    pf->root = malloc(sizeof(DNode));
    if (pf->root == NULL) {
        free(pf);
        return NULL;
    }
    pf->root->parent = NULL;
    pf->root->forwardedNumber = NULL;
    for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
        pf->root->next[i] = NULL;
    }

    return pf;
}

/**
 * @brief Deletes the nodes in phone forwarding tree.
 * Starting from the given Node and going down the tree, deletes all the nodes in the tree under the given node and the
 * node itself.
 * @param [in, out] node - pointer to the node we want to start deleting from.
 */
static void deleteIterative(DNode *node) {
    DNode *current = node;
    if (current == NULL) {
        return;
    }
    current->parent = NULL;

    while (current != NULL) {
        bool hasChild = false;

        for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
            if (current->next[i]) {
                current = current->next[i];
                hasChild = true;
                break;
            }
        }

        if (!hasChild) {
            DNode *parent = current->parent;
            if (parent == NULL) {
                free(current->forwardedNumber);
                free(current);
                break;
            }

            for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
                if (parent->next[i] == current) {
                    parent->next[i] = NULL;
                    break;
                }
            }

            free(current->forwardedNumber);
            free(current);
            current = parent;
        }
    }
}

void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL) {
        return;
    }

    deleteIterative(pf->root);

    free(pf);
}

/**
 * @brief Checks if the given char is a digit.
 * Checks whether the given char is a representation of a digit (0-9, '*' or '#').
 * @param [in] c - char to check.
 * @return Value @p true if the given char is a digit, @p false otherwise.
 */
static bool isValidDigit(char const c) {
    return isdigit(c) || c == '*' || c == '#';
}

/**
 * @brief Checks if the given string is a valid phone number.
 * @param [in] number - string to check
 * @return Value @p true if the string is a valid phone number.
 *         Value @p false otherwise.
 */
static bool isNumber(char const *number) {
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


/**
 * @brief Checks whether the strings are suitable for forwarding (i.e. they are numbers and aren't the same).
 * @param [in] num1 - first number.
 * @param [in] num2 - second number.
 * @return Value @p true if they are suitable.
 *         Value @p false otherwise.
 */
static bool checkNumbers(char const *num1, char const *num2) {
    if (num1 == NULL || num2 == NULL) {
        return false;
    }

    size_t i = 0;
    bool areEqual = true;

    while (isValidDigit(num1[i]) && isValidDigit(num2[i])) {
        if (num1[i] != num2[i]) {
            areEqual = false;
        }
        i++;
    }

    if (i == 0) {
        return false;
    }
    size_t j = i;

    while (isValidDigit(num1[i])) {
        i++;
        areEqual = false;
    }
    if (num1[i] != '\0') {
        return false;
    }

    while (isValidDigit(num2[j])) {
        j++;
        areEqual = false;
    }
    if (num2[j] != '\0') {
        return false;
    }

    if (areEqual) {
        return false;
    }
    return true;
}

/**
 * @brief Obtains length of a number.
 * Obtains length of the given number by counting its digits from the start.
 * @param [in] num - number to get length of.
 * @return Length of the number.
 */
static size_t length(const char *num) {
    size_t len = 0;
    while (isValidDigit(num[len])) {
        len++;
    }
    return len;
}

/**
 * @brief Obtains the decimal representation.
 * Obtains the decimal representation of the given char.
 * @param [in] c - digit to convert.
 * @return Decimal representation of the digit.
 */
static int toDecimalRepresentation(char const c) {
    if (c == '*') {
        return DECIMAL_STAR_REPRESENTATION;
    } else if (c == '#') {
        return DECIMAL_HASH_REPRESENTATION;
    }
    return c - '0';
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (pf == NULL || !checkNumbers(num1, num2)) {
        return false;
    }

    DNode *node = pf->root;
    DNode *beforeFirstAdded = NULL;
    DNode *firstAdded = NULL;
    size_t i = 0;
    int firstAddedDigit = 0;
    while (isValidDigit(num1[i])) {
        int digit = toDecimalRepresentation(num1[i]);
        if (node->next[digit] == NULL) {
            node->next[digit] = malloc(sizeof(DNode));
            if (node->next[digit] == NULL) {
                if (beforeFirstAdded != NULL) {
                    beforeFirstAdded->next[firstAddedDigit] = NULL;
                }
                deleteIterative(firstAdded);
                return false;
            }
            node->next[digit]->parent = node;
            node->next[digit]->forwardedNumber = NULL;
            for (int j = 0; j < NUMBER_OF_DIGITS; j++) {
                node->next[digit]->next[j] = NULL;
            }
            if (firstAdded == NULL) {
                beforeFirstAdded = node;
                firstAddedDigit = digit;
                firstAdded = node->next[digit];
            }
        }
        node = node->next[digit];
        i++;
    }

    char *result = NULL;
    result = malloc(sizeof(char) * (length(num2) + 1));
    if (result == NULL) {
        if (beforeFirstAdded != NULL) {
            beforeFirstAdded->next[firstAddedDigit] = NULL;
        }
        deleteIterative(firstAdded);
        return false;
    }
    i = 0;

    while (isValidDigit(num2[i])) {
        result[i] = num2[i];
        i++;
    }
    result[i] = '\0';

    free(node->forwardedNumber);
    node->forwardedNumber = result;
    return true;
}

/**
 * @brief Finds the number od children for given node.
 * Finds all children that are not NULL and returns the number of them.
 * @param [in] node the node to find the number of children for.
 * @return the number of children.
 */
static int numberOfChildren(DNode const *node) {
    int sum = 0;
    for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
        if (node->next[i] != NULL) {
            sum++;
        }
    }
    return sum;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf == NULL || !isNumber(num)) {
        return;
    }

    DNode *node = pf->root;
    DNode *beforePointToRemove = pf->root;
    DNode *lastPointToRemove = NULL;
    int pointToRemoveDigit = 0;
    size_t i = 0;
    while (isValidDigit(num[i])) {
        int digit = toDecimalRepresentation(num[i]);
        if (node->next[digit] == NULL) {
            return;
        }
        if (node->forwardedNumber != NULL || numberOfChildren(node) > 1 || lastPointToRemove == NULL) {
            beforePointToRemove = node;
            pointToRemoveDigit = digit;
            lastPointToRemove = node->next[digit];
        }
        node = node->next[digit];
        i++;
    }

    if (beforePointToRemove != NULL) {
        beforePointToRemove->next[pointToRemoveDigit] = NULL;
    }
    deleteIterative(lastPointToRemove);
}

/**
 * @brief Copies the number to the given buffer.
 * @param [in] num - the number to copy.
 * @param [in, out] numberPtr - the buffer to copy the number to.
 * @return Value @p true if the number was copied successfully.
 *         Value @p false if there was allocation error.
 */
static bool copyNumber(char const *num, char **numberPtr) {
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

/**
 * @brief Creates a number after forwarding the given number.
 * This function first copies the given prefix @p forwardedPrefix and then appends the rest of the given number @p num
 * without the original prefix that was forwarded.
 * @param [in] num - number to be forwarded.
 * @param [in] forwardedPrefix - the prefix which the longest original prefix is forwarded to.
 * @param [in] lenOfOriginalPrefix - length of the original prefix.
 * @param [in, out] numberPtr - pointer to the created number.
 * @return Value @p true if the number was created successfully.
 *         Value @p false if there was allocation error.
 */
static bool copyParts(char const *num,
                      char const *forwardedPrefix,
                      size_t lenOfOriginalPrefix,
                      char **numberPtr) {

    char *result = NULL;
    result = malloc(sizeof(char) * (length(forwardedPrefix) - lenOfOriginalPrefix + length(num) + 1));
    if (result == NULL) {
        return false;
    }
    size_t i = 0;

    while (isValidDigit(forwardedPrefix[i])) {
        result[i] = forwardedPrefix[i];
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

/**
 * @brief Finds the longest prefix of the number that is forwarded to another number.
 * This function is used to find the longest prefix of the number @p num that still has a number in the phone forward
 * tree @p pf. This prefix will be then used to change the number to the forwarded number using @ref copyParts or
 * @ref copyNumber.
 * @param [in] pf - PhoneForward tree.
 * @param [in] num - the number we are finding prefix of.
 * @param [in, out] maxForwardedPrefix - number the longest prefix is forwarded to.
 * @param [in, out] lenOfMaxOriginalPrefix - length of the longest prefix.
 */
static void findPrefix(PhoneForward const *pf,
                       char const *num,
                       char **maxForwardedPrefix,
                       size_t *lenOfMaxOriginalPrefix) {

    DNode *node = pf->root;
    size_t i = 0;

    while (isValidDigit(num[i])) {
        int digit = toDecimalRepresentation(num[i]);

        if (node->next[digit] == NULL) {
            break;
        }

        node = node->next[digit];
        i++;

        if (node->forwardedNumber != NULL) {
            (*maxForwardedPrefix) = node->forwardedNumber;
            (*lenOfMaxOriginalPrefix) = i;
        }
    }
}

/** @brief Adds phone number to the array.
 * Allocates memory for the new number and adds it to the array.
 * @param [in, out] pNumbers - pointer to the structure of phone numbers.
 * @param [in] number - pointer to the number to add.
 * @return Value @p true if the number was added successfully.
 *         Value @p false if there was allocation error.
 */
static bool addPhoneNumber(PhoneNumbers *pNumbers, char **number) {
    pNumbers->size++;
    pNumbers->array = realloc(pNumbers->array, pNumbers->size * sizeof(PNumber));
    if (pNumbers->array == NULL) {
        return false;
    }

    pNumbers->array[pNumbers->size - 1].number = *number;
    return true;
}

PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }

    PhoneNumbers *pn = malloc(sizeof(PhoneNumbers));
    if (pn == NULL) {
        return NULL;
    }
    pn->array = NULL;
    pn->size = 0;

    if (!isNumber(num)) {
        return pn;
    }

    char *maxForwardedPrefix = NULL;
    size_t lenOfMaxOriginalPrefix = 0;
    findPrefix(pf, num, &maxForwardedPrefix, &lenOfMaxOriginalPrefix);

    char *number = NULL;
    if (maxForwardedPrefix == NULL) {
        if (!copyNumber(num, &number)) {
            free(pn->array);
            free(pn);
            return NULL;
        }
    } else {
        if (!copyParts(num, maxForwardedPrefix, lenOfMaxOriginalPrefix, &number)) {
            free(pn->array);
            free(pn);
            return NULL;
        }
    }

    if (!addPhoneNumber(pn, &number)) {
        free(number);
        free(pn->array);
        free(pn);
        return NULL;
    }
    return pn;
}

/** @brief Dummy function.
 * For now this function doesn't do anything. It is here only to avoid potential
 * warnings from the compiler. This function will be implemented in the next
 * part of the task and this comment will be removed.
 * @param [in] pf - unused pointer to the PhoneForward tree.
 * @param [in] num - unused pointer to the number.
 * @return NULL in every case.
 */
PhoneNumbers *phfwdReverse(
        __attribute__((unused)) PhoneForward const *pf,
        __attribute__((unused)) char const *num) {
    return NULL;
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