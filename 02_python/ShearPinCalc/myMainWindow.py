from PyQt5 import QtCore, QtWidgets
from Ui_MainWindow  import Ui_WindowObject
from Ui_about import Ui_AboutDialog
from calc import myReport

import math
from PyQt5.QtGui import QIcon, QPixmap, QRegExpValidator
import ctypes
import myImages_rc

class myWindow(QtWidgets.QMainWindow, Ui_WindowObject):
    def __init__(self, WindowObject):
        super(myWindow, self).__init__(WindowObject)
        super().setupUi( WindowObject)   

        self.SetOpacity()       
        
        icon = QIcon()
        icon.addPixmap(QPixmap(":/myImages/CASC.png"))
        WindowObject.setWindowIcon(icon)
        ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID("myappid")

        self.Calculation.triggered.connect(self.On_Calculation_Clicked) 
        self.Generation.triggered.connect(self.On_Report_Clicked)
        self.Printing.triggered.connect(self.On_Print_Clicked)
        self.tabWidget.currentChanged.connect(self.tabchange) 
        self.Imperial.triggered.connect(self.UnitSwitch_2Imperial)
        self.Metric.triggered.connect(self.UnitSwitch_2Metric)
               
        self.YB_ShearPin_Default   = 3.67 
        self.YCE_ShearPin_Default = 3.78
        self.groudTemp = 25
        
        self._monitorList = [self.input_1, self.input_2, self.input_3, self.input_4, self.input_5, self.input_6, self.input_7, self.input_8, self.input_9, self.input_10, self.input_11, self.input_12]
        regExp=QtCore.QRegExp('^\0|[1-9]\\d{0,4}\\.\\d{1,2}$') 
        
        for i in range(0, 12):
            self._monitorList[i].installEventFilter(self) 
            if i > 0: 
                self._monitorList[i].setValidator((QRegExpValidator(regExp,self)))
        
        regExp=QtCore.QRegExp('^\0|[0-3]\\d{0}\\.\\d{1,2}$') 
        self.input_2.setValidator((QRegExpValidator(regExp,self)))
        self.input_5.setValidator((QRegExpValidator(regExp,self)))
        self.input_7.setValidator((QRegExpValidator(regExp,self)))
        regExp=QtCore.QRegExp('^\0|[1-5]\\d{0,2}\\.\\d{1,2}$') 
        self.input_12.setValidator((QRegExpValidator(regExp,self)))
        regExp=QtCore.QRegExp('^\0|[1-9]\\d{0,1}\\.\\d{1,2}$') 
        self.input_3.setValidator((QRegExpValidator(regExp,self)))
        self.input_9.setValidator((QRegExpValidator(regExp,self)))
        
        self.__calValueList = list(range(0, 9))
        self.__tabOrderList =  [ self.input_1, self.input_2, self.input_3,self.input_4,self.input_11, self.input_12 ]
        self.__tabOrdercurr = 0
        self.__tabNumber = 6  
        self.__tabOrderList[0].setFocus()
        self._calcReport = myReport()
        
        self.validation = 0
        self.Calc_result ={ 'P1':0, 'P2':0, 'P':0,'p':0, 'S':0, 'S1':0, 'S2':0, 'n':0, 'N':0, 'PMax':0, 'PMin':0 }
        self.Calc_input  ={ 'R':0,'No':0, 'h1':0, 'h2':0, 'h3':0, 'ρ1':0,'ρ2':0,'t':0, 'S0':0, 'P0':0 }
        
        self.meter2ft=3.28083989501 
        self.mpa2psi=145.037743897 
        
        self.dist  = [self.input_4, self.input_6,self.input_8,  self.input_10 ]
        self.pres = [self.input_3, self.input_9, self.input_11,  self.output_3, self.output_4]
        
        self.tabWidget.setCurrentIndex(0)
        self.Metric.setEnabled(False)
    
    def eventFilter(self, source, event):    
            if source in self._monitorList: 
                if event.type() == QtCore.QEvent.KeyPress: 
                    key = event.key()
                    if key == QtCore.Qt.Key_Return:
                        #self.input_1.setText("测试")
                        print("test is running")
                        self.__tabOrdercurr =self.__tabOrderList.index(source)           
                        if( self.__tabOrdercurr < self.__tabNumber - 1): 
                            self.__tabOrdercurr = self.__tabOrdercurr + 1
                        else:
                            self.__tabOrdercurr = 1
                            self.On_Calculation_Clicked()
                        self.__tabOrderList[self.__tabOrdercurr].setFocus()
                        
                        if source == self.input_4 or source == self.input_6: 
                            if source.text() != "":
                                self.input_12.setText(str(self.groudTemp + float( source.text())/100*4))
                        
            return QtWidgets.QMainWindow.eventFilter(self, source, event) 
         
    def tabchange(self):
        self.__tabOrdercurr = 1
        self.__tabOrderList.clear()
        
        self.output_2.setText(str(""))
        self.output_3.setText(str(""))
        self.output_4.setText(str(""))
            
        
        if self.tabWidget.currentIndex()==0:
            self.__tabNumber = 6
            self.Calc_input['R'] = 0
            self.input_11.setText(str(self.YB_ShearPin_Default ))
            self.__tabOrderList = [ self.input_1, self.input_2, self.input_3,self.input_4,self.input_11, self.input_12 ]
            self.YB_Shear_CalcInit( )
            #print('index:',self.tabWidget.currentIndex())
        else:
            self.__tabNumber = 9 
            self.Calc_input['R'] = 1
            self.input_11.setText(str(self.YCE_ShearPin_Default ))
            self.__tabOrderList = [ self.input_1, self.input_5, self.input_6,self.input_7,self.input_8, self.input_9,self.input_10,self.input_11, self.input_12 ]
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
        self.Calc_input['t']  = self.__calValueList[8]
         
        self.Calc_result['P1'] = self.Calc_input['h1'] * self.Calc_input['ρ1'] *0.0098
        self.Calc_result['P2'] = self.Calc_input['h3'] * self.Calc_input['ρ2'] *0.0098 + (self.Calc_input['h1'] -self.Calc_input['h2'] )* self.Calc_input['ρ1'] *0.0098
        self.Calc_result['P']  =  self.Calc_result['P1']  -  self.Calc_result['P2'] 
        #print(self.Calc_input)
         
    def Shear_Calc(self):                
        self.Calc_result['p'] = self.Calc_input['t']*(0.0016- self.Calc_input['t'] *0.000002)-0.0443
        self.Calc_result['S'] = self.Calc_input['S0'] *(1-self.Calc_result['p'])
        self.Calc_result['p'] = self.Calc_result['p']*100
        self.Calc_result['S1'] = self.Calc_result['S'] *(1+0.05)
        self.Calc_result['S2'] = self.Calc_result['S'] *(1-0.05)
        self.Calc_result['n'] = (self.Calc_result['P']+self.Calc_input['P0']) / self.Calc_result['S2'] 
        self.Calc_result['N'] = math.ceil(self.Calc_result['n'] )
        self.Calc_result['PMax'] = self.Calc_result['N'] *self.Calc_result['S1'] -self.Calc_result['P'] 
        self.Calc_result['PMin'] = self.Calc_result['N'] *self.Calc_result['S2'] -self.Calc_result['P']  
        
        #for key,  value in self.Calc_result.items():
        #    self.Calc_result[key] = round(value, 2)
        #print(self.Calc_result)
     
    def Update_Control_Value(self):
        self.validation = 1
        for i in range(1, self.__tabNumber):
            if self.__tabOrderList[i].text() != "":
                self.__calValueList[i] = float(self.__tabOrderList[i].text() )
            else: 
                self.validation = 0
                break 
        return self.validation
    
    def On_Calculation_Clicked(self):
        
        self.output_1.setText("")
        self.output_2.setText("")
        self.output_3.setText("")
        self.output_4.setText("")
            
        if self.Update_Control_Value() == 1:
            self.Calc_input['No']= self.__tabOrderList[0].text()  
            
            if self.Calc_input['R'] == 0:
                self.YB_Shear_CalcInit()
            else:
                self.YCE_Shear_CalcInit()
            
            self.Shear_Calc()
            
            self.output_2.setText(str(format(self.Calc_result['N'], '.2f')))
            self.output_1.setText(str(format(self.Calc_result['p'], '.2f')))
            self.output_3.setText(str(format(self.Calc_result['PMax'], '.2f')))
            self.output_4.setText(str(format(self.Calc_result['PMin'], '.2f')))

    def On_Report_Clicked(self):
        self.On_Calculation_Clicked( )
        self._calcReport.Param_Init(self.Calc_input, self.Calc_result)
        self._calcReport.GenerateWord()   
        self._calcReport.Save() 
        
    def On_Print_Clicked(self):
        self.On_Calculation_Clicked( )
        self._calcReport.Param_Init(self.Calc_input, self.Calc_result) 
        self._calcReport.Print()
        
    def UnitSwitch_2Metric(self):   
        for i in range(0, 4):
            if self.dist[i].text() != "":
                self.dist[i].setText(str(format(float(self.dist[i].text() )/self.meter2ft, '.2f')))
        for i in range(0, 5):       
            if self.pres[i].text() != "":
                self.pres[i].setText(str(format(float(self.pres[i].text() )/self.mpa2psi, '.2f')))
        
        self.label_18.setText("最大加压（MPa）：")
        self.label_19.setText("最小加压（MPa）：")
        
        self.label_22.setText("起爆装置垂深（m）")
        self.label_10.setText("   安全加压（MPa）   ")
        self.label_12.setText("单颗剪切值（MPa/颗）   ")
       
        self.label_24.setText("起爆装置垂深（m）")
        self.label_20.setText("测试阀垂深（m）")
        self.label_21.setText("油管内液体深度（m）")
        self.label_4.setText("        安全加压（MPa）")  
        
        self.Metric.setEnabled(False)
        self.Imperial.setEnabled(True)
        
    def UnitSwitch_2Imperial(self):   
        for i in range(0, 4):
            if self.dist[i].text() != "":
                self.dist[i].setText(str(format(float(self.dist[i].text() )*self.meter2ft,'.2f')))
        for i in range(0, 5):            
            if self.pres[i].text() != "":
                self.pres[i].setText(str(format(float(self.pres[i].text() )*self.mpa2psi, '.2f')))
        
        self.label_18.setText("最大加压（psi）：")
        self.label_19.setText("最小加压（psi）：")
        
        self.label_22.setText("起爆装置垂深（ft）")
        self.label_10.setText("   安全加压（psi）   ")
        self.label_12.setText("单颗剪切值（psi/颗）   ")
       
        self.label_24.setText("起爆装置垂深（ft）")
        self.label_20.setText("测试阀垂深（ft）")
        self.label_21.setText("油管内液体深度（ft）")
        self.label_4.setText("        安全加压（psi）")

        self.Metric.setEnabled(True)
        self.Imperial.setEnabled(False)
        
    def SetOpacity(self):    
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

class myAbout(QtWidgets.QDialog, Ui_AboutDialog):
    def __init__(self):
        QtWidgets.QDialog.__init__(self)
        self.child=Ui_AboutDialog()
        self.child.setupUi(self)
        icon = QIcon()
        icon.addPixmap(QPixmap(":/myImages/CASC.png"))
        self.setWindowIcon(icon)
    
if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    mainWindow  = QtWidgets.QMainWindow()
    #aboutWindow = QtWidgets.QDialog() 
    ui = myWindow(mainWindow)
    child=myAbout()
    ui.About.triggered.connect(child.show)
    
    #ui.setupUi(WindowObject)
    #app.installEventFilter(mainWindow)
    mainWindow.show()
    sys.exit(app.exec_())
