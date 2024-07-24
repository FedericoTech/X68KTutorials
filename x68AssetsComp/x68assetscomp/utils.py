def rgb_to_grb(red, green, blue):
    return (((blue & 0xF8) >> 2)
            | ((green & 0xF8) << 8)
            | ((red & 0xF8) << 3)
            | (int((red + green + blue) // 3) > 127))


def parse_magic_pink(magic_pink):
    magic_pink = int(magic_pink, 16)
    return rgb_to_grb(
        (magic_pink >> 16) & 0xFF,
        (magic_pink >> 8) & 0xFF,
        magic_pink & 0xFF
    )


def my_warning(message):
    print(f"\033[93mWarning: \033[0m{message}")
