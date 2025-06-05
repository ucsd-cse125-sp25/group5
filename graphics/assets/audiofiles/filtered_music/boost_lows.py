# boost_lows.py

import librosa
import numpy as np
import soundfile as sf
from scipy.signal import butter, sosfilt

# === Load audio ===
y, sr = librosa.load("lobby3.wav", sr=None)

# === Boost lower frequencies with a low-shelf filter ===
def boost_bass(y, sr, cutoff=300.0, gain_db=6.0):
    # Use a second-order low-pass as a shelf approximation
    sos = butter(2, cutoff, btype='low', fs=sr, output='sos')
    low = sosfilt(sos, y)
    boosted = y + low * (10 ** (gain_db / 20) - 1)
    return boosted

# Apply bass boost
y_boosted = boost_bass(y, sr, cutoff=300, gain_db=8.0)

# === Normalize (optional) ===
peak = np.max(np.abs(y_boosted))
if peak > 1.0:
    y_boosted /= peak

# === Save output ===
sf.write("lobbymusic.wav", y_boosted, sr)
