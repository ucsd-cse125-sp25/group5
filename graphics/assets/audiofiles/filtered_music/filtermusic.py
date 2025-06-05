import librosa
import soundfile as sf
from scipy.signal import butter, sosfilt
import numpy as np

# Load file (default sr=22050, or specify sr=44100 to increase range)
y, sr = librosa.load("lobbymusic.wav", sr=None)

# Define filters within valid frequency ranges
def highpass(y): return sosfilt(butter(3, 100, btype='high', fs=sr, output='sos'), y)
def lowpass(y): return sosfilt(butter(3, 9000, btype='low', fs=sr, output='sos'), y)

# Apply processing
y = highpass(y)
y = lowpass(y)

# Optional light distortion for "grit"
y = 0.8 * np.tanh(y * 2.0)

# Save result
sf.write("processed_for_game.wav", y, sr)
