generic configuration AlarmMilli32C()
{
  provides interface Alarm<TMilli, uint32_t>;
}
implementation
{
	components AlarmCounterMilliP;
	components new Alarm32khz32C();
	components new TransformAlarmC(TMilli, uint32_t, T32khz, uint32_t,16) as TransformTMilli;

	TransformTMilli.AlarmFrom -> Alarm32khz32C;
	TransformTMilli.Counter -> AlarmCounterMilliP.CounterMilli32;
	Alarm = TransformTMilli;
}
