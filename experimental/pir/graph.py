import matplotlib.pyplot as plt

monitor = open("E:\\Arduino\\_VISEF2017_\\_experimental\\PIR\\monitor.txt", "r")

data = []
i = -1 # lines actually start at 1, but we pretend that they start at 0
linecount = 0
max = 0

for line in monitor:
	i += 1
	if i % 2 == 0: # lines start at 0 not 1, so the data is in the "even" lines
		index = int(line)
		data.append(index)
		linecount += 1

time = [j for j in range(0, linecount)]

print(len(time))
print(len(data))
plt.plot(time, data)
plt.xlabel("Signal")
plt.ylabel("Digital input")
plt.ylim(-0.2, 1.2)
plt.xlim(0, 15000)
plt.title("PIR's Digital signal\n")
plt.show()