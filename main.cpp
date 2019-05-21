#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <stack>
#include <sstream>
#include <bitset>
#include <iterator>
#include <iomanip>
#include <fstream>
#include "SupportFunctions.h"

using namespace std;

map<string,int> UNIVERSE;
map<int,string> INVERSEUNIVERSE;

map<string,int> globalSet;

void process(string line, const map<string,int> commandList, map<string,int> precedence);
void executeCommand(string command, string suffix, const map<string,int> commandList,map<string,int> precedence);
void initializePrecedence(map<string,int> &precedence);
void initializeCommands(map<string,int> &commands);
void initializeOperators(map<string,int> &operators);
void tokenize(string &line);
bool toRPN(string &input, string &rpnExpression, map<string, int> &precedenceMap);
bool eval(const string &rpn, string whichSet);
bool isOperator(const string &token);
void initializeSet();
void initializeUniverse();
bool createSet(string whichSet, string values);
bool createSet(string whichSet, int value);
void printSet(string whichSet);
void storeInFile(string fileName);
void loadFromFile(string fileName);
void printHelp();

int main() {

    string line;
    map<string,int> commandList;
    map<string, int> precedence;
    initializeUniverse();
    initializePrecedence(precedence);
    initializeCommands(commandList);
    initializeSet();
    while(getLine(line))
        process(line,commandList, precedence);

    return 0;
}

