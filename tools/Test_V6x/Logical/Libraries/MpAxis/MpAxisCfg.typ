TYPE
	MpAXBAxBaseTypEnum :
		( (*Axis base type*)
		mcAXB_BASE_TYPE_LIN_BD := 0, (*Linear bounded - Linear axis with bounded movement scope*)
		mcAXB_BASE_TYPE_LIN := 1, (*Linear - Linear axis*)
		mcAXB_BASE_TYPE_LIN_PER := 2, (*Linear periodic - Linear axis with periodic movement scope*)
		mcAXB_BASE_TYPE_ROT_BD := 10, (*Rotary bounded - Rotary axis with bounded movement scope*)
		mcAXB_BASE_TYPE_ROT := 11, (*Rotary - Rotary axis*)
		mcAXB_BASE_TYPE_ROT_PER := 12 (*Rotary periodic - Rotary axis with periodic movement scope*)
		);
	MpAXBAxMeasUnitEnum :
		( (*Measurement unit for the axis*)
		mcAXB_MEAS_UNIT_G_SET := 0, (*Global settings*)
		mcAXB_MEAS_UNIT_MILIMETERS := 5066068, (*Milimeters*)
		mcAXB_MEAS_UNIT_M := 5067858, (*Meters*)
		mcAXB_MEAS_UNIT_INCH := 4804168, (*Inches*)
		mcAXB_MEAS_UNIT_DEG := 17476, (*Degrees*)
		mcAXB_MEAS_UNIT_GRAD := 4274481, (*Gradians*)
		mcAXB_MEAS_UNIT_REV := 5059636, (*Revolutions*)
		mcAXB_MEAS_UNIT_GEN := -1 (*Generic*)
		);
	MpAXBAxCntDirEnum :
		( (*Direction of the axis in which the position value is increasing*)
		mcAXB_COUNT_DIR_STD := 0, (*Standard*)
		mcAXB_COUNT_DIR_INV := 1 (*Inverse*)
		);
	MpAXBAxMoveLimTypEnum :
		( (*Movement limits settings*)
		mcAXB_MOV_LIM_STD := 0, (*Standard*)
		mcAXB_MOV_LIM_PATH_CTRL_AX := 1, (*Path controlled axis*)
		mcAXB_MOV_LIM_LIM_SET_REF := 2 (*Limit set reference*)
		);
	MpAXBAxMoveLimPosType : STRUCT (*Movement range of the axis via two position boundaries; Only for bounded axis*)
		LowerLimit : LREAL; (*Lower software limit position [measurement units]*)
		UpperLimit : LREAL; (*Upper software limit position [measurement units]*)
	END_STRUCT;
	MpAXBAxMoveLimVelType : STRUCT (*Limits for the velocity of the axis*)
		Positive : REAL; (*Velocity limit in positive movement direction [measurement units/s]*)
		Negative : REAL; (*Velocity limit in negative movement direction [measurement units/s]*)
	END_STRUCT;
	MpAXBAxMoveLimType : STRUCT
		Type : MpAXBAxMoveLimTypEnum; (*Movement limits settings*)
		Position : MpAXBAxMoveLimPosType; (*Movement range of the axis via two position boundaries; Only for bounded axis*)
		Velocity : MpAXBAxMoveLimVelType; (*Limits for the velocity of the axis*)
		Acceleration : REAL; (*Acceleration limit in any movement direction [measurement units/s²]*)
		Deceleration : REAL; (*Deceleration limit in any movement direction [measurement units/s²]*)
		Jerk : REAL; (*Jerk limit in any movement direction [measurement units/s³]*)
		Torque : REAL; (*Torque limit in any movement direction; Only for Axis of type rotary; only for limits type internal path controlled [Nm]*)
		Force : REAL; (*Force limit in any movement direction; Only for Axis of type linear; only for limits type internal path controlled [N]*)
		LimitSetReference : STRING[250]; (*Name of the limit reference; only for limits type external*)
	END_STRUCT;
	MpAXBAxType : STRUCT (*Axis configuration*)
		BaseType : MpAXBAxBaseTypEnum; (*Axis base type*)
		MeasurementUnit : MpAXBAxMeasUnitEnum; (*Measurement unit for the axis*)
		MeasurementResolution : LREAL; (*Possible resolution of measurement unit that can be achieved [measurement resolution]*)
		CountDirection : MpAXBAxCntDirEnum; (*Direction of the axis in which the position value is increasing*)
		Period : LREAL; (*The value range for axis positions is [0 , Period]; Only for Axis of type periodic [measurement units]*)
		MovementLimits : MpAXBAxMoveLimType;
	END_STRUCT;
	MpAXBDrvMechElmGBType : STRUCT
		Input : DINT; (*Number of rotations on the encoder side [revolutions]*)
		Output : DINT; (*Number of rotations on the load side which correspond to the number of rotations onthe encoder side [revolutions]*)
	END_STRUCT;
	MpAXBDrvMechElmRotToLinTrfType : STRUCT (*Specifies a transformation factor between the output of the gear and the actual load movement*)
		ReferenceDistance : LREAL; (*Reference distance which is considered for an axis positioning [measurement units/gearbox output revolution]*)
	END_STRUCT;
	MpAXBDrvMechElmType : STRUCT (*Parameter of hardware elements situated between motor encoder and load which influence the scaling*)
		Gearbox : MpAXBDrvMechElmGBType;
		RotaryToLinearTransformation : MpAXBDrvMechElmRotToLinTrfType; (*Specifies a transformation factor between the output of the gear and the actual load movement*)
	END_STRUCT;
	MpAXBDrvCtrlModEnum :
		( (*Mode of the axis controller*)
		mcAXB_CTRL_MODE_POS := 0, (*Position - Automatic speed feed-forward with prediction time > 0*)
		mcAXB_CTRL_MODE_POS_W_TORQ_FF := 1, (*Position with torque FF - Torque feed-forward with specified parameters*)
		mcAXB_CTRL_MODE_POS_MDL_BASED := 3, (*Position model based - Model based control with specified parameters*)
		mcAXB_CTRL_MODE_V_FREQ := 2, (*Voltage frequency - Voltage/frequency control of induction motor with specified parameters*)
		mcAXB_CTRL_MODE_STP_CUR := 4, (*Stepper current - StpAx only. Current controller is used*)
		mcAXB_CTRL_MODE_STP_POS_AND_CUR := 5, (*Stepper position and current - StpAx only. Position and current controller are used*)
		mcAXB_CTRL_MODE_NOT_USE := 6 (*Not used - Controller not used*)
		);
	MpAXBDrvCtrlPosType : STRUCT (*Position controller parameters*)
		ProportionalGain : REAL; (*Proportional amplification [1/s]*)
		IntegrationTime : REAL; (*Integral action time [s]*)
		TotalDelayTime : REAL; (*Total delay time [s]*)
		PredictionTime : REAL; (*Prediction time [s]*)
		MaximumProportionalAction : REAL; (*Maximum proportional action. Only for StpAx and PureVax with GPAI [measurement units/s]*)
		MaximumIntegralAction : REAL; (*Maximum integral action. Only for PureVax with GPAI [measurement units/s]*)
	END_STRUCT;
	MpAXBDrvCtrlSpdType : STRUCT (*Speed controller parameters*)
		ProportionalGain : REAL; (*Proportional amplification [1/s]*)
		IntegrationTime : REAL; (*Integral action time [s]*)
		FilterTime : REAL; (*Filter time constant [s]*)
	END_STRUCT;
	MpAXBDrvCtrlFFwdModEnum :
		( (*Mode of the axis controller*)
		mcAXB_FF_MODE_STD := 0, (*Standard*)
		mcAXB_FF_MODE_PRED_SPD := 1, (*Predictive speed*)
		mcAXB_FF_MODE_TWO_MASS_MDL := 2, (*Two mass model*)
		mcAXB_FF_MODE_FRICT_COMP := 3 (*Friction compensation*)
		);
	MpAXBDrvCtrlFFwdType : STRUCT (*Torque feed-forward control parameters*)
		Mode : MpAXBDrvCtrlFFwdModEnum; (*Mode of the axis controller*)
		TorqueLoad : REAL; (*Load torque [Nm]*)
		TorquePositive : REAL; (*Torque in positive direction [Nm]*)
		TorqueNegative : REAL; (*Torque in negative direction [Nm]*)
		SpeedTorqueFactor : REAL; (*Speed torque factor [Nms]*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		AccelerationFilterTime : REAL; (*Acceleration filter time constant [s]*)
		PredictionTime : REAL; (*Prediction time [s]*)
		ActivationSpeed : REAL; (*Activation speed. Only for mode mcAXB_FF_MODE_FRICT_COMP [measurement units/s]*)
		DeactivationLagError : REAL; (*Deactivation position error. Only for mode mcAXB_FF_MODE_FRICT_COMP [measurement unit]*)
		TimeConstant : REAL; (*Time constant. Only for mode mcAXB_FF_MODE_FRICT_COMP [s]*)
	END_STRUCT;
	MpAXBDrvCtrlFdbkModEnum :
		( (*Mode of the axis controller*)
		mcAXB_CTLR_FEED_STD := 0, (*Standard*)
		mcAXB_CTLR_FEED_ONE_MASS_MDL := 1, (*One mass model*)
		mcAXB_CTLR_FEED_TWO_MASS_MDL := 2, (*Two mass model*)
		mcAXB_CTLR_FEED_TWO_ENC_SPD := 3 (*Two encoder speed*)
		);
	MpAXBDrvCtrlFdbkType : STRUCT (*Feedback control parameters*)
		Mode : MpAXBDrvCtrlFdbkModEnum; (*Mode of the axis controller*)
		SpeedMixingFactor : REAL; (*Load torque [Nm]*)
		SpeedProportionalGain : REAL; (*Torque in positive direction [Nm]*)
	END_STRUCT;
	MpAXBDrvCtrlMdlMass1Type : STRUCT (*Mass 1 parameters*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		ViscousFriction : REAL; (*Viscous friction [Nms]*)
	END_STRUCT;
	MpAXBDrvCtrlMdlMass2Type : STRUCT (*Mass 2 parameters*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		ViscousFriction : REAL; (*Viscous friction [Nms]*)
		Stiffness : REAL; (*Stiffness of the coupling to mass 1 [Nm/rad]*)
		Damping : REAL; (*Damping of the coupling to mass 1 [Nms/rad]*)
	END_STRUCT;
	MpAXBDrvCtrlMdlType : STRUCT (*Load model parameters*)
		Mass1 : MpAXBDrvCtrlMdlMass1Type; (*Mass 1 parameters*)
		Mass2 : MpAXBDrvCtrlMdlMass2Type; (*Mass 2 parameters*)
	END_STRUCT;
	MpAXBDrvCtrlVFreqCtrlTypEnum :
		( (*Type of characteristic curve*)
		mcAXB_VF_TYP_LIN := 129, (*Linear - Linear characteristic curve*)
		mcAXB_VF_TYP_CONST_LD_TORQ := 131, (*Constant load torque - Characteristic curve for quadratic load curves*)
		mcAXB_VF_TYP_QUAD := 130 (*Quadratic - Characteristic curve for quadratic load curves*)
		);
	MpAXBDrvCtrlVFreqCtrlAutCfgEnum :
		( (*Automatic configuration of parameters*)
		mcAXB_VF_AUTO_CFG_NOT_USE := 0, (*Not used*)
		mcAXB_VF_AUTO_CFG_MOT_PAR_BASED := 1 (*Motor parameter based*)
		);
	MpAXBDrvCtrlVFreqCtrlType : STRUCT (*V/f control parameters*)
		Type : MpAXBDrvCtrlVFreqCtrlTypEnum; (*Type of characteristic curve*)
		AutomaticConfiguration : MpAXBDrvCtrlVFreqCtrlAutCfgEnum; (*Automatic configuration of parameters*)
		SlipCompensation : REAL; (*Slip compensation: Multiplication factor of compensated frequency*)
		TotalDelayTime : REAL; (*Total delay time [s]*)
		BoostVoltage : REAL; (*Boost voltage [V]*)
		RatedVoltage : REAL; (*Rated voltage [V]*)
		RatedFrequency : REAL; (*Rated frequency [cps]*)
	END_STRUCT;
	MpAXBDrvCtrlLoopFltrTypEnum :
		( (*Type of the loop filter*)
		mcAXB_LP_FLTR_NOT_USE := 0, (*Not used - Filter is not active*)
		mcAXB_LP_FLTR_LOW_PASS_2ND_ORD := 1, (*Low pass 2nd order*)
		mcAXB_LP_FLTR_NOTCH := 2, (*Notch*)
		mcAXB_LP_FLTR_BIQUAD := 3, (*Biquad*)
		mcAXB_LP_FLTR_DISC_TIME_TRAN_FUN := 4, (*Discrete time transfer function*)
		mcAXB_LP_FLTR_LIM := 5, (*Limiter*)
		mcAXB_LP_FLTR_LIN_LIM := 6, (*Linear limitation*)
		mcAXB_LP_FLTR_RISE_TIME_LIM := 7, (*Rise time limitation*)
		mcAXB_LP_FLTR_COMP := 8 (*Compensation*)
		);
	MpAXBLoopFltrLP2ndOrdType : STRUCT (*Lowpass 2nd order*)
		CutOffFrequency : REAL; (*Cut off frequency [Hz]*)
	END_STRUCT;
	MpAXBLoopFltrNotchType : STRUCT (*Notch*)
		CenterFrequency : REAL; (*Center frequency [Hz]*)
		Bandwidth : REAL; (*Bandwidth [Hz]*)
	END_STRUCT;
	MpAXBLoopFltrBiquadType : STRUCT (*Biquad*)
		FrequencyNumerator : REAL; (*Frequency numerator [Hz]*)
		DampingNumerator : REAL; (*Damping numerator*)
		FrequencyDenominator : REAL; (*Frequency denominator [Hz]*)
		DampingDenominator : REAL; (*Damping Denominator*)
	END_STRUCT;
	MpAXBLoopFltrDiscTimeTranFunType : STRUCT (*Discrete time transfer function*)
		CoeffA0DenominatorPolynomial : REAL; (*Coefficient a0 of denominator polynomial*)
		CoeffA1DenominatorPolynomial : REAL; (*Coefficient a1 of denominator polynomial*)
		CoeffB0NominatorPolynomial : REAL; (*Coefficient b0 of numerator polynomial*)
		CoeffB1NominatorPolynomial : REAL; (*Coefficient b1 of numerator polynomial*)
		CoeffB2NominatorPolynomial : REAL; (*Coefficient b2 of numerator polynomial*)
	END_STRUCT;
	MpAXBLoopFltrLimLimTypEnum :
		( (*Select if for the limit a fixed value or if the value of an ACOPOS ParID is used*)
		mcAXB_LOOP_FLTR_LIM_TYPE_PARID := 0, (*ParId*)
		mcAXB_LOOP_FLTR_LIM_TYPE_FIX_VAL := 1 (*Fixed value*)
		);
	MpAXBLoopFltrLimLimType : STRUCT (*Positive limit setting*)
		Type : MpAXBLoopFltrLimLimTypEnum; (*Select if for the limit a fixed value or if the value of an ACOPOS ParID is used*)
		ParID : UINT; (*ParID*)
		Value : REAL; (*Value [A]*)
	END_STRUCT;
	MpAXBLoopFltrLimType : STRUCT (*Limiter*)
		PositiveLimit : MpAXBLoopFltrLimLimType; (*Positive limit setting*)
		NegativeLimit : MpAXBLoopFltrLimLimType; (*Negative limit setting*)
	END_STRUCT;
	MpAXBLoopFltrLinLimType : STRUCT (*Linear limitation*)
		InputParID : UINT; (*ACOPOS ParID for the input*)
		InputLimit : REAL; (*Input limit value for full limitation*)
		Gradient : REAL; (*Gradient*)
	END_STRUCT;
	MpAXBLoopFltrRiseTimeLimType : STRUCT (*Rise time limitation*)
		RiseTime : REAL; (*Rise time [s]*)
		NormalizedLimit : REAL; (*Normalized limit*)
	END_STRUCT;
	MpAXBLoopFltrCompType : STRUCT (*Compensation*)
		MultiplicationFactorParID : UINT; (*Multiplication Factor ParID*)
		AdditiveValueParID : UINT; (*Additive Value ParID*)
	END_STRUCT;
	MpAXBDrvCtrlLoopFltrLoopFltrType : STRUCT (*Type of the loop filter*)
		Type : MpAXBDrvCtrlLoopFltrTypEnum; (*Type of the loop filter*)
		Lowpass2ndOrder : MpAXBLoopFltrLP2ndOrdType; (*Lowpass 2nd order*)
		Notch : MpAXBLoopFltrNotchType; (*Notch*)
		Biquad : MpAXBLoopFltrBiquadType; (*Biquad*)
		DiscreteTimeTransferFunction : MpAXBLoopFltrDiscTimeTranFunType; (*Discrete time transfer function*)
		Limiter : MpAXBLoopFltrLimType; (*Limiter*)
		LinearLimitation : MpAXBLoopFltrLinLimType; (*Linear limitation*)
		RiseTimeLimitation : MpAXBLoopFltrRiseTimeLimType; (*Rise time limitation*)
		Compensation : MpAXBLoopFltrCompType; (*Compensation*)
	END_STRUCT;
	MpAXBDrvCtrlLoopFltrType : STRUCT (*Parameters of the loop filters*)
		LoopFilter : ARRAY[0..2] OF MpAXBDrvCtrlLoopFltrLoopFltrType; (*Type of the loop filter*)
	END_STRUCT;
	MpAXBDrvCtrlCurModEnum :
		( (*Mode of current controller*)
		mcAXB_CUR_CTRL_MODE_STD := 0 (*Standard*)
		);
	MpAXBDrvCtrlCurType : STRUCT (*Current controller parameters; Only for stepper axis*)
		Mode : MpAXBDrvCtrlCurModEnum; (*Mode of current controller*)
		StandstillCurrent : REAL; (*Current that is used when no movement is active [A]*)
		ConstantSpeedCurrent : REAL; (*Current that is used when a movement with a constant speed is active [A]*)
		SpeedChangeCurrent : REAL; (*Current that is used when the axis is accelerating or decelerating [A]*)
		FullStepThreshold : REAL; (*Speed of the motor where the driver switch from microstep to full step mode [rpm]*)
		MotorSettlingTime : REAL; (*Minimum time between when the motor is powered on to when the DrvOk bit is set. Settings made in steps of 10 ms. [s]*)
		DelayedCurrentSwitchOff : REAL; (*Time for a delayed motor switch off after it is decelerated to zero because of a settime fault. Setting is made in steps of 100 ms. [s]*)
	END_STRUCT;
	MpAXBDrvCtrlType : STRUCT (*Axis controller parameters*)
		Mode : MpAXBDrvCtrlModEnum; (*Mode of the axis controller*)
		Position : MpAXBDrvCtrlPosType; (*Position controller parameters*)
		Speed : MpAXBDrvCtrlSpdType; (*Speed controller parameters*)
		FeedForward : MpAXBDrvCtrlFFwdType; (*Torque feed-forward control parameters*)
		Feedback : MpAXBDrvCtrlFdbkType; (*Feedback control parameters*)
		Model : MpAXBDrvCtrlMdlType; (*Load model parameters*)
		VoltageFrequencyControl : MpAXBDrvCtrlVFreqCtrlType; (*V/f control parameters*)
		LoopFilters : MpAXBDrvCtrlLoopFltrType; (*Parameters of the loop filters*)
		Current : MpAXBDrvCtrlCurType; (*Current controller parameters; Only for stepper axis*)
	END_STRUCT;
	MpAXBDrvHomeBlkDistUnitEnum :
		( (*Unit of reference pulse blocking distancece*)
		mcAXB_HOME_BL_DIST_MEAS_UNIT := 0, (*Measurement unit*)
		mcAXB_HOME_BL_DIST_ENC_REV := 1 (*Encoder revolution*)
		);
	MpAXBDrvHomeType : STRUCT (*Homing mode and parameters which can be used within the application program as pre-configured setting*)
		Mode : McHomingModeEnum; (*Mode of the axis controller*)
		Position : LREAL; (*Home position [measurement units]*)
		ReferencePulse : McSwitchEnum; (*Use reference pulse of encoder*)
		ReferencePulseBlockingDistance : LREAL; (*Distance for blocking the activation of triggering reference pulse. In case of Block Torque or Block Lag error homing mode and Reference Pulse not used. This represents the MinimumReturnDistance [measurement units]*)
		BlockingDistanceUnit : MpAXBDrvHomeBlkDistUnitEnum; (*Unit of reference pulse blocking distancece*)
		StartVelocity : REAL; (*Speed for searching the reference switch [measurement units/s]*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		SwitchEdge : McDirectionEnum; (*Edge of reference switch*)
		HomingDirection : McDirectionEnum; (*Movement direction in which the homing event is evaluated*)
		StartDirection : McDirectionEnum; (*Start direction of movement for searching the reference*)
		KeepDirection : McSwitchEnum; (*Keep direction (move only in one direction)*)
		TorqueLimit : REAL; (*Torque limit for homing on block [Nm]*)
		PositionErrorStopLimit : LREAL; (*Lag error for stop of the homing movement [measurement units/s²]*)
		BlockDetectionPositionError : LREAL; (*Lag error for block detection [measurement units]*)
		RestorePositionVariable : STRING[250]; (*Remanent variable used for homing mode: Restore position*)
	END_STRUCT;
	MpAXBDrvStopReacQstopEnum :
		( (*Reaction in case of a quickstop which is caused by an active quickstop input*)
		mcAXB_QSTOP_RCT_DEC_LIM := 0, (*Deceleration limit*)
		mcAXB_QSTOP_RCT_DEC_LIM_W_JERK := 1, (*Deceleration limit with jerk*)
		mcAXB_QSTOP_RCT_TORQ_LIM := 2, (*Torque limit*)
		mcAXB_QSTOP_RCT_INDUCT_HALT := 3, (*Induction halt*)
		mcAXB_QSTOP_RCT_TORQ_LIM_W_JERK := 4, (*Torque limit with jerk*)
		mcAXB_QSTOP_RCT_VEL_CTRL := 5 (*Velocity control*)
		);
	MpAXBDrvStopReacDrvErrEnum :
		( (*Reaction in case of an error stop which is caused by a drive error*)
		mcAXB_ERR_RCT_DEC_LIM := 0, (*Deceleration limit*)
		mcAXB_ERR_RCT_INDUCT_HALT := 1, (*Induction halt*)
		mcAXB_ERR_RCT_COAST_STANDSTILL := 2, (*Coast standstill*)
		mcAXB_ERR_RCT_CYC_DEC_AXESGROUP := 3, (*Cyclic deceleration AxesGroup*)
		mcAXB_ERR_RCT_TORQ_LIM := 4, (*Torque limit*)
		mcAXB_ERR_RCT_TORQ_LIM_W_JERK := 5, (*Torque limit with jerk*)
		mcAXB_ERR_RCT_VEL_CTRL := 6 (*Velocity control*)
		);
	MpAXBDrvStopReacType : STRUCT (*Reactions of the axis in case of certain stop conditions*)
		Quickstop : MpAXBDrvStopReacQstopEnum; (*Reaction in case of a quickstop which is caused by an active quickstop input*)
		DriveError : MpAXBDrvStopReacDrvErrEnum; (*Reaction in case of an error stop which is caused by a drive error*)
		DriveErrorJerkTime : REAL; (*Used for drive error stop reactiion type mcAXB_ERR_RCT_TORQ_LIM_W_JERK [s]*)
		QuickstopJerkTime : REAL; (*Used for quickstop stop reactiion type: mcAXB_QSTOP_RCT_DEC_LIM_W_JERK,mcAXB_QSTOP_RCT_TORQ_LIM_W_JERK [s]*)
		FilterTime : REAL; (*Movement stop: Monitoring: Filter time [s]*)
	END_STRUCT;
	MpAXBDrvMovVelErrMonEnum :
		( (*Velocity error monitoring mode*)
		mcAXB_VEL_MON_AUT_1 := 0, (*Automatic 1*)
		mcAXB_VEL_MON_AUT_2 := 1, (*Automatic 2*)
		mcAXB_VEL_MON_USR_DEF := 2, (*User defined*)
		mcAXB_VEL_MON_NOT_USE := 3 (*Not used*)
		);
	MpAXBDrvMovementErrorLimitsType : STRUCT (*Limit values that result in a stop reaction when exceeded*)
		PositionError : LREAL; (*Lag error limit for stopping a movement [measurement units]*)
		VelocityErrorMonitoring : MpAXBDrvMovVelErrMonEnum; (*Velocity error monitoring mode*)
		VelocityError : REAL; (*Velocity error limit for stopping a movement [measurement units/s]*)
	END_STRUCT;
	MpAXBDrvJerkFilterTypEnum :
		( (*Jerk filter setting*)
		mcAXB_JERK_FLTR_NOT_USE := 0, (*Not used*)
		mcAXB_JERK_FLTR_TIME := 1, (*Time*)
		mcAXB_JERK_FLTR_LIM := 2 (*Limited*)
		);
	MpAXBDrvJerkFilterType : STRUCT (*Jerk filter*)
		Type : MpAXBDrvJerkFilterTypEnum; (*Jerk filter setting*)
		Jerk : REAL; (*Dependant on selected type it is either the jerk limit in any movement direction or a jerk filter time.*)
	END_STRUCT;
	MpAXBDrvDigInLevelEnum :
		( (*Level of the digital input hardware which leads to an active level of the functionality*)
		mcAXB_DI_LEVEL_HIGH := 0, (*High*)
		mcAXB_DI_LEVEL_LOW := 1 (*Low*)
		);
	MpAXBDrvDigInSrcEnum :
		( (*Source of the digital input which is used for this functionality*)
		mcAXBDI_NOT_USE := 0, (*Not used*)
		mcAXBDI_ACP_DIG_IN_X8TRG_1 := 1, (*ACOPOS digital in X8.Trigger 1*)
		mcAXBDI_ACP_DIG_IN_X8TRG_2 := 2, (*ACOPOS digital in X8.Trigger 2*)
		mcAXBDI_ACP_DIG_IN_SS1X41X1 := 3, (*ACOPOS digital in SS1.X41x.1*)
		mcAXBDI_ACP_DIG_IN_SS1X41X2 := 4, (*ACOPOS digital in SS1.X41x.2*)
		mcAXBDI_ACP_DIG_IN_SS1X41X3 := 5, (*ACOPOS digital in SS1.X41x.3*)
		mcAXBDI_ACP_DIG_IN_SS1X41X4 := 6, (*ACOPOS digital in SS1.X41x.4*)
		mcAXBDI_ACP_DIG_IN_SS1X41X5 := 7, (*ACOPOS digital in SS1.X41x.5*)
		mcAXBDI_ACP_DIG_IN_SS1X41X6 := 8, (*ACOPOS digital in SS1.X41x.6*)
		mcAXBDI_ACP_DIG_IN_SS1X41X7 := 9, (*ACOPOS digital in SS1.X41x.7*)
		mcAXBDI_ACP_DIG_IN_SS1X41X8 := 10, (*ACOPOS digital in SS1.X41x.8*)
		mcAXBDI_ACP_DIG_IN_SS1X41X9 := 11, (*ACOPOS digital in SS1.X41x.9*)
		mcAXBDI_ACP_DIG_IN_SS1X41X10 := 12, (*ACOPOS digital in SS1.X41x.10*)
		mcAXBDI_ACP_DIG_IN_X23ATRG_1 := 13, (*ACOPOS digital in X23A.Trigger 1*)
		mcAXBDI_ACP_DIG_IN_X23ATRG_2 := 14, (*ACOPOS digital in X23A.Trigger 2*)
		mcAXBDI_ACP_DIG_IN_X24ATRG_2 := 15, (*ACOPOS digital in X24A.Trigger 2*)
		mcAXBDI_ACP_DIG_IN_X2TRG_1 := 16, (*ACOPOS digital in X2.Trigger 1*)
		mcAXBDI_ACP_DIG_IN_X2TRG_2 := 17, (*ACOPOS digital in X2.Trigger 2*)
		mcAXBDI_ACP_DIG_IN_X1TRG_1 := 18, (*ACOPOS digital in X1.Trigger 1*)
		mcAXBDI_ACP_DIG_IN_X1TRG_2 := 19, (*ACOPOS digital in X1.Trigger 2*)
		mcAXBDI_ACP_DIG_IN_X1REF_SW := 20, (*ACOPOS digital in X1.Reference switch*)
		mcAXBDI_ACP_DIG_IN_X1POS_HW_LIM := 21, (*ACOPOS digital in X1.Positive HW limit*)
		mcAXBDI_ACP_DIG_IN_X1NEG_HW_LIM := 22, (*ACOPOS digital in X1.Negative HW limit*)
		mcAXBDI_FORCED_BY_FUN_BLK := 23, (*Forced by function block*)
		mcAXBDI_VAR := 24, (*Variable*)
		mcAXBDI_IO_CH := 40, (*I/O Channel*)
		mcAXBDI_STP_DIG_IN_TRG_1 := 41, (*Stepper digital input trigger 1*)
		mcAXBDI_STP_DIG_IN_TRG_2 := 42, (*Stepper digital input trigger 2*)
		mcAXBDI_STP_DIG_IN_1 := 43, (*Stepper digital input 1*)
		mcAXBDI_STP_DIG_IN_2 := 44, (*Stepper digital input 2*)
		mcAXBDI_STP_DIG_IN_3 := 45, (*Stepper digital input 3*)
		mcAXBDI_STP_DIG_IN_4 := 46, (*Stepper digital input 4*)
		mcAXBDI_STP_DIG_IN_5 := 47, (*Stepper digital input 5*)
		mcAXBDI_STP_DIG_IN_6 := 48 (*Stepper digital input 6*)
		);
	MpAXBDrvDigInHomeSwType : STRUCT (*Homing switch input functionality*)
		Level : MpAXBDrvDigInLevelEnum; (*Level of the digital input hardware which leads to an active level of the functionality*)
		Source : MpAXBDrvDigInSrcEnum; (*Source of the digital input which is used for this functionality*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for digital input when type mcAXBDI_VAR or mcAXBDI_IO_CH is used*)
	END_STRUCT;
	MpAXBDrvDigInPosLimSwType : STRUCT (*Positive limit switch input functionality*)
		Level : MpAXBDrvDigInLevelEnum; (*Level of the digital input hardware which leads to an active level of the functionality*)
		Source : MpAXBDrvDigInSrcEnum; (*Source of the digital input which is used for this functionality*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for digital input when type mcAXBDI_VAR or mcAXBDI_IO_CH is used*)
	END_STRUCT;
	MpAXBDrvDigInNegLimSwType : STRUCT (*Negative limit switch input functionality*)
		Level : MpAXBDrvDigInLevelEnum; (*Level of the digital input hardware which leads to an active level of the functionality*)
		Source : MpAXBDrvDigInSrcEnum; (*Source of the digital input which is used for this functionality*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for digital input when type mcAXBDI_VAR or mcAXBDI_IO_CH is used*)
	END_STRUCT;
	MpAXBDrvDigTimeStampTypeEnum :
		( (*Time stamp setting*)
		mcAXB_DI_TIME_STAMP_NOT_USE := 0, (*Not used*)
		mcAXB_DI_TIME_STAMP_USE := 1, (*Used*)
		mcAXB_DI_TIME_STAMP_RIS_FALL_EDG := 2 (*Rising falling edge*)
		);
	MpAXBDrvDigTimeStampEdgType : STRUCT (*Parameters for the rising trigger edge for type mcAXB_DI_TIME_STAMP_RIS_FALL_EDG*)
		CountSourceMapping : STRING[250]; (*Name of the process variable (SINT) representing the trigger edge count*)
		TimeStampSourceMapping : STRING[250]; (*Name of the process variable (INT) representing the trigger edge time*)
	END_STRUCT;
	MpAXBDrvDigTimeStampType : STRUCT (*Trigger time stamp. StpAx only*)
		Type : MpAXBDrvDigTimeStampTypeEnum; (*Time stamp setting*)
		TimeStampSourceMapping : STRING[250]; (*Process variable time stamp source PV mapping for type mcAXB_DI_TIME_STAMP_USE*)
		RisingEdge : MpAXBDrvDigTimeStampEdgType; (*Parameters for the rising trigger edge for type mcAXB_DI_TIME_STAMP_RIS_FALL_EDG*)
		FallingEdge : MpAXBDrvDigTimeStampEdgType; (*Parameters for the falling trigger edge for type mcAXB_DI_TIME_STAMP_RIS_FALL_EDG*)
	END_STRUCT;
	MpAXBDrvDigInTrg1Type : STRUCT (*Trigger 1 input functionality*)
		Level : MpAXBDrvDigInLevelEnum; (*Level of the digital input hardware which leads to an active level of the functionality*)
		Source : MpAXBDrvDigInSrcEnum; (*Source of the digital input which is used for this functionality*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for digital input when type mcAXBDI_VAR or mcAXBDI_IO_CH is used*)
		TimeStamp : MpAXBDrvDigTimeStampType; (*Trigger time stamp. StpAx only*)
	END_STRUCT;
	MpAXBDrvDigInTrg2Type : STRUCT (*Trigger 2 input functionality*)
		Level : MpAXBDrvDigInLevelEnum; (*Level of the digital input hardware which leads to an active level of the functionality*)
		Source : MpAXBDrvDigInSrcEnum; (*Source of the digital input which is used for this functionality*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for digital input when type mcAXBDI_VAR or mcAXBDI_IO_CH is used*)
		TimeStamp : MpAXBDrvDigTimeStampType; (*Trigger time stamp. StpAx only*)
	END_STRUCT;
	MpAXBDrvDigInQstopInEnum :
		( (*Digital input functionality triggering an axis quickstop*)
		mcAXB_QSTOP_IN_TRG_2 := 0, (*Trigger 2*)
		mcAXB_QSTOP_IN_TRG_1 := 1, (*Trigger 1*)
		mcAXB_QSTOP_IN_POS_LIM_SW := 2, (*Positive limit switch*)
		mcAXB_QSTOP_IN_NEG_LIM_SW := 3, (*Negative limit switch*)
		mcAXB_QSTOP_IN_HOME_SW := 4, (*Homing switch*)
		mcAXB_QSTOP_IN_NOT_USE := 5, (*Not used*)
		mcAXB_QSTOP_IN_VAR := 6, (*Variable*)
		mcAXB_QSTOP_IN_IO_CH := 7 (*I/O Channel*)
		);
	MpAXBDrvDigInQstopType : STRUCT (*Quickstop input functionality*)
		Input : MpAXBDrvDigInQstopInEnum; (*Digital input functionality triggering an axis quickstop*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for digital input when following inputs types are used: mcAXB_QSTOP_VAR, mcAXB_QSTOP_IO_CH*)
	END_STRUCT;
	MpAXBDrvDigInType : STRUCT (*Various digital input functionalities e.g. like homing switch or triggers*)
		HomingSwitch : MpAXBDrvDigInHomeSwType; (*Homing switch input functionality*)
		PositiveLimitSwitch : MpAXBDrvDigInPosLimSwType; (*Positive limit switch input functionality*)
		NegativeLimitSwitch : MpAXBDrvDigInNegLimSwType; (*Negative limit switch input functionality*)
		Trigger1 : MpAXBDrvDigInTrg1Type; (*Trigger 1 input functionality*)
		Trigger2 : MpAXBDrvDigInTrg2Type; (*Trigger 2 input functionality*)
		Quickstop : MpAXBDrvDigInQstopType; (*Quickstop input functionality*)
	END_STRUCT;
	MpAXBDrvEncLinkTypEnum :
		( (*Encoder type*)
		mcAXB_ENC_ONE_ENC := 0, (*One encoder - One encoder is used for motor and position*)
		mcAXB_ENC_TWO_ENC := 1, (*Two encoder - Two separate encoders are used for motor and position*)
		mcAXB_ENC_NO_ENC := 2 (*No encoder - No position input, encoder not used*)
		);
	MpAXBEncSrcEnum :
		( (*Source of encoder information*)
		mcAXB_ENC_SRC_ACP_ENC_X6A := 0, (*ACOPOS encoder X6A - OnBoard encoder 1*)
		mcAXB_ENC_SRC_ACP_ENC_X6B := 1, (*ACOPOS encoder X6B - OnBoard encoder 2*)
		mcAXB_ENC_SRC_ACP_ENC := 2, (*ACOPOS encoder -*)
		mcAXB_ENC_SRC_ACP_ENC_SS1X11 := 3, (*ACOPOS encoder SS1.X11 - Plug-in module in SS1*)
		mcAXB_ENC_SRC_ACP_ENC_SS2X11 := 4, (*ACOPOS encoder SS2.X11 - Plug-in module in SS2*)
		mcAXB_ENC_SRC_ACP_ENC_X11A := 5, (*ACOPOS encoder X11A -*)
		mcAXB_ENC_SRC_ACP_ENC_SS3X11 := 6, (*ACOPOS encoder SS3.X11 - Plug-in module in SS3*)
		mcAXB_ENC_SRC_ACP_ENC_SS4X11 := 7, (*ACOPOS encoder SS4.X11 - Plug-in module in SS4*)
		mcAXB_ENC_SRC_ACP_ENC_X41 := 8, (*ACOPOS encoder X41 -*)
		mcAXB_ENC_SRC_ACP_ENC_SS1X41X := 9, (*ACOPOS encoder SS1.X41X - Plug-in module in SS1*)
		mcAXB_ENC_SRC_ACP_ENC_X42 := 10, (*ACOPOS encoder X42 -*)
		mcAXB_ENC_SRC_ACP_ENC_SS1X42X := 11, (*ACOPOS encoder SS1.X42X - Plug-in module in SS1*)
		mcAXB_ENC_SRC_ACP_ENC_X43 := 12, (*ACOPOS encoder X43 -*)
		mcAXB_ENC_SRC_ACP_ENC_SS1X43X := 13, (*ACOPOS encoder SS1.X43X - Plug-in module in SS1*)
		mcAXB_ENC_SRC_STP_STEP_CNT := 30, (*Stepper step counter -*)
		mcAXB_ENC_SRC_STP_ENC := 31, (*Stepper encoder -*)
		mcAXB_ENC_SRC_STP_ENC_X6 := 32, (*Stepper encoder X6 -*)
		mcAXB_ENC_SRC_STP_ENC_X6A := 33, (*Stepper encoder X6A -*)
		mcAXB_ENC_SRC_STP_ENC_X6B := 34, (*Stepper encoder X6B -*)
		mcAXB_ENC_SRC_STP_ENC_X3 := 35, (*Stepper encoder X3 -*)
		mcAXB_ENC_SRC_STP_ENC_X4 := 36, (*Stepper encoder X4 -*)
		mcAXB_ENC_SRC_ENC_EXT := 40 (*Encoder external - Only for PureVax or StpAx*)
		);
	MpAXBEncLinkEncParSetEnum :
		( (*Encoder parameter set selection. Only for AcpAx*)
		mcAXB_ENC_PAR_SET_AUT := 0, (*Automatic - Automatic selection of encoder parameter set (see AS-Help)*)
		mcAXB_ENC_PAR_SET_ENCOD1 := 1, (*ENCOD1 - Parameter set ENCOD1*)
		mcAXB_ENC_PAR_SET_ENCOD2 := 2 (*ENCOD2 - Parameter set ENCOD2*)
		);
	MpAXBEncLinkStpCntRefPSrcEnum :
		( (*Input source for the reference pulse*)
		mcAXB_ENC_SC_REF_P_DIG_IN_1 := 0, (*Digital input 1*)
		mcAXB_ENC_SC_REF_P_DIG_IN_2 := 1, (*Digital input 2*)
		mcAXB_ENC_SC_REF_P_DIG_IN_3 := 2, (*Digital input 3*)
		mcAXB_ENC_SC_REF_P_DIG_IN_5 := 3, (*Digital input 5*)
		mcAXB_ENC_SC_REF_P_DIG_IN_6 := 4, (*Digital input 6*)
		mcAXB_ENC_SC_REF_P_R_IN_OF_X6A := 5, (*R input of X6A*)
		mcAXB_ENC_SC_REF_P_R_IN_OF_X6B := 6 (*R input of X6B*)
		);
	MpAXBEncLinkStpCntRefPEdgEnum :
		( (*Detection of the reference pulse*)
		mcAXB_ENC_SC_REF_P_POS_EDG := 0, (*Positive edge*)
		mcAXB_ENC_SC_REF_P_NEG_EDG := 1 (*Negative edge*)
		);
	MpAXBEncLinkStpCntType : STRUCT (*Internal step counter for StpAx only*)
		ReferencePulseSource : MpAXBEncLinkStpCntRefPSrcEnum; (*Input source for the reference pulse*)
		ReferencePulseEdge : MpAXBEncLinkStpCntRefPEdgEnum; (*Detection of the reference pulse*)
	END_STRUCT;
	MpAXBEncExtPosTypEnum :
		( (*Type of encoder position information*)
		mcAXB_ENC_EXT_POS_ABS := 0, (*Absolute*)
		mcAXB_ENC_EXT_POS_INCR := 1 (*Incremental*)
		);
	MpAXBEncLinkExtAbsPosRngType : STRUCT (*Absolute position range of encoder range of the position value*)
		LowerLimit : DINT; (*Lower limit of encoder range*)
		UpperLimit : UDINT; (*Upper limit of encoder range*)
	END_STRUCT;
	MpAXBEncLinkExtPosEnum :
		( (*Position source type*)
		mcAXB_ENC_EXT_SRC_IO_CH_DINT := 0, (*I/O channel DINT*)
		mcAXB_ENC_EXT_SRC_IO_CH_UDINT := 1, (*I/O channel UDINT*)
		mcAXB_ENC_EXT_SRC_IO_CH_INT := 2, (*I/O channel INT*)
		mcAXB_ENC_EXT_SRC_IO_CH_UINT := 3, (*I/O channel UINT*)
		mcAXB_ENC_EXT_SRC_VAR_DINT := 4, (*Variable DINT*)
		mcAXB_ENC_EXT_SRC_VAR_UDINT := 5, (*Variable UDINT*)
		mcAXB_ENC_EXT_SRC_VAR_INT := 6, (*Variable INT*)
		mcAXB_ENC_EXT_SRC_VAR_UINT := 7 (*Variable UINT*)
		);
	MpAXBEncLinkEncExtModOkTypEnum :
		( (*Module ok information source type*)
		mcAXB_ENC_EXT_MOD_OK_POS_SRC_DEV := 0, (*Position source device*)
		mcAXB_ENC_EXT_MOD_OK_IO_CH := 1, (*I/O Channel*)
		mcAXB_ENC_EXT_MOD_OK_VAR := 2, (*Variable*)
		mcAXB_ENC_EXT_MOD_OK_NOT_USE := 3 (*Not used*)
		);
	MpAXBEncLinkEncExtModOkType : STRUCT (*Use module ok for validity check*)
		Type : MpAXBEncLinkEncExtModOkTypEnum; (*Module ok information source type*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for module Ok*)
	END_STRUCT;
	MpAXBEncLinkEncExtStDatTypEnum :
		( (*Stale data information source type*)
		mcAXB_ENC_EXT_ST_DAT_POS_SRC_DEV := 0, (*Position source device*)
		mcAXB_ENC_EXT_ST_DAT_IO_CH := 1, (*I/O Channel*)
		mcAXB_ENC_EXT_ST_DAT_VAR := 2, (*Variable*)
		mcAXB_ENC_EXT_ST_DAT_NOT_USE := 3 (*Not used*)
		);
	MpAXBEncLinkEncExtStDatType : STRUCT (*Use stale data for validity check*)
		Type : MpAXBEncLinkEncExtStDatTypEnum; (*Stale data information source type*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for stale data*)
	END_STRUCT;
	MpAXBEncLinkEncExtNetTimeTypEnum :
		( (*Net time information source type*)
		mcAXB_ENC_EXT_NET_TIME_NOT_USE := 0, (*Not Used*)
		mcAXB_ENC_EXT_NET_TIME_IO_CH := 1, (*I/O Channel*)
		mcAXB_ENC_EXT_NET_TIME_VAR := 2 (*Variable*)
		);
	MpAXBEncLinkEncExtNetTimeType : STRUCT (*Use net time for validity check*)
		Type : MpAXBEncLinkEncExtNetTimeTypEnum; (*Net time information source type*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for net time*)
	END_STRUCT;
	MpAXBEncLinkEncExtEncOkTypEnum :
		( (*Encoder ok information source type*)
		mcAXB_ENC_EXT_ENC_OK_NOT_USE := 0, (*Not Used*)
		mcAXB_ENC_EXT_ENC_OK_IO_CH := 1, (*I/O Channel*)
		mcAXB_ENC_EXT_ENC_OK_VAR := 2 (*Variable*)
		);
	MpAXBEncLinkEncExtEncOkType : STRUCT (*Use encoder ok for validity check*)
		Type : MpAXBEncLinkEncExtEncOkTypEnum; (*Encoder ok information source type*)
		SourceMapping : STRING[250]; (*Process variable or IO channel source for encoder ok*)
	END_STRUCT;
	MpAXBEncLinkEncExtValCkType : STRUCT (*Check if given position is valid*)
		ModuleOk : MpAXBEncLinkEncExtModOkType; (*Use module ok for validity check*)
		StaleData : MpAXBEncLinkEncExtStDatType; (*Use stale data for validity check*)
		NetTime : MpAXBEncLinkEncExtNetTimeType; (*Use net time for validity check*)
		EncoderOk : MpAXBEncLinkEncExtEncOkType; (*Use encoder ok for validity check*)
	END_STRUCT;
	MpAXBEncLinkEncExtRefPTypEnum :
		( (*Reference pulse type*)
		mcAXB_ENC_EXT_REF_P_NOT_USE := 0, (*Not Used*)
		mcAXB_ENC_EXT_REF_P_IO_CH_INT := 1, (*I/O Channel INT*)
		mcAXB_ENC_EXT_REF_P_VAR_INT := 2, (*Variable INT*)
		mcAXB_ENC_EXT_REF_P_IO_CH_DINT := 3, (*I/O Channel DINT*)
		mcAXB_ENC_EXT_REF_P_VAR_DINT := 4 (*Variable DINT*)
		);
	MpAXBEncLinkEncExtRefPType : STRUCT (*Usage and settings for the evaluation of the reference pulse of the encoder*)
		Type : MpAXBEncLinkEncExtRefPTypEnum; (*Reference pulse type*)
		PositionSourceMapping : STRING[250]; (*Input source for the reference pulse position*)
		CountSourceMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	MpAXBEncLinkEncExtPosFltrTypEnum :
		( (*Position filter type*)
		mcAXB_ENC_EXT_POS_FL_EXTPOL_DIST := 0 (*Extrapolation disturbance - Extrapolation and disturbance filter type*)
		);
	MpAXBEncLinkEncExtPosFltrType : STRUCT (*Filter for the encoder position. Used for StpAc, PureVax external encoder source or by AcpAx external encoder*)
		Type : MpAXBEncLinkEncExtPosFltrTypEnum; (*Position filter type*)
		TimeConstant : REAL; (*Time constant for actual position filter*)
		ExtrapolationTime : REAL; (*Extrapolation time for actual position filter [s]*)
	END_STRUCT;
	MpAXBDrvEncLinkPosEncExtType : STRUCT (*Settings for external encoder. Only used for PureVax and StpAx*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution [lines/revolutions]*)
		PositionType : MpAXBEncExtPosTypEnum; (*Type of encoder position information*)
		AbsolutePositionRange : MpAXBEncLinkExtAbsPosRngType; (*Absolute position range of encoder range of the position value*)
		PositionSource : MpAXBEncLinkExtPosEnum; (*Position source type*)
		PositionSourceMapping : STRING[250]; (*Process variable or IO channel source for encoder position*)
		ValidityCheck : MpAXBEncLinkEncExtValCkType; (*Check if given position is valid*)
		ReferencePulse : MpAXBEncLinkEncExtRefPType; (*Usage and settings for the evaluation of the reference pulse of the encoder*)
		PositionFilter : MpAXBEncLinkEncExtPosFltrType; (*Filter for the encoder position. Used for StpAc, PureVax external encoder source or by AcpAx external encoder*)
	END_STRUCT;
	MpAXBDrvEncLinkMotAndPosEncType : STRUCT (*AcpAx: Motor and position encoder settings for mcAXB_ENC_ONE_ENC or Position encoder settings for mcAXB_ENC_TWO_ENC; StpAx: Position encoder settings; PureVax: Position encoder settings*)
		Source : MpAXBEncSrcEnum; (*Source of encoder information*)
		EncoderParameterSet : MpAXBEncLinkEncParSetEnum; (*Encoder parameter set selection. Only for AcpAx*)
		StepCounter : MpAXBEncLinkStpCntType; (*Internal step counter for StpAx only*)
		External : MpAXBDrvEncLinkPosEncExtType; (*Settings for external encoder. Only used for PureVax and StpAx*)
	END_STRUCT;
	MpAXBDrvEncLinkPosEncScGBType : STRUCT (*Specifies a gearbox by defining the ratio between a gearbox input and output*)
		Input : DINT; (*Number of rotations on the encoder side [revolutions]*)
		Output : DINT; (*Number of rotations on the load side which correspond to the number of rotations on the encoder side [revolutions]*)
	END_STRUCT;
	MpAXBEncLinkRotToLinTrfType : STRUCT (*Specifies a transformation factor between the output of the gear and the actual load movement*)
		ReferenceDistance : LREAL; (*Reference distance which is considered for an axis positioning [measurement units/gearbox output revolution]*)
	END_STRUCT;
	MpAXBEncLinkCntDirEnum :
		( (*Direction of the axis in which the position value is increasing*)
		mcAXB_ENC_COUNT_DIR_AUT := 0, (*Automatic*)
		mcAXB_ENC_COUNT_DIR_INV := 1 (*Inverse*)
		);
	MpAXBDrvEncLinkPosEncScType : STRUCT (*Encoder scaling based on a gear ratio and / or a movement transformation factor*)
		Gearbox : MpAXBDrvEncLinkPosEncScGBType; (*Specifies a gearbox by defining the ratio between a gearbox input and output*)
		RotaryToLinearTransformation : MpAXBEncLinkRotToLinTrfType; (*Specifies a transformation factor between the output of the gear and the actual load movement*)
		CountDirection : MpAXBEncLinkCntDirEnum; (*Direction of the axis in which the position value is increasing*)
	END_STRUCT;
	MpAXBDrvEncLinkPosEncType : STRUCT (*Position encoder settings for AcpAx mcAXB_ENC_TWO_ENC*)
		Source : MpAXBEncSrcEnum; (*Source of encoder information*)
		Scaling : MpAXBDrvEncLinkPosEncScType; (*Encoder scaling based on a gear ratio and / or a movement transformation factor*)
		EncoderParameterSet : MpAXBEncLinkEncParSetEnum; (*Encoder parameter set selection. Only for AcpAx*)
		PositionDifferenceLimit : REAL; (*Position difference limit between motor and position encoder for stopping a movement [measurement units]*)
	END_STRUCT;
	MpAXBDrvEncLinkType : STRUCT (*Encoder Link*)
		Type : MpAXBDrvEncLinkTypEnum; (*Encoder type*)
		MotorAndPositionEncoder : MpAXBDrvEncLinkMotAndPosEncType; (*AcpAx: Motor and position encoder settings for mcAXB_ENC_ONE_ENC or Position encoder settings for mcAXB_ENC_TWO_ENC; StpAx: Position encoder settings; PureVax: Position encoder settings*)
		PositionEncoder : MpAXBDrvEncLinkPosEncType; (*Position encoder settings for AcpAx mcAXB_ENC_TWO_ENC*)
	END_STRUCT;
	MpAXBDrvType : STRUCT (*Drive configuration*)
		MechanicalElements : MpAXBDrvMechElmType; (*Parameter of hardware elements situated between motor encoder and load which influence the scaling*)
		Controller : MpAXBDrvCtrlType; (*Axis controller parameters*)
		Homing : MpAXBDrvHomeType; (*Homing mode and parameters which can be used within the application program as pre-configured setting*)
		StopReaction : MpAXBDrvStopReacType; (*Reactions of the axis in case of certain stop conditions*)
		MovementErrorLimits : MpAXBDrvMovementErrorLimitsType; (*Limit values that result in a stop reaction when exceeded*)
		JerkFilter : MpAXBDrvJerkFilterType; (*Jerk filter*)
		DigitalInputs : MpAXBDrvDigInType; (*Various digital input functionalities e.g. like homing switch or triggers*)
		EncoderLink : MpAXBDrvEncLinkType; (*Encoder Link*)
	END_STRUCT;
	MpAXBFeatRefType : STRUCT (*Feature references*)
		ConfigType : McCfgTypeEnum; (*Feature type*)
		Name : STRING[250]; (*Reference name*)
	END_STRUCT;
	MpAXBFeatAxFeatType : STRUCT (*Axis feature references*)
		Reference : ARRAY[0..9] OF MpAXBFeatRefType; (*Feature references*)
	END_STRUCT;
	MpAXBFeatChFeatType : STRUCT (*Channel feature references, only for AcpAx. For all axes sharing the channel features (Real axis and virtual axis) settings should be the same otherwise it will override the other axis settings*)
		Reference : ARRAY[0..9] OF MpAXBFeatRefType; (*Feature references*)
	END_STRUCT;
	MpAXBFeatType : STRUCT (*Used feature configuration*)
		AxisFeatures : MpAXBFeatAxFeatType; (*Axis feature references*)
		ChannelFeatures : MpAXBFeatChFeatType; (*Channel feature references, only for AcpAx. For all axes sharing the channel features (Real axis and virtual axis) settings should be the same otherwise it will override the other axis settings*)
	END_STRUCT;
	MpAxisBasicConfigType : STRUCT (*General purpose datatype*)
		Axis : MpAXBAxType; (*Axis configuration*)
		Drive : MpAXBDrvType; (*Drive configuration*)
		Features : MpAXBFeatType; (*Used feature configuration*)
	END_STRUCT;
END_TYPE