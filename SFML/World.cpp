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
#include "Pickup.h"
#include "ParticleNode.h"

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

		destroyEntitiesOutOfView();

		guideMissiles();

		while(!commandQueue_.isEmpty())
		{
			sceneGraph_.onCommand(commandQueue_.pop(), dt);
		}

		handleCollisions();
		sceneGraph_.removeWrecks();

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


		///////////////////
		addEnemy(AircraftType::RAPTOR, -250.f, 1000.f);
		addEnemy(AircraftType::RAPTOR, 0.f, 1000.f);
		addEnemy(AircraftType::RAPTOR, 250.f, 1000.f);

		addEnemy(AircraftType::RAPTOR, -250.f, 1250.f);
		addEnemy(AircraftType::RAPTOR, 0.f, 1250.f);
		addEnemy(AircraftType::RAPTOR, 250.f, 1250.f);

		addEnemy(AircraftType::AVENGER, -70.f, 1800.f);
		addEnemy(AircraftType::AVENGER, 70.f, 1800.f);

		addEnemy(AircraftType::AVENGER, -70.f, 2500.f);
		addEnemy(AircraftType::AVENGER, 70.f, 2500.f);

		addEnemy(AircraftType::AVENGER, -170.f, 3250.f);
		addEnemy(AircraftType::AVENGER, 170.f, 3250.f);

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
			sceneLayers_[UpperAir]->attachChild(std::move(enemy));
			enemySpawnPoints_.pop_back();
		}

	}

	sf::FloatRect World::getViewBounds() const	{

		return sf::FloatRect(worldView_.getCenter() - worldView_.getSize() / 2.f, worldView_.getSize());
	}

	sf::FloatRect World::getBattlefieldBounds() const
	{
		sf::FloatRect bounds = getViewBounds();
		bounds.top -= 100.f;
		bounds.height += 100.f;
		return bounds;
	}

	void World::guideMissiles()
	{
		// build a list of active Enemies
		Command enemyCollector;
		enemyCollector.category = Category::EnemyAircraft;
		enemyCollector.action = derivedAction<Aircraft>([this](Aircraft& enemy, sf::Time dt)
		{
			if (!enemy.isDestroyed())
				activeEnemies_.push_back(&enemy);
		});

		Command missileGuider;
		missileGuider.category = Category::Type::AlliedProjectile;
		missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time dt) 
		{
			//ignore unguided bullets
			if (!missile.isGuided())
				return;

			float minDistance = std::numeric_limits<float>::max();
			Aircraft* closestEnemy = nullptr;

			for (Aircraft* e : activeEnemies_)
			{
				auto d = distance(missile, *e);
				if (d < minDistance) {
					minDistance = d;
					closestEnemy = e;
				}
			}

			if (closestEnemy)
				missile.guidedTowards(closestEnemy->getWorldPosition());

		});

		commandQueue_.push(enemyCollector);
		commandQueue_.push(missileGuider);
		activeEnemies_.clear();

	}

	bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2) {
		unsigned int category1 = colliders.first->getCategory();
		unsigned int category2 = colliders.second->getCategory();

		// Make sure first pair entry has category type1 and second has type2
		if (type1 & category1 && type2 & category2)
		{
			return true;
		}
		else if (type1 & category2 && type2 & category1)
		{
			std::swap(colliders.first, colliders.second);
			return true;
		}
		else
		{
			return false;
		}

			
	}

	void World::handleCollisions()
	{
		// build a list of colliding pairs
		std::set<SceneNode::Pair> collisionPairs;
		sceneGraph_.checkSceneCollision(sceneGraph_, collisionPairs);

		for (SceneNode::Pair pair : collisionPairs)
		{
			// Player and enemy collision
			if (matchesCategories(pair, Category::Type::PlayerAircraft, Category::Type::EnemyAircraft))
			{
				auto& player = static_cast<Aircraft&>(*pair.first);
				auto& enemy = static_cast<Aircraft&>(*pair.second);

				player.damage(enemy.getHitPoints());
				enemy.destroy();
			}
			// Player and pickup collision
			else if(matchesCategories(pair, Category::Type::PlayerAircraft, Category::Type::Pickup))
			{
				auto& player = static_cast<Aircraft&>(*pair.first);
				auto& pickup = static_cast<Pickup&>(*pair.second);

				pickup.apply(player);
				pickup.destroy();

			}
			// opposing aircraft and projectile collision
			else if (matchesCategories(pair, Category::Type::PlayerAircraft, Category::Type::EnemyProjectile) ||
				matchesCategories(pair, Category::EnemyAircraft, Category::AlliedProjectile))
			{
				auto& aircraft = static_cast<Aircraft&>(*pair.first);
				auto& projectile = static_cast<Projectile&>(*pair.second);

				aircraft.damage(projectile.getDamage());
				projectile.destroy();
			}

		}
	}

	void World::destroyEntitiesOutOfView()
	{
		Command command;
		command.category = Category::Type::Projectile | Category::Type::EnemyAircraft;
		command.action = derivedAction<Entity>([this](Entity& e, sf::Time dt)
		{
			if (!getBattlefieldBounds().intersects(e.getBoundingBox()))
				e.remove();

		});

		commandQueue_.push(command);
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

	bool World::hasALivePlayer() const
	{
		return !playerAircraft_->isDestroyed();
	}

	bool World::hasPlayerReachedEnd() const
	{
		return !worldBounds_.contains(playerAircraft_->getPosition());
	}

	void World::loadTextures()
	{		
		textures_.load(TextureID::Entities, "Media/Textures/Entities.png");
		textures_.load(TextureID::Jungle, "Media/Textures/JungleBig.png");	
		textures_.load(TextureID::Particle, "Media/Textures/Particle.png");
		textures_.load(TextureID::Explosion, "Media/Textures/Explosion.png");
		textures_.load(TextureID::FinishLine, "Media/Textures/FinishLine.png");

	}

	void World::buildScene()
	{

		// initialize layers
		for (int i = 0; i < LayerCount; ++i)
		{
			Category::Type category = (i == UpperAir) ? Category::Type::AirSceneLayer : Category::Type::None;
			SceneNode::Ptr layer(new SceneNode(category));
			sceneLayers_.push_back(layer.get());
			sceneGraph_.attachChild(std::move(layer));
		}

		// Particle Systems
		std::unique_ptr<ParticleNode> smoke(new ParticleNode(Particle::Type::Smoke, textures_));
		sceneLayers_[LowerAir]->attachChild(std::move(smoke));

		std::unique_ptr<ParticleNode> fire(new ParticleNode(Particle::Type::Propellant, textures_));
		sceneLayers_[LowerAir]->attachChild(std::move(fire));

		// background
		sf::Texture& texture = textures_.get(TextureID::Jungle);
		sf::IntRect textureRect(worldBounds_);
		texture.setRepeated(true);

		std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
		backgroundSprite->setPosition(worldBounds_.left, worldBounds_.top);
		background_ = backgroundSprite.get();
		sceneLayers_[Backround]->attachChild(std::move(backgroundSprite));

		// add player aircraft and game objects
		std::unique_ptr<Aircraft> leader(new Aircraft(AircraftType::EAGLE, textures_));
		leader->setPosition(spawnPosition_);
		leader->setVelocity(50.f, scrollSpeed_);
		playerAircraft_ = leader.get();
		sceneLayers_[UpperAir]->attachChild(std::move(leader));

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