void process(string line, map<string,int> commandList ,map<string,int> precedence){
    string command,suffix;

    trimLineAndStandardize(line);
    findCommand(line, command, suffix);
    executeCommand(command,suffix,commandList,precedence);
}
void executeCommand(string command, string suffix, const map<string,int> commandList, map<string, int> precedenceMap){
    int whatToDo;
    if(commandList.count(upperAll(command)))
        whatToDo = commandList.at(upperAll(command));
    else
        whatToDo = 99;
    switch(whatToDo){
        //SET
        case 0:
        {
            string rpn;

            suffix.erase(remove(suffix.begin(), suffix.end(), '\t'), suffix.end());
            suffix.erase(remove(suffix.begin(), suffix.end(), ' '), suffix.end());

            string whichSet = suffix.substr(0,suffix.find('='));
            suffix = suffix.substr(suffix.find('=')+1);
            //cout<<"which one: "<<suffix<<endl;


            if(toRPN(suffix, rpn, precedenceMap) && suffix.find("{") > suffix.size() && !is_number(suffix))
            {
                tokenize(rpn);
                eval(rpn,whichSet);
            }else if(is_number(suffix)){
                createSet(whichSet,atoi(suffix.c_str()));
            }else{
                createSet(whichSet,suffix);
            }
        }
            break;
        //SAVE
        case 1:
            storeInFile(suffix);
            break;
        //LOAD
        case 2:
            loadFromFile(suffix);
            break;
        //HELP
        case 3:
            printHelp();
            break;
        //SHOW
        case 4:
        {
            printSet(suffix);
        }
            break;
        //EXIT
        case 5:
            exit(0);
            break;
        //IS
        case 6:{
            tokenize(suffix);
            bitset<16> l(globalSet[suffix.substr(0,suffix.find(" "))]);
            string operators = suffix.substr(suffix.find(" ")+1,suffix.find_last_of(" ")-2);
            bitset<16> r(globalSet[suffix.substr(suffix.find_last_of(" ")+1)]);
            //subset
            if(operators == "<"){
                cout<<(((r&l)==l) ? "True" : "False")<<endl;
            }else if(operators == ">"){
                cout<<(((r&l)==l) ? "False" : "True")<<endl;
            }else if(operators == "="){
                cout<<((l == r) ? "True" : "False")<<endl;
            }else{
                cout<<"sdafasfa"<<endl;
            }
        }
            break;
        //LIST
        case 7:
        {
            for(char i = 'A'; i < 'Z'+1; ++i){
                cout<<"SET "<<i<<setw(10)<<" ";
                printSet(string(1,i));
                cout<<endl;
            }
        }
            break;

    }
}
bool toRPN(string &input, string &rpnExpression, map<string, int> &precedenceMap)
{
    string token;
    tokenize(input);
    stack<string> operador;
    stringstream ss;
    ss<<input;
    while (ss >> token) {
        if (isalpha(token[0])) {
            rpnExpression += token + " ";
        } else if (isOperator(token)) {
            while (!operador.empty() && precedenceMap[token] <= precedenceMap[operador.top()]) {
                if (operador.top() != token) {
                    rpnExpression += operador.top();
                    operador.pop();
                } else {
                    break;
                }
            }
            operador.push(token);

        } else if (token == "(") {
            operador.push(token);
        } else if (token == ")") {
            while (operador.top() != "(" && !operador.empty()) {
                rpnExpression += operador.top();
                operador.pop();
            }
            operador.pop();
        }
    }
    while (!operador.empty()) {
        rpnExpression += operador.top();
        operador.pop();
    }

    return true;
}
bool eval(const string &rpn, string whichSet)
{
    map<string, int> operatorMap;
    initializeOperators(operatorMap);
    stack<bitset<16>> operand;
    istringstream ss(rpn);
    string data;
    while(getline(ss,data,' ')){
        if(isOperator(data)){
            bitset<16> operand2 = operand.top(),operand1;
            operand.pop();
            if(data != "~"){
                operand1 = operand.top();
                operand.pop();
            }
            switch(operatorMap[data]){
                //complement
                case 0:
                    operand.push(operand2.flip());
                    break;
                //union
                case 1: {
                    operand.push(operand2 |= operand1);
                    break;
                }
                //intersection
                case 2:
                    operand.push(operand2 &= operand1);
                    break;
            }
        }else{
            operand.push(bitset<16>(globalSet[data]));
        }
    }
    globalSet[whichSet] = (int)operand.top().to_ulong();
    return true;
}
bool createSet(string whichSet, string values){
    stringstream ss;
    string token;
    tokenize(values);
    bitset<16> setValue;
    ss<<values;
    while(ss>>token){
        if(token != "{" && token != "," && token != "}"){
            if(UNIVERSE.count(token)){
                setValue[UNIVERSE.at(token)] = 1;
            }else{
                return false;
            }
        }
    }
    if(globalSet.count(whichSet)){
        globalSet[whichSet] = (int)setValue.to_ulong();
        return true;
    }
    return false;
}
bool createSet(string whichSet, int value){
    if(value < 65535 && value >= 0){
        globalSet[whichSet] = value;
    }else{
        cout<<"You have entered a number too larger or too small.";
    }
    return false;
}
void printSet(string whichSet){
    if(globalSet.count(whichSet)){
        bitset<16> myValues(globalSet.at(whichSet));
        for(int i = 0; i < 16; ++i){
            if(myValues[i] == 1){
                cout<<INVERSEUNIVERSE[i]<<" ";
            }
        }
    }else{
        cout<<"You entered incorrect input"<<endl;
    }
}
void storeInFile(string fileName){
    ifstream f;
    if (fileName.find(".set") == -1)
        f.open(fileName+".set");
    else
        f.open(fileName);
    ofstream out;
    if(fileName.empty())
        cout<<"ERROR no filename!"<<endl;
    else{
        bool proceed = true;
        if(f.good()){
            char response;
            cout<<"File with that name has been found would you like to replace?(Y/N): ";
            cin>>response;
            if(tolower(response) == 'y'){
                remove(fileName.c_str());
            }else{
                cout<<"Understood.\n";
                proceed = false;
            }
        }
        if(proceed){
            if (fileName.find(".set") == -1) {
                out.open(fileName + ".set");
            } else {
                out.open(fileName);
            }
            for(char i = 'A'; i < 'Z'+1; ++i){
                out<<globalSet.at(string(1,i))<<" ";
            }
            out.close();
            cout<<"\n---FILE SUCCESSFULLY SAVED---\n";
        }
    }
}
void loadFromFile(string fileName) {
    ifstream f;
    if (fileName.find(".set") == -1)
        f.open(fileName+".set");
    else
        f.open(fileName);
    if(fileName.empty())
        cout<<"ERROR no filename!"<<endl;
    if(f.good()){
        int val;
        for(char i = 'A'; i < 'Z'+1; ++i){
            f>>val;
            globalSet[string(1,i)] = val;
        }
        cout<<"\n---FILE SUCCESSFULLY LOADED---\n";
    }else{
        cout<<"Error! file could not be found.";
    }
}
void initializeCommands(map<string,int> &commands){
    commands["SET"] = 0;
    commands["SAVE"] = 1;
    commands["LOAD"] = 2;
    commands["HELP"] = 3;
    commands["SHOW"] = 4;
    commands["\n"] = 5;
    commands["IS"] = 6;
    commands["LIST"] = 7;
}
void initializeOperators(map<string,int> &operators){
    operators["~"] = 0;
    operators["+"] = 1;
    operators["*"] = 2;
}
void initializePrecedence(map<string,int> &precedence){
    precedence["~"] = 9;
    precedence["*"] = 8;
    precedence["+"] = 7;
}
void initializeUniverse(){
    UNIVERSE["WHITE"] = 0;
    UNIVERSE["GOLD"] = 1;
    UNIVERSE["GRAY"] = 2;
    UNIVERSE["RED"] = 3;
    UNIVERSE["GREEN"] = 4;
    UNIVERSE["BLUE"] = 5;
    UNIVERSE["ORANGE"] = 6;
    UNIVERSE["PURPLE"] = 7;
    UNIVERSE["YELLOW"] = 8;
    UNIVERSE["VIOLET"] = 9;
    UNIVERSE["MAGENTA"] = 10;
    UNIVERSE["CYAN"] = 11;
    UNIVERSE["RUST"] = 12;
    UNIVERSE["NAVY"] = 13;
    UNIVERSE["BURGUNDY"] = 14;
    UNIVERSE["BLACK"] = 15;
    for (auto&& item : UNIVERSE)
        INVERSEUNIVERSE.emplace(item.second,item.first);
}
void initializeSet(){
    for(char i = 'A'; i < 'Z'+1; ++i){
        globalSet[string(1,i)] = 0;
    }
}
bool isOperator(const string &token)
{
    return  token == "+" || token == "~" || token == "*";
}
void printHelp(){
    cout<<"__________________________________HELP__________________________________________"<<endl;
    cout<<"[LOAD] function loads a file that has been created by the program previously.\n";
    cout<<"[STORE] function saves all expressions to a .truth file.\n";
    cout<<"[SET] enter a set to be stored.(SET A = {RED,...}) or (SET A = 12)\n";
    cout<<"[LIST] Displays the current sets in alphabetical order.\n";
    cout<<"[IS] enter two sets and see if it is a subset(<), superset(>), or equivalent(=).\n";
    cout<<"[EXIT] Press enter to exit the program\n";
    cout<<"________________________________________________________________________________"<<endl;
}
