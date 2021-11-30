#include "../headers/Application.h"
Application::Application(sf::Vector2u windowSize, const std::string &windowTitle)
    : m_ActiveWindow(Windows::RenderWindow)
{
    m_WindowSize = ImVec2(windowSize.x, windowSize.y);
    m_Window = new sf::RenderWindow(sf::VideoMode(windowSize.x, windowSize.y), windowTitle);
    m_Window->setFramerateLimit(60);
    ImGui::SFML::Init(*m_Window);

    for (int i = 0; i < 8; i++)
    {
        m_RegisterFileData[i] = 0;
        m_RegisterFileNames[i] = "R" + std::to_string(i);
    }

    m_RenderTexture = new sf::RenderTexture;
    m_RenderTexture->setSmooth(true);
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    m_Stations.push_back(ReservationStation("LOAD", &m_Top, Unit::LW));
    m_Stations.push_back(ReservationStation("LOAD", &m_Top, Unit::LW));
    m_Stations.push_back(ReservationStation("STORE", &m_Top, Unit::SW));
    m_Stations.push_back(ReservationStation("STORE", &m_Top, Unit::SW));
    m_Stations.push_back(ReservationStation("BEQ", &m_Top, Unit::BEQ));
    m_Stations.push_back(ReservationStation("JAL/JALR", &m_Top, Unit::JAL));
    m_Stations.push_back(ReservationStation("ADD/ADDI", &m_Top, Unit::ADD));
    m_Stations.push_back(ReservationStation("ADD/ADDI", &m_Top, Unit::ADD));
    m_Stations.push_back(ReservationStation("ADD/ADDI", &m_Top, Unit::ADD));
    m_Stations.push_back(ReservationStation("NEG", &m_Top, Unit::NEG));
    m_Stations.push_back(ReservationStation("ABS", &m_Top, Unit::ABS));
    m_Stations.push_back(ReservationStation("DIV", &m_Top, Unit::DIV));
    m_Controller = new Controller(m_Top, m_InstructionsQueue, m_Stations, m_RegFile, m_Memory);

    PC = 0;
}

void Application::Run()
{
    while (m_Window->isOpen())
    {
        HandleEvents();

        Update();

        Render();
    }
    CleanUp();
}

