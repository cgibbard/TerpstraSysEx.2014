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
	generatorIndex = generatorIn;

	coprimeGenerators = getCoprimes(period);

	if (generatorIndex >= 0)
		calculateProperties();
}

ScaleStructure::ScaleStructure(const ScaleStructure& scaleToCopy)
{
	period = scaleToCopy.period;
	generatorIndex = scaleToCopy.generatorIndex;
	generatorOffset = scaleToCopy.generatorOffset;
	currentSizeIndex = scaleToCopy.currentSizeIndex;

	coprimeGenerators = getCoprimeGenerator(period);
	calculateProperties();
	
	applyGrouping(scaleToCopy.sizeGrouping);
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

int ScaleStructure::getGeneratorIndex()
{
	return generatorIndex;
}

int ScaleStructure::getGeneratorOffset()
{
	return generatorOffset;
}

Array<int> ScaleStructure::getScaleSizes()
{
	return scaleSizes;
}

int ScaleStructure::getScaleSize(int index)
{
	return scaleSizes[index];
}

int ScaleStructure::getCurrentSizeIndex()
{
	return currentSizeIndex;
}

int ScaleStructure::getCurrentScaleSize()
{
	return scaleSizes[currentSizeIndex];
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
	return keyboardTypes[currentSizeIndex];
}

Array<Point<int>> ScaleStructure::getPGCoordinates(int indexOfSize)
{
	return pgCoordinates[indexOfSize];
}

Array<Point<int>> ScaleStructure::getCurrentPGCoordinates()
{
	return pgCoordinates[currentSizeIndex];
}

Point<int> ScaleStructure::getStepSizeVector(int indexOfSize)
{
	return stepSizes[indexOfSize];
}

Point<int> ScaleStructure::getCurrentStepSizes()
{
	return stepSizes[currentSizeIndex];
}

void ScaleStructure::resetPeriod(int periodIn)
{
	period = periodIn;
	coprimeGenerators = getCoprimes(period);
	generatorIndex = -1;
	currentSizeIndex = -1;

	scaleSizes.clear();
	keyboardTypes.clear();
	pgCoordinates.clear();
	stepSizes.clear();
}

void ScaleStructure::chooseCoprimeGenerator(int indexIn)
{
	generatorIndex = coprimeGenerators[indexIn];
	if (coprimeGenerators.contains(generatorIndex))
	{
		calculateProperties();
	}
}

void ScaleStructure::setGeneratorIndex(int generatorIn)
{
	generatorIndex = generatorIn;

	// TODO: add error checking, maybe workarounds for non-coprime input

	if (generatorIndex > 0)
		calculateProperties();
}

void ScaleStructure::setSizeIndex(int indexOfSize)
{
	currentSizeIndex = indexOfSize;
}

void ScaleStructure::setGeneratorOffset(int offsetIn)
{
	generatorOffset = offsetIn;
}

/*
	Based on Erv Wilson's Scale Tree / Gral Keyboard algorithm.
*/
void ScaleStructure::calculateProperties()
{
	scaleSizes.clear();
	keyboardTypes.clear();
	pgCoordinates.clear();

	Array<int> cf = getContinuedFraction((double)generatorIndex / period);

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

	generatorChain = getGeneratorChain(generatorIndex, period);

	// find step sizes
	calculateStepSizes();
}

/*
	This algorithm can be modified to produce cents values of L & s step sizes of MOS scales.
*/
void ScaleStructure::calculateStepSizes()
{
	Point<int> stepSizesOut;
	Point<int> periodCoordinate;
	Point<int> generatorCoordinate;
	int generator = coprimeGenerators[generatorIndex];

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

void ScaleStructure::fillSymmetricDegreeGrouping()
{
	degreeGrouping.clear();
	degreeGrouping.resize(sizeGrouping.size());

	// Fill a degree groups that is arranged symmetrically

	int indexOffset = modulo(generatorOffset, period);

	for (int t = 0; t < sizeGrouping.size(); t++)
	{
		for (int n = 0; n < scaleSizes[sizeGrouping[t]]; n++)
		{
			degreeGrouping.getReference(t).add(generatorChain[indexOffset]);
			indexOffset = modulo(indexOffset + 1, period);
		}
	}
}


int ScaleStructure::getSuggestedGeneratorIndex()
{
	int index = -1;
	float dif1, dif2 = 10e6;
	float interval = 1200 / period;

	for (int i = 1; i < coprimeGenerators.size(); i++)
	{
		dif1 = abs(702 - interval * coprimeGenerators[i]);

		if (dif1 < dif2)
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

int ScaleStructure::getSuggestedSizeIndex()
{
	int index = -1;
	int dif1, dif2 = INT_MAX;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		dif1 = abs(7 - scaleSizes[i]);

		if (dif1 < dif2)
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

int ScaleStructure::getPrefferedSizeIndex(int prefferedSize, bool preferLarger)
{
	int index = -1;
	int dif1, dif2 = INT_MAX;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		dif1 = abs(prefferedSize - scaleSizes[i]);

		if (dif1 < dif2 || (preferLarger && dif1 == dif2))
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

Array<int> ScaleStructure::getNestedSizeGrouping()
{
	int scaleSize = scaleSizes[currentSizeIndex];
	Array<int> grouping = { currentSizeIndex };

	int notesLeft = period - scaleSize;
	int subSizeInd = currentSizeIndex;
	int subSize = scaleSize;

	while (notesLeft > 0)
	{
		int q = notesLeft / subSize;

		if (notesLeft <= subSize && scaleSizes.contains(notesLeft))
		{
			grouping.add(scaleSizes.indexOf(notesLeft));
			notesLeft = 0;
		}
		else if (q >= 2)
		{
			int numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
			for (int n = 0; n < numToAdd; n++)
			{
				grouping.add(subSizeInd);
				notesLeft -= subSize;

				// check if notesLeft can be divided equally by next size
				if (subSizeInd > 0)
				{
					int stagedSubInd = subSizeInd - 1;
					int stagedSubSize = scaleSizes[stagedSubInd];
					int q2 = notesLeft / stagedSubSize;

					if (notesLeft % stagedSubSize == 0)
					{
						// extra check for symmetry
						if (q2 % 2 == 0)
						{
							for (int qn = 0; qn < q2; qn++)
							{
								grouping.add(stagedSubInd);
								notesLeft -= stagedSubSize;
							}

							break;
						}
					}
				}
			}
		}

		subSize = scaleSizes[--subSizeInd];
	}

	return grouping;
}

// TODO: possibility for improving large scales groupings.
// When checking the "staged" sizes, there could be an additional check to see if
// a different configuration of those two sizes produce a smaller grouping
// ex. with this algorithm, "Schismatic" 69/118 scale gives (7 * 4) + (5 * 18) or 22 groups
// but this could be reduced to (7 * 14) + (5 * 4) for 18 groups
Array<int> ScaleStructure::getComplimentarySizeGrouping()
{
	int scaleSize = scaleSizes[currentSizeIndex];
	Array<int> grouping = { currentSizeIndex };

	int notesLeft = period - scaleSize;
	int subSizeInd = currentSizeIndex;
	int subSize = scaleSize;

	int q = notesLeft / subSize;
	int numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
	int num = 0;

	while (notesLeft > 0)
	{
		// check if notesLeft can be divided equally by next size
		if (subSizeInd > 0)
		{
			int stagedSubInd = subSizeInd - 1;
			int stagedSubSize = scaleSizes[stagedSubInd];
			int q2 = notesLeft / stagedSubSize;

			if (notesLeft % stagedSubSize == 0)
			{
				// extra check for symmetry
				int groupSize = grouping.size() - 1;
				if (groupSize == 0 || q2 % 2 == 0)
				{
					for (int qn = 0; qn < q2; qn++)
					{
						grouping.add(stagedSubInd);
						notesLeft -= stagedSubSize;
					}

					break;
				}
			}
		}

		if (notesLeft <= subSize && scaleSizes.contains(notesLeft))
		{
			grouping.add(scaleSizes.indexOf(notesLeft));
			notesLeft = 0;
		}
		else if (q >= 0 && num < numToAdd)
		{
			grouping.add(subSizeInd);
			notesLeft -= subSize;
			num++;
		}
		else if (notesLeft > 0)
		{
			subSize = scaleSizes[--subSizeInd];
			q = notesLeft / subSize;
			numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
			num = 0;
		}
	}

	return grouping;
}

void ScaleStructure::applyGrouping(Array<int> sizeGroupingIn)
{
	// TODO: check if valid
	sizeGrouping = sizeGroupingIn;
	fillSymmetricDegreeGrouping();
}

void ScaleStructure::applySuggestedSizeGrouping()
{
	Array<Array<int>> groupings;
	groupings.add(getNestedSizeGrouping());
	groupings.add(getComplimentarySizeGrouping());

	Array<float> groupingScores;

	// create score by averaging range and length of group
	for (auto g : groupings)
	{
		int range = g[0] - g[g.size() - 1];
		groupingScores.add((range + g.size()) / (float)groupings.size());
	}

	// find lowest score and return respective group
	int index = 0;
	for (int s = 0; s < groupings.size(); s++)
	{
		if (groupingScores[s] < groupingScores[index])
			index = s;
	}

	// make symmetric and fill degree groupings
	// NOTE: if grouping cannot be made symmetric, for now the pattern will be blank
	applyGrouping(arrangeSymmetrically(groupings[index]));
}