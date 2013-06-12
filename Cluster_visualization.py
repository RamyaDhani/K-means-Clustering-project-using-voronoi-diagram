import math
import os, sys
import matplotlib.pyplot as plt 
import numpy as np

 

fig, ax = plt.subplots()


name = "Clusteringresults.txt"
f = open(name)
line = f.read()
f.close()
overallstr = line
alllines = overallstr.split("\n")

st = 1;
en = 0;
for i in range(0,len(alllines)):
    if(alllines[i] == 'NEW CENTROIDS ARE...: '):
        en = i 
        break

c_0 = []
c_1 = []
for i in range(st,en):
    c = alllines[i].split()
    c_0.append(float(c[0]))
    c_1.append(float(c[1]))
    
Old_centroids = [c_0,c_1]
    

start = []
end = []

Newcentroidslist = []
fNewcentroidslist = []
Segmentslist = []
Segments = []

for i in range(0,len(alllines)):
    if(alllines[i] == 'NEW CENTROIDS ARE...: '):
        Newcentroidslist.append(alllines[i+1])

#Input the new centroids
for i in range(0,len(Newcentroidslist)):
    x = Newcentroidslist[i].split('[[')
    x = x[1]
    x = x.split(',]]')
    x = x[0]
    x = x.split(',],[')
    x0 = x[0].split(',')
    x1 = x[1].split(',')
    x0_new = []
    x1_new = []
    x0_new = [float(m) for m in x0]
    x1_new = [float(m) for m in x1]
    x_new = [x0_new,x1_new]
    fNewcentroidslist.append(x_new)
    print x_new

for i in range(0,len(alllines)):
    if(alllines[i] == 'Segments are..: '):
        start.append(i+1)
    if(alllines[i] == 'OLD CENTROIDS ARE...: '):
        end.append(i)
    if(alllines[i] == 'Reached Convergence..'):
        end.append(i)

end = end[1:len(end)]

for i in range(0,len(start)):
    rstart = start[i]
    rend = end[i]
    for j in range(rstart,rend):
        Segments.append(alllines[j])
    
    Segmentslist.append(Segments)
    Segments = []
fSegmentslist = []
for i in range(0,len( Segmentslist)):
    Segments = []
    for j in range(0,len(Segmentslist[i])):
            
            #print Segmentslist[i][j]
            f0 = []
            f1 = []
            s0 = []
            s1 = []
            t = []
            x = Segmentslist[i][j].split()
            x = x[0].split('[[')
            x = x[1]
            x = x.split(']]')
            x = x[0]
            x = x.split('],[')
            
            x0 = x[0].split(',')
            x0_new = [float(x0[0]),float(x0[1])]
            x1 = x[1].split(',')
            x1_new = [float(x1[0]),float(x1[1])]
            x_new = [x0_new,x1_new]
            Segments.append(x_new)
    fSegmentslist.append(Segments)

name = "Points.txt"
f = open(name)
line = f.read()
f.close()
overallstr1 = line
alllines1 = overallstr1.split("\n")
z = []
m = []
for i in range(0,len(alllines1)-1):
    al = alllines1[i].split()
    
    z.append(float(al[0]))
    m.append(float(al[1]))
    
#Plot the centroids along with the points

for t in range(0,len(fSegmentslist)):
    if(t != (len(fSegmentslist)-1)):
        points, = ax.plot(z, m,marker='.',color = 'c', linestyle='None')
        points, = ax.plot(fNewcentroidslist[t][0], fNewcentroidslist[t][1], marker='o',color = 'b', linestyle='None')
        for i in range(0,len(fSegmentslist[t])):
            points, = ax.plot(fSegmentslist[t][i][0], fSegmentslist[t][i][1], marker='*',color ='r', linestyle='-')

        ax.set_xlim(-100, 100)
        ax.set_ylim(-100, 100)
        plt.pause(2.0)
        ax.clear()
    else:
        points, = ax.plot(z, m,marker='.',color = 'c', linestyle='None')
        points, = ax.plot(fNewcentroidslist[t][0], fNewcentroidslist[t][1], marker='o',color = 'b', linestyle='None')
        for i in range(0,len(fSegmentslist[t])):
            points, = ax.plot(fSegmentslist[t][i][0], fSegmentslist[t][i][1], marker='*',color ='r', linestyle='-')

        ax.set_xlim(-100, 100)
        ax.set_ylim(-100, 100)   
        plt.pause(5.0)
        ax.clear()
        
plt.close()        
    
    
            
            
            
                    
            
