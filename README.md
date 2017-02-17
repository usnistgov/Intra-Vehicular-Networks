# I- Open vSwitch Setup 
# Introduction 

Open vSwitch or OVS, is an open-source implementation of a distributed virtual multilayer switch. Providing L2 capabilities for hardware virtualization environments is its main feature, It supports multiple protocols and standards as well such as OpenFlow sFlow, SPAN, RSPAN, CLI, LACP and 802.1ag

##I-1 Infrastructure Setup 

![Image](https://image.ibb.co/mzFNRF/Untitled.png)


### I- 1-1 Setup Edge Nodes ( Pi1 and Pi2 ) as Wireless Access points

Common config for both Pi1 and Pi2 : 

_I- 1-1-a packages to install and files to edit _ 

**Download hostap + dsnmasq packages** 

`sudo apt-get install dnsmasq hostapd`

**Add the following line to the bottom of the file :*  /etc/dhcpcd.conf

`denyinterfaces wlan0`

**Edit the Pi1 /etc/network/interfaces/**

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
      
**edit the Pi2 /etc/network/interfaces/**




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
   broadcast 10.11.201.255
   
   ```
   



### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
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
