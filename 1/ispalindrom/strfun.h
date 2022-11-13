/**
 * String functions module definitions.
 * @brief Provides common utility functions for manipulating strings.
 * @details Includes string operations to trim strings, remove newlines and also transform it to lowercase.
 * @file strfun.h
 * @author Tobias Gruber, 11912367
 * @date 12.10.2022
 */

/**
 * @brief Removes the newline at the end of a string.
 * @details Replaces the newline character at the end of src with a null terminator.
 * @param src String that should be manipulated.
 */
void remove_newline(char src[]);

/**
 * @brief Removes all whitespaces from a string.
 * @details Copies each character, that is no whitespace, from src to res.<br>
 * Note that res must provide sufficient space.
 * @param res String where the result is written to.
 * @param src Source string to be evaluated.
 */
void trim(char res[], char src[]);

/**
 * @brief Converts all chars of a string to lower case.
 * @brief Replaces each character of src with its' lowercase version.
 * @param src String to be updated.
 */
void to_lower(char src[]);
