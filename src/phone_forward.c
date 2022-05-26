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
#include <stdio.h>
#include "phone_forward.h"

#define NUMBER_OF_DIGITS 12 /**< Number of different digits. */
#define DECIMAL_STAR_REPRESENTATION 10 /**< The digit which '*' represents. */
#define DECIMAL_HASH_REPRESENTATION 11 /**< The digit which '#' represents. */

/**
 * @struct Node
 * @brief Node of the tree of phone forwarding.
 * @var Node::numbers
 *      Vector of numbers, for the forwarding tree it will hold only one number (the one to which the route from
 *      root to the current node is forwarded), for the reverse tree it will hold all numbers which are
 *      forwarded to that number.
 * @var Node::parent
 *      Pointer to the parent node.
 * @var Node::next
 *      Array of pointers to the 12 children nodes in the tree.
 */
struct Node {
    PhoneNumbers *numbers;
    struct Node *parent;
    struct Node *next[NUMBER_OF_DIGITS];
};
typedef struct Node DNode; /**< Node of the forwarding tree. */

/**
 * @struct PhoneForward phone_forward.h
 * @brief Structure containing the root of the tree of phone forwarding and reverse tree.
 * @var PhoneForward::root
 *      Pointer to the root of the tree.
 * @var PhoneForward::reverseRoot
 *      Pointer to the root of the tree of reverse phone forwarding.
 */
struct PhoneForward {
    DNode *root;
    DNode *reverseRoot;
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

/**
 * @brief Creates a new Node.
 * Allocates memory for the node and initializes it.
 * @return Pointer to the new node or NULL if there was an allocation error.
 */
static DNode *nodeNew(void) {
    DNode *node = malloc(sizeof(DNode));
    if (node == NULL) {
        return NULL;
    }

    node->numbers = NULL;
    node->parent = NULL;
    for (int i = 0; i < NUMBER_OF_DIGITS; ++i) {
        node->next[i] = NULL;
    }

    return node;
}

PhoneForward *phfwdNew(void) {
    PhoneForward *pf = malloc(sizeof(PhoneForward));
    if (pf == NULL) {
        return NULL;
    }

    pf->root = nodeNew();
    if (pf->root == NULL) {
        free(pf);
        return NULL;
    }

    pf->reverseRoot = nodeNew();
    if (pf->reverseRoot == NULL) {
        free(pf->root);
        free(pf);
        return NULL;
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
                phnumDelete(current->numbers);
                free(current);
                break;
            }

            for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
                if (parent->next[i] == current) {
                    parent->next[i] = NULL;
                    break;
                }
            }

