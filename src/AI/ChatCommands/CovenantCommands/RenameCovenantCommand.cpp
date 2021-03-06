/*************************************************************************
***************  CinderBot - standalone bot for Twitch.tv ****************
******** Copyright (C) 2017  Ilya Lobanov (exanimoteam@gmail.com) ********
********         Check full copyright header in main.cpp          ********
**************************************************************************/
#include "RenameCovenantCommand.hpp"
#include "Utils/Config/ConfigurationManager.hpp"
#include "Utils/Config/ConfigurationParameters.hpp"
#include "Utils/Database/DatabaseManager.hpp"
#include "Utils/Database/CustomCommandDBHelper.hpp"
#include "Utils/Database/UserDataDBHelper.hpp"

using namespace Command::CovenantCmd;
using namespace Utils::Configuration;
using namespace Utils::Database;

#define MSG_NOT_LEADER  0
#define MSG_RENAMED     1
#define MSG_NO_COVENANT 2
#define MSG_NO_NAME     3
#define MSG_NO_CURRENCY 4

#define MAX_COVENANT_NAME_LENGTH 50
#define DEFAULT_PRICE_FOR_RENAME "500"

///////////////////////////////////////////////////////////////////////////

RenameCovenantCommand::RenameCovenantCommand()
{
    Initialize();
}

///////////////////////////////////////////////////////////////////////////

void RenameCovenantCommand::Initialize()
{
    _name = "!cov_rename";
    _answers.push_back("You are not leader of your covenant, @!");
    _answers.push_back("Your covenant has been renamed, @.");
    _answers.push_back("You are not in any covenant yet, @.");
    _answers.push_back("Please provide a new name of covenant, @.");
    _answers.push_back("You do not have enought currency, @.");
}

///////////////////////////////////////////////////////////////////////////

void RenameCovenantCommand::_GetAnswer(const ChatMessage &message, ChatAnswer &answer)
{
    QString covenant = UserDataDBHelper::GetUserParameter(UserDataParameter::Covenant, message.GetUserID()).toString();
    // Check if user in covenant
    if (covenant != "Viewer")
    {
        // Check if user is leader of its covenant
        DB_QUERY_PTR query = DB_SELECT("Covenants", "Leader", QString("Name = '%1'").arg(covenant));
        if (query->exec())
        {
            // If user is in covenant, check its leader
            if (query->first())
            {
                if (query->value("Leader").toString() == message.GetRealName())
                {
                    // Get amount currency which is needed to rename covenant
                    QString price;
                    if (!ConfigurationManager::Instance().GetStringParam(CfgParam::CovRenamePrice, price))
                    {
                        price = DEFAULT_PRICE_FOR_RENAME;
                    }
                    _price = price.toInt();
                    if (_CheckCurrency(message.GetUserID()))
                    {
                        QString newCovenantName = message.GetMessage().mid(_name.size()+1);
                        newCovenantName.replace("'", "");
                        // If covenant name too long, just make it shorter
                        if (newCovenantName.size() > MAX_COVENANT_NAME_LENGTH)
                        {
                            newCovenantName.left(MAX_COVENANT_NAME_LENGTH);
                        }
                        // If user provided covenant name, rename it
                        if (!newCovenantName.isEmpty())
                        {
                            if (DB_UPDATE("Covenants", QString("Name = '%1'").arg(newCovenantName),
                                                       QString("Leader = '%1'").arg(message.GetRealName())))
                            {
                                // If covenant was renamed, set covenant field to a new value for all users who are in that covenant
                                UserDataDBHelper::UpdateCovenantName(covenant, newCovenantName);

                                // TODO: Replace this loops in US-329
                                // Update existing commands for covenant
                                QStringList cmdNames = CustomCommandDBHelper::Instance().GetCommandNames(CmdType::CovenantCmd, covenant);
                                for (int i = 0; i < cmdNames.size(); ++i)
                                {
                                    CustomCommandDBHelper::Instance().SetParameter(CmdType::CovenantCmd, cmdNames.at(i), CustomCmdParameter::Covenant, newCovenantName);
                                }
                                cmdNames.clear();

                                // TODO: Replace this loops in US-329
                                // Update commands created by broadcaster for covenant
                                cmdNames = CustomCommandDBHelper::Instance().GetCommandNames(CmdType::StreamerCmd, covenant);
                                for (int i = 0; i < cmdNames.size(); ++i)
                                {
                                    CustomCommandDBHelper::Instance().SetParameter(CmdType::StreamerCmd, cmdNames.at(i), CustomCmdParameter::Covenant, newCovenantName);
                                }

                                // Take price to rename covenant
                                _TakeDefaultPriceFromUser(message.GetUserID());
                                answer.AddAnswer(_answers.at(MSG_RENAMED));
                            }
                        }
                        // If name was not provided
                        else
                        {
                            answer.AddAnswer(_answers.at(MSG_NO_NAME));
                        }
                    }
                    // If user do not have enought currency
                    else
                    {
                        answer.AddAnswer(_answers.at(MSG_NO_CURRENCY));
                    }
                }
                // If user is not leader
                else
                {
                    answer.AddAnswer(_answers.at(MSG_NOT_LEADER));
                }
            }
        }
    }
    // If user is not in any covenant
    else
    {
        answer.AddAnswer(_answers.at(MSG_NO_COVENANT));
    }
}

///////////////////////////////////////////////////////////////////////////

void RenameCovenantCommand::_GetRandomAnswer(const ChatMessage &message, ChatAnswer &answer)
{
    Q_UNUSED(message);
    Q_UNUSED(answer);
}

///////////////////////////////////////////////////////////////////////////
