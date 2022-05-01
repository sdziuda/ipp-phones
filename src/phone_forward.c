/** @file
 * Implementations of phone forward functions.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdlib.h>
#include <ctype.h>
#include "phone_forward.h"

#define NUMBER_OF_DIGITS 10 /**< Number of different digits. */

/**
 * @struct Node
 * @brief Node of the tree of phone forwarding.
 * @var Node::forwardedNumber
 *      String of digits containing the number to which the route from root to the current node is forwarded.
 * @var Node::next
 *      Array of pointers to the 10 children nodes in the tree.
 */
struct Node {
    char *forwardedNumber;
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
 * @brief Structure containing a node of the linked list of phone numbers.
 * @var PhoneNumber::number
 *      String containing a phone number.
 * @var PhoneNumber::next
 *      Pointer to the next node in the linked list.
 */
struct PhoneNumber {
    char *number;
    struct PhoneNumber *next;
};
typedef struct PhoneNumber PNumber; /**< type of the linked list of phone numbers. */

/**
 * @struct PhoneNumbers phone_forward.h
 * @brief Structure containing a linked list of phone numbers.
 * @var PhoneNumbers::first
 *      Pointer to the first node in the linked list.
 * @var PhoneNumbers::last
 *      Pointer to the last node in the linked list.
 * @var PhoneNumbers::size
 *      Number of nodes in the linked list.
 */
struct PhoneNumbers {
    PNumber *first;
    PNumber *last;
    size_t size;
};

PhoneForward *phfwdNew(void) {
    PhoneForward *pf = malloc(sizeof(PhoneForward));

    pf->root = malloc(sizeof(DNode));
    if (pf->root == NULL) {
        free(pf);
        return NULL;
    }
    pf->root->forwardedNumber = NULL;
    for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
        pf->root->next[i] = NULL;
    }

    return pf;
}

/**
 * @brief Deletes all the nodes in the tree under the given node and the node itself.
 * @param [in, out] node - pointer to the node to delete all the nodes under.
 */
static void deleteAllNodesUnder(DNode *node) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
        deleteAllNodesUnder(node->next[i]);
    }

    free(node->forwardedNumber);
    free(node);
}

/**
 * @brief Deletes the nodes in phone forwarding tree.
 * First changes the pointer to the child in the parent node to NULL. Then deletes all the nodes in the tree under the
 * given node and the node itself using @ref deleteAllNodesUnder function.
 * @param [in, out] parent - pointer to the parent of the node we want to delete.
 * @param [in] digit - digit of the node we want to delete.
 * @param [in, out] child - pointer to the node we want to delete.
 */
static void deleteChildAndNodesUnder(DNode *parent, int digit, DNode *child) {
    if (parent == NULL || child == NULL) {
        return;
    }

    parent->next[digit] = NULL;
    deleteAllNodesUnder(child);
}

void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL) {
        return;
    }

    deleteAllNodesUnder(pf->root);

    free(pf);
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

    int i = 0;
    while (isdigit(number[i])) {
        i++;
    }

    if (i == 0 || number[i] != '\0') {
        return false;
    }
    return true;
}

/**
 * @brief Checks whether the given numbers are equal.
 * @param [in] num1 - first number.
 * @param [in] num2 - second number.
 * @return Value @p true if the numbers are equal.
 *         Value @p false otherwise.
 */
static bool areEqual(char const *num1, char const *num2) {
    size_t len1 = 0;
    size_t len2 = 0;

    while (isdigit(num1[len1])) {
        len1++;
    }
    while (isdigit(num2[len2])) {
        len2++;
    }

    if (len1 != len2) {
        return false;
    }

    int i = 0;
    while (isdigit(num1[i]) && isdigit(num2[i])) {
        if (num1[i] != num2[i]) {
            return false;
        }
        i++;
    }

    return true;
}

/**
 * @brief Checks whether the numbers are suitable for forwarding.
 * @param [in] num1 - first number.
 * @param [in] num2 - second number.
 * @return Value @p true if they are suitable.
 *         Value @p false otherwise.
 */
