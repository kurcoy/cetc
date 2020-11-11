from PyQt5 import QtCore, QtWidgets
from Ui_MainWindow  import Ui_WindowObject
from calc import myReport

import math
from PyQt5.QtGui import QIcon, QPixmap
import ctypes

class myWindow(QtWidgets.QMainWindow, Ui_WindowObject):
    def __init__(self, WindowObject):
        super(myWindow, self).__init__(WindowObject)
        super().setupUi( WindowObject)   
        
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.lineEdit_13.setGraphicsEffect(op)
        #self.lineEdit_13.setAutoFillBackground(True)
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.label_13.setGraphicsEffect(op)
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.label_13.setGraphicsEffect(op)
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.lineEdit_7.setGraphicsEffect(op)
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.label_7.setGraphicsEffect(op)
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.lineEdit_9.setGraphicsEffect(op)
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.label_9.setGraphicsEffect(op)
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.label_23.setGraphicsEffect(op)
        op = QtWidgets.QGraphicsOpacityEffect()
        op.setOpacity(0)
        self.lineEdit_16.setGraphicsEffect(op)
                
        icon = QIcon()
        icon.addPixmap(QPixmap("C:\\Users\\Cliff\\Documents\\Projects\\trunk\\02_python\\ShearPinCalc\\CASC.png"))
        WindowObject.setWindowIcon(icon)
        
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID("myappid")

        self.Calculation.triggered.connect(self.On_Calculation_Clicked) 
        self.Generation.triggered.connect(self.On_Report_Clicked)
        self.Printing.triggered.connect(self.On_Print_Clicked)
        self.tabWidget.currentChanged.connect(self.tabchange) 
       
        self.Calculation.setShortcut('c')
        self.Generation.setShortcut('w')
        self.Printing.setShortcut('p')
 
        self._monitorList = [self.input_1, self.input_2, self.input_3, self.input_4, self.input_5, self.input_6, self.input_7, self.input_8, self.input_9, self.input_10, self.input_11, self.input_12, self.output_1]

        for i in range(0, 13):
            self._monitorList[i].installEventFilter(self) 
 
        self.__calValueList = list(range(0, 9))
        self.__tabOrderList =  [ self.input_1, self.input_2, self.input_3,self.input_4,self.input_11, self.input_12, self.output_1 ]
        self.__tabOrdercurr = 0
        self.__tabNumber = 6  
        self.__tabOrderList[0].setFocus()
        self._calcReport = myReport()
        
        self.validation = 0
        self.Calc_result ={ 'P1':0, 'P2':0, 'P':0,'p':0, 'S':0, 'S1':0, 'S2':0, 'n':0, 'N':0, 'PMax':0, 'PMin':0 }
        self.Calc_input  ={ 'R':0,'No':0, 'h1':0, 'h2':0, 'h3':0, 'ρ1':0,'ρ2':0,'t':0, 'S0':0, 'P0':0 }
        
        self.tabWidget.setCurrentIndex(0)
    
    def eventFilter(self, source, event):    
            if source in self._monitorList: 
                if event.type() == QtCore.QEvent.KeyPress: 
                    key = event.key()
                    if key == QtCore.Qt.Key_Return:
                        #self.input_1.setText("测试")
                        print("test is running")
                        self.__tabOrderList[self.__tabOrdercurr].setFocus()
                       
                        if( self.__tabOrdercurr < self.__tabNumber ): 
                            self.__tabOrdercurr = self.__tabOrdercurr + 1
                        else:
                            self.__tabOrdercurr = 1
                            self.On_Calculation_Clicked()
            return QtWidgets.QMainWindow.eventFilter(self, source, event) 
         
    def tabchange(self):
        self.__tabOrdercurr = 1
        self.__tabOrderList.clear()
        
        if self.tabWidget.currentIndex()==0:
            self.__tabNumber = 6
            self.Calc_input['R'] = 0
            self.__tabOrderList = [ self.input_1, self.input_2, self.input_3,self.input_4,self.input_11, self.input_12, self.output_1 ]
            self.YB_Shear_CalcInit( )
            #print('index:',self.tabWidget.currentIndex())
        else:
            self.__tabNumber = 9 
            self.Calc_input['R'] = 1
            self.__tabOrderList = [ self.input_1, self.input_5, self.input_6,self.input_7,self.input_8, self.input_9,self.input_10,self.input_11, self.input_12, self.output_1 ]
            #print('index:',self.tabWidget.currentIndex())
        self.__tabOrderList[self.__tabOrdercurr].setFocus() 
        self.__tabOrdercurr = self.__tabOrdercurr + 1
  
    def YB_Shear_CalcInit(self):
        
        self.Calc_input['ρ1']= self.__calValueList[1]
        self.Calc_input['ρ2']= 0
        self.Calc_input['P0']= self.__calValueList[2]
        self.Calc_input['h1']= self.__calValueList[3]
        self.Calc_input['h2']= 0
        self.Calc_input['h3']= 0
        self.Calc_input['S0']= self.__calValueList[4]
        self.Calc_input['t']  = self.__calValueList[5]
         
        self.Calc_result['P'] = self.Calc_input['h1'] * self.Calc_input['ρ1'] *0.0098
                 
    def YCE_Shear_CalcInit(self): 
        
        self.Calc_input['ρ1']= self.__calValueList[1]
        self.Calc_input['h1']= self.__calValueList[2]
        self.Calc_input['ρ2']= self.__calValueList[3]
        self.Calc_input['h2']= self.__calValueList[4]
        self.Calc_input['P0']= self.__calValueList[5]
        self.Calc_input['h3']= self.__calValueList[6]
        self.Calc_input['S0']= self.__calValueList[7]
        self.Calc_input['t']= self.__calValueList[8]
         
        self.Calc_result['P1'] = self.Calc_input['h1'] * self.Calc_input['ρ1'] *0.0098
        self.Calc_result['P2'] = self.Calc_input['h3'] * self.Calc_input['ρ2'] *0.0098 + (self.Calc_input['h1'] -self.Calc_input['h2'] )* self.Calc_input['ρ1'] *0.0098
        self.Calc_result['P']  =  self.Calc_result['P1']  -  self.Calc_result['P2'] 
        #print(self.Calc_input)
         
    def Shear_Calc(self):                
        self.Calc_result['p'] = self.Calc_input['t']*(0.0016- self.Calc_input['t'] *0.000002)-0.0443
        self.Calc_result['S'] = self.Calc_input['S0'] *(1-self.Calc_result['p'])
        self.Calc_result['S1'] = self.Calc_result['S'] *(1+0.05)
        self.Calc_result['S2'] = self.Calc_result['S'] *(1-0.05)
        self.Calc_result['n'] = (self.Calc_result['P']+self.Calc_input['P0']) / self.Calc_result['S2'] 
        self.Calc_result['N'] = math.ceil(self.Calc_result['n'] )
        self.Calc_result['PMax'] = self.Calc_result['n'] *self.Calc_result['S1'] -self.Calc_result['P'] 
        self.Calc_result['PMin'] = self.Calc_result['n'] *self.Calc_result['S2'] -self.Calc_result['P']  
        
        for key,  value in self.Calc_result.items():
            self.Calc_result[key] = round(value, 2)
        print(self.Calc_result)
     
    def Update_Control_Value(self):
        self.validation = 1
        for i in range(1, self.__tabNumber):
            self.__calValueList[i] = float( self.__tabOrderList[i].text())
            if self.__calValueList[i] <= 0: 
                self.validation = 0
                break 
        return self.validation
    
    def On_Calculation_Clicked(self):
        self.output_2.setText("")
        self.output_1.setText("")
        self.output_3.setText("")
        self.output_4.setText("")
            
        if self.Update_Control_Value() == 1:
            self.Calc_input['No']= self.__tabOrderList[0].text()  
            
            if self.Calc_input['R'] == 0:
                self.YB_Shear_CalcInit()
            else:
                self.YCE_Shear_CalcInit()
            
            self.Shear_Calc()
            
            self.output_2.setText(str(self.Calc_result['n']))
            self.output_1.setText(str(100*self.Calc_result['p']))
            self.output_3.setText(str(self.Calc_result['PMax']))
            self.output_4.setText(str(self.Calc_result['PMin']))

    def On_Report_Clicked(self):
        self.On_Calculation_Clicked( )
        self._calcReport.Param_Init(self.Calc_input, self.Calc_result)
        self._calcReport.GenerateWord()   
        self._calcReport.Save() 
        
    def On_Print_Clicked(self):
        self._calcReport.Param_Init(self.Calc_input, self.Calc_result) 
        self._calcReport.Print()
        
        
        
if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    mainWindow = QtWidgets.QMainWindow()
    ui = myWindow(mainWindow)
    #ui.setupUi(WindowObject)
    #app.installEventFilter(mainWindow)
    mainWindow.show()
    sys.exit(app.exec_())
