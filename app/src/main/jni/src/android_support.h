#ifndef SDL2PROJECT0_ANDROID_SUPPORT_H
#define SDL2PROJECT0_ANDROID_SUPPORT_H

void redirect_cout_to_log(bool always_flush = true);

FILE* android_fopen(const char* fname, const char* mode);

std::string loadFileAsString(const std::string& path);

#endif //SDL2PROJECT0_ANDROID_SUPPORT_H
