/**
 * @file file.h
 * @author Maciej Fender & Funkowski Krzysztof
 * @brief file handeling module
 * @version 1.0
 * @date 2021-03-20
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef FILE_INCLUDED
#define FILE_INCLUDED


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "singleLine.h"


/** @brief Get the new character from file and if reach end of file than change status.
 *
 * @param file File which character will be read from
 * @return char Character read from file
 * @author KF
 */
char get_new_character_from_file(int file,int* status);

/** @brief Use it to read line from file.
 * @param file_link File which line will be read from
 * @param status Using to check status of reaching the end of file
 * @return Function returns singleLine struct object with read line
 * @author KF
 */
singleLine* get_line_from_file(int file_link,int* status);
/** \brief Function splits string to array of strings that representing particular objects
 *
 * \param input_line line that will be converted to array of strings
 * \param number_of_words how many words will be in the result array
 * \param separator char used as devider
 * \return array of separeted strings
 * \author KF
 */
char ** split(char* input_line, int number_of_words, char separator);

/** \brief Function spliting commandline to different fields
 *
 * \param str Line from file in char table form.
 * \return table of different fields in commands
 */

char** split_command_line(const char* str);
/**
 * @brief Function used to open input file for reading
 * @param file_str file name to open
 * @return int Value returned is file descriptor. If error occurs, return value -1.
 * @author Krzysztof Funkowski
 */
int open_read_file(const char* file_str);

#endif // FILE_INCLUDED

