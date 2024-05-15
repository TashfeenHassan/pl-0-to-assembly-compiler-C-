
/* Umut Dönmez 80080 & Adrian Preisler 80035 */
%{
#include "lex.yy.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "pl0-symtab.h"
#include "AsmTreeGen.h"
#include "AsmCode.h"

extern symtab* st;
extern int symtablevel;
int yyerror(const char* s);
using namespace std;

int variable = 1;
int constante = 2;
int procedure = 4;


int global_ProcedureCounter = 0;
int global_VariableCounter = -1;
int global_JumpCounter = 1;

%}

%union{ char strval[256]; int intval;
        BlockNode* BLOCK_NODE;               
        StatementNode* STATEMENT_NODE; 
        ExpressionNode* EXPRESSION_NODE;
        IdentifierNode* IDENTIFIER_NODE;
        struct { StatementNode* from; StatementNode* to; } FROM_TO_NODE;}

%token t_punkt t_const t_eq t_komma
%token t_semik t_var t_proc t_assign t_call t_begin t_end
%token t_read t_write t_if t_then t_while t_do
%token t_odd t_ne t_lt t_le t_gt t_ge
%token t_plus t_minus t_mult t_div t_bra_o t_bra_c
%token t_error
%token <intval> t_number
%token <strval> t_ident 

%type <BLOCK_NODE> block proclist proc
%type <IDENTIFIER_NODE> constdek constlist vardek varlist
%type <EXPRESSION_NODE> factor expression condition term
%type <FROM_TO_NODE> statementlist statement

%%


program:    block t_punkt { DataModel::getInstance()->procedures.push_back($1); 
                            DataModel::getInstance()->mainProcedure = $1; 
                            $1->identifier = "main"; 

                            DataModel::getInstance()->prepareAssemberGeneration();}; 

block:      {st->level_up();} constdek vardek proclist statement {
                                                                                $$ = new BlockNode("", $5.from, $2, $3);
                                                                                st->level_down();};

constdek:  
         t_const  t_ident t_eq t_number constlist t_semik {
    int result;

    result = st->insert($2, constante, $4); 

    if(result != 0) {
        printf("Error at inserting constant with identifier: %s\n", $2);
        return result;
    }

    IdentifierNode * newConstant = new IdentifierNode($2);  
    newConstant->value = $4;
    newConstant->next = $5;

    $$ = newConstant;
}
                                        | /*epsilon*/                                               
                                        {  
                                        $$ = nullptr; 
                                        };

constlist:  
t_komma t_ident t_eq t_number constlist
{ 
    int result;

    result = st->insert($2, constante, $4); 

    if(result != 0) {
        printf("Error at inserting constant with identifier: %s\n", $2);
        return result;
    }

    IdentifierNode * newConstant = new IdentifierNode($2);  
    newConstant->value = $4;
    newConstant->next = $5;

    $$ = newConstant;
}
                        | /* epsilon */
                        { 
                            $$ = nullptr; 
                        }
                        ;




vardek:    
t_var t_ident varlist t_semik  {  
    int result;

    global_VariableCounter += 1;
    result = st->insert($2, variable, global_VariableCounter); 

    if(result != 0) {
        printf("Error at inserting variable with identifier: %s\n", $2);
    }

    IdentifierNode * newIdentifier = new IdentifierNode($2);  
    newIdentifier->next = $3;
    
    $$ = newIdentifier;
}



                        
                        | /*epsilon*/                                               
                        {
                            $$ = nullptr;
                        }
                        ;

varlist: 
t_komma t_ident varlist  { 
    int result;

    global_VariableCounter += 1;
    result = st->insert($2, variable, global_VariableCounter); 

    if(result != 0) {
        printf("Error at inserting variable with identifier: %s\n", $2);
    }

    IdentifierNode * newVariable = new IdentifierNode($2);  
    newVariable->next = $3;
    
    $$ = newVariable;
}
                        | /*epsilon*/                                               
                        {
                         
                            $$ = nullptr;
                        }
                        ; /* möglich direkt ersetzen durch t_ident */

proclist:   
        proc proclist {  
                            $$ = $1; 
                            DataModel::getInstance()->procedures.push_back($$); 
                        }
                        | /*epsilon*/                                               
                        {  
                            $$ = nullptr; 
                        }
                        ;

proc:       t_proc t_ident t_semik {
    int result;

    result = st->insert($2, procedure, ++global_ProcedureCounter);

    if(result != 0) {
        printf("Error at inserting procedure with identifier %s\n", $2);
        return result;
    }

    global_VariableCounter = -1;
}
block t_semik {
    int result;
    int level;
    int offset;

    result = st->lookup($2, procedure, level, offset);

    $5->identifier = std::string($2) + std::to_string(offset);

    $$ = $5;
}

