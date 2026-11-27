// Copyright EcoSort: San Juan del Rio. Todos los derechos reservados.

#include "EcoSortGameMode.h"
#include "EcoSortGameState.h"
#include "EcoSortHUDBase.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

AEcoSortGameMode::AEcoSortGameMode()
{
	PrimaryActorTick.bCanEverTick = false;

	// Asignar por defecto nuestro GameState personalizado
	GameStateClass = AEcoSortGameState::StaticClass();

	NivelActualID = FName(TEXT("Nivel1"));
	bJuegoTerminado = false;

	// Valores iniciales por defecto para Nivel 1 (Orilla Gris)
	ConfigNivelActual.NombreNivel = FText::FromString(TEXT("Nivel 1 - Orilla Gris"));
	ConfigNivelActual.MetaResiduos = 10;
	ConfigNivelActual.NivelRioInicial = 100.0f;
	ConfigNivelActual.DrenajeRioPorSegundo = 0.0f;
	ConfigNivelActual.IntervaloSpawn = 4.0f;
	ConfigNivelActual.MaxResiduosEnEscena = 6;
	ConfigNivelActual.SiguienteNivel = FName(TEXT("Nivel2_CorrienteEnMovimiento"));
}

void AEcoSortGameMode::BeginPlay()
{
	Super::BeginPlay();

	bJuegoTerminado = false;

	// 1. Cargar datos del DataTable si está asignado
	CargarConfiguracionNivel();

	// 2. Suscribirse a los eventos del GameState para vigilar victoria y derrota
	AEcoSortGameState* EcoGS = GetGameState<AEcoSortGameState>();
	if (EcoGS)
	{
		EcoGS->OnResiduoClasificado.AddDynamic(this, &AEcoSortGameMode::HandleResiduoClasificado);
		EcoGS->OnNivelRioCambiado.AddDynamic(this, &AEcoSortGameMode::HandleNivelRioCambiado);

		// Iniciar temporizador de drenaje si el nivel lo requiere (Nivel 2)
		if (ConfigNivelActual.DrenajeRioPorSegundo > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
				TimerHandle_DrenajeRio,
				this,
				&AEcoSortGameMode::AplicarDrenajeRio,
				1.0f,
				true
			);
		}
	}

	// 3. Crear el HUD en pantalla
	CrearHUD();
}

void AEcoSortGameMode::CargarConfiguracionNivel()
{
	if (TablaNiveles && !NivelActualID.IsNone())
	{
		const FEcoNivelConfig* Fila = TablaNiveles->FindRow<FEcoNivelConfig>(
			NivelActualID,
			TEXT("EcoSortGameMode_Configuracion")
		);

		if (Fila)
		{
			ConfigNivelActual = *Fila;
		}
	}

	// Aplicar meta inicial y salud de río en el GameState
	AEcoSortGameState* EcoGS = GetGameState<AEcoSortGameState>();
	if (EcoGS)
	{
		EcoGS->SetMetaResiduos(ConfigNivelActual.MetaResiduos);
		EcoGS->SetNivelRio(ConfigNivelActual.NivelRioInicial);
	}
}

void AEcoSortGameMode::CrearHUD()
{
	if (!HUDWidgetClass)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		return;
	}

	HUDWidgetInstancia = CreateWidget<UEcoSortHUDBase>(PC, HUDWidgetClass);
	if (HUDWidgetInstancia)
	{
		HUDWidgetInstancia->AddToViewport(0);
	}
}

void AEcoSortGameMode::AplicarDrenajeRio()
{
	if (bJuegoTerminado)
	{
		return;
	}

	AEcoSortGameState* EcoGS = GetGameState<AEcoSortGameState>();
	if (EcoGS)
	{
		EcoGS->ReducirNivelRio(ConfigNivelActual.DrenajeRioPorSegundo);
	}
}

void AEcoSortGameMode::HandleResiduoClasificado(int32 Clasificados, int32 Meta)
{
	if (bJuegoTerminado)
	{
		return;
	}

	// Condición de Victoria: Alcanzar o superar la meta de clasificación
	if (Clasificados >= Meta)
	{
		bJuegoTerminado = true;
		GetWorldTimerManager().ClearTimer(TimerHandle_DrenajeRio);
		OnVictoria();
	}
}

void AEcoSortGameMode::HandleNivelRioCambiado(float NuevoNivel)
{
	if (bJuegoTerminado)
	{
		return;
	}

	// Condición de Derrota: El río llegó a 0 de salud
	if (NuevoNivel <= 0.0f)
	{
		bJuegoTerminado = true;
		GetWorldTimerManager().ClearTimer(TimerHandle_DrenajeRio);
		OnDerrota();
	}
}

void AEcoSortGameMode::OnVictoria_Implementation()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && VictoriaWidgetClass)
	{
		UUserWidget* WidgetVictoria = CreateWidget<UUserWidget>(PC, VictoriaWidgetClass);
		if (WidgetVictoria)
		{
			WidgetVictoria->AddToViewport(10);
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeUIOnly());
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("¡VICTORIA! Nivel completado con éxito."));
	}
}

void AEcoSortGameMode::OnDerrota_Implementation()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && DerrotaWidgetClass)
	{
		UUserWidget* WidgetDerrota = CreateWidget<UUserWidget>(PC, DerrotaWidgetClass);
		if (WidgetDerrota)
		{
			WidgetDerrota->AddToViewport(10);
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeUIOnly());
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("¡DERROTA! El río se ha contaminado por completo."));
	}
}

void AEcoSortGameMode::CargarSiguienteNivel()
{
	if (!ConfigNivelActual.SiguienteNivel.IsNone())
	{
		UGameplayStatics::OpenLevel(this, ConfigNivelActual.SiguienteNivel);
	}
}

void AEcoSortGameMode::ReiniciarNivelActual()
{
	UWorld* World = GetWorld();
	if (World)
	{
		const FString NombreMapaActual = World->GetMapName();
		UGameplayStatics::OpenLevel(this, FName(*NombreMapaActual));
	}
}
