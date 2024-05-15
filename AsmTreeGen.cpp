#include <iostream>
#include <iostream>
#include <algorithm>
#include "AsmTreeGen.h"
using namespace std;


IdentifierNode::IdentifierNode(string identifier)
	: identifier(identifier)
{
}

IdentifierNode::~IdentifierNode()
{
}

void IdentifierNode::getNumberOfVariablesInChain(int & numVariables){

	numVariables++;
	
	if (next != nullptr)
	{
		next->getNumberOfVariablesInChain(numVariables);
	}
}

ExpressionNode::ExpressionNode(AsmCode::ExpressionType type, ExpressionNode* leftNode, ExpressionNode* rightNode)
	: type(type), leftNode(leftNode), rightNode(rightNode)
{
}


ExpressionNode::ExpressionNode(string identifier, AsmCode::ExpressionType type, ExpressionNode* leftNode, ExpressionNode* rightNode)
	: identifier(identifier), type(type), leftNode(leftNode), rightNode(rightNode)
{
}

ExpressionNode::~ExpressionNode()
{
}

void ExpressionNode::generateAssemblerCodeForExpressionTree()
{
	if (leftNode != nullptr)
	{
		leftNode->generateAssemblerCodeForExpressionTree();
	}
		
	if (rightNode != nullptr)
	{
		rightNode->generateAssemblerCodeForExpressionTree();
	}

	writeMyNodeAsAssemblerCode();
}

void ExpressionNode::writeMyNodeAsAssemblerCode()
{
	int val = 9999;
	if(type == AsmCode::CONST){
		val = value;
	}

	AsmCode::getInstance()->writeExpression(type, offset, level, identifier, val);
}

StatementNode::StatementNode(AsmCode::StatementType type, string identifier, int jumpTarget)
	: type(type), identifier(identifier), jumpTarget(jumpTarget)
{
	next = nullptr;
	nextStatementChain = nullptr;
	expressionNode = nullptr;
}

StatementNode::StatementNode(AsmCode::StatementType type, int jumpTarget)
	: type(type), jumpTarget(jumpTarget)
{
	next = nullptr;
	nextStatementChain = nullptr;
	expressionNode = nullptr;
}

StatementNode::StatementNode(AsmCode::StatementType type, string identifier)
	: type(type),  identifier(identifier)
{
	next = nullptr;
	nextStatementChain = nullptr;
	expressionNode = nullptr;
}

StatementNode::StatementNode(AsmCode::StatementType type)
	: type(type)
{
	next = nullptr;
	nextStatementChain = nullptr;
	expressionNode = nullptr;
}

StatementNode::~StatementNode()
{
}

void StatementNode::generateAssemblerCode()
{
	if (expressionNode != nullptr)
	{
		if (type == AsmCode::ASSIGN)
		{
			AsmCode::getInstance()->writeAssignmentHeader(level, offset);
            expressionNode->generateAssemblerCodeForExpressionTree();
            AsmCode::getInstance()->writeAssignmentFooter();
		}
		else
		{
			expressionNode->generateAssemblerCodeForExpressionTree();
			AsmCode::getInstance()->writeStatement(type, level, offset, identifier, jumpTarget);
		}
	}
	else if (expressionNode == nullptr){
		AsmCode::getInstance()->writeStatement(type, level, offset, identifier, jumpTarget);
    }
	if (next != nullptr)
	{
		next->generateAssemblerCode();
	}
	if (nextStatementChain != nullptr)
	{
		nextStatementChain->generateAssemblerCode();
	}
}

BlockNode::BlockNode(string identifier, StatementNode* firstStatementNode, IdentifierNode* firstConstantNode, IdentifierNode* firstVariableNode)
	: identifier(identifier), firstStatementNode(firstStatementNode), firstConstantNode(firstConstantNode), firstVariableNode(firstVariableNode)
{
	level = 0;
	offset = 0;
	numberOfVariables = 0;
}

BlockNode::~BlockNode()
{
}

void BlockNode::resolveAmountVariables()
{
	if (firstVariableNode != nullptr)
	{
		int chainLength = 0;
		firstVariableNode->getNumberOfVariablesInChain(chainLength);
		numberOfVariables = chainLength;
	}
}

void BlockNode::generateAssemblerCode()
{
	AsmCode::getInstance()->writeProcedureHeader(identifier, numberOfVariables);

	if(firstStatementNode == nullptr || firstStatementNode == NULL){
		
	} 
	else{
		firstStatementNode->generateAssemblerCode();
	}
	
	AsmCode::getInstance()->writeProcedureFooter();
}

DataModel* DataModel::instance = nullptr;

DataModel::DataModel()
{
	procedures = vector<BlockNode*>();
}

DataModel::~DataModel()
{
}

DataModel* DataModel::getInstance()
{
	if (instance == nullptr){
		instance = new DataModel();
	}

	return instance;
}


void DataModel::prepareAssemberGeneration()
{
	reverse(procedures.begin(), procedures.end());

	int i = 0;
	for (auto & blockNode : procedures)
	{
		blockNode->offset = i;
		i += 1;
	}

	for (auto& blockNode : procedures){
		blockNode->resolveAmountVariables();
	}
		
}


void DataModel::generateAssember()
{
	for (auto & blockNode : procedures)
	{
		currentLevel = blockNode->level;
		blockNode->generateAssemblerCode();
	}
}

