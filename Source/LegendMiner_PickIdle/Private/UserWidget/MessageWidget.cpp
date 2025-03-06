#include "MessageWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"

void UMessageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ConfirmButton)
    {
        ConfirmButton->OnClicked.AddDynamic(this, &UMessageWidget::OnConfirmClicked);
    }

    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &UMessageWidget::OnCancelClicked);
    }
}


void UMessageWidget::SetupMessage(const FText& Message, bool bShowCancelButton,
    const FText& ConfirmText, const FText& CancelText)
{
    if (MessageText)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message.ToString());
        MessageText->SetText(Message);
    }

    if (ConfirmButton && ConfirmButton->GetChildAt(0))
    {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ConfirmButton");
        if (UTextBlock* ConfirmTextBlock = Cast<UTextBlock>(ConfirmButton->GetChildAt(0)))
        {
            ConfirmTextBlock->SetText(ConfirmText);
        }
    }

    if (CancelButton)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "CancelButton");
        CancelButton->SetVisibility(bShowCancelButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

        if (CancelButton->GetChildAt(0))
        {
            if (UTextBlock* CancelTextBlock = Cast<UTextBlock>(CancelButton->GetChildAt(0)))
            {
                CancelTextBlock->SetText(CancelText);
            }
        }
    }
}

void UMessageWidget::OnConfirmClicked()
{
    OnMessageConfirmed.Broadcast(true);
    RemoveFromParent();
}

void UMessageWidget::OnCancelClicked()
{
    OnMessageConfirmed.Broadcast(false);
    RemoveFromParent();
}
