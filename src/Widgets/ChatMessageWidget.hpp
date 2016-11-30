#pragma once
#include <QTextEdit>

namespace Ui
{

/*!
 * Class ChatMessageWidget
 * Stores chat message and automatically resizes to match size of a message.
 */
class ChatMessageWidget : public QTextEdit
{
    Q_OBJECT
public:
    explicit ChatMessageWidget(const QString& text, QWidget* parent = 0);

private slots:
    /*!
     * Adjust minimum possible size of chat message
     * \param(IN) size - New size of the document in message. In pixels.
     */
    void _AdjustMinimumSize(const QSizeF& size);
};

}
