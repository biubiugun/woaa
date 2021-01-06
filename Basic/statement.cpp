/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
#include "parser.h"
using namespace std;
using std::invalid_argument;

/* Implementation of the Statement class */

inline bool checkLe(string token){
    if(token == "LET" || token == "REM" || token == "PRINT" || token == "END" || token == "IF" ||
       token == "THEN" || token == "GOTO" || token == "RUN" || token == "LIST" || token == "CLEAR" ||
       token == "QUIT" || token == "HELP"){return false;}
    else return true;
}

Statement *parseStatement(TokenScanner &scanner,string line){
    string token = scanner.nextToken();
    TokenType type_of_token = scanner.getTokenType(token);
    if(type_of_token != WORD){error("SYNTAX ERROR");}
    Statement *sta = nullptr;
    Expression *exp = nullptr;
    if(token == "REM"){
        sta = new REM_();
        return sta;
    }else
    if(token == "END"){
        sta = new END_();
        return sta;
    }else
    if(token == "LET"){
        if(!scanner.hasMoreTokens()){error("SYNTAX ERROR");}
        exp = parseExp(scanner);
        if(exp->getType() != COMPOUND){delete exp;error("SYNTAX ERROR");}
        if(((CompoundExp *)exp)->getLHS()->getType() != IDENTIFIER){delete exp;error("SYNTAX ERROR");}
        if(((CompoundExp *)exp)->getOp() != "="){delete exp;error("SYNTAX ERROR");}
        if(!checkLe(((IdentifierExp *)(((CompoundExp *)exp)->getLHS()))->getName())){delete exp;error("[Warning] SYNTAX ERROR");}
        sta = new LET_(exp);
        return sta;
    }else
    if(token == "PRINT"){
        if(!scanner.hasMoreTokens()){error("SYNTAX ERROR");}
        exp = parseExp(scanner);
        if(exp->getType() == COMPOUND){
            if(((CompoundExp *)exp)->getOp() == "=")
                {delete exp;error("SYNTAX ERROR");}
        }
        sta = new PRINT_(exp);
        return sta;
    }else
    if(token == "INPUT"){
        if(!scanner.hasMoreTokens()){error("SYNTAX ERROR");}
        token = scanner.nextToken();
        if(scanner.hasMoreTokens()){error("SYNTAX ERROR");}
        if(scanner.getTokenType(token) != WORD){error("SYNTAX ERROR");}
        sta = new INPUT_(token);
        return sta;
    }else
    if(token == "GOTO"){
        if(!scanner.hasMoreTokens()){error("SYNTAX ERROR");}
        token = scanner.nextToken();
        if(scanner.getTokenType(token) != NUMBER){error("SYNTAX ERROR");}
        if(scanner.hasMoreTokens()){error("SYNTAX ERROR");}
        try{
            stringToInteger(token);
        } catch (...) {
            error("SYNTAX ERROR");
        }
        sta = new GOTO_(stringToInteger(token));
        return sta;
    }else
    if(token == "IF"){
        if(!scanner.hasMoreTokens()){error("SYNTAX ERROR");}
        Expression *exp1,*exp2;
        GOTO_ *then;
        if(line.find('=') == string::npos){
            exp1 = readE(scanner);
            string op = scanner.nextToken();
            if(op != ">" && op != "=" && op != "<"){
                delete exp1;
                error("SYNTAX ERROR");
            }
            try{
                exp2 = readE(scanner);
            } catch (...) {
                error("SYNTAX ERROR");
            }
            cout << 1 << endl;
            token = scanner.nextToken();
            if(token != "THEN"){
                delete exp1;
                delete exp2;
                error("SYNTAX ERROR");
            }
            token = scanner.nextToken();
            if(scanner.getTokenType(token) != NUMBER || scanner.hasMoreTokens()){
                delete exp1;
                delete exp2;
                error("SYNTAX ERROR");
            }

            try{
                stringToInteger(token);
            } catch (...) {
                delete exp1;
                delete exp2;
                error("SYNTAX ERROR");
            }
            then = new GOTO_(stringToInteger(token));
            return new IF_(exp1,op,exp2,then);
        }else{
            int n = 0;
            for(auto & i : line){
                if(i == '='){
                    n ++;
                }
            }
            if(n != 1){error("SYNTAX ERROR");}
            string str1,str2;
            string op = "=";
            while(scanner.hasMoreTokens()){
                token = scanner.nextToken();
                if(token == "=")break;
                str1 += token + ' ';
            }

            while(scanner.hasMoreTokens()){
                token = scanner.nextToken();
                if(token == "THEN")break;
                str2 += token + ' ';
            }
            if(!scanner.hasMoreTokens()){error("SYNTAX ERROR");}
            else token = scanner.nextToken();
            if(scanner.getTokenType(token) != NUMBER){error("SYNTAX ERROR");}
            scanner.setInput(str1);
            exp1 = readE(scanner);
            scanner.setInput(str2);
            exp2 = readE(scanner);
            try{
                stringToInteger(token);
            } catch (...) {
                error("SYNTAX ERROR");
            }
            then = new GOTO_(stringToInteger(token));
            return new IF_(exp1,op,exp2,then);
        }
    }
    error("SYNTAX ERROR");
}

