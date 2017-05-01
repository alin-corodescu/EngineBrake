// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <utility>
/**
 * Singleton class used to handle score calculation for events which award score
 */
class ENGINEBRAKE_API ScoreCalculator
{
private:
	//! Pointer to the instance of this singleton
	static ScoreCalculator* Instance;

	//! Thresholds between which an Upshift is awarded score
	std::pair<float, float> GoodShiftThresholds;

	ScoreCalculator();
	~ScoreCalculator();
public:
	//! Value by which to multiply the score awards, is increased when requesting fuel
	float ScoreMultiplier;

	//! Computes the score awarded for an Upshift which happened at a given RPM
	float ComputeUpshiftScore(float RPM);

	//! Computes the score awarded for the time spent within the level
	float ComputeTickingScore(float Delta);

	//! Getter for the singleton instance of this class
	static ScoreCalculator* GetInstance() {
		if (Instance == NULL)
			Instance = new ScoreCalculator();
		return Instance;
	}
};
