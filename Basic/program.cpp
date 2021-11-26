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
#include <iostream>
#include "program.h"
#include "statement.h"

using namespace std;

Program::Program() {}

Program::~Program() {
    programtext.clear();
}

void Program::clear() {
    while (!programtext.empty()) {
        programtext.erase(programtext.begin());
    }
}

void Program::addSourceLine(int lineNumber, string line) {
    programtext[lineNumber] = line;
}

void Program::removeSourceLine(int lineNumber) {
    if (programtext.find(lineNumber) != programtext.end())
        programtext.erase(programtext.find(lineNumber));
}

string Program::getSourceLine(int lineNumber) {
    if (programtext.find(lineNumber) == programtext.end())
        return "";
    else return programtext[lineNumber];
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Replace this stub with your own code
}

Statement *Program::getParsedStatement(int lineNumber) {
    return NULL;  // Replace this stub with your own code
}

int Program::getFirstLineNumber() {
    if (programtext.empty()) return -1;
    else {
        auto iter = programtext.begin();
        return iter->first;
    }
}

int Program::getNextLineNumber(int lineNumber) {
    auto temp_wz = programtext.find(lineNumber);
    temp_wz++;
    if (temp_wz == programtext.end())
        return -1;
    else return temp_wz->first;
}
