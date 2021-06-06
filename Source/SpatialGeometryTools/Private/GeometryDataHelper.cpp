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

FGeometryData GeometryDataHelper::MakeFace(TArray<FVector> Vertices, bool bClockwise)
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
    data.Tangents.Init(FProcMeshTangent(1,0,0), Vertices.Num());
    return data;
}

void GeometryDataHelper::AppendQuad(FGeometryData &data, const FVector &P0, const FVector &P1, const FVector &P0a, const FVector &P1a)
{
    int32 StartIdx = data.Vertices.Num();

    // Append vertices
    data.Vertices.Add(P0);
    data.Vertices.Add(P1);
    data.Vertices.Add(P0a);
    data.Vertices.Add(P1a);

    // triangle p0, p1, P1a
    data.Indices.Append({StartIdx, StartIdx + 1, StartIdx + 3});
    // triangle p1a, p0a, p1
    data.Indices.Append({StartIdx + 3, StartIdx + 2, StartIdx});

    FVector FaceNormal = VectorHelper::MakeFaceNormal(P0, P0a, P1);
    FVector VTangent = (P1 - P0).GetSafeNormal(.0001);
    FVector Cotangent = FVector::CrossProduct(VTangent, FaceNormal);
    
    TArray<FVector> Normals;
    TArray<FLinearColor> Colors;
    TArray<FProcMeshTangent> Tangents;

    Normals.Init(FaceNormal, 4);
    data.Normals.Append(Normals);

    FProcMeshTangent Tangent(VTangent, false);
    Tangents.Init(Tangent, 4);
    data.Tangents.Append(Tangents);

    Colors.Init(FLinearColor(0, 0, 0, 1), 4);
    data.Colors.Append(Colors);

    FVector UP(0,0,-1);
    //auto Rotator = UP.Rotation() - FaceNormal.Rotation();
    auto Rotator = FMatrix(Cotangent, VTangent, FaceNormal, FVector::ZeroVector).Rotator();
    FVector uv0 = Rotator.UnrotateVector(P0) / 100;
    FVector uv1 = Rotator.UnrotateVector(P1) / 100;
    FVector uv0a = Rotator.UnrotateVector(P0a) / 100;
    FVector uv1a = Rotator.UnrotateVector(P1a) / 100;

    data.TexCoords.Add(FVector2D(uv0));
    data.TexCoords.Add(FVector2D(uv1));
    data.TexCoords.Add(FVector2D(uv0a));
    data.TexCoords.Add(FVector2D(uv1a));
}

