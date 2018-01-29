//
// Created by Skowron on 28.01.2018.
//

#include <SDL.h>
#include <SDL_rwops.h>
#include <cstdio>
#include <iostream>

// Taken from: https://stackoverflow.com/a/8870278/407758
class androidbuf : public std::streambuf {
public:
    enum { bufsize = 128 }; // ... or some other suitable buffer size
    androidbuf() { this->setp(buffer, buffer + bufsize - 1); }

private:
    int overflow(int c)
    {
        if (c == traits_type::eof()) {
            *this->pptr() = traits_type::to_char_type(c);
            this->sbumpc();
        }
        return this->sync()? traits_type::eof(): traits_type::not_eof(c);
    }

    int sync()
    {
        int rc = 0;
        if (this->pbase() != this->pptr()) {
            char writebuf[bufsize+1];
            memcpy(writebuf, this->pbase(), this->pptr() - this->pbase());
            writebuf[this->pptr() - this->pbase()] = '\0';

            SDL_Log("std::cout: %s", writebuf);
            rc = 1;
            this->setp(buffer, buffer + bufsize - 1);
        }
        return rc;
    }

    char buffer[bufsize];
};

void redirect_cout_to_log(bool always_flush /*= true*/) {
    std::cout.rdbuf(new androidbuf);
    if (always_flush) {
        std::cout.setf(std::ios::unitbuf);
    }
    std::cout << "cout redirected to log. always_flush=" << always_flush << std::endl;
}

// Code inspired by: http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/
static int android_read(void* cookie, char* buf, int size) {
    SDL_RWops* g = (SDL_RWops*)cookie;
    return g->read(g, buf, 1, size);
}

static int android_write(void* cookie, const char* buf, int size) {
    SDL_RWops* g = (SDL_RWops*)cookie;
    return g->write(g, buf, 1, size);
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
    SDL_RWops* g = (SDL_RWops*)cookie;
    return g->seek(g, offset, whence);
}

static int android_close(void* cookie) {
    SDL_RWops* g = (SDL_RWops*)cookie;
    g->close(g);
    return 0;
}

FILE* android_fopen(const char* fname, const char* mode) {
    SDL_RWops* g = SDL_RWFromFile(fname, mode);
    if (!g) {
        return NULL;
    }

    FILE* f = funopen(g, android_read, android_write, android_seek, android_close);
    return f;
}

std::string loadFileAsString(const std::string& path) {
    void* data = SDL_LoadFile(path.c_str(), NULL);
    std::string contents((const char *) data);
    SDL_free(data);
    return contents;
}
