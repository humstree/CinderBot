/*************************************************************************
***************  CinderBot - standalone bot for Twitch.tv ****************
******** Copyright (C) 2017  Ilya Lobanov (exanimoteam@gmail.com) ********
********         Check full copyright header in main.cpp          ********
**************************************************************************/
#include "CreateCovenantCommand.hpp"
#include "Utils/Database/UserDataDBHelper.hpp"
#include "Utils/Config/ConfigurationManager.hpp"
#include "Utils/Config/ConfigurationParameters.hpp"
#include "Utils/Database/DatabaseManager.hpp"

using namespace Command::CovenantCmd;
using namespace Utils::Configuration;
using namespace Utils::Database;

#define MSG_NO_CURRENCY      0
#define MSG_USER_IS_LEADER   1
#define MSG_COVENANT_CREATED 2
#define MSG_PROVIDE_NAME     3
#define MSG_ALREADY_EXIST    4

#define MAX_COVENANT_NAME_LENGTH 50
#define DEFAULT_PRICE_FOR_CREATE "2000"

///////////////////////////////////////////////////////////////////////////

CreateCovenantCommand::CreateCovenantCommand()
{
    Initialize();
}

///////////////////////////////////////////////////////////////////////////

void CreateCovenantCommand::Initialize()
{
    _name = "!cov_create";
    _answers.push_back("You do not have enought currency, @.");
    _answers.push_back("You are leader of another covenant, @! Leave or disband it and try again.");
    _answers.push_back("Covenant 'COV_NAME' was created. @! Now you are leader of it.");
    _answers.push_back("Please provide name of new covenant, @.");
    _answers.push_back("Such covenant already exist, @.");
}

///////////////////////////////////////////////////////////////////////////

void CreateCovenantCommand::_GetAnswer(const ChatMessage &message, ChatAnswer &answer)
{
    QString covenant = UserDataDBHelper::GetUserParameter(UserDataParameter::Covenant, message.GetUserID()).toString();
    // Check if user is leader of its covenant
    DB_QUERY_PTR query = DB_SELECT("Covenants", "Leader", QString("Name = '%1'").arg(covenant));
    if (query->exec())
    {
        // If user is in covenant, check its leader
        if (query->first())
        {
            if (query->value("Leader").toString() == message.GetRealName())
            {
                // User is leader of another covenant
                answer.AddAnswer(_answers.at(MSG_USER_IS_LEADER));
            }
        }
        // If user is not a leader or he not in a covenant, he is allowed to create covenant
        if (answer.GetAnswers().isEmpty())
        {
            // Get amount currency which is needed to create covenant
            QString price;
            if (!ConfigurationManager::Instance().GetStringParam(CfgParam::CovCreatePrice, price))
            {
                price = DEFAULT_PRICE_FOR_CREATE;
            }
            _price = price.toInt();
            // Check if user have enough currency
            if (_CheckCurrency(message.GetUserID()))
            {
                // Get covenant name
                QString newCovenant;
                if (message.GetMessage().size() > _name.size())
                {
                   newCovenant = message.GetMessage().right(message.GetMessage().size()-_name.size()-1);
                   newCovenant.replace("'", "");
                }
                // If covenant name too long, just make it shorter
                if (newCovenant.size() > MAX_COVENANT_NAME_LENGTH)
                {
                    newCovenant.left(MAX_COVENANT_NAME_LENGTH);
                }
                // If user provided covenant name, create it
                if (!newCovenant.isEmpty())
                {
                    // Check if such covenant already exist
                    DB_QUERY_PTR queryName = DB_SELECT("Covenants", "COUNT(*)",
                                                                    QString("Name = '%1'").arg(newCovenant));
                    if (queryName != NULL)
                    {
                        if (queryName->first())
                        {
                            if (queryName->value(0).toInt() != 0)
                            {
                                answer.AddAnswer(_answers.at(MSG_ALREADY_EXIST));
                            }
                        }
                    }

                    // Create new covenant if such covenant not exist
                    // Columns: Id | Name | Leader | Description | CmdSlots | Level | Exp | MaxMembers
                    if ((answer.GetAnswers().isEmpty()) &&
                        (DB_INSERT("Covenants", QString("NULL, '%1', '%2', "
                                                        "'',   1,    1,"
                                                        "0,    10").arg(newCovenant).arg(message.GetRealName()))))
                    {
                        // Update covenant field for user
                        UserDataDBHelper::UpdateUserParameter(UserDataParameter::Covenant, newCovenant, message.GetUserID());
                        // Take price to create covenant
                        _TakeDefaultPriceFromUser(message.GetUserID());
                        // Set answer
                        answer.AddAnswer(_answers.at(MSG_COVENANT_CREATED));
                        (*answer.GetAnswers().begin()).replace("COV_NAME", newCovenant);
                    }
                }
                // Name of covenant doesn't provided
                else
                {
                    answer.AddAnswer(_answers.at(MSG_PROVIDE_NAME));
                }
            }
            // Not enough currency
            else
            {
                answer.AddAnswer(_answers.at(MSG_NO_CURRENCY));
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////

void CreateCovenantCommand::_GetRandomAnswer(const ChatMessage &message, ChatAnswer &answer)
{
    Q_UNUSED(message);
    Q_UNUSED(answer);
}

///////////////////////////////////////////////////////////////////////////
