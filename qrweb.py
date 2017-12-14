from flask import Flask
from flask import request,render_template
from Engine import query
from flask import jsonify

import main

app = Flask(__name__, static_url_path='')



def readfile(file):
    text = open(file, 'r', encoding="utf-8")
    retstr = ""
    temp = text.readline()
    while temp:
        retstr += temp
        temp = text.readline()
    return retstr


@app.route("/",methods=['GET','POST'])
def home():
    if request.method == "POST":
        ct = request.form.get('content', '')
        return jsonify("content:"+ct);
    else:
        return app.send_static_file('index.html')
    #return render_template('index.html')


@app.route("/catch",methods=['GET','POST'])
def catch():
    if request.method == 'GET':
        qstr = request.args.get('Token','')
        retstr = main.entry(fname,qstr)#query.qry_interface(qstr)

        return render_template('resulttp.html',list=retstr)
    else:
        return 'post'

@app.route("/test_get",methods=['GET','POST'])
def test_get():
    head = request.args.get('head')
    content = request.args.get('content')
    return jsonify({'result':'content：'+content})


if __name__ == "__main__":
    fname = 'new_2'
    main.init(fname)
    app.run(debug=True)

