#ifndef PPM_H
#define PPM_H

#include <vector>

void writePpmScreenshot(const int width, const int height, const char *filename);


// A 3-byte structure storing R,G,B value of a pixel
struct PackedPixel {
  unsigned char r,g,b;
};

// The image file is read into `pixels' and its dimension stored into `width'
// and `height'. Throws an exception on error.
void ppmRead(const char *filename, int& width, int& height, std::vector<PackedPixel>& pixels);

#endif
