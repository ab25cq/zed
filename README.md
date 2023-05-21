Zed interpreter written by comelang

In my hobby

USAGE

```
    > zed '"HELLO WORLD"'
    HELLO WORLD
    > cat a.c
    AAA
    BBB
    CCC
    > cat a.c | zed '.split(/\n/)'
    [AAA,BBB,CCC]
    > cat a.c | zed '.split(/\n/).join("+")'
    AAA+BBB+CCC
    > echo 123 | zed '.scan(/./)'
    [1,2,3]
    > echo 123 | zed '.scan(/./).map  { it + "ab" }'
    [1ab,2ab,3ab]
    > zed '(1+1).to_string() + "B"'
    2B
    > echo abc | zed '.sub_block(/./g) { it * 2 }'
    aabbcc
    
    > echo abc | zed '(.scan(/./) * 2).join("")'
    abcabc
    > zed '"HELLO WORLD".scan(/./).map { it * 2 }.join("")'
    HHEELLLLOOO  WWOORRLLDD
    > zed '"HELLO WORLD".scan(/./).map { it * 2 + "\n" }.join("").chomp()'
    HH
    EE
    LL
    OO
    
    WW
    OO
    RR
    LL
    DD
    
    > zed '[1,4,3,10].sort()'
    [1,3,4,10]
    > zed 'sum=0; [1,4,3,10].each { sum = sum + it }; sum'
    18
    > zed '"HELLO WORLD".scan(/./).map { if it == " " { it } else { it + "B" } }.join("")'
    HBEBLBLBOB WBOBRBLBDB
    > ls | zed '.scan(/./).sort().uniq()'
    [-,.,2,3,4,5,6,7,8,9,A,C,D,E,I,L,M,N,R,S,_,a,b,c,d,e,f,g,h,i,k,l,m,n,o,p,r,s,t,u,v,x,y,z]
```
