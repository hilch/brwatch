TYPE
	McCfgString250Type : STRING[250]; (**)
	McCfgTypeEnum :
		( (*Configuration system enum*)
		mcCFG_NONE := 0, (*None -*)
		mcCFG_HW_MODULE := 5, (*Associated with data type McCfgHwModuleSpecificsType*)
		mcCFG_CFG_ELEMENTS := 6, (*Associated with data type McCfgElementsType*)
		mcCFG_MMCFG := 10, (*Associated with data type McCfgMMCfgType*)
		mcCFG_OBJ_HIER_GCS := 210, (*Associated with data type McCfgObjHierGCSType*)
		mcCFG_OBJ_HIER := 200, (*Associated with data type McCfgObjHierType*)
		mcCFG_WS := 800, (*Associated with data type McCfgWorkspaceType*)
		mcCFG_TOOLTBL := 900, (*Tooltable -*)
		mcCFG_FRMTBL := 1000, (*Associated with data type McCfgFrmTblType*)
		mcCFG_DYNPARTBL := 1100, (*Associated with data type McCfgDynParTblType*)
		mcCFG_TOOL := 1300, (*Tool -*)
		mcCFG_LIMSET_LIN := 1411, (*Associated with data type McCfgLimSetLinType*)
		mcCFG_LIMSET_ROT := 1412, (*Associated with data type McCfgLimSetRotType*)
		mcCFG_CAMLST := 1500, (*Associated with data type McCfgCamLstType*)
		mcCFG_PROC_POINT := 1601, (*Associated with data type McCfgProcPointType*)
		mcCFG_PROC_TRAK_POS := 1602, (*Associated with data type McCfgProcPtTrakPosType*)
		mcCFG_PROC_TRAK_BARRIER := 1603, (*Associated with data type McCfgProcPtTrakBarrierType*)
		mcCFG_PROC_TRAK_MON := 1604, (*Associated with data type McCfgProcPtTrakMonType*)
		mcCFG_TRK_PATH := 1700, (*Associated with data type McCfgTrkPathType*)
		mcCFG_TRK_PATH_SCN := 1701, (*Associated with data type McCfgTrkPathScnType*)
		mcCFG_TRK_PATH_AUT_CRT_COND := 1702, (*Associated with data type McCfgTrkPathAutCrtCondType*)
		mcCFG_PICK_CORE := 2100, (*Associated with data type MpCfgPickCoreType*)
		mcCFG_PICK_REG := 2110, (*Associated with data type MpCfgPickRegType*)
		mcCFG_PICK_REG_SCN := 2111, (*Associated with data type MpCfgPickRegScnType*)
		mcCFG_PICK_OBJ_LST := 2120, (*Associated with data type MpCfgPickObjLstType*)
		mcCFG_AX := 10000, (*Associated with data type McCfgAxType*)
		mcCFG_AX_BASE_TYP := 10011, (*Associated with data type McCfgAxBaseTypType*)
		mcCFG_AX_MOVE_LIM := 10012, (*Associated with data type McCfgAxMoveLimType*)
		mcCFG_AX_FEAT_CAM_AUT := 10101, (*AxisFeatureCamAut -*)
		mcCFG_AX_FEAT_PROF_GEN := 10102, (*Associated with data type McCfgAxFeatProfGenType*)
		mcCFG_AX_FEAT_PG_JERK_FLTR := 10109, (*Associated with data type McCfgAxFeatPgJerkFltrType*)
		mcCFG_AX_FEAT_PG_ZERO_VIB_FLTR := 10110, (*Associated with data type McCfgAxFeatPgZeroVibFltrType*)
		mcCFG_AX_FEAT_PG_CPLG_SET := 10111, (*Associated with data type McCfgAxFeatPgCplgSetType*)
		mcCFG_AX_FEAT_DIG_CAM_SW := 10103, (*Associated with data type McCfgAxFeatDigCamSwType*)
		mcCFG_AX_FEAT_DIG_OUT := 10107, (*Associated with data type McCfgAxFeatDigOutType*)
		mcCFG_AX_FEAT_CAM_LST := 11102, (*AxisFeatureCamList -*)
		mcCFG_AX_FEAT_ALT_VAL_SRC := 10104, (*Associated with data type McCfgAxFeatAltValSrcType*)
		mcCFG_AX_FEAT_BRK := 10105, (*Associated with data type McCfgAxFeatBrkType*)
		mcCFG_AX_FEAT_MECH_DEV_COMP := 10106, (*Associated with data type McCfgAxFeatMechDevCompType*)
		mcCFG_AX_FEAT_ACP_NETW_ERR_REAC := 10108, (*Associated with data type McCfgAxFeatAcpNetwErrReacType*)
		mcCFG_AX_FEAT_ACP_CYC_DAT_PROC := 10112, (*Associated with data type McCfgAxFeatAcpCycDatProcType*)
		mcCFG_AX_FEAT_SDO_PAR_TAB := 10113, (*Associated with data type McCfgAxFeatSdoParTabType*)
		mcCFG_SDO_PAR_TAB := 1800, (*Associated with data type McCfgSdoParTabType*)
		mcCFG_MOT_SYN := 10500, (*Associated with data type McCfgMotSynType*)
		mcCFG_MOT_INDUCT := 10501, (*Associated with data type McCfgMotInductType*)
		mcCFG_MOT_SYN_AMC := 10502, (*Associated with data type McCfgMotSynAmcType*)
		mcCFG_MOT_BLDC := 10503, (*Associated with data type McCfgMotBLDCType*)
		mcCFG_MOT_STEP := 10504, (*Associated with data type McCfgMotStepType*)
		mcCFG_ACP_PL_IN_CARD_ENC := 10510, (*Associated with data type McCfgAcpPlInCrdEncType*)
		mcCFG_ACP_MUL_PL_IN_CARD_ENC := 10511, (*Associated with data type McCfgAcpMulPlInCrdEncType*)
		mcCFG_ACP_P3_PL_IN_CARD_ENC := 10512, (*Associated with data type McCfgAcpP3PlInCrdEncType*)
		mcCFG_ACP_P3_SNG_PL_IN_CARD_ENC := 10513, (*Associated with data type McCfgAcpP3SngPlInCrdEncType*)
		mcCFG_ACP_PL_IN_CARD_IO := 10514, (*Associated with data type McCfgAcpPlInCrdIOType*)
		mcCFG_BRK_RES := 10530, (*Associated with data type McCfgBrkResType*)
		mcCFG_VIBR_UNIT := 10540, (*Associated with data type McCfgVibrUnitType*)
		mcCFG_ACP_AX := 11000, (*Associated with data type McCfgAcpAxType*)
		mcCFG_ACP_AX_REF := 11011, (*Associated with data type McCfgAcpAxRefType*)
		mcCFG_ACP_MECH_ELM := 11012, (*Associated with data type McCfgAcpMechElmType*)
		mcCFG_ACP_ENC_LINK := 11013, (*Associated with data type McCfgAcpEncLinkType*)
		mcCFG_ACP_CTRL := 11014, (*Associated with data type McCfgAcpCtrlType*)
		mcCFG_ACP_SPD_CTRL := 11022, (*Associated with data type McCfgAcpSpdCtrlType*)
		mcCFG_ACP_HOME := 11015, (*Associated with data type McCfgAcpHomeType*)
		mcCFG_ACP_STOP_REAC := 11016, (*Associated with data type McCfgAcpStopReacType*)
		mcCFG_ACP_MOVE_ERR_LIM := 11017, (*Associated with data type McCfgAcpMoveErrLimType*)
		mcCFG_ACP_JERK_FLTR := 11018, (*Associated with data type McCfgAcpJerkFltrType*)
		mcCFG_ACP_DIG_IN := 11019, (*Associated with data type McCfgAcpDigInType*)
		mcCFG_ACP_SIM := 11020, (*Associated with data type McCfgAcpSimType*)
		mcCFG_ACP_AX_FEAT := 11021, (*Associated with data type McCfgAcpAxFeatType*)
		mcCFG_ACP_ZERO_VIB_FLTR := 11023, (*Associated with data type McCfgAcpZeroVibFltrType*)
		mcCFG_ACP_AUX_PWR_SUP_MOD := 11030, (*Associated with data type McCfgAcpAuxPwrSupModType*)
		mcCFG_ACP_PSM_PWR_SEC := 11031, (*Associated with data type McCfgAcpPSMPwrSecType*)
		mcCFG_ACP_PWR_SUP := 11040, (*Associated with data type McCfgAcpPwrSupType*)
		mcCFG_ACP_MOD := 11041, (*Associated with data type McCfgAcpModType*)
		mcCFG_ACP_ENC := 11045, (*Associated with data type McCfgAcpEncType*)
		mcCFG_ACP_VIRT_AX := 11050, (*Associated with data type McCfgAcpVirtAxType*)
		mcCFG_ACP_VIRT_AX_REF := 11051, (*Associated with data type McCfgAcpVirtAxRefType*)
		mcCFG_ACP_VIRT_HOME := 11052, (*Associated with data type McCfgAcpVirtHomeType*)
		mcCFG_ACP_VIRT_JERK_FLTR := 11053, (*Associated with data type McCfgAcpVirtJerkFltrType*)
		mcCFG_ACP_VIRT_AX_FEAT := 11054, (*Associated with data type McCfgAcpVirtAxFeatType*)
		mcCFG_ACP_VIRT_ZERO_VIB_FLTR := 11055, (*Associated with data type McCfgAcpVirtZeroVibFltrType*)
		mcCFG_ACP_CH_FEAT := 11060, (*Associated with data type McCfgAcpChFeatType*)
		mcCFG_ACP_EXT_ENC_AX := 11070, (*Associated with data type McCfgAcpExtEncAxType*)
		mcCFG_ACP_EXT_ENC_AX_REF := 11071, (*Associated with data type McCfgAcpExtEncAxRefType*)
		mcCFG_ACP_EXT_ENC_AX_ENC_LINK := 11072, (*Associated with data type McCfgAcpExtEncAxEncLinkType*)
		mcCFG_ACP_EXT_ENC_AX_MECH_ELM := 11073, (*Associated with data type McCfgAcpExtEncAxMechElmType*)
		mcCFG_ACP_EXT_ENC_AX_HOME := 11074, (*Associated with data type McCfgAcpExtEncAxHomeType*)
		mcCFG_AX_FEAT_A_IN := 11103, (*Associated with data type McCfgAxFeatAInType*)
		mcCFG_AX_FEAT_ACP_PAR_TBL := 11104, (*Associated with data type McCfgAxFeatAcpParTblType*)
		mcCFG_AX_FEAT_ACP_SPT_CHART := 11105, (*Associated with data type McCfgAxFeatAcpSptChartType*)
		mcCFG_ACP_SPT_CHART := 11120, (**)
		mcCFG_PURE_V_AX := 12000, (*Associated with data type McCfgPureVAxType*)
		mcCFG_PURE_V_AX_REF := 12011, (*Associated with data type McCfgPureVAxRefType*)
		mcCFG_PURE_V_AX_HOME := 12012, (*Associated with data type McCfgPureVAxHomeType*)
		mcCFG_PURE_V_AX_JERK_FLTR := 12013, (*Associated with data type McCfgPureVAxJerkFltrType*)
		mcCFG_PURE_V_AX_FEAT := 12014, (*Associated with data type McCfgPureVAxFeatType*)
		mcCFG_PURE_V_AX_MECH_ELM := 12015, (*Associated with data type McCfgPureVAxMechElmType*)
		mcCFG_PURE_V_AX_ENC_LINK := 12016, (*Associated with data type McCfgPureVAxEncLinkType*)
		mcCFG_PURE_V_AX_ENC_LINK_POS_FLT := 12027, (*Associated with data type McCfgPureVAxEncLinkPosFltrType*)
		mcCFG_PURE_V_AX_DIG_IN := 12020, (*Associated with data type McCfgPureVAxDigInType*)
		mcCFG_PURE_V_AX_CTRL := 12017, (*Associated with data type McCfgPureVAxCtrlType*)
		mcCFG_PURE_V_AX_STOP_REAC := 12018, (*Associated with data type McCfgPureVAxStopReacType*)
		mcCFG_PURE_V_AX_MOVE_ERR_LIM := 12019, (*Associated with data type McCfgPureVAxMoveErrLimType*)
		mcCFG_PURE_V_AX_STAT_IN := 12021, (*Associated with data type McCfgPureVAxStatInType*)
		mcCFG_PURE_V_AX_CTRL_OUT := 12022, (*Associated with data type McCfgPureVAxCtrlOutType*)
		mcCFG_PURE_V_AX_SIM := 12023, (*Associated with data type McCfgPureVAxSimType*)
		mcCFG_PURE_V_AX_MOD := 12024, (*Associated with data type McCfgPureVAxModType*)
		mcCFG_PURE_V_AX_ZERO_VIB_FLTR := 12025, (*Associated with data type McCfgPureVAxZeroVibFltrType*)
		mcCFG_PURE_V_AX_ENC_SET := 12116, (*Associated with data type McCfgPureVAxEncSetType*)
		mcCFG_PURE_V_AX_POS_ACT_VAL := 12126, (*Associated with data type McCfgPureVAxPosActValType*)
		mcCFG_PURE_V_AX_STAT_IN_DS402 := 12121, (*Associated with data type McCfgPureVAxStatInDs402Type*)
		mcCFG_PURE_V_AX_CTRL_OUT_DS402C := 12122, (*Associated with data type McCfgPureVAxCtrlOutDs402CSPType*)
		mcCFG_PURE_V_AX_CTRL_OUT_DS402V := 12132, (*Associated with data type McCfgPureVAxCtrlOutDs402VLType*)
		mcCFG_PURE_V_AX_VEL_ACT_VAL := 12136, (*Associated with data type McCfgPureVAxVelActValType*)
		mcCFG_STP_AX := 13000, (*Associated with data type McCfgStpAxType*)
		mcCFG_STP_AX_REF := 13011, (*Associated with data type McCfgStpAxRefType*)
		mcCFG_STP_AX_MECH_ELM := 13012, (*Associated with data type McCfgStpAxMechElmType*)
		mcCFG_STP_AX_MOT := 13013, (*Associated with data type McCfgStpAxMotType*)
		mcCFG_STP_AX_ENC_LINK := 13014, (*Associated with data type McCfgStpAxEncLinkType*)
		mcCFG_STP_AX_ENC_LINK_POS_FLTR := 13025, (*Associated with data type McCfgStpAxEncLinkPosFltrType*)
		mcCFG_STP_AX_CTRL := 13015, (*Associated with data type McCfgStpAxCtrlType*)
		mcCFG_STP_AX_HOME := 13016, (*Associated with data type McCfgStpAxHomeType*)
		mcCFG_STP_AX_STOP_REAC := 13017, (*Associated with data type McCfgStpAxStopReacType*)
		mcCFG_STP_AX_MOVE_ERR_LIM := 13018, (*Associated with data type McCfgStpAxMoveErrLimType*)
		mcCFG_STP_AX_JERK_FLTR := 13019, (*Associated with data type McCfgStpAxJerkFltrType*)
		mcCFG_STP_AX_ZERO_VIB_FLTR := 13024, (*Associated with data type McCfgStpAxZeroVibFltrType*)
		mcCFG_STP_AX_DIG_IN := 13020, (*Associated with data type McCfgStpAxDigInType*)
		mcCFG_STP_AX_DIG_OUT := 13021, (*Associated with data type McCfgStpAxDigOutType*)
		mcCFG_STP_AX_SIM := 13023, (*Associated with data type McCfgStpAxSimType*)
		mcCFG_STP_AX_FEAT := 13022, (*Associated with data type McCfgStpAxFeatType*)
		mcCFG_STP_ENC := 13100, (*Associated with data type McCfgStpEncType*)
		mcCFG_STP_MOD := 13210, (*Associated with data type McCfgStpModType*)
		mcCFG_DS402_AX := 13500, (*Associated with data type McCfgDS402AxType*)
		mcCFG_DS402_INV_AX := 13501, (*Associated with data type McCfgDS402InvAxType*)
		mcCFG_DS402_AX_REF := 13511, (*Associated with data type McCfgDS402AxRefType*)
		mcCFG_DS402_AX_MECH_ELM := 13512, (*Associated with data type McCfgDS402AxMechElmType*)
		mcCFG_DS402_AX_ENC_SET := 13513, (*Associated with data type McCfgDS402AxEncSetType*)
		mcCFG_DS402_AX_POS_ACT_VAL_RNG := 13514, (*Associated with data type McCfgDS402AxPosActValRngType*)
		mcCFG_DS402_AX_CYC_SET_VAL_MOD := 13515, (*Associated with data type McCfgDS402AxCycSetValModType*)
		mcCFG_DS402_AX_FEAT := 13516, (*Associated with data type McCfgDS402AxFeatType*)
		mcCFG_ACP_INV_AX := 13600, (*Associated with data type McCfgAcpInvAxType*)
		mcCFG_ACP_INV_AX_REF := 13611, (*Associated with data type McCfgAcpInvAxRefType*)
		mcCFG_ACP_INV_AX_MECH_ELM := 13612, (*Associated with data type McCfgAcpInvAxMechElmType*)
		mcCFG_ACP_INV2_AX := 14600, (*Associated with data type McCfgAcpInv2AxType*)
		mcCFG_ACP_INV2_MOD := 14700, (*Associated with data type McCfgAcpInv2ModType*)
		mcCFG_ACP_INV2_ENC_AND_IO := 14710, (*Associated with data type McCfgAcpInv2EncAndIOsType*)
		mcCFG_ACP_INV2_AX_REF := 14611, (*Associated with data type McCfgAcpInv2AxRefType*)
		mcCFG_ACP_INV2_AX_MECH_ELM := 14612, (*Associated with data type McCfgAcpInv2MechElmType*)
		mcCFG_ACP_INV2_ENC_LINK := 14014, (*Associated with data type McCfgAcpInv2EncLinkType*)
		mcCFG_ACP_INV2_ENC_LINK_POS_FLT := 14025, (*Associated with data type McCfgAcpInv2EncLinkPosFltrType*)
		mcCFG_ACP_INV2_CTRL := 14015, (*Associated with data type McCfgAcpInv2CtrlType*)
		mcCFG_ACP_INV2_HOME := 14016, (*Associated with data type McCfgAcpInv2HomeType*)
		mcCFG_ACP_INV2_STOP_REAC := 14017, (*Associated with data type McCfgAcpInv2StopReacType*)
		mcCFG_ACP_INV2_MOVE_ERR_LIM := 14018, (*Associated with data type McCfgAcpInv2MoveErrLimType*)
		mcCFG_ACP_INV2_JERK_FLTR := 14019, (*Associated with data type McCfgAcpInv2JerkFltrType*)
		mcCFG_ACP_INV2_ZERO_VIB_FLTR := 14024, (*Associated with data type McCfgAcpInv2ZeroVibFltrType*)
		mcCFG_ACP_INV2_DIG_IN := 14020, (*Associated with data type McCfgAcpInv2DigInType*)
		mcCFG_ACP_INV2_AX_EXCEP_STOP_BEH := 14720, (*Associated with data type McCfgAcpInv2ExcptStopBehType*)
		mcCFG_ACP_INV2_AX_FEAT := 14022, (*Associated with data type McCfgAcpInv2AxFeatType*)
		mcCFG_AXGRP_ADMIN := 20000, (*Associated with data type McCfgAxGrpAdminType*)
		mcCFG_AXGRP_FEAT_HOME_ORD := 20101, (*Associated with data type McCfgAxGrpFeatHomeOrdType*)
		mcCFG_AXGRP_FEAT_PWR_ON_ORD := 20102, (*Associated with data type McCfgAxGrpFeatPwrOnOrdType*)
		mcCFG_AXGRP_FEAT_EX_SNG_AX := 20103, (*Associated with data type McCfgAxGrpFeatExSngAxType*)
		mcCFG_AXGRP_FEAT_SHR_BRK_SIG := 20104, (*Associated with data type McCfgAxGrpFeatShrBrkSigType*)
		mcCFG_AXGRP_PATHGEN := 21000, (*Associated with data type McCfgAxGrpPathGenType*)
		mcCFG_AXGRP_PATHGEN_BASE_SET := 21013, (*Associated with data type McCfgAxGrpPathGenBaseSetType*)
		mcCFG_AXGRP_FEAT_PRG := 21101, (*Associated with data type McCfgAxGrpFeatPrgType*)
		mcCFG_AXGRP_FEAT_COMP := 21102, (*Associated with data type McCfgAxGrpFeatCompType*)
		mcCFG_AXGRP_FEAT_CDC := 21103, (*Associated with data type McCfgAxGrpFeatCdcType*)
		mcCFG_AXGRP_FEAT_FF := 21104, (*Associated with data type McCfgAxGrpFeatFfType*)
		mcCFG_AXGRP_FEAT_FF_M := 21204, (*Associated with data type McCfgAxGrpFeatFfModeType*)
		mcCFG_AXGRP_FEAT_FRM_HIER_STD := 21105, (*Associated with data type McCfgAxGrpFeatFrmHierStdType*)
		mcCFG_AXGRP_FEAT_FRM_HIER_CUS := 21106, (*Associated with data type McCfgAxGrpFeatFrmHierCusType*)
		mcCFG_AXGRP_FEAT_JOG := 21107, (*Associated with data type McCfgAxGrpFeatJogType*)
		mcCFG_AXGRP_FEAT_LAH := 21108, (*Associated with data type McCfgAxGrpFeatLahType*)
		mcCFG_AXGRP_FEAT_MFUN := 21109, (*Associated with data type McCfgAxGrpFeatMFunType*)
		mcCFG_AXGRP_FEAT_MON_ELEM := 21110, (*Associated with data type McCfgAxGrpFeatMonElemType*)
		mcCFG_AXGRP_FEAT_MP_LOG := 21111, (*Associated with data type McCfgAxGrpFeatMpLogType*)
		mcCFG_AXGRP_FEAT_PATH_DEF := 21112, (*Associated with data type McCfgAxGrpFeatPathDefType*)
		mcCFG_AXGRP_FEAT_PRG_SIM := 21113, (*Associated with data type McCfgAxGrpFeatPrgSimType*)
		mcCFG_AXGRP_FEAT_SPINDLES := 21114, (*Associated with data type McCfgAxGrpFeatSpindlesType*)
		mcCFG_AXGRP_FEAT_TOOL := 21115, (*Associated with data type McCfgAxGrpFeatToolType*)
		mcCFG_AXGRP_FEAT_WSM := 21116, (*Associated with data type McCfgAxGrpFeatWsmType*)
		mcCFG_AXGRP_FEAT_EX_PATH_AX := 21117, (*Associated with data type McCfgAxGrpFeatExPathAxType*)
		mcCFG_AXGRP_FEAT_PROBE := 21118, (*Associated with data type McCfgAxGrpFeatProbeType*)
		mcCFG_AXGRP_FEAT_SIG := 21119, (*Associated with data type McCfgAxGrpFeatSigType*)
		mcCFG_AXGRP_FEAT_2D_COMP := 21120, (*Associated with data type McCfgAxGrpFeat2DCompType*)
		mcCFG_AXGRP_FEAT_3D_COMP := 21121, (*Associated with data type McCfgAxGrpFeat3DCompType*)
		mcCFG_AXGRP_FEAT_PATH_PREVIEW := 21122, (*Associated with data type McCfgAxGrpFeatPathPreviewType*)
		mcCFG_AXGRP_FEAT_TAN_TOOL := 21124, (*Associated with data type McCfgAxGrpFeatTanToolType*)
		mcCFG_AXGRP_FEAT_REV_MOVE := 21125, (*Associated with data type McCfgAxGrpFeatRevMoveType*)
		mcCFG_AXGRP_FEAT_TRK := 21126, (*Associated with data type McCfgAxGrpFeatTrkType*)
		mcCFG_AXGRP_FEAT_PIPE_CUT := 21127, (*Associated with data type McCfgAxGrpPipeCutType*)
		mcCFG_AXGRP_FEAT_TRKFRM_SEL := 21128, (*Associated with data type McCfgAxGrpFeatTrkFrmSelType*)
		mcCFG_AXGRP_FEAT_TRKFRM_SEL_AREA := 21129, (*Associated with data type McCfgAxGrpFeatTrkFrmSelAreaType*)
		mcCFG_ASM := 31000, (*Associated with data type McCfgAsmType*)
		mcCFG_ASM_COGG_COMP := 31001, (*Associated with data type McCfgAsmCoggComp*)
		mcCFG_ASM_ELONG_COMP := 31002, (*Associated with data type McCfgAsmElongComp*)
		mcCFG_ASM_DEFAULT_CTRL_PARAM := 31003, (*Associated with data type McCfgAsmDefCtrlParam*)
		mcCFG_ASM_ADD_CTRL_PARAM := 31004, (*Associated with data type McCfgAsmAddCtrlParam*)
		mcCFG_ASM_STOP_REACTION := 31005, (*Associated with data type McCfgAsmStopReaction*)
		mcCFG_ASM_SPEED_FILTER := 31006, (*Associated with data type McCfgAsmSpeedFilt*)
		mcCFG_ASM_MAGNET_PLATE := 31007, (*Associated with data type McCfgAsmMagnetPlate*)
		mcCFG_ASM_SCOPE_OF_ERR_REAC := 31008, (*Associated with data type McCfgAsmScopeOfErrReaction*)
		mcCFG_ASM_SH_IDENT_TIME := 31009, (*Associated with data type McCfgAsmShIdentTime*)
		mcCFG_ASM_POS_CTRL_LAG_MON := 31010, (*Associated with data type McCfgAsmPosCtrlLagMonitor*)
		mcCFG_ASM_DIVERTER := 31011, (*Associated with data type McCfgAsmDiverter*)
		mcCFG_ASM_STRATEGY := 31012, (*Associated with data type McCfgAsmColAvoidStrategy*)
		mcCFG_ASM_ADJUSTMENT_MODE := 31013, (*Associated with data type McCfgAsmColAvoidAdjustMode*)
		mcCFG_ASM_DIST_RESERVES := 31014, (*Associated with data type McCfgAsmDistReserves*)
		mcCFG_ASM_VIRT_SH_SCOPE := 31015, (*Associated with data type McCfgAsmColAvoidVirtShScope*)
		mcCFG_ASM_BR_SH_DATA := 31016, (*Associated with data type McCfgAsmBaReShDatType*)
		mcCFG_ASM_FEAT_CPLG := 31101, (*Associated with data type McCfgAsmFeatCplgType*)
		mcCFG_ASM_FEAT_SIM_SH_DEF := 31102, (*Associated with data type McCfgAsmFeatSimShDefType*)
		mcCFG_ASM_FEAT_SEC_TRACE := 31103, (*Associated with data type McCfgAsmFeatSecTraceType*)
		mcCFG_ASM_FEAT_SH_AUT_ATT := 31104, (*Associated with data type McCfgAsmFeatShAutAttType*)
		mcCFG_ASM_FEAT_LOC_LIM := 31105, (*Associated with data type McCfgAsmFeatLocLimType*)
		mcCFG_ASM_FEAT_EX_SEG := 31106, (*Associated with data type McCfgAsmFeatExSegType*)
		mcCFG_ASM_FEAT_SH_SHP_REG := 31107, (*Associated with data type McCfgAsmFeatShShpRegType*)
		mcCFG_ASM_FEAT_SEG_GRP := 31108, (*Associated with data type McCfgAsmFeatSegGrpType*)
		mcCFG_ASM_FEAT_SNAP := 31109, (*Associated with data type McCfgAsmFeatSnapType*)
		mcCFG_SEC := 31300, (*Associated with data type McCfgSecType*)
		mcCFG_SEC_COMP := 31301, (*Associated with data type McCfgSecCompType*)
		mcCFG_SEC_SUB := 31302, (*Associated with data type McCfgSecSubType*)
		mcCFG_SEC_COMP_POS := 31303, (*Associated with data type McCfgSecCompPosType*)
		mcCFG_SH_STEREO_TYP := 31400, (*Associated with data type McCfgShStereoTypType*)
		mcCFG_SH_JERK_FILT := 31401, (*Associated with data type McCfgShJerkFiltType*)
		mcCFG_SH_STATE_TRANS := 31402, (*Associated with data type McCfgShStateTransType*)
		mcCFG_SH_ZERO_VIB_FILTER := 31403, (*Associated with data type McCfgShZeroVibFilterType*)
		mcCFG_SEG := 31500, (*Associated with data type McCfgSegType*)
		mcCFG_VIRT_SEG := 31501, (*Associated with data type McCfgVirtSegType*)
		mcCFG_MS_CUS_STD := 50001, (*Associated with data type McCfgMSCusStdType*)
		mcCFG_MS_2AX_CNC_XY := 51201, (*Associated with data type McCfgMS2AxCncXYType*)
		mcCFG_MS_2AX_CNC_XZ := 51202, (*Associated with data type McCfgMS2AxCncXZType*)
		mcCFG_MS_2AX_CNC_YZ := 51203, (*Associated with data type McCfgMS2AxCncYZType*)
		mcCFG_MS_3AX_CNC_XYZ := 51301, (*Associated with data type McCfgMS3AxCncXYZType*)
		mcCFG_MS_3AX_CNC_XZC := 51302, (*Associated with data type McCfgMS3AxCncXZCType*)
		mcCFG_MS_3AX_CNC_XZB := 51303, (*Associated with data type McCfgMS3AxCncXZBType*)
		mcCFG_MS_4AX_CNC_XYZA := 51400, (*Associated with data type McCfgMS4AxCncXYZAType*)
		mcCFG_MS_4AX_CNC_XYZB := 51401, (*Associated with data type McCfgMS4AxCncXYZBType*)
		mcCFG_MS_4AX_CNC_XYZC := 51402, (*Associated with data type McCfgMS4AxCncXYZCType*)
		mcCFG_MS_5AX_CNC_XYZBA := 51502, (*Associated with data type McCfgMS5AxCncXYZBAType*)
		mcCFG_MS_5AX_CNC_XYZBC := 51503, (*Associated with data type McCfgMS5AxCncXYZBCType*)
		mcCFG_MS_5AX_CNC_XYZCA := 51504, (*Associated with data type McCfgMS5AxCncXYZCAType*)
		mcCFG_MS_5AX_CNC_XYZCB := 51505, (*Associated with data type McCfgMS5AxCncXYZCBType*)
		mcCFG_MS_6AX_CNC_ZXYBCA := 51603, (*Associated with data type McCfgMS6AxCncZXYBCAType*)
		mcCFG_MS_3AX_SCARA_A := 52042, (*Associated with data type McCfgMS3AxScaraAType*)
		mcCFG_MS_4AX_SCARA_A := 52041, (*Associated with data type McCfgMS4AxScaraAType*)
		mcCFG_MS_2AX_DELTA_A := 52121, (*Associated with data type McCfgMS2AxDeltaAType*)
		mcCFG_MS_2AX_DELTA_B := 52122, (*Associated with data type McCfgMS2AxDeltaBType*)
		mcCFG_MS_3AX_DELTA_A := 52131, (*Associated with data type McCfgMS3AxDeltaAType*)
		mcCFG_MS_3AX_DELTA_XZB := 52132, (*Associated with data type McCfgMS3AxDeltaXZBType*)
		mcCFG_MS_3AX_DELTA_B := 52133, (*Associated with data type McCfgMS3AxDeltaBType*)
		mcCFG_MS_3AX_DELTA_XZC := 52134, (*Associated with data type McCfgMS3AxDeltaXZCType*)
		mcCFG_MS_4AX_DELTA_A := 52141, (*Associated with data type McCfgMS4AxDeltaAType*)
		mcCFG_MS_4AX_DELTA_B := 52142, (*Associated with data type McCfgMS4AxDeltaBType*)
		mcCFG_MS_4AX_DELTA_C := 52143, (*Associated with data type McCfgMS4AxDeltaCType*)
		mcCFG_MS_5AX_DELTA_A := 52151, (*Associated with data type McCfgMS5AxDeltaAType*)
		mcCFG_MS_3AX_ROB_A := 52301, (*Associated with data type McCfgMS3AxRobAType*)
		mcCFG_MS_4AX_ROB_A := 52401, (*Associated with data type McCfgMS4AxRobAType*)
		mcCFG_MS_4AX_ROB_B := 52402, (*Associated with data type McCfgMS4AxRobBType*)
		mcCFG_MS_5AX_ROB_A := 52501, (*Associated with data type McCfgMS5AxRobAType*)
		mcCFG_MS_5AX_ROB_B := 52502, (*Associated with data type McCfgMS5AxRobBType*)
		mcCFG_MS_6AX_ROB_A := 52601, (*Associated with data type McCfgMS6AxRobAType*)
		mcCFG_MS_6AX_ROB_B := 52602, (*Associated with data type McCfgMS6AxRobBType*)
		mcCFG_MS_6AX_ROB_C := 52603, (*Associated with data type McCfgMS6AxRobCType*)
		mcCFG_ACP_INV2_MOT := 14500 (*Associated with data type McCfgAcpInv2MotType*)
		);
	McCfgUnboundedArrayType : STRUCT (*General purpose datatype*)
		NumberOfElements : UDINT;
		DataAddress : UDINT;
		NumberOfArrayElements : UDINT;
	END_STRUCT;
	McCfgReferenceType : STRUCT (*General purpose datatype*)
		Name : STRING[250];
		ConfigType : McCfgTypeEnum;
	END_STRUCT;
	McCfgHwModuleSpecificsType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_HW_MODULE*)
		Name : STRING[250]; (*Order number*)
	END_STRUCT;
	McCfgElementsType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_CFG_ELEMENTS*)
		ConfigSystemElement : McCfgUnboundedArrayType; (*Configuration System element %I (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McMMCProcProcTskCEnum :
		( (*Cyclic task class for command processing*)
		mcMMCPPTC_CYC_1 := 1, (*Cyclic #1 - Task class 1*)
		mcMMCPPTC_CYC_2 := 2 (*Cyclic #2 - Task class 2*)
		);
	McMMCPECOAEnum :
		( (*Axis selector setting*)
		mcMMCPECOA_NOT_USE := 0, (*Not used -*)
		mcMMCPECOA_USE := 1 (*Used - Explicit computation order of axes is used*)
		);
	McMMCPECOAUseAxCompB4AllOthType : STRUCT (*Explicit computation order for axis components that are computed before all others*)
		AxisReference : McCfgUnboundedArrayType; (*Name of the referenced axis component (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McMMCPECOAUseAxCompAShType : STRUCT (*Explicit computation order for axis components that are computed after shuttles*)
		AxisReference : McCfgUnboundedArrayType; (*Name of the referenced axis component (Connect array of type McCfgReferenceType)*)
	END_STRUCT;
	McMMCPECOAUseType : STRUCT (*Type mcMMCPECOA_USE settings*)
		AxesComputedBeforeAllOthers : McMMCPECOAUseAxCompB4AllOthType; (*Explicit computation order for axis components that are computed before all others*)
		AxesComputedAfterShuttles : McMMCPECOAUseAxCompAShType; (*Explicit computation order for axis components that are computed after shuttles*)
	END_STRUCT;
	McMMCPECOAType : STRUCT (*Defines the order of computation of axis components explicitly*)
		Type : McMMCPECOAEnum; (*Axis selector setting*)
		Used : McMMCPECOAUseType; (*Type mcMMCPECOA_USE settings*)
	END_STRUCT;
	McMMCProcExpCompOrdType : STRUCT (*Defines the order of computation of components explicitly*)
		Axis : McMMCPECOAType; (*Defines the order of computation of axis components explicitly*)
	END_STRUCT;
	McMMCProcSymMulEnum :
		( (*Symmetric multiprocessing selector setting*)
		mcMMCPSM_NOT_USE := 0, (*Not used -*)
		mcMMCPSM_USE := 1 (*Used - Symmetric multiprocessing is used*)
		);
	McMMCProcSymMulType : STRUCT (*Symmetric multiprocessing (SMP) enhances performance by efficiently distributing computational tasks across multiple CPU cores.*)
		Type : McMMCProcSymMulEnum; (*Symmetric multiprocessing selector setting*)
	END_STRUCT;
	McMMCProcType : STRUCT
		ProcessingTaskClass : McMMCProcProcTskCEnum; (*Cyclic task class for command processing*)
		ExplicitComputationOrder : McMMCProcExpCompOrdType; (*Defines the order of computation of components explicitly*)
		SymmetricMultiprocessing : McMMCProcSymMulType; (*Symmetric multiprocessing (SMP) enhances performance by efficiently distributing computational tasks across multiple CPU cores.*)
	END_STRUCT;
	McMMCLogSelEnum :
		( (*Selective logging selector setting*)
		mcMMCLS_NOT_USE := 0, (*Not used -*)
		mcMMCLS_USE := 1 (*Used -*)
		);
	McCfgVisEnum :
		( (*Component state change logger entries (PLCopen state)*)
		mcCV_VIS := 0, (*Visible*)
		mcCV_SUP := 1 (*Suppressed*)
		);
	McMMCLogSelUseAdmCmdEnum :
		( (*Administrative commands selector setting*)
		mcMMCLSUAC_VIS := 0, (*Visible - This command group of logger entries will be visible*)
		mcMMCLSUAC_SUP := 1, (*Suppressed - This command group of logger entries will be suppressed*)
		mcMMCLSUAC_SEL := 2 (*Selective - Defines which sections of this command group should be visible*)
		);
	McMMCLogSelUseAdmCmdSelType : STRUCT (*Type mcMMCLSUAC_SEL settings*)
		CallerInstance : McCfgVisEnum; (*Logging of the caller instance entries*)
		ComponentResponse : McCfgVisEnum; (*Logging of component response entries*)
		AdditionalInformation : McCfgVisEnum; (*Logging of additional information entries*)
	END_STRUCT;
	McMMCLogSelUseAdmCmdType : STRUCT (*Administrative logger entries*)
		Type : McMMCLogSelUseAdmCmdEnum; (*Administrative commands selector setting*)
		Selective : McMMCLogSelUseAdmCmdSelType; (*Type mcMMCLSUAC_SEL settings*)
	END_STRUCT;
	McMMCLogSelUseMoveCmdEnum :
		( (*Movement commands selector setting*)
		mcMMCLSUMC_VIS := 0, (*Visible - This command group of logger entries will be visible*)
		mcMMCLSUMC_SUP := 1, (*Suppressed - This command group of logger entries will be suppressed*)
		mcMMCLSUMC_SEL := 2 (*Selective - Defines which sections of this command group should be visible*)
		);
	McMMCLogSelUseMoveCmdSelType : STRUCT (*Type mcMMCLSUMC_SEL settings*)
		CallerInstance : McCfgVisEnum; (*Logging of the caller instance entries*)
		ComponentResponse : McCfgVisEnum; (*Logging of component response entries*)
		AdditionalInformation : McCfgVisEnum; (*Logging of additional information entries*)
	END_STRUCT;
	McMMCLogSelUseMoveCmdType : STRUCT (*Movement logger entries*)
		Type : McMMCLogSelUseMoveCmdEnum; (*Movement commands selector setting*)
		Selective : McMMCLogSelUseMoveCmdSelType; (*Type mcMMCLSUMC_SEL settings*)
	END_STRUCT;
	McMMCLogSelUseInfoCmdEnum :
		( (*Informational commands selector setting*)
		mcMMCLSUIC_VIS := 0, (*Visible - This command group of logger entries will be visible*)
		mcMMCLSUIC_SUP := 1, (*Suppressed - This command group of logger entries will be suppressed*)
		mcMMCLSUIC_SEL := 2 (*Selective - Defines which sections of this command group should be visible*)
		);
	McMMCLogSelUseInfoCmdSelType : STRUCT (*Type mcMMCLSUIC_SEL settings*)
		CallerInstance : McCfgVisEnum; (*Logging of the caller instance entries*)
		ComponentResponse : McCfgVisEnum; (*Logging of component response entries*)
		AdditionalInformation : McCfgVisEnum; (*Logging of additional information entries*)
	END_STRUCT;
	McMMCLogSelUseInfoCmdType : STRUCT (*Status information logger entries*)
		Type : McMMCLogSelUseInfoCmdEnum; (*Informational commands selector setting*)
		Selective : McMMCLogSelUseInfoCmdSelType; (*Type mcMMCLSUIC_SEL settings*)
	END_STRUCT;
	McMMCLogSelUseSupSubcEEnum :
		( (*Suppress entries for internally invoked commands for subcomponents*)
		mcMMCLSUSSE_INACT := 0, (*Inactive - Entries for internally invoked commands for subcomponents are shown unless the base entries are suppressed*)
		mcMMCLSUSSE_ACT := 1 (*Active - Entries for internally invoked commands for subcomponents are suppressed*)
		);
	McMMCLogSelUseType : STRUCT (*Type mcMMCLS_USE settings*)
		StateChange : McCfgVisEnum; (*Component state change logger entries (PLCopen state)*)
		AdministrativeCommands : McMMCLogSelUseAdmCmdType; (*Administrative logger entries*)
		MovementCommands : McMMCLogSelUseMoveCmdType; (*Movement logger entries*)
		InformationalCommands : McMMCLogSelUseInfoCmdType; (*Status information logger entries*)
		SuppressSubcomponentEntries : McMMCLogSelUseSupSubcEEnum; (*Suppress entries for internally invoked commands for subcomponents*)
	END_STRUCT;
	McMMCLogSelType : STRUCT (*Define which logging areas should be visible*)
		Type : McMMCLogSelEnum; (*Selective logging selector setting*)
		Used : McMMCLogSelUseType; (*Type mcMMCLS_USE settings*)
	END_STRUCT;
	McMMCLogType : STRUCT
		Selective : McMMCLogSelType; (*Define which logging areas should be visible*)
	END_STRUCT;
	McMMCMcAcpDrvPLKCycPerParIDEnum :
		( (*Defines the number of POWERLINK cycles per ParID for parameter transfer*)
		mcMMCMPCPP_TWO := 2, (*Two - One ParID every second cycle*)
		mcMMCMPCPP_ONE := 1 (*One - One ParID every cycle*)
		);
	McMMCMcAcpDrvType : STRUCT (*ACOPOS driver configuration*)
		POWERLINKCyclesPerParID : McMMCMcAcpDrvPLKCycPerParIDEnum; (*Defines the number of POWERLINK cycles per ParID for parameter transfer*)
	END_STRUCT;
	McMcMMCDiSnEnum :
		( (*Snapshot selector setting*)
		mcMMCDS_NOT_USE := 0, (*Not used -*)
		mcMMCDS_USE := 1 (*Used -*)
		);
	McMcMMCDiSnUseFCompressionEnum :
		( (*Compression selector setting*)
		mcMMCDSUFC_NOT_USE := 0, (*Not used -*)
		mcMMCDSUFC_USE := 1 (*Used -*)
		);
	McMcMMCDiSnUseFCompressionType : STRUCT (*Compression*)
		Type : McMcMMCDiSnUseFCompressionEnum; (*Compression selector setting*)
	END_STRUCT;
	McMcMMCDiSnUseFLimEnum :
		( (*Limits selector setting*)
		mcMMCDSUFL_NOT_USE := 0, (*Not used -*)
		mcMMCDSUFL_USE := 1 (*Used -*)
		);
	McMcMMCDiSnUseFLimUseType : STRUCT (*Type mcMMCDSUFL_USE settings*)
		MaximumSize : UDINT; (*Maximum total size of snapshots (0 for unlimited) [KB]*)
		MaximumNumberOfSnapshots : UDINT; (*Maximum number of snapshots*)
	END_STRUCT;
	McMcMMCDiSnUseFLimType : STRUCT (*Limits*)
		Type : McMcMMCDiSnUseFLimEnum; (*Limits selector setting*)
		Used : McMcMMCDiSnUseFLimUseType; (*Type mcMMCDSUFL_USE settings*)
	END_STRUCT;
	McMcMMCDiSnUseFType : STRUCT
		FileDevice : STRING[250]; (*Name of file device*)
		DirectoryName : STRING[250]; (*Name of snapshot directory*)
		Compression : McMcMMCDiSnUseFCompressionType; (*Compression*)
		Limits : McMcMMCDiSnUseFLimType; (*Limits*)
	END_STRUCT;
	McMcMMCDiSnUseType : STRUCT (*Type mcMMCDS_USE settings*)
		Files : McMcMMCDiSnUseFType;
	END_STRUCT;
	McMcMMCDiSnType : STRUCT (*Snapshot configuration*)
		Type : McMcMMCDiSnEnum; (*Snapshot selector setting*)
		Used : McMcMMCDiSnUseType; (*Type mcMMCDS_USE settings*)
	END_STRUCT;
	McMMCDiagnosticsType : STRUCT
		Snapshot : McMcMMCDiSnType; (*Snapshot configuration*)
	END_STRUCT;
	McCfgMMCfgType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_MMCFG*)
		Processing : McMMCProcType;
		Logger : McMMCLogType;
		McAcpDrv : McMMCMcAcpDrvType; (*ACOPOS driver configuration*)
		Diagnostics : McMMCDiagnosticsType;
	END_STRUCT;
	McOHGCSOTypEnum :
		( (*Type selector setting*)
		mcOHGCSOT_CMPT := 0, (*Component - Object type component*)
		mcOHGCSOT_STD_FRM := 1 (*Standard frame - Object type standard frame*)
		);
	McCfgTransXYZType : STRUCT (*Translation parameters*)
		X : LREAL; (*Translation in X direction [measurement units]*)
		Y : LREAL; (*Translation in Y direction [measurement units]*)
		Z : LREAL; (*Translation in Z direction [measurement units]*)
	END_STRUCT;
	McCfgOrientType : STRUCT (*Orientation parameters*)
		Angle1 : LREAL; (*Rotation around the first coordinate axis of the rotation order [measurement units]*)
		Angle2 : LREAL; (*Rotation around the second coordinate axis of the rotation order [measurement units]*)
		Angle3 : LREAL; (*Rotation around the third coordinate axis of the rotation order [measurement units]*)
	END_STRUCT;
	McOHGCSOTCSVIECEnum :
		( (*Scene Viewer Object selector setting*)
		mcOHGCSOTCSVIEC_USE := 0 (*Used - Scene viewer object is used.*)
		);
	McOHGCSOTCSVIECUICmptEnum :
		( (*Component specific frames selector setting*)
		mcOHGCSOTCSVIECUIC_NOT_USE := 0, (*Not used - Not used*)
		mcOHGCSOTCSVIECUIC_AX_GRP := 1 (*Axes group - Component specific axes group frames*)
		);
	McOHGCSOTCSVIECUICmptAxGrpType : STRUCT (*Type mcOHGCSOTCSVIECUIC_AX_GRP settings*)
		FlangeSceneFile : STRING[250]; (*Name of scene file to add to this coordinate system. Without a path, the file must be located next to the auto-generated scene when the scene viewer opens.*)
	END_STRUCT;
	McOHGCSOTCSVIECUICmptType : STRUCT (*Include scene at a component specific coordinate system.*)
		Type : McOHGCSOTCSVIECUICmptEnum; (*Component specific frames selector setting*)
		AxesGroup : McOHGCSOTCSVIECUICmptAxGrpType; (*Type mcOHGCSOTCSVIECUIC_AX_GRP settings*)
	END_STRUCT;
	McOHGCSOTCSVIECUIType : STRUCT (*Scene files to be included in the automatically generated scene.*)
		SceneFile : STRING[250]; (*Name of scene file to add to this coordinate system. Without a path, the file must be located next to the auto-generated scene when the scene viewer opens.*)
		Component : McOHGCSOTCSVIECUICmptType; (*Include scene at a component specific coordinate system.*)
	END_STRUCT;
	McOHGCSOTCSVIECUseType : STRUCT (*Type mcOHGCSOTCSVIEC_USE settings*)
		IncludedScenes : McOHGCSOTCSVIECUIType; (*Scene files to be included in the automatically generated scene.*)
	END_STRUCT;
	McOHGCSOTCSVIECType : STRUCT
		Type : McOHGCSOTCSVIECEnum; (*Scene Viewer Object selector setting*)
		Used : McOHGCSOTCSVIECUseType; (*Type mcOHGCSOTCSVIEC_USE settings*)
	END_STRUCT;
	McOHGCSOTCType : STRUCT (*Type mcOHGCSOT_CMPT settings*)
		ComponentReference : McCfgReferenceType;
		Translation : McCfgTransXYZType; (*Translation parameters*)
		Orientation : McCfgOrientType; (*Orientation parameters*)
		SceneViewerObject : McOHGCSOTCSVIECType;
	END_STRUCT;
	McOHGCSOTCSVEnum :
		( (*Scene Viewer Object selector setting*)
		mcOHGCSOTCSV_USE := 0 (*Used - Scene viewer object is used.*)
		);
	McOHGCSOTCSVUseInclScnType : STRUCT (*Scene files to be included in the automatically generated scene.*)
		SceneFile : STRING[250]; (*Name of scene file to add to this coordinate system. Without a path, the file must be located next to the auto-generated scene when the scene viewer opens.*)
	END_STRUCT;
	McOHGCSOTCSVUseType : STRUCT (*Type mcOHGCSOTCSV_USE settings*)
		IncludedScenes : McOHGCSOTCSVUseInclScnType; (*Scene files to be included in the automatically generated scene.*)
	END_STRUCT;
	McOHGCSOTCSVType : STRUCT (*Settings for automatically generated scene.*)
		Type : McOHGCSOTCSVEnum; (*Scene Viewer Object selector setting*)
		Used : McOHGCSOTCSVUseType; (*Type mcOHGCSOTCSV_USE settings*)
	END_STRUCT;
	McOHGCSOTSFType : STRUCT (*Type mcOHGCSOT_STD_FRM settings*)
		FrameName : STRING[250]; (*Standard frame name*)
		Translation : McCfgTransXYZType; (*Translation parameters*)
		Orientation : McCfgOrientType; (*Orientation parameters*)
		SceneViewerObject : McOHGCSOTCSVType; (*Settings for automatically generated scene.*)
	END_STRUCT;
	McOHGCSOTypType : STRUCT (*Object type*)
		Type : McOHGCSOTypEnum; (*Type selector setting*)
		Component : McOHGCSOTCType; (*Type mcOHGCSOT_CMPT settings*)
		StandardFrame : McOHGCSOTSFType; (*Type mcOHGCSOT_STD_FRM settings*)
	END_STRUCT;
	McOHGCSObjType : STRUCT
		ParentObjectName : STRING[250]; (*Name of the parent object*)
		Type : McOHGCSOTypType; (*Object type*)
	END_STRUCT;
	McOHGCSType : STRUCT (*Systemwide reference frame*)
		Object : McCfgUnboundedArrayType; (*Connect array of type McOHGCSObjType*)
	END_STRUCT;
	McCfgObjHierGCSType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_OBJ_HIER_GCS*)
		GlobalCoordinateSystem : McOHGCSType; (*Systemwide reference frame*)
	END_STRUCT;
	McOHMeasUnitLenMeasUnitEnum :
		( (*Defines the measurement unit for length*)
		mcOHMULMU_MILL := 5066068, (*Millimeters*)
		mcOHMULMU_M := 5067858, (*Meters*)
		mcOHMULMU_INCH := 4804168 (*Inches*)
		);
	McOHMeasUnitAngMeasUnitEnum :
		( (*Defines the measurement unit for angles*)
		mcOHMUAMU_DEG := 17476, (*Degrees*)
		mcOHMUAMU_GRAD := 4274481, (*Gradians*)
		mcOHMUAMU_REV := 5059636 (*Revolutions*)
		);
	McOHMeasUnitType : STRUCT (*Defines the measurement units*)
		LengthMeasurementUnit : McOHMeasUnitLenMeasUnitEnum; (*Defines the measurement unit for length*)
		AngleMeasurementUnit : McOHMeasUnitAngMeasUnitEnum; (*Defines the measurement unit for angles*)
	END_STRUCT;
	McOHRotDescEnum :
		( (*Rotation description selector setting*)
		mcOHRD_CA := 0, (*Cardan - Cardan angles*)
		mcOHRD_EU := 1, (*Euler - Euler angles*)
		mcOHRD_NAUTICAL := 2, (*Nautical - Nautical notation*)
		mcOHRD_TAIT_BRYAN := 3 (*Tait Bryan - Tait Bryan notation*)
		);
	McOHRotDescCaRotOrdEnum :
		( (*Order of rotations*)
		mcOHRDCRO_XYZ := 0, (*XYZ - Rotation XYZ*)
		mcOHRDCRO_XZY := 1, (*XZY - Rotation XZY*)
		mcOHRDCRO_YXZ := 2, (*YXZ - Rotation YXZ*)
		mcOHRDCRO_YZX := 3, (*YZX - Rotation YZX*)
		mcOHRDCRO_ZXY := 4, (*ZXY - Rotation ZXY*)
		mcOHRDCRO_ZYX := 5 (*ZYX - Rotation ZYX*)
		);
	McOHRotDescAngIntEnum :
		( (*Interpretation of angles*)
		mcOHRDAI_INTRINSIC := 0, (*Intrinsic - Intrinsic interpretation of angles*)
		mcOHRDAI_EXTRINSIC := 1 (*Extrinsic - Extrinsic interpretation of angles*)
		);
	McOHRotDescRotMatIntEnum :
		( (*Interpretation of angles for rotation matrix*)
		mcOHRDRMI_DIR := 0, (*Direct - Direct interpretation of angles for rotation matrix*)
		mcOHRDRMI_INDIRECT := 1 (*Indirect - Indirect interpretation of angles for rotation matrix*)
		);
	McOHRotDescMathSenseEnum :
		( (*Mathematical sense of angles*)
		mcOHRDMS_POS := 0, (*Positive - Positive mathematical sense of angles for rotation matrix*)
		mcOHRDMS_NEG := 1 (*Negative - Negative mathematical sense of angles for rotation matrix*)
		);
	McOHRotDescCaType : STRUCT (*Type mcOHRD_CA settings*)
		RotationOrder : McOHRotDescCaRotOrdEnum; (*Order of rotations*)
		AngleInterpretation : McOHRotDescAngIntEnum; (*Interpretation of angles*)
		RotationMatrixInterpretation : McOHRotDescRotMatIntEnum; (*Interpretation of angles for rotation matrix*)
		MathematicalSense : McOHRotDescMathSenseEnum; (*Mathematical sense of angles*)
	END_STRUCT;
	McOHRotDescEuRotOrdEnum :
		( (*Order of rotations*)
		mcOHRDERO_XYZ := 0, (*XYZ - Rotation XYZ*)
		mcOHRDERO_XZY := 1, (*XZY - Rotation XZY*)
		mcOHRDERO_YXZ := 2, (*YXZ - Rotation YXZ*)
		mcOHRDERO_YZX := 3, (*YZX - Rotation YZX*)
		mcOHRDERO_ZXY := 4, (*ZXY - Rotation ZXY*)
		mcOHRDERO_ZYX := 5, (*ZYX - Rotation ZYX*)
		mcOHRDERO_XYX := 6, (*XYX - Rotation XYX*)
		mcOHRDERO_XZX := 7, (*XZX - Rotation XZX*)
		mcOHRDERO_YXY := 8, (*YXY - Rotation YXY*)
		mcOHRDERO_YZY := 9, (*YZY - Rotation YZY*)
		mcOHRDERO_ZXZ := 10, (*ZXZ - Rotation ZXZ*)
		mcOHRDERO_ZYZ := 11 (*ZYZ - Rotation ZYZ*)
		);
	McOHRotDescEuType : STRUCT (*Type mcOHRD_EU settings*)
		RotationOrder : McOHRotDescEuRotOrdEnum; (*Order of rotations*)
		AngleInterpretation : McOHRotDescAngIntEnum; (*Interpretation of angles*)
		RotationMatrixInterpretation : McOHRotDescRotMatIntEnum; (*Interpretation of angles for rotation matrix*)
		MathematicalSense : McOHRotDescMathSenseEnum; (*Mathematical sense of angles*)
	END_STRUCT;
	McOHRotDescType : STRUCT (*Defines the description of the rotation*)
		Type : McOHRotDescEnum; (*Rotation description selector setting*)
		Cardan : McOHRotDescCaType; (*Type mcOHRD_CA settings*)
		Euler : McOHRotDescEuType; (*Type mcOHRD_EU settings*)
	END_STRUCT;
	McOHFrmTrfType : STRUCT (*Defines frame transformations properties*)
		RotationTolerance : LREAL; (*Allowed rotation tolerance for frame to frame transformation [measurement units]*)
	END_STRUCT;
	McScnSurfaceEnum :
		( (*Material*)
		mcSOS_UDEF := 0, (*Undefined*)
		mcSOS_RED_MATTE := 1, (*Red Matte*)
		mcSOS_BLUE_MATTE := 2, (*Blue Matte*)
		mcSOS_GREY_MATTE := 3, (*Grey Matte*)
		mcSOS_YELLOW_MATTE := 4, (*Yellow Matte*)
		mcSOS_GREEN_MATTE := 5, (*Green Matte*)
		mcSOS_ORANGE_MATTE := 6, (*Orange Matte*)
		mcSOS_WHITE_MATTE := 7, (*White Matte*)
		mcSOS_BLACK_MATTE := 8, (*Black Matte*)
		mcSOS_VIOLET_MATTE := 9, (*Violet Matte*)
		mcSOS_METAL_SHINE := 10, (*Metal Shine*)
		mcSOS_RED_METAL_SHINE := 11, (*Red Metal Shine*)
		mcSOS_YELLOW_METAL_SHINE := 12, (*Yellow Metal Shine*)
		mcSOS_BLACK_METAL_SHINE := 13, (*Black Metal Shine*)
		mcSOS_CYAN_MATTE := 14, (*Cyan Matte*)
		mcSOS_MAGENTA_MATTE := 15, (*Magenta Matte*)
		mcSOS_LIGHT_GREY_MATTE := 16, (*Light Grey Matte*)
		mcSOS_CERULEAN_BLUE_SHINE := 17, (*Cerulean Blue Shine*)
		mcSOS_SILVER := 18 (*Silver*)
		);
	McOHScnExType : STRUCT (*Defines Scene Viewer export settings*)
		FileDevice : STRING[250]; (*File device where the Object Hierarchy Scene Viewer file will be created*)
	END_STRUCT;
	McCfgObjHierType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_OBJ_HIER*)
		MeasurementUnits : McOHMeasUnitType; (*Defines the measurement units*)
		RotationDescription : McOHRotDescType; (*Defines the description of the rotation*)
		FrameTransformations : McOHFrmTrfType; (*Defines frame transformations properties*)
		SceneViewer : McOHScnExType; (*Defines Scene Viewer export settings*)
		GlobalCoordinateSystem : McOHGCSType; (*Systemwide reference frame*)
	END_STRUCT;
	McCfgVarDatTypEnum :
		( (*Data type selector setting*)
		mcCVDT_TYP_BOOL := 0, (*Type BOOL -*)
		mcCVDT_TYP_SINT := 1, (*Type SINT -*)
		mcCVDT_TYP_USINT := 2, (*Type USINT -*)
		mcCVDT_TYP_INT := 3, (*Type INT -*)
		mcCVDT_TYP_UINT := 4, (*Type UINT -*)
		mcCVDT_TYP_DINT := 5, (*Type DINT -*)
		mcCVDT_TYP_UDINT := 6, (*Type UDINT -*)
		mcCVDT_TYP_REAL := 7, (*Type REAL -*)
		mcCVDT_TYP_LREAL := 8, (*Type LREAL -*)
		mcCVDT_TYP_STRING := 9, (*Type STRING -*)
		mcCVDT_TYP_DER := 10 (*Type Derived - Derived data type*)
		);
	McCfgVarDatTypTypSTRINGType : STRUCT (*Type mcCVDT_TYP_STRING settings*)
		Length : UDINT;
	END_STRUCT;
	McCfgVarDatTypTypDerType : STRUCT (*Type mcCVDT_TYP_DER settings*)
		Name : STRING[250]; (*Name*)
	END_STRUCT;
	McCfgVarDatTypType : STRUCT (*Data type*)
		Type : McCfgVarDatTypEnum; (*Data type selector setting*)
		TypeSTRING : McCfgVarDatTypTypSTRINGType; (*Type mcCVDT_TYP_STRING settings*)
		TypeDerived : McCfgVarDatTypTypDerType; (*Type mcCVDT_TYP_DER settings*)
	END_STRUCT;
	McCfgFunDatTypEnum :
		( (*Function return type selector setting*)
		mcCFDT_TYP_BOOL := 0, (*Type BOOL -*)
		mcCFDT_TYP_SINT := 1, (*Type SINT -*)
		mcCFDT_TYP_USINT := 2, (*Type USINT -*)
		mcCFDT_TYP_INT := 3, (*Type INT -*)
		mcCFDT_TYP_UINT := 4, (*Type UINT -*)
		mcCFDT_TYP_DINT := 5, (*Type DINT -*)
		mcCFDT_TYP_UDINT := 6, (*Type UDINT -*)
		mcCFDT_TYP_REAL := 7, (*Type REAL -*)
		mcCFDT_TYP_LREAL := 8, (*Type LREAL -*)
		mcCFDT_TYP_STRING := 9, (*Type STRING -*)
		mcCFDT_TYP_DER := 10, (*Type Derived - Derived data type*)
		mcCFDT_TYP_NONE := 11 (*Type NONE - Function does not return a value*)
		);
	McCfgFunDatTypTypSTRINGType : STRUCT (*Type mcCFDT_TYP_STRING settings*)
		Length : UDINT;
	END_STRUCT;
	McCfgFunDatTypTypDerType : STRUCT (*Type mcCFDT_TYP_DER settings*)
		Name : STRING[250]; (*Name*)
	END_STRUCT;
	McCfgFunDatTypType : STRUCT (*Return data type of function*)
		Type : McCfgFunDatTypEnum; (*Function return type selector setting*)
		TypeSTRING : McCfgFunDatTypTypSTRINGType; (*Type mcCFDT_TYP_STRING settings*)
		TypeDerived : McCfgFunDatTypTypDerType; (*Type mcCFDT_TYP_DER settings*)
	END_STRUCT;
	McWSCubeDimType : STRUCT (*Dimension parameters*)
		X : LREAL; (*Dimension of the cuboid in the x-direction (endpoint) [measurement units]*)
		Y : LREAL; (*Dimension of the cuboid in the y-direction (endpoint) [measurement units]*)
		Z : LREAL; (*Dimension of the cuboid in the z-direction (endpoint) [measurement units]*)
	END_STRUCT;
	McWSCubeType : STRUCT
		ID : UDINT; (*Unique cuboid ID*)
		Translation : McCfgTransXYZType; (*Translation parameters*)
		Orientation : McCfgOrientType; (*Orientation parameters*)
		Dimension : McWSCubeDimType; (*Dimension parameters*)
	END_STRUCT;
	McWSCubesType : STRUCT (*Defines cuboids*)
		Cuboid : McCfgUnboundedArrayType; (*Connect array of type McWSCubeType*)
	END_STRUCT;
	McWSHalfSpcPlEnum :
		( (*Defines plane*)
		mcWSHSP_PL_XY := 0, (*Plane XY*)
		mcWSHSP_PL_YZ := 1, (*Plane YZ*)
		mcWSHSP_PL_ZX := 2 (*Plane ZX*)
		);
	McWSHalfSpcType : STRUCT
		ID : UDINT; (*Unique half-space ID*)
		Translation : McCfgTransXYZType; (*Translation parameters*)
		Orientation : McCfgOrientType; (*Orientation parameters*)
		Plane : McWSHalfSpcPlEnum; (*Defines plane*)
	END_STRUCT;
	McWSHalfSpcsType : STRUCT (*Defines half-spaces*)
		HalfSpace : McCfgUnboundedArrayType; (*Connect array of type McWSHalfSpcType*)
	END_STRUCT;
	McWSTcConeParType : STRUCT (*Object description parameters*)
		BaseRadius : LREAL; (*Base radius of the cone [measurement units]*)
		Height : LREAL; (*Height of the cone [measurement units]*)
		TopRadius : LREAL; (*Top radius of the cone [measurement units]*)
	END_STRUCT;
	McWSTcConeType : STRUCT (*Selects object type*)
		ID : UDINT; (*Unique object ID*)
		Translation : McCfgTransXYZType; (*Translation parameters*)
		Orientation : McCfgOrientType; (*Orientation parameters*)
		Parameters : McWSTcConeParType; (*Object description parameters*)
	END_STRUCT;
	McWSTcConesType : STRUCT (*Defines truncated cones*)
		TruncatedCone : McCfgUnboundedArrayType; (*Selects object type (Connect array of type McWSTcConeType)*)
	END_STRUCT;
	McWSWorkSpaceType : STRUCT (*Defines the WorkSpace*)
		Cuboids : McWSCubesType; (*Defines cuboids*)
		HalfSpaces : McWSHalfSpcsType; (*Defines half-spaces*)
		TruncatedCones : McWSTcConesType; (*Defines truncated cones*)
	END_STRUCT;
	McWSSafeSpaceType : STRUCT (*Defines the SafeSpace*)
		Cuboids : McWSCubesType; (*Defines cuboids*)
		HalfSpaces : McWSHalfSpcsType; (*Defines half-spaces*)
		TruncatedCones : McWSTcConesType; (*Defines truncated cones*)
	END_STRUCT;
	McCfgWorkspaceType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_WS*)
		WorkSpace : McWSWorkSpaceType; (*Defines the WorkSpace*)
		SafeSpace : McWSSafeSpaceType; (*Defines the SafeSpace*)
	END_STRUCT;
	McFTRowType : STRUCT
		Index : UINT;
		X : LREAL;
		Y : LREAL;
		Z : LREAL;
		Angle1 : LREAL;
		Angle2 : LREAL;
		Angle3 : LREAL;
		Description : STRING[250];
	END_STRUCT;
	McCfgFrmTblType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_FRMTBL*)
		Row : McCfgUnboundedArrayType; (*Connect array of type McFTRowType*)
	END_STRUCT;
	McDPTRowType : STRUCT
		BaseParameterIndex : UINT;
		Value : LREAL;
		Unit : STRING[250];
		Description : STRING[250];
	END_STRUCT;
	McCfgDynParTblType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_DYNPARTBL*)
		Row : McCfgUnboundedArrayType; (*Connect array of type McDPTRowType*)
	END_STRUCT;
	McLSPosEnum :
		( (*Position selector setting*)
		mcLSP_NOT_USE := 0, (*Not used -*)
		mcLSP_USE := 1 (*Used -*)
		);
	McCfgLimPosType : STRUCT (*Type mcLSP_USE settings*)
		LowerLimit : LREAL; (*Lower software limit position [measurement units]*)
		UpperLimit : LREAL; (*Upper software limit position [measurement units]*)
	END_STRUCT;
	McLSPosType : STRUCT (*Movement range of the axis via two position boundaries*)
		Type : McLSPosEnum; (*Position selector setting*)
		Used : McCfgLimPosType; (*Type mcLSP_USE settings*)
	END_STRUCT;
	McLSVelEnum :
		( (*Velocity selector setting*)
		mcLSV_NOT_USE := 0, (*Not used -*)
		mcLSV_BASIC := 1, (*Basic -*)
		mcLSV_ADV := 2 (*Advanced -*)
		);
	McCfgLimVelBaseType : STRUCT (*Type mcLSV_BASIC settings*)
		Velocity : REAL; (*Velocity limit in any movement direction [measurement units/s]*)
	END_STRUCT;
	McCfgLimVelAdvType : STRUCT (*Type mcLSV_ADV settings*)
		Positive : REAL; (*Velocity limit in positive movement direction [measurement units/s]*)
		Negative : REAL; (*Velocity limit in negative movement direction [measurement units/s]*)
	END_STRUCT;
	McLSVelType : STRUCT (*Velocity limits*)
		Type : McLSVelEnum; (*Velocity selector setting*)
		Basic : McCfgLimVelBaseType; (*Type mcLSV_BASIC settings*)
		Advanced : McCfgLimVelAdvType; (*Type mcLSV_ADV settings*)
	END_STRUCT;
	McLSAccEnum :
		( (*Acceleration selector setting*)
		mcLSA_NOT_USE := 0, (*Not used -*)
		mcLSA_BASIC := 1, (*Basic -*)
		mcLSA_ADV := 2 (*Advanced -*)
		);
	McCfgLimAccBaseType : STRUCT (*Type mcLSA_BASIC settings*)
		Acceleration : REAL; (*Acceleration limit in any movement direction [measurement units/s²]*)
	END_STRUCT;
	McCfgLimAccAdvType : STRUCT (*Type mcLSA_ADV settings*)
		Positive : REAL; (*Acceleration limit in positive movement direction [measurement units/s²]*)
		Negative : REAL; (*Acceleration limit in negative movement direction [measurement units/s²]*)
	END_STRUCT;
	McLSAccType : STRUCT (*Acceleration limits*)
		Type : McLSAccEnum; (*Acceleration selector setting*)
		Basic : McCfgLimAccBaseType; (*Type mcLSA_BASIC settings*)
		Advanced : McCfgLimAccAdvType; (*Type mcLSA_ADV settings*)
	END_STRUCT;
	McLSDecEnum :
		( (*Deceleration selector setting*)
		mcLSD_NOT_USE := 0, (*Not used -*)
		mcLSD_BASIC := 1, (*Basic -*)
		mcLSD_ADV := 2 (*Advanced -*)
		);
	McCfgLimDecBaseType : STRUCT (*Type mcLSD_BASIC settings*)
		Deceleration : REAL; (*Deceleration limit in any movement direction [measurement units/s²]*)
	END_STRUCT;
	McCfgLimDecAdvType : STRUCT (*Type mcLSD_ADV settings*)
		Positive : REAL; (*Deceleration limit in positive movement direction [measurement units/s²]*)
		Negative : REAL; (*Deceleration limit in negative movement direction [measurement units/s²]*)
	END_STRUCT;
	McLSDecType : STRUCT (*Deceleration limits*)
		Type : McLSDecEnum; (*Deceleration selector setting*)
		Basic : McCfgLimDecBaseType; (*Type mcLSD_BASIC settings*)
		Advanced : McCfgLimDecAdvType; (*Type mcLSD_ADV settings*)
	END_STRUCT;
	McCfgLimJerkEnum :
		( (*Jerk selector setting*)
		mcCLJ_NOT_USE := 0, (*Not used -*)
		mcCLJ_BASIC := 1, (*Basic -*)
		mcCLJ_ADV := 2 (*Advanced -*)
		);
	McCfgLimJerkBasicType : STRUCT (*Type mcCLJ_BASIC settings*)
		Jerk : REAL; (*Jerk limit in any movement direction [measurement units/s³]*)
	END_STRUCT;
	McCfgLimJerkAdvType : STRUCT (*Type mcCLJ_ADV settings*)
		AccelerationPositive : REAL; (*Jerk limit in positive movement direction during acceleration [measurement units/s³]*)
		AccelerationNegative : REAL; (*Jerk limit in negative movement direction during acceleration [measurement units/s³]*)
		DecelerationPositive : REAL; (*Jerk limit in positive movement direction during deceleration [measurement units/s³]*)
		DecelerationNegative : REAL; (*Jerk limit in negative movement direction during deceleration [measurement units/s³]*)
	END_STRUCT;
	McCfgLimJerkType : STRUCT (*Jerk limits*)
		Type : McCfgLimJerkEnum; (*Jerk selector setting*)
		Basic : McCfgLimJerkBasicType; (*Type mcCLJ_BASIC settings*)
		Advanced : McCfgLimJerkAdvType; (*Type mcCLJ_ADV settings*)
	END_STRUCT;
	McCfgLimForEnum :
		( (*Force selector setting*)
		mcCLF_NOT_USE := 0, (*Not used -*)
		mcCLF_BASIC := 1, (*Basic -*)
		mcCLF_ADV := 2 (*Advanced -*)
		);
	McCfgLimForBasicType : STRUCT (*Type mcCLF_BASIC settings*)
		Force : REAL; (*Force limit in any movement direction [N]*)
	END_STRUCT;
	McCfgLimForAdvType : STRUCT (*Type mcCLF_ADV settings*)
		AccelerationPositive : REAL; (*Force limit in positive movement direction during acceleration [N]*)
		AccelerationNegative : REAL; (*Force limit in negative movement direction during acceleration [N]*)
		DecelerationPositive : REAL; (*Force limit in positive movement direction during deceleration [N]*)
		DecelerationNegative : REAL; (*Force limit in negative movement direction during deceleration [N]*)
	END_STRUCT;
	McCfgLimForType : STRUCT (*Limits for the force of the axis*)
		Type : McCfgLimForEnum; (*Force selector setting*)
		Basic : McCfgLimForBasicType; (*Type mcCLF_BASIC settings*)
		Advanced : McCfgLimForAdvType; (*Type mcCLF_ADV settings*)
	END_STRUCT;
	McCfgLimSetLinType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_LIMSET_LIN*)
		Position : McLSPosType; (*Movement range of the axis via two position boundaries*)
		Velocity : McLSVelType; (*Velocity limits*)
		Acceleration : McLSAccType; (*Acceleration limits*)
		Deceleration : McLSDecType; (*Deceleration limits*)
		Jerk : McCfgLimJerkType; (*Jerk limits*)
		Force : McCfgLimForType; (*Limits for the force of the axis*)
	END_STRUCT;
	McCfgLimTorqEnum :
		( (*Torque selector setting*)
		mcCLT_NOT_USE := 0, (*Not used -*)
		mcCLT_BASIC := 1, (*Basic -*)
		mcCLT_ADV := 2 (*Advanced -*)
		);
	McCfgLimTorqBasicType : STRUCT (*Type mcCLT_BASIC settings*)
		Torque : REAL; (*Torque limit in any movement direction [Nm]*)
	END_STRUCT;
	McCfgLimTorqAdvType : STRUCT (*Type mcCLT_ADV settings*)
		AccelerationPositive : REAL; (*Torque limit in positive movement direction during acceleration [Nm]*)
		AccelerationNegative : REAL; (*Torque limit in negative movement direction during acceleration [Nm]*)
		DecelerationPositive : REAL; (*Torque limit in positive movement direction during deceleration [Nm]*)
		DecelerationNegative : REAL; (*Torque limit in negative movement direction during deceleration [Nm]*)
	END_STRUCT;
	McCfgLimTorqType : STRUCT (*Torque limits*)
		Type : McCfgLimTorqEnum; (*Torque selector setting*)
		Basic : McCfgLimTorqBasicType; (*Type mcCLT_BASIC settings*)
		Advanced : McCfgLimTorqAdvType; (*Type mcCLT_ADV settings*)
	END_STRUCT;
	McCfgLimSetRotType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_LIMSET_ROT*)
		Position : McLSPosType; (*Movement range of the axis via two position boundaries*)
		Velocity : McLSVelType; (*Velocity limits*)
		Acceleration : McLSAccType; (*Acceleration limits*)
		Deceleration : McLSDecType; (*Deceleration limits*)
		Jerk : McCfgLimJerkType; (*Jerk limits*)
		Torque : McCfgLimTorqType; (*Torque limits*)
	END_STRUCT;
	McCLRowCamIDEnum :
		(
		mcCLRCI_CAM_ID_1 := 0, (*Cam ID 1 - Cam ID 1*)
		mcCLRCI_CAM_ID_2 := 1, (*Cam ID 2 - Cam ID 2*)
		mcCLRCI_CAM_ID_3 := 2, (*Cam ID 3 - Cam ID 3*)
		mcCLRCI_CAM_ID_4 := 3, (*Cam ID 4 - Cam ID 4*)
		mcCLRCI_CAM_ID_5 := 4, (*Cam ID 5 - Cam ID 5*)
		mcCLRCI_CAM_ID_6 := 5, (*Cam ID 6 - Cam ID 6*)
		mcCLRCI_CAM_ID_7 := 6, (*Cam ID 7 - Cam ID 7*)
		mcCLRCI_CAM_ID_8 := 7, (*Cam ID 8 - Cam ID 8*)
		mcCLRCI_CAM_ID_9 := 8, (*Cam ID 9 - Cam ID 9*)
		mcCLRCI_CAM_ID_10 := 9, (*Cam ID 10 - Cam ID 10*)
		mcCLRCI_CAM_ID_11 := 10, (*Cam ID 11 - Cam ID 11*)
		mcCLRCI_CAM_ID_12 := 11, (*Cam ID 12 - Cam ID 12*)
		mcCLRCI_CAM_ID_13 := 12, (*Cam ID 13 - Cam ID 13*)
		mcCLRCI_CAM_ID_14 := 13, (*Cam ID 14 - Cam ID 14*)
		mcCLRCI_CAM_ID_15 := 14, (*Cam ID 15 - Cam ID 15*)
		mcCLRCI_CAM_ID_16 := 15, (*Cam ID 16 - Cam ID 16*)
		mcCLRCI_CAM_ID_17 := 16, (*Cam ID 17 - Cam ID 17*)
		mcCLRCI_CAM_ID_18 := 17, (*Cam ID 18 - Cam ID 18*)
		mcCLRCI_CAM_ID_19 := 18, (*Cam ID 19 - Cam ID 19*)
		mcCLRCI_CAM_ID_20 := 19, (*Cam ID 20 - Cam ID 20*)
		mcCLRCI_CAM_ID_21 := 20, (*Cam ID 21 - Cam ID 21*)
		mcCLRCI_CAM_ID_22 := 21, (*Cam ID 22 - Cam ID 22*)
		mcCLRCI_CAM_ID_23 := 22, (*Cam ID 23 - Cam ID 23*)
		mcCLRCI_CAM_ID_24 := 23, (*Cam ID 24 - Cam ID 24*)
		mcCLRCI_CAM_ID_25 := 24, (*Cam ID 25 - Cam ID 25*)
		mcCLRCI_CAM_ID_26 := 25, (*Cam ID 26 - Cam ID 26*)
		mcCLRCI_CAM_ID_27 := 26, (*Cam ID 27 - Cam ID 27*)
		mcCLRCI_CAM_ID_28 := 27, (*Cam ID 28 - Cam ID 28*)
		mcCLRCI_CAM_ID_29 := 28, (*Cam ID 29 - Cam ID 29*)
		mcCLRCI_CAM_ID_30 := 29, (*Cam ID 30 - Cam ID 30*)
		mcCLRCI_CAM_ID_31 := 30, (*Cam ID 31 - Cam ID 31*)
		mcCLRCI_CAM_ID_32 := 31, (*Cam ID 32 - Cam ID 32*)
		mcCLRCI_CAM_ID_33 := 32, (*Cam ID 33 - Cam ID 33*)
		mcCLRCI_CAM_ID_34 := 33, (*Cam ID 34 - Cam ID 34*)
		mcCLRCI_CAM_ID_35 := 34, (*Cam ID 35 - Cam ID 35*)
		mcCLRCI_CAM_ID_36 := 35, (*Cam ID 36 - Cam ID 36*)
		mcCLRCI_CAM_ID_37 := 36, (*Cam ID 37 - Cam ID 37*)
		mcCLRCI_CAM_ID_38 := 37, (*Cam ID 38 - Cam ID 38*)
		mcCLRCI_CAM_ID_39 := 38, (*Cam ID 39 - Cam ID 39*)
		mcCLRCI_CAM_ID_40 := 39 (*Cam ID 40 - Cam ID 40*)
		);
	McCLRowType : STRUCT
		CamName : STRING[250];
		CamID : McCLRowCamIDEnum;
		Description : STRING[250];
	END_STRUCT;
	McCfgCamLstType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_CAMLST*)
		Row : McCfgUnboundedArrayType; (*Connect array of type McCLRowType*)
	END_STRUCT;
	McPPTypEnum :
		( (*Type selector setting*)
		mcPPT_ACPTRAK_PT := 0 (*AcpTrak point - ACOPOSTrak*)
		);
	McPPTAcpTrakPtPosRelToEnum :
		( (*Absolute position from which the position is counted*)
		mcPPTAPPRT_ST_OF_SEC := 0, (*Start of sector - Tail of the sector*)
		mcPPTAPPRT_END_OF_SEC := 1 (*End of sector - Head of the sector*)
		);
	McPPTAcpTrakPtBarrFunEnum :
		( (*Barrier functionality selector setting*)
		mcPPTAPBF_OFF := 0, (*Off - Disable barrier functionality*)
		mcPPTAPBF_ON := 1 (*On - Enable barrier functionality*)
		);
	McPPTAcpTrakPtShStopPosEnum :
		( (*Mode that determines the stop position of a shuttle in front of a barrier*)
		mcPPTASPSP_EXT := 0, (*Extent - Stop shuttle with extent in front of barrier*)
		mcPPTASPSP_CTR_PT := 1 (*Center point - Stop shuttle center point in front of barrier*)
		);
	McPPTAcpTrakPtBarrFunOnType : STRUCT (*Type mcPPTAPBF_ON settings*)
		ShuttleStopPosition : McPPTAcpTrakPtShStopPosEnum; (*Mode that determines the stop position of a shuttle in front of a barrier*)
		BarrierStopDistance : LREAL; (*Additional safety distance between a shuttle and a process point [Measurement units]*)
	END_STRUCT;
	McPPTAcpTrakPtBarrFunType : STRUCT (*Enable barrier functionality for process point*)
		Type : McPPTAcpTrakPtBarrFunEnum; (*Barrier functionality selector setting*)
		On : McPPTAcpTrakPtBarrFunOnType; (*Type mcPPTAPBF_ON settings*)
	END_STRUCT;
	McPPTAcpTrakPtPPMMonEnum :
		( (*Throughput monitor selector setting*)
		mcPPTAcpTrakPtPPMMon_OFF := 0, (*Off - Disable throughput monitor functionality*)
		mcPPTAcpTrakPtPPMMon_SH_TRG := 1 (*Shuttle triggered - Enable the shuttle triggered throughput monitor*)
		);
	McPPTAcpTrakPtPPMMonShTrgType : STRUCT (*Type mcPPTAcpTrakPtPPMMon_SH_TRG settings*)
		BatchSize : UINT; (*Number of shuttles based on which the throughput is computed*)
		PartsPerShuttle : REAL; (*Number of parts transported per shuttle*)
	END_STRUCT;
	McPPTAcpTrakPtPPMMonType : STRUCT (*Enable the throughput monitor functionality*)
		Type : McPPTAcpTrakPtPPMMonEnum; (*Throughput monitor selector setting*)
		ShuttleTriggered : McPPTAcpTrakPtPPMMonShTrgType; (*Type mcPPTAcpTrakPtPPMMon_SH_TRG settings*)
	END_STRUCT;
	McPPTAcpTrakPtType : STRUCT (*Type mcPPT_ACPTRAK_PT settings*)
		SectorReference : McCfgReferenceType; (*Name of the referenced sector component*)
		Position : LREAL; (*Position on the sector [measurement units]*)
		PositionRelativeTo : McPPTAcpTrakPtPosRelToEnum; (*Absolute position from which the position is counted*)
		TriggerEventBufferSize : UINT; (*Amount of maximum saved trigger point events*)
		BarrierFunctionality : McPPTAcpTrakPtBarrFunType; (*Enable barrier functionality for process point*)
		ThroughputMonitor : McPPTAcpTrakPtPPMMonType; (*Enable the throughput monitor functionality*)
	END_STRUCT;
	McPPTypType : STRUCT (*Type of the configured process point*)
		Type : McPPTypEnum; (*Type selector setting*)
		AcpTrakPoint : McPPTAcpTrakPtType; (*Type mcPPT_ACPTRAK_PT settings*)
	END_STRUCT;
	McCfgProcPointType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_PROC_POINT*)
		Type : McPPTypType; (*Type of the configured process point*)
	END_STRUCT;
	McCfgProcPtTrakPosType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_PROC_TRAK_POS*)
		SectorReference : McCfgReferenceType; (*Name of the referenced sector component*)
		Position : LREAL; (*Position on the sector [measurement units]*)
		PositionRelativeTo : McPPTAcpTrakPtPosRelToEnum; (*Absolute position from which the position is counted*)
		TriggerEventBufferSize : UINT; (*Amount of maximum saved trigger point events*)
	END_STRUCT;
	McCfgProcPtTrakBarrierType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_PROC_TRAK_BARRIER*)
		BarrierFunctionality : McPPTAcpTrakPtBarrFunType; (*Enable barrier functionality for process point*)
	END_STRUCT;
	McCfgProcPtTrakMonType : STRUCT (*Main data type corresponding to McCfgTypeEnum mcCFG_PROC_TRAK_MON*)
		ThroughputMonitor : McPPTAcpTrakPtPPMMonType; (*Enable the throughput monitor functionality*)
	END_STRUCT;
	McCfgLocLenUnitEnum :
		( (*Measurement unit for the axis*)
		mcCLLU_G_SET := 0, (*Global settings*)
		mcCLLU_MILL := 5066068, (*Millimeters*)
		mcCLLU_M := 5067858, (*Meters*)
		mcCLLU_INCH := 4804168, (*Inches*)
		mcCLLU_GEN := -1 (*Generic*)
		);
	McCfgCntDirEnum :
		( (*Direction of the axis in which the position value is increasing*)
		mcCCD_STD := 0, (*Standard - Rotary axis: clockwise*)
		mcCCD_INV := 1 (*Inverse - Rotary axis: counter-clockwise*)
		);
	McCfgLocRotUnitEnum :
		( (*Measurement unit for the axis*)
		mcCLRU_G_SET := 0, (*Global settings*)
		mcCLRU_DEG := 17476, (*Degrees*)
		mcCLRU_GRAD := 4274481, (*Gradians*)
		mcCLRU_REV := 5059636, (*Revolutions*)
		mcCLRU_GEN := -1 (*Generic*)
		);
	McCfgExtLimRefType : STRUCT (*Type mcAML_EXT settings*)
		LimitReference : McCfgReferenceType; (*Name of the limit reference*)
	END_STRUCT;
	McPTCEnum :
		( (*Task class for cyclic data processing*)
		mcPTC_CYC_1 := 1, (*Cyclic #1 - Task class 1*)
		mcPTC_USE_MP_MOT_SET := 255 (*Use mapp Motion setting - Use the defined setting from the mapp Motion configuration or Task class 1 if no mapp Motion Configuration exists*)
		);
	McCfgGearBoxType : STRUCT (*Ratio between a gearbox input and output*)
		Input : DINT; (*Number of rotations on the encoder side [revolutions]*)
		Output : DINT; (*Number of rotations on the load side which correspond to the number of rotations on the encoder side [revolutions]*)
	END_STRUCT;
	McCfgRotToLinTrfType : STRUCT (*Specifies a transformation factor between the output of the gear and the actual load movement*)
		ReferenceDistance : LREAL; (*Distance corresponding to one gearbox output revolution [measurement units/gearbox output revolution]*)
	END_STRUCT;
	McCfgLimJerkBaseType : STRUCT (*Type mcAGFPDPLIJ_BASIC settings*)
		Jerk : REAL; (*Jerk limit in any movement direction [measurement units/s³]*)
	END_STRUCT;
	McCfgTransXType : STRUCT (*Translation from base to QX*)
		X : LREAL; (*Translation in X direction [measurement units]*)
	END_STRUCT;
	McCfgTransZType : STRUCT (*Translation from QX to QZ*)
		Z : LREAL; (*Translation in Z direction [measurement units]*)
	END_STRUCT;
	McCfgTransXZType : STRUCT (*Translation from QZ to QB*)
		X : LREAL; (*Translation in X direction [measurement units]*)
		Z : LREAL; (*Translation in Z direction [measurement units]*)
	END_STRUCT;
	McCfgTransYType : STRUCT (*Translation from QX to QY*)
		Y : LREAL; (*Translation in Y direction [measurement units]*)
	END_STRUCT;
END_TYPE