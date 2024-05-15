#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
using namespace std;

class AsmCode{

    public: 

        enum StatementType
        {
        	READ,
            WRITE, 
			ASSIGN, 
            CALL, 
            JUMP_FALSE, 
            JUMP,
			NOP
        };


        enum ExpressionType
	    {   
		    VAR, 
		    NUMBER, 
		    CONST, 
			EQUAL, 
		    NOT_EQUAL, 
		    LESS, 
		    LESS_OR_EQUAL, 
		    GREATER, 
		    GREATER_OR_EQUAL, 
		    ODD, 
		    MULT, 
		    DIV, 
			PLUS, 
		    MINUS, 
		    CHANGESIGN
	    };

        ~AsmCode();

        static AsmCode* getInstance();

        FILE * f;

        void setFile(FILE* file);

        void writeProcedureHeader(string identifier, int numberVariables){
            if (identifier == "main"){
		        emit_Command_Argument_Comment("loadc", "0", "RAM-INIT");
		        emit_Command_Argument("loadc", numberVariables);
	        }
	        else{
		        emit_Label_Command_Argument(identifier, "loadc", numberVariables);
	        }

            writeDefaultProcedureHeader();
        }



        void writeProcedureFooter(){
            emit_Command_Argument("loadr", 0);
	        emit_Command_Argument("dec", 1);
	        emit_Command("loads");
	        emit_Command_Argument("storer", 0);
	        emit_Command("return");
        };

        void writeExpression(ExpressionType type, int offset, int level, string identifier, int constValue)
        {

        	switch (type)
        	{
        	    case VAR:
        	        emit_Command_Argument("loadr", 0);
        		    for (int i = 0; i < level; i++)
        		    {
        			    emit_Command("loads");
        		    }
        		    emit_Command_Argument("dec", 2);
        		    emit_Command_Argument("dec", offset);
        		    emit_Command("loads");
        		    break;

        	    case CONST:
        		    emit_Command_Argument("loadc", constValue);		
        		    break;

        	    case NUMBER:
        	        emit_Command_Argument("loadc", identifier);
        		    break;

        	    case EQUAL:
        	    	emit_Command("cmpeq");
        	    	break;

        	    case NOT_EQUAL:
        	    	emit_Command("cmpne");
        	    	break;

        	    case LESS:
        	    	emit_Command("cmplt");
        	    	break;

        	    case LESS_OR_EQUAL:
        	    	emit_Command("cmple");
        	    	break;

        	    case GREATER:
        	    	emit_Command("cmpgt");
        	    	break;

        	    case GREATER_OR_EQUAL:
        	    	emit_Command("cmpge");
        	    	break;

        	    case ODD:
        	    	emit_Command_Argument("loadc",2);
        	    	emit_Command("mod");
        	    	break;

        	    case MULT:
        	    	emit_Command("mult");
        	    	break;

        	    case DIV:
        	    	emit_Command("div");
		            break;

        	    case PLUS:
        	    	emit_Command("add");
        	    	break;

        	    case MINUS:
        	    	emit_Command("sub");
        	    	break;

	            case CHANGESIGN:
		            emit_Command("chs");
		            break;

	            default:
		            break;
	            }
        }


    void writeAssignmentHeader(int level, int offset)
    {
            emit_Command_Argument("loadr", 0);
            emitLevelTimesLoads(level);
            emit_Command_Argument("dec", 2);
            emit_Command_Argument("dec", offset);
    }

    void writeAssignmentFooter()
    {
            emit_Command("swap");
            emit_Command("stores");
    }

