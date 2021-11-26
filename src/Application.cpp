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

    m_Stations.push_back(ReservationStation("Load1", Unit::LW));
    m_Stations.push_back(ReservationStation("Load2", Unit::LW));
    m_Stations.push_back(ReservationStation("BEQ", Unit::BEQ));
    m_Stations.push_back(ReservationStation("DIV", Unit::DIV));
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
    ImGui::SFML::Shutdown();
    if (m_RenderTexture != nullptr)
        delete m_RenderTexture;
    if (m_Window != nullptr)
        delete m_Window;
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

    ImGui::DockSpace(ImGui::GetID("Docking Space"));
}

void Application::ReservationStationsLayer()
{
    ImGui::Begin("Reservation Stations");
    if (ImGui::IsWindowFocused())
    {
        m_ActiveWindow = Windows::ReservationStations;
    }
    ImGui::BeginChild("Stations Table");
    ImGui::Separator();
    ImGui::Columns(8);
    {
        ImGui::Text("Station Name");
        ImGui::NextColumn();
        ImGui::Text("Busy?");
        ImGui::NextColumn();
        ImGui::Text("Vi");
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
    for (auto &station : m_Stations)
    {
        station.ImGuiLayer();
        ImGui::Separator();
    }
    ImGui::EndChild();
    ImGui::End();
}

void Application::MemoryImGuiLayer()
{
    ImGui::Begin("Memory");
    if (ImGui::IsWindowFocused())
    {
        m_ActiveWindow = Windows::Memory;
    }
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
    {
        m_ActiveWindow = Windows::RegisterFile;
    }
    ImGui::Columns(2, "Table");
    ImGui::Separator();
    ImGui::Text("Register");
    ImGui::NextColumn();
    ImGui::Text("Value");
    ImGui::NextColumn();
    ImGui::Separator();
    for (int i = 0; i < 8; i++)
    {
        ImGui::Text("%s", m_RegisterFileNames[i].c_str());
        ImGui::NextColumn();
        ImGui::Text("%d", m_RegisterFileData[i]);
        ImGui::NextColumn();
        ImGui::Separator();
    }

    ImGui::Columns(1);

    ImGui::End();
}

void Application::RenderWindowImGuiLayer()
{
    ImGui::Begin("GameWindow");
    if (ImGui::IsWindowFocused())
    {
        m_ActiveWindow = Windows::RenderWindow;
    }
    ImGui::BeginChild("GameRenderer");
    sf::Vector2f size = sf::Vector2f(m_Window->getSize().x, m_Window->getSize().y);
    m_RenderTexture->create(m_Window->getSize().x, m_Window->getSize().y);

    m_WindowSize = ImGui::GetWindowSize();
    ImGui::Image(*m_RenderTexture, sf::Vector2f(m_WindowSize.x, m_WindowSize.y));
    m_View.setSize(m_WindowSize.x * 2, m_WindowSize.y * 2);
    ImGui::EndChild();
    ImGui::End();
}

void Application::InstructionsImGuiLayer()
{
    ImGui::Begin("Instructions Queue");
    if (ImGui::IsWindowFocused())
    {
        m_ActiveWindow = Windows::Instructions;
    }
    ImGui::End();
}
