import os
import sys
import csv

import serial

from time import sleep

DEFAULT_PORT = 'COM4'
DEFAULT_BAUD = 9600
CSV_FILE = "cards.csv"


class SerialInterface:
    def __init__(self, port=DEFAULT_PORT, baudrate=DEFAULT_BAUD, timeout=1):
        self._ser = serial.Serial()
        self._ser.port = port
        self._ser.baudrate = baudrate
        self._ser.timeout = timeout
        try:
            self._ser.open()
        except serial.SerialException:
            print("Error while opening")
            sys.exit()

    def read_data(self): # returns received data as string
        try:
            return self._ser.readline().decode("utf-8")
        except:
            return "something went wrong"

    def write_data(self, data): # sends string data
        to_send = bytes(data,'ASCII')
        self._ser.write(to_send)

    def close(self):
        self._ser.close()


# converts string of cards and pins to dict
def decode_cards(card_string):
    pairs = card_string.split(',')
    card_dict = {}
    for pair in pairs:
        card, pin = pair.strip().split(':')
        card_dict[card] = pin
    return card_dict


# converts dict to string of cards and pins
def encode_cards(card_dict):
    return ','.join(f'{card}:{pin}' for card, pin in card_dict.items())


# returns line number and corresponding data if found
# returns -1 and error message if not
def find_line(card, pin):
    with open(CSV_FILE) as csvfile:
        reader = csv.reader(csvfile)

        line_number = 0
        while True:
            try:
                line = next(reader)
                cards = decode_cards(line[0])
                balance = line[1]

                if card in cards.keys():
                    if cards[card] == pin:
                        return line_number, cards, balance
                    else:
                        return -1, "Error: wrong pin"

                line_number += 1

            except StopIteration:
                return -1, "Error: card not found"


# rewrites a line in csv file
def change_line(line_number, new_line):
    temp_path = CSV_FILE + '.tmp'

    with open(CSV_FILE, 'r') as src, open(temp_path, 'w') as dst:
        for i, line in enumerate(src):
            if i == line_number:
                dst.write(new_line + '\n')
                print(new_line)
            else:
                dst.write(line)
                print(line, end="")

    os.replace(temp_path, CSV_FILE)


def response(serial_inp):
    operation = serial_inp[0]
    card = serial_inp[1]
    pin = serial_inp[2]

    result = find_line(card, pin)
    if result[0] == -1:
        return result[1]

    if operation == "pay":
        change_line(result[0], f"\"{encode_cards(result[1])}\",{int(result[2]) - int(serial_inp[3])}")
        return "Payment: success"
    elif operation == "dep":
        change_line(result[0], f"\"{encode_cards(result[1])}\",{int(result[2]) + int(serial_inp[3])}")
        return "Deposit: success"
    elif operation == "bal":
        return f"Balance: {result[2]}"
    elif operation == "ch_pin":
        result[1][card] = serial_inp[3]
        change_line(result[0], f"\"{encode_cards(result[1])}\",{result[2]}")
        return "Pin changed"


def main():
    com_port = input("COM port: ")
    if com_port:
        ser = SerialInterface(port=com_port)
    else:
        ser = SerialInterface()

    while True:
        try:
            values = ser.read_data().split(",")
            res = response(values)
            ser.writeData(res)
        except KeyboardInterrupt:
            ser.close()

if __name__ == "__main__":
    main()