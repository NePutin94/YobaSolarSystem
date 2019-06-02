#include <thread>
#include <future>
#include <mutex>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <random>
using namespace std;
using namespace sf;
sf::Texture tex1;
sf::Texture tex2;
sf::Texture tex3;

sf::Vector2f GetIntersectionDepth(const sf::FloatRect& rectA, const FloatRect& rectB)
{
	float halfWidthA = rectA.width / 2.0f;
	float halfHeightA = rectA.height / 2.0f;
	float halfWidthB = rectB.width / 2.0f;
	float halfHeightB = rectB.height / 2.0f;

	Vector2f centerA(rectA.left + halfWidthA, rectA.top + halfHeightA);
	Vector2f centerB(rectB.left + halfWidthB, rectB.top + halfHeightB);

	float distanceX = centerA.x - centerB.x;
	float distanceY = centerA.y - centerB.y;
	float minDistanceX = halfWidthA + halfWidthB;
	float minDistanceY = halfHeightA + halfHeightB;

	if (abs(distanceX) >= minDistanceX || abs(distanceY) >= minDistanceY)
		return Vector2f(0, 0);

	float depthX = distanceX > 0 ? minDistanceX - distanceX : -minDistanceX - distanceX;
	float depthY = distanceY > 0 ? minDistanceY - distanceY : -minDistanceY - distanceY;

	return Vector2f(depthX, depthY);
}
class CelestialObject
{
public:
	sf::CircleShape obj;
	Vector2f acceleration;
	int r, m;

