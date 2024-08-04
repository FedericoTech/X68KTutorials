import warnings


def rgb_to_grb(red, green, blue):
    try:
        with warnings.catch_warnings():
            warnings.simplefilter("error", RuntimeWarning)
            return (((blue & 0xF8) >> 2)
                    | ((green & 0xF8) << 8)
                    | ((red & 0xF8) << 3)
                    | (int((red + green + blue) // 3) > 127))
    except RuntimeWarning as e:
        print(f"Types - red: {type(red)}, green: {type(green)}, blue: {type(blue)}")
        print(f"Values - red: {red}, green: {green}, blue: {blue}")
        raise e


def parse_magic_pink(magic_pink):
    magic_pink = int(magic_pink, 16)
    return rgb_to_grb(
        (magic_pink >> 16) & 0xFF,
        (magic_pink >> 8) & 0xFF,
        magic_pink & 0xFF
    )


def my_warning(message):
    print(f"\033[93mWarning: \033[0m{message}")
