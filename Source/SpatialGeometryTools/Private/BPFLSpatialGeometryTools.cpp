// Copyright (c) Iwer Petersen. All rights reserved.


#include "BPFLSpatialGeometryTools.h"

#include "VectorHelper.h"
#include "PolygonHelper.h"
#include "GeometryDataHelper.h"

FVector UBPFLSpatialGeometryTools::GetCenterOfMass(TArray<FVector> Vertices)
{
    return VectorHelper::CenterOfMass(Vertices);
}

FGeometryData UBPFLSpatialGeometryTools::MakeFace(TArray<FVector> Vertices, bool bClockwise)
{
    return GeometryDataHelper::MakeFace(Vertices, bClockwise);
}

FGeometryData UBPFLSpatialGeometryTools::ExtrudeFaceAlongNormal(TArray<FVector> Vertices, const float Distance)
{
    FGeometryData data;

    auto OffsetVertices = PolygonHelper::GenerateOffsetVertices(Vertices, 0, Distance);

    // for each Vertices Add Quad
    for(int32 i = 0; i < Vertices.Num(); ++i) {
        auto P0 = Vertices[i];
        auto P1 = Vertices[(i + 1) % Vertices.Num()];
        auto P0a = OffsetVertices[i];
        auto P1a = OffsetVertices[(i + 1) % Vertices.Num()];
        GeometryDataHelper::AppendQuad(data, P0, P1, P0a, P1a);
    }
    
    return data;
}

bool UBPFLSpatialGeometryTools::IsClockwise(TArray<FVector> Polygon)
{
    return PolygonHelper::IsClockwise(Polygon);
}

bool UBPFLSpatialGeometryTools::IsConvex(TArray<FVector> Polygon)
{
    return PolygonHelper::IsConvex(Polygon);
}

void UBPFLSpatialGeometryTools::SortVerticesByAngle(TArray<FVector>& Vertices, const bool bClockwise)
{
    PolygonHelper::AngularSortVertices(Vertices,bClockwise);
}

void UBPFLSpatialGeometryTools::ConcatenateGeometryData(FGeometryData& Base, FGeometryData& Appender)
{
    GeometryDataHelper::AppendGeometryData(Base, Appender);
}

UStaticMesh *  UBPFLSpatialGeometryTools::SaveStaticMesh(FGeometryData& Geometry, FString ObjectName, FString AssetPath,
                                               UMaterialInterface* Material)
{
    return GeometryDataHelper::CreateStaticMeshAsset(Geometry, ObjectName, AssetPath, Material);
}
