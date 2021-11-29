filein = open("antiqs.in", "r")
N = int(filein.readline())
arr = []

for i in range(1, N + 1):
    arr.append(i)

for j in range(1, N):
    arr[j // 2], arr[j] = arr[j], arr[j // 2]

fileout = open("antiqs.out", "w")
print(*arr, file = fileout)

fileout.close()