    void writeStatement(StatementType type, int level, int offset, string identifier, int jumpTo)
    {
        
        switch (type)
	    {
			case READ:
                emit_Command("read");
                emit_Command_Argument("loadr", 0);
	        	emitLevelTimesLoads(level);
                emit_Command_Argument("dec",2);
                emit_Command_Argument("dec",offset);
	        	emit_Command("stores");
	        	break;

	        case WRITE:
                emit_Command("write");
	        	break;

	        case CALL:
                emit_Command_Argument("loadr", 0);
                emitLevelTimesLoads(level);
                emit_Command_Argument("call", identifier);
	        	break;

	        case JUMP_FALSE:
                emit_Command_Argument("jumpz", jumpTo);
	        	break;

	        case JUMP:
                emit_Command_Argument("jump", jumpTo);
	        	break;

	        case NOP:
                emit_Label_Command(std::to_string(jumpTo), "nop");
	        	break;

	        default:
	        	break;
	    }
    }




    private:

        void emitLevelTimesLoads(int level){
            for (int i = 0; i < level; i++)
	        {
                 emit_Command("loads");
	        }
        }

        void writeDefaultProcedureHeader(){
	        emit_Command_Argument("loadr", 0);
	        emit_Command("add");
	        emit_Command_Argument("inc", 2);
	        emit_Command("dup");
	        emit_Command_Argument("dec", 1);
	        emit_Command_Argument("loadr", 0);
	        emit_Command("swap");
	        emit_Command("stores");
	        emit_Command("dup");
	        emit_Command_Argument("storer", 0);
	        emit_Command("stores");
        };

	    AsmCode();

	    static AsmCode* instance;

        void emit_Label_Command_Argument_Comment(string label, string command, string argument, string comment){
            fprintf(f, "%s\t%s\t%s\t#%s", label.c_str(), command.c_str(), argument.c_str(), comment.c_str());
	        fprintf(f, "\n");
        };

        void emit_Label_Command_Argument_Comment(string label, string command, int argument, string comment){
            fprintf(f, "%s\t%s\t%d\t#%s", label.c_str(), command.c_str(), argument, comment.c_str());
	        fprintf(f, "\n");
        };

        void emit_Label_Command_Argument(string label, string command, string argument){
            fprintf(f, "%s\t%s\t%s", label.c_str(), command.c_str(), argument.c_str());
	        fprintf(f, "\n");
        };
        
        void emit_Label_Command_Argument(string label, string command, int argument){
            fprintf(f, "%s\t%s\t%d", label.c_str(), command.c_str(), argument);
	        fprintf(f, "\n");
        };

        void emit_Label_Command(string label, string command){
            fprintf(f, "%s\t%s", label.c_str(), command.c_str());
	        fprintf(f, "\n");
        };

        void emit_Command_Argument_Comment(string command, string argument, string comment){
            fprintf(f, "\t%s\t%s\t#%s", command.c_str(), argument.c_str(), comment.c_str());
	        fprintf(f, "\n");
        }

        void emit_Command_Argument(string command, string argument){
            fprintf(f, "\t%s\t%s", command.c_str(), argument.c_str());
	        fprintf(f, "\n");
        };

        void emit_Command_Argument(string command, int argument){
            fprintf(f, "\t%s\t%d", command.c_str(), argument);
            fprintf(f, "\n");
        };

        void emit_Command_Comment(string command, string comment){
            fprintf(f, "\t%s\t\t#%s", command.c_str(), comment.c_str());
            fprintf(f, "\n");
        };
        
        void emit_Command(string command){
            fprintf(f, "\t%s", command.c_str());
            fprintf(f, "\n");
        };

        void emit(const string  label, const char * command, int arg, const char * comment) {
	        fprintf(f, "%s\t%s\t%d", label.c_str(), command, arg);
	        if (comment != NULL)
		        fprintf(f, "\t# %s", comment);
	        fprintf(f, "\n");
        };

        void emit(const string label, const char * command, const char * comment) {
	        fprintf(f, "%s\t%s\t", label.c_str(), command);
	        if (comment != NULL)
	            fprintf(f, "\t# %s", comment);
	        fprintf(f, "\n");
        };

        void emit(const string label, const char * command, const char * arg, const char * comment) {
	        fprintf(f, "%s\t%s", label.c_str(), command);
	        fprintf(f, "\t%s", arg);
	        if (comment != NULL)
		        fprintf(f, "\t# %s", comment);
	        fprintf(f, "\n");
        };
};
