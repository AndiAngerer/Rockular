// RocketLauncherDriver.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "RocketLauncherDeviceSim.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <list>
#include <utility>

using namespace rocketlauncher;
using namespace std;

int main()
{
	list<tuple<Direction, Direction, int>> inputs;

	inputs.emplace_back(Direction::POSITIVE, Direction::POSITIVE, 1);
	inputs.emplace_back(Direction::NEGATIVE, Direction::NEGATIVE, 8);
	inputs.emplace_back(Direction::POSITIVE, Direction::POSITIVE, 15);
	inputs.emplace_back(Direction::NEGATIVE, Direction::NEGATIVE, 5);
	inputs.emplace_back(Direction::POSITIVE, Direction::NEGATIVE, 3);

	RocketLauncherDeviceSim *sim = new RocketLauncherDeviceSim();

	for (auto it = inputs.cbegin(); it != inputs.cend(); ++it)
	{
		Direction pan = get<0>(*it);
		Direction tilt = get<1>(*it);
		int dur = get<2>(*it);

		sim->setMovement(pan, tilt);

		for (int i = 0; i < dur; ++i)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << "Estimated pan:" << sim->getEstimatedPanPosition();
			std::cout << " Estimated tilt:" << sim->getEstimatedTiltPosition() << std::endl;
		}

		sim->setMovement(Direction::NONE, Direction::NONE);
		cout << "Press key to issue next input..." << endl;
		std::cin.get();
	}

	cout << "Press key to terminate..." << endl;
	std::cin.get();

	delete sim;

	return 0;
}


