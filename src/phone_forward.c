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
#include "string_utils.h"
#include "phone_numbers.h"
#include "node_utils.h"

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

void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL) {
        return;
    }

    deleteIterative(pf->root);
    deleteIterative(pf->reverseRoot);

    free(pf);
}

/*static void printPhnum(PhoneNumbers const *pNumbers) {
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
}*/

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
        removeReverse(pf->reverseRoot, overWrittenNumber, num1);
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
    deleteIterativeWithReverse(pf->reverseRoot, lastPointToRemove, num);

    /*printf("Forward:\n");
    printAll(pf->root);
    printf("Reverse:\n");
    printAll(pf->reverseRoot);*/
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

    char const *maxForwardedPrefix = NULL;
    size_t lenOfMaxOriginalPrefix = 0;
    findPrefix(pf->root, num, &maxForwardedPrefix, &lenOfMaxOriginalPrefix);

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