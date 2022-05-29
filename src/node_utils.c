/** @file
 * Implementations of functions regarding operations on nodes.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "phone_forward.h"
#include "phone_numbers.h"
#include "string_utils.h"
#include "node_utils.h"

DNode *nodeNew(void) {
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

void deleteIterative(DNode *node) {
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

void deleteIterativeWithReverse(DNode *deleteReverseStart, DNode *deleteForwardStart, char const *prefix) {
    DNode *current = deleteForwardStart;
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
                if (current->numbers != NULL && current->numbers->size > 0) {
                    removeReverseWithPrefix(deleteReverseStart, phnumGet(current->numbers, 0), prefix);
                }
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

            if (current->numbers != NULL && current->numbers->size > 0) {
                removeReverseWithPrefix(deleteReverseStart, phnumGet(current->numbers, 0), prefix);
            }
            phnumDelete(current->numbers);
            free(current);
            current = parent;
        }
    }
}

DNode *getEndNode(DNode *start, DNode **beforeFirstAddedPtr, DNode **firstAddedPtr, int *firstAddedDigit,
                  char const *num) {
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

int numberOfChildren(DNode const *node) {
    int sum = 0;
    for (int i = 0; i < NUMBER_OF_DIGITS; i++) {
        if (node->next[i] != NULL) {
            sum++;
        }
    }
    return sum;
}

bool overWriteForwarding(DNode *node, DNode *beforeFirstAdded, DNode *firstAdded, int firstAddedDigit, char const *num,
                         char **overWritten) {
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
        !copyNumber(phnumGet(node->numbers, 0), overWritten)) {
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

bool addReverse(DNode *node, DNode *beforeFirstAdded, DNode *firstAdded, int firstAddedDigit, char const *num) {
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

void removeReverse(DNode *start, char const *num1, char const *num2) {
    DNode *node = start;
    DNode *beforePointToRemove = start;
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

void removeReverseWithPrefix(DNode *start, char const *num, char const *prefix) {
    DNode *node = start;
    DNode *beforePointToRemove = start;
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

    phnumRemoveWithPrefix(&node->numbers, prefix);

    if (node->numbers == NULL && numberOfChildren(node) == 0) {
        if (beforePointToRemove != NULL) {
            beforePointToRemove->next[pointToRemoveDigit] = NULL;
        }
        deleteIterative(lastPointToRemove);
    }
}

void findPrefix(DNode *start, char const *num, char const **maxForwardedPrefix, size_t *lenOfMaxOriginalPrefix) {
    DNode *node = start;
    size_t i = 0;

    while (isValidDigit(num[i])) {
        int digit = toDecimalRepresentation(num[i]);

        if (node->next[digit] == NULL) {
            break;
        }

        node = node->next[digit];
        i++;

        if (node->numbers != NULL && node->numbers->size > 0) {
            (*maxForwardedPrefix) = phnumGet(node->numbers, 0);
            (*lenOfMaxOriginalPrefix) = i;
        }
    }
}

bool addAllFromReverseTree(DNode *start, char const *num, PhoneNumbers *pnum) {
    DNode *node = start;
    size_t i = 0;

    while (isValidDigit(num[i])) {
        int digit = toDecimalRepresentation(num[i]);

        if (node->next[digit] == NULL) {
            break;
        } else if (node->next[digit]->numbers != NULL) {
            if (!phnumAddAllCopiedParts(node->next[digit]->numbers, pnum, num, i + 1)) {
                return false;
            }
        }

        node = node->next[digit];
        i++;
    }

    char *numCopy = NULL;
    if (!copyNumber(num, &numCopy)) {
        return false;
    }
    if (!phnumAdd(pnum, &numCopy)) {
        free(numCopy);
        return false;
    }

    return true;
}