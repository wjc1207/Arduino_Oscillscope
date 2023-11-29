# Arduino_Oscillscope

This is an Arduino-based Oscillscope written in the Atmel Studio 7.0 environment. A PyQt5 based python program is written to read the voltage data and plot them. It can also be used to switch the ADC channels from A0 to A5, show the statistics data and save voltage data.

![volatge plot of the RC circuit](graphs/plot1.png)

![volatge plot of the RC circuit](graphs/plot2.png)

![statistics and save data](graphs/statistics_data_and_save_data.png)

![switch ADC channel](graphs/switch_ADC_channel.png)

The performance of this application is not so good as expected, which can only measure the sine wave for 20Hz, that is the sample frequency Fs = 40Hz.

![generate 20Hz sine wave](20Hz_sine_wave_generation.JPG)

![plot of 20Hz sine wave](plot_20Hz_sine_wave.JPG)


