/** @file
 * Interface of the class containing operations on strings.
 *
 * @author Szymon Dziuda <sd438422@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Checks if the given char is a digit.
 * Checks whether the given char is a representation of a digit (0-9, '*' or '#').
 * @param [in] c - char to check.
 * @return Value @p true if the given char is a digit, @p false otherwise.
 */
bool isValidDigit(char c);

/**
 * @brief Checks if the given string is a valid phone number.
 * @param [in] number - string to check
 * @return Value @p true if the string is a valid phone number.
 *         Value @p false otherwise.
 */
bool isNumber(char const *number);

/**
 * @brief Checks if two numbers are equal.
 * @param [in] num1 - first number to compare.
 * @param [in] num2 - second number to compare.
 * @return Value @p true if the two numbers are equal.
 *         Value @p false otherwise.
 */
bool areEqual(char const *num1, char const *num2);

/**
 * @brief Checks if one number is a prefix of another.
 * @param [in] num - number to check.
 * @param [in] prefix - prefix to check.
 * @return Value @p true if the number is a prefix of the other one.
 *         Value @p false otherwise.
 */
bool isPrefix(char const *num, char const *prefix);

/**
 * @brief Checks if the strings are suitable for forwarding.
 * The function checks if the string represent a valid phone number and if they are not the same.
 * @param [in] num1 - first number.
 * @param [in] num2 - second number.
 * @return Value @p true if they are suitable.
 *         Value @p false otherwise.
 */
bool checkNumbers(char const *num1, char const *num2);

/**
 * @brief Obtains length of a number.
 * Obtains length of the given number by counting its digits from the start.
 * @param [in] num - number to get length of.
 * @return Length of the number.
 */
size_t length(char const *num);

/**
 * @brief Obtains the decimal representation.
 * Obtains the decimal representation of the given char.
 * @param [in] c - digit to convert.
 * @return Decimal representation of the digit.
 */
int toDecimalRepresentation(char c);

/**
 * @brief Copies the number to the given buffer.
 * @param [in] num - the number to copy.
 * @param [in, out] numberPtr - the buffer to copy the number to.
 * @return Value @p true if the number was copied successfully.
 *         Value @p false if there was allocation error.
 */
bool copyNumber(char const *num, char **numberPtr);

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
bool copyParts(char const *num, char const *forwardedPrefix, size_t lenOfOriginalPrefix, char **numberPtr);

#endif /* __STRING_UTILS_H__ */
