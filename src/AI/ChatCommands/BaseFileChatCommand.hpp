#pragma once
#include "BaseChatCommand.hpp"
#include <QPair>
#include <QString>
#include <QVector>
#include <QXmlStreamReader>

/*!
 * Class BaseFileChatCommand
 * Stores custom commands that was defined user in special file.
 */
class BaseFileChatCommand : public BaseChatCommand
{
public:
    /*!
     * Try to initialize custom commands from file
     * \return true if custom commands were found
     */
    bool Initialize();
    ////////////////////////////////
    /// BaseChatCommand override
    bool GetAnswer(ChatMessage& message, QString& answer);

protected:
    /*! List of custom commands */
    QVector<QPair<QString, QVector<QString> > > _commands;
    /*! Xml stream reader */
    QXmlStreamReader _xmlReader;

private:
    /*!
     * Try to read command and add it to the list
     */
    void _ReadCommand();
};
