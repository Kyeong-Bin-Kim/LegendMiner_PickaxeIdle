#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API UMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    // 블루프린트에서 레벨을 직접 선택할 수 있도록 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game", meta = (AllowedClasses = "World"))
    TSoftObjectPtr<UWorld> LevelToLoad;

    UFUNCTION()
    void OnStartGameClicked();

    UFUNCTION()
    void OnContinueGameClicked();

    UFUNCTION()
    void OnQuitGameClicked();

    UFUNCTION()
    void UpdateUIBasedOnSaveData();

	UFUNCTION()
	void OnNewGameConfirmed(bool bConfirmed);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UButton* StartGameButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* ContinueGameButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* QuitGameButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StartGameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ContinueGameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* QuitGameText;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* ButtonContainer;

private:
    UFUNCTION()
    void CloseMainMenu();

    UFUNCTION()
    void StartNewGame();
};
