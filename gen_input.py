import random

random.seed(42)

V = 100000
E = 4000000

print(V)
for i in range(1, V+1):
    print(i)

print(E)
for _ in range(E):
    a = random.randint(1, V)
    b = random.randint(1, V)
    w = random.randint(1, 20)
    print(a, b, w)

print(1, V)
