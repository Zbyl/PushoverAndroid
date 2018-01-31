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

#include "leveldisplay.h"

#include "textsections.h"
#include "sha1.h"
#include "graphics.h"
#include "soundsys.h"
#include "screen.h"
#include "ant.h"

#include <stdio.h>

#include <sstream>
#include <iomanip>

void levelDisplay_c::load(const textsections_c & sections, const std::string & userString) {

  levelData_c::load(sections, userString);

  // attention don't use many levels on the same graphics object
  gr.setTheme(getTheme());

  Min = Sec = -1;

  dynamicallyModfied = true;  // Hack
}

levelDisplay_c::levelDisplay_c(surface_c & t, graphics_c & g) : background(t.getIdentical()), target(t), gr(g) {
  Min = Sec = -1;
}

levelDisplay_c::~levelDisplay_c(void) { }

void levelDisplay_c::prepareBackground(void)
{
  for (unsigned int y = 0; y < 13; y++)
  {
    for (unsigned int x = 0; x < 20; x++)
    {
      for (unsigned char b = 0; b < getNumBgLayer(); b++)
        background.blitBlock(gr.getBgTile(getBg(x, y, b)), x*gr.blockX(), y*gr.blockY());
      background.blitBlock(gr.getFgTile(getFg(x, y)), x*gr.blockX(), y*gr.blockY());

      background.gradient(gr.blockX()*x, gr.blockY()*y, gr.blockX(), gr.blockY());
    }
  }
}

void levelDisplay_c::drawDominos(void) {

  int timeLeft = getTimeLeft();

  {

    // calculate the second left
    int tm = timeLeft/18;

    // if negative make positive again
    if (timeLeft < 0)
      tm = -tm+1;

    int newSec = tm%60;
    int newMin = tm/60;

    Min = newMin;
    Sec = newSec;
  }

  if (dynamicallyModfied) {
    prepareBackground();
    dynamicallyModfied = false;
  }

  SDL_BlitSurface(background.getSurface(), NULL, target.getSurface(), NULL);

  static int XposOffset[] = {-16, -16,  0,-16,  0,  0, 0, 0, 0,  0, 0, 16,  0, 16, 16, 0};
  static int YposOffset[] = { -8,  -6,  0, -4,  0, -2, 0, 0, 0, -2, 0, -4,  0, -6, -8, 0};
  static int StoneImageOffset[] = {  7, 6, 0, 5, 0, 4, 0, 0, 0, 3, 0, 2, 0, 1, 0, 0};

  int SpriteYPos = gr.getDominoYStart();

  for (int y = 0; y < 13; y++, SpriteYPos += gr.blockY()) {

    int SpriteXPos = -2*gr.blockX();

    for (int x = 0; x < 20; x++, SpriteXPos += gr.blockX()) {

      // paint the splitting domino for the splitter
      if (getDominoType(x, y) == DominoTypeSplitter &&
          getDominoState(x, y) == 6 &&
          getDominoExtra(x, y) != 0)
      {
        target.blit(gr.getDomino(getDominoExtra(x, y)-1, getDominoExtra(x, y)>=DominoTypeCrash0?0:7),
            SpriteXPos,
            SpriteYPos-gr.splitterY());
        clearDominoExtra(x, y);
      }

      // paint the actual domino but take care of the special cases of the ascender domino
      if (getDominoType(x, y) == DominoTypeAscender && getDominoExtra(x, y) == 0x60 &&
          getDominoState(x, y) < 16 && getDominoState(x, y) != 8)
      {
        target.blit(gr.getDomino(DominoTypeRiserCont-1, StoneImageOffset[getDominoState(x, y)-1]),
            SpriteXPos+gr.convertDominoX(XposOffset[getDominoState(x, y)-1]),
            SpriteYPos+gr.convertDominoY(YposOffset[getDominoState(x, y)-1]+getDominoYOffset(x, y)));
      }
      else if (getDominoType(x, y) == DominoTypeAscender && getDominoState(x, y) == 1 && getDominoExtra(x, y) == 0 &&
          getFg(x-1, y-2) == FgElementEmpty)
      { // this is the case of the ascender domino completely horizontal and with the plank it is below not existing
        // so we see the above face of the domino. Normally there is a wall above us so we only see
        // the front face of the domino
        target.blit(gr.getDomino(DominoTypeRiserCont-1, StoneImageOffset[getDominoState(x, y)-1]),
            SpriteXPos+gr.convertDominoX(XposOffset[getDominoState(x, y)-1]+6),
            SpriteYPos+gr.convertDominoY(YposOffset[getDominoState(x, y)-1]+getDominoYOffset(x, y)));
      }
      else if (getDominoType(x, y) == DominoTypeAscender && getDominoState(x, y) == 15 && getDominoExtra(x, y) == 0 &&
          getFg(x+1, y-2) == FgElementEmpty)
      {
        target.blit(gr.getDomino(DominoTypeRiserCont-1, StoneImageOffset[getDominoState(x, y)-1]),
            SpriteXPos+gr.convertDominoX(XposOffset[getDominoState(x, y)-1]-2),
            SpriteYPos+gr.convertDominoY(YposOffset[getDominoState(x, y)-1]+getDominoYOffset(x, y)));
      }
      else if (getDominoType(x, y) != DominoTypeEmpty)
      {
        target.blit(gr.getDomino(getDominoType(x, y)-1, getDominoState(x, y)-1),
            SpriteXPos,
            SpriteYPos+gr.convertDominoY(getDominoYOffset(x, y)));
      }
    }
  }

  // repaint the ladders in front of dominos
  for (unsigned int y = 0; y < 13; y++)
    for (unsigned int x = 0; x < 20; x++) {
      {
        if (getFg(x, y) == FgElementPlatformLadderDown || getFg(x, y) == FgElementLadder) {
          SDL_Rect dst;
          dst.x = x*gr.blockX();
          dst.y = y*gr.blockY();
          dst.w = gr.blockX();
          dst.h = gr.blockY();
          target.blitBlock(gr.getFgTile(FgElementLadder2), dst.x, dst.y);
        }
        else if (getFg(x, y) == FgElementPlatformLadderUp)
        {
          SDL_Rect dst;
          dst.x = x*gr.blockX();
          dst.y = y*gr.blockY();
          dst.w = gr.blockX();
          dst.h = gr.blockY();
          target.blitBlock(gr.getFgTile(FgElementLadderMiddle), dst.x, dst.y);
        }
      }
    }

  if (timeLeft < 60*60*18)
  { // output the time
    char time[6];
    snprintf(time, 6, "%02i:%02i", Min, Sec);

    fontParams_s pars;
    if (timeLeft >= 0)
    {
      pars.color.r = pars.color.g = 255; pars.color.b = 0;
    }
    else
    {
      pars.color.r = 255; pars.color.g = pars.color.b = 0;
    }
    pars.font = FNT_BIG;
    pars.alignment = ALN_TEXT;
    pars.box.x = gr.timeXPos();
    pars.box.y = gr.timeYPos();
    pars.box.w = 50;
    pars.box.h = 50;
    pars.shadow = 1;

    target.renderText(&pars, time);
  }
}


