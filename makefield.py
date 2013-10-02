import sys, random

def matrix(n,m):
  a = []
  for i in range(n):
    a.append(['.']*m)
  return a

def printmatrix(a, f):
  n = len(a)
  m = len(a[0]) 
  print >> f, n, m
  for i in range(n):
    print >> f, "".join(a[i])

cfg = open(sys.argv[1],"r")

s = cfg.readline()
while s.strip() != '[end]':
  s = cfg.readline()
  
body = "".join(cfg.readlines())

exec(body)

f = open("_input_.dat","w")
a = init()  
printmatrix(a,f)                         
f.close()

