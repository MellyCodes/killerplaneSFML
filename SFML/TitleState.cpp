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

#include "TitleState.h"
#include "Utility.h"
#include "FontManager.h"

TitleState::TitleState(GEX::StateStack & stack, Context context)
	:	State(stack, context)
	,	text_()
	,	showText_(true)
	,	textEffectTime_(sf::Time::Zero)
{
	backgroundSprite_.setTexture(context.textures->get(GEX::TextureID::TitleScreen));

	text_.setFont(GEX::FontManager::getInstance().get(GEX::FontID::Main));
	text_.setString("Press any key to Start");

	centerOrigin(text_);

	text_.setPosition(context.window->getView().getSize() / 2.f);
}

void TitleState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.draw(backgroundSprite_);

	if(showText_)
		window.draw(text_);

}

bool TitleState::update(sf::Time dt, GEX::CommandQueue& commands)
{
	textEffectTime_ += dt;

	if(textEffectTime_ >= sf::seconds(0.5f))
	{
		showText_ = !showText_;
		textEffectTime_ = sf::Time::Zero;
	}

	return true;
}

bool TitleState::handleEvent(const sf::Event & event)
{

	if (event.type == sf::Event::KeyPressed)
	{
		requestStackPop();
		requestStackPush(GEX::StateID::Menu);
	}



	return true;
}
