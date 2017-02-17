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

#### I- 1-1-h : Pi1 : configure DNSMASQ by editing the file : /etc/dnsmasq.conf : 
```
    interface=wlan0
    listen-address=10.11.201.1
    bind-interfaces
    server=8.8.8.8
    domain-needed
    bogus-priv
    dhcp-range=10.11.201.2,10.11.201.10,48h
```

#### I- 1-1-i : Pi2 : configure DNSMASQ by editing the file : /etc/dnsmasq.conf : 

```
    interface=wlan0
    listen-address=10.11.202.11
    bind-interfaces
    server=8.8.8.8
    domain-needed
    bogus-priv
    dhcp-range=10.11.202.12,10.11.202.19,48h
```

#### I- 1-1-j : Commands to Run Pi1 and Pi2 as access point : 

```
sudo ifdown wlan0
sudo ifup wlan0
sudo service dhcpd restart
sudo service dnsmasq restart
sudo /usr/sbin/hostapd /etc/hostapd/hostapd.conf

```
### I- 1-2 : Setup IoT Switch (Pi3)


#### I- 1-2-A : Network configuration

##### I- 1-2-A-a : Edit the /etc/network/interfaces

```allow-eth0 
       iface eth0 inet static
       address 10.11.200.43 
       netmask 255.255.255.0  
       gateway 10.11.200.200
       dns-nameservers 10.11.200.200

      auto wlan1 
      iface wlan1 inet dhcp
      wireless-essid Pi1-AP
      
      auto wlan2 
      iface wlan2 inet dhcp
      wireless-essid Pi2-AP
 
 ```    

##### I- 1-2-A-b : restart wlan1 and wlan2 to get their addresses from Pi1 access point and Pi2 access point respectively

```
sudo ifdown wlan1
sudo ifup wlan1

sudo ifdown wlan2
sudo ifup wlan2
 ```  

#### I- 1-2-B : OVS Configuration 

##### I- 1-2-B-a : Install Open VSwitch on the IoT Switch (Pi3)

`sudo apt-get install ovsdbmonitor openvswitch-switch openvswitch-controller openvswitch-pki` 

##### I- 1-2-B-b : Add Bridges

```
sudo ovs-vsctl add-br br1 
sudo ovs-vsctl add-br br2
 ```  

##### I- 1-2-B-c : Add ports wlan1 and wlan2 to br1 and br2 respectively 
```
sudo ovs-vsctl add-port br1 wlan1
sudo ovs-vsctl add-port br2 wlan2
 ```
##### I- 1-2-B-d : Assign wlan1 and wlan2 addresses to OVS local interfaces br1 and br2 respectively ( here Pi1 leased 10.11.201.9/24 to wlan1 and Pi2 leased 10.11.202.12/24 to wlan2 ): 
```
sudo ifconfig wlan1 0
sudo ifconfig br1 10.11.201.9 netmask 255.255.255.0
sudo ifconfig wlan2 0
sudo ifconfig br1 10.11.202.12 netmask 255.255.255.0
```    
    

##### I- 1-2-B-e : Create patch interfaces to connect br1 and br2

```
   sudo ovs-vsctl add-port br1 patch1
   sudo ovs-vsctl add-port br2 patch2
   sudo ovs-vsctl set-interface patch1 type=patch
   sudo ovs-vsctl set interface patch1 type=patch
   sudo ovs-vsctl set interface patch2 type=patch
   sudo ovs-vsctl set interface patch1 options:peer=patch2
   sudo ovs-vsctl set interface patch2 options:peer=patch1
   
```


##### I- 1-2-B-f : Checking the configguration : 

**flows**
```
sudo ovs-ofctl dump-flows br1
sudo ovs-ofctl dump-flows br2
```
**ports**
```
sudo ovs-ofctl dump-ports br1
sudo ovs-ofctl dump-ports br2
```
**bridges**

`sudo ovs-vsctl show`


##### I- 1-2-B-g : connect br1 and br2 to the OpenDayLight Controller 
```
   sudo ovs-vsctl set-controller br1 tcp:10.11.200.121:6633
   sudo ovs-vsctl set-controller br2 tcp:10.11.200.121:6633
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
