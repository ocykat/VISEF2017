bin_str = str(raw_input("binary: "))

multiplier = 1
dec_num = 0

for i in range(len(bin_str)-1, -1, -1):
	dec_num += int(bin_str[i])*multiplier
	multiplier *= 2

print dec_num