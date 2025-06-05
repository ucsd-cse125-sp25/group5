# round_cello.py

import librosa
import numpy as np
import soundfile as sf
from scipy.signal import butter, sosfilt, iirnotch, lfilter

# === Load audio ===
y, sr = librosa.load("lobbymusic.wav", sr=None)

# === 1. Boost body (200–400 Hz) ===
def boost_body(y, sr, center=300.0, gain_db=5.0):
    sos = butter(2, [center - 100, center + 100], btype='band', fs=sr, output='sos')
    band = sosfilt(sos, y)
    return y + band * (10 ** (gain_db / 20) - 1)

# === 2. Cut nasal harshness (notch 1–3 kHz) ===
def cut_nasal(y, sr, freqs=[1500.0, 2500.0], Q=5.0):
    for freq in freqs:
        b, a = iirnotch(freq / (0.5 * sr), Q)
        y = lfilter(b, a, y)
    return y

# === 3. Add subtle resonance/reverb ===
def subtle_reverb(y, sr, delay=0.02, decay=0.3):
    samples_delay = int(delay * sr)
    echo = np.zeros_like(y)
    echo[samples_delay:] = y[:-samples_delay]
    return y + decay * echo

# === Apply processing ===
y_rounded = boost_body(y, sr)
y_rounded = cut_nasal(y_rounded, sr)
y_rounded = subtle_reverb(y_rounded, sr)

# === Optional normalization ===
peak = np.max(np.abs(y_rounded))
if peak > 1.0:
    y_rounded /= peak

# === Save output ===
sf.write("lobbymusic_rounded.wav", y_rounded, sr)
