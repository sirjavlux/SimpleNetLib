#pragma once

#include <memory>

namespace Tga
{
	class AbstractCommand;

	class CommandManager
	{
	public:
		static void Clear();
		static void DoCommand(const std::shared_ptr<AbstractCommand>& aCommand);
		static void Undo();
		static void Redo();
		static bool CanUndo();
		static bool CanRedo();
	};
}