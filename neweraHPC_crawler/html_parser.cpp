/*
 *      (C) 2011 Udit Kaushal <uditkaushal@newerahpc.com>
 *      NeweraHPC program is distributed under the terms of the GNU General Public License v2
 *
 *      This file is part of NeweraHPC.
 *
 *      NeweraHPC is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation version 2 of the License.
 *
 *      NeweraHPC is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
 {
     char *str1, *str2, *token, *subtoken;
     char *saveptr1, *saveptr2;
     int j;

     ifstream in_file("index.html");

     string line;
     getline(in_file, line);
     char *line_c = (char *)line.c_str();
     printf("%s\n",line_c);

     char *dim = (char *)"<>";
     char *subdim = (char *)" =/";

     for (j = 1, str1 = line_c; ; j++, str1 = NULL) {
         token = strtok_r(str1, dim, &saveptr1);
         if (token == NULL)
               break;
         printf("%d: %s\n", j, token);

         for (str2 = token; ; str2 = NULL) {
               subtoken = strtok_r(str2, subdim, &saveptr2);
               if (subtoken == NULL)
                    break;
               printf(" --> %s\n", subtoken);
          }
     }
     exit(EXIT_SUCCESS);
} /* main */


