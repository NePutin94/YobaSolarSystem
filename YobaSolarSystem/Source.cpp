#include <thread>
#include <future>
#include <mutex>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <random>
#include "imgui.h"
#include "imgui-sfml.h"
using namespace std;
typedef sf::Vector2f vec2;

sf::Texture tex1;
sf::Texture tex2;
sf::Texture tex3;

vec2 operator * (const vec2& vector, const vec2& vector2)
{
	vec2 dot;
	dot.x = vector2.x * vector.x;
	dot.y = vector2.y * vector.y;
	return dot;
}
vec2 GetIntersectionDepth(const sf::FloatRect& rectA, const sf::FloatRect& rectB)
{
	float halfWidthA = rectA.width / 2.0f;
	float halfHeightA = rectA.height / 2.0f;
	float halfWidthB = rectB.width / 2.0f;
	float halfHeightB = rectB.height / 2.0f;

	vec2 centerA(rectA.left + halfWidthA, rectA.top + halfHeightA);
	vec2 centerB(rectB.left + halfWidthB, rectB.top + halfHeightB);

	float distanceX = centerA.x - centerB.x;
	float distanceY = centerA.y - centerB.y;
	float minDistanceX = halfWidthA + halfWidthB;
	float minDistanceY = halfHeightA + halfHeightB;

	if (abs(distanceX) >= minDistanceX || abs(distanceY) >= minDistanceY)
		return vec2(0, 0);

	float depthX = distanceX > 0 ? minDistanceX - distanceX : -minDistanceX - distanceX;
	float depthY = distanceY > 0 ? minDistanceY - distanceY : -minDistanceY - distanceY;

	return vec2(depthX, depthY);
}

class CelestialObject
{
public:
	sf::CircleShape obj;
	vec2 acceleration;
	int r, m;

	CelestialObject()
	{
		acceleration = { 0, 0 };
		r = 0;
		m = 0;
	}
	CelestialObject(vec2 _pos, vec2 _vel, int _r, int _m) : acceleration(_vel), r(_r), m(_m)
	{
		obj.setPosition(_pos);
		obj.setRadius(r);
		obj.setTexture(&tex1);
		obj.setTextureRect(sf::IntRect(0, 0, 792, 792));
		obj.setOrigin(obj.getLocalBounds().width / 2, obj.getLocalBounds().height / 2);
		r = _r;
		m = _m;
	}
	void addedAcceleration(sf::Vector2f a)
	{
		acceleration = acceleration + a;
	}
	operator sf::Drawable& ()
	{
		return obj;
	}
	void addMass(float _m)
	{
		m += _m;
	}
	void addRadius(float _r)
	{
		r += _r;
		obj.setRadius(r);
	}
	void addedPosition(vec2 vector)
	{
		obj.move(vector);
	}
	vec2 getAccelerationVec()
	{
		return vec2(obj.getPosition() + acceleration * vec2(20, 20));
	}
};

