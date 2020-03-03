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
	int generatorIndex = -1;

	Array<int> coprimeGenerators; // Degrees that produce MOS scales
	
	// Scale data after generator is chosen
	Array<int> scaleSizes;
	Array<Point<int>> keyboardTypes; // [(generator degree, scale size), ...]
	Array<Array<Point<int>>> pgCoordinates; // [[(coordinate of period, coordinate of generator)], ...]
	Array<Point<int>> stepSizes; // [(horizontal, right upward), ...]
	Array<int> generatorChain; // All degrees in the scale as a chain of generators

	int currentSizeIndex = -1; // Index of size
	int generatorOffset = 0;

	// Properties for grouping scale degrees, based off of scale size chosen
	Array<int> sizeGrouping; // Each value refers to an index of scale sizes. Needs to add up to period.
	Array<Array<int>> degreeGrouping; // The scale degrees within each group
	Array<Colour> groupColours;

	// Calculates the necessary data after a period and generator are chosen
	void calculateProperties();

	// Calculates the step sizes that should be applied to the keyboard
	void calculateStepSizes();

	// Separates scale degrees based on the current sizeGrouping
	void fillSymmetricDegreeGrouping();

public:

	ScaleStructure(int periodIn, int generatorIn=-1);
	ScaleStructure(const ScaleStructure& scaleToCopy);

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

	void resetPeriod(int periodIn);
	void chooseCoprimeGenerator(int index);
	void setGeneratorIndex(int generatorIn);
	void setSizeIndex(int indexOfSize);

	/*	
		Returns the index whose generator is closest to a perfect fifth
	*/
	int getSuggestedGeneratorIndex();

	/*
		Returns an index whose size is closest to 7
	*/
	int getSuggestedSizeIndex();

	/*
		Returns an index closest to prefferedSize, with an additional size option
	*/
	int getPrefferedSizeIndex(int prefferedSize, bool preferLarger = true);

	/*
		Returns a symmetrical size grouping that tries to use as many of the 
		main scale size, and iterates to each smaller size until full.
	*/
	Array<int> getNestedSizeGrouping();

	/*
		Returns a symmetrical size grouping that with each iteration, looks for 
		the best combination of the current size plus the next smaller size.
	*/
	Array<int> getComplimentarySizeGrouping();

	/*
		Sets sizeGrouping to input if valid, then fills degrees
	*/
	void applyGrouping(Array<int> sizeGroupingIn);

	/*
		Employs a "best out of two" procedure with the above two 
		size grouping algorithms, then applies it to the current structure
		TODO: After preset implementation, allow this to override algorithmic groupings
		with predefined ones if they exist
	*/
	void applySuggestedSizeGrouping();

	// TODO: Method for custom size groupings
};