/*
  ==============================================================================

    ScaleStructure.cpp
    Created: 22 Feb 2020 7:13:41pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "ScaleStructure.h"

ScaleStructure::ScaleStructure(int periodIn, int generatorIn)
{
	period = periodIn;
	generator = generatorIn;

	coprimeGenerators = getCoprimes(period);

	if (generator > 0)
		calculateProperties();
}

Array<int> ScaleStructure::getCoprimeGenerators()
{
	return coprimeGenerators;
}

int ScaleStructure::getCoprimeGenerator(int index)
{
	return coprimeGenerators[index];
}

int ScaleStructure::getPeriod()
{
	return period;
}

int ScaleStructure::getGenerator()
{
	return generator;
}

Array<int> ScaleStructure::getScaleSizes()
{
	return scaleSizes;
}

int ScaleStructure::getScaleSize(int index)
{
	return scaleSizes[index];
}

int ScaleStructure::getCurrentScaleSize()
{
	return scaleSizes[currentScaleSize];
}

Array<Point<int>> ScaleStructure::getKeyboardTypes()
{
	return keyboardTypes;
}

Point<int> ScaleStructure::getKeyboardType(int indexOfSize)
{
	return keyboardTypes[indexOfSize];
}

Point<int> ScaleStructure::getCurrentKeyboardType()
{
	return keyboardTypes[currentScaleSize];
}

Array<Point<int>> ScaleStructure::getPGCoordinates(int indexOfSize)
{
	return pgCoordinates[indexOfSize];
}

Array<Point<int>> ScaleStructure::getCurrentPGCoordinates()
{
	return pgCoordinates[currentScaleSize];
}

Point<int> ScaleStructure::getStepSizeVector(int indexOfSize)
{
	return stepSizes[indexOfSize];
}

Point<int> ScaleStructure::getCurrentStepSizes()
{
	return stepSizes[currentScaleSize];
}

void ScaleStructure::chooseCoprimeGenerator(int indexIn)
{
	generator = coprimeGenerators[indexIn];
	if (coprimeGenerators.contains(generator))
	{
		calculateProperties();
	}
}

void ScaleStructure::setGenerator(int generatorIn)
{
	generator = generatorIn;

	// TODO: add error checking, maybe workarounds for non-coprime input

	if (generator > 0)
		calculateProperties();
}

void ScaleStructure::setSize(int indexOfSize)
{
	currentScaleSize = indexOfSize;
}

void ScaleStructure::useSuggestedGroupings()
{
	sizeGroupings.clear();
	
	// Find grouping
	sizeGroupings.add(currentScaleSize);

	Array<int> sizesChosenAndLess = scaleSizes;
	sizesChosenAndLess.removeRange(currentScaleSize + 1, scaleSizes.size());

	int groupingSum = scaleSizes[sizeGroupings[0]];
	int degreesRemaining = period - scaleSizes[sizeGroupings[0]];
	int subSizeInd = currentScaleSize - 1;
	int nextSubSize = scaleSizes[subSizeInd];
	int quotient = degreesRemaining / nextSubSize;
	int nextRemainder = degreesRemaining - (quotient * nextSubSize);

	// Needs testing
	while (groupingSum < period)
	{
		while (nextRemainder % 2 == 1 && !sizesChosenAndLess.contains(nextRemainder))
		{
			// Find new subsize if not compatible
			while (quotient < 1)
			{
				nextSubSize = scaleSizes[--subSizeInd];
				quotient = degreesRemaining / nextSubSize;
				nextRemainder = degreesRemaining - (quotient * nextSubSize);
			}
		}

		for (int i = 0; i < quotient; i++)
			sizeGroupings.add(subSizeInd);

		groupingSum += quotient * nextSubSize;
	}

	// For now, just print a message if it's a bad grouping
	if (groupingSum > period)
		DBG("Grouping went a little overboard. It is " + String(groupingSum) + " when it should be " + String(period));

	degreeGroupings.resize(sizeGroupings.size());

	// Fill degree groups symmetrically

	int indexForward = 0;
	int indexBackwards = period - 1;

	for (int t = 0; t < sizeGroupings.size(); t++)
	{
		for (int n = 0; n < scaleSizes[sizeGroupings[t]]; n++)
		{
			if (t % 2 == 0)
			{
				degreeGroupings.getReference(t).add(generatorChain[indexForward]);
				indexForward++;
			}
			else
			{
				degreeGroupings.getReference(t).add(generatorChain[indexBackwards]);
				indexBackwards--;
			}
		}
	}
}

/*
	Based on Erv Wilson's Scale Tree / Gral Keyboard algorithm.
*/
void ScaleStructure::calculateProperties()
{
	scaleSizes.clear();
	keyboardTypes.clear();
	pgCoordinates.clear();

	Array<int> cf = getContinuedFraction((double)generator / period);

	// seed the sequence
	Point<int> parent1 = Point<int>(-1 + cf[0], 1);
	Point<int> parent2 = Point<int>(1, 0);
	Point<int> gp = Point<int>(cf[0], 1);

	Array<Point<int>> packet = {gp, parent2, gp + parent2}; // makes for cleaner code

	// find keyboard types, and their period/generator coordinates
	for (int i = 1; i < cf.size(); i++)
	{
		for (int d = 0; d < cf[i]; d++)
		{
			pgCoordinates.add(Array<Point<int>>({ packet[0], packet[1] }));
			keyboardTypes.add(packet[2]);
			scaleSizes.add(packet[2].y);

			parent1 = packet[0];
			parent2 = packet[1];
			gp = packet[2];

			// if previous continued fraction index is even,
			// set parent2 to previous result
			if (i % 2 == 0)
				parent1 = gp;
			else // if odd, set parent1 to previous result
				parent2 = gp;

			packet = { parent1, parent2, parent1 + parent2 };
		}
	}

	generatorChain = getGeneratorChain(generator, period);

	// find step sizes
	calculateSizeVectors();
}

/*
	This algorithm can be modified to produce cents values of L & s step sizes of MOS scales.
*/
void ScaleStructure::calculateSizeVectors()
{
	Point<int> stepSizesOut;
	Point<int> periodCoordinate;
	Point<int> generatorCoordinate;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		generatorCoordinate = Point<int>(pgCoordinates[i][0].x, pgCoordinates[i][1].x);
		periodCoordinate = Point<int>(pgCoordinates[i][0].y, pgCoordinates[i][1].y);

		// find horiztonal step size (X)
		if (periodCoordinate.y == generatorCoordinate.y)
			stepSizesOut.setX(period - generator);
		else if (periodCoordinate.y == 0)
			stepSizesOut.setX(period);
		else if (generatorCoordinate.y == 0)
			stepSizesOut.setX(generator);
		else
			stepSizesOut.setX(abs(period * generatorCoordinate.y - generator * periodCoordinate.y));

		// find upward right step size (Y)
		if (periodCoordinate.x == generatorCoordinate.x)
			stepSizesOut.setY(period - generator);
		else if (periodCoordinate.x == 0)
			stepSizesOut.setX(period);
		else if (generatorCoordinate.y == 0)
			stepSizesOut.setX(generator);
		else
			stepSizesOut.setY(abs(period * generatorCoordinate.x - generator * periodCoordinate.x));

		stepSizes.add(stepSizesOut);
	}
}