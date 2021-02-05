/*
  ==============================================================================

    HexagonTilingGeometry.cpp
    Created: 6 Jan 2021 11:42:30pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "HexagonTilingGeometry.h"

void LinearTiling::setBasesAngle(double thetaBetweenBasisVectors)
{
    theta = thetaBetweenBasisVectors;
    angleCos = cos(theta);
    angleSin = sin(theta);

    recalculateBasisComponents();
}

void LinearTiling::setHorizontalBasisRadius(double radius)
{
    radiusHorizontal = radius;
    recalculateBasisComponents();
}

void LinearTiling::setAngledBasisRadius(double radius)
{
    radiusAngled = radius;
    recalculateBasisComponents();
}

void LinearTiling::setTileRotationAngle(double angleIn)
{
    rotationAngle = angleIn;
    recalculateTileTransform(origin);
}

void LinearTiling::setOrigin(Point<double> originPointIn) 
{ 
    origin = originPointIn; 
    recalculateTileTransform(origin);
}

void LinearTiling::scaleRotatedTileToBounds(bool doScaling, Rectangle<double> boundsIn, bool centreInBounds)
{
    scaleToFitBounds = doScaling;
    givenBounds = boundsIn;

    Point<double> originToUse = (centreInBounds) ? boundsIn.getCentre() : origin;
    recalculateTileTransform(originToUse);
}

void LinearTiling::fitTilingTo(
    Point<float>        originPoint,
    Point<float>        firstBasisPoint,
    Point<float>        secondBasisPoint,
    bool                firstBasisIsColumn,
    int                 columnBasisOffset,
    int                 rowBasisOffset
)
{
    origin = originPoint.toDouble();
    Point<double> colNorm, rowNorm;

    colNorm = firstBasisPoint.toDouble() - origin;

    if (firstBasisIsColumn)
    {
        rowNorm = (secondBasisPoint - firstBasisPoint).toDouble();
    }
    else
    {
        rowNorm = colNorm;
        colNorm = (secondBasisPoint - firstBasisPoint).toDouble();
    }
    
    rotationAngle = atan(colNorm.y / colNorm.x);

     radiusHorizontal = colNorm.getDistanceFromOrigin() / columnBasisOffset;
    radiusAngled     = rowNorm.getDistanceFromOrigin() / rowBasisOffset;

    double theta = atan(rowNorm.y / rowNorm.x) - rotationAngle;

    setBasesAngle(theta);
    recalculateTileTransform(origin);
}


Point<double> LinearTiling::getPointFromOrigin(
    double columnAngleX, double columnAngleY, double rowAngleX, double rowAngleY, 
    double columnUnit, double rowUnit, double columnOffset, double rowOffset) 
{
    double colX = columnOffset * columnUnit * columnAngleX + rowOffset * rowUnit * rowAngleX;
    double colY = columnOffset * columnUnit * columnAngleY + rowOffset * rowUnit * rowAngleY;
    return { colX, colY };
}

AffineTransform LinearTiling::getTileMatrix() const
{
    AffineTransform matrix;
    matrix.mat00 = horizontalBasisScalar * radiusHorizontal;
    //matrix.mat10 = 0; // This is default
    matrix.mat01 = angledBasisScalar * angledXComponent;
    matrix.mat11 = angledBasisScalar * angledYComponent;
    return matrix;
}

Array<Point<float>> LinearTiling::transformPointsFromOrigin(const Array<Point<int>>& pointsIn, bool centreInBounds) const
{
    Array<Point<float>> pointsOut;
    AffineTransform matrix = getTileMatrix().followedBy(transform);

    for (auto point : pointsIn)
        pointsOut.add((point.toDouble().transformedBy(matrix)).toFloat());

    return pointsOut;
}

Array<Point<float>> LinearTiling::getPointsBetween(Point<int> horizontalRange, Point<int> verticalRange) const
{
    Array<Point<int>> points; // TODO enumerate input
    return transformPointsFromOrigin(points);
}

void LinearTiling::recalculateBasisComponents()
{
    angledXComponent = radiusAngled * angleCos;
    angledYComponent = radiusAngled * angleSin;
}

void LinearTiling::recalculateTileTransform(Point<double> rotationOrigin)
{
    transform = AffineTransform::translation(origin);

    // centre
    if (scaleToFitBounds)
    {
        // TODO: fix transformed size pre-calculation and get more accurate rotationScalar
        //auto transformedBounds = (lastCalculatedBounds.getWidth() == 0 || lastCalculatedBounds.getHeight() == 0)
        //    ? givenBounds.transformedBy(AffineTransform::rotation(rotationAngle)) // Position doesn't matter, we just care about the size
        //    : lastCalculatedBounds;
        auto transformedBounds = givenBounds.transformedBy(AffineTransform::rotation(rotationAngle)); // Position doesn't matter, we just care about the size


        // Find amount to scale by so it fits in the bounds
        rotationScalar = juce::jmin(
            (double)givenBounds.getWidth() / transformedBounds.getWidth(),
            (double)givenBounds.getHeight() / transformedBounds.getHeight()
        );

        // TODO: parameterize justification

        double centreWidth = (givenBounds.getWidth() - transformedBounds.getWidth() * rotationScalar) * 0.5;
        double centreHeight = (givenBounds.getHeight() - transformedBounds.getHeight() * rotationScalar) * 0.5;

        
        transform = transform.followedBy(AffineTransform::scale(rotationScalar, rotationScalar, rotationOrigin.x, rotationOrigin.y))
                             .followedBy(AffineTransform::rotation(rotationAngle, rotationOrigin.x, rotationOrigin.y))
                             .followedBy(AffineTransform::translation(-centreWidth, -centreHeight))
        ;

    }
    else
    {
        transform = transform.followedBy(AffineTransform::rotation(rotationAngle, rotationOrigin.x, rotationOrigin.y));
    }
}

Rectangle<double> LinearTiling::findSmallestBounds(int numColumns, int numRows, bool withTransformations)
{
    // TODO with transformations

    Rectangle<double> bounds(
        Point<double>(),
        getPointFromOrigin(1.0, 0.0, angleCos, angleSin, radiusHorizontal, radiusAngled, numColumns, numRows)
    );

    return bounds;
}

//==============================================================================

void HexagonalTiling::setRadius(double radius)
{
    outerRadius = radius;
    lateral = outerRadius * HEXRADIUSTOLATERAL;
    recalculateHexagonComponents();
}

void HexagonalTiling::setMargin(double margin)
{
    hexagonMargin = margin;
    recalculateHexagonComponents();
}

void HexagonalTiling::recalculateHexagonComponents()
{
    radiusAngled = (outerRadius + hexagonMargin) * HEXRADIUSTOLATERAL;
    radiusHorizontal = 2.0 * outerRadius * HEXRADIUSTOLATERAL + hexagonMargin;

    recalculateBasisComponents();
}

void HexagonalTiling::recalculateBasisComponents()
{
    angledXComponent = radiusAngled;
    angledYComponent = 1.5 * (outerRadius) + hexagonMargin * HEXRADIUSTOLATERAL;
}

Array<Point<float>> transformPointsFromOrigin(const Array<Point<int>>& pointsIn) 
{
    return Array<Point<float>>();
}

void HexagonalTiling::fitTilingTo(
    Rectangle<double>  boundsIn,
    int                numColumns,
    int                numRows,
    double             marginSize,
    double             rotateAngle,
    bool               scaleToFitRotation,
    double             horizontalScalarIn,
    double             diagonalScalarIn
)
{
    hexagonMargin = marginSize;
    rotationAngle = rotateAngle;

    setHorizontalBasisScalar(horizontalScalarIn);
    setAngledBasisScalar(diagonalScalarIn);

    givenBounds = boundsIn;
    outerRadius = findBestRadius(boundsIn, numColumns, numRows);

    lateral = outerRadius * HEXRADIUSTOLATERAL;

    lastCalculatedBounds = findSmallestBounds(numColumns, numRows, outerRadius, lateral, rotationAngle);

    Point<double> origin(givenBounds.getX() + lateral, givenBounds.getY() + outerRadius);
    setOrigin(origin);

    recalculateHexagonComponents();
    scaleRotatedTileToBounds(scaleToFitRotation, givenBounds /*, TODO: centre/Justification option */);
}

