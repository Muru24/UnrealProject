// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeBody.h"
#include "SnakeBodyChargeComponent.h"

ASnakeBody::ASnakeBody()
{
	BodyChargeComponent = CreateDefaultSubobject<USnakeBodyChargeComponent>(TEXT("BodyChargeComponent"));
}

