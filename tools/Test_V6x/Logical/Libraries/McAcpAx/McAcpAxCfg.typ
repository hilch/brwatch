TYPE
	McMSMotEnum :
		( (*Motor selector setting*)
		mcMSM_DEF := 0 (*Default -*)
		);
	McMSMotDefVLimEnum :
		( (*Voltage limitation selector setting*)
		mcMSMDVL_NOT_USE := 0, (*Not used -*)
		mcMSMDVL_USE := 1 (*Used -*)
		);
	McMSMotDefVLimUseType : STRUCT (*Type mcMSMDVL_USE settings*)
		MaximumDCBusVoltage : REAL; (*Maximum permissible DC bus voltage [V]*)
	END_STRUCT;
	McMSMotDefVLimType : STRUCT (*Voltage limitation (Motor has no protective conductor or windings should be protected)*)
		Type : McMSMotDefVLimEnum; (*Voltage limitation selector setting*)
		Used : McMSMotDefVLimUseType; (*Type mcMSMDVL_USE settings*)
	END_STRUCT;
	McMMDEMAngEnum :
		( (*Angle selector setting*)
		mcMSMDEMA_USRDEF := 0, (*User-defined -*)
		mcMSMDEMA_UDEF := 1, (*Undefined -*)
		mcMSMDEMA_VAL_STO_ON_ENC := 2 (*Value stored on encoder -*)
		);
	McMMDEMAngUsrDefType : STRUCT (*Type mcMSMDEMA_USRDEF settings*)
		CommutationOffset : REAL; (*Angle between motor encoder zero point and flux space vector [rad]*)
	END_STRUCT;
	McMMDEMAngUdefAutIdentEnum :
		( (*Automatic identification selector setting*)
		mcMSMDEMAUAI_NOT_USE := 0, (*Not used -*)
		mcMSMDEMAUAI_SAT := 1, (*Saturation -*)
		mcMSMDEMAUAI_DIT := 2, (*Dither -*)
		mcMSMDEMAUAI_DIT2 := 5 (*Dither2 -*)
		);
	McMMDEMAngUdefAutIdentSatType : STRUCT (*Type mcMSMDEMAUAI_SAT settings*)
		PhasingCurrent : REAL; (*Current for identification (optional) [A]*)
	END_STRUCT;
	McMMDEMAngUdefAutIdentDitType : STRUCT (*Type mcMSMDEMAUAI_DIT settings*)
		PhasingCurrent : REAL; (*Current for identification (optional) [A]*)
		PhasingTime : REAL; (*Duration of identification (optional) [s]*)
	END_STRUCT;
	McMMDEMAngUdefAutIdentDit2Type : STRUCT (*Type mcMSMDEMAUAI_DIT2 settings*)
		PhasingCurrent : REAL; (*Current for identification (optional) [A]*)
		PhasingTime : REAL; (*Duration of identification (optional) [s]*)
	END_STRUCT;
	McMMDEMAngUdefAutIdentType : STRUCT (*Automatic identification of the angle when switching on the controller*)
		Type : McMMDEMAngUdefAutIdentEnum; (*Automatic identification selector setting*)
		Saturation : McMMDEMAngUdefAutIdentSatType; (*Type mcMSMDEMAUAI_SAT settings*)
		Dither : McMMDEMAngUdefAutIdentDitType; (*Type mcMSMDEMAUAI_DIT settings*)
		Dither2 : McMMDEMAngUdefAutIdentDit2Type; (*Type mcMSMDEMAUAI_DIT2 settings*)
	END_STRUCT;
	McMMDEMAngUdefType : STRUCT (*Type mcMSMDEMA_UDEF settings*)
		AutomaticIdentification : McMMDEMAngUdefAutIdentType; (*Automatic identification of the angle when switching on the controller*)
	END_STRUCT;
	McMMDEMAngType : STRUCT (*Angle between motor encoder zero point and flux space vector*)
		Type : McMMDEMAngEnum; (*Angle selector setting*)
		UserDefined : McMMDEMAngUsrDefType; (*Type mcMSMDEMA_USRDEF settings*)
		Undefined : McMMDEMAngUdefType; (*Type mcMSMDEMA_UDEF settings*)
	END_STRUCT;
	McMSMotDefEncMntType : STRUCT (*Encoder mounting*)
		Angle : McMMDEMAngType; (*Angle between motor encoder zero point and flux space vector*)
	END_STRUCT;
	McMMTmpSensEnum :
		( (*Temperature sensor selector setting*)
		mcMMTS_THERM := 0, (*Thermistor -*)
		mcMMTS_SW_PTC_THERM := 1, (*Switching PTC thermistor -*)
		mcMMTS_THRMSW := 2, (*Thermoswitches -*)
		mcMMTS_NOT_USE := 3 (*Not used -*)
		);
	McMMTSThermTmpSensIfEnum :
		( (*Temperature sensor configuration*)
		mcMMTSTTSI_MOT_CON_WRD := 0, (*Motor connector wired - Motor connector two-wire line*)
		mcMMTSTTSI_ENC_CON_WRD := 1, (*Encoder connector wired - Encoder connector two-wire line*)
		mcMMTSTTSI_ENC_DAT_TRAN := 2, (*Encoder data transfer - Encoder two-wire line*)
		mcMMTSTTSI_ENC_DAT_TRAN_RES := 3 (*Encoder data transfer resistance - Encoder two-wire line with consideration of the resistance*)
		);
	McMMTSThermType : STRUCT (*Type mcMMTS_THERM settings*)
		LimitTemperature : UINT; (*Maximum permissible temperature [°C]*)
		TemperatureSensorInterface : McMMTSThermTmpSensIfEnum; (*Temperature sensor configuration*)
		ResistanceR0 : REAL; (*Resistance at temperature T0 [Ω]*)
		ResistanceR7 : REAL; (*Resistance at temperature T7 [Ω]*)
		TemperatureT0 : REAL; (*Temperature at resistance R0 [°C]*)
		TemperatureT1 : REAL; (*Temperature at resistance R0 + 1/7 * (R7 - R0) [°C]*)
		TemperatureT2 : REAL; (*Temperature at resistance R0 + 2/7 * (R7 - R0) [°C]*)
		TemperatureT3 : REAL; (*Temperature at resistance R0 + 3/7 * (R7 - R0) [°C]*)
		TemperatureT4 : REAL; (*Temperature at resistance R0 + 4/7 * (R7 - R0) [°C]*)
		TemperatureT5 : REAL; (*Temperature at resistance R0 + 5/7 * (R7 - R0) [°C]*)
		TemperatureT6 : REAL; (*Temperature at resistance R0 + 6/7 * (R7 - R0) [°C]*)
		TemperatureT7 : REAL; (*Temperature at resistance R7 [°C]*)
	END_STRUCT;
	McMMTSSwPTCThermTmpSensIfEnum :
		( (*Temperature sensor configuration*)
		mcMMTSSPTTSI_MOT_CON_WRD := 0, (*Motor connector wired - Motor connector two-wire line*)
		mcMMTSSPTTSI_ENC_CON_WRD := 1, (*Encoder connector wired - Encoder connector two-wire line*)
		mcMMTSSPTTSI_ENC_DAT_TRAN := 2, (*Encoder data transfer - Encoder two-wire line*)
		mcMMTSSPTTSI_ENC_DAT_TRAN_RES := 3 (*Encoder data transfer resistance - Encoder two-wire line with consideration of the resistance*)
		);
	McMMTSSwPTCThermType : STRUCT (*Type mcMMTS_SW_PTC_THERM settings*)
		TemperatureSensorInterface : McMMTSSwPTCThermTmpSensIfEnum; (*Temperature sensor configuration*)
		NominalResponseResistance : REAL; (*Resistance at Nominal response temperature [Ω]*)
		MinimumResistance : REAL; (*Minimum permissible resistance [Ω]*)
		NominalResponseTemperature : REAL; (*Temperature at Nominal response resistance [°C]*)
	END_STRUCT;
	McMMTSThrmSwTmpSensIfEnum :
		( (*Temperature sensor configuration*)
		mcMMTSTSTSI_MOT_CON_WRD := 0, (*Motor connector wired - Motor connector two-wire line*)
		mcMMTSTSTSI_ENC_CON_WRD := 1, (*Encoder connector wired - Encoder connector two-wire line*)
		mcMMTSTSTSI_ENC_DAT_TRAN := 2, (*Encoder data transfer - Encoder two-wire line*)
		mcMMTSTSTSI_ENC_DAT_TRAN_RES := 3 (*Encoder data transfer resistance - Encoder two-wire line with consideration of the resistance*)
		);
	McMMTSThrmSwSwStatOnOvrTmpEnum :
		( (*Switching state on overtemperature*)
		mcMMTSTSSSOO_NORM_CLSD := 0, (*Normally Closed*)
		mcMMTSTSSSOO_NORM_OP := 1 (*Normally Open*)
		);
	McMMTSThrmSwType : STRUCT (*Type mcMMTS_THRMSW settings*)
		TemperatureSensorInterface : McMMTSThrmSwTmpSensIfEnum; (*Temperature sensor configuration*)
		NominalResponseTemperature : REAL; (*Temperature at nominal response resistance of 1000 ohms [°C]*)
		SwitchingStateOnOvertemperature : McMMTSThrmSwSwStatOnOvrTmpEnum; (*Switching state on overtemperature*)
	END_STRUCT;
	McMMTmpSensType : STRUCT (*Temperature sensor configuration*)
		Type : McMMTmpSensEnum; (*Temperature sensor selector setting*)
		Thermistor : McMMTSThermType; (*Type mcMMTS_THERM settings*)
		SwitchingPTCThermistor : McMMTSSwPTCThermType; (*Type mcMMTS_SW_PTC_THERM settings*)
		Thermoswitches : McMMTSThrmSwType; (*Type mcMMTS_THRMSW settings*)
	END_STRUCT;
	McMMTmpMdlEnum :
		( (*Temperature model selector setting*)
		mcMMTM_CUR_AND_SPDBASED := 0, (*Current- and speed-based -*)
		mcMMTM_CURBASED := 1, (*Current-based -*)
		mcMMTM_NOT_USE := 2 (*Not used -*)
		);
	McMMTMCSBCalcMethEnum :
		( (*Calculation method selector setting*)
		mcMMTMCSBCM_SECORD_THRM_NETW := 0, (*Second-order thermal network -*)
		mcMMTMCSBCM_FTHORD_THRM_NETW := 1, (*Fourth-order thermal network -*)
		mcMMTMCSBCM_FTH_ORD_W_CPLG := 2 (*Fourth order with couplings -*)
		);
	McMMTMCSBCMRefTmpEnum :
		( (*Reference temperature selector setting*)
		mcMMTMCSBCMRT_MOT_TMP_SENS := 0, (*Motor temperature sensor -*)
		mcMMTMCSBCMRT_NOM_AMB_TMP := 1, (*Nominal ambient temperature -*)
		mcMMTMCSBCMRT_ENC_TMP_SENS := 2 (*Encoder temperature sensor -*)
		);
	McMMTMCSBCMRefTmpEncTmpSensType : STRUCT (*Type mcMMTMCSBCMRT_ENC_TMP_SENS settings*)
		TemperatureOffset : REAL; (*Temperature offset for reference temperature [°C]*)
		TimeConstant : REAL; (*Time constant for the reference temperature [s]*)
	END_STRUCT;
	McMMTMCSBCMRefTmpType : STRUCT (*Reference temperature of winding temperature monitoring*)
		Type : McMMTMCSBCMRefTmpEnum; (*Reference temperature selector setting*)
		EncoderTemperatureSensor : McMMTMCSBCMRefTmpEncTmpSensType; (*Type mcMMTMCSBCMRT_ENC_TMP_SENS settings*)
	END_STRUCT;
	McMMTMCSBCMSecOrdType : STRUCT (*Type mcMMTMCSBCM_SECORD_THRM_NETW settings*)
		WindingCrossSection : REAL; (*Phase conductor cross section [mm²]*)
		ThermalTrippingTime : REAL; (*Tripping time in the event of overload (not required with known winding cross section) [s]*)
		ThermalTimeConstant : REAL; (*Thermal time constant [s]*)
		ReferenceTemperature : McMMTMCSBCMRefTmpType; (*Reference temperature of winding temperature monitoring*)
	END_STRUCT;
	McMMTMCSBCMFthOrdType : STRUCT (*Type mcMMTMCSBCM_FTHORD_THRM_NETW settings*)
		ThermalResistance1 : REAL; (*Thermal resistance 1 [K/W]*)
		ThermalCapacity1 : REAL; (*Thermal capacity 1 [J/K]*)
		ThermalResistance2 : REAL; (*Thermal resistance 2 [K/W]*)
		ThermalCapacity2 : REAL; (*Thermal capacity 2 [J/K]*)
		StatorThermalLoss1 : REAL; (*Linear coefficient of speed-dependent losses at stator [Ws]*)
		StatorThermalLoss2 : REAL; (*Quadratic coefficient of speed-dependent losses at stator [Ws²]*)
		ReferenceTemperature : McMMTMCSBCMRefTmpType; (*Reference temperature of winding temperature monitoring*)
	END_STRUCT;
	McMMTMCSBCMFthOrdWCplgType : STRUCT (*Type mcMMTMCSBCM_FTH_ORD_W_CPLG settings*)
		ThermalResistance1 : REAL; (*Thermal resistance 1 [K/W]*)
		ThermalCapacity1 : REAL; (*Thermal capacity 1 [J/K]*)
		ThermalResistance2 : REAL; (*Thermal resistance 2 [K/W]*)
		ThermalCapacity2 : REAL; (*Thermal capacity 2 [J/K]*)
		ThermalResistance3 : REAL; (*Thermal resistance 3 [K/W]*)
		StatorThermalLoss1 : REAL; (*Linear coefficient of speed-dependent losses at stator [Ws]*)
		StatorThermalLoss2 : REAL; (*Quadratic coefficient of speed-dependent losses at stator [Ws²]*)
		WindingThermalLoss1 : REAL; (*Linear coefficient of speed-dependent losses at windings [Ws]*)
		WindingThermalLoss2 : REAL; (*Quadratic coefficient of speed-dependent losses at windings [Ws²]*)
		ReferenceTemperature : McMMTMCSBCMRefTmpType; (*Reference temperature of winding temperature monitoring*)
	END_STRUCT;
	McMMTMCSBCalcMethType : STRUCT
		Type : McMMTMCSBCalcMethEnum; (*Calculation method selector setting*)
		SecondOrderThermalNetwork : McMMTMCSBCMSecOrdType; (*Type mcMMTMCSBCM_SECORD_THRM_NETW settings*)
		FourthOrderThermalNetwork : McMMTMCSBCMFthOrdType; (*Type mcMMTMCSBCM_FTHORD_THRM_NETW settings*)
		FourthOrderWithCouplings : McMMTMCSBCMFthOrdWCplgType; (*Type mcMMTMCSBCM_FTH_ORD_W_CPLG settings*)
	END_STRUCT;
	McMMTMCurSpdBsdType : STRUCT (*Type mcMMTM_CUR_AND_SPDBASED settings*)
		LimitTemperature : REAL; (*Maximum permissible winding temperature [°C]*)
		CalculationMethod : McMMTMCSBCalcMethType;
	END_STRUCT;
	McMMTMCurBsdType : STRUCT (*Type mcMMTM_CURBASED settings*)
		LimitTemperature : REAL; (*Maximum permissible winding temperature [°C]*)
		WindingCrossSection : REAL; (*Phase conductor cross section [mm²]*)
		ThermalTrippingTime : REAL; (*Tripping time in the event of overload (not required with known winding cross section) [s]*)
		ThermalTimeConstant : REAL; (*Thermal time constant [s]*)
	END_STRUCT;
	McMMTmpMdlType : STRUCT (*Model for winding temperature monitoring*)
		Type : McMMTmpMdlEnum; (*Temperature model selector setting*)
		CurrentAndSpeedBased : McMMTMCurSpdBsdType; (*Type mcMMTM_CUR_AND_SPDBASED settings*)
		CurrentBased : McMMTMCurBsdType; (*Type mcMMTM_CURBASED settings*)
	END_STRUCT;
	McMSMotDefType : STRUCT (*Type mcMSM_DEF settings*)
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		NominalSpeed : REAL; (*Nominal speed [rpm]*)
		MaximumSpeed : REAL; (*Maximum permissible speed [rpm]*)
		NominalVoltage : REAL; (*Nominal voltage (RMS value, phase-phase) [V]*)
		NominalCurrent : REAL; (*Phase current for generating the nominal torque at nominal speed (RMS value) [A]*)
		StallCurrent : REAL; (*Phase current for generating the stall torque (RMS value) [A]*)
		PeakCurrent : REAL; (*Phase current for generating the peak torque (RMS value) [A]*)
		NominalTorque : REAL; (*Motor torque at nominal current [Nm]*)
		StallTorque : REAL; (*Motor torque at stall current [Nm]*)
		PeakTorque : REAL; (*Motor torque at peak current [Nm]*)
		VoltageConstant : REAL; (*Induced voltage per speed (RMS value of voltage at 1000 rpm, phase-phase) [mV/rpm]*)
		TorqueConstant : REAL; (*Torque constant [Nm/A]*)
		StatorResistance : REAL; (*Stator resistance (phase-phase) [Ω]*)
		StatorInductance : REAL; (*Stator inductance (phase-phase) [mH]*)
		MomentOfInertia : REAL; (*Mass moment of inertia [kgcm²]*)
		NominalAmbientTemperature : REAL; (*Nominal ambient temperature [°C]*)
		VoltageLimitation : McMSMotDefVLimType; (*Voltage limitation (Motor has no protective conductor or windings should be protected)*)
		EncoderMounting : McMSMotDefEncMntType; (*Encoder mounting*)
		TemperatureSensor : McMMTmpSensType; (*Temperature sensor configuration*)
		TemperatureModel : McMMTmpMdlType; (*Model for winding temperature monitoring*)
	END_STRUCT;
	McMSMotType : STRUCT
		Type : McMSMotEnum; (*Motor selector setting*)
		Default : McMSMotDefType; (*Type mcMSM_DEF settings*)
	END_STRUCT;
	McMSBrkEnum :
		( (*Brake selector setting*)
		mcMSB_NOT_USE := 0, (*Not used -*)
		mcMSB_USE := 1 (*Used -*)
		);
	McMSBrkUseCtrlModEnum :
		( (*Control mode selector setting*)
		mcMSBUCM_SW := 0, (*Switched -*)
		mcMSBUCM_V_CTRL := 1 (*Voltage controlled -*)
		);
	McMSBrkUseCtrlModVCtrlType : STRUCT (*Type mcMSBUCM_V_CTRL settings*)
		ReleaseVoltage : REAL; (*Nominal voltage, to release (open) the holding brake [V]*)
		HoldVoltage : REAL; (*Nominal voltage to ensure the holding brake remains open [V]*)
	END_STRUCT;
	McMSBrkUseCtrlModType : STRUCT (*Behaviour of holding brake control*)
		Type : McMSBrkUseCtrlModEnum; (*Control mode selector setting*)
		VoltageControlled : McMSBrkUseCtrlModVCtrlType; (*Type mcMSBUCM_V_CTRL settings*)
	END_STRUCT;
	McMSBrkUseLimEnum :
		( (*Limits selector setting*)
		mcMSBUL_NOT_USE := 0, (*Not used -*)
		mcMSBUL_USE := 1 (*Used -*)
		);
	McMSBrkUseLimUseType : STRUCT (*Type mcMSBUL_USE settings*)
		MaximumVoltage : REAL; (*Maximum permissible voltage to release (open) the holding brake [V]*)
		PermittedFrictionWork : REAL; (*Permitted friction work up to the waer limit [J]*)
	END_STRUCT;
	McMSBrkUseLimType : STRUCT (*Holding brake limits*)
		Type : McMSBrkUseLimEnum; (*Limits selector setting*)
		Used : McMSBrkUseLimUseType; (*Type mcMSBUL_USE settings*)
	END_STRUCT;
	McMSBrkUseType : STRUCT (*Type mcMSB_USE settings*)
		NominalCurrent : REAL; (*Current of the holding brake [A]*)
		NominalTorque : REAL; (*Minimum holding torque of the holding brake [Nm]*)
		ActivationDelay : REAL; (*Holding torque build-up time after switching off the operating voltage [s]*)
		ReleaseDelay : REAL; (*Holding torque decaying time after switching on the operating voltage [s]*)
		MomentOfInertia : REAL; (*Moment of inertia for the holding brake [kgcm²]*)
		ControlMode : McMSBrkUseCtrlModType; (*Behaviour of holding brake control*)
		Limits : McMSBrkUseLimType; (*Holding brake limits*)
	END_STRUCT;
	McMSBrkType : STRUCT (*Holding brake*)
		Type : McMSBrkEnum; (*Brake selector setting*)
		Used : McMSBrkUseType; (*Type mcMSB_USE settings*)
	END_STRUCT;
	McMSEncEnum :
		( (*Encoder selector setting*)
		mcMSE_NOT_USE := 0, (*Not used -*)
		mcMSE_USE := 1 (*Used -*)
		);
	McMSEncUseTmpSensEnum :
		( (*Temperature sensor selector setting*)
		mcMSEUTS_NOT_USE := 0, (*Not used -*)
		mcMSEUTS_USE := 1 (*Used -*)
		);
	McMSEncUseTmpSensUseType : STRUCT (*Type mcMSEUTS_USE settings*)
		LimitTemperature : UINT; (*Maximum permissible encoder temperature [°C]*)
	END_STRUCT;
	McMSEncUseTmpSensType : STRUCT (*Encoder temperature sensor*)
		Type : McMSEncUseTmpSensEnum; (*Temperature sensor selector setting*)
		Used : McMSEncUseTmpSensUseType; (*Type mcMSEUTS_USE settings*)
	END_STRUCT;
	McMSEncUseType : STRUCT (*Type mcMSE_USE settings*)
		MomentOfInertia : REAL; (*Moment of inertia for the encoder [kgcm²]*)
		TemperatureSensor : McMSEncUseTmpSensType; (*Encoder temperature sensor*)
	END_STRUCT;
	McMSEncType : STRUCT (*Motor encoder*)
		Type : McMSEncEnum; (*Encoder selector setting*)
		Used : McMSEncUseType; (*Type mcMSE_USE settings*)
	END_STRUCT;
	McMSGBEnum :
		( (*Gearbox selector setting*)
		mcMSG_NOT_USE := 0, (*Not used -*)
		mcMSG_USE := 1, (*Used -*)
		mcMSG_USE_W_DYN_TORQ_LIM := 2 (*Used with dynamic torque limitation -*)
		);
	McMSGBUseType : STRUCT (*Type mcMSG_USE settings*)
		GearRatio : McCfgGearBoxType; (*Ratio between a gearbox input and output*)
		MaximumInputSpeed : REAL; (*Maximum permissible speed at gearbox input [rpm]*)
		NominalOutputTorque : REAL; (*Nominal torque at gearbox output [Nm]*)
		PeakOutputTorque : REAL; (*Peak torque at gearbox output [Nm]*)
		MomentOfInertia : REAL; (*Moment of inertia for the gearbox at gearbox input [kgcm²]*)
	END_STRUCT;
	McMSGBUseWDynTorqLimType : STRUCT (*Type mcMSG_USE_W_DYN_TORQ_LIM settings*)
		GearRatio : McCfgGearBoxType; (*Ratio between a gearbox input and output*)
		MaximumInputSpeed : REAL; (*Maximum permissible speed at gearbox input [rpm]*)
		NominalOutputTorque : REAL; (*Nominal torque at gearbox output [Nm]*)
		PeakOutputTorque : REAL; (*Peak torque at gearbox output [Nm]*)
		MomentOfInertia : REAL; (*Moment of inertia for the gearbox at gearbox input [kgcm²]*)
		ViscousFriction : REAL; (*Speed dependent friction at gearbox input [torque per rotations/secound] [Nms]*)
		StaticFriction : REAL; (*Static friction at gearbox input [Nm]*)
	END_STRUCT;
	McMSGBType : STRUCT (*Gearbox*)
		Type : McMSGBEnum; (*Gearbox selector setting*)
		Used : McMSGBUseType; (*Type mcMSG_USE settings*)
		UsedWithDynamicTorqueLimitation : McMSGBUseWDynTorqLimType; (*Type mcMSG_USE_W_DYN_TORQ_LIM settings*)
	END_STRUCT;
	McCfgMotSynType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_MOT_SYN*)
		Motor : McMSMotType;
		Brake : McMSBrkType; (*Holding brake*)
		Encoder : McMSEncType; (*Motor encoder*)
		Gearbox : McMSGBType; (*Gearbox*)
	END_STRUCT;
	McMIMotEnum :
		( (*Motor selector setting*)
		mcMIM_PWR_RTG_PLT := 0, (*Power rating plate - Data used for calculation and identification of motor data*)
		mcMIM_STAR_EQ_CIR := 1 (*Star equivalent circuit -*)
		);
	McMIMotPwrRtgPltOptParType : STRUCT
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		MaximumSpeed : REAL; (*Maximum permissible speed [rpm]*)
		MaximumDCBusVoltage : REAL; (*Maximum permissible DC bus voltage [V]*)
		StallCurrent : REAL; (*Phase current for generating the stall torque (RMS value) [A]*)
		PeakCurrent : REAL; (*Phase current for generating the peak torque (RMS value) [A]*)
		MagnetizingCurrent : REAL; (*Flux-generating current (RMS value, not needed if power factor is available) [A]*)
		NominalPower : REAL; (*Nominal power (not needed if power factor or magnetizing current is available) [kW]*)
		NominalTorque : REAL; (*Motor torque at nominal current [Nm]*)
		StallTorque : REAL; (*Motor torque at stall current [Nm]*)
		PeakTorque : REAL; (*Motor torque at peak current [Nm]*)
		MomentOfInertia : REAL; (*Mass moment of inertia [kgcm²]*)
	END_STRUCT;
	McMIMotPwrRtgPltType : STRUCT (*Type mcMIM_PWR_RTG_PLT settings*)
		NominalSpeed : REAL; (*Nominal speed [rpm]*)
		NominalFrequency : REAL; (*Nominal frequency [Hz]*)
		NominalVoltage : REAL; (*Nominal voltage (RMS value, phase-phase) [V]*)
		NominalCurrent : REAL; (*Phase current for generating the nominal torque at nominal speed (RMS value) [A]*)
		PowerFactor : REAL; (*Power factor (cos φ)*)
		NominalAmbientTemperature : REAL; (*Nominal ambient temperature [°C]*)
		OptionalParameter : McMIMotPwrRtgPltOptParType;
		TemperatureSensor : McMMTmpSensType; (*Temperature sensor configuration*)
		TemperatureModel : McMMTmpMdlType; (*Model for winding temperature monitoring*)
	END_STRUCT;
	McMIMotStarEqCirVLimEnum :
		( (*Voltage limitation selector setting*)
		mcMIMSECVL_NOT_USE := 0, (*Not used -*)
		mcMIMSECVL_USE := 1 (*Used -*)
		);
	McMIMotStarEqCirVLimUseType : STRUCT (*Type mcMIMSECVL_USE settings*)
		MaximumDCBusVoltage : REAL; (*Maximum permissible DC bus voltage [V]*)
	END_STRUCT;
	McMIMotStarEqCirVLimType : STRUCT (*Voltage limitation (Motor has no protective conductor or windings should be protected)*)
		Type : McMIMotStarEqCirVLimEnum; (*Voltage limitation selector setting*)
		Used : McMIMotStarEqCirVLimUseType; (*Type mcMIMSECVL_USE settings*)
	END_STRUCT;
	McMIMotStarEqCirType : STRUCT (*Type mcMIM_STAR_EQ_CIR settings*)
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		NominalSpeed : REAL; (*Nominal speed [rpm]*)
		MaximumSpeed : REAL; (*Maximum permissible speed [rpm]*)
		NominalVoltage : REAL; (*Nominal voltage (RMS value, phase-phase) [V]*)
		NominalCurrent : REAL; (*Phase current for generating the nominal torque at nominal speed (RMS value) [A]*)
		StallCurrent : REAL; (*Phase current for generating the stall torque (RMS value) [A]*)
		PeakCurrent : REAL; (*Phase current for generating the peak torque (RMS value) [A]*)
		MagnetizingCurrent : REAL; (*Flux-generating current (RMS value) [A]*)
		NominalTorque : REAL; (*Motor torque at nominal current [Nm]*)
		StallTorque : REAL; (*Motor torque at stall current [Nm]*)
		PeakTorque : REAL; (*Motor torque at peak current [Nm]*)
		StatorResistance : REAL; (*Stator resistance (phase) [Ω]*)
		RotorResistance : REAL; (*Rotor resistance (phase) [Ω]*)
		StatorInductance : REAL; (*Stator leakage inductance (phase) [mH]*)
		RotorInductance : REAL; (*Rotor leakage inductance (phase) [mH]*)
		MutualInductance : REAL; (*Mutual inductance (phase) [mH]*)
		MomentOfInertia : REAL; (*Mass moment of inertia [kgcm²]*)
		NominalAmbientTemperature : REAL; (*Nominal ambient temperature [°C]*)
		VoltageLimitation : McMIMotStarEqCirVLimType; (*Voltage limitation (Motor has no protective conductor or windings should be protected)*)
		TemperatureSensor : McMMTmpSensType; (*Temperature sensor configuration*)
		TemperatureModel : McMMTmpMdlType; (*Model for winding temperature monitoring*)
	END_STRUCT;
	McMIMotType : STRUCT
		Type : McMIMotEnum; (*Motor selector setting*)
		PowerRatingPlate : McMIMotPwrRtgPltType; (*Type mcMIM_PWR_RTG_PLT settings*)
		StarEquivalentCircuit : McMIMotStarEqCirType; (*Type mcMIM_STAR_EQ_CIR settings*)
	END_STRUCT;
	McMIBrkEnum :
		( (*Brake selector setting*)
		mcMIB_NOT_USE := 0, (*Not used -*)
		mcMIB_USE := 1 (*Used -*)
		);
	McMIBrkUseCtrlModEnum :
		( (*Control mode selector setting*)
		mcMIBUCM_SW := 0, (*Switched -*)
		mcMIBUCM_V_CTRL := 1 (*Voltage controlled -*)
		);
	McMIBrkUseCtrlModVCtrlType : STRUCT (*Type mcMIBUCM_V_CTRL settings*)
		ReleaseVoltage : REAL; (*Nominal voltage, to release (open) the holding brake [V]*)
		HoldVoltage : REAL; (*Nominal voltage to ensure the holding brake remains open [V]*)
	END_STRUCT;
	McMIBrkUseCtrlModType : STRUCT (*Behaviour of holding brake control*)
		Type : McMIBrkUseCtrlModEnum; (*Control mode selector setting*)
		VoltageControlled : McMIBrkUseCtrlModVCtrlType; (*Type mcMIBUCM_V_CTRL settings*)
	END_STRUCT;
	McMIBrkUseLimEnum :
		( (*Limits selector setting*)
		mcMIBUL_NOT_USE := 0, (*Not used -*)
		mcMIBUL_USE := 1 (*Used -*)
		);
	McMIBrkUseLimUseType : STRUCT (*Type mcMIBUL_USE settings*)
		MaximumVoltage : REAL; (*Maximum permissible voltage to release (open) the holding brake [V]*)
		PermittedFrictionWork : REAL; (*Permitted friction work up to the waer limit [J]*)
	END_STRUCT;
	McMIBrkUseLimType : STRUCT (*Holding brake limits*)
		Type : McMIBrkUseLimEnum; (*Limits selector setting*)
		Used : McMIBrkUseLimUseType; (*Type mcMIBUL_USE settings*)
	END_STRUCT;
	McMIBrkUseType : STRUCT (*Type mcMIB_USE settings*)
		NominalCurrent : REAL; (*Current of the holding brake [A]*)
		NominalTorque : REAL; (*Minimum holding torque of the holding brake [Nm]*)
		ActivationDelay : REAL; (*Holding torque build-up time after switching off the operating voltage [s]*)
		ReleaseDelay : REAL; (*Holding torque decaying time after switching on the operating voltage [s]*)
		MomentOfInertia : REAL; (*Moment of inertia for the holding brake [kgcm²]*)
		ControlMode : McMIBrkUseCtrlModType; (*Behaviour of holding brake control*)
		Limits : McMIBrkUseLimType; (*Holding brake limits*)
	END_STRUCT;
	McMIBrkType : STRUCT (*Holding brake*)
		Type : McMIBrkEnum; (*Brake selector setting*)
		Used : McMIBrkUseType; (*Type mcMIB_USE settings*)
	END_STRUCT;
	McMIEncEnum :
		( (*Encoder selector setting*)
		mcMIE_NOT_USE := 0, (*Not used -*)
		mcMIE_USE := 1 (*Used -*)
		);
	McMIEncUseTmpSensEnum :
		( (*Temperature sensor selector setting*)
		mcMIEUTS_NOT_USE := 0, (*Not used -*)
		mcMIEUTS_USE := 1 (*Used -*)
		);
	McMIEncUseTmpSensUseType : STRUCT (*Type mcMIEUTS_USE settings*)
		LimitTemperature : UINT; (*Maximum permissible encoder temperature [°C]*)
	END_STRUCT;
	McMIEncUseTmpSensType : STRUCT (*Encoder temperature sensor*)
		Type : McMIEncUseTmpSensEnum; (*Temperature sensor selector setting*)
		Used : McMIEncUseTmpSensUseType; (*Type mcMIEUTS_USE settings*)
	END_STRUCT;
	McMIEncUseType : STRUCT (*Type mcMIE_USE settings*)
		MomentOfInertia : REAL; (*Moment of inertia for the encoder [kgcm²]*)
		TemperatureSensor : McMIEncUseTmpSensType; (*Encoder temperature sensor*)
	END_STRUCT;
	McMIEncType : STRUCT (*Motor encoder*)
		Type : McMIEncEnum; (*Encoder selector setting*)
		Used : McMIEncUseType; (*Type mcMIE_USE settings*)
	END_STRUCT;
	McMIGBEnum :
		( (*Gearbox selector setting*)
		mcMIG_NOT_USE := 0, (*Not used -*)
		mcMIG_USE := 1, (*Used -*)
		mcMIG_USE_W_DYN_TORQ_LIM := 2 (*Used with dynamic torque limitation -*)
		);
	McMIGBUseType : STRUCT (*Type mcMIG_USE settings*)
		GearRatio : McCfgGearBoxType; (*Ratio between a gearbox input and output*)
		MaximumInputSpeed : REAL; (*Maximum permissible speed at gearbox input [rpm]*)
		NominalOutputTorque : REAL; (*Nominal torque at gearbox output [Nm]*)
		PeakOutputTorque : REAL; (*Peak torque at gearbox output [Nm]*)
		MomentOfInertia : REAL; (*Moment of inertia for the gearbox at gearbox input [kgcm²]*)
	END_STRUCT;
	McMIGBUseWDynTorqLimType : STRUCT (*Type mcMIG_USE_W_DYN_TORQ_LIM settings*)
		GearRatio : McCfgGearBoxType; (*Ratio between a gearbox input and output*)
		MaximumInputSpeed : REAL; (*Maximum permissible speed at gearbox input [rpm]*)
		NominalOutputTorque : REAL; (*Nominal torque at gearbox output [Nm]*)
		PeakOutputTorque : REAL; (*Peak torque at gearbox output [Nm]*)
		MomentOfInertia : REAL; (*Moment of inertia for the gearbox at gearbox input [kgcm²]*)
		ViscousFriction : REAL; (*Speed dependent friction at gearbox input [torque per rotations/secound] [Nms]*)
		StaticFriction : REAL; (*Static friction at gearbox input [Nm]*)
	END_STRUCT;
	McMIGBType : STRUCT (*Gearbox*)
		Type : McMIGBEnum; (*Gearbox selector setting*)
		Used : McMIGBUseType; (*Type mcMIG_USE settings*)
		UsedWithDynamicTorqueLimitation : McMIGBUseWDynTorqLimType; (*Type mcMIG_USE_W_DYN_TORQ_LIM settings*)
	END_STRUCT;
	McCfgMotInductType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_MOT_INDUCT*)
		Motor : McMIMotType;
		Brake : McMIBrkType; (*Holding brake*)
		Encoder : McMIEncType; (*Motor encoder*)
		Gearbox : McMIGBType; (*Gearbox*)
	END_STRUCT;
	McMSAMCMotEnum :
		( (*Motor selector setting*)
		mcMSAMCM_DEF := 0 (*Default -*)
		);
	McMSAMCMotDefVLimEnum :
		( (*Voltage limitation selector setting*)
		mcMSAMCMDVL_NOT_USE := 0, (*Not used -*)
		mcMSAMCMDVL_USE := 1 (*Used -*)
		);
	McMSAMCMotDefVLimUseType : STRUCT (*Type mcMSAMCMDVL_USE settings*)
		MaximumDCBusVoltage : REAL; (*Maximum permissible DC bus voltage [V]*)
	END_STRUCT;
	McMSAMCMotDefVLimType : STRUCT (*Voltage limitation (Motor has no protective conductor or windings should be protected)*)
		Type : McMSAMCMotDefVLimEnum; (*Voltage limitation selector setting*)
		Used : McMSAMCMotDefVLimUseType; (*Type mcMSAMCMDVL_USE settings*)
	END_STRUCT;
	McMSAMCMotDefEncMntType : STRUCT (*Encoder mounting*)
		Angle : McMMDEMAngType; (*Angle between motor encoder zero point and flux space vector*)
	END_STRUCT;
	McMSAMCMotDefType : STRUCT (*Type mcMSAMCM_DEF settings*)
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		NominalSpeed : REAL; (*Nominal speed [rpm]*)
		MaximumSpeed : REAL; (*Maximum permissible speed [rpm]*)
		NominalVoltage : REAL; (*Nominal voltage (RMS value, phase-phase) [V]*)
		NominalCurrent : REAL; (*Phase current for generating the nominal torque at nominal speed (RMS value) [A]*)
		StallCurrent : REAL; (*Phase current for generating the stall torque (RMS value) [A]*)
		PeakCurrent : REAL; (*Phase current for generating the peak torque (RMS value) [A]*)
		NominalTorque : REAL; (*Motor torque at nominal current [Nm]*)
		StallTorque : REAL; (*Motor torque at stall current [Nm]*)
		PeakTorque : REAL; (*Motor torque at peak current [Nm]*)
		VoltageConstant : REAL; (*Induced voltage per speed (RMS value of voltage at 1000 rpm, phase-phase) [mV/rpm]*)
		TorqueConstant : REAL; (*Torque constant [Nm/A]*)
		StatorResistance : REAL; (*Stator resistance (phase-phase) [Ω]*)
		StatorInductance : REAL; (*Stator inductance (phase-phase) [mH]*)
		MomentOfInertia : REAL; (*Mass moment of inertia [kgcm²]*)
		NominalAmbientTemperature : REAL; (*Nominal ambient temperature [°C]*)
		VoltageLimitation : McMSAMCMotDefVLimType; (*Voltage limitation (Motor has no protective conductor or windings should be protected)*)
		EncoderMounting : McMSAMCMotDefEncMntType; (*Encoder mounting*)
		TemperatureSensor : McMMTmpSensType; (*Temperature sensor configuration*)
		TemperatureModel : McMMTmpMdlType; (*Model for winding temperature monitoring*)
	END_STRUCT;
	McMSAMCMotType : STRUCT
		Type : McMSAMCMotEnum; (*Motor selector setting*)
		Default : McMSAMCMotDefType; (*Type mcMSAMCM_DEF settings*)
	END_STRUCT;
	McMSAMCBrkEnum :
		( (*Brake selector setting*)
		mcMSAMCB_NOT_USE := 0, (*Not used -*)
		mcMSAMCB_USE := 1 (*Used -*)
		);
	McMSAMCBrkUseCtrlModEnum :
		( (*Control mode selector setting*)
		mcMSAMCBUCM_SW := 0, (*Switched -*)
		mcMSAMCBUCM_V_CTRL := 1 (*Voltage controlled -*)
		);
	McMSAMCBrkUseCtrlModVCtrlType : STRUCT (*Type mcMSAMCBUCM_V_CTRL settings*)
		ReleaseVoltage : REAL; (*Nominal voltage, to release (open) the holding brake [V]*)
		HoldVoltage : REAL; (*Nominal voltage to ensure the holding brake remains open [V]*)
	END_STRUCT;
	McMSAMCBrkUseCtrlModType : STRUCT (*Behaviour of holding brake control*)
		Type : McMSAMCBrkUseCtrlModEnum; (*Control mode selector setting*)
		VoltageControlled : McMSAMCBrkUseCtrlModVCtrlType; (*Type mcMSAMCBUCM_V_CTRL settings*)
	END_STRUCT;
	McMSAMCBrkUseLimEnum :
		( (*Limits selector setting*)
		mcMSAMCBUL_NOT_USE := 0, (*Not used -*)
		mcMSAMCBUL_USE := 1 (*Used -*)
		);
	McMSAMCBrkUseLimUseType : STRUCT (*Type mcMSAMCBUL_USE settings*)
		MaximumVoltage : REAL; (*Maximum permissible voltage to release (open) the holding brake [V]*)
		PermittedFrictionWork : REAL; (*Permitted friction work up to the waer limit [J]*)
	END_STRUCT;
	McMSAMCBrkUseLimType : STRUCT (*Holding brake limits*)
		Type : McMSAMCBrkUseLimEnum; (*Limits selector setting*)
		Used : McMSAMCBrkUseLimUseType; (*Type mcMSAMCBUL_USE settings*)
	END_STRUCT;
	McMSAMCBrkUseType : STRUCT (*Type mcMSAMCB_USE settings*)
		NominalCurrent : REAL; (*Current of the holding brake [A]*)
		NominalTorque : REAL; (*Minimum holding torque of the holding brake [Nm]*)
		ActivationDelay : REAL; (*Holding torque build-up time after switching off the operating voltage [s]*)
		ReleaseDelay : REAL; (*Holding torque decaying time after switching on the operating voltage [s]*)
		MomentOfInertia : REAL; (*Moment of inertia for the holding brake [kgcm²]*)
		ControlMode : McMSAMCBrkUseCtrlModType; (*Behaviour of holding brake control*)
		Limits : McMSAMCBrkUseLimType; (*Holding brake limits*)
	END_STRUCT;
	McMSAMCBrkType : STRUCT (*Holding brake*)
		Type : McMSAMCBrkEnum; (*Brake selector setting*)
		Used : McMSAMCBrkUseType; (*Type mcMSAMCB_USE settings*)
	END_STRUCT;
	McMSAMCEncEnum :
		( (*Encoder selector setting*)
		mcMSAMCE_NOT_USE := 0, (*Not used -*)
		mcMSAMCE_USE := 1 (*Used -*)
		);
	McMSAMCEncUseTmpSensEnum :
		( (*Temperature sensor selector setting*)
		mcMSAMCEUTS_NOT_USE := 0, (*Not used -*)
		mcMSAMCEUTS_USE := 1 (*Used -*)
		);
	McMSAMCEncUseTmpSensUseType : STRUCT (*Type mcMSAMCEUTS_USE settings*)
		LimitTemperature : UINT; (*Maximum permissible encoder temperature [°C]*)
	END_STRUCT;
	McMSAMCEncUseTmpSensType : STRUCT (*Encoder temperature sensor*)
		Type : McMSAMCEncUseTmpSensEnum; (*Temperature sensor selector setting*)
		Used : McMSAMCEncUseTmpSensUseType; (*Type mcMSAMCEUTS_USE settings*)
	END_STRUCT;
	McMSAMCEncUseType : STRUCT (*Type mcMSAMCE_USE settings*)
		MomentOfInertia : REAL; (*Moment of inertia for the encoder [kgcm²]*)
		TemperatureSensor : McMSAMCEncUseTmpSensType; (*Encoder temperature sensor*)
	END_STRUCT;
	McMSAMCEncType : STRUCT (*Motor encoder*)
		Type : McMSAMCEncEnum; (*Encoder selector setting*)
		Used : McMSAMCEncUseType; (*Type mcMSAMCE_USE settings*)
	END_STRUCT;
	McMSAMCGBEnum :
		( (*Gearbox selector setting*)
		mcMSAMCG_NOT_USE := 0, (*Not used -*)
		mcMSAMCG_USE := 1, (*Used -*)
		mcMSAMCG_USE_W_DYN_TORQ_LIM := 2 (*Used with dynamic torque limitation -*)
		);
	McMSAMCGBUseType : STRUCT (*Type mcMSAMCG_USE settings*)
		GearRatio : McCfgGearBoxType; (*Ratio between a gearbox input and output*)
		MaximumInputSpeed : REAL; (*Maximum permissible speed at gearbox input [rpm]*)
		NominalOutputTorque : REAL; (*Nominal torque at gearbox output [Nm]*)
		PeakOutputTorque : REAL; (*Peak torque at gearbox output [Nm]*)
		MomentOfInertia : REAL; (*Moment of inertia for the gearbox at gearbox input [kgcm²]*)
	END_STRUCT;
	McMSAMCGBUseWDynTorqLimType : STRUCT (*Type mcMSAMCG_USE_W_DYN_TORQ_LIM settings*)
		GearRatio : McCfgGearBoxType; (*Ratio between a gearbox input and output*)
		MaximumInputSpeed : REAL; (*Maximum permissible speed at gearbox input [rpm]*)
		NominalOutputTorque : REAL; (*Nominal torque at gearbox output [Nm]*)
		PeakOutputTorque : REAL; (*Peak torque at gearbox output [Nm]*)
		MomentOfInertia : REAL; (*Moment of inertia for the gearbox at gearbox input [kgcm²]*)
		ViscousFriction : REAL; (*Speed dependent friction at gearbox input [torque per rotations/secound] [Nms]*)
		StaticFriction : REAL; (*Static friction at gearbox input [Nm]*)
	END_STRUCT;
	McMSAMCGBType : STRUCT (*Gearbox*)
		Type : McMSAMCGBEnum; (*Gearbox selector setting*)
		Used : McMSAMCGBUseType; (*Type mcMSAMCG_USE settings*)
		UsedWithDynamicTorqueLimitation : McMSAMCGBUseWDynTorqLimType; (*Type mcMSAMCG_USE_W_DYN_TORQ_LIM settings*)
	END_STRUCT;
	McCfgMotSynAmcType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_MOT_SYN_AMC*)
		Motor : McMSAMCMotType;
		Brake : McMSAMCBrkType; (*Holding brake*)
		Encoder : McMSAMCEncType; (*Motor encoder*)
		Gearbox : McMSAMCGBType; (*Gearbox*)
	END_STRUCT;
	McMBLDCMotEnum :
		( (*Motor selector setting*)
		mcMBLDCM_DEF := 0 (*Default -*)
		);
	McMSBEMAngEnum :
		( (*Angle selector setting*)
		mcMBLDCMDEMA_USRDEF := 0, (*User-defined -*)
		mcMBLDCMDEMA_UDEF := 1 (*Undefined -*)
		);
	McMSBEMAngUsrDefType : STRUCT (*Type mcMBLDCMDEMA_USRDEF settings*)
		CommutationOffset : REAL; (*Angle between motor encoder zero point and flux space vector [rad]*)
	END_STRUCT;
	McMSBEMAngUdefAutIdentEnum :
		( (*Automatic identification selector setting*)
		mcMBLDCMDEMAUAI_NOT_USE := 0, (*Not used -*)
		mcMBLDCMDEMAUAI_SAT := 1, (*Saturation -*)
		mcMBLDCMDEMAUAI_DIT := 2, (*Dither -*)
		mcMBLDCMDEMAUAI_DIT2 := 5 (*Dither2 -*)
		);
	McMSBEMAngUdefAutIdentSatType : STRUCT (*Type mcMBLDCMDEMAUAI_SAT settings*)
		PhasingCurrent : REAL; (*Current for identification (optional) [A]*)
	END_STRUCT;
	McMSBEMAngUdefAutIdentDitType : STRUCT (*Type mcMBLDCMDEMAUAI_DIT settings*)
		PhasingCurrent : REAL; (*Current for identification (optional) [A]*)
		PhasingTime : REAL; (*Duration of identification (optional) [s]*)
	END_STRUCT;
	McMSBEMAngUdefAutIdentDit2Type : STRUCT (*Type mcMBLDCMDEMAUAI_DIT2 settings*)
		PhasingCurrent : REAL; (*Current for identification (optional) [A]*)
		PhasingTime : REAL; (*Duration of identification (optional) [s]*)
	END_STRUCT;
	McMSBEMAngUdefAutIdentType : STRUCT (*Automatic identification of the angle when switching on the controller*)
		Type : McMSBEMAngUdefAutIdentEnum; (*Automatic identification selector setting*)
		Saturation : McMSBEMAngUdefAutIdentSatType; (*Type mcMBLDCMDEMAUAI_SAT settings*)
		Dither : McMSBEMAngUdefAutIdentDitType; (*Type mcMBLDCMDEMAUAI_DIT settings*)
		Dither2 : McMSBEMAngUdefAutIdentDit2Type; (*Type mcMBLDCMDEMAUAI_DIT2 settings*)
	END_STRUCT;
	McMSBEMAngUdefType : STRUCT (*Type mcMBLDCMDEMA_UDEF settings*)
		AutomaticIdentification : McMSBEMAngUdefAutIdentType; (*Automatic identification of the angle when switching on the controller*)
	END_STRUCT;
	McMSBEMAngType : STRUCT (*Angle between motor encoder zero point and flux space vector*)
		Type : McMSBEMAngEnum; (*Angle selector setting*)
		UserDefined : McMSBEMAngUsrDefType; (*Type mcMBLDCMDEMA_USRDEF settings*)
		Undefined : McMSBEMAngUdefType; (*Type mcMBLDCMDEMA_UDEF settings*)
	END_STRUCT;
	McMBLDCMotDefEncMntType : STRUCT (*Encoder mounting*)
		Angle : McMSBEMAngType; (*Angle between motor encoder zero point and flux space vector*)
	END_STRUCT;
	McMMSBTmpMdlEnum :
		( (*Temperature model selector setting*)
		mcMMSBTM_CURBASED := 0, (*Current-based -*)
		mcMMSBTM_NOT_USE := 1 (*Not used -*)
		);
	McMMSBTMCurBsdType : STRUCT (*Type mcMMSBTM_CURBASED settings*)
		LimitTemperature : REAL; (*Maximum permissible winding temperature [°C]*)
		WindingCrossSection : REAL; (*Phase conductor cross section [mm²]*)
	END_STRUCT;
	McMMSBTmpMdlType : STRUCT (*Model for winding temperature monitoring*)
		Type : McMMSBTmpMdlEnum; (*Temperature model selector setting*)
		CurrentBased : McMMSBTMCurBsdType; (*Type mcMMSBTM_CURBASED settings*)
	END_STRUCT;
	McMBLDCMotDefType : STRUCT (*Type mcMBLDCM_DEF settings*)
		NumberOfPolePairs : USINT; (*Number of pole pairs*)
		NominalSpeed : REAL; (*Nominal speed [rpm]*)
		MaximumSpeed : REAL; (*Maximum permissible speed [rpm]*)
		NominalCurrent : REAL; (*Phase current for generating the nominal torque at nominal speed (RMS value) [A]*)
		StallCurrent : REAL; (*Phase current for generating the stall torque (RMS value) [A]*)
		PeakCurrent : REAL; (*Phase current for generating the peak torque (RMS value) [A]*)
		NominalTorque : REAL; (*Motor torque at nominal current [Nm]*)
		StallTorque : REAL; (*Motor torque at stall current [Nm]*)
		PeakTorque : REAL; (*Motor torque at peak current [Nm]*)
		VoltageConstant : REAL; (*Induced voltage per speed (RMS value of voltage at 1000 rpm, phase-phase) [mV/rpm]*)
		TorqueConstant : REAL; (*Torque constant [Nm/A]*)
		StatorResistance : REAL; (*Stator resistance (phase-phase) [Ω]*)
		StatorInductance : REAL; (*Stator inductance (phase-phase) [mH]*)
		MomentOfInertia : REAL; (*Mass moment of inertia [kgcm²]*)
		EncoderMounting : McMBLDCMotDefEncMntType; (*Encoder mounting*)
		TemperatureModel : McMMSBTmpMdlType; (*Model for winding temperature monitoring*)
	END_STRUCT;
	McMBLDCMotType : STRUCT
		Type : McMBLDCMotEnum; (*Motor selector setting*)
		Default : McMBLDCMotDefType; (*Type mcMBLDCM_DEF settings*)
	END_STRUCT;
	McMBLDCBrkEnum :
		( (*Brake selector setting*)
		mcMBLDCB_NOT_USE := 0, (*Not used -*)
		mcMBLDCB_USE := 1 (*Used -*)
		);
	McMBLDCBrkUseType : STRUCT (*Type mcMBLDCB_USE settings*)
		NominalCurrent : REAL; (*Current of the holding brake [A]*)
		NominalTorque : REAL; (*Minimum holding torque of the holding brake [Nm]*)
		ActivationDelay : REAL; (*Holding torque build-up time after switching off the operating voltage [s]*)
		ReleaseDelay : REAL; (*Holding torque decaying time after switching on the operating voltage [s]*)
		MomentOfInertia : REAL; (*Moment of inertia for the holding brake [kgcm²]*)
	END_STRUCT;
	McMBLDCBrkType : STRUCT (*Holding brake*)
		Type : McMBLDCBrkEnum; (*Brake selector setting*)
		Used : McMBLDCBrkUseType; (*Type mcMBLDCB_USE settings*)
	END_STRUCT;
	McCfgMotBLDCType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_MOT_BLDC*)
		Motor : McMBLDCMotType;
		Brake : McMBLDCBrkType; (*Holding brake*)
	END_STRUCT;
	McMSTEPMotEnum :
		( (*Motor selector setting*)
		mcMSTEPM_DEF := 0, (*Default -*)
		mcMSTEPM_SIMPLE := 1 (*Simple - Only Encoderless current control possible*)
		);
	McMSTEPMotDefEncMntType : STRUCT (*Encoder mounting*)
		Angle : McMSBEMAngType; (*Angle between motor encoder zero point and flux space vector*)
	END_STRUCT;
	McMSTEPMotDefType : STRUCT (*Type mcMSTEPM_DEF settings*)
		StepAngle : REAL; (*Step angle [°]*)
		NominalSpeed : REAL; (*Nominal speed [rpm]*)
		MaximumSpeed : REAL; (*Maximum permissible speed [rpm]*)
		NominalCurrent : REAL; (*Phase current for generating the nominal torque at nominal speed (RMS value) [A]*)
		ContinuousStallCurrent : REAL; (*Phase current for generating the holding torque (RMS value) [A]*)
		PeakCurrent : REAL; (*Phase current for generating the peak torque (RMS value) [A]*)
		NominalTorque : REAL; (*Motor torque at nominal current [Nm]*)
		HoldingTorque : REAL; (*Motor torque at continuous stall current [Nm]*)
		PeakTorque : REAL; (*Motor torque at peak current [Nm]*)
		VoltageConstant : REAL; (*Induced voltage per speed (RMS value of voltage at 1000 rpm, phase-phase) [mV/rpm]*)
		TorqueConstant : REAL; (*Torque constant [Nm/A]*)
		StatorResistance : REAL; (*Stator resistance (phase-phase) [Ω]*)
		StatorInductance : REAL; (*Stator inductance (phase-phase) [mH]*)
		MomentOfInertia : REAL; (*Mass moment of inertia [kgcm²]*)
		EncoderMounting : McMSTEPMotDefEncMntType; (*Encoder mounting*)
		TemperatureModel : McMMSBTmpMdlType; (*Model for winding temperature monitoring*)
	END_STRUCT;
	McMSTEPMotSimpleEncMntType : STRUCT (*Encoder mounting*)
		Angle : McMSBEMAngType; (*Angle between motor encoder zero point and flux space vector*)
	END_STRUCT;
	McMSTEPMotSimpleType : STRUCT (*Type mcMSTEPM_SIMPLE settings*)
		StepAngle : REAL; (*Step angle [°]*)
		MaximumSpeed : REAL; (*Maximum permissible speed [rpm]*)
		ContinuousCurrent : REAL; (*Phase current (RMS value) that does not overheat the motor, required for temperature model [A]*)
		PeakCurrent : REAL; (*Maximum phase current (RMS value) [A]*)
		HoldingTorque : REAL; (*Motor torque at continuous stall current [Nm]*)
		StatorResistance : REAL; (*Stator resistance (phase-phase) [Ω]*)
		StatorInductance : REAL; (*Stator inductance (phase-phase) [mH]*)
		MomentOfInertia : REAL; (*Mass moment of inertia [kgcm²]*)
		EncoderMounting : McMSTEPMotSimpleEncMntType; (*Encoder mounting*)
		TemperatureModel : McMMSBTmpMdlType; (*Model for winding temperature monitoring*)
	END_STRUCT;
	McMSTEPMotType : STRUCT
		Type : McMSTEPMotEnum; (*Motor selector setting*)
		Default : McMSTEPMotDefType; (*Type mcMSTEPM_DEF settings*)
		Simple : McMSTEPMotSimpleType; (*Type mcMSTEPM_SIMPLE settings*)
	END_STRUCT;
	McMSTEPBrkEnum :
		( (*Brake selector setting*)
		mcMSTEPB_NOT_USE := 0, (*Not used -*)
		mcMSTEPB_USE := 1 (*Used -*)
		);
	McMSTEPBrkUseType : STRUCT (*Type mcMSTEPB_USE settings*)
		NominalCurrent : REAL; (*Current of the holding brake [A]*)
		NominalTorque : REAL; (*Minimum holding torque of the holding brake [Nm]*)
		ActivationDelay : REAL; (*Holding torque build-up time after switching off the operating voltage [s]*)
		ReleaseDelay : REAL; (*Holding torque decaying time after switching on the operating voltage [s]*)
		MomentOfInertia : REAL; (*Moment of inertia for the holding brake [kgcm²]*)
	END_STRUCT;
	McMSTEPBrkType : STRUCT (*Holding brake*)
		Type : McMSTEPBrkEnum; (*Brake selector setting*)
		Used : McMSTEPBrkUseType; (*Type mcMSTEPB_USE settings*)
	END_STRUCT;
	McCfgMotStepType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_MOT_STEP*)
		Motor : McMSTEPMotType;
		Brake : McMSTEPBrkType; (*Holding brake*)
	END_STRUCT;
	McAPICEIfTypEnum :
		( (*Interface type selector setting*)
		mcAPICEIT_NOT_USE := 0, (*Not used -*)
		mcAPICEIT_ENDAT := 1, (*EnDat -*)
		mcAPICEIT_SIN := 2, (*Sine -*)
		mcAPICEIT_SSI_SIN := 3, (*SSI sine -*)
		mcAPICEIT_INCR := 4, (*Incremental -*)
		mcAPICEIT_INCR_W_DCM := 5, (*Incremental with DCM -*)
		mcAPICEIT_BISS := 6, (*BiSS -*)
		mcAPICEIT_SSI := 7, (*SSI -*)
		mcAPICEIT_RES := 8 (*Resolver -*)
		);
	McAPICEITSinType : STRUCT (*Type mcAPICEIT_SIN settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAPICEITSSISinFCStatBit1Enum :
		( (*Warning or error bit*)
		mcAPICEITSSCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAPICEITSSCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAPICEITSSCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAPICEITSSCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAPICEITSSCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAPICEITSSISinFCStatBit2Enum :
		( (*Warning or error bit*)
		mcAPICEITSSCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAPICEITSSCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAPICEITSSCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAPICEITSSCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAPICEITSSCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAPICEITSSISinFCSSIDatCEnum :
		( (*SSI data code*)
		mcAPICEITSSCSDC_GRAY := 0, (*Gray - Gray*)
		mcAPICEITSSCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAPICEITSSISinFCSSIParCkEnum :
		( (*SSI parity check*)
		mcAPICEITSSCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAPICEITSSCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAPICEITSSCSPC_OFF := 2 (*Off - Off*)
		);
	McAPICEITSSISinFCType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAPICEITSSISinFCStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAPICEITSSISinFCStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAPICEITSSISinFCSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAPICEITSSISinFCSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAPICEITSSISinType : STRUCT (*Type mcAPICEIT_SSI_SIN settings*)
		SSIFrameConfiguration : McAPICEITSSISinFCType; (*Frame structure in the order of transfer*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		PositionValuesEncoderRevolution : DINT; (*Absolute resolution of an encoder revolution*)
		SerialPositionPhaseShift : DINT; (*Serial position phase shift*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAPICEITIncrType : STRUCT (*Type mcAPICEIT_INCR settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAPICEITIncrWDCMType : STRUCT (*Type mcAPICEIT_INCR_W_DCM settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		DCMBasicDistance : UDINT; (*DCM basic distance*)
		DCMBasicDifference : DINT; (*DCM basic difference*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAPICEITBiSSFCStatBit1Enum :
		( (*Warning or error bit*)
		mcAPICEITBFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAPICEITBFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAPICEITBFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAPICEITBFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAPICEITBFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAPICEITBiSSFCStatBit2Enum :
		( (*Warning or error bit*)
		mcAPICEITBFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAPICEITBFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAPICEITBFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAPICEITBFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAPICEITBFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAPICEITBiSSFCType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAPICEITBiSSFCStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAPICEITBiSSFCStatBit2Enum; (*Warning or error bit*)
	END_STRUCT;
	McAPICEITBiSSType : STRUCT (*Type mcAPICEIT_BISS settings*)
		BiSSFrameConfiguration : McAPICEITBiSSFCType; (*Frame structure in the order of transfer*)
		CRCPolynomial : UDINT; (*Data verification via CRC (decimal value, 0 deactivates verification)*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
	END_STRUCT;
	McAPICEITSSIFCStatBit1Enum :
		( (*Warning or error bit*)
		mcAPICEITSCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAPICEITSCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAPICEITSCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAPICEITSCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAPICEITSCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAPICEITSSIFCStatBit2Enum :
		( (*Warning or error bit*)
		mcAPICEITSCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAPICEITSCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAPICEITSCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAPICEITSCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAPICEITSCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAPICEITSSIFCSSIDatCEnum :
		( (*SSI data code*)
		mcAPICEITSCSDC_GRAY := 0, (*Gray - Gray*)
		mcAPICEITSCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAPICEITSSIFCSSIParCkEnum :
		( (*SSI parity check*)
		mcAPICEITSCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAPICEITSCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAPICEITSCSPC_OFF := 2 (*Off - Off*)
		);
	McAPICEITSSIFCType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAPICEITSSIFCStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAPICEITSSIFCStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAPICEITSSIFCSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAPICEITSSIFCSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAPICEITSSIType : STRUCT (*Type mcAPICEIT_SSI settings*)
		SSIFrameConfiguration : McAPICEITSSIFCType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAPICAEITResType : STRUCT (*Type mcAPICEIT_RES settings*)
		ResolverPolepairs : USINT; (*Resolver polepairs per encoder revolution*)
		ResolverTransmissionRatio : REAL; (*Resolver transmission ratio*)
	END_STRUCT;
	McAPICEIfTypType : STRUCT (*Interface type of the encoder*)
		Type : McAPICEIfTypEnum; (*Interface type selector setting*)
		Sine : McAPICEITSinType; (*Type mcAPICEIT_SIN settings*)
		SSISine : McAPICEITSSISinType; (*Type mcAPICEIT_SSI_SIN settings*)
		Incremental : McAPICEITIncrType; (*Type mcAPICEIT_INCR settings*)
		IncrementalWithDCM : McAPICEITIncrWDCMType; (*Type mcAPICEIT_INCR_W_DCM settings*)
		BiSS : McAPICEITBiSSType; (*Type mcAPICEIT_BISS settings*)
		SSI : McAPICEITSSIType; (*Type mcAPICEIT_SSI settings*)
		Resolver : McAPICAEITResType; (*Type mcAPICEIT_RES settings*)
	END_STRUCT;
	McCfgAcpPlInCrdEncType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_PL_IN_CARD_ENC*)
		InterfaceType : McAPICEIfTypType; (*Interface type of the encoder*)
	END_STRUCT;
	McAMPICEIfTypEnum :
		( (*Interface type selector setting*)
		mcAMPICEIT_NOT_USE := 0, (*Not used -*)
		mcAMPICEIT_ENDAT := 1, (*EnDat -*)
		mcAMPICEIT_SIN := 2, (*Sine -*)
		mcAMPICEIT_SIN_W_DCM := 3, (*Sine with DCM -*)
		mcAMPICEIT_SSI_SIN := 4, (*SSI sine -*)
		mcAMPICEIT_HIPERFACE := 5, (*HIPERFACE -*)
		mcAMPICEIT_INCR := 6, (*Incremental -*)
		mcAMPICEIT_INCR_W_DCM := 7, (*Incremental with DCM -*)
		mcAMPICEIT_BISS := 8, (*BiSS -*)
		mcAMPICEIT_SSI := 9, (*SSI -*)
		mcAMPICEIT_RES := 10 (*Resolver -*)
		);
	McAMPICEITSinType : STRUCT (*Type mcAMPICEIT_SIN settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
	END_STRUCT;
	McAMPICEITSinWDCMType : STRUCT (*Type mcAMPICEIT_SIN_W_DCM settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		DCMBasicDistance : UDINT; (*DCM basic distance*)
		DCMBasicDifference : DINT; (*DCM basic difference*)
	END_STRUCT;
	McAMPICEITIncrMaxExpOutFreqEnum :
		( (*Maximal expected output frequency [Hz]*)
		mcAMPICEITIMEOF_MEOF_25000 := 0, (*MEOF 25000 - 25000 Hz*)
		mcAMPICEITIMEOF_MEOF_50000 := 1, (*MEOF 50000 - 50000 Hz*)
		mcAMPICEITIMEOF_MEOF_100000 := 2, (*MEOF 100000 - 100000 Hz*)
		mcAMPICEITIMEOF_MEOF_200000 := 3, (*MEOF 200000 - 200000 Hz*)
		mcAMPICEITIMEOF_MEOF_400000 := 4 (*MEOF 400000 - 400000 Hz*)
		);
	McAMPICEITIncrOutStgEnum :
		( (*Output stage*)
		mcAMPICEITIOS_PUSH_PULL := 0, (*Push pull - Push pull*)
		mcAMPICEITIOS_PULL := 1, (*Pull - Pull*)
		mcAMPICEITIOS_PUSH := 2 (*Push - Push*)
		);
	McAMPICEITIncrType : STRUCT (*Type mcAMPICEIT_INCR settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		MaxExpectedOutputFrequency : McAMPICEITIncrMaxExpOutFreqEnum; (*Maximal expected output frequency [Hz]*)
		OutputStage : McAMPICEITIncrOutStgEnum; (*Output stage*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAMPICEITIWDCMMaxExpOutFreqEnum :
		( (*Maximal expected output frequency [Hz]*)
		mcAMPICEITIWDCMMEOF_MEOF_25000 := 0, (*MEOF 25000 - 25000 Hz*)
		mcAMPICEITIWDCMMEOF_MEOF_50000 := 1, (*MEOF 50000 - 50000 Hz*)
		mcAMPICEITIWDCMMEOF_MEOF_100000 := 2, (*MEOF 100000 - 100000 Hz*)
		mcAMPICEITIWDCMMEOF_MEOF_200000 := 3, (*MEOF 200000 - 200000 Hz*)
		mcAMPICEITIWDCMMEOF_MEOF_400000 := 4 (*MEOF 400000 - 400000 Hz*)
		);
	McAMPICEITIWDCMOutStgEnum :
		( (*Output stage*)
		mcAMPICEITIWDCMOS_PUSH_PULL := 0, (*Push pull - Push pull*)
		mcAMPICEITIWDCMOS_PULL := 1, (*Pull - Pull*)
		mcAMPICEITIWDCMOS_PUSH := 2 (*Push - Push*)
		);
	McAMPICEITIWDCMType : STRUCT (*Type mcAMPICEIT_INCR_W_DCM settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		DCMBasicDistance : UDINT; (*DCM basic distance*)
		DCMBasicDifference : DINT; (*DCM basic difference*)
		MaxExpectedOutputFrequency : McAMPICEITIWDCMMaxExpOutFreqEnum; (*Maximal expected output frequency [Hz]*)
		OutputStage : McAMPICEITIWDCMOutStgEnum; (*Output stage*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAPICEITResType : STRUCT (*Type mcAMPICEIT_RES settings*)
		PolepairsPerEncoderRevolution : USINT; (*Resolver polepairs per encoder revolution*)
	END_STRUCT;
	McAMPICEIfTypType : STRUCT (*Interface type of the encoder*)
		Type : McAMPICEIfTypEnum; (*Interface type selector setting*)
		Sine : McAMPICEITSinType; (*Type mcAMPICEIT_SIN settings*)
		SineWithDCM : McAMPICEITSinWDCMType; (*Type mcAMPICEIT_SIN_W_DCM settings*)
		SSISine : McAPICEITSSISinType; (*Type mcAMPICEIT_SSI_SIN settings*)
		Incremental : McAMPICEITIncrType; (*Type mcAMPICEIT_INCR settings*)
		IncrementalWithDCM : McAMPICEITIWDCMType; (*Type mcAMPICEIT_INCR_W_DCM settings*)
		BiSS : McAPICEITBiSSType; (*Type mcAMPICEIT_BISS settings*)
		SSI : McAPICEITSSIType; (*Type mcAMPICEIT_SSI settings*)
		Resolver : McAPICEITResType; (*Type mcAMPICEIT_RES settings*)
	END_STRUCT;
	McCfgAcpMulPlInCrdEncType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_MUL_PL_IN_CARD_ENC*)
		InterfaceType : McAMPICEIfTypType; (*Interface type of the encoder*)
	END_STRUCT;
	McAPICEncXIfTypEnum :
		( (*Interface type selector setting*)
		mcAPICEXIT_NOT_USE := 0, (*Not used -*)
		mcAPICEXIT_ENDAT := 1, (*EnDat -*)
		mcAPICEXIT_SIN := 2, (*Sine -*)
		mcAPICEXIT_SIN_W_DCM := 3, (*Sine with DCM -*)
		mcAPICEXIT_SSI_SIN := 4, (*SSI sine -*)
		mcAPICEXIT_HIPERFACE := 5, (*HIPERFACE -*)
		mcAPICEXIT_INCR := 6, (*Incremental -*)
		mcAPICEXIT_BISS := 7, (*BiSS -*)
		mcAPICEXIT_SSI := 8, (*SSI -*)
		mcAPICEXIT_HIPERFACE_DSL := 9, (*HIPERFACE DSL -*)
		mcAPICEXIT_TFMT := 10, (*T-Format - Tamagawa digital interface*)
		mcAPICEXIT_RES := 11, (*Resolver -*)
		mcAPICEXIT_ENDAT_3 := 12 (*EnDat 3 -*)
		);
	McAP3PICEITEnDatType : STRUCT (*Type mcAPICEXIT_ENDAT settings*)
		LineResistance : REAL; (*Line resistance encoder supply [Ohm]*)
	END_STRUCT;
	McAP3PICEITSinType : STRUCT (*Type mcAPICEXIT_SIN settings*)
		LineResistance : REAL; (*Line resistance encoder supply [Ohm]*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
	END_STRUCT;
	McAP3PICEITSinWDCMType : STRUCT (*Type mcAPICEXIT_SIN_W_DCM settings*)
		LineResistance : REAL; (*Line resistance encoder supply [Ohm]*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		DCMBasicDistance : UDINT; (*DCM basic distance*)
		DCMBasicDifference : DINT; (*DCM basic difference*)
	END_STRUCT;
	McAP3PICEITSSISinType : STRUCT (*Type mcAPICEXIT_SSI_SIN settings*)
		LineResistance : REAL; (*Line resistance encoder supply [Ohm]*)
		SSIFrameConfiguration : McAPICEITSSISinFCType; (*Frame structure in the order of transfer*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		PositionValuesEncoderRevolution : DINT; (*Absolute resolution of an encoder revolution*)
		SerialPositionPhaseShift : DINT; (*Serial position phase shift*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAPICEITIncrPwrSupEnum :
		( (*Power supply selector setting*)
		mcAPICEITIPS_PWR_SUP_5V := 0, (*Power supply 5V -*)
		mcAPICEITIPS_PWR_SUP_12V := 1 (*Power supply 12V -*)
		);
	McAPICEITIPS5VLinResEnum :
		( (*Line resistance selector setting*)
		mcAPICEITIPS5VLR_SPEC_VAL := 0, (*Specify value -*)
		mcAPICEITIPS5VLR_CALC_VAL := 1 (*Calculate value -*)
		);
	McAPICEITIPS5VLinResSpecValType : STRUCT (*Type mcAPICEITIPS5VLR_SPEC_VAL settings*)
		Value : REAL; (*Resistance value of the encoder supply line (1 line) [Ohm]*)
	END_STRUCT;
	McAPICEITIPS5VLinResCalcValType : STRUCT (*Type mcAPICEITIPS5VLR_CALC_VAL settings*)
		Length : REAL; (*Length of the encoder supply line (1 line) [m]*)
		CrossSection : REAL; (*Cross section of the encoder supply line [mm²]*)
	END_STRUCT;
	McAPICEITIPS5VLinResType : STRUCT (*Resistance of the encoder supply line (1line)(Calculate value is only executed correct for copper lines)*)
		Type : McAPICEITIPS5VLinResEnum; (*Line resistance selector setting*)
		SpecifyValue : McAPICEITIPS5VLinResSpecValType; (*Type mcAPICEITIPS5VLR_SPEC_VAL settings*)
		CalculateValue : McAPICEITIPS5VLinResCalcValType; (*Type mcAPICEITIPS5VLR_CALC_VAL settings*)
	END_STRUCT;
	McAPICEITIPS5VSymEnum :
		( (*Symmetry selector setting*)
		mcAPICEITIPS5VS_SYM := 0, (*Symmetrical -*)
		mcAPICEITIPS5VS_ASYM := 1 (*Asymmetrical -*)
		);
	McAPICEITIPS5VSymSymLinTermEnum :
		( (*Line termination*)
		mcAPICEITIPS5VSSLT_ACT_RS422 := 0, (*Active (RS422) - Active (RS422)*)
		mcAPICEITIPS5VSSLT_INACT := 1 (*Inactive - Inactive*)
		);
	McAPICEITIPS5VSymSymType : STRUCT (*Type mcAPICEITIPS5VS_SYM settings*)
		LineTermination : McAPICEITIPS5VSymSymLinTermEnum; (*Line termination*)
	END_STRUCT;
	McAPICEITIPS5VSymAsymOutDrvEnum :
		( (*Output driver of the encoder*)
		mcAPICEITIPS5VSAOD_PUSH_PULL := 0, (*Push pull - Push pull*)
		mcAPICEITIPS5VSAOD_PULL := 1, (*Pull - Pull*)
		mcAPICEITIPS5VSAOD_PUSH := 2 (*Push - Push*)
		);
	McAPICEITIPS5VSymAsymType : STRUCT (*Type mcAPICEITIPS5VS_ASYM settings*)
		OutputDriver : McAPICEITIPS5VSymAsymOutDrvEnum; (*Output driver of the encoder*)
	END_STRUCT;
	McAPICEITIPS5VSymType : STRUCT (*Symmetry of the encoder signals*)
		Type : McAPICEITIPS5VSymEnum; (*Symmetry selector setting*)
		Symmetrical : McAPICEITIPS5VSymSymType; (*Type mcAPICEITIPS5VS_SYM settings*)
		Asymmetrical : McAPICEITIPS5VSymAsymType; (*Type mcAPICEITIPS5VS_ASYM settings*)
	END_STRUCT;
	McAPICEITIPS5VRefPDetectEnum :
		( (*Reference pulse detection*)
		mcAPICEITIPS5VRPD_NORM_MOD := 0, (*Normal mode - Normal mode*)
		mcAPICEITIPS5VRPD_EDG_TRG_MOD := 1 (*Edge triggered mode - Edge triggered mode*)
		);
	McAPICEITIPS5VType : STRUCT (*Type mcAPICEITIPS_PWR_SUP_5V settings*)
		LineResistance : McAPICEITIPS5VLinResType; (*Resistance of the encoder supply line (1line)(Calculate value is only executed correct for copper lines)*)
		Symmetry : McAPICEITIPS5VSymType; (*Symmetry of the encoder signals*)
		ReferencePulseDetection : McAPICEITIPS5VRefPDetectEnum; (*Reference pulse detection*)
		LineCount : UDINT; (*Number of pulses per encoder revolution [lines per revolution]*)
		MaxExpectedOutputFrequency : UDINT; (*RS422 mode 50kHz to 6250kHz, other modes 50kHz to 200kHz [kHz]*)
	END_STRUCT;
	McAPICEITIPS12VLogLvlEnum :
		( (*Logic (Level) selector setting*)
		mcAPICEITIPS12VLL_HTL := 0, (*HTL -*)
		mcAPICEITIPS12VLL_TTL := 1 (*TTL -*)
		);
	McAPICEITIPS12VLogLvlHTLSymEnum :
		( (*Symmetry selector setting*)
		mcAPICEITIPS12VLLHS_SYM := 0, (*Symmetrical -*)
		mcAPICEITIPS12VLLHS_ASYM := 1 (*Asymmetrical -*)
		);
	McAPICEITIHTLSAsymOutDrvEnum :
		( (*Output driver of the encoder*)
		mcAPICEITIHTLSAOD_PUSH_PULL := 0, (*Push pull - Push pull*)
		mcAPICEITIHTLSAOD_PULL := 1, (*Pull - Pull*)
		mcAPICEITIHTLSAOD_PUSH := 2 (*Push - Push*)
		);
	McAPICEITIHTLSAsymType : STRUCT (*Type mcAPICEITIPS12VLLHS_ASYM settings*)
		OutputDriver : McAPICEITIHTLSAsymOutDrvEnum; (*Output driver of the encoder*)
	END_STRUCT;
	McAPICEITIPS12VLogLvlHTLSymType : STRUCT (*Symmetry of the encoder signals*)
		Type : McAPICEITIPS12VLogLvlHTLSymEnum; (*Symmetry selector setting*)
		Asymmetrical : McAPICEITIHTLSAsymType; (*Type mcAPICEITIPS12VLLHS_ASYM settings*)
	END_STRUCT;
	McAPICEITIPS12VLogLvlHTLType : STRUCT (*Type mcAPICEITIPS12VLL_HTL settings*)
		Symmetry : McAPICEITIPS12VLogLvlHTLSymType; (*Symmetry of the encoder signals*)
	END_STRUCT;
	McAPICEITIPS12VLogLvlTTLSymEnum :
		( (*Symmetry selector setting*)
		mcAPICEITIPS12VLLTS_SYM := 0 (*Symmetrical -*)
		);
	McAPICEITITTLSSymLinTermEnum :
		( (*Line termination*)
		mcAPICEITITTLSSLT_ACT_RS422 := 0, (*Active (RS422) - Active (RS422)*)
		mcAPICEITITTLSSLT_INACT := 1 (*Inactive - Inactive*)
		);
	McAPICEITITTLSSymType : STRUCT (*Type mcAPICEITIPS12VLLTS_SYM settings*)
		LineTermination : McAPICEITITTLSSymLinTermEnum; (*Line termination*)
	END_STRUCT;
	McAPICEITIPS12VLogLvlTTLSymType : STRUCT (*Symmetry of the encoder signals*)
		Type : McAPICEITIPS12VLogLvlTTLSymEnum; (*Symmetry selector setting*)
		Symmetrical : McAPICEITITTLSSymType; (*Type mcAPICEITIPS12VLLTS_SYM settings*)
	END_STRUCT;
	McAPICEITIPS12VLogLvlTTLType : STRUCT (*Type mcAPICEITIPS12VLL_TTL settings*)
		Symmetry : McAPICEITIPS12VLogLvlTTLSymType; (*Symmetry of the encoder signals*)
	END_STRUCT;
	McAPICEITIPS12VLogLvlType : STRUCT (*Logic (level) of the encoder output signals*)
		Type : McAPICEITIPS12VLogLvlEnum; (*Logic (Level) selector setting*)
		HTL : McAPICEITIPS12VLogLvlHTLType; (*Type mcAPICEITIPS12VLL_HTL settings*)
		TTL : McAPICEITIPS12VLogLvlTTLType; (*Type mcAPICEITIPS12VLL_TTL settings*)
	END_STRUCT;
	McAPICEITIPS12VRefPDetectEnum :
		( (*Reference pulse detection*)
		mcAPICEITIPS12VRPD_NORM_MOD := 0, (*Normal mode - Normal mode*)
		mcAPICEITIPS12VRPD_EDG_TRG_MOD := 1 (*Edge triggered mode - Edge triggered mode*)
		);
	McAPICEITIPS12VType : STRUCT (*Type mcAPICEITIPS_PWR_SUP_12V settings*)
		LogicLevel : McAPICEITIPS12VLogLvlType; (*Logic (level) of the encoder output signals*)
		ReferencePulseDetection : McAPICEITIPS12VRefPDetectEnum; (*Reference pulse detection*)
		LineCount : UDINT; (*Number of pulses per encoder revolution [lines per revolution]*)
		MaxExpectedOutputFrequency : UDINT; (*RS422 mode 50kHz to 6250kHz, other modes 50kHz to 200kHz [kHz]*)
	END_STRUCT;
	McAPICEITIncrPwrSupType : STRUCT (*Power supply of the encoder*)
		Type : McAPICEITIncrPwrSupEnum; (*Power supply selector setting*)
		PowerSupply5V : McAPICEITIPS5VType; (*Type mcAPICEITIPS_PWR_SUP_5V settings*)
		PowerSupply12V : McAPICEITIPS12VType; (*Type mcAPICEITIPS_PWR_SUP_12V settings*)
	END_STRUCT;
	McAP3PICEITIncrType : STRUCT (*Type mcAPICEXIT_INCR settings*)
		PowerSupply : McAPICEITIncrPwrSupType; (*Power supply of the encoder*)
	END_STRUCT;
	McAP3PICEITBiSSPwrSupEnum :
		( (*Power supply of the encoder*)
		mcAP3PICEITBPS_EXT := 0, (*External - External (0V)*)
		mcAP3PICEITBPS_PS_5_V := 1, (*PS 5 V - 5 Volt*)
		mcAP3PICEITBPS_PS_12_V := 2 (*PS 12 V - 12 Volt*)
		);
	McAP3PICEITBiSSType : STRUCT (*Type mcAPICEXIT_BISS settings*)
		PowerSupply : McAP3PICEITBiSSPwrSupEnum; (*Power supply of the encoder*)
		BiSSFrameConfiguration : McAPICEITBiSSFCType; (*Frame structure in the order of transfer*)
		CRCPolynomial : UDINT; (*Data verification via CRC (decimal value, 0 deactivates verification)*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAP3PICEITSSIPwrSupEnum :
		( (*Power supply of the encoder*)
		mcAP3PICEITSSIPS_EXT := 0, (*External - External (0V)*)
		mcAP3PICEITSSIPS_PS_5_V := 1, (*PS 5 V - 5 Volt*)
		mcAP3PICEITSSIPS_PS_12_V := 2 (*PS 12 V - 12 Volt*)
		);
	McAP3PICEITSSIType : STRUCT (*Type mcAPICEXIT_SSI settings*)
		PowerSupply : McAP3PICEITSSIPwrSupEnum; (*Power supply of the encoder*)
		SSIFrameConfiguration : McAPICEITSSIFCType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAPICEncXIfTypType : STRUCT (*Interface type of the encoder*)
		Type : McAPICEncXIfTypEnum; (*Interface type selector setting*)
		EnDat : McAP3PICEITEnDatType; (*Type mcAPICEXIT_ENDAT settings*)
		Sine : McAP3PICEITSinType; (*Type mcAPICEXIT_SIN settings*)
		SineWithDCM : McAP3PICEITSinWDCMType; (*Type mcAPICEXIT_SIN_W_DCM settings*)
		SSISine : McAP3PICEITSSISinType; (*Type mcAPICEXIT_SSI_SIN settings*)
		Incremental : McAP3PICEITIncrType; (*Type mcAPICEXIT_INCR settings*)
		BiSS : McAP3PICEITBiSSType; (*Type mcAPICEXIT_BISS settings*)
		SSI : McAP3PICEITSSIType; (*Type mcAPICEXIT_SSI settings*)
		Resolver : McAPICEITResType; (*Type mcAPICEXIT_RES settings*)
	END_STRUCT;
	McAP3PICEEncX41xType : STRUCT
		InterfaceType : McAPICEncXIfTypType; (*Interface type of the encoder*)
	END_STRUCT;
	McAP3PICEEncX42xType : STRUCT
		InterfaceType : McAPICEncXIfTypType; (*Interface type of the encoder*)
	END_STRUCT;
	McAP3PICEEncX43xType : STRUCT
		InterfaceType : McAPICEncXIfTypType; (*Interface type of the encoder*)
	END_STRUCT;
	McCfgAcpP3PlInCrdEncType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_P3_PL_IN_CARD_ENC*)
		EncoderX41x : McAP3PICEEncX41xType;
		EncoderX42x : McAP3PICEEncX42xType;
		EncoderX43x : McAP3PICEEncX43xType;
	END_STRUCT;
	McAP3SPICEIfTypEnum :
		( (*Interface type selector setting*)
		mcAP3SPICEIT_NOT_USE := 0, (*Not used -*)
		mcAP3SPICEIT_ENDAT := 1, (*EnDat -*)
		mcAP3SPICEIT_SIN := 2, (*Sine -*)
		mcAP3SPICEIT_SIN_W_DCM := 3, (*Sine with DCM -*)
		mcAP3SPICEIT_SSI_SIN := 4, (*SSI sine -*)
		mcAP3SPICEIT_HIPERFACE := 5, (*HIPERFACE -*)
		mcAP3SPICEIT_INCR := 6, (*Incremental -*)
		mcAP3SPICEIT_BISS := 7, (*BiSS -*)
		mcAP3SPICEIT_SSI := 8, (*SSI -*)
		mcAP3SPICEIT_HIPERFACE_DSL := 9, (*HIPERFACE DSL -*)
		mcAP3SPICEIT_TFMT := 10, (*T-Format - Tamagawa digital encoder*)
		mcAP3SPICEIT_RES := 11, (*Resolver -*)
		mcAP3SPICEIT_ENDAT_3 := 12 (*EnDat 3 -*)
		);
	McAP3SPICEITSinType : STRUCT (*Type mcAP3SPICEIT_SIN settings*)
		LineResistance : REAL; (*Line resistance encoder supply [Ohm]*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAP3SPICEITSinWDCMType : STRUCT (*Type mcAP3SPICEIT_SIN_W_DCM settings*)
		LineResistance : REAL; (*Line resistance encoder supply [Ohm]*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		DCMBasicDistance : UDINT; (*DCM basic distance*)
		DCMBasicDifference : DINT; (*DCM basic difference*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAP3SPICEIfTypType : STRUCT (*Interface type of the encoder*)
		Type : McAP3SPICEIfTypEnum; (*Interface type selector setting*)
		EnDat : McAP3PICEITEnDatType; (*Type mcAP3SPICEIT_ENDAT settings*)
		Sine : McAP3SPICEITSinType; (*Type mcAP3SPICEIT_SIN settings*)
		SineWithDCM : McAP3SPICEITSinWDCMType; (*Type mcAP3SPICEIT_SIN_W_DCM settings*)
		SSISine : McAP3PICEITSSISinType; (*Type mcAP3SPICEIT_SSI_SIN settings*)
		Incremental : McAP3PICEITIncrType; (*Type mcAP3SPICEIT_INCR settings*)
		BiSS : McAP3PICEITBiSSType; (*Type mcAP3SPICEIT_BISS settings*)
		SSI : McAP3PICEITSSIType; (*Type mcAP3SPICEIT_SSI settings*)
		Resolver : McAPICEITResType; (*Type mcAP3SPICEIT_RES settings*)
	END_STRUCT;
	McCfgAcpP3SngPlInCrdEncType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_P3_SNG_PL_IN_CARD_ENC*)
		InterfaceType : McAP3SPICEIfTypType; (*Interface type of the encoder*)
	END_STRUCT;
	McAPICIODigIO1To3Enum :
		( (*Digital I/O 1 to 3 selector setting*)
		mcAPICIODIO1T3_SNG_IO_CFG := 0, (*Single I/O configuration - I/O Pins X41E/19,20,21 are used as I/Os*)
		mcAPICIODIO1T3_INCR_ENC_ABR_IN := 1, (*Incremental encoder (ABR) input - I/O Pins X41E/19,20,21 are used as ABR single ended encoder input*)
		mcAPICIODIO1T3_INCR_ENC_ABR_EMU := 2, (*Incremental encoder (ABR) emulation - I/O Pins X41E/19,20,21 are used as ABR single ended encoder emulation*)
		mcAPICIODIO1T3_INCR_ENC_AB_EMU := 3 (*Incremental encoder (AB) emulation - I/O Pins X41E/19,20 are used as AB single ended encoder emulation, X41E/21 is used as I/O*)
		);
	McAPICIOSngIOCfgDIOEnum :
		( (*Digital I/O 1 selector setting*)
		mcAPICIOSIOCDIO_NOT_USE := 0, (*Not used -*)
		mcAPICIOSIOCDIO_IN := 1, (*Input -*)
		mcAPICIOSIOCDIO_OUT := 2, (*Output -*)
		mcAPICIOSIOCDIO_EVNT_CNT := 3 (*Event counter - I/O Pin X41E/19 is used as event counter*)
		);
	McAPICIOSngIOCfgDIOOutAsgEnum :
		( (*Assignment of which channel the IO can be used*)
		mcAPICIOSIOCDIOOA_CH_1 := 0, (*Channel 1*)
		mcAPICIOSIOCDIOOA_CH_2 := 1, (*Channel 2*)
		mcAPICIOSIOCDIOOA_CH_3 := 2 (*Channel 3*)
		);
	McAPICIOSngIOCfgDIOOutType : STRUCT (*Type mcAPICIOSIOCDIO_OUT settings*)
		Assignment : McAPICIOSngIOCfgDIOOutAsgEnum; (*Assignment of which channel the IO can be used*)
	END_STRUCT;
	McAPICIOSIOCDIOEvntCntEvalEnum :
		(
		mcAPICIOSIOCDIOECE_POS_EDG := 0, (*Positive edge - 1-fold evaluation. Positive edges are counted*)
		mcAPICIOSIOCDIOECE_BOTH_EDG := 1 (*Both edges - 2-fold evaluation. Positive and negative edges are counted*)
		);
	McAPICIOSIOCDIOEvntCntType : STRUCT (*Type mcAPICIOSIOCDIO_EVNT_CNT settings*)
		Evaluation : McAPICIOSIOCDIOEvntCntEvalEnum;
	END_STRUCT;
	McAPICIOSngIOCfgDIOType : STRUCT (*I/O Pin X41E/19*)
		Type : McAPICIOSngIOCfgDIOEnum; (*Digital I/O 1 selector setting*)
		Output : McAPICIOSngIOCfgDIOOutType; (*Type mcAPICIOSIOCDIO_OUT settings*)
		EventCounter : McAPICIOSIOCDIOEvntCntType; (*Type mcAPICIOSIOCDIO_EVNT_CNT settings*)
	END_STRUCT;
	McAPICIODigIOXEnum :
		( (*Digital I/O 3 selector setting*)
		mcAPICIODIOX_NOT_USE := 0, (*Not used -*)
		mcAPICIODIOX_IN := 1, (*Input -*)
		mcAPICIODIOX_OUT := 2 (*Output -*)
		);
	McAPICIODigIOXOutAsgEnum :
		( (*Assignment of which channel the IO can be used*)
		mcAPICIODIOXOA_CH_1 := 0, (*Channel 1*)
		mcAPICIODIOXOA_CH_2 := 1, (*Channel 2*)
		mcAPICIODIOXOA_CH_3 := 2 (*Channel 3*)
		);
	McAPICIODigIOXOutType : STRUCT (*Type mcAPICIODIOX_OUT settings*)
		Assignment : McAPICIODigIOXOutAsgEnum; (*Assignment of which channel the IO can be used*)
	END_STRUCT;
	McAPICIODigIOXType : STRUCT (*I/O Pin X41E/21*)
		Type : McAPICIODigIOXEnum; (*Digital I/O 3 selector setting*)
		Output : McAPICIODigIOXOutType; (*Type mcAPICIODIOX_OUT settings*)
	END_STRUCT;
	McAPICIOSngIOCfgType : STRUCT (*Type mcAPICIODIO1T3_SNG_IO_CFG settings*)
		DigitalIO1 : McAPICIOSngIOCfgDIOType; (*I/O Pin X41E/19*)
		DigitalIO2 : McAPICIOSngIOCfgDIOType; (*I/O Pin X41E/20*)
		DigitalIO3 : McAPICIODigIOXType; (*I/O Pin X41E/21*)
	END_STRUCT;
	McAPICIOIncrEncABRInType : STRUCT (*Type mcAPICIODIO1T3_INCR_ENC_ABR_IN settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
	END_STRUCT;
	McAPICIOIncrEmuValSrcEnum :
		( (*Value source selector setting*)
		mcAPICIOIEVS_REAL_AX_SET_POS := 0, (*Real axis set position - Real axis set position*)
		mcAPICIOIEVS_REAL_AX_ACT_POS := 1, (*Real axis actual position - Real axis actual position*)
		mcAPICIOIEVS_VIRT_AX_POS := 2, (*Virtual axis position - Virtual axis position*)
		mcAPICIOIEVS_PARID := 3 (*ParID - ParID of the channel*)
		);
	McAPICIOIncrEmuValSrcParIDType : STRUCT (*Type mcAPICIOIEVS_PARID settings*)
		ParID : UINT; (*This ParID is used*)
	END_STRUCT;
	McAPICIOIncrEmuValSrcType : STRUCT (*Value which should be output by the emulation*)
		Type : McAPICIOIncrEmuValSrcEnum; (*Value source selector setting*)
		ParID : McAPICIOIncrEmuValSrcParIDType; (*Type mcAPICIOIEVS_PARID settings*)
	END_STRUCT;
	McAPICIOIncrEncABREmuCntDirEnum :
		( (*Direction of the encoder in which the position value is increasing*)
		mcAPICIOIEABRECD_STD := 0, (*Standard - Clockwise*)
		mcAPICIOIEABRECD_INV := 1 (*Inverse - Counter-clockwise*)
		);
	McAPICIOIncrEncABREmuType : STRUCT (*Type mcAPICIODIO1T3_INCR_ENC_ABR_EMU settings*)
		ValueSource : McAPICIOIncrEmuValSrcType; (*Value which should be output by the emulation*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		UnitsPerEncoderRevolutions : LREAL; (*Absolute number of units per encoder revolutions [measurement units]*)
		UnitsPerEncoderRevolutionsParID : UDINT; (*Absolute number of units per encoder revolutions*)
		NumberOfEncoderRevolutions : UDINT; (*Number of encoder revolutions relating to units*)
		CountDirection : McAPICIOIncrEncABREmuCntDirEnum; (*Direction of the encoder in which the position value is increasing*)
	END_STRUCT;
	McAPICIOIncrEncABEmuCntDirEnum :
		( (*Direction of the encoder in which the position value is increasing*)
		mcAPICIOIEABECD_STD := 0, (*Standard - Clockwise*)
		mcAPICIOIEABECD_INV := 1 (*Inverse - Counter-clockwise*)
		);
	McAPICIOIncrEncABEmuType : STRUCT (*Type mcAPICIODIO1T3_INCR_ENC_AB_EMU settings*)
		ValueSource : McAPICIOIncrEmuValSrcType; (*Value which should be output by the emulation*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		UnitsPerEncoderRevolutions : LREAL; (*Absolute number of units per encoder revolutions [measurement units]*)
		UnitsPerEncoderRevolutionsParID : UDINT; (*Absolute number of units per encoder revolutions*)
		NumberOfEncoderRevolutions : UDINT; (*Number of encoder revolutions relating to units*)
		CountDirection : McAPICIOIncrEncABEmuCntDirEnum; (*Direction of the encoder in which the position value is increasing*)
		DigitalIO3 : McAPICIODigIOXType; (*I/O Pin X41E/21*)
	END_STRUCT;
	McAPICIODigIO1To3Type : STRUCT (*I/O Pins X41E/19,20,21*)
		Type : McAPICIODigIO1To3Enum; (*Digital I/O 1 to 3 selector setting*)
		SingleIOConfiguration : McAPICIOSngIOCfgType; (*Type mcAPICIODIO1T3_SNG_IO_CFG settings*)
		IncrementalEncoderABRInput : McAPICIOIncrEncABRInType; (*Type mcAPICIODIO1T3_INCR_ENC_ABR_IN settings*)
		IncrementalEncoderABREmulation : McAPICIOIncrEncABREmuType; (*Type mcAPICIODIO1T3_INCR_ENC_ABR_EMU settings*)
		IncrementalEncoderABEmulation : McAPICIOIncrEncABEmuType; (*Type mcAPICIODIO1T3_INCR_ENC_AB_EMU settings*)
	END_STRUCT;
	McAPICIODigIO9To10Enum :
		( (*Digital I/O 9 to 10 selector setting*)
		mcAPICIODIO9T10_NOT_USE := 0, (*Not used -*)
		mcAPICIODIO9T10_IN := 1, (*Input -*)
		mcAPICIODIO9T10_OUT := 2 (*Output -*)
		);
	McAPICIODigIO9To10InDigIO9Enum :
		( (*Digital I/O 9 selector setting*)
		mcAPICIODIO9T10IDIO9_NOT_USE := 0, (*Not used -*)
		mcAPICIODIO9T10IDIO9_USE := 1 (*Used -*)
		);
	McAPICIODigIO9To10InDigIO9Type : STRUCT (*I/O Pin X41E/27*)
		Type : McAPICIODigIO9To10InDigIO9Enum; (*Digital I/O 9 selector setting*)
	END_STRUCT;
	McAPICIODigIO9To10InDigIO10Enum :
		( (*Digital I/O 10 selector setting*)
		mcAPICIODIO9T10IDIO1_NOT_USE := 0, (*Not used -*)
		mcAPICIODIO9T10IDIO1_USE := 1 (*Used -*)
		);
	McAPICIODigIO9To10InDigIO10Type : STRUCT (*I/O Pin X41E/28*)
		Type : McAPICIODigIO9To10InDigIO10Enum; (*Digital I/O 10 selector setting*)
	END_STRUCT;
	McAPICIODigIO9To10InType : STRUCT (*Type mcAPICIODIO9T10_IN settings*)
		DigitalIO9 : McAPICIODigIO9To10InDigIO9Type; (*I/O Pin X41E/27*)
		DigitalIO10 : McAPICIODigIO9To10InDigIO10Type; (*I/O Pin X41E/28*)
	END_STRUCT;
	McAPICIODigIOOutDigIOEnum :
		( (*Digital I/O 9 selector setting*)
		mcAPICIODIOODIO_NOT_USE := 0, (*Not used -*)
		mcAPICIODIOODIO_USE := 1 (*Used -*)
		);
	McAPICIODigIOOutDigIOUseAsgEnum :
		( (*Assignment of which channel the IO can be used*)
		mcAPICIODIOODIOUA_CH_1 := 0, (*Channel 1*)
		mcAPICIODIOODIOUA_CH_2 := 1, (*Channel 2*)
		mcAPICIODIOODIOUA_CH_3 := 2 (*Channel 3*)
		);
	McAPICIODigIOOutDigIOUseType : STRUCT (*Type mcAPICIODIOODIO_USE settings*)
		Assignment : McAPICIODigIOOutDigIOUseAsgEnum; (*Assignment of which channel the IO can be used*)
	END_STRUCT;
	McAPICIODigIOOutDigIOType : STRUCT (*I/O Pin X41E/27*)
		Type : McAPICIODigIOOutDigIOEnum; (*Digital I/O 9 selector setting*)
		Used : McAPICIODigIOOutDigIOUseType; (*Type mcAPICIODIOODIO_USE settings*)
	END_STRUCT;
	McAPICIODigIO9To10OutType : STRUCT (*Type mcAPICIODIO9T10_OUT settings*)
		DigitalIO9 : McAPICIODigIOOutDigIOType; (*I/O Pin X41E/27*)
		DigitalIO10 : McAPICIODigIOOutDigIOType; (*I/O Pin X41E/28*)
	END_STRUCT;
	McAPICIODigIO9To10Type : STRUCT (*I/O Pins X41E/27, 28*)
		Type : McAPICIODigIO9To10Enum; (*Digital I/O 9 to 10 selector setting*)
		Input : McAPICIODigIO9To10InType; (*Type mcAPICIODIO9T10_IN settings*)
		Output : McAPICIODigIO9To10OutType; (*Type mcAPICIODIO9T10_OUT settings*)
	END_STRUCT;
	McAPICIODigInOutType : STRUCT
		DigitalInputFilterTime : REAL; (*[µs]*)
		DigitalIO1To3 : McAPICIODigIO1To3Type; (*I/O Pins X41E/19,20,21*)
		DigitalIO4 : McAPICIODigIOXType; (*I/O Pin X41E/22*)
		DigitalIO5 : McAPICIODigIOXType; (*I/O Pin X41E/23*)
		DigitalIO6 : McAPICIODigIOXType; (*I/O Pin X41E/24*)
		DigitalIO7 : McAPICIODigIOXType; (*I/O Pin X41E/25*)
		DigitalIO8 : McAPICIODigIOXType; (*I/O Pin X41E/26*)
		DigitalIO9To10 : McAPICIODigIO9To10Type; (*I/O Pins X41E/27, 28*)
	END_STRUCT;
	McAPICIOAnInAnInEnum :
		( (*Analog input 1 selector setting*)
		mcAPICIOAIAI_NOT_USE := 0, (*Not used -*)
		mcAPICIOAIAI_USE := 1 (*Used -*)
		);
	McAPICIOAnInAnInUseFltrStgEnum :
		( (*Stage of low pass filter*)
		mcAPICIOAIAIUFS_SET_TIME_009MS := 0, (*Settling time 0.09ms - Settling time 0->100% = 0.09ms; Attenuation 3dB at 10.2kHz, 60dB at 240kHz*)
		mcAPICIOAIAIUFS_SET_TIME_012MS := 1, (*Settling time 0.12ms - Settling time 0->100% = 0.12ms; Attenuation 3dB at 8.5kHz, 60dB at 52kHz*)
		mcAPICIOAIAIUFS_SET_TIME_015MS := 2, (*Settling time 0.15ms - Settling time 0->100% = 0.15ms; Attenuation 3dB at 6.1kHz, 60dB at 30kHz*)
		mcAPICIOAIAIUFS_SET_TIME_025MS := 3, (*Settling time 0.25ms - Settling time 0->100% = 0.25ms; Attenuation 3dB at 3.5kHz, 60dB at 16kHz*)
		mcAPICIOAIAIUFS_SET_TIME_05MS := 4, (*Settling time 0.5ms - Settling time 0->100% = 0.5ms; Attenuation 3dB at 1.75kHz, 60dB at 7.6kHz*)
		mcAPICIOAIAIUFS_SET_TIME_1MS := 5, (*Settling time 1ms - Settling time 0->100% = 1ms; Attenuation 3dB at 800Hz, 60dB at 3.4kHz*)
		mcAPICIOAIAIUFS_SET_TIME_2MS := 6, (*Settling time 2ms - Settling time 0->100% = 2ms; Attenuation 3dB at 450Hz, 60dB at 2.1kHz*)
		mcAPICIOAIAIUFS_SET_TIME_39MS := 7 (*Settling time 3.9ms - Settling time 0->100% = 3.9ms; Attenuation 3dB at 230Hz, 60dB at 1kHz*)
		);
	McAPICIOAnInAnInUseType : STRUCT (*Type mcAPICIOAIAI_USE settings*)
		FilterStage : McAPICIOAnInAnInUseFltrStgEnum; (*Stage of low pass filter*)
		MaximumSlewRate : REAL; (*Limitation of input voltage change [V/ms]*)
	END_STRUCT;
	McAPICIOAnInAnInType : STRUCT (*I/O Pins X41E/1, 3, 5*)
		Type : McAPICIOAnInAnInEnum; (*Analog input 1 selector setting*)
		Used : McAPICIOAnInAnInUseType; (*Type mcAPICIOAIAI_USE settings*)
	END_STRUCT;
	McAPICIOAnInType : STRUCT
		AnalogInput1 : McAPICIOAnInAnInType; (*I/O Pins X41E/1, 3, 5*)
		AnalogInput2 : McAPICIOAnInAnInType; (*I/O Pins X41E/7, 9, 11*)
		AnalogInput3 : McAPICIOAnInAnInType; (*I/O Pins X41E/13, 15, 17*)
	END_STRUCT;
	McAPICIOAnOutAnOutEnum :
		( (*Analog output 1 selector setting*)
		mcAPICIOAOAO_NOT_USE := 0, (*Not used -*)
		mcAPICIOAOAO_USE := 1 (*Used -*)
		);
	McAPICIOAnOutAnOutUseAsgEnum :
		( (*Assignment of which channel the IO can be used*)
		mcAPICIOAOAOUA_CH_1 := 0, (*Channel 1*)
		mcAPICIOAOAOUA_CH_2 := 1, (*Channel 2*)
		mcAPICIOAOAOUA_CH_3 := 2 (*Channel 3*)
		);
	McAPICIOAnOutAnOutUseTypEnum :
		(
		mcAPICIOAOAOUT_V_10V := 0, (*Voltage 10V*)
		mcAPICIOAOAOUT_CUR_020MA := 1 (*Current 0-20mA*)
		);
	McAPICIOAnOutAnOutUseType : STRUCT (*Type mcAPICIOAOAO_USE settings*)
		Assignment : McAPICIOAnOutAnOutUseAsgEnum; (*Assignment of which channel the IO can be used*)
		Type : McAPICIOAnOutAnOutUseTypEnum;
	END_STRUCT;
	McAPICIOAnOutAnOutType : STRUCT (*I/O Pins X41E/2, 4, 6*)
		Type : McAPICIOAnOutAnOutEnum; (*Analog output 1 selector setting*)
		Used : McAPICIOAnOutAnOutUseType; (*Type mcAPICIOAOAO_USE settings*)
	END_STRUCT;
	McAPICIOAnOutType : STRUCT
		AnalogOutput1 : McAPICIOAnOutAnOutType; (*I/O Pins X41E/2, 4, 6*)
		AnalogOutput2 : McAPICIOAnOutAnOutType; (*I/O Pins X41E/8, 10, 12*)
		AnalogOutput3 : McAPICIOAnOutAnOutType; (*I/O Pins X41E/14, 16, 18*)
	END_STRUCT;
	McCfgAcpPlInCrdIOType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_PL_IN_CARD_IO*)
		DigitalInputsOutputs : McAPICIODigInOutType;
		AnalogInputs : McAPICIOAnInType;
		AnalogOutputs : McAPICIOAnOutType;
	END_STRUCT;
	McBRMntEnum :
		( (*Mounting selector setting*)
		mcBRM_VERTICAL := 0, (*Vertical - Braking resistor is mounted vertical*)
		mcBRM_HORIZONTAL := 1 (*Horizontal - Braking resistor is mounted horizontal*)
		);
	McBRMntVerticalType : STRUCT (*Type mcBRM_VERTICAL settings*)
		ThermalResistance : REAL; (*Thermal resistance [K/W]*)
	END_STRUCT;
	McBRMntHorizontalType : STRUCT (*Type mcBRM_HORIZONTAL settings*)
		ThermalResistance : REAL; (*Thermal resistance [K/W]*)
	END_STRUCT;
	McBRMntType : STRUCT (*Mounting variant*)
		Type : McBRMntEnum; (*Mounting selector setting*)
		Vertical : McBRMntVerticalType; (*Type mcBRM_VERTICAL settings*)
		Horizontal : McBRMntHorizontalType; (*Type mcBRM_HORIZONTAL settings*)
	END_STRUCT;
	McCfgBrkResType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_BRK_RES*)
		Resistance : REAL; (*Electrical resistance [Ohm]*)
		LimitTemperature : REAL; (*Maximum temperature [°C]*)
		ThermalCapacity : REAL; (*Thermal capacity [Ws/K]*)
		Mounting : McBRMntType; (*Mounting variant*)
	END_STRUCT;
	McVUTmpMdlEnum :
		( (*Temperature model selector setting*)
		mcVUTM_CURBASED := 1, (*Current-based -*)
		mcVUTM_NOT_USE := 2 (*Not used -*)
		);
	McVUTMCurBsdType : STRUCT (*Type mcVUTM_CURBASED settings*)
		LimitTemperature : REAL; (*Maximum permissible winding temperature [°C]*)
		WindingCrossSection : REAL; (*Phase conductor cross section [mm²]*)
		ThermalTimeConstant : REAL; (*Thermal time constant [s]*)
	END_STRUCT;
	McVUTmpMdlType : STRUCT (*Model for winding temperature monitoring*)
		Type : McVUTmpMdlEnum; (*Temperature model selector setting*)
		CurrentBased : McVUTMCurBsdType; (*Type mcVUTM_CURBASED settings*)
	END_STRUCT;
	McCfgVibrUnitType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_VIBR_UNIT*)
		MaximumMechanicalFrequency : REAL; (*Maximum mechanical frequency of the unit [Hz]*)
		NominalVoltage : REAL; (*Nominal voltage (RMS value, phase-phase) [V]*)
		NominalCurrent : REAL; (*Phase current rated (RMS value) [A]*)
		PeakCurrent : REAL; (*Peak phase current (RMS value) [A]*)
		StatorResistance : REAL; (*Stator resistance (phase-phase) [Ω]*)
		StatorInductance : REAL; (*Stator inductance (phase-phase) [mH]*)
		TemperatureModel : McVUTmpMdlType; (*Model for winding temperature monitoring*)
	END_STRUCT;
	McAMEType : STRUCT (*Parameter of hardware elements situated between motor encoder and load which influence the scaling*)
		Gearbox : McCfgGearBoxType; (*Specifies a gearbox by defining the ratio between a gearbox input and output*)
		RotaryToLinearTransformation : McCfgRotToLinTrfType; (*Specifies a transformation factor between the output of the gear and the actual load movement*)
	END_STRUCT;
	McAELEnum :
		( (*Encoder link selector setting*)
		mcAEL_ONE_ENC := 0, (*One encoder -*)
		mcAEL_TWO_ENC := 1, (*Two encoders -*)
		mcAEL_NO_ENC := 10 (*No encoder -*)
		);
	McAELAllEncEnum :
		( (*Motor and position encoder selector setting*)
		mcAELAE_ENC_X6A := 0, (*Encoder X6A - OnBoard encoder 1*)
		mcAELAE_ENC_X6B := 1, (*Encoder X6B - OnBoard encoder 2*)
		mcAELAE_ENC := 2, (*Encoder -*)
		mcAELAE_ENC_SS1X11 := 3, (*Encoder SS1.X11 - Plug-in module in SS1*)
		mcAELAE_ENC_SS2X11 := 4, (*Encoder SS2.X11 - Plug-in module in SS2*)
		mcAELAE_ENC_X11A := 5, (*Encoder X11A -*)
		mcAELAE_ENC_SS3X11 := 6, (*Encoder SS3.X11 - Plug-in module in SS3*)
		mcAELAE_ENC_SS4X11 := 7, (*Encoder SS4.X11 - Plug-in module in SS4*)
		mcAELAE_ENC_X41 := 8, (*Encoder X41 -*)
		mcAELAE_ENC_SS1X41X := 9, (*Encoder SS1.X41x - Plug-in module in SS1*)
		mcAELAE_ENC_X42 := 10, (*Encoder X42 -*)
		mcAELAE_ENC_SS1X42X := 11, (*Encoder SS1.X42x - Plug-in module in SS1*)
		mcAELAE_ENC_X43 := 12, (*Encoder X43 -*)
		mcAELAE_ENC_SS1X43X := 13, (*Encoder SS1.X43x - Plug-in module in SS1*)
		mcAELAE_ENC_1 := 14, (*Encoder 1 - OnBoard encoder 1*)
		mcAELAE_ENC_2 := 15, (*Encoder 2 - OnBoard encoder 2*)
		mcAELAE_EXT := 16 (*External - External encoder is used*)
		);
	McAELOEExtPosTypEnum :
		( (*Position type selector setting*)
		mcAELOEEPT_ABS := 0, (*Absolute - Get position from an absolute encoder*)
		mcAELOEEPT_INCR := 1 (*Incremental - Get position from an incremental encoder*)
		);
	McAELOEExtPosTypAbsPosRngType : STRUCT (*Defines the range of the position value*)
		LowerLimit : DINT; (*Lower limit of encoder range*)
		UpperLimit : UDINT; (*Upper limit of encoder range*)
	END_STRUCT;
	McAELOEExtPosTypAbsType : STRUCT (*Type mcAELOEEPT_ABS settings*)
		PositionRange : McAELOEExtPosTypAbsPosRngType; (*Defines the range of the position value*)
	END_STRUCT;
	McAELOEExtPosTypType : STRUCT (*Type of the encoder*)
		Type : McAELOEExtPosTypEnum; (*Position type selector setting*)
		Absolute : McAELOEExtPosTypAbsType; (*Type mcAELOEEPT_ABS settings*)
	END_STRUCT;
	McAELOEExtPosSrcEnum :
		( (*Position source selector setting*)
		mcAELOEEPS_IO_CH_DINT := 0, (*I/O channel DINT - Get position from a signed 32 bit I/O channel*)
		mcAELOEEPS_IO_CH_UDINT := 1, (*I/O channel UDINT - Get position from an unsigned 32 bit I/O channel*)
		mcAELOEEPS_IO_CH_INT := 2, (*I/O channel INT - Get position from a signed 16 bit I/O channel*)
		mcAELOEEPS_IO_CH_UINT := 3, (*I/O channel UINT - Get position from an unsigned 16 bit I/O channel*)
		mcAELOEEPS_VAR_DINT := 4, (*Variable DINT - Get position from a signed 32 bit variable*)
		mcAELOEEPS_VAR_UDINT := 5, (*Variable UDINT - Get position from an unsigned 32 bit variable*)
		mcAELOEEPS_VAR_INT := 6, (*Variable INT - Get position from a signed 16 bit variable*)
		mcAELOEEPS_VAR_UINT := 7 (*Variable UINT - Get position from an unsigned 16 bit variable*)
		);
	McAELOEExtPosSrcIOChDINTType : STRUCT (*Type mcAELOEEPS_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAELOEExtPosSrcIOChUDINTType : STRUCT (*Type mcAELOEEPS_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAELOEExtPosSrcIOChINTType : STRUCT (*Type mcAELOEEPS_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAELOEExtPosSrcIOChUINTType : STRUCT (*Type mcAELOEEPS_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAELOEExtPosSrcVarDINTType : STRUCT (*Type mcAELOEEPS_VAR_DINT settings*)
		PVMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAELOEExtPosSrcVarUDINTType : STRUCT (*Type mcAELOEEPS_VAR_UDINT settings*)
		PVMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAELOEExtPosSrcVarINTType : STRUCT (*Type mcAELOEEPS_VAR_INT settings*)
		PVMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAELOEExtPosSrcVarUINTType : STRUCT (*Type mcAELOEEPS_VAR_UINT settings*)
		PVMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAELOEExtPosSrcType : STRUCT (*Position source*)
		Type : McAELOEExtPosSrcEnum; (*Position source selector setting*)
		IOChannelDINT : McAELOEExtPosSrcIOChDINTType; (*Type mcAELOEEPS_IO_CH_DINT settings*)
		IOChannelUDINT : McAELOEExtPosSrcIOChUDINTType; (*Type mcAELOEEPS_IO_CH_UDINT settings*)
		IOChannelINT : McAELOEExtPosSrcIOChINTType; (*Type mcAELOEEPS_IO_CH_INT settings*)
		IOChannelUINT : McAELOEExtPosSrcIOChUINTType; (*Type mcAELOEEPS_IO_CH_UINT settings*)
		VariableDINT : McAELOEExtPosSrcVarDINTType; (*Type mcAELOEEPS_VAR_DINT settings*)
		VariableUDINT : McAELOEExtPosSrcVarUDINTType; (*Type mcAELOEEPS_VAR_UDINT settings*)
		VariableINT : McAELOEExtPosSrcVarINTType; (*Type mcAELOEEPS_VAR_INT settings*)
		VariableUINT : McAELOEExtPosSrcVarUINTType; (*Type mcAELOEEPS_VAR_UINT settings*)
	END_STRUCT;
	McAELOEExtValCkModOkEnum :
		( (*Module ok selector setting*)
		mcAELOEEVCMO_POS_SRC_DEV := 0, (*Position source device - Get module ok from the same device as the position source*)
		mcAELOEEVCMO_IO_CH := 1, (*I/O channel - Get module ok from an I/O channel*)
		mcAELOEEVCMO_VAR := 2, (*Variable - Get module ok from a variable*)
		mcAELOEEVCMO_NOT_USE := 3 (*Not used - Module ok is not used*)
		);
	McAELOEExtValCkModOkIOChType : STRUCT (*Type mcAELOEEVCMO_IO_CH settings*)
		ChannelMapping : STRING[250]; (*Input source for module ok*)
	END_STRUCT;
	McAELOEExtValCkModOkVarType : STRUCT (*Type mcAELOEEVCMO_VAR settings*)
		PVMapping : STRING[250]; (*Input source for module ok*)
	END_STRUCT;
	McAELOEExtValCkModOkType : STRUCT (*Use module ok for validity check*)
		Type : McAELOEExtValCkModOkEnum; (*Module ok selector setting*)
		IOChannel : McAELOEExtValCkModOkIOChType; (*Type mcAELOEEVCMO_IO_CH settings*)
		Variable : McAELOEExtValCkModOkVarType; (*Type mcAELOEEVCMO_VAR settings*)
	END_STRUCT;
	McAELOEExtValCkStDatEnum :
		( (*Stale data selector setting*)
		mcAELOEEVCSD_POS_SRC_DEV := 0, (*Position source device - Get stale data from the same device as the position source*)
		mcAELOEEVCSD_IO_CH := 1, (*I/O channel - Get stale data from an I/O channel*)
		mcAELOEEVCSD_VAR := 2, (*Variable - Get stale data from a variable*)
		mcAELOEEVCSD_NOT_USE := 3 (*Not used - Stale data is not used*)
		);
	McAELOEExtValCkStDatIOChType : STRUCT (*Type mcAELOEEVCSD_IO_CH settings*)
		ChannelMapping : STRING[250]; (*Input source for stale data*)
	END_STRUCT;
	McAELOEExtValCkStDatVarType : STRUCT (*Type mcAELOEEVCSD_VAR settings*)
		PVMapping : STRING[250]; (*Input source for stale data*)
	END_STRUCT;
	McAELOEExtValCkStDatType : STRUCT (*Use stale data for validity check*)
		Type : McAELOEExtValCkStDatEnum; (*Stale data selector setting*)
		IOChannel : McAELOEExtValCkStDatIOChType; (*Type mcAELOEEVCSD_IO_CH settings*)
		Variable : McAELOEExtValCkStDatVarType; (*Type mcAELOEEVCSD_VAR settings*)
	END_STRUCT;
	McAELOEExtValCkNetTimeEnum :
		( (*Net time selector setting*)
		mcAELOEEVCNT_NOT_USE := 0, (*Not used - Net time is not used*)
		mcAELOEEVCNT_IO_CH := 1, (*I/O channel - Get net time from an I/O channel*)
		mcAELOEEVCNT_VAR := 2 (*Variable - Get net time from a variable*)
		);
	McAELOEExtValCkNetTimeIOChType : STRUCT (*Type mcAELOEEVCNT_IO_CH settings*)
		ChannelMapping : STRING[250]; (*Input source for net time*)
	END_STRUCT;
	McAELOEExtValCkNetTimeVarType : STRUCT (*Type mcAELOEEVCNT_VAR settings*)
		PVMapping : STRING[250]; (*Input source for net time*)
	END_STRUCT;
	McAELOEExtValCkNetTimeType : STRUCT (*Use net time for validity check*)
		Type : McAELOEExtValCkNetTimeEnum; (*Net time selector setting*)
		IOChannel : McAELOEExtValCkNetTimeIOChType; (*Type mcAELOEEVCNT_IO_CH settings*)
		Variable : McAELOEExtValCkNetTimeVarType; (*Type mcAELOEEVCNT_VAR settings*)
	END_STRUCT;
	McAELOEExtValCkEncOkEnum :
		( (*Encoder ok selector setting*)
		mcAELOEEVCEO_NOT_USE := 0, (*Not used - Encoder ok is not used*)
		mcAELOEEVCEO_IO_CH := 1, (*I/O channel - Get encoder ok from an I/O channel*)
		mcAELOEEVCEO_VAR := 2 (*Variable - Get encoder ok from a variable*)
		);
	McAELOEExtValCkEncOkIOChType : STRUCT (*Type mcAELOEEVCEO_IO_CH settings*)
		ChannelMapping : STRING[250]; (*Input source for encoder ok*)
	END_STRUCT;
	McAELOEExtValCkEncOkVarType : STRUCT (*Type mcAELOEEVCEO_VAR settings*)
		PVMapping : STRING[250]; (*Input source for encoder ok*)
	END_STRUCT;
	McAELOEExtValCkEncOkType : STRUCT (*Use encoder ok flag for validity check (True = position valid)*)
		Type : McAELOEExtValCkEncOkEnum; (*Encoder ok selector setting*)
		IOChannel : McAELOEExtValCkEncOkIOChType; (*Type mcAELOEEVCEO_IO_CH settings*)
		Variable : McAELOEExtValCkEncOkVarType; (*Type mcAELOEEVCEO_VAR settings*)
	END_STRUCT;
	McAELOEExtValCkType : STRUCT (*Check if given position is valid*)
		ModuleOk : McAELOEExtValCkModOkType; (*Use module ok for validity check*)
		StaleData : McAELOEExtValCkStDatType; (*Use stale data for validity check*)
		NetTime : McAELOEExtValCkNetTimeType; (*Use net time for validity check*)
		EncoderOk : McAELOEExtValCkEncOkType; (*Use encoder ok flag for validity check (True = position valid)*)
	END_STRUCT;
	McAELOEExtRefPEnum :
		( (*Reference pulse selector setting*)
		mcAELOEERP_NOT_USE := 0, (*Not used - Reference pulse is not used*)
		mcAELOEERP_IO_CH := 1, (*I/O channel - Reference pulse is used with I/O channel*)
		mcAELOEERP_VAR := 2, (*Variable - Reference pulse is used with variable*)
		mcAELOEERP_IO_CH_DINT := 3, (*I/O channel DINT - Reference pulse is used with I/O channel*)
		mcAELOEERP_VAR_DINT := 4 (*Variable DINT - Reference pulse is used with variable*)
		);
	McAELOEExtRefPIOChPosType : STRUCT (*Position of the reference pulse*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse position*)
	END_STRUCT;
	McAELOEExtRefPIOChCntType : STRUCT (*Count of the reference pulse*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	McAELOEExtRefPIOChType : STRUCT (*Type mcAELOEERP_IO_CH settings*)
		Position : McAELOEExtRefPIOChPosType; (*Position of the reference pulse*)
		Count : McAELOEExtRefPIOChCntType; (*Count of the reference pulse*)
	END_STRUCT;
	McAELOEExtRefPVarPosType : STRUCT (*Position of the reference pulse*)
		PVMapping : STRING[250]; (*Input source for the reference pulse position*)
	END_STRUCT;
	McAELOEExtRefPVarCntType : STRUCT (*Count of the reference pulse*)
		PVMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	McAELOEExtRefPVarType : STRUCT (*Type mcAELOEERP_VAR settings*)
		Position : McAELOEExtRefPVarPosType; (*Position of the reference pulse*)
		Count : McAELOEExtRefPVarCntType; (*Count of the reference pulse*)
	END_STRUCT;
	McAELOEExtRefPIOChDINTPosType : STRUCT (*Position of the reference pulse*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse position*)
	END_STRUCT;
	McAELOEExtRefPIOChDINTCntType : STRUCT (*Count of the reference pulse*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	McAELOEExtRefPIOChDINTType : STRUCT (*Type mcAELOEERP_IO_CH_DINT settings*)
		Position : McAELOEExtRefPIOChDINTPosType; (*Position of the reference pulse*)
		Count : McAELOEExtRefPIOChDINTCntType; (*Count of the reference pulse*)
	END_STRUCT;
	McAELOEExtRefPVarDINTPosType : STRUCT (*Position of the reference pulse*)
		PVMapping : STRING[250]; (*Input source for the reference pulse position*)
	END_STRUCT;
	McAELOEExtRefPVarDINTCntType : STRUCT (*Count of the reference pulse*)
		PVMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	McAELOEExtRefPVarDINTType : STRUCT (*Type mcAELOEERP_VAR_DINT settings*)
		Position : McAELOEExtRefPVarDINTPosType; (*Position of the reference pulse*)
		Count : McAELOEExtRefPVarDINTCntType; (*Count of the reference pulse*)
	END_STRUCT;
	McAELOEExtRefPType : STRUCT (*Usage and settings for the evaluation of the reference pulse of the encoder*)
		Type : McAELOEExtRefPEnum; (*Reference pulse selector setting*)
		IOChannel : McAELOEExtRefPIOChType; (*Type mcAELOEERP_IO_CH settings*)
		Variable : McAELOEExtRefPVarType; (*Type mcAELOEERP_VAR settings*)
		IOChannelDINT : McAELOEExtRefPIOChDINTType; (*Type mcAELOEERP_IO_CH_DINT settings*)
		VariableDINT : McAELOEExtRefPVarDINTType; (*Type mcAELOEERP_VAR_DINT settings*)
	END_STRUCT;
	McAELOEEPosFltrEnum :
		( (*Position filter selector setting*)
		mcAELOEEPF_EXTPOL_AND_DIST := 0 (*Extrapolation and disturbance - An extrapolation and disturbance filter is used*)
		);
	McAELOEEPosFltrExtpolDistType : STRUCT (*Type mcAELOEEPF_EXTPOL_AND_DIST settings*)
		PositionFilterTimeConstant : REAL; (*Time constant for actual position filter*)
		ExtrapolationTime : REAL; (*Extrapolation time for actual position filter*)
	END_STRUCT;
	McAELOEEPosFltrType : STRUCT (*Filter for the encoder position*)
		Type : McAELOEEPosFltrEnum; (*Position filter selector setting*)
		ExtrapolationAndDisturbance : McAELOEEPosFltrExtpolDistType; (*Type mcAELOEEPF_EXTPOL_AND_DIST settings*)
	END_STRUCT;
	McAELOneEncMotAndPosEncExtType : STRUCT (*Type mcAELAE_EXT settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution [Lines/Rev]*)
		PositionType : McAELOEExtPosTypType; (*Type of the encoder*)
		PositionSource : McAELOEExtPosSrcType; (*Position source*)
		ValidityCheck : McAELOEExtValCkType; (*Check if given position is valid*)
		ReferencePulse : McAELOEExtRefPType; (*Usage and settings for the evaluation of the reference pulse of the encoder*)
		PositionFilter : McAELOEEPosFltrType; (*Filter for the encoder position*)
	END_STRUCT;
	McAELOneEncMotAndPosEncType : STRUCT
		Type : McAELAllEncEnum; (*Motor and position encoder selector setting*)
		External : McAELOneEncMotAndPosEncExtType; (*Type mcAELAE_EXT settings*)
	END_STRUCT;
	McAELEncParSetEnum :
		( (*Encoder parameter set selection*)
		mcAELEPS_AUT := 0, (*Automatic - Automatic selection of encoder parameter set (see AS-Help)*)
		mcAELEPS_ENCOD1 := 1, (*ENCOD1 - Parameter set ENCOD1*)
		mcAELEPS_ENCOD2 := 2 (*ENCOD2 - Parameter set ENCOD2*)
		);
	McAELOneEncType : STRUCT (*Type mcAEL_ONE_ENC settings*)
		MotorAndPositionEncoder : McAELOneEncMotAndPosEncType;
		EncoderParameterSet : McAELEncParSetEnum; (*Encoder parameter set selection*)
	END_STRUCT;
	McAELTwoEncMotEncType : STRUCT
		Type : McAELAllEncEnum; (*Motor encoder selector setting*)
	END_STRUCT;
	McAELTwoEncPosEncCmnScCntDirEnum :
		( (*Direction of the encoder in which the position value is increasing*)
		mcAELTEPECSCD_STD := 0, (*Standard - Clockwise*)
		mcAELTEPECSCD_INV := 1 (*Inverse - Counter-clockwise*)
		);
	McAELTwoEncPosEncCmnScType : STRUCT (*Encoder scaling based on a gear ratio and / or a movement transformation factor*)
		Gearbox : McCfgGearBoxType; (*Specifies a gearbox by defining the ratio between a gearbox input and output*)
		RotaryToLinearTransformation : McCfgRotToLinTrfType; (*Specifies a transformation factor between the output of the gear and the actual load movement*)
		CountDirection : McAELTwoEncPosEncCmnScCntDirEnum; (*Direction of the encoder in which the position value is increasing*)
	END_STRUCT;
	McAELTwoEncPosEncCmnType : STRUCT (*Common settings for all Type values*)
		Scaling : McAELTwoEncPosEncCmnScType; (*Encoder scaling based on a gear ratio and / or a movement transformation factor*)
		EncoderParameterSet : McAELEncParSetEnum; (*Encoder parameter set selection*)
	END_STRUCT;
	McAELTwoEncPosEncType : STRUCT
		Type : McAELAllEncEnum; (*Position encoder selector setting*)
		Common : McAELTwoEncPosEncCmnType; (*Common settings for all Type values*)
	END_STRUCT;
	McAELTwoEncType : STRUCT (*Type mcAEL_TWO_ENC settings*)
		MotorEncoder : McAELTwoEncMotEncType;
		EncoderParameterSet : McAELEncParSetEnum; (*Encoder parameter set selection*)
		PositionEncoder : McAELTwoEncPosEncType;
		PositionDifferenceLimit : REAL; (*Position difference limit between motor and position encoder for stopping a movement [measurement units]*)
	END_STRUCT;
	McAELNoEncELCtrlModEnum :
		( (*Encoderless control mode selector setting*)
		mcAELNEECM_NOT_USE_ := 0, (*Not used - Encoderless control is not used*)
		mcAELNEECM_EL_CUR_CTRL := 1, (*Encoderless current control - Motor follows the current space vector*)
		mcAELNEECM_EL_POS_CTRL := 2 (*Encoderless position control - Actual position is estimated from magnetic flux*)
		);
	McACELCSetCurEnum :
		( (*Set current direct component selector setting*)
		mcACELCSC_AUT := 0, (*Automatic - Default value from drive is used*)
		mcACELCSC_USR := 1 (*User - Defined value is used*)
		);
	McACELCSetCurUsrType : STRUCT (*Type mcACELCSC_USR settings*)
		Value : REAL; (*Encoderless control: Set current direct component [A]*)
	END_STRUCT;
	McACELCSetCurType : STRUCT (*Encoderless control: Set current direct component*)
		Type : McACELCSetCurEnum; (*Set current direct component selector setting*)
		User : McACELCSetCurUsrType; (*Type mcACELCSC_USR settings*)
	END_STRUCT;
	McACELCKeepHomeEnum :
		( (*Keep homing status selector setting*)
		mcACELCKH_NOT_USE := 0, (*Not used - Homing status is reset when controller is switched off*)
		mcACELCKH_USE := 1 (*Used - Homing status is kept when controller is switched off*)
		);
	McACELCKeepHomeType : STRUCT (*Keep homing status when controller is switched off*)
		Type : McACELCKeepHomeEnum; (*Keep homing status selector setting*)
	END_STRUCT;
	McACELCKeepPhaseEnum :
		( (*Keep phasing status selector setting*)
		mcACELCKP_NOT_USE := 0, (*Not used - Phasing status is reset when controller is switched off*)
		mcACELCKP_USE := 1 (*Used - Phasing status is kept when controller is switched off*)
		);
	McACELCKeepPhaseType : STRUCT (*Keep phasing status when controller is switched off*)
		Type : McACELCKeepPhaseEnum; (*Keep phasing status selector setting*)
	END_STRUCT;
	McACELCCurType : STRUCT (*Type mcAELNEECM_EL_CUR_CTRL settings*)
		SetCurrent : McACELCSetCurType; (*Encoderless control: Set current direct component*)
		KeepHoming : McACELCKeepHomeType; (*Keep homing status when controller is switched off*)
		KeepPhasing : McACELCKeepPhaseType; (*Keep phasing status when controller is switched off*)
	END_STRUCT;
	McACELCPosTlEnum :
		( (*Transition level selector setting*)
		mcACELCPT_TLAUTOMATIC := 0, (*TlAutomatic - Default value from drive is used (10/Number of motor pol pairs)*)
		mcACELCPT_TLUSER := 1 (*TlUser - Defined value is used*)
		);
	McACELCPosTlTlUserType : STRUCT (*Type mcACELCPT_TLUSER settings*)
		Value : REAL; (*Encoderless control: Transition level [1/s]*)
	END_STRUCT;
	McACELCPosTlType : STRUCT (*Encoderless control: Transition level*)
		Type : McACELCPosTlEnum; (*Transition level selector setting*)
		TlUser : McACELCPosTlTlUserType; (*Type mcACELCPT_TLUSER settings*)
	END_STRUCT;
	McACELCPosTzEnum :
		( (*Transition zone selector setting*)
		mcACELCPT_TZAUTOMATIC := 0, (*TzAutomatic - Default value from drive is used (5/Number of motor pol pairs)*)
		mcACELCPT_TZUSER := 1 (*TzUser - Defined value is used*)
		);
	McACELCPosTzTzUserType : STRUCT (*Type mcACELCPT_TZUSER settings*)
		Value : REAL; (*Encoderless control: Transition zone [1/s]*)
	END_STRUCT;
	McACELCPosTzType : STRUCT (*Encoderless control: Transition zone*)
		Type : McACELCPosTzEnum; (*Transition zone selector setting*)
		TzUser : McACELCPosTzTzUserType; (*Type mcACELCPT_TZUSER settings*)
	END_STRUCT;
	McACELCStalDetEnum :
		( (*Stall detection selector setting*)
		mcACELCSD_NOT_USE := 0, (*Not used - Stall detection is inactive*)
		mcACELCSD_USE := 1 (*Used - Stall detection is active after given stall detection time*)
		);
	McACELCStalDetUseType : STRUCT (*Type mcACELCSD_USE settings*)
		StallDetectionTime : REAL; (*Encoderless control: Stall detection time [s]*)
	END_STRUCT;
	McACELCStalDetType : STRUCT (*Encoderless control: Stall detection*)
		Type : McACELCStalDetEnum; (*Stall detection selector setting*)
		Used : McACELCStalDetUseType; (*Type mcACELCSD_USE settings*)
	END_STRUCT;
	McACELCInvAdjType : STRUCT (*Encoderless control: Inverter parameters of characteristic current-voltage curve*)
		GainFactor : REAL; (*Encoderless control: Inverter Amplification factor*)
		Exponent : REAL; (*Encoderless control: Inverter Exponent [1/A]*)
	END_STRUCT;
	McACELCPosType : STRUCT (*Type mcAELNEECM_EL_POS_CTRL settings*)
		SetCurrent : McACELCSetCurType; (*Encoderless control: Set current direct component*)
		Tl : McACELCPosTlType; (*Encoderless control: Transition level*)
		Tz : McACELCPosTzType; (*Encoderless control: Transition zone*)
		KeepHoming : McACELCKeepHomeType; (*Keep homing status when controller is switched off*)
		KeepPhasing : McACELCKeepPhaseType; (*Keep phasing status when controller is switched off*)
		TransferTime : REAL; (*Encoderless control: Transfer time [s]*)
		StallDetection : McACELCStalDetType; (*Encoderless control: Stall detection*)
		InverterAdjustment : McACELCInvAdjType; (*Encoderless control: Inverter parameters of characteristic current-voltage curve*)
	END_STRUCT;
	McAELNoEncELCtrlModType : STRUCT (*Encoderless control mode*)
		Type : McAELNoEncELCtrlModEnum; (*Encoderless control mode selector setting*)
		EncoderlessCurrentControl : McACELCCurType; (*Type mcAELNEECM_EL_CUR_CTRL settings*)
		EncoderlessPositionControl : McACELCPosType; (*Type mcAELNEECM_EL_POS_CTRL settings*)
	END_STRUCT;
	McAELNoEncType : STRUCT (*Type mcAEL_NO_ENC settings*)
		EncoderlessControlMode : McAELNoEncELCtrlModType; (*Encoderless control mode*)
	END_STRUCT;
	McAELType : STRUCT
		Type : McAELEnum; (*Encoder link selector setting*)
		OneEncoder : McAELOneEncType; (*Type mcAEL_ONE_ENC settings*)
		TwoEncoders : McAELTwoEncType; (*Type mcAEL_TWO_ENC settings*)
		NoEncoder : McAELNoEncType; (*Type mcAEL_NO_ENC settings*)
	END_STRUCT;
	McACModEnum :
		( (*Mode selector setting*)
		mcACM_POS_CTRL := 0, (*Position controller - Automatic speed feed-forward with prediction time > 0*)
		mcACM_POS_CTRL_TORQ_FF := 1, (*Position controller torque ff - Torque feed-forward with specified parameters*)
		mcACM_POS_CTRL_MDL_BASED := 3, (*Position controller model based - Model based control with specified parameters*)
		mcACM_V_FREQ_CTRL := 2, (*Voltage frequency control - Voltage/frequency control of induction motor with specified parameters*)
		mcACM_VIB_CTRL := 4 (*Vibration control - Vibration control with specified parameters*)
		);
	McACPCType : STRUCT (*Position controller parameters*)
		ProportionalGain : REAL; (*Proportional amplification [1/s]*)
		IntegrationTime : REAL; (*Integral action time [s]*)
		PredictionTime : REAL; (*Prediction time [s]*)
		TotalDelayTime : REAL; (*Total delay time [s]*)
	END_STRUCT;
	McACSCType : STRUCT (*Speed controller parameters*)
		ProportionalGain : REAL; (*Proportional amplification [As]*)
		IntegrationTime : REAL; (*Integral action time [s]*)
		FilterTime : REAL; (*Filter time constant [s]*)
	END_STRUCT;
	McACLFSEnum :
		( (*Loop filter 1-3 selector setting*)
		mcACLFS_NOT_USE := 0, (*Not used - Filter is not active*)
		mcACLFS_LP_2ND_ORD := 1, (*Lowpass 2nd order - Lowpass 2nd order*)
		mcACLFS_NOTCH := 2, (*Notch - Notch*)
		mcACLFS_BIQUAD := 3, (*Biquad - Biquad*)
		mcACLFS_DISC_TIME_TRAN_FUN := 4, (*Discrete time transfer function - Discrete time transfer function*)
		mcACLFS_LIM := 5, (*Limiter - Limiter*)
		mcACLFS_LIN_LIM := 6, (*Linear limitation - Linear limitation*)
		mcACLFS_RISE_TIME_LIM := 7, (*Rise time limitation - Rise time limitation*)
		mcACLFS_COMP := 8, (*Compensation - Compensation*)
		mcACLFS_ADPT_NOTCH := 9, (*Adaptive notch - Adaptive notch*)
		mcACLFS_VAR_CTRL_NOTCH := 10 (*Variable controlled notch - Variable controlled notch*)
		);
	McACLFSLP2ndOrdType : STRUCT (*Type mcACLFS_LP_2ND_ORD settings*)
		CutOffFrequency : REAL; (*Cut off frequency [Hz]*)
	END_STRUCT;
	McACLFSNotchType : STRUCT (*Type mcACLFS_NOTCH settings*)
		CenterFrequency : REAL; (*Center frequency [Hz]*)
		Bandwidth : REAL; (*Bandwidth [Hz]*)
	END_STRUCT;
	McACLFSBiquadType : STRUCT (*Type mcACLFS_BIQUAD settings*)
		FrequencyNumerator : REAL; (*Frequency numerator [Hz]*)
		DampingNumerator : REAL; (*Damping numerator*)
		FrequencyDenominator : REAL; (*Frequency denominator [Hz]*)
		DampingDenominator : REAL; (*Damping Denominator*)
	END_STRUCT;
	McACLFSDiscTimeTranFunType : STRUCT (*Type mcACLFS_DISC_TIME_TRAN_FUN settings*)
		a0DenominatorPolynomial : REAL; (*Coefficient a0 of denominator polynomial*)
		a1DenominatorPolynomial : REAL; (*Coefficient a1 of denominator polynomial*)
		b0NumeratorPolynomial : REAL; (*Coefficient b0 of numerator polynomial*)
		b1NumeratorPolynomial : REAL; (*Coefficient b1 of numerator polynomial*)
		b2NumeratorPolynomial : REAL; (*Coefficient b2 of numerator polynomial*)
	END_STRUCT;
	McACLFSLLimEnum :
		( (*Positive limit selector setting*)
		mcACLFSLLim_ACP_PARID := 0, (*ACOPOS ParID - Use an ACOPOS ParID*)
		mcACLFSLLim_FIX_VAL := 1 (*Fixed value - Use a fixed value*)
		);
	McACLFSLLimAcpParIDType : STRUCT (*Type mcACLFSLLim_ACP_PARID settings*)
		ParID : UINT; (*ParID*)
	END_STRUCT;
	McACLFSLLimFixValType : STRUCT (*Type mcACLFSLLim_FIX_VAL settings*)
		Value : REAL; (*Value [A]*)
	END_STRUCT;
	McACLFSLLimType : STRUCT (*Select if a fixed value for the positive limit or if the value of an ACOPOS ParID is used therefore*)
		Type : McACLFSLLimEnum; (*Positive limit selector setting*)
		ACOPOSParID : McACLFSLLimAcpParIDType; (*Type mcACLFSLLim_ACP_PARID settings*)
		FixedValue : McACLFSLLimFixValType; (*Type mcACLFSLLim_FIX_VAL settings*)
	END_STRUCT;
	McACLFSLimType : STRUCT (*Type mcACLFS_LIM settings*)
		PositiveLimit : McACLFSLLimType; (*Select if a fixed value for the positive limit or if the value of an ACOPOS ParID is used therefore*)
		NegativeLimit : McACLFSLLimType; (*Select if a fixed value for the negative limit or if the value of an ACOPOS ParID is used therefore*)
	END_STRUCT;
	McACLFSLinLimType : STRUCT (*Type mcACLFS_LIN_LIM settings*)
		InputParID : UINT; (*ACOPOS ParID for the input*)
		InputLimit : REAL; (*Input limit value for full limitation*)
		Gradient : REAL; (*Gradient*)
	END_STRUCT;
	McACLFSRiseTimeLimType : STRUCT (*Type mcACLFS_RISE_TIME_LIM settings*)
		RiseTime : REAL; (*Rise time [s]*)
		NormalizedLimit : REAL; (*Normalized limit*)
	END_STRUCT;
	McACLFSCompType : STRUCT (*Type mcACLFS_COMP settings*)
		MultiplicationFactorParID : UINT; (*Multiplication Factor ParID*)
		AdditiveValueParID : UINT; (*Additive Value ParID*)
	END_STRUCT;
	McACLFSAdptNotchType : STRUCT (*Type mcACLFS_ADPT_NOTCH settings*)
		CenterFrequency : REAL; (*Center frequency [Hz]*)
		Bandwidth : REAL; (*Bandwidth [Hz]*)
		LowerFrequencyLimit : REAL; (*Lower frequency limit [Hz]*)
		UpperFrequencyLimit : REAL; (*Upper frequency limit [Hz]*)
		AdaptionThreshold : REAL; (*Adaption threshold [A]*)
	END_STRUCT;
	McACLFSVarCtrlNotchType : STRUCT (*Type mcACLFS_VAR_CTRL_NOTCH settings*)
		CenterFrequencyParID : UINT; (*Center frequency ParID*)
		BandwidthParID : UINT; (*Bandwidth ParID*)
	END_STRUCT;
	McACLFSType : STRUCT (*Type of the loop filter*)
		Type : McACLFSEnum; (*Loop filter 1-3 selector setting*)
		Lowpass2ndOrder : McACLFSLP2ndOrdType; (*Type mcACLFS_LP_2ND_ORD settings*)
		Notch : McACLFSNotchType; (*Type mcACLFS_NOTCH settings*)
		Biquad : McACLFSBiquadType; (*Type mcACLFS_BIQUAD settings*)
		DiscreteTimeTransferFunction : McACLFSDiscTimeTranFunType; (*Type mcACLFS_DISC_TIME_TRAN_FUN settings*)
		Limiter : McACLFSLimType; (*Type mcACLFS_LIM settings*)
		LinearLimitation : McACLFSLinLimType; (*Type mcACLFS_LIN_LIM settings*)
		RiseTimeLimitation : McACLFSRiseTimeLimType; (*Type mcACLFS_RISE_TIME_LIM settings*)
		Compensation : McACLFSCompType; (*Type mcACLFS_COMP settings*)
		AdaptiveNotch : McACLFSAdptNotchType; (*Type mcACLFS_ADPT_NOTCH settings*)
		VariableControlledNotch : McACLFSVarCtrlNotchType; (*Type mcACLFS_VAR_CTRL_NOTCH settings*)
	END_STRUCT;
	McACLFType : STRUCT (*Parameters of the loop filters*)
		LoopFilter : ARRAY[0..2] OF McACLFSType; (*Type of the loop filter*)
	END_STRUCT;
	McACCTMEnum :
		( (*Cycle time mode selector setting*)
		mcACCTM_STD := 0, (*Standard - 400µs/200µs/200µs*)
		mcACCTM_ADV := 1, (*Advanced - 100µs/100µs/100µs*)
		mcACCTM_PWR := 2 (*Power - 50µs/50µs/50µs*)
		);
	McACCTMSgenEnum :
		( (*Set value generation selector setting*)
		mcACCTMSgen_STD := 0, (*Standard - Standard: 400µs*)
		mcACCTMSgen_CYCLE_TIME_OF_CTRL := 128 (*Cycle time of controller - Position controller cycle time*)
		);
	McACCTMSgenType : STRUCT (*Selects if set value generation is done in the faster position controller cycle*)
		Type : McACCTMSgenEnum; (*Set value generation selector setting*)
	END_STRUCT;
	McACCTMIOsEnum :
		( (*IOs on plug-in cards selector setting*)
		mcACCTMIOs_STD := 0, (*Standard - Standard: 400µs*)
		mcACCTMIOs_CYCLE_TIME_OF_CTRL := 1 (*Cycle time of controller - Position controller cycle time*)
		);
	McACCTMIOsType : STRUCT (*Selects if DIO/AIOs on plug-in cards are handled in faster position controller cycle*)
		Type : McACCTMIOsEnum; (*IOs on plug-in cards selector setting*)
	END_STRUCT;
	McACCTMAdvType : STRUCT (*Type mcACCTM_ADV settings*)
		SetValueGeneration : McACCTMSgenType; (*Selects if set value generation is done in the faster position controller cycle*)
		IOsOnPlugInCards : McACCTMIOsType; (*Selects if DIO/AIOs on plug-in cards are handled in faster position controller cycle*)
	END_STRUCT;
	McACCTMPwrType : STRUCT (*Type mcACCTM_PWR settings*)
		SetValueGeneration : McACCTMSgenType; (*Selects if set value generation is done in the faster position controller cycle*)
		IOsOnPlugInCards : McACCTMIOsType; (*Selects if DIO/AIOs on plug-in cards are handled in faster position controller cycle*)
	END_STRUCT;
	McACCTMType : STRUCT (*Controller cascade cycle time mode; Position/Speed/Current; Check documentation for limitations*)
		Type : McACCTMEnum; (*Cycle time mode selector setting*)
		Advanced : McACCTMAdvType; (*Type mcACCTM_ADV settings*)
		Power : McACCTMPwrType; (*Type mcACCTM_PWR settings*)
	END_STRUCT;
	McACMPCType : STRUCT (*Type mcACM_POS_CTRL settings*)
		Position : McACPCType; (*Position controller parameters*)
		Speed : McACSCType; (*Speed controller parameters*)
		LoopFilters : McACLFType; (*Parameters of the loop filters*)
		CycleTimeMode : McACCTMType; (*Controller cascade cycle time mode; Position/Speed/Current; Check documentation for limitations*)
	END_STRUCT;
	McACPCFFType : STRUCT (*Position controller parameters*)
		ProportionalGain : REAL; (*Proportional amplification [1/s]*)
		IntegrationTime : REAL; (*Integral action time [s]*)
		TotalDelayTime : REAL; (*Total delay time [s]*)
	END_STRUCT;
	McACMPCFFFFwdEnum :
		( (*Feed-forward selector setting*)
		mcACMPCFFFF_STD := 0, (*Standard - Based on several parameters the torque feed-forward calculation is done on the axis*)
		mcACMPCFFFF_CYC_VAL_FROM_AX_GRP := 1 (*Cyclic value from axes group - The torque feed-forward calculation is done by the axes group or the application on the PLC and the value is forwarded to the axis*)
		);
	McACMPCFFFFwdStdType : STRUCT (*Type mcACMPCFFFF_STD settings*)
		TorqueLoad : REAL; (*Load torque [Nm]*)
		TorquePositive : REAL; (*Torque in positive direction [Nm]*)
		TorqueNegative : REAL; (*Torque in negative direction [Nm]*)
		SpeedTorqueFactor : REAL; (*Speed torque factor [Nms]*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		AccelerationFilterTime : REAL; (*Acceleration filter time constant [s]*)
	END_STRUCT;
	McACMPCFFFFwdType : STRUCT (*Torque feed-forward control parameters*)
		Type : McACMPCFFFFwdEnum; (*Feed-forward selector setting*)
		Standard : McACMPCFFFFwdStdType; (*Type mcACMPCFFFF_STD settings*)
	END_STRUCT;
	McACMPCFFType : STRUCT (*Type mcACM_POS_CTRL_TORQ_FF settings*)
		Position : McACPCFFType; (*Position controller parameters*)
		Speed : McACSCType; (*Speed controller parameters*)
		FeedForward : McACMPCFFFFwdType; (*Torque feed-forward control parameters*)
		LoopFilters : McACLFType; (*Parameters of the loop filters*)
		CycleTimeMode : McACCTMType; (*Controller cascade cycle time mode; Position/Speed/Current; Check documentation for limitations*)
	END_STRUCT;
	McACMPCMBCPosType : STRUCT (*Position controller parameters*)
		ProportionalGain : REAL; (*Proportional amplification [1/s]*)
		IntegrationTime : REAL; (*Integral action time [s]*)
		TotalDelayTime : REAL; (*Total delay time [s]*)
	END_STRUCT;
	McACMPCMBCFFEnum :
		( (*Feed-forward selector setting*)
		mcACMPCMBCFF_STD := 0, (*Standard - Based on several parameters the torque feed-forward calculation is done on the axis*)
		mcACMPCMBCFF_PRED_SPD := 1, (*Predictive speed - Predictive speed*)
		mcACMPCMBCFF_TWO_MASS_MDL_BASED := 2, (*Two mass model based - Two mass model based*)
		mcACMPCMBCFF_FRICT_COMP := 3 (*Friction compensation - Friction compensation*)
		);
	McACMPCMBCFFStdType : STRUCT (*Type mcACMPCMBCFF_STD settings*)
		TorqueLoad : REAL; (*Load torque [Nm]*)
		TorquePositive : REAL; (*Torque in positive direction [Nm]*)
		TorqueNegative : REAL; (*Torque in negative direction [Nm]*)
		SpeedTorqueFactor : REAL; (*Speed torque factor [Nms]*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		AccelerationFilterTime : REAL; (*Acceleration filter time constant [s]*)
	END_STRUCT;
	McACMPCMBCFFPredSpdType : STRUCT (*Type mcACMPCMBCFF_PRED_SPD settings*)
		PredictionTime : REAL; (*Prediction time [s]*)
	END_STRUCT;
	McACMPCMBCFFTwoMassMdlBasedType : STRUCT (*Type mcACMPCMBCFF_TWO_MASS_MDL_BASED settings*)
		TorqueLoad : REAL; (*Load torque [Nm]*)
		TorquePositive : REAL; (*Torque in positive direction [Nm]*)
		TorqueNegative : REAL; (*Torque in negative direction [Nm]*)
		AccelerationFilterTime : REAL; (*Acceleration filter time constant [s]*)
	END_STRUCT;
	McACMPCMBCFFFrictCompType : STRUCT (*Type mcACMPCMBCFF_FRICT_COMP settings*)
		TorqueLoad : REAL; (*Load torque [Nm]*)
		TorquePositive : REAL; (*Torque in positive direction [Nm]*)
		TorqueNegative : REAL; (*Torque in negative direction [Nm]*)
		SpeedTorqueFactor : REAL; (*Speed torque factor [Nms]*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		AccelerationFilterTime : REAL; (*Acceleration filter time constant [s]*)
		ActivationSpeed : REAL; (*Compensation is activated when the speed enters this window [measurement units/s]*)
		DeactivationLagError : REAL; (*Compensation is deactivated when the lag error enters this window [measurement units]*)
		TimeConstant : REAL; (*Time constant of the filter for smoothing the feed-forward signal [s]*)
	END_STRUCT;
	McACMPCMBCFFType : STRUCT (*Feed-forward control parameters*)
		Type : McACMPCMBCFFEnum; (*Feed-forward selector setting*)
		Standard : McACMPCMBCFFStdType; (*Type mcACMPCMBCFF_STD settings*)
		PredictiveSpeed : McACMPCMBCFFPredSpdType; (*Type mcACMPCMBCFF_PRED_SPD settings*)
		TwoMassModelBased : McACMPCMBCFFTwoMassMdlBasedType; (*Type mcACMPCMBCFF_TWO_MASS_MDL_BASED settings*)
		FrictionCompensation : McACMPCMBCFFFrictCompType; (*Type mcACMPCMBCFF_FRICT_COMP settings*)
	END_STRUCT;
	McACMPCMBCFdbkEnum :
		( (*Feedback selector setting*)
		mcACMPCMBCF_STD := 0, (*Standard - Standard*)
		mcACMPCMBCF_ONE_MASS_MDL_BASED := 1, (*One mass model based - One mass model based*)
		mcACMPCMBCF_TWO_MASS_MDL_BASED := 2, (*Two mass model based - Two mass model based*)
		mcACMPCMBCF_TWO_ENC_SPD_BASED := 3 (*Two encoder speed based - Two encoder speed based*)
		);
	McACMPCMBCFdbkOneMassMdlBsdType : STRUCT (*Type mcACMPCMBCF_ONE_MASS_MDL_BASED settings*)
		SpeedMixingFactor : REAL; (*Speed mixing factor*)
		SpeedProportionalGain : REAL; (*Speed proportional gain [As]*)
	END_STRUCT;
	McACMPCMBCFdbkTwoMassMdlBsdType : STRUCT (*Type mcACMPCMBCF_TWO_MASS_MDL_BASED settings*)
		SpeedMixingFactor : REAL; (*Speed mixing factor*)
		SpeedProportionalGain : REAL; (*Speed proportional gain [As]*)
	END_STRUCT;
	McACMPCMBCFdbkTwoEncSpdBasedType : STRUCT (*Type mcACMPCMBCF_TWO_ENC_SPD_BASED settings*)
		SpeedMixingFactor : REAL; (*Speed mixing factor*)
		SpeedProportionalGain : REAL; (*Speed proportional gain [As]*)
	END_STRUCT;
	McACMPCMBCFdbkType : STRUCT (*Feedback control parameters*)
		Type : McACMPCMBCFdbkEnum; (*Feedback selector setting*)
		OneMassModelBased : McACMPCMBCFdbkOneMassMdlBsdType; (*Type mcACMPCMBCF_ONE_MASS_MDL_BASED settings*)
		TwoMassModelBased : McACMPCMBCFdbkTwoMassMdlBsdType; (*Type mcACMPCMBCF_TWO_MASS_MDL_BASED settings*)
		TwoEncoderSpeedBased : McACMPCMBCFdbkTwoEncSpdBasedType; (*Type mcACMPCMBCF_TWO_ENC_SPD_BASED settings*)
	END_STRUCT;
	McACMPCMBCMdlMass1Type : STRUCT (*Mass 1 parameters*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		ViscousFriction : REAL; (*Viscous friction [Nms]*)
	END_STRUCT;
	McACMPCMBCMdlMass2Type : STRUCT (*Mass 2 parameters*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		ViscousFriction : REAL; (*Viscous friction [Nms]*)
		Stiffness : REAL; (*Stiffness of the coupling to mass 1 [Nm/rad]*)
		Damping : REAL; (*Damping of the coupling to mass 1 [Nms/rad]*)
	END_STRUCT;
	McACMPCMBCMdlType : STRUCT (*Load model parameters*)
		Mass1 : McACMPCMBCMdlMass1Type; (*Mass 1 parameters*)
		Mass2 : McACMPCMBCMdlMass2Type; (*Mass 2 parameters*)
	END_STRUCT;
	McACMPCMBCType : STRUCT (*Type mcACM_POS_CTRL_MDL_BASED settings*)
		Position : McACMPCMBCPosType; (*Position controller parameters*)
		Speed : McACSCType; (*Speed controller parameters*)
		Feedforward : McACMPCMBCFFType; (*Feed-forward control parameters*)
		Feedback : McACMPCMBCFdbkType; (*Feedback control parameters*)
		Model : McACMPCMBCMdlType; (*Load model parameters*)
		LoopFilters : McACLFType; (*Parameters of the loop filters*)
		CycleTimeMode : McACCTMType; (*Controller cascade cycle time mode; Position/Speed/Current; Check documentation for limitations*)
	END_STRUCT;
	McACMVFCVFTypEnum :
		( (*Type of characteristic curve*)
		mcACMVFCVFT_LIN := 129, (*Linear - Linear characteristic curve*)
		mcACMVFCVFT_CONST_LD_TORQ := 131, (*Constant load torque - Characteristic curve for constant load torque*)
		mcACMVFCVFT_QUAD := 130 (*Quadratic - Characteristic curve for quadratic load curves*)
		);
	McACMVFCVFAutCfgEnum :
		( (*Automatic configuration selector setting*)
		mcACMVFCVFAC_MOT_PAR_BASED := 1, (*Motor parameter based - Automatic configuration based on motor parameters*)
		mcACMVFCVFAC_NOT_USE := 0 (*Not used - No automatic configuration*)
		);
	McACMVFCVFAutCfgNotUseType : STRUCT (*Type mcACMVFCVFAC_NOT_USE settings*)
		BoostVoltage : REAL; (*Boost voltage [V]*)
		RatedVoltage : REAL; (*Rated voltage [V]*)
		RatedFrequency : REAL; (*Rated frequency [Hz]*)
	END_STRUCT;
	McACMVFCVFAutCfgType : STRUCT (*Automatic configuration of parameters*)
		Type : McACMVFCVFAutCfgEnum; (*Automatic configuration selector setting*)
		NotUsed : McACMVFCVFAutCfgNotUseType; (*Type mcACMVFCVFAC_NOT_USE settings*)
	END_STRUCT;
	McACMVFCVFType : STRUCT (*V/f control parameters*)
		Type : McACMVFCVFTypEnum; (*Type of characteristic curve*)
		AutomaticConfiguration : McACMVFCVFAutCfgType; (*Automatic configuration of parameters*)
		SlipCompensation : REAL; (*Slip compensation: Multiplication factor of compensated frequency*)
		TotalDelayTime : REAL; (*Total delay time [s]*)
	END_STRUCT;
	McACMVFCType : STRUCT (*Type mcACM_V_FREQ_CTRL settings*)
		VoltageFrequency : McACMVFCVFType; (*V/f control parameters*)
	END_STRUCT;
	McACMVCFreqAdptEnum :
		( (*Frequency adaption selector setting*)
		mcACMVCFA_NOT_USE := 0, (*Not used - The frequency adaption is not used*)
		mcACMVCFA_USE := 1 (*Used - The frequency adaption is not used*)
		);
	McACMVCFreqAdptUseType : STRUCT (*Type mcACMVCFA_USE settings*)
		ProportionalGain : REAL; (*Vibration control: Amplification factor frequency control [Hz/rad]*)
		IntegrationTime : REAL; (*Vibration control: Integration time frequency control [s]*)
		ReferenceValue : REAL; (*Vibration control: Reference value of 3rd harmonic phase shift [rad]*)
	END_STRUCT;
	McACMVCFreqAdptType : STRUCT (*Usage of the frequency adaption*)
		Type : McACMVCFreqAdptEnum; (*Frequency adaption selector setting*)
		Used : McACMVCFreqAdptUseType; (*Type mcACMVCFA_USE settings*)
	END_STRUCT;
	McACMVCAmpAdptEnum :
		( (*Amplitude adaption selector setting*)
		mcACMVCAA_NOT_USE := 0, (*Not used - The amplitude adaption is not used*)
		mcACMVCAA_USE := 1 (*Used - The amplitude adaption is not used*)
		);
	McACMVCAmpAdptUseType : STRUCT (*Type mcACMVCAA_USE settings*)
		ProportionalGain : REAL; (*Vibration control: Amplification factor amplitude control [A/V]*)
		IntegrationTime : REAL; (*Vibration control: Integration time amplitude control [s]*)
		ReferenceValue : REAL; (*Vibration control: Reference value of 3rd harmonic amplitude [V]*)
	END_STRUCT;
	McACMVCAmpAdptType : STRUCT (*Usage of the amplitude adaption*)
		Type : McACMVCAmpAdptEnum; (*Amplitude adaption selector setting*)
		Used : McACMVCAmpAdptUseType; (*Type mcACMVCAA_USE settings*)
	END_STRUCT;
	McACMVCCurCtrlEnum :
		( (*Current controller selector setting*)
		mcACMVCCC_DEF := 0, (*Default - Usage of the default values calculated on the drive*)
		mcACMVCCC_USR_DEF := 1 (*User defined - The current controller parameters are set by the user*)
		);
	McACMVCCurCtrlUsrDefType : STRUCT (*Type mcACMVCCC_USR_DEF settings*)
		ProportionalGain : REAL; (*Current controller Proportional gain [V/A]*)
		IntegrationTime : REAL; (*Current controller Integration time [s]*)
	END_STRUCT;
	McACMVCCurCtrlType : STRUCT (*Current controller settings*)
		Type : McACMVCCurCtrlEnum; (*Current controller selector setting*)
		UserDefined : McACMVCCurCtrlUsrDefType; (*Type mcACMVCCC_USR_DEF settings*)
	END_STRUCT;
	McACMVCOptParAmpScEnum :
		( (*Amplitude scaling selector setting*)
		mcACMVCOPAS_NOT_USE := 0, (*Not used - Scaling is not used*)
		mcACMVCOPAS_USE := 1 (*Used - Scaling is used*)
		);
	McACMVCOptParAmpScType : STRUCT (*Vibration control: Mode for scaling of amplitude of 3rd harmonic*)
		Type : McACMVCOptParAmpScEnum; (*Amplitude scaling selector setting*)
	END_STRUCT;
	McACMVCOptParType : STRUCT (*Optional parameters*)
		BoostGain : REAL; (*Vibration control: Boost gain*)
		BoostTime : REAL; (*Vibration control: Boost time [s]*)
		BrakeTime : REAL; (*Vibration control: Brake time [s]*)
		AmplitudeLimit : REAL; (*Vibration control: Upper amplitude limit [A]*)
		AmplitudeScaling : McACMVCOptParAmpScType; (*Vibration control: Mode for scaling of amplitude of 3rd harmonic*)
	END_STRUCT;
	McACMVCType : STRUCT (*Type mcACM_VIB_CTRL settings*)
		ExcitationFrequency : REAL; (*Vibration control: Excitation frequency. Range 10..MOTOR_SPEED_MAX/60*0.5 [Hz]*)
		ExcitationAmplitude : REAL; (*Vibration control: Excitation amplitude [A]*)
		AmplificationFactorFirstHarmonic : REAL; (*Vibration control: Amplification factor fundamental first harmonic*)
		AmplificationFactorThirdHarmonic : REAL; (*Vibration control: Amplification factor third harmonic*)
		FrequencyAdaption : McACMVCFreqAdptType; (*Usage of the frequency adaption*)
		AmplitudeAdaption : McACMVCAmpAdptType; (*Usage of the amplitude adaption*)
		CurrentController : McACMVCCurCtrlType; (*Current controller settings*)
		OptionalParameters : McACMVCOptParType; (*Optional parameters*)
		CycleTimeMode : McACCTMType; (*Controller cascade cycle time mode; Position/Speed/Current; Check documentation for limitations*)
	END_STRUCT;
	McACModType : STRUCT (*Mode of the axis controller*)
		Type : McACModEnum; (*Mode selector setting*)
		PositionController : McACMPCType; (*Type mcACM_POS_CTRL settings*)
		PositionControllerTorqueFf : McACMPCFFType; (*Type mcACM_POS_CTRL_TORQ_FF settings*)
		PositionControllerModelBased : McACMPCMBCType; (*Type mcACM_POS_CTRL_MDL_BASED settings*)
		VoltageFrequencyControl : McACMVFCType; (*Type mcACM_V_FREQ_CTRL settings*)
		VibrationControl : McACMVCType; (*Type mcACM_VIB_CTRL settings*)
	END_STRUCT;
	McACType : STRUCT (*Axis controller parameters*)
		Mode : McACModType; (*Mode of the axis controller*)
	END_STRUCT;
	McAHModEnum :
		( (*Mode selector setting*)
		mcAHM_DIR := 0, (*Direct - Direct homing*)
		mcAHM_ABS_SW := 1, (*Absolute switch - Homing with absolute reference switch*)
		mcAHM_SW_GATE := 2, (*Switch gate - Homing with reference switch gate*)
		mcAHM_LIM_SW := 3, (*Limit switch - Homing with hardware end switch*)
		mcAHM_ABS := 4, (*Absolute - Homing by setting the home offset*)
		mcAHM_ABS_CORR := 5, (*Absolute correction - Homing by setting the home offset with counting range correction*)
		mcAHM_DIST_C_MARKS := 6, (*Distance coded marks - Homing with distance coded reference marks*)
		mcAHM_DIST_C_MARKS_CORR := 7, (*Distance coded marks correction - Homing with distance coded reference marks and counting range correction*)
		mcAHM_BLK_TORQ := 8, (*Block torque - Homing on block, criterion for homing event: torque*)
		mcAHM_BLK_LAG_ERR := 9, (*Block lag error - Homing on block, criterion for homing event: lag error*)
		mcAHM_RES_POS := 10, (*Restore position - Homing by restoring the position from remanent variable data*)
		mcAHM_NOT_USE := 100 (*Not used - No preconfigured homing settings used*)
		);
	McAHModDirRefPEnum :
		( (*Reference pulse selector setting*)
		mcAHMDRP_NOT_USE := 0, (*Not used - Reference pulse is not used*)
		mcAHMDRP_USE := 1 (*Used - Reference pulse is used*)
		);
	McAHModHomeDirEnum :
		( (*Movement direction in which the homing event is evaluated*)
		mcAHMHD_POS := 0, (*Positive - Positive movement direction*)
		mcAHMHD_NEG := 1 (*Negative - Negative movement direction*)
		);
	McAHModKeepDirEnum :
		( (*Keep direction (move only in one direction)*)
		mcAHMKD_NO := 0, (*No - mcSWITCH_OFF*)
		mcAHMKD_YES := 1 (*Yes - mcSWITCH_ON*)
		);
	McAHRPUBDUEnum :
		( (*Unit of reference pulse blocking distance*)
		mcAHRPUBDU_MEAS_UNIT := 0, (*Measurement units - Reference pulse blocking distance in measurement units*)
		mcAHRPUBDU_ENC_REV := 1 (*Encoder revolutions - Reference pulse blocking distance in encoder revolutions*)
		);
	McAHModDirRefPUseType : STRUCT (*Type mcAHMDRP_USE settings*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		HomingDirection : McAHModHomeDirEnum; (*Movement direction in which the homing event is evaluated*)
		KeepDirection : McAHModKeepDirEnum; (*Keep direction (move only in one direction)*)
		ReferencePulseBlockingDistance : LREAL; (*Distance for blocking the activation of triggering reference pulse*)
		BlockingDistanceUnit : McAHRPUBDUEnum; (*Unit of reference pulse blocking distance*)
	END_STRUCT;
	McAHModDirRefPType : STRUCT (*Use reference pulse of encoder*)
		Type : McAHModDirRefPEnum; (*Reference pulse selector setting*)
		Used : McAHModDirRefPUseType; (*Type mcAHMDRP_USE settings*)
	END_STRUCT;
	McAHModDirType : STRUCT (*Type mcAHM_DIR settings*)
		Position : LREAL; (*Home position [measurement units]*)
		ReferencePulse : McAHModDirRefPType; (*Use reference pulse of encoder*)
	END_STRUCT;
	McAHModSwEdgEnum :
		( (*Edge of reference switch*)
		mcAHMSE_POS := 0, (*Positive - Positive switch edge*)
		mcAHMSE_NEG := 1 (*Negative - Negative switch edge*)
		);
	McAHModRefPEnum :
		( (*Reference pulse selector setting*)
		mcAHMRP_NOT_USE := 0, (*Not used - Reference pulse is not used*)
		mcAHMRP_USE := 1 (*Used - Reference pulse is used*)
		);
	McAHModRefPUseType : STRUCT (*Type mcAHMRP_USE settings*)
		ReferencePulseBlockingDistance : LREAL; (*Distance for blocking the activation of triggering reference pulse*)
		BlockingDistanceUnit : McAHRPUBDUEnum; (*Unit of reference pulse blocking distance*)
	END_STRUCT;
	McAHModRefPType : STRUCT (*Use reference pulse of encoder*)
		Type : McAHModRefPEnum; (*Reference pulse selector setting*)
		Used : McAHModRefPUseType; (*Type mcAHMRP_USE settings*)
	END_STRUCT;
	McAHModAbsSwType : STRUCT (*Type mcAHM_ABS_SW settings*)
		Position : LREAL; (*Home position [measurement units]*)
		StartVelocity : REAL; (*Speed for searching the reference switch [measurement units/s]*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		SwitchEdge : McAHModSwEdgEnum; (*Edge of reference switch*)
		HomingDirection : McAHModHomeDirEnum; (*Movement direction in which the homing event is evaluated*)
		KeepDirection : McAHModKeepDirEnum; (*Keep direction (move only in one direction)*)
		ReferencePulse : McAHModRefPType; (*Use reference pulse of encoder*)
	END_STRUCT;
	McAHModStartDirEnum :
		( (*Start direction of movement for searching the reference switch*)
		mcAHMSD_POS := 0, (*Positive - Positive movement direction*)
		mcAHMSD_NEG := 1 (*Negative - Negative movement direction*)
		);
	McAHModSwGateType : STRUCT (*Type mcAHM_SW_GATE settings*)
		Position : LREAL; (*Home position [measurement units]*)
		StartVelocity : REAL; (*Speed for searching the reference switch [measurement units/s]*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		SwitchEdge : McAHModSwEdgEnum; (*Edge of reference switch*)
		StartDirection : McAHModStartDirEnum; (*Start direction of movement for searching the reference switch*)
		HomingDirection : McAHModHomeDirEnum; (*Movement direction in which the homing event is evaluated*)
		KeepDirection : McAHModKeepDirEnum; (*Keep direction (move only in one direction)*)
		ReferencePulse : McAHModRefPType; (*Use reference pulse of encoder*)
	END_STRUCT;
	McAHModLimSwType : STRUCT (*Type mcAHM_LIM_SW settings*)
		Position : LREAL; (*Home position [measurement units]*)
		StartVelocity : REAL; (*Speed for searching the reference switch [measurement units/s]*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		SwitchEdge : McAHModSwEdgEnum; (*Edge of reference switch*)
		HomingDirection : McAHModHomeDirEnum; (*Movement direction in which the homing event is evaluated*)
		KeepDirection : McAHModKeepDirEnum; (*Keep direction (move only in one direction)*)
		ReferencePulse : McAHModRefPType; (*Use reference pulse of encoder*)
	END_STRUCT;
	McAHModAbsType : STRUCT (*Type mcAHM_ABS settings*)
		Position : LREAL; (*Home offset [measurement units]*)
	END_STRUCT;
	McAHModAbsCorrType : STRUCT (*Type mcAHM_ABS_CORR settings*)
		Position : LREAL; (*Home offset [measurement units]*)
	END_STRUCT;
	McAHModDistCMarksType : STRUCT (*Type mcAHM_DIST_C_MARKS settings*)
		Position : LREAL; (*Home position [measurement units]*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		HomingDirection : McAHModHomeDirEnum; (*Movement direction in which the homing event is evaluated*)
		KeepDirection : McAHModKeepDirEnum; (*Keep direction (move only in one direction)*)
	END_STRUCT;
	McAHModDistCMarksCorrType : STRUCT (*Type mcAHM_DIST_C_MARKS_CORR settings*)
		Position : LREAL; (*Home position [measurement units]*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		HomingDirection : McAHModHomeDirEnum; (*Movement direction in which the homing event is evaluated*)
		KeepDirection : McAHModKeepDirEnum; (*Keep direction (move only in one direction)*)
	END_STRUCT;
	McAHModBlkRefPNotUseType : STRUCT (*Type mcAHMRP_NOT_USE settings*)
		MinimumReturnDistance : LREAL; (*Minimum return distance after the blockade is reached [measurement units]*)
	END_STRUCT;
	McAHModBlkRefPUseType : STRUCT (*Type mcAHMRP_USE settings*)
		ReferencePulseBlockingDistance : LREAL; (*Distance for blocking the activation of triggering reference pulse*)
		BlockingDistanceUnit : McAHRPUBDUEnum; (*Unit of reference pulse blocking distance*)
	END_STRUCT;
	McAHModBlkRefPType : STRUCT (*Use reference pulse of encoder*)
		Type : McAHModRefPEnum; (*Reference pulse selector setting*)
		NotUsed : McAHModBlkRefPNotUseType; (*Type mcAHMRP_NOT_USE settings*)
		Used : McAHModBlkRefPUseType; (*Type mcAHMRP_USE settings*)
	END_STRUCT;
	McAHModBlkTorqAddTorqLimEnum :
		( (*Additional torque limit selector setting*)
		mcAHMBTATL_NOT_USE := 0, (*Not used - Additional torque limit is not used*)
		mcAHMBTATL_POS_DIR := 1, (*Positive direction - Additional torque limit is active in positive movement direction*)
		mcAHMBTATL_NEG_DIR := 2 (*Negative direction - Additional torque limit is active in negative movement direction*)
		);
	McAHModBlkTqAddTqLimPosDirType : STRUCT (*Type mcAHMBTATL_POS_DIR settings*)
		TorqueLimit : REAL; (*Additional torque limit in positive movement direction for homing on block [Nm]*)
	END_STRUCT;
	McAHModBlkTqAddTqLimNegDirType : STRUCT (*Type mcAHMBTATL_NEG_DIR settings*)
		TorqueLimit : REAL; (*Additional torque limit in negative movement direction for homing on block [Nm]*)
	END_STRUCT;
	McAHModBlkTorqAddTorqLimType : STRUCT (*Activate an additional torque limit for a defined movement direction*)
		Type : McAHModBlkTorqAddTorqLimEnum; (*Additional torque limit selector setting*)
		PositiveDirection : McAHModBlkTqAddTqLimPosDirType; (*Type mcAHMBTATL_POS_DIR settings*)
		NegativeDirection : McAHModBlkTqAddTqLimNegDirType; (*Type mcAHMBTATL_NEG_DIR settings*)
	END_STRUCT;
	McAHModBlkTorqType : STRUCT (*Type mcAHM_BLK_TORQ settings*)
		Position : LREAL; (*Home position [measurement units]*)
		StartVelocity : REAL; (*Speed for searching the reference switch [measurement units/s]*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		StartDirection : McAHModStartDirEnum; (*Start direction of movement for searching the reference switch*)
		HomingDirection : McAHModHomeDirEnum; (*Movement direction in which the homing event is evaluated*)
		ReferencePulse : McAHModBlkRefPType; (*Use reference pulse of encoder*)
		TorqueLimit : REAL; (*Torque limit for homing on block [Nm]*)
		PositionErrorStopLimit : LREAL; (*Lag error for stop of the homing movement [measurement units]*)
		AdditionalTorqueLimit : McAHModBlkTorqAddTorqLimType; (*Activate an additional torque limit for a defined movement direction*)
	END_STRUCT;
	McAHModBlkLagErrAddTorqLimEnum :
		( (*Additional torque limit selector setting*)
		mcAHMBLEATL_NOT_USE := 0, (*Not used - Additional torque limit is not used*)
		mcAHMBLEATL_POS_DIR := 1, (*Positive direction - Additional torque limit is active in positive movement direction*)
		mcAHMBLEATL_NEG_DIR := 2 (*Negative direction - Additional torque limit is active in negative movement direction*)
		);
	McAHModBlkLErrAddTqLimPosDirType : STRUCT (*Type mcAHMBLEATL_POS_DIR settings*)
		TorqueLimit : REAL; (*Additional torque limit in positive movement direction for homing on block [Nm]*)
	END_STRUCT;
	McAHModBlkLErrAddTqLimNegDirType : STRUCT (*Type mcAHMBLEATL_NEG_DIR settings*)
		TorqueLimit : REAL; (*Additional torque limit in negative movement direction for homing on block [Nm]*)
	END_STRUCT;
	McAHModBlkLagErrAddTorqLimType : STRUCT (*Activate an additional torque limit for a defined movement direction*)
		Type : McAHModBlkLagErrAddTorqLimEnum; (*Additional torque limit selector setting*)
		PositiveDirection : McAHModBlkLErrAddTqLimPosDirType; (*Type mcAHMBLEATL_POS_DIR settings*)
		NegativeDirection : McAHModBlkLErrAddTqLimNegDirType; (*Type mcAHMBLEATL_NEG_DIR settings*)
	END_STRUCT;
	McAHModBlkLagErrType : STRUCT (*Type mcAHM_BLK_LAG_ERR settings*)
		Position : LREAL; (*Home position [measurement units]*)
		StartVelocity : REAL; (*Speed for searching the reference switch [measurement units/s]*)
		HomingVelocity : REAL; (*Speed which is used while searching for the homing event (e.g. after reference switch has been reached) [measurement units/s]*)
		Acceleration : REAL; (*Acceleration for homing movement [measurement units/s²]*)
		StartDirection : McAHModStartDirEnum; (*Start direction of movement for searching the reference switch*)
		HomingDirection : McAHModHomeDirEnum; (*Movement direction in which the homing event is evaluated*)
		ReferencePulse : McAHModBlkRefPType; (*Use reference pulse of encoder*)
		TorqueLimit : REAL; (*Torque limit for homing on block [Nm]*)
		PositionErrorStopLimit : LREAL; (*Lag error for stop of the homing movement [measurement units]*)
		BlockDetectionPositionError : LREAL; (*Lag error for block detection [measurement units]*)
		AdditionalTorqueLimit : McAHModBlkLagErrAddTorqLimType; (*Activate an additional torque limit for a defined movement direction*)
	END_STRUCT;
	McAHModType : STRUCT (*Homing mode*)
		Type : McAHModEnum; (*Mode selector setting*)
		Direct : McAHModDirType; (*Type mcAHM_DIR settings*)
		AbsoluteSwitch : McAHModAbsSwType; (*Type mcAHM_ABS_SW settings*)
		SwitchGate : McAHModSwGateType; (*Type mcAHM_SW_GATE settings*)
		LimitSwitch : McAHModLimSwType; (*Type mcAHM_LIM_SW settings*)
		Absolute : McAHModAbsType; (*Type mcAHM_ABS settings*)
		AbsoluteCorrection : McAHModAbsCorrType; (*Type mcAHM_ABS_CORR settings*)
		DistanceCodedMarks : McAHModDistCMarksType; (*Type mcAHM_DIST_C_MARKS settings*)
		DistanceCodedMarksCorrection : McAHModDistCMarksCorrType; (*Type mcAHM_DIST_C_MARKS_CORR settings*)
		BlockTorque : McAHModBlkTorqType; (*Type mcAHM_BLK_TORQ settings*)
		BlockLagError : McAHModBlkLagErrType; (*Type mcAHM_BLK_LAG_ERR settings*)
	END_STRUCT;
	McAHMRPAPCEnum :
		( (*Restore position axis scaling check selector setting*)
		mcAHMRPAPC_NOT_USE := 0, (*Not used - Axis parameterization check is not performed*)
		mcAHMRPAPC_USE := 1 (*Used - Axis parameterization check is performed*)
		);
	McAHMRPAPCType : STRUCT (*Activate check if axis parameterization has change for restore position*)
		Type : McAHMRPAPCEnum; (*Restore position axis scaling check selector setting*)
	END_STRUCT;
	McAHType : STRUCT (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
		Mode : McAHModType; (*Homing mode*)
		RestorePositionVariable : STRING[250]; (*Remanent variable used for homing mode: Restore position*)
		AxParCk : McAHMRPAPCType; (*Activate check if axis parameterization has change for restore position*)
	END_STRUCT;
	McASRQstopEnum :
		( (*Quickstop selector setting*)
		mcASRQ_DEC_LIM := 0, (*Deceleration limit - Stop with deceleration limits*)
		mcASRQ_DEC_LIM_W_JERK_FLTR := 1, (*Deceleration limit with jerk filter - Stop with deceleration limits and jerk filter*)
		mcASRQ_TORQ_LIM := 2, (*Torque limit - Stop with torque limits*)
		mcASRQ_TORQ_LIM_W_JERK_FLTR := 4, (*Torque limit with jerk filter - Stop with torque limits and jerk filter*)
		mcASRQ_VEL_CTRL := 5, (*Velocity control - Stop velocity controlled with axis limits*)
		mcASRQ_INDUCT_HALT := 3 (*Induction halt - Stop with an induction halt*)
		);
	McASRQstopDecLimWJerkFltrType : STRUCT (*Type mcASRQ_DEC_LIM_W_JERK_FLTR settings*)
		JerkTime : REAL; (*Jerk filter time, max. Axis Jerk time / 2 [s]*)
	END_STRUCT;
	McASRQstopTorqLimWJerkFltrType : STRUCT (*Type mcASRQ_TORQ_LIM_W_JERK_FLTR settings*)
		JerkTime : REAL; (*Jerk filter time, max. Axis Jerk time / 2 [s]*)
	END_STRUCT;
	McASRQstopType : STRUCT (*Deceleration ramp / reaction in case of a quickstop which is caused by an active quickstop input*)
		Type : McASRQstopEnum; (*Quickstop selector setting*)
		DecelerationLimitWithJerkFilter : McASRQstopDecLimWJerkFltrType; (*Type mcASRQ_DEC_LIM_W_JERK_FLTR settings*)
		TorqueLimitWithJerkFilter : McASRQstopTorqLimWJerkFltrType; (*Type mcASRQ_TORQ_LIM_W_JERK_FLTR settings*)
	END_STRUCT;
	McASRDrvErrEnum :
		( (*Drive error selector setting*)
		mcASRDE_DEC_LIM := 0, (*Deceleration limit - Stop with deceleration limits*)
		mcASRDE_TORQ_LIM := 4, (*Torque limit - Stop with torque limits*)
		mcASRDE_TORQ_LIM_W_JERK_FLTR := 5, (*Torque limit with jerk filter - Stop with torque limits and jerk filter*)
		mcASRDE_VEL_CTRL := 6, (*Velocity control - Stop velocity controlled with axis limits*)
		mcASRDE_INDUCT_HALT := 1, (*Induction halt - Stop with an induction halt*)
		mcASRDE_COAST_TO_STANDSTILL := 2, (*Coast to standstill - Controller is deactivated*)
		mcASRDE_CYC_DEC_FROM_AX_GRP := 3 (*Cyclic deceleration from axes group - The deceleration calculation is done by the axes group on the PLC and the value is forwarded to the axis*)
		);
	McASRDrvErrTorqLimWJerkFltrType : STRUCT (*Type mcASRDE_TORQ_LIM_W_JERK_FLTR settings*)
		JerkTime : REAL; (*Jerk filter time, max. Axis Jerk time / 2 [s]*)
	END_STRUCT;
	McASRDrvErrCycDecFromAxGrpType : STRUCT (*Type mcASRDE_CYC_DEC_FROM_AX_GRP settings*)
		DefaultDeceleration : REAL; (*Default deceleration value. If 0.0, the maximum allowed value is used [Measurement units/s²]*)
	END_STRUCT;
	McASRDrvErrType : STRUCT (*Deceleration ramp / Response in the event of ErrorStop caused by drive error*)
		Type : McASRDrvErrEnum; (*Drive error selector setting*)
		TorqueLimitWithJerkFilter : McASRDrvErrTorqLimWJerkFltrType; (*Type mcASRDE_TORQ_LIM_W_JERK_FLTR settings*)
		CyclicDecelerationFromAxesGroup : McASRDrvErrCycDecFromAxGrpType; (*Type mcASRDE_CYC_DEC_FROM_AX_GRP settings*)
	END_STRUCT;
	McASRType : STRUCT (*Reactions of the axis in case of certain stop conditions*)
		Quickstop : McASRQstopType; (*Deceleration ramp / reaction in case of a quickstop which is caused by an active quickstop input*)
		DriveError : McASRDrvErrType; (*Deceleration ramp / Response in the event of ErrorStop caused by drive error*)
		FilterTime : REAL; (*Filter time for stop reaction [s]*)
	END_STRUCT;
	McAMELVelErrMonEnum :
		( (*Velocity error monitoring selector setting*)
		mcAMELVEM_AUT_1 := 0, (*Automatic 1 - Initialize the stop limit automatically by using calculation method 1*)
		mcAMELVEM_AUT_2 := 1, (*Automatic 2 - Initialize the stop limit automatically by using calculation method 2*)
		mcAMELVEM_USRDEF := 2, (*User-defined - Initialize the stop limit with the value of the parameter velocity error*)
		mcAMELVEM_NOT_USE := 3 (*Not used - Velocity error monitoring is not active*)
		);
	McAMELVelErrMonUsrDefType : STRUCT (*Type mcAMELVEM_USRDEF settings*)
		VelocityError : REAL; (*Velocity error limit for stopping a movement [measurement units/s]*)
	END_STRUCT;
	McAMELVelErrMonType : STRUCT (*Velocity error monitoring mode*)
		Type : McAMELVelErrMonEnum; (*Velocity error monitoring selector setting*)
		UserDefined : McAMELVelErrMonUsrDefType; (*Type mcAMELVEM_USRDEF settings*)
	END_STRUCT;
	McAMELType : STRUCT (*Limit values that result in a stop reaction when exceeded*)
		PositionError : LREAL; (*Lag error limit for stopping a movement [measurement units]*)
		VelocityErrorMonitoring : McAMELVelErrMonType; (*Velocity error monitoring mode*)
	END_STRUCT;
	McAJFEnum :
		( (*Jerk filter selector setting*)
		mcAJF_NOT_USE := 0, (*Not used - No jerk filter is applied*)
		mcAJF_USE := 1, (*Used - Jerk filter is applied*)
		mcAJF_JERK_LIM := 2 (*Jerk limited - Jerk is considered in the profile generator*)
		);
	McAJFUseType : STRUCT (*Type mcAJF_USE settings*)
		JerkTime : REAL; (*Used jerk filter time ('Jerk time' <= 'Maximum jerk time') [s]*)
		MaximumJerkTime : REAL; (*Maximum configurable jerk filter time [s]*)
	END_STRUCT;
	McAJFJerkLimType : STRUCT (*Type mcAJF_JERK_LIM settings*)
		JerkLimit : REAL; (*Jerk limit in any movement direction [Measurement units/s³]*)
	END_STRUCT;
	McAJFType : STRUCT (*Jerk filter*)
		Type : McAJFEnum; (*Jerk filter selector setting*)
		Used : McAJFUseType; (*Type mcAJF_USE settings*)
		JerkLimited : McAJFJerkLimType; (*Type mcAJF_JERK_LIM settings*)
	END_STRUCT;
	McAZVFEnum :
		( (*Zero vibration filter selector setting*)
		mcAZVF_NOT_USE := 0, (*Not used - No zero vibration filter is applied*)
		mcAZVF_USE := 1 (*Used - Zero vibration filter is applied*)
		);
	McAZVFUseType : STRUCT (*Type mcAZVF_USE settings*)
		ZeroVibrationFilterCoefficient : REAL; (*Zero vibration filter coefficient*)
		ZeroVibrationFilterTime : REAL; (*Zero vibration filter time ('Zero vibration filter time' <= 'Maximum zero vibration filter time') [s]*)
		MaximumZeroVibrationFilterTime : REAL; (*Maximum configurable zero vibration filter time [s]*)
	END_STRUCT;
	McAZVFType : STRUCT (*Zero vibration filter*)
		Type : McAZVFEnum; (*Zero vibration filter selector setting*)
		Used : McAZVFUseType; (*Type mcAZVF_USE settings*)
	END_STRUCT;
	McADIAllSrcEnum :
		( (*Source selector setting*)
		mcADIAS_NOT_USE := 0, (*Not used -*)
		mcADIAS_DIG_IN_X8TRG_1 := 1, (*Digital in X8.Trigger 1 -*)
		mcADIAS_DIG_IN_X8TRG_2 := 2, (*Digital in X8.Trigger 2 -*)
		mcADIAS_DIG_IN_SS1X41X1 := 3, (*Digital in SS1.X41x.1 -*)
		mcADIAS_DIG_IN_SS1X41X2 := 4, (*Digital in SS1.X41x.2 -*)
		mcADIAS_DIG_IN_SS1X41X3 := 5, (*Digital in SS1.X41x.3 -*)
		mcADIAS_DIG_IN_SS1X41X4 := 6, (*Digital in SS1.X41x.4 -*)
		mcADIAS_DIG_IN_SS1X41X5 := 7, (*Digital in SS1.X41x.5 -*)
		mcADIAS_DIG_IN_SS1X41X6 := 8, (*Digital in SS1.X41x.6 -*)
		mcADIAS_DIG_IN_SS1X41X7 := 9, (*Digital in SS1.X41x.7 -*)
		mcADIAS_DIG_IN_SS1X41X8 := 10, (*Digital in SS1.X41x.8 -*)
		mcADIAS_DIG_IN_SS1X41X9 := 11, (*Digital in SS1.X41x.9 -*)
		mcADIAS_DIG_IN_SS1X41X10 := 12, (*Digital in SS1.X41x.10 -*)
		mcADIAS_DIG_IN_X23ATRG_1 := 13, (*Digital in X23A.Trigger 1 -*)
		mcADIAS_DIG_IN_X23ATRG_2 := 14, (*Digital in X23A.Trigger 2 -*)
		mcADIAS_DIG_IN_X24A_TRIGGER2 := 15, (*Digital in X24A Trigger2 -*)
		mcADIAS_DIG_IN_X2TRG_1 := 16, (*Digital in X2.Trigger 1 -*)
		mcADIAS_DIG_IN_X2TRG_2 := 17, (*Digital in X2.Trigger 2 -*)
		mcADIAS_DIG_IN_X1TRG_1 := 18, (*Digital in X1.Trigger 1 -*)
		mcADIAS_DIG_IN_X1TRG_2 := 19, (*Digital in X1.Trigger 2 -*)
		mcADIAS_DIG_IN_X1REF_SW := 20, (*Digital in X1.Reference switch -*)
		mcADIAS_DIG_IN_X1POS_HW_LIM := 21, (*Digital in X1.Positive HW limit -*)
		mcADIAS_DIG_IN_X1NEG_HW_LIM := 22, (*Digital in X1.Negative HW limit -*)
		mcADIAS_FOR_BY_FUN_BLK := 23, (*Force by function block -*)
		mcADIAS_VAR := 24, (*Variable -*)
		mcADIAS_IO_CH := 25 (*I/O channel - Get value from an I/O channel*)
		);
	McADIAllSrcVarTSEnum :
		( (*Time stamp selector setting*)
		mcADIASVTS_NOT_USE := 0, (*Not used - Not used*)
		mcADIASVTS_USE := 1, (*Used - Used*)
		mcADIASVTS_RIS_AND_FALL_EDG := 2 (*Rising and falling edge - Detect the trigger by using a rising and a falling time stamp*)
		);
	McADIAllSrcVarTSUseType : STRUCT (*Type mcADIASVTS_USE settings*)
		PVMapping : STRING[250]; (*Name of the process variable (DINT) representing trigger time stamp*)
	END_STRUCT;
	McADIASVTSRAFERECntType : STRUCT (*Count*)
		PVMapping : STRING[250]; (*Name of the process variable (SINT) representing the rising trigger edge count*)
	END_STRUCT;
	McADIASVTSRAFERETimStmpType : STRUCT (*Time stamp*)
		PVMapping : STRING[250]; (*Name of the process variable (INT) representing the rising trigger edge time*)
	END_STRUCT;
	McADIASVTSRAFERisEdgType : STRUCT (*Parameters for the rising trigger edge*)
		Count : McADIASVTSRAFERECntType; (*Count*)
		TimeStamp : McADIASVTSRAFERETimStmpType; (*Time stamp*)
	END_STRUCT;
	McADIASVTSRAFEFECntType : STRUCT (*Count*)
		PVMapping : STRING[250]; (*Name of the process variable (SINT) representing the falling trigger edge count*)
	END_STRUCT;
	McADIASVTSRAFEFETimStmpType : STRUCT (*Time stamp*)
		PVMapping : STRING[250]; (*Name of the process variable (INT) representing the falling trigger edge time*)
	END_STRUCT;
	McADIASVTSRAFEFallEdgType : STRUCT (*Parameters for the falling trigger edge*)
		Count : McADIASVTSRAFEFECntType; (*Count*)
		TimeStamp : McADIASVTSRAFEFETimStmpType; (*Time stamp*)
	END_STRUCT;
	McADIASVTSRisAndFallEdgType : STRUCT (*Type mcADIASVTS_RIS_AND_FALL_EDG settings*)
		RisingEdge : McADIASVTSRAFERisEdgType; (*Parameters for the rising trigger edge*)
		FallingEdge : McADIASVTSRAFEFallEdgType; (*Parameters for the falling trigger edge*)
	END_STRUCT;
	McADIAllSrcVarTSType : STRUCT (*Trigger time stamp*)
		Type : McADIAllSrcVarTSEnum; (*Time stamp selector setting*)
		Used : McADIAllSrcVarTSUseType; (*Type mcADIASVTS_USE settings*)
		RisingAndFallingEdge : McADIASVTSRisAndFallEdgType; (*Type mcADIASVTS_RIS_AND_FALL_EDG settings*)
	END_STRUCT;
	McADIAllSrcVarType : STRUCT (*Type mcADIAS_VAR settings*)
		PVMapping : STRING[250];
		TimeStamp : McADIAllSrcVarTSType; (*Trigger time stamp*)
	END_STRUCT;
	McADIAllSrcIOChTSEnum :
		( (*Time stamp selector setting*)
		mcADIASIOCTS_NOT_USE := 0, (*Not used - Not used*)
		mcADIASIOCTS_USE := 1, (*Used - Used*)
		mcADIASIOCTS_RIS_AND_FALL_EDG := 2 (*Rising and falling edge - Detect the trigger by using a rising and a falling time stamp*)
		);
	McADIAllSrcIOChTSUseType : STRUCT (*Type mcADIASIOCTS_USE settings*)
		ChannelMapping : STRING[250]; (*Input source (DINT) for representing the trigger time stamp*)
	END_STRUCT;
	McADIASIOCTSRAFERECntType : STRUCT (*Count*)
		ChannelMapping : STRING[250]; (*Input source (SINT) for representing the rising trigger edge count*)
	END_STRUCT;
	McADIASIOCTSRAFERETimStmpType : STRUCT (*Time stamp*)
		ChannelMapping : STRING[250]; (*Input source (INT) for representing the rising trigger edge time*)
	END_STRUCT;
	McADIASIOCTSRAFERisEdgType : STRUCT (*Parameters for the rising trigger edge*)
		Count : McADIASIOCTSRAFERECntType; (*Count*)
		TimeStamp : McADIASIOCTSRAFERETimStmpType; (*Time stamp*)
	END_STRUCT;
	McADIASIOCTSRAFEFECntType : STRUCT (*Count*)
		ChannelMapping : STRING[250]; (*Input source (SINT) for representing the falling trigger edge count*)
	END_STRUCT;
	McADIASIOCTSRAFEFETimStmpType : STRUCT (*Time stamp*)
		ChannelMapping : STRING[250]; (*Input source (INT) for representing the falling trigger edge time*)
	END_STRUCT;
	McADIASIOCTSRAFEFallEdgType : STRUCT (*Parameters for the falling trigger edge*)
		Count : McADIASIOCTSRAFEFECntType; (*Count*)
		TimeStamp : McADIASIOCTSRAFEFETimStmpType; (*Time stamp*)
	END_STRUCT;
	McADIASIOCTSRisAndFallEdgType : STRUCT (*Type mcADIASIOCTS_RIS_AND_FALL_EDG settings*)
		RisingEdge : McADIASIOCTSRAFERisEdgType; (*Parameters for the rising trigger edge*)
		FallingEdge : McADIASIOCTSRAFEFallEdgType; (*Parameters for the falling trigger edge*)
	END_STRUCT;
	McADIAllSrcIOChTSType : STRUCT (*Trigger time stamp*)
		Type : McADIAllSrcIOChTSEnum; (*Time stamp selector setting*)
		Used : McADIAllSrcIOChTSUseType; (*Type mcADIASIOCTS_USE settings*)
		RisingAndFallingEdge : McADIASIOCTSRisAndFallEdgType; (*Type mcADIASIOCTS_RIS_AND_FALL_EDG settings*)
	END_STRUCT;
	McADIAllSrcIOChType : STRUCT (*Type mcADIAS_IO_CH settings*)
		ChannelMapping : STRING[250]; (*Channel input source*)
		TimeStamp : McADIAllSrcIOChTSType; (*Trigger time stamp*)
	END_STRUCT;
	McADIHomeSwSrcType : STRUCT (*Source of the digital input hardware which is used for this functionality*)
		Type : McADIAllSrcEnum; (*Source selector setting*)
		Variable : McADIAllSrcVarType; (*Type mcADIAS_VAR settings*)
		IOChannel : McADIAllSrcIOChType; (*Type mcADIAS_IO_CH settings*)
	END_STRUCT;
	McADILvlEnum :
		( (*Level of the digital input hardware which leads to an active level of the functionality, not used with 'Force by function block'*)
		mcADIL_HIGH := 0, (*High*)
		mcADIL_LOW := 1 (*Low*)
		);
	McADIHomeSwType : STRUCT (*Homing switch input functionality*)
		Source : McADIHomeSwSrcType; (*Source of the digital input hardware which is used for this functionality*)
		Level : McADILvlEnum; (*Level of the digital input hardware which leads to an active level of the functionality, not used with 'Force by function block'*)
	END_STRUCT;
	McADIPosLimSwSrcType : STRUCT (*Source of the digital input hardware which is used for this functionality*)
		Type : McADIAllSrcEnum; (*Source selector setting*)
		Variable : McADIAllSrcVarType; (*Type mcADIAS_VAR settings*)
		IOChannel : McADIAllSrcIOChType; (*Type mcADIAS_IO_CH settings*)
	END_STRUCT;
	McADIPosLimSwType : STRUCT (*Positive limit switch input functionality*)
		Source : McADIPosLimSwSrcType; (*Source of the digital input hardware which is used for this functionality*)
		Level : McADILvlEnum; (*Level of the digital input hardware which leads to an active level of the functionality, not used with 'Force by function block'*)
	END_STRUCT;
	McADINegLimSwSrcType : STRUCT (*Source of the digital input hardware which is used for this functionality*)
		Type : McADIAllSrcEnum; (*Source selector setting*)
		Variable : McADIAllSrcVarType; (*Type mcADIAS_VAR settings*)
		IOChannel : McADIAllSrcIOChType; (*Type mcADIAS_IO_CH settings*)
	END_STRUCT;
	McADINegLimSwType : STRUCT (*Negative limit switch input functionality*)
		Source : McADINegLimSwSrcType; (*Source of the digital input hardware which is used for this functionality*)
		Level : McADILvlEnum; (*Level of the digital input hardware which leads to an active level of the functionality, not used with 'Force by function block'*)
	END_STRUCT;
	McADITrg1SrcType : STRUCT (*Source of the digital input hardware which is used for this functionality*)
		Type : McADIAllSrcEnum; (*Source selector setting*)
		Variable : McADIAllSrcVarType; (*Type mcADIAS_VAR settings*)
		IOChannel : McADIAllSrcIOChType; (*Type mcADIAS_IO_CH settings*)
	END_STRUCT;
	McADITrg1Type : STRUCT (*Trigger 1 input functionality*)
		Source : McADITrg1SrcType; (*Source of the digital input hardware which is used for this functionality*)
		Level : McADILvlEnum; (*Level of the digital input hardware which leads to an active level of the functionality, not used with 'Force by function block'*)
	END_STRUCT;
	McADITrg2SrcType : STRUCT (*Source of the digital input hardware which is used for this functionality*)
		Type : McADIAllSrcEnum; (*Source selector setting*)
		Variable : McADIAllSrcVarType; (*Type mcADIAS_VAR settings*)
		IOChannel : McADIAllSrcIOChType; (*Type mcADIAS_IO_CH settings*)
	END_STRUCT;
	McADITrg2Type : STRUCT (*Trigger 2 input functionality*)
		Source : McADITrg2SrcType; (*Source of the digital input hardware which is used for this functionality*)
		Level : McADILvlEnum; (*Level of the digital input hardware which leads to an active level of the functionality, not used with 'Force by function block'*)
	END_STRUCT;
	McADIQstopInEnum :
		( (*Digital input functionality triggering an axis quickstop*)
		mcADIQI_TRG_2 := 0, (*Trigger 2*)
		mcADIQI_TRG_1 := 1, (*Trigger 1*)
		mcADIQI_POS_LIM_SW := 2, (*Positive limit switch*)
		mcADIQI_NEG_LIM_SW := 3, (*Negative limit switch*)
		mcADIQI_HOME_SW := 4, (*Homing switch*)
		mcADIQI_NOT_USE := 5 (*Not used*)
		);
	McADIQstopType : STRUCT (*Quickstop functionality*)
		Input : McADIQstopInEnum; (*Digital input functionality triggering an axis quickstop*)
	END_STRUCT;
	McADIType : STRUCT (*Various digital input functionalities e.g. like homing switch or triggers*)
		HomingSwitch : McADIHomeSwType; (*Homing switch input functionality*)
		PositiveLimitSwitch : McADIPosLimSwType; (*Positive limit switch input functionality*)
		NegativeLimitSwitch : McADINegLimSwType; (*Negative limit switch input functionality*)
		Trigger1 : McADITrg1Type; (*Trigger 1 input functionality*)
		Trigger2 : McADITrg2Type; (*Trigger 2 input functionality*)
		Quickstop : McADIQstopType; (*Quickstop functionality*)
	END_STRUCT;
	McASPMEnum :
		( (*Simulation mode on PLC selector setting*)
		mcASPM_SET_VAL_GEN := 0, (*Set value generation - Only set value generation is active, no controlled system is simulated*)
		mcASPM_COMPL := 1 (*Complete - Complete simulation of a controlled system*)
		);
	McASLMEnum :
		( (*Load model selector setting*)
		mcASLM_ONE_MASS_MOT_BASED := 0, (*One mass motor based - 1-mass load model automatically calculated based on motor parameters*)
		mcASLM_ONE_MASS := 1, (*One mass - 1-mass load model, manual parameter setting necessary*)
		mcASLM_TWO_MASS := 2 (*Two masses - 2-mass load model, manual parameter setting necessary*)
		);
	McASLMOneMassMotBasedType : STRUCT (*Type mcASLM_ONE_MASS_MOT_BASED settings*)
		AdditiveLoadParID : UINT; (*Additive load ParID*)
	END_STRUCT;
	McASLMOneMassType : STRUCT (*Type mcASLM_ONE_MASS settings*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		StaticFriction : REAL; (*Static friction [Nm]*)
		ViscousFriction : REAL; (*Viscous Friction [Nms]*)
		AdditiveLoadParID : UINT; (*Additive load ParID*)
	END_STRUCT;
	McASLMTwoMassMass1Type : STRUCT (*Mass 1*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		StaticFriction : REAL; (*Static friction [Nm]*)
		ViscousFriction : REAL; (*Viscous Friction [Nms]*)
	END_STRUCT;
	McASLMTwoMassMass2Type : STRUCT (*Mass 2*)
		Inertia : REAL; (*Mass moment of inertia [kgm²]*)
		StaticFriction : REAL; (*Static friction [Nm]*)
		ViscousFriction : REAL; (*Viscous Friction [Nms]*)
		Stiffness : REAL; (*Stiffness of the coupling to mass 1 [Nm/rad]*)
		Damping : REAL; (*Damping of the coupling to mass 1 [Nms/rad]*)
		AdditiveLoadParID : UINT; (*Additive load ParID*)
	END_STRUCT;
	McASLMTwoMassType : STRUCT (*Type mcASLM_TWO_MASS settings*)
		Mass1 : McASLMTwoMassMass1Type; (*Mass 1*)
		Mass2 : McASLMTwoMassMass2Type; (*Mass 2*)
	END_STRUCT;
	McASLMType : STRUCT (*Parameters of the load simulation model*)
		Type : McASLMEnum; (*Load model selector setting*)
		OneMassMotorBased : McASLMOneMassMotBasedType; (*Type mcASLM_ONE_MASS_MOT_BASED settings*)
		OneMass : McASLMOneMassType; (*Type mcASLM_ONE_MASS settings*)
		TwoMasses : McASLMTwoMassType; (*Type mcASLM_TWO_MASS settings*)
	END_STRUCT;
	McASPMComplType : STRUCT (*Type mcASPM_COMPL settings*)
		LoadModel : McASLMType; (*Parameters of the load simulation model*)
	END_STRUCT;
	McASPMType : STRUCT (*Parameters for the simulation of this real axis on the PLC in case of 'Activate ACOPOS simulation on PLC = On' or ARsim is active*)
		Type : McASPMEnum; (*Simulation mode on PLC selector setting*)
		Complete : McASPMComplType; (*Type mcASPM_COMPL settings*)
	END_STRUCT;
	McASAMEnum :
		( (*Simulation mode on ACOPOS selector setting*)
		mcASAM_NOT_USE := 0, (*Not used - Load simulation is not switched on during the axis initialization*)
		mcASAM_SET_VAL_GEN := 1, (*Set value generation - Only set value generation is active, no controlled system is simulated*)
		mcASAM_COMPL := 2 (*Complete - Complete load simulation and automatic activation during the axis initialization*)
		);
	McASAMComplType : STRUCT (*Type mcASAM_COMPL settings*)
		LoadModel : McASLMType; (*Parameters of the load simulation model*)
	END_STRUCT;
	McASAMType : STRUCT (*Parameters for the motor and load simulation on the drive*)
		Type : McASAMEnum; (*Simulation mode on ACOPOS selector setting*)
		Complete : McASAMComplType; (*Type mcASAM_COMPL settings*)
	END_STRUCT;
	McASType : STRUCT (*Parameters which influence the simulation possibilities of this axis*)
		ModeOnPLC : McASPMType; (*Parameters for the simulation of this real axis on the PLC in case of 'Activate ACOPOS simulation on PLC = On' or ARsim is active*)
		ModeOnACOPOS : McASAMType; (*Parameters for the motor and load simulation on the drive*)
	END_STRUCT;
	McAAFType : STRUCT (*Features for an axis*)
		FeatureReference : McCfgUnboundedArrayType; (*Name of the axis feature reference (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McCfgAcpAxType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_AX*)
		AxisReference : McCfgReferenceType; (*Name of the referenced axis component*)
		MechanicalElements : McAMEType; (*Parameter of hardware elements situated between motor encoder and load which influence the scaling*)
		EncoderLink : McAELType;
		Controller : McACType; (*Axis controller parameters*)
		Homing : McAHType; (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
		StopReaction : McASRType; (*Reactions of the axis in case of certain stop conditions*)
		MovementErrorLimits : McAMELType; (*Limit values that result in a stop reaction when exceeded*)
		JerkFilter : McAJFType; (*Jerk filter*)
		ZeroVibrationFilter : McAZVFType; (*Zero vibration filter*)
		DigitalInputs : McADIType; (*Various digital input functionalities e.g. like homing switch or triggers*)
		Simulation : McASType; (*Parameters which influence the simulation possibilities of this axis*)
		AxisFeatures : McAAFType; (*Features for an axis*)
	END_STRUCT;
	McCfgAcpAxRefType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_AX_REF*)
		AxisReference : McCfgReferenceType; (*Name of the referenced axis component*)
	END_STRUCT;
	McCfgAcpMechElmType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_MECH_ELM*)
		MechanicalElements : McAMEType; (*Parameter of hardware elements situated between motor encoder and load which influence the scaling*)
	END_STRUCT;
	McCfgAcpEncLinkType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_ENC_LINK*)
		EncoderLink : McAELType;
	END_STRUCT;
	McCfgAcpCtrlType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_CTRL*)
		Controller : McACType; (*Axis controller parameters*)
	END_STRUCT;
	McCfgAcpSpdCtrlType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_SPD_CTRL*)
		Speed : McACSCType; (*Speed controller parameters*)
	END_STRUCT;
	McCfgAcpHomeType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_HOME*)
		Homing : McAHType; (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
	END_STRUCT;
	McCfgAcpStopReacType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_STOP_REAC*)
		StopReaction : McASRType; (*Reactions of the axis in case of certain stop conditions*)
	END_STRUCT;
	McCfgAcpMoveErrLimType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_MOVE_ERR_LIM*)
		MovementErrorLimits : McAMELType; (*Limit values that result in a stop reaction when exceeded*)
	END_STRUCT;
	McCfgAcpJerkFltrType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_JERK_FLTR*)
		JerkFilter : McAJFType; (*Jerk filter*)
	END_STRUCT;
	McCfgAcpDigInType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_DIG_IN*)
		DigitalInputs : McADIType; (*Various digital input functionalities e.g. like homing switch or triggers*)
	END_STRUCT;
	McCfgAcpSimType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_SIM*)
		Simulation : McASType; (*Parameters which influence the simulation possibilities of this axis*)
	END_STRUCT;
	McCfgAcpAxFeatType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_AX_FEAT*)
		AxisFeatures : McAAFType; (*Features for an axis*)
	END_STRUCT;
	McCfgAcpZeroVibFltrType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_ZERO_VIB_FLTR*)
		ZeroVibrationFilter : McAZVFType; (*Zero vibration filter*)
	END_STRUCT;
	McAPSMOutParEnum :
		( (*Output parameters selector setting*)
		mcAPSMOP_ACPMOT_CMPCT := 0, (*ACOPOSmotor compact - Output parameters for ACOPOSmotor compact modules*)
		mcAPSMOP_ACOPOSTRAK := 1, (*ACOPOStrak - Output parameters for ACOPOStrak segments*)
		mcAPSMOP_USR_DEF := 2 (*User defined - User defined output parameters*)
		);
	McAPSMOutParAcpMotCmpctType : STRUCT (*Type mcAPSMOP_ACPMOT_CMPCT settings*)
		Voltage : REAL; (*Output voltage [V]*)
		CurrentLimit : REAL; (*Output current limit [A]*)
		CurrentLimitTime : USINT; (*Output current limit time [s]*)
	END_STRUCT;
	McAPSMOutParAcpTrkSteVRmpEnum :
		( (*Steep voltage ramp selector setting*)
		mcAPSMOPATSVR_NOT_USE := 0, (*Not used - Normal output voltage ramp*)
		mcAPSMOPATSVR_USE := 1 (*Used - Steep output voltage ramp*)
		);
	McAPSMOutParAcpTrkSteVRmpType : STRUCT (*Activate steep output voltage ramp*)
		Type : McAPSMOutParAcpTrkSteVRmpEnum; (*Steep voltage ramp selector setting*)
	END_STRUCT;
	McAPSMOutParACOPOStrakType : STRUCT (*Type mcAPSMOP_ACOPOSTRAK settings*)
		Voltage : REAL; (*Output voltage [V]*)
		CurrentLimit : REAL; (*Output current limit [A]*)
		CurrentLimitTime : USINT; (*Output current limit time [s]*)
		SteepVoltageRamp : McAPSMOutParAcpTrkSteVRmpType; (*Activate steep output voltage ramp*)
	END_STRUCT;
	McAPSMOutParUsrDefCurLimEnum :
		( (*Current limitation selector setting*)
		mcAPSMOPUDCL_NOT_USE := 0, (*Not used - Current limited automatically to maximum*)
		mcAPSMOPUDCL_USE := 1 (*Used - Current limited to specified value*)
		);
	McAPSMOutParUsrDefCurLimUseType : STRUCT (*Type mcAPSMOPUDCL_USE settings*)
		CurrentLimit : REAL; (*Output current limit [A]*)
		CurrentLimitTime : USINT; (*Output current limit time [s]*)
	END_STRUCT;
	McAPSMOutParUsrDefCurLimType : STRUCT (*Current limitation*)
		Type : McAPSMOutParUsrDefCurLimEnum; (*Current limitation selector setting*)
		Used : McAPSMOutParUsrDefCurLimUseType; (*Type mcAPSMOPUDCL_USE settings*)
	END_STRUCT;
	McAPSMOutParUsrDefType : STRUCT (*Type mcAPSMOP_USR_DEF settings*)
		Voltage : REAL; (*Output voltage [V]*)
		CurrentLimitation : McAPSMOutParUsrDefCurLimType; (*Current limitation*)
	END_STRUCT;
	McAPSMOutParType : STRUCT (*Output parameters selection*)
		Type : McAPSMOutParEnum; (*Output parameters selector setting*)
		ACOPOSmotorCompact : McAPSMOutParAcpMotCmpctType; (*Type mcAPSMOP_ACPMOT_CMPCT settings*)
		ACOPOStrak : McAPSMOutParACOPOStrakType; (*Type mcAPSMOP_ACOPOSTRAK settings*)
		UserDefined : McAPSMOutParUsrDefType; (*Type mcAPSMOP_USR_DEF settings*)
	END_STRUCT;
	McAPSMOutVOnEnum :
		( (*Output voltage on selector setting*)
		mcAPSMOVO_NO := 0, (*No - Output voltage off, switch on by FB call*)
		mcAPSMOVO_YES := 1 (*Yes - Output voltage on*)
		);
	McAPSMOutVOnType : STRUCT (*Output voltage is on after start-up and error reset*)
		Type : McAPSMOutVOnEnum; (*Output voltage on selector setting*)
	END_STRUCT;
	McCfgAcpAuxPwrSupModType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_AUX_PWR_SUP_MOD*)
		AxisReference : STRING[250]; (*Name of the power supply module component*)
		OutputParameters : McAPSMOutParType; (*Output parameters selection*)
		OutputVoltageOn : McAPSMOutVOnType; (*Output voltage is on after start-up and error reset*)
	END_STRUCT;
	McAPSMPSPwrSecType : STRUCT
		AxisReference : STRING[250]; (*Name of the power supply module component*)
	END_STRUCT;
	McCfgAcpPSMPwrSecType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_PSM_PWR_SEC*)
		PowerSection : McAPSMPSPwrSecType;
	END_STRUCT;
	McAPSPwrSupEnum :
		( (*Power supply selector setting*)
		mcAPSPS_AC := 0, (*AC -*)
		mcAPSPS_DC_PWR_SUP_MOD_REF := 1, (*DC power supply module reference -*)
		mcAPSPS_DC_BUS_V := 2, (*DC bus voltage -*)
		mcAPSPS_ETA_SYS_FOR_TR_ONLY := 3 (*ETA system (for training only) - This mode can be used for training purpose, when using the drive within an ETA system with 24 VDC supply voltage*)
		);
	McAPSPwrSupACSngPhOpEnum :
		( (*Single phase operation selector setting*)
		mcAPSPSASPO_NOT_USE := 0, (*Not used -*)
		mcAPSPSASPO_USE := 1 (*Used -*)
		);
	McAPSPwrSupACSngPhOpUseType : STRUCT (*Type mcAPSPSASPO_USE settings*)
		SupplyVoltage : REAL; (*Supply voltage for the single phase operation [V]*)
	END_STRUCT;
	McAPSPwrSupACSngPhOpType : STRUCT
		Type : McAPSPwrSupACSngPhOpEnum; (*Single phase operation selector setting*)
		Used : McAPSPwrSupACSngPhOpUseType; (*Type mcAPSPSASPO_USE settings*)
	END_STRUCT;
	McAPSPwrSupACType : STRUCT (*Type mcAPSPS_AC settings*)
		SinglePhaseOperation : McAPSPwrSupACSngPhOpType;
	END_STRUCT;
	McAPSPwrSupDCPwrSupModRefType : STRUCT (*Type mcAPSPS_DC_PWR_SUP_MOD_REF settings*)
		PowerSupplyModuleReference : STRING[250]; (*DC bus voltage is read from the referenced power supply module*)
	END_STRUCT;
	McAPSPwrSupDCBusVType : STRUCT (*Type mcAPSPS_DC_BUS_V settings*)
		BusVoltage : UINT; (*Bus voltage [V]*)
	END_STRUCT;
	McAPSPwrSupType : STRUCT (*Selects the power supply or DC bus voltage*)
		Type : McAPSPwrSupEnum; (*Power supply selector setting*)
		AC : McAPSPwrSupACType; (*Type mcAPSPS_AC settings*)
		DCPowerSupplyModuleReference : McAPSPwrSupDCPwrSupModRefType; (*Type mcAPSPS_DC_PWR_SUP_MOD_REF settings*)
		DCBusVoltage : McAPSPwrSupDCBusVType; (*Type mcAPSPS_DC_BUS_V settings*)
	END_STRUCT;
	McCfgAcpPwrSupType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_PWR_SUP*)
		PowerSupply : McAPSPwrSupType; (*Selects the power supply or DC bus voltage*)
	END_STRUCT;
	McAMActAcpSimOnPLCEnum :
		( (*Activates or deactivates the ACOPOS simulation on the PLC*)
		mcAMAASOP_OFF := 0, (*Off - The drive is not simulated on the PLC*)
		mcAMAASOP_ON := 1 (*On - The drive is simulated on the PLC*)
		);
	McAMPwrSupEnum :
		( (*Power supply selector setting*)
		mcAMPS_AC := 0, (*AC -*)
		mcAMPS_DC_PWR_SUP_MOD_REF := 1, (*DC power supply module reference -*)
		mcAMPS_DC_BUS_V := 2, (*DC bus voltage -*)
		mcAMPS_ETA_SYS_FOR_TR_ONLY := 3, (*ETA system (for training only) - This mode can be used for training purpose, when using the drive within an ETA system with 24 VDC supply voltage*)
		mcAMPS_AUT_DETECT := 4 (*Automatic detection -*)
		);
	McAMPwrSupACSngPhOpEnum :
		( (*Single phase operation selector setting*)
		mcAMPSASPO_NOT_USE := 0, (*Not used -*)
		mcAMPSASPO_USE := 1 (*Used -*)
		);
	McAMPwrSupACSngPhOpUseType : STRUCT (*Type mcAMPSASPO_USE settings*)
		SupplyVoltage : REAL; (*Supply voltage for the single phase operation [V]*)
	END_STRUCT;
	McAMPwrSupACSngPhOpType : STRUCT
		Type : McAMPwrSupACSngPhOpEnum; (*Single phase operation selector setting*)
		Used : McAMPwrSupACSngPhOpUseType; (*Type mcAMPSASPO_USE settings*)
	END_STRUCT;
	McAMPwrSupACType : STRUCT (*Type mcAMPS_AC settings*)
		SinglePhaseOperation : McAMPwrSupACSngPhOpType;
	END_STRUCT;
	McAMPwrSupDCPwrSupModRefType : STRUCT (*Type mcAMPS_DC_PWR_SUP_MOD_REF settings*)
		PowerSupplyModuleReference : STRING[250]; (*DC bus voltage is read from the referenced power supply module*)
	END_STRUCT;
	McAMPwrSupDCBusVType : STRUCT (*Type mcAMPS_DC_BUS_V settings*)
		BusVoltage : UINT; (*Bus voltage [V]*)
	END_STRUCT;
	McAMPwrSupType : STRUCT (*Selects the power supply or DC bus voltage*)
		Type : McAMPwrSupEnum; (*Power supply selector setting*)
		AC : McAMPwrSupACType; (*Type mcAMPS_AC settings*)
		DCPowerSupplyModuleReference : McAMPwrSupDCPwrSupModRefType; (*Type mcAMPS_DC_PWR_SUP_MOD_REF settings*)
		DCBusVoltage : McAMPwrSupDCBusVType; (*Type mcAMPS_DC_BUS_V settings*)
	END_STRUCT;
	McAMSWRstAFwUpdEnum :
		( (*If 'On' is selected, an ACOPOS reset is automatically carried out after the firmware update by sending ParID CMD_SW_RESET*)
		mcAMSRAFU_ON := 0, (*On - Perform automatic ACOPOS reset after firmware download*)
		mcAMSRAFU_OFF := 1 (*Off - ACOPOS reset is not performed automatically after firmware download*)
		);
	McCfgAcpModType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_MOD*)
		ActivateACOPOSSimulationOnPLC : McAMActAcpSimOnPLCEnum; (*Activates or deactivates the ACOPOS simulation on the PLC*)
		BusVoltage : UINT; (*Bus voltage [V]*)
		PowerSupply : McAMPwrSupType; (*Selects the power supply or DC bus voltage*)
		SWResetAfterFirmwareUpdate : McAMSWRstAFwUpdEnum; (*If 'On' is selected, an ACOPOS reset is automatically carried out after the firmware update by sending ParID CMD_SW_RESET*)
	END_STRUCT;
	McAEEncX6AIfTypEnum :
		( (*Interface type selector setting*)
		mcAEX6AIT_ENDAT := 0, (*EnDat -*)
		mcAEX6AIT_SSI := 1, (*SSI -*)
		mcAEX6AIT_SIN := 2, (*Sine -*)
		mcAEX6AIT_INCR := 3, (*Incremental -*)
		mcAEX6AIT_LINMOT := 4, (*LinMot -*)
		mcAEX6AIT_RES := 5 (*Resolver -*)
		);
	McAEX6ASSSIFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX6ASSFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX6ASSFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX6ASSFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX6ASSFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX6ASSFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX6ASSSIFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX6ASSFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX6ASSFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX6ASSFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX6ASSFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX6ASSFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX6ASSSIFrmCfgSSIDatCEnum :
		( (*SSI data code*)
		mcAEX6ASSFCSDC_GRAY := 0, (*Gray - Gray*)
		mcAEX6ASSFCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAEX6ASSSIFrmCfgSSIParCkEnum :
		( (*SSI parity check*)
		mcAEX6ASSFCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAEX6ASSFCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAEX6ASSFCSPC_OFF := 2 (*Off - Off*)
		);
	McAEX6ASSSIFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX6ASSSIFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX6ASSSIFrmCfgStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAEX6ASSSIFrmCfgSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAEX6ASSSIFrmCfgSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAEX6ASType : STRUCT (*Type mcAEX6AIT_SSI settings*)
		SSIFrameConfiguration : McAEX6ASSSIFrmCfgType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEX6ASinType : STRUCT (*Type mcAEX6AIT_SIN settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
	END_STRUCT;
	McAEX6AIMaxExpectedOutFreqEnum :
		( (*Maximal expected output frequency [Hz]*)
		mcAEX6AIMEOF_MEOF_25000 := 0, (*MEOF 25000 - 25000 Hz*)
		mcAEX6AIMEOF_MEOF_50000 := 1, (*MEOF 50000 - 50000 Hz*)
		mcAEX6AIMEOF_MEOF_100000 := 2, (*MEOF 100000 - 100000 Hz*)
		mcAEX6AIMEOF_MEOF_200000 := 3 (*MEOF 200000 - 200000 Hz*)
		);
	McAEX6AIType : STRUCT (*Type mcAEX6AIT_INCR settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		MaxExpectedOutputFrequency : McAEX6AIMaxExpectedOutFreqEnum; (*Maximal expected output frequency [Hz]*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAEX6ALinMotType : STRUCT (*Type mcAEX6AIT_LINMOT settings*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
	END_STRUCT;
	McAEX6AResType : STRUCT (*Type mcAEX6AIT_RES settings*)
		PolepairsPerEncoderRevolution : USINT; (*Resolver polepairs per encoder revolution*)
	END_STRUCT;
	McAEEncX6AIfTypType : STRUCT
		Type : McAEEncX6AIfTypEnum; (*Interface type selector setting*)
		SSI : McAEX6ASType; (*Type mcAEX6AIT_SSI settings*)
		Sine : McAEX6ASinType; (*Type mcAEX6AIT_SIN settings*)
		Incremental : McAEX6AIType; (*Type mcAEX6AIT_INCR settings*)
		LinMot : McAEX6ALinMotType; (*Type mcAEX6AIT_LINMOT settings*)
		Resolver : McAEX6AResType; (*Type mcAEX6AIT_RES settings*)
	END_STRUCT;
	McAEEncX6AType : STRUCT
		InterfaceType : McAEEncX6AIfTypType;
	END_STRUCT;
	McAEEncX6BIfTypEnum :
		( (*Interface type selector setting*)
		mcAEX6BIT_ENDAT := 0, (*EnDat -*)
		mcAEX6BIT_SSI := 1, (*SSI -*)
		mcAEX6BIT_SIN := 2, (*Sine -*)
		mcAEX6BIT_INCR := 3, (*Incremental -*)
		mcAEX6BIT_LINMOT := 4, (*LinMot -*)
		mcAEX6BIT_RES := 5 (*Resolver -*)
		);
	McAEX6BSSSIFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX6BSSFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX6BSSFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX6BSSFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX6BSSFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX6BSSFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX6BSSSIFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX6BSSFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX6BSSFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX6BSSFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX6BSSFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX6BSSFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX6BSSSIFrmCfgSSIDatCEnum :
		( (*SSI data code*)
		mcAEX6BSSFCSDC_GRAY := 0, (*Gray - Gray*)
		mcAEX6BSSFCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAEX6BSSSIFrmCfgSSIParCkEnum :
		( (*SSI parity check*)
		mcAEX6BSSFCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAEX6BSSFCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAEX6BSSFCSPC_OFF := 2 (*Off - Off*)
		);
	McAEX6BSSSIFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX6BSSSIFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX6BSSSIFrmCfgStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAEX6BSSSIFrmCfgSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAEX6BSSSIFrmCfgSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAEX6BSType : STRUCT (*Type mcAEX6BIT_SSI settings*)
		SSIFrameConfiguration : McAEX6BSSSIFrmCfgType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEX6BSinType : STRUCT (*Type mcAEX6BIT_SIN settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
	END_STRUCT;
	McAEX6BIMaxExpectedOutFreqEnum :
		( (*Maximal expected output frequency [Hz]*)
		mcAEX6BIMEOF_MEOF_25000 := 0, (*MEOF 25000 - 25000 Hz*)
		mcAEX6BIMEOF_MEOF_50000 := 1, (*MEOF 50000 - 50000 Hz*)
		mcAEX6BIMEOF_MEOF_100000 := 2, (*MEOF 100000 - 100000 Hz*)
		mcAEX6BIMEOF_MEOF_200000 := 3 (*MEOF 200000 - 200000 Hz*)
		);
	McAEX6BIType : STRUCT (*Type mcAEX6BIT_INCR settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		MaxExpectedOutputFrequency : McAEX6BIMaxExpectedOutFreqEnum; (*Maximal expected output frequency [Hz]*)
		IgnoreCheck : UINT; (*Ignore check*)
	END_STRUCT;
	McAEX6BLinMotType : STRUCT (*Type mcAEX6BIT_LINMOT settings*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
	END_STRUCT;
	McAEX6BResType : STRUCT (*Type mcAEX6BIT_RES settings*)
		PolepairsPerEncoderRevolution : USINT; (*Resolver polepairs per encoder revolution*)
	END_STRUCT;
	McAEEncX6BIfTypType : STRUCT
		Type : McAEEncX6BIfTypEnum; (*Interface type selector setting*)
		SSI : McAEX6BSType; (*Type mcAEX6BIT_SSI settings*)
		Sine : McAEX6BSinType; (*Type mcAEX6BIT_SIN settings*)
		Incremental : McAEX6BIType; (*Type mcAEX6BIT_INCR settings*)
		LinMot : McAEX6BLinMotType; (*Type mcAEX6BIT_LINMOT settings*)
		Resolver : McAEX6BResType; (*Type mcAEX6BIT_RES settings*)
	END_STRUCT;
	McAEEncX6BType : STRUCT
		InterfaceType : McAEEncX6BIfTypType;
	END_STRUCT;
	McAEEncX41IfTypEnum :
		( (*Interface type selector setting*)
		mcAEX41IT_NOT_USE := 0, (*Not used -*)
		mcAEX41IT_BISS := 1, (*BiSS -*)
		mcAEX41IT_SSI := 2, (*SSI -*)
		mcAEX41IT_ENDAT := 3, (*EnDat -*)
		mcAEX41IT_HIPERFACE_DSL := 4, (*HIPERFACE DSL -*)
		mcAEX41IT_TFMT := 5, (*T-Format - Tamagawa digital interface*)
		mcAEX41IT_MOT_DAT_IF := 6, (*Motion Data Interface - B&R bi-directional asynchronous serial interface*)
		mcAEX41IT_ENDAT_SAFEMOTION := 7, (*EnDat SafeMOTION -*)
		mcAEX41IT_ENDAT_3 := 8 (*EnDat 3 -*)
		);
	McAEX41BPwrSupEnum :
		( (*Power supply of the encoder*)
		mcAEX41BPS_EXT := 0, (*External - External (0V)*)
		mcAEX41BPS_PS_5_V := 1, (*PS 5 V - 5 Volt*)
		mcAEX41BPS_PS_12_V := 2 (*PS 12 V - 12 Volt*)
		);
	McAEX41BBiSSFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX41BBFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX41BBFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX41BBFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX41BBFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX41BBFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX41BBiSSFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX41BBFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX41BBFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX41BBFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX41BBFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX41BBFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX41BBiSSFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX41BBiSSFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX41BBiSSFrmCfgStatBit2Enum; (*Warning or error bit*)
	END_STRUCT;
	McAEX41BType : STRUCT (*Type mcAEX41IT_BISS settings*)
		PowerSupply : McAEX41BPwrSupEnum; (*Power supply of the encoder*)
		BiSSFrameConfiguration : McAEX41BBiSSFrmCfgType; (*Frame structure in the order of transfer*)
		CRCPolynomial : UDINT; (*Data verification via CRC (decimal value, 0 deactivates verification)*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEX41SPwrSupEnum :
		( (*Power supply of the encoder*)
		mcAEX41SPS_EXT := 0, (*External - External (0V)*)
		mcAEX41SPS_PS_5_V := 1, (*PS 5 V - 5 Volt*)
		mcAEX41SPS_PS_12_V := 2 (*PS 12 V - 12 Volt*)
		);
	McAEX41SSSIFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX41SSFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX41SSFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX41SSFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX41SSFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX41SSFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX41SSSIFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX41SSFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX41SSFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX41SSFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX41SSFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX41SSFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX41SSSIFrmCfgSSIDatCEnum :
		( (*SSI data code*)
		mcAEX41SSFCSDC_GRAY := 0, (*Gray - Gray*)
		mcAEX41SSFCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAEX41SSSIFrmCfgSSIParCkEnum :
		( (*SSI parity check*)
		mcAEX41SSFCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAEX41SSFCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAEX41SSFCSPC_OFF := 2 (*Off - Off*)
		);
	McAEX41SSSIFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX41SSSIFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX41SSSIFrmCfgStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAEX41SSSIFrmCfgSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAEX41SSSIFrmCfgSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAEX41SType : STRUCT (*Type mcAEX41IT_SSI settings*)
		PowerSupply : McAEX41SPwrSupEnum; (*Power supply of the encoder*)
		SSIFrameConfiguration : McAEX41SSSIFrmCfgType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEEncX41IfTypType : STRUCT
		Type : McAEEncX41IfTypEnum; (*Interface type selector setting*)
		BiSS : McAEX41BType; (*Type mcAEX41IT_BISS settings*)
		SSI : McAEX41SType; (*Type mcAEX41IT_SSI settings*)
	END_STRUCT;
	McAEEncX41Type : STRUCT
		InterfaceType : McAEEncX41IfTypType;
	END_STRUCT;
	McAEEncX42IfTypEnum :
		( (*Interface type selector setting*)
		mcAEX42IT_NOT_USE := 0, (*Not used -*)
		mcAEX42IT_BISS := 1, (*BiSS -*)
		mcAEX42IT_SSI := 2, (*SSI -*)
		mcAEX42IT_ENDAT := 3, (*EnDat -*)
		mcAEX42IT_HIPERFACE_DSL := 4, (*HIPERFACE DSL -*)
		mcAEX42IT_TFMT := 5, (*T-Format - Tamagawa digital interface*)
		mcAEX42IT_MOT_DAT_IF := 6, (*Motion Data Interface - B&R bi-directional asynchronous serial interface*)
		mcAEX42IT_ENDAT_SAFEMOTION := 7, (*EnDat SafeMOTION -*)
		mcAEX42IT_ENDAT_3 := 8 (*EnDat 3 -*)
		);
	McAEX42BPwrSupEnum :
		( (*Power supply of the encoder*)
		mcAEX42BPS_EXT := 0, (*External - External (0V)*)
		mcAEX42BPS_PS_5_V := 1, (*PS 5 V - 5 Volt*)
		mcAEX42BPS_PS_12_V := 2 (*PS 12 V - 12 Volt*)
		);
	McAEX42BBiSSFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX42BBFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX42BBFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX42BBFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX42BBFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX42BBFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX42BBiSSFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX42BBFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX42BBFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX42BBFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX42BBFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX42BBFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX42BBiSSFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX42BBiSSFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX42BBiSSFrmCfgStatBit2Enum; (*Warning or error bit*)
	END_STRUCT;
	McAEX42BType : STRUCT (*Type mcAEX42IT_BISS settings*)
		PowerSupply : McAEX42BPwrSupEnum; (*Power supply of the encoder*)
		BiSSFrameConfiguration : McAEX42BBiSSFrmCfgType; (*Frame structure in the order of transfer*)
		CRCPolynomial : UDINT; (*Data verification via CRC (decimal value, 0 deactivates verification)*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEX42SPwrSupEnum :
		( (*Power supply of the encoder*)
		mcAEX42SPS_EXT := 0, (*External - External (0V)*)
		mcAEX42SPS_PS_5_V := 1, (*PS 5 V - 5 Volt*)
		mcAEX42SPS_PS_12_V := 2 (*PS 12 V - 12 Volt*)
		);
	McAEX42SSSIFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX42SSFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX42SSFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX42SSFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX42SSFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX42SSFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX42SSSIFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX42SSFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX42SSFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX42SSFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX42SSFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX42SSFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX42SSSIFrmCfgSSIDatCEnum :
		( (*SSI data code*)
		mcAEX42SSFCSDC_GRAY := 0, (*Gray - Gray*)
		mcAEX42SSFCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAEX42SSSIFrmCfgSSIParCkEnum :
		( (*SSI parity check*)
		mcAEX42SSFCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAEX42SSFCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAEX42SSFCSPC_OFF := 2 (*Off - Off*)
		);
	McAEX42SSSIFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX42SSSIFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX42SSSIFrmCfgStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAEX42SSSIFrmCfgSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAEX42SSSIFrmCfgSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAEX42SType : STRUCT (*Type mcAEX42IT_SSI settings*)
		PowerSupply : McAEX42SPwrSupEnum; (*Power supply of the encoder*)
		SSIFrameConfiguration : McAEX42SSSIFrmCfgType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEEncX42IfTypType : STRUCT
		Type : McAEEncX42IfTypEnum; (*Interface type selector setting*)
		BiSS : McAEX42BType; (*Type mcAEX42IT_BISS settings*)
		SSI : McAEX42SType; (*Type mcAEX42IT_SSI settings*)
	END_STRUCT;
	McAEEncX42Type : STRUCT
		InterfaceType : McAEEncX42IfTypType;
	END_STRUCT;
	McAEEncX43IfTypEnum :
		( (*Interface type selector setting*)
		mcAEX43IT_NOT_USE := 0, (*Not used -*)
		mcAEX43IT_BISS := 1, (*BiSS -*)
		mcAEX43IT_SSI := 2, (*SSI -*)
		mcAEX43IT_ENDAT := 3, (*EnDat -*)
		mcAEX43IT_HIPERFACE_DSL := 4, (*HIPERFACE DSL -*)
		mcAEX43IT_TFMT := 5, (*T-Format - Tamagawa digital interface*)
		mcAEX43IT_MOT_DAT_IF := 6, (*Motion Data Interface - B&R bi-directional asynchronous serial interface*)
		mcAEX43IT_ENDAT_SAFEMOTION := 7, (*EnDat SafeMOTION -*)
		mcAEX43IT_ENDAT_3 := 8 (*EnDat 3 -*)
		);
	McAEX43BPwrSupEnum :
		( (*Power supply of the encoder*)
		mcAEX43BPS_EXT := 0, (*External - External (0V)*)
		mcAEX43BPS_PS_5_V := 1, (*PS 5 V - 5 Volt*)
		mcAEX43BPS_PS_12_V := 2 (*PS 12 V - 12 Volt*)
		);
	McAEX43BBiSSFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX43BBFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX43BBFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX43BBFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX43BBFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX43BBFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX43BBiSSFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX43BBFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX43BBFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX43BBFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX43BBFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX43BBFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX43BBiSSFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX43BBiSSFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX43BBiSSFrmCfgStatBit2Enum; (*Warning or error bit*)
	END_STRUCT;
	McAEX43BType : STRUCT (*Type mcAEX43IT_BISS settings*)
		PowerSupply : McAEX43BPwrSupEnum; (*Power supply of the encoder*)
		BiSSFrameConfiguration : McAEX43BBiSSFrmCfgType; (*Frame structure in the order of transfer*)
		CRCPolynomial : UDINT; (*Data verification via CRC (decimal value, 0 deactivates verification)*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEX43SPwrSupEnum :
		( (*Power supply of the encoder*)
		mcAEX43SPS_EXT := 0, (*External - External (0V)*)
		mcAEX43SPS_PS_5_V := 1, (*PS 5 V - 5 Volt*)
		mcAEX43SPS_PS_12_V := 2 (*PS 12 V - 12 Volt*)
		);
	McAEX43SSSIFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX43SSFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX43SSFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX43SSFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX43SSFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX43SSFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX43SSSIFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX43SSFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX43SSFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX43SSFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX43SSFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX43SSFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX43SSSIFrmCfgSSIDatCEnum :
		( (*SSI data code*)
		mcAEX43SSFCSDC_GRAY := 0, (*Gray - Gray*)
		mcAEX43SSFCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAEX43SSSIFrmCfgSSIParCkEnum :
		( (*SSI parity check*)
		mcAEX43SSFCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAEX43SSFCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAEX43SSFCSPC_OFF := 2 (*Off - Off*)
		);
	McAEX43SSSIFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX43SSSIFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX43SSSIFrmCfgStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAEX43SSSIFrmCfgSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAEX43SSSIFrmCfgSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAEX43SType : STRUCT (*Type mcAEX43IT_SSI settings*)
		PowerSupply : McAEX43SPwrSupEnum; (*Power supply of the encoder*)
		SSIFrameConfiguration : McAEX43SSSIFrmCfgType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEEncX43IfTypType : STRUCT
		Type : McAEEncX43IfTypEnum; (*Interface type selector setting*)
		BiSS : McAEX43BType; (*Type mcAEX43IT_BISS settings*)
		SSI : McAEX43SType; (*Type mcAEX43IT_SSI settings*)
	END_STRUCT;
	McAEEncX43Type : STRUCT
		InterfaceType : McAEEncX43IfTypType;
	END_STRUCT;
	McAEEncX11AIfTypEnum :
		( (*Interface type selector setting*)
		mcAEX11AIT_ENDAT := 0, (*EnDat -*)
		mcAEX11AIT_SSI_SIN := 1, (*SSI sine -*)
		mcAEX11AIT_SSI := 2, (*SSI -*)
		mcAEX11AIT_BISS := 3, (*BiSS -*)
		mcAEX11AIT_SIN := 4, (*Sine -*)
		mcAEX11AIT_SIN_W_DCM := 5, (*Sine with DCM -*)
		mcAEX11AIT_HIPERFACE := 6 (*HIPERFACE -*)
		);
	McAEX11ASSSSIFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX11ASSFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX11ASSFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX11ASSFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX11ASSFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX11ASSFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX11ASSSSIFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX11ASSFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX11ASSFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX11ASSFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX11ASSFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX11ASSFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX11ASSSSIFrmCfgSSIDatCEnum :
		( (*SSI data code*)
		mcAEX11ASSFCSDC_GRAY := 0, (*Gray - Gray*)
		mcAEX11ASSFCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAEX11ASSSSIFrmCfgSSIParCkEnum :
		( (*SSI parity check*)
		mcAEX11ASSFCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAEX11ASSFCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAEX11ASSFCSPC_OFF := 2 (*Off - Off*)
		);
	McAEX11ASSSSIFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX11ASSSSIFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX11ASSSSIFrmCfgStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAEX11ASSSSIFrmCfgSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAEX11ASSSSIFrmCfgSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAEX11ASSType : STRUCT (*Type mcAEX11AIT_SSI_SIN settings*)
		SSIFrameConfiguration : McAEX11ASSSSIFrmCfgType; (*Frame structure in the order of transfer*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		PositionValuesEncoderRevolution : DINT; (*Absolute resolution of an encoder revolution*)
		SerialPositionPhaseShift : DINT; (*Serial position phase shift*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEX11ASSSIFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX11ASFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX11ASFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX11ASFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX11ASFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX11ASFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX11ASSSIFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX11ASFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX11ASFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX11ASFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX11ASFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX11ASFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX11ASSSIFrmCfgSSIDatCEnum :
		( (*SSI data code*)
		mcAEX11ASFCSDC_GRAY := 0, (*Gray - Gray*)
		mcAEX11ASFCSDC_BIN := 1 (*Binary - Binary*)
		);
	McAEX11ASSSIFrmCfgSSIParCkEnum :
		( (*SSI parity check*)
		mcAEX11ASFCSPC_SSI_PAR_CK_ODD := 0, (*SSI parity check odd - SSI parity check odd*)
		mcAEX11ASFCSPC_SSI_PAR_CK_EVEN := 1, (*SSI parity check even - SSI parity check even*)
		mcAEX11ASFCSPC_OFF := 2 (*Off - Off*)
		);
	McAEX11ASSSIFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX11ASSSIFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX11ASSSIFrmCfgStatBit2Enum; (*Warning or error bit*)
		SSIDataCode : McAEX11ASSSIFrmCfgSSIDatCEnum; (*SSI data code*)
		SSIParityCheck : McAEX11ASSSIFrmCfgSSIParCkEnum; (*SSI parity check*)
	END_STRUCT;
	McAEX11ASType : STRUCT (*Type mcAEX11AIT_SSI settings*)
		SSIFrameConfiguration : McAEX11ASSSIFrmCfgType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEX11ABBiSSFrmCfgStatBit1Enum :
		( (*Warning or error bit*)
		mcAEX11ABBFCSB1_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX11ABBFCSB1_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX11ABBFCSB1_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX11ABBFCSB1_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX11ABBFCSB1_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX11ABBiSSFrmCfgStatBit2Enum :
		( (*Warning or error bit*)
		mcAEX11ABBFCSB2_NOT_AVL := 0, (*Not available - Not available*)
		mcAEX11ABBFCSB2_ERR_BIT_ACT_HIGH := 1, (*Error bit active high - Error bit active high*)
		mcAEX11ABBFCSB2_ERR_BIT_ACT_LOW := 2, (*Error bit active low - Error bit active low*)
		mcAEX11ABBFCSB2_WAR_BIT_ACT_HIGH := 3, (*Warning bit active high - Warning bit active high*)
		mcAEX11ABBFCSB2_WAR_BIT_ACT_LOW := 4 (*Warning bit active low - Warning bit active low*)
		);
	McAEX11ABBiSSFrmCfgType : STRUCT (*Frame structure in the order of transfer*)
		NumberOfLeadingZeros : USINT; (*Zero bits before position data*)
		NumberOfPositionBits : USINT; (*Position data*)
		NumberOfTrailingZeros : USINT; (*Zero bits after position data*)
		StatusBit1 : McAEX11ABBiSSFrmCfgStatBit1Enum; (*Warning or error bit*)
		StatusBit2 : McAEX11ABBiSSFrmCfgStatBit2Enum; (*Warning or error bit*)
	END_STRUCT;
	McAEX11ABType : STRUCT (*Type mcAEX11AIT_BISS settings*)
		BiSSFrameConfiguration : McAEX11ABBiSSFrmCfgType; (*Frame structure in the order of transfer*)
		CRCPolynomial : UDINT; (*Data verification via CRC (decimal value, 0 deactivates verification)*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
	END_STRUCT;
	McAEX11ASinType : STRUCT (*Type mcAEX11AIT_SIN settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
	END_STRUCT;
	McAEX11ASinWDCMType : STRUCT (*Type mcAEX11AIT_SIN_W_DCM settings*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		DCMBasicDistance : UDINT; (*DCM basic distance*)
		DCMBasicDifference : DINT; (*DCM basic difference*)
	END_STRUCT;
	McAEEncX11AIfTypType : STRUCT
		Type : McAEEncX11AIfTypEnum; (*Interface type selector setting*)
		SSISine : McAEX11ASSType; (*Type mcAEX11AIT_SSI_SIN settings*)
		SSI : McAEX11ASType; (*Type mcAEX11AIT_SSI settings*)
		BiSS : McAEX11ABType; (*Type mcAEX11AIT_BISS settings*)
		Sine : McAEX11ASinType; (*Type mcAEX11AIT_SIN settings*)
		SineWithDCM : McAEX11ASinWDCMType; (*Type mcAEX11AIT_SIN_W_DCM settings*)
	END_STRUCT;
	McAEEncX11AType : STRUCT
		InterfaceType : McAEEncX11AIfTypType;
	END_STRUCT;
	McAEEncIfTypEnum :
		( (*Interface type selector setting*)
		mcAEIT_ENDAT := 0 (*EnDat -*)
		);
	McAEEncIfTypType : STRUCT
		Type : McAEEncIfTypEnum; (*Interface type selector setting*)
	END_STRUCT;
	McAEEncType : STRUCT
		InterfaceType : McAEEncIfTypType;
	END_STRUCT;
	McAEEnc1IfTypEnum :
		( (*Interface type selector setting*)
		mcAE1IT_NOT_USE := 0, (*Not used -*)
		mcAE1IT_INCR := 1, (*Incremental -*)
		mcAE1IT_SSI := 2, (*SSI -*)
		mcAE1IT_BISS := 3 (*BiSS -*)
		);
	McAEEnc1IfTypIncrSymEnum :
		( (*Symmetry selector setting*)
		mcAE1ITIS_SYM := 0, (*Symmetrical -*)
		mcAE1ITIS_ASYM := 1 (*Asymmetrical -*)
		);
	McAEISAOutDrvEnum :
		( (*Output driver of the encoder*)
		mcAEISAOD_PUSH_PULL := 0, (*Push pull - Push pull*)
		mcAEISAOD_PULL := 1 (*Pull - Pull*)
		);
	McAEISAType : STRUCT (*Type mcAE1ITIS_ASYM settings*)
		OutputDriver : McAEISAOutDrvEnum; (*Output driver of the encoder*)
	END_STRUCT;
	McAEEnc1IfTypIncrSymType : STRUCT (*Symmetry of the encoder signals*)
		Type : McAEEnc1IfTypIncrSymEnum; (*Symmetry selector setting*)
		Asymmetrical : McAEISAType; (*Type mcAE1ITIS_ASYM settings*)
	END_STRUCT;
	McAEEnc1IfTypIncrRefPDetectEnum :
		( (*Reference pulse detection*)
		mcAE1ITIRPD_NORM_MOD := 0, (*Normal mode - Normal mode*)
		mcAE1ITIRPD_EDG_TRG_MOD := 1 (*Edge triggered mode - Edge triggered mode*)
		);
	McAEEnc1IfTypIncrRefPMonEnum :
		( (*Reference pulse monitoring selector setting*)
		mcAE1ITIRPM_NOT_USE := 0, (*Not used -*)
		mcAE1ITIRPM_USE := 1 (*Used -*)
		);
	McAEEnc1IfTypIncrRefPMonUseType : STRUCT (*Type mcAE1ITIRPM_USE settings*)
		CheckWindow : UDINT; (*Encoder 1: Incremental Reference pulse check window [Increments]*)
		Interval : UDINT; (*Encoder 1: Incremental Reference pulse interval [Increments]*)
		Width : UDINT; (*Encoder 1: Incremental Reference pulse width [Increments]*)
	END_STRUCT;
	McAEEnc1IfTypIncrRefPMonType : STRUCT
		Type : McAEEnc1IfTypIncrRefPMonEnum; (*Reference pulse monitoring selector setting*)
		Used : McAEEnc1IfTypIncrRefPMonUseType; (*Type mcAE1ITIRPM_USE settings*)
	END_STRUCT;
	McAEEnc1IfTypIncrType : STRUCT (*Type mcAE1IT_INCR settings*)
		Symmetry : McAEEnc1IfTypIncrSymType; (*Symmetry of the encoder signals*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		ReferencePulseDetection : McAEEnc1IfTypIncrRefPDetectEnum; (*Reference pulse detection*)
		ReferencePulseMonitoring : McAEEnc1IfTypIncrRefPMonType;
	END_STRUCT;
	McAEEnc1IfTypSSIType : STRUCT (*Type mcAE1IT_SSI settings*)
		SSIFrameConfiguration : McAPICEITSSIFCType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEEnc1IfTypBiSSType : STRUCT (*Type mcAE1IT_BISS settings*)
		BiSSFrameConfiguration : McAPICEITBiSSFCType; (*Frame structure in the order of transfer*)
		CRCPolynomial : UDINT; (*Data verification via CRC (decimal value, 0 deactivates verification)*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEEnc1IfTypType : STRUCT
		Type : McAEEnc1IfTypEnum; (*Interface type selector setting*)
		Incremental : McAEEnc1IfTypIncrType; (*Type mcAE1IT_INCR settings*)
		SSI : McAEEnc1IfTypSSIType; (*Type mcAE1IT_SSI settings*)
		BiSS : McAEEnc1IfTypBiSSType; (*Type mcAE1IT_BISS settings*)
	END_STRUCT;
	McAEEnc1Type : STRUCT
		InterfaceType : McAEEnc1IfTypType;
	END_STRUCT;
	McAEEnc2IfTypEnum :
		( (*Interface type selector setting*)
		mcAE2IT_NOT_USE := 0, (*Not used -*)
		mcAE2IT_INCR := 1, (*Incremental -*)
		mcAE2IT_SSI := 2, (*SSI -*)
		mcAE2IT_BISS := 3 (*BiSS -*)
		);
	McAEEnc2IfTypIncrSymEnum :
		( (*Symmetry selector setting*)
		mcAE2ITIS_SYM := 0, (*Symmetrical -*)
		mcAE2ITIS_ASYM := 1 (*Asymmetrical -*)
		);
	McAEEnc2IfTypIncrSymType : STRUCT (*Symmetry of the encoder signals*)
		Type : McAEEnc2IfTypIncrSymEnum; (*Symmetry selector setting*)
		Asymmetrical : McAEISAType; (*Type mcAE2ITIS_ASYM settings*)
	END_STRUCT;
	McAEEnc2IfTypIncrRefPDetectEnum :
		( (*Reference pulse detection*)
		mcAE2ITIRPD_NORM_MOD := 0, (*Normal mode - Normal mode*)
		mcAE2ITIRPD_EDG_TRG_MOD := 1 (*Edge triggered mode - Edge triggered mode*)
		);
	McAEEnc2IfTypIncrRefPMonEnum :
		( (*Reference pulse monitoring selector setting*)
		mcAE2ITIRPM_NOT_USE := 0, (*Not used -*)
		mcAE2ITIRPM_USE := 1 (*Used -*)
		);
	McAEEnc2IfTypIncrRefPMonUseType : STRUCT (*Type mcAE2ITIRPM_USE settings*)
		CheckWindow : UDINT; (*Encoder 1: Incremental Reference pulse check window [Increments]*)
		Interval : UDINT; (*Encoder 1: Incremental Reference pulse interval [Increments]*)
		Width : UDINT; (*Encoder 1: Incremental Reference pulse width [Increments]*)
	END_STRUCT;
	McAEEnc2IfTypIncrRefPMonType : STRUCT
		Type : McAEEnc2IfTypIncrRefPMonEnum; (*Reference pulse monitoring selector setting*)
		Used : McAEEnc2IfTypIncrRefPMonUseType; (*Type mcAE2ITIRPM_USE settings*)
	END_STRUCT;
	McAEEnc2IfTypIncrType : STRUCT (*Type mcAE2IT_INCR settings*)
		Symmetry : McAEEnc2IfTypIncrSymType; (*Symmetry of the encoder signals*)
		LinesPerEncoderRevolution : UDINT; (*Absolute number of lines of an encoder revolution*)
		ReferencePulseDetection : McAEEnc2IfTypIncrRefPDetectEnum; (*Reference pulse detection*)
		ReferencePulseMonitoring : McAEEnc2IfTypIncrRefPMonType;
	END_STRUCT;
	McAEEnc2IfTypSSIType : STRUCT (*Type mcAE2IT_SSI settings*)
		SSIFrameConfiguration : McAPICEITSSIFCType; (*Frame structure in the order of transfer*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEEnc2IfTypBiSSType : STRUCT (*Type mcAE2IT_BISS settings*)
		BiSSFrameConfiguration : McAPICEITBiSSFCType; (*Frame structure in the order of transfer*)
		CRCPolynomial : UDINT; (*Data verification via CRC (decimal value, 0 deactivates verification)*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute resolution of an encoder revolution*)
		BaudRate : DINT; (*Transfer rate [kBaud]*)
	END_STRUCT;
	McAEEnc2IfTypType : STRUCT
		Type : McAEEnc2IfTypEnum; (*Interface type selector setting*)
		Incremental : McAEEnc2IfTypIncrType; (*Type mcAE2IT_INCR settings*)
		SSI : McAEEnc2IfTypSSIType; (*Type mcAE2IT_SSI settings*)
		BiSS : McAEEnc2IfTypBiSSType; (*Type mcAE2IT_BISS settings*)
	END_STRUCT;
	McAEEnc2Type : STRUCT
		InterfaceType : McAEEnc2IfTypType;
	END_STRUCT;
	McCfgAcpEncType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_ENC*)
		EncoderX6A : McAEEncX6AType;
		EncoderX6B : McAEEncX6BType;
		EncoderX41 : McAEEncX41Type;
		EncoderX42 : McAEEncX42Type;
		EncoderX43 : McAEEncX43Type;
		EncoderX11A : McAEEncX11AType;
		Encoder : McAEEncType;
		Encoder1 : McAEEnc1Type;
		Encoder2 : McAEEnc2Type;
	END_STRUCT;
	McAVAVirtAxEnum :
		( (*Virtual axis selector setting*)
		mcAVAVA_NOT_USE := 0, (*Not used - Virtual axis is not used*)
		mcAVAVA_USE := 1 (*Used - Virtual axis is used*)
		);
	McAVAVirtAxUseHomeModEnum :
		( (*Mode selector setting*)
		mcAVAVAUHM_DIR := 0, (*Direct - Direct homing*)
		mcAVAVAUHM_RES_POS := 1, (*Restore position - Homing by restoring the position from remanent variable data*)
		mcAVAVAUHM_NOT_USE := 100 (*Not used - No preconfigured homing settings used*)
		);
	McAVAVirtAxUseHomeModDirType : STRUCT (*Type mcAVAVAUHM_DIR settings*)
		Position : LREAL; (*Home position [measurement units]*)
	END_STRUCT;
	McAVAVirtAxUseHomeModType : STRUCT (*Homing mode*)
		Type : McAVAVirtAxUseHomeModEnum; (*Mode selector setting*)
		Direct : McAVAVirtAxUseHomeModDirType; (*Type mcAVAVAUHM_DIR settings*)
	END_STRUCT;
	McAVAVirtAxUseHomeType : STRUCT (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
		Mode : McAVAVirtAxUseHomeModType; (*Homing mode*)
		RestorePositionVariable : STRING[250]; (*Remanent variable used for homing mode: Restore position*)
		AxParCk : McAHMRPAPCType; (*Activate check if axis parameterization has change for restore position*)
	END_STRUCT;
	McAVAVirtAxUseType : STRUCT (*Type mcAVAVA_USE settings*)
		AxisReference : McCfgReferenceType; (*Name of the referenced axis component*)
		Homing : McAVAVirtAxUseHomeType; (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
		JerkFilter : McAJFType; (*Jerk filter*)
		ZeroVibrationFilter : McAZVFType; (*Zero vibration filter*)
		AxisFeatures : McAAFType; (*Features for an axis*)
	END_STRUCT;
	McAVAVirtAxType : STRUCT
		Type : McAVAVirtAxEnum; (*Virtual axis selector setting*)
		Used : McAVAVirtAxUseType; (*Type mcAVAVA_USE settings*)
	END_STRUCT;
	McCfgAcpVirtAxType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_VIRT_AX*)
		VirtualAxis : McAVAVirtAxType;
	END_STRUCT;
	McCfgAcpVirtAxRefType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_VIRT_AX_REF*)
		AxisReference : McCfgReferenceType; (*Name of the referenced axis component*)
	END_STRUCT;
	McAVHHomeModEnum :
		( (*Mode selector setting*)
		mcAVHHM_DIR := 0, (*Direct - Direct homing*)
		mcAVHHM_RES_POS := 1, (*Restore position - Homing by restoring the position from remanent variable data*)
		mcAVHHM_NOT_USE := 100 (*Not used - No preconfigured homing settings used*)
		);
	McAVHHomeModDirType : STRUCT (*Type mcAVHHM_DIR settings*)
		Position : LREAL; (*Home position [measurement units]*)
	END_STRUCT;
	McAVHHomeModType : STRUCT (*Homing mode*)
		Type : McAVHHomeModEnum; (*Mode selector setting*)
		Direct : McAVHHomeModDirType; (*Type mcAVHHM_DIR settings*)
	END_STRUCT;
	McAVHHomeType : STRUCT (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
		Mode : McAVHHomeModType; (*Homing mode*)
		RestorePositionVariable : STRING[250]; (*Remanent variable used for homing mode: Restore position*)
		AxParCk : McAHMRPAPCType; (*Activate check if axis parameterization has change for restore position*)
	END_STRUCT;
	McCfgAcpVirtHomeType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_VIRT_HOME*)
		Homing : McAVHHomeType; (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
	END_STRUCT;
	McCfgAcpVirtJerkFltrType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_VIRT_JERK_FLTR*)
		JerkFilter : McAJFType; (*Jerk filter*)
	END_STRUCT;
	McCfgAcpVirtAxFeatType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_VIRT_AX_FEAT*)
		AxisFeatures : McAAFType; (*Features for an axis*)
	END_STRUCT;
	McCfgAcpVirtZeroVibFltrType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_VIRT_ZERO_VIB_FLTR*)
		ZeroVibrationFilter : McAZVFType; (*Zero vibration filter*)
	END_STRUCT;
	McACFChFeatType : STRUCT (*Features for the channel of a module*)
		FeatureReference : McCfgUnboundedArrayType; (*Name of the axis feature reference (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McCfgAcpChFeatType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_CH_FEAT*)
		ChannelFeatures : McACFChFeatType; (*Features for the channel of a module*)
	END_STRUCT;
	McAEEAExtEncAxEnum :
		( (*External encoder axis selector setting*)
		mcAEEAEEA_NOT_USE := 0, (*Not used - External encoder axis is not used*)
		mcAEEAEEA_USE := 1 (*Used - External encoder axis is used*)
		);
	McAEEAUseEncLinkEnum :
		( (*Encoder link selector setting*)
		mcAEEAUEL_ONE_ENC := 0 (*One encoder -*)
		);
	McAEEAUseEncLinkOneEncPosEncEnum :
		( (*Position encoder selector setting*)
		mcAEEAUELOEPE_ENC_X41 := 0, (*Encoder X41 -*)
		mcAEEAUELOEPE_ENC_SS1X41X := 1, (*Encoder SS1.X41x - Plug-in module in SS1*)
		mcAEEAUELOEPE_ENC_X42 := 2, (*Encoder X42 -*)
		mcAEEAUELOEPE_ENC_SS1X42X := 3, (*Encoder SS1.X42x - Plug-in module in SS1*)
		mcAEEAUELOEPE_ENC_X43 := 4, (*Encoder X43 -*)
		mcAEEAUELOEPE_ENC_SS1X43X := 5, (*Encoder SS1.X43x - Plug-in module in SS1*)
		mcAEEAUELOEPE_ENC_SS1X11 := 6, (*Encoder SS1.X11 - Plug-in module in SS1*)
		mcAEEAUELOEPE_ENC_SS2X11 := 7, (*Encoder SS2.X11 - Plug-in module in SS2*)
		mcAEEAUELOEPE_ENC_SS3X11 := 8, (*Encoder SS3.X11 - Plug-in module in SS3*)
		mcAEEAUELOEPE_ENC_SS4X11 := 9, (*Encoder SS4.X11 - Plug-in module in SS4*)
		mcAEEAUELOEPE_ENC_ON_NETW := 10 (*Encoder on network - Encoder on Network*)
		);
	McAEEON0Enum :
		( (*Position type selector setting*)
		mcAEEON0_ABS := 0, (*Absolute - Position from an absolute encoder*)
		mcAEEON0_INCR := 1 (*Incremental - Position from an incremental encoder*)
		);
	McAEEON00Enum :
		( (*Position format selector setting*)
		mcAEEON00_D64B := 0, (*d64b - Data from a 64 bit source*)
		mcAEEON00_D32B := 1, (*d32b - Data from a 32 bit source*)
		mcAEEON00_D16B := 2 (*d16b - Data from a 16 bit source*)
		);
	McAEEON00d64bPosLWEnum :
		( (*Position LW selector setting*)
		mcAEEON00DPL_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON00DPL_IO_CH_UDINT := 1 (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		);
	McAEEON00d64bPosLWIOChDINTType : STRUCT (*Type mcAEEON00DPL_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position low word*)
	END_STRUCT;
	McAEEON00d64bPosLWIOChUDINTType : STRUCT (*Type mcAEEON00DPL_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position low word*)
	END_STRUCT;
	McAEEON00d64bPosLWType : STRUCT (*Position low word source*)
		Type : McAEEON00d64bPosLWEnum; (*Position LW selector setting*)
		IOChannelDINT : McAEEON00d64bPosLWIOChDINTType; (*Type mcAEEON00DPL_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON00d64bPosLWIOChUDINTType; (*Type mcAEEON00DPL_IO_CH_UDINT settings*)
	END_STRUCT;
	McAEEON00d64bPosHWEnum :
		( (*Position HW selector setting*)
		mcAEEON00DPH_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON00DPH_IO_CH_UDINT := 1 (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		);
	McAEEON00d64bPosHWIOChDINTType : STRUCT (*Type mcAEEON00DPH_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position high word*)
	END_STRUCT;
	McAEEON00d64bPosHWIOChUDINTType : STRUCT (*Type mcAEEON00DPH_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position high word*)
	END_STRUCT;
	McAEEON00d64bPosHWType : STRUCT (*Position high word source*)
		Type : McAEEON00d64bPosHWEnum; (*Position HW selector setting*)
		IOChannelDINT : McAEEON00d64bPosHWIOChDINTType; (*Type mcAEEON00DPH_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON00d64bPosHWIOChUDINTType; (*Type mcAEEON00DPH_IO_CH_UDINT settings*)
	END_STRUCT;
	McAEEON00d64bType : STRUCT (*Type mcAEEON00_D64B settings*)
		ValueRangeOfPositionLW : UDINT; (*Value range (span) of position low word [increment]*)
		ValueRangeOfPositionHW : UDINT; (*Value range (span) of position high word [increment]*)
		PositionLW : McAEEON00d64bPosLWType; (*Position low word source*)
		PositionHW : McAEEON00d64bPosHWType; (*Position high word source*)
	END_STRUCT;
	McAEEON00d32bPosEnum :
		( (*Position selector setting*)
		mcAEEON00DP_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON00DP_IO_CH_UDINT := 1 (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		);
	McAEEON00d32bPosIOChDINTType : STRUCT (*Type mcAEEON00DP_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAEEON00d32bPosIOChUDINTType : STRUCT (*Type mcAEEON00DP_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAEEON00d32bPosType : STRUCT (*Position source*)
		Type : McAEEON00d32bPosEnum; (*Position selector setting*)
		IOChannelDINT : McAEEON00d32bPosIOChDINTType; (*Type mcAEEON00DP_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON00d32bPosIOChUDINTType; (*Type mcAEEON00DP_IO_CH_UDINT settings*)
	END_STRUCT;
	McAEEON00d32bType : STRUCT (*Type mcAEEON00_D32B settings*)
		ValueRangeOfPosition : UDINT; (*Value range (span) of position [increment]*)
		Position : McAEEON00d32bPosType; (*Position source*)
	END_STRUCT;
	McAEEON00d16bPosEnum :
		( (*Position selector setting*)
		mcAEEON00DP_IO_CH_INT := 2, (*I/O channel INT - Data from a signed 16 bit I/O channel*)
		mcAEEON00DP_IO_CH_UINT := 3 (*I/O channel UINT - Data from an unsigned 16 bit I/O channel*)
		);
	McAEEON00d16bPosIOChINTType : STRUCT (*Type mcAEEON00DP_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAEEON00d16bPosIOChUINTType : STRUCT (*Type mcAEEON00DP_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAEEON00d16bPosType : STRUCT (*Position source*)
		Type : McAEEON00d16bPosEnum; (*Position selector setting*)
		IOChannelINT : McAEEON00d16bPosIOChINTType; (*Type mcAEEON00DP_IO_CH_INT settings*)
		IOChannelUINT : McAEEON00d16bPosIOChUINTType; (*Type mcAEEON00DP_IO_CH_UINT settings*)
	END_STRUCT;
	McAEEON00d16bType : STRUCT (*Type mcAEEON00_D16B settings*)
		ValueRangeOfPosition : UDINT; (*Value range (span) of position [increment]*)
		Position : McAEEON00d16bPosType; (*Position source*)
	END_STRUCT;
	McAEEON00Type : STRUCT (*Position value format*)
		Type : McAEEON00Enum; (*Position format selector setting*)
		d64b : McAEEON00d64bType; (*Type mcAEEON00_D64B settings*)
		d32b : McAEEON00d32bType; (*Type mcAEEON00_D32B settings*)
		d16b : McAEEON00d16bType; (*Type mcAEEON00_D16B settings*)
	END_STRUCT;
	McAEEON01Enum :
		( (*Position rollover selector setting*)
		mcAEEON01_AT_ENC_MEAS_RNG := 0, (*At encoder measurement range - At encoder measurement range: for true absolute encoder*)
		mcAEEON01_AT_DAT_TYP_RNG := 1 (*At data type range - At data type range: for incremental encoder with absolute information*)
		);
	McAEEON01Type : STRUCT (*Position value rollover mode*)
		Type : McAEEON01Enum; (*Position rollover selector setting*)
	END_STRUCT;
	McAEEON02Enum :
		( (*Position time selector setting*)
		mcAEEON02_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON02_IO_CH_UDINT := 1, (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		mcAEEON02_IO_CH_INT := 2, (*I/O channel INT - Data from a signed 16 bit I/O channel*)
		mcAEEON02_IO_CH_UINT := 3, (*I/O channel UINT - Data from an unsigned 16 bit I/O channel*)
		mcAEEON02_NOT_USE := 4 (*Not used - Position time not used*)
		);
	McAEEON02IOChDINTType : STRUCT (*Type mcAEEON02_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position time*)
	END_STRUCT;
	McAEEON02IOChUDINTType : STRUCT (*Type mcAEEON02_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position time*)
	END_STRUCT;
	McAEEON02IOChINTType : STRUCT (*Type mcAEEON02_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position time*)
	END_STRUCT;
	McAEEON02IOChUINTType : STRUCT (*Type mcAEEON02_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position time*)
	END_STRUCT;
	McAEEON02Type : STRUCT (*Position time source*)
		Type : McAEEON02Enum; (*Position time selector setting*)
		IOChannelDINT : McAEEON02IOChDINTType; (*Type mcAEEON02_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON02IOChUDINTType; (*Type mcAEEON02_IO_CH_UDINT settings*)
		IOChannelINT : McAEEON02IOChINTType; (*Type mcAEEON02_IO_CH_INT settings*)
		IOChannelUINT : McAEEON02IOChUINTType; (*Type mcAEEON02_IO_CH_UINT settings*)
	END_STRUCT;
	McAEEON03Enum :
		( (*Encoder status selector setting*)
		mcAEEON03_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON03_IO_CH_UDINT := 1, (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		mcAEEON03_IO_CH_INT := 2, (*I/O channel INT - Data from a signed 16 bit I/O channel*)
		mcAEEON03_IO_CH_UINT := 3, (*I/O channel UINT - Data from an unsigned 16 bit I/O channel*)
		mcAEEON03_IO_CH_SINT := 4, (*I/O channel SINT - Data from a signed 8 bit I/O channel*)
		mcAEEON03_IO_CH_USINT := 5, (*I/O channel USINT - Data from an unsigned 8 bit I/O channel*)
		mcAEEON03_NOT_USE := 6 (*Not used - Encoder status not used*)
		);
	McAEEON03IOChDINTType : STRUCT (*Type mcAEEON03_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON03IOChUDINTType : STRUCT (*Type mcAEEON03_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON03IOChINTType : STRUCT (*Type mcAEEON03_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON03IOChUINTType : STRUCT (*Type mcAEEON03_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON03IOChSINTType : STRUCT (*Type mcAEEON03_IO_CH_SINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON03IOChUSINTType : STRUCT (*Type mcAEEON03_IO_CH_USINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON03Type : STRUCT (*Encoder status source*)
		Type : McAEEON03Enum; (*Encoder status selector setting*)
		IOChannelDINT : McAEEON03IOChDINTType; (*Type mcAEEON03_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON03IOChUDINTType; (*Type mcAEEON03_IO_CH_UDINT settings*)
		IOChannelINT : McAEEON03IOChINTType; (*Type mcAEEON03_IO_CH_INT settings*)
		IOChannelUINT : McAEEON03IOChUINTType; (*Type mcAEEON03_IO_CH_UINT settings*)
		IOChannelSINT : McAEEON03IOChSINTType; (*Type mcAEEON03_IO_CH_SINT settings*)
		IOChannelUSINT : McAEEON03IOChUSINTType; (*Type mcAEEON03_IO_CH_USINT settings*)
	END_STRUCT;
	McAEEON0AbsType : STRUCT (*Type mcAEEON0_ABS settings*)
		PositionFormat : McAEEON00Type; (*Position value format*)
		PositionRollover : McAEEON01Type; (*Position value rollover mode*)
		PositionTime : McAEEON02Type; (*Position time source*)
		EncoderStatus : McAEEON03Type; (*Encoder status source*)
	END_STRUCT;
	McAEEON04Enum :
		( (*Position selector setting*)
		mcAEEON04_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON04_IO_CH_UDINT := 1, (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		mcAEEON04_IO_CH_INT := 2, (*I/O channel INT - Data from a signed 16 bit I/O channel*)
		mcAEEON04_IO_CH_UINT := 3 (*I/O channel UINT - Data from an unsigned 16 bit I/O channel*)
		);
	McAEEON04IOChDINTType : STRUCT (*Type mcAEEON04_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAEEON04IOChUDINTType : STRUCT (*Type mcAEEON04_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAEEON04IOChINTType : STRUCT (*Type mcAEEON04_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAEEON04IOChUINTType : STRUCT (*Type mcAEEON04_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position*)
	END_STRUCT;
	McAEEON04Type : STRUCT (*Position source*)
		Type : McAEEON04Enum; (*Position selector setting*)
		IOChannelDINT : McAEEON04IOChDINTType; (*Type mcAEEON04_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON04IOChUDINTType; (*Type mcAEEON04_IO_CH_UDINT settings*)
		IOChannelINT : McAEEON04IOChINTType; (*Type mcAEEON04_IO_CH_INT settings*)
		IOChannelUINT : McAEEON04IOChUINTType; (*Type mcAEEON04_IO_CH_UINT settings*)
	END_STRUCT;
	McAEEON05Enum :
		( (*Position time selector setting*)
		mcAEEON05_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON05_IO_CH_UDINT := 1, (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		mcAEEON05_IO_CH_INT := 2, (*I/O channel INT - Data from a signed 16 bit I/O channel*)
		mcAEEON05_IO_CH_UINT := 3, (*I/O channel UINT - Data from an unsigned 16 bit I/O channel*)
		mcAEEON05_NOT_USE := 4 (*Not used - Position time not used*)
		);
	McAEEON05IOChDINTType : STRUCT (*Type mcAEEON05_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position time*)
	END_STRUCT;
	McAEEON05IOChUDINTType : STRUCT (*Type mcAEEON05_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position time*)
	END_STRUCT;
	McAEEON05IOChINTType : STRUCT (*Type mcAEEON05_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position time*)
	END_STRUCT;
	McAEEON05IOChUINTType : STRUCT (*Type mcAEEON05_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the position time*)
	END_STRUCT;
	McAEEON05Type : STRUCT (*Position time source*)
		Type : McAEEON05Enum; (*Position time selector setting*)
		IOChannelDINT : McAEEON05IOChDINTType; (*Type mcAEEON05_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON05IOChUDINTType; (*Type mcAEEON05_IO_CH_UDINT settings*)
		IOChannelINT : McAEEON05IOChINTType; (*Type mcAEEON05_IO_CH_INT settings*)
		IOChannelUINT : McAEEON05IOChUINTType; (*Type mcAEEON05_IO_CH_UINT settings*)
	END_STRUCT;
	McAEEON06Enum :
		( (*Encoder status selector setting*)
		mcAEEON06_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON06_IO_CH_UDINT := 1, (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		mcAEEON06_IO_CH_INT := 2, (*I/O channel INT - Data from a signed 16 bit I/O channel*)
		mcAEEON06_IO_CH_UINT := 3, (*I/O channel UINT - Data from an unsigned 16 bit I/O channel*)
		mcAEEON06_IO_CH_SINT := 4, (*I/O channel SINT - Data from a signed 8 bit I/O channel*)
		mcAEEON06_IO_CH_USINT := 5, (*I/O channel USINT - Data from an unsigned 8 bit I/O channel*)
		mcAEEON06_NOT_USE := 6 (*Not used - Encoder status not used*)
		);
	McAEEON06IOChDINTType : STRUCT (*Type mcAEEON06_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON06IOChUDINTType : STRUCT (*Type mcAEEON06_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON06IOChINTType : STRUCT (*Type mcAEEON06_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON06IOChUINTType : STRUCT (*Type mcAEEON06_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON06IOChSINTType : STRUCT (*Type mcAEEON06_IO_CH_SINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON06IOChUSINTType : STRUCT (*Type mcAEEON06_IO_CH_USINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the encoder status*)
	END_STRUCT;
	McAEEON06Type : STRUCT (*Encoder status source*)
		Type : McAEEON06Enum; (*Encoder status selector setting*)
		IOChannelDINT : McAEEON06IOChDINTType; (*Type mcAEEON06_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON06IOChUDINTType; (*Type mcAEEON06_IO_CH_UDINT settings*)
		IOChannelINT : McAEEON06IOChINTType; (*Type mcAEEON06_IO_CH_INT settings*)
		IOChannelUINT : McAEEON06IOChUINTType; (*Type mcAEEON06_IO_CH_UINT settings*)
		IOChannelSINT : McAEEON06IOChSINTType; (*Type mcAEEON06_IO_CH_SINT settings*)
		IOChannelUSINT : McAEEON06IOChUSINTType; (*Type mcAEEON06_IO_CH_USINT settings*)
	END_STRUCT;
	McAEEON07Enum :
		( (*Reference pulse selector setting*)
		mcAEEON07_NOT_USE := 0, (*Not used - Reference pulse not used*)
		mcAEEON07_USE := 1 (*Used - Reference pulse used*)
		);
	McAEEON07UsePosEnum :
		( (*Position selector setting*)
		mcAEEON07UP_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON07UP_IO_CH_UDINT := 1, (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		mcAEEON07UP_IO_CH_INT := 2, (*I/O channel INT - Data from a signed 16 bit I/O channel*)
		mcAEEON07UP_IO_CH_UINT := 3 (*I/O channel UINT - Data from an unsigned 16 bit I/O channel*)
		);
	McAEEON07UsePosIOChDINTType : STRUCT (*Type mcAEEON07UP_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse position*)
	END_STRUCT;
	McAEEON07UsePosIOChUDINTType : STRUCT (*Type mcAEEON07UP_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse position*)
	END_STRUCT;
	McAEEON07UsePosIOChINTType : STRUCT (*Type mcAEEON07UP_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse position*)
	END_STRUCT;
	McAEEON07UsePosIOChUINTType : STRUCT (*Type mcAEEON07UP_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse position*)
	END_STRUCT;
	McAEEON07UsePosType : STRUCT (*Reference pulse position source*)
		Type : McAEEON07UsePosEnum; (*Position selector setting*)
		IOChannelDINT : McAEEON07UsePosIOChDINTType; (*Type mcAEEON07UP_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON07UsePosIOChUDINTType; (*Type mcAEEON07UP_IO_CH_UDINT settings*)
		IOChannelINT : McAEEON07UsePosIOChINTType; (*Type mcAEEON07UP_IO_CH_INT settings*)
		IOChannelUINT : McAEEON07UsePosIOChUINTType; (*Type mcAEEON07UP_IO_CH_UINT settings*)
	END_STRUCT;
	McAEEON07UseCntEnum :
		( (*Count selector setting*)
		mcAEEON07UC_IO_CH_DINT := 0, (*I/O channel DINT - Data from a signed 32 bit I/O channel*)
		mcAEEON07UC_IO_CH_UDINT := 1, (*I/O channel UDINT - Data from an unsigned 32 bit I/O channel*)
		mcAEEON07UC_IO_CH_INT := 2, (*I/O channel INT - Data from a signed 16 bit I/O channel*)
		mcAEEON07UC_IO_CH_UINT := 3 (*I/O channel UINT - Data from an unsigned 16 bit I/O channel*)
		);
	McAEEON07UseCntIOChDINTType : STRUCT (*Type mcAEEON07UC_IO_CH_DINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	McAEEON07UseCntIOChUDINTType : STRUCT (*Type mcAEEON07UC_IO_CH_UDINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	McAEEON07UseCntIOChINTType : STRUCT (*Type mcAEEON07UC_IO_CH_INT settings*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	McAEEON07UseCntIOChUINTType : STRUCT (*Type mcAEEON07UC_IO_CH_UINT settings*)
		ChannelMapping : STRING[250]; (*Input source for the reference pulse count*)
	END_STRUCT;
	McAEEON07UseCntType : STRUCT (*Reference pulse count source*)
		Type : McAEEON07UseCntEnum; (*Count selector setting*)
		IOChannelDINT : McAEEON07UseCntIOChDINTType; (*Type mcAEEON07UC_IO_CH_DINT settings*)
		IOChannelUDINT : McAEEON07UseCntIOChUDINTType; (*Type mcAEEON07UC_IO_CH_UDINT settings*)
		IOChannelINT : McAEEON07UseCntIOChINTType; (*Type mcAEEON07UC_IO_CH_INT settings*)
		IOChannelUINT : McAEEON07UseCntIOChUINTType; (*Type mcAEEON07UC_IO_CH_UINT settings*)
	END_STRUCT;
	McAEEON07UseType : STRUCT (*Type mcAEEON07_USE settings*)
		Position : McAEEON07UsePosType; (*Reference pulse position source*)
		Count : McAEEON07UseCntType; (*Reference pulse count source*)
	END_STRUCT;
	McAEEON07Type : STRUCT (*Use reference pulse*)
		Type : McAEEON07Enum; (*Reference pulse selector setting*)
		Used : McAEEON07UseType; (*Type mcAEEON07_USE settings*)
	END_STRUCT;
	McAEEON0IncrType : STRUCT (*Type mcAEEON0_INCR settings*)
		Position : McAEEON04Type; (*Position source*)
		PositionTime : McAEEON05Type; (*Position time source*)
		EncoderStatus : McAEEON06Type; (*Encoder status source*)
		ReferencePulse : McAEEON07Type; (*Use reference pulse*)
	END_STRUCT;
	McAEEON0Type : STRUCT (*Type of the encoder*)
		Type : McAEEON0Enum; (*Position type selector setting*)
		Absolute : McAEEON0AbsType; (*Type mcAEEON0_ABS settings*)
		Incremental : McAEEON0IncrType; (*Type mcAEEON0_INCR settings*)
	END_STRUCT;
	McAEEON10Enum :
		( (*Position interpolation selector setting*)
		mcAEEON10_NOT_USE := 0, (*Not used - Encoder position interpolation is not used*)
		mcAEEON10_USE := 1 (*Used - Encoder position inerpolation is used*)
		);
	McAEEON10Type : STRUCT (*Encoder position interpolation*)
		Type : McAEEON10Enum; (*Position interpolation selector setting*)
	END_STRUCT;
	McAEEON1Type : STRUCT (*Position information processing parameters*)
		TimeoutValue : REAL; (*Encoder position information refresh timeout time [s]*)
		PositionInterpolation : McAEEON10Type; (*Encoder position interpolation*)
		NetworkCompensationTime : REAL; (*The encoder position is pre-calculated by this amount of time [s]*)
	END_STRUCT;
	McAEEAUseEncLinkOEPEEONType : STRUCT (*Type mcAEEAUELOEPE_ENC_ON_NETW settings*)
		IncrementsPerEncoderRevolution : UDINT; (*Absolute number of increments of an encoder revolution [increment/rev]*)
		PositionType : McAEEON0Type; (*Type of the encoder*)
		PositionProcessing : McAEEON1Type; (*Position information processing parameters*)
	END_STRUCT;
	McAEEAUseEncLinkOneEncPosEncType : STRUCT
		Type : McAEEAUseEncLinkOneEncPosEncEnum; (*Position encoder selector setting*)
		EncoderOnNetwork : McAEEAUseEncLinkOEPEEONType; (*Type mcAEEAUELOEPE_ENC_ON_NETW settings*)
	END_STRUCT;
	McAEEAUELOneEncPosFltrEnum :
		( (*Position filter selector setting*)
		mcAEEAUELOEPF_EXTPOL_AND_DIST := 0 (*Extrapolation and disturbance - Extrapolation filter and disturbance filter*)
		);
	McAEEAUELOEPosFltrExtpolDistType : STRUCT (*Type mcAEEAUELOEPF_EXTPOL_AND_DIST settings*)
		PositionFilterTimeConstant : REAL; (*Time constant for acutal position filter*)
		ExtrapolationTime : REAL; (*Extrapolation time for acutal position filter [s]*)
	END_STRUCT;
	McAEEAUELOneEncPosFltrType : STRUCT (*Filter for the encoder position*)
		Type : McAEEAUELOneEncPosFltrEnum; (*Position filter selector setting*)
		ExtrapolationAndDisturbance : McAEEAUELOEPosFltrExtpolDistType; (*Type mcAEEAUELOEPF_EXTPOL_AND_DIST settings*)
	END_STRUCT;
	McAEEAUseEncLinkOneEncType : STRUCT (*Type mcAEEAUEL_ONE_ENC settings*)
		PositionEncoder : McAEEAUseEncLinkOneEncPosEncType;
		PositionFilter : McAEEAUELOneEncPosFltrType; (*Filter for the encoder position*)
		EncoderParameterSet : McAELEncParSetEnum; (*Encoder parameter set selection*)
	END_STRUCT;
	McAEEAUseEncLinkType : STRUCT
		Type : McAEEAUseEncLinkEnum; (*Encoder link selector setting*)
		OneEncoder : McAEEAUseEncLinkOneEncType; (*Type mcAEEAUEL_ONE_ENC settings*)
	END_STRUCT;
	McAEEAHModEnum :
		( (*Mode selector setting*)
		mcAEEAHM_DIR := 0, (*Direct - Direct homing*)
		mcAEEAHM_ABS := 4, (*Absolute - Homing by setting the home offset*)
		mcAEEAHM_RES_POS := 10, (*Restore position - Homing by restoring the position from remanent variable data*)
		mcAEEAHM_NOT_USE := 100 (*Not used - No preconfigured homing settings used*)
		);
	McAEEAHModDirRefPEnum :
		( (*Reference pulse selector setting*)
		mcAEEAHMDRP_NOT_USE := 0, (*Not used - Reference pulse is not used*)
		mcAEEAHMDRP_USE := 1 (*Used - Reference pulse is used*)
		);
	McAEEAHModDirRefPType : STRUCT (*Use reference pulse of encoder*)
		Type : McAEEAHModDirRefPEnum; (*Reference pulse selector setting*)
	END_STRUCT;
	McAEEAHModDirType : STRUCT (*Type mcAEEAHM_DIR settings*)
		Position : LREAL; (*Home position [measurement units]*)
		ReferencePulse : McAEEAHModDirRefPType; (*Use reference pulse of encoder*)
	END_STRUCT;
	McAEEAHModAbsType : STRUCT (*Type mcAEEAHM_ABS settings*)
		Position : LREAL; (*Home offset [measurement units]*)
	END_STRUCT;
	McAEEAHModType : STRUCT (*Homing mode*)
		Type : McAEEAHModEnum; (*Mode selector setting*)
		Direct : McAEEAHModDirType; (*Type mcAEEAHM_DIR settings*)
		Absolute : McAEEAHModAbsType; (*Type mcAEEAHM_ABS settings*)
	END_STRUCT;
	McAEEAHType : STRUCT (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
		Mode : McAEEAHModType; (*Homing mode*)
		RestorePositionVariable : STRING[250]; (*Remanent variable used for homing mode: Restore position*)
		AxParCk : McAHMRPAPCType; (*Activate check if axis parameterization has change for restore position*)
	END_STRUCT;
	McAEEAExtEncAxUseType : STRUCT (*Type mcAEEAEEA_USE settings*)
		AxisReference : McCfgReferenceType; (*Name of the referenced axis component*)
		EncoderLink : McAEEAUseEncLinkType;
		MechanicalElements : McAMEType; (*Parameter of hardware elements situated between motor encoder and load which influence the scaling*)
		Homing : McAEEAHType; (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
	END_STRUCT;
	McAEEAExtEncAxType : STRUCT (*Use External encoder axis*)
		Type : McAEEAExtEncAxEnum; (*External encoder axis selector setting*)
		Used : McAEEAExtEncAxUseType; (*Type mcAEEAEEA_USE settings*)
	END_STRUCT;
	McCfgAcpExtEncAxType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_EXT_ENC_AX*)
		ExternalEncoderAxis : McAEEAExtEncAxType; (*Use External encoder axis*)
	END_STRUCT;
	McCfgAcpExtEncAxRefType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_EXT_ENC_AX_REF*)
		AxisReference : McCfgReferenceType; (*Name of the referenced axis component*)
	END_STRUCT;
	McCfgAcpExtEncAxEncLinkType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_EXT_ENC_AX_ENC_LINK*)
		EncoderLink : McAEEAUseEncLinkType;
	END_STRUCT;
	McCfgAcpExtEncAxMechElmType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_EXT_ENC_AX_MECH_ELM*)
		MechanicalElements : McAMEType; (*Parameter of hardware elements situated between motor encoder and load which influence the scaling*)
	END_STRUCT;
	McCfgAcpExtEncAxHomeType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_ACP_EXT_ENC_AX_HOME*)
		Homing : McAEEAHType; (*Homing mode and parameters which can be used within the application program as preconfigured setting*)
	END_STRUCT;
	McAFAIProdFamEnum :
		( (*ACOPOS product family selector setting*)
		mcAFAIPF_ACP := 0, (*ACOPOS -*)
		mcAFAIPF_ACPM := 1, (*ACOPOSmulti -*)
		mcAFAIPF_ACP_P3 := 2 (*ACOPOS P3 -*)
		);
	McAFAIACPAnInEnum :
		( (*Analog input 1-4 selector setting*)
		mcAFAIACPAI_SS2X111 := 0, (*SS2.X11.1 -*)
		mcAFAIACPAI_SS2X112 := 1, (*SS2.X11.2 -*)
		mcAFAIACPAI_SS3X111 := 2, (*SS3.X11.1 -*)
		mcAFAIACPAI_SS3X112 := 3, (*SS3.X11.2 -*)
		mcAFAIACPAI_SS4X111 := 4, (*SS4.X11.1 -*)
		mcAFAIACPAI_SS4X112 := 5 (*SS4.X11.2 -*)
		);
	McAFAIAnInScEnum :
		( (*Scaling selector setting*)
		mcAFAIAIS_NOT_USE := 0, (*Not used -*)
		mcAFAIAIS_USE := 1 (*Used -*)
		);
	McAFAIAnInScUseType : STRUCT (*Type mcAFAIAIS_USE settings*)
		MinimumVoltage : REAL; (*Minimum voltage of the analog input [V]*)
		MaximumVoltage : REAL; (*Maximum voltage of the analog input [V]*)
		MinimumScaledValue : REAL; (*Minimum scaled value of the analog input [signal units]*)
		MaximumScaledValue : REAL; (*Maximum scaled value of the analog input [signal units]*)
	END_STRUCT;
	McAFAIAnInScType : STRUCT
		Type : McAFAIAnInScEnum; (*Scaling selector setting*)
		Used : McAFAIAnInScUseType; (*Type mcAFAIAIS_USE settings*)
	END_STRUCT;
	McAFAIACPAnInCmnType : STRUCT (*Common settings for all Type values*)
		Scaling : McAFAIAnInScType;
	END_STRUCT;
	McAFAIACPAnInType : STRUCT
		Type : McAFAIACPAnInEnum; (*Analog input 1-4 selector setting*)
		Common : McAFAIACPAnInCmnType; (*Common settings for all Type values*)
	END_STRUCT;
	McAFAIACPType : STRUCT (*Type mcAFAIPF_ACP settings*)
		AnalogInput : McCfgUnboundedArrayType; (*Connect array of type McAFAIACPAnInType*)
	END_STRUCT;
	McAFAIACPmultiAnInEnum :
		( (*Analog input 1-4 selector setting*)
		mcAFAIACPMULTIAI_SS2X111 := 0, (*SS2.X11.1 -*)
		mcAFAIACPMULTIAI_SS2X112 := 1, (*SS2.X11.2 -*)
		mcAFAIACPMULTIAI_SS2X113 := 2, (*SS2.X11.3 -*)
		mcAFAIACPMULTIAI_SS2X114 := 3 (*SS2.X11.4 -*)
		);
	McAFAIACPmultiAnInCmnType : STRUCT (*Common settings for all Type values*)
		Scaling : McAFAIAnInScType;
	END_STRUCT;
	McAFAIACPmultiAnInType : STRUCT
		Type : McAFAIACPmultiAnInEnum; (*Analog input 1-4 selector setting*)
		Common : McAFAIACPmultiAnInCmnType; (*Common settings for all Type values*)
	END_STRUCT;
	McAFAIACPmultiType : STRUCT (*Type mcAFAIPF_ACPM settings*)
		AnalogInput : McCfgUnboundedArrayType; (*Connect array of type McAFAIACPmultiAnInType*)
	END_STRUCT;
	McAFAIACPP3AnInEnum :
		( (*Analog input 1-3 selector setting*)
		mcAFAIACPP3AI_SS1X41E1 := 0, (*SS1.X41E.1 -*)
		mcAFAIACPP3AI_SS1X41E2 := 1, (*SS1.X41E.2 -*)
		mcAFAIACPP3AI_SS1X41E3 := 2 (*SS1.X41E.3 -*)
		);
	McAFAIACPP3AnInCmnType : STRUCT (*Common settings for all Type values*)
		Scaling : McAFAIAnInScType;
	END_STRUCT;
	McAFAIACPP3AnInType : STRUCT
		Type : McAFAIACPP3AnInEnum; (*Analog input 1-3 selector setting*)
		Common : McAFAIACPP3AnInCmnType; (*Common settings for all Type values*)
	END_STRUCT;
	McAFAIACPP3Type : STRUCT (*Type mcAFAIPF_ACP_P3 settings*)
		AnalogInput : McCfgUnboundedArrayType; (*Connect array of type McAFAIACPP3AnInType*)
	END_STRUCT;
	McAFAIProdFamType : STRUCT
		Type : McAFAIProdFamEnum; (*ACOPOS product family selector setting*)
		ACOPOS : McAFAIACPType; (*Type mcAFAIPF_ACP settings*)
		ACOPOSmulti : McAFAIACPmultiType; (*Type mcAFAIPF_ACPM settings*)
		ACOPOSP3 : McAFAIACPP3Type; (*Type mcAFAIPF_ACP_P3 settings*)
	END_STRUCT;
	McCfgAxFeatAInType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_AX_FEAT_A_IN*)
		ProductFamily : McAFAIProdFamType;
	END_STRUCT;
	McAFAPTTranOrdEnum :
		( (*Transfer order in reference to other configuration parameters*)
		mcAFAPTTO_END_OF_INIT := 0, (*End of initialization - Transfer after other configuration parameters*)
		mcAFAPTTO_ST_OF_INIT := 1 (*Start of initialization - Transfer before other configuration parameters*)
		);
	McCfgAxFeatAcpParTblType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_AX_FEAT_ACP_PAR_TBL*)
		ACOPOSParameterTableReference : STRING[250]; (*Name of the ACOPOS parameter table*)
		TransferOrder : McAFAPTTranOrdEnum; (*Transfer order in reference to other configuration parameters*)
	END_STRUCT;
	McAFASPTCTranOrdEnum :
		( (*Transfer order in reference to other configuration parameters*)
		mcAFASPTCTO_END_OF_INIT := 0, (*End of initialization - Transfer after other configuration parameters*)
		mcAFASPTCTO_ST_OF_INIT := 1 (*Start of initialization - Transfer before other configuration parameters*)
		);
	McCfgAxFeatAcpSptChartType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_AX_FEAT_ACP_SPT_CHART*)
		ACOPOSSptChartReference : McCfgReferenceType; (*Name of the ACOPOS spt chart*)
		TransferOrder : McAFASPTCTranOrdEnum; (*Transfer order in reference to other configuration parameters*)
	END_STRUCT;
END_TYPE