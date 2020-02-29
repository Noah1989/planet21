#define TEXT_LEFT    1
#define TEXT_TOP     10
#define TEXT_WIDTH  38
#define TEXT_HEIGHT 5
#define TEXT_RIGHT  (TEXT_LEFT + TEXT_WIDTH  - 1)
#define TEXT_BOTTOM (TEXT_TOP  + TEXT_HEIGHT - 1)

extern unsigned char TEXT_PALETTE_TOP;
extern unsigned char TEXT_PALETTE_BOT;

void load_text_palettes(Color *bg, Color *fg);
