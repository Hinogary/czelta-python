# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'coincidence.ui'
#
# Created: Fri Apr 18 22:24:18 2014
#      by: PyQt4 UI code generator 4.10.4
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
        MainWindow.resize(477, 703)
        self.centralWidget = QtGui.QWidget(MainWindow)
        self.centralWidget.setObjectName(_fromUtf8("centralWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.centralWidget)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.double_coin = QtGui.QRadioButton(self.centralWidget)
        self.double_coin.setChecked(True)
        self.double_coin.setObjectName(_fromUtf8("double_coin"))
        self.horizontalLayout_3.addWidget(self.double_coin)
        self.triple_coin = QtGui.QRadioButton(self.centralWidget)
        self.triple_coin.setObjectName(_fromUtf8("triple_coin"))
        self.horizontalLayout_3.addWidget(self.triple_coin)
        self.verticalLayout.addLayout(self.horizontalLayout_3)
        self.groupBox = QtGui.QGroupBox(self.centralWidget)
        self.groupBox.setObjectName(_fromUtf8("groupBox"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.groupBox)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.label = QtGui.QLabel(self.groupBox)
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout.addWidget(self.label)
        self.data_1 = QtGui.QLineEdit(self.groupBox)
        self.data_1.setObjectName(_fromUtf8("data_1"))
        self.horizontalLayout.addWidget(self.data_1)
        self.data_1_select = QtGui.QToolButton(self.groupBox)
        self.data_1_select.setObjectName(_fromUtf8("data_1_select"))
        self.horizontalLayout.addWidget(self.data_1_select)
        self.verticalLayout.addWidget(self.groupBox)
        self.groupBox_2 = QtGui.QGroupBox(self.centralWidget)
        self.groupBox_2.setObjectName(_fromUtf8("groupBox_2"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.groupBox_2)
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.label_2 = QtGui.QLabel(self.groupBox_2)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.horizontalLayout_2.addWidget(self.label_2)
        self.data_2 = QtGui.QLineEdit(self.groupBox_2)
        self.data_2.setObjectName(_fromUtf8("data_2"))
        self.horizontalLayout_2.addWidget(self.data_2)
        self.data_2_select = QtGui.QToolButton(self.groupBox_2)
        self.data_2_select.setObjectName(_fromUtf8("data_2_select"))
        self.horizontalLayout_2.addWidget(self.data_2_select)
        self.verticalLayout.addWidget(self.groupBox_2)
        self.st_g_3 = QtGui.QGroupBox(self.centralWidget)
        self.st_g_3.setEnabled(False)
        self.st_g_3.setObjectName(_fromUtf8("st_g_3"))
        self.horizontalLayout_4 = QtGui.QHBoxLayout(self.st_g_3)
        self.horizontalLayout_4.setObjectName(_fromUtf8("horizontalLayout_4"))
        self.label_9 = QtGui.QLabel(self.st_g_3)
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.horizontalLayout_4.addWidget(self.label_9)
        self.data_3 = QtGui.QLineEdit(self.st_g_3)
        self.data_3.setObjectName(_fromUtf8("data_3"))
        self.horizontalLayout_4.addWidget(self.data_3)
        self.data_3_select = QtGui.QToolButton(self.st_g_3)
        self.data_3_select.setObjectName(_fromUtf8("data_3_select"))
        self.horizontalLayout_4.addWidget(self.data_3_select)
        self.verticalLayout.addWidget(self.st_g_3)
        self.groupBox_3 = QtGui.QGroupBox(self.centralWidget)
        self.groupBox_3.setObjectName(_fromUtf8("groupBox_3"))
        self.gridLayout = QtGui.QGridLayout(self.groupBox_3)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label_3 = QtGui.QLabel(self.groupBox_3)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 0, 0, 1, 1)
        self.delta_spin = QtGui.QDoubleSpinBox(self.groupBox_3)
        self.delta_spin.setDecimals(3)
        self.delta_spin.setMinimum(0.1)
        self.delta_spin.setMaximum(1000000000.0)
        self.delta_spin.setProperty("value", 2.2)
        self.delta_spin.setObjectName(_fromUtf8("delta_spin"))
        self.gridLayout.addWidget(self.delta_spin, 0, 1, 1, 1)
        self.save_events = QtGui.QCheckBox(self.groupBox_3)
        self.save_events.setChecked(True)
        self.save_events.setObjectName(_fromUtf8("save_events"))
        self.gridLayout.addWidget(self.save_events, 1, 0, 1, 2)
        self.verticalLayout.addWidget(self.groupBox_3)
        self.calc_coincidences = QtGui.QPushButton(self.centralWidget)
        self.calc_coincidences.setObjectName(_fromUtf8("calc_coincidences"))
        self.verticalLayout.addWidget(self.calc_coincidences)
        self.groupBox_4 = QtGui.QGroupBox(self.centralWidget)
        self.groupBox_4.setEnabled(True)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_4.sizePolicy().hasHeightForWidth())
        self.groupBox_4.setSizePolicy(sizePolicy)
        self.groupBox_4.setObjectName(_fromUtf8("groupBox_4"))
        self.formLayout = QtGui.QFormLayout(self.groupBox_4)
        self.formLayout.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout.setObjectName(_fromUtf8("formLayout"))
        self.label_4 = QtGui.QLabel(self.groupBox_4)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_4.sizePolicy().hasHeightForWidth())
        self.label_4.setSizePolicy(sizePolicy)
        self.label_4.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.formLayout.setWidget(0, QtGui.QFormLayout.LabelRole, self.label_4)
        self.number_of_coincidences = QtGui.QLabel(self.groupBox_4)
        self.number_of_coincidences.setText(_fromUtf8(""))
        self.number_of_coincidences.setObjectName(_fromUtf8("number_of_coincidences"))
        self.formLayout.setWidget(0, QtGui.QFormLayout.FieldRole, self.number_of_coincidences)
        self.label_7 = QtGui.QLabel(self.groupBox_4)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_7.sizePolicy().hasHeightForWidth())
        self.label_7.setSizePolicy(sizePolicy)
        self.label_7.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_7.setObjectName(_fromUtf8("label_7"))
        self.formLayout.setWidget(2, QtGui.QFormLayout.LabelRole, self.label_7)
        self.medium_value = QtGui.QLabel(self.groupBox_4)
        self.medium_value.setText(_fromUtf8(""))
        self.medium_value.setObjectName(_fromUtf8("medium_value"))
        self.formLayout.setWidget(2, QtGui.QFormLayout.FieldRole, self.medium_value)
        self.label_10 = QtGui.QLabel(self.groupBox_4)
        self.label_10.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_10.setObjectName(_fromUtf8("label_10"))
        self.formLayout.setWidget(3, QtGui.QFormLayout.LabelRole, self.label_10)
        self.percents = QtGui.QLabel(self.groupBox_4)
        self.percents.setText(_fromUtf8(""))
        self.percents.setObjectName(_fromUtf8("percents"))
        self.formLayout.setWidget(3, QtGui.QFormLayout.FieldRole, self.percents)
        self.line = QtGui.QFrame(self.groupBox_4)
        self.line.setFrameShape(QtGui.QFrame.HLine)
        self.line.setFrameShadow(QtGui.QFrame.Sunken)
        self.line.setObjectName(_fromUtf8("line"))
        self.formLayout.setWidget(4, QtGui.QFormLayout.LabelRole, self.line)
        self.label_5 = QtGui.QLabel(self.groupBox_4)
        self.label_5.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_5.setObjectName(_fromUtf8("label_5"))
        self.formLayout.setWidget(5, QtGui.QFormLayout.LabelRole, self.label_5)
        self.lenght_of_measuring = QtGui.QLabel(self.groupBox_4)
        self.lenght_of_measuring.setText(_fromUtf8(""))
        self.lenght_of_measuring.setObjectName(_fromUtf8("lenght_of_measuring"))
        self.formLayout.setWidget(5, QtGui.QFormLayout.FieldRole, self.lenght_of_measuring)
        self.label_6 = QtGui.QLabel(self.groupBox_4)
        self.label_6.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_6.setObjectName(_fromUtf8("label_6"))
        self.formLayout.setWidget(6, QtGui.QFormLayout.LabelRole, self.label_6)
        self.all_events_0 = QtGui.QLabel(self.groupBox_4)
        self.all_events_0.setText(_fromUtf8(""))
        self.all_events_0.setObjectName(_fromUtf8("all_events_0"))
        self.formLayout.setWidget(6, QtGui.QFormLayout.FieldRole, self.all_events_0)
        self.label_8 = QtGui.QLabel(self.groupBox_4)
        self.label_8.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.label_8.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_8.setObjectName(_fromUtf8("label_8"))
        self.formLayout.setWidget(7, QtGui.QFormLayout.LabelRole, self.label_8)
        self.all_events_1 = QtGui.QLabel(self.groupBox_4)
        self.all_events_1.setText(_fromUtf8(""))
        self.all_events_1.setObjectName(_fromUtf8("all_events_1"))
        self.formLayout.setWidget(7, QtGui.QFormLayout.FieldRole, self.all_events_1)
        self.label_11 = QtGui.QLabel(self.groupBox_4)
        self.label_11.setEnabled(True)
        self.label_11.setObjectName(_fromUtf8("label_11"))
        self.formLayout.setWidget(8, QtGui.QFormLayout.LabelRole, self.label_11)
        self.all_events_2 = QtGui.QLabel(self.groupBox_4)
        self.all_events_2.setText(_fromUtf8(""))
        self.all_events_2.setObjectName(_fromUtf8("all_events_2"))
        self.formLayout.setWidget(8, QtGui.QFormLayout.FieldRole, self.all_events_2)
        self.coincidence_text_edit = QtGui.QTextEdit(self.groupBox_4)
        self.coincidence_text_edit.setEnabled(False)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(1)
        sizePolicy.setHeightForWidth(self.coincidence_text_edit.sizePolicy().hasHeightForWidth())
        self.coincidence_text_edit.setSizePolicy(sizePolicy)
        self.coincidence_text_edit.setTabChangesFocus(False)
        self.coincidence_text_edit.setUndoRedoEnabled(True)
        self.coincidence_text_edit.setReadOnly(True)
        self.coincidence_text_edit.setObjectName(_fromUtf8("coincidence_text_edit"))
        self.formLayout.setWidget(10, QtGui.QFormLayout.SpanningRole, self.coincidence_text_edit)
        self.verticalLayout.addWidget(self.groupBox_4)
        MainWindow.setCentralWidget(self.centralWidget)
        self.statusBar = QtGui.QStatusBar(MainWindow)
        self.statusBar.setObjectName(_fromUtf8("statusBar"))
        MainWindow.setStatusBar(self.statusBar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "coincidence_title", None))
        self.double_coin.setText(_translate("MainWindow", "double_coin", None))
        self.triple_coin.setText(_translate("MainWindow", "triple_coin", None))
        self.groupBox.setTitle(_translate("MainWindow", "1_station", None))
        self.label.setText(_translate("MainWindow", "data_path", None))
        self.data_1_select.setText(_translate("MainWindow", "...", None))
        self.groupBox_2.setTitle(_translate("MainWindow", "2_station", None))
        self.label_2.setText(_translate("MainWindow", "data_path", None))
        self.data_2_select.setText(_translate("MainWindow", "...", None))
        self.st_g_3.setTitle(_translate("MainWindow", "3_station", None))
        self.label_9.setText(_translate("MainWindow", "data_path", None))
        self.data_3_select.setText(_translate("MainWindow", "...", None))
        self.groupBox_3.setTitle(_translate("MainWindow", "parameters_coincidences", None))
        self.label_3.setText(_translate("MainWindow", "max_diff", None))
        self.delta_spin.setSuffix(_translate("MainWindow", " μs", None))
        self.save_events.setText(_translate("MainWindow", "save_events", None))
        self.calc_coincidences.setText(_translate("MainWindow", "find_coincidence", None))
        self.groupBox_4.setTitle(_translate("MainWindow", "result", None))
        self.label_4.setText(_translate("MainWindow", "number_of_coincidnces", None))
        self.label_7.setText(_translate("MainWindow", "medium_value", None))
        self.label_10.setToolTip(_translate("MainWindow", "chance_tooltip", None))
        self.label_10.setText(_translate("MainWindow", "chance", None))
        self.label_5.setText(_translate("MainWindow", "total_measure_time", None))
        self.label_6.setText(_translate("MainWindow", "sum_1_station", None))
        self.label_8.setText(_translate("MainWindow", "sum_2_station", None))
        self.label_11.setText(_translate("MainWindow", "sum_3_station", None))
        self.coincidence_text_edit.setHtml(_translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'Ubuntu\'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", None))

