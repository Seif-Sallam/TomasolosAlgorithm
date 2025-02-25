#include "../headers/Application.h"
Application::Application()
    : m_ActiveWindow(Windows::RenderWindow)
{
    m_WindowSize = ImVec2(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height);
    m_Window = new sf::RenderWindow(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "Tomasolo's Algorithm");
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
    m_InstructionsQueue.reserve(3000);
    m_Controller = new Controller(m_Top, m_InstructionsQueue, m_InstructionMemory, m_Stations, m_RegFile, m_Memory);
    PC = 0;
    if (LoadData("Tests/DefaultProgram.txt"))
    {
        std::cerr << "Couldn't open the default program\n";
    }
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
        InstructionsMemoryImGuiLayer();
        InstructionsQueueImGuiLayer();
        InstructionExecutionLayer();
    }
    ImGui::End(); // Docking space end
    m_Started = true;
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
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F)
        {
            if (!m_Fullscreen)
            {
                m_Window->create(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "Tomasolo's Algorithm", sf::Style::Fullscreen);
                m_Window->setFramerateLimit(60);
                m_Fullscreen = true;
            }
            else
            {
                m_Window->create(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "Tomasolo's Algorithm", sf::Style::Default);
                m_Window->setFramerateLimit(60);
                m_Fullscreen = false;
            }
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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    static bool flag = true;
    ImGui::Begin("Docking Space", &flag, windowFlags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::BeginMenuBar();
    static bool Opened = false;
    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("Load Instructions", "", &Opened);
        if (ImGui::MenuItem("Close Application", "Escape"))
            m_Window->close();
        ImGui::EndMenu();
    }

    if (Opened)
    {
        const char *filters = "Text Files (*.txt){.txt}, Assembly Files (*.asm *.s){.asm,.s}";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", filters, ".");
        LoadInstructionsFile(Opened);
    }
    ImGui::Separator();
    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 460.0f);
    HelpMarker("Setting the PC value will change the value of the Index (Instruction Memory) and the PC (Instruction Queue).\n"
               "It does not affect the performance of the algorithm nor the Execution of the instructions.\n"
               "The Cycle Number starts with -1. It indicates that we did not start the simulation yet.\n");

    ImGui::TextUnformatted("Custom Starting PC Value:");
    ImGui::SameLine();
    ImGui::PushItemWidth(45.0f);
    ImGui::InputInt("", &PC, 0, 0, ImGuiInputTextFlags_AlwaysOverwrite | ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();
    ImGui::Separator();
    ImGui::Text("PC: %d", m_Top + PC);
    ImGui::Separator();
    ImGui::Text("Cycle Number: %d", m_Controller->GetCycleNumber());

    ImGui::EndMenuBar();
    ImGui::DockSpace(ImGui::GetID("Docking Space"));
}

