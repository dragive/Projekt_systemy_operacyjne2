/**
 * @file singleLine.c
 * @author Maciej Fender
 * @brief singleLine is meant to handle reading line from file
 * @version 1.0
 * @date 2021-03-20
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "singleLine.h"


/** \brief Function extends buffor in single line
 *
 * \param sl singleLine struct object which buffor will be extended
 * \author MF
 */
void extend_buffor_singleLine(singleLine* sl){
    ///allocating bigger buffor
    char * temp = (char* ) malloc(sizeof(char)*(sl->size_max+10));
    ///rewriting to bigger buffor from source
    for(int i = 0;i<sl->size_max;i++){
        temp[i] = sl->value[i];
    }
    ///freeing old table
    free(sl->value);
    ///storing new table in struct
    sl->value = temp;
    ///storing updated size
    sl->size_max += 10;
}

/** \brief Function adds character to singleLine
 *
 * \param sl singleLine struct pointer which char will be written to from file
 * \param c character which will be added to singleLine
 * \author MF
 */
void add_char_to_singleLine(singleLine * sl,char c){
    ///if value of struct is null, initializing variables and buffor
    if(sl->value == NULL){
        ///initial size is 10 chars
        sl->size_max = 10;
        ///begin is set to 0
        sl->size_current = 0;
        /// allocating buffor to store chars
        sl->value = (char*) malloc(sizeof(char)*sl->size_max);
        ///begin is set to end of line
        sl->value[sl->size_current]='\0';
    }
    ///if more space is needed buffor is extended
    if(sl->size_current ==  sl->size_max-1){
        extend_buffor_singleLine(sl);
    }
    ///adding new char and moving EOL to next character
    sl->value[sl->size_current++] = c;
    sl->value[sl->size_current] = '\0';
}

