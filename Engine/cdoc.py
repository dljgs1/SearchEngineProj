# -*- coding: utf-8 -*-
"""
Created on 2017.12.12 22:30

@author: HW

"""

#文档类：包含一篇新闻的全部信息:

class doc:
    def __init__(self):
        self.id = 0
        self.url = ""
        self.title = ""
        self.content = ""

    def print(self):
        print(self.url)
        print(self.title)
        print(self.content)

    def textload(self,fpath):
        path = fpath
        self.id = int(fpath.split('\\')[-1].split('.')[-2])
        text = open(path, 'r', encoding="utf-8")
        # 原始文档：第一行url  第二行title 第三行 content
        row = text.readline()
        if row[:4]!='http':
            return False
        self.url = row
        row = text.readline()
        if len(row)<=1:
            return False
        self.title = row

        row = text.readline()
        if len(row)<=1:
            return False
        self.content = row
        text.close()
        return True
