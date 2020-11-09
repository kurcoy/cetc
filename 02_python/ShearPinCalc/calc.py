import docx
from docx.enum.text import WD_PARAGRAPH_ALIGNMENT
from docx.shared import Pt
from docx.oxml.ns import qn 
from docx.shared import RGBColor
from docx.shared import Inches
from docx.enum.text import WD_LINE_SPACING

import copy
import time
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtPrintSupport import  QPrinter, QPrintDialog, QPrintPreviewDialog
from PyQt5.QtGui import QFont,QTextDocument,QTextCursor
import tempfile
import math
import win32print
import win32api

class myReport():
    def __init__(self):
        self.file=docx.Document()
        self.number = 0

        self.Param_result ={ 'P1':0, 'P2':0, 'P':0,'p':0, 'S':0, 'S1':0, 'S2':0, 'n':0, 'PMax':0, 'PMin':0 }
        self.Param_input ={ 'h1':0, 'h2':0, 'h3':0, 'ρ1':0,'ρ2':0,'t':0, 'S0':0, 'P0':0 }
        
    def Generate(self):
        #写入若干段落ρ
        self.file.styles['Normal'].font.name=u'宋体'
        self.file.styles['Normal'].font.size = Pt(12)
        self.file.styles['Normal']._element.rPr.rFonts.set(qn('w:eastAsia'), u'宋体')
        paragraph_format = self.file.styles['Normal'].paragraph_format
        paragraph_format.line_spacing_rule = WD_LINE_SPACING.SINGLE
        paragraph_format.space_after = Pt(0)
        paragraph_format.space_before  = Pt(0)
        
        #p = self.file.add_heading(u"压力起爆装置-剪切销数量设计报告", 0)
        Head = self.file.add_heading("",level=0)
        Head.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run  = Head.add_run(u"压力起爆装置-剪切销数量设计报告")
        run.font.name=u'宋体'
        run.font.color.rgb = RGBColor(250,0,0)
        run.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run._element.rPr.rFonts.set(qn('w:eastAsia'), u'宋体')
              
        p = self.file.add_paragraph(u"起爆装置型号：")
        p.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run = p.add_run(u"YB压力起爆装置    ")
        run = p.add_run(u"井号：")
        run = p.add_run(u"119")

        p = self.file.add_paragraph(u"日期：")
        p.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run = p.add_run(str(time.strftime('%Y{y}%m{m}%d{d}').format(y='年', m='月', d='日')))

        p = self.file.add_paragraph(u" ") 
        
        p = self.file.add_paragraph(u"1、确定压力起爆装置的垂直深度 ")#  style='List Number')
        run = p.add_run(str(self.Param_input['h1']))

        run = p.add_run(u"（m）")
        p = self.file.add_paragraph(u"活塞受到的压力为 P = ") 
        p.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run = p.add_run(str(self.Param_result['P']))
        run = p.add_run(u"（MPa）")
        p = self.file.add_paragraph(u"ρ -- ") 
        p.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run = p.add_run(str(self.Param_input['ρ1']))
        run = p.add_run(u"（g/cm3）	g -- 0.0098")
        
        p = self.file.add_paragraph(u"2、确定压力起爆装置所处位置温度 t = ") 
        run = p.add_run(str(self.Param_input['t']))
        run = p.add_run(u"（℃）")
    
        p = self.file.add_paragraph(u"3、根据温度t查曲线，得剪切销值随温度降低百分率 δ = ") 
        run = p.add_run(str(self.Param_result['p']))
        run = p.add_run(u"%")  
        
        self.file.add_picture('TempChart.png', width=Inches(5))
        
        p = self.file.add_paragraph(u"4、查剪切销合格证，得到剪切销在常温下的剪切值 S0 = ") 
        run = p.add_run(str(self.Param_input['S0']))
        run = p.add_run(u"（MPa）")  

        p = self.file.add_paragraph(u"5、计算剪切销在井下的实际剪切值 S = S0×（1-δ） = ")#  style='List Number')
        run = p.add_run(str(self.Param_result['S']))
        run = p.add_run(u"（MPa）")
        p = self.file.add_paragraph(u"剪切销的最大剪切值 S1 = ") 
        p.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run = p.add_run(str(self.Param_result['S1']))
        run = p.add_run(u"（MPa）")
        p = self.file.add_paragraph(u"剪切销的最小剪切值 S2 = ") 
        p.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run = p.add_run(str(self.Param_result['S2']))
        run = p.add_run(u"（MPa）")

        p = self.file.add_paragraph(u"6、剪切销的数量，安全加压 P0 = ")#  style='List Number')
        run = p.add_run(str(self.Param_input['P0']))
        run = p.add_run(u"（MPa）")
        p = self.file.add_paragraph(u"n = (P+P0)/S2 = ") 
        p.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run = p.add_run(str(self.Param_result['n']))
        p = self.file.add_paragraph(u"将n向上取整得N = ") 
        p.alignment = WD_PARAGRAPH_ALIGNMENT.CENTER
        run = p.add_run(str(self.Param_result['n']))
        run = p.add_run(u"，N为装配剪切销的数量")
 
        p = self.file.add_paragraph(u"7、井口加压最大值 Pmax = (N×S1)-P = ") 
        run = p.add_run(str(self.Param_result['PMax']))
        run = p.add_run(u"（MPa）")  
    
        p = self.file.add_paragraph(u"8、井口加压最小值 Pmin = (N×S2)-P = ") 
        run = p.add_run(str(self.Param_result['PMin']))
        run = p.add_run(u"（MPa）")   

        p = self.file.add_paragraph(u" ") 
        p = self.file.add_paragraph(u" ") 
        p = self.file.add_paragraph(u"工程师签名：                       甲方负责人：") 
        p = self.file.add_paragraph(u"年    月    日                    年    月    日") 
        p.alignment = WD_PARAGRAPH_ALIGNMENT.RIGHT

    def Save(self):    
        #保存
        curTime = time.strftime("%Y-%m-%d %H%M%S", time.localtime())
        self.file.save("C:\\Users\\Cliff\\Desktop\\word_" +curTime+".docx")
       
    def printPreview(self, printer):
        #self.file.print(printer)
        self.file.document().print_(printer)
    def Print(self):
        print ("print Preview")
        printer = QPrinter(QPrinter.HighResolution)
        preview = QPrintPreviewDialog(printer)
        preview.paintRequested.connect(self.handlePaintRequest)
        preview.exec_()    
    
    def handlePaintRequest(self, printer):
        document = QTextDocument()
        cursor = QTextCursor(document)
        cursor.insertText("打印预览")
        fp = tempfile.TemporaryFile()
        fp.write('压力起爆装置-剪切销数量设计报告\n，'.encode('utf-8'))
        fp.write('起爆装置型号：YB压力起爆装置    井号\n。'.encode('utf-8'))
        fp.seek(0)
        cursor.insertText(fp.read().decode('utf-8')) 
        document.print(printer)
        
    def Temp(self):
        fp = tempfile.TemporaryFile()
        fp.write('压力起爆装置-剪切销数量设计报告\n，'.encode('utf-8'))
        fp.write('起爆装置型号：YB压力起爆装置    井号\n。'.encode('utf-8'))
        fp.seek(0)
        print(fp.read().decode('utf-8')) 
        
    def Param_Init(self, Calc_input={}, Calc_result={} ): 
         self.Param_input = copy.deepcopy(Calc_input)
         self.Param_result = copy.deepcopy(Calc_result)
         print(self.Param_input )
