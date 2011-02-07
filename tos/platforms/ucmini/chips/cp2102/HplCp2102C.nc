configuration HplCp2102C {
  provides interface GpioPCInterrupt as Vdd;
	#if (UCMINI_REV != 49)
		provides interface GpioPCInterrupt as NSuspend;
	#endif

}
implementation {
  components HplAtm128InterruptC;
  
  Vdd=HplAtm128InterruptC.PCInt7;
	#if (UCMINI_REV != 49)
	NSuspend=HplAtm128InterruptC.PCInt5;
	#endif
}