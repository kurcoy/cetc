# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'C:\Users\Cliff\Documents\Projects\trunk\02_python\ShearPinCalc\about.ui'
#
# Created by: PyQt5 UI code generator 5.11.3
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_AboutDialog(object):
    def setupUi(self, AboutDialog):
        AboutDialog.setObjectName("AboutDialog")
        AboutDialog.resize(485, 199)
        self.buttonBox = QtWidgets.QDialogButtonBox(AboutDialog)
        self.buttonBox.setGeometry(QtCore.QRect(100, 160, 341, 32))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.label = QtWidgets.QLabel(AboutDialog)
        self.label.setGeometry(QtCore.QRect(20, 10, 431, 161))
        self.label.setObjectName("label")

        self.retranslateUi(AboutDialog)
        self.buttonBox.accepted.connect(AboutDialog.accept)
        self.buttonBox.rejected.connect(AboutDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(AboutDialog)

    def retranslateUi(self, AboutDialog):
        _translate = QtCore.QCoreApplication.translate
        AboutDialog.setWindowTitle(_translate("AboutDialog", "软件帮助"))
        self.label.setText(_translate("AboutDialog", "<html><head/><style type=\"text/css\">p{color:balck;font-size:10pt; font-family:\"宋体\";margin:5px}\\n</style><body><p>快捷键 </p><p>进制 ---- Art+M   计算 ---- Art+C   </p><p>输出 ---- Art+W   打印 ---- Art+P  </p><p><br/></p><p>剪切销计算器 1.0（测试版） </p><p>川南航天能源科技有限公司 版权所有！</p></body></html>"))


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    AboutDialog = QtWidgets.QDialog()
    ui = Ui_AboutDialog()
    ui.setupUi(AboutDialog)
    AboutDialog.show()
    sys.exit(app.exec_())

