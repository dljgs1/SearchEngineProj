# -*- coding: utf-8 -*-
"""
Created on 2017.12.12 22:44

@author: HW
"""

"""
倒排索引类：
    结构：
        双层索引模式：
        第一层：高频单字 高频单词 关键词
        第二层：完整索引
    数据：
        词项
        字项
        
    功能：
        建立全文索引
        为查询字段返回初始候选（未筛）
        
"""

import glob
import math
import json
from Engine import cdoc

import jieba
import jieba.analyse


class idx:
    def __init__(self):
        self.dicts = {} #词项-[[文档号],[tf-idf]]
        self.docpath = [] #文档所在路径??考虑移除(文档内容不常驻内存,如果慢了改为常驻)
        self.qtb = {} #关键词-[文档号]
        self.kgram = {} #字-[[位于开头的词项],[位于中间的词项],[位于末尾的词项]]
    # 读取
    def load(self,fpath):
        try:
            findex=open(fpath+"\index.json","r",encoding='utf-8')
            pathidx=open(fpath+"\docpath.json","r",encoding='utf-8')
            qtbidx=open(fpath+"\qtb.json","r",encoding='utf-8')
            kgram=open(fpath+"\kgram.json","r",encoding='utf-8')

            self.dicts = json.load(findex)
            self.docpath = json.load(pathidx)
            self.qtb = json.load(qtbidx)
            self.kgram=json.load(kgram)

            findex.close()
            pathidx.close()
            qtbidx.close()
            kgram.close()
            return True
        except FileNotFoundError:
            return False

    # 存储
    def save(self,fpath):
        file = open(fpath+"\index.json","w",encoding='utf-8')
        file.write(json.dumps(self.dicts,ensure_ascii=False))
        file.close()
        file = open(fpath+"\docpath.json","w",encoding='utf-8')
        file.write(json.dumps(self.docpath,ensure_ascii=False))
        file.close()
        file = open(fpath+"\qtb.json","w",encoding='utf-8')
        file.write(json.dumps(self.qtb,ensure_ascii=False))
        file.close()
        file = open(fpath+"\kgram.json","w",encoding='utf-8')
        file.write(json.dumps(self.kgram,ensure_ascii=False))
        file.close()

    # 建立全文索引
    def build(self,fpath,maxnum):#预设最大文档数
        self.docpath = [""]*maxnum
        # 添加文件
        for f in glob.glob('%s\*.txt' % fpath):
            d = cdoc.doc()
            d.textload(f)
            if d.id >= maxnum:
                continue
            self.docpath[d.id] = f
            self.addindex(d.title+d.content,d.id)
            self.add_qtb(d.title,d.id)
            print(d.id)

        print('计算tf-idf..')
        for wd in self.dicts:
            tf = self.dicts[wd][1]
            df = len(tf)
            self.dicts[wd][1] = list(map(lambda x:math.log(float(maxnum)/x),tf))
        print('计算kgram..')
        self.build_kgram()
        print('存储索引..')
        self.save('data')

    # 获取倒排表
    def gettb(self,qstr):
        res = []
        qtb = []
        qlist = qstr.split('*')
        if len(qlist)>1:
            for i in range(0,len(qlist)):
                if len(qlist[i])<1:
                    continue
                if qlist[i] in self.dicts:
                    res.append(self.dicts[qlist[i]])

                if i==0 and qlist[i][-1] in self.kgram:
                    for term in self.kgram[qlist[i][-1]][0]:
                        res.append(self.dicts[term])
                if i==len(qlist)-1 and qlist[i][0] in self.kgram:
                    for term in self.kgram[qlist[i][0]][2]:
                        res.append(self.dicts[term])

                if 0 < i < len(qlist)-1 and qlist[i][0] in self.kgram and qlist[i][-1] in self.kgram:
                    for term in self.kgram[qlist[i][-1]][1]:
                        res.append(self.dicts[term])
                    for term in self.kgram[qlist[i][0]][1]:
                        res.append(self.dicts[term])
        else:
            for wd in self.subindex(qstr,True):
                res.append(self.dicts[wd])

        return res


    # cut - gram
    def cut_gram(self,qlist):
        pass


    # k-gram : 建立对全词项的kgram索引 实现通配符查询

    def build_kgram(self):
        self.kgram = {}
        for term in self.dicts:
            if len(term)==1:
                continue
            for i in range(0,len(term)):
                swd = term[i]
                if swd not in self.kgram:
                    self.kgram[swd] = [[],[],[]]
                if i == 0:
                    self.kgram[swd][0].append(term)
                elif i==len(term)-1:
                    self.kgram[swd][2].append(term)
                else:
                    self.kgram[swd][1].append(term)






#######################################private:

    # 添加第二索引
    def addindex(self, content, docid):
        titf = self.subindex(content)
        self.add_tf(titf, docid)

    # 添加第一索引
    def add_qtb(self,content,docid):
        for wd in self.subindex(content,e=True):
            if wd in self.qtb:
                self.qtb[wd].append(docid)
            else:
                self.qtb[wd] = [docid]

    # 词项频率
    def add_tf(self,titf,docid):
        for wd in titf:
            if wd in self.dicts:
                self.dicts[wd][0].append(docid)
                self.dicts[wd][1].append(titf[wd])  # tft,d
            else:
                self.dicts[wd] = []
                self.dicts[wd].append([docid])
                self.dicts[wd].append([titf[wd]])  # tft,d

    # 得到 词项-词项信息 e代表是否使用全划分
    def subindex(self,str,e = False):
        ct = {}
        tk = jieba.cut(str,cut_all=e)
        for it in tk:
            if it in ct:
                ct[it] = ct[it]+1
            else:
                ct[it] = 1
        return ct

    # 关键词索引
    def weiindex(self,str):
        tags_weight = jieba.analyse.extract_tags(str,withWeight=False)
        return tags_weight
