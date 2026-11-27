// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortSpawner.h"
#include "EcoSortResiduo.h"
#include "EcoSortGameMode.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

AEcoSortSpawner::AEcoSortSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	EscenaRaiz = CreateDefaultSubobject<USceneComponent>(TEXT("EscenaRaiz"));
	RootComponent = EscenaRaiz;

	RadioSpawn = 700.0f;          // 7 metros de dispersión
	IntervaloSpawn = 4.5f;        // Cada 4.5 segundos
	MaxResiduosEnEscena = 8;      // Máximo 8 residuos libres al mismo tiempo
	AlturaCaida = 40.0f;          // 40 cm sobre el suelo para caída física natural
	bAutoIniciar = true;
}

void AEcoSortSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Sincronizar intervalo y límite con el GameMode si están configurados en el nivel
	AEcoSortGameMode* EcoGM = Cast<AEcoSortGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (EcoGM)
	{
		const FEcoNivelConfig& Config = EcoGM->GetConfigNivelActual();
		if (Config.IntervaloSpawn > 0.0f)
		{
			IntervaloSpawn = Config.IntervaloSpawn;
		}
		if (Config.MaxResiduosEnEscena > 0)
		{
			MaxResiduosEnEscena = Config.MaxResiduosEnEscena;
		}
	}

	if (bAutoIniciar)
	{
		IniciarSpawner();
	}
}

void AEcoSortSpawner::IniciarSpawner()
{
	if (!GetWorld() || ClasesResiduos.Num() == 0)
	{
		return;
	}

	GetWorldTimerManager().SetTimer(
		TimerHandle_Spawn,
		this,
		&AEcoSortSpawner::HandleTimerSpawn,
		IntervaloSpawn,
		true,
		1.0f // Primer intento 1 segundo después del inicio
	);
}

void AEcoSortSpawner::HandleTimerSpawn()
{
	GenerarResiduo();
}

void AEcoSortSpawner::DetenerSpawner()
{
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Spawn);
	}
}

int32 AEcoSortSpawner::ContarResiduosActivos() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return 0;
	}

	TArray<AActor*> ResiduosEncontrados;
	UGameplayStatics::GetAllActorsOfClass(World, AEcoSortResiduo::StaticClass(), ResiduosEncontrados);

	int32 ConteoActivos = 0;
	for (AActor* Actor : ResiduosEncontrados)
	{
		AEcoSortResiduo* Residuo = Cast<AEcoSortResiduo>(Actor);
		if (Residuo && !Residuo->EstaCargado())
		{
			ConteoActivos++;
		}
	}

	return ConteoActivos;
}

bool AEcoSortSpawner::CalcularPosicionSpawn(FVector& OutLocation) const
{
	// Opción A: Si hay puntos fijos asignados en el editor, usar uno al azar
	if (PuntosSpawnFijos.Num() > 0)
	{
		TArray<AActor*> PuntosValidos;
		for (AActor* Punto : PuntosSpawnFijos)
		{
			if (IsValid(Punto))
			{
				PuntosValidos.Add(Punto);
			}
		}

		if (PuntosValidos.Num() > 0)
		{
			const int32 Indice = FMath::RandRange(0, PuntosValidos.Num() - 1);
			OutLocation = PuntosValidos[Indice]->GetActorLocation() + FVector(0.0f, 0.0f, AlturaCaida);
			return true;
		}
	}

	// Opción B: Posición aleatoria en círculo XY con raycast hacia el suelo
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector Centro = GetActorLocation();
	const float AnguloAleatorio = FMath::FRandRange(0.0f, 2.0f * PI);
	const float DistanciaAleatoria = FMath::FRandRange(100.0f, RadioSpawn);

	const float OffsetX = FMath::Cos(AnguloAleatorio) * DistanciaAleatoria;
	const float OffsetY = FMath::Sin(AnguloAleatorio) * DistanciaAleatoria;

	const FVector RayoInicio = Centro + FVector(OffsetX, OffsetY, 200.0f);
	const FVector RayoFin = Centro + FVector(OffsetX, OffsetY, -600.0f);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EcoSortSpawnerTrace), false, this);

	if (World->LineTraceSingleByChannel(Hit, RayoInicio, RayoFin, ECC_WorldStatic, Params))
	{
		OutLocation = Hit.ImpactPoint + FVector(0.0f, 0.0f, AlturaCaida);
		return true;
	}

	// Si no impactó el suelo, usar el centro con la altura de caída
	OutLocation = Centro + FVector(OffsetX, OffsetY, AlturaCaida);
	return true;
}

AEcoSortResiduo* AEcoSortSpawner::GenerarResiduo()
{
	UWorld* World = GetWorld();
	if (!World || ClasesResiduos.Num() == 0)
	{
		return nullptr;
	}

	// 1. Respetar el límite de residuos simultáneos
	if (ContarResiduosActivos() >= MaxResiduosEnEscena)
	{
		return nullptr;
	}

	// 2. Elegir clase de residuo al azar (PET, Lata, Papel, etc.)
	const int32 IndiceClase = FMath::RandRange(0, ClasesResiduos.Num() - 1);
	TSubclassOf<AEcoSortResiduo> ClaseElegida = ClasesResiduos[IndiceClase];
	if (!ClaseElegida)
	{
		return nullptr;
	}

	// 3. Obtener ubicación en el suelo
	FVector PosicionSpawn;
	if (!CalcularPosicionSpawn(PosicionSpawn))
	{
		return nullptr;
	}

	// 4. Rotación aleatoria en Z para variación visual
	const FRotator RotacionSpawn(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEcoSortResiduo* NuevoResiduo = World->SpawnActor<AEcoSortResiduo>(
		ClaseElegida,
		PosicionSpawn,
		RotacionSpawn,
		SpawnParams
	);

	if (NuevoResiduo)
	{
		OnResiduoGenerado(NuevoResiduo);
	}

	return NuevoResiduo;
}
