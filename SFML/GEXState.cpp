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

#include "GEXState.h"
#include "Utility.h"
#include "FontManager.h"

GEXState::GEXState(GEX::StateStack & stack, Context context)
	: State(stack, context)
	, faceSprite_()
	, pauseText_()
	, GEXText_()
	, returnMenuText_()
	, returnGameText_()
{
	//sf::Font& font = *context.font;
	sf::Vector2f viewSize = context.window->getView().getSize();


	// initialize 4 text objects
	pauseText_.setFont(GEX::FontManager::getInstance().get(GEX::FontID::Main));
	pauseText_.setString("Game Paused");
	centerOrigin(pauseText_);

	GEXText_.setFont(GEX::FontManager::getInstance().get(GEX::FontID::Main));
	GEXText_.setString("GEX State");
	centerOrigin(GEXText_);

	returnMenuText_.setFont(GEX::FontManager::getInstance().get(GEX::FontID::Main));
	returnMenuText_.setString("Press backspace to return to menu");
	centerOrigin(returnMenuText_);

	returnGameText_.setFont(GEX::FontManager::getInstance().get(GEX::FontID::Main));
	returnGameText_.setString("Press G to return to game");
	centerOrigin(returnGameText_);

	
	// position 4 text objects
	pauseText_.setPosition(0.5f * viewSize.x, 0.2f * viewSize.y);
	GEXText_.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);
	returnMenuText_.setPosition(0.5f * viewSize.x, 0.6f * viewSize.y);
	returnGameText_.setPosition(0.5f * viewSize.x, 0.8f * viewSize.y);


	// load FACE image	
	faceSprite_.setTexture(context.textures->get(GEX::TextureID::Face));

	// set transparency (alpha)
	faceSprite_.setColor(sf::Color(255, 255, 255, 100));

	// get size of sprite
	sf::FloatRect faceSize = faceSprite_.getLocalBounds();

	// position sprite
	faceSprite_.setPosition(0.5f * viewSize.x - faceSize.width /2, 
		0.5f * viewSize.y - faceSize.height /2);
	
}

void GEXState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	/*c) draws three layers of objects*/

	/*i a red trasparent layer covering the entire view with alpha = 100*/
	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(200, 0, 0, 100));
	backgroundShape.setSize(window.getView().getSize());
	window.draw(backgroundShape);

	/*ii an image (Media/face.png) centered and transparent with alpha = 100*/
	window.draw(faceSprite_); // alpha set in constructor

			
	/*iii text objects "game paused", "GEX State", "Press backspace to return to menu", 
	 *and "Press G to return to game"*/
	window.draw(pauseText_);
	window.draw(GEXText_);
	window.draw(returnMenuText_);
	window.draw(returnGameText_);

}

bool GEXState::update(sf::Time dt)
{
	// b) pauses the game 
	return false;
}

bool GEXState::handleEvent(const sf::Event & event)
{	 
	if (event.type != sf::Event::KeyPressed)
		return false;


	//d) in Gex state "G" returns the player to the game
	if (event.key.code == sf::Keyboard::G)
		requestStackPop();

	//e) in Gex state backspace returns the player to the main menu
	if (event.key.code == sf::Keyboard::BackSpace)
	{
		requestStackClear();
		requestStackPush(GEX::StateID::Menu);
	}

	return false;
}
