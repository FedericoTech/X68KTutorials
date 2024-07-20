def rgb_to_grb(red, green, blue):
    return (((blue & 0xF8) >> 2)
            | ((green & 0xF8) << 8)
            | ((red & 0xF8) << 3)
            | (int((red + green + blue) // 3) > 127))


def my_warning(message):
    print(f"\033[93mWarning: \033[0m{message}")
