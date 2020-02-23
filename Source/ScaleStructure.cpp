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
	currentScaleSize = scaleSizes[indexOfSize];
}

void ScaleStructure::calculateProperties()
{
	scaleSizes.clear();
	keyboardTypes.clear();
	pgCoordinates.clear();

	Array<int> cf = getContinuedFraction(generator, period);

	// seed the sequence
	Point<int> parent1 = Point<int>(-1 + cf[0], 1);
	Point<int> parent2 = Point<int>(1, 0);
	Point<int> gp = Point<int>(cf[0], 1);

	scaleSizes.add(gp.y);
	keyboardTypes.add(gp);
	pgCoordinates.add(Array<Point<int>>({parent1, parent2}));

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
			{
				parent1 = gp;
			}
			else // if odd, set parent1 to previous result
			{
				parent2 = gp;
			}

			packet = { parent1, parent2, parent1 + parent2 };
		}
	}

	// find step sizes
	calculateSizeVectors();
}

void ScaleStructure::calculateSizeVectors()
{
	Point<int> stepSizesOut;
	Point<int> periodCoordinate;
	Point<int> generatorCoordinate;

	int gMult, pMult, lcmX, lcmY;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		periodCoordinate = pgCoordinates[currentScaleSize][0];
		generatorCoordinate = pgCoordinates[currentScaleSize][1];

		// find horiztonal step size (X)
		lcmY = getLCM(periodCoordinate.y, generatorCoordinate.y);
		gMult = (lcmY / generatorCoordinate.y) * generator;
		pMult = (lcmY / periodCoordinate.y) * period;
		stepSizesOut.setX(jmax(pMult, gMult) - jmin(pMult, gMult));

		// find upward right step size (Y)
		lcmX = getLCM(periodCoordinate.x, generatorCoordinate.x);
		gMult = (lcmX / generatorCoordinate.x) * generator;
		pMult = (lcmX / periodCoordinate.x) * period;
		stepSizesOut.setY(jmax(pMult, gMult) - jmin(pMult, gMult));

		stepSizes.add(stepSizesOut);
	}
}