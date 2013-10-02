import sys

C = open(sys.argv[1],"r").readlines()

def findsection(sname):
  i = 0
  while i<len(C) and C[i].strip() != "[" + sname + "]":
    i += 1
  if i==len(C): 
    return None
  j = i+1
  while j<len(C) and C[j].strip()[:1] != '[':
    j += 1
  if j==len(C):
    return None
  return C[i+1:j]    

def split(s):
  l = []
  i = 0
  n = len(s)
  while True:
    while i < n and s[i]==' ': i += 1
    if i >= n: break
    j = i+1
    if s[i]=='"':
      while j < n and s[j]!='"': j += 1
      l.append(s[i+1:j])
    else:
      while j < n and s[j]!=' ': j += 1
      l.append(s[i:j])
    i = j + 1
  #print l  
  return l  
  
def addrule(R, a, b, p):
  for r in R:
    if r[0] == a:
      r[1].append([b,p])
      return
  R.append([a,[[b,p]]])      

def normalize(R):
  R.sort(key = lambda x: x[0] )
  for r in R:
    r[1].sort(key = lambda x: -x[1])
  
def make(sname):
  S = findsection(sname)
  if S == None:
    S = findsection("schema")
  if S == None:
    print "Error! No sections with rules were found in the config file" 
    return None
  R = []
  for s in S:
    l = split(s.strip())
    if l==[]: continue
    if l[0] == 'rule':
      addrule(R, l[1], l[2], float(l[3]))
    elif l[0] == 'swap':
      A = l[1]
      p = float(l[2])
      for a in A:
        for b in A:
          if a != b:
            addrule(R, a+b, b+a, p)
    elif l[0] == 'syml':
      p = float(l[3])
      addrule(R,l[1], l[2], p)
      addrule(R,l[1][::-1], l[2], p)
  normalize(R)
  return R  

def env(a):
  if " " in a: return '"'+a+'"'
  return a  
  
def save(g, S, sname):
  print >> g, "["+sname+"]"
  for r in S: 
    print >> g, "  rule", env(r[0]), 
    for i in r[1]:
      print >> g, env(i[0]), i[1],
    print >> g    
  print >> g    
  
H = make("horizontal")
V = make("vertical")  

g = open("_input_.cfg","w")
save(g, H, "horizontal")
save(g, V, "vertical")
print >> g, "[end]"
g.close()

    

