import argparse
import os
import numpy as np
from scipy.signal import fftconvolve
from scipy.fft import fft, fftfreq
from soundfile import read

# Define constants (same as original code)
WS = 1024
SH = 256
DS = 4
NF = 4


def parse_arguments():
    """Parses command-line arguments."""
    parser = argparse.ArgumentParser(description="Get maximum frequencies from audio file")
    parser.add_argument("-v", "--verbose", action="store_true", help="Print verbose output")
    parser.add_argument("-w", "--output", help="Output filename for frequency signature")
    parser.add_argument("-ws", "--window_size", type=int, default=WS, help="Window size for FFT")
    parser.add_argument("-sh", "--shift", type=int, default=SH, help="Window shift for FFT")
    parser.add_argument("-ds", "--downsample", type=int, default=DS, help="Downsampling factor")
    parser.add_argument("-nf", "--num_freqs", type=int, default=NF, help="Number of significant frequencies")
    parser.add_argument("audio_file", help="Path to the audio file")
    return parser.parse_args()


def main():
    """Main function to analyze audio and get maximum frequencies."""
    args = parse_arguments()

    # Read audio data
    data, samplerate = read(args.audio_file)

    if len(data.shape) != 2 or data.shape[1] != 2:
        print("Error: Currently supports only stereo (2 channels) audio files")
        return 1

    if samplerate != 44100:
        print("Error: Currently supports only 44100 Hz sample rate")
        return 1

    if args.verbose:
        print(f"Sample rate: {samplerate} Hz")
        print(f"Channels: {data.shape[1]}")
        print(f"Frames: {data.shape[0]}")

    # Combine channels to mono
    audio_data = np.mean(data, axis=1)

    # Prepare window and hann function
    window = np.hanning(args.window_size)

    def get_fft(data):
        """Calculates FFT of a data segment with windowing."""
        return fft(window * data)

    # Analyze segments with overlap
    spectrogram = np.empty((int(np.ceil((len(audio_data) - WS) / (SH * DS)) + 1), WS // 2 + 1))
    for n in range(spectrogram.shape[0]):
        start = n * (SH * DS)
        end = min(start + WS, len(audio_data))
        segment = audio_data[start:end]
        if len(segment) < WS:
            segment = np.pad(segment, (0, WS - len(segment)), mode="constant")
        spectrogram[n] = np.abs(get_fft(segment))[: WS // 2 + 1]

    # Downsample spectrogram (average multiple bins)
    ones_array = np.ones((1, args.downsample))  # Create a 1D array of ones
    downsample_filter = np.repeat(ones_array, spectrogram.shape[0], axis=0)  # Repeat for each row
    spectrogram_downsampled = fftconvolve(spectrogram, downsample_filter, mode="same") / args.downsample

    # Identify maximum frequencies
    max_freq_indices = np.argsort(spectrogram_downsampled, axis=1)[:, -args.num_freqs:]

    # Write frequency signature to file (if specified)
    if args.output:
        try:
            with open(args.output, "wb") as f:
                for i in range(max_freq_indices.shape[0]):
                    # Truncate to byte value
                    f.write(np.clip(max_freq_indices[i], 0, 255).astype(np.uint8))
        except OSError:
            print(f"Warning: Failed to write output file {args.output}")

if __name__ == "__main__":
    main()