void Application::ReservationStationsLayer()
{
    ImGui::Begin("Reservation Stations");
    if (!m_Started)
        ImGui::SetWindowSize(ImVec2(m_WindowSize.x / 2.0f, m_WindowSize.y / 2.0f));
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
        std::sort(m_Stations.begin(), m_Stations.end());
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete all"))
    {
        for (int i = 0; i < int(Unit::UNIT_COUNT); i++)
            ReservationStation::stationsCount[i] = 0;
        m_Stations.clear();
    }
    ImGui::SameLine();
    HelpMarker("This window shows the different reservation stations that are used in the current simulation.\n"
               "You can add new reservation stations by selecting the one you want to add and then click on Add Station button\n");

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
    // ImGui::BeginMenuBar();
    if (!m_Started)
        ImGui::SetWindowSize(ImVec2(m_WindowSize.x / 2.0f, m_WindowSize.y / 2.0f));
    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 30.0f);
    HelpMarker("The Memory window here is used to observe the different memory addresses accessed by the instructions.\n"
               "You can also add certain addresses to watch OR preload the memory with certain data.");

    // ImGui::EndMenuBar();
    if (ImGui::IsWindowFocused())
        m_ActiveWindow = Windows::Memory;

    ImGui::BeginChild("AddToWatch");
    {
        static int addr = 0;
        ImGui::Text("Address: ");
        ImGui::SameLine();
        ImGui::InputScalar("", ImGuiDataType_U16, &addr);
        if (ImGui::Button("Add to watch"))
        {
            if (m_Memory.find(addr) == m_Memory.end())
            {
                m_Memory[addr] = 0;
            }
        }
        ImGui::Columns(1);
    }
    {
        // ImGui::BeginChild("Add Addresses");
        ImGui::Columns(1);
        static int addr = 0;
        static int value = 0;
        ImGui::Text("Address: ");
        ImGui::SameLine();
        ImGui::InputInt("###0", &addr, 0);
        ImGui::Text("Value:   ");
        ImGui::SameLine();
        ImGui::InputInt("###1", &value, 0);
        if (ImGui::Button("Add Address with Value"))
        {
            m_Memory[addr] = value;
        }
        // ImGui::EndChild();
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
    if (!m_Started)
        ImGui::SetWindowSize(ImVec2(m_WindowSize.x / 2.0f, m_WindowSize.y / 2.0f));
    if (ImGui::IsWindowFocused())
        m_ActiveWindow = Windows::RegisterFile;
    HelpMarker("The Register File window shows the 8 registers in our simulation and the producing unit of each register at any point in time.\n"
               "The producing unit will be initialized with \"N\" as in NO_UNIT, and the values will start with 0 all the time.\n");

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
        ImGui::Text("%s", m_RegFile.m_ProducingUnit[i].front().c_str());
        ImGui::NextColumn();
        ImGui::Separator();
    }
    ImGui::Columns(1);
    HelpMarker("The Next Cycle Button is used to advance in the cycles of the simulation.\n"
               "Reset button resets the state of the simulation but NOT the register file VALUES, the register file producing units will only be cleared."
               "To clear the producing units and the register file values (NOT RECOMMENDED WITHOUT RESETING), you can click Reset Register File values");
    ImGui::SameLine();
    if (ImGui::Button("Next Cycle", ImVec2(ImGui::GetWindowWidth() - 30.f, 22.0f)))
        m_Controller->Advance();

    if (ImGui::Button("Reset", ImVec2(ImGui::GetWindowWidth() / 3.0f, 22.0f)))
        Reset();

    ImGui::SameLine();
    if (ImGui::Button("Reset Register File", ImVec2(ImGui::GetWindowWidth() * 2.0f / 3.0f, 22.0f)))
    {
        for (int i = 0; i < 8; i++)
        {
            m_RegFile.m_RegisterValue[i] = 0;
            while (m_RegFile.m_ProducingUnit[i].front() != "N")
                m_RegFile.m_ProducingUnit[i].pop_front();
        }
    }
    ImGui::Separator();

    ImGui::PushTextWrapPos(ImGui::GetWindowWidth());
    ImGui::TextUnformatted("Changes the number of cycles for each instruction and an extra cycle for JAL, JALR, and BEQ instructions to compute the address, and 2 extra cycles for LW and SW to read/write from the memory");

    ImGui::PopTextWrapPos();

    for (int i = 0; i < int32_t(Unit::UNIT_COUNT) - 1; i++)
        ImGui::SliderInt(InstructionsUnitCycles::s_UnitName[i].c_str(), &InstructionsUnitCycles::s_CyclesCount[i], 1, 100);

    ImGui::Text("Saving will reset the simulation");
    if (ImGui::Button("Save Data"))
    {
        for (int i = 0; i < m_InstructionMemory.size(); i++)
        {
            m_InstructionMemory[i].UpdateCycleCount();
        }
        Reset();
    }
    ImGui::End();
}

void Application::LoadInstructionsFile(bool &opened)
{
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            int err = LoadData(filePathName);
            if (err)
            {
                std::cerr << "Error file was not read correctly\n";
            }
        }
        ImGuiFileDialog::Instance()->Close();
        opened = false;
    }
}

