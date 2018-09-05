# -*- coding: utf-8 -*-
from peewee import *

db = SqliteDatabase('IR.db')

class IRInfo(Model):
    id = IntegerField(primary_key=True)
    name = CharField()
    data = TextField(default=u'')

    class Meta:
        database = db



db.connect()
db.create_tables([IRInfo])
IRInfo.insert(name=u"空调",data="{'Encoding':'Gree'}").execute()