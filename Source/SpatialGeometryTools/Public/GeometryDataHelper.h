// Copyright (c) Iwer Petersen. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeometryData.h"

/**
 *
 */
class SPATIALGEOMETRYTOOLS_API GeometryDataHelper
{
public:
    GeometryDataHelper();
    ~GeometryDataHelper();
	/**
	 * Make a face of a set of vertices. Uses earcut triangulation. Assumes vertices in X-Y plane.
	 * Winds individual triangles clockwise in bClockwise is true.
	 */
	static FGeometryData MakeFace(TArray<FVector> Vertices, bool bClockwise);
	/**
	 * Appends a quad consisting of P0---P1 and
	 *                              P0a--P1a
	 * to an existing FgeometryData structure.
	 */
    static void AppendQuad(FGeometryData &data, const FVector &P0, const FVector &P1, const FVector &P0a, const FVector &P1a);
	static void AppendGeometryData(FGeometryData &Root, FGeometryData &Appendage);
	/**
	 * Creates a static mesh asset named ObjectName at Contentpath AssetPath with default material
	 */
	static UStaticMesh * CreateStaticMeshAsset(const FGeometryData Geometry, FString ObjectName, FString AssetPath, UMaterialInterface * Material);
};
