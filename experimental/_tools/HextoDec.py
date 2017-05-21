hex_str = str(raw_input("hexadecimal: "))

hex_digits = [str(i) for i in range(10)] + ["A", "B", "C", "D", "E", "F"]
multiplier = 1
dec_num = 0

for i in range(len(hex_str)-1, -1, -1):
    dec_num += int(hex_digits.index(hex_str[i]))*multiplier
    multiplier *= 16

print dec_num