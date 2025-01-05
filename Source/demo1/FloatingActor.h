// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/* clang-format off */
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingActor.generated.h"
/* clang-format on */

UCLASS()
class DEMO1_API AFloatingActor : public AActor {
  GENERATED_BODY()

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *VisualMesh;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent *ZeroMesh;

  UPROPERTY(EditAnywhere, Category = "Collision")
  TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;

  UPROPERTY(EditAnywhere)
  float ScanLength = 1000.0f;

  UPROPERTY(EditAnywhere)
  float RotateSpeed = 40.0f;

public:
  using ScanFrameIndex = std::pair<uint8_t, uint8_t>;
  // Sets default values for this actor's properties
  AFloatingActor();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

private:
  ScanFrameIndex LastScanFrameIndex = {255, 255};
  void EmitLidarScan();
};