void Application::Update()
{

    if (m_ActiveWindow == Windows::RenderWindow)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            m_View.move(sf::Vector2f(-10.0f, 0.0f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            m_View.move(sf::Vector2f(10.0f, 0.0f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            m_View.move(sf::Vector2f(0.0f, -10.0f));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            m_View.move(sf::Vector2f(0.0f, 10.0f));
    }
    ImGui::SFML::Update(*m_Window, deltaClock.restart());
    SetupDockingSpace();
    {
        ReservationStationsLayer();
        MemoryImGuiLayer();
        RegisterFileImGuiLayer();
        RenderWindowImGuiLayer();
        InstructionsImGuiLayer();
        InstructionExecutationLayer();
    }
    ImGui::End(); // Docking space end
}

void Application::HandleEvents()
{
    sf::Event event;
    while (m_Window->pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(event);
        if (event.type == sf::Event::Closed)
        {
            m_Window->close();
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            m_Window->close();
        }
    }
}

Application::~Application()
{
    CleanUp();
}

void Application::Render()
{
    m_Window->clear();
    m_RenderTexture->clear();

    RenderElements();
    m_RenderTexture->setView(m_View);
    ImGui::SFML::Render(*m_Window);
    m_RenderTexture->display();

    m_Window->display();
}

void Application::RenderElements()
{
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    m_RenderTexture->draw(shape);
}

void Application::CleanUp()
{
    if (m_Window != nullptr)
        ImGui::SFML::Shutdown();
    if (m_RenderTexture != nullptr)
        delete m_RenderTexture;
    if (m_Window != nullptr)
        delete m_Window;
    if (m_Controller != nullptr)
        delete m_Controller;
    m_Controller = nullptr;
    m_RenderTexture = nullptr;
    m_Window = nullptr;
}

void Application::SetupDockingSpace()
{
    int windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_Window->getSize().x, m_Window->getSize().y));
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    static bool flag = true;
    ImGui::Begin("Docking Space", &flag, windowFlags);
    // ImGui::PopStyleVar();
    ImGui::BeginMenuBar();
    LoadInstructionsFile();
    ImGui::EndMenuBar();
    ImGui::DockSpace(ImGui::GetID("Docking Space"));
}

void Application::ReservationStationsLayer()
{
    ImGui::Begin("Reservation Stations");
    if (ImGui::IsWindowFocused())
        m_ActiveWindow = Windows::ReservationStations;
    const char *items[] = {"LW", "SW", "BEQ", "JAL/JALR", "ADD/ADDI", "NEG", "ABS", "DIV"};
    static const char *currentItem = NULL;
    if (ImGui::BeginCombo("##COMBO", currentItem))
    {
        for (int i = 0; i < sizeof(items) / sizeof(char *); i++)
        {
            bool isSelected = (items[i] == currentItem);
            if (ImGui::Selectable(items[i], isSelected))
            {
                currentItem = items[i];
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Station"))
    {
        if (currentItem != "")
        {
            if (currentItem == "LW")
            {
                m_Stations.push_back(ReservationStation("LOAD", &m_Top, Unit::LW));
            }
            else if (currentItem == "SW")
            {
                m_Stations.push_back(ReservationStation("STORE", &m_Top, Unit::SW));
            }
            else if (currentItem == "BEQ")
            {
                m_Stations.push_back(ReservationStation("BEQ", &m_Top, Unit::BEQ));
            }
            else if (currentItem == "JAL/JALR")
            {
                m_Stations.push_back(ReservationStation("JAL/JALR", &m_Top, Unit::JAL));
            }
            else if (currentItem == "ADD/ADDI")
            {
                m_Stations.push_back(ReservationStation("ADD/ADDI", &m_Top, Unit::ADD));
            }
            else if (currentItem == "NEG")
            {
                m_Stations.push_back(ReservationStation("NEG", &m_Top, Unit::NEG));
            }
            else if (currentItem == "ABS")
            {
                m_Stations.push_back(ReservationStation("ABS", &m_Top, Unit::ABS));
            }
            else if (currentItem == "DIV")
            {
                m_Stations.push_back(ReservationStation("DIV", &m_Top, Unit::DIV));
            }
        }
        // Sort();
        std::sort(m_Stations.begin(), m_Stations.end());
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete all"))
    {
        for (int i = 0; i < int(Unit::UNIT_COUNT); i++)
            ReservationStation::stationsCount[i] = 0;
        m_Stations.clear();
    }
    ImGui::BeginChild("Stations Table");
    ImGui::Separator();
    ImGui::Columns(8);
    {
        ImGui::Text("Station Name");
        ImGui::NextColumn();
        ImGui::Text("Busy?");
        ImGui::NextColumn();

        ImGui::Text("Vj");
        ImGui::NextColumn();
        ImGui::Text("Vk");
        ImGui::NextColumn();
        ImGui::Text("Qj");
        ImGui::NextColumn();
        ImGui::Text("Qk");
        ImGui::NextColumn();
        ImGui::Text("A");
    }
    ImGui::Columns(1);
    ImGui::Separator();
    for (int i = 0; i < m_Stations.size(); i++)
    {
        m_Stations[i].ImGuiLayer();
        ImGui::NextColumn();
        ImGui::PushID(i);
        if (ImGui::Button("X"))
        {
            m_Stations.erase(m_Stations.begin() + i);
        }
        ImGui::PopID();
        ImGui::Columns(1);
        ImGui::Separator();
    }
    ImGui::EndChild();
    ImGui::End();
}

void Application::MemoryImGuiLayer()
{
    ImGui::Begin("Memory");
    if (ImGui::IsWindowFocused())
        m_ActiveWindow = Windows::Memory;

    ImGui::BeginChild("Table Mem");
    {
        static int addr = 0;
        ImGui::Text("Enter the address you want to watch");
        ImGui::Columns(2);
        ImGui::Text("Address:");
        ImGui::SameLine();
        ImGui::InputScalar("", ImGuiDataType_U16, &addr);
        ImGui::NextColumn();
        if (ImGui::Button("Add to watch"))
        {
            if (m_Memory.find(addr) == m_Memory.end())
            {
                m_Memory[addr] = 0;
            }
        }
        ImGui::Columns(1);
    }

    ImGui::Columns(2, "Memory Table");
    ImGui::Separator();
    ImGui::Text("Address");
    ImGui::NextColumn();
    ImGui::Text("Value");
    ImGui::NextColumn();
    ImGui::Separator();
    for (auto &element : m_Memory)
    {
        ImGui::Text("%d", element.first);
        ImGui::NextColumn();
        ImGui::Text("%d", element.second);
        ImGui::NextColumn();
        ImGui::Separator();
    }
    ImGui::Columns(1);

    ImGui::EndChild();
    ImGui::End();
}

void Application::RegisterFileImGuiLayer()
{
    ImGui::Begin("Register File");
    if (ImGui::IsWindowFocused())
        m_ActiveWindow = Windows::RegisterFile;
    ImGui::Columns(3, "Table");
    ImGui::Separator();
    ImGui::Text("Register");
    ImGui::NextColumn();
    ImGui::Text("Value");
    ImGui::NextColumn();
    ImGui::Text("Producing Unit");
    ImGui::NextColumn();
    ImGui::Separator();
    for (int i = 0; i < 8; i++)
    {
        ImGui::Text("%s", m_RegFile.m_RegisterName[i].c_str());
        ImGui::NextColumn();
        ImGui::Text("%d", m_RegFile.m_RegisterValue[i]);
        ImGui::NextColumn();
        ImGui::Text("%s", m_RegFile.m_ProducingUnit[i].c_str());
        ImGui::NextColumn();
        ImGui::Separator();
    }
    ImGui::Columns(1);

    ImGui::Text("PC: %d", m_Top + PC);
    ImGui::Text("Cycle Number: %d", m_Controller->GetCycleNumber());
    if (ImGui::Button("Click me to Advance"))
        m_Controller->Advance();
    if (ImGui::Button("Reset"))
        Reset();

    ImGui::Separator();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
    ImGui::TextUnformatted("Changes the number of cycles for each instruction and an extra cycle for JAL, JALR, and BEQ instructions to compute the address, and 2 extra cycles for LW and SW to read/write from the memory");

    ImGui::PopTextWrapPos();
    for (int i = 0; i < int32_t(Unit::UNIT_COUNT); i++)
        ImGui::SliderInt(s_UnitName[i].c_str(), &s_CyclesCount[i], 1, 20);

    ImGui::Text("Saving will reset the simulation");
    if (ImGui::Button("Save Data"))
    {
        for (int i = 0; i < m_InstructionsQueue.size(); i++)
            m_InstructionsQueue[i].UpdateCycleCount();
        Reset();
    }
    ImGui::End();
}

void Application::RenderWindowImGuiLayer()
{
    // ImGui::Begin("GameWindow");
    // if (ImGui::IsWindowFocused())
    // {
    //     m_ActiveWindow = Windows::RenderWindow;
    // }
    // ImGui::BeginChild("GameRenderer");
    // sf::Vector2f size = sf::Vector2f(m_Window->getSize().x, m_Window->getSize().y);
    // m_RenderTexture->create(m_Window->getSize().x, m_Window->getSize().y);

    // m_WindowSize = ImGui::GetWindowSize();
    // ImGui::Image(*m_RenderTexture, sf::Vector2f(m_WindowSize.x, m_WindowSize.y));
    // m_View.setSize(m_WindowSize.x * 2, m_WindowSize.y * 2);
    // ImGui::EndChild();
    // ImGui::End();
}

void Application::LoadInstructionsFile()
{
    if (ImGui::Button("Load Instruction"))

        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "ChooseFile", ".txt", ".");
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            int err = LoadData(filePathName);
            if (err)
            {
                std::cout << "Error file was not read correctly\n";
            }
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void Application::InstructionsImGuiLayer()
{
    ImGui::Begin("Instructions Queue");
    if (ImGui::IsWindowFocused())
    {
        m_ActiveWindow = Windows::Instructions;
    }
    ImGui::BeginChild("InstructionTable");
    ImGui::BeginChild("Instruction");
    {
        ImGui::Separator();
        ImGui::Columns(7);
        ImGui::Text("Instruction");
        ImGui::NextColumn();
        ImGui::Text("Type");
        ImGui::NextColumn();
        ImGui::Text("Rs1");
        ImGui::NextColumn();
        ImGui::Text("Rs2");
        ImGui::NextColumn();
        ImGui::Text("Rd");
        ImGui::NextColumn();
        ImGui::Text("Imm");
        ImGui::NextColumn();
        ImGui::Text("Top");
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::EndChild();

        for (int i = 0; i < m_InstructionsQueue.size(); i++)
        {
            m_InstructionsQueue[i].ImGuiLayer(i == m_Top);
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void Application::InstructionExecutationLayer()
{
    ImGui::Begin("Instruction Executation");
    {
        ImGui::Text("Logging to a imgui_log.txt");
        ImGui::LogButtons();

        ImGui::BeginChild("Tabular");
        {
            ImGui::Columns(4);
            ImGui::Text("Instruction");
            ImGui::NextColumn();
            ImGui::Text("Issue");
            ImGui::NextColumn();
            ImGui::Text("Execute");
            ImGui::NextColumn();
            ImGui::Text("WriteBack");
            ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Columns(4);
            for (int i = 0; i < m_InstructionsQueue.size(); i++)
            {
                ImGui::Text(m_InstructionsQueue[i].str.c_str(), "");
                ImGui::NextColumn();
                if (m_InstructionsQueue[i].issue.first)
                    ImGui::Text("Y(%d)", m_InstructionsQueue[i].issue.second);
                else
                    ImGui::Text("N");

                ImGui::NextColumn();
                if (m_InstructionsQueue[i].execute.first)
                    ImGui::Text("Y(%d)", m_InstructionsQueue[i].execute.second);
                else
                    ImGui::Text("N");

                ImGui::NextColumn();
                if (m_InstructionsQueue[i].writeBack.first)
                    ImGui::Text("Y(%d)", m_InstructionsQueue[i].writeBack.second);
                else
                    ImGui::Text("N");
                ImGui::NextColumn();
                ImGui::Separator();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

int Application::LoadData(const std::string &inFileName)
{
    std::ifstream inputFile(inFileName);
    if (!inputFile.is_open())
        return 1;

    m_InstructionsQueue.clear();
    m_Top = 0;
    while (!inputFile.eof())
    {
        std::string instruction;
        std::getline(inputFile, instruction);
        if (instruction.size() > 3)
            m_InstructionsQueue.push_back(Instruction(instruction));
    }
    inputFile.close();
    return 0;
}

void Application::Reset()
{
    m_Controller->GetCycleNumber() = 0;
    m_Top = 0;
    for (int i = 0; i < m_InstructionsQueue.size(); i++)
    {
        m_InstructionsQueue[i].Clean();
    }
    for (int i = 0; i < m_Stations.size(); i++)
    {
        m_Stations[i].Clean();
    }
    m_Controller->Clean();
    for (int i = 0; i < 8; i++)
    {
        m_RegFile.m_ProducingUnit[i] = "N";
    }
}