void GeometryDataHelper::CreateStaticMeshAsset(const FGeometryData Geometry, FString ObjectName, FString AssetPath, UMaterialInterface * Material)
{
    UE_LOG(LogTemp, Warning, TEXT("GeometryDataHelper::CreateStaticMeshAsset: %d vertices, %d indices, %d normals, %d colors, %d tangents, %d texcoords"),
        Geometry.Vertices.Num(), Geometry.Indices.Num(), Geometry.Normals.Num(), Geometry.Colors.Num(), Geometry.Tangents.Num(), Geometry.TexCoords.Num())
    // Create Package
    FString PathPackage = FPaths::Combine(FString("/Game"), AssetPath);
    FString AbsolutePathPackage = FPaths::Combine(FPaths::ProjectContentDir(), AssetPath, FString("/"));

    UPackage * Package = CreatePackage(*PathPackage);
    FName StaticMeshName = MakeUniqueObjectName(Package, UStaticMesh::StaticClass(), FName(*ObjectName));
    UPackage * MeshPackage = CreatePackage(*FPaths::Combine(PathPackage, StaticMeshName.ToString()));

    // Create Static Mesh
    UStaticMesh * StaticMesh = NewObject<UStaticMesh>(MeshPackage, StaticMeshName, RF_Public | RF_Standalone);

    if(StaticMesh)
    {
        FRawMesh RawMesh = FRawMesh();
        FColor ColorBlack = FColor(0,0,0,255);

        // add vertices
        for(auto &v : Geometry.Vertices)
        {
            RawMesh.VertexPositions.Add(v);
        }
        // add faces, normals, ...
        for (int i = 0; i < Geometry.Indices.Num() - 2; i+=3)
        {
            RawMesh.WedgeIndices.Add(Geometry.Indices[i+0]);
            RawMesh.WedgeIndices.Add(Geometry.Indices[i+1]);
            RawMesh.WedgeIndices.Add(Geometry.Indices[i+2]);

            RawMesh.WedgeColors.Add(ColorBlack);
            RawMesh.WedgeColors.Add(ColorBlack);
            RawMesh.WedgeColors.Add(ColorBlack);

            RawMesh.WedgeTangentX.Add(FVector::ZeroVector);
            RawMesh.WedgeTangentX.Add(FVector::ZeroVector);
            RawMesh.WedgeTangentX.Add(FVector::ZeroVector);

            RawMesh.WedgeTangentY.Add(FVector::ZeroVector);
            RawMesh.WedgeTangentY.Add(FVector::ZeroVector);
            RawMesh.WedgeTangentY.Add(FVector::ZeroVector);

            RawMesh.WedgeTangentZ.Add(Geometry.Normals[Geometry.Indices[i+0]]);
            RawMesh.WedgeTangentZ.Add(Geometry.Normals[Geometry.Indices[i+1]]);
            RawMesh.WedgeTangentZ.Add(Geometry.Normals[Geometry.Indices[i+2]]);

            RawMesh.FaceMaterialIndices.Add(0);
            RawMesh.FaceSmoothingMasks.Add(0xFFFFFFFF); // Phong

            for(int UVIndex = 0; UVIndex < MAX_MESH_TEXTURE_COORDS; UVIndex++)
            {
                RawMesh.WedgeTexCoords[UVIndex].Add(Geometry.TexCoords[Geometry.Indices[i+0]]);
                RawMesh.WedgeTexCoords[UVIndex].Add(Geometry.TexCoords[Geometry.Indices[i+1]]);
                RawMesh.WedgeTexCoords[UVIndex].Add(Geometry.TexCoords[Geometry.Indices[i+2]]);
            }
        }

        // Save RawMesh in StaticMesh
        StaticMesh->AddSourceModel();
        StaticMesh->GetSourceModel(0).RawMeshBulkData->SaveRawMesh(RawMesh);

        // Configuration
        StaticMesh->GetSourceModel(0).BuildSettings.bRecomputeNormals = true;
        StaticMesh->GetSourceModel(0).BuildSettings.bRecomputeTangents = true;
        StaticMesh->GetSourceModel(0).BuildSettings.bUseMikkTSpace = false;
        StaticMesh->GetSourceModel(0).BuildSettings.bGenerateLightmapUVs = true;
        StaticMesh->GetSourceModel(0).BuildSettings.bBuildAdjacencyBuffer = false;
        StaticMesh->GetSourceModel(0).BuildSettings.bBuildReversedIndexBuffer = false;
        StaticMesh->GetSourceModel(0).BuildSettings.bUseFullPrecisionUVs = false;
        StaticMesh->GetSourceModel(0).BuildSettings.bUseHighPrecisionTangentBasis = false;

        // Assign Material
        StaticMesh->StaticMaterials.Add(Material);
        StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
        StaticMesh->CreateBodySetup();
        StaticMesh->SetLightingGuid();
        FAssetRegistryModule::AssetCreated(StaticMesh);
        
        StaticMesh->PostEditChange();
        if(!MeshPackage->MarkPackageDirty())
        {
            UE_LOG(LogTemp, Warning, TEXT("GeometryDataHelper: Editor suppressed marking package  %s dirty!"), *Package->GetName())
        } else
        {
            UE_LOG( LogTemp, Log, TEXT("GeometryDataHelper: Created static mesh %s"), *ObjectName);
        }
        
    }
    
}
