//
// Created by dimka on 6/10/2020.
//

#ifndef SFML_WORLD_H
#define SFML_WORLD_H

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <utility>
#include "CelestialObject.h"

class World
{
    enum debug_state
    {
        AddNewObject = 0,
        DebugWindow,
        SetViewPort
    };
    sf::Texture tex1;
    debug_state state;
    sf::RenderWindow* w{};
    sf::View* view{};
    CelestialObject* centralObj{};
    [[maybe_unused]] float ftStep{1.f}, ftSlice{1.f};
public:
    std::vector<CelestialObject*> objectsArray;
    std::vector<sf::VertexArray> vertex;
    CelestialObject* biggestObject{};

    World() = default;

    World(sf::RenderWindow* window, sf::View* _view) : w(window), view(_view), centralObj(nullptr), state(debug_state::SetViewPort)
    { tex1.loadFromFile("gordon.png"); }

    [[maybe_unused]] void clearVertexArray()
    { vertex.clear(); }

    static float ForceOfGravity(CelestialObject* o1, CelestialObject* o2)
    {

        float distance =
                sqrt(pow(o1->obj.getPosition().x - o2->obj.getPosition().x, 2) +
                     pow(o1->obj.getPosition().y - o2->obj.getPosition().y, 2));
        //return (o1->m * o2->m / std::pow(distance, 4));
        return (o1->m * o2->m / sqrt(distance)) / 9000000;
        // return (o1->m * o2->m / pow(distance,2)) / 60000; //ведет себя менее
        // стабильно Йобу достаточно сложно вывести на орбиту
    }

    void add(CelestialObject object)
    {
        objectsArray.push_back(new CelestialObject(std::move(object)));
    }

    static bool isCollision(CelestialObject* o1, CelestialObject* o2)
    {
        return o1->obj.getGlobalBounds().intersects(o2->obj.getGlobalBounds());
    }

    void setSprite(CelestialObject* obj)
    {
        tex1.setSmooth(true);
        obj->obj.setTexture(&tex1);
        obj->obj.setTextureRect(sf::IntRect(0, 0, 2000, 1955));
        /*if (abs(obj->acceleration.x) > 1.5 || abs(obj->acceleration.y) > 1.5)
        {
            obj->obj.setTexture(&tex2);
            obj->obj.setTextureRect(sf::IntRect(0, 0, 319, 525));
        }
        if (abs(obj->acceleration.x) > 3.6 || abs(obj->acceleration.y) > 2)
        {
            obj->obj.setTexture(&tex3);
            obj->obj.setTextureRect(sf::IntRect(0, 0, 319, 525));
        }*/
    }

    static void ifCollision(CelestialObject* obj1, CelestialObject* obj2)
    {
        vec2 direction2;
        if((obj1->obj.getPosition().x > obj2->obj.getPosition().x))
            direction2.x = 1;
        else
            direction2.x = -1;

        if((obj1->obj.getPosition().y > obj2->obj.getPosition().y))
            direction2.x = 1;
        else
            direction2.x = -1;

        if(obj2->m > obj1->m)
        {
            obj1->addedAcceleration(vec2(0.5, 0.5) * direction2);
            /*(*iter2)->addMass((*iter)->m * 0.5f);
            (*iter2)->addRadius((*iter)->r * massItoJ);
            (*iter)->addRadius(-newR);
            (*iter)->addMass(-newMass);
            delete objectsArray[i];
            objectsArray.erase(objectsArray.begin() + i);*/
        } else
        {
            obj2->addedAcceleration(vec2(0.5, 0.5) * -direction2);
            /*
            (*iter)->addMass(newMass);
            (*iter)->addRadius(newR);
            (*iter2)->addRadius(-newR);
            (*iter2)->addMass(-newMass);
            delete objectsArray[j];
            objectsArray.erase(objectsArray.begin() + j);*/
        }
    }

    sf::Vector2f Center()
    {
        return ((centralObj != nullptr) ? centralObj->pos : sf::Vector2f{0.f, 0.f});
    }

    [[maybe_unused]] void updateAcceleration(float t)
    {
        bool collision = false;
        vertex.clear();
        const float g = 0.8;
        const float softeningConstant = 0.15;
        for(auto iter = objectsArray.begin(); iter != objectsArray.end(); iter++)
        {
            sf::Vector2f accumulator;
            setSprite((*iter));
            for(auto iter2 = objectsArray.begin(); iter2 != objectsArray.end(); iter2++)
            {
                if(iter != iter2)
                {
                    auto d = lenght((*iter)->pos - (*iter2)->pos);
                    auto ForceDir = normalize((*iter2)->pos - (*iter)->pos);
                    auto Force = ForceDir * g * (*iter)->getMass() * (*iter2)->getMass() / (d);
                    (*iter)->addedAcceleration(Force / (pow((*iter)->getMass(), 2)));
                    calculateVertex((*iter)->obj.getPosition(), (*iter2)->obj.getPosition());
                }
            }
        }
    }

