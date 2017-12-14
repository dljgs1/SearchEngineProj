#encoding=utf-8
import json
import string
import glob
import numpy as np
import math
import jieba
import jieba.analyse
import pypinyin as pinyin
from pypinyin import (NORMAL)


def textload(fpath):
    path=fpath
    text=open(path,'r',encoding="utf-8")
    k=0
    fout=[]
    line=[]


    #原始文档：第一行url  第二行title 第三行 content
    while 1:
        row=text.readline()
        if row.startswith('content:'):
            k=1
            line=row[8:]   #type(line)==str
            fout.extend(line)
        elif len(row)>0 and k==1 and row!='\n':
            line=row
            fout.extend(line)
        if len(row)==0 and k==1:
            break
    text.close()
    fout=''.join(fout)
    return fout


def isnumber(x):#提出包括整数 小数 百分数在内的各种数字
    xiaoshu=string.digits+'.'+'%'
    for i in range(len(x)):
        if x[i] in xiaoshu:
            continue
        else:
            return False
    return True

    
def symboljudge(word):#判决是否为符号
    punct='，,。,！,？,：,；,“,”,‘,’,【,】,{,},、,|,/,~,·,（,）,■, ,......,±,»,Ô,×,—,…,《,》,﹐,﹔,﹕,﹖,＆,－,／,～,￥，ñ,ö,÷,–,•,‰,Ⅱ,Ⅲ,Ⅴ,↑,→,↓,⊙,①,②,③,④,⑤,⑥,⑦,⑧,⑨,⑩,⒈,⒉,⒊,⒋,⒌,─,┃,╱,▌,□,▪,○,★,〈,〉,〕,す,金,﹒,＂,％,＋,．,＝,Ａ,￭'
    if (isnumber(word)) or (word in string.punctuation) or (word in punct) or (word==' 'or word==r'\u3000')or(word in string.printable):
        return False
    else:
        return True

def sort(termList,indexList):#按照拼音顺序进行排序 大写字母在前 小写字母和汉字拼音灾后
    termpinyin=[]
    terms=termList[0]
    for term in terms:
        pyall=''
        termpy=pinyin.pinyin(term,style=NORMAL)
        for i in termpy:
            for j in i:
                pyall+=j           
        termpinyin.append(pyall)
    termList.append(termpinyin)
    sortList=[]
    for term in zip(termList[0],termList[1],termList[2],indexList):
        sortList.append(term)
    sortList.sort(key=lambda x:x[2])
    return sortList

def getindex(content,num,termList,indexList):#建立索引
    g_mode='default'
    token=[]
    result = jieba.tokenize(content,mode=g_mode)#词条化函数
    print('tokenized')
    for tk in result:
        token.append([tk[0],tk[1],tk[2]])
    for tk in token:
        if symboljudge(tk[0]) and (tk[0] not in ['\n','\u3000','\xa0']):#剔除空格
            if tk[0] in termList[0]:
                indexList[termList[0].index(tk[0])].append([num,tk[1],tk[2]])
                termList[1][termList[0].index(tk[0])]=1+int(termList[1][termList[0].index(tk[0])])
            else:
                termList[0].append(tk[0])
                termList[1].append('1')
                indexList.append([[num,tk[1],tk[2]]])
    return termList,indexList


def getFeaturesWeight(content):#获取文档关键词和对应的权重
    withWeight = True
    tags_weight = jieba.analyse.extract_tags(content,withWeight=withWeight)
    print('get')
    temptags=[]
    tempweight=[]
    for (tag,weight) in tags_weight:   
        temptags.append(tag)
        tempweight.append(weight)
    return temptags,tempweight

def DF_Vocab(tagsList):#获取关键词的词汇表和每个词的文档频率
    myVocab=[]
    TF=[]
    tempTF=[1]
    k=0
    for tags in tagsList:
        for term in tags:
            if term not in myVocab:
                myVocab.append(term)
                if k==0:
                    k=1
                else:
                    tempTF=np.column_stack((tempTF,1))
                    TF=tempTF[0]
            else:
                pos=myVocab.index(term)
                TF[pos]=1+TF[pos]
    return myVocab,TF


def tf_idf(tagsList,weightList,DF,VocabularyList):#计算tf_idf值
    tf_idfArr=[]
    tf_idfLine=[]
    for (tags,weight) in zip(tagsList,weightList):
        for term in tags:
            df=DF[VocabularyList.index(term)]
            tf=weight[tags.index(term)]
            idf=math.log((float(len(tagsList)))/(float(df)))
            tf_idf=(float(tf))*idf
            tf_idfLine.append(tf_idf)
        tf_idfArr.append(tf_idfLine)
        tf_idfLine=[]
    return tf_idfArr



