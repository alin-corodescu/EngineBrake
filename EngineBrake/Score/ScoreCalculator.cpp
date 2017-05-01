// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineBrake.h"
#include "ScoreCalculator.h"


#define LOW_RPM 3000.0f
#define HIGH_RPM 4000.0f
#define UPSHIFT_SCORE 100.0f
#define TICK_SCORE_SECOND 10.0f
ScoreCalculator* ScoreCalculator::Instance = NULL;

ScoreCalculator::ScoreCalculator()
{
	GoodShiftThresholds = std::make_pair(LOW_RPM, HIGH_RPM);
	ScoreMultiplier = 1;
}

ScoreCalculator::~ScoreCalculator()
{
}

float ScoreCalculator::ComputeUpshiftScore(float RPM)
{
	if (RPM > GoodShiftThresholds.first && RPM < GoodShiftThresholds.second)
		// Good shift occured
		return UPSHIFT_SCORE * ScoreMultiplier;
	return 0.0f;
}

float ScoreCalculator::ComputeTickingScore(float Delta)
{
	return TICK_SCORE_SECOND * Delta * ScoreMultiplier;
}
