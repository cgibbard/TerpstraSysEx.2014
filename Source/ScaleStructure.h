/*
  ==============================================================================

    ScaleStructure.h
    Created: 22 Feb 2020 7:13:41pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "ScaleMath.h"

/*
	A class that contains the properties of a scale that can be mapped to the Lumatone keyboard.
	This will calculate the horiztonal & vertical step sizes that best maps a given scale
	and provides a way to group scale degrees so that colours can be applied in a complimentary way.
*/
class ScaleStructure
{
	int period;
	int generator;

	Array<int> coprimeGenerators; // Degrees that produce MOS scales
	
	// Scale data after generator is chosen
	Array<int> scaleSizes;
	Array<Point<int>> keyboardTypes; // [(generator degree, scale size), ...]
	Array<Array<Point<int>>> pgCoordinates; // [[(coordinate of period, coordinate of generator)], ...]
	Array<Point<int>> stepSizes; // [(horizontal, right upward), ...]

	// Properties for grouping scale degrees, based off of scale size chosen
	int currentScaleSize = -1; // Index of size
	Array<int> sizeGroupings; // Each value refers to an index of scale sizes. Needs to add up to period.
	Array<Array<int>> degreeGroupings; // The scale degrees within each group
	Array<Colour> groupColours;

	void calculateProperties();
	void calculateSizeVectors();

public:

	ScaleStructure(int periodIn, int generatorIn=0);

	Array<int> getCoprimeGenerators();
	int getCoprimeGenerator(int index);
	
	int getPeriod();
	int getGenerator();

	Array<int> getScaleSizes();
	int getScaleSize(int index);
	int getCurrentScaleSize();

	Array<Point<int>> getKeyboardTypes();
	Point<int> getKeyboardType(int indexOfSize);
	Point<int> getCurrentKeyboardType();

	Array<Point<int>> getPGCoordinates(int indexOfSize);
	Array<Point<int>> getCurrentPGCoordinates();
	
	Point<int> getStepSizeVector(int indexOfSize);
	Point<int> getCurrentStepSizes();

	void chooseCoprimeGenerator(int index);
	void setGenerator(int generatorIn);
	void setSize(int indexOfSize);

	// TODO: Method for constructing size groupings

	void setGroupColour(int groupIndex, Colour colour);
};