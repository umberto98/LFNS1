1) It is best to begin with the quick-start guide of the product you have purchased.

2) The ADMCA software is the display and acquisition software for all Amptek Products. It is located in the ADMCA directory on  
this CD. Only this program is needed to control all Amptek hardware and display the data. Copy this entire directory to your  
C:\ drive, e.g. C:\ADMCA. If you use a USB enabled device (X-123, PX4, DP4, Gamma-Rad) you will be prompted for the USB driver  
when you plug in and turn on the device for the first time. The USB driver is found on this CD in the USB_driver directory. The  
MCA8000A is only an RS232 device and requires no drivers. Once you have copied the software and load the USB software, open  
ADMCA.exe and select the appropriate device from the drop-down menu.

NOTE: You must copy the entire ADMCA directory to your local machine. The software cannot run from CD.

3) If you have purchased the optional XRF-FP Quantitative Analysis Software, then install it from the Quantitative Analysis  
(XRF-FP) directory using the setup.exe file. It is recommended to install the XRF-FP into the same directory that you copied  
the ADMCA software.

3) If you are writing your own code to control the Amptek hardware, then the other directories on this CD will be needed. 

The DPP API directory contains the API/DLL for the digital processor family, DP4/PX4/X-123/Gamma-Rad. If you are writing custom code for these devices start here. There are many examples. The API is for the USB interface only.

The Additional DP4 PX4 Software directory contains advanced programming information for the digital processor family, DP4/PX4/X-123/Gamma-Rad. This includes examples in RS232, Ethernet for the Gamma-Rad product, and how to make the USB calls directly if not using the DPP API. The main example program is in the DP4 VB App directory. This is a comprehensive program written in VB with source code.

The MCA8000A API is the DLL interface for the MCA8000A multichannel analyzer.

The Additional DP5 Software directory is for use with the DP5 product which is only available with Amptek XR100SDD silicon drift detectors.