
#define SOL_ALL_SAFETIES_ON 0
//#include <sol/sol.hpp>
#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sol/sol.hpp>

using namespace std;
sf::Texture tex1;
sf::Texture tex2;
sf::Texture tex3;
using vec2 = sf::Vector2f;
constexpr float ftStep{ 1.f }, ftSlice{ 1.f };
vec2 operator*(const vec2& vector, const vec2& vector2)
{
	vec2 dot;
	dot.x = vector2.x * vector.x;
	dot.y = vector2.y * vector.y;
	return dot;
}
vec2 multiply(const vec2& vector, const vec2& vector2)
{
	vec2 dot;
	dot.x = vector2.x * vector.x;
	dot.y = vector2.y * vector.y;
	return dot;
}
vec2 invert(sf::Vector2f vec) { return vec * sf::Vector2f(-1, -1); }
vec2 GetIntersectionDepth(const sf::FloatRect& rectA,
	const sf::FloatRect& rectB)
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

	float depthX =
		distanceX > 0 ? minDistanceX - distanceX : -minDistanceX - distanceX;
	float depthY =
		distanceY > 0 ? minDistanceY - distanceY : -minDistanceY - distanceY;

	return vec2(depthX, depthY);
}

class CelestialObject
{
public:
	sf::CircleShape obj;
	vec2 acceleration;
	int r, m;
	sf::Vector2f pos;
	CelestialObject()
	{
		acceleration = { 0, 0 };
		r = 0;
		m = 0;
	}
	CelestialObject(vec2 _pos, vec2 _vel, int _r, int _m)
		: acceleration(_vel), r(_r), m(_m)
	{
		obj.setPosition(_pos);
		obj.setRadius(r);
		obj.setTexture(&tex1);
		obj.setTextureRect(sf::IntRect(0, 0, 792, 792));
		obj.setOrigin(obj.getLocalBounds().width / 2,
			obj.getLocalBounds().height / 2);
		r = _r;
		m = _m;
		pos = _pos;
	}
	int getMass() { return m; }
	void addedAcceleration(sf::Vector2f a) { acceleration = acceleration + a; }
	operator sf::Drawable& () { return obj; }
	void addMass(float _m) { m += _m; }
	void addRadius(float _r)
	{
		r += _r;
		obj.setRadius(r);
	}
	void addedPosition(vec2 vector)
	{
		obj.move(vector);
		pos = obj.getPosition();
	}
	vec2 getAccelerationVec()
	{
		return vec2(obj.getPosition() + acceleration * vec2(20, 20));
	}
};

class World
{
	enum debug_state
	{
		AddNewObject = 0,
		DebugWindow,
		SetViewPort
	};
	debug_state state;
	sf::RenderWindow* w;
	sf::View* view;
	CelestialObject* centralObj;
public:
	std::vector<CelestialObject*> objectsArray;
	std::vector<sf::VertexArray> vertex;
	CelestialObject* biggestObject;
	World() = default;
	World(sf::RenderWindow* window, sf::View* _view) : w(window), view(_view)
	{

	}

	void clearVertexArray() { vertex.clear(); }

	float ForceOfGravity(CelestialObject* o1, CelestialObject* o2)
	{
		float distance =
			sqrt(pow(o1->obj.getPosition().x - o2->obj.getPosition().x, 2) +
				pow(o1->obj.getPosition().y - o2->obj.getPosition().y, 2));
		return (o1->m * o2->m / sqrt(distance)) / 9000000;
		// return (o1->m * o2->m / pow(distance,2)) / 60000; //ведет себя менее
		// стабильно Йобу достаточно сложно вывести на орбиту
	}

	void add(CelestialObject object)
	{
		objectsArray.push_back(new CelestialObject(object));
	}

	bool isCollision(CelestialObject* o1, CelestialObject* o2)
	{
		return o1->obj.getGlobalBounds().intersects(o2->obj.getGlobalBounds());
	}

	void setSprite(CelestialObject* obj)
	{
		if (abs(obj->acceleration.x) > 0 || abs(obj->acceleration.y) > 0)
		{
			obj->obj.setTexture(&tex1);
			obj->obj.setTextureRect(sf::IntRect(0, 0, 792, 792));
		}
		if (abs(obj->acceleration.x) > 1.5 || abs(obj->acceleration.y) > 1.5)
		{
			obj->obj.setTexture(&tex2);
			obj->obj.setTextureRect(sf::IntRect(0, 0, 128, 128));
		}
		if (abs(obj->acceleration.x) > 3.6 || abs(obj->acceleration.y) > 2)
		{
			obj->obj.setTexture(&tex3);
			obj->obj.setTextureRect(sf::IntRect(0, 0, 512, 512));
		}
	}

