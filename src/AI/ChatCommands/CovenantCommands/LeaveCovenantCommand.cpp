/*************************************************************************
***************  CinderBot - standalone bot for Twitch.tv ****************
******** Copyright (C) 2017  Ilya Lobanov (exanimoteam@gmail.com) ********
********         Check full copyright header in main.cpp          ********
**************************************************************************/
#include "LeaveCovenantCommand.hpp"
#include "Utils/Config/ConfigurationManager.hpp"
#include "Utils/Database/DatabaseManager.hpp"
#include "Utils/Database/UserDataDBHelper.hpp"

using namespace Command::CovenantCmd;
using namespace Utils::Database;

#define MSG_LEFT_COV            0
#define MSG_USER_LEADER         1
#define MSG_LEFT_AND_NEW_LEADER 2
#define MSG_SPECIFIE_NEW_LEADER 3
#define MSG_NOT_IN_COVENANT     4

///////////////////////////////////////////////////////////////////////////

LeaveCovenantCommand::LeaveCovenantCommand()
{
    Initialize();
}

///////////////////////////////////////////////////////////////////////////

void LeaveCovenantCommand::Initialize()
{
    _name = "!cov_leave";
    _answers.push_back("You left covenant! Now you are just a viewer, @");
    _answers.push_back("You are leader of covenant, @! Please provide name of the new leader.");
    _answers.push_back("You left covenant, @! New leader of 'COV_NAME' is LEADER_NAME.");
    _answers.push_back("Specified user is not member of your covenant!");
    _answers.push_back("You are not in any covenant yet, @.");
}

///////////////////////////////////////////////////////////////////////////

void LeaveCovenantCommand::_GetAnswer(const ChatMessage &message, ChatAnswer &answer)
{
    QString covenant = UserDataDBHelper::GetUserParameter(UserDataParameter::Covenant, message.GetUserID()).toString();
    if (covenant != "Viewer")
    {
        // Check if user is leader of its covenant
        DB_QUERY_PTR query = DB_SELECT("Covenants", "Leader", QString("Name = '%1'").arg(covenant));
        if (query->exec())
        {
            query->first();
            // If user is leader, than try to find specified name of new leader
            if (query->value("Leader").toString() == message.GetRealName())
            {
                int startOfName = message.GetMessage().indexOf(" ");
                if (startOfName > 0)
                {
                    // Try to get name of new leader
                    QString newLeader = message.GetMessage().right(message.GetMessage().size() - startOfName - 1);
                    // If leader name was found, try to use it
                    if (!newLeader.isEmpty())
                    {
                        // Check if user provided his own name
                        if ((newLeader == message.GetRealName()) || (newLeader == message.GetAuthor()))
                        {
                            answer.AddAnswer(_answers.at(MSG_USER_LEADER));
                        }
                        // If user provided different name
                        if (answer.GetAnswers().isEmpty())
                        {
                            if (_SetNewLeaderToCovenant(newLeader, message.GetUserID(), covenant))
                            {
                                answer.AddAnswer(_answers.at(MSG_LEFT_AND_NEW_LEADER));
                                (*answer.GetAnswers().begin()).replace("COV_NAME", covenant);
                                (*answer.GetAnswers().begin()).replace("LEADER_NAME", newLeader);
                            }
                            // Notify that user is not member of covenant
                            else
                            {
                                answer.AddAnswer(_answers.at(MSG_SPECIFIE_NEW_LEADER));
                            }
                        }
                    }
                    // Notify that name was not specified
                    else
                    {
                        answer.AddAnswer(_answers.at(MSG_USER_LEADER));
                    }
                }
                // If message do not contain ampersand, so that means user do not provide name of new leader
                else
                {
                    answer.AddAnswer(_answers.at(MSG_USER_LEADER));
                }
            }
            // If user is not the leader of covenant, that just change its state
            else
            {
                UserDataDBHelper::UpdateUserParameter(UserDataParameter::Covenant, "Viewer", message.GetUserID());
                answer.AddAnswer(_answers.at(MSG_LEFT_COV));
            }
        }
    }
    // If user is not in any covenant
    else
    {
        answer.AddAnswer(_answers.at(MSG_NOT_IN_COVENANT));
    }
}

///////////////////////////////////////////////////////////////////////////

void LeaveCovenantCommand::_GetRandomAnswer(const ChatMessage &message, ChatAnswer &answer)
{
    Q_UNUSED(message);
    Q_UNUSED(answer);
}

///////////////////////////////////////////////////////////////////////////

bool LeaveCovenantCommand::_SetNewLeaderToCovenant(const QString &newLeader, int oldLeaderID, const QString &covName)
{
    bool result(false);
    // Check if provided user is member of covenant
    if (UserDataDBHelper::IsUserInCovenant(covName, newLeader))
    {
        // Change covenant of user
        UserDataDBHelper::UpdateUserParameter(UserDataParameter::Covenant, "Viewer", oldLeaderID);
        // TODO: When CovenantDBHelper will be implemented, replace this direct database calls
        DB_QUERY_PTR newLeaderQuery = DB_SELECT("UserData", "Name, Covenant", QString("Name='%1' OR Author='%1'").arg(newLeader));
        if (newLeaderQuery != nullptr)
        {
            if (newLeaderQuery->first())
            {
                // Update leader name of covenant
                result = DB_UPDATE("Covenants", QString("Leader = '%1'").arg(newLeaderQuery->value("Name").toString()), QString("Name = '%1'").arg(covName));
            }
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////
