#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <chrono>
using namespace std;
sf::Texture tex1;
sf::Texture tex2;
sf::Texture tex3;
using vec2 = sf::Vector2f;
constexpr float ftStep{ 1.f }, ftSlice{ 1.f };
vec2 operator * (const vec2& vector, const vec2& vector2)
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
	int getMass()
	{
		return m;
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
	enum debug_state { AddNewObject = 0, DebugWindow };
	debug_state state;
public:
	std::vector<CelestialObject*> objectsArray;
	std::vector<sf::VertexArray> vertex;
	float ForceOfGravity(CelestialObject* o1, CelestialObject* o2)
	{
		float distance = sqrt(pow(o1->obj.getPosition().x - o2->obj.getPosition().x, 2) + pow(o1->obj.getPosition().y - o2->obj.getPosition().y, 2));
		return (o1->m * o2->m / sqrt(distance)) / 9000000;
		//return (o1->m * o2->m / pow(distance,2)) / 60000; //ведет себя менее стабильно Йобу достаточно сложно вывести на орбиту
	}
	void add(CelestialObject object)
	{
		objectsArray.push_back(new CelestialObject(object));
	}
	bool isCollision(CelestialObject* o1, CelestialObject* o2)
	{
		if (o1->obj.getGlobalBounds().intersects(o2->obj.getGlobalBounds()))
		{
			return true;
		}
	}
	void accelerationObjects(float t)
	{
		vertex.clear();
		bool collision = false;//глобальный флаг True, если iter и iter2 касаютс

		for (auto iter = objectsArray.begin(); iter != objectsArray.end(); iter++)
		{
			//меняем текстуру йобы, в зависимости от ее скорости
			if (abs((*iter)->acceleration.x) > 0 || abs((*iter)->acceleration.y) > 0)
			{
				(*iter)->obj.setTexture(&tex1);
				(*iter)->obj.setTextureRect(sf::IntRect(0, 0, 792, 792));
			}
			if (abs((*iter)->acceleration.x) > 1.5 || abs((*iter)->acceleration.y) > 1.5)
			{
				(*iter)->obj.setTexture(&tex2);
				(*iter)->obj.setTextureRect(sf::IntRect(0, 0, 128, 128));
			}
			if (abs((*iter)->acceleration.x) > 3.6 || abs((*iter)->acceleration.y) > 2)
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

			//	float fG = ForceOfGravity(*iter, *iter2);
		 //       bool collision = isCollision(*iter, *iter2);
			//	if (collision)
			//		fG = 0.00000000001f;
			//	float distance = sqrt(pow((*iter2)->obj.getPosition().x - (*iter)->obj.getPosition().x, 2) + pow((*iter2)->obj.getPosition().y - (*iter)->obj.getPosition().y, 2));
			////	//Отношение их масс:
			//	float massJtoI = ((float)(*iter2)->m / (*iter)->m); /// iter2/iter mass
			//	float massItoJ = ((float)(*iter)->m / (*iter2)->m); /// iter/iter2 mass

			////	//конечная сила взаимодействия двух йоб
			//	float fGmassJtoI = fG * pow(massJtoI, 3) / pow(distance, 0.25f);
			//	float fGmassItoJ = fG * pow(massItoJ, 3) / pow(distance, 0.25f);

			//	/*
			//	Работает на небольших расстояниях между йобами (100-2000)
			//	float fGmassJtoI = fG * pow(massJtoI, 2) * sqrt(sqrt(distance)); //чем больше дистанция тем больше сила ! ! ! !
			//	float fGmassItoJ = fG * pow(massItoJ, 2) * sqrt(sqrt(distance));
			//	*/

			//	vec2 direction(
			//		(*iter)->obj.getPosition().x > (*iter2)->obj.getPosition().x ? -1 : 1,
			//		(*iter)->obj.getPosition().y > (*iter2)->obj.getPosition().y ? -1 : 1);
				//if (collision)
				//{
				//	vec2 direction2(
				//		(*iter)->obj.getPosition().x < (*iter2)->obj.getPosition().x ? -1 : 1,
				//		(*iter)->obj.getPosition().y < (*iter2)->obj.getPosition().y ? -1 : 1);
				//	if ((*iter2)->m > (*iter)->m)
				//	{
				//		(*iter)->addedAcceleration(vec2(fGmassJtoI, fGmassJtoI) * direction2);
				//		float newR = (*iter)->r * massItoJ;
				//		float newMass = (*iter)->m * 0.5f;
				//		/*(*iter2)->addMass((*iter)->m * 0.5f);
				//		(*iter2)->addRadius((*iter)->r * massItoJ);
				//		(*iter)->addRadius(-newR);
				//		(*iter)->addMass(-newMass);
				//		delete objectsArray[i];
				//		objectsArray.erase(objectsArray.begin() + i);*/
				//	}
				//	else
				//	{
				//		(*iter2)->addedAcceleration(vec2(fGmassItoJ, fGmassItoJ) * -direction2);
				//		float newR = (*iter2)->r * massItoJ;
				//		float newMass = (*iter2)->m * 0.5f;
				//		/*
				//		(*iter)->addMass(newMass);
				//		(*iter)->addRadius(newR);
				//		(*iter2)->addRadius(-newR);
				//		(*iter2)->addMass(-newMass);
				//		delete objectsArray[j];
				//		objectsArray.erase(objectsArray.begin() + j);*/
				//	}
				//}
				//if (!collision)
				//{
				//	//максимально уменьшаяем силу для йобы с большей массой
				//	if ((*iter2)->m > (*iter)->m)
				//	{
				//		(*iter)->addedAcceleration(vec2(fGmassJtoI / sqrt(distance) * massJtoI, fGmassJtoI / sqrt(distance) * massJtoI) * direction);
				//		(*iter2)->addedAcceleration(vec2(fGmassItoJ, fGmassItoJ) * -direction);
				//	}
				//	else
				//	{
				//		(*iter)->addedAcceleration(vec2(fGmassJtoI / sqrt(distance) * massJtoI, fGmassJtoI / sqrt(distance) * massJtoI) * direction);
				//		(*iter2)->addedAcceleration(vec2(fGmassItoJ, fGmassItoJ) * -direction);
				//	}
				//}
				sf::VertexArray triangle(sf::Lines, 2);
				triangle[0].position = (*iter)->obj.getPosition();
				triangle[1].position = (*iter2)->obj.getPosition();
				triangle[0].color = sf::Color::Yellow;
				triangle[1].color = sf::Color::Red;
				vertex.emplace_back(triangle);
			}
		}

	}
	void moveObjects()
	{
		for (CelestialObject* object : objectsArray)
		{
			object->addedPosition(object->acceleration * ftStep);
		}
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
		auto lambda_addNewObject = [this]()
		{

			ImGui::BeginChild("AddObject", ImVec2(ImGui::GetWindowSize().x - 20, ImGui::GetWindowSize().y - 40), true);
			ImGui::PushItemWidth(80.f);
			static vec2 pos;
			ImGui::InputFloat("pos.x", &pos.x, 0, 0, 0);
			ImGui::InputFloat("pos.y", &pos.y, 0, 0, 0);
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
		auto lambda_debugWindow = [this]()
		{
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
		default:
			break;
		}

		ImGui::End();
	}
};

