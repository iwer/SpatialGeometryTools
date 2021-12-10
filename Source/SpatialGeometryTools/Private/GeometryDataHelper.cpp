// Copyright (c) Iwer Petersen. All rights reserved.


#include "GeometryDataHelper.h"
#include "PolygonHelper.h"
#include "RawMesh.h"
#include "VectorHelper.h"
#include "AssetRegistry/AssetRegistryModule.h"


GeometryDataHelper::GeometryDataHelper()
{
}

GeometryDataHelper::~GeometryDataHelper()
{
}

FGeometryData GeometryDataHelper::MakeFace(TArray<FVector> Vertices, const bool bClockwise)
{
    FGeometryData data;
    data.Vertices = Vertices;

    // if we have less than 3 vertices or all vertices are in line
    // we cant make a face
    if(Vertices.Num() < 3
        || VectorHelper::IsLine(Vertices)) {
        return data;
    }

    const FVector Normal = VectorHelper::MakeFaceNormal(Vertices[0], Vertices[1], Vertices[2]);

    // rotate vertices into x-y plane using normal
    const FRotator Rotator = FRotationMatrix::MakeFromZ(Normal).Rotator();
    TArray<FVector> RotVerts;
    for(auto &V : data.Vertices)
    {
        RotVerts.Add(Rotator.UnrotateVector(V));
    }

    data.Indices = PolygonHelper::TesselatePolygon(RotVerts, TArray<FVector>(), bClockwise);
    data.Normals.Init(Normal, Vertices.Num());

    data.TexCoords = PolygonHelper::FlatUVMapTilted(RotVerts);
    data.Colors.Init(FLinearColor(0,0,0,1), Vertices.Num());
    data.Tangents.Init(FProcMeshTangent(1,0,0), Vertices.Num());
    return data;
}

FGeometryData GeometryDataHelper::MakeFace(TArray<FVector> Vertices, TArray<FVector> HoleVertices, const bool bClockwise)
{
    FGeometryData Data;
    Data.Vertices = Vertices;
    Data.Vertices.Append(HoleVertices);
    // if we have less than 3 vertices or all vertices are in line
    // we cant make a face
    if(Vertices.Num() < 3
        || VectorHelper::IsLine(Vertices)) {
        return Data;
    }

    const FVector Normal = VectorHelper::MakeFaceNormal(Vertices[0], Vertices[1], Vertices[2]);

    // rotate vertices into x-y plane using normal
    const FRotator Rotator = FRotationMatrix::MakeFromZ(Normal).Rotator();
    TArray<FVector> RotVerts;
    TArray<FVector> RotHoleVerts;
    for(auto &V : Vertices)
    {
        RotVerts.Add(Rotator.UnrotateVector(V));
    }
    for(auto &V : HoleVertices)
    {
        RotHoleVerts.Add(Rotator.UnrotateVector(V));
    }


    Data.Indices = PolygonHelper::TesselatePolygon(RotVerts, RotHoleVerts, bClockwise);
    Data.Normals.Init(Normal, Vertices.Num() + HoleVertices.Num());

    Data.TexCoords = PolygonHelper::FlatUVMapTilted(RotVerts);
    Data.TexCoords.Append(PolygonHelper::FlatUVMapTilted(RotHoleVerts));
    Data.Colors.Init(FLinearColor(0,0,0,1), Vertices.Num() + HoleVertices.Num());
    Data.Tangents.Init(FProcMeshTangent(1,0,0), Vertices.Num() + HoleVertices.Num());
    return Data;
}

