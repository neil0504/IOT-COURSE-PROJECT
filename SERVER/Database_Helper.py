import sqlite3


class DatabaseHelper:

    def __init__(self):
        self.TABLE_NAME = "Data"
        self.COLUMN_ID = "id"
        self.COLUMN_PATIENT_NAME = "patient_name"
        self.COLUMN_ENTRY_TIME = "entry_time"
        self.COLUMN_TEMP = "temperature"
        self.COLUMN_HEART_RATE = "heart_rate"
        self.COLUMN_SPO2 = "oxygen_level"

        self.conn = None
        self.cursor = None
        self.create()

    def create(self):
        database_name = f"{self.TABLE_NAME}.sqlite"
        self.conn = sqlite3.connect(database_name)
        self.cursor = self.conn.cursor()
        query = f"CREATE TABLE IF NOT EXISTS {self.TABLE_NAME}" \
                f"({self.COLUMN_ID} INTEGER NOT NULL PRIMARY KEY, " \
                f"{self.COLUMN_PATIENT_NAME} TEXT, " \
                f"{self.COLUMN_ENTRY_TIME} TEXT, " \
                f"{self.COLUMN_TEMP} TEXT, " \
                f"{self.COLUMN_HEART_RATE} TEXT, " \
                f"{self.COLUMN_SPO2} TEXT)"
        self.cursor.execute(query)

    def get_data(self):
        try:
            query = f"SELECT * FROM {self.TABLE_NAME}"
            data = self.cursor.execute(query).fetchall()
            l = []
            print(data)
            for data_item in data:
                d = {
                    f"{self.COLUMN_ID}": data_item[0],
                    f"{self.COLUMN_PATIENT_NAME}": data_item[1],
                    f"{self.COLUMN_ENTRY_TIME}": data_item[2],
                    f"{self.COLUMN_TEMP}": data_item[3],
                    f"{self.COLUMN_HEART_RATE}": data_item[4],
                    f"{self.COLUMN_SPO2}": data_item[5]
                }
                l.append(d)
            return {"data": l}
        except Exception as e:
            raise Exception(e)

    def insert(self, patient_name, entry_time, temperature, heart_rate, oxygen_level):
        try:
            self.cursor.execute(f"INSERT INTO {self.TABLE_NAME} ({self.COLUMN_PATIENT_NAME}, {self.COLUMN_ENTRY_TIME}, {self.COLUMN_TEMP}, {self.COLUMN_HEART_RATE}, {self.COLUMN_SPO2}) VALUES(?, ?, ?, ?, ?)", (patient_name, entry_time, temperature, heart_rate, oxygen_level))
            self.conn.commit()
            return {"insert_status": "successfull"}
        except Exception as e:
            raise Exception(e)
            # return {"insert_status": "unsuccessfull"}

    # def delete_cat(self, cat_name):
    #     try:
    #         self.cursor.execute(f"DELETE  FROM {self.TABLE_NAME} WHERE cat_name=?", (cat_name, ))
    #         self.conn.commit()
    #     except Exception as e:
    #         raise Exception(e)
    #
    # def update_cat(self):
    #     pass
