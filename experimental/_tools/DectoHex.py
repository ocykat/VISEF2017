dec_num = input("decimal: ")

hex_digits = [str(i) for i in range(10)] + ["A", "B", "C", "D", "E", "F"]
reverse_hex_arr = []
hex_str = ""

while True:
	if (dec_num / 16 == 0):
		reverse_hex_arr.append(hex_digits[dec_num % 16])
		break
	reverse_hex_arr.append(hex_digits[dec_num % 16])
	dec_num /= 16

for i in range(len(reverse_hex_arr) - 1, -1, -1):
	hex_str += str(reverse_hex_arr[i])

print hex_str