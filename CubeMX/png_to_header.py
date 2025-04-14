import argparse, os
from PIL import Image

def generate_header(png_path, header_path, array_name):
    try:
        img = Image.open(png_path).getchannel('A').convert('1', dither=Image.NONE)
        width, height = img.size
        pixels = list(img.getdata()) # 0=black, 255=white

        # Basic C identifier sanitization
        c_name = "".join(c if c.isalnum() else "_" for c in array_name)
        if not c_name or c_name[0].isdigit(): c_name = "_" + c_name
        guard = os.path.basename(header_path).upper().replace('.', '_') + "_H_"

        bytes_list = []
        byte_val = 0
        bit_idx = 0
        for i, px_val in enumerate(pixels):
            # Shift left and add bit (1 if black)
            byte_val <<= 1
            if px_val == 0: # Black pixel = 1 bit
                byte_val |= 1
            bit_idx += 1

            # If byte is full or end of row reached
            if bit_idx == 8 or (i + 1) % width == 0:
                # Pad last bits if end of row before byte is full
                if bit_idx < 8:
                     byte_val <<= (8 - bit_idx)
                bytes_list.append(f"0x{byte_val:02X}")
                byte_val = 0
                bit_idx = 0

        # Format bytes into C array string
        bytes_per_line = 16
        formatted_bytes = ""
        for i in range(0, len(bytes_list), bytes_per_line):
             line = ", ".join(bytes_list[i:min(i + bytes_per_line, len(bytes_list))])
             formatted_bytes += f"    {line},\n"
        formatted_bytes = formatted_bytes.rstrip(",\n") # Remove last comma/newline


        header_content = f"""\
#ifndef {guard}
#define {guard}

#include <stdint.h> // For uint8_t

#define {c_name.upper()}_WIDTH  {width}
#define {c_name.upper()}_HEIGHT {height}

// Bitmap data: 1=Black, 0=White; MSB first; Rows padded
const uint8_t {c_name}_data[] = {{
{formatted_bytes}
}};

#endif // {guard}
"""
        with open(header_path, 'w') as f:
            f.write(header_content)
        print(f"Generated: {header_path}")

    except Exception as e:
        print(f"Error converting {png_path}: {e}")
        exit(1)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Minimal PNG to C header converter.')
    parser.add_argument('-i', '--input', required=True, help='Input PNG')
    parser.add_argument('-o', '--output', required=True, help='Output C header')
    parser.add_argument('-n', '--name', default='image_asset', help='C variable base name')
    args = parser.parse_args()
    generate_header(args.input, args.output, args.name)
