import matplotlib.pyplot as plt

monitor = open("E:\\Arduino\\pir2\\monitor.txt", "r")

data = []
i = -1 # lines actually start at 1, but we pretend that they start at 0
linecount = 0
max = 0

def ReadtoVolt(x):
	return x/1023*5000

for line in monitor:
	i += 1
	if i % 2 == 0: # lines start at 0 not 1, so the data is in the "even" lines
		index = float(line)
		if max < index:
			max = index
		data.append(ReadtoVolt(index))
		linecount += 1

time = [j for j in range(0, linecount)]

print(len(time))
print(len(data))
plt.plot(time, data)
plt.xlabel("Time (bit - rate: 9600 bit/sec)")
plt.ylabel("Analog signal (mV)")
plt.title("PIR Analog signal analyzing\n")
plt.show()