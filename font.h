#define FONT8X16

#ifdef FONT8X8
extern const unsigned char font[96][8];
#endif

#ifdef FONT8X16
extern const unsigned char font[96][16];
#endif

void load_font();
