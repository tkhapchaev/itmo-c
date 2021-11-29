fin = open("smallsort.in", "r")
fout = open("smallsort.out", "w")
N = int(fin.readline())
A = list(map(int, fin.readline().split()))

for i in range(1, len(A)):
    k = A[i]
    j = i - 1
    while k < A[j] and j >= 0:
        A[j + 1] = A[j]
        j = j - 1
    A[j + 1] = k

print(*A, file = fout)
fout.close()