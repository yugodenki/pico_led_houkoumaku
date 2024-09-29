# This scripts converts 32x128 png files to C array format in RGB888
from PIL import Image
import sys
import os


def main():
  if len(sys.argv) != 2:
    print("Usage: python png_to_header.py <input directory with png files>")
    sys.exit(1)

  input_dir = sys.argv[1]

  if not os.path.exists(input_dir):
    print("Input path does not exist!")
    sys.exit(1)

  
  image_counter = 0
  header_string = \
    """#ifndef IMAGES_H
#define IMAGES_H

#include <stdint.h>

const uint32_t IMAGES[][4096] = {
"""

  for file_name in sorted(os.listdir(input_dir)):
    if file_name.lower().endswith(".png"):
      print(f"file: {file_name}")
      file_path = os.path.join(input_dir, file_name)
      image = Image.open(file_path)
      image = image.convert("RGB")
      
      width, height = image.size
      if width != 128 or height != 32:
        print(f"All images have to be of size 32 x 128. Skipping image {file_name} due to its size {width} x {height}.")
        continue

      image_string = "  {\n    "
      
      for y in range(32):
        for x in range(128):
          px = image.getpixel((x, y))
          r = px[0]
          g = px[1]
          b = px[2]
          image_string += f"0x{b:02X}{g:02X}{r:02X}, "
      
      image_string = image_string[:-2]
      image_string += "\n  },\n\n"
      header_string += image_string

      image_counter += 1

    
  if image_counter:
    header_string = header_string[:-3]
    header_string += f"\n}};\n\nconst uint32_t NUM_IMAGES = {image_counter};\n\n#endif"
    with open(f"{input_dir}/images.h", "w") as f:
      f.write(header_string)
  
  print("Done")


if __name__ == "__main__":
  main()