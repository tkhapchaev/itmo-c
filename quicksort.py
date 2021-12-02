import random
file_in = open("quicksort.in", "r")
file_out = open("quicksort.out", "w")

N = int(file_in.readline())
arr = list(map(int, file_in.readline().split()))

def quicksort(array):
    if len(array) <= 1:
        return array

    else:
        pivot = array.pop(random.randint(0, len(array) - 1))
        left = [i for i in array if i < pivot]
        right = [j for j in array if j >= pivot]

        return quicksort(left) + [pivot] + quicksort(right)

arr = quicksort(arr)
print(*arr, file = file_out)