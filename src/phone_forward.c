/** @file
 * Implementations of phone forwarding functions.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
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
            nodeSetNext(beforeFirstAdded, firstAddedDigit, NULL);
        }
        deleteIterative(firstAdded);
        return false;
    }

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
        if (nodeGetNext(node, digit) == NULL) {
            return;
        }
        if (nodeGetNumbers(node) != NULL || numberOfChildren(node) > 1 || lastPointToRemove == NULL) {
            beforePointToRemove = node;
            pointToRemoveDigit = digit;
            lastPointToRemove = nodeGetNext(node, digit);
        }
        node = nodeGetNext(node, digit);
        i++;
    }

    if (beforePointToRemove != NULL) {
        nodeSetNext(beforePointToRemove, pointToRemoveDigit, NULL);
    }
    deleteIterativeWithReverse(pf->reverseRoot, lastPointToRemove, num);
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

PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {
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

    if (!addAllFromReverseTree(pf->reverseRoot, num, pn)) {
        phnumDelete(pn);
        return NULL;
    }

    sortPhoneNumbers(pn);
    removeDuplicates(pn);

    return pn;
}