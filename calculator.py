filein = open("calcin.txt", "r")
fileout = open("calcout.txt", "w")
string = filein.readline().split()

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
print(result, file = fileout)

fileout.close()