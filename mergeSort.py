filein = open("sort.in", "r")
N = int(filein.readline())
A = list(map(int, filein.readline().split()))

def merging(left, right):
    i = 0
    j = 0
    result = []
    left_len = len(left)
    right_len = len(right)

    while i < left_len and j < right_len:
        if left[i] <= right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            j += 1

    result = result + left[i:] + right[j:]
    return result

def merge_sort(array):
    if len(array) == 1 or len(array) == 0:
        return array
    middle = len(array) // 2
    arr1 = array[:middle]
    arr2 = array[middle:]
    arr1 = merge_sort(arr1)
    arr2 = merge_sort(arr2)

    return merging(arr1, arr2)

fileout = open("sort.out", "w")
print(*merge_sort(A), file = fileout)

fileout.close()
