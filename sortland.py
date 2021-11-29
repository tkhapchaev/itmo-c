fin = open("sortland.in", "r")
fout = open("sortland.out", "w")
N = int(fin.readline())
A = list(map(float, fin.readline().split()))
B = []

for i in range(0, len(A)):
    B.append(A[i])
for i in range(1, len(A)):
    key = A[i]
    j = i - 1
    while key < A[j] and j >= 0:
        A[j + 1] = A[j]
        j = j - 1
    A[j + 1] = key

richest = A[-1]
poorest = A[0]
average = A[len(A) // 2]

for i in range(0, len(B)):
    if B[i] == richest:
        richestplace = i
    if B[i] == poorest:
        poorestplace = i
    if B[i] == average:
        averageplace = i

richestplace = str(richestplace + 1)
poorestplace = str(poorestplace + 1)
averageplace = str(averageplace + 1)
fout.write(poorestplace + " " + averageplace + " " + richestplace)
fout.close()