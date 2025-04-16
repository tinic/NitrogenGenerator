#ifndef _SCREEN_H_
#define _SCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif  // #ifdef __cplusplus

void screen_init();

#ifdef __cplusplus
};
#endif  // #ifdef __cplusplus

#ifdef __cplusplus
#include <cstdint>

void screen_clear();
int32_t screen_draw_string(int32_t x, int32_t y, const char *str, bool calcWidthOnly = false);
void screen_flip();
#endif  // #ifdef __cplusplus

#endif  // #ifndef _SCREEN_H_
