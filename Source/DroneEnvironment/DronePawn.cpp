// Fill out your copyright notice in the Description page of Project Settings.

#include "DronePawn.h"

#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"

#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h"

#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include <iostream>
#include <fstream>
#include <bitset>

FName ADronePawn::CameraComponentName(TEXT("CameraComponent0"));
FName ADronePawn::SceneCapture2DComponentName(TEXT("SceneCapture2DComponent0"));

ADronePawn::ADronePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(ADronePawn::CameraComponentName);
	CameraComponent->AttachToComponent(GetMeshComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SceneCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(ADronePawn::SceneCapture2DComponentName);
	SceneCaptureComponent2D->AttachToComponent(GetMeshComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

void ADronePawn::OnConstruction(const FTransform &Transform)
{
}

void ADronePawn::Tick(float DeltaTime) { 
	Super::Tick(DeltaTime);

	SceneCaptureComponent2D->SetWorldRotation(CameraComponent->GetComponentRotation());
}

void ADronePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ADefaultPawn::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("SaveCapture", EInputEvent::IE_Pressed, this, &ADronePawn::SaveCapture);
}

void ADronePawn::SaveCapture()
{
	FString SaveDirectory = FPaths::ProjectContentDir() + FString("Frames");

	std::string saveDirectoryStr = std::string(TCHAR_TO_UTF8(*SaveDirectory));

	SceneCaptureComponent2D->CaptureScene();
	SaveCaptureDepth(saveDirectoryStr);

	//SceneCaptureComponent2D->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	//SceneCaptureComponent2D->TextureTarget->CompressionSettings = TextureCompressionSettings::TC_HDR;
	//SceneCaptureComponent2D->CaptureScene();
	//SaveCaptureRGB(saveDirectoryStr);
}

void ADronePawn::SaveCaptureDepth(std::string saveDirectoryStr)
{
	//READ FIRST
	std::ifstream ifs;
	ifs.open(saveDirectoryStr + "/0000.pgm", std::ios::binary);

	int i = 0;

	while (!ifs.eof()) {
		
		char c = ifs.get();
		FString str = FString(&c);
		UE_LOG(LogTemp, Warning, TEXT("%d"), i);
		i++;
	}

	ifs.close();


	TArray<FLinearColor> SurfDepthData;
	FTextureRenderTargetResource* Resource = SceneCaptureComponent2D->TextureTarget->GameThread_GetRenderTargetResource();
	SurfDepthData.Reset();

	if (Resource->ReadLinearColorPixels(SurfDepthData) == true) {

		std::ofstream file;
		file.open(saveDirectoryStr + "/Capture.pgm", std::ios::binary);
		std::string textToSave = "P2\n1024 1024\n65504\n";

		char *p = new char[textToSave.size()];

		std::memcpy(p, textToSave.c_str(), textToSave.size());

		file << p;

		UE_LOG(LogTemp, Warning, TEXT("%s"), &SurfDepthData == nullptr ? TEXT("True") : TEXT("False"));
		UE_LOG(LogTemp, Warning, TEXT("%d"), SurfDepthData.Num() / 1024);
		
		int j = 0;
		for (int i = 0; i < SurfDepthData.Num(); i++)
		{
			bool nl = false;

			if (i % 10 == 0)
			{
				j++;
				file << "\n";
				nl = true;
			}

			unsigned short x = 65504;
			int size = sizeof(x);
			const char* asdf = reinterpret_cast<const char *>(&x);
			file.write(reinterpret_cast<const char *>(&x), sizeof(x));

			//char buff[6];
			//snprintf(buff, sizeof(buff), "%05d", (int)SurfDepthData[i].A);
			//textToSave += " " + std::string(buff) + " ";
		}
		UE_LOG(LogTemp, Warning, TEXT("lines: %d"), SurfDepthData.Num());


		file.close();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString((saveDirectoryStr + "/Capture.pgm").c_str()));
	}

	TArray<FColor> SurfColorData;
	Resource = SceneCaptureComponent2D->TextureTarget->GameThread_GetRenderTargetResource();

	SurfColorData.Reset();

	if (Resource->ReadPixels(SurfColorData) == true) {
		std::ofstream file;
		file.open(saveDirectoryStr + "/Capture.ppm", std::ios::binary);
		std::string textToSave = "P3\n1024 1024\n256";

		UE_LOG(LogTemp, Warning, TEXT("%s"), &SurfColorData == nullptr ? TEXT("True") : TEXT("False"));
		UE_LOG(LogTemp, Warning, TEXT("%d"), SurfColorData.Num() / 1024);

		int j = 0;
		for (int i = 0; i < SurfColorData.Num(); i++)
		{
			bool nl = false;

			if (i % 4 == 0)
			{
				j++;
				textToSave += "\n";
				nl = true;
			}

			char buff[6];
			snprintf(buff, sizeof(buff), "%05d  %05d  %05d", (int)SurfColorData[i].R, (int)SurfColorData[i].G, (int)SurfColorData[i].B);
			textToSave += " " + std::string(buff) + " ";
		}
		UE_LOG(LogTemp, Warning, TEXT("lines: %d"), (int)j);

		char *p = new char[textToSave.size()];

		std::memcpy(p, textToSave.c_str(), textToSave.size());

		file << p;
		file.close();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString((saveDirectoryStr + "/Capture.ppm").c_str()));
	}
}

void ADronePawn::SaveCaptureRGB(std::string saveDirectoryStr)
{

	std::ofstream file;
	file.open(saveDirectoryStr + "/Capture.ppm", std::ios::binary);
	std::string textToSave = "P3\n1024 1024\n256";

	TArray<FLinearColor> SurfData;
	FTextureRenderTargetResource* Resource = SceneCaptureComponent2D->TextureTarget->GameThread_GetRenderTargetResource();
	SurfData.Reset();
	if (Resource->ReadLinearColorPixels(SurfData) == true) {

		UE_LOG(LogTemp, Warning, TEXT("%s"), &SurfData == nullptr ? TEXT("True") : TEXT("False"));
		UE_LOG(LogTemp, Warning, TEXT("%d"), SurfData.Num() / 1024);

		int j = 0;
		for (int i = 0; i < SurfData.Num(); i++)
		{
			bool nl = false;

			if (i % 4 == 0)
			{
				j++;
				textToSave += "\n";
				nl = true;
			}

			char buff[5];
			snprintf(buff, sizeof(buff), "%02d  %02d  %02d", (int)SurfData[i].R, (int)SurfData[i].G, (int)SurfData[i].B);
			textToSave += (nl ? " " : "") + std::string(buff) + " ";
		}
		UE_LOG(LogTemp, Warning, TEXT("lines: %d"), (int)j);

		char *p = new char[textToSave.size() + 1];

		std::memcpy(p, textToSave.c_str(), textToSave.size());

		file << p;
		file.close();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString((saveDirectoryStr + "/Capture.pgm").c_str()));
	}
}