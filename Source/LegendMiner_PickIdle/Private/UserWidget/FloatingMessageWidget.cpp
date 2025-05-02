#include "FloatingMessageWidget.h"
#include "Components/TextBlock.h"

void UFloatingMessageWidget::SetMessageText(const FText& NewText)
{
    if (MessageText)
    {
        MessageText->SetText(NewText);
    }
}
