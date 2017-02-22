# Infrastructure Setup 

![Image](https://image.ibb.co/mzFNRF/Untitled.png)


## Setup Edge Nodes ( Pi1 and Pi2 ) as Wireless Access points

### Pi1 and Pi2 :Download hostap + dsnmasq packages 

`sudo apt-get install dnsmasq hostapd`

### Pi1 and Pi2 : Add the following line to the bottom of the file :  /etc/dhcpcd.conf

`denyinterfaces wlan0`

### Pi1 : edit the file /etc/network/interfaces/

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
 
### Pi2 : edit the file /etc/network/interfaces/ 

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

### Edit the Pi1's /etc/hostapd/hostapd.conf

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
### Edit the Pi2's /etc/hostapd/hostapd.conf

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

### Pi1 and Pi2 : Load hostapd config on boot by editing the file : /etc/default/hostapd

`DAEMON_CONF="/etc/hostapd/hostapd.conf"`

### Pi1 : configure DNSMASQ by editing the file : /etc/dnsmasq.conf : 
```
    interface=wlan0
    listen-address=10.11.201.1
    bind-interfaces
    server=8.8.8.8
    domain-needed
    bogus-priv
    dhcp-range=10.11.201.2,10.11.201.10,48h
```

### Pi2 : configure DNSMASQ by editing the file : /etc/dnsmasq.conf : 

```
    interface=wlan0
    listen-address=10.11.202.11
    bind-interfaces
    server=8.8.8.8
    domain-needed
    bogus-priv
    dhcp-range=10.11.202.12,10.11.202.19,48h
```

### Commands to Run Pi1 and Pi2 as access point : 

```
sudo ifdown wlan0
sudo ifup wlan0
sudo service dhcpd restart
sudo service dnsmasq restart
sudo /usr/sbin/hostapd /etc/hostapd/hostapd.conf

```
#  Setup IoT Switch (Pi3)


##   Network configuration

###  Edit the /etc/network/interfaces

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

#### Restart wireless interfaces

Restart wlan1 and wlan2 to get their addresses from Pi1 access point and Pi2 access point respectively

```
sudo ifdown wlan1
sudo ifup wlan1

sudo ifdown wlan2
sudo ifup wlan2
 ```  

### OVS Configuration 

#### install OVS

Install Open VSwitch on the IoT Switch (Pi3)

`sudo apt-get install ovsdbmonitor openvswitch-switch openvswitch-controller openvswitch-pki` 

#### Add Bridges

```
sudo ovs-vsctl add-br br1 
sudo ovs-vsctl add-br br2
 ```  

#### Adding ports

Add ports wlan1 and wlan2 to br1 and br2 respectively
```
sudo ovs-vsctl add-port br1 wlan1
sudo ovs-vsctl add-port br2 wlan2
 ```
#### Assign wireless adresses

Assign wlan1 and wlan2 addresses to OVS local interfaces br1 and br2 respectively ( here Pi1 leased 10.11.201.9/24 to wlan1 and Pi2 leased 10.11.202.12/24 to wlan2 ): 

```
sudo ifconfig wlan1 0
sudo ifconfig br1 10.11.201.9 netmask 255.255.255.0
sudo ifconfig wlan2 0
sudo ifconfig br1 10.11.202.12 netmask 255.255.255.0
```    
    

#### patch interfaces

Create patch interfaces to connect br1 and br2

```
   sudo ovs-vsctl add-port br1 patch1
   sudo ovs-vsctl add-port br2 patch2
   sudo ovs-vsctl set-interface patch1 type=patch
   sudo ovs-vsctl set interface patch1 type=patch
   sudo ovs-vsctl set interface patch2 type=patch
   sudo ovs-vsctl set interface patch1 options:peer=patch2
   sudo ovs-vsctl set interface patch2 options:peer=patch1
   
```


#### Checking the configguration : 

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


#### connect to the controller

Connect br1 and br2 to the OpenDayLight Controller 

```
   sudo ovs-vsctl set-controller br1 tcp:10.11.200.121:6633
   sudo ovs-vsctl set-controller br2 tcp:10.11.200.121:6633
```
# Setup OpenDayLight Controller

## Download packages from OpenDayLight Website : 

`https://www.opendaylight.org/software/downloads/beryllium-sr4`

## Run the following command in the /ODL-Folder/build/ folder : 

`./karaf -Of13`

### Install required features : 

```
opendaylight-user@root>feature:list -i
Name                                  | Version             | Installed | Repository                                 | Description                                       
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
odl-netconf-connector-all             | 1.0.2-Beryllium-SR2 | x         | odl-controller-1.0.2-Beryllium-SR2         | OpenDaylight :: Netconf Connector :: All          
...
odl-dlux-all                          | 0.3.2-Beryllium-SR2 | x         | odl-dlux-0.3.2-Beryllium-SR2               | Opendaylight dlux all features                    
...
odl-config-all                        | 0.4.2-Beryllium-SR2 | x         | odl-config-0.4.2-Beryllium-SR2             | OpenDaylight :: Config :: All                     
...
odl-mdsal-all                         | 1.3.2-Beryllium-SR2 | x         | odl-mdsal-1.3.2-Beryllium-SR2              | OpenDaylight :: MDSAL :: All                      
...
odl-l2switch-switch                   | 0.3.2-Beryllium-SR2 | x         | l2switch-0.3.2-Beryllium-SR2               | OpenDaylight :: L2Switch :: Switch                
...
odl-restconf-all                      | 1.3.2-Beryllium-SR2 | x         | odl-controller-1.3.2-Beryllium-SR2         | OpenDaylight :: Restconf :: All                   
...
odl-netconf-all                       | 1.0.2-Beryllium-SR2 | x         | odl-netconf-1.0.2-Beryllium-SR2            | OpenDaylight :: Netconf :: All                    
...
```

## URL to access DLUX

Now OpenDayLight is up and running with the required functionalities, use the following URL to access DLUX.
[http://10.11.200.121:8181/index.html](http://10.11.200.121:8181/index.html)



```
Use the following credentials to login : 
login :  admin
password : admin 

```
## DLUX Interface 

**TOPLOGY**

![Image](https://image.ibb.co/bUQgmF/Topology_1.png)

** BR1 Ports : openflow_128480596856940**

![Image](https://image.ibb.co/jASBmF/nodes_Node_Id_openflow_128480596856940_11.png)

** BR1 Stats : openflow_128480596856940**

![Image](https://image.ibb.co/dcTTYv/nodes_Node_Connector_Statistics_for_Node_Id_openflow_128480596856940_1.png)

** BR2 Ports : openflow_128480596857414**

![Image](https://image.ibb.co/dMX0fa/nodes_Node_Id_openflow_128480596857414_1.png)

** BR2 Stats : openflow_128480596857414**

![Image](https://image.ibb.co/fX6nRF/nodes_Node_Connector_Statistics_for_Node_Id_openflow_128480596857414_br2.png)

# Setup POSTMAN

Postman helps adding flows in OpenDaylight RestConf REST API.
With Postman, wou can construct request and save them for future use and analyze the responses sent by the  restconf API. 
We will use POSTMAN to add, delete, and update flows using different HTTP methods for RESTful services.

## POSTMAN

Add the POSTMAN ADDON to chrome using the link below and push the launch app button : 

[https://chrome.google.com/webstore/detail/postman/fhbjgbiflinjbdggehcddcbncdddomop?hl=en](https://chrome.google.com/webstore/detail/postman/fhbjgbiflinjbdggehcddcbncdddomop?hl=en)

## Flows using POSTMAN

Adding flows example : 

The example below shows adding a flow with to the IoT Switch with the brdige open flow identifier : openflow:128480596856940 , which corresponds to br1 in the Pi3. 

The NORMAL" action in the flow causes the bridge to behave like a simple MAC learning switch. It allplies to all ports because no in_port was specified and that is like a wildcard for all ports.

METHOD : PUT

Headers : 

  - Accept : application/xml
  - Content Type : application / xml
  - Authorization : Basic Auth. Login : admin / password : admin

URL : [http://10.11.200.121:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:128480596856940/table/0/flow/1](http://10.11.200.121:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:128480596856940/table/0/flow/1)

BODY : 

```XML
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<flow xmlns="urn:opendaylight:flow:inventory">
    <priority>0</priority>
     <hard-timeout>0</hard-timeout>
    <idle-timeout>0</idle-timeout>
    <id>1</id>
    <cookie_mask>0</cookie_mask>
    <cookie>0</cookie>
    <table_id>0</table_id>
    <instructions>
        <instruction>
            <order>0</order>
            <apply-actions>
               <action>
                    <order>0</order>
                    <output-action>
                        <output-node-connector>NORMAL</output-node-connector>
                    </output-action>
                </action>
            </apply-actions>
        </instruction>
    </instructions>
</flow>
```
Feedback : 

A successful request will return the HTTP Success Code 200 OK. 


We can check whether the flow has been added in the br1 switch by issuing the following command : 

Before adding the flow

```
Pi@pi3:~sudo ovs-ofctl dump-flows br1`
NXST_FLOW reply (xid=0x4)
```
After adding the flow


```
Pi@pi3:~sudo ovs-ofctl dump-flows br1`
cookie=0x0,duration=495.149s,table=0, n_packets=1077, n_bytes=110259, idle_age=0, priority=0 actions=NORMAL

```
## Identifying Metadata : 


```XML
<priority>                        
Value is a number between 0 and 65535 inclusive.A higher flow's value will match before a lower one
<hard-timeout>                     
causes the flow entry to be removed after the given number of seconds, regardless of how many packets it has matched
<idle-timeout>0</idle-timeout>    
causes the flow entry to be removed when it has matched no packets in the given number of seconds.
<id>1</id>                         
Flow id according to ODL.
<cookie_mask>                      
Used to perform group operations on flows. This allows more options in how you use cookie
<cookie>                           
opaque data value chosen by the controller. May be used by the controller to filter flow statistics, flow
modification and flow deletion, not used when processing packets. For example, the controller could choose to
modify or delete all flows matching a certain cookie

<table_id>           
Each table has an id.  The flow table is consisted of flow entries, these entries contain the following attributes :  Match fields, Priority, Counters, Instructions,Timeouts, and the cookie. 
<order>                   
order of the action. 
<apply-actions>                    
Applies the specific action immediately, without any change to the Action Set.This instruction may be used to modify the packet between two tables or to execute multiple actions of
the same type. The actions are specified as an action list.
<action>   
Actions describe packet forwarding, packet modification and group table processing. There are several actions that
every OpenFlow Switch is required to support:
  * Output port_no:
  * Group group_id:
  * Drop:
<output-action>   
The Output action forwards a packet to a specified OpenFlow port. OpenFlow switches must
support forwarding to physical ports, switch-defined logical ports and the required reserved ports.
```

## Updating flows example : 

METHOD :  POST

Headers : Same as the PUT request.

URL :     Same as the PUT request. 

BODY :    Updated body , keeping flow id and flow table and cookie. Flow priority is one of the parameters that can be updated.


## deleting flows example: 

Same as PUT command, change PUT to DELETE.


## Getting all active flows on all open_flow bridges from restconf config database : 


METHOD : 

Headers : same as the PUT request. 

URL : [http://10.11.200.121:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:128480596856940/table/0/flow/1](http://10.11.200.121:8181/restconf/config/opendaylight-inventory:nodes/node/openflow:128480596856940/table/0/flow/1)

BODY : Not required. 

Feedback : 

The details of the flows are distplayed in the response body.  

### Experiment to test the infrastructure.

1. Start the Pi1 and Pi2 Access Point.
2. Turn On wlan1 and wlan2 client adapters on Pi3
3. remove address configuration from wlan adapters and assign the same addresses to local OVS switches according to the configuration described before.
4. make sure the
5. ping Pi2 from Pi1 and vice-versa.make sure the pings are successful as seen below : 

**Pi1 : ping to the gateway and to Pi2

![Image](https://image.ibb.co/eTAvwF/ping_Pi1_Success.png)

**Pi2 : ping to the gateway and to Pi1

![Image](https://image.ibb.co/jZZAVa/ping_Pi2_Success.png)


#### 6 Turn on/off the ping by adding/deleting the appropriate flow as described before using POSTMAN.
