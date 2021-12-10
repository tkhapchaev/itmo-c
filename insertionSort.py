file_in = open("smallsort.in", "r")
file_out = open("smallsort.out", "w")
N = int(file_in.readline())
array = list(map(int, file_in.readline().split()))

for i in range(1, len(array)):
    k = array[i]
    j = i - 1
    while k < array[j] and j >= 0:
        array[j + 1] = array[j]
        j = j - 1
    array[j + 1] = k

print(*array, file = file_out)
file_out.close()
