file_in = open("calcin.txt", "r")
file_out = open("calcout.txt", "w")
string = file_in.readline().split()
plus = []
minus = []
result = 0

if string[0] != "+" and string[0] != "-":
    plus.append(int(string[0]))
if string[0] == "-":
    minus.append(int(string[0]))

for i in range(len(string)):
    if string[i] == "+":
        plus.append(int(string[i + 1]))
    if string[i] == "-":
        minus.append(int(string[i + 1]))

result = sum(plus) - sum(minus)
print(result, file = file_out)
file_in.close()
file_out.close()
