def on_key_press(event):
    key_press_handler(event, canvas, toolbar)

def clicked1():
    global canvas, toolbar
    if canvas is not None:
        canvas.get_tk_widget().destroy()
        toolbar.destroy()
    ECGData = []
    ser = serial.Serial('COM4', 115200)
    eng.test_bio(nargout=0)
    freqSampECG = ser.read(7).decode('UTF-8').rstrip('\x00')
    for i in range(0,12000):
        pom = ser.read(7).decode('UTF-8').rstrip('\x00')
        pom=float(pom)
        ECGData.append(pom)
    freqECG = ser.read(7).decode('UTF-8').rstrip('\x00')
    ispis = freqSampECG + " Hz"
    sampleRate.config(text=ispis, font=("Arial", 12))
    ispis = freqECG + " bpm"
    result.config(text=ispis, font=("Arial", 12))

    freq = float(freqSampECG)
    fig = Figure(figsize=(8, 4.5), dpi=100)
    t = range(0, len(ECGData))
    tSimp = [x/freq for x in t]
    fullPlot = fig.add_subplot(111)
    fullPlot.set_xlabel('Vrijeme [s]')
    fullPlot.set_ylabel('Amplituda')
    fullPlot.set_title('Signal EKG-a u vremenu')
    fullPlot.plot(tSimp, ECGData)
    fig.tight_layout()
    canvas = FigureCanvasTkAgg(fig, master=centralGraph)  # A tk.DrawingArea.
    canvas.draw()
    canvas.get_tk_widget().grid(row=1, column=1)
    toolbar = NavigationToolbar2Tk(canvas, navigationFrame)
    toolbar.update()
    canvas.get_tk_widget().grid(row=1, column=0)
    canvas.mpl_connect("key_press_event", on_key_press)

def clicked2():
    global canvas, toolbar
    if canvas is not None:
        canvas.get_tk_widget().destroy()
        toolbar.destroy()
    respData = []
    ser = serial.Serial('COM4', 115200)
    eng.resp_test(nargout=0)
    freqSampResp = ser.read(7).decode('UTF-8').rstrip('\x00')
    for i in range(0,12000):
        pom = ser.read(7).decode('UTF-8').rstrip('\x00')
        pom=float(pom)
        respData.append(pom)
    freqResp = ser.read(7).decode('UTF-8').rstrip('\x00')
    ispis = freqSampResp + " Hz"
    sampleRate.config(text=ispis, font=("Arial", 12))
    ispis = freqResp + " bpm"
    result.config(text=ispis, font=("Arial", 12))

    freq = float(freqSampResp)
    fig = Figure(figsize=(8, 4.5), dpi=100)
    t = range(0, len(respData))
    tSimp = [x/freq for x in t]
    fullPlot = fig.add_subplot(111)
    fullPlot.set_xlabel('Vrijeme [s]')
    fullPlot.set_ylabel('Amplituda')
    fullPlot.set_title('Signal disanja u vremenu')
    fullPlot.plot(tSimp, respData)
    fig.tight_layout()
    canvas = FigureCanvasTkAgg(fig, master=centralGraph)  # A tk.DrawingArea.
    canvas.draw()
    canvas.get_tk_widget().grid(row=1, column=1)
    toolbar = NavigationToolbar2Tk(canvas, navigationFrame)
    toolbar.update()
    canvas.get_tk_widget().grid(row=1, column=0)
    canvas.mpl_connect("key_press_event", on_key_press)