static bool checkNumbers(char const *num1, char const *num2) {
    if (!isNumber(num1) || !isNumber(num2) || areEqual(num1, num2)) {
        return false;
    }
    return true;
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (!checkNumbers(num1, num2)) {
        return false;
    }

    DNode *node = pf->root;
    DNode *beforeFirstAdded = NULL;
    DNode *firstAdded = NULL;
    int i = 0;
    int firstAddedDigit = 0;
    while (isdigit(num1[i])) {
        int digit = num1[i] - '0';
        if (node->next[digit] == NULL) {
            node->next[digit] = malloc(sizeof(DNode));
            if (node->next[digit] == NULL) {
                deleteChildAndNodesUnder(beforeFirstAdded, firstAddedDigit, firstAdded);
                return false;
            }
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

    node->forwardedNumber = malloc(sizeof(num2) + sizeof(char));
    if (node->forwardedNumber == NULL) {
        deleteChildAndNodesUnder(beforeFirstAdded, firstAddedDigit, firstAdded);
        return false;
    }

    i = 0;
    while (isdigit(num2[i])) {
        node->forwardedNumber[i] = num2[i];
        i++;
    }
    node->forwardedNumber[i] = '\0';

    return true;
}

/**
 * @brief Finds the number od children for given node.
 * Finds all children that are not NULL and returns the number of them.
 * @param [in] node the node to find the number of children for.
 * @return the number of children.
 */
static inline int numberOfChildren(DNode *node) {
    int sum = 0;
    for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
        if (node->next[i] != NULL) {
            sum++;
        }
    }
    return sum;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (!isNumber(num)) {
        return;
    }

    DNode *node = pf->root;
    DNode *beforePointToRemove = pf->root;
    DNode *lastPointToRemove = NULL;
    int pointToRemoveDigit = 0;
    int i = 0;
    while (isdigit(num[i])) {
        int digit = num[i] - '0';
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

    deleteChildAndNodesUnder(beforePointToRemove, pointToRemoveDigit, lastPointToRemove);
}

/**
 * @brief Copies the number to the given buffer.
 * @param [in] num - the number to copy.
 * @param [in, out] numberPtr - the buffer to copy the number to.
 * @return Value @p true if the number was copied successfully.
 *         Value @p false if there was allocation error.
 */
static inline bool copyNumber(const char *num, char **numberPtr) {
    char *result = malloc(sizeof(num) + sizeof(char));
    if (result == NULL) {
        return false;
    }

    size_t i = 0;
    while (isdigit(num[i])) {
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
static inline bool copyParts(const char *num,
                             char const *forwardedPrefix,
                             size_t lenOfOriginalPrefix,
                             char **numberPtr) {

    char *result = malloc(sizeof(forwardedPrefix) + sizeof(num) - lenOfOriginalPrefix * sizeof(char) + sizeof(char));
    if (result == NULL) {
        return false;
    }

    size_t i = 0;
    while (isdigit(forwardedPrefix[i])) {
        result[i] = forwardedPrefix[i];
        i++;
    }

    size_t j = lenOfOriginalPrefix;
    while (isdigit(num[j])) {
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
static inline void findPrefix(const PhoneForward *pf,
                              const char *num,
                              char **maxForwardedPrefix,
                              size_t *lenOfMaxOriginalPrefix) {

    DNode *node = pf->root;
    size_t i = 0;

    while (isdigit(num[i])) {
        int digit = num[i] - '0';

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

PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL) {
        return NULL;
    }

    PhoneNumbers *pn = malloc(sizeof(PhoneNumbers));
    if (pn == NULL) {
        return NULL;
    }

    pn->first = malloc(sizeof(PNumber));
    if (pn->first == NULL) {
        free(pn);
        return NULL;
    }
    pn->first->number = NULL;
    pn->first->next = NULL;
    pn->size = 0;

    if (!isNumber(num)) {
        free(pn->first);
        return pn;
    }

    char *maxForwardedPrefix = NULL;
    size_t lenOfMaxOriginalPrefix = 0;
    findPrefix(pf, num, &maxForwardedPrefix, &lenOfMaxOriginalPrefix);

    char *number = NULL;
    if (maxForwardedPrefix == NULL) {
        if (!copyNumber(num, &number)) {
            free(pn->first);
            free(pn);
            return NULL;
        }
    } else {
        if (!copyParts(num, maxForwardedPrefix, lenOfMaxOriginalPrefix, &number)) {
            free(pn->first);
            free(pn);
            return NULL;
        }
    }

    pn->first->number = number;
    pn->last = pn->first;
    pn->size = 1;
    return pn;
}

void phnumDelete(PhoneNumbers *pn) {
    if (pn == NULL) {
        return;
    }

    for (size_t i = 0; i < pn->size; i++) {
        free(pn->first->number);
        PNumber *tmp = pn->first;
        pn->first = pn->first->next;
        free(tmp);
    }
    free(pn);
}

char const *phnumGet(PhoneNumbers const *pn, size_t idx) {
    if (pn == NULL || idx >= pn->size) {
        return NULL;
    }

    PNumber *tmp = pn->first;
    for (size_t i = 0; i < idx; i++) {
        tmp = tmp->next;
    }

    if (tmp == NULL) {
        return NULL;
    }
    return tmp->number;
}