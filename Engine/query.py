# -*- coding: utf-8 -*-
"""
Created on Sat Nov 11 17:45:40 2017

@author: L
"""
import math
import numpy as np
import string
import json
import jieba
import pypinyin as pinyin
from pypinyin import (NORMAL)

def loadfile():#读入索引，关联信息和文件编号对应
    findex=open("index.json","r")
    frelevant=open("relevant.json","r")
    fname=open("name_num.json","r") 
    decindex=json.load(findex)
    decrelevant=json.load(frelevant)
    decname=json.load(fname)
    lstindex=[]  
    lstrelevant=[]
    lstname=[]
    firstLetterList=[0]*26
    lastAscii=97
    indexnum=0
    k=0
    for x in decindex:
        lstindex.append([x["pinyin"],x["term"],x["df"],x["index"]])
        if ord(x["pinyin"][0])  in range(97,123):
            if ord(x["pinyin"][0])==97 and k==0:
                firstLetterList[0]=indexnum
                lastAscii=97
                lastpos=firstLetterList[0]
                k=1
            elif ord(x["pinyin"][0])!=lastAscii:
                d=ord(x["pinyin"][0])-97              
                firstLetterList[d]=indexnum
                if (ord(x["pinyin"][0])-lastAscii)!=1:
                    for i in range(lastAscii-96,ord(x["pinyin"][0])-97):
                        firstLetterList[i]=lastpos
                lastAscii=ord(x["pinyin"][0])
                lastpos=firstLetterList[d]
        indexnum=1+indexnum     
    firstLetterList.append(indexnum-1)
    for y in decrelevant:
        lstrelevant.append([y["Doc"],y["relevant"]])
    for z in decname:
        lstname.append([z["Docname"],z["Num"]])
    findex.close()
    frelevant.close()
    fname.close()
    return  lstindex,lstrelevant,lstname,firstLetterList

def inputprocess(query):#查询内容词条还
    punct='，,。,！,？,：,；,“,”,‘,’,【,】,{,},、,|,/,~,·,（,）,■, ,......,±,»,Ô,×,—,…,《,》,﹐,﹔,﹕,﹖,＆,－,／,～,￥，ñ,ö,÷,–,•,‰,Ⅱ,Ⅲ,Ⅴ,↑,→,↓,⊙,①,②,③,④,⑤,⑥,⑦,⑧,⑨,⑩,⒈,⒉,⒊,⒋,⒌,─,┃,╱,▌,□,▪,○,★,〈,〉,〕,す,金,﹒,＂,％,＋,．,＝,Ａ,￭'
    result = jieba.tokenize(query)#,mode='default')
    lst=[]
    for i in result:
        if i[0] not in (string.punctuation and punct):
            if len(lst)==0:
                lst.append([i[0],1])
            else:
                for j in range(len(lst)):
                    if i[0]==lst[j][0]:
                        lst[j][1]=1+lst[j][1]
                        break
                    else:
                        lst.append([i[0],1])
                        break
    return lst

def searchDoc(request,base,letterList):#返回所有包含查询里词条的文档
    allDoc=[]
    for terminf in request:
        term=terminf[0]
        termDoc=[]
        tf=[]
        termpy=pinyin.pinyin(term,style=NORMAL)
        df=[]
        if ord(termpy[0][0][0]) not in range(97,123):
            for i in range(letterList[0]):
                if base[i][1]==term:
                    df=base[i][2]
                    for record in base[i][3]:
                        if record[0] in termDoc:
                            tf[termDoc.index(record[0])]=1+int(tf[termDoc.index(record[0])])
                        else:
                            termDoc.append(record[0])
                            tf.append(1)         
                            
        else:
            order=ord(termpy[0][0][0])-97
            for j in range(letterList[order],letterList[order+1]):
                if base[j][1]==term:
                    df=base[j][2]
                    for record in base[j][3]:
                        if record[0] in termDoc:
                            tf[termDoc.index(record[0])]=1+int(tf[termDoc.index(record[0])])
                        else:
                            termDoc.append(record[0])
                            tf.append(1)         
        allDoc.append([term,df,termDoc,tf])#返回词项，出现该词项的文档数，包含该词项的文档列表，以及该词项的词频
    return allDoc

