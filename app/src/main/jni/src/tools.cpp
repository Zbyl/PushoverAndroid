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

#include "tools.h"

#include <SDL.h>

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <lmcons.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <dirent.h>

#include <stdexcept>

#if defined(__AROS__)
#include <proto/locale.h>
#include <proto/dos.h>
#endif

uint64_t getTime(void)
{
#ifdef WIN32
  FILETIME currentTime;
  GetSystemTimeAsFileTime(&currentTime);
  return currentTime.dwLowDateTime;
#else
  return time(0);
#endif
}


void srandFromTime(void) {

  srand(getTime());
}

std::string getHome() {
  char* prefDir = SDL_GetPrefPath("it.skowron.zbyl", "pushover");
  std::string prefDirString(prefDir);
  SDL_free(prefDir);
  return prefDirString;
}

const char* levels[] = {
  "001.level",
  "002.level",
  "003.level",
  "004.level",
  "005.level",
  "006.level",
  "007.level",
  "008.level",
  "009.level",
  "010.level",
  "011.level",
  "012.level",
  "013.level",
  "014.level",
  "015.level",
  "016.level",
  "017.level",
  "018.level",
  "019.level",
  "020.level",
  "021.level",
  "022.level",
  "023.level",
  "024.level",
  "025.level",
  "026.level",
  "027.level",
  "028.level",
  "029.level",
  "030.level",
  "031.level",
  "032.level",
  "033.level",
  "034.level",
  "035.level",
  "036.level",
  "037.level",
  "038.level",
  "039.level",
  "040.level",
  "041.level",
  "042.level",
  "043.level",
  "044.level",
  "045.level",
  "046.level",
  "047.level",
  "048.level",
  "049.level",
  "050.level",
  "051.level",
  "052.level",
  "053.level",
  "054.level",
  "055.level",
  "056.level",
  "057.level",
  "058.level",
  "059.level",
  "060.level",
  "061.level",
  "062.level",
  "063.level",
  "064.level",
  "065.level",
  "066.level",
  "067.level",
  "068.level",
  "069.level",
  "070.level",
  "071.level",
  "072.level",
  "073.level",
  "074.level",
  "075.level",
  "076.level",
  "077.level",
  "078.level",
  "079.level",
  "080.level",
  "081.level",
  "082.level",
  "083.level",
  "084.level",
  "085.level",
  "086.level",
  "087.level",
  "088.level",
  "089.level",
  "090.level",
  "091.level",
  "091_snes.level",
  "092.level",
  "093.level",
  "094.level",
  "095.level",
  "096.level",
  "097.level",
  "098.level",
  "099.level",
  "101.level",
  "100.level",
};

std::vector<std::string> directoryEntries(const std::string & path) {
  std::vector<std::string> entries;

  if (path == "pushover/levels") {
    entries.push_back("original");
  }

  if (path == "pushover/levels/original") {
    entries.push_back("index.level");
    for (auto level : levels)
      entries.push_back(level);
  }

  return entries;

#if 0
  DIR * dir = ::opendir(path.c_str());
  if (dir == NULL)
    throw std::runtime_error("Can't open directory: " + path);
  std::vector<std::string> entries;
  for (struct dirent * i = ::readdir(dir); i != NULL; i = ::readdir(dir))
    entries.push_back(i->d_name);
  if (::closedir(dir) != 0)
    throw std::runtime_error("Can't close directory: " + path);
  return entries;
#endif
}


void amigaLocale(void)
{

#if defined (__AROS__)
  struct LocaleConv
  {
    char *langstr;
    char *gmostr;
  };

  struct LocaleConv LocaleConvTab[] =
  {
    {"catal�", "ca"}, // Catalan
    {"czech", "cz"}, // French
    {"dansk", "da"}, // Danish
    {"deutsch", "de"}, // German
    {"espa�ol", "es"}, // Spanish
    {"esperanto", "eo"}, // Esperanto
    {"fran�ais", "fr"}, // French
    {"hrvatski", "hr"}, // Croatian
    {"ellinik�", "el"}, // Greek
    {"�slenska", "is"}, // Icelandic
    {"italiano", "it"}, // Italian
    {"magyar", "hu"}, // Hungarian <3
    {"malti", "mt"}, // Maltese
    {"nederlands", "nl"}, // Dutch
    {"norsk", "no"}, // Norwegian Nynorsk
    {"polski", "pl"}, // Polish
    {"portugu�s" , "pt"}, // Portuguese
    {"russian", "ru"}, // Russian
    {"shqipja", "sq"}, // Albanian
    {"svenska", "sv"}, // Swedish
    {"suomi", "fi"}, // Finnish
    {"thai", "th"}, // Thai
    {"t�rk�e", "tr"}, // Turkish
    {0, 0}
  };

  struct Locale *defaultLocale;
  char *locale = NULL;

  if ((defaultLocale = OpenLocale(NULL)))
  {
    int i = 0;
    while (!locale && (i < 10) && defaultLocale->loc_PrefLanguages[i])
    {
      struct LocaleConv *lcptr = LocaleConvTab;

      while (lcptr->langstr)
      {
        if (!StrnCmp(defaultLocale, lcptr->langstr, defaultLocale->loc_PrefLanguages[i], -1, SC_ASCII))
          break;

        lcptr++;
      }

      locale = lcptr->gmostr;

      i++;
    }
    CloseLocale(defaultLocale);
  }

  if (locale)
  {
    SetVar("LC_MESSAGES", locale, strlen(locale) + 1, GVF_LOCAL_ONLY);
  }
#endif
}
