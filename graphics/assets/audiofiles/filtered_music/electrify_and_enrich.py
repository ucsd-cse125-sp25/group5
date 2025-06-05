# electrify_and_enrich.py

import librosa
import numpy as np
import soundfile as sf
from scipy.signal import butter, sosfilt, iirnotch, lfilter

# === Load audio ===
y, sr = librosa.load("lobbymusic.wav", sr=None)

# === ELECTRIFYING ===

# Soft saturation
y_fx = np.tanh(y * 2.0)

# Tremolo
def tremolo(signal, rate=6, depth=0.2, sr=22050):
    t = np.arange(len(signal)) / sr
    return signal * (1 + depth * np.sin(2 * np.pi * rate * t))
y_fx = tremolo(y_fx, rate=6, depth=0.2, sr=sr)

# Pitch shift layer
y_shifted = librosa.effects.pitch_shift(y_fx, sr=sr, n_steps=5)
y_fx = 0.6 * y_fx + 0.4 * y_shifted

# Lowpass filter to remove harsh static
def lowpass(y, sr, cutoff=8000):
    sos = butter(4, cutoff, btype='low', fs=sr, output='sos')
    return sosfilt(sos, y)
y_fx = lowpass(y_fx, sr)

# === ENRICHING ===

def fake_reverb(y, sr, delay=0.03, decay=0.4):
    samples_delay = int(delay * sr)
    echo = np.zeros_like(y)
    echo[samples_delay:] = y[:-samples_delay]
    return y + decay * echo

def boost_low_mids(y, sr, center=300.0, gain_db=6.0):
    sos = butter(2, [center - 50, center + 50], btype='band', fs=sr, output='sos')
    boost = sosfilt(sos, y)
    return y + boost * (10 ** (gain_db / 20) - 1)

def notch_harsh(y, sr, freq=4000.0, Q=3.0):
    b, a = iirnotch(freq / (0.5 * sr), Q)
    return lfilter(b, a, y)

# Apply enrichment
y_fx = fake_reverb(y_fx, sr)
y_fx = boost_low_mids(y_fx, sr)
y_fx = notch_harsh(y_fx, sr)

# === Save output ===
sf.write("lobbymusic_electric_enriched.wav", y_fx, sr)
