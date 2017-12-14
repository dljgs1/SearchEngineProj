import Engine.cdoc as cdoc
import Engine.cindex as idx
import Engine.crank as rk

indx = idx.idx()
rank = rk.ranker()

def init(fname):
    global indx
    global rank
    if not indx.load('data'):
        indx.build(fname,100000)


def entry(fname,str):
    global indx
    global rank

    print('start')
    ivtbs = indx.gettb(str)
    lst = rank.qurry1(ivtbs)

    ans = []
    for i in lst:
        d = cdoc.doc()
        d.textload(fname+r'\%s.txt' % i)
        ans.append({'title':d.title,'url':d.url})
        d.print()
    return ans

if __name__ == "__main__":
    init('new_2')
    entry('new_2')