//
// Created by Matthew Hong on 5/20/2019.
//
#include "SupportFunctions.h"
#include <iostream>
#include <algorithm>
#include <string>


using namespace std;


void findCommand(string line, string &whichSet, string &suffix)
{
    unsigned int pos = line.find(' ');
    if(pos < line.size())
    {
        whichSet = line.substr(0,pos);
        suffix = line.substr(pos+1);
    }
    else
    {
        whichSet = line;
        suffix = "";
    }
}
void trimLineAndStandardize(string &line)
{
    line = upperAll(line);
    while(line[0] == ' ')
        line.erase(0,1);
    while(line[line.size()-1] == ' ')
        line.erase(line.size()-1);
    int pos = -1;
    while((pos = line.find(' ', pos + 1)) < line.size())
        while(line[pos+1] == ' ')
            line.erase(pos+1,1);

}

bool getLine(string &line)
{
    fflush(stdin);
    cout<<endl<<"Command: ";
    getline(cin, line);

    fflush(stdin);
    return !line.empty();
}

string upperAll(string convertee){
    for(int i = 0; i < convertee.size(); i++){
        if(isalpha(convertee[i]))
            convertee[i] = toupper(convertee[i]);
    }
    return convertee;
}
void tokenize(string &line)
{
    int pos = -1;
    while( (pos = line.find_first_of("~()+*<>{},=", pos + 1)) < line.size())
    {
        if(line[pos + 1] != ' ')
            line.insert(pos + 1,1,' ');
        if(line[pos - 1] != ' ')
            line.insert(pos,1,' ');
    }
}
bool is_number(const string& s)
{
    return !s.empty() && find_if(s.begin(),s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}