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
#include <memory>
#include <string>
#include "Utility.h"
#include "CommandQueue.h"
#include "World.h"

#include <iostream>

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
		, healthDisplay_(nullptr)
		, missileDisplay_(nullptr)
		, travelDistance_(0.f)
		, directionIndex_(0)
		, isFiring_(false)
		, fireRateLevel_(1)
		, fireSpreadLevel_(1)
		, missileAmmo_(12)
		, fireCountdown_(sf::Time::Zero)
		,	fireCommand_()
	{

		centerOrigin(sprite_);
		//
		//	Set Up Commands
		//
		fireCommand_.category = Category::AirSceneLayer;
		fireCommand_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			createBullets(node, textures);
		};

		launchMissileCommand_.category = Category::AirSceneLayer;
		launchMissileCommand_.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			createProjectiles(node, Projectile::Type::Missile, 0.f, 0.5f, textures);
		};
		

		std::unique_ptr<TextNode> health(new TextNode("GEx"));
		healthDisplay_ = health.get();
		attachChild(std::move(health));

		//std::unique_ptr<TextNode> missile(new TextNode(""));
		//missileDisplay_ = missile.get();
		//attachChild(std::move(missile));

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

	void Aircraft::createBullets(SceneNode & node, const TextureManager & textures)
	{
		Projectile::Type type = isAllied() ? Projectile::Type::AlliedBullet : Projectile::Type::EnemyBulletType;

		
		switch(fireSpreadLevel_)
		{
		case 1:
			createProjectiles(node, type, 0.f, 0.5f, textures);
			break;
		case 2:
			createProjectiles(node, type, -0.33f, 0.5f, textures);
			createProjectiles(node, type, 0.33f, 0.5f, textures);

			break;
		case 3:
			createProjectiles(node, type, -0.5f, 0.5f, textures);
			createProjectiles(node, type, 0.f, 0.5f, textures);
			createProjectiles(node, type, 0.5f, 0.5f, textures);
			break;
		}
	}

	void Aircraft::createProjectiles(SceneNode & node, Projectile::Type type, float xOffset, float yOffset, const TextureManager & textures)
	{
		// create projectile
		std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

		sf::Vector2f offset(xOffset * sprite_.getGlobalBounds().width, yOffset * sprite_.getGlobalBounds().height);
		sf::Vector2f velocity(0, projectile->getMaxSpeed());

		float sign = isAllied() ? -1.f : 1.f;
		//auto blah = getWorldPosition();		
		projectile->setPosition(getWorldPosition() + offset * sign);
		projectile->setVelocity(velocity * sign);
		
		node.attachChild(std::move(projectile));
		
		//std::cerr << "yes" << count++ << std::endl;
	}

	void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue & commands)
	{
		// Enemies are always firing
		if (!isAllied())
			fire();

		if(isFiring_ && fireCountdown_ <= sf::Time::Zero)
		{
			commands.push(fireCommand_);			
			fireCountdown_ = TABLE.at(type_).fireInterval / (fireRateLevel_ + 1.f);
			isFiring_ = false;
		}
		else if(fireCountdown_ > sf::Time::Zero)
		{
			fireCountdown_ -= dt;			
		}

		 //missile
		if(isLaunchingMissile_)
		{
			if (missileAmmo_ > 0) {
				commands.push(launchMissileCommand_);
				isLaunchingMissile_ = false;
				missileAmmo_--;
			}			
		}

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
			break;
		case AircraftType::RAPTOR:
			return Category::EnemyAircraft;
			break;
		default:
			return Category::None;
			break;
		}
	}

	void Aircraft::updateTexts()
	{
		healthDisplay_->setText(std::to_string(getHitPoints()) + "HP");		
		healthDisplay_->setPosition(0.f, 50.f);

		healthDisplay_->setRotation(-getRotation());
	}

	void Aircraft::fire()
	{
		if(TABLE.at(type_).fireInterval != sf::Time::Zero)
			isFiring_ = true;
	}

	void Aircraft::launchMissile()
	{
		isLaunchingMissile_ = true;
	}

	void Aircraft::increaseFireRate()
	{
		if (fireRateLevel_ < 10) 
		{
			++fireRateLevel_;
		}
	}

	void Aircraft::increaseFireSpread()
	{
		if (fireSpreadLevel_ < 3)
		{
			++fireSpreadLevel_;
		}
	}

	void Aircraft::collectMissiles(unsigned int count)
	{

		missileAmmo_ += count;
	}



	sf::FloatRect Aircraft::getBoundingBox() const
	{
		return getWorldTransform().transformRect(sprite_.getGlobalBounds());
	}

	bool Aircraft::isAllied() const
	{
		return type_ == AircraftType::EAGLE;
	}

	void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
	{
		updateMovementPattern(dt);
		Entity::updateCurrent(dt, commands);
		updateTexts();
		checkProjectileLaunch(dt, commands);
	}
}


