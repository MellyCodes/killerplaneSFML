#include "GameOverState.h"
#include "FontManager.h"
#include "Utility.h"
#include "GameState.h"

GameOverState::GameOverState(GEX::StateStack & stack, Context context)
	:	State(stack, context)
	,	gameOverText_()
	,	elapsedTime_(sf::Time::Zero)
{
	sf::Font& font = GEX::FontManager::getInstance().get(GEX::FontID::Main);
	sf::Vector2f windowSize(context.window->getSize());

	gameOverText_.setFont(font);
	if (context.player->getMissionStatus() == GEX::MissionStatus::MissionFailure)  // MissionStatus needs implementing
	{
		gameOverText_.setString("Mission Failure");
	}
	else 
	{
		gameOverText_.setString("Mission Successful");
	}

	gameOverText_.setCharacterSize(70);
	centerOrigin(gameOverText_);
	gameOverText_.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);

}

void GameOverState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(gameOverText_);
}

bool GameOverState::update(sf::Time dt)
{
	elapsedTime_ += dt;
	if(elapsedTime_ > sf::seconds(3))
	{
		requestStackClear();
		requestStackPush(GEX::StateID::Menu);
	}
	return false;
}

bool GameOverState::handleEvent(const sf::Event & event)
{
	return false;
}
