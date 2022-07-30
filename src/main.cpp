#include <string.h>
#include <unistd.h>
#include <iostream>
#include "Ast.h"
#include "ConstAsm.h"
#include "CopyProp.h"
#include "DeadCodeElimination.h"
#include "ElimUnreachCode.h"
#include "GraphColor.h"
#include "LinearScan.h"
#include "MachineCode.h"
#include "MachineDeadCodeElimination.h"
#include "MachineStraighten.h"
#include "Mem2reg.h"
#include "SSADestruction.h"
#include "Starighten.h"
#include "TreeHeightBalance.h"
#include "Unit.h"
#include "ValueNumber.h"
using namespace std;

Ast ast;
Unit unit;
MachineUnit mUnit;
extern FILE* yyin;
extern FILE* yyout;

int yyparse();

char outfile[256] = "a.out";
bool dump_tokens;
bool dump_ast;
bool dump_ir;
bool dump_asm;
bool optimize;

int main(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "Siato:O::")) != -1) {
        switch (opt) {
            case 'o':
                strcpy(outfile, optarg);
                break;
            case 'a':
                dump_ast = true;
                break;
            case 't':
                dump_tokens = true;
                break;
            case 'i':
                dump_ir = true;
                break;
            case 'O':
                optimize = true;
                break;
            case 'S':
                dump_asm = true;
                break;
            default:
                // fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
                // exit(EXIT_FAILURE);
                dump_asm = false;

                break;
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r"))) {
        fprintf(stderr, "%s: No such file or directory\nno input file\n",
                argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (!(yyout = fopen(outfile, "w"))) {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    yyparse();
    if (dump_ast)
        ast.output();
    ast.typeCheck();
    ast.genCode(&unit);
    if (optimize) {
        ElimUnreachCode e(&unit);
        DeadCodeElimination d(&unit);
        Starighten s(&unit);
        Mem2reg m(&unit);
        SSADestruction s1(&unit);
        CopyProp c(&unit);
        ValueNumber vn(&unit);
        TreeHeightBalance thb(&unit);
        m.pass();
        d.pass();
        c.copy_prop();
        thb.pass();
        vn.pass();
        e.pass();
        s.pass();
        s1.pass();
    }
    if (dump_ir) {
        unit.output();
        return 0;
    }
    unit.genMachineCode(&mUnit);
    if (optimize) {
        MachineDeadCodeElimination mdce(&mUnit);
        MachineStraighten ms(&mUnit);
        mdce.pass();
        ms.pass();
    }

    // if (optimize) {
    //     ConstAsm const_asm(&mUnit);
    //     const_asm.pass();
    // }

    if (!optimize) {
        LinearScan linearScan(&mUnit);
        linearScan.allocateRegisters();
    } else {
        GraphColor GraphColor(&mUnit);
        GraphColor.allocateRegisters();
    }
    if (optimize) {
        MachineDeadCodeElimination mdce(&mUnit);
        MachineStraighten ms(&mUnit);
        mdce.pass();
        ms.pass();
    }
    if (dump_asm)
        mUnit.output();
    return 0;
}
