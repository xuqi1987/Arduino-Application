# -*- coding:utf-8 -*-
from flask import Flask, request,jsonify
import json

app = Flask(__name__)

@app.route('/',methods=['POST','GET'])
def sendjson2():
# 接收前端发来的数据,转化为Json格式,我个人理解就是Python里面的字典格式
	#data = json.loads(request.get_data())
	# 然后在本地对数据进行处理,再返回给前端
	# name = data["name"]
	# age = data["age"]
	# location = data["location"]
	# data["time"] = "2016"

	# Output: {u'age': 23, u'name': u'Peng Shuang', u'location': u'China'}
	if request.methods == "POST":
		print request.get_data()
	return "ok"

if __name__ == '__main__':
    app.run(host="0.0.0.0",debug=True)