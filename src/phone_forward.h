/** @file
 * Interface of the class storing phone forwarding.
 *
 * @authors Marcin Peczarski <marpe@mimuw.edu.pl>, Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __PHONE_FORWARD_H__
#define __PHONE_FORWARD_H__

#include <stdbool.h>
#include <stddef.h>

/**
 * This is a structure containing phone forwarding information.
 */
struct PhoneForward;
typedef struct PhoneForward PhoneForward; /**< Type of structure containing phone forwarding information. */

/**
 * This is a structure containing a sequence of phone numbers.
 */
struct PhoneNumbers;
typedef struct PhoneNumbers PhoneNumbers; /**< Type of structure containing a sequence of phone numbers. */

/** @brief Creates new structure.
 * Creates new structure without any phone forwarding information.
 * @return Pointer to the new structure or NULL if there was an allocation error.
 */
PhoneForward * phfwdNew(void);

/** @brief Deletes the structure.
 * Deletes the structure pointed by @p pf. Does nothing when the pointer is NULL.
 * @param[in] pf – pointer to the structure to be deleted.
 */
void phfwdDelete(PhoneForward *pf);

/** @brief Adds a phone forwarding.
 * Adds a phone forwarding for all numbers containing prefix @p num1, to numbers,
 * in which this prefix was replaced by prefix @p num2. Each number is its own prefix.
 * If phone forwarding with the same parameter @p num1 was already added, it gets replaced.
 * Relation of phone forwarding is not transitive.
 * @param[in,out] pf – pointer to the structure containing phone forwarding information.
 * @param[in] num1   – pointer to the prefix of the phone numbers to be forwarded.
 * @param[in] num2   – pointer to the prefix of the phone numbers to be forwarded to.
 * @return Value @p true, if the phone forwarding was added.
 *         Value @p false, if an error occurred, for example the given string doesn't represent a number,
 *         both numbers are identical, there was an allocation error or the given structure is NULL.
 */
bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2);

/** @brief Deletes a phone forwarding.
 * Deletes all phone forwards, in which parameter @p num is a prefix
 * of parameter @p num1 used while adding. If no such phone forwarding was added, the
 * given string doesn't represent a number or @p pf is NULL, nothing happens.
 * @param[in,out] pf – pointer to the structure containing phone forwarding information.
 * @param[in] num    – pointer to the prefix of the phone numbers to be deleted.
 */
void phfwdRemove(PhoneForward *pf, char const *num);

/** @brief Obtains the number after forwarding.
 * Obtains the phone forwarding of the given phone number. Finds the longest suitable
 * prefix. The result is a sequence containing no more than one number. If the given number
 * hasn't been forwarded, the result is the exact same number.
 * If the given string doesn't represent a number, the result is empty sequence.
 * Allocates structure @p PhoneNumbers, which must be then freed using @ref phnumDelete.
 * @param[in] pf  – pointer to the structure containing phone forwarding information.
 * @param[in] num – pointer to the string containing the phone number to be forwarded.
 * @return Pointer to the structure containing the sequence of phone numbers or NULL if
 *         there was an allocation error or the given structure is NULL.
 */
PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num);

/** @brief Obtains all forwards to the prefixes of the given number.
 * For each phone number, which is a prefix of the given number @p num, finds all
 * phone numbers that are forwarded to this prefix. The function also concatenates
 * each number that is forwarded to such prefix with the suffix of the given number
 * @p num. Additionally, the result sequence should contain @p num. Numbers in the
 * result sequence are sorted in lexicographic order and occur only once. If the
 * given string doesn't represent a number, the result is an empty sequence. Allocates
 * structure @p PhoneNumbers, which must be then freed using @ref phnumDelete.
 * @param[in] pf  – pointer to the structure containing phone forwarding information.
 * @param[in] num – pointer to the string containing the phone number to be reversed.
 * @return Pointer to the structure containing the sequence of phone numbers or NULL if
 *         there was an allocation error or the given structure is NULL.
 */
PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num);

/**
 * @brief Obtains inverse image of the phfwdGet() function.
 * Obtains a list of phone numbers, so that phone number @p x is a part of the result
 * if and only if @p phfwdGet(x) = @p num. This list is sorted in lexicographic order
 * and contains no duplicates. If the given string doesn't represent a number, the
 * result is an empty sequence. Allocates structure @p PhoneNumbers, which must be
 * then freed using @ref phnumDelete.
 * @param[in] pf - pointer to the structure containing phone forwarding information.
 * @param[in] num - pointer to the string containing the phone number to find inverse
 *                  image of.
 * @return Pointer to the structure containing the sequence of phone numbers or NULL if
 *         there was an allocation error or the given structure is NULL.
 */
PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num);

/** @brief Deletes the structure.
 * Deletes the structure pointed by @p pnum. Does nothing when the pointer is NULL.
 * @param[in] pnum – pointer to the structure to be deleted.
 */
void phnumDelete(PhoneNumbers *pnum);

/** @brief Gives access to the number.
 * Gives access to the pointer to the string representing a number. Numbers are indexed
 * from 0.
 * @param[in] pnum – pointer to the structure containing the sequence of phone numbers.
 * @param[in] idx  – index of the number to be accessed.
 * @return Pointer to the string representing a phone number. Value NULL, if pointer
 * @p pnum points to NULL or @p idx is out of range.
 */
char const * phnumGet(PhoneNumbers const *pnum, size_t idx);

#endif /* __PHONE_FORWARD_H__ */
