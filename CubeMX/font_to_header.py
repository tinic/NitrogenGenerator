#!/usr/bin/env python3

import json
import argparse
import textwrap
import os
import sys

def generate_cpp_header(data, header_name_base):
    """Generates C++ header content from the loaded JSON data."""

    # --- Struct Definitions ---
    struct_defs = textwrap.dedent(f"""\
    #ifndef {header_name_base.upper()}_H
    #define {header_name_base.upper()}_H

    // Data structures generated from JSON

    struct CharInfo {{
        int chnl;
        int height;
        int id;
        int page;
        int width;
        int x;
        int xadvance;
        int xoffset;
        int y;
        int yoffset;
    }};

    struct CommonInfo {{
        int alphaChnl;
        int base;
        int blueChnl;
        int greenChnl;
        int lineHeight;
        int packed;
        int pages;
        int redChnl;
        int scaleH;
        int scaleW;
    }};

    struct FontInfo {{
        int aa;
        int bold;
        const char* charset; // Assuming charset is always empty or a string literal
        const char* face;
        int italic;
        int outline;
        int padding[4];
        int size;
        int smooth;
        int spacing[2];
        int stretchH;
        int unicode;
    }};

    struct KerningInfo {{
        int amount;
        int first;
        int second;
    }};

    """)

    # --- Data Extraction ---
    chars_data = data.get("chars", [])
    common_data = data.get("common", {})
    info_data = data.get("info", {})
    kernings_data = data.get("kernings", [])

    # --- Generate Chars Array ---
    chars_array_content = []
    for char in chars_data:
        chars_array_content.append(
            f"    {{ {char.get('chnl', 0)}, {char.get('height', 0)}, {char.get('id', 0)}, "
            f"{char.get('page', 0)}, {char.get('width', 0)}, {char.get('x', 0)}, "
            f"{char.get('xadvance', 0)}, {char.get('xoffset', 0)}, {char.get('y', 0)}, "
            f"{char.get('yoffset', 0)} }}"
        )
    chars_array = textwrap.dedent(f"""\
    const int FONT_CHARS_COUNT = {len(chars_data)};
    const CharInfo FONT_CHARS[FONT_CHARS_COUNT] = {{
    """) + ",\n".join(chars_array_content) + "\n};"

    # --- Generate Common Data ---
    common_struct = textwrap.dedent(f"""\
    const CommonInfo FONT_COMMON = {{
        {common_data.get('alphaChnl', 0)}, {common_data.get('base', 0)}, {common_data.get('blueChnl', 0)},
        {common_data.get('greenChnl', 0)}, {common_data.get('lineHeight', 0)}, {common_data.get('packed', 0)},
        {common_data.get('pages', 0)}, {common_data.get('redChnl', 0)}, {common_data.get('scaleH', 0)},
        {common_data.get('scaleW', 0)}
    }};""")

    # --- Generate Info Data ---
    # Escape backslashes and quotes in face/charset strings for C++
    face_str = json.dumps(info_data.get('face', ""))
    charset_str = json.dumps(info_data.get('charset', ""))
    padding_arr = info_data.get('padding', [0, 0, 0, 0])
    spacing_arr = info_data.get('spacing', [0, 0])

    info_struct = textwrap.dedent(f"""\
    const FontInfo FONT_INFO = {{
        {info_data.get('aa', 0)}, {info_data.get('bold', 0)}, {charset_str},
        {face_str}, {info_data.get('italic', 0)}, {info_data.get('outline', 0)},
        {{ {padding_arr[0]}, {padding_arr[1]}, {padding_arr[2]}, {padding_arr[3]} }},
        {info_data.get('size', 0)}, {info_data.get('smooth', 0)},
        {{ {spacing_arr[0]}, {spacing_arr[1]} }},
        {info_data.get('stretchH', 0)}, {info_data.get('unicode', 0)}
    }};""")


    # --- Generate Kernings Array ---
    kernings_array_content = []
    for kerning in kernings_data:
        kernings_array_content.append(
            f"    {{ {kerning.get('amount', 0)}, {kerning.get('first', 0)}, {kerning.get('second', 0)} }}"
        )

    kernings_array = ""
    if kernings_data:
        kernings_array = textwrap.dedent(f"""\
        const int FONT_KERNINGS_COUNT = {len(kernings_data)};
        const KerningInfo FONT_KERNINGS[FONT_KERNINGS_COUNT] = {{
        """) + ",\n".join(kernings_array_content) + "\n};"
    else:
         kernings_array = textwrap.dedent("""\
         const int FONT_KERNINGS_COUNT = 0;
         // No kerning data found in JSON.
         // const KerningInfo FONT_KERNINGS[FONT_KERNINGS_COUNT]; // Cannot declare zero-size array
         """)


    # --- Assemble Final Header ---
    footer = "\n#endif // " + header_name_base.upper() + "_H\n"
    return struct_defs + "\n" + info_struct + "\n\n" + common_struct + "\n\n" + chars_array + "\n\n" + kernings_array + footer


def main():
    parser = argparse.ArgumentParser(
        description="Convert font data from JSON format (like BMFont json export) to a C++ header file.",
        formatter_class=argparse.RawDescriptionHelpFormatter
        )
    parser.add_argument("-i", "--input", required=True,
                        help="Path to the input JSON file.")
    parser.add_argument("-o", "--output", required=True,
                        help="Path for the output C++ header file (.h).")

    args = parser.parse_args()

    # Validate input file path
    if not os.path.isfile(args.input):
        print(f"Error: Input file not found: {args.input}", file=sys.stderr)
        sys.exit(1)

    # Read JSON data
    try:
        with open(args.input, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON file: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error reading input file '{args.input}': {e}", file=sys.stderr)
        sys.exit(1)

    # Determine header guard name base from output filename
    header_name_base = os.path.splitext(os.path.basename(args.output))[0].replace("-", "_").replace(" ", "_")
    if not header_name_base: # Handle cases like "-o .h"
         print(f"Error: Invalid output filename '{args.output}'. Cannot generate header guard name.", file=sys.stderr)
         sys.exit(1)


    # Generate C++ header content
    cpp_content = generate_cpp_header(data, header_name_base)

    # Write to output file
    try:
        with open(args.output, 'w', encoding='utf-8') as f:
            f.write(cpp_content)
        print(f"Successfully generated C++ header file: {args.output}")
    except Exception as e:
        print(f"Error writing output file '{args.output}': {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
