# -*- coding: utf-8 -*-
"""
Created on 2017.12.12 23:50

@author: HW

"""

"""
排序器类：
    三级联排序：
        第一层：初始筛选 按质量推算尽量减少候选页面
        第二层：依据当前页 选择 K-N 
        第三层：依据用户需求排序（质量、时间、热度……）
"""
class ranker:
    def __init__(self):
        self.nperpage = 20 #每页最多展示20
        self.curn = 1 #当前默认第一页
        self.judg = 'p' #默认以质量作为排序依据

    # 查询词预处理
    def qinit(self,qstr):
        pass

    # 传入倒排表 返回文档序列
    def qurry1(self,ivtb):
        res = {}
        # 累积文档评分
        for tb in ivtb:
            for i in range(0,len(tb[0])):
                if tb[0][i] in res:
                    res[tb[0][i]]+=tb[1][i]
                else:
                    res[tb[0][i]]=tb[1][i]

        lres=[]
        for doc_i in res:
            lres.append(doc_i)

        print(res)
        # 按分值排序
        lres.sort(key=lambda x:res[x],reverse=True)
        return lres

    # 返回第n页
    def qurry2(self,lst):
        l = len(lst)
        curmax = self.curn*self.nperpage
        curmin = (self.curn-1)*self.nperpage
        if l<curmin:
            return []
        elif l<curmax:
            return lst[curmin:]
        else:
            return lst[curmin:curmax]

    # 依据指标对当前页排序
    def qurry3(self,lst):
        return lst