def rank(features,weights,idf):#对相关文档按照相关度进行排序
    textnum=[]
    for i in range(len(features)):
        textnum.append(i)
    path=r'relation_degree.txt'
    score=[]
    for i in range(len(features)):  #取文档
        text=i
        tags=features[i]
        scoretemp=[0]*len(features)
        for j in range(len(tags)):  #文档取词
            term=tags[j]
            for k in range(len(features)):   #开始比较
                if k==i:                #比较到自己时得分为负
                    scoretemp[k]=-1;
                else:
                    compared=features[k] #与其他行比较时
                    comparedidf=idf[k]
                    for word in compared:
                        if word==term:
                            scoretemp[k]=scoretemp[k]+float(comparedidf[compared.index(word)])
        k=0
        picked=[0]*len(scoretemp)
        num=[0]*len(scoretemp)
        for i in range(len(scoretemp)):
            if not (scoretemp[i]==0 or scoretemp[i]==-1):
                picked[k]=scoretemp[i]
                num[k]=i
                k=k+1   
        pickedNoZero=picked[0:k]
        numNoZero=num[0:k]
        RD=open(path,'a')
        picked=[0]*len(features)
        filelist=[]
        if (len(pickedNoZero))==0:
            filelist='No related text'
            RD.write('No related text\n')
        else:  #插入排序法
            for i in range(1,k):
                for j in range(0,i):
                    if pickedNoZero[i]>pickedNoZero[j]:
                        temp=pickedNoZero[j]
                        temp1=numNoZero[j]
                        pickedNoZero[j]=pickedNoZero[i]
                        numNoZero[j]=numNoZero[i]
                        pickedNoZero[i]=temp
                        numNoZero[i]=temp1
            for i in range(len(numNoZero)):
                filelist.append(textnum[numNoZero[i]])
                RD.write('   '+str(textnum[numNoZero[i]]))
        RD.write('\n')
        score.append(filelist)
        RD.close()
    relationArr=[]
    for (text,filelist) in zip(textnum,score):
        relationArr.append((text,filelist))
    return relationArr


def getRelevantDoc(tagsList,weightList):#获取相关文档
    VoList,tf=DF_Vocab(tagsList)
    TF_IDF=tf_idf(tagsList,weightList,tf,VoList)
    List=rank(tagsList,weightList,TF_IDF)
    return List

def indexfilewrite(table):#索引写文件
    lst=[]
    for x in table:
        termline = {"pinyin":x[2],"term":x[0],"df":x[1],"index":x[3]}
        lst.append(termline)
    file = open("index.json","w")
    file.write(json.dumps(lst,ensure_ascii=False))
    file.close()
    
def rele_Docwrite(table):#文档相关信息写文档
    lst=[]
    for data in table:
        dict1 = {"Doc": data[0], "relevant": data[1]}
        lst.append(dict1)
    f = open("relevant.json", "w")
    f.write(json.dumps(lst))
    f.close()

def name_Numwrite(table):#文件名和处理时文档编号对应情况写文件
    lst=[]
    for x in table:
        termline = {"Docname":x[0],"Num":x[1]}
        lst.append(termline)
    file = open("name_num.json","w",encoding="utf-8")
    file.write(json.dumps(lst))
    file.close()
    
def entrance(path):#主函数
    name_Num=[]
    termTable=[[],[]]
    indexTable=[]
    featureArr=[]
    weightArr=[]
    globnum=0
    for f in glob.glob('%s\*.txt' %path):
        print(f)
        name_Num.append([f,globnum])
        text=textload(f)
        termTable,indexTable=getindex(str(text),globnum,termTable,indexTable)
        print('以加入索引…')
        tagsout,weightout=getFeaturesWeight(text)
        featureArr.append(tagsout)
        weightArr.append(weightout)
        print('已获取特征…')
        print('…')
        globnum=globnum+1
    rele_Doc=getRelevantDoc(featureArr,weightArr)
    finalTable=sort(termTable,indexTable)
    indexfilewrite(finalTable)
    print('索引写入完成…')
    rele_Docwrite(rele_Doc)
    print('文档相关信息写入完成…')
    name_Numwrite(name_Num)
    print('Done!')
    return  finalTable,rele_Doc,name_Num


a,b,c=entrance(r'news')

