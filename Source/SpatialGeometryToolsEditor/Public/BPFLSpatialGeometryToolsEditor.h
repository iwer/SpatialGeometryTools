// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeometryData.h"

#include "BPFLSpatialGeometryToolsEditor.generated.h"

/**
 * 
 */
UCLASS()
class SPATIALGEOMETRYTOOLSEDITOR_API UBPFLSpatialGeometryToolsEditor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	/**
	* Creates a StaticMesh Asset from Geometry named ObjectName, placed at AssetPath in /Game with a default Material.
	*/
	UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Mesh")
	static UStaticMesh * SaveStaticMesh(UPARAM(ref)  FGeometryData &Geometry, FString ObjectName, FString AssetPath, UMaterialInterface * Material);
};
