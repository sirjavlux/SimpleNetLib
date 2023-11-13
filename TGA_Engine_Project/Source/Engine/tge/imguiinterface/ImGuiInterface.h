#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
namespace Tga
{
	class ImGuiInterfaceImpl;
	class ImGuiInterface
	{
	public:
		ImGuiInterface();
		~ImGuiInterface();
		void Init();
		void PreFrame();
		void Render();

	private:
		std::unique_ptr<ImGuiInterfaceImpl> myImpl;
	};


}
