#include <iostream>
#include <string>
#include <cstdio>
#include "pl0-symtab.h"
#include "AsmTreeGen.h"
#include "AsmCode.h"

symtab* st = new symtab();
int symtablevel;

int yylex();
int yyparse();

int yacc_error = 0;

int main(int argc, char* argv[]) {
    extern FILE *yyin;

    if (argc < 2) {
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }

    FILE *file;
    std::string toOpen = std::string(argv[1]) + ".pl0";
    file = fopen(toOpen.c_str(), "r+");

    if (!file) {
        std::cout << toOpen << " can not be opened" << std::endl;
        return 1;
    }

    yyin = file;

    if (yyparse() == 0) {
        std::string outputFileName = std::string(argv[1]) + ".asm";
        FILE *outputFile = fopen(outputFileName.c_str(), "w");

        AsmCode::getInstance()->setFile(outputFile);

        DataModel::getInstance()->generateAssember();
        fclose(outputFile);
        return 0;
    } else {
        return 1;
    }
}
