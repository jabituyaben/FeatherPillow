# FeatherPillow

Original Blog Post:
https://www.majorinput.co.uk/post/arduino-based-eeg-sleep-monitoring

The arduino file is uploaded to the Feather M0, there might be some redundancy in the code with elements I dont use etc but I've copied the latest version I have on my board myself.

In the Python script that processes the DAT files, I use a comb filter sometimes to remove an artefact I had with my setup, these can be commented out if you don't see these issues. This script will display a spectrogram and also export CSV and WAV files, which are common formats in EEG analysis software.
