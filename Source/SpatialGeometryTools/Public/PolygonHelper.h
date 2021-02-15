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
     * Generates triangle indices for a set of vertices. Assumes vertices are in X-Y plane.
     * TODO: This my also work in 3D,  check earcut docs.
     */
    static TArray<int32> TesselatePolygon(const TArray<FVector> &vertices);

    /**
     * Assumes a polygon in X-Y plane and projects a UV map from Z direction
     * onto it with 1m raster size.
     */
    static TArray<FVector2D> FlatUVMap(const TArray<FVector> &vertices);

    /**
     * Does the same as FlatUVMap but rotates polygon into x-y plane and therefore does not require vertices to be
     * in x-y plane beforehand
     */
    static TArray<FVector2D> FlatUVMapTilted(const TArray<FVector> &vertices);

    /**
     * Generates a set of vertices that is offset from the original polygon vertices by an equal amount in all directions.
     * As oposed to a linear scale this produces an equal border around the original polygon.
     * This implementation is inspired by https://stackoverflow.com/questions/54033808/how-to-offset-polygon-edges
     * it assumes that incomming vertices are in clockwise order
     */
    static TArray<FVector> GenerateOffsetVertices(const TArray<FVector> &vertices, float offset, float heightdifference);
};