class World
{
public:
	std::vector<CelestialObject*> objectsArray;
	float ForceOfGravity(bool& collision, CelestialObject* o1, CelestialObject* o2)
	{
		float distance = sqrt(pow(o1->obj.getPosition().x - o2->obj.getPosition().x, 2) + pow(o1->obj.getPosition().y - o2->obj.getPosition().y, 2));

		if (o1->obj.getGlobalBounds().intersects(o2->obj.getGlobalBounds()))
		{
			collision = true;
			/*if (o1->m < o2->m)
			{
				auto intersec = GetIntersectionDepth(o1->obj.getGlobalBounds(), o2->obj.getGlobalBounds());
				if (abs(intersec.x) > abs(intersec.y))
					intersec.x = 0;
				else
					intersec.y = 0;
				o1->obj.setPosition(o1->obj.getPosition() + intersec);
			}
			else
			{
				auto intersec = GetIntersectionDepth(o2->obj.getGlobalBounds(), o1->obj.getGlobalBounds());
				if (abs(intersec.x) > abs(intersec.y))
					intersec.x = 0;
				else
					intersec.y = 0;
				o2->obj.setPosition(o2->obj.getPosition() + intersec);
			}*/
			return 0.0000001;
		}
		return (o1->m * o2->m / sqrt(distance)) / 90000000;
		//return (o1->m * o2->m / pow(distance,2)) / 60000; //ведет себя менее стабильно Йобу достаточно сложно вывести на орбиту
	}
	void add(CelestialObject object)
	{
		objectsArray.push_back(new CelestialObject(object));
	}
	void accelerationObjects()
	{
		bool collision = false;//глобальный флаг True, если iter и iter2 касаются

		for (auto iter = objectsArray.begin(); iter != objectsArray.end(); iter++)
		{
			//меняем текстуру йобы, в зависимости от ее скорости
			if (abs((*iter)->acceleration.x) > 0 || abs((*iter)->acceleration.y) > 0)
			{
				(*iter)->obj.setTexture(&tex1);
				(*iter)->obj.setTextureRect(sf::IntRect(0, 0, 792, 792));
			}
			if (abs((*iter)->acceleration.x) > 1 || abs((*iter)->acceleration.y) > 1)
			{
				(*iter)->obj.setTexture(&tex2);
				(*iter)->obj.setTextureRect(sf::IntRect(0, 0, 128, 128));
			}
			if (abs((*iter)->acceleration.x) > 2.6 || abs((*iter)->acceleration.y) > 2.6)
			{
				(*iter)->obj.setTexture(&tex3);
				(*iter)->obj.setTextureRect(sf::IntRect(0, 0, 512, 512));
			}
			//Чтобы йобы не улетали слишком далеко друг от друга, можно убрать
			if ((*iter)->obj.getPosition().x > 2000)
				(*iter)->addedAcceleration({ -0.08, 0 });
			if ((*iter)->obj.getPosition().y > 2000)
				(*iter)->addedAcceleration({ 0, -0.08 });

			if ((*iter)->obj.getPosition().x < -2000)
				(*iter)->addedAcceleration({ 0.08, 0 });
			if ((*iter)->obj.getPosition().y < -2000)
				(*iter)->addedAcceleration({ 0, 0.08 });

			for (auto iter2 = iter + 1; iter2 != objectsArray.end(); iter2++)
			{

				float fG = ForceOfGravity(collision, *iter, *iter2);
				float distance = sqrt(pow((*iter2)->obj.getPosition().x - (*iter)->obj.getPosition().x, 2) + pow((*iter2)->obj.getPosition().y - (*iter)->obj.getPosition().y, 2));
				//Отношение их масс:
				float massJtoI = ((float)(*iter2)->m / (*iter)->m); /// iter2/iter mass
				float massItoJ = ((float)(*iter)->m / (*iter2)->m); /// iter/iter2 mass

				//конечная сила взаимодействия двух йоб
				float fGmassJtoI = fG * pow(massJtoI, 2) * sqrt(sqrt(distance));
				float fGmassItoJ = fG * pow(massItoJ, 2) * sqrt(sqrt(distance));

				/*
				float fGmassJtoI = fG * pow(massJtoI, 2) * sqrt(distance);
				float fGmassItoJ = fG * pow(massItoJ, 2) * sqrt(distance);
				*/

				vec2 direction(
					(*iter)->obj.getPosition().x > (*iter2)->obj.getPosition().x ? -1 : 1,
					(*iter)->obj.getPosition().y > (*iter2)->obj.getPosition().y ? -1 : 1);
				if (collision)
				{
					vec2 direction2(
						(*iter)->obj.getPosition().x < (*iter2)->obj.getPosition().x ? -1 : 1,
						(*iter)->obj.getPosition().y < (*iter2)->obj.getPosition().y ? -1 : 1);
					if ((*iter2)->m > (*iter)->m)
					{
						(*iter)->addedAcceleration(vec2(0.01, 0.01) * direction2);
						float newR = (*iter)->r * massItoJ;
						float newMass = (*iter)->m * 0.5f;
						/*(*iter2)->addMass((*iter)->m * 0.5f);
						(*iter2)->addRadius((*iter)->r * massItoJ);
						(*iter)->addRadius(-newR);
						(*iter)->addMass(-newMass);
						delete objectsArray[i];
						objectsArray.erase(objectsArray.begin() + i);*/
					}
					else
					{
						(*iter2)->addedAcceleration(vec2(0.01, 0.01) * -direction2);
						float newR = (*iter2)->r * massItoJ;
						float newMass = (*iter2)->m * 0.5f;
						/*
						(*iter)->addMass(newMass);
						(*iter)->addRadius(newR);
						(*iter2)->addRadius(-newR);
						(*iter2)->addMass(-newMass);
						delete objectsArray[j];
						objectsArray.erase(objectsArray.begin() + j);*/
					}
				}
				if (!collision)
				{
					//максимально уменьшаяем силу для йобы с большей массой
					if ((*iter2)->m > (*iter)->m)
					{
						(*iter)->addedAcceleration(vec2(fGmassJtoI, fGmassJtoI) * direction);
						(*iter2)->addedAcceleration(vec2(fGmassItoJ / sqrt(distance) * massItoJ, fGmassItoJ / sqrt(distance) * massItoJ) * -direction);
					}
					else
					{
						(*iter)->addedAcceleration(vec2(fGmassJtoI / sqrt(distance) * massJtoI, fGmassJtoI / sqrt(distance) * massJtoI) * direction);
						(*iter2)->addedAcceleration(vec2(fGmassItoJ, fGmassItoJ) * -direction);
					}
				}
			}
		}
	}
	void moveObjects()
	{
		for (CelestialObject* object : objectsArray)
			object->addedPosition(object->acceleration);

	}
	void DrawingAllObjects(sf::RenderWindow& w)
	{
		for (CelestialObject* object : objectsArray)
			w.draw(*object);
		for (CelestialObject* object : objectsArray)
		{
			sf::VertexArray triangle(sf::Lines, 2);

			// define the position of the triangle's points
			triangle[0].position = object->obj.getPosition();
			triangle[1].position = object->getAccelerationVec();
			triangle[0].color = sf::Color::Red;
			triangle[1].color = sf::Color::Red;
			w.draw(triangle);
		}
	}
	void Debug()
	{
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver || ImGuiWindowFlags_NoResize);
		if (!ImGui::Begin("ObjEditor"))
		{
			ImGui::End();
			return;
		}

