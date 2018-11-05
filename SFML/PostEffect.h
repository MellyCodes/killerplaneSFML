#pragma once
#include <SFML/System/NonCopyable.hpp>

namespace sf
{
	class RenderTarget;
	class RenderTexture;
	class Shader;
}
namespace GEX
{
	class PostEffect : sf::NonCopyable
	{

	public:
		virtual						~PostEffect() = default;
									/*PostEffect(PostEffect& pe) = delete;
		PostEffect&					operator=(const PostEffect&) = delete;*/

		virtual void				apply(const sf::RenderTexture& input, sf::RenderTarget& output) = 0;

		static bool					isSupported(); // Does the GPU support shaders??

	protected:

		static void					applyShader(const sf::Shader& shader, sf::RenderTarget& output);

		
	};
}


