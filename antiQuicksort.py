file_in = open("antiqs.in", "r")
N = int(file_in.readline())
arr = []

for i in range(1, N + 1):
    arr.append(i)

for j in range(1, N):
    arr[j // 2], arr[j] = arr[j], arr[j // 2]

file_out = open("antiqs.out", "w")
print(*arr, file = file_out)

file_out.close()
