// Copyright (c) Iwer Petersen. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class SPATIALGEOMETRYTOOLS_API PolygonHelper
{
public:
    PolygonHelper();
    ~PolygonHelper();

    /**
     * Calculates the area surrounded by this polygon.
     */
    static float PolygonArea(const TArray<FVector> &Polygon);

    /**
     * Sort Vertices by their angular order around the center of mass.
     * Returns true if order is definite (no ambiguous angles).
     */
    static bool AngularSortVertices(TArray<FVector> &Vertices, bool bClockwise = true);

    static bool IsFlat(const TArray<FVector> &Polygon);
    static bool IsConvex(const TArray<FVector> &Polygon);
    /**
     * Determines if vertices are in clockwise order
     */
    static bool IsClockwise(const TArray<FVector> &Polygon);

    /**
     * Generates triangle indices for a set of vertices. Assumes vertices are in X-Y plane.
     * TODO: This may also work in 3D,  check earcut docs.
     */
    static TArray<int32> TesselatePolygon(const TArray<FVector> &Vertices, const TArray<FVector> &HoleVertices, bool bClockwise);

    /**
     * Assumes a polygon in X-Y plane and projects a UV map from Z direction
     * onto it with 1m raster size.
     */
    static TArray<FVector2D> FlatUVMap(const TArray<FVector> &Vertices);

    /**
     * Does the same as FlatUVMap but rotates polygon into x-y plane and therefore does not require vertices to be
     * in x-y plane beforehand
     */
    static TArray<FVector2D> FlatUVMapTilted(const TArray<FVector> &Vertices);

    /**
     * Generates a set of vertices that is offset from the original polygon vertices by an equal amount in all directions.
     * As oposed to a linear scale this produces an equal border around the original polygon.
     * This implementation is inspired by https://stackoverflow.com/questions/54033808/how-to-offset-polygon-edges
     * it assumes that incomming vertices are in clockwise order for inward facing facets and anticlockwise for outward facing facets.
     */
    static TArray<FVector> GenerateOffsetVertices(const TArray<FVector> &Vertices, float Offset, float HeightDifference);
};
