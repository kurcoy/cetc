from win32com.client import constants, gencache
import os

#创建PDF
def createPdf(wordPath, pdfPath):
    """
    word转pdf
    :param wordPath: word文件路径
    :param pdfPath:  生成pdf文件路径
    """
    word = gencache.EnsureDispatch('Word.Application')
    doc = word.Documents.Open(wordPath, ReadOnly=1)
    doc.ExportAsFixedFormat(pdfPath,
                            constants.wdExportFormatPDF,
                            Item=constants.wdExportDocumentContent,
                            CreateBookmarks=constants.wdExportCreateHeadingBookmarks)
    word.Quit(constants.wdDoNotSaveChanges)

#遍历当前目录，并把Word文件转换为PDF
def wordToPdf():
    print("转换中...")
    # 获取当前运行路径
    path = os.getcwd()
    # 获取所有文件名的列表
    filename_list = os.listdir(path)
    # 获取所有word文件名列表
    wordname_list = [filename for filename in filename_list \
                        if filename.endswith((".doc", ".docx"))]
    for wordname in wordname_list:
        # 分离word文件名称和后缀，转化为pdf名称
        pdfname = os.path.splitext(wordname)[0] + '.pdf'
        # 如果当前word文件对应的pdf文件存在，则不转化
        if pdfname in filename_list:
            continue
        # 拼接 路径和文件名
        wordpath = os.path.join(path, wordname)
        pdfpath = os.path.join(path, pdfname)
        createPdf(wordpath,pdfpath)

#word转pdf
if __name__ == '__main__':
    wordToPdf()