//struct player {
//private:
//	sf::RectangleShape s;
//public:
//
//	player()
//	{
//		s.setFillColor(sf::Color::Red);
//		s.setSize({ 20.f,20.f });
//		s.setPosition(0,0);
//	}
//	static player* create() { return new player(); }
//	void move(sf::Vector2f newPos)
//	{
//		s.move(newPos);
//	}
//	void setPos(sf::Vector2f newPos)
//	{
//		s.setPosition(newPos);
//	}
//	void setSize(sf::Vector2f size)
//	{
//		s.setSize(size);
//	}
//	operator sf::Drawable& ()
//	{
//		return s;
//	}
//};
//int main() {
//
//	sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
//
//	sol::state lua;
//	lua.open_libraries(sol::lib::base);
//	std::vector<player*> players;
//	lua["players"] = &players;
//	sol::usertype<player> player_type = lua.new_usertype<player>("player",
//		sol::constructors<player()>(),
//		"create", sol::factories(&player::create)
//		);
//	player_type["move"] = &player::move;
//	player_type["setSize"] = &player::setSize;
//	player_type["setPos"] = &player::setPos;
//	lua.new_enum("Key",
//		"A", sf::Keyboard::A,
//		"D", sf::Keyboard::D,
//		"S", sf::Keyboard::S,
//		"W", sf::Keyboard::W
//	);
//	lua.set_function("IsKeyPressed", sf::Keyboard::isKeyPressed);
//
//	sol::usertype<sf::Vector2f> vec_type = lua.new_usertype<sf::Vector2f>("vec2",
//		sol::constructors<sf::Vector2f(float, float)>()
//		);
//	vec_type["x"] = &sf::Vector2f::x;
//	vec_type["y"] = &sf::Vector2f::y;
//	lua.script_file("init_lua_script.lua");
//	while (window.isOpen())
//	{
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//			{
//				for (auto i = 0; i < players.size(); i++)
//					delete players[i];
//				window.close();
//			}
//		}
//		lua.script_file("update_lua_script.lua");
//		window.clear();
//		for (auto& item : players)
//			window.draw(*item);
//		window.display();
//	}
//	return 0;
//}