	CelestialObject()
	{
		acceleration = { 0,0 };
		r = 0;
		m = 0;
	}
	CelestialObject(Vector2f _pos, Vector2f _vel, int _r, int _m) : acceleration(_vel), r(_r), m(_m)
	{
		obj.setPosition(_pos);
		obj.setRadius(r);
		obj.setOrigin(sf::Vector2f(r / 2, r / 2));
		obj.setTexture(&tex1);
		obj.setTextureRect(sf::IntRect(0, 0, 792, 792));
		//obj.setFillColor(sf::Color::Red);
		r = _r;
		m = _m;
	}
	void addedAcceleration(sf::Vector2f a)
	{
		acceleration = acceleration + a;
	}
	operator Drawable& ()
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
	void addedPosition(Vector2f vector)
	{
		obj.move(vector);
	}
};
Vector2f operator * (const Vector2f& vector, const Vector2f& vector2)
{
	sf::Vector2f dot;
	dot.x = vector2.x * vector.x;
	dot.y = vector2.y * vector.y;
	return dot;
}
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
			//if (o1->m < o2->m)
			//{
			//	auto intersec = GetIntersectionDepth(o1->obj.getGlobalBounds(), o2->obj.getGlobalBounds());
			//	if (abs(intersec.x) > abs(intersec.y))
			//		intersec.x = 0;
			//	else
			//		intersec.y = 0;
			//	o1->obj.setPosition(o1->obj.getPosition() + intersec);
			//}
			//else
			//{
			//	auto intersec = GetIntersectionDepth(o2->obj.getGlobalBounds(), o1->obj.getGlobalBounds());
			//	if (abs(intersec.x) > abs(intersec.y))
			//		intersec.x = 0;
			//	else
			//		intersec.y = 0;
			//	o2->obj.setPosition(o2->obj.getPosition() + intersec);
			//}
			return 0.0000001;
		}
		return (o1->m * o2->m / pow(distance, 2)) / 60000;
	}
	void add(CelestialObject object)
	{
		objectsArray.push_back(new CelestialObject(object));
	}
	void accelerationObjects()
	{
		bool collision = false;

		for (auto iter = objectsArray.begin(); iter != objectsArray.end(); iter++)
		{
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
			if ((*iter)->obj.getPosition().x > 2000)
				(*iter)->addedAcceleration({ -0.08,0 });
			if ((*iter)->obj.getPosition().y > 2000)
				(*iter)->addedAcceleration({ 0,-0.08 });

			if ((*iter)->obj.getPosition().x < -2000)
				(*iter)->addedAcceleration({ 0.08,0 });
			if ((*iter)->obj.getPosition().y < -2000)
				(*iter)->addedAcceleration({ 0,0.08 });

			for (auto iter2 = iter + 1; iter2 != objectsArray.end(); iter2++)
			{

				float fG = ForceOfGravity(collision, *iter, *iter2);//некая сила с которой йобы действуют друг на друга
				float distance = sqrt(pow((*iter2)->obj.getPosition().x - (*iter)->obj.getPosition().x, 2) + pow((*iter2)->obj.getPosition().y - (*iter)->obj.getPosition().y, 2));
				//Отношение их масс:
				float massJtoI = ((float)(*iter2)->m / (*iter)->m);
				float massItoJ = ((float)(*iter)->m / (*iter2)->m);

				//конечная сила их взаимодействия сила 
				float fGmassJtoI = fG * pow(massJtoI, 2) * sqrt(distance);
				float fGmassItoJ = fG * pow(massItoJ, 2) * sqrt(distance);

				Vector2f direction(
					(*iter)->obj.getPosition().x > (*iter2)->obj.getPosition().x ? -1 : 1,
					(*iter)->obj.getPosition().y > (*iter2)->obj.getPosition().y ? -1 : 1);
				if (collision)
				{
					Vector2f direction2(
						(*iter)->obj.getPosition().x < (*iter2)->obj.getPosition().x ? -0.5 : 2,
						(*iter)->obj.getPosition().y < (*iter2)->obj.getPosition().y ? -2 : 0.5);
					if ((*iter2)->m > (*iter)->m)
					{
						(*iter)->addedAcceleration(Vector2f(0.01, 0.01) * direction2);

						////Change big object
						float newR = (*iter)->r * massItoJ;
						float newMass = (*iter)->m * 0.5f;
						//(*iter2)->addMass((*iter)->m * 0.5f);
						//(*iter2)->addRadius((*iter)->r * massItoJ);
						//(*iter)->addRadius(-newR);
						//(*iter)->addMass(-newMass);
						////Delete small
						//delete objectsArray[i];
						//objectsArray.erase(objectsArray.begin() + i);
					}
					else
					{
						(*iter2)->addedAcceleration(Vector2f(0.01, 0.01) * -direction2);
						float newR = (*iter2)->r * massItoJ;
						float newMass = (*iter2)->m * 0.5f;
						////Change big object
						//(*iter)->addMass(newMass);
						//(*iter)->addRadius(newR);
						//(*iter2)->addRadius(-newR);
						//(*iter2)->addMass(-newMass);
						////Delete small
						//delete objectsArray[j];
						//objectsArray.erase(objectsArray.begin() + j);
					}
				}
				if (!collision)
				{
					if ((*iter2)->m > (*iter)->m)
						(*iter)->addedAcceleration(Vector2f(fGmassJtoI, fGmassJtoI) * direction);
					else
						(*iter2)->addedAcceleration(Vector2f(fGmassItoJ, fGmassItoJ) * -direction);
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
		{
			w.draw(*object);
		}
	}
};

int main()
{
	tex1.loadFromFile("yoba3.png");
	tex2.loadFromFile("yoba2.png");
	tex3.loadFromFile("yoba4.png");
	World gravity;
	sf::View view;
	sf::FloatRect viewPort = { 0,0,1800,1000 };
	view.reset(viewPort);
	sf::RenderWindow window(sf::VideoMode(1800, 1000), "yobaSolarSystem");
	window.setFramerateLimit(100);
	srand(time(0));
	sf::Vector2f center = { viewPort.width / 2,viewPort.height / 2 };

	/*for (int i = 1; i < 4; i++)
	{
		int r = rand() % 40 + 15;
		int m = r * 40;
		Vector2f position(100 + rand() % 1000, 100 + rand() % 800);
		Vector2f direction(1 - rand() % 2, (1 - rand() % 2));
		gravity.add(CelestialObject(position, direction, r, m));
	}*/
	gravity.add(CelestialObject(Vector2f(300, 300), Vector2f(0, 0), 25, 1200));
	gravity.add(CelestialObject(Vector2f(300, 484), Vector2f(2.2f, 0), 15, 735));
	gravity.add(CelestialObject(Vector2f(300, 580), Vector2f(1.f, 0), 10, 235));
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
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

		gravity.accelerationObjects();
		gravity.moveObjects();
		view.reset(viewPort);
		view.setCenter(center);
		window.setView(view);
		window.clear();
		gravity.DrawingAllObjects(window);
		window.display();
	}

	return 0;
}
