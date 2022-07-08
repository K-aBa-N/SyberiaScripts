modded class PlayerSwayConstants
{
	static const float SWAY_TIME_STABLE = 30.0;
};

modded class StaminaHandler
{
	override void DepleteStamina(EStaminaModifiers modifier, float dT = -1)
	{
		float depleteMod = 1;
		StaminaModifier sm = m_StaminaModifiers.GetModifierData(modifier);
		if (modifier == EStaminaModifiers.HOLD_BREATH)
		{
			depleteMod = m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_BREATH_DEC, 1, 1);
			sm.SetMinValue( GameConstants.STAMINA_DRAIN_HOLD_BREATH_START / depleteMod );
		}
		else if (modifier == EStaminaModifiers.JUMP)
		{
			depleteMod = 1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_STRENGTH_JUMP_STAMINA_DEC, 0, 0);
			sm.SetMaxValue( GameConstants.STAMINA_DRAIN_JUMP * depleteMod );
		}
		else if (modifier == EStaminaModifiers.VAULT)
		{
			depleteMod = 1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_STRENGTH_JUMP_STAMINA_DEC, 0, 0);
			sm.SetMaxValue( GameConstants.STAMINA_DRAIN_VAULT * depleteMod );
		}
		else if (modifier == EStaminaModifiers.CLIMB)
		{
			depleteMod = 1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_STRENGTH_JUMP_STAMINA_DEC, 0, 0);
			sm.SetMaxValue( GameConstants.STAMINA_DRAIN_CLIMB * depleteMod );
		}
		else if (modifier == EStaminaModifiers.MELEE_EVADE)
		{
			depleteMod = 1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_STRENGTH_BLOCK_STAMINA_DEC, 0, 0);
			sm.SetMaxValue( GameConstants.STAMINA_DRAIN_MELEE_EVADE * depleteMod );
			sm.SetMinValue( sm.GetMaxValue() * 0.5 );
		}
		else if (modifier == EStaminaModifiers.MELEE_LIGHT)
		{
			depleteMod = 1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_STRENGTH_FAST_ATTACK_STAMINA_DEC, 0, 0);
			sm.SetMaxValue( GameConstants.STAMINA_DRAIN_MELEE_LIGHT * depleteMod );
		}
		else if (modifier == EStaminaModifiers.MELEE_HEAVY)
		{
			depleteMod = 1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_STRENGTH_HEAVY_ATTACK_STAMINA_DEC, 0, 0);
			sm.SetMaxValue( GameConstants.STAMINA_DRAIN_MELEE_HEAVY * depleteMod );
		}
		
		super.DepleteStamina(modifier, dT);
	}
	
	override protected void StaminaProcessor_Move(HumanMovementState pHumanMovementState)
	{
		switch ( pHumanMovementState.m_iMovement )
		{
		case DayZPlayerConstants.MOVEMENTIDX_SPRINT: //sprint
			if ( pHumanMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_ERECT )
			{
				m_StaminaDelta = -GameConstants.STAMINA_DRAIN_STANDING_SPRINT_PER_SEC * (1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_SPRINT_DEC, 0, 0));
				SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION);
				break;
			}
			if ( pHumanMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_CROUCH)
			{
				m_StaminaDelta = -GameConstants.STAMINA_DRAIN_CROUCHED_SPRINT_PER_SEC * (1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_SPRINT_DEC, 0, 0));
				SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION);
				break;
			}
			m_StaminaDelta = GameConstants.STAMINA_GAIN_JOG_PER_SEC;
		break;
			
		case DayZPlayerConstants.MOVEMENTIDX_RUN: //jog
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_JOG_PER_SEC + CalcStaminaGainBonus()) * m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_JOG_INC, 1, 1);
			}
		break;
			
		case DayZPlayerConstants.MOVEMENTIDX_WALK: //walk
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_WALK_PER_SEC + CalcStaminaGainBonus()) * m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_JOG_INC, 1, 1);
			}
		break;
			
		case DayZPlayerConstants.MOVEMENTIDX_IDLE: //idle
			if (m_Player.IsRolling())
			{
				m_StaminaDelta = GameConstants.STAMINA_GAIN_ROLL_PER_SEC;
				break;
			}
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_IDLE_PER_SEC + CalcStaminaGainBonus()) * m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_IDDLE_INC, 1, 1);
			}
		break;
			
		default:
			if ( !m_IsInCooldown )
			{
				m_StaminaDelta = GameConstants.STAMINA_GAIN_IDLE_PER_SEC * m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_IDDLE_INC, 1, 1);
			}
		break;
		}
	}
	
	override protected void StaminaProcessor_Swimming(HumanMovementState pHumanMovementState)
	{
		switch ( pHumanMovementState.m_iMovement )
		{
		case 3: //swim fast
			m_StaminaDelta = -GameConstants.STAMINA_DRAIN_SWIM_FAST_PER_SEC * (1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_SWIM_SPRINT_DEC, 0, 0));
			SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION);
			break;
		break;
			
		case 2: //swim slow
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_SWIM_PER_SEC + CalcStaminaGainBonus()) * m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_SWIM_INC, 1, 1);
			}
		break;
			
		default:
			if( !m_IsInCooldown )
			{
				m_StaminaDelta = GameConstants.STAMINA_GAIN_IDLE_PER_SEC;
			}
		break;
		}
	}
	
	override protected void StaminaProcessor_Ladder(HumanMovementState pHumanMovementState)
	{
		switch ( pHumanMovementState.m_iMovement )
		{
		case 2: //climb up (fast)
			m_StaminaDelta = -GameConstants.STAMINA_DRAIN_LADDER_FAST_PER_SEC * (1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_LADDER_SPRINT_DEC, 0, 0));
			SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION);
			break;
		break;
			
		case 1: //climb up (slow)
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_LADDER_PER_SEC + CalcStaminaGainBonus()) * m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_LADDER_INC, 1, 1);
			}
		break;
			
		default:
			if( !m_IsInCooldown )
			{
				m_StaminaDelta = GameConstants.STAMINA_GAIN_IDLE_PER_SEC;
			}
		break;
		}
	}
	
	override protected void SetCooldown(float time, int modifier = -1)
	{
		super.SetCooldown(time * (1.0 - m_Player.GetPerkFloatValue(SyberiaPerkType.SYBPERK_ATHLETICS_COLDOWN_DEC, 0, 0)), modifier);
	}
};