void Application::InstructionsMemoryImGuiLayer()
{
    ImGui::Begin("Instructions Memory");
    HelpMarker("Instruction Memory window shows the loaded instructions from the text file and which one is the top."
               "The top instruction is the one about to be issued next cycle.\n");

    if (!m_Started)
        ImGui::SetWindowSize(ImVec2(m_WindowSize.x / 2.0f, m_WindowSize.y / 2.0f));
    if (ImGui::IsWindowFocused())
    {
        m_ActiveWindow = Windows::Instructions;
    }
    ImGui::BeginChild("InstructionTable");
    ImGui::BeginChild("Instruction");
    {
        ImGui::Separator();
        ImGui::Columns(8);
        ImGui::Text("Index");
        ImGui::NextColumn();
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
        for (int i = 0; i < m_InstructionMemory.size(); i++)
        {
            m_InstructionMemory[i].ImGuiLayer(PC, i == m_Top, true);
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void Application::InstructionsQueueImGuiLayer()
{
    ImGui::Begin("Instructions Queue");
    HelpMarker("The instruction Queue just shows the different instructions that got into the Queue (issued already).\n"
               "It is designed this way and not an ever growing queue that is preloaded because of the branch and jump instructions that doesn't help with this implementation."
               "You cannot always tell which instruction will be next to the issuing. That is why we only add it to the Queue iff it was issued.\n");
    if (!m_Started)
        ImGui::SetWindowSize(ImVec2(m_WindowSize.x / 2.0f, m_WindowSize.y / 2.0f));
    if (ImGui::IsWindowFocused())
    {
        m_ActiveWindow = Windows::Instructions;
    }
    ImGui::BeginChild("InstructionTable");

    {
        ImGui::BeginChild("Instruction");

        ImGui::Separator();
        ImGui::Columns(7);
        ImGui::PushItemWidth(20.0f);
        ImGui::Text("PC");
        ImGui::PopItemWidth();
        ImGui::NextColumn();
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
        ImGui::Separator();
        ImGui::EndChild();
        for (int i = 0; i < m_InstructionsQueue.size(); i++)
        {
            m_InstructionsQueue[i].ImGuiLayer(PC, 0, false);
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void Application::InstructionExecutionLayer()
{
    ImGui::Begin("Instruction Execution");
    if (!m_Started)
        ImGui::SetWindowSize(ImVec2(m_WindowSize.x / 2.0f, m_WindowSize.y / 2.0f));
    {
        ImGui::Text("Logging to a \"Green_table.txt\"");
        ImGui::SameLine();
        static bool addFlushed = true;
        ImGui::Checkbox("Add Flushed", &addFlushed);
        ImGui::SameLine();
        if (ImGui::Button("Log to file###1"))
            LogToFile(addFlushed);
        ImGui::SameLine();
        HelpMarker("This window shows the progress of the provided program to the instructions and when it was issues, started Execution and when it finished Execution and when it wrote back.\n"
                   "Click the button Log to file to save the data to the file \"Green_Table.txt\". It will also contain the number of cycles, IPC, and the branch mis-prediction percentage.\n");
        int lastInstructionCycle = m_Controller->GetLastInstructionWrote();
        int numberOfInstructions = m_Controller->GetNumberOfInstructions();
        if (lastInstructionCycle != 0)
            IPC = (double)numberOfInstructions / (double)lastInstructionCycle;
        else
            IPC = 0.0;
        ImGui::Text("IPC: %.1f", IPC);
        int branchInst = m_Controller->GetBranchInstructionsCount();
        branchMisPri = 0.0;
        if (branchInst != 0)
            branchMisPri = (double)m_Controller->GetMisPredictions() / branchInst * 100.0;
        ImGui::Text("Branch Misses Percentage: %.1f ", branchMisPri);
        ImGui::Separator();
        ImGui::BeginChild("Tabular");
        {
            ImGui::Columns(5);
            ImGui::Text("Instruction");
            ImGui::NextColumn();
            ImGui::Text("Issue");
            ImGui::NextColumn();
            ImGui::Text("Execution Start");
            ImGui::NextColumn();
            ImGui::Text("Execution End");
            ImGui::NextColumn();
            ImGui::Text("WriteBack");
            ImGui::NextColumn();
            ImGui::Separator();
            ImGui::Columns(5);
            for (int i = 0; i < m_InstructionsQueue.size(); i++)
            {
                ImGui::Text(m_InstructionsQueue[i].str.c_str(), "");
                ImGui::NextColumn();
                if (!m_InstructionsQueue[i].IsFlushed())
                    if (m_InstructionsQueue[i].issue.first)
                        ImGui::Text("Y(%d)", m_InstructionsQueue[i].issue.second);
                    else
                        ImGui::Text("N");
                else
                {
                    ImGui::TextUnformatted("FLUSHED");
                }
                ImGui::NextColumn();
                if (!m_InstructionsQueue[i].IsFlushed())
                    if (m_InstructionsQueue[i].startExecute.first)
                        ImGui::Text("Y(%d)", m_InstructionsQueue[i].startExecute.second);
                    else
                        ImGui::Text("N");
                else
                {
                    ImGui::TextUnformatted("FLUSHED");
                }
                ImGui::NextColumn();
                if (!m_InstructionsQueue[i].IsFlushed())
                    if (m_InstructionsQueue[i].execute.first)
                        ImGui::Text("Y(%d)", m_InstructionsQueue[i].execute.second);
                    else
                        ImGui::Text("N");
                else
                {
                    ImGui::TextUnformatted("FLUSHED");
                }
                ImGui::NextColumn();
                if (!m_InstructionsQueue[i].IsFlushed())
                    if (m_InstructionsQueue[i].writeBack.first)
                        ImGui::Text("Y(%d)", m_InstructionsQueue[i].writeBack.second);
                    else
                        ImGui::Text("N");
                else
                {
                    ImGui::TextUnformatted("FLUSHED");
                }
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
    m_InstructionMemory.clear();
    m_Top = 0;
    int i = 0;
    while (!inputFile.eof())
    {
        std::string instruction;
        std::getline(inputFile, instruction);

        if (instruction.size() > 3)
        {
            if (instruction[0] == '#')
            {
                continue;
            }
            m_InstructionMemory.emplace_back(instruction, i);
            i++;
        }
        else
            continue;
    }
    inputFile.close();
    return 0;
}

void Application::Reset()
{
    m_Controller->GetCycleNumber() = -1;
    m_Top = 0;
    m_Memory.clear();
    m_InstructionsQueue.clear();
    m_InstructionsQueue.reserve(3000);
    for (int i = 0; i < m_Stations.size(); i++)
    {
        m_Stations[i].Clean();
    }
    m_Controller->Clean();
    for (int i = 0; i < 8; i++)
    {
        while (m_RegFile.m_ProducingUnit[i].front() != "N")
            m_RegFile.m_ProducingUnit[i].pop_front();
    }
}

void Application::HelpMarker(const char *desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void Application::LogToFile(bool addFlushed)
{
    std::ofstream output("Green_Table.txt");
    int lastInstructionCycle = 0;
    if (m_InstructionsQueue.size() > 0)
        lastInstructionCycle = m_InstructionsQueue.back().writeBack.second;
    output << "Number of Cycles: " << m_Controller->GetCycleNumber() << std::endl;
    output << "Last Instruction Cycle: " << lastInstructionCycle << std::endl;
    output << "IPC: " << IPC << std::endl;

    output << "Branch mis-prediction percentage: " << branchMisPri << "%" << std::endl;
    int32_t spaces = 20;
    output << "\nGreen Table:\n";
    output << std::left << std::setfill('-') << std::setw(125) << "";
    output << std::setfill(' ') << std::endl;
    output << std::left << "| " << std::setw(6) << "PC"
           << " | " << std::setw(spaces) << "Instruction"
           << " | " << std::setw(spaces) << "Issue"
           << " | " << std::setw(spaces) << "Execution Start"
           << " | " << std::setw(spaces) << "Execution End"
           << " | " << std::setw(spaces) << "Write back"
           << " |" << std::endl;

    output << std::left << std::setfill('=') << std::setw(125) << "";
    output << std::setfill(' ') << std::endl;

    for (int i = 0; i < m_InstructionsQueue.size(); i++)
    {
        if (m_InstructionsQueue[i].IsFlushed())
        {
            if (!addFlushed)
                continue;
            else
                output << "| " << std::setw(6) << m_InstructionsQueue[i].getPC() + PC << " | " << std::setw(spaces) << m_InstructionsQueue[i].str << " | " << std::setw(spaces) << "FLUSHED"
                       << " | " << std::setw(spaces) << "FLUSHED"
                       << " | " << std::setw(spaces) << "FLUSHED"
                       << " | " << std::setw(spaces) << "FLUSHED"
                       << " |" << std::endl;
        }
        else
            output << "| " << std::setw(6) << m_InstructionsQueue[i].getPC() + PC
                   << " | " << std::setw(spaces) << m_InstructionsQueue[i].str
                   << " | " << std::setw(spaces) << m_InstructionsQueue[i].issue.second << " | "
                   << std::setw(spaces) << m_InstructionsQueue[i].startExecute.second << " | "
                   << std::setw(spaces) << m_InstructionsQueue[i].execute.second << " | "
                   << std::setw(spaces) << m_InstructionsQueue[i].writeBack.second << " |" << std::endl;
    }
    if (m_InstructionsQueue.size() >= 1)
    {
        output << std::left << std::setfill('-') << std::setw(125) << "";
        output << std::setfill(' ') << std::endl;
    }

    output << "\n\nMemory Content: \n\n";
    output << std::left << std::setfill('-') << std::setw(47) << "";
    output << std::setfill(' ') << std::endl;
    output << std::left << "| " << std::setw(spaces) << "Address"
           << " | " << std::setw(spaces) << "Value"
           << " |" << std::endl;
    output << std::left << std::setfill('=') << std::setw(47) << "";
    output << std::setfill(' ') << std::endl;
    for (auto &elm : m_Memory)
    {
        output << std::left << "| " << std::setw(spaces) << elm.first << " | " << std::setw(spaces) << elm.second << " |" << std::endl;
    }
    output << std::left << std::setfill('-') << std::setw(47) << "";
    output << std::setfill(' ') << std::endl;
    output.close();
}