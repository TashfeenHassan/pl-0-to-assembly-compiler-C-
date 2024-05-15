#pragma once
#include <string>
#include <vector>
#include "AsmCode.h"

class IdentifierNode
{
	public:
		IdentifierNode(std::string identifier);
		~IdentifierNode();

		IdentifierNode* next;
		std::string identifier;
	
		int value;

		void getNumberOfVariablesInChain(int & numVariables);
};

class ExpressionNode
{
	public:

		ExpressionNode(AsmCode::ExpressionType type, ExpressionNode* leftNode, ExpressionNode* rightNode);

		ExpressionNode(std::string id, AsmCode::ExpressionType type, ExpressionNode* leftNode, ExpressionNode* rightNode);

		~ExpressionNode();

		AsmCode::ExpressionType type;

		ExpressionNode* leftNode;
		ExpressionNode* rightNode;

		std::string identifier;
		int level;
		int offset;

		int value;

		void generateAssemblerCodeForExpressionTree();

		void writeMyNodeAsAssemblerCode();
};

class StatementNode
{
public:

	StatementNode(AsmCode::StatementType type, std::string identifier, int jumpTarget);

	StatementNode(AsmCode::StatementType type, int jumpTarget);

	StatementNode(AsmCode::StatementType type, std::string identifier);

	StatementNode(AsmCode::StatementType type);

	~StatementNode();


	AsmCode::StatementType type;


	StatementNode* next;
	ExpressionNode* expressionNode;


	int jumpTarget;


	StatementNode* nextStatementChain;

	std::string identifier;
	int level;
	int offset;

	void generateAssemblerCode();
};

class BlockNode
{
public:
	BlockNode(std::string identifier, StatementNode* firstStatementNode, IdentifierNode* firstConstantNode, IdentifierNode* firstVariableNode);
	~BlockNode();

	std::string identifier;

	StatementNode* firstStatementNode;
	IdentifierNode* firstConstantNode;
	IdentifierNode* firstVariableNode;

	int numberOfVariables;
	int level;
	int offset;

	void generateAssemblerCode();
	void resolveAmountVariables();
};

class DataModel
{
	public:
		~DataModel();
		static DataModel* getInstance();

		BlockNode* mainProcedure;
		std::vector<BlockNode*> procedures;
		int currentLevel;

		void prepareAssemberGeneration();
		void generateAssember();

	private:
		DataModel();
		static DataModel* instance;
};








