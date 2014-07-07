#!/usr/bin/env python
# vim: set fileencoding=UTF-8 :
import coincidence_ui
from coincidence_ui import _translate
from PyQt4 import QtGui, QtCore
import sys, os, czelta,locale

path = os.path.dirname(__file__)
path = path+os.sep if path!="" else ""
sys_lang = locale.getdefaultlocale()[0][:2]

for i in range(len(sys.argv)):
    if sys.argv[i]=="-lang":
        sys_lang = sys.argv[i+1]

class MainWindow(coincidence_ui.Ui_MainWindow):
    def select_1_data(self):
        self.select_data(1)
        
    def select_2_data(self):
        self.select_data(2)
        
    def select_3_data(self):
        self.select_data(3)
    
    def data_1_changed(self):
        self.ers_changed[0] = True
    
    def data_2_changed(self):
        self.ers_changed[1] = True
        
    def data_3_changed(self):
        self.ers_changed[2] = True
        
    def select_data(self, station):
        fname = str(QtGui.QFileDialog.getOpenFileName(self.mainwindow, 'Open file', 
            self.last_directory if self.last_directory else os.path.expanduser('~'),"Shower data (*.dat *.txt)"))
        if fname=='':
            return
        self.last_directory = os.path.dirname(fname)
        data = self.data_1 if station==1 else self.data_2 if station==2 else self.data_3
        data.setText(fname)

    def set_triple(self, val):
        self.st_g_3.setEnabled(val)

    def find_coincidences(self):
        save_events = self.save_events.isChecked()
        triple = self.triple_coin.isChecked()
        data = self.data_1, self.data_2, self.data_3
        for i in range(3 if triple else 2):
            try:
                if self.ers_changed[i]:
                    self.ers[i].load(str(data[i].text()))
                    self.ers_changed[i] = False
            except IOError:
                QtGui.QMessageBox.warning(self.mainwindow,
                    _translate("MainWindow", "error_title", None), 
                    _translate("MainWindow", "error_cant_open_data_%d", None)%(i+1))
                return
            except NotImplementedError:
                QtGui.QMessageBox.warning(self.mainwindow,
                    _translate("MainWindow", "error_title", None), 
                    _translate("MainWindow", "error_unsupported_data_%d", None)%(i+1))
                return
        if save_events and self.delta_spin.value()*1e-6 > 0.1:
            answer = QtGui.QMessageBox.question(self.mainwindow, 
                _translate("MainWindow", "question_limit", None), 
                _translate("MainWindow", "are_you_sure_giant_limit", None), 
                QtGui.QMessageBox.Yes, QtGui.QMessageBox.No)
            if answer!=QtGui.QMessageBox.Yes:
                self.save_events.setChecked(False)
                save_events = False
        if triple:
            ers = self.ers[0], self.ers[1], self.ers[2]
        else:
            ers = self.ers[0], self.ers[1]
        c = czelta.coincidence(ers, self.delta_spin.value()*1e-6, save_events = save_events)
        self.number_of_coincidences.setText("%d"%len(c))
        self.medium_value.setText("%.2f"%c.expected_value)
        self.percents.setText("%.2f %%"%(c.chance*100))
        self.lenght_of_measuring.setText("%.2f %s"%(c.overlap_measure_time/86400, _translate("MainWindow","days",None)))
        self.all_events_0.setText("%d"%c.overlap_normal_events[0])
        self.all_events_1.setText("%d"%c.overlap_normal_events[1])
        if triple:
            self.all_events_2.setText("%d"%c.overlap_normal_events[2])
        else:
            self.all_events_2.setText("")
        if save_events:
            self.coincidence_text_edit.setPlainText("")
            for coin in c:
                if triple:
                    if len(coin)==6 and coin[4]!=None:
                        text = u"%.2f μs (%.2f, %.2f) (%.2f, %.2f)\n%s\n%s\n%s\n"%(coin[0]*1e6, coin[4][0], coin[4][1], coin[5][0], coin[5][1],str(coin[1]),str(coin[2]),str(coin[3]))
                    else:
                        text = u"%.2f μs\n%s\n%s\n%s\n"%(coin[0]*1e6,str(coin[1]),str(coin[2]),str(coin[3]))
                else:
                    text = u"%.2f μs\n%s\n%s\n"%(coin[0]*1e6,str(coin[1]),str(coin[2]))
                self.coincidence_text_edit.insertPlainText(text)
            self.coincidence_text_edit.show()
        else:
            self.coincidence_text_edit.hide()
        self.coincidence_text_edit.setEnabled(True);
    
    def __init__(self):
        self.ers = czelta.event_reader(), czelta.event_reader(), czelta.event_reader()
        self.ers_changed = [True, True, True]
        self.last_directory = None
        self.mainwindow = QtGui.QMainWindow()
        self.setupUi(self.mainwindow)
        self.mainwindow.show()
        self.statusBar.showMessage(u"© 2013-2014 Martin Quarda")
        QtCore.QObject.connect(self.data_1_select, QtCore.SIGNAL('clicked()'), self.select_1_data)
        QtCore.QObject.connect(self.data_2_select, QtCore.SIGNAL('clicked()'), self.select_2_data)
        QtCore.QObject.connect(self.data_3_select, QtCore.SIGNAL('clicked()'), self.select_3_data)
        QtCore.QObject.connect(self.calc_coincidences, QtCore.SIGNAL('clicked()'), self.find_coincidences)
        QtCore.QObject.connect(self.triple_coin, QtCore.SIGNAL('toggled(bool)'), self.set_triple)
        
        QtCore.QObject.connect(self.data_1, QtCore.SIGNAL('textChanged(const QString&)'), self.data_1_changed)
        QtCore.QObject.connect(self.data_2, QtCore.SIGNAL('textChanged(const QString&)'), self.data_2_changed)
        QtCore.QObject.connect(self.data_3, QtCore.SIGNAL('textChanged(const QString&)'), self.data_3_changed)

def main():
    app = QtGui.QApplication(sys.argv)
    trans = QtCore.QTranslator()
    trans.load("%scoincidence_%s.qm"%(path, sys_lang)) or trans.load("%scoincidence_en.qm"%path)
    app.installTranslator(trans)
    mw = MainWindow()
    sys.exit(app.exec_())
    
if __name__ == "__main__":
    main()
