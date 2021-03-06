/*************************************************************************
***************  CinderBot - standalone bot for Twitch.tv ****************
******** Copyright (C) 2017  Ilya Lobanov (exanimoteam@gmail.com) ********
********         Check full copyright header in main.cpp          ********
**************************************************************************/
#pragma once
#include "AI/ChatCommands/CovenantCommands/CovenantBaseCommand.hpp"

/*!
 * \brief Contains all chat command things.
 */
namespace Command
{
/*!
 * \brief All covenant related commands.
 */
namespace CovenantCmd
{

/*!
 * class LeaveCovenantCommand
 * Handles user's leaving from covenant
 */
class LeaveCovenantCommand : public CovenantBaseCommand
{
public:
    LeaveCovenantCommand();

protected:
    ////////////////////////////////
    /// BaseChatCommand overrides
    void Initialize();
    void _GetAnswer(const ChatMessage &message, ChatAnswer &answer);
    void _GetRandomAnswer(const ChatMessage &message, ChatAnswer &answer);

private:
    /*!
     * \brief Try to set new leader to specified covenant and make old leader just a viewer
     * \param newLeader - name of new leader
     * \param oldLeaderID - id of old leader
     * \param covName - covenant name
     * \return True if new leader was set, false of specified user is not member of covenant or something faild
     */
    bool _SetNewLeaderToCovenant(const QString &newLeader, int oldLeaderID, const QString &covName);
};

}
}
