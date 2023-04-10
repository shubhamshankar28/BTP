import os
import sys

print(sys.argv)
location = sys.argv[1]
dataset = sys.argv[2]
potentialFailure = int(sys.argv[3])

fileName = "longRunResult" + dataset + ".txt"
longRunResult = open(fileName , "w")
potentialFailures = 0
totalRuns = 100
ateAvg = 0.0
rpeTransAvg = 0.0
rpeRotAvg = 0.0



bestAte = 10000000.00

for i in range(totalRuns):
    os.system('./runBTPAggregate.bash ' + location + ' ' + dataset)
    print("done executing")
    f = open("ateResult" + dataset + ".txt" , "r")
    res = f.readline()
    totruns , ateVal = res.split(" ")
    f.close()
    print("done reading ate values")
    f = open("rpeResult" + dataset + ".txt" , "r")
    res1 = f.readline()
    totruns1 , rpeTransVal , rpeRotVal = res1.split(" ")
    print("done reading rpe values")
    longRunResult.write(str(i) + " " + res + " " + res1)
    longRunResult.write("\n")


    # if((float(ateVal) < 0.06) and (totruns > 859)):
    #     print("code breaks at ")
    #     print(i)
    #     print("current result : " + dataset)
    #     print(ateVal + " " + rpeTransVal + " " + rpeRotVal)    
    #     break

    if(int(totruns) < potentialFailure):
        potentialFailures = potentialFailures + 1
    else:
        if(float(ateVal) < bestAte):
            with open("./CameraTrajectory"+dataset+".txt" , "r") as f:
                with open("bestResult" +  dataset + ".txt" , "w") as f1:
                    for line in f:
                        f1.write(line)
            # bestResultFile.write(ateVal + " " + rpeTransVal + " " + rpeRotVal)
            bestAte = float(ateVal)


    print("current result : " + dataset)
    print(ateVal + " " + rpeTransVal + " " + rpeRotVal)
    print(" ")
    print(" ")
    print(" ")
    ateAvg = ateAvg + float(ateVal)
    rpeTransAvg = rpeTransAvg + float(rpeTransVal)
    rpeRotAvg = rpeRotAvg + float(rpeRotVal)

longRunResult.write(str(ateAvg/totalRuns) + " " + str(rpeTransAvg/totalRuns) + " " + str(rpeRotAvg/totalRuns))
longRunResult.write("\n")
longRunResult.write(str(potentialFailures))


