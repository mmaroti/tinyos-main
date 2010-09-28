configuration HalSensirionSht21C {
  provides interface Resource[ uint8_t client ];
  provides interface SensirionSht21[ uint8_t client ];
}
implementation {
  components new SensirionSht21LogicP();
  SensirionSht21 = SensirionSht21LogicP;

  components HplSensirionSht21C;
  Resource = HplSensirionSht21C.Resource;
  
  components new TimerMilliC();
  SensirionSht21LogicP.Timer -> TimerMilliC;   //wiring timer to Timer<TMilli> interface of SensirionSht21LogicP 

}
