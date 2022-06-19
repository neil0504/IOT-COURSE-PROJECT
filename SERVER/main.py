from flask import Flask, request, json
from flask_restful import Resource, Api, abort, reqparse
from Database_Helper import DatabaseHelper

app = Flask(__name__)
api = Api(app)
database_helper = DatabaseHelper()


class GetData(Resource):

    def get(self):
        json_data = database_helper.get_data()
        return json_data


class SendData(Resource):

    def post(self):
        print(request.get_data())
        json_data = request.get_json()
        insert_status = database_helper.insert(json_data[database_helper.COLUMN_PATIENT_NAME],
                                               json_data[database_helper.COLUMN_ENTRY_TIME],
                                               json_data[database_helper.COLUMN_TEMP],
                                               json_data[database_helper.COLUMN_HEART_RATE],
                                               json_data[database_helper.COLUMN_SPO2])
        return insert_status


api.add_resource(GetData, "/getData")
api.add_resource(SendData, "/sendData")
if __name__ == '__main__':
    app.secret_key = "ItIsASecret"
    app.debug = True
    app.run()