Rectangle<double> HexagonalTiling::findSmallestBounds(int numColumns, int numRows, bool withTransformations)
{
    double width = numColumns * (2.0 * lateral) + !(numRows % 2 ) * lateral;
    double height = (numRows - 1.0) * (1.5 * outerRadius) + 2.0 * outerRadius;
    Rectangle<double> tileBounds(0.0, 0.0, width, height);

    return (withTransformations) ? tileBounds.transformedBy(transform) : tileBounds;
}

Rectangle<double> HexagonalTiling::findSmallestBounds(int numColumns, int numRows, double radius, double rotationAngle)
{
    return findSmallestBounds(numColumns, numRows, radius, radius * HEXRADIUSTOLATERAL, rotationAngle);
}

Rectangle<double> HexagonalTiling::findSmallestBounds(int numColumns, int numRows, double radius, double lateral, double rotationAngle)
{
    Rectangle<double> bounds(0.0, 0.0,
        numColumns * 2.0 * lateral + !(numRows % 2) * lateral,
        (numRows - 1.0) * (1.5 * radius) + 2.0 * radius
    );

    // TODO figure out what's wrong with this
    //if (rotationAngle != 0.0)
    //    bounds = bounds.transformedBy(AffineTransform::rotation(rotationAngle, bounds.getCentreX(), bounds.getCentreY()));

    return bounds;
}

double HexagonalTiling::findBestRadius(Rectangle<double> boundsIn, int numColumns, int numRows)
{
    double widthBased = boundsIn.getWidth() / (2.0 * numColumns + 1.0) / HEXRADIUSTOLATERAL;
    double heightBased = 2.0 * boundsIn.getHeight() / (3.0 * numRows + 1.0);

    return juce::jmin(widthBased, heightBased);
}