		ImGui::BeginChild("Objects", ImVec2(ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 40), true);
		int i = 0;
		for (auto obj : objectsArray)
		{
			if (ImGui::TreeNode(to_string(i).c_str()))
			{
				ImGui::Text("acceleration %.20f", obj->acceleration);
				ImGui::Text("mass %i", obj->m);
				ImGui::Text("radius %i", obj->r);
				ImGui::Separator();
				static vec2 acceleration;
				ImGui::DragFloat("vel.x", &acceleration.x, 0.01f, -0.9f, 0.9f);
				ImGui::DragFloat("vel.y", &acceleration.y, 0.01f, -0.9f, 0.9f);
				if (ImGui::Button("addAc"))
					obj->addedAcceleration(acceleration);
				ImGui::TreePop();
			}
			++i;
		}
		ImGui::EndChild();
		ImGui::End();
	}
};

int main()
{
	tex1.loadFromFile("yoba3.png");
	tex2.loadFromFile("yoba2.png");
	tex3.loadFromFile("yoba4.png");

	World world;

	sf::View view;
	sf::FloatRect viewPort = { 0, 0, 1800, 1000 };
	view.reset(viewPort);
	vec2 center = { viewPort.width / 2, viewPort.height / 2 };
	sf::RenderWindow window(sf::VideoMode(1800, 1000), "yobaSolarSystem");
	window.setFramerateLimit(100);
	ImGui::SFML::Init(window);
	/*
	srand(time(0));
	for (int i = 1; i < 4; i++)
	{
		int r = rand() % 40 + 15;
		int m = r * 40;
		Vector2f position(100 + rand() % 1000, 100 + rand() % 800);
		Vector2f direction(1 - rand() % 2, (1 - rand() % 2));
		gravity.add(CelestialObject(position, direction, r, m));
	}*/

	world.add(CelestialObject(vec2(300, 300), vec2(0, 0), 25, 1200));
	world.add(CelestialObject(vec2(300, 430), vec2(1.5f, 0), 15, 735));
	//world.add(CelestialObject(Vector2f(300, 580), Vector2f(1.5f, 0), 10, 435));
	sf::Clock deltaClock;
	while (window.isOpen())
	{
		sf::Event event;
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
			center.y += 10;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			center.x -= 10;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			center.y -= 10;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			center.x += 10;

		ImGui::SFML::Update(window, deltaClock.restart());
		world.accelerationObjects();
		world.moveObjects();
		world.Debug();
		view.reset(viewPort);
		view.setCenter(center);
		window.setView(view);
		window.clear();

		world.DrawingAllObjects(window);
		ImGui::SFML::Render(window);
		window.display();
	}

	return 0;
}