statement:  
                t_ident t_assign expression 
                        { 

                            int level;
                            int offset;
                            int result = st->lookup($1,variable, level, offset);
                            if(result != 0)
						    {
								printf("Error at assignment with identifier: %s\n", $1);
								return result;
							}
                            StatementNode* assignmentNode = new StatementNode(AsmCode::ASSIGN, $1); 
                            assignmentNode->level = level;
                            assignmentNode->offset = offset;
                            assignmentNode->expressionNode = $3; 

                            $$.from = assignmentNode;
                            $$.to = assignmentNode;

                        }
    | t_call t_ident    { 

                            int level;
                            int offset;
						    int result = st->lookup($2, procedure, level, offset);
						    if(result != 0)
							{
						        printf("Error at CALL with identifier: %s\n", $2);
							    return result;
							}

                            StatementNode* callNode = new StatementNode(AsmCode::CALL, $2+std::to_string(offset)); 
                            callNode->level = level;
                            callNode->offset = offset;

                            $$.from = callNode;
                            $$.to = callNode;

                        }

    | t_read t_ident    { 
                            
                            int level;
                            int offset;
							int result = st->lookup($2, variable, level, offset);
						    if(result != 0)
						    {
								printf("Error at READ with identifier: %s\n", $2);
								return result;
							}
                            StatementNode* readNode = new StatementNode(AsmCode::READ, $2); 
                            readNode->level = level;
                            readNode->offset = offset;

                            $$.from = readNode;
                            $$.to = readNode;
            
                        }
    | t_write expression{ 
                            
                            ExpressionNode* expression = $2;
                            
                            StatementNode* writeNode = new StatementNode(AsmCode::WRITE, expression->identifier);
                            writeNode->expressionNode = expression; 
                            
                            $$.from = writeNode;
                            $$.to = writeNode;

                        }
                        | /*epsilon*/
                        {
                            
                            $$.from = $$.to = nullptr;

                        }
    | t_begin statementlist t_end {

                            $$ = $2; 

                        }
    | t_if condition t_then statement  {


                            StatementNode* ifStatement = new StatementNode(AsmCode::JUMP_FALSE, global_JumpCounter);
                            
                            ExpressionNode* condition = $2;
                            ifStatement->expressionNode = condition;
                            
                            ifStatement->next = $4.from;

                            StatementNode* endIfStatement = new StatementNode(AsmCode::NOP, global_JumpCounter);
                            
                            ifStatement->nextStatementChain = endIfStatement;
                            
                            global_JumpCounter++;

                            $$.from = ifStatement;
                            $$.to = endIfStatement;   
                        }

    | t_while condition t_do statement{

                            int currentCounter = global_JumpCounter;
                            global_JumpCounter++;
                            global_JumpCounter++;

                            StatementNode* nopStartNode = new StatementNode(AsmCode::NOP, currentCounter);
                            StatementNode* whileStartNode = new StatementNode(AsmCode::JUMP_FALSE, currentCounter + 1);

                            StatementNode* whileEndNode = new StatementNode(AsmCode::JUMP, currentCounter);
                            StatementNode* nopEndNode = new StatementNode(AsmCode::NOP, currentCounter + 1);

                            nopStartNode->next = whileStartNode;

                            ExpressionNode * condition = $2;
                            whileStartNode->expressionNode = condition;

                            whileStartNode->nextStatementChain = whileEndNode;
                            
                            whileStartNode->next = $4.from;

                            whileEndNode->next = nopEndNode;

                            $$.from = nopStartNode;
                            $$.to = nopEndNode;

                        };


statementlist:  statement
                        { 

                            $$ = $1;

                        }
                        | statementlist t_semik statement
                        {

                            if( $1.from == nullptr && $1.to == nullptr && 
                                $3.from == nullptr && $3.to == nullptr )
                            {
                                $$ = $1;
                            }
                            else if($1.from == nullptr && $1.to == nullptr)
                            {
                                $$ = $3;
                            }
                            else if($3.from == nullptr && $3.to == nullptr)
                            {
                                $$ = $1;
                            }
                            else {
                                $$ = $1;
                                $$.to->next = $3.from;
                                $$.to = $3.to;
                            }
                            
                        };


condition:
            t_odd expression            {$$ = new ExpressionNode(AsmCode::ODD, $2, nullptr); }
        |   expression t_eq expression  {$$ = new ExpressionNode(AsmCode::EQUAL, $1, $3);  }
        |   expression t_ne expression  {$$ = new ExpressionNode(AsmCode::NOT_EQUAL, $1, $3);} 
        |   expression t_lt expression  {$$ = new ExpressionNode(AsmCode::LESS, $1, $3);}
        |   expression t_le expression  {$$ = new ExpressionNode(AsmCode::LESS_OR_EQUAL, $1, $3);}
        |   expression t_gt expression  {$$ = new ExpressionNode(AsmCode::GREATER, $1, $3);}
        |   expression t_ge expression  {$$ = new ExpressionNode(AsmCode::GREATER_OR_EQUAL, $1, $3);};

expression: 
         expression  t_plus term {$$ = new ExpressionNode(AsmCode::PLUS, $1, $3);}
        |   expression  t_minus term{ $$ = new ExpressionNode(AsmCode::MINUS, $1, $3);}
        |   term  {$$ = $1;}
        ;

term:   factor {$$ = $1;}
    |   term t_mult factor{$$ = new ExpressionNode(AsmCode::MULT, $1, $3);}
    |   term t_div factor{$$ = new ExpressionNode(AsmCode::DIV, $1, $3);};

factor: t_ident {
    int level;
    int offset;
    int result = st->lookup($1, constante, level, offset);
    
    if (result != 0) {
        result = st->lookup($1, variable, level, offset);
        
        if (result != 0) {
            printf("Error in factor with identifier: %s\n", $1);
            $$ = nullptr;
        } else {
            ExpressionNode* variableNode = new ExpressionNode($1, AsmCode::VAR, nullptr, nullptr);
            variableNode->level = level;
            variableNode->offset = offset;
            $$ = variableNode;
        }
    } else {
        ExpressionNode* constant = new ExpressionNode($1, AsmCode::CONST, nullptr, nullptr);
        constant->value = offset;
        constant->level = level;
        constant->offset = offset;
        $$ = constant;
    }
}

    |   t_number{$$ = new ExpressionNode(std::to_string($1), AsmCode::NUMBER, nullptr, nullptr);}
    |   t_bra_o expression t_bra_c{$$ = $2;}
    |   t_plus factor{$$ = $2; }
    |   t_minus factor{$$ = new ExpressionNode(AsmCode::CHANGESIGN, nullptr, $2);};
    
%%

int yyerror(char const* s) 
{
	printf("%s\n", s);
	return 1;
}
