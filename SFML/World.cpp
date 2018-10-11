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

#include "World.h"
#include "Aircraft.h"

namespace GEX
{
	World::World(sf::RenderWindow& window)
	:window_(window)
	,worldView_(window.getDefaultView())
	,textures_()
	,sceneGraph_()
	,sceneLayers_()	
	,worldBounds_(0.f, 0.f, worldView_.getSize().x, 5000.f)		
	,spawnPosition_(worldView_.getSize().x /2.f, worldBounds_.height - worldView_.getSize().y / 2.f)
	,scrollSpeed_(-50.f)
	,playerAircraft_(nullptr)	
	{
		loadTextures();
		buildScene();
		
		// prepare the view
		worldView_.setCenter(spawnPosition_);
	}

	void World::update(sf::Time dt, CommandQueue& commands)
	{		
		// scroll the world
		worldView_.move(0.f, scrollSpeed_ * dt.asSeconds());

		playerAircraft_->setVelocity(0.f, 0.f);

		while(!commandQueue_.isEmpty())
		{
			sceneGraph_.onCommand(commandQueue_.pop(), dt);
		}

		adaptPlayerVelocity();
		sceneGraph_.update(dt, commands);
		adaptPlayerPosition();
		spawnEnemies();
	}

	void World::addEnemies()
	{
		addEnemy(AircraftType::RAPTOR, -250.f, 200.f);
		addEnemy(AircraftType::RAPTOR, 0.f, 200.f);
		addEnemy(AircraftType::RAPTOR, 250.f, 200.f);

		addEnemy(AircraftType::RAPTOR, -250.f, 600.f);
		addEnemy(AircraftType::RAPTOR, 0.f, 600.f);
		addEnemy(AircraftType::RAPTOR, 250.f, 600.f);

		addEnemy(AircraftType::AVENGER, -70.f, 400.f); 
		addEnemy(AircraftType::AVENGER, 70.f, 400.f);

		addEnemy(AircraftType::AVENGER, -70.f, 800.f);
		addEnemy(AircraftType::AVENGER, 70.f, 800.f);

		addEnemy(AircraftType::AVENGER, -170.f, 850.f);
		addEnemy(AircraftType::AVENGER, 170.f, 850.f);
		//addEnemy(AircraftType::RAPTOR, -250.f, 200.f);

		std::sort(enemySpawnPoints_.begin(), enemySpawnPoints_.end(), 
			[](SpawnPoint lhs, SpawnPoint rhs)
			{
				return lhs.y < rhs.y;
			});
	}

	void World::addEnemy(AircraftType type, float rel_x, float rel_y)
	{
		SpawnPoint spawnPoint(type, spawnPosition_.x + rel_x, spawnPosition_.y - rel_y);
		enemySpawnPoints_.push_back(spawnPoint);
	}

	void World::spawnEnemies()
	{
		while(!enemySpawnPoints_.empty() && 
			enemySpawnPoints_.back().y > getBattlefieldBounds().top)
		{
			auto spawnPoint = enemySpawnPoints_.back();
			std::unique_ptr<Aircraft> enemy(new Aircraft(spawnPoint.type, textures_));
			enemy->setPosition(spawnPoint.x, spawnPoint.y);
			enemy->setRotation(180);
			sceneLayers_[Air]->attachChild(std::move(enemy));
			enemySpawnPoints_.pop_back();
		}

	}

	sf::FloatRect World::getViewBounds() const
	{

		return sf::FloatRect(worldView_.getCenter() - worldView_.getSize() / 2.f, worldView_.getSize());
	}

	sf::FloatRect World::getBattlefieldBounds() const
	{
		sf::FloatRect bounds = getViewBounds();
		bounds.top -= 100.f;
		bounds.height += 100.f;
		return bounds;
	}

	void World::draw()
	{
		window_.setView(worldView_);
		window_.draw(sceneGraph_);
	}

	CommandQueue & World::getCommandQueue()
	{
		return commandQueue_;
		
	}

	void World::loadTextures()
	{
		textures_.load(TextureID::Eagle, "Media/Textures/Eagle.png");
		textures_.load(TextureID::Raptor, "Media/Textures/Raptor.png");
		textures_.load(TextureID::Avenger, "Media/Textures/Avenger.png");
		textures_.load(TextureID::Landscape, "Media/Textures/Desert.png");
	}

	void World::buildScene()
	{

		// initialize layers
		for (int i = 0; i < LayerCount; ++i)
		{
			SceneNode::Ptr layer(new SceneNode());
			sceneLayers_.push_back(layer.get());
			sceneGraph_.attachChild(std::move(layer));
		}

		//background
		sf::Texture& texture = textures_.get(TextureID::Landscape);
		sf::IntRect textureRect(worldBounds_);
		texture.setRepeated(true);

		std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
		backgroundSprite->setPosition(worldBounds_.left, worldBounds_.top);
		sceneLayers_[Backround]->attachChild(std::move(backgroundSprite));

		// add player aircraft and game objects
		std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::EAGLE, textures_));
		leader->setPosition(spawnPosition_);
		//leader->setVelocity(150.f, scrollSpeed_);
		playerAircraft_ = leader.get();
		sceneLayers_[Air]->attachChild(std::move(leader));

		// add enemy aircrafts
		addEnemies();

	}

	void World::adaptPlayerPosition()
	{
		// Keep player's position inside the screen bounds, at least borderDistance units from the border
		sf::FloatRect viewBounds(worldView_.getCenter() - worldView_.getSize() / 2.f, worldView_.getSize());
		const float borderDistance = 40.f;

		sf::Vector2f position = playerAircraft_->getPosition();
		position.x = std::max(position.x, viewBounds.left + borderDistance);
		position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
		position.y = std::max(position.y, viewBounds.top + borderDistance);
		position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
		playerAircraft_->setPosition(position);
	}


	void World::adaptPlayerVelocity()
	{
		sf::Vector2f velocity = playerAircraft_->getVelocity();

		// If moving diagonally, reduce velocity (to have always same velocity)
		if (velocity.x != 0.f && velocity.y != 0.f)
			playerAircraft_->setVelocity(velocity / std::sqrt(2.f));

		// Add scrolling velocity
		playerAircraft_->accelerate(0.f, scrollSpeed_);
	}





}


