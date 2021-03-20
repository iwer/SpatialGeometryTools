// Copyright (c) Iwer Petersen. All rights reserved.


#include "BPFLSpatialGeometryTools.h"
#include "VectorHelper.h"
#include "PolygonHelper.h"

FGeometryData UBPFLSpatialGeometryTools::MakeFace(TArray<FVector> vertices)
{
    FGeometryData data;
    data.Vertices = vertices;

    // if we have less than 3 vertices or all vertices are in line
    // we cant make a face
    if(vertices.Num() < 3
        || VectorHelper::IsLine(vertices)) {
        return data;
    }

    data.Indices = PolygonHelper::TesselatePolygon(vertices);
    data.Normals.Init(VectorHelper::MakeFaceNormal(vertices[0], vertices[1], vertices[2]), vertices.Num());

    data.TexCoords = PolygonHelper::FlatUVMapTilted(vertices);
    data.Colors.Init(FLinearColor(0,0,0,1), vertices.Num());
    data.Tangents.Init(FVector(1,0,0), vertices.Num());
    return data;
}

