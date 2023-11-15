#include <FastNoise/FastNoise.h>

#include <cmath>
#include <cstdint>
#include <vector>

#include <iostream>
#include <fstream>

#include <png.h>

//TODO: Make this configurable
const int WIDTH = 4096;
const int HEIGHT = 4096;

int main() {
  //Set up simple Fractal Noise
  auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
  auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();

  fnFractal->SetSource(fnSimplex);
  fnFractal->SetOctaveCount(5);

  std::vector<float> noiseValues(WIDTH * HEIGHT);
  std::vector<uint16_t> imagePixels(WIDTH * HEIGHT);

  fnFractal->GenUniformGrid2D(noiseValues.data(), 0, 0, WIDTH, HEIGHT, 0.001f, 1337);

  for (int index = 0; index < HEIGHT * WIDTH; index++) {
    double value = static_cast<double>(noiseValues[index]);

    //Normalize from Range (-1 to +1) to (0 to +1)
    value *= 0.5d;
    value += 0.5d;

    //Raise to Power because it looks good imo
    value = pow(value, 1.3d);

    //Multiply by 65535 so we get Values from 0 to 255, the uint8 Max Value
    value *= 65535.0d;

    //Back into the Array you go
    imagePixels[index] = static_cast<uint16_t>(value);
  }

  //Create or Overwrite File
  FILE * filePointer = fopen("noise.png", "wb");
  if (!filePointer) {
    std::cerr << "Cannot open 'noise.png' for writing." << std::endl;

    return 1;
  }

  //Create PNG Write Structure (File Header i believe)
  png_structp pngStructure = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!pngStructure) {
    std::cerr << "Could not allocate PNG Write Structure" << std::endl;

    return 1;
  }

  // Initialize info structure
  png_infop infoStructure = png_create_info_struct(pngStructure);
  if (!infoStructure) {
    png_destroy_write_struct(&pngStructure, nullptr);
    std::cerr << "Could not allocate PNG Info Structure" << std::endl;

    return 1;
  }

  //Setup Error Handling
  if (setjmp(png_jmpbuf(pngStructure))) {
    png_destroy_write_struct(&pngStructure, &infoStructure);
    fclose(filePointer);
    std::cerr << "Error during PNG Creation" << std::endl;

    return 1;
  }

  png_init_io(pngStructure, filePointer);

  //Write PNG Header (16 Bit Grayscale)
  png_set_IHDR(pngStructure, infoStructure, WIDTH, HEIGHT, 16, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(pngStructure, infoStructure);
  
  //This tells libpng to do the endian swapping itself, i think
  png_set_swap(pngStructure);

  //Write image data
  for (int y = 0; y < HEIGHT; y++) {
    png_write_row(pngStructure, (png_const_bytep) &imagePixels[y * WIDTH]);
  }

  //End Write
  png_write_end(pngStructure, nullptr);

  //Clean Up
  fclose(filePointer);

  png_free_data(pngStructure, infoStructure, PNG_FREE_ALL, -1);
  png_destroy_write_struct(&pngStructure, (png_infopp) nullptr);

  std::cout << "PNG Image generated successfully!" << std::endl;
  return 0;
}
