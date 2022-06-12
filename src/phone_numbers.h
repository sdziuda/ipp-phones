/** @file
 * Interface of the class storing phone numbers.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __PHONE_NUMBERS_H__
#define __PHONE_NUMBERS_H__

#include <stdbool.h>
#include <stddef.h>

struct PhoneNumbers;
typedef struct PhoneNumbers PhoneNumbers; /**< Vector containing an array of phone numbers. */

/**
 * @brief Creates new PhoneNumbers structure.
 * Creates new structure without any numbers.
 * @return Pointer to the new structure or NULL if there was an allocation error.
 */
PhoneNumbers *phnumNew(void);

/**
 * @brief Obtains size of the phone numbers vector.
 * Obtains the size of the phone numbers vector stored by its structure.
 * @param [in] pNumbers - vector of phone numbers.
 * @return Value of the size of the vector.
 */
size_t phnumGetSize(PhoneNumbers const *pNumbers);

/** @brief Adds phone number to the vector.
 * Allocates memory (if needed) for the new number and adds it to the array.
 * @param [in, out] pNumbers - pointer to the structure of phone numbers.
 * @param [in] number - pointer to the number to add.
 * @return Value @p true if the number was added successfully.
 *         Value @p false if there was an allocation error.
 */
bool phnumAdd(PhoneNumbers *pNumbers, char **number);

/**
 * @brief Removes number from the vector.
 * This function will remove the number from the given vector. If the vector is then empty it will delete the vector
 * structure.
 * @param [in, out] pNumbersPtr - pointer to the vector to remove the number from.
 * @param [in] num - the number to remove from the vector.
 */
void phnumRemove(PhoneNumbers **pNumbersPtr, char const *num);

/**
 * @brief Removes all the numbers containing the given prefix.
 * This function is used to delete all phone numbers that contain the given prefix from the vector. If there are no
 * numbers left in the vector, the vector is deleted.
 * @param [in, out] pNumbersPtr - pointer to the vector of phone numbers.
 * @param [in] prefix - the prefix of numbers we want to remove.
 */
void phnumRemoveWithPrefix(PhoneNumbers **pNumbersPtr, char const *prefix);

/**
 * @brief Removes phone number at the given index.
 * This function will free the memory allocated for the phone number at the given index and then shift all other
 * phone numbers in the array to the left.
 * @param [in, out] pNumbers - pointer to the phone numbers structure.
 * @param [in] index - index of the phone number to be removed.
 */
void phnumRemoveAtIndex(PhoneNumbers *pNumbers, size_t index);

/**
 * @brief Adds all numbers with changed prefix to the vector.
 * This function will create a copy of the original number, with a changed prefix, stored in one vector (using
 * @ref copyParts), and add these new numbers to the other vector.
 * @param [in] from - vector containing the prefixes.
 * @param [in, out] to - vector to add the new numbers to.
 * @param [in] originalNumber - the original number we want to change prefixes in.
 * @param [in] prefixLength - the length of the prefix we want to change in the original number.
 * @return Value @p true if the numbers were added successfully.
 *         Value @p false if there was an allocation error.
 */
bool phnumAddAllCopiedParts(PhoneNumbers *from, PhoneNumbers *to, char const *originalNumber, size_t prefixLength);

/**
 * @brief Sorts the vector of phone numbers.
 * @param [in] pNumbers - pointer to the vector to sort.
 */
void sortPhoneNumbers(PhoneNumbers *pNumbers);

/**
 * @brief Removes duplicates from the vector.
 * @param [in, out] pNumbers - pointer to the vector to remove duplicates from.
 */
void removeDuplicates(PhoneNumbers *pNumbers);

#endif /* __PHONE_NUMBERS_H__ */
