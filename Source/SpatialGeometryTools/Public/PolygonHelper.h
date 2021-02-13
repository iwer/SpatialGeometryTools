// Fill out your copyright notice in the Description page of Project Settings.

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
     * Generates triangle indices for a set of vertices.
     */
    static TArray<int32> TesselatePolygon(const TArray<FVector2D> &vertices);

    /**
     * Assumes a polygon in X-Y direction and projects a UV map from Z direction
     * onto it with 1m raster size.
     */
    static TArray<FVector2D> FlatUVMap(const TArray<FVector2D> &vertices);

    /**
     * Generates a set of vertices that is offset from the original polygon vertices by an equal amount in all directions.
     * As oposed to a linear scale this produces an equal border around the original polygon.
     * This implementation is inspired by https://stackoverflow.com/questions/54033808/how-to-offset-polygon-edges
     * it assumes that incomming vertices are in clockwise order
     */
    static TArray<FVector> GenerateOffsetVertices(const TArray<FVector> &vertices, float offset, float heightdifference);
};
