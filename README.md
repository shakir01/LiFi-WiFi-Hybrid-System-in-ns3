# LiFi-in-ns3
We provide Light Fidelity (LiFi) module, implemented in NS3, and integrated with WiFi module of the ns3 to perform scalable hybrid LiFi and WiFi simulations. Complete details including class diagram, documentation and example codes will be provided soon.

But before you execute LiFi module you need to have NS3 installed on your Ubuntu/MAC machine. This work is based on the on ns3.31 and Ubuntu 20.04, however, you can adopt it with minimum modifications to the NS3 and ubuntu versions of your choice. For complete installation and tutorial refere to the relevant sections at the links given below:


Installation guide: https://www.nsnam.org/wiki/Installation 
Tutorials: https://www.nsnam.org/docs/release/3.24/tutorial/ 
Documentations: https://www.nsnam.org/documentation/

LiFi Installation Guide: 

Prerequisites:


Installation:
1.  (If you have not installed already) Install NS3 on a Linux/Unix (MAC)
2.  Clone or download .zip LiFi module to your local machine. Once you clone/download LiFi module you will see "lifi" folder.  
3.  Go to the terminal and create a new module in NS3 based on instructions give at link:https://www.nsnam.org/docs/manual/html/new-modules.html
4.  Once create in Step3 you will find LiFi module (similar in structure to the standard ns3 modules) in the "src" folder.
5.  From you should delete those files as they will be replaced by the files downloaded from the repository. Copy files from cloned/downloaded module to your LiFi module in NS33. Make sure that files are put in their respective directories. 
6. Direct your machine terminal to ns3 top level director (i.e. your terminal control must be outside the "src" and "scratch" folders).
7. Build NS3 using tools such as "waf" and "bake".
8. Your LiFi module is now ready for simulations
