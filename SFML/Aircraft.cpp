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

#include "Aircraft.h"
#include "Category.h"
#include "DataTables.h"
#include "TextNode.h"
#include <string>
#include "Utility.h"
#include "CommandQueue.h"

namespace GEX
{
	namespace
	{
		const std::map<AircraftType, AircraftData> TABLE = initializeAircraftData();
	}

	Aircraft::Aircraft(AircraftType type, TextureManager & textures)
		: Entity(TABLE.at(type).hitpoint)
		, type_(type)
		, sprite_(textures.get(TABLE.at(type).texture))
	{
		sf::FloatRect bounds = sprite_.getLocalBounds();
		sprite_.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

		std::unique_ptr<TextNode> health(new TextNode("GEx"));
		healthDisplay_ = health.get();
		attachChild(std::move(health));

	}

	void Aircraft::updateMovementPattern(sf::Time dt)
	{
		// movement pattern
		const std::vector<Direction>& directions = TABLE.at(type_).directions;

		if(!directions.empty())
		{
			if(travelDistance_ > (directions[directionIndex_].distance))
			{
				directionIndex_ = (++directionIndex_) % directions.size();
				travelDistance_ = 0;
			}

			float radians = toRadian(directions[directionIndex_].angle + 90.f);
			float vx = getMaxSpeed() * std::cos(radians);
			float vy = getMaxSpeed() * std::sin(radians);

			setVelocity(vx, vy);
			travelDistance_ += getMaxSpeed() * dt.asSeconds();
		}
	}

	float Aircraft::getMaxSpeed() const
	{
		return TABLE.at(type_).speed;
	}

	void Aircraft::drawCurrent(sf::RenderTarget & target, sf::RenderStates states)const
	{
		target.draw(sprite_, states);
	}

	unsigned int Aircraft::getCategory() const
	{
		switch (type_)
		{
		case AircraftType::EAGLE:
			return Category::PlayerAircraft;

		default:
			return Category::EnemyAircraft;
		}
	}

	void Aircraft::updateTexts()
	{
		healthDisplay_->setText(std::to_string(getHitPoints()) + "HP");		
		healthDisplay_->setPosition(0.f, 50.f);

		healthDisplay_->setRotation(-getRotation());
	}

	void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
	{
		updateMovementPattern(dt);
		Entity::updateCurrent(dt, commands);
		updateTexts();
	}
}


