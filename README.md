# I- Open vSwitch Setup 
# Introduction 

Open vSwitch or OVS, is an open-source implementation of a distributed virtual multilayer switch. Providing L2 capabilities for hardware virtualization environments is its main feature, It supports multiple protocols and standards as well such as OpenFlow sFlow, SPAN, RSPAN, CLI, LACP and 802.1ag

##I-1 Infrastructure Setup 

![Image](https://image.ibb.co/mzFNRF/Untitled.png)


### I- 1-1 Setup Edge Nodes ( Pi1 and Pi2 ) as Wireless Access points

#### I- 1-1-a : Pi1 and Pi2 :Download hostap + dsnmasq packages 

`sudo apt-get install dnsmasq hostapd`

#### I- 1-1-b : Pi1 and Pi2 : Add the following line to the bottom of the file :  /etc/dhcpcd.conf

`denyinterfaces wlan0`

#### I- 1-1-c : Pi1 : edit the file /etc/network/interfaces/

```allow-eth0 
       iface eth0 inet static
       address 10.11.200.41 
       netmask 255.255.255.0  
       gateway 10.11.200.200
       dns-nameservers 10.11.200.200

      allow-hotplug wlan0 
      iface wlan0 inet static
      address 10.11.201.1 
      netmask 255.255.255.0 
      network 10.11.201.0 
      broadcast 10.11.201.255 
 ```    
 
#### I- 1-1-d : Pi2 : edit the file /etc/network/interfaces/ 

```
allow-eth0
   iface eth0 inet static
   address 10.11.200.42 
   netmask 255.255.255.0  
   gateway 10.11.200.200
   dns-nameservers 10.11.200.200
   
   allow-hotplug wlan0 
   iface wlan0 inet static
   address 10.11.202.11 
   netmask 255.255.255.0 
   network 10.11.202.0 
   broadcast 10.11.202.255
 ```

#### I- 1-1-e : Edit the Pi1's /etc/hostapd/hostapd.conf

```
interface=wlan0
driver=nl80211
ssid=Pi1-AP
hw_mode=g
channel=6
ieee80211n=1
wmm_enabled=1
ht_capab=[HT40][SHORT-GI-20][DSSS_CCK-40] 
macaddr_acl=0 
```
#### I- 1-1-f : Edit the Pi2's /etc/hostapd/hostapd.conf

```
interface=wlan0
driver=nl80211
ssid=Pi2-AP
hw_mode=g
channel=6
ieee80211n=1
wmm_enabled=1
ht_capab=[HT40][SHORT-GI-20][DSSS_CCK-40] 
macaddr_acl=0 
```


#### I- 1-1-g : Pi1 and Pi2 : Load hostapd config on boot by editing the file : /etc/default/hostapd

`DAEMON_CONF="/etc/hostapd/hostapd.conf"`

#### I- 1-1-h : Pi1 : configure DNSMASQ by editiing the file : /etc/dnsmasq.conf : 
```
    interface=wlan0
    listen-address=10.11.201.1
    bind-interfaces
    server=8.8.8.8
    domain-needed
    bogus-priv
    dhcp-range=10.11.201.2,10.11.201.10,48h
```

#### I- 1-1-i : Pi2 : configure DNSMASQ by editiing the file : /etc/dnsmasq.conf : 

```
    interface=wlan0
    listen-address=10.11.202.11
    bind-interfaces
    server=8.8.8.8
    domain-needed
    bogus-priv
    dhcp-range=10.11.202.12,10.11.202.19,48h
```

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```



markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/usnistgov/Intra-Vehicular-Networks/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
