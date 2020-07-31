#pragma once

#include <iostream>
#include "engine.h"
#include "constants.h"
#include "matrix.h"

typedef struct
{
	std::string	name = "Robofootball";
	const char *file_of_matlab = "main";
	int RULE_AMOUNT = 16;
	int RULE_LENGTH = 7;
	int BACK_AMOUNT = 10;
	int BACK_LENGTH = 8;
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
	mxArray *state;
	mxArray *team;
	mxArray *partOfFieldLeft;
	mxArray *fieldInfo;
};
