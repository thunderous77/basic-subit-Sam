/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "statement.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"

using namespace std;

/* Function prototypes */

void processLine(string line, Program &program, EvalState &state);

void variable_undefined(std::string line, int beginn);

void interpreter_judge(string line);

void runprogram();

void runline(std::string line);

void print_program();

void input_dg();

bool name_judge(std::string);

bool number_judge(std::string number);

/* Main program */
std::string input_variable;
bool flag_quit = false;
EvalState state;
Program program;

int main() {
    int num, iter;
    while (true) {
        try {
            string input, line;
            input = getLine();
            if (input.empty())
                return 0;
            if (input[0] >= '0' && input[0] <= '9') {
                num = 0;
                for (iter = 0; iter < input.length(); ++iter) {
                    if (input[iter] == ' ') break;
                    else num = num * 10 + int(input[iter] - '0');
                }
                if (iter == input.length()) {
                    program.removeSourceLine(num);
                    continue;
                }
                line = input.substr(iter + 1);
                program.addSourceLine(num, line);
            } else {
                interpreter_judge(input);
                if (flag_quit) return 0;
            }
        } catch (ErrorException &ex) {
            cerr << ex.getMessage() << endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);
    Expression *exp = parseExp(scanner);
    int value = exp->eval(state);
    cout << value << endl;
    delete exp;
}

bool name_judge(std::string name) {
    return (name == "LET" || name == "RUN");
}

void interpreter_judge(string line) {
    if (line == "RUN") {
        runprogram();
    }
    if (line == "QUIT") {
        flag_quit = true;
        return;
    }
    if (line == "LIST") {
        print_program();
        return;
    }
    if (line == "CLEAR") {
        program.clear();
        state.clear();
        return;
    }
    if (line == "HELP")
        return;
    if (line[0] == 'L' && line[1] == 'E' && line[2] == 'T') {//LET
        int line_wz = 4;
        while (line[line_wz] != ' ' && line[line_wz] != '=') { line_wz++; }
        std::string let_variable = line.substr(4, line_wz - 4);
        if (name_judge(let_variable)) {
            cout << "SYNTAX ERROR" << "\n";
            error("SYNTAX ERROR");
            return;
        }
        variable_undefined(line, 4);
        std::string token = line.substr(4);
        TokenScanner scanner;
        scanner.ignoreWhitespace();
        scanner.scanNumbers();
        scanner.setInput(token);
        Expression *exp = parseExp(scanner);
        int value = exp->eval(state);
        delete exp;
        return;
    }
    if (line[0] == 'P' && line[1] == 'R' && line[2] == 'I') {//PRINT
        variable_undefined(line, 6);
        std::string token = line.substr(6);
        TokenScanner scanner;
        scanner.ignoreWhitespace();
        scanner.scanNumbers();
        scanner.setInput(token);
        Expression *exp = parseExp(scanner);
        int value = exp->eval(state);
        cout << value << endl;
        delete exp;
        return;
    }
    if (line[0] == 'I' && line[1] == 'N' && line[2] == 'P') {//INPUT
        cout << " " << "?" << " ";
        std::string input;
        input_variable = line.substr(6);
        input = getLine();
        if (number_judge(input)) {
            int value = 0;
            for (int input_wz = 0; input_wz < input.length(); ++input_wz) {
                if (input[input_wz] != '-')
                    value = value * 10 + int(input[input_wz] - '0');
            }
            if (input[0] == '-') value = 0 - value;
            state.setValue(input_variable, value);
        } else {
            cout << "INVALID NUMBER" << "\n";
            cout << " " << "?" << " ";
            input_dg();
        }
        return;
    }
}

int linenum;
bool flag_run, flag_continue;

void runprogram() {
    flag_run = true;
    flag_continue = false;
    linenum = program.getFirstLineNumber();
    if (linenum == -1) return;
    if (!flag_run) return;
    runline(program.getSourceLine(linenum));
    while (program.getNextLineNumber(linenum) != -1) {
        linenum = program.getNextLineNumber(linenum);
        if (!flag_run) return;
        runline(program.getSourceLine(linenum));
    }
}

void runline(std::string line) {
    if (line == "") {
        cout << "LINE NUMBER ERROR" << "\n";
        error("LINE NUMBER ERROR");
    }
    if (line[0] == 'E' && line[1] == 'N' && line[2] == 'D') {//END
        flag_run = false;
        return;
    }
    if (line[0] == 'R' && line[1] == 'E' && line[2] == 'M')//REM
        return;
    if (line[0] == 'G' && line[1] == 'O' && line[2] == 'T') {//GOTO
        int goto_num = 0;
        for (int goto_wz = 5; goto_wz < line.length(); ++goto_wz)
            goto_num = goto_num * 10 + int(line[goto_wz] - '0');
        linenum = goto_num;
        runline(program.getSourceLine(goto_num));
        return;
    }
    if (line[0] == 'I' && line[1] == 'F') {
        int if_wz1 = 0, if_wz2, value1, value2;
        std::string l, r;
        while (line[if_wz1] != '=' && line[if_wz1] != '<' && line[if_wz1] != '>') {
            if_wz1++;
        }
        if_wz2 = if_wz1 + 1;
        while (line[if_wz2] != 'T') { if_wz2++; }
        l = line.substr(3, if_wz1 - 4);
        TokenScanner scanner;
        scanner.ignoreWhitespace();
        scanner.scanNumbers();
        scanner.setInput(l);
        Expression *exp = parseExp(scanner);
        value1 = exp->eval(state);
        delete exp;
        r = line.substr(if_wz1 + 2, if_wz2 - 3 - if_wz1);
        TokenScanner scannerr;
        scannerr.ignoreWhitespace();
        scannerr.scanNumbers();
        scannerr.setInput(r);
        Expression *expp = parseExp(scannerr);
        value2 = expp->eval(state);
        delete expp;
        if (line[if_wz1] == '=') {
            if (value1 == value2) {
                int goto_num = 0;
                for (int goto_wz = if_wz2 + 5; goto_wz < line.length(); ++goto_wz)
                    goto_num = goto_num * 10 + int(line[goto_wz] - '0');
                linenum = goto_num;
                runline(program.getSourceLine(goto_num));
                return;
            }
        }
        if (line[if_wz1] == '<') {
            if (value1 < value2) {
                int goto_num = 0;
                for (int goto_wz = if_wz2 + 5; goto_wz < line.length(); ++goto_wz)
                    goto_num = goto_num * 10 + int(line[goto_wz] - '0');
                linenum = goto_num;
                runline(program.getSourceLine(goto_num));
                return;
            }
        }
        if (line[if_wz1] == '>') {
            if (value1 > value2) {
                int goto_num = 0;
                for (int goto_wz = if_wz2 + 5; goto_wz < line.length(); ++goto_wz)
                    goto_num = goto_num * 10 + int(line[goto_wz] - '0');
                linenum = goto_num;
                runline(program.getSourceLine(goto_num));
                return;
            }
        }
    }
    interpreter_judge(line);
}

void input_dg() {
    std::string inp;
    inp = getLine();
    if (number_judge(inp)) {
        int value = 0;
        for (int inp_wz = 0; inp_wz < inp.length(); ++inp_wz) {
            if (inp[inp_wz] != '-')
                value = value * 10 + int(inp[inp_wz] - '0');
        }
        if (inp[0] == '-') value = 0 - value;
        state.setValue(input_variable, value);
        state.setValue(input_variable, value);
    } else {
        cout << "INVALID NUMBER" << "\n";
        cout << " " << "?" << " ";
        input_dg();
    }
    return;
}

void print_program() {
    int linenum = program.getFirstLineNumber();
    if (linenum == -1) return;
    while (program.getNextLineNumber(linenum) != -1) {
        cout << linenum << " " << program.getSourceLine(linenum) << "\n";
        linenum = program.getNextLineNumber(linenum);
    }
    cout << linenum << " " << program.getSourceLine(linenum) << "\n";
}

void variable_undefined(std::string line, int beginn) {
    int line_wz = beginn, equal_wz = beginn, line_wz_pre;
    while (line[equal_wz] != '=' && equal_wz < line.length()) { equal_wz++; }
    if (equal_wz == line.length()) {
        while (line_wz < line.length()) {
            line_wz_pre = line_wz;
            while ((line[line_wz] >= 'A' && line[line_wz] <= 'Z') ||
                   (line[line_wz] >= 'a' && line[line_wz] <= 'z')) { line_wz++; }
            std::string print_variable = line.substr(line_wz_pre, line_wz - line_wz_pre);
            if ((!state.isDefined(print_variable)) && print_variable != "") {
                cout << "VARIABLE NOT DEFINED" << "\n";
                error("VARIABLE NOT DEFINED");
                return;
            }
            while ((line[line_wz] < 'A' || line[line_wz] > 'Z') &&
                   (line[line_wz] < 'a' || line[line_wz] > 'z') && line_wz < line.length()) { line_wz++; }
        }
    } else {
        line_wz = equal_wz + 1;
        while (line_wz < line.length()) {
            line_wz_pre = line_wz;
            while ((line[line_wz] >= 'A' && line[line_wz] <= 'Z') ||
                   (line[line_wz] >= 'a' && line[line_wz] <= 'z')) { line_wz++; }
            std::string print_variable = line.substr(line_wz_pre, line_wz - line_wz_pre);
            if ((!state.isDefined(print_variable)) && print_variable != "") {
                cout << "VARIABLE NOT DEFINED" << "\n";
                error("VARIABLE NOT DEFINED");
                return;
            }
            while ((line[line_wz] < 'A' || line[line_wz] > 'Z') &&
                   (line[line_wz] < 'a' || line[line_wz] > 'z') && line_wz < line.length()) { line_wz++; }
        }
    }
}

bool number_judge(std::string number) {
    for (int num_wz = 0; num_wz < number.length(); ++num_wz) {
        if ((number[num_wz] < '0' || number[num_wz] > '9') && number[num_wz] != '-')
            return false;
    }
    return true;
}
