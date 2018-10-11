/**
* @file
* @author Melanie Roy-Plommer
* @version 1.0
*
* @section DESCRIPTION
* <  >
*
* @section LICENSE
* Copyright 2018 - 2019
* Permission to use, copy, modify, and/or distribute this software for
* any purpose with or without fee is hereby granted, provided that the
* above copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
* @section Academic Integrity
* I certify that this work is solely my own and complies with
* NBCC Academic Integrity Policy (policy 1111)
*/


#include "PlayerControl.h"
#include "Aircraft.h"
#include "Command.h"
#include "CommandQueue.h"
#include "Category.h"
#include "World.h"

namespace GEX
{
	struct AircraftMover
	{
	public:
		AircraftMover(float vx, float vy)
			: velocity(vx, vy)
		{}


		void operator()(Aircraft& aircraft, sf::Time dt)const
		{
			aircraft.accelerate(velocity);
		}

		sf::Vector2f velocity;

	};

	struct AircraftRotater
	{
	public:
		AircraftRotater(float r)
			: rotation(r)
		{}


		void operator()(Aircraft& aircraft, sf::Time dt)const
		{
			aircraft.rotate(rotation);
		}

		float rotation;

	};

	PlayerControl::PlayerControl()
	{
		//set up bindings
		keyBindings_[sf::Keyboard::Left] = Action::MOVE_LEFT;
		keyBindings_[sf::Keyboard::A] = Action::MOVE_LEFT;

		keyBindings_[sf::Keyboard::Right] = Action::MOVE_RIGHT;
		keyBindings_[sf::Keyboard::D] = Action::MOVE_RIGHT;

		keyBindings_[sf::Keyboard::Up] = Action::MOVE_UP;
		keyBindings_[sf::Keyboard::W] = Action::MOVE_UP;

		keyBindings_[sf::Keyboard::Down] = Action::MOVE_DOWN;
		keyBindings_[sf::Keyboard::S] = Action::MOVE_DOWN;

		//
		keyBindings_[sf::Keyboard::Space] = Action::FIRE;
		keyBindings_[sf::Keyboard::M] = Action::LAUNCH_MISSILE;

		//set up action bindings
		initializeActions();

		for (auto& pair : actionBindings_)
		{
			pair.second.category = Category::PlayerAircraft;
		}
	}

	void PlayerControl::handleEvent(const sf::Event & event, CommandQueue & commands)
	{

		if (event.type == sf::Event::KeyPressed)
		{
			// Check if pressed key appears in key binding, trigger command if so
			auto found = keyBindings_.find(event.key.code);
			if (found != keyBindings_.end())
				commands.push(actionBindings_[found->second]);
		}
	}

	void PlayerControl::handleRealTimeInput(CommandQueue & commands)
	{
		// Traverse all assigned keys and check if they are pressed
		for (auto pair : keyBindings_)
		{
			// If key is pressed, lookup action and trigger corresponding command
			if (sf::Keyboard::isKeyPressed(pair.first) && isRealTimeAction(pair.second))
				commands.push(actionBindings_[pair.second]);

			//
		}
	}

	void PlayerControl::initializeActions()
	{
		const float playerSpeed = 200.f;

		// Movements

		actionBindings_[Action::MOVE_LEFT].action = derivedAction<Aircraft>(AircraftMover(-playerSpeed, 0.f));

		actionBindings_[Action::MOVE_RIGHT].action = derivedAction<Aircraft>(AircraftMover(+playerSpeed, 0.f));

		actionBindings_[Action::MOVE_UP].action = derivedAction<Aircraft>(AircraftMover(0.f, -playerSpeed));

		actionBindings_[Action::MOVE_DOWN].action = derivedAction<Aircraft>(AircraftMover(0.f, +playerSpeed));

		// Fire and Launch Missile

		actionBindings_[Action::FIRE].action = derivedAction<Aircraft>(std::bind(&Aircraft::fire, std::placeholders::_1));
		actionBindings_[Action::FIRE].category = Category::Type::PlayerAircraft;


		actionBindings_[Action::LAUNCH_MISSILE].action = derivedAction<Aircraft>(std::bind(&Aircraft::launchMissile, std::placeholders::_1));
		actionBindings_[Action::LAUNCH_MISSILE].category = Category::Type::PlayerAircraft;

	}

	bool PlayerControl::isRealTimeAction(Action action)
	{
		switch (action)
		{
		case Action::MOVE_LEFT:
		case Action::MOVE_RIGHT:
		case Action::MOVE_DOWN:
		case Action::MOVE_UP:
		case Action::ROLL_LEFT:
		case Action::ROLL_RIGHT:
			return true;
		default:
			return false;
		}
	}



}




