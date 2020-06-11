#include "World.h"

int main()
{

    sf::RenderWindow window(sf::VideoMode(1800, 1000), "yobaSolarSystem");
    window.setFramerateLimit(200);
    sf::View view;
    World world(&window, &view);

    sf::FloatRect viewPort = { 0, 0, 1920, 1080 };
    view.reset(viewPort);
    vec2 center = { viewPort.width / 2, viewPort.height / 2 };

    ImGui::SFML::Init(window);

    world.add(CelestialObject(vec2(0, 0), vec2(0, 0), 180, 2000));
    world.add(CelestialObject(vec2(800, 800), vec2(2.f, -3), 100, 100));

    sf::Clock deltaClock;
    sf::Clock MainClock;
    double Time = 0.0;
    const double DeltaTime = 0.01;

    double CurrentTime = MainClock.getElapsedTime().asSeconds();
    double Accumulator = 0.0;
    world.biggestObject = world.objectsArray[0];
    while (window.isOpen())
    {
        double NewTime = MainClock.getElapsedTime().asSeconds();
        double FrameTime = NewTime - CurrentTime;
        CurrentTime = NewTime;
        Accumulator += FrameTime;

        sf::Event event{};
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::EventType::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Z)
                {
                    viewPort.width *= 0.8;
                    viewPort.height *= 0.8;
                }
                if (event.key.code == sf::Keyboard::X)
                {
                    viewPort.width *= 1.2;
                    viewPort.height *= 1.2;
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            center.y -= 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            center.x -= 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            center.y += 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            center.x += 10;

        ImGui::SFML::Update(window, deltaClock.restart());
        world.Debug();
        while (Accumulator >= DeltaTime)
        {
            //world.accelerationObjects(1.f);
            world.updateAcceleration(1.);
            world.moveObjects();
            Accumulator -= DeltaTime;
            Time += DeltaTime;
        }
        view.reset(viewPort);
        auto c = world.Center();
        view.setCenter(((c == sf::Vector2f{ 0,0 }) ? center : c));
        window.setView(view);
        window.clear();
        world.DrawingAllObjects(window);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}