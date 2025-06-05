# enrich_cello.py

import librosa
import numpy as np
import soundfile as sf
from scipy.signal import butter, sosfilt, iirnotch, lfilter

# === Load audio ===
y, sr = librosa.load("lobbymusic.wav", sr=None)

# === Reverb / Resonance simulation ===
def fake_reverb(y, sr, delay=0.03, decay=0.4):
    samples_delay = int(delay * sr)
    echo = np.zeros_like(y)
    echo[samples_delay:] = y[:-samples_delay]
    return y + decay * echo

# === Warm low-mid boost ===
def boost_low_mids(y, sr, center=300.0, gain_db=6.0):
    sos = butter(2, [center - 50, center + 50], btype='band', fs=sr, output='sos')
    boost = sosfilt(sos, y)
    return y + boost * (10 ** (gain_db / 20) - 1)

# === Notch harsh upper mids ===
def notch_harsh(y, sr, freq=4000.0, Q=3.0):
    b, a = iirnotch(freq / (0.5 * sr), Q)
    return lfilter(b, a, y)

# === Apply effects ===
y_enriched = fake_reverb(y, sr)
y_enriched = boost_low_mids(y_enriched, sr)
y_enriched = notch_harsh(y_enriched, sr)

# === Save output ===
sf.write("lobbymusic_enriched.wav", y_enriched, sr)
