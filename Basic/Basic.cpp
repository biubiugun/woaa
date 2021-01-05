/*
 * File: Basic.cpp
 * ---------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;
using std::invalid_argument;

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
   EvalState state;
   Program program;
//   cout << "Stub implementation of BASIC" << endl;
   while (true) {
      try {
         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
         cerr << "Error: " << ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
   TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);
   string token;
   token = scanner.nextToken();
   if(scanner.getTokenType(token) == NUMBER){
       int line_num;
       try{
           line_num = stringToInteger(token);
       } catch (...) {
           printf("SYNTAX ERROR\n");return;
       }
       if(!scanner.hasMoreTokens()){
           program.removeSourceLine(line_num);
           return;
       }
       Statement *sta;
       try{
           sta = parseStatement(scanner,line);
           program.addSourceLine(line_num,line);
           program.setParsedStatement(line_num,sta);
       }
       catch (...) {
               printf("SYNTAX ERROR\n");
               return;
           }
       return;
   }
   if(scanner.getTokenType(token) == WORD){
       if(token == "RUN"){
           if(scanner.hasMoreTokens()){
               printf("SYNTAX ERROR\n");
               return;
           }
           try{
               program.run(state);
           } catch (ErrorException &err) {
               if(err.getMessage() == "END")return;
               if(err.getMessage() == "DIVIDED BY ZERO"){
                   printf("DIVIDE BY ZERO\n");
                   return;
               }
               if(err.getMessage() == "LINE NUMBER ERROR"){
                   printf("LINE NUMBER ERROR\n");
                   return;
               }
               printf("VARIABLE NOT DEFINED\n");
               return;
           }
       }else
       if(token == "LIST"){
           if(scanner.hasMoreTokens()){
               printf("SYNTAX ERROR\n");
               return;
           }
           program.get_list();
       }else
       if(token == "CLEAR"){
           if(scanner.hasMoreTokens()){
               printf("SYNTAX ERROR\n");
               return;
           }
           program.clear();
           state.clear();
       }else
       if(token == "QUIT"){
           exit(0);
       }else
       if(token == "HELP"){
           cout << "Please ask for artificial help" << endl;
       }else
       if(token == "INPUT" || token == "PRINT" || token == "LET"){
           scanner.setInput(line);
           Statement *sta;
           try{
               sta = parseStatement(scanner,token);
           } catch (...) {
               printf("SYNTAX ERROR\n");
               return;
           }
           try{
               sta->execute(state); ///sss
           } catch (ErrorException &err) {
               if(err.getMessage() == "DIVIDED BY ZERO"){
                   printf("DIVIDE BY ZERO\n");
               }else if(scanner.getTokenType(token) == WORD){
                   printf("VARIABLE NOT DEFINED\n");
               }
           }
       }else{
           printf("SYNTAX ERROR\n");
           return;
       }
   }else printf("SYNTAX ERROR\n");
}
