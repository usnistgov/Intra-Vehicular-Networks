# Open vSwitch Setup 
## Introduction 

Open vSwitch or OVS, is an open-source implementation of a distributed virtual multilayer switch. Providing L2 capabilities for hardware virtualization environments is its main feature, It supports multiple protocols and standards as well such as OpenFlow sFlow, SPAN, RSPAN, CLI, LACP and 802.1ag

### Infrastructure Setup 

![Image](https://image.ibb.co/mzFNRF/Untitled.png)


#### Setup Edge Nodes ( Pi1 and Pi2 ) as Wireless Access points

##### Pi1 and Pi2 :Download hostap + dsnmasq packages 

`sudo apt-get install dnsmasq hostapd`

##### Pi1 and Pi2 : Add the following line to the bottom of the file :  /etc/dhcpcd.conf

`denyinterfaces wlan0`

##### Pi1 : edit the file /etc/network/interfaces/

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
 
##### Pi2 : edit the file /etc/network/interfaces/ 

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

##### Edit the Pi1's /etc/hostapd/hostapd.conf

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
##### Edit the Pi2's /etc/hostapd/hostapd.conf

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


##### Pi1 and Pi2 : Load hostapd config on boot by editing the file : /etc/default/hostapd

`DAEMON_CONF="/etc/hostapd/hostapd.conf"`

##### Pi1 : configure DNSMASQ by editing the file : /etc/dnsmasq.conf : 
```
    interface=wlan0
    listen-address=10.11.201.1
    bind-interfaces
    server=8.8.8.8
    domain-needed
    bogus-priv
    dhcp-range=10.11.201.2,10.11.201.10,48h
```

##### Pi2 : configure DNSMASQ by editing the file : /etc/dnsmasq.conf : 

```
    interface=wlan0
    listen-address=10.11.202.11
    bind-interfaces
    server=8.8.8.8
    domain-needed
    bogus-priv
    dhcp-range=10.11.202.12,10.11.202.19,48h
```

##### Commands to Run Pi1 and Pi2 as access point : 

```
sudo ifdown wlan0
sudo ifup wlan0
sudo service dhcpd restart
sudo service dnsmasq restart
sudo /usr/sbin/hostapd /etc/hostapd/hostapd.conf

```
###  Setup IoT Switch (Pi3)


####   Network configuration

#####  Edit the /etc/network/interfaces

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

###### Restart wlan1 and wlan2 to get their addresses from Pi1 access point and Pi2 access point respectively

```
sudo ifdown wlan1
sudo ifup wlan1

sudo ifdown wlan2
sudo ifup wlan2
 ```  

##### OVS Configuration 

###### Install Open VSwitch on the IoT Switch (Pi3)

`sudo apt-get install ovsdbmonitor openvswitch-switch openvswitch-controller openvswitch-pki` 

###### Add Bridges

```
sudo ovs-vsctl add-br br1 
sudo ovs-vsctl add-br br2
 ```  

###### Add ports wlan1 and wlan2 to br1 and br2 respectively 
```
sudo ovs-vsctl add-port br1 wlan1
sudo ovs-vsctl add-port br2 wlan2
 ```
###### Assign wlan1 and wlan2 addresses to OVS local interfaces br1 and br2 respectively ( here Pi1 leased 10.11.201.9/24 to wlan1 and Pi2 leased 10.11.202.12/24 to wlan2 ): 
```
sudo ifconfig wlan1 0
sudo ifconfig br1 10.11.201.9 netmask 255.255.255.0
sudo ifconfig wlan2 0
sudo ifconfig br1 10.11.202.12 netmask 255.255.255.0
```    
    

###### Create patch interfaces to connect br1 and br2

```
   sudo ovs-vsctl add-port br1 patch1
   sudo ovs-vsctl add-port br2 patch2
   sudo ovs-vsctl set-interface patch1 type=patch
   sudo ovs-vsctl set interface patch1 type=patch
   sudo ovs-vsctl set interface patch2 type=patch
   sudo ovs-vsctl set interface patch1 options:peer=patch2
   sudo ovs-vsctl set interface patch2 options:peer=patch1
   
```


###### Checking the configguration : 

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


###### connect br1 and br2 to the OpenDayLight Controller 
```
   sudo ovs-vsctl set-controller br1 tcp:10.11.200.121:6633
   sudo ovs-vsctl set-controller br2 tcp:10.11.200.121:6633
```
### Setup OpenDayLight Controller

#### Download packages from OpenDayLight Website : 

`https://www.opendaylight.org/software/downloads/beryllium-sr4`

#### Run the following command in the /ODL-Folder/build/ folder : 

`./karaf -Of13`

#### Install required features : 

```
opendaylight-user@root>feature:list -i
Name                                  | Version             | Installed | Repository                                 | Description                                       
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
odl-netconf-connector-all             | 1.0.2-Beryllium-SR2 | x         | odl-controller-1.0.2-Beryllium-SR2         | OpenDaylight :: Netconf Connector :: All          
odl-netconf-connector                 | 1.0.2-Beryllium-SR2 | x         | odl-controller-1.0.2-Beryllium-SR2         | OpenDaylight :: Netconf Connector :: Netconf Conne
odl-netconf-connector-ssh             | 1.0.2-Beryllium-SR2 | x         | odl-controller-1.0.2-Beryllium-SR2         | OpenDaylight :: Netconf Connector :: Netconf Conne
odl-mdsal-binding-base                | 2.0.2-Beryllium-SR2 | x         | odl-yangtools-2.0.2-Beryllium-SR2          | OpenDaylight :: MD-SAL :: Binding Base Concepts   
odl-mdsal-binding-runtime             | 2.0.2-Beryllium-SR2 | x         | odl-yangtools-2.0.2-Beryllium-SR2          | OpenDaylight :: MD-SAL :: Binding Generator       
odl-aaa-api                           | 0.3.2-Beryllium-SR2 | x         | odl-aaa-0.3.2-Beryllium-SR2                | OpenDaylight :: AAA :: APIs                       
odl-aaa-shiro                         | 0.3.2-Beryllium-SR2 | x         | odl-aaa-0.3.2-Beryllium-SR2                | OpenDaylight :: AAA :: Shiro                      
odl-protocol-framework                | 0.7.2-Beryllium-SR2 | x         | odl-protocol-framework-0.7.2-Beryllium-SR2 | OpenDaylight :: Protocol Framework                
pax-jetty                             | 8.1.15.v20140411    | x         | org.ops4j.pax.web-3.1.4                    | Provide Jetty engine support                      
pax-http                              | 3.1.4               | x         | org.ops4j.pax.web-3.1.4                    | Implementation of the OSGI HTTP Service           
pax-http-whiteboard                   | 3.1.4               | x         | org.ops4j.pax.web-3.1.4                    | Provide HTTP Whiteboard pattern support           
pax-war                               | 3.1.4               | x         | org.ops4j.pax.web-3.1.4                    | Provide support of a full WebContainer            
odl-config-persister                  | 0.4.2-Beryllium-SR2 | x         | odl-config-persister-0.4.2-Beryllium-SR2   | OpenDaylight :: Config Persister                  
odl-config-startup                    | 0.4.2-Beryllium-SR2 | x         | odl-config-persister-0.4.2-Beryllium-SR2   | OpenDaylight :: Config Persister:: Config Startup 
odl-config-manager-facade-xml         | 0.4.2-Beryllium-SR2 | x         | odl-config-persister-0.4.2-Beryllium-SR2   | Opendaylight :: Config Persister:: Mapping for Con
standard                              | 3.0.3               | x         | standard-3.0.3                             | Karaf standard feature                            
config                                | 3.0.3               | x         | standard-3.0.3                             | Provide OSGi ConfigAdmin support                  
region                                | 3.0.3               | x         | standard-3.0.3                             | Provide Region Support                            
package                               | 3.0.3               | x         | standard-3.0.3                             | Package commands and mbeans                       
http                                  | 3.0.3               | x         | standard-3.0.3                             | Implementation of the OSGI HTTP Service           
war                                   | 3.0.3               | x         | standard-3.0.3                             | Turn Karaf as a full WebContainer                 
kar                                   | 3.0.3               | x         | standard-3.0.3                             | Provide KAR (KARaf archive) support               
ssh                                   | 3.0.3               | x         | standard-3.0.3                             | Provide a SSHd server on Karaf                    
management                            | 3.0.3               | x         | standard-3.0.3                             | Provide a JMX MBeanServer and a set of MBeans in K
odl-dlux-all                          | 0.3.2-Beryllium-SR2 | x         | odl-dlux-0.3.2-Beryllium-SR2               | Opendaylight dlux all features                    
odl-dlux-core                         | 0.3.2-Beryllium-SR2 | x         | odl-dlux-0.3.2-Beryllium-SR2               | Opendaylight dlux minimal feature                 
odl-dlux-node                         | 0.3.2-Beryllium-SR2 | x         | odl-dlux-0.3.2-Beryllium-SR2               | Enable nodes in Opendaylight dlux                 
odl-dlux-yangui                       | 0.3.2-Beryllium-SR2 | x         | odl-dlux-0.3.2-Beryllium-SR2               | Enable Yang UI in Opendaylight dlux               
odl-dlux-yangvisualizer               | 0.3.2-Beryllium-SR2 | x         | odl-dlux-0.3.2-Beryllium-SR2               | Enable Yang visualizer in Opendaylight dlux       
odl-openflowplugin-all                | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: Openflow Plugin :: All            
odl-openflowplugin-southbound         | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: Openflow Plugin :: SouthBound     
odl-openflowplugin-flow-services      | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: Openflow Plugin :: Flow Services  
odl-openflowplugin-nsf-services       | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: OpenflowPlugin :: NSF :: Services 
odl-openflowplugin-nsf-model          | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: OpenflowPlugin :: NSF :: Model    
odl-openflowplugin-flow-services-rest | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: Openflow Plugin :: Flow Services :
odl-openflowplugin-flow-services-ui   | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: Openflow Plugin :: Flow Services :
odl-openflowplugin-app-config-pusher  | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: Openflow Plugin :: app - default c
odl-openflowplugin-app-lldp-speaker   | 0.2.2-Beryllium-SR2 | x         | openflowplugin-0.2.2-Beryllium-SR2         | OpenDaylight :: Openflow Plugin :: app lldp-speake
odl-aaa-authn                         | 0.3.2-Beryllium-SR2 | x         | odl-aaa-0.3.2-Beryllium-SR2                | OpenDaylight :: AAA :: Authentication - NO CLUSTER
odl-netty                             | 4.0.33.Final        | x         | odlparent-1.6.2-Beryllium-SR2              | OpenDaylight :: Netty                             
odl-lmax                              | 3.3.2               | x         | odlparent-1.6.2-Beryllium-SR2              | OpenDaylight :: LMAX Disruptor                    
odl-config-all                        | 0.4.2-Beryllium-SR2 | x         | odl-config-0.4.2-Beryllium-SR2             | OpenDaylight :: Config :: All                     
odl-config-api                        | 0.4.2-Beryllium-SR2 | x         | odl-config-0.4.2-Beryllium-SR2             | OpenDaylight :: Config :: API                     
odl-config-netty-config-api           | 0.4.2-Beryllium-SR2 | x         | odl-config-0.4.2-Beryllium-SR2             | OpenDaylight :: Config :: Netty Config API        
odl-config-core                       | 0.4.2-Beryllium-SR2 | x         | odl-config-0.4.2-Beryllium-SR2             | OpenDaylight :: Config :: Core                    
odl-config-manager                    | 0.4.2-Beryllium-SR2 | x         | odl-config-0.4.2-Beryllium-SR2             | OpenDaylight :: Config :: Manager                 
odl-config-netty                      | 0.4.2-Beryllium-SR2 | x         | odl-config-persister-0.4.2-Beryllium-SR2   | OpenDaylight :: Config-Netty                      
odl-mdsal-all                         | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              | OpenDaylight :: MDSAL :: All                      
odl-mdsal-common                      | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              | OpenDaylight :: Config :: All                     
odl-mdsal-broker-local                | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              | OpenDaylight :: MDSAL :: Broker                   
odl-toaster                           | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              | OpenDaylight :: Toaster                           
odl-mdsal-xsql                        | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              |                                                   
odl-mdsal-clustering-commons          | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              |                                                   
odl-mdsal-distributed-datastore       | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              |                                                   
odl-mdsal-remoterpc-connector         | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              |                                                   
odl-mdsal-broker                      | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              |                                                   
odl-akka-scala                        | 2.11                | x         | odl-controller-1.6.2-Beryllium-SR2         | Scala Runtime for OpenDaylight                    
odl-akka-system                       | 2.3.14              | x         | odl-controller-1.6.2-Beryllium-SR2         | Akka Actor Framework System Bundles               
odl-akka-clustering                   | 2.3.14              | x         | odl-controller-1.6.2-Beryllium-SR2         | Akka Clustering                                   
odl-akka-leveldb                      | 0.7                 | x         | odl-controller-1.6.2-Beryllium-SR2         | LevelDB                                           
odl-akka-persistence                  | 2.3.14              | x         | odl-controller-1.6.2-Beryllium-SR2         | Akka Persistence                                  
odl-l2switch-switch                   | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: Switch                
odl-l2switch-switch-rest              | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: Switch                
odl-l2switch-switch-ui                | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: Switch                
odl-l2switch-hosttracker              | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: HostTracker           
odl-l2switch-addresstracker           | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: AddressTracker        
odl-l2switch-arphandler               | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: ArpHandler            
odl-l2switch-loopremover              | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: LoopRemover           
odl-l2switch-packethandler            | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: PacketHandler         
odl-mdsal-models                      | 0.8.2-Beryllium-SR2 | x         | odl-mdsal-models-0.8.2-Beryllium-SR2       | OpenDaylight :: MD-SAL :: Models                  
odl-restconf-all                      | 1.3.2-Beryllium-SR2 | x         | odl-controller-1.3.2-Beryllium-SR2         | OpenDaylight :: Restconf :: All                   
odl-restconf                          | 1.3.2-Beryllium-SR2 | x         | odl-controller-1.3.2-Beryllium-SR2         | OpenDaylight :: Restconf                          
odl-restconf-noauth                   | 1.3.2-Beryllium-SR2 | x         | odl-controller-1.3.2-Beryllium-SR2         | OpenDaylight :: Restconf                          
odl-mdsal-apidocs                     | 1.3.2-Beryllium-SR2 | x         | odl-controller-1.3.2-Beryllium-SR2         | OpenDaylight :: MDSAL :: APIDOCS                  
odl-yangpush-api                      | 1.0.2-Beryllium-SR2 | x         | odl-yangpush-1.0.2-Beryllium-SR2           | OpenDaylight :: yangpush :: api                   
odl-yangpush                          | 1.0.2-Beryllium-SR2 | x         | odl-yangpush-1.0.2-Beryllium-SR2           | OpenDaylight :: yangpush                          
odl-netconf-all                       | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: All                    
odl-netconf-api                       | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: API                    
odl-netconf-mapping-api               | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: Mapping API            
odl-netconf-util                      | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            |                                                   
odl-netconf-impl                      | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: Impl                   
odl-config-netconf-connector          | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: Connector              
odl-netconf-netty-util                | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: Netty Util             
odl-netconf-client                    | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: Client                 
odl-netconf-monitoring                | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: Monitoring             
odl-netconf-notifications-api         | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: Notification :: Api    
odl-netconf-notifications-impl        | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: Monitoring :: Impl     
odl-netconf-ssh                       | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf Connector :: SSH          
odl-netconf-tcp                       | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf Connector :: TCP          
odl-aaa-netconf-plugin                | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: AAA :: ODL NETCONF Plugin         
odl-openflowjava-protocol             | 0.7.2-Beryllium-SR2 | x         | odl-openflowjava-0.7.2-Beryllium-SR2       | OpenDaylight :: Openflow Java :: Protocol         
odl-yangtools-yang-data               | 0.8.2-Beryllium-SR2 | x         | odl-yangtools-0.8.2-Beryllium-SR2          | OpenDaylight :: Yangtools :: Data Binding         
odl-yangtools-common                  | 0.8.2-Beryllium-SR2 | x         | odl-yangtools-0.8.2-Beryllium-SR2          | OpenDaylight :: Yangtools :: Common               
odl-yangtools-yang-parser             | 0.8.2-Beryllium-SR2 | x         | odl-yangtools-0.8.2-Beryllium-SR2          | OpenDaylight :: Yangtools :: YANG Parser          

```

#### Now OpenDayLight is up and running with the required functionalities, use the following URL to access DLUX. 

[Link](http://10.11.200.121:8181/index.html)
  ```
  login :  admin
   password : admin 
```


#### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```



.
.
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
