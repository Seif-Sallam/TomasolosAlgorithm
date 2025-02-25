#pragma once

#include "imgui.h"
#include "imgui-SFML.h"
#include "ImGuiFileDialog.h"
#include <SFML/Graphics.hpp>
#include "ReservationStation.h"
#include "Controller.h"
#include "RegisterFile.h"
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
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
    Application();
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
    void InstructionsMemoryImGuiLayer();
    void InstructionsQueueImGuiLayer();
    void InstructionExecutionLayer();

    void Reset();
    void LoadInstructionsFile(bool &);

    int LoadData(const std::string &inFileName);
    void HelpMarker(const char *desc);
    void LogToFile(bool removeFlushed);

private:
    std::map<uint16_t, int16_t> m_Memory;
    sf::RenderWindow *m_Window;
    sf::RenderTexture *m_RenderTexture;
    std::vector<ReservationStation> m_Stations;
    Windows m_ActiveWindow;

    std::string m_RegisterFileNames[8];
    int16_t m_RegisterFileData[8];
    sf::View m_View;
    ImVec2 m_WindowSize;

    std::vector<Instruction> m_InstructionsQueue;
    std::vector<Instruction> m_InstructionMemory;
    int m_Top;
    sf::Clock deltaClock;
    RegisterFile m_RegFile;
    Controller *m_Controller;
    int32_t PC;
    bool m_Fullscreen = false;
    double IPC = 0;
    double branchMisPri = 0;
    bool m_Started = false;
};