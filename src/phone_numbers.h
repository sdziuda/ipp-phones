/** @file
 * Interface of the class containing phone numbers.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __PHONE_NUMBERS_H__
#define __PHONE_NUMBERS_H__

#include <stdbool.h>
#include <stddef.h>

/**
 * This is a structure containing a single phone number.
 */
struct PhoneNumber;
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
typedef struct PhoneNumbers PhoneNumbers; /**< Vector containing an array of phone numbers. */

/**
 * @brief Creates new PhoneNumbers structure.
 * Creates new structure without any numbers.
 * @return Pointer to the new structure or NULL if there was an allocation error.
 */
PhoneNumbers *phnumNew(void);

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

#endif /* __PHONE_NUMBERS_H__ */
