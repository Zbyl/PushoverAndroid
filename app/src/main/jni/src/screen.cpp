/* Pushover
 *
 * Pushover is the legal property of its developers, whose
 * names are listed in the COPYRIGHT file, which is included
 * within the source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335 USA
 */

#include "screen.h"

#include "graphics.h"

#include <SDL_ttf.h>

#include <iostream>
#include <vector>

screen_c::screen_c(const graphics_c & g) :
  animationState(0), blockX(g.blockX()), blockY(g.blockY()), showFullscreen(true)
{
  gWindow = SDL_CreateWindow("Zbyls Pushover", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
  if( gWindow == NULL )
  {
    SDL_Log( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
    throw std::exception();
  }

  video = SDL_CreateRGBSurfaceWithFormat(0, g.resolutionX(), g.resolutionY(), 24, SDL_GetWindowPixelFormat( gWindow ));
  if( video == NULL )
  {
    SDL_Log( "Surface could not be created! SDL Error: %s\n", SDL_GetError() );
    throw std::exception();
  }

  gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
  if( gRenderer == NULL )
  {
    SDL_Log( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
    throw std::exception();
  }

  //Initialize renderer color
  SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

  screenTexture = SDL_CreateTexture(gRenderer, SDL_GetWindowPixelFormat( gWindow ), SDL_TEXTUREACCESS_STREAMING, g.resolutionX(), g.resolutionY());
  if( screenTexture == NULL )
  {
    SDL_Log( "Screen texture could not be created! SDL Error: %s\n", SDL_GetError() );
    throw std::exception();
  }
}

screen_c::~screen_c(void) {
  SDL_DestroyTexture(screenTexture);
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  if (!showFullscreen) {
    video = NULL;
  }
}

surface_c::~surface_c(void)
{
  if (video) SDL_FreeSurface(video);
}

void screen_c::toggleFullscreen(void)
{
  //SDL_WM_ToggleFullScreen(video);
}

void surface_c::blit(SDL_Surface * s, int x, int y) {

  if (s && video)
  {
    SDL_Rect dst;

    dst.x = x;
    dst.y = y - s->h;
    dst.w = s->w;
    dst.h = s->h;

    SDL_BlitSurface(s, 0, video, &dst);
  }
}

void surface_c::fillRect(int x, int y, int w, int h, int r, int g, int b) {
  if (video)
  {
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    SDL_FillRect(video, &dst, SDL_MapRGB(video->format, r, g, b));
  }
}

void surface_c::blitBlock(SDL_Surface * s, int x, int y) {

  if (s && video)
  {
    SDL_Rect dst;

    dst.x = x;
    dst.y = y;
    dst.w = s->w;
    dst.h = s->h;

    SDL_BlitSurface(s, 0, video, &dst);
  }
}

void surface_c::copy(surface_c & s, int x, int y, int w, int h) {

  if (s.video && video)
  {
    SDL_Rect src, dst;
    src.x = dst.x = x;
    src.y = dst.y = y;
    src.w = dst.w = w;
    src.h = dst.h = h;
    SDL_BlitSurface(s.video, &src, video, &dst);
  }
}

void surface_c::gradient(int x, int y, int w, int h) {

  // make sure we only work with valid pixels
  if (y+h >= video->h) h = video->h-y;

  if (SDL_MUSTLOCK(video)) {
      if(SDL_LockSurface(video) != 0)
      {
        SDL_Log( "Surface could not be locked! SDL Error: %s\n", SDL_GetError() );
        throw std::exception();
      }
  }

  for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++) {
      uint32_t* pixelAddress = ((uint32_t*)(((uint8_t*)video->pixels) + (y+i) * video->pitch + video->format->BytesPerPixel*(x+j)));

      Uint8 r, g, b, a;

      SDL_GetRGBA(*pixelAddress, video->format, &r, &g, &b, &a);

      double val = (2.0-((1.0*x+j)/video->w + (1.0*y+i)/video->h));
      val += (1.0*rand()/RAND_MAX)/20 - 1.0/40;
      if (val < 0) val = 0;
      if (val > 2) val = 2;

      r = (Uint8)(((255.0-r)*val+r)*r/255);
      g = (Uint8)(((255.0-g)*val+g)*g/255);
      b = (Uint8)(((255.0-b)*val+b)*b/255);

      Uint32 color = SDL_MapRGBA(video->format, r, g, b, a);
      for (int k = 0; k < video->format->BytesPerPixel; ++k) {
        *((uint8_t*)pixelAddress + k) = *((uint8_t*)&color + k);
      }
    }

  if (SDL_MUSTLOCK(video)) {
      SDL_UnlockSurface(video);
  }
}

/**
 * Copies surface to texture.
 * Both must have the same dimensions and pixel format.
 */
void copySurfaceToTexture(SDL_Surface* surface, SDL_Texture* texture)
{
  if (SDL_MUSTLOCK(surface)) {
      if(SDL_LockSurface(surface) != 0)
      {
        SDL_Log( "Screen surface could not be locked! SDL Error: %s\n", SDL_GetError() );
        throw std::exception();
      }
  }

  void* mPixels;
  int mPitch; // ignoring that for now...
  SDL_LockTexture( texture, NULL, &mPixels, &mPitch );
  if (mPitch != surface->pitch) {
    SDL_Log("mPitch != surface->pitch");
    throw std::exception();
  }
  memcpy( mPixels, surface->pixels, surface->pitch * surface->h );

  //Unlock texture to update
  SDL_UnlockTexture( texture );

  if (SDL_MUSTLOCK(surface)) {
      SDL_UnlockSurface(surface);
  }
}

void screen_c::flipComplete(void)
{
  copySurfaceToTexture(video, screenTexture);
  //Clear screen
  //SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
  //SDL_RenderClear( gRenderer );
  SDL_RenderCopyEx(gRenderer, screenTexture, NULL, NULL, 0, NULL, SDL_FLIP_NONE);

  SDL_RenderPresent( gRenderer );

  animationState = 0;
}

static int clip(int v) {
  if (v < 0) return 0;
  if (v > 256) return 256;
  return v;
}

// a list of functions that return value between 0 and 255, depending on x and y
static int f1(int x, int y, int a) { return clip((13-y)*64 - 13*64 + a*((13*64+256)/64)); }
static int f2(int x, int y, int a) { return clip(x*32 - 20*32 + a*((32*20+256)/64)); }
static int f3(int x, int y, int a) { return clip(((2*y-12)*(2*y-12)+(2*x-19)*(2*x-19))*256/127 - 1024 + a*((1024+256)/64)); }

static SDL_Rect rects[13*20*255];
static int count;

static void u1(int x, int y, int f0, int f, int blx, int bly) {
  int by = bly*y;
  int bx = blx*x;
  int bw = f*blx/256;
  int bh = (y == 12) ? bly/2 : bly;

  if (bw > 0){
    rects[count].x=bx;
    rects[count].y=by;
    rects[count].w=bw;
    rects[count].h=bh;
    count++;
  }
}

static void u2(int x, int y, int f0, int f, int blx, int bly) {
  int by = bly*y;
  int bx = blx*x;
  int bw = blx;
  int bh = (y == 12) ? bly/2 : bly;

  bh = bh*f/256;

  if (bh > 0) {
    rects[count].x=bx;
    rects[count].y=by;
    rects[count].w=bw;
    rects[count].h=bh;
    count++;
  }
}

static void u3(int x, int y, int f0, int f, int blx, int bly) {
  int by = bly*y + bly/2 - bly/2*f/256;
  int bx = blx*x + blx/2 - blx/2*f/256;
  int bw = blx;
  int bh = (y == 12) ? bly/2 : bly;

  bh = bh*f/256;
  bw = bw*f/256;

  if (bh > 0) {
    rects[count].x=bx;
    rects[count].y=by;
    rects[count].w=bw;
    rects[count].h=bh;
    count++;
  }
}

static void u4(int x, int y, int f0, int f, int blx, int bly) {

  uint8_t rnd = (x+20*y) % 64;

  f0 /= 4;
  f /= 4;

  if (f0 == f) return;

  for (int i = 0; i <= f; i++)
  {
    if (i >= f0)
    {
      int xp = rnd % 8;
      int yp = rnd / 8;

      int by = bly*y+yp*bly/8;
      int bx = blx*x+xp*blx/8;
      int bw = blx/8;
      int bh = bly/8;

      if (bh > 0 && by < 600) {
        rects[count].x=bx;
        rects[count].y=by;
        rects[count].w=bw;
        rects[count].h=bh;
        count++;
      }
    }

    rnd = (21*rnd+11) % 64;
  }
}

bool screen_c::flipAnimate(void)
{
  static int (*f)(int, int, int);
  static void (*u)(int, int, int, int, int, int);

  if (animationState == 0) {
    switch (rand()%3) {
      case 0: f = f1; break;  // up down sweep
      case 1: f = f2; break;  // right to left sewep
      case 2: f = f3; break;  // circular outside in
      default: f = f3; break;
    }
    switch (rand()%4) {
      case 0: u = u1; break;  // left --> right update
      case 1: u = u2; break;  // up --> down update
      case 2: u = u3; break;  // inside out update
      case 3: u = u4; break;  // diffuse update
      default: u = u3; break;
    }
  }


  animationState++;
  count=0;

  for (int y = 0; y < 13; y++)
  {
    for (int x = 0; x < 20; x++)
    {
      int valNew = f(x, y, animationState);
      int valOld = f(x, y, animationState-1);

      if (valNew != valOld)
      {
        u(x, y, valOld, valNew, blockX, blockY);
      }
    }
  }

  // @todo Update only count rectangles from rects array.
  int saveAnimationState = animationState;
  flipComplete();
  animationState = saveAnimationState;
  animationState = 64;

  if (animationState == 64)
  {
    animationState = 0;
    return true;
  }

  return false;
}

SDL_Surface * surface_c::getIdentical(void) const {

  if (!video)
    return 0;

  SDL_Surface* newSurface = SDL_CreateRGBSurfaceWithFormat(0, video->w, video->h, 32, video->format->format);
  if (!newSurface) {
      SDL_Log( "Identical surface could not be created! SDL Error: %s\n", SDL_GetError() );
      throw std::exception();
  }
  return newSurface;
}

static std::vector<TTF_Font * > fonts;


void initText(std::string dir) {

  std::string fname = dir+"/data/FreeSans.ttf";

  if (TTF_Init() == -1) {
    std::cout << "Can't initialize font engine" << std::endl;
    exit(1);
  }

  TTF_Font * ft;

  ft = TTF_OpenFont(fname.c_str(), 18);
  if (!ft) {
    std::cout << "Can't open Font file: " << fname << std::endl;
    exit(1);
  }

  fonts.push_back(ft);

  ft = TTF_OpenFont(fname.c_str(), 30);
  if (!ft) {
    std::cout << "Can't open Font file: " << fname << std::endl;
    exit(1);
  }

  fonts.push_back(ft);

  ft = TTF_OpenFont(fname.c_str(), 35);
  if (!ft) {
    std::cout << "Can't open Font file: " << fname << std::endl;
    exit(1);
  }

  fonts.push_back(ft);
}

void deinitText(void) {

  for (unsigned int i = 0; i < fonts.size(); i++)
    TTF_CloseFont(fonts[i]);

  TTF_Quit();
}

static std::vector<std::string> split(const std::string & text, char splitter)
{
  std::string current;
  std::vector<std::string> res;

  for (unsigned int i = 0; i < text.length(); i++)
  {
    if (text[i] == splitter)
    {
      res.push_back(current);
      current = "";
    }
    else
    {
      current += text[i];
    }
  }

  if (current.length()) res.push_back(current);

  return res;
}

unsigned int surface_c::renderText(const fontParams_s * par, const std::string & t)
{
  // make some safety checks, empty strings are not output
  bool onlySpace = true;
  for (size_t i = 0; i < t.length(); i++)
    if (t[i] != ' ')
    {
      onlySpace = false;
      break;
    }

  if (   (t.length() == 0)
      || (onlySpace)
     )
  {
    return 1;
  }

  std::vector<std::string> words = split(t.c_str(), ' ');;

  int ypos = par->box.y;

  if (par->alignment == ALN_CENTER)
    ypos += (par->box.h-getTextHeight(par, t))/2;

  unsigned int word = 0;
  unsigned int lines = 0;

  while (word < words.size()) {

    std::string curLine = words[word];
    word++;
    lines++;

    while (word < words.size())
    {
      int w;
      TTF_SizeUTF8(fonts[par->font], (curLine+words[word]).c_str(), &w, 0);

      if (w > par->box.w) break;

      curLine = curLine + " " + words[word];
      word++;
    }

    SDL_Surface * vv = TTF_RenderUTF8_Blended(fonts[par->font], curLine.c_str(), par->color);
    SDL_Surface * vb = NULL;

    if (par->shadow)
    {
      SDL_Color bg;
      bg.r = bg.g = bg.b = 0;
      vb = TTF_RenderUTF8_Blended(fonts[par->font], curLine.c_str(), bg);
    }

    if (par->alignment == ALN_TEXT) {

      SDL_Rect r = par->box;
      r.w = vv->w;
      r.h = vv->h;
      r.y = ypos;

      if (par->shadow == 1)
      {
        int sa = 1;
        if (par->font == FNT_BIG) sa = 2;

        r.x-=sa; r.y-=sa; SDL_BlitSurface(vb, 0, video, &r);
        r.x+=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.x+=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.y+=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.y+=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.x-=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.x-=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.y-=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.x+=sa;
      }
      else if (par->shadow == 2)
      {
        int sa = 1;

        r.x+=sa; r.y+=sa; SDL_BlitSurface(vb, 0, video, &r);
        r.x-=sa; r.y-=sa;
      }
      SDL_BlitSurface(vv, 0, video, &r);
    }
    else if (par->alignment == ALN_TEXT_CENTER || par->alignment == ALN_CENTER) {

      SDL_Rect r;

      r.x = par->box.x + (par->box.w - vv->w)/2;
      r.y = ypos;

      r.w = vv->w;
      r.h = vv->h;

      if (par->shadow == 1)
      {
        int sa = 1;
        if (par->font == FNT_BIG) sa = 2;

        r.x-=sa; r.y-=sa; SDL_BlitSurface(vb, 0, video, &r);
        r.x+=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.x+=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.y+=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.y+=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.x-=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.x-=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.y-=sa;          SDL_BlitSurface(vb, 0, video, &r);
        r.x+=sa;
      }
      else if (par->shadow == 2)
      {
        int sa = 1;

        r.x+=sa; r.y+=sa; SDL_BlitSurface(vb, 0, video, &r);
        r.x-=sa; r.y-=sa;
      }
      SDL_BlitSurface(vv, 0, video, &r);
    }

    ypos += vv->h;

    SDL_FreeSurface(vv);
    if (par->shadow) SDL_FreeSurface(vb);
  }

  return lines;
}

int getFontHeight(unsigned int font) {
  if (font < fonts.size())
    return TTF_FontLineSkip(fonts[font]);
  else
    return 0;
}

int getTextWidth(unsigned int font, const std::string & t) {
  int w = 0;
  TTF_SizeUTF8(fonts[font], t.c_str(), &w, 0);

  return w;
}

int getTextHeight(const fontParams_s * par, const std::string & t) {

  std::vector<std::string> words = split(t.c_str(), ' ');

  unsigned int word = 0;
  int height = 0;

  while (word < words.size()) {

    std::string curLine = words[word];
    word++;

    while (word < words.size())
    {
      int w;
      TTF_SizeUTF8(fonts[par->font], (curLine+words[word]).c_str(), &w, 0);

      if (w > par->box.w) break;

      curLine = curLine + " " + words[word];
      word++;
    }

    int h, w;
    TTF_SizeUTF8(fonts[par->font], curLine.c_str(), &w, &h);

    height += h;
  }

  return height;
}
