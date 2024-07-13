from pydub import AudioSegment

from . import os

import numpy as np
from scipy.signal import resample

import struct


def clamp(x, low, high):
    return max(low, min(x, high))


# Define constants and lookup tables
OKI_STEP_TABLE = [
    16, 17, 19, 21, 23, 25, 28, 31,
    34, 37, 41, 45, 50, 55, 60, 66,
    73, 80, 88, 97, 107, 118, 130, 143,
    157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658,
    724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552
]


def oki_step(step, history, step_hist):
    ADJUST_TABLE = [-1, -1, -1, -1, 2, 4, 6, 8]

    step_size = OKI_STEP_TABLE[step_hist]

    delta = step_size >> 3

    if step & 1:
        delta += step_size >> 2
    if step & 2:
        delta += step_size >> 1
    if step & 4:
        delta += step_size
    if step & 8:
        delta = -delta

    out = history + delta

    history = out = clamp(out, -2048, 2047)  # Saturate output

    adjusted_step = step_hist + ADJUST_TABLE[step & 7]

    step_hist = clamp(adjusted_step, 0, 48)

    return out, history, step_hist


def oki_encode_step(input, history, step_hist):
    step_size = OKI_STEP_TABLE[step_hist]

    # get the difference between the input and the previous step
    delta = input - history

    adpcm_sample = 8 if delta < 0 else 0

    # make delta positive
    delta = abs(delta)

    # we go through the bits of the nibble
    for bit in range(2, -1, -1):
        if delta >= step_size:
            adpcm_sample |= (1 << bit)
            delta -= step_size
        step_size >>= 1

    _, history, step_hist = oki_step(adpcm_sample, history, step_hist)

    return adpcm_sample, history, step_hist


def oki6258_encode(buffer):
    adpcm_data = bytearray()
    history = 0
    step_hist = 0
    buf_sample = 0
    nibble = 0

    for sample in buffer:

        if sample < 0x7ff8:  # round up
            sample += 8

        # we make it 12 bits
        sample >>= 4

        step, history, step_hist = oki_encode_step(sample, history, step_hist)

        if nibble:
            # print(f"{buf_sample | ((step & 0x0f) << 4)}")
            adpcm_data.append(buf_sample | ((step & 0x0f) << 4))
        else:
            buf_sample = step & 0x0f

        nibble ^= 1

    return adpcm_data


def oki6258_decode(buffer):
    out_buffer = bytearray();
    history = 0
    step_hist = 0
    nibble = 4

    for i in range(len(buffer)):
        step = buffer[i] << nibble
        step >>= 4
        if not nibble:
            buffer = buffer[1:]

        nibble ^= 4
        out, history, step_hist = oki_step(step, history, step_hist)
        out_buffer.append(out << 4)

    return out_buffer


def convert_audio(file_path, output_dir, output_name, sample):
    # Load the stereo audio file
    stereo_audio = AudioSegment.from_wav(file_path)

    # Convert to mono by setting channels to 1
    mono_audio = stereo_audio.set_channels(1)

    del stereo_audio

    # Get the sample rate and samples from the AudioSegment
    original_sample_rate = mono_audio.frame_rate
    samples = np.array(mono_audio.get_array_of_samples())

    match sample:
        case '1':  # 3.9Khz
            new_sample_rate = 3900
        case '2':  # 5.2Khz
            new_sample_rate = 5200
        case '3':  # 7.8Khz
            new_sample_rate = 7800
        case '4':  # 10.4Khz
            new_sample_rate = 10400
        case '5':  # 15.6Khz
            new_sample_rate = 15600
        case _:
            return "This is the default case"

    number_of_samples = round(len(samples) * float(new_sample_rate) / original_sample_rate)

    # Resample the audio data
    resampled_samples = resample(samples, number_of_samples)

    # Convert resampled samples to int16 (the format pydub uses)
    resampled_samples = np.array(resampled_samples, dtype=np.int16)

    # Verify the resampled samples
    # print("Resampled Samples:", resampled_samples[:10])

    # with open(os.path.join(output_dir, f"{output_name}.raw"), 'wb') as file:
            # file.write(resampled_samples.tobytes())

    # Encode to ADPCM
    adpcm_data = oki6258_encode(resampled_samples)

    with open(os.path.join(output_dir, f"{output_name}.raw"), 'wb') as file:
        file.write(adpcm_data)


    print("Conversion to mono completed successfully.")
