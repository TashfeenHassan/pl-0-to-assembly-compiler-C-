#include "AsmCode.h"

AsmCode* AsmCode::instance = nullptr;

AsmCode::AsmCode()
{
}

AsmCode::~AsmCode()
{
}

AsmCode* AsmCode::getInstance()
{
	if (instance == nullptr)
		instance = new AsmCode();

	return instance;
}

void AsmCode::setFile(FILE* file)
{
	f = file;
}