Statement::Statement() {
   /* Empty */
}

Statement::~Statement() {
   /* Empty */
}

REM_::REM_() = default;

REM_::~REM_() noexcept = default;

void REM_::execute(EvalState &state) {}

LET_::LET_(Expression *ord) {
    LET_::ord = ord;
}

LET_::~LET_() noexcept {
    delete ord;
}

void LET_::execute(EvalState &state) {
    ord->eval(state);
}

PRINT_::PRINT_(Expression *exp) {
    PRINT_::exp = exp;
}

PRINT_::~PRINT_() noexcept {
    delete exp;
}

void PRINT_::execute(EvalState &state) {
    cout << exp->eval(state) << endl;
}

INPUT_::INPUT_(string &str) {
    INPUT_::name = str;
}

INPUT_::~INPUT_() noexcept = default;

void INPUT_::execute(EvalState &state) {
    string token;
    TokenScanner scanner;
    TokenType type_of_token;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(getLine(" ? "));
    if(!scanner.hasMoreTokens()){
        getLine(" ? ");
    }
    while(true){
        int value;
        token = scanner.nextToken();
        if(token == "-"){
            if(!scanner.hasMoreTokens()){
                cout << "INVALID NUMBER\n" << " ? ";
                continue;
            }
            token = scanner.nextToken();
            type_of_token = scanner.getTokenType(token);
            if(type_of_token != NUMBER || scanner.hasMoreTokens()){
                cout << "INVALID NUMBER\n" << " ? ";
                continue;
            }
            try {
                value = stringToInteger(token);
            } catch (...) {
                cout << "INVALID NUMBER\n" << " ? ";
                continue;
            }
            state.setValue(name,-value);
            break;
        }
        if(scanner.getTokenType(token) != NUMBER || scanner.hasMoreTokens()){
            cout << "INVALID NUMBER\n" << " ? ";
            continue;
        }
        try{
            value = stringToInteger(token);
        } catch (...) {
            cout << "INVALID NUMBER\n" << " ? ";
            continue;
        }
        state.setValue(name,value);
        break;
    }
}

END_::END_() = default;

END_::~END_() noexcept = default;

void END_::execute(EvalState &state) {
    error("END");
}

GOTO_::GOTO_(int num):the_goto_num(num){}

GOTO_::~GOTO_() noexcept = default;

void GOTO_::execute(EvalState &state) {
    error(integerToString(the_goto_num));
}

IF_::IF_(Expression *exp1, string &opt, Expression *exp2, GOTO_ *st):exp1(exp1),opt(opt),exp2(exp2), st(st){}

IF_::~IF_() noexcept {
    delete exp1;
    delete exp2;
    delete st;
}

void IF_::execute(EvalState &state) {
    bool for_if = false;
    switch (opt[0]) {
        case '=' : for_if = (exp1->eval(state) == exp2->eval(state));break;
        case '<' : for_if = (exp1->eval(state) < exp2->eval(state));break;
        case '>' : for_if = (exp1->eval(state) > exp2->eval(state));break;
        default :for_if = false;
    }
    if(for_if)st->execute(state);
}





