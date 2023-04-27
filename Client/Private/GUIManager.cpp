#include "stdafx.h"
#include "..\Public\GUIManager.h"

IMPLEMENT_SINGLETON(CGUIManager)

CGUIManager::CGUIManager()
{
}

void CGUIManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;
	io.ConfigViewportsNoDefaultParent = true;
	io.ConfigDockingAlwaysTabBar = true;
	io.ConfigDockingTransparentPayload = true;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

}

void CGUIManager::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CGUIManager::RenderDrawData()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CGUIManager::Render()
{
	D3D11_VIEWPORT ViewPort;
	UINT nViewPorts = 1;
	
	m_pContext->RSGetViewports(&nViewPorts, &ViewPort);

	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(210, 20));
		ImGui::Begin("Frame", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::SameLine();
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	//ImGui::ShowDemoWindow();

	ImGui::Render();
}

void CGUIManager::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGUIManager::DestroyInstance();
}

void CGUIManager::Free()
{
}
