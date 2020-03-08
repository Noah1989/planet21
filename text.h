#define TEXT_LEFT 1
#define TEXT_WIDTH 38

#ifdef FONT8X8
#define TEXT_TOP 20
#define TEXT_HEIGHT 9
#endif

#ifdef FONT8X16
#define TEXT_TOP 9
#define TEXT_HEIGHT 5
#define TEXT_ODD 1
#endif

#define TEXT_RIGHT  (TEXT_LEFT + TEXT_WIDTH  - 1)
#define TEXT_BOTTOM (TEXT_TOP  + TEXT_HEIGHT - 1)

extern unsigned char TEXT_PALETTE_BASE;

void load_text_palettes(Color *base);
void load_text_border_patterns();
void draw_text_border();
