/*
  ==============================================================================

    HexagonTilingGeometry.h
    Created: 6 Jan 2021 11:42:30pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include "BoardGeometry.h"
#include "KeyboardDataStructure.h"

//==============================================================================
// Used to map a rectangular coordinate system to some other linear coodinate system
// One basis vector is fixed horizontal while the other uses a specifed angle.

#define HEXRADIUSTOLATERAL 0.8660254037844 // sqrt(3) / 2, proportional to radius, the length between the center and a vertex)
#define HEXBASESANGLE      1.0471975511966

class LinearTiling
{
public:

    // Square grid by default
    LinearTiling() { recalculateBasisComponents(); }

    LinearTiling(
        double          basesAngle, 
        double          horizontalRadius, 
        double          verticalRadius, 
        Point<double>   originIn = Point<double>(), 
        double          tileRotationAngle = 0.0, 
        double          horizontalScalar = 1.0f, 
        double          verticalScalar = 1.0f
    ) : 
        radiusHorizontal(horizontalRadius),
        origin(originIn),
        horizontalBasisScalar(horizontalScalar),
        angledBasisScalar(verticalScalar)
    {
        radiusAngled = verticalRadius;
        setBasesAngle(basesAngle);
        setTileRotationAngle(tileRotationAngle);
    };

    //=======================================================================
    // Setters

    void setBasesAngle(double thetaBetweenBasisVectors);

    void setHorizontalBasisRadius(double radius);

    void setAngledBasisRadius(double radius);

    void setTileRotationAngle(double angleIn);

    void setOrigin(Point<double> originPointIn);

    void scaleRotatedTileToBounds(bool doScaling, Rectangle<double> boundsIn = {}, bool centreInBounds = true);

    void setHorizontalBasisScalar(double scalarIn) { horizontalBasisScalar = scalarIn; }

    void setAngledBasisScalar(double scalarIn) { angledBasisScalar = scalarIn; }

    //=======================================================================
    // Getters

    double getBasesAngle() const { return theta; }

    double getHorizontalBasisRadius() const { return radiusHorizontal; }

    double getAngledBasisRadius() const { return radiusAngled; }

    double getTileRotationAngle() const { return rotationAngle; }

    Point<double> getOrigin() const { return origin; }

    /// <summary>
    /// Returns the bounds passed in certain 'fitTilingTo' methods
    /// </summary>
    /// <returns></returns>
    Rectangle<double> getGivenBounds() const { return givenBounds; }

    bool isScalingToBounds() const { return scaleToFitBounds; }

    float getHorizontalBasisScalar() const { return horizontalBasisScalar; }

    float getAngledBasisScalar() const { return angledBasisScalar; }

    //=======================================================================
    // Calculations

    virtual Array<Point<float>> transformPointsFromOrigin(const Array<Point<int>>& pointsIn, bool centreInBounds = false) const;

    virtual Array<Point<float>> getPointsBetween(Point<int> horizontalRange, Point<int> verticalRange) const;

    /// <summary>
    /// Calculate properties of the hexagon tiling given certain parameters
    /// </summary>
    /// <param name="originPoint">The point on the tiling that represents (0,0) </param>
    /// <param name="firstBasisPoint">A point on the tiling that represents a linearly independent vector in the direction spanning columns</param>
    /// <param name="secondBasisPoint">A point on the tiling that represents a the sum of the first vector and a linearly independent vector in the direction spanning rows</param>
    /// <param name="firstBasisIsColumn">Use rowBasisPoint as origin for the columnBasisPoiont</param>
    /// <param name="columnBasisOffset">The number of units the column basis point is away from the origin.</param>
    /// <param name="rowBasisOffset">The number of units the row basis point is away from the origin</param>
    void fitTilingTo(
        Point<float>        originPoint,
        Point<float>        firstBasisPoint,
        Point<float>        secondBasisPoint,
        bool                firstBasisIsColumn = true,
        int                 columnBasisOffset = 1,
        int                 rowBasisOffset = 1
    );

public:

    static Point<double> getPointFromOrigin(
        double columnAngleX, double columnAngleY, double rowAngleX, double rowAngleY,
        double columnUnit, double rowUnit, double columnOffset, double rowOffset
    );

protected:

    virtual AffineTransform getTileMatrix() const;

    virtual void recalculateBasisComponents();

    virtual void recalculateTileTransform(Point<double> originToUse);

    virtual Rectangle<double> findSmallestBounds(int numColumns, int numRows, bool withTransformations = true);

    //=======================================================================
    // Properties based on parameters

    double rotationScalar = 1.0f;
    AffineTransform transform = AffineTransform();

    double angleCos = 0;
    double angleSin = 0;
    
    double angledXComponent = 0;
    double angledYComponent = 0;

    Rectangle<double> lastCalculatedBounds;
    AffineTransform tileTransform;

    //=======================================================================
    // Parameters

    double radiusHorizontal = 1.0;
    double radiusAngled = 1.0;
    double rotationAngle = 0.0;
    Rectangle<double> givenBounds;

private:

    double theta = float_Pi * 0.5;
    Point<double> origin;

    bool scaleToFitBounds = false;

    double horizontalBasisScalar = 1.0;
    double angledBasisScalar = 1.0;
};

//==============================================================================
// Used to calculate the center point coordinates for a hexagon tiling.
// This class treats a standard hexagon with a vertex aligned with the y-axis.

class HexagonalTiling : public LinearTiling
{
public:

    HexagonalTiling() { recalculateHexagonComponents(); };
    
    HexagonalTiling(Point<double> origin,
        double radius, 
        double margin           = 0.0, 
        double tileRotation     = 0,
        double horizontalScalar = 1.0,
        double verticalScalar   = 1.0
    ) : hexagonMargin(margin)
    {
        setRadius(radius);
        setTileRotationAngle(tileRotation);
        setHorizontalBasisScalar(horizontalScalar);
        setAngledBasisScalar(verticalScalar);
    }

    void setRadius(double radius);

    void setMargin(double margin);

    double getRadius(bool scaled = true) const { return (scaled) ? rotationScalar * outerRadius : outerRadius; }

    double getMargin(bool scaled = true) const { return (scaled) ? rotationScalar * hexagonMargin : hexagonMargin; }

    void setBasesAngle(double) = delete;

    void setHorizontalBasisRadius(double) = delete;

    void setAngledBasisRadius(double) = delete;

    //==============================================================================
    // Helper functions to fit a tile to certain specifications

    /// <summary>
    /// Calculate properties of the hexagon tiling given bounds, margin size, and rotation
    /// </summary>
    /// <param name="boundsIn"></param>
    /// <param name="numColumns"></param>
    /// <param name="numRows"></param>
    /// <param name="marginSize"></param>
    /// <param name="rotateAngle"></param>
    /// <param name="scaleToFitRotation"></param>
    /// <param name="radiusScalarIn"></param>
    /// <param name="lateralScalarIn"></param>
    void fitTilingTo(
        Rectangle<double> boundsIn, 
        int               numColumns,
        int               numRows,
        double            marginSize = 0.0f, 
        double            rotateAngle = 0.0f, 
        bool              scaleToFitRotation = true,
        double            horizontalScalarIn = 1.0f,
        double            diagonalScalarIn = 1.0f
    );

    Rectangle<double> findSmallestBounds(int numColumns, int numRows, bool withTransformations = true) override;

    static Rectangle<double> findSmallestBounds(int numColumns, int numRows, double radius, double rotationAngle);
    static Rectangle<double> findSmallestBounds(int numColumns, int numRows, double radius, double lateral, double rotationAngle);

public:

    static double findBestRadius(Rectangle<double> boundsIn, int numColumns, int numRows);

private:

    void recalculateHexagonComponents();

    void recalculateBasisComponents() override;

private:

    double outerRadius = 1.0;
    double lateral = HEXRADIUSTOLATERAL;
    double hexagonMargin = 0.0;
};