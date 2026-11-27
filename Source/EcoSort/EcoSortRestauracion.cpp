// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortRestauracion.h"
#include "EcoSortGameState.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

AEcoSortRestauracion::AEcoSortRestauracion()
{
	PrimaryActorTick.bCanEverTick = false;

	NombreParametroColor = FName(TEXT("ColorAgua"));
	NombreParametroClaridad = FName(TEXT("Claridad"));

	// Color inicial del agua contaminada (café verdoso oscuro/lodoso)
	ColorAguaContaminada = FLinearColor(0.22f, 0.17f, 0.09f, 1.0f);

	// Color del agua restaurada al 100% (turquesa cristalino del río limpio)
	ColorAguaLimpia = FLinearColor(0.05f, 0.45f, 0.65f, 0.75f);

	bHito30Alcanzado = false;
	bHito60Alcanzado = false;
	bHito100Alcanzado = false;
}

void AEcoSortRestauracion::BeginPlay()
{
	Super::BeginPlay();

	// 1. Preparar el material dinámico del río
	InicializarMaterialAgua();

	// 2. Al inicio, la fauna y vegetación de los hitos comienzan ocultas
	ConfigurarVisibilidadGrupo(ActoresHito30, false);
	ConfigurarVisibilidadGrupo(ActoresHito60, false);
	ConfigurarVisibilidadGrupo(ActoresHito100, false);

	// 3. Suscribirse a los cambios de Hábitat en el GameState
	AEcoSortGameState* EcoGS = Cast<AEcoSortGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (EcoGS)
	{
		EcoGS->OnHabitatCambiado.AddDynamic(this, &AEcoSortRestauracion::HandleHabitatCambiado);
		AplicarRestauracion(EcoGS->GetHabitatPorcentaje());
	}
}

void AEcoSortRestauracion::InicializarMaterialAgua()
{
	if (!ActorAguaRio)
	{
		return;
	}

	UStaticMeshComponent* MallaAgua = ActorAguaRio->FindComponentByClass<UStaticMeshComponent>();
	if (MallaAgua)
	{
		MaterialDinamicoAgua = MallaAgua->CreateAndSetMaterialInstanceDynamic(0);
		if (MaterialDinamicoAgua)
		{
			MaterialDinamicoAgua->SetVectorParameterValue(NombreParametroColor, ColorAguaContaminada);
			MaterialDinamicoAgua->SetScalarParameterValue(NombreParametroClaridad, 0.0f);
		}
	}
}

void AEcoSortRestauracion::ConfigurarVisibilidadGrupo(const TArray<TObjectPtr<AActor>>& Grupo, bool bVisible)
{
	for (AActor* Actor : Grupo)
	{
		if (IsValid(Actor))
		{
			Actor->SetActorHiddenInGame(!bVisible);
			Actor->SetActorEnableCollision(bVisible);
			Actor->SetActorTickEnabled(bVisible);
		}
	}
}

void AEcoSortRestauracion::HandleHabitatCambiado(float NuevoPorcentaje)
{
	AplicarRestauracion(NuevoPorcentaje);
}

void AEcoSortRestauracion::AplicarRestauracion(float Porcentaje)
{
	const float Alpha = FMath::Clamp(Porcentaje / 100.0f, 0.0f, 1.0f);

	// 1. Actualizar el agua del río (de café oscuro a turquesa brillante)
	if (MaterialDinamicoAgua)
	{
		const FLinearColor ColorActual = FMath::Lerp(ColorAguaContaminada, ColorAguaLimpia, Alpha);
		MaterialDinamicoAgua->SetVectorParameterValue(NombreParametroColor, ColorActual);
		MaterialDinamicoAgua->SetScalarParameterValue(NombreParametroClaridad, Alpha);
	}

	// 2. Comprobar Hito 30% (Vegetación ribereña / pequeños brotes)
	if (Porcentaje >= 30.0f && !bHito30Alcanzado)
	{
		bHito30Alcanzado = true;
		ConfigurarVisibilidadGrupo(ActoresHito30, true);
		OnHitoAlcanzado(30, Porcentaje);
	}

	// 3. Comprobar Hito 60% (Fauna de agua / patos nadando)
	if (Porcentaje >= 60.0f && !bHito60Alcanzado)
	{
		bHito60Alcanzado = true;
		ConfigurarVisibilidadGrupo(ActoresHito60, true);
		OnHitoAlcanzado(60, Porcentaje);
	}

	// 4. Comprobar Hito 100% (Fauna completa / garzas y mariposas)
	if (Porcentaje >= 100.0f && !bHito100Alcanzado)
	{
		bHito100Alcanzado = true;
		ConfigurarVisibilidadGrupo(ActoresHito100, true);
		OnHitoAlcanzado(100, Porcentaje);
	}
}
