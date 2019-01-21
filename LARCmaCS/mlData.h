#pragma once

#include <iostream>
#include "engine.h"
#include "matrix.h"

typedef struct
{
	std::string	name;
	const char *file_of_matlab;
	int RULE_AMOUNT;
	int RULE_LENGTH;
	int BACK_AMOUNT;
	int BACK_LENGTH;
} RCConfig;

class MlData
{
public:
	MlData(){}
	MlData(RCConfig conf);
	MlData(const MlData &data);

	RCConfig config;

	Engine  *ep;
	mxArray *Rule;
	mxArray *Ball;
	mxArray *Blue;
	mxArray *Yellow;
	mxArray *Back_Params;
	mxArray *Back_Amount;
	mxArray *Back_Length;
	mxArray *ballInside;
};