            phnumDelete(current->numbers);
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
    deleteIterative(pf->reverseRoot);

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
 * @brief Checks if two numbers are equal.
 * @param [in] num1 - first number to compare.
 * @param [in] num2 - second number to compare.
 * @return Value @p true if the two numbers are equal.
 *         Value @p false otherwise.
 */
static bool areEqual(char const *num1, char const *num2) {
    size_t i = 0;

    while (isValidDigit(num1[i]) && isValidDigit(num2[i])) {
        if (num1[i] != num2[i]) {
            return false;
        }
        i++;
    }

    return num1[i] == '\0' && num2[i] == '\0';
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
 * @brief Obtains the node at the end of the route.
 * Obtains the node which represents the last digit of the given number. If there was any allocation error,
 * the function will delete all the nodes it has already added on the route to the current node and return NULL.
 * This function will also update values pointed by the given pointers.
 * @param [in] start - pointer to the node we want to start looking from.
 * @param [in, out] beforeFirstAddedPtr - pointer to the node before the first node added by this function.
 * @param [in, out] firstAddedPtr - pointer to the first node added by this function.
 * @param [in, out] firstAddedDigit - pointer to the digit of the first node added by this function.
 * @param [in] num - number to get the last digit of.
 * @return pointer to the node at the end of the route or NULL if there was an allocation error.
 */
static DNode *getEndNode(DNode *start, DNode **beforeFirstAddedPtr, DNode **firstAddedPtr,
                         int *firstAddedDigit, char const *num) {
    DNode *result = start;
    DNode *beforeFirstAdded = *beforeFirstAddedPtr;
    DNode *firstAdded = *firstAddedPtr;
    size_t i = 0;

    while (isValidDigit(num[i])) {
        int digit = toDecimalRepresentation(num[i]);
        if (result->next[digit] == NULL) {
            result->next[digit] = nodeNew();
            if (result->next[digit] == NULL) {
                if (beforeFirstAdded != NULL) {
                    beforeFirstAdded->next[*firstAddedDigit] = NULL;
                }
                deleteIterative(firstAdded);
                return NULL;
            }

            result->next[digit]->parent = result;
            if (firstAdded == NULL) {
                beforeFirstAdded = result;
                (*firstAddedDigit) = digit;
                firstAdded = result->next[digit];
            }
        }
        result = result->next[digit];
        i++;
    }

    *beforeFirstAddedPtr = beforeFirstAdded;
    *firstAddedPtr = firstAdded;
    return result;
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

/**
 * @brief Creates new PhoneNumbers structure.
 * Creates new structure without any numbers.
 * @return Pointer to the new structure or NULL if there was an allocation error.
 */
static PhoneNumbers *phnumNew(void) {
    PhoneNumbers *numbers = malloc(sizeof(struct PhoneNumbers));
    if (numbers == NULL) {
        return NULL;
    }

    numbers->array = NULL;
    numbers->size = 0;
    numbers->capacity = 1;
    return numbers;
}

/** @brief Adds phone number to the vector.
 * Allocates memory (if needed) for the new number and adds it to the array.
 * @param [in, out] pNumbers - pointer to the structure of phone numbers.
 * @param [in] number - pointer to the number to add.
 * @return Value @p true if the number was added successfully.
 *         Value @p false if there was an allocation error.
 */
static bool phnumAdd(PhoneNumbers *pNumbers, char **number) {
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

/**
 * @brief Overwrites the forwarding in the node.
 * This function will overwrite the forwarding in the given node (or create it if it doesn't exist). If there was an
 * allocation error, the function will also delete all the nodes that were created down to this node.
 * @param [in] node - pointer to the node to overwrite the forwarding in.
 * @param [in, out] beforeFirstAdded - pointer to the node before the first added node.
 * @param [in, out] firstAdded - pointer to the first added node.
 * @param [in] firstAddedDigit - digit of the first added node.
 * @param [in] num - the number to overwrite current number in the node with.
 * @param [in, out] overWritten - pointer to the string representing a number that was overwritten.
 * @return Value @p true if the number was overwritten successfully.
 *         Value @p false if there was an allocation error.
 */
static bool overWriteForwarding(DNode *node, DNode *beforeFirstAdded, DNode *firstAdded, int firstAddedDigit,
                                char const *num, char **overWritten) {
    char *result = NULL;
    result = malloc(sizeof(char) * (length(num) + 1));
    if (result == NULL) {
        if (beforeFirstAdded != NULL) {
            beforeFirstAdded->next[firstAddedDigit] = NULL;
        }
        deleteIterative(firstAdded);
        return false;
    }
    size_t i = 0;

    while (isValidDigit(num[i])) {
        result[i] = num[i];
        i++;
    }
    result[i] = '\0';

    if (node->numbers != NULL && node->numbers->size > 0 &&
        !copyNumber(node->numbers->array[0].number, overWritten)) {
        if (beforeFirstAdded != NULL) {
            beforeFirstAdded->next[firstAddedDigit] = NULL;
        }
        deleteIterative(firstAdded);
        free(result);
        return false;
    }

    phnumDelete(node->numbers);

    node->numbers = phnumNew();
    if (node->numbers == NULL || !phnumAdd(node->numbers, &result)) {
        if (beforeFirstAdded != NULL) {
            beforeFirstAdded->next[firstAddedDigit] = NULL;
        }
        deleteIterative(firstAdded);
        free(*overWritten);
        *overWritten = NULL;
        free(result);
        return false;
    }
    return true;
}

/**
 * @brief Adds the number to the node.
 * This function will add the (reverse) number to the vector of numbers in the given node (and create the vector if
 * it doesn't exist). If there was an allocation error, the function will also delete all the nodes that were created
 * down to this node.
 * @param [in] node - pointer to the node to add the number to.
 * @param [in, out] beforeFirstAdded - pointer to the node before the first added node in the reverse tree.
 * @param [in, out] firstAdded - pointer to the first added node in the reverse tree.
 * @param [in] firstAddedDigit - digit of the first added node in the reverse tree.
 * @param [in] num - the number to add to the node.
 * @return Value @p true if the number was added successfully.
 *         Value @p false if there was an allocation error.
 */
static bool addReverse(DNode *node, DNode *beforeFirstAdded, DNode *firstAdded, int firstAddedDigit, char const *num) {
    char *result = NULL;
    result = malloc(sizeof(char) * (length(num) + 1));
    if (result == NULL) {
        if (beforeFirstAdded != NULL) {
            beforeFirstAdded->next[firstAddedDigit] = NULL;
        }
        deleteIterative(firstAdded);
        return false;
    }
    size_t i = 0;

    while (isValidDigit(num[i])) {
        result[i] = num[i];
        i++;
    }
    result[i] = '\0';

    if (node->numbers == NULL) {
        node->numbers = phnumNew();
        if (node->numbers == NULL) {
            if (beforeFirstAdded != NULL) {
                beforeFirstAdded->next[firstAddedDigit] = NULL;
            }
            deleteIterative(firstAdded);
            free(result);
            return false;
        }
    }

    if (!phnumAdd(node->numbers, &result)) {
        if (beforeFirstAdded != NULL) {
            beforeFirstAdded->next[firstAddedDigit] = NULL;
        }
        deleteIterative(firstAdded);
        free(result);
        return false;
    }
    return true;
}

/**
 * @brief Removes number from the vector.
 * This function will remove the number from the given vector. If the vector is then empty it will delete the vector
 * structure.
 * @param [in, out] pNumbersPtr - pointer to the vector to remove the number from.
 * @param [in] num - the number to remove from the vector.
 */
static void phnumRemove(PhoneNumbers **pNumbersPtr, char const *num) {
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

/**
 * @brief Removes a number from the reverse tree.
 * This function will go to the node at the end of route represent the number and remove the number from the vector
 * in that node. If the vector is then empty it will delete the vector structure and the route to the node (starting
 * from the point that can be safely deleted).
 * @param [in, out] pf - pointer to structure that holds the reverse tree.
 * @param [in] num1 - the number representing the route to the node.
 * @param [in] num2 - the number we want to remove from the vector.
 */
static void removeReverse(PhoneForward const *pf, char const *num1, char const *num2) {
    DNode *node = pf->reverseRoot;
    DNode *beforePointToRemove = pf->reverseRoot;
    DNode *lastPointToRemove = NULL;
    int pointToRemoveDigit = 0;
    size_t i = 0;

    while (isValidDigit(num1[i])) {
        int digit = toDecimalRepresentation(num1[i]);
        if (node->next[digit] == NULL) {
            return;
        }
        if (node->numbers != NULL || numberOfChildren(node) > 1 || lastPointToRemove == NULL) {
            beforePointToRemove = node;
            pointToRemoveDigit = digit;
            lastPointToRemove = node->next[digit];
        }
        node = node->next[digit];
        i++;
    }

    phnumRemove(&node->numbers, num2);

    if (node->numbers == NULL) {
        if (beforePointToRemove != NULL) {
            beforePointToRemove->next[pointToRemoveDigit] = NULL;
        }
        deleteIterative(lastPointToRemove);
    }
}

static void printPhnum(PhoneNumbers const *pNumbers) {
    for (size_t i = 0; i < pNumbers->size; i++) {
        printf("%s ", pNumbers->array[i].number);
    }
    printf("\n");
}

static void printAll(DNode *node) {
    if (node == NULL) {
        return;
    }
    if (node->numbers != NULL) {
        printPhnum(node->numbers);
    }
    for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
        if (node->next[i] != NULL) printf("\t%d: ", i);
        printAll(node->next[i]);
    }
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (pf == NULL || !checkNumbers(num1, num2)) {
        return false;
    }

    DNode *beforeFirstAdded = NULL;
    DNode *firstAdded = NULL;
    int firstAddedDigit;
    char *overWrittenNumber = NULL;
    DNode *node = getEndNode(pf->root, &beforeFirstAdded, &firstAdded, &firstAddedDigit, num1);
    if (node == NULL || !overWriteForwarding(node, beforeFirstAdded, firstAdded, firstAddedDigit, num2,
                                             &overWrittenNumber)) {
        return false;
    }

    if (overWrittenNumber != NULL) {
        removeReverse(pf, overWrittenNumber, num1);
    }
    free(overWrittenNumber);

    DNode *beforeFirstReverseAdded = NULL;
    DNode *firstReverseAdded = NULL;
    int firstReverseAddedDigit;
    DNode *nodeReverse = getEndNode(pf->reverseRoot, &beforeFirstReverseAdded, &firstReverseAdded,
                                    &firstReverseAddedDigit, num2);

    if (nodeReverse == NULL || !addReverse(nodeReverse, beforeFirstReverseAdded, firstReverseAdded,
                                           firstReverseAddedDigit, num1)) {
        if (beforeFirstAdded != NULL) {
            beforeFirstAdded->next[firstAddedDigit] = NULL;
        }
        deleteIterative(firstAdded);
        return false;
    }

    /*printf("Forward:\n");
    printAll(pf->root);
    printf("Reverse:\n");
    printAll(pf->reverseRoot);*/

    return true;
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
        if (node->numbers != NULL || numberOfChildren(node) > 1 || lastPointToRemove == NULL) {
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
static bool copyParts(char const *num, char const *forwardedPrefix, size_t lenOfOriginalPrefix, char **numberPtr) {
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
static void findPrefix(PhoneForward const *pf, char const *num, char **maxForwardedPrefix,
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

        if (node->numbers != NULL && node->numbers->size > 0) {
            (*maxForwardedPrefix) = node->numbers->array[0].number;
            (*lenOfMaxOriginalPrefix) = i;
        }
    }
}

PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }

    PhoneNumbers *pn = phnumNew();
    if (pn == NULL) {
        return NULL;
    }

    if (!isNumber(num)) {
        return pn;
    }

    char *maxForwardedPrefix = NULL;
    size_t lenOfMaxOriginalPrefix = 0;
    findPrefix(pf, num, &maxForwardedPrefix, &lenOfMaxOriginalPrefix);

    char *number = NULL;
    if (maxForwardedPrefix == NULL) {
        if (!copyNumber(num, &number)) {
            phnumDelete(pn);
            return NULL;
        }
    } else {
        if (!copyParts(num, maxForwardedPrefix, lenOfMaxOriginalPrefix, &number)) {
            phnumDelete(pn);
            return NULL;
        }
    }

    if (!phnumAdd(pn, &number)) {
        free(number);
        phnumDelete(pn);
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

    //qsort(pn->array, pn->size, sizeof(PNumber), compareNumbers);

    return pn->array[idx].number;
}