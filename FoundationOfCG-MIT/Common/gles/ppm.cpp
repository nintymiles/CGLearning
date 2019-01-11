#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#include "ppm.h"

using namespace std;

void writePpmScreenshot(const int width, const int height, const char *filename) {
  vector<char> image(width*height*3);

  glReadPixels(0,0, width, height, GL_RGB, GL_UNSIGNED_BYTE, &image[0]);

  ofstream f(filename, ios::binary);
  f << "P6 " << width << " " << height << " 255\n";
  for (int i = 0; i < height; ++i) {
    f.write(&image[3*width*(height-1-i)], 3*width);
  }
}

// Read one positive integer from a (text) file. Line beginning with
// "#" are ignored as comments.
static int ppmReadInteger(istream& is) {
  unsigned char ch;
  int got = 0, done = 0, accum = 0, inComment = 0;
  while (!done) {
    ch = is.get();

    if (inComment) {
      if (ch=='\n')
        inComment=0;
      continue;
    }

    if (isdigit(ch)) {
      accum = accum*10 + ch-'0';
      got = 1;
    }
    else if (ch=='#')
      inComment=1;
    else if (!ch || !strchr(" \t\r\n", ch))
      throw runtime_error("ppmRead: invalid character");
    else if (got)
      done = 1;
    else
      ; // nothing
  }
  return accum;
}

// Read the PPM header from the input stream and initialize the width and height
// to the appropriate values, and throws rumtime_error on invalid width/height
static void ppmReadHeader(istream& is, int &width, int &height) {
  if ((width = ppmReadInteger(is)) < 0) {
    throw runtime_error("ppmRead: invalid width");
  }
  if ((height = ppmReadInteger(is)) < 0) {
    throw runtime_error("ppmRead: invalid height");
  }
  if (ppmReadInteger(is) != 255) {
    cerr << "Warning: maxcolor not 255 : won't work well" << endl;
  }
}

//Reads the actual PPM data and stores returns in in a pixels.
void ppmRead(const char *filename, int& width, int& height, std::vector<PackedPixel>& pixels) {
  ifstream is(filename, ios::binary);
  if (!is.is_open())
    throw runtime_error(string("ppmRead: Cannot open file ") + filename + " for read");

  // Sets bits to report IO error using exception
  is.exceptions(ios::eofbit | ios::failbit | ios::badbit);

  char buf[2];
  is.read(buf, 2);

  bool isbinary = false;
  if (!memcmp(buf, "P3", 2))
    isbinary = false;
  else if (!memcmp(buf, "P6", 2))
    isbinary = true;
  else
    throw runtime_error("ppmRead: bad file format");

  ppmReadHeader(is, width, height);

  pixels.resize(width * height);

  if (isbinary) {
    for (int row = height - 1; row >= 0; row--) {
      is.read(reinterpret_cast<char*>(&pixels[row * width]), width * sizeof(PackedPixel));
    }
  }
  else {
    for (int row = height - 1; row >= 0; row--) {
      for (int l = 0; l < width; l++) {
        PackedPixel &p = pixels[row * width + l];
        p.r = ppmReadInteger(is);
        p.g = ppmReadInteger(is);
        p.b = ppmReadInteger(is);
      }
    }
  }
}