def cal_di(selectInf):#计算文档权值
    k=len(selectInf)
    existed=[]
    termFre=[0]*k
    df=[]
    tfList=[]
    for i in range(len(selectInf)):
        termDoc=selectInf[i]
        df.append([termDoc[0],len(termDoc[2])])  
        for j in range(len(termDoc[2])):
            doc=termDoc[2][j]
            if doc not in existed:
                existed.append(doc)
                termFre[i]=termDoc[3][j]
                for n in range(i+1,k):
                    if doc in selectInf[n][2]:
                        termFre[n]=selectInf[n][3][selectInf[n][2].index(doc)]
            tfList.append([doc,termFre])
            termFre=[0]*k
    N=len(tfList)
    idf=[]
    for i in df:
        if i[1]==0 or N==0:
            calcuate=0
        else:
            calcuate=math.log(N/i[1])
        idf.append(calcuate)
    Ditemp=[0]*N
    Di=[]
    terms=[]
    for i in range(N):
        tf=tfList[i][1]
        Ditemp[i]=np.multiply(tf,idf)
        terms.append(tfList[i][0])
    Di=[terms,Ditemp]
    return Di,idf


def  cal_qi(quedf,quetf):#计算查询权重
    tf=[]
    terms=[]
    for i in quetf:
        tf.append(i[1])
        terms.append(i[0])
    Qitemp=np.multiply(tf,quedf)
    Qi=[terms,Qitemp]
    return Qi
    
def rank(docgrade,quegrade):#对选中文档按照得分进行排序
    gradetemp=[]
    for dg in docgrade[1]:
        temp=np.multiply(quegrade[1],dg)
        gradetemp.append(sum(temp))
    rankList=[]
    for i in range(len(gradetemp)):
        rankList.append([gradetemp[i],docgrade[0][i]])
    rankList.sort(reverse =True)
    orderedList=[]
    for i in rankList:
        orderedList.append(i[1])
    return orderedList


def loadpushinfo(paths):#载入推送文档
    title=[]
    url=[]
    for path in paths:
        fo=open(r'%s' %path,'r',encoding='utf-8')
        urlread=0
        titleread=0
        while 1:
            line=fo.readline()
            print('loading...')
            if line.startswith('url:'):
                url.append(line[4:-1])
                urlread=1
            elif line.startswith('head:'):
                title.append(line[5:-1])
                titleread=1
            if urlread==1 and titleread==1:
                break
    return title,url


def user():#main函数
    index,reletionship,filename,firstletter=loadfile()
    inputque=input("查啥自己输进去：")
    print('\n')
    inputterms=inputprocess(inputque)
    Docfound=searchDoc(inputterms,index,firstletter)
    di,idf=cal_di(Docfound)
    qi=cal_qi(idf,inputterms)
    s=rank(di,qi)
    topK=10
    selected=[]
    k=min(len(s),topK)
    for i in range(k):
        selected.append(s[i])
    pushpaths=[0]*k
    for file in filename:
        if file[1] in selected:
            pushpaths[selected.index(file[1])]=file[0] 
    titles,urls=loadpushinfo(pushpaths)
    for i in range(len(titles)):
        print(titles[i])
        print(urls[i])
        print('\n')
    return 1



def qry_interface(qstr):
    index,reletionship,filename,firstletter=loadfile()
    inputque=qstr
    inputterms=inputprocess(inputque)
    Docfound=searchDoc(inputterms,index,firstletter)
    di,idf=cal_di(Docfound)
    qi=cal_qi(idf,inputterms)
    s=rank(di,qi)
    topK=10
    selected=[]
    ans = []

    k=min(len(s),topK)
    for i in range(k):
        selected.append(s[i])
    pushpaths=[0]*k
    for file in filename:
        if file[1] in selected:
            pushpaths[selected.index(file[1])]=file[0]
    titles,urls=loadpushinfo(pushpaths)
    for i in range(len(titles)):
        temp = {}
        temp['title'] = titles[i]
        temp['url'] = urls[i]
        ans.append(temp)

    return ans

