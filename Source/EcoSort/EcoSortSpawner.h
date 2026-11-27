// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EcoSortSpawner.generated.h"

class AEcoSortResiduo;
class USceneComponent;

/**
 * Generador automático de residuos reciclables en el nivel.
 * Monitorea la cantidad de residuos activos y genera nuevos periódicamente
 * en ubicaciones aleatorias dentro de su radio o en puntos fijos.
 */
UCLASS()
class ECOSORT_API AEcoSortSpawner : public AActor
{
	GENERATED_BODY()

public:
	AEcoSortSpawner();

protected:
	virtual void BeginPlay() override;

	/** Componente raíz para posicionar el Spawner en el mundo */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EcoSort|Componentes")
	TObjectPtr<USceneComponent> EscenaRaiz;

	/** Clases de residuos que este generador puede instanciar (BP_Residuo_PET, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Spawner")
	TArray<TSubclassOf<AEcoSortResiduo>> ClasesResiduos;

	/** Puntos fijos opcionales en el mapa donde pueden aparecer residuos */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Spawner")
	TArray<TObjectPtr<AActor>> PuntosSpawnFijos;

	/** Radio en centímetros alrededor del Spawner si no se usan puntos fijos (por defecto 6 metros) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Spawner", meta = (ClampMin = "100.0", UIMin = "100.0"))
	float RadioSpawn;

	/** Intervalo en segundos entre cada intento de generación */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Spawner", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float IntervaloSpawn;

	/** Límite máximo de residuos simultáneos en el nivel para evitar saturación */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Spawner", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxResiduosEnEscena;

	/** Altura adicional desde donde cae el residuo con física al suelo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Spawner")
	float AlturaCaida;

	/** Si es verdadero, el Spawner comenzará automáticamente en BeginPlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Spawner")
	bool bAutoIniciar;

	/** Manejador del temporizador cíclico de spawn */
	FTimerHandle TimerHandle_Spawn;

	/** Función invocada por el temporizador que ejecuta la generación */
	void HandleTimerSpawn();

	/** Cuenta cuántos residuos hay actualmente en el mundo que no estén en manos del jugador */
	int32 ContarResiduosActivos() const;

	/** Calcula una posición válida sobre el suelo dentro del radio */
	bool CalcularPosicionSpawn(FVector& OutLocation) const;

public:
	/** Inicia la generación periódica de residuos */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Spawner")
	void IniciarSpawner();

	/** Detiene la generación de residuos */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Spawner")
	void DetenerSpawner();

	/** Ejecuta un intento de generación de residuo */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Spawner")
	AEcoSortResiduo* GenerarResiduo();

	/** Evento disparado en Blueprint cuando un nuevo residuo es creado con éxito */
	UFUNCTION(BlueprintImplementableEvent, Category = "EcoSort|Spawner")
	void OnResiduoGenerado(AEcoSortResiduo* NuevoResiduo);
};
