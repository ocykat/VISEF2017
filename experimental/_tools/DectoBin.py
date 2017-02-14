dec_num = int(raw_input("decimal: "))

reverse_bin_arr = []
bin_str = ""

while True:
	if (dec_num / 2 == 0):
		reverse_bin_arr.append(1)
		break
	reverse_bin_arr.append(dec_num % 2)
	dec_num /= 2

for i in range(len(reverse_bin_arr) - 1, -1, -1):
	bin_str += str(reverse_bin_arr[i])

print bin_str