def clicked3():
    global canvas, toolbar
    if canvas is not None:
        canvas.get_tk_widget().destroy()
        toolbar.destroy()
    EMGData1 = []
    EMGData2 = []
    freqSampEMG1 = "Prvi: "
    freqEMG1 = "Prvi: "
    ser = serial.Serial('COM4', 115200)
    eng.test_emg1(nargout=0)
    freq1str = ser.read(7).decode('UTF-8').rstrip('\x00')
    freq1flt = float(freq1str)
    for i in range(0,1024):
        pom = ser.read(7).decode('UTF-8').rstrip('\x00')
        pom=float(pom)
        EMGData1.append(pom)
    EMGData1 = [x*x/freq1flt/1024 for x in EMGData1]
    freqEMG1 += ser.read(7).decode('UTF-8').rstrip('\x00')

    eng.test_emg2(nargout=0)
    freq2str = ser.read(7).decode('UTF-8').rstrip('\x00')
    freq2flt = float(freq2str)
    for i in range(0,1024):
        pom = ser.read(7).decode('UTF-8').rstrip('\x00')
        pom=float(pom)
        EMGData2.append(pom)
    EMGData2 = [x*x/freq2flt/1024 for x in EMGData2]
    freqEMG2 = ser.read(7).decode('UTF-8').rstrip('\x00')
    freqSampEMG1 += freq1str
    freqSampEMG1 += " Hz\nDrugi: "
    freqSampEMG1 += freq2str + " Hz"
    freqEMG1 += " Hz\nDrugi: "
    freqEMG1 += freqEMG2 + " Hz"
    sampleRate.config(text=freqSampEMG1, font=("Arial", 12))
    result.config(text=freqEMG1, font=("Arial", 12))

    fig = Figure(figsize=(8, 4.5), dpi=100)
    t = range(0, len(EMGData1))
    tSimp = [x/freq1flt for x in t]
    fullPlot = fig.add_subplot(211)
    fullPlot.set_xlabel('Frekvencija [Hz]')
    fullPlot.set_ylabel('Amplituda snage')
    fullPlot.set_title('Spektar snage EMG-a, prvi uzorak')
    fullPlot.plot(tSimp, EMGData1)
    canvas = FigureCanvasTkAgg(fig, master=centralGraph)  # A tk.DrawingArea.
    canvas.draw()
    canvas.get_tk_widget().grid(row=0, column=0, sticky='new')
    canvas.get_tk_widget().grid(row=1, column=0)
    canvas.mpl_connect("key_press_event", on_key_press)
    fullPlot = fig.add_subplot(212)
    fullPlot.set_xlabel('Frekvencija [Hz]')
    fullPlot.set_ylabel('Amplituda snage')
    fullPlot.set_title('Spektar snage EMG-a, drugi uzorak')
    fullPlot.plot(tSimp, EMGData2)
    fig.tight_layout()
    canvas = FigureCanvasTkAgg(fig, master=centralGraph)  # A tk.DrawingArea.
    canvas.draw()
    canvas.get_tk_widget().grid(row=0, column=0, sticky='sew')
    toolbar = NavigationToolbar2Tk(canvas, navigationFrame)
    toolbar.update()
    canvas.get_tk_widget().grid(row=1, column=0)
    canvas.mpl_connect("key_press_event", on_key_press)

try:
    import tkinter
except ImportError:  # python 2
    import Tkinter as tkinter
import serial
import numpy as np
from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2Tk)
# Implement the default Matplotlib key bindings.
from matplotlib.backend_bases import key_press_handler
from matplotlib.figure import Figure
import matlab.engine

eng = matlab.engine.start_matlab()
mainWindow = tkinter.Tk()
canvas = None
toolbar = None

mainWindow.title("Sustav za analizu i obradu biomedicinskih signala")
mainWindow.geometry('1280x600')
mainWindow['padx'] = 10
label= tkinter.Label(mainWindow, text="Sustav za analizu i obradu biomedicinskih signala", font=("Arial Bold", 24))
label.grid(row=0, column=0, columnspan=3, sticky='n')

leftSide=tkinter.LabelFrame(mainWindow, text="Odabir biomedicinskog signala", font=("Arial", 14))
leftSide.grid(row=1, column=0, sticky='ns')
leftSide.config(relief='sunken', border=3)
leftSide['padx'] = 100
leftSide['pady'] = 50
centralGraph=tkinter.LabelFrame(mainWindow, text="Prikaz signala u vremenu", font=("Arial", 14))
centralGraph.grid(row=1, column=1, sticky='nsew')
centralGraph.config(relief='sunken', border=3)
rightSide=tkinter.LabelFrame(mainWindow, text="Podatci o signalu", font=("Arial", 14))
rightSide.grid(row=1, column=2, sticky='ns')
rightSide.config(relief='sunken', border=3)
navigationFrame=tkinter.LabelFrame(mainWindow)
navigationFrame.grid(row=2, column=1, sticky='ew', padx=20)
navigationFrame.config(relief='sunken', border=3)

mainWindow.columnconfigure(0, weight=1)
mainWindow.columnconfigure(1, weight=5)
mainWindow.columnconfigure(2, weight=1)
mainWindow.rowconfigure(0, weight=1)
mainWindow.rowconfigure(1, weight=5)
mainWindow.rowconfigure(2, weight=1)

button1 = tkinter.Button(leftSide, text="EKG", font=("Arial", 12), command=clicked1)
button2 = tkinter.Button(leftSide, text="Disanje", font=("Arial", 12), command=clicked2)
button3 = tkinter.Button(leftSide, text="EMG", font=("Arial", 12), command=clicked3)
button1.grid(row=0, column=0, sticky='ew', pady=20)
button2.grid(row=1, column=0, sticky='ew', pady=20)
button3.grid(row=2, column=0, sticky='ew', pady=20)

sampleRateLabel = tkinter.Label(rightSide, text="Frekvencija otipkavanja:", font=("Arial", 12))
sampleRate = tkinter.Label(rightSide, text="")
resultLabel = tkinter.Label(rightSide, text="Frekvencija signala:", font=("Arial", 12))
result = tkinter.Label(rightSide, text="")
sampleRateLabel.grid(row=0, column=2, sticky='ew')
sampleRate.grid(row=1, column=2, sticky='ew')
resultLabel.grid(row=2, column=2, sticky='ew')
result.grid(row=3, column=2, sticky='ew')

exitButton = tkinter.Button(mainWindow, text="Exit", font=("Arial", 12), command=mainWindow.destroy)
exitButton.grid(row=2, column=2, sticky='se', padx=80, pady=30)

mainWindow.mainloop()
