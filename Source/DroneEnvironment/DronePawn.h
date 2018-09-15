// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"

#include <string>

#include "DronePawn.generated.h"


class UCameraComponent;
class USceneCaptureComponent2D;
class UMaterialInterface;

/**
 * 
 */
UCLASS()
class DRONEENVIRONMENT_API ADronePawn : public ADefaultPawn
{
	GENERATED_UCLASS_BODY()

	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	virtual void OnConstruction(const FTransform &Transform) override;

public:
	/** Name of the CameraComponent. */
	static FName CameraComponentName;
	/** Name of the SceneCapture2DComponent. */
	static FName SceneCapture2DComponentName;

private:
	/** DronePawn CameraComponent */
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* CameraComponent;
	/** DronePawn SceneCapture2dComponent */
	UPROPERTY(Category = Pawn, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USceneCaptureComponent2D* SceneCaptureComponent2D;

private:
	void SaveCaptureDepth(std::string saveDirectoryStr);
	void SaveCaptureRGB(std::string saveDirectoryStr);
	
public:
	virtual void Tick(float DeltaTime) override;

	void SaveCapture();

	/** Returns CameraComponent subobject **/
	UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns SceneCapture2DComponent subobject **/
	USceneCaptureComponent2D* GetSceneCapture2DComponent() const { return SceneCaptureComponent2D; }
};
