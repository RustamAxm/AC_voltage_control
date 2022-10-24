import requests
import re
import time


class AcVoltageControl:
    def __init__(self, ip):
        self.ip = ip

    def set_voltage(self, value):
        response = requests.get('http://{}/?set_U{}'.format(self.ip, value))
        if response.ok:
            time.sleep(2)
        else:
            print('response error')

    def get_current_voltage(self):
        response = requests.get('http://{}/?get_U'.format(self.ip))
        if response.ok:
            time.sleep(2)
        else:
            print('response error')

        data = re.findall('Current voltage: [0-9]{1,3}[\.][0-9]{1,3}', response.text)[0]
        return data.split(':')[1]


def latr_test():
    LATR = AcVoltageControl('172.17.182.159')
    LATR.set_voltage(230)
    time.sleep(2)
    print(LATR.get_current_voltage())


if __name__ == '__main__':
    latr_test()