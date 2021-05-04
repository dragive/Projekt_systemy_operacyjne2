/**
 * @file file.c
 * @author Maciej Fender & Funkowski Krzysztof
 * @brief file handeling module
 * @version 1.0
 * @date 2021-03-20
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "singleLine.h"
#include "file.h"


/** @brief Get the new character from file and if reach end of file than change status.
 *
 * @param file File which character will be read from
 * @return char Character read from file
 * @author KF
 */
char get_new_character_from_file(int file,int* status){
    char buffer;
    ssize_t c;
    ///reading character until read function returns 0
    if(c = read(file,&buffer,1)>0)
    {
        ///returning read character
        return buffer;
    }
    /// when 0 bytes is read, status 1 is set
    *status=1;
    return 'x';
}

/** @brief Use it to read line from file.
 * @param file_link File which line will be read from
 * @param status Using to check status of reaching the end of file
 * @return Function returns singleLine struct object with read line
 * @author KF
 */
singleLine* get_line_from_file(int file_link,int* status){
    singleLine* sl = (singleLine*) malloc(sizeof(singleLine));
    ///initializing value of single line
    sl->value=NULL;
    char read;
    ///Reading characters until \n is read
    while((read = get_new_character_from_file(file_link,status))!='\n')
    {

        ///if end of file or 0 bytes were read
        if(*status==1)
        {
            ///freeing sl
            free(sl);
            sl=NULL;
            break;
        }
        ///adding character to single line struct after being read
        add_char_to_singleLine(sl,read);
    }
    return sl;
}

/** \brief Function splits string to array of strings that representing particular objects
 *
 * \param input_line line that will be converted to array of strings
 * \param number_of_words how many words will be in the result array
 * \param separator char used as devider
 * \return array of separeted strings
 * \author KF
 */
char ** split(char* input_line, int number_of_words, char separator)
{
    int i=0, length=0, word=0;
    ///initializing array which will be returned
    char** str_tab = (char**)malloc(number_of_words*sizeof(char*));
    char* temp_str;
     ///allocating memory for fields
    for(i=0; i<number_of_words;i++)
    {
        ///malloc allocate max size of chars array of lenght of source array to every possible filed.
        str_tab[i] = (char*)malloc(strlen(input_line)*sizeof(char));
    }

    ///initializing i before loop
    i=0;

    ///while until end of line
    while(input_line[i]!='\0')
    {
        ///if there's separator in curent char
        if(input_line[i]==separator)
        {
            ///ending last filed of created array
            str_tab[word][length]='\0';
            ///going to next fiels
            word++;
            ///setting begin of field to 0
            length=0;
        }
        ///continuing rewriting chars to current field
        else
        {
            ///after assign lenght od of string is increased
            str_tab[word][length++]=input_line[i];
        }
        ///going to next char from source
        i++;
    }
    ///ending last word with end of line character
    str_tab[word][length]='\0';
    return str_tab;
}

/** \brief Function spliting commandline to different fields
 *
 * \param str Line from file in char table form.
 * \return table of different fields in commands
 * @author Krzysztof Funkowski & Maciej Fender
 */

char** split_command_line(const char* str)
{
    int i;
    /// allocating memory
    char** tab = (char**)malloc(4*sizeof(char*));
    tab[0] = (char*)malloc(3*sizeof(char));
    tab[1] = (char*)malloc(3*sizeof(char));
    tab[2] = (char*)malloc((strlen(str)-7)*sizeof(char));
    tab[3] = (char*)malloc(2*sizeof(char));
    tab[0][0]=str[0];///hour
    tab[0][1]=str[1];///hour
    tab[1][0]=str[3];///minute
    tab[1][1]=str[4];///minute
    tab[0][2]='\0';///ending hour field
    tab[1][2]='\0';///ending minute field
    tab[3][0]=str[strlen(str)-1];
    tab[3][1]='\0'; ///type of writing output of command and ending it with end of line
    for(i=6;i<strlen(str)-2;i++)///rewriting command / program lunch command
    {
        tab[2][i-6]=str[i];
    }
    tab[2][strlen(str)-2-6]='\0'; ///ending command with end of line
    return tab;
}


/**
 * @brief Function used to open input file for reading
 * @param file_str file name to open
 * @return int Value returned is file descriptor. If error occurs, return value -1.
 * @author Krzysztof Funkowski
 */
int open_read_file(const char* file_str)
{
    int file;
    ///opening file and returning -1 if something is going wrong
    if((file = open(file_str, O_RDONLY))<0) return -1;

    return file;
}
