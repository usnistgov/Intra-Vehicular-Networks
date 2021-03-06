# Context                                                             


## SDN for Intra-Vehicular Networks

Vehicular networks nowadays are getting a significant attention from the car manufacturing industries, academia, research institutes, and governments  [1] [2]. As connected objects trends are gaining more momentum thanks to the widespread use of cloud solutions and IoT technologies, not much work has been done to bring intra-vehicular networks into this trend [3].  Intra vehicular networks technologies must ensure a set of requirements some of which are time critical and safety related: Antilock Braking System for example must not fail, experience response delays or lack of resources for obvious safety reasons. Another example is engine control unit that must reflect reliably the engine condition (air-fuel mixture, ignition timing ...) to avoid false positive or false negative alerts. 
Due to the complexity of intra-vehicular networks and the diverse requirements of each functionality, many technologies have been developed, among which: CAN [4]: double wire technology with speeds ranging from 20kbps to 1mbps, this bus technology is widely used for ABS. LIN [5]: single wire technology with low bandwidth (<20kbps) used for Door Locking. FlexRay  [6]is an advanced technology that does the same as CAN but offers more bandwidth. MOST [7] is a fiber technology that is not time sensitive and that can be used for multimedia applications thanks to the large BW (up to 20Mbps) it offers. Bluetooth [8] is an example of wireless intra vehicular network, it is used for external communications, telemetric and tolls.
This contribution is an effort to define an intra-vehicular networking architecture for the next generation of intelligent transportation systems. First, the interoperability and the portability of devices with different make and vendor is provided thru a decent networking infrastructure instead of nowadays “islands” architectures. We believe that vehicular IoT devices should be smoothly replaced as well. However, intra-vehicular networks reveal safety and security constraints because any hijack attempt or malfunction can cause a hazardous situation in the road. This effort promotes a formal verification approach that preserves the critical properties of the system after any change in the network. In fact, Intra-vehicular networks must be resilient and self-adapted to the potential threats and failures. This effort proposes a solution based on Software-Defined Networking (SDN) [ref] that enforces the preservation of the critical properties regardless of possible re-configurations. 
Using such a solution for the next-generation of intelligent transportation systems will lead to a safer, open, and secure vehicle. Moreover, it opens the path to define an interoperability standard for vehicle devices across the multiple vendors and use cases.

### Intra-Vehicular Networks challenges: 

#### Interoperability

Existing intra-vehicular networks leverage a variety of mediums (single wire, twisted pair, optical fiber ...) and protocols (Serial, Parallel, Ethernet, TDMA, CSMA/CR). Network Interoperability is a challenge for interconnecting  and exchanging data among different networks. In such an Intra-vehicular networks, components have to interoperate regardless of the vendor or technology used. The challenge to ensure a safe interoperability among the vehicle’s components is proportional to the heterogeneity of the networks and protocols used. The benefits of  interoperability include but are not limited to the following: (1) Since it is unlikely that manufacturers have the ability to create a totally homogeneous environment, the ability of the vehicle to use the best components available from different vendors based on the performance required and the available budget is very beneficial; (2)The manufacturers benefit as they can sell the same solution to different car vendors as well, and the maintenance operations become less complex as the diagnosis information will be analyzed in a timely fashion. The portability of the solution is an interesting addition as the usability of the technology in different vehicle makes and models is a time saving and a cost effective requirement for both consumers and manufacturers.

#### Resource allocation 

Smart assignment of available resources among priority applications and in an efficient way allows time crucial functionalities to run in a timely manner, and bandwidth consuming applications to run with the best possible performance. Real time applications traffic must be forwarded through low delay paths, especially for critical and safety related applications such as the ABS or the ECU. Infotainment applications or visibility cameras require enough bandwidth to run smoothly and to offer the best possible user experience. The need for a traffic prioritization mechanism is a must to allocate fairly and efficiently network resources among the different functionalities and applications according to a set of requirements and priorities. Network technologies like SDN brings a set of mature QoS technologies and protocols that can help in tackling the resources allocation challenge in intra-vehicular networks. 

#### Verification 