    [[maybe_unused]] void accelerationObjects(float t)
    {
        bool collision = false; //глобальный флаг True, если iter и iter2 касаютс
        //vertex.clear();
        for(auto iter = objectsArray.begin(); iter != objectsArray.end(); iter++)
        {
            //меняем текстуру йобы, в зависимости от ее скорости
            setSprite((*iter));
            //Чтобы йобы не улетали слишком далеко друг от друга, можно убрать
            /*if ((*iter)->obj.getPosition().x > 2000)
                (*iter)->addedAcceleration({ -0.08, 0 });
            if ((*iter)->obj.getPosition().y > 2000)
                (*iter)->addedAcceleration({ 0, -0.08 });

            if ((*iter)->obj.getPosition().x < -2000)
                (*iter)->addedAcceleration({ 0.08, 0 });
            if ((*iter)->obj.getPosition().y < -2000)
                (*iter)->addedAcceleration({ 0, 0.08 });*/

            for(auto iter2 = iter + 1; iter2 != objectsArray.end(); iter2++)
            {
                float fG = ForceOfGravity(*iter, *iter2);
                collision = isCollision(*iter, *iter2);

                float distance = sqrt(pow((*iter2)->obj.getPosition().x - (*iter)->obj.getPosition().x, 2) +
                                      pow((*iter2)->obj.getPosition().y - (*iter)->obj.getPosition().y, 2));
                //the mass ratio
                float massJtoI = ((float) (*iter2)->m / (*iter)->m); /// iter2/iter mass
                float massItoJ = ((float) (*iter)->m / (*iter2)->m); /// iter/iter2 mass

                //interaction force
                float fGmassJtoI = fG * pow(massJtoI, 3) / pow(distance, 0.25f);
                float fGmassItoJ = fG * pow(massItoJ, 3) / pow(distance, 0.25f);

                //float fGmassJtoI = fG * massJtoI / distance, 1.f);
                //float fGmassItoJ = fG * massItoJ / distance, 1.f);
                /*
                Работает на небольших расстояниях между йобами (100-2000)
                float fGmassJtoI = fG * pow(massJtoI, 2) * sqrt(sqrt(distance)); //чем
                больше дистанция тем больше сила ! ! ! ! float fGmassItoJ = fG *
                pow(massItoJ, 2) * sqrt(sqrt(distance));
                */

                if(collision)
                    ifCollision((*iter), (*iter2));
                vec2 direction((*iter)->obj.getPosition().x > (*iter2)->obj.getPosition().x ? -1 : 1,
                               (*iter)->obj.getPosition().y > (*iter2)->obj.getPosition().y ? -1 : 1);
                if(!collision)
                {
                    if(((*iter2) == biggestObject) || ((*iter) == biggestObject))
                    {
                        //максимально уменьшаяем силу для йобы с большей массой
                        if((*iter2)->m > (*iter)->m)
                        {
                            (*iter)->addedAcceleration(vec2(fGmassJtoI, fGmassJtoI) * direction);
                            (*iter2)->addedAcceleration(
                                    vec2(fGmassItoJ / distance * massItoJ, fGmassItoJ / distance * massItoJ) * -direction);
                        } else
                        {
                            (*iter)->addedAcceleration(
                                    vec2(fGmassJtoI / distance * massJtoI, fGmassJtoI / distance * massJtoI) * direction);
                            (*iter2)->addedAcceleration(vec2(fGmassItoJ, fGmassItoJ) * -direction);
                        }
                        //calculateVertex((*iter)->obj.getPosition(), (*iter2)->obj.getPosition());
                    }
                    /*else
                    {
                            (*iter)->addedAcceleration(vec2(fGmassItoJ * pow(massItoJ, 2)
                    / sqrt(distance), fGmassItoJ / sqrt(distance) * pow(massItoJ, 2)) *
                    direction);
                            (*iter2)->addedAcceleration(vec2(fGmassJtoI * pow(massJtoI, 2)
                    / sqrt(distance), fGmassJtoI / sqrt(distance) * pow(massJtoI, 2)) *
                    -direction); sf::VertexArray triangle(sf::Lines, 2);
                            triangle[0].position = (*iter)->obj.getPosition();
                            triangle[1].position = (*iter2)->obj.getPosition();
                            triangle[0].color = sf::Color::Green;
                            triangle[1].color = sf::Color::Green;
                            vertex.emplace_back(triangle);
                    }*/
                }
            }
        }
    }

