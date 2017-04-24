// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <utility>
/**
 * 
 */
class ENGINEBRAKE_API ScoreCalculator
{
private:
	static ScoreCalculator* instance;
	std::pair<float, float> GoodShiftThresholds;
	ScoreCalculator();
	~ScoreCalculator();
public:
	//! Value of the Score Multiplier, may be used later on when requesting fuel
	float ScoreMultiplier;

	float ComputeUpshiftScore(float RPM);

	static ScoreCalculator* GetInstance() {
		if (instance == NULL)
			instance = new ScoreCalculator();
		return instance;
	}
};
