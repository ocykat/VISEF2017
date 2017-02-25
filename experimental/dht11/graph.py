import matplotlib.pyplot as plt

monitor = open("E:\\Arduino\\_VISEF2017_\\_experimental\\DHT11\\monitor.txt", "r")

temp = []
hum = []
time = []

linecnt = 0
readcnt = 0
timetracker = 0

for line in monitor:
	linecnt+= 1
	if linecnt % 6 == 1:
		index = float(line)
		temp.append(index)
		readcnt += 1
		timetracker += 2;
		time.append(timetracker);
	if linecnt % 6 == 3:
		index = float(line)
		hum.append(index)

# Graph
# Temperature graph
plt.subplot(2, 1, 1)
plt.plot(time, temp)
plt.xlabel("Time (s)")
plt.ylabel("Temperature")
plt.xlim([0, 30*60]) # 30 mins
# plt.ylim([-1, 2])
plt.title("Water-level Sensor's Read")

# #Humidity graph
plt.subplot(2, 1, 2)
plt.plot(time, hum)
plt.xlabel("Time (s)")
plt.ylabel("Humidity")
plt.xlim([0, 30*60]) # 30 mins
plt.ylim([44.5, 47.5])

plt.show()