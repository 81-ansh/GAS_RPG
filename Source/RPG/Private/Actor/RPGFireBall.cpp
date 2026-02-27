// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/RPGFireBall.h"

void ARPGFireBall::BeginPlay()
{
	Super::BeginPlay();
}

void ARPGFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}
