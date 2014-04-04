# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'test.ui'
#
# Created: Fri Apr  4 17:46:04 2014
#      by: PyQt4 UI code generator 4.10.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(691, 241)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.verticalLayout_2 = QtGui.QVBoxLayout()
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.label = QtGui.QLabel(self.centralwidget)
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout_2.addWidget(self.label)
        self.path_data = QtGui.QLineEdit(self.centralwidget)
        self.path_data.setObjectName(_fromUtf8("path_data"))
        self.horizontalLayout_2.addWidget(self.path_data)
        self.button_select_data = QtGui.QToolButton(self.centralwidget)
        self.button_select_data.setObjectName(_fromUtf8("button_select_data"))
        self.horizontalLayout_2.addWidget(self.button_select_data)
        self.verticalLayout_2.addLayout(self.horizontalLayout_2)
        spacerItem = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout_2.addItem(spacerItem)
        self.horizontalLayout.addLayout(self.verticalLayout_2)
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.radio_dat_file = QtGui.QRadioButton(self.centralwidget)
        self.radio_dat_file.setChecked(True)
        self.radio_dat_file.setObjectName(_fromUtf8("radio_dat_file"))
        self.verticalLayout.addWidget(self.radio_dat_file)
        self.radio_txt_file = QtGui.QRadioButton(self.centralwidget)
        self.radio_txt_file.setObjectName(_fromUtf8("radio_txt_file"))
        self.verticalLayout.addWidget(self.radio_txt_file)
        self.filter_x_events = QtGui.QCheckBox(self.centralwidget)
        self.filter_x_events.setEnabled(False)
        self.filter_x_events.setCheckable(True)
        self.filter_x_events.setObjectName(_fromUtf8("filter_x_events"))
        self.verticalLayout.addWidget(self.filter_x_events)
        self.filter_calibrations = QtGui.QCheckBox(self.centralwidget)
        self.filter_calibrations.setObjectName(_fromUtf8("filter_calibrations"))
        self.verticalLayout.addWidget(self.filter_calibrations)
        self.filter_maximum_TDC = QtGui.QCheckBox(self.centralwidget)
        self.filter_maximum_TDC.setObjectName(_fromUtf8("filter_maximum_TDC"))
        self.verticalLayout.addWidget(self.filter_maximum_TDC)
        self.filter_maximum_ADC = QtGui.QCheckBox(self.centralwidget)
        self.filter_maximum_ADC.setObjectName(_fromUtf8("filter_maximum_ADC"))
        self.verticalLayout.addWidget(self.filter_maximum_ADC)
        self.filter_minimum_ADC = QtGui.QCheckBox(self.centralwidget)
        self.filter_minimum_ADC.setObjectName(_fromUtf8("filter_minimum_ADC"))
        self.verticalLayout.addWidget(self.filter_minimum_ADC)
        spacerItem1 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem1)
        self.horizontalLayout.addLayout(self.verticalLayout)
        self.verticalLayout_3 = QtGui.QVBoxLayout()
        self.verticalLayout_3.setObjectName(_fromUtf8("verticalLayout_3"))
        self.button_convert = QtGui.QPushButton(self.centralwidget)
        self.button_convert.setObjectName(_fromUtf8("button_convert"))
        self.verticalLayout_3.addWidget(self.button_convert)
        spacerItem2 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout_3.addItem(spacerItem2)
        self.horizontalLayout.addLayout(self.verticalLayout_3)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 691, 25))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow", None))
        self.label.setText(_translate("MainWindow", "Selected data:", None))
        self.button_select_data.setText(_translate("MainWindow", "...", None))
        self.radio_dat_file.setText(_translate("MainWindow", "output Web-like .dat", None))
        self.radio_txt_file.setText(_translate("MainWindow", "output Web-like .txt", None))
        self.filter_x_events.setText(_translate("MainWindow", "Filter \"x\" events", None))
        self.filter_calibrations.setText(_translate("MainWindow", "Filter calibrations", None))
        self.filter_maximum_TDC.setText(_translate("MainWindow", "Filter events with any TDC = 4095", None))
        self.filter_maximum_ADC.setText(_translate("MainWindow", "Filter events with any ADC = 2047", None))
        self.filter_minimum_ADC.setText(_translate("MainWindow", "Filter events with any ADC = 0", None))
        self.button_convert.setText(_translate("MainWindow", "Convert", None))

