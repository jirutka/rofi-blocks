/**
 * rofi-blocks
 *
 * MIT/X11 License
 * Copyright (c) 2020 Omar Castro <omar.castro.360@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <gmodule.h>
#include "string_utils.h"

/******
  private method references
******/


static void str_escape_for_json_string(const char *in, char *out);


/******
  public method definitions
******/


// Result is an allocated a new string
char *str_replace(const char *orig, const char *rep, const char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *remainder; // remainder point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = remainder = (char *) orig;
    count = 0;
    tmp = strstr(ins, rep);
    while(tmp){
        ins = tmp + len_rep;
	    tmp = strstr(ins, rep);
	    count++;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(remainder, rep);
        len_front = ins - remainder;
        tmp = strncpy(tmp, remainder, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        remainder += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, remainder);
    return result;
}

char *str_replace_in(char **orig, const char *rep, const char *with) {
    char * result = str_replace(*orig, rep, with);
    if( result != NULL ){
        free(*orig);
        *orig = result;
    }
    return *orig;
}

char *str_replace_in_escaped(char **orig, const char *rep, const char *with) {
    int len = strlen(with);

    gchar * escaped_with = NULL;
    escaped_with = (char*)calloc(len*2, sizeof(gchar));

    str_escape_for_json_string(with, escaped_with);
    char * result = str_replace_in(orig, rep, escaped_with);
    g_free((char *) escaped_with);
    return result;
}


/******
  private method definitions
******/


static void str_escape_for_json_string(const char *in, char *out) {
    while (*in) {
        switch (*in) {
        case '\\':
            *(out++) = '\\';
            *(out++) = *in;
            break;
        case '"':
            *(out++) = '\\';
            *(out++) = '"';
            break;
        case '\t':
            *(out++) = '\\';
            *(out++) = 't';
            break;
        case '\r':
            *(out++) = '\\';
            *(out++) = 'r';
            break;
        case '\f':
            *(out++) = '\\';
            *(out++) = 'f';
            break;
        case '\b':
            *(out++) = '\\';
            *(out++) = 'b';
            break;
        case '\n':
            *(out++) = '\\';
            *(out++) = 'n';
            break;
        default:
            *(out++) = *in;
            break;
        }
        in++;
    }
}