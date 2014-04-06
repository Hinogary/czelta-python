import coincidence_ui
from coincidence_ui import _translate
from PyQt4 import QtGui, QtCore
import sys, os, czelta,locale

sys_lang = locale.getdefaultlocale()[0][:2]

for i in range(len(sys.argv)):
    if sys.argv[i]=="-lang":
        sys_lang = sys.argv[i+1]

class MainWindow(coincidence_ui.Ui_MainWindow):

    def __init__(self):
        self.last_directory = None
        self.mainwindow = QtGui.QMainWindow()
        self.setupUi(self.mainwindow)
        self.mainwindow.show()
        #QtCore.QObject.connect(self.button_select_data, QtCore.SIGNAL('clicked()'), self.select_data)

def main():
    app = QtGui.QApplication(sys.argv)
    trans = QtCore.QTranslator()
    trans.load("coincidence_%s.qm"%sys_lang) or trans.load("coincidence_en.qm")
    app.installTranslator(trans)
    mw = MainWindow()
    sys.exit(app.exec_())
    
if __name__ == "__main__":
    main()
