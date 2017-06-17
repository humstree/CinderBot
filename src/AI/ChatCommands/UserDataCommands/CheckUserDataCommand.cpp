/*************************************************************************
***************  CinderBot - standalone bot for Twitch.tv ****************
******** Copyright (C) 2017  Ilya Lobanov (exanimoteam@gmail.com) ********
********         Check full copyright header in main.cpp          ********
**************************************************************************/
#include "CheckUserDataCommand.hpp"
#include "Utils/UserData/UserData.hpp"
#include "Utils/DatabaseManager.hpp"
#include "Utils/Config/ConfigurationManager.hpp"
#include "Utils/Config/ConfigurationParameters.hpp"

using namespace Command::UserDataCmd;

#define MSG_NOT_LEADER 0
#define MSG_NO_COV     1
#define MSG_IS_LEADER  2
#define MSG_NO_NAME    3
#define MSG_NO_USER    4

///////////////////////////////////////////////////////////////////////////

CheckUserDataCommand::CheckUserDataCommand()
{
    Initialize();
}

///////////////////////////////////////////////////////////////////////////

void CheckUserDataCommand::Initialize()
{
    _name = "!check";
    _moderatorOnly = true;
    _answers.push_back("USER_NAME: Message - MSG_COUNT; MSG_NAME_CUR - MSG_CUR; Covenant - MSG_COV.");
    _answers.push_back("USER_NAME: Message - MSG_COUNT; MSG_NAME_CUR - MSG_CUR; Not in covenant.");
    _answers.push_back("USER_NAME: Message - MSG_COUNT; MSG_NAME_CUR - MSG_CUR; Leader of covenant 'MSG_COV'.");
    _answers.push_back("Please provide a name, @!");
    _answers.push_back("User with such name is not exist, @!");
}

///////////////////////////////////////////////////////////////////////////

void CheckUserDataCommand::_GetAnswer(const ChatMessage &message, QStringList &answer)
{
    if (_CheckModerationFlag(message.IsModerator()))
    {
        QString userName;
        if (message.GetMessage().size() > _name.size())
        {
            userName = message.GetMessage().right(message.GetMessage().size() - _name.size() - 1);
        }
        qDebug() << userName;
        if (!userName.isEmpty())
        {
            DB_QUERY_PTR userQuery = DB_SELECT("UserData", "*", QString("Author='%1'").arg(userName));
            if ((userQuery != nullptr) && (userQuery->first()))
            {
                QString covenant = UD_GET_PARAM(userQuery->value("Name").toString() ,UDP_Covenant);
                DB_QUERY_PTR query = DB_SELECT("Covenants", "Leader", QString("Name = '%1'").arg(covenant));
                if (query != nullptr)
                {
                    if (query->first())
                    {
                        if (query->value("Leader").toString() == userQuery->value("Name"))
                        {
                            answer.append(_answers.at(MSG_IS_LEADER));
                        }
                    }
                }
                if (answer.isEmpty())
                {
                    if (covenant == "Viewer")
                    {
                        answer.append(_answers.at(MSG_NO_COV));
                    }
                    else
                    {
                        answer.append(_answers.at(MSG_NOT_LEADER));
                    }
                }
                if (!answer.isEmpty())
                {
                    auto firstAnswer = answer.begin();
                    QString curName = "NomNom ";
                    ConfigurationManager::Instance().GetStringParam(CFGP_CURRENCY, curName);
                    (*firstAnswer).replace("MSG_NAME_CUR", curName);
                    (*firstAnswer).replace("MSG_COUNT", UD_GET_PARAM(userName.toLower() ,UDP_Messages));
                    (*firstAnswer).replace("MSG_CUR", UD_GET_PARAM(userName.toLower() ,UDP_Currency));
                    (*firstAnswer).replace("MSG_COV", UD_GET_PARAM(userName.toLower() ,UDP_Covenant));
                    (*firstAnswer).replace("USER_NAME", userName);
                }
            }
            else
            {
                answer.append(_answers.at(MSG_NO_USER));
            }
        }
        else
        {
            answer.append(_answers.at(MSG_NO_NAME));
        }
    }
}

///////////////////////////////////////////////////////////////////////////

void CheckUserDataCommand::_GetRandomAnswer(const ChatMessage &message, QStringList &answer)
{
    Q_UNUSED(message);
    Q_UNUSED(answer);
}

///////////////////////////////////////////////////////////////////////////