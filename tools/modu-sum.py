filename = "./build/src/datastore.dat"

if __name__ == "__main__":
    f = open(filename, 'r')
    res = [0 for i in range(0, 13)]
    for line in f:
        l = line.split()
        if len(l) <= 1:
            continue
        cnum = int(l[0])
        if (len(l)-2)/cnum > 9:
            res[cnum] += 1
    print(res)
