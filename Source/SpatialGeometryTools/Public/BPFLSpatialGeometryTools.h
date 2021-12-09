// Copyright (c) Iwer Petersen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeometryData.h"

#include "BPFLSpatialGeometryTools.generated.h"

/**
 *
 */
UCLASS()
class SPATIALGEOMETRYTOOLS_API UBPFLSpatialGeometryTools : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /**
     * Returns center vector of Vertices
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Vector")
    static FVector GetCenterOfMass(TArray<FVector> Vertices);

    /**
     * Returns polygon normal. Returns zero vector when polygon is not flat.
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Vector")
    static FVector GetPolygonNormal(TArray<FVector> Vertices);

    /**
     * Tesselate polygon using earcut algorithm.
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static FGeometryData MakeFace(TArray<FVector> Vertices, bool bClockwise);

    /**
     * Tesselate polygon with a second polygon as hole using earcut algorithm.
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static FGeometryData MakeFaceWithHole(TArray<FVector> Vertices, TArray<FVector> HoleVertices, bool bClockwise);

    /**
     * Generates geometry for a skirt around given polygon with height=Distance. Think Blender extrude face but the
     * face stays in place.
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static FGeometryData ExtrudeFaceAlongNormal(TArray<FVector> Vertices, const float Distance);

    /**
     * Determines winding order of vertices by evaluating the sign of the area of the polygon using the shoelace formula.
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static bool IsClockwise(TArray<FVector> Polygon);

    /**
     * Checks if polygon is convex by checking if all consecutive edges turn to the same direction. Technically this
     * incorrectly returns true for self intersecting polygons f.e. star polygons!
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static bool IsConvex(TArray<FVector> Polygon);

    /**
     * Checks if polygon is flat by determining plane equation from three vertices and check if all vertices fit
     * this equation.
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static bool IsFlat(TArray<FVector> Polygon);

    /**
     * Sorts vertices by their angle around the common center of mass.
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static void SortVerticesByAngle(UPARAM(ref) TArray<FVector> &Vertices, const bool bClockwise);

    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|GeometryData")
    static bool IsValid(UPARAM(ref) FGeometryData &GeometryData);


    /**
     * Appends GeometryData Appender to Base. Joins Vertices, Normal, Tangents, Colors and TexCoords and shifts indices
     * of Appender to match new vertex indices in Base.
     */
    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Mesh")
    static void ConcatenateGeometryData(UPARAM(ref) FGeometryData &Base, UPARAM(ref) FGeometryData &Appender);

    /**
     * Creates a StaticMesh Asset from Geometry named ObjectName, placed at AssetPath in /Game with a default Material.
     */
    //UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Mesh")
    //static UStaticMesh * SaveStaticMesh(UPARAM(ref)  FGeometryData &Geometry, FString ObjectName, FString AssetPath, UMaterialInterface * Material);

};
