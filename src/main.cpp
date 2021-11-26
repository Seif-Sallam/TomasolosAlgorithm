// #include <iostream>
// #include "../headers/ImGuiFunctions.h"

// int main()
// {
//     std::map<unsigned short, unsigned short> memory;
//     std::string registerNames[8];
//     ImVec2 windowSize = {640, 480};
//     short registerData[8];
//     sf::RenderWindow window(sf::VideoMode(640, 480), "Simulation :)");
//     window.setFramerateLimit(60);
//     ImGui::SFML::Init(window);
//     for (int i = 0; i < 8; i++)
//     {
//         registerNames[i] = "R" + std::to_string(i);
//         registerData[i] = i;
//     }
//     std::vector<ReservationStation> stations;
//     stations.push_back(ReservationStation("Load1", Unit::LW));
//     stations.push_back(ReservationStation("Load2", Unit::LW));
//     stations.push_back(ReservationStation("BEQ", Unit::BEQ));
//     stations.push_back(ReservationStation("DIV", Unit::DIV));
//     sf::CircleShape shape(100.f);
//     shape.setFillColor(sf::Color::Green);
//     ImGuiIO &io = ImGui::GetIO();
//     io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//     float f = 0.f;
//     sf::Clock deltaClock;
//     sf::View view;
//     sf::RenderTexture texture;
//     while (window.isOpen())
//     {
//         sf::Event event;
//         while (window.pollEvent(event))
//         {
//             ImGui::SFML::ProcessEvent(event);

//             if (event.type == sf::Event::Closed)
//             {
//                 window.close();
//             }
//             if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)
//                 window.close();
//         }
//         if (s_ActiveWindow == Windows::RenderWindow)
//         {
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
//                 view.move(sf::Vector2f(-10.0f, 0.0f));
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
//                 view.move(sf::Vector2f(10.0f, 0.0f));
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
//                 view.move(sf::Vector2f(0.0f, -10.0f));
//             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
//                 view.move(sf::Vector2f(0.0f, 10.0f));
//         }
//         static float f = 0.f;
//         ImGui::SFML::Update(window, deltaClock.restart());
//         {
//             SetupDockingSpace(window);
//             MemoryImGuiLayer(memory);
//             RegisterFileImGuiLayer(registerData, registerNames);
//             InstructionsImGuiLayer();
//             RenderWindowImGuiLayer(window, texture, view, windowSize);
//             ReservationStationsLayer(stations);
//             ImGui::End();
//         }

//         window.clear();
//         texture.clear();
//         texture.setView(view);
//         texture.draw(shape);
//         ImGui::SFML::Render(window);
//         texture.display();
//         window.display();
//     }

//     ImGui::SFML::Shutdown();
// }
#include "../headers/Application.h"
int main()
{
    Application app(sf::Vector2u(640, 480), "Simulation :)");
    app.Run();
}