Deploying verified configuration in vehicular networks is necessary especially when it comes to safety critical systems that need to be verified prior to any use. As an example of the impact of an unverified configuration, a Faulty connection between the ECU and the ABS controller can lead to an accident. Formal verification must be brought to the design of any intra vehicular networks and must take into consideration systematic errors: i.e. verifying whether the system is broken by design, and random errors: i.e. verifying whether the system is broken when used in a particular environment. Failure to comply with rigorous requirements of safety can result in harm to people and properties, loss of business and prosecution. Traditional implementation of intra vehicular networks brings isolated sub networks each of which responding to a specific functionality and are designed with well-defined resources that comply with systematic errors verification requirements.  One of the verification related challenges that arises when thinking about resource sharing between different intra vehicular systems is the impact these components will have on one another. The challenge is to verify whether random errors that will show up due to the new environment where components compete over network resources will not compromise safety requirements or principal functions. Safety systems reconfiguration must be verified as well to make sure all the functionalities will be regained when they are subject to systematic or random errors resulting in a component failure.

### SDN Value-added to intra-vehicular networks

SDN can improve the way intra-vehicular networks are implemented and leveraged. SDN programmability allows building southbound APIs for Interoperability between different technologies within a centralized network, or northbound APIs to build customized applications that respond to specific needs. In addition, SDN’s isolation of control plane from data plane makes it more amenable for implementing security and can bring more resiliency to the intra-vehicular networks. 
Resource allocation challenges can be addressed by implementing SDN traffic prioritization capabilities, especially when addressing safety concerns. SDN for intra vehicular networks facilitates the implementation of bump-in-the-wire which can bring a significant improvement to the existing systems.
SDN for intra vehicular networks can enhance the integrity, reliability, and security of existing systems without compromising their critical functions. 

### References

1. S. Tuohy, M. Glavin, C. Hughes, E. Jones, M. Trivedi and L. Kilmartin, "Intra-vehicle networks: A review," IEEE Transactions on Intelligent Transportation Systems, vol. 16, no. IEEE, pp. 534--545, 2015. 
2. DOT, "ITS Strategic Plan 2015-2019," 2015. [Online]. Available: http://www.its.dot.gov/strategicplan/index.html.
3. IEEE, "IEEE COMMUNICATIONS SOCIETY : Software Defined Vehicular Networks: Architectures, Algorithms and Applications," 2016. [Online]. Available: http://www.comsoc.org/commag/cfp/software-defined-vehicular-networks-architectures-algorithms-and-applications.
4.	M. Farsi, K. Ratcliff and M. Barbosa, "An Overview of Controller Area Network," Networking Systems, pp. 113-120, 1999. 
5. M. Ruff, "Evolution of Local Interconnect Network (LIN) Solutions," Vehicular Technology Conference, 2003. VTC 2003-Fall. 2003 IEEE 58th, pp. 3382-3389, 2003. 
6.	"FlexRay Automotive Communication Bus Overview," National Instruments, August 2016. [Online]. Available: http://www.ni.com/white-paper/3352/en/.
7.	M.-Y. Lee, S.-M. Chung and H.-W. Jin, "Automotive network gateway to control electronic units through MOST network," 2010 Digest of Technical Papers International Conference on Consumer Electronics (ICCE), pp. 309-310, 2010. 
8.	M. Ahmed, C. U. Saraydar, T. ElBatt, J. Yin, T. Talty and M. Ames, "Intra-vehicular wireless networks," in 2007 IEEE Globecom Workshops, 2007. 


# Code :

##  The code in the repository "can2ipwrapper" enables Interoperability between different CAN ECUs troughout an ethernet meduim. 

### description of files : 

   #### Interoperability Code  (contributors :  khalid.halba@nist.gov & charif.mahmoudi@nist.gov ) :
   
       
      /can2ipwrapper/IPtoCanwrapper.c       : convert IP frames to CAN frames : CAN+IP sockets. 
      /can2ipwrapper/can2IPwrapper.c        : convert CAN frames to IP frames : CAN+IP sockets. 
      /can2ipwrapper/CANSx.sh               : CAN interface configuration. 
      /can2ipwrapper/Licence-can-utils.md   : licence to reuse / modify files.
      
   #### CAN Libraries and traffic generators  ( Reused Code )  : https://github.com/linux-can/can-utils 
      
      /can2ipwrapper/Licence-can-utils.md   : licence to reuse / modify files 
      /can2ipwrapper/cangen.c               : generate CAN frames. 
      /can2ipwrapper/candump.c              : receive and dump CAN frames.
      /can2ipwrapper/include/linux/*        : CAN libraries 
      
##  The file "infra-setup.md" explains in detail how to run SDN over a wireless network. this is the first step into implementing Software Defined In Vehicule networks over a wireless network. 










