Question: Why my raspberry PI has 
My Raspberry Pi B+ is running Raspbian (Linux version 3.18.11+) updated, etc.
It's connected to the network by ethernet. I want a static IP, and have assigned it one in /etc/network/interfaces (shown below). However, every boot it requests an address via DHCP and is assigned one! Both the static and dynamic addresses work to access the machine on the network.
Where can I stop the DHCP address from being requested and assigned??

From the command prompt, type:
nano /etc/network/interfaces and press enter

Change the following entry from:
allow-hotplug eth0
iface eth0 inet static
address 192.168.0.8
netmask 255.255.255.0
gateway 192.168.0.254
network 192.168.0.0
broadcast 192.168.0.255

disabled the second IP
CMD: sudo ip addr del 10.0.55.55/24 dev eth0 
disabled the second IP
I had the same problem. I had configured x.x.x.250 in /etc/network/interfaces and was getting x.x.x.20 as well, although that did not exist anywhere.
I'm using ethernet and because I don't need dhcp I disabled the dhcpcd service as follows:

sudo systemctl stop dhcpcd
sudo systemctl disable dhpcd
sudo systemctl daemon-reload
sudo systemctl restart networking
sudo reboot


mail flush queue
# mailq
# postsuper -d  ALL
# postsuper -d ALL  deferred


Search package
#  aptitude search php5

@@@ Add monodb, apache2, php5, mysql , postgresql , cacti

apt-get install cacti tree 




@@@ Play music 
# apt-get install mpg321

@@@ application
phpsysinfo,  phpIPAM, phpmyadmin




# aplay CD_track13.wav
# mpg321 bubbling_water_1.mp3



# php -m
root@pjt-pi2:~# php -m
[PHP Modules]
bcmath
bz2
calendar
Core
ctype
curl
date
dba
dom
ereg
exif
fileinfo
filter
ftp
gd
gettext
gmp
hash
iconv
imagick
imap
json
ldap
libxml
mbstring
mcrypt
mhash
mysql
mysqli
openssl
pcntl
pcre
PDO
pdo_mysql
Phar
posix
readline
Reflection
sasl
session
shmop
SimpleXML
snmp
soap
sockets
SPL
ssh2
standard
sysvmsg
sysvsem
sysvshm
tokenizer
wddx
XCache
XCache Cacher
XCache Coverager
XCache Optimizer
xml
xmlreader
xmlrpc
xmlwriter
Zend OPcache
zip
zlib

[Zend Modules]
XCache
XCache Cacher
XCache Coverager
XCache Optimizer
Zend OPcache


主节点：
GRANT REPLICATION SLAVE ON *.* TO 'root'@'192.168.0.6' IDENTIFIED BY 'pzgzy457'
show master status; 
输出pos=9920


从节点：
change master to master_host='192.168.0.8',    master_user='root',  master_password='pzgzy457',master_log_file='mysql-bin.000002',master_log_pos=9920;
...

# arp-scan #
# arp-scan --interface=eth1 192.168.100.0/29 
# arp-scan --interface=eth0 --localnet --ouifile=/usr/share/ieee-oui.txt

##### Memcache 使用， 它用来在内存中保存一个二维表，有key,有value， 以服务形式提供


# redis-server redis-cli #
内存使用的另外一种用法

# 安装 php-calendar @ 192.168.0.8
download and move to /var/www/html

