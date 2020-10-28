import docx

def ReportGenerate():
    file=docx.Document()

    #写入若干段落
    file.add_paragraph("压力起爆装置-剪切销数量设计报告")
    file.add_paragraph("起爆装置型号：YB压力起爆装置    井号：")
    file.add_paragraph("日期：2020年09月15日")
    file.add_paragraph("1、确定压力起爆装置的垂直深度 20 （m）")
    file.add_paragraph("活塞受到的压力为 P = 0.20 （MPa）")

    #保存
    file.save("C:\\Users\\Cliff\\Desktop\\word.docx")
