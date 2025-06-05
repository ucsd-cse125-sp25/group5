# boost_and_round_cello.py

import librosa
import numpy as np
import soundfile as sf
from scipy.signal import butter, sosfilt, iirnotch, lfilter

# === Load audio ===
y, sr = librosa.load("lobbymusic.wav", sr=None)

# === 1. Bass Boost (shelving effect below 300 Hz) ===
def boost_bass(y, sr, cutoff=300.0, gain_db=8.0):
    sos = butter(2, cutoff, btype='low', fs=sr, output='sos')
    low = sosfilt(sos, y)
    boosted = y + low * (10 ** (gain_db / 20) - 1)
    return boosted

# === 2. Body Boost (200–400 Hz band) ===
def boost_body(y, sr, center=300.0, gain_db=5.0):
    sos = butter(2, [center - 100, center + 100], btype='band', fs=sr, output='sos')
    band = sosfilt(sos, y)
    return y + band * (10 ** (gain_db / 20) - 1)

# === 3. Notch Nasal Frequencies (1.5kHz & 2.5kHz) ===
def cut_nasal(y, sr, freqs=[1500.0, 2500.0], Q=5.0):
    for freq in freqs:
        b, a = iirnotch(freq / (0.5 * sr), Q)
        y = lfilter(b, a, y)
    return y

# === 4. Add Subtle Reverb ===
def subtle_reverb(y, sr, delay=0.02, decay=0.3):
    samples_delay = int(delay * sr)
    echo = np.zeros_like(y)
    echo[samples_delay:] = y[:-samples_delay]
    return y + decay * echo

# === Apply all processing steps ===
y_processed = boost_bass(y, sr)
y_processed = boost_body(y_processed, sr)
y_processed = cut_nasal(y_processed, sr)
y_processed = subtle_reverb(y_processed, sr)

# === Normalize (to prevent clipping) ===
peak = np.max(np.abs(y_processed))
if peak > 1.0:
    y_processed /= peak

# === Save output ===
sf.write("lobbymusic_bass_boosted_rounded.wav", y_processed, sr)
