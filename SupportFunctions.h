//
// Created by Matthew Hong on 5/20/2019.
//

#include <string>
#include <iostream>

using namespace std;

#ifndef SETTHEORY_SUPPORTFUNCTIONS_H
#define SETTHEORY_SUPPORTFUNCTIONS_H

void trimLineAndStandardize(string &line);
string upperAll(string convertee);
bool getLine(string &line);
void findCommand(string line, string &whichSet, string &suffix);
void tokenize(string &line);
bool is_number(const string& s);

#endif //SETTHEORY_SUPPORTFUNCTIONS_H