	void ifCollision(CelestialObject* obj1, CelestialObject* obj2)
	{
		vec2 direction2;
		if ((obj1->obj.getPosition().x > obj2->obj.getPosition().x))
			direction2.x = 1;
		else
			direction2.x = -1;

		if ((obj1->obj.getPosition().y > obj2->obj.getPosition().y))
			direction2.x = 1;
		else
			direction2.x = -1;

		if (obj2->m > obj1->m)
		{
			obj1->addedAcceleration(vec2(0.5, 0.5) * direction2);
			/*(*iter2)->addMass((*iter)->m * 0.5f);
			(*iter2)->addRadius((*iter)->r * massItoJ);
			(*iter)->addRadius(-newR);
			(*iter)->addMass(-newMass);
			delete objectsArray[i];
			objectsArray.erase(objectsArray.begin() + i);*/
		}
		else
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
		return (centralObj != nullptr) ? centralObj->pos : sf::Vector2f{ 0.f, 0.f };
	}

	void accelerationObjects(float t)
	{
		bool collision = false; //глобальный флаг True, если iter и iter2 касаютс
		vertex.clear();
		for (auto iter = objectsArray.begin(); iter != objectsArray.end(); iter++)
		{
			//меняем текстуру йобы, в зависимости от ее скорости
			setSprite((*iter));
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
				float fG = ForceOfGravity(*iter, *iter2);
				collision = isCollision(*iter, *iter2);

				float distance = sqrt(pow((*iter2)->obj.getPosition().x - (*iter)->obj.getPosition().x, 2) + pow((*iter2)->obj.getPosition().y - (*iter)->obj.getPosition().y, 2));
				//the mass ratio
				float massJtoI = ((float)(*iter2)->m / (*iter)->m); /// iter2/iter mass
				float massItoJ = ((float)(*iter)->m / (*iter2)->m); /// iter/iter2 mass

				//interaction force
				float fGmassJtoI = fG * pow(massJtoI, 3) / pow(distance, 0.25f);
				float fGmassItoJ = fG * pow(massItoJ, 3) / pow(distance, 0.25f);

				/*
				Работает на небольших расстояниях между йобами (100-2000)
				float fGmassJtoI = fG * pow(massJtoI, 2) * sqrt(sqrt(distance)); //чем
				больше дистанция тем больше сила ! ! ! ! float fGmassItoJ = fG *
				pow(massItoJ, 2) * sqrt(sqrt(distance));
				*/

				if (collision)
					ifCollision((*iter), (*iter2));
				vec2 direction((*iter)->obj.getPosition().x > (*iter2)->obj.getPosition().x ? -1 : 1,
					(*iter)->obj.getPosition().y > (*iter2)->obj.getPosition().y ? -1 : 1);
				if (!collision)
				{
					if (((*iter2) == biggestObject) || ((*iter) == biggestObject))
					{
						//максимально уменьшаяем силу для йобы с большей массой
						if ((*iter2)->m > (*iter)->m)
						{
							(*iter)->addedAcceleration(vec2(fGmassJtoI, fGmassJtoI) * direction);
							(*iter2)->addedAcceleration(vec2(fGmassItoJ / distance * massItoJ, fGmassItoJ / distance * massItoJ) * -direction);
						}
						else
						{
							(*iter)->addedAcceleration(vec2(fGmassJtoI / distance * massJtoI, fGmassJtoI / distance * massJtoI) * direction);
							(*iter2)->addedAcceleration(vec2(fGmassItoJ, fGmassItoJ) * -direction);
						}
						calculateVertex((*iter)->obj.getPosition(),
							(*iter2)->obj.getPosition());
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
		for (CelestialObject* object : objectsArray)
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

	void DrawingAllObjects(sf::RenderWindow& w)
	{
		for (CelestialObject* object : objectsArray)
			w.draw(*object);
		//Визуализация вектора velocity объектов
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
		for (const auto& v : vertex)
			w.draw(v);
	}

	void Debug()
	{
		auto lambda_addNewObject = [this]() {
			ImGui::BeginChild("AddObject", ImVec2(ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 40), true);
			ImGui::PushItemWidth(80.f);
			static vec2 pos;
			ImGui::InputFloat("pos.y", &pos.y, 0, 0, 0);
			ImGui::InputFloat("pos.x", &pos.x, 0, 0, 0);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
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
			if (ImGui::Button("add"))
			{
				CelestialObject object(pos, acceleration, r, m);
				objectsArray.push_back(new CelestialObject(object));
			}
			ImGui::EndChild();
		};

		auto lambda_set_viewport = [this]() {
			ImGui::BeginChild("Select Object", ImVec2(ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 40), true);
			if (ImGui::Selectable("Free Move"))
				centralObj = nullptr;
			int i = 1;
			for (const auto& obj : objectsArray)
			{
				if (ImGui::Selectable(("follow " + std::to_string(i)).c_str()))
					centralObj = obj;
				++i;
			}
			ImGui::EndChild();
		};

		auto lambda_debugWindow = [this]() {
			ImGui::BeginChild("Objects", ImVec2(ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 40), true);
			int i = 0;
			for (auto obj : objectsArray)
			{
				if (ImGui::TreeNode(to_string(i).c_str()))
				{
					obj->obj.setFillColor(sf::Color(90, 150, 220));
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
				else
					obj->obj.setFillColor(sf::Color::White);
				++i;
			}
			ImGui::EndChild();
		};

		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver || ImGuiWindowFlags_NoResize);
		if (!ImGui::Begin("ObjEditor"))
		{
			ImGui::End();
			return;
		}
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("addNewObject", "", state == AddNewObject))
					state = AddNewObject;

				if (ImGui::MenuItem("debugWindow", "", state == DebugWindow))
					state = DebugWindow;

				if (ImGui::MenuItem("setViewPort", "", state == SetViewPort))
					state = SetViewPort;

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		switch (state)
		{
		case AddNewObject:
			lambda_addNewObject();
			break;
		case DebugWindow:
			lambda_debugWindow();
			break;
		case SetViewPort:
			lambda_set_viewport();
			break;
		default:
			break;
		}

		ImGui::End();
	}
};

using namespace chrono;
int main()
{
	sf::Context c;
	tex1.loadFromFile("yoba3.png");
	tex2.loadFromFile("yoba2.png");
	tex3.loadFromFile("yoba4.png");
	sf::RenderWindow window(sf::VideoMode(1800, 1000), "yobaSolarSystem");
	window.setFramerateLimit(200);
	sf::View view;
	World world(&window, &view);

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::math);
	lua["world"] = &world;
	sol::usertype<World> player_type = lua.new_usertype<World>(
		"World", sol::constructors<World()>(), "addObject", &World::add,
		"clearVertexArray", &World::clearVertexArray, "ifCollision",
		&World::ifCollision, "setSprite", &World::setSprite, "ForceOfGravity",
		&World::ForceOfGravity, "isCollision", &World::isCollision,
		"calculateVertex", &World::calculateVertex, "objects",
		&World::objectsArray);
	lua.new_usertype<CelestialObject>(
		"CObject", sol::constructors<CelestialObject(vec2, vec2, int, int)>(),
		"addedAcceleration", &CelestialObject::addedAcceleration, "obj",
		&CelestialObject::obj, "addedPosition", &CelestialObject::addedPosition,
		"addMass", &CelestialObject::addMass, "mass", &CelestialObject::m,
		"addRadius", &CelestialObject::addRadius, "acceleration",
		&CelestialObject::acceleration, "pos", &CelestialObject::pos);
	lua.set_function("multiply", &multiply);
	lua.set_function("invert", &invert);
	sol::usertype<sf::Vector2f> vec_type = lua.new_usertype<sf::Vector2f>(
		"vec2", sol::constructors<sf::Vector2f(), sf::Vector2f(float, float)>());
	vec_type["x"] = &sf::Vector2f::x;
	vec_type["y"] = &sf::Vector2f::y;

	sol::usertype<sf::CircleShape> circle_type =
		lua.new_usertype<sf::CircleShape>("circleShape",
			sol::constructors<sf::CircleShape()>());
	circle_type["getPos"] = &sf::CircleShape::getPosition;


	sf::FloatRect viewPort = { 0, 0, 1920, 1080 };
	view.reset(viewPort);
	vec2 center = { viewPort.width / 2, viewPort.height / 2 };

	ImGui::SFML::Init(window);

	srand(time(0));
	/*for (int i = 1; i < 4; i++)
	{
			int r = rand() % 30 + 10;
			int m = r * 40;
			vec2 position(100 + rand() % 1000, 100 + rand() % 800);
			vec2 direction(1 - rand() % 2, (1 - rand() % 2));
			world.add(CelestialObject(position, direction, r, m));
	}*/

	/*world.add(CelestialObject(vec2(300, 300), vec2(0, 0), 25, 1200));
	world.add(CelestialObject(vec2(300, 980), vec2(3.1f, 0), 15, 735));
	world.add(CelestialObject(vec2(300, 590), vec2(2.9f, 0), 10, 600));
	world.add(CelestialObject(vec2(300, 900), vec2(2.f, 0), 10, 705));*/
	lua.script_file("init_lua_script.lua");

	sf::Clock deltaClock;
	// auto f = lua.load_file("update_lua_script.lua");
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
			// lua.script_file("update_lua_script.lua");
			// lua.do_file("update_lua_script.lua");
			world.accelerationObjects(1.f);
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

	return 0;
}
