file_in = open("race.in", "r")
N = int(file_in.readline())
flag = True
string = []
passed = []
country = ""

for i in range(N):
    string.append(list(map(str, file_in.readline().split())))

def merging(left, right):
    i = 0
    j = 0
    result = []
    left_len = len(left)
    right_len = len(right)

    while i < left_len and j < right_len:
        if left[i][0] <= right[j][0]:
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
    merged_array_1 = array[:middle]
    merged_array_2 = array[middle:]
    merged_array_1 = merge_sort(merged_array_1)
    merged_array_2 = merge_sort(merged_array_2)

    return merging(merged_array_1, merged_array_2)

file_out = open("race.out", "w")
string = merge_sort(string)
for j in range(N):
    if string[j][0] != country:
        flag = True
    if flag:
        print("===" + " " + string[j][0] + " " + "===", file = file_out)
        country = string[j][0]
        flag = False
    if string[j][0] == country:
        print(string[j][1], file = file_out)

file_out.close()        
