import sys
import serial
import numpy as np
from PyQt5.QtWidgets import QApplication, QMainWindow
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore
import threading  # Import the threading module for asynchronous input reading

class RealTimePlot(QMainWindow):
    def __init__(self):
        super().__init__()

        self.init_ui()

        # set baud rate
        self.ser = serial.Serial('COM3', 38400)  

        # create voltage list
        self.voltages = []

        # create timer
        self.timer = QtCore.QTimer(self)
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(0)

    def init_ui(self):
        # create plot widget
        self.plot_widget = pg.PlotWidget()
        self.setCentralWidget(self.plot_widget)

        # set background color as white and curve color as red
        self.plot_widget.setBackground('w')

        # set window size
        self.resize(800, 600)

        # create plot curve
        pen = pg.mkPen(color='r', width=5)  # Adjust the width as needed
        self.curve = self.plot_widget.plot(pen=pen)

        # set plot title and axis labels
        self.plot_widget.setTitle('Voltage Reading from Serial Port')
        self.plot_widget.setLabel('left', 'Voltage', 'V')
        self.plot_widget.setLabel('bottom', 'Time', 's')
        self.plot_widget.showGrid(x=True, y=True)
        self.plot_widget.setYRange(0, 5)

    def update_plot(self):
        # read from the serial port
        line = self.ser.readline().decode('utf-8').strip()

        # convert data
        voltage = float(line)/1024*5

        # add to the voltage list
        self.voltages.append(voltage)

        # limit the number of data points shown to improve performance
        if len(self.voltages) > 100:
            self.voltages.pop(0)

        # update the plot
        self.curve.setData(np.array(self.voltages))

    # Function to read input from the console and write it to the serial port
    def write_to_serial(self):
        while True:
            user_input = input("Enter data to send to serial: ")
            if user_input:
                try: #check if the input is a number
                    channel_num = int(user_input)
                    if (channel_num >=0 and channel_num <= 5):
                        self.ser.write(user_input.encode())
                        print('ADC channel changed to channel %d' % channel_num)
                except: #if the input is not a number
                    if user_input == 'save':
                        np.savetxt('voltage_data.txt', np.array(self.voltages).T)
                        print('Data saved')
                    elif user_input == 'show stat': #show statistics
                        print('')
                        print('Average voltage: ' + str(np.mean(self.voltages)))
                        print('Minimum voltage: ' + str(np.min(self.voltages)))
                        print('Maximum voltage: ' + str(np.max(self.voltages)))
                        print('Peak-to-peak voltage: ' + str(np.max(self.voltages) - np.min(self.voltages)))
                        print('RMSE: ' + str(np.sqrt(np.mean(np.array(self.voltages)**2))))
                        print('')
                    elif user_input == 'help':
                        print('')
                        print('save: save data to voltage_data.txt')
                        print('show stat: show statistics of the voltage data')
                        print('0-5: change the adc channel (default: 0)')
                        print('')
                    else:
                        print('Invalid input')

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = RealTimePlot()
    window.show()
    # Start the input reading thread
    input_thread = threading.Thread(target=window.write_to_serial)
    input_thread.start()
    sys.exit(app.exec_())