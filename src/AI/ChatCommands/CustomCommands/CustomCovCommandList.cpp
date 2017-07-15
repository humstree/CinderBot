/*************************************************************************
***************  CinderBot - standalone bot for Twitch.tv ****************
******** Copyright (C) 2017  Ilya Lobanov (exanimoteam@gmail.com) ********
********         Check full copyright header in main.cpp          ********
**************************************************************************/
#include "CustomCovCommandList.hpp"
#include "AI/ChatCommands/CustomCommands/CustomCovChatCommand.hpp"
#include "Utils/DatabaseManager.hpp"

using namespace Command::CustomChatCmd;

///////////////////////////////////////////////////////////////////////////

CustomCovCommandList::CustomCovCommandList()
{
    _commandTableName = "CustomCovCommands";
    _commandAnswersTableName = "CustomCovCommandAnswers";
}

///////////////////////////////////////////////////////////////////////////

void CustomCovCommandList::_InitializeCommands()
{
    // Clear all commands that was already created
    for (int i = 0; i < _commands.size(); ++i)
    {
        delete _commands[i];
    }
    _commands.clear();

    DB_QUERY_PTR query = DB_SELECT(_commandTableName, "Name");
    if (query != NULL)
    {
        while (query->next())
        {
            CustomChatCommand* chatCommand = new CustomCovChatCommand();
            chatCommand->InitializeByName(query->value("Name").toString());
            _commands.push_back(chatCommand);
        }
    }
}

///////////////////////////////////////////////////////////////////////////