int main()
{
	tex1.loadFromFile("yoba3.png");
	tex2.loadFromFile("yoba2.png");
	tex3.loadFromFile("yoba4.png");

	World world;

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::math);
	lua["world"] = &world;
	sol::usertype<World> player_type = lua.new_usertype<World>("player",
		sol::constructors<World()>(),
		"addObject", &World::add,
		"ForceOfGravity", &World::ForceOfGravity,
		"isCollision", &World::isCollision,
		"objects", &World::objectsArray
		);
	lua.new_usertype<CelestialObject>("CelestialObject",
		sol::constructors<CelestialObject(vec2, vec2, int, int)>(),
		
		"addedAcceleration", &CelestialObject::addedAcceleration,
		"obj", &CelestialObject::obj,
		"addedPosition", &CelestialObject::addedPosition,
		"addMass", &CelestialObject::addMass,
		"getMass", & CelestialObject::getMass,
		"addRadius", &CelestialObject::addRadius,
		"getAcceleration", &CelestialObject::getAccelerationVec
		);
	lua.set_function("multiply", &multiply);
	sol::usertype<sf::Vector2f> vec_type = lua.new_usertype<sf::Vector2f>("vec2",
		sol::constructors<sf::Vector2f(),sf::Vector2f(float, float)>()
		);
	vec_type["x"] = &sf::Vector2f::x;
	vec_type["y"] = &sf::Vector2f::y;

	sol::usertype<sf::CircleShape> circle_type = lua.new_usertype<sf::CircleShape>("circleShape",
		sol::constructors<sf::CircleShape()>()
		);
	circle_type["getPos"] = &sf::CircleShape::getPosition;

	sf::View view;
	sf::FloatRect viewPort = { 0, 0, 1920, 1080 };
	view.reset(viewPort);
	vec2 center = { viewPort.width / 2, viewPort.height / 2 };
	sf::RenderWindow window(sf::VideoMode(1800, 1000), "yobaSolarSystem");
	window.setFramerateLimit(200);
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

	world.add(CelestialObject(vec2(300, 300), vec2(0, 0), 25, 1200));
	//world.add(CelestialObject(vec2(300, 980), vec2(3.1f, 0), 15, 735));
	//world.add(CelestialObject(vec2(300, 590), vec2(2.9f, 0), 10, 600));
	world.add(CelestialObject(vec2(300, 900), vec2(2.f, 0), 10, 705));

	float lastFt{ 0.f };
	float currentSlice{ 0.f };
	auto f = lua.load_file("update_lua_script.lua");
	lua.script_file("init_lua_script.lua");
	sf::Clock deltaClock;
	while (window.isOpen())
	{
		auto timePoint1(chrono::high_resolution_clock::now());
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
		currentSlice += lastFt;
		ImGui::SFML::Update(window, deltaClock.restart());
		//for (; currentSlice >= ftSlice; currentSlice -= ftSlice)
		//{
			f.call();
			//world.accelerationObjects(ftStep);
			world.moveObjects();
		//}

		view.reset(viewPort);
		view.setCenter(center);
		window.setView(view);
		window.clear();
		world.DrawingAllObjects(window);
		ImGui::SFML::Render(window);
		window.display();
		auto timePoint2(chrono::high_resolution_clock::now());
		auto elapsedTime(timePoint2 - timePoint1);
		float ft{ chrono::duration_cast<chrono::duration<float, milli>>(elapsedTime).count() };

		lastFt = ft;

		auto ftSeconds(ft / 1000.f);
		auto fps(1.f / ftSeconds);
		window.setTitle("FT: " + to_string(ft) + "\tFPS: " + to_string(fps));
	}

	return 0;
}
