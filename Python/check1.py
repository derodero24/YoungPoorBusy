
import sys
sys.stdin = open('../input.txt')

while True:
    count = int(input())
    print(count)

    if count == 0: break

#     for i in range(0,count):
#         s = input()
#         print(s)
    for i in range(0,count):
        s = input().split()
        for j in s: print(j)
