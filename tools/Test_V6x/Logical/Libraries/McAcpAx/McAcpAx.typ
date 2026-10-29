TYPE
	McAcpAxDataTypeEnum :
	(
		mcACPAX_PARTYPE_BOOL := 1,  (*Data type: Digital information, 1 bit (1 byte)*)
		mcACPAX_PARTYPE_SINT,  (*Data type: Whole number, 1 byte*)
		mcACPAX_PARTYPE_INT,  (*Data type: Whole number, 2 bytes*)
		mcACPAX_PARTYPE_DINT,  (*Data type: Whole number, 4 bytes*)
		mcACPAX_PARTYPE_USINT,  (*Data type: Whole number, 1 byte, positive numbers only*)
		mcACPAX_PARTYPE_UINT,  (*Data type: Whole number, 2 bytes, positive numbers only*)
		mcACPAX_PARTYPE_UDINT,  (*Data type: Whole number, 4 bytes, positive numbers only*)
		mcACPAX_PARTYPE_REAL,  (*Data type: Floating point, 4 bytes*)
		mcACPAX_PARTYPE_DINT_REAL := 64, (*Data type: 4 byte signed integer + 4 byte floating point (a.k.a I4+R4)*)
		mcACPAX_PARTYPE_DINT_REAL_COUNT := 65, (*Data type: 4 byte signed integer + 4 byte floating point + 4 byte unsigned integer for count (a.k.a I4+R4+UI4Cnt)*)
		mcACPAX_PARTYPE_DINT_REAL_TIME := 66, (*Data type: 4 byte signed integer + 4 byte floating point + 4 byte unsigned integer for time stamp (a.k.a I4+R4+UI4Time)*)
		mcACPAX_PARTYPE_VOID := 65535   (*General data type*)
	);

	McAcpAxProcessDataBlockModeEnum :
	(
		mcACPAX_DATA_BLOCK_GET := 0,  (*Read data block*)
		mcACPAX_DATA_BLOCK_SET	 (*Write data block*)
	);

	McAcpAxProcessParIDModeEnum :
	(
		mcACPAX_PARID_GET := 0,  (*Read ParID(s)*)
		mcACPAX_PARID_SET,	 (*Write ParID(s)*)
		mcACPAX_PARID_GET_NO_NCT,  (*Read ParID(s) without entry in the NCT*)
		mcACPAX_PARID_GET_NO_LOG  (*Read ParID(s) without NCT and logger entry. If an error occurs while reading, an entry is still created.*)
	);

	McAcpAxProcessParTabModeEnum :
	(
		mcACPAX_PARTAB_SET := 0	(*Write parameter(s)*)
	);

	McAcpAxCycParIDModeEnum :
	(
		mcACPAX_CYCLIC_PARID_READ := 0,  (*Read ParID(s) cyclically*)
		mcACPAX_CYCLIC_PARID_WRITE  (*Write ParID(s) cyclically*)
	);

	McAcpAxCycParIDRefreshModeEnum :
	(
		mcACPAX_CYCLIC_MULTIPLEXED := 0,  (*Entry in only one data telegram*)
		mcACPAX_CYCLIC_EVERY_RECORD  (*Entry in the current data telegram and every subsequent automatically created data telegram (ParID value is updated every network cycle)*)
	);

	McAcpAxTriggerEnum :
	(
		mcACPAX_TRIGGER_1,	 (*Selects trigger input 1*)
		mcACPAX_TRIGGER_2		 (*Selects trigger input 2*)
	);

	McAcpAxLimitLoadModeEnum :
	(
		mcACPAX_LL_WITH_FEED_FORWARD := 0,  (*The overall torque is limited, i.e. the sum of feed-forward torque and corrective action*)
		mcACPAX_LL_WITHOUT_FEED_FORWARD    (*Only the share of torque that results from control deviations is limited. Feed-forward torque is ignored*)
	);

	McAcpAxLimitLoadParIDModeEnum :
	(
		mcACPAX_LLPM_NO_INIT := 0, (*Value of ParID will not be initialized - default value*)
		mcACPAX_LLPM_INIT_FB_INPUT := 1 (*Value of ParID will be initialized with the value of respective input*)

	);


	McAcpAxBrakeTestCmdEnum :
	(
		mcACPAX_BRAKE_TEST_INIT := 0,		 (*Transfers parameters for holding brake test*)
		mcACPAX_BRAKE_TEST_START := 1,	 (*Starts the holding brake test*)
		mcACPAX_BRAKE_TEST_INIT_START 	 (*Initializes and starts the holding brake test*)
	);

	McAcpAxBrakeTestModeEnum :
	(
		mcACPAX_BRAKE_TEST_STANDARD := 0,		 (*Standard brake test*)
		mcACPAX_BRAKE_TEST_SAFETY			 (*Safe brake test*)
	);

	McAcpAxAxisTypeEnum :
	(
		mcACPAX_AXIS_REAL,		(*Real axis*)
		mcACPAX_AXIS_VIRTUAL,	(*Virtual axis*)
		mcACPAX_AXIS_EXT_ENC	(*External encoder axis*)
	);

	McAcpAxProductFamilyEnum :
	(
		mcACPAX_ACOPOS, (*ACOPOS*)
		mcACPAX_ACOPOS_MULTI,		 (*ACOPOSmulti*)
		mcACPAX_ACOPOS_MICRO,		 (*ACOPOSmicro*)
		mcACPAX_ACOPOS_REMOTE, 	 (*ACOPOSremote*)
		mcACPAX_ACOPOS_MOTOR,		 (*ACOPOSmotor*)
		mcACPAX_ACOPOS_P3,			 (*ACOPOS P3*)
		mcACPAX_ACOPOS_XSB			 (*ACOPOS XSB*)
	);

	McAcpAxModuleTypeEnum :
	(
		mcACPAX_MODULE_INVERTER,				 (*Inverter module*)
		mcACPAX_MODULE_ACTIVE_SUPPLY, 	 (*Active power supply module*)
		mcACPAX_MODULE_PASSIVE_SUPPLY	 (*Passive power supply module*)
	);

	McAcpAxSimulationOnPlcEnum :
	(
		mcACPAX_SIM_STATE_OFF,				 (*Simulation disabled*)
		mcACPAX_SIM_STATE_ON, 				 (*Simulation enabled*)
		mcACPAX_SIM_SET_VALUE_GENERATION, 	 (*Simulation based on target value generation only*)
		mcACPAX_SIM_COMPLETE_CTRL_SYSTEM	 (*Full simulation*)
	);

	McAcpAxCtrlModeEnum :
	(
		mcACPAX_CTRL_POSITION := 1, 		 (*Position control*)
		mcACPAX_CTRL_POSITION_WITH_FF := 33 	 (*Position controller with feed-forward control*)
	);

	McAcpAxCtrlParSelectEnum :
	(
		mcACPAX_CTRL_SELECT_ALL_PAR, 		 (*Default setting; all parameters are transferred*)
		mcACPAX_CTRL_SELECT_POSITION,		 (*Used for selecting the position controller parameters*)
		mcACPAX_CTRL_SELECT_SPEED,			 (*Used for selecting the velocity controller parameters*)
		mcACPAX_CTRL_SELECT_FEED_FORWARD,	 (*Used for selecting the feed-forward control parameters*)
		mcACPAX_CTRL_SELECT_ADV_PAR_ONLY	 (*Used for selecting advanced parameters (if written)*)
	);

	McAcpAxLoopFilterTypeEnum :
	(
		mcACPAX_LOOP_FILTER_NO_TRANSFER, 	 (*Filter parameter not transferred*)
		mcACPAX_LOOP_FILTER_OFF, 			 (*Filter switched off*)
		mcACPAX_LOOP_FILTER_LOWPASS, 		 (*Low-pass filter*)
		mcACPAX_LOOP_FILTER_NOTCH,			 (*Notch*)
		mcACPAX_LOOP_FILTER_Z_TRANS_FUN,	 (*Z-transfer function*)
		mcACPAX_LOOP_FILTER_COMPENSATION,	 (*Compensation*)
		mcACPAX_LOOP_FILTER_BIQUAD,			 (*Biquad filter*)
		mcACPAX_LOOP_FILTER_LIM,			 (*Limits*)
		mcACPAX_LOOP_FILTER_LIM_LINEAR,		 (*Linear limit*)
		mcACPAX_LOOP_FILTER_LIM_R_TIME 		 (*Rise limit*)
	);

	McAcpAxAutoTuneExSignalEnum :
	(
		mcACPAX_EX_SIGNAL_PRBS := 0, 		 (*Standard*)
		mcACPAX_EX_SIGNAL_CHIRP := 1, 		 (*Chirp (linear frequency modulation)*)
		mcACPAX_EX_SIGNAL_CHIRP_TRAPEZ := 2  (*Chirp (linear frequency modulation) with trapezoidal amplitude response*)
	);

	McAcpAxAutoTuneTestModeEnum:
	(
		mcACPAX_TEST,  (*Controller test*)
		mcACPAX_TEST_POSITION,  (*Controller test (position controller)*)
		mcACPAX_TEST_SPEED  (*Controller test (rotary speed controller)*)
	);

	McAcpAxIntegrationTimeModeEnum:
	(
		mcACPAX_INTEGRATION_TIME_IGNORE,  (*Integral action time is neither taken into account nor calculated*)
		mcACPAX_INTEGRATION_TIME_USE,  (*Integral action time is taken into account for autotuning*)
		mcACPAX_INTEGRATION_TIME_TUNE  (*Integral action time is calculated for autotuning*)
	);

	McAcpAxAutoTuneOperatPointEnum:
	(
		mcACPAX_OP_TUNE_STANDSTILL,  (*Autotuning at standstill*)
		mcACPAX_OP_TUNE_V_CONSTANT  (*Autotuning at constant velocity*)
	);

	McAcpAxAutoTuneMotorModeEnum:
	(
		mcACPAX_ATM_IDENTIFICATION := 10,	(*Identification of the parameter on the drive*)
		mcACPAX_ATM_TEST := 12 				(*Test of the motor to detect aging*)
	);

	 McAcpAxLoadModelIdentModeEnum:
	(
		 mcACPAX_MODEL_IDENT_CLOSED_LOOP := 0, (*Model identification in closed loop, using excitation signal with potentially variable amplitude.*)
		 mcACPAX_MODEL_IDENT_OPEN_LOOP := 1 (*Model identification in open loop, using excitation signal with constant amplitude.*)
	);

	McAcpAxAutoTuneMotPhasModeEnum:
	(
		mcACPAX_ATMP_SATURATION := 30, (*Saturation*)
		mcACPAX_ATMP_STEPPER := 31,    (*Stepper*)
		mcACPAX_ATMP_DITHER := 32,     (*Dither*)
		mcACPAX_ATMP_SET_OFFSET := 34  (*Set commutation offset*)
	);

	McAcpAxSimulationModeEnum :
	(
		mcACPAX_SIMULATION_1MASS_AUTO,	 (*Standard (1-mass model, parameters determined automatically)*)
		mcACPAX_SIMULATION_1MASS,		 (*1-mass load model*)
		mcACPAX_SIMULATION_2MASS,		 (*2-mass load model*)
		mcACPAX_SIMULATION_SET_GEN_ONLY		 (*set value generation only*)
	);

	McAcpAxFeedbackModeEnum:
	(
		 mcACPAX_FBCTRL_MODE_STANDARD := 0, (*Standard: ncSTANDARD*)
		 mcACPAX_FBCTRL_MODE_1MASS_MODEL := 4, (*One mass load model: ncMODEL_1MASS*)
		 mcACPAX_FBCTRL_MODE_2MASS_MODEL := 3, (*Two mass load model: ncMODEL_2MASS*)
		 mcACPAX_FBCTRL_MODE_2ENC_SPEED := 5 (*Two encoder speed: nc2ENCOD_SPEED*)
	);

 	McAcpAxSendChannelEnum :
	(
		mcACPAX_SEND_CHANNEL_AUTO := 0, (*Select channel automatically*)
		mcACPAX_SEND_CHANNEL_1 := 1, (*Select channel 1*)
		mcACPAX_SEND_CHANNEL_2 := 2, (*Select channel 2*)
		mcACPAX_SEND_CHANNEL_3 := 3 (*Select channel 3*)
	);

	McAcpAxReceiveChannelEnum :
	(
		mcACPAX_RECEIVE_CHANNEL_AUTO := 0, (*Select channel automatically*)
		mcACPAX_RECEIVE_CHANNEL_1 := 1, (*Select channel 1*)
		mcACPAX_RECEIVE_CHANNEL_2 := 2, (*Select channel 2*)
		mcACPAX_RECEIVE_CHANNEL_3 := 3, (*Select channel 3*)
		mcACPAX_RECEIVE_CHANNEL_4 := 4, (*Select channel 4*)
		mcACPAX_RECEIVE_CHANNEL_5 := 5 (*Select channel 5*)
	);

	McAcpAxSctrlLimitLoadModeEnum :
	(
		mcACPAX_SLL_LIMIT_AND_REPORT := 0  (*Limit load (torque) and report limitation status.*)
	);

	McAcpAxVibCtrlStatusEnum :
	(
		mcACPAX_VIBCRL_OFF := 0, (*Vibration control is not active*)
		mcACPAX_VIBCTRL_IDLE := 1, (*Vibration control is idle*)
		mcACPAX_VIBCTRL_BOOST := 2, (*Vibration control boost*)
		mcACPAX_VIBCTRL_OPERATIONAL := 3, (*Vibration control is operational *)
		mcACPAX_VIBCTRL_BRAKE := 4 (*Vibration control brake*)
	);


	McAcpAxHomingAddTorqLimParType : STRUCT
		PositiveDirection : REAL; (*Positive torque limit value for homing to blocks. If '0.0' is specified, the value of 'TorqueLimit' is used for positive direction. [Nm]*)
		NegativeDirection : REAL; (*Negative torque limit value for homing to blocks. If '0.0' is specified, the value of 'TorqueLimit' is used for negative direction. [Nm]*)
	END_STRUCT;

   	McAcpAxHomingParType : STRUCT
        HomingMode : McHomingModeEnum; (*Mode for homing*)
		Position : LREAL; (*Absolute position or homing offset when homing signal [Measurement units] occurs*)
		StartVelocity : REAL; (*Velocity for reference switch search [Measurement units/s]*)
		HomingVelocity : REAL; (*Velocity (after reaching reference switch) [Measurement units/s]*)
		Acceleration : REAL; (*Maximum acceleration [Measurement units/s²]*)
		SwitchEdge : McDirectionEnum; (*Edge of reference switch*)
		StartDirection : McDirectionEnum; (*Start direction for searching the reference edge*)
		HomingDirection : McDirectionEnum; (*Direction for homing (after reaching reference switch)*)
		ReferencePulse : McSwitchEnum; (*The encoder's reference pulse is used for homing*)
		KeepDirection : McSwitchEnum; (*The direction of movement is or is not permitted to be changed during the homing procedure*)
		ReferencePulseBlockingDistance : REAL; (*Distance for blocking activation of "triggering reference pulse" [Measurement units]*)
		TorqueLimit : REAL; (*Torque limit value for homing to blocks [Nm]*)
		BlockDetectionPositionError : REAL; (*Lag error for block detection [Measurement units]*)
		PositionErrorStopLimit : REAL; (*Lag error for canceling homing procedure [Measurement units]*)
		RestorePositionVariableAddress : UDINT; (*Address of a remanent variable of type McAcpAxRestorePosType that is needed for "HomingMode" mcHOMING_RESTORE_POSITION*)
		AdditionalTorqueLimit : McAcpAxHomingAddTorqLimParType; (*Additional, direction dependent torque limit values for homing to block*)
	END_STRUCT;

	McAcpAxProcessParIDType : STRUCT
		ParID : UINT; (*Parameter ID number to be read or written*)
		VariableAddress : UDINT; (*Address of the variable that receives the read value or the corresponding value that is to be written*)
		DataType : McAcpAxDataTypeEnum; (*Data type of the variable:*)
	END_STRUCT;

	McAcpAxProcessParTabDataType : STRUCT
		DataObjectName : STRING[12]; (*Name of the ACOPOS parameter table data object*)
	END_STRUCT;

	McAcpAxProcessParTabAddInfoType : STRUCT
		NumberOfParameters : UDINT; (*Number of transferred parameters*)
	END_STRUCT;

	McAcpAxCycParIDType : STRUCT
		ParID : UINT; (*Parameter ID number to be read or written*)
		VariableAddress : UDINT; (*Address of the variable that receives the read value or the corresponding value that is to be written*)
		DataType : McAcpAxDataTypeEnum; (*Data type of the variable:*)
		RefreshMode : McAcpAxCycParIDRefreshModeEnum; (*Defines how often a ParID should be read/written*)
	END_STRUCT;

	McAcpAxTriggerStopType : STRUCT
		TriggerSource : McAcpAxTriggerEnum; (*Source of the trigger event*)
		TriggerEdge : McEdgeEnum; (*Selection of the edge for the trigger event*)
		TriggerDistance : LREAL; (*Distance after the trigger event occurs [Measurement units]*)
		ForceTriggerDistance : McSwitchEnum; (*Forces the trigger distance to be traveled even if it exceeds the end position*)
	END_STRUCT;

	McAcpAxAdvLimitLoadParType : STRUCT
		LoadPosAccelParID : UINT; (*ParID with limit value for accelerating torque in the positive direction*)
		LoadPosDecelParID : UINT; (*ParID with limit value for decelerating torque in the positive direction*)
		LoadNegAccelParID : UINT; (*ParID with limit value for accelerating torque in the negative direction*)
		LoadNegDecelParID : UINT; (*ParID with limit value for decelerating torque in the negative direction*)
		LoadParIDMode : McAcpAxLimitLoadParIDModeEnum; (*Mode which defines if the ParID is initialized with the respective input value*)
		StopMode : McLimitLoadStopModeEnum; (*Mode defines how and if limits are switched when movement is aborted*)
		StopTorque : REAL; (*If Stop mode is mcLLSM_USER_DEFINED, switch over to limit value contained in StopTorque is performed*)
	END_STRUCT;

	McAcpAxBrakeParType : STRUCT
		AutomaticControl : McSwitchEnum := mcSWITCH_ON; (*Automatic control on/off (Default setting:*)
		RestrictedBrakeControl : McSwitchEnum := mcSWITCH_ON; (*Holding brake can only be applied and released (Default setting:*)
		ControlMonitoring : McSwitchEnum := mcSWITCH_ON; (*Enables/disables control monitoring (Default setting:*)
		MovementMonitoring : McSwitchEnum := mcSWITCH_ON; (*Enables/disables movement monitoring (Default setting:*)
		VoltageMonitoring : McSwitchEnum := mcSWITCH_ON; (*Enables/disables monitoring of external voltage over 24 V (Default setting:*)
		TestAtPowerOn : McSwitchEnum := mcSWITCH_OFF; (*Enables/disables automatic torque testing when the controller is switched on (Default setting:*)
		TestAtPowerOff : McSwitchEnum := mcSWITCH_OFF; (*Enables/disables automatic torque testing when the controller is switched off (Default setting:*)
		AutomaticInductionStop : McSwitchEnum := mcSWITCH_OFF; (*Enables/disables automatic induction stop (Default setting:*)
		EnableSBTRequestBySMC : McSwitchEnum := mcSWITCH_OFF; (*Enables the automatic safe brake test requested and monitored by module SafeMC (Default setting:*)
		ControlMonitoringFilterTime : REAL := 0.5; (*Time after which an error is reported after control monitoring is enabled. [s] (Default setting: 0.5)*)
	END_STRUCT;

	McAcpAxBrakeTestParType : STRUCT
		Mode : McAcpAxBrakeTestModeEnum; (*Mode for brake test*)
		Torque : REAL; (*Torque for holding brake test [Nm]*)
		Duration : REAL; (*Duration of holding brake test [s]*)
		PositionLimit : LREAL; (*Position error limit for holding brake test [Measurement units]*)
	END_STRUCT;

	McAcpAxSimulationMass1Type : STRUCT
		Inertia : REAL; (*Inertia [kgm²]*)
		StaticFriction : REAL; (*Static friction [Nm]*)
		ViscousFriction : REAL; (*Viscous friction*)
	END_STRUCT;

	McAcpAxSimulationMass2Type : STRUCT
		Inertia : REAL; (*Inertia [kgm²]*)
		StaticFriction : REAL; (*Static friction [Nm]*)
		ViscousFriction : REAL; (*Viscous friction*)
		Stiffness : REAL; (*Stiffness to coupled mass 1 [Nm]*)
		Damping : REAL; (*Damping to coupled mass 1 [Nms]*)
	END_STRUCT;

	McAcpAxSimulationParType : STRUCT
		Mode : McAcpAxSimulationModeEnum; (*Mode for load simulation on the drive*)
		AdditiveLoadParID : UINT; (*Parameter ID for applying an additional load*)
		Mass1 :  McAcpAxSimulationMass1Type; (*Parameter for determining the first mass*)
		Mass2 :  McAcpAxSimulationMass2Type; (*Parameter for determining the second mass*)
	END_STRUCT;

	McAcpAxRestorePosType: STRUCT
		Data : ARRAY[0..17] OF DINT; (*Data for restoring the position*)
	END_STRUCT;

	McAcpAxAxisInfoType : STRUCT
		AxisType : McAcpAxAxisTypeEnum; (*Axis type*)
		ChannelNumber : UDINT; (*Channel number of the axis*)
		AcoposSimulationOnPlcMode : McAcpAxSimulationOnPlcEnum; (*Information about the ACOPOS drive simulation mode that is enabled on the controller for this axis*)
	END_STRUCT;

	McAcpAxModuleInfoType : STRUCT
		ProductFamily : McAcpAxProductFamilyEnum; (*Product family*)
		ModuleType : McAcpAxModuleTypeEnum; (*Module type*)
		NetworkType : McNetworkTypeEnum; (*Network type*)
		NodeNumber : UDINT; (*Node number*)
		AcoposSimulationOnPlc : McAcpAxSimulationOnPlcEnum; (*Information about whether ACOPOS drive simulation is enabled on the controller for this module*)
	END_STRUCT;

	McAcpAxPosCtrlParType : STRUCT
		ProportionalGain : REAL; (*Proportional gain [1/s]*)
		IntegrationTime : REAL; (*Integral action time of integral component [s]*)
		PredictionTime : REAL; (*Prediction time [s]*)
		TotalDelayTime : REAL; (*Total delay [s]*)
	END_STRUCT;

	McAcpAxSpeedCtrlParType : STRUCT
		ProportionalGain : REAL; (*Proportional gain [As/Rev.]*)
		IntegrationTime : REAL; (*Integral action time of integral component [s]*)
		FilterTime : REAL; (*Filter time constant [s]*)
	END_STRUCT;

	McAcpAxFeedForwardParType : STRUCT
		TorqueLoad : REAL; (*Load torque [Nm}*)
		TorquePositive : REAL; (*Torque in positive direction [Nm]*)
		TorqueNegative : REAL; (*Torque in negative direction [Nm]*)
		SpeedTorqueFactor : REAL; (*Velocity torque factor [Nms]*)
		Inertia : REAL; (*Moment of inertia [kgm²]*)
		AccelerationFilterTime : REAL; (*Acceleration filter time constant [s]*)
	END_STRUCT;

	McAcpAxLoopFilterParType : STRUCT
		Type : McAcpAxLoopFilterTypeEnum; (*Loop filter type used*)
		LowPass : McAcpAxLoopFilterLowPassType; (*Parameter for low pass*)
		Notch : McAcpAxLoopFilterNotchType; (*Parameter for band-stop filter*)
		ZTransferFunction : McAcpAxLoopFilterZTransFunType; (*Parameter for z-transfer function*)
		Compensation : McAcpAxLoopFilterCompType; (*Parameter for compensation*)
		Biquad : McAcpAxLoopFilterBiquadType; (*Parameter for biquad filter*)
		Limiter : McAcpAxLoopFilterLimType; (*Parameter for limit*)
		LimiterLinear : McAcpAxLoopFilterLimLinearType; (*Parameter for linear limit*)
		LimiterRiseTime : McAcpAxLoopFilterLimRiseTimeType; (*Parameter for rise limit*)
	END_STRUCT;

	McAcpAxLoopFilterLowPassType : STRUCT
		CutOffFrequency : REAL; (*Low pass: Limit frequency [Hz]*)
	END_STRUCT;

	McAcpAxLoopFilterNotchType : STRUCT
		CenterFrequency : REAL; (*Band-stop: Center frequency [Hz]*)
		Bandwidth : REAL; (*Band-stop: bandwidth [Hz]*)
	END_STRUCT;

	McAcpAxLoopFilterZTransFunType: STRUCT
		A0 : REAL; (*Coefficient a0 of the denominator polynomial*)
		A1 : REAL; (*Coefficient a1 of the denominator polynomial*)
		B0 : REAL; (*Coefficient b0 of the numerator polynomial*)
		B1 : REAL; (*Coefficient b1 of the numerator polynomial*)
		B2 : REAL; (*Coefficient b2 of the numerator polynomial*)
	END_STRUCT;

	McAcpAxLoopFilterCompType : STRUCT
		MultiplicationFactorParID : UINT; (*ACOPOS drive parameter ID for multiplication point*)
		AdditiveValueParID : UINT; (*ACOPOS drive parameter ID for addition point*)
	END_STRUCT;

	McAcpAxLoopFilterBiquadType : STRUCT
		FrequencyNumerator : REAL; (*Biquad filter: Known frequency counter [Hz]*)
		DampingNumerator : REAL; (*Biquad filter: Damping counter*)
		FrequencyDenominator : REAL; (*Biquad filter: Known frequency denominator [Hz]*)
		DampingDenominator : REAL; (*Biquad filter: Damping denominator*)
	END_STRUCT;

	McAcpAxLoopFilterLimType : STRUCT
		PositiveLimit : REAL; (*Positive limit [A]*)
		NegativeLimit : REAL; (*Negative limit [A]*)
		PositiveLimitParID : UINT; (*ACOPOS drive parameter ID for the magnitude of the positive limit*)
		NegativeLimitParID : UINT; (*ACOPOS drive parameter ID for the magnitude of the negative limit*)
	END_STRUCT;

	McAcpAxLoopFilterLimLinearType : STRUCT
		InputParID : UINT; (*ACOPOS drive parameter ID for the function input*)
		InputLimit : REAL; (*Input limit value for complete limit [Hz]*)
		Gradient : REAL; (*Slope [As]*)
	END_STRUCT;

	McAcpAxLoopFilterLimRiseTimeType : STRUCT
		RiseTime : REAL; (*Rise time [s]*)
		NormalizedLimit : REAL; (*Normalized limit*)
	END_STRUCT;

	McAcpAxFeedbackParType : STRUCT
		SpeedMixRatio : REAL; (*Speed mixing ratio.*)
		SpeedProportionalGain : REAL; (*Speed proportional gain [As].*)
	END_STRUCT;

	McAcpAxAutoTuneExSignalType: STRUCT
		SignalType:  McAcpAxAutoTuneExSignalEnum; (*Type of excitation signal*)
		SignalOrder : UDINT := 9; (*Order of the excitation signal (only for signal type PRBS)*)
		DelayTime : REAL; (*Delay time for transient operations [s] (only for signal type PRBS)*)
		SignalStartFrequency : REAL; (*Starting frequency of the excitation signal [Hz] (only for chirp signal types)*)
		SignalStopFrequency : REAL; (*Stopping frequency of the excitation signal [Hz] (only for chirp signal types)*)
		SignalTime : REAL; (*Duration of the excitation signal [s] (only for chirp signal types)*)
	END_STRUCT;

	McAcpAxAutoTuneParType : STRUCT
		Orientation : McAcpAxAutoTuneOrientationEnum; (*Selects the orientation for autotuning*)
		MaxCurrentPercent : REAL; (*Percentage of the rated current that is used during autotuning [%]*)
		MaxDistance : LREAL; (*Maximum distance traveled during autotuning [Measurement units]*)
		MaxPositionError : LREAL; (*Maximum permitted lag error during autotuning[Measurement units]*)
	END_STRUCT;

	McAcpAxAutoTuneSpeedCtrlOutType : STRUCT
		Quality : REAL; (*Quality of parameter identification [%]*)
		EstimatedInertia : REAL; (*Estimated drive inertia [kg/m*)
		ProportionalGain : REAL; (*Estimated proportional gain factor [As]*)
		IntegrationTime : REAL; (*Estimated integral action time [s]*)
		FilterTime : REAL; (*Filter time constant [s]*)
		LoopFilter1 : McAcpAxLoopFilterParType; (*LoopFilter1 settings*)
		PhaseCrossoverFrequency : REAL; (*Phase crossover frequency of the controlled system [Hz]*)
		Feedback : McAcpAxFeedbackParType; (*Feedback parameters for one mass model, two mass model and two encoder speed feedback mode*)
		Parameters : McCfgAcpCtrlType; (*Parameter structure for usage on MC_BR_ProcessConfig and MC_BR_ProcessParam*)
	END_STRUCT;

	McAcpAxAutoTuneLoopFilterOutType : STRUCT
		Quality : REAL; (*Quality of parameter identification [%]*)
		LoopFilter1 : McAcpAxLoopFilterParType; (*Parameter for first control loop filter*)
		LoopFilter2 : McAcpAxLoopFilterParType; (*Parameter for additional control loop filter*)
		LoopFilter3 : McAcpAxLoopFilterParType; (*Parameter for additional control loop filter*)
		Parameters : McCfgAcpCtrlType; (*Parameter structure for usage on MC_BR_ProcessConfig and MC_BR_ProcessParam*)
	END_STRUCT;

	McAcpAxAutoTunePosCtrlOutType : STRUCT
		Quality : REAL; (*Quality of parameter identification [%]*)
		ProportionalGain : REAL; (*Estimated proportional gain factor [As/U]*)
		Parameters : McCfgAcpCtrlType; (*Parameter structure for usage on MC_BR_ProcessConfig and MC_BR_ProcessParam*)
	END_STRUCT;

	McAcpAxAutoTuneTestOutType : STRUCT
		Quality : REAL; (*Quality of parameter identification [%]*)
	END_STRUCT;

	McAcpAxAutoTuneLoopFiltersType : STRUCT
		LoopFilter1Mode : McAcpAxLoopFilterModeEnum := mcACPAX_LOOP_FILTER_IGNORE; (*Mode for loop filter tuning:*)
		LoopFilter2Mode : McAcpAxLoopFilterModeEnum := mcACPAX_LOOP_FILTER_IGNORE; (*Mode for loop filter tuning:*)
		LoopFilter3Mode : McAcpAxLoopFilterModeEnum := mcACPAX_LOOP_FILTER_IGNORE; (*Mode for loop filter tuning:*)
	END_STRUCT;

	McAcpAxAdvAutoTuneSpeedCtrlType : STRUCT
		FeedbackMode : McAcpAxFeedbackModeEnum; (*Defines the controller feedback mode during the tuning process.*)
	    LoopFilter1Mode : McAcpAxLoopFilterModeEnum; (*Mode for taking LoopFilter1 into account*)
	    FilterTimeMode : McAcpAxFilterTimeModeEnum; (*Mode for taking the filter time constant into account*)
	    IntegrationTimeMode : McAcpAxIntegrationTimeModeEnum; (*Mode for taking integral action time into account*)
	    OperatingPoint : McAcpAxAutoTuneOperatPointEnum; (*Selects the operating point for autotuning*)
	    Velocity : REAL; (*Note: Not used, should be deprecated.*)
	    MaxVelocityPercent : REAL := 50.0; (*Maximum velocity, in percent of the the axis velocity limit, applied during autotuning if "OperatingPoint" = mcACPAX_OP_TUNE_V_CONSTANT. [%]*)
	    Acceleration : REAL; (*Acceleration applied during autotuning if "OperatingPoint" = mcACPAX_OP_TUNE_V_CONSTANT. [Measurement units / s^2]*)
	    MaxProportionalGain : REAL := 2000; (*Maximum proportional gain [As]*)
	    ProportionalGainPercent : REAL := 100; (*Percentage of the proportional gain determined during autotuning that will be used for the control parameters [%]*)
	    ResonanceFactor : REAL := 2; (*Factor for detecting resonance*)
	    InertiaEstimationLowerFrequency  : REAL := 10; (*Lower frequency for estimating the mass moment of inertia of the drive [Hz]*)
	    InertiaEstimationUpperFrequency  : REAL := 40; (*Upper frequency for estimating the mass moment of inertia of the drive [Hz]*)
	    ExcitationSignal : McAcpAxAutoTuneExSignalType; (*Parameter for excitation signal*)
	    LoadModel : McAcpAxLoadModelType; (*Load model parameters required for autotuning in one mass, two mass and two encoder speed feedback mode.*)
	END_STRUCT;

	McAcpAxAdvAutoTuneLoopFilterType : STRUCT
	    OperatingPoint : McAcpAxAutoTuneOperatPointEnum; (*Selects the operating point for autotuning*)
	    Velocity : REAL; (*Maximum velocity applied during autotuning if "OperatingPoint =*)
	    Acceleration : REAL; (*Acceleration applied during autotuning if "OperatingPoint =*)
	    ResonanceFactor : REAL := 2; (*Factor for detecting resonance*)
	    ExcitationSignal : McAcpAxAutoTuneExSignalType; (*Parameter for excitation signal*)
	END_STRUCT;

	McAcpAxAdvAutoTunePosCtrlType : STRUCT
	    OperatingPoint : McAcpAxAutoTuneOperatPointEnum; (*Selects the operating point for autotuning*)
	    Velocity : REAL; (*Maximum velocity applied during autotuning if "OperatingPoint =*)
	    Acceleration : REAL; (*Acceleration applied during autotuning if "OperatingPoint =*)
	    MaxProportionalGain : REAL := 2000; (*Maximum proportional gain [As]*)
	    ProportionalGainPercent : REAL := 100; (*Percentage of the proportional gain determined during autotuning that will be used for the control parameters [%]*)
	    ExcitationSignal : McAcpAxAutoTuneExSignalType; (*Parameter for excitation signal*)
	END_STRUCT;

	McAcpAxAdvAutoTuneTestType : STRUCT
	    ExcitationSignal : McAcpAxAutoTuneExSignalType; (*Parameter for excitation signal*)
	END_STRUCT;

	McAcpAxAutoTuneIndMotParType : STRUCT
		NominalVoltage : REAL; (*Nominal voltage (RMS value, phase-phase) [V]*)
		NominalCurrent : REAL; (*Phase current for generating the nominal torque at nominal speed (RMS value) [A]*)
		NominalSpeed : REAL;  (*Nominal speed [RPM]*)
		NominalFrequency : REAL; (*Nominal frequency [Hz]*)
		PowerFactor : REAL; (*Power factor (cos phi)*)
		ThermalTrippingTime : REAL; (*Tripping time for thermal overload [s]*)
	END_STRUCT;

	McAcpAxAdvAutoTuneIndMotType : STRUCT
		Phase : USINT; (*Motor phase the setup is executed with (1=U, 2=V, 3=W*)
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		MaximumSpeed : REAL; (*Maximal speed [RPM]*)
		StallTorque : REAL; (*Torque at standstill [Nm]*)
		NominalTorque : REAL; (*Nominal torque [Nm]*)
		PeakTorque : REAL; (*Peak torque [Nm]*)
		StallCurrent : REAL; (*Current at standstill [A]*)
		PeakCurrent : REAL; (*Peak current [A]*)
		MagnetizingCurrent : REAL; (*Magnetizing current [A]*)
		WindingCrossSection : REAL; (*Cunductor cross section of a phase [mm^2]*)
		InverterCharacteristicGain : REAL; (*Inverter characteristic curve: Gain factor*)
		InverterCharacteristicExponent : REAL; (*Inverter characteristic curve: Exponent [1/A]*)
	END_STRUCT;

	McAcpAxAutoTuneIndMotOutType : STRUCT
		Quality : REAL;
		Parameters : McCfgMotInductType; (*Parameter structure for usage on MC_BR_ProcessConfig*)
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		MaximumSpeed : REAL; (*Maximal speed [RPM]*)
		StallTorque : REAL; (*Torque at standstill [Nm]*)
		NominalTorque : REAL; (*Nominal torque [Nm]*)
		PeakTorque : REAL; (*Peak torque [Nm]*)
		StallCurrent : REAL; (*Current at standstill [A]*)
		PeakCurrent : REAL; (*Peak current [A]*)
		WindingCrossSection : REAL; (*Cunductor cross section of a phase [mm^2]*)
		StatorResistance : REAL; (*Stator resistance (phase) [Ohm]*)
		StatorInductance : REAL; (*Stator leakage inductance (phase) [mH]*)
		RotorResistance : REAL; (*Rotor resistance (phase) [Ohm]*)
		RotorInductance : REAL; (*Rotor leakage inductance (phase) [mH]*)
		MutualInductance : REAL; (*Mutual inductance (phase) [mH]*)
		MagnetizingCurrent : REAL; (*Magnetizing current [A]*)
	END_STRUCT;

	McAcpAxAutoTuneSyncMotParType : STRUCT
		NominalVoltage : REAL; (*Nominal voltage (RMS value, phase-phase) [V]*)
		NominalCurrent : REAL; (*Phase current for generating the nominal torque at nominal speed (RMS value) [A]*)
		NominalSpeed : REAL;  (*Nominal speed [RPM]*)
		NominalTorque : REAL; (*Nominal torque [Nm]*)
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		PeakCurrent : REAL; (*Peak current [A]*)
		PeakTorque : REAL; (*Peak torque [NM]*)
		ThermalTrippingTime : REAL; (*Tripping time for thermal overload [s]*)
	END_STRUCT;

	McAcpAxAdvAutoTuneSyncMotType : STRUCT
		Phase : USINT; (*Motor phase the setup is executed with (1=U, 2=V, 3=W*)
		VoltageConstant : REAL; (*Voltage constant [mVmin]*)
		MaximumSpeed : REAL; (*Maximal speed [RPM]*)
		StallTorque : REAL; (*Torque at standstill [Nm]*)
		TorqueConstant : REAL; (*Torque constant [Nm/A]*)
		StallCurrent : REAL; (*Current at standstill [A]*)
		WindingCrossSection : REAL; (*Cunductor cross section of a phase [mm^2]*)
		InverterCharacteristicGain : REAL; (*Inverter characteristic curve: Gain factor*)
		InverterCharacteristicExponent : REAL; (*Inverter characteristic curve: Exponent [1/A]*)
	END_STRUCT;

	McAcpAxAutoTuneSyncMotOutType : STRUCT
		Quality : REAL;
		Parameters : McCfgMotSynType; (*Parameter structure for usage on MC_BR_ProcessConfig*)
		VoltageConstant : REAL; (*Voltage constant [mVmin]*)
		MaximumSpeed : REAL; (*Maximal speed [RPM]*)
		StallTorque : REAL; (*Torque at standstill [Nm]*)
		TorqueConstant : REAL; (*Torque constant [Nm/A]*)
		StallCurrent : REAL; (*Current at standstill [A]*)
		WindingCrossSection : REAL; (*Cunductor cross section of a phase [mm^2]*)
		StatorResistance : REAL; (*Stator resistance (phase) [Ohm]*)
		StatorInductance : REAL; (*Stator leakage inductance (phase) [mH]*)
	END_STRUCT;

	McAcpAxAutoTuneMotPhasParType : STRUCT
		PhasingCurrent : REAL; (*Motor current durring phasing [A]*)
		PhasingTime : REAL; (*Time for phasing the motor [s]*)
	END_STRUCT;

	McAcpAxAdvAutoTuneMotPhasType : STRUCT
		CommutationOffset : REAL; (*Commutation offset [rad]*)
	END_STRUCT;

	McAcpAxAutoTuneMotPhasOutType : STRUCT
		Quality : REAL;
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		CommutationOffset : REAL; (*Commutation offset [rad]*)
	END_STRUCT;

	McAcpAxAdvAutoTuneLoadModelType : STRUCT
		IdentMode : McAcpAxLoadModelIdentModeEnum; (*Load model identification mode, i.e. closed or open loop.*)
		ExcitationSignal : McAcpAxAutoTuneExSignalType; (*Parameters for excitation signal.*)
	END_STRUCT;

	McAcpAxAutoTuneLoadModelOutType : STRUCT
		Quality : REAL; (*Quality of parameter identification [%]*)
		LoadModel : McAcpAxLoadModelType; (*Load model parameters resulting from the autotuning (identification).*)
		Parameters : McCfgAcpCtrlType; (*Parameter structure for usage on MC_BR_ProcessConfig and MC_BR_ProcessParam*)
	END_STRUCT;

	McAcpAxAutoTuneFeedFwdParType : STRUCT
		Direction : McDirectionEnum; (*Used for selecting the direction of movement for autotuning the feed-forward control*)
	    Orientation : McAcpAxAutoTuneOrientationEnum; (*Selects the orientation for autotuning*)
	    MaxCurrentPercent : REAL := 25.0; (*Percentage of the rated current that is used during autotuning [%]*)
	    MaxVelocityPercent : REAL := 50.0; (*Percentage of the velocity used during autotuning [%]*)
	    MaxDistance : LREAL; (*Maximum distance traveled during autotuning [Measurement units]*)
	    MaxPositionError : LREAL; (*Maximum permitted lag error during autotuning [Measurement units]*)
	    Acceleration : REAL; (*Acceleration that is used during autotuning [Measurement units/s]*)
	END_STRUCT;

	McAcpAxAdvAutoTuneFeedFwdType : STRUCT
	    ExcitationSignal : McAcpAxAutoTuneExSignalType; (*Parameter for excitation signal*)
	END_STRUCT;

	McAcpAxAutoTuneFeedFwdOutType : STRUCT
		Quality : REAL; (*Quality of parameter identification [%]*)
		FeedForward : McAcpAxFeedForwardParType; (*Parameter for first control loop filter*)
		Parameters : McCfgAcpCtrlType; (*Parameter structure for usage on MC_BR_ProcessConfig and MC_BR_ProcessParam*)
	END_STRUCT;

	McAcpAxAdvPhasingParType : STRUCT
	    VelocityParID : UINT; (*ParID from which the velocity for the phase shift is read*)
	    PosVelocityTriggerParID : UINT; (*ParID controls the addition of velocity "CyclicVelocity" or the value of "VelocityParID"*)
	    NegVelocityTriggerParID : UINT; (*ParID controls the subtraction of velocity "CyclicVelocity" or the value of "VelocityParID"*)
	END_STRUCT;

	McAcpAxAdvOffsetParType : STRUCT
	    VelocityParID : UINT; (*ParID from which the velocity for the phase shift is read*)
	    PosVelocityTriggerParID : UINT; (*ParID controls the addition of velocity "CyclicVelocity" or the value of "VelocityParID"*)
	    NegVelocityTriggerParID : UINT; (*ParID controls the subtraction of velocity "CyclicVelocity" or the value of "VelocityParID"*)
	END_STRUCT;

	McAcpAxLoadSimInputDataType : STRUCT
		Position : LREAL; (*Position value [rad]*)
		Velocity : REAL; (*Velocity value [rad/s]*)
 		Acceleration : REAL; (*Acceleration value [rad/s²] Note: This structure element is not supported currently, and always the value "0.0" is output*)
	END_STRUCT;

	McAcpAxLoadModelMass1Type : STRUCT
		Inertia : REAL; (*Mass moment of inertia [kg*m^2].*)
		ViscousFriction : REAL; (*Viscous friction [Nm*s].*)
	END_STRUCT;

	McAcpAxLoadModelMass2Type : STRUCT
		Inertia : REAL; (*Mass moment of inertia [kg*m^2].*)
		ViscousFriction : REAL; (*Viscous friction [Nm*s].*)
		Stiffness : REAL; (*Stiffness of the coupling to Mass 1 [Nm/rad].*)
		Damping : REAL; (*Damping of the coupling to Mass 1 [Nm/(rad/s)].*)
	END_STRUCT;

	McAcpAxLoadModelType : STRUCT
		Mass1 : McAcpAxLoadModelMass1Type; (*Mass 1 component of the load model.*)
		Mass2 : McAcpAxLoadModelMass2Type; (*Mass 2 component of the load model.*)
	END_STRUCT;

	McAcpAxAdvInitParIDTransferType : STRUCT
		MasterSendChannel : McAcpAxSendChannelEnum; (*Requested channel specifier on the Master axis to be used to send the value of the MasterParID.*)
		SlaveReceiveChannel : McAcpAxReceiveChannelEnum; (*Requested channel specifier on the Slave axis to be used to receive the value of the MasterParID.*)
	END_STRUCT;

	McAcpAxParIDMasterSendInfoType : STRUCT
		Used : BOOL; (*Indicates whether this channel is currently being used and the rest of the data below is valid.*)
		ParID :  UINT; (*Master source ParID being sent via this channel.*)
		ChangeAllowed : BOOL; (*Indicates whether it is possible to reconfigure this channel.*)
	END_STRUCT;

	McAcpAxParIDSlaveReceiveInfoType : STRUCT
		Used : BOOL; (*Indicates whether this channel is currently being used and the rest of the data below is valid.*)
		ParID :  UINT; (*Master source ParID being received via this channel.*)
		ChangeAllowed : BOOL; (*Indicates whether it is possible to reconfigure this channel.*)
		InterpolationMode : McIplModeEnum; (*Interpolation mode for the received value.*)
		SendModuleAndElement : STRING[64]; (*Sender module (and element) information. Example: 'EPL: IF3.ST6 (CHAN1)'.*)
	END_STRUCT;

	McAcpAxParIDTransferInfoType : STRUCT
		MasterSendInfo : ARRAY[0..3] OF McAcpAxParIDMasterSendInfoType; (*Detailed information about each of the individual send channels for the axis.*)
		SlaveReceiveInfo : ARRAY[0..5] OF McAcpAxParIDSlaveReceiveInfoType; (*Detailed information about each of the individual receive channels for the axis.*)
	END_STRUCT;

	McAcpAxAdvInitReceiveNetDataType : STRUCT
		NodeNumber : USINT; (*Node number of the POWERLINK station from which data should be received.*)
		BitOffset : UINT; (*Bit offset of the POWERLINK data in the telegram from the transmitter from which point the data is read, must be a multiple of 16.*)
		ReceiveChannel : McAcpAxReceiveChannelEnum; (*Requested channel number on the axis to be used to receive the data.*)
		CycleTime : UDINT; (*Update cycle time of the transmitted user data [us].*)
	END_STRUCT;

	McAcpAxAdvReceiveParIDOnPLCType : STRUCT
		SendChannel : McAcpAxSendChannelEnum; (*Requested channel specifier on the source Axis to be used to send the value of the ParID.*)
	END_STRUCT;

	McAcpAxCyclicDataInfoType : STRUCT
		Write : McAcpAxCyclicDataWriteInfoType; (*Information about the cyclic write data configuration.*)
		Read : McAcpAxCyclicDataReadInfoType; (*Information about the cyclic read data configuration.*)
	END_STRUCT;

	McAcpAxCyclicDataWriteInfoType : STRUCT
		RecordUpdateTime : UDINT; (*Time interval in which an individual telegram record is written to the drive channel [us].*)
		Record : McAcpAxCyclicDataRecordInfoType; (*Information about the configuration of the telegram record written to the drive channel.*)
	END_STRUCT;

	McAcpAxCyclicDataReadInfoType : STRUCT
		RecordUpdateTime : UDINT; (*Time interval in which an individual telegram record is read from the drive channel [us].*)
		TotalUpdateTime : UDINT; (*Time interval in which all configured telegram records are read from the drive channel [us].*)
		TotalParIDCount : USINT; (*Total number of ParIDs whose values are currently being read from the drive channel.*)
		RecordCount : USINT; (*Number of configured telegrams records read from the drive channel.*)
		Record : ARRAY[0..15] OF McAcpAxCyclicDataRecordInfoType; (*Information array about the configuration of the telegram records read from the drive channel.*)
	END_STRUCT;

	McAcpAxCyclicDataRecordInfoType : STRUCT
		Size : USINT; (*Number of data bytes in this record.*)
		OneByteCount : USINT; (*Number of one byte (8 bit) data in this record.*)
		TwoByteCount : USINT; (*Number of two byte (16 bit) data in this record.*)
		FourByteCount : USINT; (*Number of four byte (32 bit) data in this record.*)
		ParIDCount : USINT; (*Number of ParIDs configured in this record (i.e. OneByteCount + TwoByteCount + FourByteCount).*)
		ParID : ARRAY[0..11] OF UINT; (*Array of ParIDs configured in this record.*)
	END_STRUCT;

	McAcpAxAdvSctrlLimitLoadParType : STRUCT
		LoadPositiveParID : UINT; (*Parameter ID of the positive load (torque) limit.*)
		LoadNegativeParID : UINT; (*Parameter ID of the negative load (torque) limit.*)
	END_STRUCT;

	McAcpAxSafeOutDataType : STRUCT
		Control_Reset : BOOL; (*Reset bit*)
		Control_Activate : BOOL; (*Enable axis of the SafeMotion module*)
		Control_STO : BOOL; (*STO control bit (FALSE = requested, if configured in safety application)*)
		Control_SBC : BOOL; (*SBC control bit (FALSE = requested, if configured in safety application)*)
		Control_SS1 : BOOL; (*SS1 control bit (FALSE = requested, if configured in safety application)*)
		reserved_bit5 : BOOL; (*Reserved*)
		Control_STO1 : BOOL; (*STO1 control bit (FALSE = requested, if configured in safety application)*)
		reserved_bit7 : BOOL; (*Reserved*)
		Control_SOS : BOOL; (*SOS control bit (FALSE = requested, if configured in safety application)*)
		Control_SS2 : BOOL; (*SS2 control bit (FALSE = requested, if configured in safety application)*)
		Control_SLA : BOOL; (*SLA control bit (FALSE = requested, if configured in safety application)*)
		Control_SLS1 : BOOL; (*SLS1 control bit (FALSE = requested, if configured in safety application)*)
		Control_SLS2 : BOOL; (*SLS2 control bit (FALSE = requested, if configured in safety application)*)
		reserved_bit13 : BOOL; (*Reserved*)
		Control_SLS3 : BOOL; (*SLS3 control bit (FALSE = requested, if configured in safety application)*)
		Control_SLS4 : BOOL; (*SLS4 control bit (FALSE = requested, if configured in safety application)*)
		Control_SDI_P : BOOL; (*SDI positive control bit (FALSE = requested, if configured in safety application)*)
		Control_SDI_N : BOOL; (*SDI negative control bit (FALSE = requested, if configured in safety application)*)
		Control_SLI : BOOL; (*SLI control bit (FALSE = requested, if configured in safety application)*)
		Control_SBT : BOOL; (*SBT control bit (FALSE = requested, if configured in safety application)*)
		reserved_bit20 : BOOL; (*Reserved*)
		Control_SLT : BOOL; (*SLT control bit (FALSE = requested, if configured in safety application)*)
		Control_SwitchUserData : BOOL; (*Switch between output of SafeSpeed (FALSE) and SafeTorque (TRUE) on output SafeUserData*)
		reserved_bit23 : BOOL; (*Reserved*)
		reserved_bit24 : BOOL; (*Reserved*)
		Control_Homing : BOOL; (*Homing control bit (TRUE = safe homing requested)*)
		Control_RefSwitch : BOOL; (*Reference switch input*)
		Control_SLP : BOOL; (*SLP control bit (FALSE = requested, if configured in safety application)*)
		reserved_bit28 : BOOL; (*Reserved*)
		reserved_bit29 : BOOL; (*Reserved*)
		Control_SwitchHomingMode : BOOL; (*Switch between configured homing mode (FALSE) and restore Remanent Safe Position (TRUE)*)
		reserved_bit31 : BOOL; (*Reserved*)
	END_STRUCT;

	McAcpAxSafeInDataType : STRUCT
		Status_NotErrFunc : BOOL; (*Functional Fail Safe status bit (FALSE = functional error)*)
		Status_Operational : BOOL; (*Axis is in state Operational*)
		Status_STO : BOOL; (*STO status bit (TRUE = active)*)
		Status_SBC : BOOL; (*SBC status bit (TRUE = active)*)
		Status_SS1 : BOOL; (*SS1 status bit (TRUE = active)*)
		Status_NotErrEnc : BOOL; (*Encoder error status bit (FALSE = encoder error)*)
		Status_STO1 : BOOL; (*STO1 status bit (TRUE = active)*)
		Status_SDC : BOOL; (*SDC status bit (TRUE = active)*)
		Status_SOS : BOOL; (*SOS status bit (TRUE = active)*)
		Status_SS2 : BOOL; (*SS2 status bit (TRUE = active)*)
		Status_SLA : BOOL; (*SLA status bit (TRUE = active)*)
		Status_SLS1 : BOOL; (*SLS1 status bit (TRUE = active)*)
		Status_SLS2 : BOOL; (*SLS2 status bit (TRUE = active)*)
		reserved_bit13 : BOOL; (*Reserved*)
		Status_SLS3 : BOOL; (*SLS3 status bit (TRUE = active)*)
		Status_SLS4 : BOOL; (*SLS4 status bit (TRUE = active)*)
		Status_SDI_P : BOOL; (*SDI positive status bit (TRUE = active)*)
		Status_SDI_N : BOOL; (*SDI negative status bit (TRUE = active)*)
		Status_SLI : BOOL; (*SLI status bit (TRUE = active)*)
		Status_SBT_Valid : BOOL; (*SBT valid bit (TRUE = valid)*)
		Status_SBT_Active : BOOL; (*SBT active bit (TRUE = active)*)
		Status_SLT : BOOL; (*SLT status bit (TRUE = active)*)
		Status_SFR : BOOL; (*At least one safety function is requested*)
		Status_AllReqActive : BOOL; (*All requested safety functions are active*)
		Status_NotErrEnc2 : BOOL; (*Encoder error status bit 2 (FALSE = encoder error)*)
		Status_Homing : BOOL; (*Safe position valid bit (TRUE = valid)*)
		Status_ReqHomingOK : BOOL; (*State of the safe homing request*)
		Status_SLP : BOOL; (*SLP status bit (TRUE = active)*)
		Status_SMP : BOOL; (*SMP status bit (TRUE = active)*)
		Status_SafeUserData : BOOL; (*SafeUserData status bit (TRUE = SafeUserData active)*)
		Status_RSP_Valid : BOOL; (*RSP valid bit (TRUE = valid)*)
		Status_SetPosAlive : BOOL; (*Alive-testing of set position is valid*)
		SafePosition : DINT; (*Safe position*)
		SafeUserData : DINT; (*Safe speed or safe torque*)
	END_STRUCT;


	McAcpAxVibCtrlStatusType : STRUCT
		Status : McAcpAxVibCtrlStatusEnum; (*Vibration control status*)
		PhaseDifference : REAL; (*Difference of the phase (3rd to 1st harmonic)*)
		Amplitude : REAL; (*Amplitude of 3rd harmonic*)
		AdditionalAmplitude : REAL; (*Change of the amplitude*)
		AdditionalFrequency : REAL; (*Change of the frequency*)
		ReferenceSignal : REAL; (*Reference excitation signal*)
	END_STRUCT;
END_TYPE
