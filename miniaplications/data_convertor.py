import data_convertor_ui
from PyQt4 import QtGui, QtCore
import sys, os, czelta

class MainWindow(data_convertor_ui.Ui_MainWindow):

    def select_data(self):
        fname = str(QtGui.QFileDialog.getOpenFileName(self.mainwindow, 'Open file', 
            self.last_directory if self.last_directory else os.path.expanduser('~'),"Shower data (*.dat *.txt)"))
        if fname=='':
            return
        self.last_directory = os.path.dirname(fname)
        self.path_data.setText(fname)
        
    def convert_data(self):
        format = "dat" if self.radio_dat_file.isChecked() else "txt"
        fname = QtGui.QFileDialog.getSaveFileName(self.mainwindow, 'Save file', self.last_directory, "Shower data (*.%s)"%format)
        if fname=='':
            return
        if not fname[-4:] in (".txt",".dat"):
            fname += "."+format

        event_reader = czelta.event_reader()
        event_reader.load(str(self.path_data.displayText()))
        
        if self.filter_calibrations.isChecked():
            event_reader.filter_calibrations()
            
        if self.filter_maximum_TDC.isChecked():
            event_reader.filter_maximum_TDC()
            
        if self.filter_maximum_ADC.isChecked():
            event_reader.filter_maximum_ADC()
        
        if self.filter_minimum_ADC.isChecked():
            event_reader.filter_minimum_ADC()
        event_reader.save(str(fname), filter_x_events.isChecked())
        
    def x_events_set_enabled(self, enabled):
        self.filter_x_events.setEnabled(enabled)
    def __init__(self):
        self.last_directory = None
        self.mainwindow = QtGui.QMainWindow()
        self.setupUi(self.mainwindow)
        self.mainwindow.show()
        QtCore.QObject.connect(self.button_select_data, QtCore.SIGNAL('clicked()'), self.select_data)
        QtCore.QObject.connect(self.button_convert, QtCore.SIGNAL('clicked()'), self.convert_data)
        QtCore.QObject.connect(self.radio_txt_file, QtCore.SIGNAL('toggled(bool)'), self.x_events_set_enabled)

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    mw = MainWindow()
    sys.exit(app.exec_())
