#include <stdlib.h>
#include <ctype.h>
#include "phone_forward.h"

#define NUMBER_OF_DIGITS 10

struct Node {
    char *forwardedNumber;
    struct Node *next[NUMBER_OF_DIGITS];
};
typedef struct Node DNode;

struct PhoneForward {
    DNode *root;
};

struct PhoneNumber {
    char *number;
    struct PhoneNumber *next;
};
typedef struct PhoneNumber PNumber;

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

void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL) {
        return;
    }

    deleteAllNodesUnder(pf->root);

    free(pf);
}

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

static bool areEqual(char const *num1, char const *num2) {
    size_t len1 = 0;
    size_t len2 = 0;

    while (num1[len1] != '\0') {
        len1++;
    }
    while (num2[len2] != '\0') {
        len2++;
    }

    if (len1 != len2) {
        return false;
    }

    int i = 0;
    while (num1[i] != '\0' && num2[i] != '\0') {
        if (num1[i] != num2[i]) {
            return false;
        }
        i++;
    }

    return true;
}

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
    int i = 0;
    while (num1[i] != '\0') {
        int digit = num1[i] - '0';
        if (node->next[digit] == NULL) {
            node->next[digit] = malloc(sizeof(DNode));
            if (node->next[digit] == NULL) {
                return false;
            }
            node->next[digit]->forwardedNumber = NULL;
            for (int j = 0; j < NUMBER_OF_DIGITS; j++) {
                node->next[digit]->next[j] = NULL;
            }
        }
        node = node->next[digit];
        i++;
    }

    node->forwardedNumber = malloc(sizeof(num2) + sizeof(char));
    if (node->forwardedNumber == NULL) {
        return false;
    }

    i = 0;
    while (num2[i] != '\0') {
        node->forwardedNumber[i] = num2[i];
        i++;
    }
    node->forwardedNumber[i] = '\0';

    return true;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (!isNumber(num)) {
        return;
    }

    DNode *node = pf->root;
    DNode *prev = NULL;
    int i = 0;
    while (num[i] != '\0') {
        int digit = num[i] - '0';
        if (node->next[digit] == NULL) {
            return;
        }
        prev = node;
        node = node->next[digit];
        i++;
    }

    prev->next[num[i - 1] - '0'] = NULL;
    deleteAllNodesUnder(node);
}

static inline bool copyNumber(const char *num, char **numberPtr) {
    char *number = malloc(sizeof(num) + sizeof(char));
    if (number == NULL) {
        return false;
    }

    size_t i = 0;
    while (num[i] != '\0') {
        number[i] = num[i];
        i++;
    }
    number[i] = '\0';

    *numberPtr = number;
    return true;
}

static inline bool copyParts(const char *num,
                             char const *forwardedPrefix,
                             size_t lenOfOriginalPrefix,
                             char **numberPtr) {

    char *number = malloc(sizeof(forwardedPrefix) + sizeof(num) - lenOfOriginalPrefix * sizeof(char) + sizeof(char));
    if (number == NULL) {
        return false;
    }

    size_t i = 0;
    while (forwardedPrefix[i] != '\0') {
        number[i] = forwardedPrefix[i];
        i++;
    }

    size_t j = lenOfOriginalPrefix;
    while (num[j] != '\0') {
        number[i] = num[j];
        i++;
        j++;
    }
    number[i] = '\0';

    *numberPtr = number;
    return true;
}

static inline void findPrefix(const PhoneForward *pf,
                              const char *num,
                              char **maxForwardedPrefix,
                              size_t *lenOfMaxOriginalPrefix) {

    DNode *node = pf->root;
    size_t i = 0;

    while (num[i] != '\0') {
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
    if (!isNumber(num)) {
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
    return tmp->number;
}