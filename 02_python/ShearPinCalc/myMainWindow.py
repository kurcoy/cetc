from PyQt5 import QtCore, QtGui, QtWidgets
from Ui_MainWindow  import Ui_WindowObject
from calc import myReport
import time
import math

class myWindow(QtWidgets.QMainWindow, Ui_WindowObject):
    def __init__(self, WindowObject):
        super(myWindow, self).__init__(WindowObject)
        super().setupUi( WindowObject)   
       
        self._monitorList = [self.input_1, self.input_2, self.input_3, self.input_4, self.input_5, self.input_6, self.input_7, self.input_8, self.input_9, self.input_10, self.input_11, self.input_12, self.output_1]
        
        self.action_5.triggered.connect(self.On_Calculation_Clicked)
        self.tabWidget.currentChanged.connect(self.tabchange) 
        for i in range(0, 13):
            self._monitorList[i].installEventFilter(self) 
 
        self.__calValueList = list(range(0, 9))
        self.__tabOrderList =  [ self.input_1, self.input_2, self.input_3,self.input_4,self.input_11, self.input_12, self.output_1 ]
        self.__tabOrdercurr = 0
        self.__tabNumber = 6  
        self.__tabOrderList[0].setFocus()
        self._calcReport = myReport()
        
        self.Calc_result ={ 'P1':0, 'P2':0, 'P':0,'p':0, 'S':0, 'S1':0, 'S2':0, 'n':0, 'PMax':0, 'PMin':0 }
        self.Calc_input ={ 'h1':0, 'h2':0, 'h3':0, 'ρ1':0,'ρ2':0,'t':0, 'S0':0, 'P0':0 }
        
        self.tabWidget.setCurrentIndex(0)
    
    def eventFilter(self, source, event):    
            if source in self._monitorList: 
                if event.type() == QtCore.QEvent.KeyPress: 
                    key = event.key()
                    if key == QtCore.Qt.Key_Return:
                        self.input_1.setText("测试")
                        print("test is running")
                        self.__tabOrderList[self.__tabOrdercurr].setFocus()
                       
                        if( self.__tabOrdercurr < self.__tabNumber ): 
                            self.__tabOrdercurr = self.__tabOrdercurr + 1
                        else:
                            self.__tabOrdercurr = 1
                            #triger to calculation and report generated                            
                            #self._calcReport.Generate()
                            #calcReport.Save()
                            #self._calcReport.Print()
                            print(time.strftime("%Y-%m-%d %H%M%S", time.localtime()))
                            self.On_Calculation_Clicked()
            return QtWidgets.QMainWindow.eventFilter(self, source, event) 
         
    def tabchange(self):
        self.__tabOrdercurr = 1
        self.__tabOrderList.clear()
        
        if self.tabWidget.currentIndex()==0:
            self.__tabNumber = 6
            self.__tabOrderList = [ self.input_1, self.input_2, self.input_3,self.input_4,self.input_11, self.input_12, self.output_1 ]
            self.YB_CalcInit()
            
            print('index:',self.tabWidget.currentIndex())
        else:
            self.__tabNumber = 9 
            self.__tabOrderList = [ self.input_1, self.input_5, self.input_6,self.input_7,self.input_8, self.input_9,self.input_10,self.input_11, self.input_12, self.output_1 ]
            self.YCE_CalcInit()
            
            print('index:',self.tabWidget.currentIndex())
        self.__tabOrderList[self.__tabOrdercurr].setFocus() 
        self.__tabOrdercurr = self.__tabOrdercurr + 1
        
        
    def On_Calculation_Clicked(self):
        
        print(time.strftime("%Y-%m-%d %H%M%S", time.localtime()))
        for i in range(1, self.__tabNumber):
            self.__calValueList[i] = float( self.__tabOrderList[i].text())
        
        self.YB_Calc()
        self.output_2.setText(str(self.Calc_result['n']))
        self.output_1.setText(str(round(100*self.Calc_result['p'], 2)))
        self.output_3.setText(str(round(self.Calc_result['PMax'], 2)))
        self.output_4.setText(str(round(self.Calc_result['PMin'], 2)))
        self._calcReport.Param_Init(self.Calc_input, self.Calc_result)
        self._calcReport.Generate()   
        self._calcReport.Save() 
        
    def YB_CalcInit(self):
        for i in range(1, self.__tabNumber):
            self.__calValueList[i] = float( self.__tabOrderList[i].text())
            
        self.Calc_input['ρ1']= self.__calValueList[1]
        self.Calc_input['ρ2']= 0
        self.Calc_input['P0']= self.__calValueList[2]
        self.Calc_input['h1']= self.__calValueList[3]
        self.Calc_input['h2']= 0
        self.Calc_input['h3']= 0
        self.Calc_input['S0']= self.__calValueList[4]
        self.Calc_input['t']= self.__calValueList[5]
         
        self.Calc_result['P'] = self.Calc_input['h1'] * self.Calc_input['ρ1'] *0.0098
        #print(self.Calc_input)
         
    def YCE_CalcInit(self): 
        for i in range(1, self.__tabNumber):
            self.__calValueList[i] = float( self.__tabOrderList[i].text())
            
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
         
    def YB_Calc(self):                
        self.Calc_result['p'] = self.Calc_input['t'] * (self.Calc_input['t'] *(-0.000002) + 0.0016)-0.0443
        self.Calc_result['S'] = self.Calc_input['S0'] *(1-self.Calc_result['p'])
        self.Calc_result['S1'] = self.Calc_result['S'] *(1+0.05)
        self.Calc_result['S2'] = self.Calc_result['S'] *(1-0.05)
        self.Calc_result['n'] = math.ceil((self.Calc_result['P']+self.Calc_input['P0']) / self.Calc_result['S2'] )
        self.Calc_result['PMax'] = self.Calc_result['n'] *self.Calc_result['S1'] -self.Calc_result['P'] 
        self.Calc_result['PMin'] = self.Calc_result['n'] *self.Calc_result['S2'] -self.Calc_result['P']  
        print(self.Calc_result)
         
if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    mainWindow = QtWidgets.QMainWindow()
    ui = myWindow(mainWindow)
    #ui.setupUi(WindowObject)
    #app.installEventFilter(mainWindow)
    mainWindow.show()
    sys.exit(app.exec_())
