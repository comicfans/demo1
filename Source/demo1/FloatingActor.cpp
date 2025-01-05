// Fill out your copyright notice in the Description page of Project Settings.

#include "FloatingActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "Math/Rotator.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AFloatingActor::AFloatingActor() {
  // Set this actor to call Tick() every frame.  You can turn this off to
  // improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;

  {
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ouster1"));
    VisualMesh->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> OusterVisualAsset(
        TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));

    if (OusterVisualAsset.Succeeded()) {
      VisualMesh->SetStaticMesh(OusterVisualAsset.Object);
      VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.5f));
    }
  }

  {

    ZeroMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Zero"));
    ZeroMesh->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> ZeroVisualAsset(
        TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));

    if (ZeroVisualAsset.Succeeded()) {
      ZeroMesh->SetStaticMesh(ZeroVisualAsset.Object);
      ZeroMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
      ZeroMesh->SetRelativeRotation(FRotator(90.0f));
    }
  }
}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay() { Super::BeginPlay(); }
// lidar run 10 hz, every full scan take 1024/64 = 16 frames
constexpr auto FULL_SCAN_HZ = 0.1;
constexpr auto FRAME_COLUMN = 64;
constexpr auto FULL_COLUMN = 1024;
constexpr auto FRAME_PER_SCAN = FULL_COLUMN / FRAME_COLUMN;
constexpr auto ONE_FRAME_TIME = 1.0f / FULL_SCAN_HZ / FRAME_PER_SCAN;
constexpr auto VERTICAL_RESOLUTION = 128;
static AFloatingActor::ScanFrameIndex

TimeToIndex(const float elapsedSinceCreate) {

  const uint64_t FrameCount = elapsedSinceCreate / ONE_FRAME_TIME;

  return {uint8_t(FrameCount / FRAME_PER_SCAN), FrameCount % FRAME_PER_SCAN};
}

void AFloatingActor::EmitLidarScan() {

  FVector center = GetActorLocation();
  FRotator rotate = GetActorRotation();

  auto frame_percent = 1.0 / FRAME_PER_SCAN;

  auto percent_start = frame_percent * LastScanFrameIndex.second;
  auto column_percent = frame_percent / FRAME_COLUMN;

  constexpr auto EL_ANGLE = 128.0f;

  constexpr auto EL_PERCENT = 1.0f / VERTICAL_RESOLUTION;

  for (int el = 0; el < VERTICAL_RESOLUTION; ++el) {
    for (auto az = 0; az < FRAME_COLUMN; ++az) {

      auto this_rotate =
          rotate +
          FRotator((el - VERTICAL_RESOLUTION / 2) * EL_PERCENT * EL_ANGLE,
                   (frame_percent * LastScanFrameIndex.second +
                    az * column_percent) *
                       360,
                   0);

      FVector TraceStart = center + this_rotate.Vector() * 0.05;
      FVector TraceEnd = center + this_rotate.Vector() * 1000;
      FHitResult Hit;
      // You can use FCollisionQueryParams to further configure the query
      // Here we add ourselves to the ignored list so we won't block the trace
      FCollisionQueryParams QueryParams;
      QueryParams.AddIgnoredActor(this);
      // To run the query, you need a pointer to the current level, which you
      // can get from an Actor with GetWorld()
      // UWorld()->LineTraceSingleByChannel runs a line trace and returns the
      // first actor hit over the provided collision channel.
      GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd,
                                           TraceChannelProperty, QueryParams);
      // You can use DrawDebug helpers and the log to help visualize and debug
      // your trace queries.
      DrawDebugLine(
          GetWorld(), TraceStart,
          Hit.bBlockingHit ? (TraceStart + this_rotate.Vector() * Hit.Distance)
                           : TraceEnd,
          Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 1.0f, 0, 1.0f);
    }
  }
}

// Called every frame
void AFloatingActor::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  // auto NewRotation = GetActorRotation();

  auto RunningTime = GetGameTimeSinceCreation();
  // auto DeltaHeight =
  //     (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime)) * 5;
  // auto DeltaRotation = DeltaTime * RotateSpeed;
  // NewRotation.Yaw += DeltaRotation;
  // SetActorRotation(NewRotation);

  auto ThisScanFrameIndex = TimeToIndex(RunningTime);
  if (ThisScanFrameIndex != LastScanFrameIndex) {
    LastScanFrameIndex = ThisScanFrameIndex;
    EmitLidarScan();
  }
}
