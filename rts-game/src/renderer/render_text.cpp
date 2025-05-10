#include "render_text.h"

#include "renderer.h"

#include "SpriteBatch.h"
#include "SpriteFont.h"

std::unique_ptr<SpriteFont> debug_font;
std::unique_ptr<SpriteBatch> sprite_batch;

void load_font() {
	debug_font = std::make_unique<SpriteFont>(device, L"../resources/fonts/courier.spritefont");
	sprite_batch = std::make_unique<SpriteBatch>(context);
}

void draw_debug_text(const wchar_t* str) {
	assert(sprite_batch.get());
	assert(debug_font.get());

	XMFLOAT2 font_pos {500.f, 100.f};

	sprite_batch->Begin();

	XMVECTOR str_size = debug_font->MeasureString(str);
	float f = 2.0;
	XMVectorDivide(str_size, XMVectorReplicate(2.f));
	XMFLOAT2 origin;
	XMStoreFloat2(&origin, str_size);

	debug_font->DrawString(sprite_batch.get(), str, font_pos, Colors::Black, 0.f, origin, 1.0);

	sprite_batch->End();
}