    void moveObjects()
    {
        for(CelestialObject* object : objectsArray)
            object->addedPosition(object->acceleration * ftStep);
    }

    void calculateVertex(sf::Vector2f start, sf::Vector2f end)
    {
        sf::VertexArray triangle(sf::Lines, 2);
        triangle[0].position = start;
        triangle[1].position = end;
        triangle[0].color = sf::Color::Green;
        triangle[1].color = sf::Color::Green;
        vertex.emplace_back(triangle);
    }

    void DrawingAllObjects(sf::RenderWindow& window)
    {
        for(CelestialObject* object : objectsArray)
            window.draw(*object);
        //Визуализация вектора velocity объектов
        for(CelestialObject* object : objectsArray)
        {
            sf::VertexArray triangle(sf::Lines, 2);

            // define the position of the triangle's points
            triangle[0].position = object->obj.getPosition();
            triangle[1].position = object->getAccelerationVec();
            triangle[0].color = sf::Color::Red;
            triangle[1].color = sf::Color::Red;
            window.draw(triangle);
        }
        for(const auto& v : vertex)
            window.draw(v);
    }

    void Debug()
    {
        auto lambda_addNewObject = [this]()
        {
            ImGui::BeginChild("AddObject", ImVec2(ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 40), true);
            ImGui::PushItemWidth(80.f);
            static vec2 pos;
            ImGui::InputFloat("pos.y", &pos.y, 0, 0, 0);
            ImGui::InputFloat("pos.x", &pos.x, 0, 0, 0);
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
                pos = w->mapPixelToCoords(sf::Mouse::getPosition(), *view);
            ImGui::Separator();
            static vec2 acceleration;
            ImGui::InputFloat("vel.x", &acceleration.x, 0, 0, 0);
            ImGui::InputFloat("vel.y", &acceleration.y, 0, 0, 0);
            ImGui::Separator();
            static int r;
            ImGui::InputInt("r", &r, 0, 0, 0);
            ImGui::Separator();
            static int m;
            ImGui::InputInt("m", &m, 0, 0, 0);
            ImGui::PopItemWidth();
            if(ImGui::Button("add"))
                objectsArray.push_back(new CelestialObject(pos, acceleration, r, m));
            ImGui::EndChild();
        };

        auto lambda_set_viewport = [this]()
        {
            ImGui::BeginChild("Select Object", ImVec2(ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 40), true);
            if(ImGui::Selectable("Free Move"))
                centralObj = nullptr;
            int i = 1;
            for(const auto& obj : objectsArray)
            {
                if(ImGui::Selectable(("follow " + std::to_string(i)).c_str()))
                    centralObj = obj;
                ++i;
            }
            ImGui::EndChild();
        };

        auto lambda_debugWindow = [this]()
        {
            ImGui::BeginChild("Objects", ImVec2(ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 40), true);
            int i = 0;
            for(auto obj : objectsArray)
            {
                if(ImGui::TreeNode(std::to_string(i).c_str()))
                {
                    obj->obj.setFillColor(sf::Color(90, 150, 220));
                    ImGui::Text("acceleration %.20f", obj->acceleration);
                    ImGui::Text("mass %.2f", obj->m);
                    ImGui::Text("radius %f", obj->r);
                    ImGui::Separator();
                    static vec2 acceleration;
                    ImGui::DragFloat("vel.x", &acceleration.x, 0.01f, -0.9f, 0.9f);
                    ImGui::DragFloat("vel.y", &acceleration.y, 0.01f, -0.9f, 0.9f);
                    if(ImGui::Button("addAc"))
                        obj->addedAcceleration(acceleration);
                    ImGui::TreePop();
                } else
                    obj->obj.setFillColor(sf::Color::White);
                ++i;
            }
            ImGui::EndChild();
        };

        ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver || ImGuiWindowFlags_NoResize);
        if(!ImGui::Begin("ObjEditor"))
        {
            ImGui::End();
            return;
        }

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("Edit"))
            {
                if(ImGui::MenuItem("addNewObject", "", state == AddNewObject))
                    state = AddNewObject;

                if(ImGui::MenuItem("debugWindow", "", state == DebugWindow))
                    state = DebugWindow;

                if(ImGui::MenuItem("setViewPort", "", state == SetViewPort))
                    state = SetViewPort;

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        switch(state)
        {
            case AddNewObject:
                lambda_addNewObject();
                break;
            case DebugWindow:
                lambda_debugWindow();
                break;
            case SetViewPort:
            default:
                lambda_set_viewport();
                break;
        }

        ImGui::End();
    }
};


#endif //SFML_WORLD_H
