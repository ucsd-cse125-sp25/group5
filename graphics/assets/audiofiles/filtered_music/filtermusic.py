import librosa
import numpy as np
import soundfile as sf
from scipy.signal import butter, sosfilt

# Load audio
y, sr = librosa.load("lobbymusic.wav", sr=None)

# Soft saturation
y_electric = np.tanh(y * 2.0)

# Gentle tremolo
def tremolo(signal, rate=6, depth=0.2, sr=22050):
    t = np.arange(len(signal)) / sr
    return signal * (1 + depth * np.sin(2 * np.pi * rate * t))

y_electric = tremolo(y_electric, rate=6, depth=0.2, sr=sr)

# Optional pitch-shift layer (use with care)
y_shifted = librosa.effects.pitch_shift(y_electric, sr=sr, n_steps=5)
y_electric = 0.6 * y_electric + 0.4 * y_shifted

# Lowpass filter to smooth high static
def lowpass(y, sr, cutoff=8000):
    sos = butter(4, cutoff, btype='low', fs=sr, output='sos')
    return sosfilt(sos, y)

y_electric = lowpass(y_electric, sr)

# Save the output
sf.write("electric_lobby_clean.wav", y_electric, sr)
