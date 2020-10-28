from PyQt5 import QtCore, QtGui, QtWidgets
from Ui_MainWindow  import Ui_WindowObject

class myWindow(QtWidgets.QMainWindow, Ui_WindowObject):

    def __init__(self, WindowObject):
        super(myWindow, self).__init__(WindowObject)
        super().setupUi( WindowObject)   
        
        self.inputList = [self.input_1, self.input_2, self.input_3, self.input_4, self.input_5, self.input_6, self.input_7, self.input_8, self.input_9, self.input_10, self.input_11, self.input_12, self.input_13]
        for i in range(0, 13):
            self.inputList[i].installEventFilter(self) 
 
        self.__tabOrderList = [ self.input_1, self.input_11, self.input_12, self.input_13 ]
        self.__tabOrdercurr = 0
        
        #when tab control changed, the insertion controls also changed
        self.__tabOrderList.insert( 1,  self.input_4)
        self.__tabOrderList.insert( 1,  self.input_3)
        self.__tabOrderList.insert( 1,  self.input_2)
        
    ''''
    self.input_1.installEventFilter(self) 
    self.input_2.installEventFilter(self) 
    self.input_3.installEventFilter(self) 
    self.input_4.installEventFilter(self) 
    self.input_5.installEventFilter(self) 
    self.input_6.installEventFilter(self) 
    self.input_7.installEventFilter(self) 
    self.input_8.installEventFilter(self) 
    self.input_9.installEventFilter(self) 
    self.input_10.installEventFilter(self) 
    self.input_11.installEventFilter(self) 
    self.input_12.installEventFilter(self) 
    self.input_13.installEventFilter(self) 
        
    self.input_2.textchanged.connect(self.input_2_textchanged)
    def input_2_textchanged(self):
        print( "单击了OK按钮")     
        self.input_1.setText("测试")
  '''
    
    def eventFilter(self, source, event):    
            if source in self.inputList: # 只对input_x的事件进行过滤，重写其行为，其他的事件会被忽略 source == self.input_1
                if event.type() == QtCore.QEvent.KeyPress: # 这里对鼠标按下事件进行过滤，重写其行为
                    key = event.key()
                    if key == QtCore.Qt.Key_Return:
                        self.input_1.setText("测试")
                        print("test is running")
                        self.__tabOrderList[self.__tabOrdercurr].setFocus()
                       
                        if( self.__tabOrdercurr < 6 ): 
                            self.__tabOrdercurr = self.__tabOrdercurr + 1
                        else:
                            self.__tabOrdercurr = 0
            return QtWidgets.QMainWindow.eventFilter(self, source, event) #其他情况会返回系统默认的事件处理方法        
            
if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    mainWindow = QtWidgets.QMainWindow()
    ui = myWindow(mainWindow)
    #ui.setupUi(WindowObject)
    #app.installEventFilter(mainWindow)
    mainWindow.show()
    sys.exit(app.exec_())
