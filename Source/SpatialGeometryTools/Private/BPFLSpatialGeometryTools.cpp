// Copyright (c) Iwer Petersen. All rights reserved.


#include "BPFLSpatialGeometryTools.h"
#include "VectorHelper.h"
#include "PolygonHelper.h"

FGeometryData UBPFLSpatialGeometryTools::MakeFace(TArray<FVector> Vertices, bool bClockwise)
{
    FGeometryData data;
    data.Vertices = Vertices;

    // if we have less than 3 vertices or all vertices are in line
    // we cant make a face
    if(Vertices.Num() < 3
        || VectorHelper::IsLine(Vertices)) {
        return data;
    }

    data.Indices = PolygonHelper::TesselatePolygon(Vertices, bClockwise);
    data.Normals.Init(VectorHelper::MakeFaceNormal(Vertices[0], Vertices[1], Vertices[2]), Vertices.Num());

    data.TexCoords = PolygonHelper::FlatUVMapTilted(Vertices);
    data.Colors.Init(FLinearColor(0,0,0,1), Vertices.Num());
    data.Tangents.Init(FVector(1,0,0), Vertices.Num());
    return data;
}

void UBPFLSpatialGeometryTools::SortVerticesByAngle(TArray<FVector>& Vertices, bool bClockwise)
{
    PolygonHelper::AngularSortVertices(Vertices,bClockwise);
}

