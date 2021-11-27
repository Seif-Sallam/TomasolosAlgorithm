#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "ImGuiFileDialog.h"
#include <SFML/Graphics.hpp>
#include "ReservationStation.h"
#include "Controller.h"
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
enum class Windows
{
    Memory = 0,
    RegisterFile,
    Instructions,
    RenderWindow,
    ReservationStations
};

class Application
{
public:
    Application(sf::Vector2u windowsSize, const std::string &windowTitle);
    void Run();
    ~Application();

private:
    void Update();
    void HandleEvents();
    void CleanUp();
    void Render();
    void RenderElements();

    void SetupDockingSpace();
    void ReservationStationsLayer();
    void MemoryImGuiLayer();
    void RegisterFileImGuiLayer();
    void RenderWindowImGuiLayer();
    void InstructionsImGuiLayer();
    void InstructionExecutationLayer();

    void LoadInstructionsFile();

    int LoadData(const std::string &inFileName);

private:
    sf::RenderWindow *m_Window;
    sf::RenderTexture *m_RenderTexture;
    std::vector<ReservationStation> m_Stations;
    std::map<uint16_t, uint16_t> m_Memory;
    Windows m_ActiveWindow;

    std::string m_RegisterFileNames[8];
    int16_t m_RegisterFileData[8];
    sf::View m_View;
    ImVec2 m_WindowSize;

    std::vector<Instruction> m_InstructionsQueue;
    int m_Top;
    sf::Clock deltaClock;
};