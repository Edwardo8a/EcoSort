// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EcoSortTipos.h"
#include "EcoSortGameMode.generated.h"

class UDataTable;
class UEcoSortHUDBase;
class UUserWidget;
class AEcoSortGameState;

/**
 * GameMode principal para EcoSort.
 * Aplica la configuración de nivel desde un DataTable (FEcoNivelConfig),
 * controla la victoria (meta alcanzada), la derrota (salud del río agotada)
 * y genera el HUD en pantalla.
 */
UCLASS()
class ECOSORT_API AEcoSortGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEcoSortGameMode();

protected:
	virtual void BeginPlay() override;

	/** DataTable que contiene la configuración de los niveles (DT_Niveles) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Configuracion")
	TObjectPtr<UDataTable> TablaNiveles;

	/** Identificador de la fila en el DataTable para este nivel (ej. "Nivel1", "Nivel2") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|Configuracion")
	FName NivelActualID;

	/** Clase del Widget Blueprint de interfaz a mostrar (WBP_HUD) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|UI")
	TSubclassOf<UEcoSortHUDBase> HUDWidgetClass;

	/** Instancia del HUD creada en pantalla */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|UI")
	TObjectPtr<UEcoSortHUDBase> HUDWidgetInstancia;

	/** Clase del Widget de Victoria a mostrar (WBP_Victoria) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|UI")
	TSubclassOf<UUserWidget> VictoriaWidgetClass;

	/** Clase del Widget de Derrota a mostrar (WBP_Derrota) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EcoSort|UI")
	TSubclassOf<UUserWidget> DerrotaWidgetClass;

	/** Datos del nivel actual leídos del DataTable */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "EcoSort|Configuracion")
	FEcoNivelConfig ConfigNivelActual;

	/** Manejador del temporizador para el drenaje del río (Nivel 2) */
	FTimerHandle TimerHandle_DrenajeRio;

	/** Indica si el juego en este nivel ya terminó (para evitar múltiples disparos) */
	bool bJuegoTerminado;

	/** Aplica el drenaje continuo al río si está configurado en el nivel */
	void AplicarDrenajeRio();

	/** Lee y aplica la configuración desde el DataTable */
	void CargarConfiguracionNivel();

	/** Crea e inicializa el HUD en pantalla */
	void CrearHUD();

	// Handlers de eventos del GameState
	UFUNCTION()
	void HandleResiduoClasificado(int32 Clasificados, int32 Meta);

	UFUNCTION()
	void HandleNivelRioCambiado(float NuevoNivel);

public:
	/** Evento disparado al ganar el nivel (meta de residuos alcanzada) */
	UFUNCTION(BlueprintNativeEvent, Category = "EcoSort|Flujo")
	void OnVictoria();

	/** Evento disparado al perder el nivel (la salud del río llegó a 0) */
	UFUNCTION(BlueprintNativeEvent, Category = "EcoSort|Flujo")
	void OnDerrota();

	/** Pasa al siguiente nivel configurado en FEcoNivelConfig */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Flujo")
	void CargarSiguienteNivel();

	/** Reinicia el nivel actual */
	UFUNCTION(BlueprintCallable, Category = "EcoSort|Flujo")
	void ReiniciarNivelActual();

	/** Consulta la configuración del nivel actual */
	UFUNCTION(BlueprintPure, Category = "EcoSort|Configuracion")
	const FEcoNivelConfig& GetConfigNivelActual() const { return ConfigNivelActual; }
};
