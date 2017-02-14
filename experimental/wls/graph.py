import matplotlib.pyplot as plt
import os 
dir_path = os.path.dirname(os.path.realpath(__file__))
print dir_path

monitor = open("E:\\Arduino\\_VISEF2017_\\_experimental\\WLS\\monitor1.txt", "r")

lb_data = []
ub_data = []
time = []

linecnt = 0
readcnt = 0
lb_accuracy = ub_accuracy = 0
for line in monitor:
    linecnt+= 1
    if linecnt % 6 == 1:
        index = int(line)
        lb_data.append(index)
        readcnt += 1
        if index == 0:
        	lb_accuracy += 1
    if linecnt % 6 == 3:
        index = int(line)
        ub_data.append(index)
        if index == 0:
        	ub_accuracy += 1
    if linecnt % 6 == 5:
    	time_index = round(float(line)/1000, 2)
    	time.append(time_index)
    if linecnt == 7199:
    	break

print "LB's accurate reads: %d/%d" %(lb_accuracy, len(lb_data))
print "UB's accurate reads: %d/%d" %(ub_accuracy, len(ub_data))

# Graph
# Temperature graph
plt.subplot(2, 1, 1)
plt.plot(time, lb_data)
plt.xlabel("Time (s)")
plt.ylabel("LB's input")
plt.xlim([0, 10*60]) # 10 mins
plt.ylim([-1, 2])
plt.title("Water-level Sensor's Read")

# #Humidity graph
plt.subplot(2, 1, 2)
plt.plot(time, lb_data)
plt.xlabel("Time (s)")
plt.ylabel("UB's input")
plt.ylim([-1, 2])
plt.xlim([0, 10*60]) # 10 mins

plt.show()