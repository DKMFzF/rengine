#pragma once

#include "utils.hpp"

class Texture {
public:
	Texture(const std::filesystem::path& filePath);

	void bind() const noexcept;
	void unbind() const noexcept;

private:
	GlTexture m_texture;

};

