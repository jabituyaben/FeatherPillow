import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from scipy.signal import butter, lfilter, resample
import yasa
from scipy.io.wavfile import write

lowcut = 0.1
highcut = 46
sf = 250

f0 = 0.98
Q = 20.0  # Quality factor

def butter_highpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = signal.butter(order, normal_cutoff, btype = "high", analog = False)
    return b, a

def butter_highpass_filter(data, cutoff, fs, order=5):
    b, a = butter_highpass(cutoff, fs, order=order)
    y = signal.filtfilt(b, a, data)
    return y

def butter_bandpass(lowcut, highcut, fs, order=3):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def butter_bandpass_filter(data, lowcut, highcut, fs, order=1):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

# Design notch filter
b, a = signal.iircomb(f0, Q, ftype='notch', fs=250)

with open("ANALOG01.DAT") as f:
    rectype = np.dtype(np.int16)
    eeg = np.fromfile(f, dtype=rectype)

    #apply comb filter
    eeg = signal.filtfilt(b, a, eeg)
    
    eeg = butter_bandpass_filter(eeg, 0.1, 47, 250, order=5)
    fig = yasa.plot_spectrogram(eeg, sf, cmap='Spectral_r')

    plt.show()

print(len(eeg))
eeg = 3.3 * (eeg/1023)
print(len(eeg))
np.savetxt("eeg.csv", eeg, delimiter=",")
write("eeg.wav", 250, eeg.astype(np.int16))
