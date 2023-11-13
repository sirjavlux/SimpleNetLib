#include "stdafx.h"
#include "AbstractCommand.h"
#include "CommandManager.h"

#include <stack>

using namespace Tga;

static std::stack<std::shared_ptr<AbstractCommand>> locUndoStack;
static std::stack<std::shared_ptr<AbstractCommand>> locRedoStack;

void CommandManager::Clear()
{
	while (!locUndoStack.empty())
		locUndoStack.pop();
	while (!locRedoStack.empty())
		locRedoStack.pop();
}

void CommandManager::DoCommand(const std::shared_ptr<AbstractCommand>& aCommand) 
{
	aCommand->Execute();
	locUndoStack.push(aCommand);
	while (locRedoStack.empty() == false)
	{
		locRedoStack.pop();
	}
}

void CommandManager::Undo() 
{
	if (locUndoStack.empty() == false)
	{
		locUndoStack.top()->Undo();
		locRedoStack.push(locUndoStack.top());
		locUndoStack.pop();
	}
}

void CommandManager::Redo() 
{
	if (locRedoStack.empty() == false)
	{
		locRedoStack.top()->Execute();
		locUndoStack.push(locRedoStack.top());
		locRedoStack.pop();
	}
}

bool CommandManager::CanUndo()
{
	return !locUndoStack.empty();
}

bool CommandManager::CanRedo()
{
	return !locRedoStack.empty();
}