void GeometryDataHelper::AppendQuad(FGeometryData &Data, const FVector &P0, const FVector &P1, const FVector &P0A, const FVector &P1A)
{
    int32 StartIdx = Data.Vertices.Num();

    // Append vertices
    Data.Vertices.Add(P0);
    Data.Vertices.Add(P1);
    Data.Vertices.Add(P0A);
    Data.Vertices.Add(P1A);

    // triangle p0, p1, P1a
    Data.Indices.Append({StartIdx, StartIdx + 1, StartIdx + 3});
    // triangle p1a, p0a, p1
    Data.Indices.Append({StartIdx + 3, StartIdx + 2, StartIdx});

    const FVector FaceNormal = VectorHelper::MakeFaceNormal(P0, P0A, P1);
    const FVector VTangent = (P1 - P0).GetSafeNormal(.0001);
    const FVector Cotangent = FVector::CrossProduct(VTangent, FaceNormal);

    TArray<FVector> Normals;
    TArray<FLinearColor> Colors;
    TArray<FProcMeshTangent> Tangents;

    Normals.Init(FaceNormal, 4);
    Data.Normals.Append(Normals);

    const FProcMeshTangent Tangent(VTangent, false);
    Tangents.Init(Tangent, 4);
    Data.Tangents.Append(Tangents);

    Colors.Init(FLinearColor(0, 0, 0, 1), 4);
    Data.Colors.Append(Colors);

    FVector UP(0,0,-1);
    //auto Rotator = UP.Rotation() - FaceNormal.Rotation();
    const auto Rotator = FMatrix(Cotangent, VTangent, FaceNormal, FVector::ZeroVector).Rotator();
    const FVector UV0 = Rotator.UnrotateVector(P0) / 100;
    const FVector UV1 = Rotator.UnrotateVector(P1) / 100;
    const FVector UV0A = Rotator.UnrotateVector(P0A) / 100;
    const FVector UV1A = Rotator.UnrotateVector(P1A) / 100;

    Data.TexCoords.Add(FVector2D(UV0));
    Data.TexCoords.Add(FVector2D(UV1));
    Data.TexCoords.Add(FVector2D(UV0A));
    Data.TexCoords.Add(FVector2D(UV1A));
}

void GeometryDataHelper::AppendGeometryData(FGeometryData& Root, FGeometryData& Appendage)
{
    const int RootVertices = Root.Vertices.Num();

    Root.Vertices.Append(Appendage.Vertices);
    Root.Colors.Append(Appendage.Colors);
    Root.Normals.Append(Appendage.Normals);
    Root.Tangents.Append(Appendage.Tangents);
    Root.TexCoords.Append(Appendage.TexCoords);

    for(const auto &Idx : Appendage.Indices)
    {
        Root.Indices.Add(RootVertices + Idx);
    }
}

bool GeometryDataHelper::IsValid(const FGeometryData& Geometry)
{
    // indices must be multiple of 3
    if(Geometry.Indices.Num() % 3 != 0)
        return false;
    // Same N for vertices, Colors, Normals, Tangents and TexCoords
    int N = Geometry.Vertices.Num();
    if(Geometry.Colors.Num() != N){
        UE_LOG(LogTemp,Warning,TEXT("FGeometryData has invalid number of colors: %d (should be %d)"), Geometry.Colors.Num(), N)
        return false;
    }
    if(Geometry.Normals.Num() != N){
        UE_LOG(LogTemp,Warning,TEXT("FGeometryData has invalid number of normals: %d (should be %d)"), Geometry.Normals.Num(), N)
        return false;
    }
    if(Geometry.Tangents.Num() != N){
        UE_LOG(LogTemp,Warning,TEXT("FGeometryData has invalid number of tangents: %d (should be %d)"), Geometry.Tangents.Num(), N)
        return false;
    }
    if(Geometry.TexCoords.Num() != N){
        UE_LOG(LogTemp,Warning,TEXT("FGeometryData has invalid number of texcoords: %d (should be %d)"), Geometry.TexCoords.Num(), N)
        return false;
    }

    // each index must be in range of N
    for(const auto Idx : Geometry.Indices)
    {
        if(Idx >= N){
            UE_LOG(LogTemp,Warning,TEXT("FGeometryData has invalid index: %d (should inside [0;%d])"), Idx, N)
            return false;
        }
    }

    // this is fine
    return true;
}
