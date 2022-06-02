/** @file
 * Interface of the class storing the node.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __NODE_UTILS_H__
#define __NODE_UTILS_H__

#include <stdbool.h>
#include <stddef.h>
#include "phone_numbers.h"

/**
 * This is a structure storing a node in the tree of phone numbers.
 */
struct Node;
typedef struct Node DNode; /**< Node of the forwarding tree. */

/**
 * @brief Creates a new Node.
 * Allocates memory for the node and initializes it.
 * @return Pointer to the new node or NULL if there was an allocation error.
 */
DNode *nodeNew(void);

/**
 * @brief Obtains the vector of phone numbers from the node.
 * @param [in] node - pointer to the node.
 * @return The vector of phone numbers stored in the node.
 */
PhoneNumbers *nodeGetNumbers(DNode *node);

/**
 * @brief Obtains the next node in the tree.
 * Obtains the pointer to the next node at the given index.
 * @param [in] node - pointer to the node we want to obtain the next node from.
 * @param [in] digit - the index of the next node.
 * @return The pointer to the next node at the given index.
 */
DNode *nodeGetNext(DNode *node, int digit);

/**
 * @brief Sets the next node in the tree.
 * Sets the pointer to the next node at the given index.
 * @param [in] node - pointer to the node we want to set the next node for.
 * @param [in] digit - the index of the next node.
 * @param [in] next - pointer to node we want to set as the next node.
 */
void nodeSetNext(DNode *node, int digit, DNode *next);

/**
 * @brief Deletes the nodes in phone forwarding tree.
 * Starting from the given Node and going down the tree, deletes all the nodes in the tree under the given node and the
 * node itself.
 * @param [in, out] node - pointer to the node we want to start deleting from.
 */
void deleteIterative(DNode *node);

/**
 * @brief Removes nodes from the forward tree and numbers with certain prefix from the reverse tree.
 * This function will delete all the nodes under the given node and if any node contain a number, the function will
 * start looking in the reverse tree for numbers that contain the given prefix using function
 * @ref removeReverseWithPrefix.
 * @param [in] deleteReverseStart - pointer to the node we want to start deleting from in the reverse tree.
 * @param [in, out] deleteForwardStart - pointer to the node that we want to start deleting from in the forward tree.
 * @param [in] prefix - the prefix of numbers we want to remove.
 */
void deleteIterativeWithReverse(DNode *deleteReverseStart, DNode *deleteForwardStart, char const *prefix);

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
DNode *getEndNode(DNode *start, DNode **beforeFirstAddedPtr, DNode **firstAddedPtr, int *firstAddedDigit,
                  char const *num);

/**
* @brief Finds the number od children for given node.
* Finds all children that are not NULL and returns the number of them.
* @param [in] node the node to find the number of children for.
* @return the number of children.
*/
int numberOfChildren(DNode const *node);

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
bool overWriteForwarding(DNode *node, DNode *beforeFirstAdded, DNode *firstAdded, int firstAddedDigit, char const *num,
                         char **overWritten);

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
bool addReverse(DNode *node, DNode *beforeFirstAdded, DNode *firstAdded, int firstAddedDigit, char const *num);

/**
 * @brief Removes a number from the reverse tree.
 * This function will go to the node at the end of route represented by the number and remove the number from the vector
 * in that node. If the vector is then empty it will delete the vector structure and the route to the node (starting
 * from the point that can be safely deleted).
 * @param [in] start - pointer to the node at the beginning of the route.
 * @param [in] num1 - the number representing the route to the node.
 * @param [in] num2 - the number we want to remove from the vector.
 */
void removeReverse(DNode *start, char const *num1, char const *num2);

/**
 * @brief Finds the longest prefix of the number that is forwarded to another number.
 * This function is used to find the longest prefix of the number @p num that still has a number in the phone forward
 * tree starting from the node @p start. This prefix will be then used to change the number to the forwarded number
 * using @ref copyParts or@ref copyNumber.
 * @param [in] start - pointer to the node that we want to start searching from.
 * @param [in] num - the number we are finding prefix of.
 * @param [in, out] maxForwardedPrefix - number the longest prefix is forwarded to.
 * @param [in, out] lenOfMaxOriginalPrefix - length of the longest prefix.
 */
void findPrefix(DNode *start, char const *num, char const **maxForwardedPrefix, size_t *lenOfMaxOriginalPrefix);

/**
 * @brief Adds all numbers after the operation of reversing.
 * This function will function will go down the reverse tree and add all the numbers on its way, changing the prefix
 * and adding the new number to the given vector.
 * @param [in] start - pointer to the node that we want to start searching from.
 * @param [in] num - the number we are finding reverse forwards of.
 * @param [in, out] pnum - the vector to add the numbers to.
 * @return Value @p true if the numbers were added successfully.
 *         Value @p false if there was an allocation error.
 */
bool addAllFromReverseTree(DNode *start, char const *num, PhoneNumbers *pnum);

#endif /* __NODE_UTILS_H__ */
