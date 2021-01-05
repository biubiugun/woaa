/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"
using namespace std;

Program::Program() {
   // Replace this stub with your own code
}

Program::~Program() {
   clear();
}

void Program::clear() {
   auto iter = table.begin();
   while(iter != table.end()){
       if(iter->second.sta){
           delete iter->second.sta;
       }
       iter ++;
   }
   table.erase(table.begin(),table.end());
}

void Program::addSourceLine(int lineNumber, string line) {
   // Replace this stub with your own code
   bool contain;
   auto iter = table.begin();
   iter = table.find(lineNumber);
   if(iter == table.end())contain = false;
   else contain = true;
   if(!contain){
       table.insert(make_pair(lineNumber,the_line(line)));
   }else{
       iter->second.content = line;
       delete iter->second.sta;
   }
}

void Program::removeSourceLine(int lineNumber) {
   // Replace this stub with your own code
    bool contain;
    auto iter = table.begin();
    iter = table.find(lineNumber);
    if(iter == table.end())contain = false;
    else contain = true;
    if(contain){
        delete iter->second.sta;
        table.erase(iter);
    }
}

string Program::getSourceLine(int lineNumber) {
   // Replace this stub with your own code
    bool contain;
    auto iter = table.begin();
    iter = table.find(lineNumber);
    if(iter == table.end())contain = false;
    else contain = true;
    if(!contain){
        return "";
    }else{
        return iter->second.content;
    }
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
   // Replace this stub with your own code
    bool contain;
    auto iter = table.begin();
    iter = table.find(lineNumber);
    if(iter == table.end())contain = false;
    else contain = true;
    if(!contain){
        error("SYNTAX ERROR");
    }else{
        iter->second.sta = stmt;
    }
}

Statement *Program::getParsedStatement(int lineNumber) {
    // Replace this stub with your own code
    bool contain;
    auto iter = table.begin();
    iter = table.find(lineNumber);
    if(iter == table.end())contain = false;
    else contain = true;
    if(!contain)error("SYNTAX ERROR");
    return iter->second.sta;
}

int Program::getFirstLineNumber() {
   return table.begin()->first;     // Replace this stub with your own code
}

int Program::getNextLineNumber(int lineNumber) {
    // Replace this stub with your own code
    bool contain;
    auto iter = table.begin();
    iter = table.find(lineNumber);
    if(iter == table.end())contain = false;
    else contain = true;
    if(!contain)error("SYNTAX ERROR");
    return (++iter)->first;
}

void Program::get_list(){
    auto iter = table.begin();
    if(!table.empty()){
        while(iter != table.end()){
            cout << iter->second.content << endl;
            ++iter;
        }
    }
}

void Program::run(EvalState &state) {
    if(table.empty())return;
    auto iter = table.begin();
    while(iter != table.end()){
        try{
            iter->second.sta->execute(state);
            ++iter;
        } catch (ErrorException &err) {
            TokenScanner scanner;
            TokenType type = scanner.getTokenType(err.getMessage());
            //process goto
            if(type == NUMBER){
                int num = stringToInteger(err.getMessage());
                iter = table.find(num);
                if(iter == table.end()){
                    error("LINE NUMBER ERROR");
                }
                continue;
            }else{
                error(err.getMessage());
            }
        }
    }
}