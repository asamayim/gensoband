/* File: do-spell.c */

/* Purpose: Do everything for each spell */

#include "angband.h"


/*
 * Generate dice info string such as "foo 2d10"
 */
static cptr info_string_dice(cptr str, int dice, int sides, int base)
{
	/* Fix value */
	if (!dice)
		return format("%s%d", str, base);

	/* Dice only */
	else if (!base)
		return format("%s%dd%d", str, dice, sides);

	/* Dice plus base value */
	else
		return format("%s%dd%d%+d", str, dice, sides, base);
}


/*
 * Generate damage-dice info string such as "dam 2d10"
 */
static cptr info_damage(int dice, int sides, int base)
{
#ifdef JP
	return info_string_dice("����:", dice, sides, base);
#else
	return info_string_dice("dam ", dice, sides, base);
#endif
}


/*
 * Generate duration info string such as "dur 20+1d20"
 */
static cptr info_duration(int base, int sides)
{
#ifdef JP
	return format("����:%d+1d%d", base, sides);
#else
	return format("dur %d+1d%d", base, sides);
#endif
}


/*
 * Generate range info string such as "range 5"
 */
static cptr info_range(int range)
{
#ifdef JP
	return format("�͈�:%d", range);
#else
	return format("range %d", range);
#endif
}


/*
 * Generate heal info string such as "heal 2d8"
 */
static cptr info_heal(int dice, int sides, int base)
{
#ifdef JP
	return info_string_dice("��:", dice, sides, base);
#else
	return info_string_dice("heal ", dice, sides, base);
#endif
}


/*
 * Generate delay info string such as "delay 15+1d15"
 */
static cptr info_delay(int base, int sides)
{
#ifdef JP
	return format("�x��:%d+1d%d", base, sides);
#else
	return format("delay %d+1d%d", base, sides);
#endif
}


/*
 * Generate multiple-damage info string such as "dam 25 each"
 */
static cptr info_multi_damage(int dam)
{
#ifdef JP
	return format("����:�e%d", dam);
#else
	return format("dam %d each", dam);
#endif
}


/*
 * Generate multiple-damage-dice info string such as "dam 5d2 each"
 */
static cptr info_multi_damage_dice(int dice, int sides)
{
#ifdef JP
	return format("����:�e%dd%d", dice, sides);
#else
	return format("dam %dd%d each", dice, sides);
#endif
}


/*
 * Generate power info string such as "power 100"
 */
static cptr info_power(int power)
{
#ifdef JP
	return format("����:%d", power);
#else
	return format("power %d", power);
#endif
}


/*
 * Generate power info string such as "power 1d100"
 */
static cptr info_power_dice(int dice, int sides)
{
#ifdef JP
	return format("����:%dd%d", dice, sides);
#else
	return format("power %dd%d", dice, sides);
#endif
}


/*
 * Generate radius info string such as "rad 100"
 */
static cptr info_radius(int rad)
{
#ifdef JP
	return format("���a:%d", rad);
#else
	return format("rad %d", rad);
#endif
}


/*
 * Generate weight info string such as "max wgt 15"
 */
static cptr info_weight(int weight)
{
#ifdef JP
//	return format("�ő�d��:%d.%dkg", lbtokg1(weight/10), lbtokg2(weight/10));
	///mod151024 �悭�������/10�������
	return format("�ő�d��:%d.%dkg", lbtokg1(weight), lbtokg2(weight));
#else
	return format("max wgt %d", weight/10);
#endif
}


/*
 * Prepare standard probability to become beam for fire_bolt_or_beam()
 */
static int beam_chance(void)
{

	if (p_ptr->pclass == CLASS_MAGE)
		return p_ptr->lev;
	if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER)
		return p_ptr->lev + 10;

	if(cp_ptr->magicmaster) return p_ptr->lev + 5;

	return p_ptr->lev / 2;
}


/*
 * Handle summoning and failure of trump spells
 */
static bool trump_summoning(int num, bool pet, int y, int x, int lev, int type, u32b mode)
{
	int plev = p_ptr->lev;

	int who;
	int i;
	bool success = FALSE;

	/* Default level */
	if (!lev) lev = plev * 2 / 3 + randint1(plev / 2);

	if (pet)
	{
		/* Become pet */
		mode |= PM_FORCE_PET;

		/* Only sometimes allow unique monster */
		if (mode & PM_ALLOW_UNIQUE)
		{
			/* Forbid often */
			if (randint1(50 + plev) >= plev / 10)
				mode &= ~PM_ALLOW_UNIQUE;
		}

		/* Player is who summons */
		who = -1;
	}
	else
	{
		/* Prevent taming, allow unique monster */
		mode |= PM_NO_PET;

		/* Behave as if they appear by themselfs */
		who = 0;
	}

	for (i = 0; i < num; i++)
	{
		if (summon_specific(who, y, x, lev, type, mode))
			success = TRUE;
	}

	if (!success)
	{
#ifdef JP
		msg_print("�N�����Ȃ��̃J�[�h�̌Ăѐ��ɓ����Ȃ��B");
#else
		msg_print("Nobody answers to your Trump call.");
#endif
	}

	return success;
}


/*
 * This spell should become more useful (more controlled) as the
 * player gains experience levels.  Thus, add 1/5 of the player's
 * level to the die roll.  This eliminates the worst effects later on,
 * while keeping the results quite random.  It also allows some potent
 * effects only at high level.
 */
static void cast_wonder(int dir)
{
	int plev = p_ptr->lev;
	int die = randint1(100) + plev / 5;
	//int vir = virtue_number(V_CHANCE);
	/*
	if (vir)
	{
		if (p_ptr->virtues[vir - 1] > 0)
		{
			while (randint1(400) < p_ptr->virtues[vir - 1]) die++;
		}
		else
		{
			while (randint1(400) < (0-p_ptr->virtues[vir - 1])) die--;
		}
	}
	*/
	//if (die < 26)
	//	chg_virtue(V_CHANCE, 1);

	if (die > 100)
	{
#ifdef JP
		msg_print("���Ȃ��͗͂��݂Ȃ���̂��������I");
#else
		msg_print("You feel a surge of power!");
#endif
	}

	if (die < 8) clone_monster(dir);
	else if (die < 14) speed_monster(dir, plev);
	else if (die < 26) heal_monster(dir, damroll(4, 6));
	else if (die < 31) poly_monster(dir, plev);
	else if (die < 36)
		fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir,
				  damroll(3 + ((plev - 1) / 5), 4));
	else if (die < 41) confuse_monster(dir, plev);
	else if (die < 46) fire_ball(GF_POIS, dir, 20 + (plev / 2), 3);
	else if (die < 51) (void)lite_line(dir, damroll(6, 8));
	else if (die < 56)
		fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir,
				  damroll(3 + ((plev - 5) / 4), 8));
	else if (die < 61)
		fire_bolt_or_beam(beam_chance() - 10, GF_COLD, dir,
				  damroll(5 + ((plev - 5) / 4), 8));
	else if (die < 66)
		fire_bolt_or_beam(beam_chance(), GF_ACID, dir,
				  damroll(6 + ((plev - 5) / 4), 8));
	else if (die < 71)
		fire_bolt_or_beam(beam_chance(), GF_FIRE, dir,
				  damroll(8 + ((plev - 5) / 4), 8));
	else if (die < 76) drain_life(dir, 75);
	else if (die < 81) fire_ball(GF_ELEC, dir, 30 + plev / 2, 2);
	else if (die < 86) fire_ball(GF_ACID, dir, 40 + plev, 2);
	else if (die < 91) fire_ball(GF_ICE, dir, 70 + plev, 3);
	else if (die < 96) fire_ball(GF_FIRE, dir, 80 + plev, 3);
	else if (die < 101) drain_life(dir, 100 + plev);
	else if (die < 104)
	{
		earthquake(py, px, 12);
	}
	else if (die < 106)
	{
		(void)destroy_area(py, px, 13 + randint0(5), FALSE,FALSE,FALSE);
	}
	else if (die < 108)
	{
		symbol_genocide(plev+50, TRUE,0);
	}
	else if (die < 110) dispel_monsters(120);
	else /* RARE */
	{
		dispel_monsters(150);
		slow_monsters(plev);
		sleep_monsters(plev);
		hp_player(300);
	}
}


//v1.1.14 �V�Ƒ��_�~�Ղ̎��s���@
//�˕P���Z�Ɩ��@�œ��g�����V�F�󔄖��ŕ��򂷂�̂Ń��[�`���������ċ��L������
void call_amaterasu(void)
{
	int plev = p_ptr->lev;

	if(p_ptr->kamioroshi & KAMIOROSHI_AMENOUZUME)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];
		int i;
		int delay;
		set_kamioroshi(KAMIOROSHI_AMATERASU, 0);
		msg_print("�˔@�A�_���W���������z�̂悤��ῂ����ɕ���ꂽ�I");
		project(0, 3, py, px, plev*24, GF_HOLY_FIRE, PROJECT_KILL, -1);
		project(0, 8, py, px, plev*20, GF_DISP_ALL, PROJECT_KILL, -1);
		project_hack2(GF_LITE, 0,0,plev * 10);
		stun_monsters(plev * 6);
		for (i = 1; (i < m_max); i++)
		{
			m_ptr = &m_list[i];
			if (!m_ptr->r_idx) continue;
			r_ptr = &r_info[m_ptr->r_idx];
			if(p_ptr->riding == i) continue;
			if(r_ptr->flagsr & RFR_RES_LITE) continue;
			if(m_ptr->cdis > 3) continue;
			monster_desc(m_name, m_ptr, 0);
			delay = 200 / (m_ptr->cdis+1);
			if(r_ptr->flagsr & RFR_HURT_LITE) delay *= 2;
			m_ptr->energy_need += delay;
			msg_format("%s�͂��܂�̌��ɋ���ł���I",m_name);

		}				
		wiz_lite(FALSE);
	}
	else
	{
		set_kamioroshi(KAMIOROSHI_AMATERASU, 0);
		msg_print("�V�Ƒ��_������Aῂ�����������I");
		project(0, 1, py, px, plev*12, GF_HOLY_FIRE, PROJECT_KILL, -1);
		project(0, 7, py, px, plev*10, GF_DISP_ALL, PROJECT_KILL, -1);
		project_hack2(GF_LITE, 0,0,plev * 5);
		stun_monsters(plev * 6);
	}

}


/*:::�Í��̈�̈��쏢��*/
static void cast_invoke_spirits(int dir)
{
	int plev = p_ptr->lev;
	int die = randint1(100) + plev / 5;
	///del131214 virtue
	//int vir = virtue_number(V_CHANCE);
	/*
	if (vir)
	{
		if (p_ptr->virtues[vir - 1] > 0)
		{
			while (randint1(400) < p_ptr->virtues[vir - 1]) die++;
		}
		else
		{
			while (randint1(400) < (0-p_ptr->virtues[vir - 1])) die--;
		}
	}
	*/
#ifdef JP
	msg_print("���Ȃ��͎��҂����̗͂����W����...");
#else
	msg_print("You call on the power of the dead...");
#endif
	///del131214 virtue
	//if (die < 26)
	//chg_virtue(V_CHANCE, 1);

	if (die > 100)
	{
#ifdef JP
		msg_print("���Ȃ��͂��ǂ남�ǂ낵���͂̂��˂���������I");
#else
		msg_print("You feel a surge of eldritch force!");
#endif
	}


	if (die < 8)
	{
#ifdef JP
		msg_print("�Ȃ�Ă������I���Ȃ��̎���̒n�ʂ��狀�����l�e�������オ���Ă����I");
#else
		msg_print("Oh no! Mouldering forms rise from the earth around you!");
#endif

		(void)summon_specific(0, py, px, dun_level, SUMMON_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			///del131214 virtue
		//chg_virtue(V_UNLIFE, 1);
	}
	else if (die < 14)
	{
#ifdef JP
		msg_print("���󂵓�׈��ȑ��݂����Ȃ��̐S��ʂ�߂��čs����...");
#else
		msg_print("An unnamable evil brushes against your mind...");
#endif

		set_afraid(p_ptr->afraid + randint1(4) + 4);
	}
	else if (die < 26)
	{
#ifdef JP
		msg_print("���Ȃ��̓��ɑ�ʂ̗H�삽���̑��X�������������񂹂Ă���...");
#else
		msg_print("Your head is invaded by a horde of gibbering spectral voices...");
#endif

		set_confused(p_ptr->confused + randint1(4) + 4);
	}
	else if (die < 31)
	{
		poly_monster(dir, plev);
	}
	else if (die < 36)
	{
		fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir,
				  damroll(3 + ((plev - 1) / 5), 4));
	}
	else if (die < 41)
	{
		confuse_monster (dir, plev);
	}
	else if (die < 46)
	{
		fire_ball(GF_POIS, dir, 20 + (plev / 2), 3);
	}
	else if (die < 51)
	{
		(void)lite_line(dir, damroll(6, 8));
	}
	else if (die < 56)
	{
		fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir,
				  damroll(3+((plev-5)/4),8));
	}
	else if (die < 61)
	{
		fire_bolt_or_beam(beam_chance() - 10, GF_COLD, dir,
				  damroll(5+((plev-5)/4),8));
	}
	else if (die < 66)
	{
		fire_bolt_or_beam(beam_chance(), GF_ACID, dir,
				  damroll(6+((plev-5)/4),8));
	}
	else if (die < 71)
	{
		fire_bolt_or_beam(beam_chance(), GF_FIRE, dir,
				  damroll(8+((plev-5)/4),8));
	}
	else if (die < 76)
	{
		drain_life(dir, 75);
	}
	else if (die < 81)
	{
		fire_ball(GF_ELEC, dir, 30 + plev / 2, 2);
	}
	else if (die < 86)
	{
		fire_ball(GF_ACID, dir, 40 + plev, 2);
	}
	else if (die < 91)
	{
		fire_ball(GF_ICE, dir, 70 + plev, 3);
	}
	else if (die < 96)
	{
		fire_ball(GF_FIRE, dir, 80 + plev, 3);
	}
	else if (die < 101)
	{
		drain_life(dir, 100 + plev);
	}
	else if (die < 104)
	{
		earthquake(py, px, 12);
	}
	else if (die < 106)
	{
		(void)destroy_area(py, px, 13 + randint0(5), FALSE,FALSE,FALSE);
	}
	else if (die < 108)
	{
		symbol_genocide(plev+50, TRUE,0);
	}
	else if (die < 110)
	{
		dispel_monsters(120);
	}
	else
	{ /* RARE */
		dispel_monsters(150);
		slow_monsters(plev);
		sleep_monsters(plev);
		hp_player(300);
	}

	if (die < 31)
	{
#ifdef JP
		msg_print("�A�T�Ȑ����N�X�N�X�΂��B�u�����������܂��͉�X�̒��ԂɂȂ邾�낤�B�カ�҂�B�v");
#else
		msg_print("Sepulchral voices chuckle. 'Soon you will join us, mortal.'");
#endif
	}
}

/*:::�J�I�X�̈掸�s�y�i���e�B�A�g�����v�̈�V���b�t�����ʂȂǁE�E�Ƃ�����cmd5.c��do-spell.c�̗����ɂ���H*/
static void wild_magic(int spell)
{
	int counter = 0;
	int type = SUMMON_BIZARRE1 + randint0(6);

	if (type < SUMMON_BIZARRE1) type = SUMMON_BIZARRE1;
	else if (type > SUMMON_BIZARRE6) type = SUMMON_BIZARRE6;

	switch (randint1(spell) + randint1(8) + 1)
	{
	case 1:
	case 2:
	case 3:
		teleport_player(10, TELEPORT_PASSIVE);
		break;
	case 4:
	case 5:
	case 6:
		teleport_player(100, TELEPORT_PASSIVE);
		break;
	case 7:
	case 8:
		teleport_player(200, TELEPORT_PASSIVE);
		break;
	case 9:
	case 10:
	case 11:
		unlite_area(10, 3);
		break;
	case 12:
	case 13:
	case 14:
		lite_area(damroll(2, 3), 2);
		break;
	case 15:
		destroy_doors_touch();
		break;
	case 16: case 17:
		wall_breaker();
	case 18:
		sleep_monsters_touch();
		break;
	case 19:
	case 20:
		trap_creation(py, px);
		break;
	case 21:
	case 22:
		door_creation(1);
		break;
	case 23:
	case 24:
	case 25:
		aggravate_monsters(0,TRUE);
		break;
	case 26:
		earthquake(py, px, 5);
		break;
	case 27:
	case 28:
		(void)gain_random_mutation(0);
		break;
	case 29:
	case 30:
		apply_disenchant(1,0);
		break;
	case 31:
		lose_all_info();
		break;
	case 32:
		fire_ball(GF_CHAOS, 0, spell + 5, 1 + (spell / 10));
		break;
	case 33:
		wall_stone();
		break;
	case 34:
	case 35:
		while (counter++ < 8)
		{
			(void)summon_specific(0, py, px, (dun_level * 3) / 2, type, (PM_ALLOW_GROUP | PM_NO_PET));
		}
		break;
	case 36:
	case 37:
		activate_hi_summon(py, px, FALSE);
		break;
	case 38:
		(void)summon_cyber(-1, py, px);
		break;
	default:
		{
			int count = 0;
			(void)activate_ty_curse(FALSE, &count);
			break;
		}
	}
}

/*:::�g�����v���@�̃V���b�t��*/
static void cast_shuffle(void)
{
	int plev = p_ptr->lev;
	int dir;
	int die;
	///del131214 virtue
	//int vir = virtue_number(V_CHANCE);
	int i;

	/* Card sharks and high mages get a level bonus */
	///class �V���b�t�����@�ւ̃{�[�i�X
	if (cp_ptr->magicmaster)
		die = (randint1(110)) + plev / 5;
	else
		die = randint1(120);

	set_deity_bias(DBIAS_COSMOS, -1);


			///del131214 virtue
/*
	if (vir)
	{
		if (p_ptr->virtues[vir - 1] > 0)
		{
			while (randint1(400) < p_ptr->virtues[vir - 1]) die++;
		}
		else
		{
			while (randint1(400) < (0-p_ptr->virtues[vir - 1])) die--;
		}
	}
*/
#ifdef JP
	msg_print("���Ȃ��̓J�[�h��؂��Ĉꖇ������...");
#else
	msg_print("You shuffle the deck and draw a card...");
#endif
	///del131214 virtue
	//if (die < 30)
	//	chg_virtue(V_CHANCE, 1);

	if (die < 7)
	{
#ifdef JP
		msg_print("�Ȃ�Ă������I�s���t���I");
#else
		msg_print("Oh no! It's Death!");
#endif

		for (i = 0; i < randint1(3); i++)
			activate_hi_summon(py, px, FALSE);
	}
	else if (die < 14)
	{
#ifdef JP
		msg_print("�Ȃ�Ă������I�s�����t���I");
#else
		msg_print("Oh no! It's the Devil!");
#endif

		summon_specific(0, py, px, dun_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
	}
	else if (die < 18)
	{
		int count = 0;
#ifdef JP
		msg_print("�Ȃ�Ă������I�s�݂�ꂽ�j�t���I");
#else
		msg_print("Oh no! It's the Hanged Man.");
#endif

		activate_ty_curse(FALSE, &count);
	}
	else if (die < 22)
	{
#ifdef JP
		msg_print("�s�s���a�̌��t���B");
#else
		msg_print("It's the swords of discord.");
#endif

		aggravate_monsters(0,TRUE);
	}
	else if (die < 26)
	{
#ifdef JP
		msg_print("�s���ҁt���B");
#else
		msg_print("It's the Fool.");
#endif

		do_dec_stat(A_INT);
		do_dec_stat(A_WIS);
	}
	else if (die < 30)
	{
#ifdef JP
		msg_print("��ȃ����X�^�[�̊G���B");
#else
		msg_print("It's the picture of a strange monster.");
#endif

		trump_summoning(1, FALSE, py, px, (dun_level * 3 / 2), (32 + randint1(6)), PM_ALLOW_GROUP | PM_ALLOW_UNIQUE);
	}
	else if (die < 33)
	{
#ifdef JP
		msg_print("�s���t���B");
#else
		msg_print("It's the Moon.");
#endif

		unlite_area(10, 3);
	}
	else if (die < 38)
	{
#ifdef JP
		msg_print("�s�^���̗ցt���B");
#else
		msg_print("It's the Wheel of Fortune.");
#endif

		wild_magic(randint0(32));
	}
	else if (die < 40)
	{
#ifdef JP
		msg_print("�e���|�[�g�E�J�[�h���B");
#else
		msg_print("It's a teleport trump card.");
#endif

		teleport_player(10, TELEPORT_PASSIVE);
	}
	else if (die < 42)
	{
#ifdef JP
		msg_print("�s���`�t���B");
#else
		msg_print("It's Justice.");
#endif

		set_blessed(p_ptr->lev, FALSE);
	}
	else if (die < 47)
	{
#ifdef JP
		msg_print("�e���|�[�g�E�J�[�h���B");
#else
		msg_print("It's a teleport trump card.");
#endif

		teleport_player(100, TELEPORT_PASSIVE);
	}
	else if (die < 52)
	{
#ifdef JP
		msg_print("�e���|�[�g�E�J�[�h���B");
#else
		msg_print("It's a teleport trump card.");
#endif

		teleport_player(200, TELEPORT_PASSIVE);
	}
	else if (die < 60)
	{
#ifdef JP
		msg_print("�s���t���B");
#else
		msg_print("It's the Tower.");
#endif

		wall_breaker();
	}
	else if (die < 72)
	{
#ifdef JP
		msg_print("�s�ߐ��t���B");
#else
		msg_print("It's Temperance.");
#endif

		sleep_monsters_touch();
	}
	else if (die < 80)
	{
#ifdef JP
		msg_print("�s���t���B");
#else
		msg_print("It's the Tower.");
#endif

		earthquake(py, px, 5);
	}
	else if (die < 82)
	{
#ifdef JP
		msg_print("�F�D�I�ȃ����X�^�[�̊G���B");
#else
		msg_print("It's the picture of a friendly monster.");
#endif

		trump_summoning(1, TRUE, py, px, (dun_level * 3 / 2), SUMMON_BIZARRE1, 0L);
	}
	else if (die < 84)
	{
#ifdef JP
		msg_print("�F�D�I�ȃ����X�^�[�̊G���B");
#else
		msg_print("It's the picture of a friendly monster.");
#endif

		trump_summoning(1, TRUE, py, px, (dun_level * 3 / 2), SUMMON_BIZARRE2, 0L);
	}
	else if (die < 86)
	{
#ifdef JP
		msg_print("�F�D�I�ȃ����X�^�[�̊G���B");
#else
		msg_print("It's the picture of a friendly monster.");
#endif

		trump_summoning(1, TRUE, py, px, (dun_level * 3 / 2), SUMMON_BIZARRE4, 0L);
	}
	else if (die < 88)
	{
#ifdef JP
		msg_print("�F�D�I�ȃ����X�^�[�̊G���B");
#else
		msg_print("It's the picture of a friendly monster.");
#endif

		trump_summoning(1, TRUE, py, px, (dun_level * 3 / 2), SUMMON_BIZARRE5, 0L);
	}
	else if (die < 96)
	{
#ifdef JP
		msg_print("�s���l�t���B");
#else
		msg_print("It's the Lovers.");
#endif

		if (get_aim_dir(&dir))
			charm_monster(dir, MIN(p_ptr->lev, 20));
	}
	else if (die < 101)
	{
#ifdef JP
		msg_print("�s�B�ҁt���B");
#else
		msg_print("It's the Hermit.");
#endif

		wall_stone();
	}
	else if (die < 111)
	{
#ifdef JP
		msg_print("�s�R���t���B");
#else
		msg_print("It's the Judgement.");
#endif

		do_cmd_rerate(FALSE);
		if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
		{
#ifdef JP
			if(muta_erasable_count()) msg_print("�S�Ă̓ˑR�ψق��������B");
#else
			msg_print("You are cured of all mutations.");
#endif

			p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
			///mod141204 �i���ψٓK�p
			p_ptr->muta1 = p_ptr->muta1_perma;
			p_ptr->muta2 = p_ptr->muta2_perma;
			p_ptr->muta3 = p_ptr->muta3_perma;
			p_ptr->muta4 = p_ptr->muta4_perma;
			p_ptr->update |= PU_BONUS;
			handle_stuff();
		}
	}
	else if (die < 120)
	{
#ifdef JP
		msg_print("�s���z�t���B");
#else
		msg_print("It's the Sun.");
#endif
		///del131214 virtue
		//chg_virtue(V_KNOWLEDGE, 1);
		//chg_virtue(V_ENLIGHTEN, 1);
		wiz_lite(FALSE);
	}
	else
	{
#ifdef JP
		msg_print("�s���E�t���B");
#else
		msg_print("It's the World.");
#endif

		if (p_ptr->exp < PY_MAX_EXP)
		{
			s32b ee = (p_ptr->exp / 25) + 1;
			if (ee > 5000) ee = 5000;
#ifdef JP
			msg_print("�X�Ɍo����ς񂾂悤�ȋC������B");
#else
			msg_print("You feel more experienced.");
#endif

			gain_exp(ee);
		}
	}
}


/*:::�X�v�����O�t���A�@�s���������ꍇTRUE���Ԃ�*/
bool cast_spring_floor(int dist)
{
	s16b this_o_idx, next_o_idx = 0;
	int dir,target_dir;
	int tx = 0, ty = 0;
	msg_print("�ǂ��̏��𒵂ˏグ�܂����H(���p�w�肵���ꍇ�v���C���[�ׂ̗ɂȂ�܂�)");
	if (!get_aim_dir(&target_dir)) return FALSE;

	tx = px + ddx[target_dir];
	ty = py + ddy[target_dir];

	if ((target_dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}
	if (!in_bounds(ty, tx)) return FALSE;
	if( !player_has_los_bold(ty, tx))
	{
		msg_print("�����͂��Ȃ�����͌����Ȃ��B");
		return FALSE;
	}
	if(!cave_have_flag_bold(ty, tx, FF_FLOOR))
	{
		msg_print("�����͏��ł͂Ȃ��B");
		return FALSE;
	}
	while(1)
	{
		char c;

		if(!get_com("�ǂ���Ɍ����Ē��ˏグ�܂����H(1-4,5-9)",&c,(TRUE))) return FALSE;
		dir = get_keymap_dir(c);
		if( dir < 1 || dir == 5 || dir > 9) continue;
		break;

	}
	if(cave[ty][tx].info & CAVE_ICKY)
	{
		msg_print("�����̏��͓����Ȃ������B");
		return TRUE;
	}
	else
	{
		msg_print("�˔@�A���������d�|���Œ��ˏオ�����I");
	}
	/*:::�ΏۃO���b�h�̃����X�^�[�ɑ΂��鏈��*/
	if(cave[ty][tx].m_idx)
	{
		int m_idx = cave[ty][tx].m_idx;
		int i;
		monster_type *m_ptr = &m_list[m_idx];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		char m_name[80];
		monster_desc(m_name, m_ptr, 0);
		(void)set_monster_csleep(m_idx, 0);
		if( (r_ptr->flags7 & RF7_CAN_FLY) )
		{
			msg_format("%s�͒��ˏオ�������̏��I�X�Ɣ��ł���E�E",m_name);
		}
		else if( (r_ptr->flags2 & RF2_GIGANTIC) || (r_ptr->flags2 & RF2_POWERFUL)&&r_ptr->level > randint1(p_ptr->lev) )
		{
			msg_format("%s�͒��ˏオ�肩�������𓥂ݒׂ����I",m_name);
			return TRUE;
		}
		else
		{
			int dx=tx,dy=ty;
			bool move = FALSE;
			msg_format("%s�͔�яo�������ɒ��˔�΂��ꂽ�I",m_name);
			for (i = 0; i < dist; i++)
			{
				if (cave_empty_bold(dy+ddy[dir], dx+ddx[dir]))
				{
					dy += ddy[dir];
					dx += ddx[dir];
					move = TRUE;
				}
				else break;
			}
			if(move)
			{
				cave[ty][tx].m_idx=0;
				cave[dy][dx].m_idx=m_idx;
				m_ptr->fy = dy;
				m_ptr->fx = dx;
				update_mon(m_idx, TRUE);
				lite_spot(dy, dx);
				lite_spot(ty, tx);
				if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
				p_ptr->update |= (PU_MON_LITE);	
			}
			project(0, 0, dy, dx, damroll(10,10), GF_MISSILE,PROJECT_KILL, -1);
		}
	}
	/*:::@�ɑ΂��鏈��*/
	if(player_bold(ty,tx))
	{
		int i;
		int dx=tx,dy=ty;
		bool move = FALSE;
		msg_format("���Ȃ��͔�яo�������ɒ��˔�΂��ꂽ�I");
		for (i = 0; i < dist; i++)
		{
			if (cave_empty_bold(dy+ddy[dir], dx+ddx[dir]))
			{
				dy += ddy[dir];
				dx += ddx[dir];
				move = TRUE;
			}
			else break;
		}
		if(move)
		{
			teleport_player_to(dy,dx,TELEPORT_NONMAGICAL);
		}
		take_hit(DAMAGE_ATTACK,damroll(10,10),"���������˂�������",-1);
	}
	//�A�C�e���������@�^�[�Q�b�g�O���b�h�ߕӂւ܂Ƃ߂ė��Ƃ��B���Ŋm������B
	if(cave[ty][tx].o_idx)
	{	
		int i;
		int dx=tx,dy=ty;
		msg_format("���̏�ɂ������A�C�e�������˔�񂾁I");
		for (i = 0; i < dist; i++)
		{
			if ( cave_have_flag_bold(dy+ddy[dir], dx+ddx[dir],FF_PROJECT))
			{
				dy += ddy[dir];
				dx += ddx[dir];
			}
			else break;
		}


		for (this_o_idx = cave[ty][tx].o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type forge;
			object_type *q_ptr = &forge;
			object_type *o_ptr;
			o_ptr = &o_list[this_o_idx];
			next_o_idx = o_ptr->next_o_idx;

			object_copy(q_ptr, o_ptr);
			(void)drop_near(q_ptr, 10, dy, dx);
					
			floor_item_increase(this_o_idx,-255);
			floor_item_optimize(this_o_idx);
		}
	}
	return TRUE;
}


/*
 * Drop 10+1d10 meteor ball at random places near the player
 */
///mod140512 �����ύX�\�ɂ����B
void cast_meteor(int dam, int rad, int typ)
{
	int i;
	int b = 10 + randint1(10);

	int typ_use;


	for (i = 0; i < b; i++)
	{
		int y, x;
		int count;

		for (count = 0; count <= 20; count++)
		{
			int dy, dx, d;

			x = px - 8 + randint0(17);
			y = py - 8 + randint0(17);

			dx = (px > x) ? (px - x) : (x - px);
			dy = (py > y) ? (py - y) : (y - py);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));

			if (d >= 9) continue;

			if (!in_bounds(y, x) || !projectable(py, px, y, x)
			    || !cave_have_flag_bold(y, x, FF_PROJECT)) continue;

			/* Valid position */
			break;
		}

		if (count > 20) continue;

		if(!typ) //typ��0�Ȃ�Η�d�̃����_����
		{
			if(one_in_(3)) typ_use = GF_FIRE;
			else if(one_in_(2)) typ_use = GF_COLD;
			else typ_use = GF_ELEC;
		}
		else typ_use = typ;


		project(0, rad, y, x, dam, typ_use, PROJECT_KILL | PROJECT_GRID | PROJECT_JUMP | PROJECT_ITEM, -1);
	}
}


/*
 * Drop 10+1d10 disintegration ball at random places near the target
 */
/*:::aim=FALSE�ŌĂԂƎ������S�ɔ�������悤�ɂ���*/
bool cast_wrath_of_the_god(int dam, int rad, bool aim)
{
	int x, y, tx, ty;
	int nx, ny;
	int dir, i;
	int b = 10 + randint1(10);

	if(aim)
	{
		if (!get_aim_dir(&dir)) return FALSE;
		/* Use the given direction */
		tx = px + 99 * ddx[dir];
		ty = py + 99 * ddy[dir];
	
		/* Hack -- Use an actual "target" */
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}
	}
	else
	{
		tx = px;
		ty = py;
	}

	x = px;
	y = py;

	while (1)
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		ny = y;
		nx = x;
		mmove2(&ny, &nx, py, px, ty, tx);

		/* Stop at maximum range */
		if (MAX_RANGE <= distance(py, px, ny, nx)) break;

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(ny, nx, FF_PROJECT)) break;

		/* Stopped by monsters */
		if ((dir != 5) && cave[ny][nx].m_idx != 0) break;

		/* Save the new location */
		x = nx;
		y = ny;
	}
	tx = x;
	ty = y;

	for (i = 0; i < b; i++)
	{
		int count = 20, d = 0;

		while (count--)
		{
			int dx, dy;

			x = tx - 5 + randint0(11);
			y = ty - 5 + randint0(11);

			dx = (tx > x) ? (tx - x) : (x - tx);
			dy = (ty > y) ? (ty - y) : (y - ty);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
			/* Within the radius */
			if (d < 5) break;
		}

		if (count < 0) continue;

		/* Cannot penetrate perm walls */
		if (!in_bounds(y,x) ||
		    cave_stop_disintegration(y,x) ||
		    !in_disintegration_range(ty, tx, y, x))
			continue;

		project(0, rad, y, x, dam, GF_DISINTEGRATE, PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
	}

	return TRUE;
}


/*
 * An "item_tester_hook" for offer
 */
///del131221 item death �O���[�^�[�f�[���������̐��т̎��̔���
#if 0
static bool item_tester_offer(object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if (o_ptr->tval != TV_CORPSE) return (FALSE);

	if (o_ptr->sval != SV_CORPSE) return (FALSE);

	if (my_strchr("pht", r_info[o_ptr->pval].d_char)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}
#endif

/*
 * Daemon spell Summon Greater Demon
 */
///sys item �O���[�^�[�f�[��������
static bool cast_summon_greater_demon(void)
{
	int plev = p_ptr->lev;
	int item;
	cptr q, s;
	int summon_lev;
	object_type *o_ptr;
	///del131221 ���̔p�~�Ƌ��ɃO���[�^�[�f�[���������ꎞ������
	msg_format("���͔̂p�~���ꂽ�B");
	return FALSE;


	//item_tester_hook = item_tester_offer;
#ifdef JP
	q = "�ǂ̎��̂�����܂���? ";
	s = "�������鎀�̂������Ă��Ȃ��B";
#else
	q = "Sacrifice which corpse? ";
	s = "You have nothing to scrifice.";
#endif
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	summon_lev = plev * 2 / 3 + r_info[o_ptr->pval].level;

	if (summon_specific(-1, py, px, summon_lev, SUMMON_HI_DEMON, (PM_ALLOW_GROUP | PM_FORCE_PET)))
	{
#ifdef JP
		msg_print("�����̈��L���[�������B");
#else
		msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


#ifdef JP
		msg_print("�u���p�ł������܂����A����l�l�v");
#else
		msg_print("'What is thy bidding... Master?'");
#endif

		/* Decrease the item (from the pack) */
		if (item >= 0)
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}

		/* Decrease the item (from the floor) */
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}
	}
	else
	{
#ifdef JP
		msg_print("�����͌���Ȃ������B");
#else
		msg_print("No Greater Demon arrive.");
#endif
	}

	return TRUE;
}


/*
 * Start singing if the player is a Bard 
 */
/*:::��V���l�̉̂̊J�n����*/
static void start_singing(int spell, int song)
{
	/* Remember the song index */
	p_ptr->magic_num1[0] = song;

	/* Remember the index of the spell which activated the song */
	p_ptr->magic_num2[0] = spell;


	/* Now the player is singing */
	set_action(ACTION_SING);


	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}


/*
 * Stop singing if the player is a Bard 
 */

///mod140817 �̊֌W�ɋ�V���l�ȊO�̐E���֏�
void stop_singing(void)
{

	//if (p_ptr->pclass != CLASS_BARD) return;
	if (!(CHECK_MUSIC_CLASS)) return;

	//�فX�Ɣ����͂��̏����ł͉��y�I�����Ȃ�
	//���ۂ�
	if (p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI || p_ptr->pclass == CLASS_RAIKO) return;

 	/* Are there interupted song? */
	if (p_ptr->magic_num1[1])
	{
		/* Forget interupted song */
		p_ptr->magic_num1[1] = 0;
		return;
	}

	/* The player is singing? */
	if (!p_ptr->magic_num1[0]) return;

	/* Hack -- if called from set_action(), avoid recursive loop */
	if (p_ptr->action == ACTION_SING) set_action(ACTION_NONE);

	/* Message text of each song or etc. */
	do_spell(TV_BOOK_MUSIC, p_ptr->magic_num2[0], SPELL_STOP);

	p_ptr->magic_num1[0] = MUSIC_NONE;
	p_ptr->magic_num2[0] = 0;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}

/*:::��\��o����p��stop_singing()*/
void stop_tsukumo_music(void)
{

	if (p_ptr->pclass != CLASS_BENBEN && p_ptr->pclass != CLASS_YATSUHASHI) return;

 	/* Are there interupted song? */
	if (p_ptr->magic_num1[1])
	{
		/* Forget interupted song */
		p_ptr->magic_num1[1] = 0;
		return;
	}

	/* The player is singing? */
	if (!p_ptr->magic_num1[0]) return;

	/* Hack -- if called from set_action(), avoid recursive loop */
	if (p_ptr->action == ACTION_SING) set_action(ACTION_NONE);

	/* Message text of each song or etc. */
	do_spell(TV_BOOK_MUSIC, p_ptr->magic_num2[0], SPELL_STOP);

	p_ptr->magic_num1[0] = MUSIC_NONE;
	p_ptr->magic_num2[0] = 0;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}


/*:::���ې�p��stop_singing() ���y��~�����𕪂��邽�ߊ֐������@stop_singing��mode�Ƃ��������Ă���ŕ������ق����ǂ������C�����邪���X���B*/
void stop_raiko_music(void)
{

	if (p_ptr->pclass != CLASS_RAIKO) return;

 	/* Are there interupted song? */
	if (p_ptr->magic_num1[1])
	{
		/* Forget interupted song */
		p_ptr->magic_num1[1] = 0;
		return;
	}
	/* The player is singing? */
	if (!p_ptr->magic_num1[0]) return;
	/* Hack -- if called from set_action(), avoid recursive loop */
	if (p_ptr->action == ACTION_SING) set_action(ACTION_NONE);
	/* Message text of each song or etc. */
	do_spell(TV_BOOK_MUSIC, p_ptr->magic_num2[0], SPELL_STOP);

	p_ptr->magic_num1[0] = MUSIC_NONE;
	p_ptr->magic_num2[0] = 0;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}




/*:::�����p�̕�V�I�胋�[�`�� �ĂԑO��item_tester_hook��ݒ�v�@�K�����A�C�e�����Ȃ����L�����Z�������Ƃ�FALSE��Ԃ��B�A�C�e����I�����������炷�B*/
bool select_pay(int *cost)
{
	cptr q,s;
	int item;
	object_type *o_ptr;

	if(!item_tester_hook && !item_tester_tval)
	{
		msg_print("ERROR:�����㏞�I��hook/tval���ݒ�");
		return FALSE;
	}
	q = "�����̑㏞�Ƃ��ĉ�������܂���? ";
	s = "���Ȃ��͏����̑㏞�ƂȂ���̂������Ă��Ȃ��B";
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;
	if (item >= 0)
	{
		o_ptr = &inventory[item];
		*cost = (int)object_value_real(o_ptr);
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		o_ptr = &o_list[0 - item];
		*cost = (int)object_value_real(o_ptr);
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	return TRUE;
}


/*
 * Handle summoning and failure of trump spells
 */
/*:::Trump_summoning()����������
 *::: ����pet�𖳂���FORCE_PET��ALLOW_UNIQUE�͊O�Ŏw�� 
 *::: ����mode�Ƃ���FORCE_FRIENDLY���ǉ�����
 *::: */
static bool new_summoning(int num, int y, int x, int lev, int type, u32b mode)
{
	int plev = p_ptr->lev;

	int who;
	int i;
	bool success = FALSE;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev *= 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	/* Default level */
	if (!lev) lev = plev * 2 / 3 + randint1(plev / 2);

	/*:::���s���̓n�C���x���ȓG���o��*/
	if(mode & PM_NO_PET)
	{
		mode |= PM_ALLOW_UNIQUE;
		lev += randint1(lev / 2);
	}
	if(lev > 120) lev = 120;

	if (mode & PM_FORCE_PET) who = -1;
	else who = 0;

	if(cheat_hear) msg_format("Lev:%d mode:%d type:%d num:%d PET:%s NOPET:%s UNIQUE:%s",lev,mode,type,num,(mode&PM_FORCE_PET)?"TRUE":"FALSE",(mode&PM_NO_PET)?"TRUE":"FALSE",(mode&PM_ALLOW_UNIQUE)?"TRUE":"FALSE");

	for (i = 0; i < num; i++)
	{
		if (summon_specific(who, y, x, lev, type, mode)) success = TRUE;
	}

	if (!success)
	{
#ifdef JP
		msg_print("���Ȃ��̌Ăѐ��ɓ�����҂͂��Ȃ������B");
#else
		msg_print("Nobody answers to your Trump call.");
#endif
	}

	return success;
}


/*:::�u�΍��v��I�����邽�߂�hook*/
static bool item_tester_hook_ishizakura(object_type *o_ptr)
{
	if (o_ptr->tval != TV_MATERIAL) return (FALSE);
	if (o_ptr->sval != SV_MATERIAL_ISHIZAKURA) return (FALSE);
	return (TRUE);
}

/*:::�u�����̖I�����v�̍ޗ���I�����邽�߂�hook*/
static bool item_tester_hook_make_space_mead(object_type *o_ptr)
{
	if (o_ptr->tval == TV_POTION && o_ptr->sval == SV_POTION_STAR_ENLIGHTENMENT ) return (TRUE);
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_DRAGONNAIL ) return (TRUE);
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_TAKAKUSAGORI ) return (TRUE);
	if (o_ptr->tval == TV_ALCOHOL  && o_ptr->sval == SV_ALCOHOL_KUSHINADA ) return (TRUE);
	if (o_ptr->tval == TV_SWEETS   && o_ptr->sval == SV_SWEETS_MIRACLE_FRUIT ) return (TRUE);

	return (FALSE);
}
/*:::�u�ɜQ�������v��I�����邽�߂�hook*/
static bool item_tester_hook_izanagi_object(object_type *o_ptr)
{
	if (o_ptr->tval != TV_MATERIAL) return (FALSE);
	if (o_ptr->sval == SV_MATERIAL_IZANAGIOBJECT) return (TRUE);
	if (o_ptr->sval == SV_MATERIAL_RYUUZYU) return (TRUE);
	return (FALSE);
}



/*:::�h���S���֌W�̕��i*/
static bool item_tester_hook_dragonkind(object_type *o_ptr)
{
	if (o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DRAGON_DAGGER)return (TRUE);
	if (o_ptr->tval == TV_SHIELD && o_ptr->sval == SV_DRAGON_SHIELD)return (TRUE);
	if (o_ptr->tval == TV_HEAD && o_ptr->sval == SV_HEAD_DRAGONHELM)return (TRUE);
	if (o_ptr->tval == TV_GLOVES && o_ptr->sval == SV_HAND_DRAGONGLOVES)return (TRUE);
	if (o_ptr->tval == TV_BOOTS && o_ptr->sval == SV_LEG_DRAGONBOOTS)return (TRUE);
	if (o_ptr->tval == TV_DRAG_ARMOR)return (TRUE);
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_DRAGONNAIL)return (TRUE);
	if (o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_DRAGONSCALE)return (TRUE);


	/* Assume not wearable */
	return (FALSE);
}



static cptr do_life_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�y���̎���";
		if (desc) return "����Ƒ̗͂������񕜂�����B";
#else
		if (name) return "Cure Light Wounds";
		if (desc) return "Heals cut and HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(p_ptr->cut - 10);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�j��";
		if (desc) return "��莞�ԁA��������AC�Ƀ{�[�i�X�𓾂�B";
#else
		if (name) return "Bless";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�y��";
		if (desc) return "1�̂̃����X�^�[�ɏ��_���[�W��^����B��R�����Ɩ����B";
#else
		if (name) return "Cause Light Wounds";
		if (desc) return "Wounds a monster a little unless resisted.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_WOUNDS, dir, damroll(dice, sides), 0);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "���̏���";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Call Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "� & �B�������m";
		if (desc) return "�߂��̑S�Ă�㩂Ɣ��ƊK�i�����m����B";
#else
		if (name) return "Detect Doors & Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�d���̎���";
		if (desc) return "����Ƒ̗͂𒆒��x�񕜂�����B";
#else
		if (name) return "Cure Medium Wounds";
		if (desc) return "Heals cut and HP more.";
#endif
    
		{
			int dice = 4;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut((p_ptr->cut / 2) - 20);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���";
		if (desc) return "�̓��̓ł���菜���B";
#else
		if (name) return "Cure Poison";
		if (desc) return "Cure poison status.";
#endif
    
		{
			if (cast)
			{
				set_poisoned(0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�󕠏[��";
		if (desc) return "�����ɂ���B";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif
    
		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "����";
		if (desc) return "�A�C�e���ɂ��������ア�􂢂���������B";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�d��";
		if (desc) return "1�̂̃����X�^�[�ɒ��_���[�W��^����B��R�����Ɩ����B";
#else
		if (name) return "Cause Medium Wounds";
		if (desc) return "Wounds a monster unless resisted.";
#endif
    
		{
			int sides = 8 + (plev - 5) / 4;
			int dice = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_WOUNDS, dir, damroll(sides, dice), 0);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�v�����̎���";
		if (desc) return "�̗͂�啝�ɉ񕜂����A�����ƞN�O��Ԃ��S������B";
#else
		if (name) return "Cure Critical Wounds";
		if (desc) return "Heals cut, stun and HP greatly.";
#endif
    
		{
			int dice = 8;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�ϔM�ϊ�";
		if (desc) return "��莞�ԁA�Ή��Ɨ�C�ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Heat and Cold";
		if (desc) return "Gives resistance to fire and cold. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "���ӊ��m";
		if (desc) return "���ӂ̒n�`�����m����B";
#else
		if (name) return "Sense Surroundings";
		if (desc) return "Maps nearby area.";
#endif
    
		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�p�j�b�N�E�A���f�b�h";
		if (desc) return "���E���̃A���f�b�h�����|������B��R�����Ɩ����B";
#else
		if (name) return "Turn Undead";
		if (desc) return "Attempts to scare undead monsters in sight.";
#endif
    
		{
			if (cast)
			{
				turn_undead();
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�̗͉�";
		if (desc) return "�ɂ߂ċ��͂ȉ񕜎����ŁA�����ƞN�O��Ԃ��S������B";
#else
		if (name) return "Healing";
		if (desc) return "Much powerful healing magic, and heals cut and stun completely.";
#endif
    
		{
			int heal = 300;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���E�̖��";
		if (desc) return "�����̂��鏰�̏�ɁA�����X�^�[���ʂ蔲�����菢�����ꂽ�肷�邱�Ƃ��ł��Ȃ��Ȃ郋�[����`���B";
#else
		if (name) return "Glyph of Warding";
		if (desc) return "Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.";
#endif
    
		{
			if (cast)
			{
				warding_glyph();
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "*����*";
		if (desc) return "�A�C�e���ɂ����������͂Ȏ􂢂���������B";
#else
		if (name) return "Dispel Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�ӎ�";
		if (desc) return "�A�C�e�������ʂ���B";
#else
		if (name) return "Perception";
		if (desc) return "Identifies an item.";
#endif
    
		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�A���f�b�h�ގU";
		if (desc) return "���E���̑S�ẴA���f�b�h�Ƀ_���[�W��^����B";
#else
		if (name) return "Dispel Undead";
		if (desc) return "Damages all undead monsters in sight.";
#endif
    
		{
			int dice = 1;
			int sides = plev * 5;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				dispel_undead(damroll(dice, sides));
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "��̍�";
		if (desc) return "���E���̑S�Ẵ����X�^�[�𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Day of the Dove";
		if (desc) return "Attempts to charm all monsters in sight.";
#endif
    
		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				charm_monsters(power);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�v����";
		if (desc) return "1�̂̃����X�^�[�ɑ�_���[�W��^����B��R�����Ɩ����B";
#else
		if (name) return "Cause Critical Wounds";
		if (desc) return "Wounds a monster critically unless resisted.";
#endif
    
		{
			int dice = 5 + (plev - 5) / 3;
			int sides = 15;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_WOUNDS, dir, damroll(dice, sides), 0);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�A�҂̏�";
		if (desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�^���̍Ւd";
		if (desc) return "���݂̊K���č\������B";
#else
		if (name) return "Alter Reality";
		if (desc) return "Recreates current dungeon level.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				alter_reality();
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�^�E���E";
		if (desc) return "�����̂��鏰�Ǝ���8�}�X�̏��̏�ɁA�����X�^�[���ʂ蔲�����菢�����ꂽ�肷�邱�Ƃ��ł��Ȃ��Ȃ郋�[����`���B";
#else
		if (name) return "Warding True";
		if (desc) return "Creates glyphs in all adjacent squares and under you.";
#endif
    
		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				warding_glyph();
				glyph_creation();
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�s�щ�";
		if (desc) return "���̊K�̑��B���郂���X�^�[�����B�ł��Ȃ��Ȃ�B";
#else
		if (name) return "Sterilization";
		if (desc) return "Prevents any breeders on current level from breeding.";
#endif
    
		{
			if (cast)
			{
				num_repro += MAX_REPRO;
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�S���m";
		if (desc) return "�߂��̑S�Ẵ����X�^�[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if (name) return "Detection";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�A���f�b�h����";
		if (desc) return "�����̎��͂ɂ���A���f�b�h�����݂̊K�����������B��R�����Ɩ����B";
#else
		if (name) return "Annihilate Undead";
		if (desc) return "Eliminates all nearby undead monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif
    
		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide_undead(power, TRUE);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�痢��";
		if (desc) return "���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			if (cast)
			{
				wiz_lite(FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�S����";
		if (desc) return "���ׂẴX�e�[�^�X�ƌo���l���񕜂���B";
#else
		if (name) return "Restoration";
		if (desc) return "Restores all stats and experience.";
#endif
    
		{
			if (cast)
			{
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				restore_level();
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "*�̗͉�*";
		if (desc) return "�ŋ��̎����̖��@�ŁA�����ƞN�O��Ԃ��S������B";
#else
		if (name) return "Healing True";
		if (desc) return "The greatest healing magic. Heals all HP, cut and stun.";
#endif
    
		{
			int heal = 2000;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "���Ȃ�r�W����";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if (name) return "Holy Vision";
		if (desc) return "*Identifies* an item.";
#endif
    
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "���ɂ̑ϐ�";
		if (desc) return "��莞�ԁA������ϐ���t���AAC�Ɩ��@�h��\�͂��㏸������B";
#else
		if (name) return "Ultimate Resistance";
		if (desc) return "Gives ultimate resistance, bonus to AC and speed.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int v = randint1(base) + base;
				set_fast(v, FALSE);
				set_oppose_acid(v, FALSE);
				set_oppose_elec(v, FALSE);
				set_oppose_fire(v, FALSE);
				set_oppose_cold(v, FALSE);
				set_oppose_pois(v, FALSE);
				set_ultimate_res(v, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_sorcery_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�����X�^�[���m";
		if (desc) return "�߂��̑S�Ă̌����郂���X�^�[�����m����B";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�V���[�g�E�e���|�[�g";
		if (desc) return "�ߋ����̃e���|�[�g������B";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "㩂Ɣ����m";
		if (desc) return "�߂��̑S�Ă̔���㩂����m����B";
#else
		if (name) return "Detect Doors and Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "���C�g�E�G���A";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Light Area";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�p�j�b�N�E�����X�^�[";
		if (desc) return "�����X�^�[1�̂�����������B��R�����Ɩ����B";
#else
		if (name) return "Confuse Monster";
		if (desc) return "Attempts to confuse a monster.";
#endif
    
		{
			int power = (plev * 3) / 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				confuse_monster(dir, power);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�e���|�[�g";
		if (desc) return "�������̃e���|�[�g������B";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�X���[�v�E�����X�^�[";
		if (desc) return "�����X�^�[1�̂𖰂点��B��R�����Ɩ����B";
#else
		if (name) return "Sleep Monster";
		if (desc) return "Attempts to sleep a monster.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				sleep_monster(dir, plev);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���͏[�U";
		if (desc) return "��/���@�_�̏[�U�񐔂𑝂₷���A�[�U���̃��b�h�̏[�U���Ԃ����炷�B";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "���@�̒n�}";
		if (desc) return "���ӂ̒n�`�����m����B";
#else
		if (name) return "Magic Mapping";
		if (desc) return "Maps nearby area.";
#endif
    
		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�Ӓ�";
		if (desc) return "�A�C�e�������ʂ���B";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif
    
		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�X���E�E�����X�^�[";
		if (desc) return "�����X�^�[1�̂���������B��R�����Ɩ����B";
#else
		if (name) return "Slow Monster";
		if (desc) return "Attempts to slow a monster.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				slow_monster(dir, plev);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "���ӃX���[�v";
		if (desc) return "���E���̑S�Ẵ����X�^�[�𖰂点��B��R�����Ɩ����B";
#else
		if (name) return "Mass Sleep";
		if (desc) return "Attempts to sleep all monsters in sight.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				sleep_monsters(plev);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�e���|�[�g�E�����X�^�[";
		if (desc) return "�����X�^�[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�X�s�[�h";
		if (desc) return "��莞�ԁA��������B";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif
    
		{
			int base = plev;
			int sides = 20 + plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�^�E���m";
		if (desc) return "�߂��̑S�Ẵ����X�^�[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if (name) return "Detection True";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�^�E�Ӓ�";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if (name) return "Identify True";
		if (desc) return "*Identifies* an item.";
#endif
    
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "���̂ƍ��󊴒m";
		if (desc) return "�߂��̑S�ẴA�C�e���ƍ�������m����B";
#else
		if (name) return "Detect items and Treasure";
		if (desc) return "Detects all treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_objects_normal(rad);
				detect_treasure(rad);
				detect_objects_gold(rad);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�`���[���E�����X�^�[";
		if (desc) return "�����X�^�[1�̂𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Charm Monster";
		if (desc) return "Attempts to charm a monster.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				charm_monster(dir, power);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "���_���m";
		if (desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#else
		if (name) return "Sense Minds";
		if (desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�X�ړ�";
		if (desc) return "�X�ֈړ�����B�n��ɂ���Ƃ������g���Ȃ��B";
#else
		if (name) return "Teleport to town";
		if (desc) return "Teleport to a town which you choose in a moment. Can only be used outdoors.";
#endif
    
		{
			if (cast)
			{
				if (!tele_town(TRUE)) return NULL;
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "���ȕ���";
		if (desc) return "���݂̎����̏�Ԃ����S�ɒm��B";
#else
		if (name) return "Self Knowledge";
		if (desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif
    
		{
			if (cast)
			{
				self_knowledge();
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�e���|�[�g�E���x��";
		if (desc) return "�u���ɏォ���̊K�Ƀe���|�[�g����B";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				if (!get_check("�{���ɑ��̊K�Ƀe���|�[�g���܂����H")) return NULL;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�A�҂̎���";
		if (desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�����̔�";
		if (desc) return "�Z�������̎w�肵���ꏊ�Ƀe���|�[�g����B";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif
    
		{
			int range = plev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("�����̔����J�����B�ړI�n��I��ŉ������B");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(D_DOOR_NORMAL)) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "����";
		if (desc) return "�����X�^�[�̑����A�c��̗́A�ő�̗́A�X�s�[�h�A���̂�m��B";
#else
		if (name) return "Probing";
		if (desc) return "Proves all monsters' alignment, HP, speed and their true character.";
#endif
    
		{
			if (cast)
			{
				probing();
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�����̃��[��";
		if (desc) return "�����̂��鏰�̏�ɁA�����X�^�[���ʂ�Ɣ������ă_���[�W��^���郋�[����`���B";
#else
		if (name) return "Explosive Rune";
		if (desc) return "Sets a glyph under you. The glyph will explode when a monster moves on it.";
#endif
    
		{
			int dice = 7;
			int sides = 7;
			int base = plev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				explosive_rune();
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�O����";
		if (desc) return "�A�C�e���������̑����ֈړ�������B";
#else
		if (name) return "Telekinesis";
		if (desc) return "Pulls a distant item close to you.";
#endif
    
		{
			int weight = plev * 15;

			if (info) return info_weight(weight);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fetch(dir, weight, FALSE);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�痢��";
		if (desc) return "���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B����ɁA��莞�ԃe���p�V�[�\�͂𓾂�B";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				chg_virtue(V_KNOWLEDGE, 1);
				chg_virtue(V_ENLIGHTEN, 1);

				wiz_lite(FALSE);

				if (!p_ptr->telepathy)
				{
					set_tim_esp(randint1(sides) + base, FALSE);
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�����̎���";
		if (desc) return "���E���̑S�Ẵ����X�^�[�𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Charm monsters";
		if (desc) return "Attempts to charm all monsters in sight.";
#endif
    
		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				charm_monsters(power);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�B���p";
		if (desc) return "�A�C�e��1�������ɕς���B";
#else
		if (name) return "Alchemy";
		if (desc) return "Turns an item into 1/3 of its value in gold.";
#endif
    
		{
			if (cast)
			{
				if (!alchemy()) return NULL;
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�����Ǖ�";
		if (desc) return "���E���̑S�Ẵ����X�^�[���e���|�[�g������B��R�����Ɩ����B";
#else
		if (name) return "Banishment";
		if (desc) return "Teleports all monsters in sight away unless resisted.";
#endif
    
		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				banish_monsters(power);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�����̋�";
		if (desc) return "��莞�ԁA�_���[�W���󂯂Ȃ��Ȃ�o���A�𒣂�B�؂ꂽ�u�Ԃɏ����^�[���������̂Œ��ӁB";
#else
		if (name) return "Globe of Invulnerability";
		if (desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks or duration time is exceeded.";
#endif
    
		{
			int base = 4;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_invuln(randint1(base) + base, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_nature_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
	static const char s_rng[] = "�˒�";
#else
	static const char s_dam[] = "dam ";
	static const char s_rng[] = "rng ";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�����X�^�[���m";
		if (desc) return "�߂��̑S�Ă̌����郂���X�^�[�����m����B";
#else
		if (name) return "Detect Creatures";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "���";
		if (desc) return "�d���̒Z���r�[������B";
#else
		if (name) return "Lightning";
		if (desc) return "Fires a short beam of lightning.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int range = plev / 6 + 2;

			if (info) return format("%s%dd%d %s%d", s_dam, dice, sides, s_rng, range);

			if (cast)
			{
				project_length = range;

				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "㩂Ɣ����m";
		if (desc) return "�߂��̑S�Ă�㩂Ɣ������m����B";
#else
		if (name) return "Detect Doors and Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�H�Ɛ���";
		if (desc) return "�H��������o���B";
#else
		if (name) return "Produce Food";
		if (desc) return "Produces a Ration of Food.";
#endif
    
		{
			if (cast)
			{
				object_type forge, *q_ptr = &forge;

#ifdef JP
				msg_print("�H���𐶐������B");
#else
				msg_print("A food ration is produced.");
#endif

				/* Create the food ration */
				///item �H�Ɛ���
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

				/* Drop the object from heaven */
				drop_near(q_ptr, -1, py, px);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "���̌�";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Daylight";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);

				if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
				{
#ifdef JP
					msg_print("���̌������Ȃ��̓��̂��ł������I");
#else
					msg_print("The daylight scorches your flesh!");
#endif

#ifdef JP
					take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "���̌�", -1);
#else
					take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "daylight", -1);
#endif
				}
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�����K��";
		if (desc) return "����1�̂𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Animal Taming";
		if (desc) return "Attempts to charm an animal.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				charm_animal(dir, power);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���ւ̑ϐ�";
		if (desc) return "��莞�ԁA��C�A���A�d���ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Environment";
		if (desc) return "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���ƓŎ���";
		if (desc) return "�����S�������A�ł�̂��犮�S�Ɏ�菜���A�̗͂������񕜂�����B";
#else
		if (name) return "Cure Wounds & Poison";
		if (desc) return "Heals all cut and poison status. Heals HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(0);
				set_poisoned(0);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "��Ηn��";
		if (desc) return "�ǂ�n�����ď��ɂ���B";
#else
		if (name) return "Stone to Mud";
		if (desc) return "Turns one rock square to mud.";
#endif
    
		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wall_to_mud(dir, 20 + randint1(30));
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�A�C�X�E�{���g";
		if (desc) return "��C�̃{���g�������̓r�[������B";
#else
		if (name) return "Frost Bolt";
		if (desc) return "Fires a bolt or beam of cold.";
#endif
    
		{
			int dice = 3 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt_or_beam(beam_chance() - 10, GF_COLD, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "���R�̊o��";
		if (desc) return "���ӂ̒n�`�����m���A�߂���㩁A���A�K�i�A�S�Ẵ����X�^�[�����m����B";
#else
		if (name) return "Nature Awareness";
		if (desc) return "Maps nearby area. Detects all monsters, traps, doors and stairs.";
#endif
    
		{
			int rad1 = DETECT_RAD_MAP;
			int rad2 = DETECT_RAD_DEFAULT;

			if (info) return info_radius(MAX(rad1, rad2));

			if (cast)
			{
				map_area(rad1);
				detect_traps(rad2, TRUE);
				detect_doors(rad2);
				detect_stairs(rad2);
				detect_monsters_normal(rad2);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�t�@�C�A�E�{���g";
		if (desc) return "�Ή��̃{���g�������̓r�[������B";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 5 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt_or_beam(beam_chance() - 10, GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "���z����";
		if (desc) return "��������B��������������X�^�[�Ɍ��ʂ�����B";
#else
		if (name) return "Ray of Sunlight";
		if (desc) return "Fires a beam of light which damages to light-sensitive monsters.";
#endif
    
		{
			int dice = 6;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				msg_print("���z���������ꂽ�B");
#else
				msg_print("A line of sunlight appears.");
#endif

				lite_line(dir, damroll(6, 8));
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "������";
		if (desc) return "���E���̑S�Ẵ����X�^�[������������B��R�����Ɩ����B";
#else
		if (name) return "Entangle";
		if (desc) return "Attempts to slow all monsters in sight.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				slow_monsters(plev);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "��������";
		if (desc) return "������1�̏�������B";
#else
		if (name) return "Summon Animal";
		if (desc) return "Summons an animal.";
#endif
    
		{
			if (cast)
			{
				if (!(summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET))))
				{
#ifdef JP
					msg_print("�����͌���Ȃ������B");
#else
					msg_print("No animals arrive.");
#endif
				}
				break;
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�򑐎���";
		if (desc) return "�̗͂�啝�ɉ񕜂����A�����A�N�O��ԁA�ł���S������B";
#else
		if (name) return "Herbal Healing";
		if (desc) return "Heals HP greatly. And heals cut, stun and poison completely.";
#endif
    
		{
			int heal = 500;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�K�i����";
		if (desc) return "�����̂���ʒu�ɊK�i�����B";
#else
		if (name) return "Stair Building";
		if (desc) return "Creates a stair which goes down or up.";
#endif
    
		{
			if (cast)
			{
				stair_creation();
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "���Ή�";
		if (desc) return "��莞�ԁAAC���㏸������B";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif
    
		{
			int base = 20;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�^�E�ϐ�";
		if (desc) return "��莞�ԁA�_�A�d���A���A��C�A�łɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resistance True";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�X�ёn��";
		if (desc) return "���͂ɖ؂����o���B";
#else
		if (name) return "Forest Creation";
		if (desc) return "Creates trees in all adjacent squares.";
#endif
    
		{
			if (cast)
			{
				tree_creation();
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�����F�a";
		if (desc) return "���E���̑S�Ă̓����𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Animal Friendship";
		if (desc) return "Attempts to charm all animals in sight.";
#endif
    
		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				charm_animals(power);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "������";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if (name) return "Stone Tell";
		if (desc) return "*Identifies* an item.";
#endif
    
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�΂̕�";
		if (desc) return "�����̎��͂ɉԛ���̕ǂ����B";
#else
		if (name) return "Wall of Stone";
		if (desc) return "Creates granite walls in all adjacent squares.";
#endif
    
		{
			if (cast)
			{
				wall_stone();
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "���H�h�~";
		if (desc) return "�A�C�e�����_�ŏ����Ȃ��悤���H����B";
#else
		if (name) return "Protect from Corrosion";
		if (desc) return "Makes an equipment acid-proof.";
#endif
    
		{
			if (cast)
			{
				if (!rustproof()) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�n�k";
		if (desc) return "���͂̃_���W������h�炵�A�ǂƏ��������_���ɓ���ς���B";
#else
		if (name) return "Earthquake";
		if (desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif
    
		{
			int rad = 10;

			if (info) return info_radius(rad);

			if (cast)
			{
				earthquake(py, px, rad);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�J�}�C�^�`";
		if (desc) return "�S�����Ɍ������čU������B";
#else
		if (name) return "Cyclone";
		if (desc) return "Attacks all adjacent monsters.";
#endif
    
		{
			///sys �J�}�C�^�`�̖��@
			if (cast)
			{
				int y = 0, x = 0;
				cave_type       *c_ptr;
				monster_type    *m_ptr;

				for (dir = 0; dir < 8; dir++)
				{
					y = py + ddy_ddd[dir];
					x = px + ddx_ddd[dir];
					c_ptr = &cave[y][x];

					/* Get the monster */
					m_ptr = &m_list[c_ptr->m_idx];

					/* Hack -- attack monsters */
					if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
						py_attack(y, x, 0);
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�u���U�[�h";
		if (desc) return "����ȗ�C�̋�����B";
#else
		if (name) return "Blizzard";
		if (desc) return "Fires a huge ball of cold.";
#endif
    
		{
			int dam = 70 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_COLD, dir, dam, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "��ȗ�";
		if (desc) return "����ȓd���̋�����B";
#else
		if (name) return "Lightning Storm";
		if (desc) return "Fires a huge electric ball.";
#endif
    
		{
			int dam = 90 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_ELEC, dir, dam, rad);
				break;
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�Q��";
		if (desc) return "����Ȑ��̋�����B";
#else
		if (name) return "Whirlpool";
		if (desc) return "Fires a huge ball of water.";
#endif
    
		{
			int dam = 100 + plev * 3 / 2;
			int rad = plev / 12 + 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_WATER, dir, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�z������";
		if (desc) return "�����𒆐S�Ƃ������̋��𔭐�������B����ɁA���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B";
#else
		if (name) return "Call Sunlight";
		if (desc) return "Generates ball of light centered on you. Maps and lights whole dungeon level. Knows all objects location.";
#endif
    
		{
			int dam = 150;
			int rad = 8;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				fire_ball(GF_LITE, 0, dam, rad);
				chg_virtue(V_KNOWLEDGE, 1);
				chg_virtue(V_ENLIGHTEN, 1);
				wiz_lite(FALSE);

				if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
				{
#ifdef JP
					msg_print("���������Ȃ��̓��̂��ł������I");
#else
					msg_print("The sunlight scorches your flesh!");
#endif

#ifdef JP
					take_hit(DAMAGE_NOESCAPE, 50, "����", -1);
#else
					take_hit(DAMAGE_NOESCAPE, 50, "sunlight", -1);
#endif
				}
			}
		}
		break;


#ifdef JP
		if (name) return "���R�̋���";
		if (desc) return "�߂��̑S�Ẵ����X�^�[�Ƀ_���[�W��^���A�n�k���N�����A�����𒆐S�Ƃ��������̋��𔭐�������B";
#else
		if (name) return "Nature's Wrath";
		if (desc) return "Damages all monsters in sight. Makes quake. Generates disintegration ball centered on you.";
#endif
    
		{
			int d_dam = 4 * plev;
			int b_dam = (100 + plev) * 2;
			int b_rad = 1 + plev / 12;
			int q_rad = 20 + plev / 2;

			if (info) return format("%s%d+%d", s_dam, d_dam, b_dam/2);

			if (cast)
			{
				dispel_monsters(d_dam);
				earthquake(py, px, q_rad);
				project(0, b_rad, py, px, b_dam, GF_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;
	}

	return "";
}


static cptr do_chaos_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
	static const char s_random[] = "�����_��";
#else
	static const char s_dam[] = "dam ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�}�W�b�N�E�~�T�C��";
		if (desc) return "�ア���@�̖����B";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + ((plev - 1) / 5);
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�g���b�v/�h�A�j��";
		if (desc) return "�אڂ���㩂Ɣ���j�󂷂�B";
#else
		if (name) return "Trap / Door Destruction";
		if (desc) return "Destroys all traps in adjacent squares.";
#endif
    
		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				destroy_doors_touch();
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�M��";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Flash of Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�����̎�";
		if (desc) return "���������������U�����ł���悤�ɂ���B";
#else
		if (name) return "Touch of Confusion";
		if (desc) return "Attempts to confuse the next monster that you hit.";
#endif
    
		{
			if (cast)
			{
				if (!(p_ptr->special_attack & ATTACK_CONFUSE))
				{
#ifdef JP
					msg_print("���Ȃ��̎�͌���n�߂��B");
#else
					msg_print("Your hands start glowing.");
#endif

					p_ptr->special_attack |= ATTACK_CONFUSE;
					p_ptr->redraw |= (PR_STATUS);
				}
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�����y��";
		if (desc) return "���@�̋�����B";
#else
		if (name) return "Mana Burst";
		if (desc) return "Fires a ball of magic.";
#endif
    
		{
			int dice = 3;
			int sides = 5;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			///class ���C�W�n�͈З͂��㏸���閂�@
			if (p_ptr->pclass == CLASS_MAGE ||
			    p_ptr->pclass == CLASS_HIGH_MAGE ||
			    p_ptr->pclass == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MISSILE, dir, damroll(dice, sides) + base, rad);

				/*
				 * Shouldn't actually use GF_MANA, as
				 * it will destroy all items on the
				 * floor
				 */
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�t�@�C�A�E�{���g";
		if (desc) return "���̃{���g�������̓r�[������B";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�͂̌�";
		if (desc) return "���������ȕ����̋�����B";
#else
		if (name) return "Fist of Force";
		if (desc) return "Fires a tiny ball of disintegration.";
#endif
    
		{
			int dice = 8 + ((plev - 5) / 4);
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir,
					damroll(dice, sides), 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�e���|�[�g";
		if (desc) return "�������̃e���|�[�g������B";
#else
		if (name) return "Teleport Self";
		if (desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�����_�[";
		if (desc) return "�����X�^�[�Ƀ����_���Ȍ��ʂ�^����B";
#else
		if (name) return "Wonder";
		if (desc) return "Fires something with random effects.";
#endif
    
		{
			if (info) return s_random;

			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;

				cast_wonder(dir);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�J�I�X�E�{���g";
		if (desc) return "�J�I�X�̃{���g�������̓r�[������B";
#else
		if (name) return "Chaos Bolt";
		if (desc) return "Fires a bolt or ball of chaos.";
#endif
    
		{
			int dice = 10 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_CHAOS, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�\�j�b�N�E�u�[��";
		if (desc) return "�����𒆐S�Ƃ��������̋��𔭐�������B";
#else
		if (name) return "Sonic Boom";
		if (desc) return "Generates a ball of sound centered on you.";
#endif
    
		{
			int dam = 60 + plev;
			int rad = plev / 10 + 2;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
#ifdef JP
				msg_print("�h�[���I�������h�ꂽ�I");
#else
				msg_print("BOOM! Shake the room!");
#endif

				project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�j�ł̖�";
		if (desc) return "�����Ȗ��͂̃r�[������B";
#else
		if (name) return "Doom Bolt";
		if (desc) return "Fires a beam of pure mana.";
#endif
    
		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_MANA, dir, damroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�t�@�C�A�E�{�[��";
		if (desc) return "���̋�����B";
#else
		if (name) return "Fire Ball";
		if (desc) return "Fires a ball of fire.";
#endif
    
		{
			int dam = plev + 55;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�e���|�[�g�E�A�E�F�C";
		if (desc) return "�����X�^�[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if (name) return "Teleport Other";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�j��̌��t";
		if (desc) return "���ӂ̃A�C�e���A�����X�^�[�A�n�`��j�󂷂�B";
#else
		if (name) return "Word of Destruction";
		if (desc) return "Destroy everything in nearby area.";
#endif
    
		{
			int base = 12;
			int sides = 4;

			if (cast)
			{
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���O���X����";
		if (desc) return "����ȃJ�I�X�̋�����B";
#else
		if (name) return "Invoke Logrus";
		if (desc) return "Fires a huge ball of chaos.";
#endif
    
		{
			int dam = plev * 2 + 99;
			int rad = plev / 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_CHAOS, dir, dam, rad);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "���ҕϗe";
		if (desc) return "�����X�^�[1�̂�ϐg������B��R�����Ɩ����B";
#else
		if (name) return "Polymorph Other";
		if (desc) return "Attempts to polymorph a monster.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				poly_monster(dir, plev);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�A�����";
		if (desc) return "�S�����ɑ΂��ēd���̃r�[������B";
#else
		if (name) return "Chain Lightning";
		if (desc) return "Fires lightning beams in all directions.";
#endif
    
		{
			int dice = 5 + plev / 10;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				for (dir = 0; dir <= 9; dir++)
					fire_beam(GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "���͕���";
		if (desc) return "��/���@�_�̏[�U�񐔂𑝂₷���A�[�U���̃��b�h�̏[�U���Ԃ����炷�B";
#else
		if (name) return "Arcane Binding";
		if (desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			int power = 90;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "���q����";
		if (desc) return "����ȕ����̋�����B";
#else
		if (name) return "Disintegrate";
		if (desc) return "Fires a huge ball of disintegration.";
#endif
    
		{
			int dam = plev + 70;
			int rad = 3 + plev / 40;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir, dam, rad);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�����ϗe";
		if (desc) return "���݂̊K���č\������B";
#else
		if (name) return "Alter Reality";
		if (desc) return "Recreates current dungeon level.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				alter_reality();
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�}�W�b�N�E���P�b�g";
		if (desc) return "���P�b�g�𔭎˂���B";
#else
		if (name) return "Magic Rocket";
		if (desc) return "Fires a magic rocket.";
#endif
    
		{
			int dam = 120 + plev * 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_print("���P�b�g���ˁI");
#else
				msg_print("You launch a rocket!");
#endif

				fire_rocket(GF_ROCKET, dir, dam, rad);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "���ׂ̐n";
		if (desc) return "����ɃJ�I�X�̑���������B";
#else
		if (name) return "Chaos Branding";
		if (desc) return "Makes current weapon a Chaotic weapon.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(2);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "��������";
		if (desc) return "������1�̏�������B";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif
    
		{
			if (cast)
			{
				u32b summon_flags = 0L;
				bool pet = !one_in_(3);

				if (pet) summon_flags |= PM_FORCE_PET;
				else summon_flags |= PM_NO_PET;
				if (!(pet && (plev < 50))) summon_flags |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, SUMMON_DEMON, summon_flags))
				{
#ifdef JP
					msg_print("�����̈��L���[�������B");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif

					if (pet)
					{
#ifdef JP
						msg_print("�u���p�ł������܂����A����l�l�v");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("�u�ڂ����҂�A��͓��̉��l�ɂ��炸�I ���O�̍��𒸂����I�v");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�d�͌���";
		if (desc) return "�d�͂̃r�[������B";
#else
		if (name) return "Beam of Gravity";
		if (desc) return "Fires a beam of gravity.";
#endif
    
		{
			int dice = 9 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_GRAVITY, dir, damroll(dice, sides));
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�����Q";
		if (desc) return "�����̎��ӂ�覐΂𗎂Ƃ��B";
#else
		if (name) return "Meteor Swarm";
		if (desc) return "Makes meteor balls fall down to nearby random locations.";
#endif
    
		{
			int dam = plev * 2;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				cast_meteor(dam, rad,GF_METEOR);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "���̈ꌂ";
		if (desc) return "�����𒆐S�Ƃ���������ȉ��̋��𔭐�������B";
#else
		if (name) return "Flame Strike";
		if (desc) return "Generate a huge ball of fire centered on you.";
#endif
    
		{
			int dam = 300 + 3 * plev;
			int rad = 8;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				fire_ball(GF_FIRE, 0, dam, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "���׏���";
		if (desc) return "�����_���ȑ����̋���r�[���𔭐�������B";
#else
		if (name) return "Call Chaos";
		if (desc) return "Generate random kind of balls or beams.";
#endif
    
		{
			if (info) return format("%s150 / 250", s_dam);

			if (cast)
			{
				call_chaos();
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "���ȕϗe";
		if (desc) return "������ϐg�����悤�Ƃ���B";
#else
		if (name) return "Polymorph Self";
		if (desc) return "Polymorphs yourself.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				if (!get_check("�ϐg���܂��B��낵���ł����H")) return NULL;
#else
				if (!get_check("You will polymorph yourself. Are you sure? ")) return NULL;
#endif
				do_poly_self();
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "���̗͂�";
		if (desc) return "���ɋ��͂ŋ���ȏ����Ȗ��͂̋�����B";
#else
		if (name) return "Mana Storm";
		if (desc) return "Fires an extremely powerful huge ball of pure mana.";
#endif
    
		{
			int dam = 300 + plev * 4;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MANA, dir, dam, rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "���O���X�̃u���X";
		if (desc) return "���ɋ��͂ȃJ�I�X�̋�����B";
#else
		if (name) return "Breathe Logrus";
		if (desc) return "Fires an extremely powerful ball of chaos.";
#endif
    
		{
			int dam = p_ptr->chp;
			int rad = 2;


			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_CHAOS, dir, dam, rad);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "��������";
		if (desc) return "�����Ɏ��͂Ɍ������āA���P�b�g�A�����Ȗ��͂̋��A���ː��p�����̋�����B�������A�ǂɗאڂ��Ďg�p����ƍL�͈͂�j�󂷂�B";
#else
		if (name) return "Call the Void";
		if (desc) return "Fires rockets, mana balls and nuclear waste balls in all directions each unless you are not adjacent to any walls. Otherwise *destroys* huge area.";
#endif
    
		{
			if (info) return format("%s3 * 175", s_dam);

			if (cast)
			{
				call_the_();
			}
		}
		break;
	}

	return "";
}


static cptr do_death_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
	static const char s_random[] = "�����_��";
#else
	static const char s_dam[] = "dam ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "���������m";
		if (desc) return "�߂��̐����̂Ȃ������X�^�[�����m����B";
#else
		if (name) return "Detect Unlife";
		if (desc) return "Detects all nonliving monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_nonliving(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "��E�e";
		if (desc) return "���������Ȏ׈��ȗ͂����{�[������B�P�ǂȃ����X�^�[�ɂ͑傫�ȃ_���[�W��^����B";
#else
		if (name) return "Malediction";
		if (desc) return "Fires a tiny ball of evil power which hurts good monsters greatly.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int rad = 0;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				/*
				 * A radius-0 ball may (1) be aimed at
				 * objects etc., and will affect them;
				 * (2) may be aimed at ANY visible
				 * monster, unlike a 'bolt' which must
				 * travel to the monster.
				 */

				fire_ball(GF_HELL_FIRE, dir, damroll(dice, sides), rad);

				if (one_in_(5))
				{
					/* Special effect first */
					int effect = randint1(1000);

					if (effect == 666)
						fire_ball_hide(GF_DEATH_RAY, dir, plev * 200, 0);
					else if (effect < 500)
						fire_ball_hide(GF_TURN_ALL, dir, plev, 0);
					else if (effect < 800)
						fire_ball_hide(GF_OLD_CONF, dir, plev, 0);
					else
						fire_ball_hide(GF_STUN, dir, plev, 0);
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�׈����m";
		if (desc) return "�߂��̎׈��ȃ����X�^�[�����m����B";
#else
		if (name) return "Detect Evil";
		if (desc) return "Detects all evil monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_evil(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "���L�_";
		if (desc) return "�ł̋�����B";
#else
		if (name) return "Stinking Cloud";
		if (desc) return "Fires a ball of poison.";
#endif
    
		{
			int dam = 10 + plev / 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POIS, dir, dam, rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "��������";
		if (desc) return "1�̂̃����X�^�[�𖰂点��B��R�����Ɩ����B";
#else
		if (name) return "Black Sleep";
		if (desc) return "Attempts to sleep a monster.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				sleep_monster(dir, plev);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�ϓ�";
		if (desc) return "��莞�ԁA�łւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���Q";
		if (desc) return "�����X�^�[1�̂����|�����A�N�O������B��R�����Ɩ����B";
#else
		if (name) return "Horrify";
		if (desc) return "Attempts to scare and stun a monster.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fear_monster(dir, power);
				stun_monster(dir, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�A���f�b�h�]��";
		if (desc) return "�A���f�b�h1�̂𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Enslave Undead";
		if (desc) return "Attempts to charm an undead monster.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				control_one_undead(dir, power);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�G���g���s�[�̋�";
		if (desc) return "�����̂���҂Ɍ��ʂ̂��鋅����B";
#else
		if (name) return "Orb of Entropy";
		if (desc) return "Fires a ball which damages living monsters.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;
///class ���C�W�n�͈З͂��オ�閂�@
			if (p_ptr->pclass == CLASS_MAGE ||
			    p_ptr->pclass == CLASS_HIGH_MAGE ||
			    p_ptr->pclass == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_OLD_DRAIN, dir, damroll(dice, dice) + base, rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�n���̖�";
		if (desc) return "�n���̃{���g�������̓r�[������B";
#else
		if (name) return "Nether Bolt";
		if (desc) return "Fires a bolt or beam of nether.";
#endif
    
		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_NETHER, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�E�C�_";
		if (desc) return "�����𒆐S�Ƃ����ł̋��𔭐�������B";
#else
		if (name) return "Cloud kill";
		if (desc) return "Generate a ball of poison centered on you.";
#endif
    
		{
			int dam = (30 + plev) * 2;
			int rad = plev / 10 + 2;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				project(0, rad, py, px, dam, GF_POIS, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�����X�^�[����";
		if (desc) return "�����X�^�[1�̂���������B�o���l��A�C�e���͎�ɓ���Ȃ��B��R�����Ɩ����B";
#else
		if (name) return "Genocide One";
		if (desc) return "Attempts to vanish a monster.";
#endif
    
		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_GENOCIDE, dir, power, 0);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�ł̐n";
		if (desc) return "����ɓł̑���������B";
#else
		if (name) return "Poison Branding";
		if (desc) return "Makes current weapon poison branded.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(3);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�z���h���C��";
		if (desc) return "�����X�^�[1�̂��琶���͂��z���Ƃ�B�z���Ƃ��������͂ɂ���Ė����x���オ��B";
#else
		if (name) return "Vampiric Drain";
		if (desc) return "Absorbs some HP from a monster and gives them to you. You will also gain nutritional sustenance from this.";
#endif
    
		{
			int dice = 1;
			int sides = plev * 2;
			int base = plev * 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int dam = base + damroll(dice, sides);

				if (!get_aim_dir(&dir)) return NULL;

				if (drain_life(dir, dam))
				{
	///del131214 virtue
					//chg_virtue(V_SACRIFICE, -1);
					//chg_virtue(V_VITALITY, -1);

					hp_player(dam);

					/*
					 * Gain nutritional sustenance:
					 * 150/hp drained
					 *
					 * A Food ration gives 5000
					 * food points (by contrast)
					 * Don't ever get more than
					 * "Full" this way But if we
					 * ARE Gorged, it won't cure
					 * us
					 */
					dam = p_ptr->food + MIN(5000, 100 * dam);

					/* Not gorged already */
					if (p_ptr->food < PY_FOOD_MAX)
						set_food(dam >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dam);
				}
			}
		}
		break;

		///sysdel ���l�Ԃ�
	case 14:
#ifdef JP
		if (name) return "�����̏p";
		if (desc) return "���͂̎��̂⍜�𐶂��Ԃ��B";
#else
		if (name) return "Animate dead";
		if (desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif
    
		{
			if (cast)
			{
				///del131214 dead
				//animate_dead(0, py, px);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���E";
		if (desc) return "�w�肵�������̃����X�^�[�����݂̊K�����������B��R�����Ɩ����B";
#else
		if (name) return "Genocide";
		if (desc) return "Eliminates an entire class of monster, exhausting you.  Powerful or unique monsters may resist.";
#endif
    
		{
			int power = plev+50;

			if (info) return info_power(power);

			if (cast)
			{
				symbol_genocide(power, TRUE,0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "����m��";
		if (desc) return "����m�����A���|����������B";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_shero(randint1(base) + base, FALSE);
				hp_player(30);
				set_afraid(0);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "���쏢��";
		if (desc) return "�����_���ŗl�X�Ȍ��ʂ��N����B";
#else
		if (name) return "Invoke Spirits";
		if (desc) return "Causes random effects.";
#endif
    
		{
			if (info) return s_random;

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				cast_invoke_spirits(dir);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�Í��̖�";
		if (desc) return "�Í��̃{���g�������̓r�[������B";
#else
		if (name) return "Dark Bolt";
		if (desc) return "Fires a bolt or beam of darkness.";
#endif
    
		{
			int dice = 4 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_DARK, dir, damroll(dice, sides));
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "������m";
		if (desc) return "����m�����A���|���������A��������B";
#else
		if (name) return "Battle Frenzy";
		if (desc) return "Gives another bonus to hit and HP, immunity to fear for a while. Hastes you. But decreases AC.";
#endif
    
		{
			int b_base = 25;
			int sp_base = plev / 2;
			int sp_sides = 20 + plev / 2;

			if (info) return info_duration(b_base, b_base);

			if (cast)
			{
				set_shero(randint1(25) + 25, FALSE);
				hp_player(30);
				set_afraid(0);
				set_fast(randint1(sp_sides) + sp_base, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�z���̐n";
		if (desc) return "����ɋz���̑���������B";
#else
		if (name) return "Vampiric Branding";
		if (desc) return "Makes current weapon Vampiric.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(4);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�^�E�z��";
		if (desc) return "�����X�^�[1�̂��琶���͂��z���Ƃ�B�z���Ƃ��������͂ɂ���đ̗͂��񕜂���B";
#else
		if (name) return "Vampirism True";
		if (desc) return "Fires 3 bolts. Each of the bolts absorbs some HP from a monster and gives them to you.";
#endif
    
		{
			int dam = 100;

			if (info) return format("%s3*%d", s_dam, dam);

			if (cast)
			{
				int i;

				if (!get_aim_dir(&dir)) return NULL;
				///del131214 virtue
				//chg_virtue(V_SACRIFICE, -1);
				//chg_virtue(V_VITALITY, -1);

				for (i = 0; i < 3; i++)
				{
					if (drain_life(dir, dam))
						hp_player(dam);
				}
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "���̌���";
		if (desc) return "���E���̐����̂��郂���X�^�[�Ƀ_���[�W��^����B";
#else
		if (name) return "Nether Wave";
		if (desc) return "Damages all living monsters in sight.";
#endif
    
		{
			int sides = plev * 3;

			if (info) return info_damage(1, sides, 0);

			if (cast)
			{
				dispel_living(randint1(sides));
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�Í��̗�";
		if (desc) return "����ȈÍ��̋�����B";
#else
		if (name) return "Darkness Storm";
		if (desc) return "Fires a huge ball of darkness.";
#endif
    
		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DARK, dir, dam, rad);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "���̌���";
		if (desc) return "���̌�������B";
#else
		if (name) return "Death Ray";
		if (desc) return "Fires a beam of death.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				death_ray(dir, plev);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "���ҏ���";
		if (desc) return "1�̂̃A���f�b�h����������B";
#else
		if (name) return "Raise the Dead";
		if (desc) return "Summons an undead monster.";
#endif
    
		{
			if (cast)
			{
				int type;
				bool pet = one_in_(3);
				u32b summon_flags = 0L;

				type = (plev > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);

				if (!pet || (pet && (plev > 24) && one_in_(3)))
					summon_flags |= PM_ALLOW_GROUP;

				if (pet) summon_flags |= PM_FORCE_PET;
				else summon_flags |= (PM_ALLOW_UNIQUE | PM_NO_PET);

				if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, type, summon_flags))
				{
#ifdef JP
					msg_print("�₽���������Ȃ��̎���ɐ����n�߂��B����͕��s�L���^��ł���...");
#else
					msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("�Â��̎�����ҋ������Ȃ��Ɏd���邽�ߓy�����S�����I");
#else
						msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("���҂��S�����B�����W���邠�Ȃ��𔱂��邽�߂ɁI");
#else
						msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
					}
					///del131214 virtue
					//chg_virtue(V_UNLIFE, 1);
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "���҂̔�`";
		if (desc) return "�A�C�e����1���ʂ���B���x���������ƃA�C�e���̔\�͂����S�ɒm�邱�Ƃ��ł���B";
#else
		if (name) return "Esoteria";
		if (desc) return "Identifies an item. Or *identifies* an item at higher level.";
#endif
    
		{
			if (cast)
			{
				if (randint1(50) > plev)
				{
					if (!ident_spell(FALSE)) return NULL;
				}
				else
				{
					if (!identify_fully(FALSE)) return NULL;
				}
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�z���S�ω�";
		if (desc) return "��莞�ԁA�z���S�ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����ɋz���S�Ƃ��Ă̔\�͂𓾂�B";
#else
		if (name) return "Polymorph Vampire";
		if (desc) return "Mimic a vampire for a while. Loses abilities of original race and gets abilities as a vampire.";
#endif
    
		{
			int base = 10 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_VAMPIRE, FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�����͕���";
		if (desc) return "�������o���l���񕜂���B";
#else
		if (name) return "Restore Life";
		if (desc) return "Restore lost experience.";
#endif
    
		{
			if (cast)
			{
				restore_level();
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "���Ӗ��E";
		if (desc) return "�����̎��͂ɂ��郂���X�^�[�����݂̊K�����������B��R�����Ɩ����B";
#else
		if (name) return "Mass Genocide";
		if (desc) return "Eliminates all nearby monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif
    
		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide(power, TRUE);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�n���̍���";
		if (desc) return "�׈��ȗ͂���������B�P�ǂȃ����X�^�[�ɂ͑傫�ȃ_���[�W��^����B";
#else
		if (name) return "Hellfire";
		if (desc) return "Fires a powerful ball of evil power. Hurts good monsters greatly.";
#endif
    
		{
			int dam = 666;
			int rad = 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HELL_FIRE, dir, dam, rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "�n���̍��΂̎�������������J", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "the strain of casting Hellfire", -1);
#endif
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�H�̉�";
		if (desc) return "��莞�ԁA�ǂ�ʂ蔲���邱�Ƃ��ł��󂯂�_���[�W���y�������H�̂̏�Ԃɕϐg����B";
#else
		if (name) return "Wraithform";
		if (desc) return "Becomes wraith form which gives ability to pass walls and makes all damages half.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_wraith_form(randint1(base) + base, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_trump_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;

#ifdef JP
	static const char s_random[] = "�����_��";
#else
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�V���[�g�E�e���|�[�g";
		if (desc) return "�ߋ����̃e���|�[�g������B";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�w偂̃J�[�h";
		if (desc) return "�w偂���������B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("���Ȃ��͒w偂̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of an spider...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_SPIDER, PM_ALLOW_GROUP))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�w偂͓{���Ă���I");
#else
						msg_print("The summoned spiders get angry!");
#endif
					}
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�V���b�t��";
		if (desc) return "�J�[�h�̐肢������B";
#else
		if (name) return "Shuffle";
		if (desc) return "Causes random effects.";
#endif
    
		{
			if (info) return s_random;

			if (cast)
			{
				cast_shuffle();
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�t���A�E���Z�b�g";
		if (desc) return "�Ő[�K��ύX����B";
#else
		if (name) return "Reset Recall";
		if (desc) return "Resets the 'deepest' level for recall spell.";
#endif
    
		{
			if (cast)
			{
				if (!reset_recall()) return NULL;
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�e���|�[�g";
		if (desc) return "�������̃e���|�[�g������B";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 4;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "���m�̃J�[�h";
		if (desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#else
		if (name) return "Trump Spying";
		if (desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�e���|�[�g�E�����X�^�[";
		if (desc) return "�����X�^�[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�����̃J�[�h";
		if (desc) return "1�̂̓�������������B";
#else
		if (name) return "Trump Animals";
		if (desc) return "Summons an animal.";
#endif
    
		{
			if (cast || fail)
			{
				int type = (!fail ? SUMMON_ANIMAL_RANGER : SUMMON_ANIMAL);

#ifdef JP
				msg_print("���Ȃ��͓����̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of an animal...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, type, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�����͓{���Ă���I");
#else
						msg_print("The summoned animal gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�ړ��̃J�[�h";
		if (desc) return "�A�C�e���������̑����ֈړ�������B";
#else
		if (name) return "Trump Reach";
		if (desc) return "Pulls a distant item close to you.";
#endif
    
		{
			int weight = plev * 15;

			if (info) return info_weight(weight);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fetch(dir, weight, FALSE);
			}
		}
		break;
///sysdel death �J�~�J�[�̃J�[�h
	case 9:
#ifdef JP
		if (name) return "�J�~�J�[�̃J�[�h";
		if (desc) return "�����̔������郂���X�^�[����������B";
#else
		if (name) return "Trump Kamikaze";
		if (desc) return "Summons monsters which explode by itself.";
#endif
    
		{
			if (cast || fail)
			{
				int x, y;
				int type;

				if (cast)
				{
					if (!target_set(TARGET_KILL)) return NULL;
					x = target_col;
					y = target_row;
				}
				else
				{
					/* Summons near player when failed */
					x = px;
					y = py;
				}

				if (p_ptr->pclass == CLASS_BEASTMASTER)
					type = SUMMON_KAMIKAZE_LIVING;
				else
					type = SUMMON_KAMIKAZE;

#ifdef JP
				msg_print("���Ȃ��̓J�~�J�[�̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on several trumps at once...");
#endif

				if (trump_summoning(2 + randint0(plev / 7), !fail, y, x, 0, type, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�����X�^�[�͓{���Ă���I");
#else
						msg_print("The summoned creatures get angry!");
#endif
					}
				}
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "���쏢��";
		if (desc) return "1�̗̂H�����������B";
#else
		if (name) return "Phantasmal Servant";
		if (desc) return "Summons a ghost.";
#endif
    
		{
			/* Phantasmal Servant is not summoned as enemy when failed */
			if (cast)
			{
				int summon_lev = plev * 2 / 3 + randint1(plev / 2);

				if (trump_summoning(1, !fail, py, px, (summon_lev * 3 / 2), SUMMON_PHANTOM, 0L))
				{
#ifdef JP
					msg_print("��p�ł������܂����A���l�l�H");
#else
					msg_print("'Your wish, master?'");
#endif
				}
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�X�s�[�h�E�����X�^�[";
		if (desc) return "�����X�^�[1�̂�����������B";
#else
		if (name) return "Haste Monster";
		if (desc) return "Hastes a monster.";
#endif
    
		{
			if (cast)
			{
				bool result;

				/* Temporary enable target_pet option */
				bool old_target_pet = target_pet;
				target_pet = TRUE;

				result = get_aim_dir(&dir);

				/* Restore target_pet option */
				target_pet = old_target_pet;

				if (!result) return NULL;

				speed_monster(dir, plev);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�e���|�[�g�E���x��";
		if (desc) return "�u���ɏォ���̊K�Ƀe���|�[�g����B";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				if (!get_check("�{���ɑ��̊K�Ƀe���|�[�g���܂����H")) return NULL;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�����̔�";
		if (desc) return "�Z�������̎w�肵���ꏊ�Ƀe���|�[�g����B";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif
    
		{
			int range = plev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("�����̔����J�����B�ړI�n��I��ŉ������B");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(FALSE)) return NULL;
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�A�҂̎���";
		if (desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�����Ǖ�";
		if (desc) return "���E���̑S�Ẵ����X�^�[���e���|�[�g������B��R�����Ɩ����B";
#else
		if (name) return "Banish";
		if (desc) return "Teleports all monsters in sight away unless resisted.";
#endif
    
		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				banish_monsters(power);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�ʒu�����̃J�[�h";
		if (desc) return "1�̂̃����X�^�[�ƈʒu����������B";
#else
		if (name) return "Swap Position";
		if (desc) return "Swap positions of you and a monster.";
#endif
    
		{
			if (cast)
			{
				bool result;

				/* HACK -- No range limit */
				project_length = -1;

				result = get_aim_dir(&dir);

				/* Restore range to default */
				project_length = 0;

				if (!result) return NULL;

				teleport_swap(dir);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�A���f�b�h�̃J�[�h";
		if (desc) return "1�̂̃A���f�b�h����������B";
#else
		if (name) return "Trump Undead";
		if (desc) return "Summons an undead monster.";
#endif
    
		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("���Ȃ��̓A���f�b�h�̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of an undead creature...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_UNDEAD, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�A���f�b�h�͓{���Ă���I");
#else
						msg_print("The summoned undead creature gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "঒��ނ̃J�[�h";
		if (desc) return "1�̂̃q�h������������B";
#else
		if (name) return "Trump Reptiles";
		if (desc) return "Summons a hydra.";
#endif
    
		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("���Ȃ���঒��ނ̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of a reptile...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_HYDRA, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ঒��ނ͓{���Ă���I");
#else
						msg_print("The summoned reptile gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�����X�^�[�̃J�[�h";
		if (desc) return "�����̃����X�^�[����������B";
#else
		if (name) return "Trump Monsters";
		if (desc) return "Summons some monsters.";
#endif
    
		{
			if (cast || fail)
			{
				int type;

#ifdef JP
				msg_print("���Ȃ��̓����X�^�[�̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on several trumps at once...");
#endif

				if (p_ptr->pclass == CLASS_BEASTMASTER)
					type = SUMMON_LIVING;
				else
					type = 0;

				if (trump_summoning((1 + (plev - 15)/ 10), !fail, py, px, 0, type, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�����X�^�[�͓{���Ă���I");
#else
						msg_print("The summoned creatures get angry!");
#endif
					}
				}

			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�n�E���h�̃J�[�h";
		if (desc) return "1�O���[�v�̃n�E���h����������B";
#else
		if (name) return "Trump Hounds";
		if (desc) return "Summons a group of hounds.";
#endif
    
		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("���Ȃ��̓n�E���h�̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of a hound...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_HOUND, PM_ALLOW_GROUP))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�n�E���h�͓{���Ă���I");
#else
						msg_print("The summoned hounds get angry!");
#endif
					}
				}
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�g�����v�̐n";
		if (desc) return "����Ƀg�����v�̑���������B";
#else
		if (name) return "Trump Branding";
		if (desc) return "Makes current weapon a Trump weapon.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(5);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�l�ԃg�����v";
		if (desc) return "�����_���Ƀe���|�[�g����ˑR�ψق��A�����̈ӎv�Ńe���|�[�g����ˑR�ψق��g�ɂ��B";
#else
		if (name) return "Living Trump";
		if (desc) return "Gives mutation which makes you teleport randomly or makes you able to teleport at will.";
#endif
    
		{
			if (cast)
			{
				///sys muta �l�ԃg�����v�@�n�[�h�R�[�h
				int mutation;

				if (one_in_(7))
					/* Teleport control */
					mutation = 12;
				else
					/* Random teleportation (uncontrolled) */
					mutation = 77;

				/* Gain the mutation */
				if (gain_random_mutation(mutation))
				{
#ifdef JP
					msg_print("���Ȃ��͐����Ă���J�[�h�ɕς�����B");
#else
					msg_print("You have turned into a Living Trump.");
#endif
				}
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�T�C�o�[�f�[�����̃J�[�h";
		if (desc) return "1�̂̃T�C�o�[�f�[��������������B";
#else
		if (name) return "Trump Cyberdemon";
		if (desc) return "Summons a cyber demon.";
#endif
    
		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("���Ȃ��̓T�C�o�[�f�[�����̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of a Cyberdemon...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_CYBER, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�T�C�o�[�f�[�����͓{���Ă���I");
#else
						msg_print("The summoned Cyberdemon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�\���̃J�[�h";
		if (desc) return "�߂��̑S�Ẵ����X�^�[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if (name) return "Trump Divination";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�m���̃J�[�h";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if (name) return "Trump Lore";
		if (desc) return "*Identifies* an item.";
#endif
    
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�񕜃����X�^�[";
		if (desc) return "�����X�^�[1�̗̂̑͂��񕜂�����B";
#else
		if (name) return "Heal Monster";
		if (desc) return "Heal a monster.";
#endif
    
		{
			int heal = plev * 10 + 200;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				bool result;

				/* Temporary enable target_pet option */
				bool old_target_pet = target_pet;
				target_pet = TRUE;

				result = get_aim_dir(&dir);

				/* Restore target_pet option */
				target_pet = old_target_pet;

				if (!result) return NULL;

				heal_monster(dir, heal);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�h���S���̃J�[�h";
		if (desc) return "1�̂̃h���S������������B";
#else
		if (name) return "Trump Dragon";
		if (desc) return "Summons a dragon.";
#endif
    
		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("���Ȃ��̓h���S���̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of a dragon...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_DRAGON, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�h���S���͓{���Ă���I");
#else
						msg_print("The summoned dragon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "覐΂̃J�[�h";
		if (desc) return "�����̎��ӂ�覐΂𗎂Ƃ��B";
#else
		if (name) return "Trump Meteor";
		if (desc) return "Makes meteor balls fall down to nearby random locations.";
#endif
    
		{
			int dam = plev * 2;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				cast_meteor(dam, rad,GF_METEOR);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�f�[�����̃J�[�h";
		if (desc) return "1�̂̈�������������B";
#else
		if (name) return "Trump Demon";
		if (desc) return "Summons a demon.";
#endif
    
		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("���Ȃ��̓f�[�����̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of a demon...");
#endif

				if (trump_summoning(1, !fail, py, px, 0, SUMMON_DEMON, 0L))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�f�[�����͓{���Ă���I");
#else
						msg_print("The summoned demon gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�n���̃J�[�h";
		if (desc) return "1�̂̏㋉�A���f�b�h����������B";
#else
		if (name) return "Trump Greater Undead";
		if (desc) return "Summons a greater undead.";
#endif
    
		{
			if (cast || fail)
			{
#ifdef JP
				msg_print("���Ȃ��͋��͂ȃA���f�b�h�̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of a greater undead being...");
#endif
				/* May allow unique depend on level and dice roll */
				if (trump_summoning(1, !fail, py, px, 0, SUMMON_HI_UNDEAD, PM_ALLOW_UNIQUE))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�㋉�A���f�b�h�͓{���Ă���I");
#else
						msg_print("The summoned greater undead creature gets angry!");
#endif
					}
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�Ñ�h���S���̃J�[�h";
		if (desc) return "1�̂̌Ñ�h���S������������B";
#else
		if (name) return "Trump Ancient Dragon";
		if (desc) return "Summons an ancient dragon.";
#endif
    
		{
			if (cast)
			{
				int type;

				if (p_ptr->pclass == CLASS_BEASTMASTER)
					type = SUMMON_HI_DRAGON_LIVING;
				else
					type = SUMMON_HI_DRAGON;

#ifdef JP
				msg_print("���Ȃ��͌Ñ�h���S���̃J�[�h�ɏW������...");
#else
				msg_print("You concentrate on the trump of an ancient dragon...");
#endif

				/* May allow unique depend on level and dice roll */
				if (trump_summoning(1, !fail, py, px, 0, type, PM_ALLOW_UNIQUE))
				{
					if (fail)
					{
#ifdef JP
						msg_print("�������ꂽ�Ñ�h���S���͓{���Ă���I");
#else
						msg_print("The summoned ancient dragon gets angry!");
#endif
					}
				}
			}
		}
		break;
	}

	return "";
}


static cptr do_arcane_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�d��";
		if (desc) return "�d���̃{���g�������̓r�[������B";
#else
		if (name) return "Zap";
		if (desc) return "Fires a bolt or beam of lightning.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 3;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "���@�̎{��";
		if (desc) return "���Ɍ���������B";
#else
		if (name) return "Wizard Lock";
		if (desc) return "Locks a door.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wizard_lock(dir);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�����̊��m";
		if (desc) return "�߂��̓����ȃ����X�^�[�����m����B";
#else
		if (name) return "Detect Invisibility";
		if (desc) return "Detects all invisible monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_invis(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�����X�^�[���m";
		if (desc) return "�߂��̑S�Ă̌����郂���X�^�[�����m����B";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�V���[�g�E�e���|�[�g";
		if (desc) return "�ߋ����̃e���|�[�g������B";
#else
		if (name) return "Blink";
		if (desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "���C�g�E�G���A";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Light Area";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "㩂Ɣ� �j��";
		if (desc) return "�꒼����̑S�Ă�㩂Ɣ���j�󂷂�B";
#else
		if (name) return "Trap & Door Destruction";
		if (desc) return "Fires a beam which destroy traps and doors.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				destroy_door(dir);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�y���̎���";
		if (desc) return "����Ƒ̗͂������񕜂�����B";
#else
		if (name) return "Cure Light Wounds";
		if (desc) return "Heals cut and HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(p_ptr->cut - 10);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "㩂Ɣ� ���m";
		if (desc) return "�߂��̑S�Ă�㩂Ɣ��ƊK�i�����m����B";
#else
		if (name) return "Detect Doors & Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�R�f";
		if (desc) return "�����ɔR����⋋����B";
#else
		if (name) return "Phlogiston";
		if (desc) return "Adds more turns of light to a lantern or torch.";
#endif
    
		{
			if (cast)
			{
				///del131214 �R�f�̖��@
				//phlogiston();
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "���󊴒m";
		if (desc) return "�߂��̍�������m����B";
#else
		if (name) return "Detect Treasure";
		if (desc) return "Detects all treasures in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_treasure(rad);
				detect_objects_gold(rad);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "���@ ���m";
		if (desc) return "�߂��̖��@�����������A�C�e�������m����B";
#else
		if (name) return "Detect Enchantment";
		if (desc) return "Detects all magical items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_objects_magic(rad);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�A�C�e�����m";
		if (desc) return "�߂��̑S�ẴA�C�e�������m����B";
#else
		if (name) return "Detect Objects";
		if (desc) return "Detects all items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_objects_normal(rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "���";
		if (desc) return "�ł�̓����犮�S�Ɏ�菜���B";
#else
		if (name) return "Cure Poison";
		if (desc) return "Cures poison status.";
#endif
    
		{
			if (cast)
			{
				set_poisoned(0);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�ϗ�";
		if (desc) return "��莞�ԁA��C�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Cold";
		if (desc) return "Gives resistance to cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�ω�";
		if (desc) return "��莞�ԁA���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Fire";
		if (desc) return "Gives resistance to fire. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�ϓd";
		if (desc) return "��莞�ԁA�d���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�ώ_";
		if (desc) return "��莞�ԁA�_�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�d���̎���";
		if (desc) return "����Ƒ̗͂𒆒��x�񕜂�����B";
#else
		if (name) return "Cure Medium Wounds";
		if (desc) return "Heals cut and HP more.";
#endif
    
		{
			int dice = 4;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut((p_ptr->cut / 2) - 50);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�e���|�[�g";
		if (desc) return "�������̃e���|�[�g������B";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�Ӓ�";
		if (desc) return "�A�C�e�������ʂ���B";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif
    
		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "��Ηn��";
		if (desc) return "�ǂ�n�����ď��ɂ���B";
#else
		if (name) return "Stone to Mud";
		if (desc) return "Turns one rock square to mud.";
#endif
    
		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wall_to_mud(dir, 20 + randint1(30));
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�M��";
		if (desc) return "��������B��������������X�^�[�Ɍ��ʂ�����B";
#else
		if (name) return "Ray of Light";
		if (desc) return "Fires a beam of light which damages to light-sensitive monsters.";
#endif
    
		{
			int dice = 6;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_print("�����������ꂽ�B");
#else
				msg_print("A line of light appears.");
#endif

				lite_line(dir, damroll(6, 8));
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�󕠏[��";
		if (desc) return "�����ɂ���B";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif
    
		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�������F";
		if (desc) return "��莞�ԁA�����Ȃ��̂�������悤�ɂȂ�B";
#else
		if (name) return "See Invisible";
		if (desc) return "Gives see invisible for a while.";
#endif
    
		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�G�������^������";
		if (desc) return "1�̂̃G�������^������������B";
#else
		if (name) return "Conjure Elemental";
		if (desc) return "Summons an elemental.";
#endif
    
		{
			if (cast)
			{
				if (!summon_specific(-1, py, px, plev, SUMMON_ELEMENTAL, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				{
#ifdef JP
					msg_print("�G�������^���͌���Ȃ������B");
#else
					msg_print("No Elementals arrive.");
#endif
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�e���|�[�g�E���x��";
		if (desc) return "�u���ɏォ���̊K�Ƀe���|�[�g����B";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				if (!get_check("�{���ɑ��̊K�Ƀe���|�[�g���܂����H")) return NULL;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�e���|�[�g�E�����X�^�[";
		if (desc) return "�����X�^�[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "���f�̋�";
		if (desc) return "���A�d���A��C�A�_�̂ǂꂩ�̋�����B";
#else
		if (name) return "Elemental Ball";
		if (desc) return "Fires a ball of some elements.";
#endif
    
		{
			int dam = 75 + plev;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				int type;

				if (!get_aim_dir(&dir)) return NULL;

				switch (randint1(4))
				{
					case 1:  type = GF_FIRE;break;
					case 2:  type = GF_ELEC;break;
					case 3:  type = GF_COLD;break;
					default: type = GF_ACID;break;
				}

				fire_ball(type, dir, dam, rad);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�S���m";
		if (desc) return "�߂��̑S�Ẵ����X�^�[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if (name) return "Detection";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�A�҂̎���";
		if (desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�痢��";
		if (desc) return "���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B����ɁA��莞�ԃe���p�V�[�\�͂𓾂�B";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
	///del131214 virtue
				//chg_virtue(V_KNOWLEDGE, 1);
				//chg_virtue(V_ENLIGHTEN, 1);

				wiz_lite(FALSE);

				if (!p_ptr->telepathy)
				{
					set_tim_esp(randint1(sides) + base, FALSE);
				}
			}
		}
		break;
	}

	return "";
}


static cptr do_craft_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�ԊO������";
		if (desc) return "��莞�ԁA�ԊO�����͂����������B";
#else
		if (name) return "Infravision";
		if (desc) return "Gives infravision for a while.";
#endif
    
		{
			int base = 100;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_infra(base + randint1(base), FALSE);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�񕜗͋���";
		if (desc) return "��莞�ԁA�񕜗͂����������B";
#else
		if (name) return "Regeneration";
		if (desc) return "Gives regeneration ability for a while.";
#endif
    
		{
			int base = 80;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_regen(base + randint1(base), FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�󕠏[��";
		if (desc) return "�����ɂȂ�B";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif
    
		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�ϗ�C";
		if (desc) return "��莞�ԁA��C�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Cold";
		if (desc) return "Gives resistance to cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�ωΉ�";
		if (desc) return "��莞�ԁA���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Fire";
		if (desc) return "Gives resistance to fire. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�m�C���g";
		if (desc) return "��莞�ԁA�q�[���[�C���ɂȂ�B";
#else
		if (name) return "Heroism";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�ϓd��";
		if (desc) return "��莞�ԁA�d���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�ώ_";
		if (desc) return "��莞�ԁA�_�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�������F";
		if (desc) return "��莞�ԁA�����Ȃ��̂�������悤�ɂȂ�B";
#else
		if (name) return "See Invisibility";
		if (desc) return "Gives see invisible for a while.";
#endif
    
		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "����";
		if (desc) return "�A�C�e���ɂ��������ア�􂢂���������B";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�ϓ�";
		if (desc) return "��莞�ԁA�łւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "����m��";
		if (desc) return "����m�����A���|����������B";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_shero(randint1(base) + base, FALSE);
				hp_player(30);
				set_afraid(0);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "���ȕ���";
		if (desc) return "���݂̎����̏�Ԃ����S�ɒm��B";
#else
		if (name) return "Self Knowledge";
		if (desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif
    
		{
			if (cast)
			{
				self_knowledge();
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�Ύ׈����E";
		if (desc) return "�׈��ȃ����X�^�[�̍U����h���o���A�𒣂�B";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif
    
		{
			int base = 3 * plev;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_protevil(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "����";
		if (desc) return "�ŁA�N�O��ԁA������S�������A���o�𒼂��B";
#else
		if (name) return "Cure";
		if (desc) return "Heals poison, stun, cut and hallucination completely.";
#endif
    
		{
			if (cast)
			{
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
				set_image(0);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���@��";
		if (desc) return "��莞�ԁA����ɗ�C�A���A�d���A�_�A�ł̂����ꂩ�̑���������B����������Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Mana Branding";
		if (desc) return "Makes current weapon some elemental branded. You must wield weapons.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_ele_attack()) return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�e���p�V�[";
		if (desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#else
		if (name) return "Telepathy";
		if (desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "���Ή�";
		if (desc) return "��莞�ԁAAC���㏸������B";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif
    
		{
			int base = 30;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�S�ϐ�";
		if (desc) return "��莞�ԁA�_�A�d���A���A��C�A�łɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�X�s�[�h";
		if (desc) return "��莞�ԁA��������B";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif
    
		{
			int base = plev;
			int sides = 20 + plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�ǔ���";
		if (desc) return "��莞�ԁA�����������ǂ�ʂ蔲������悤�ɂȂ�B";
#else
		if (name) return "Walk through Wall";
		if (desc) return "Gives ability to pass walls for a while.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_kabenuke(randint1(base) + base, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "������";
		if (desc) return "���ɔ��˂̑���������B";
#else
		if (name) return "Polish Shield";
		if (desc) return "Makes a shield a shield of reflection.";
#endif
    
		{
			if (cast)
			{
				// pulish_shield();
			msg_print("�������͖��������B");
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�S�[��������";
		if (desc) return "1�̂̃S�[�����𐻑�����B";
#else
		if (name) return "Create Golem";
		if (desc) return "Creates a golem.";
#endif
    
		{
			if (cast)
			{
				if (summon_specific(-1, py, px, plev, SUMMON_GOLEM, PM_FORCE_PET))
				{
#ifdef JP
					msg_print("�S�[������������B");
#else
					msg_print("You make a golem.");
#endif
				}
				else
				{
#ifdef JP
					msg_print("���܂��S�[���������Ȃ������B");
#else
					msg_print("No Golems arrive.");
#endif
				}
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "���@�̊Z";
		if (desc) return "��莞�ԁA���@�h��͂�AC���オ��A�����ƖӖڂ̑ϐ��A���˔\�́A��გm�炸�A���V�𓾂�B";
#else
		if (name) return "Magical armor";
		if (desc) return "Gives resistance to magic, bonus to AC, resistance to confusion, blindness, reflection, free action and levitation for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_magicdef(randint1(base) + base, FALSE);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�������͉�";
		if (desc) return "����E�h��ɂ�����ꂽ�����閂�͂����S�ɉ�������B";
#else
		if (name) return "Remove Enchantment";
		if (desc) return "Removes all magics completely from any weapon or armor.";
#endif
    
		{
			if (cast)
			{
				if (!mundane_spell(TRUE)) return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�􂢕���";
		if (desc) return "�A�C�e���ɂ����������͂Ȏ􂢂���������B";
#else
		if (name) return "Remove All Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "���S�Ȃ�m��";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if (name) return "Knowledge True";
		if (desc) return "*Identifies* an item.";
#endif
    
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "���틭��";
		if (desc) return "����̖������C���ƃ_���[�W�C������������B";
#else
		if (name) return "Enchant Weapon";
		if (desc) return "Attempts to increase +to-hit, +to-dam of a weapon.";
#endif
    
		{
			if (cast)
			{
				if (!enchant_spell(randint0(4) + 1, randint0(4) + 1, 0)) return NULL;
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�h���";
		if (desc) return "�Z�̖h��C������������B";
#else
		if (name) return "Enchant Armor";
		if (desc) return "Attempts to increase +AC of an armor.";
#endif
    
		{
			if (cast)
			{
				if (!enchant_spell(0, 0, randint0(3) + 2)) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "���푮���t�^";
		if (desc) return "����Ƀ����_���ɑ���������B";
#else
		if (name) return "Brand Weapon";
		if (desc) return "Makes current weapon a random ego weapon.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(randint0(18));
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�l�ԃg�����v";
		if (desc) return "�����_���Ƀe���|�[�g����ˑR�ψق��A�����̈ӎv�Ńe���|�[�g����ˑR�ψق��g�ɂ��B";
#else
		if (name) return "Living Trump";
		if (desc) return "Gives mutation which makes you teleport randomly or makes you able to teleport at will.";
#endif
    
		{
			if (cast)
			{
				///sys muta �l�ԃg�����v�@�ψٔԍ��n�[�h�R�[�f�B���O
				int mutation;

				if (one_in_(7))
					/* Teleport control */
					mutation = 12;
				else
					/* Random teleportation (uncontrolled) */
					mutation = 77;

				/* Gain the mutation */
				if (gain_random_mutation(mutation))
				{
#ifdef JP
					msg_print("���Ȃ��͐����Ă���J�[�h�ɕς�����B");
#else
					msg_print("You have turned into a Living Trump.");
#endif
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�����ւ̖Ɖu";
		if (desc) return "��莞�ԁA��C�A���A�d���A�_�̂����ꂩ�ɑ΂���Ɖu�𓾂�B";
#else
		if (name) return "Immunity";
		if (desc) return "Gives an immunity to fire, cold, electricity or acid for a while.";
#endif
    
		{
			int base = 13;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_ele_immune(base + randint1(base))) return NULL;
			}
		}
		break;
	}

	return "";
}


static cptr do_daemon_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
#else
	static const char s_dam[] = "dam ";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�}�W�b�N�E�~�T�C��";
		if (desc) return "�ア���@�̖����B";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "���������m";
		if (desc) return "�߂��̐����̂Ȃ������X�^�[�����m����B";
#else
		if (name) return "Detect Unlife";
		if (desc) return "Detects all nonliving monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_nonliving(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�ׂȂ�j��";
		if (desc) return "��莞�ԁA��������AC�Ƀ{�[�i�X�𓾂�B";
#else
		if (name) return "Evil Bless";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�ωΉ�";
		if (desc) return "��莞�ԁA���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Fire";
		if (desc) return "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "���Q";
		if (desc) return "�����X�^�[1�̂����|�����A�N�O������B��R�����Ɩ����B";
#else
		if (name) return "Horrify";
		if (desc) return "Attempts to scare and stun a monster.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fear_monster(dir, power);
				stun_monster(dir, power);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�n���̖�";
		if (desc) return "�n���̃{���g�������̓r�[������B";
#else
		if (name) return "Nether Bolt";
		if (desc) return "Fires a bolt or beam of nether.";
#endif
    
		{
			int dice = 6 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_NETHER, dir, damroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�Ñ�̎��쏢��";
		if (desc) return "�Ñ�̎������������B";
#else
		if (name) return "Summon Manes";
		if (desc) return "Summons a manes.";
#endif
    
		{
			if (cast)
			{
				if (!summon_specific(-1, py, px, (plev * 3) / 2, SUMMON_MANES, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				{
#ifdef JP
					msg_print("�Ñ�̎���͌���Ȃ������B");
#else
					msg_print("No Manes arrive.");
#endif
				}
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�n���̉�";
		if (desc) return "�׈��ȗ͂����{�[������B�P�ǂȃ����X�^�[�ɂ͑傫�ȃ_���[�W��^����B";
#else
		if (name) return "Hellish Flame";
		if (desc) return "Fires a ball of evil power. Hurts good monsters greatly.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			///class ���C�W�n�͈З͂��オ�閂�@
			if (p_ptr->pclass == CLASS_MAGE ||
			    p_ptr->pclass == CLASS_HIGH_MAGE ||
			    p_ptr->pclass == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HELL_FIRE, dir, damroll(dice, sides) + base, rad);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�f�[�����x�z";
		if (desc) return "����1�̂𖣗�����B��R�����Ɩ���";
#else
		if (name) return "Dominate Demon";
		if (desc) return "Attempts to charm a demon.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				control_one_demon(dir, power);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�r�W����";
		if (desc) return "���ӂ̒n�`�����m����B";
#else
		if (name) return "Vision";
		if (desc) return "Maps nearby area.";
#endif
    
		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�ϒn��";
		if (desc) return "��莞�ԁA�n���ւ̑ϐ��𓾂�B";
#else
		if (name) return "Resist Nether";
		if (desc) return "Gives resistance to nether for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_nether(randint1(base) + base, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�v���Y�}�E�{���g";
		if (desc) return "�v���Y�}�̃{���g�������̓r�[������B";
#else
		if (name) return "Plasma bolt";
		if (desc) return "Fires a bolt or beam of plasma.";
#endif
    
		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_PLASMA, dir, damroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�t�@�C�A�E�{�[��";
		if (desc) return "���̋�����B";
#else
		if (name) return "Fire Ball";
		if (desc) return "Fires a ball of fire.";
#endif
    
		{
			int dam = plev + 55;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "���̐n";
		if (desc) return "����ɉ��̑���������B";
#else
		if (name) return "Fire Branding";
		if (desc) return "Makes current weapon fire branded.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(1);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�n����";
		if (desc) return "�傫�Ȓn���̋�����B";
#else
		if (name) return "Nether Ball";
		if (desc) return "Fires a huge ball of nether.";
#endif
    
		{
			int dam = plev * 3 / 2 + 100;
			int rad = plev / 20 + 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_NETHER, dir, dam, rad);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�f�[��������";
		if (desc) return "����1�̂���������B";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif
    
		{
			if (cast)
			{
				bool pet = !one_in_(3);
				u32b summon_flags = 0L;

				if (pet) summon_flags |= PM_FORCE_PET;
				else summon_flags |= PM_NO_PET;
				if (!(pet && (plev < 50))) summon_flags |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, plev*2/3+randint1(plev/2), SUMMON_DEMON, summon_flags))
				{
#ifdef JP
					msg_print("�����̈��L���[�������B");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("�u���p�ł������܂����A����l�l�v");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("�u�ڂ����҂�A��͓��̉��l�ɂ��炸�I ���O�̍��𒸂����I�v");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("�����͌���Ȃ������B");
#else
					msg_print("No demons arrive.");
#endif
				}
				break;
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�����̖�";
		if (desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#else
		if (name) return "Devilish Eye";
		if (desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 30;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(base) + sides, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�����̃N���[�N";
		if (desc) return "���|����菜���A��莞�ԁA���Ɨ�C�̑ϐ��A���̃I�[���𓾂�B�ϐ��͑����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Devil Cloak";
		if (desc) return "Removes fear. Gives resistance to fire and cold, and aura of fire. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int dur = randint1(base) + base;
					
				set_oppose_fire(dur, FALSE);
				set_oppose_cold(dur, FALSE);
				set_tim_sh_fire(dur, FALSE);
				set_afraid(0);
				break;
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�n�◬";
		if (desc) return "�����𒆐S�Ƃ������̋������o���A����n��ɕς���B";
#else
		if (name) return "The Flow of Lava";
		if (desc) return "Generates a ball of fire centered on you which transforms floors to magma.";
#endif
    
		{
			int dam = (55 + plev) * 2;
			int rad = 3;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				fire_ball(GF_FIRE, 0, dam, rad);
				fire_ball_hide(GF_LAVA_FLOW, 0, 2 + randint1(2), rad);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�v���Y�}��";
		if (desc) return "�v���Y�}�̋�����B";
#else
		if (name) return "Plasma Ball";
		if (desc) return "Fires a ball of plasma.";
#endif
    
		{
			int dam = plev * 3 / 2 + 80;
			int rad = 2 + plev / 40;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_PLASMA, dir, dam, rad);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�����ω�";
		if (desc) return "��莞�ԁA�����ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����Ɉ����Ƃ��Ă̔\�͂𓾂�B";
#else
		if (name) return "Polymorph Demon";
		if (desc) return "Mimic a demon for a while. Loses abilities of original race and gets abilities as a demon.";
#endif
    
		{
			int base = 10 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_DEMON, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�n���̔g��";
		if (desc) return "���E���̑S�Ẵ����X�^�[�Ƀ_���[�W��^����B�P�ǂȃ����X�^�[�ɓ��ɑ傫�ȃ_���[�W��^����B";
#else
		if (name) return "Nather Wave";
		if (desc) return "Damages all monsters in sight. Hurts good monsters greatly.";
#endif
    
		{
			int sides1 = plev * 2;
			int sides2 = plev * 2;

			if (info) return format("%sd%d+d%d", s_dam, sides1, sides2);

			if (cast)
			{
				dispel_monsters(randint1(sides1));
				dispel_good(randint1(sides2));
			}
		}
		break;
///sysdel NEXUS
	case 22:
#ifdef JP
		if (name) return "�T�L���o�X�̐ڕ�";
		if (desc) return "���ʍ����̋�����B";
#else
		if (name) return "Kiss of Succubus";
		if (desc) return "Fires a ball of nexus.";
#endif
    
		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_NEXUS, dir, dam, rad);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�j�ł̎�";
		if (desc) return "�j�ł̎����B�H����������X�^�[�͂��̂Ƃ���HP�̔����O��̃_���[�W���󂯂�B";
#else
		if (name) return "Doom Hand";
		if (desc) return "Attempts to make a monster's HP almost half.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				else msg_print("<�j�ł̎�>��������I");
#else
				else msg_print("You invoke the Hand of Doom!");
#endif

				fire_ball_hide(GF_HAND_DOOM, dir, plev * 2, 0);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�m�C���g";
		if (desc) return "��莞�ԁA�q�[���[�C���ɂȂ�B";
#else
		if (name) return "Raise the Morale";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�s�ł̓���";
		if (desc) return "��莞�ԁA���ԋt�]�ւ̑ϐ��𓾂�B";
#else
		if (name) return "Immortal Body";
		if (desc) return "Gives resistance to time for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_time(randint1(base)+base, FALSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "���C�̉~��";
		if (desc) return "�����𒆐S�Ƃ����J�I�X�̋��A�����̋��𔭐������A�߂��̃����X�^�[�𖣗�����B";
#else
		if (name) return "Insanity Circle";
		if (desc) return "Generate balls of chaos, confusion and charm centered on you.";
#endif
    
		{
			int dam = 50 + plev;
			int power = 20 + plev;
			int rad = 3 + plev / 20;

			if (info) return format("%s%d+%d", s_dam, dam/2, dam/2);

			if (cast)
			{
				fire_ball(GF_CHAOS, 0, dam, rad);
				fire_ball(GF_CONFUSION, 0, dam, rad);
				fire_ball(GF_CHARM, 0, power, rad);
			}
		}
		break;
		///sysdel death
	case 27:
#ifdef JP
		if (name) return "�y�b�g���j";
		if (desc) return "�S�Ẵy�b�g�������I�ɔ��j������B";
#else
		if (name) return "Explode Pets";
		if (desc) return "Makes all pets explode.";
#endif
    
		{
			if (cast)
			{
				discharge_minion();
			}
		}
		break;

		///sys item �O���[�^�[�f�[���������@���̊֌W
	case 28:
#ifdef JP
		if (name) return "�O���[�^�[�f�[��������";
		if (desc) return "�㋉�f�[��������������B��������ɂ͐l��('p','h','t'�ŕ\����郂���X�^�[)�̎��̂�����Ȃ���΂Ȃ�Ȃ��B";
#else
		if (name) return "Summon Greater Demon";
		if (desc) return "Summons greater demon. It need to sacrifice a corpse of human ('p','h' or 't').";
#endif
    
		{
			if (cast)
			{
				if (!cast_summon_greater_demon()) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�n����";
		if (desc) return "������Ȓn���̋�����B";
#else
		if (name) return "Nether Storm";
		if (desc) return "Generate a huge ball of nether.";
#endif
    
		{
			int dam = plev * 15;
			int rad = plev / 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_NETHER, dir, dam, rad);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "���̎�";
		if (desc) return "�������_���[�W���󂯂邱�Ƃɂ���đΏۂɎ􂢂������A�_���[�W��^���l�X�Ȍ��ʂ������N�����B";
#else
		if (name) return "Bloody Curse";
		if (desc) return "Puts blood curse which damages and causes various effects on a monster. You also take damage.";
#endif
    
		{
			int dam = 600;
			int rad = 0;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_BLOOD_CURSE, dir, dam, rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "���̎�", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "Blood curse", -1);
#endif
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�����ω�";
		if (desc) return "�����̉��ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����Ɉ����̉��Ƃ��Ă̔\�͂𓾁A�ǂ�j�󂵂Ȃ�������B";
#else
		if (name) return "Polymorph Demonlord";
		if (desc) return "Mimic a demon lord for a while. Loses abilities of original race and gets great abilities as a demon lord. Even hard walls can't stop your walking.";
#endif
    
		{
			int base = 15;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_DEMON_LORD, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_crusade_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "����";
		if (desc) return "�d���̃{���g�������̓r�[������B";
#else
		if (name) return "Punishment";
		if (desc) return "Fires a bolt or beam of lightning.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�׈����݊��m";
		if (desc) return "�߂��̎׈��ȃ����X�^�[�����m����B";
#else
		if (name) return "Detect Evil";
		if (desc) return "Detects all evil monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_evil(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "���|����";
		if (desc) return "���|����菜���B";
#else
		if (name) return "Remove Fear";
		if (desc) return "Removes fear.";
#endif
    
		{
			if (cast)
			{
				set_afraid(0);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�Ј�";
		if (desc) return "�����X�^�[1�̂����|������B��R�����Ɩ����B";
#else
		if (name) return "Scare Monster";
		if (desc) return "Attempts to scare a monster.";
#endif
    
		{
			//v1.1.90 ����
			int power = plev*2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fear_monster(dir, power);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "����";
		if (desc) return "�אڂ����S�Ẵ����X�^�[�𖰂点��B��R�����Ɩ����B";
#else
		if (name) return "Sanctuary";
		if (desc) return "Attempts to sleep monsters in the adjacent squares.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				sleep_monsters_touch();
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "����";
		if (desc) return "�������̃e���|�[�g������B";
#else
		if (name) return "Portal";
		if (desc) return "Teleport medium distance.";
#endif
    
		{
			int range = 25 + plev / 2;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�X�^�[�_�X�g";
		if (desc) return "�^�[�Q�b�g�t�߂ɑM���̃{���g��A�˂���B";
#else
		if (name) return "Star Dust";
		if (desc) return "Fires many bolts of light near the target.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 9;
			int sides = 2;

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_blast(GF_LITE, dir, dice, sides, 10, 3, 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�g�̏�";
		if (desc) return "���A�ŁA�N�O����S������B";
#else
		if (name) return "Purify";
		if (desc) return "Heals all cut, stun and poison status.";
#endif
    
		{
			if (cast)
			{
				set_cut(0);
				set_poisoned(0);
				set_stun(0);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�׈���΂�";
		if (desc) return "�׈��ȃ����X�^�[1�̂��e���|�[�g������B��R�����Ɩ����B";
#else
		if (name) return "Scatter Evil";
		if (desc) return "Attempts to teleport an evil monster away.";
#endif
    
		{
			int power = MAX_SIGHT * 5;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_AWAY_EVIL, dir, power, 0);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "���Ȃ����";
		if (desc) return "���Ȃ�͂���������B�׈��ȃ����X�^�[�ɑ΂��đ傫�ȃ_���[�W��^���邪�A�P�ǂȃ����X�^�[�ɂ͌��ʂ��Ȃ��B";
#else
		if (name) return "Holy Orb";
		if (desc) return "Fires a ball with holy power. Hurts evil monsters greatly, but don't effect good monsters.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			///class ���C�W�n�͈З͂��オ�閂�@
			if (p_ptr->pclass == CLASS_PRIEST ||
			    p_ptr->pclass == CLASS_HIGH_MAGE ||
			    p_ptr->pclass == CLASS_SORCERER)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HOLY_FIRE, dir, damroll(dice, sides) + base, rad);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�ޖ��̎���";
		if (desc) return "���E���̃A���f�b�h�E�����E�d���Ƀ_���[�W��^���N�O�Ƃ�����B";
#else
		if (name) return "Exorcism";
		if (desc) return "Damages all undead and demons in sight, and scares all evil monsters in sight.";
#endif
    
		{
			int dam = plev;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				dispel_undead(dam);
				dispel_demons(dam);
				dispel_kwai(dam);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "����";
		if (desc) return "�A�C�e���ɂ��������ア�􂢂���������B";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�������F";
		if (desc) return "��莞�ԁA�����Ȃ��̂�������悤�ɂȂ�B";
#else
		if (name) return "Sense Unseen";
		if (desc) return "Gives see invisible for a while.";
#endif
    
		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�Ύ׈����E";
		if (desc) return "�׈��ȃ����X�^�[�̍U����h���o���A�𒣂�B";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif
    
		{
			int base = 25;
			int sides = 3 * plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_protevil(randint1(sides) + sides, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�ق��̗�";
		if (desc) return "���͂ȓd���̃{���g����B";
#else
		if (name) return "Judgment Thunder";
		if (desc) return "Fires a powerful bolt of lightning.";
#endif

		{
			int dam = plev * 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt(GF_ELEC, dir, dam);
			}
		}
		break;


	case 15:
#ifdef JP
		if (name) return "���Ȃ�䌾�t";
		if (desc) return "���E���̎׈��ȑ��݂ɑ傫�ȃ_���[�W��^���A�̗͂��񕜂��A�ŁA���|�A�N�O��ԁA��������S������B";
#else
		if (name) return "Holy Word";
		if (desc) return "Damages all evil monsters in sight, heals HP somewhat, and completely heals poison, fear, stun and cut status.";
#endif
    
		{
			int dam_sides = plev * 6;
			int heal = 100;

#ifdef JP
			if (info) return format("��:1d%d/��%d", dam_sides, heal);
#else
			if (info) return format("dam:d%d/h%d", dam_sides, heal);
#endif

			if (cast)
			{
				dispel_evil(randint1(dam_sides));
				hp_player(heal);
				set_afraid(0);
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�J���ꂽ��";
		if (desc) return "�꒼����̑S�Ă�㩂Ɣ���j�󂷂�B";
#else
		if (name) return "Unbarring Ways";
		if (desc) return "Fires a beam which destroy traps and doors.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				destroy_door(dir);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "����";
		if (desc) return "�׈��ȃ����X�^�[�̓������~�߂�B";
#else
		if (name) return "Arrest";
		if (desc) return "Attempts to paralyze an evil monster.";
#endif
    
		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				stasis_evil(dir);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "���Ȃ�I�[��";
		if (desc) return "��莞�ԁA�׈��ȃ����X�^�[�������鐹�Ȃ�I�[���𓾂�B";
#else
		if (name) return "Holy Aura";
		if (desc) return "Gives aura of holy power which injures evil monsters which attacked you for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_sh_holy(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�A���f�b�h&�����ގU";
		if (desc) return "���E���̑S�ẴA���f�b�h�y�ш����Ƀ_���[�W��^����B";
#else
		if (name) return "Dispel Undead & Demons";
		if (desc) return "Damages all undead and demons in sight.";
#endif
    
		{
			int sides = plev * 4;

			if (info) return info_damage(1, sides, 0);

			if (cast)
			{
				dispel_undead(randint1(sides));
				dispel_demons(randint1(sides));
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�׈��ގU";
		if (desc) return "���E���̑S�Ă̎׈��ȃ����X�^�[�Ƀ_���[�W��^����B";
#else
		if (name) return "Dispel Evil";
		if (desc) return "Damages all evil monsters in sight.";
#endif
    
		{
			int sides = plev * 4;

			if (info) return info_damage(1, sides, 0);

			if (cast)
			{
				dispel_evil(randint1(sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "���Ȃ�n";
		if (desc) return "�ʏ�̕���ɖŎׂ̑���������B";
#else
		if (name) return "Holy Blade";
		if (desc) return "Makes current weapon especially deadly against evil monsters.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(13);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�X�^�[�o�[�X�g";
		if (desc) return "����ȑM���̋�����B";
#else
		if (name) return "Star Burst";
		if (desc) return "Fires a huge ball of powerful light.";
#endif
    
		{
			int dam = 100 + plev * 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_LITE, dir, dam, rad);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�V�g����";
		if (desc) return "�V�g��1�̏�������B";
#else
		if (name) return "Summon Angel";
		if (desc) return "Summons an angel.";
#endif
    
		{
			if (cast)
			{
				bool pet = !one_in_(3);
				u32b summon_flags = 0L;

				if (pet) summon_flags |= PM_FORCE_PET;
				else summon_flags |= PM_NO_PET;
				if (!(pet && (plev < 50))) summon_flags |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, SUMMON_ANGEL, summon_flags))
				{
					if (pet)
					{
#ifdef JP
						msg_print("�u���p�ł������܂����A����l�l�v");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("�u��͓��̉��l�ɂ��炸�I ���s�҂�A�������߂�I�v");
#else
						msg_print("Mortal! Repent of thy impiousness.");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�m�C���g";
		if (desc) return "��莞�ԁA�q�[���[�C���ɂȂ�B";
#else
		if (name) return "Heroism";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�􂢑ގU";
		if (desc) return "�A�C�e���ɂ����������͂Ȏ􂢂���������B";
#else
		if (name) return "Dispel Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�׈��Ǖ�";
		if (desc) return "���E���̑S�Ă̎׈��ȃ����X�^�[���e���|�[�g������B��R�����Ɩ����B";
#else
		if (name) return "Banish Evil";
		if (desc) return "Teleports all evil monsters in sight away unless resisted.";
#endif
    
		{
			int power = 100;

			if (info) return info_power(power);

			if (cast)
			{
				if (banish_evil(power))
				{
#ifdef JP
					msg_print("�_���ȗ͂��׈���ł��������I");
#else
					msg_print("The holy power banishes evil!");
#endif

				}
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�n���}�Q�h��";
		if (desc) return "���ӂ̃A�C�e���A�����X�^�[�A�n�`��j�󂷂�B";
#else
		if (name) return "Armageddon";
		if (desc) return "Destroy everything in nearby area.";
#endif
    
		{
			int base = 12;
			int sides = 4;

			if (cast)
			{
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�ڂɂ͖ڂ�";
		if (desc) return "��莞�ԁA�������_���[�W���󂯂��Ƃ��ɍU�����s���������X�^�[�ɑ΂��ē����̃_���[�W��^����B";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 10;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_eyeeye(randint1(base) + base, FALSE);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�_�̓{��";
		if (desc) return "�^�[�Q�b�g�̎��͂ɕ����̋��𑽐����Ƃ��B";
#else
		if (name) return "Wrath of the God";
		if (desc) return "Drops many balls of disintegration near the target.";
#endif
    
		{
			int dam = plev * 3 + 25;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				if (!cast_wrath_of_the_god(dam, rad,TRUE)) return NULL;
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�_��";
		if (desc) return "�אڂ��郂���X�^�[�ɐ��Ȃ�_���[�W��^���A���E���̃����X�^�[�Ƀ_���[�W�A�����A�N�O�A�����A���|�A�����^����B����ɑ̗͂��񕜂���B";
#else
		if (name) return "Divine Intervention";
		if (desc) return "Damages all adjacent monsters with holy power. Damages and attempt to slow, stun, confuse, scare and freeze all monsters in sight. And heals HP.";
#endif
    
		{
			int b_dam = plev * 11;
			int d_dam = plev * 4;
			int heal = 100;
			int power = plev * 4;

#ifdef JP
			if (info) return format("��%d/��%d+%d", heal, d_dam, b_dam/2);
#else
			if (info) return format("h%d/dm%d+%d", heal, d_dam, b_dam/2);
#endif

			if (cast)
			{
				project(0, 1, py, px, b_dam, GF_HOLY_FIRE, PROJECT_KILL, -1);
				dispel_monsters(d_dam);
				slow_monsters(plev);
				stun_monsters(power);
				confuse_monsters(power);
				turn_monsters(power);
				stasis_monsters(power);
				hp_player(heal);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "����";
		if (desc) return "���E���̑P�ǂȃ����X�^�[���y�b�g�ɂ��悤�Ƃ��A�Ȃ�Ȃ������ꍇ�y�ёP�ǂłȂ������X�^�[�����|������B����ɑ����̉������ꂽ�R�m���������A�q�[���[�A�j���A�����A�Ύ׈����E�𓾂�B";
#else
		if (name) return "Crusade";
		if (desc) return "Attempts to charm all good monsters in sight, and scare all non-charmed monsters, and summons great number of knights, and gives heroism, bless, speed and protection from evil.";
#endif
    
		{
			if (cast)
			{
				int base = 25;
				int sp_sides = 20 + plev;
				int sp_base = plev;

				int i;
				crusade();
				for (i = 0; i < 12; i++)
				{
					int attempt = 10;
					int my, mx;

					while (attempt--)
					{
						scatter(&my, &mx, py, px, 4, 0);

						/* Require empty grids */
						if (cave_empty_bold2(my, mx)) break;
					}
					if (attempt < 0) continue;
					summon_specific(-1, my, mx, plev, SUMMON_KNIGHTS, (PM_ALLOW_GROUP | PM_FORCE_PET | PM_HASTE));
				}
				set_hero(randint1(base) + base, FALSE);
				set_blessed(randint1(base) + base, FALSE);
				set_fast(randint1(sp_sides) + sp_base, FALSE);
				set_protevil(randint1(base) + base, FALSE);
				set_afraid(0);
			}
		}
		break;
	}

	return "";
}

///sys�@realm �́@���΂炭�͖����ɂ��Ă���
static cptr do_music_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
#else
	static const char s_dam[] = "dam ";
#endif

	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�x�݂̉�";
		if (desc) return "���E���̑S�Ẵ����X�^�[������������B��R�����Ɩ����B";
#else
		if (name) return "Song of Holding";
		if (desc) return "Attempts to slow all monsters in sight.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�������Ƃ��������f�B���������ݎn�߂��D�D�D");
#else
			msg_print("You start humming a slow, steady melody...");
#endif
			start_singing(spell, MUSIC_SLOW);
		}

		{
			int power = plev;

			if (info) return info_power(power);

			if (cont)
			{
				slow_monsters(plev);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�j���̉�";
		if (desc) return "��������AC�̃{�[�i�X�𓾂�B";
#else
		if (name) return "Song of Blessing";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�����ȃ����f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("The holy power of the Music of the Ainur enters you...");
#endif
			start_singing(spell, MUSIC_BLESS);
		}

		if (stop)
		{
			if (!p_ptr->blessed)
			{
#ifdef JP
				msg_print("�����ȋC���������������B");
#else
				msg_print("The prayer has expired.");
#endif
			}
		}

		break;

	case 2:
#ifdef JP
		if (name) return "����̉��F";
		if (desc) return "�����̃{���g����B";
#else
		if (name) return "Wrecking Note";
		if (desc) return "Fires a bolt of sound.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		{
			int dice = 4 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt(GF_SOUND, dir, damroll(dice, sides));
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�N�O�̐���";
		if (desc) return "���E���̑S�Ẵ����X�^�[��N�O������B��R�����Ɩ����B";
#else
		if (name) return "Stun Pattern";
		if (desc) return "Attempts to stun all monsters in sight.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("Ῐf�����郁���f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You weave a pattern of sounds to bewilder and daze...");
#endif
			start_singing(spell, MUSIC_STUN);
		}

		{
			int dice = plev / 10;
			int sides = 2;

			if (info) return info_power_dice(dice, sides);

			if (cont)
			{
				stun_monsters(damroll(dice, sides));
			}
		}

		break;

	case 4:
#ifdef JP
		if (name) return "�����̗���";
		if (desc) return "�̗͂������񕜂�����B";
#else
		if (name) return "Flow of Life";
		if (desc) return "Heals HP a little.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�̂�ʂ��đ̂Ɋ��C���߂��Ă����D�D�D");
#else
			msg_print("Life flows through you as you sing a song of healing...");
#endif
			start_singing(spell, MUSIC_L_LIFE);
		}

		{
			int dice = 2;
			int sides = 6;

			if (info) return info_heal(dice, sides, 0);

			if (cont)
			{
				hp_player(damroll(dice, sides));
			}
		}

		break;

	case 5:
#ifdef JP
		if (name) return "���z�̉�";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Song of the Sun";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
#ifdef JP
				msg_print("����P���̂��ӂ���Ƃ炵���B");
#else
				msg_print("Your uplifting song brings brightness to dark places...");
#endif

				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���|�̉�";
		if (desc) return "���E���̑S�Ẵ����X�^�[�����|������B��R�����Ɩ����B";
#else
		if (name) return "Song of Fear";
		if (desc) return "Attempts to scare all monsters in sight.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("���ǂ남�ǂ낵�������f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You start weaving a fearful pattern...");
#endif
			start_singing(spell, MUSIC_FEAR);			
		}

		{
			int power = plev;

			if (info) return info_power(power);

			if (cont)
			{
				project_hack(GF_TURN_ALL, power);
			}
		}

		break;

	case 7:
#ifdef JP
		if (name) return "�킢�̉�";
		if (desc) return "�q�[���[�C���ɂȂ�B";
#else
		if (name) return "Heroic Ballad";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�������킢�̉̂��̂����D�D�D");
#else
			msg_print("You start singing a song of intense fighting...");
#endif

			(void)hp_player(10);
			(void)set_afraid(0);

			/* Recalculate hitpoints */
			p_ptr->update |= (PU_HP);

			start_singing(spell, MUSIC_HERO);
		}

		if (stop)
		{
			if (!p_ptr->hero)
			{
#ifdef JP
				msg_print("�q�[���[�̋C���������������B");
#else
				msg_print("The heroism wears off.");
#endif
				/* Recalculate hitpoints */
				p_ptr->update |= (PU_HP);
			}
		}

		break;

	case 8:
#ifdef JP
		if (name) return "��I�m�o";
		if (desc) return "�߂����/��/�K�i�����m����B���x��15�őS�Ẵ����X�^�[�A20�ō���ƃA�C�e�������m�ł���悤�ɂȂ�B���x��25�Ŏ��ӂ̒n�`�����m���A40�ł��̊K�S�̂��i�v�ɏƂ炵�A�_���W�������̂��ׂẴA�C�e�������m����B���̌��ʂ͉̂������邱�Ƃŏ��ɋN����B";
#else
		if (name) return "Clairaudience";
		if (desc) return "Detects traps, doors and stairs in your vicinity. And detects all monsters at level 15, treasures and items at level 20. Maps nearby area at level 25. Lights and know the whole level at level 40. These effects occurs by turns while this song continues.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�Â��ȉ��y�����o���������܂������D�D�D");
#else
			msg_print("Your quiet music sharpens your sense of hearing...");
#endif

			/* Hack -- Initialize the turn count */
			p_ptr->magic_num1[2] = 0;

			start_singing(spell, MUSIC_DETECT);
		}

		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cont)
			{
				int count = p_ptr->magic_num1[2];

				if (count >= 19) wiz_lite(FALSE);
				if (count >= 11)
				{
					map_area(rad);
					if (plev > 39 && count < 19)
						p_ptr->magic_num1[2] = count + 1;
				}
				if (count >= 6)
				{
					/* There are too many hidden treasure.  So... */
					/* detect_treasure(rad); */
					detect_objects_gold(rad);
					detect_objects_normal(rad);

					if (plev > 24 && count < 11)
						p_ptr->magic_num1[2] = count + 1;
				}
				if (count >= 3)
				{
					detect_monsters_invis(rad);
					detect_monsters_normal(rad);

					if (plev > 19 && count < 6)
						p_ptr->magic_num1[2] = count + 1;
				}
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);

				if (plev > 14 && count < 3)
					p_ptr->magic_num1[2] = count + 1;
			}
		}

		break;

	case 9:
#ifdef JP
		if (name) return "���̉�";
		if (desc) return "���E���̑S�Ẵ����X�^�[�ɑ΂��Đ��_�U�����s���B";
#else
		if (name) return "Soul Shriek";
		if (desc) return "Damages all monsters in sight with PSI damages.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("���_��P���Ȃ���̂��̂����D�D�D");
#else
			msg_print("You start singing a song of soul in pain...");
#endif
			start_singing(spell, MUSIC_PSI);
		}

		{
			int dice = 1;
			int sides = plev * 3 / 2;

			if (info) return info_damage(dice, sides, 0);

			if (cont)
			{
				project_hack(GF_PSI, damroll(dice, sides));
			}
		}

		break;

	case 10:
#ifdef JP
		if (name) return "�m���̉�";
		if (desc) return "�����̂���}�X�Ɨׂ�̃}�X�ɗ����Ă���A�C�e�����Ӓ肷��B";
#else
		if (name) return "Song of Lore";
		if (desc) return "Identifies all items which are in the adjacent squares.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("���̐��E�̒m�������ꍞ��ł����D�D�D");
#else
			msg_print("You recall the rich lore of the world...");
#endif
			start_singing(spell, MUSIC_ID);
		}

		{
			int rad = 1;

			if (info) return info_radius(rad);

			/*
			 * �̂̊J�n���ɂ����ʔ����F
			 * MP�s���ŊӒ肪���������O�ɉ̂����f���Ă��܂��̂�h�~�B
			 */
			if (cont || cast)
			{
				project(0, rad, py, px, 0, GF_IDENTIFY, PROJECT_ITEM, -1);
			}
		}

		break;

	case 11:
#ifdef JP
		if (name) return "�B�ق̉�";
		if (desc) return "�B���s���\�͂��㏸������B";
#else
		if (name) return "Hiding Tune";
		if (desc) return "Gives improved stealth.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("���Ȃ��̎p���i�F�ɂƂ�����ł������D�D�D");
#else
			msg_print("Your song carries you beyond the sight of mortal eyes...");
#endif
			start_singing(spell, MUSIC_STEALTH);
		}

		if (stop)
		{
			if (!p_ptr->tim_stealth)
			{
#ifdef JP
				msg_print("�p���͂�����ƌ�����悤�ɂȂ����B");
#else
				msg_print("You are no longer hided.");
#endif
			}
		}

		break;

	case 12:
#ifdef JP
		if (name) return "���e�̐���";
		if (desc) return "���E���̑S�Ẵ����X�^�[������������B��R�����Ɩ����B";
#else
		if (name) return "Illusion Pattern";
		if (desc) return "Attempts to confuse all monsters in sight.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�ӂ��ʂɌ��e�����ꂽ�D�D�D");
#else
			msg_print("You weave a pattern of sounds to beguile and confuse...");
#endif
			start_singing(spell, MUSIC_CONF);
		}

		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cont)
			{
				confuse_monsters(power);
			}
		}

		break;

	case 13:
#ifdef JP
		if (name) return "�j�ł̋���";
		if (desc) return "���E���̑S�Ẵ����X�^�[�ɑ΂��č����U�����s���B";
#else
		if (name) return "Doomcall";
		if (desc) return "Damages all monsters in sight with booming sound.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�������������D�D�D");
#else
			msg_print("The fury of the Downfall of Numenor lashes out...");
#endif
			start_singing(spell, MUSIC_SOUND);
		}

		{
			int dice = 10 + plev / 5;
			int sides = 7;

			if (info) return info_damage(dice, sides, 0);

			if (cont)
			{
				project_hack(GF_SOUND, damroll(dice, sides));
			}
		}

		break;

	case 14:
#ifdef JP
		if (name) return "�t�B���G���̉�";
		if (desc) return "���͂̎��̂⍜�𐶂��Ԃ��B";
#else
		if (name) return "Firiel's Song";
		if (desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif
    
		{
			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("�����ƕ����̃e�[�}��t�Ŏn�߂��D�D�D");
#else
				msg_print("The themes of life and revival are woven into your song...");
#endif

				animate_dead(0, py, px);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���̒���";
		if (desc) return "���E���̑S�Ẵ����X�^�[�𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Fellowship Chant";
		if (desc) return "Attempts to charm all monsters in sight.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("���炩�ȃ����f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You weave a slow, soothing melody of imploration...");
#endif
			start_singing(spell, MUSIC_CHARM);
		}

		{
			int dice = 10 + plev / 15;
			int sides = 6;

			if (info) return info_power_dice(dice, sides);

			if (cont)
			{
				charm_monsters(damroll(dice, sides));
			}
		}

		break;

	case 16:
#ifdef JP
		if (name) return "�������g";
		if (desc) return "�ǂ��@��i�ށB�����̑����̃A�C�e���͏�������B";
#else
		if (name) return "Sound of disintegration";
		if (desc) return "Makes you be able to burrow into walls. Objects under your feet evaporate.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("���ӂ��郁���f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You weave a violent pattern of sounds to break wall.");
#endif
			start_singing(spell, MUSIC_WALL);
		}

		{
			/*
			 * �̂̊J�n���ɂ����ʔ����F
			 * MP�s���Ō��ʂ����������O�ɉ̂����f���Ă��܂��̂�h�~�B
			 */
			if (cont || cast)
			{
				project(0, 0, py, px,
					0, GF_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE, -1);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "���f�ϐ�";
		if (desc) return "�_�A�d���A���A��C�A�łɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Finrod's Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("���f�̗͂ɑ΂���E�ς̉̂��̂����B");
#else
			msg_print("You sing a song of perseverance against powers...");
#endif
			start_singing(spell, MUSIC_RESIST);
		}

		if (stop)
		{
			if (!p_ptr->oppose_acid)
			{
#ifdef JP
				msg_print("�_�ւ̑ϐ������ꂽ�C������B");
#else
				msg_print("You feel less resistant to acid.");
#endif
			}

			if (!p_ptr->oppose_elec)
			{
#ifdef JP
				msg_print("�d���ւ̑ϐ������ꂽ�C������B");
#else
				msg_print("You feel less resistant to elec.");
#endif
			}

			if (!p_ptr->oppose_fire)
			{
#ifdef JP
				msg_print("�΂ւ̑ϐ������ꂽ�C������B");
#else
				msg_print("You feel less resistant to fire.");
#endif
			}

			if (!p_ptr->oppose_cold)
			{
#ifdef JP
				msg_print("��C�ւ̑ϐ������ꂽ�C������B");
#else
				msg_print("You feel less resistant to cold.");
#endif
			}

			if (!p_ptr->oppose_pois)
			{
#ifdef JP
				msg_print("�łւ̑ϐ������ꂽ�C������B");
#else
				msg_print("You feel less resistant to pois.");
#endif
			}
		}

		break;

	case 18:
#ifdef JP
		if (name) return "�z�r�b�g�̃����f�B";
		if (desc) return "��������B";
#else
		if (name) return "Hobbit Melodies";
		if (desc) return "Hastes you.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�y���ȉ̂��������ݎn�߂��D�D�D");
#else
			msg_print("You start singing joyful pop song...");
#endif
			start_singing(spell, MUSIC_SPEED);
		}

		if (stop)
		{
			if (!p_ptr->fast)
			{
#ifdef JP
				msg_print("�����̑f�������Ȃ��Ȃ����悤���B");
#else
				msg_print("You feel yourself slow down.");
#endif
			}
		}

		break;

	case 19:
#ifdef JP
		if (name) return "�c�񂾐��E";
		if (desc) return "�߂��̃����X�^�[���e���|�[�g������B��R�����Ɩ����B";
#else
		if (name) return "World Contortion";
		if (desc) return "Teleports all nearby monsters away unless resisted.";
#endif
    
		{
			int rad = plev / 15 + 1;
			int power = plev * 3 + 1;

			if (info) return info_radius(rad);

			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("�̂���Ԃ�c�߂��D�D�D");
#else
				msg_print("Reality whirls wildly as you sing a dizzying melody...");
#endif

				project(0, rad, py, px, power, GF_AWAY_ALL, PROJECT_KILL, -1);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�ގU�̉�";
		if (desc) return "���E���̑S�Ẵ����X�^�[�Ƀ_���[�W��^����B�׈��ȃ����X�^�[�ɓ��ɑ傫�ȃ_���[�W��^����B";
#else
		if (name) return "Dispelling chant";
		if (desc) return "Damages all monsters in sight. Hurts evil monsters greatly.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�ς����Ȃ��s���a�����G��ӂߗ��Ă��D�D�D");
#else
			msg_print("You cry out in an ear-wracking voice...");
#endif
			start_singing(spell, MUSIC_DISPEL);
		}

		{
			int m_sides = plev * 3;
			int e_sides = plev * 3;

			if (info) return format("%s1d%d+1d%d", s_dam, m_sides, e_sides);

			if (cont)
			{
				dispel_monsters(randint1(m_sides));
				dispel_evil(randint1(e_sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�T���}���̊Ì�";
		if (desc) return "���E���̑S�Ẵ����X�^�[�����������A���点�悤�Ƃ���B��R�����Ɩ����B";
#else
		if (name) return "The Voice of Saruman";
		if (desc) return "Attempts to slow and sleep all monsters in sight.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�D�����A���͓I�ȉ̂��������ݎn�߂��D�D�D");
#else
			msg_print("You start humming a gentle and attractive song...");
#endif
			start_singing(spell, MUSIC_SARUMAN);
		}

		{
			int power = plev;

			if (info) return info_power(power);

			if (cont)
			{
				slow_monsters(plev);
				sleep_monsters(plev);
			}
		}

		break;

	case 22:
#ifdef JP
		if (name) return "���̉��F";
		if (desc) return "�����̃r�[������B";
#else
		if (name) return "Song of the Tempest";
		if (desc) return "Fires a beam of sound.";
#endif
    
		{
			int dice = 15 + (plev - 1) / 2;
			int sides = 10;

			if (info) return info_damage(dice, sides, 0);

			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_SOUND, dir, damroll(dice, sides));
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "������̐��E";
		if (desc) return "���݂̊K���č\������B";
#else
		if (name) return "Ambarkanta";
		if (desc) return "Recreates current dungeon level.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("���͂��ω����n�߂��D�D�D");
#else
				msg_print("You sing of the primeval shaping of Middle-earth...");
#endif

				alter_reality();
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�j��̐���";
		if (desc) return "���͂̃_���W������h�炵�A�ǂƏ��������_���ɓ���ς���B";
#else
		if (name) return "Wrecking Pattern";
		if (desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif

		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�j��I�ȉ̂������킽�����D�D�D");
#else
			msg_print("You weave a pattern of sounds to contort and shatter...");
#endif
			start_singing(spell, MUSIC_QUAKE);
		}

		{
			int rad = 10;

			if (info) return info_radius(rad);

			if (cont)
			{
				earthquake(py, px, 10);
			}
		}

		break;


	case 25:
#ifdef JP
		if (name) return "��؂̉�";
		if (desc) return "���E���̑S�Ẵ����X�^�[��Ⴢ����悤�Ƃ���B��R�����Ɩ����B";
#else
		if (name) return "Stationary Shriek";
		if (desc) return "Attempts to freeze all monsters in sight.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�������Ƃ��������f�B��t�Ŏn�߂��D�D�D");
#else
			msg_print("You weave a very slow pattern which is almost likely to stop...");
#endif
			start_singing(spell, MUSIC_STASIS);
		}

		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cont)
			{
				stasis_monsters(power);
			}
		}

		break;

	case 26:
#ifdef JP
		if (name) return "���̉�";
		if (desc) return "�����̂��鏰�̏�ɁA�����X�^�[���ʂ蔲�����菢�����ꂽ�肷�邱�Ƃ��ł��Ȃ��Ȃ郋�[����`���B";
#else
		if (name) return "Endurance";
		if (desc) return "Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.";
#endif
    
		{
			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("�̂��_���ȏ�����o�����D�D�D");
#else
				msg_print("The holy power of the Music is creating sacred field...");
#endif

				warding_glyph();
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�p�Y�̎�";
		if (desc) return "�������A�q�[���[�C���ɂȂ�A���E���̑S�Ẵ����X�^�[�Ƀ_���[�W��^����B";
#else
		if (name) return "The Hero's Poem";
		if (desc) return "Hastes you. Gives heroism. Damages all monsters in sight.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�p�Y�̉̂��������񂾁D�D�D");
#else
			msg_print("You chant a powerful, heroic call to arms...");
#endif
			(void)hp_player(10);
			(void)set_afraid(0);

			/* Recalculate hitpoints */
			p_ptr->update |= (PU_HP);

			start_singing(spell, MUSIC_SHERO);
		}

		if (stop)
		{
			if (!p_ptr->hero)
			{
#ifdef JP
				msg_print("�q�[���[�̋C���������������B");
#else
				msg_print("The heroism wears off.");
#endif
				/* Recalculate hitpoints */
				p_ptr->update |= (PU_HP);
			}

			if (!p_ptr->fast)
			{
#ifdef JP
				msg_print("�����̑f�������Ȃ��Ȃ����悤���B");
#else
				msg_print("You feel yourself slow down.");
#endif
			}
		}

		{
			int dice = 1;
			int sides = plev * 3;

			if (info) return info_damage(dice, sides, 0);

			if (cont)
			{
				dispel_monsters(damroll(dice, sides));
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�����@���i�̏���";
		if (desc) return "���͂ȉ񕜂̉̂ŁA�����ƞN�O��Ԃ��S������B";
#else
		if (name) return "Relief of Yavanna";
		if (desc) return "Powerful healing song. Also heals cut and stun completely.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
			msg_print("�̂�ʂ��đ̂Ɋ��C���߂��Ă����D�D�D");
#else
			msg_print("Life flows through you as you sing the song...");
#endif
			start_singing(spell, MUSIC_H_LIFE);
		}

		{
			int dice = 15;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cont)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}

		break;

	case 29:
#ifdef JP
		if (name) return "�Đ��̉�";
		if (desc) return "���ׂẴX�e�[�^�X�ƌo���l���񕜂���B";
#else
		if (name) return "Goddess' rebirth";
		if (desc) return "Restores all stats and experience.";
#endif
    
		{
			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
#ifdef JP
				msg_print("�Í��̒��Ɍ��Ɣ����ӂ�܂����B�̂����̊��͂����߂����B");
#else
				msg_print("You strewed light and beauty in the dark as you sing. You feel refreshed.");
#endif
				(void)do_res_stat(A_STR);
				(void)do_res_stat(A_INT);
				(void)do_res_stat(A_WIS);
				(void)do_res_stat(A_DEX);
				(void)do_res_stat(A_CON);
				(void)do_res_stat(A_CHR);
				(void)restore_level();
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�T�E�����̖��p";
		if (desc) return "���ɋ��͂ł��������������̋�����B";
#else
		if (name) return "Wizardry of Sauron";
		if (desc) return "Fires an extremely powerful tiny ball of sound.";
#endif
    
		{
			int dice = 50 + plev;
			int sides = 10;
			int rad = 0;

			if (info) return info_damage(dice, sides, 0);

			/* Stop singing before start another */
			if (cast || fail) stop_singing();

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_SOUND, dir, damroll(dice, sides), rad);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�t�B���S���t�B���̒���";
		if (desc) return "�_���[�W���󂯂Ȃ��Ȃ�o���A�𒣂�B";
#else
		if (name) return "Fingolfin's Challenge";
		if (desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks.";
#endif
    
		/* Stop singing before start another */
		if (cast || fail) stop_singing();

		if (cast)
		{
#ifdef JP
				msg_print("�t�B���S���t�B���̖����ւ̒�����̂����D�D�D");
#else
				msg_print("You recall the valor of Fingolfin's challenge to the Dark Lord...");
#endif

				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);
		
				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);
		
				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				start_singing(spell, MUSIC_INVULN);
		}

		if (stop)
		{
			if (!p_ptr->invuln)
			{
#ifdef JP
				msg_print("���G�ł͂Ȃ��Ȃ����B");
#else
				msg_print("The invulnerability wears off.");
#endif
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}
		}

		break;
	}

	return "";
}


///mod140817 �̏W�łȂ��E�Ƃ��Ɠ��Z���璼�ڎg�����y��p�@�~�X�e�B�A�Ȃǂ��g��
//cast:�g�p���@cont:�p�������Q�[���^�[�����ƌ��� stop:���~�����b�Z�[�W
//��V���l�Ƃ͈Ⴂ�p���n�̉̂����Ȃ̂�start_singing��song_index��spell_index�͓����ɂ���
//�p�����R�X�g�v�Z�̂��߂�check_music()�ɕʏ����̋L�q���K�v
static cptr do_new_spell_music2(int spell, int mode)
{
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	int dir;
	int plev = p_ptr->lev;

	int cont_count = 0;

	if(!cast && !cont && !stop)
	{
		msg_format("ERROR:do_new_spell_music2()������`��mode(%d)�ŌĂ΂ꂽ",mode);
		return NULL;
	}

	//p_ptr->magic_num1[2](�J�n���Ă���̌o�߃^�[��)�̏���;
	if (cont)
	{
		if(p_ptr->magic_num1[2] < 1000)p_ptr->magic_num1[2]++;
		cont_count = p_ptr->magic_num1[2];
	}
	else
	{
		p_ptr->magic_num1[2] = 0;

	}


	switch (spell)
	{
	case MUSIC_NONE: //�̂��̂���߂��Ƃ�
		break;

	case MUSIC_NEW_MYSTIA_YAZYAKU: //�~�X�e�B�A�@�鐝�̉�
   		if (cast)
		{
			stop_singing();
			msg_print("�O�ȉ̐��������n�����E�E");
			start_singing(MUSIC_NEW_MYSTIA_YAZYAKU, MUSIC_NEW_MYSTIA_YAZYAKU);
		}

		if (cont)
		{
			//�������Ȃ��@��ӏ����͕ʂɋL�q
		}
		break;
	case MUSIC_NEW_MYSTIA_MYSTERIOUS: //�~�X�e�B�A�@�~�X�e���A�X�\���O
   		if (cast)
		{

			stop_singing();
			msg_print("�s�v�c�ȉ̐��������n�����E�E");
			start_singing(MUSIC_NEW_MYSTIA_MYSTERIOUS, MUSIC_NEW_MYSTIA_MYSTERIOUS);
		}

		if (cont)
		{
			int power = plev * 2+ chr_adj * 2;
			project_hack(GF_REDEYE, power);
			project_hack(GF_DEC_MAG, power );
		}
		break;
	case MUSIC_NEW_MYSTIA_CHORUSMASTER: //�~�X�e�B�A�@�^�钆�̃R�[���X�}�X�^�[
   		if (cast)
		{
			stop_singing();
			msg_print("�d�����̐��������n�����E�E");
			start_singing(MUSIC_NEW_MYSTIA_CHORUSMASTER, MUSIC_NEW_MYSTIA_CHORUSMASTER);
		}

		if (cont)
		{
				charm_monsters(randint1(chr_adj * 5) );
		}
		break;
	case MUSIC_NEW_KYOUKO_NENBUTSU: //���q�@�����O��
   		if (cast)
		{
			stop_singing();
			msg_print("�吺�ŔO���������n�߂��B");
			start_singing(MUSIC_NEW_KYOUKO_NENBUTSU, MUSIC_NEW_KYOUKO_NENBUTSU);
		}

		if (cont)
		{
			int dice = 5 + plev/5;
			if(one_in_(2)) msg_print("�u����`�ā`����`�ā`�E�E�v");
			else msg_print("�u���`�ށ`�Ɓ`�ǁ`����`�E�E�v");
			project_hack(GF_PUNISH_1, damroll(dice,dice));
		}
		break;
	case MUSIC_NEW_KYOUKO_SCREAM: //���q�@�v���C�}���X�N���[��
   		if (cast)
		{
			stop_singing();
			msg_print("���Ȃ��͑傫�������z�����񂾁E�E");
			start_singing(MUSIC_NEW_KYOUKO_SCREAM, MUSIC_NEW_KYOUKO_SCREAM);
		}

		if (cont)
		{

			if(one_in_(5))msg_print("����U�藐���M�������I");
			else if(one_in_(4))msg_print("���̃V���E�g��������I");
			else if(one_in_(3))msg_print("�������_���W������h�炵���I");
			else if(one_in_(2))msg_print("�}�����ꂽ�s�������񂾁I");
			else msg_print("�����̏C�s�̐h����吺�ŋ�s�����I");

			project_hack(GF_SOUND, randint1(plev * 2));
			aggravate_monsters(0,FALSE);

		}
		break;
	case MUSIC_NEW_LUNASA_SOLO:
		{
			if(cast)
			{
				start_singing(MUSIC_NEW_LUNASA_SOLO, MUSIC_NEW_LUNASA_SOLO);
			}
			else if(cont)
			{
				monster_type *m_ptr;
				monster_race *r_ptr;
				char m_name[80];
				int i;
				int delay;

				msg_print("���@�C�I�������A�T�ȉ���t�ł��E�E");
				for (i = 1; (i < m_max); i++)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if(p_ptr->riding == i) continue;
					if(!projectable(m_ptr->fy,m_ptr->fx,py,px)) continue;
					monster_desc(m_name, m_ptr, 0);
					delay = plev + randint1(plev);
					if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) delay /= 2;
					if(r_ptr->flags2 & RF2_GIGANTIC) delay -= 30;
					if(r_ptr->flags2 & RF2_POWERFUL) delay -= 20;
					if(r_ptr->flags2 & RF2_EMPTY_MIND) delay = 0;
					if(delay < 0) delay = 0;
					m_ptr->energy_need += delay;

					if(!delay)
						msg_format("%s�͑S������ł��Ȃ��B",m_name);
					else if(delay < 30)
						msg_format("%s�͏�������ł���B",m_name);
					else
						msg_format("%s�͋C���œ����Ă���悤���B",m_name);

				}
			}
			break;
		}
	case MUSIC_NEW_MERLIN_SOLO:
			if(cast)
			{
				msg_format("���Ȃ��͒ꔲ���Ƀn�C�e���V�����ȋȂ����t���n�߂��I");
				start_singing(MUSIC_NEW_MERLIN_SOLO, MUSIC_NEW_MERLIN_SOLO);
			}

		break;
	case MUSIC_NEW_LYRICA_SOLO:
			if(cast)
			{
				msg_format("���Ȃ��͌��z�̉���t�Ŏn�߂��E�E");
				start_singing(MUSIC_NEW_LYRICA_SOLO, MUSIC_NEW_LYRICA_SOLO);
			}
		break;
	case MUSIC_NEW_TSUKUMO_HEIKE: //�فX�@���Ƃ̑剅��
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print("���߂������i�̉����������E�E�B");
			start_singing(MUSIC_NEW_TSUKUMO_HEIKE, MUSIC_NEW_TSUKUMO_HEIKE);
		}

		if (cont)
		{
			int dice = 7 + plev/6;
			fire_random_target(GF_CAUSE_1,damroll(dice,dice),4,0,0);
		}
		break;
	case MUSIC_NEW_TSUKUMO_STORM: //�فX�������@���̃A���T���u��
   		if (cast)
		{
			stop_tsukumo_music();
			if(p_ptr->pclass == CLASS_BENBEN) msg_print("�����Ƌ��Ɍ��������y��t�ł͂��߂��B");
			else msg_print("�فX�Ƌ��Ɍ��������y��t�ł͂��߂��B");
			start_singing(MUSIC_NEW_TSUKUMO_STORM, MUSIC_NEW_TSUKUMO_STORM);
		}

		if (cont)
		{
			int base = plev * 2 + adj_general[p_ptr->stat_ind[A_CHR]] * 2;

			msg_print("���͂𔺂��嗒�������N�������I");
			project_hack2(GF_TSUKUMO_STORM,1,base,0);

		}
		break;

	case MUSIC_NEW_TSUKUMO_SCOREWEB: //�فX�X�R�A�E�F�u
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print("���Ȃ��͌��������i�������炵�n�߂��B");
			start_singing(MUSIC_NEW_TSUKUMO_SCOREWEB, MUSIC_NEW_TSUKUMO_SCOREWEB);
		}

		if (cont)
		{
			int dam = plev/2 + chr_adj / 2;
			int num = 5;
			for(;num>0;num--) fire_random_target(GF_LITE,dam,2,0,0);


		}
		break;

	case MUSIC_NEW_TSUKUMO_DOUBLESCORE: //�فX�_�u���X�R�A
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print("�����̌������Ȃ��̎���ɉQ�������E�E");
			start_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE, MUSIC_NEW_TSUKUMO_DOUBLESCORE);
		}
		break;
	case MUSIC_NEW_TSUKUMO_NORIGOTO: //�����@�V�ً̏�
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print("���Ȃ��͍U����҂��\���Ă���B");
			start_singing(MUSIC_NEW_TSUKUMO_NORIGOTO, MUSIC_NEW_TSUKUMO_NORIGOTO);
		}
		break;
	case MUSIC_NEW_TSUKUMO_JINKIN: //�����@�l�ՂƂ��ɖS��
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print("���Ȃ��͋�����ߕt����悤�Ȑ�����t�Ŏn�߂��B");
			start_singing(MUSIC_NEW_TSUKUMO_JINKIN, MUSIC_NEW_TSUKUMO_JINKIN);
		}

		break;
	case MUSIC_NEW_TSUKUMO_ECHO: //�����@�G�R�[�`�F���o�[
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print("���Ȃ��̎���ɖ��͂��Q�������E�E");
			start_singing(MUSIC_NEW_TSUKUMO_ECHO, MUSIC_NEW_TSUKUMO_ECHO);
		}
		break;
	case MUSIC_NEW_TSUKUMO_ECHO2: //�����@�����̎c��
   		if (cast)
		{
			stop_tsukumo_music();
			msg_print("���Ȃ��̉��͊�Ȕ��������n�߂��B");
			start_singing(MUSIC_NEW_TSUKUMO_ECHO2, MUSIC_NEW_TSUKUMO_ECHO2);
		}

		break;

	case MUSIC_NEW_RAIKO_AYANOTUDUMI:
		//�A���m�c�d�~�@������
		if (cast)
		{
			stop_raiko_music();
			msg_print("�H���ȋ������_���W�����ɐ��ݓ������D�D�D");

			start_singing(MUSIC_NEW_RAIKO_AYANOTUDUMI, MUSIC_NEW_RAIKO_AYANOTUDUMI);
		}

		{//cast�ł�cont�ł������ɓ���
			int rad = DETECT_RAD_DEFAULT;


			if (cont)
			{
				if (plev > 24 )
				{
					map_area(rad);
				}
				if (plev > 29 )
				{
					detect_objects_gold(rad);
					detect_objects_normal(rad);
				}
				if (plev > 34)
				{
					detect_traps(rad, TRUE);
				}
				detect_monsters_invis(rad);
				detect_monsters_normal(rad);


			}
		}

		break;
	case MUSIC_NEW_RAIKO_DENDEN: //�{��̃f���f������
   		if (cast)
		{
			stop_raiko_music();
			msg_print("���Ȃ��͌������h������ł��炵�n�߂��I");
			start_singing(MUSIC_NEW_RAIKO_DENDEN, MUSIC_NEW_RAIKO_DENDEN);
		}
		if (cont)
		{
			int dam = plev*3 + randint1(adj_general[p_ptr->stat_ind[A_CHR]]*5);
			int typ;
			if(one_in_(2)) typ = GF_ELEC;
			else typ = GF_SOUND;
			fire_random_target(typ,dam,2,0,0);

		}
		break;
	case MUSIC_NEW_RAIKO_PRISTINE: //�v���X�e�B���r�[�g
   		if (cast)
		{
			stop_raiko_music();
			msg_print("�n���̍����Ăъo�܂����I");
			start_singing(MUSIC_NEW_RAIKO_PRISTINE, MUSIC_NEW_RAIKO_PRISTINE);
		}
		break;
	case MUSIC_NEW_MYSTIA_HUMAN_CAGE: //�~�X�e�B�A�@�q���[�}���P�[�W
		if (cast)
		{
			stop_singing();
			msg_print("������ߕt����悤�ȉ̐��������n�����E�E");
			start_singing(MUSIC_NEW_MYSTIA_HUMAN_CAGE, MUSIC_NEW_MYSTIA_HUMAN_CAGE);
		}
		if (cont)
		{
			//�������Ȃ��@�����͕ʂɋL�q
		}
		break;

	case MUSIC_NEW_WAKASAGI_SCHOOLOFFISH: //�킩�����P�@�X�N�[���I�u�t�B�b�V��
		if (cast)
		{
			stop_singing();
			msg_print("���Ȃ��͋������Ɍ����Ċy�����ɉ̂��n�߂��B");
			start_singing(MUSIC_NEW_WAKASAGI_SCHOOLOFFISH, MUSIC_NEW_WAKASAGI_SCHOOLOFFISH);
		}

		if (cont)
		{
			project_hack(GF_CONTROL_FISH, plev + chr_adj * 3);
		}
		break;

	case MUSIC_NEW_SANNYO_SMOKE:
		if (cast)
		{
			msg_print("���Ȃ��͂��C�ɓ���̉������z���n�߂��B");
			start_singing(MUSIC_NEW_SANNYO_SMOKE, MUSIC_NEW_SANNYO_SMOKE);
		}

		if (cont)
		{
			player_gain_mana( 1 + plev / 15 + randint1((plev+10) / 15));
		}
		break;

	case MUSIC_NEW_SANNYO_FEAR:
		if (cast)
		{
			msg_print("���Ȃ��͋�ݑ��鉌�����z���n�߂��B");
			start_singing(MUSIC_NEW_SANNYO_FEAR, MUSIC_NEW_SANNYO_FEAR);
		}

		if (cont)
		{
			int power = 20 + plev * 2 + cont_count * (5 + plev / 2);

			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_TURN_ALL,(PROJECT_KILL|PROJECT_JUMP|PROJECT_HIDE), -1);
		}
		break;
	case MUSIC_NEW_SANNYO_CONF:
		if (cast)
		{
			msg_print("���Ȃ��͊Â����邢�������z���n�߂��B");
			start_singing(MUSIC_NEW_SANNYO_CONF, MUSIC_NEW_SANNYO_CONF);
		}

		if (cont)
		{
			int power = 20 + plev * 2 + cont_count * (5 + plev / 2);
			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_OLD_CONF, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);
		}
		break;

	case MUSIC_NEW_SANNYO_STUN:
		if (cast)
		{
			msg_print("���Ȃ��͓˂�������悤�ȉ������z���n�߂��B");
			start_singing(MUSIC_NEW_SANNYO_STUN, MUSIC_NEW_SANNYO_STUN);
		}

		if (cont)
		{
			int power = 20 + plev * 2 + cont_count * (5 + plev / 2);
			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_STUN, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);
		}
		break;

	case MUSIC_NEW_SANNYO_SLOW:
		if (cast)
		{
			msg_print("���Ȃ��͂�������d���������z���n�߂��B");
			start_singing(MUSIC_NEW_SANNYO_SLOW, MUSIC_NEW_SANNYO_SLOW);
		}

		if (cont)
		{
			int power = 20 + plev * 2+ cont_count * (5 + plev / 2);
			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_OLD_SLOW, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);
		}
		break;
	case MUSIC_NEW_SANNYO_MINDBLAST:
		if (cast)
		{
			msg_print("���Ȃ��͌���ɔZ���ȉ������z���n�߂��B");
			start_singing(MUSIC_NEW_SANNYO_MINDBLAST, MUSIC_NEW_SANNYO_MINDBLAST);
		}

		if (cont)
		{
			int power = 50 + plev * 2 + cont_count * (5 + plev / 2);
			if (power > 400) power = 400;
			project(0, 2 + plev / 15, py, px, power * 2, GF_REDEYE, (PROJECT_KILL | PROJECT_JUMP | PROJECT_HIDE), -1);
		}
		break;
	case MUSIC_NEW_SANNYO_BERSERK:
		if (cast)
		{
			msg_print("�Ƃ��Ă����̉����t�ɉ΂�_�����I");
			start_singing(MUSIC_NEW_SANNYO_BERSERK, MUSIC_NEW_SANNYO_BERSERK);
			set_shero(500, FALSE);
			p_ptr->update |= PU_BONUS;
			update_stuff();
			hp_player(1000);

		}
		if (stop)
		{
			set_shero(0, TRUE);
			p_ptr->update |= PU_BONUS;
			update_stuff();
		}
		break;


		//�p�����R�X�g�v�Z�̂��߂�check_music()�ɂ������̋L�q���K�v

		//13�ӂ肩�猳�̋�V���l���ʂ��e�����Ă���B���ӁB
		//��MUSIC_XXXX��+100���Ďg���Ȃ����Ƃ���

	default:
		msg_format("ERROR:���̉��yIDX(%d)��do_new_spell_music2()�ɒ�`����Ă��Ȃ�",spell);
		return NULL;
	}

	return "";
}

/*:::���p�Ƃ̕K�E��*/
///mod140913
/*:::���[�h"check"�𓱓��B����̎�ނȂǂ����؂��Z���g�p�\�Ȃ�"",�s�\�Ȃ�NULL��Ԃ��B*/
static cptr do_hissatsu_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool check =  (mode == SPELL_CHECK) ? TRUE : FALSE;

	bool have_weapon = buki_motteruka(INVEN_RARM) || buki_motteruka(INVEN_LARM);
	bool have_edge = (object_has_a_blade(&inventory[INVEN_RARM]) || object_has_a_blade(&inventory[INVEN_LARM]));
	bool have_sword = (inventory[INVEN_RARM].tval == TV_SWORD || inventory[INVEN_LARM].tval == TV_SWORD ||inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA );



	int dir;
	int plev = p_ptr->lev;

	switch (spell)
	{
	case 0:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "��эj";
		if (desc) return "(2+���x��/16)�}�X���ꂽ�Ƃ���ɂ��郂���X�^�[���U������B���������Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Tobi-Izuna";
		if (desc) return "Attacks a two squares distant monster.";
#endif
    
		if (cast)
		{
			project_length = 2 + plev / 16;
			if (!get_aim_dir(&dir)) return NULL;

			project_hook(GF_ATTACK, dir, HISSATSU_2, PROJECT_STOP | PROJECT_KILL);
		}
		break;

	case 1:
		if(check)
		{
			if(have_edge) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "�܌��J�a��";
		if (desc) return "3�����ɑ΂��čU������B�n�̂��镐��������Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "3-Way Attack";
		if (desc) return "Attacks in 3 directions in one time.";
#endif
    
		if (cast)
		{
			int cdir;
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			for (cdir = 0;cdir < 8; cdir++)
			{
				if (cdd[cdir] == dir) break;
			}

			if (cdir == 8) return NULL;

			y = py + ddy_cdd[cdir];
			x = px + ddx_cdd[cdir];
			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_3WAY);
			else
#ifdef JP
				msg_print("�U���͋��؂����B");
#else
				msg_print("You attack the empty air.");
#endif
			y = py + ddy_cdd[(cdir + 7) % 8];
			x = px + ddx_cdd[(cdir + 7) % 8];
			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_3WAY);
			else
#ifdef JP
				msg_print("�U���͋��؂����B");
#else
				msg_print("You attack the empty air.");
#endif
			y = py + ddy_cdd[(cdir + 1) % 8];
			x = px + ddx_cdd[(cdir + 1) % 8];
			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_3WAY);
			else
#ifdef JP
				msg_print("�U���͋��؂����B");
#else
				msg_print("You attack the empty air.");
#endif
		}
		break;

	case 2:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "�u�[������";
		if (desc) return "������茳�ɖ߂��Ă���悤�ɓ�����B�߂��Ă��Ȃ����Ƃ�����A�󂯎~�߂����˂邱�Ƃ�����B���X���������ɖ߂��Ă��镐��̏ꍇ�A����ɖ߂�₷���󂯂₷���Ȃ�B��������������Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Boomerang";
		if (desc) return "Throws current weapon. And it'll return to your hand unless failed.";
#endif
    
		if (cast)
		{
			if (!do_cmd_throw_aux(1, TRUE, -1)) return NULL;
		}
		break;

	case 3:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}

#ifdef JP
		if (name) return "����";
		if (desc) return "�Ή��ϐ��̂Ȃ������X�^�[�ɑ�_���[�W��^����B��������������Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Burning Strike";
		if (desc) return "Attacks a monster with more damage unless it has resistance to fire.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_FIRE);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 4:
		if(check) return "";
#ifdef JP
		if (name) return "�E�C���m";
		if (desc) return "�߂��̎v�l���邱�Ƃ��ł��郂���X�^�[�����m����B";
#else
		if (name) return "Detect Ferocity";
		if (desc) return "Detects all monsters except mindless in your vicinity.";
#endif
    
		if (cast)
		{
			detect_monsters_mind(DETECT_RAD_DEFAULT);
		}
		break;

	case 5:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "�݂ˑł�";
		if (desc) return "����Ƀ_���[�W��^���Ȃ����A�N�O�Ƃ�����B��������������Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Strike to Stun";
		if (desc) return "Attempts to stun a monster in the adjacent.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_MINEUCHI);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 6:
#ifdef JP
		if(check) return "";
		if (name) return "�J�E���^�[";
		if (desc) return "����ɍU�����ꂽ�Ƃ��ɔ�������B�������邽�т�MP�������B";
#else
		if (name) return "Counter";
		if (desc) return "Prepares to counterattack. When attack by a monster, strikes back using SP each time.";
#endif
    
		if (cast)
		{
			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("��n���ɂ͖������B");
#else
				msg_print("You cannot do it when riding.");
#endif
				return NULL;
			}
#ifdef JP
			msg_print("����̍U���ɑ΂��Đg�\�����B");
#else
			msg_print("You prepare to counter blow.");
#endif
			p_ptr->counter = TRUE;
		}
		break;

	case 7:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}

#ifdef JP
		if (name) return "��������";
		if (desc) return "�U��������A���Α��ɔ�����B��������������Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Harainuke";
		if (desc) return "Attacks monster with your weapons normally, then move through counter side of the monster.";
#endif
    
		if (cast)
		{
			int y, x;

			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("��n���ɂ͖������B");
#else
				msg_print("You cannot do it when riding.");
#endif
				return NULL;
			}
	
			if (!get_rep_dir2(&dir)) return NULL;
	
			if (dir == 5) return NULL;
			y = py + ddy[dir];
			x = px + ddx[dir];
	
			if (!cave[y][x].m_idx)
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
	
			py_attack(y, x, 0);
	
			if (!player_can_enter(cave[y][x].feat, 0) || is_trap(cave[y][x].feat))
				break;
	
			y += ddy[dir];
			x += ddx[dir];
	
			if (player_can_enter(cave[y][x].feat, 0) && !is_trap(cave[y][x].feat) && !cave[y][x].m_idx)
			{
				msg_print(NULL);
	
				/* Move the player */
				(void)move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
			}
		}
		break;

	case 8:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}

#ifdef JP
		if (name) return "�T�[�y���c�^��";
		if (desc) return "�őϐ��̂Ȃ������X�^�[�ɑ�_���[�W��^����B��������������Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Serpent's Tongue";
		if (desc) return "Attacks a monster with more damage unless it has resistance to poison.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_POISON);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 9:
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "�a������̑���";
		if (desc) return "���ׂ̐��͂ɑ�����A���f�b�h�∫���ɑ�_���[�W��^���邪�A���̃����X�^�[�ɂ͑S�����ʂ��Ȃ��B�������𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Zammaken";
		if (desc) return "Attacks an evil unliving monster with great damage. No effect to other  monsters.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_ZANMA);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
		if (name) return "������";
		if (desc) return "�U���������������֐�����΂��B�������𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Wind Blast";
		if (desc) return "Attacks an adjacent monster, and blow it away.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_FUKI);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
				return "";
			}
			if (cave[y][x].m_idx)
			{
				int i;
				int ty = y, tx = x;
				int oy = y, ox = x;
				int m_idx = cave[y][x].m_idx;
				monster_type *m_ptr = &m_list[m_idx];
				char m_name[80];
	
				monster_desc(m_name, m_ptr, 0);
	
				for (i = 0; i < 5; i++)
				{
					y += ddy[dir];
					x += ddx[dir];
					if (cave_empty_bold(y, x))
					{
						ty = y;
						tx = x;
					}
					else break;
				}
				if ((ty != oy) || (tx != ox))
				{
#ifdef JP
					msg_format("%s�𐁂���΂����I", m_name);
#else
					msg_format("You blow %s away!", m_name);
#endif
					cave[oy][ox].m_idx = 0;
					cave[ty][tx].m_idx = m_idx;
					m_ptr->fy = ty;
					m_ptr->fx = tx;
	
					update_mon(m_idx, TRUE);
					lite_spot(oy, ox);
					lite_spot(ty, tx);
	
					if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
						p_ptr->update |= (PU_MON_LITE);
				}
			}
		}
		break;

	case 11:
#ifdef JP
		if(check) return "";
		if (name) return "�����̖ڗ���";
		if (desc) return "����E�h���1���ʂ���B���x��45�ȏ�ŕ���E�h��̔\�͂����S�ɒm�邱�Ƃ��ł���B";
#else
		if (name) return "Judge";
		if (desc) return "Identifies a weapon or armor. Or *identifies* these at level 45.";
#endif
    
		if (cast)
		{
			if (plev > 44)
			{
				if (!identify_fully(TRUE)) return NULL;
			}
			else
			{
				if (!ident_spell(TRUE)) return NULL;
			}
		}
		break;

	case 12:
#ifdef JP
		if(check)
		{
			if(have_edge) return "";
			else return NULL;
		}
		if (name) return "�j��a";
		if (desc) return "����󂵁A��Όn�̃����X�^�[�ɑ�_���[�W��^����B�n�̂��镐��𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Rock Smash";
		if (desc) return "Breaks rock. Or greatly damage a monster made by rocks.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_HAGAN);
	
			if (!cave_have_flag_bold(y, x, FF_HURT_ROCK)) break;
	
			/* Destroy the feature */
			cave_alter_feat(y, x, FF_HURT_ROCK);
	
			/* Update some things */
			p_ptr->update |= (PU_FLOW);
		}
		break;

	case 13:
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "����ጎ��";
		if (desc) return "�U���񐔂������A��C�ϐ��̂Ȃ������X�^�[�ɑ�_���[�W��^����B�������𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Midare-Setsugekka";
		if (desc) return "Attacks a monster with increased number of attacks and more damage unless it has resistance to cold.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_COLD);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 14:
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
#ifdef JP
		if (name) return "�}���˂�";
		if (desc) return "�����X�^�[���ꌂ�œ|���U�����J��o���B���s�����1�_�����_���[�W��^�����Ȃ��B����𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Spot Aiming";
		if (desc) return "Attempts to kill a monster instantly. If failed cause only 1HP of damage.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_KYUSHO);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_AXE || inventory[INVEN_LARM].tval == TV_AXE) return "";
			else return NULL;
		}
		if (name) return "���_�a��";
		if (desc) return "�U����������ΕK����S�̈ꌂ�ɂȂ�B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Majingiri";
		if (desc) return "Attempts to attack with critical hit. But this attack is easy to evade for a monster.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_MAJIN);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 16:
#ifdef JP
		if(check) return "";
		if (name) return "�̂Đg";
		if (desc) return "���͂ȍU�����J��o���B���̃^�[���܂ł̊ԁA�H�炤�_���[�W��������B";
#else
		if (name) return "Desperate Attack";
		if (desc) return "Attacks with all of your power. But all damages you take will be doubled for one turn.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_SUTEMI);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
			p_ptr->sutemi = TRUE;
		}
		break;

	case 17:
#ifdef JP
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}

		if (name) return "�����h�܎a";
		if (desc) return "�d���ϐ��̂Ȃ������X�^�[�ɔ��ɑ傫���_���[�W��^����B�������𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Lightning Eagle";
		if (desc) return "Attacks a monster with more damage unless it has resistance to electricity.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_ELEC);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if(check) return "";
		if (name) return "���g";
		if (desc) return "�f��������ɋߊ��U������B";
#else
		if (name) return "Rush Attack";
		if (desc) return "Steps close to a monster and attacks at a time.";
#endif
    
		if (cast)
		{
			if (!rush_attack(NULL,0,0)) return NULL;
		}
		break;

	case 19:
#ifdef JP
		if(check) return "";
		if (name) return "�ԗ��Q";
		if (desc) return "�������g���������A���̏����[���قǑ傫���З͂őS�����̓G���U���ł���B�����Ă��Ȃ������X�^�[�ɂ͌��ʂ��Ȃ��B";
#else
		if (name) return "Bloody Maelstrom";
		if (desc) return "Attacks all adjacent monsters with power corresponding to your cut status. Then increases your cut status. No effect to unliving monsters.";
#endif
    
		if (cast)
		{
			int y = 0, x = 0;

			cave_type       *c_ptr;
			monster_type    *m_ptr;
	
			if (p_ptr->cut < 300)
				set_cut(p_ptr->cut + 300);
			else
				set_cut(p_ptr->cut * 2);
	
			for (dir = 0; dir < 8; dir++)
			{
				y = py + ddy_ddd[dir];
				x = px + ddx_ddd[dir];
				c_ptr = &cave[y][x];
	
				/* Get the monster */
				m_ptr = &m_list[c_ptr->m_idx];
	
				/* Hack -- attack monsters */
				if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
				{
					if (!monster_living(&r_info[m_ptr->r_idx]))
					{
						char m_name[80];
	
						monster_desc(m_name, m_ptr, 0);
#ifdef JP
						msg_format("%s�ɂ͌��ʂ��Ȃ��I", m_name);
#else
						msg_format("%s is unharmed!", m_name);
#endif
					}
					else py_attack(y, x, HISSATSU_SEKIRYUKA);
				}
			}
		}
		break;

	case 20:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].weight >= 100 || inventory[INVEN_LARM].weight >= 100) return "";
			else return NULL;
		}

		if (name) return "���k��";
		if (desc) return "�n�k���N�����B5kg�ȏ�̕���𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Earthquake Blow";
		if (desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif
    
		if (cast)
		{
			int y,x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_QUAKE);
			else
				earthquake(py, px, 10);
		}
		break;

	case 21:
#ifdef JP
		if(check)
		{
			if(have_weapon) return "";
			else return NULL;
		}
		if (name) return "�n����";
		if (desc) return "�Ռ��g�̃r�[������B����𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Crack";
		if (desc) return "Fires a beam of shock wave.";
#endif
    
		if (cast)
		{
			int total_damage = 0, basedam, i;
			u32b flgs[TR_FLAG_SIZE];
			object_type *o_ptr;
			if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
			msg_print("�����傫���U�艺�낵���B");
#else
			msg_print("You swing your weapon downward.");
#endif
			///item sys ���p�̒n����@�_���[�W�v�Z�Ƀ��H�[�p�������g�p
			for (i = 0; i < 2; i++)
			{
				int damage;
	
				if (!buki_motteruka(INVEN_RARM+i)) break;
				o_ptr = &inventory[INVEN_RARM+i];
				basedam = (o_ptr->dd * (o_ptr->ds + 1)) * 50;
				damage = o_ptr->to_d * 100;
				object_flags(o_ptr, flgs);
			//	if ((o_ptr->name1 == ART_VORPAL_BLADE) || (o_ptr->name1 == ART_CHAINSWORD))
				if (have_flag(flgs, TR_EX_VORPAL))
				{
					/* vorpal blade */
					basedam *= 5;
					basedam /= 3;
				}
				else if (have_flag(flgs, TR_VORPAL))
				{
					/* vorpal flag only */
					basedam *= 11;
					basedam /= 9;
				}
				damage += basedam;
				damage *= p_ptr->num_blow[i];
				total_damage += damage / 200;
				if (i) total_damage = total_damage*7/10;
			}
			fire_beam(GF_FORCE, dir, total_damage);
		}
		break;

	case 22:
#ifdef JP
		if(check) return "";
		if (name) return "�C���̗Y����";
		if (desc) return "���E���̑S�����X�^�[�ɑ΂��č����̍U�����s���B�������߂��ɂ��郂���X�^�[���N����B";
#else
		if (name) return "War Cry";
		if (desc) return "Damages all monsters in sight with sound. Aggravate nearby monsters.";
#endif
    
		if (cast)
		{
#ifdef JP
			msg_print("�Y���т��������I");
#else
			msg_print("You roar out!");
#endif
			project_hack(GF_SOUND, randint1(plev * 3));
			aggravate_monsters(0,FALSE);
		}
		break;

	case 23:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_SPEAR || inventory[INVEN_LARM].tval == TV_SPEAR) return "";
			else if(inventory[INVEN_RARM].tval == TV_POLEARM || inventory[INVEN_LARM].tval == TV_POLEARM) return "";
			else return NULL;
		}

		if (name) return "���o�O�i";
		if (desc) return "���͂�3�i�U�����J��o���B������������𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Musou-Sandan";
		if (desc) return "Attacks with powerful 3 strikes.";
#endif
    
		if (cast)
		{
			int i;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			for (i = 0; i < 3; i++)
			{
				int y, x;
				int ny, nx;
				int m_idx;
				cave_type *c_ptr;
				monster_type *m_ptr;
	
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];
	
				if (c_ptr->m_idx)
					py_attack(y, x, HISSATSU_3DAN);
				else
				{
#ifdef JP
					msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
					msg_print("There is no monster.");
#endif
					return NULL;
				}
	
				if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
				{
					return "";
				}
	
				/* Monster is dead? */
				if (!c_ptr->m_idx) break;
	
				ny = y + ddy[dir];
				nx = x + ddx[dir];
				m_idx = c_ptr->m_idx;
				m_ptr = &m_list[m_idx];
	
				/* Monster cannot move back? */
				if (!monster_can_enter(ny, nx, &r_info[m_ptr->r_idx], 0))
				{
					/* -more- */
					if (i < 2) msg_print(NULL);
					continue;
				}
	
				c_ptr->m_idx = 0;
				cave[ny][nx].m_idx = m_idx;
				m_ptr->fy = ny;
				m_ptr->fx = nx;
	
				update_mon(m_idx, TRUE);
	
				/* Redraw the old spot */
				lite_spot(y, x);
	
				/* Redraw the new spot */
				lite_spot(ny, nx);
	
				/* Player can move forward? */
				if (player_can_enter(c_ptr->feat, 0))
				{
					/* Move the player */
					if (!move_player_effect(y, x, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP)) break;
				}
				else
				{
					break;
				}

				/* -more- */
				if (i < 2) msg_print(NULL);
			}
		}
		break;

	case 24:
#ifdef JP
		if(check) return "";
		if (name) return "�z���S�̉�";
		if (desc) return "�U����������̗̑͂��z���Ƃ�A�����̗̑͂��񕜂�����B�����������Ȃ������X�^�[�ɂ͒ʂ��Ȃ��B";
#else
		if (name) return "Vampire's Fang";
		if (desc) return "Attacks with vampiric strikes which absorbs HP from a monster and gives them to you. No effect to unliving monsters.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_DRAIN);
			else
			{
#ifdef JP
					msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
					msg_print("There is no monster.");
#endif
				return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if(check) return "";
		if (name) return "���f";
		if (desc) return "���E���̋N���Ă���S�����X�^�[�ɞN�O�A�����A�����^���悤�Ƃ���B";
#else
		if (name) return "Moon Dazzling";
		if (desc) return "Attempts to stun, confuse and sleep all waking monsters.";
#endif
    
		if (cast)
		{
#ifdef JP

			if(have_weapon) msg_print("�����s�K���ɗh�炵���D�D�D");
			else msg_print("�s�v�c�ȗx���x�����B");
#else
			msg_print("You irregularly wave your weapon...");
#endif
			project_hack(GF_ENGETSU, plev * 4);
			project_hack(GF_ENGETSU, plev * 4);
			project_hack(GF_ENGETSU, plev * 4);
		}
		break;

	case 26:
#ifdef JP
		if(check) return "";
		if (name) return "�S�l�a��";
		if (desc) return "�A�����ē��g�Ń����X�^�[���U������B�U�����邽�т�MP������BMP���Ȃ��Ȃ邩�A�����X�^�[��|���Ȃ�������S�l�a��͏I������B";
#else
		if (name) return "Hundred Slaughter";
		if (desc) return "Performs a series of rush attacks. The series continues while killing each monster in a time and SP remains.";
#endif
    
		if (cast)
		{
			const int mana_cost_per_monster = 8;
			bool flag_new = TRUE;
			bool mdeath;

			do
			{
				if (!rush_attack(&mdeath,0,0)) break;
				if (flag_new)
				{
					/* Reserve needed mana point */
				//	p_ptr->csp -= technic_info[REALM_HISSATSU - MIN_TECHNIC][26].smana;
					p_ptr->csp -= hissatsu_info[26].smana;
					flag_new = FALSE;
				}
				else
					p_ptr->csp -= mana_cost_per_monster;

				if (!mdeath) break;
				command_dir = 0;

				p_ptr->redraw |= PR_MANA;
				handle_stuff();
			}
			while (p_ptr->csp > mana_cost_per_monster);

			if (flag_new) return NULL;
	
			/* Restore reserved mana */
			p_ptr->csp += hissatsu_info[26].smana;
		}
		break;

	case 27:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;

		}
		if (name) return "�V�ė��M";
		if (desc) return "���E���̏ꏊ���w�肵�āA���̏ꏊ�Ǝ����̊Ԃɂ���S�����X�^�[���U�����A���̏ꏊ�Ɉړ�����B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Dragonic Flash";
		if (desc) return "Runs toward given location while attacking all monsters on the path.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!tgt_pt(&x, &y)) return NULL;

			if (!cave_player_teleportable_bold(y, x, 0L) ||
			    (distance(y, x, py, px) > MAX_SIGHT / 2) ||
			    !projectable(py, px, y, x))
			{
#ifdef JP
				msg_print("���s�I");
#else
				msg_print("You cannot move to that place!");
#endif
				break;
			}
			if (p_ptr->anti_tele)
			{
#ifdef JP
				msg_print("�s�v�c�ȗ͂��e���|�[�g��h�����I");
#else
				msg_print("A mysterious force prevents you from teleporting!");
#endif
	
				break;
			}
			project(0, 0, y, x, HISSATSU_ISSEN, GF_ATTACK, PROJECT_BEAM | PROJECT_KILL, -1);
			teleport_player_to(y, x, 0L);
		}
		break;

	case 28:
#ifdef JP
		if(check)
		{
			if(have_sword) return "";
			else return NULL;
		}
		if (name) return "��d�̌���";
		if (desc) return "1�^�[����2�x�U�����s���B�������𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Twin Slash";
		if (desc) return "double attacks at a time.";
#endif
    
		if (cast)
		{
			int x, y;
	
			if (!get_rep_dir(&dir, FALSE)) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
			{
				py_attack(y, x, HISSATSU_DOUBLE);
				if (cave[y][x].m_idx)
				{
					handle_stuff();
					py_attack(y, x, HISSATSU_DOUBLE);
				}
			}
			else
			{
#ifdef JP
	msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("You don't see any monster in this direction");
#endif
				return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}

		if (name) return "�Օ��Ⓛ��";
		if (desc) return "���͂ȍU�����s���A�߂��̏ꏊ�ɂ����ʂ��y�ԁB���𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Kofuku-Zettousei";
		if (desc) return "Performs a powerful attack which even effect nearby monsters.";
#endif
    
		if (cast)
		{
			int total_damage = 0, basedam, i;
			int y, x;
			u32b flgs[TR_FLAG_SIZE];
			object_type *o_ptr;
	
			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("�Ȃ����U�����邱�Ƃ��ł��Ȃ��B");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return "";
			}
#ifdef JP
			msg_print("�����傫���U�艺�낵���B");
#else
			msg_print("You swing your weapon downward.");
#endif
			for (i = 0; i < 2; i++)
			{
				///item sys ���p�@�_���[�W�v�Z�Ƀ��H�[�p�������g�p
				int damage;
				if (!buki_motteruka(INVEN_RARM+i)) break;
				if(inventory[INVEN_RARM+i].tval != TV_KATANA) continue;

				o_ptr = &inventory[INVEN_RARM+i];
				basedam = (o_ptr->dd * (o_ptr->ds + 1)) * 50;
				damage = o_ptr->to_d * 100;
				object_flags(o_ptr, flgs);
				if (have_flag(flgs, TR_EX_VORPAL))
				{
					/* vorpal blade */
					basedam *= 5;
					basedam /= 3;
				}
				else if (have_flag(flgs, TR_VORPAL))
				{
					/* vorpal flag only */
					basedam *= 11;
					basedam /= 9;
				}
				damage += basedam;
				damage += p_ptr->to_d[i] * 100;
				damage *= p_ptr->num_blow[i];
				total_damage += (damage / 100);
			}
			project(0, (cave_have_flag_bold(y, x, FF_PROJECT) ? 5 : 0), y, x, total_damage * 2, GF_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
		}
		break;

	case 30:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "�c�_�S�E��";
		if (desc) return "�������_���[�W�����炤���A����ɔ��ɑ傫�ȃ_���[�W��^����B�A���f�b�h�ɂ͓��Ɍ��ʂ�����B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Keiun-Kininken";
		if (desc) return "Attacks a monster with extremely powerful damage. But you also takes some damages. Hurts a undead monster greatly.";
#endif
    
		if (cast)
		{
			int y, x;

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;

			y = py + ddy[dir];
			x = px + ddx[dir];

			if (cave[y][x].m_idx)
				py_attack(y, x, HISSATSU_UNDEAD);
			else
			{
#ifdef JP
				msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
				msg_print("There is no monster.");
#endif
				return NULL;
			}
#ifdef JP
			take_hit(DAMAGE_NOESCAPE, 100 + randint1(100), "�c�_�S�E�����g�����Ռ�", -1);
#else
			take_hit(DAMAGE_NOESCAPE, 100 + randint1(100), "exhaustion on using Keiun-Kininken", -1);
#endif
		}
		break;

	case 31:
#ifdef JP
		if(check)
		{
			if(inventory[INVEN_RARM].tval == TV_KATANA || inventory[INVEN_LARM].tval == TV_KATANA) return "";
			else return NULL;
		}
		if (name) return "�����a��";
		if (desc) return "�u�����a���l�ɂȂ�ɂ͓�S�N�͊|����ƌ����B�v�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@�@(�������z)�@�@�@�@���E���̑S�Ăɑ΂����ʍ��������̍U�����d�|����B���̍U���ł͍s�����Ԃ��قƂ�Ǐ���Ȃ��B���𑕔����Ă��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Harakiri";
		if (desc) return "'Busido is found in death'";
#endif

		if (cast)
		{
			int dam = 0;
			if(inventory[INVEN_RARM].tval == TV_KATANA ) dam += calc_weapon_dam(0);
			if(inventory[INVEN_LARM].tval == TV_KATANA ) dam += calc_weapon_dam(1);
			msg_print("���Ȃ��̌��͈��ʂ�؂�􂢂��I");
			project_hack2(GF_NEXUS,0,0,dam);

		}
		break;
	}

	return "";
}


/* Hex */
static bool item_tester_hook_weapon_except_bow(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
	case TV_KNIFE:
	case TV_SWORD:
	case TV_KATANA:
	case TV_HAMMER:
	case TV_STICK:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:		
	case TV_OTHERWEAPON:	

		{
			return (TRUE);
		}
	}

	return (FALSE);
}



///sysdel realm ��p
#if 0
static cptr do_hex_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool cont = (mode == SPELL_CONT) ? TRUE : FALSE;
	bool stop = (mode == SPELL_STOP) ? TRUE : FALSE;

	bool add = TRUE;

	int plev = p_ptr->lev;
	int power;

	switch (spell)
	{
	/*** 1st book (0-7) ***/
	case 0:
#ifdef JP
		if (name) return "�ׂȂ�j��";
		if (desc) return "�j���ɂ��U�����x�Ɩh��͂��オ��B";
#else
		if (name) return "Evily blessing";
		if (desc) return "Attempts to increase +to_hit of a weapon and AC";
#endif
		if (cast)
		{
			if (!p_ptr->blessed)
			{
#ifdef JP
				msg_print("�����ȋC���ɂȂ����I");
#else
				msg_print("You feel righteous!");
#endif
			}
		}
		if (stop)
		{
			if (!p_ptr->blessed)
			{
#ifdef JP
				msg_print("�����ȋC���������������B");
#else
				msg_print("The prayer has expired.");
#endif
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�y���̎���";
		if (desc) return "HP�⏝�������񕜂�����B";
#else
		if (name) return "Cure light wounds";
		if (desc) return "Heals cut and HP a little.";
#endif
		if (info) return info_heal(1, 10, 0);
		if (cast)
		{
#ifdef JP
			msg_print("�C�����ǂ��Ȃ��Ă���B");
#else
			msg_print("You feel better and better.");
#endif
		}
		if (cast || cont)
		{
			hp_player(damroll(1, 10));
			set_cut(p_ptr->cut - 10);
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�����̃I�[��";
		if (desc) return "���̃I�[����g�ɂ܂Ƃ��A�񕜑��x�������Ȃ�B";
#else
		if (name) return "Demonic aura";
		if (desc) return "Gives fire aura and regeneration.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("�̂����̃I�[���ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by fiery aura!");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("���̃I�[���������������B");
#else
			msg_print("Fiery aura disappeared.");
#endif
		}
		break;

	case 3:
#ifdef JP
		if (name) return "���L��";
		if (desc) return "���E���̃����X�^�[�ɔ���ʂ̓ł̃_���[�W��^����B";
#else
		if (name) return "Stinking mist";
		if (desc) return "Deals few damages of poison to all monsters in your sight.";
#endif
		power = plev / 2 + 5;
		if (info) return info_damage(1, power, 0);
		if (cast || cont)
		{
			project_hack(GF_POIS, randint1(power));
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�r�͋���";
		if (desc) return "�p�҂̘r�͂��㏸������B";
#else
		if (name) return "Extra might";
		if (desc) return "Attempts to increase your strength.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("�������͂��N���ė���B");
#else
			msg_print("You feel you get stronger.");
#endif
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�������";
		if (desc) return "�������Ă��镐����􂤁B";
#else
		if (name) return "Curse weapon";
		if (desc) return "Curses your weapon.";
#endif
		if (cast)
		{
			int item;
			char *q, *s;
			char o_name[MAX_NLEN];
			object_type *o_ptr;
			u32b f[TR_FLAG_SIZE];

			item_tester_hook = item_tester_hook_weapon_except_bow;
#ifdef JP
			q = "�ǂ���􂢂܂����H";
			s = "����𑕔����Ă��Ȃ��B";
#else
			q = "Which weapon do you curse?";
			s = "You wield no weapons.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP))) return FALSE;

			o_ptr = &inventory[item];
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
			object_flags(o_ptr, f);

#ifdef JP
			if (!get_check(format("�{���� %s ���􂢂܂����H", o_name))) return FALSE;
#else
			if (!get_check(format("Do you curse %s, really�H", o_name))) return FALSE;
#endif

			if (!one_in_(3) &&
				(object_is_artifact(o_ptr) || have_flag(f, TR_BLESSED)))
			{
#ifdef JP
				msg_format("%s �͎􂢂𒵂˕Ԃ����B", o_name);
#else
				msg_format("%s resists the effect.", o_name);
#endif
				if (one_in_(3))
				{
					if (o_ptr->to_d > 0)
					{
						o_ptr->to_d -= randint1(3) % 2;
						if (o_ptr->to_d < 0) o_ptr->to_d = 0;
					}
					if (o_ptr->to_h > 0)
					{
						o_ptr->to_h -= randint1(3) % 2;
						if (o_ptr->to_h < 0) o_ptr->to_h = 0;
					}
					if (o_ptr->to_a > 0)
					{
						o_ptr->to_a -= randint1(3) % 2;
						if (o_ptr->to_a < 0) o_ptr->to_a = 0;
					}
#ifdef JP
					msg_format("%s �͗򉻂��Ă��܂����B", o_name);
#else
					msg_format("Your %s was disenchanted!", o_name);
#endif
				}
			}
			else
			{
				int power = 0;
#ifdef JP
				msg_format("���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I", o_name);
#else
				msg_format("A terrible black aura blasts your %s!", o_name);
#endif
				o_ptr->curse_flags |= (TRC_CURSED);

				if (object_is_artifact(o_ptr) || object_is_ego(o_ptr))
				{

					if (one_in_(3)) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
					if (one_in_(666))
					{
						o_ptr->curse_flags |= (TRC_TY_CURSE);
						if (one_in_(666)) o_ptr->curse_flags |= (TRC_PERMA_CURSE);

						add_flag(o_ptr->art_flags, TR_AGGRAVATE);
						add_flag(o_ptr->art_flags, TR_VORPAL);
						add_flag(o_ptr->art_flags, TR_VAMPIRIC);
#ifdef JP
						msg_print("�����I�����I�����I");
#else
						msg_print("Blood, Blood, Blood!");
#endif
						power = 2;
					}
				}

				o_ptr->curse_flags |= get_curse(power, o_ptr);
			}

			p_ptr->update |= (PU_BONUS);
			add = FALSE;
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�׈����m";
		if (desc) return "���͂̎׈��ȃ����X�^�[�����m����B";
#else
		if (name) return "Evil detection";
		if (desc) return "Detects evil monsters.";
#endif
		if (info) return info_range(MAX_SIGHT);
		if (cast)
		{
#ifdef JP
			msg_print("�׈��Ȑ����̑��݂�������낤�Ƃ����B");
#else
			msg_print("You attend to the presence of evil creatures.");
#endif
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�䖝";
		if (desc) return "���^�[���U����ς�����A�󂯂��_���[�W��n���̋Ɖ΂Ƃ��Ď��͂ɕ��o����B";
#else
		if (name) return "Patience";
		if (desc) return "Bursts hell fire strongly after patients any damage while few turns.";
#endif
		power = MIN(200, (p_ptr->magic_num1[2] * 2));
		if (info) return info_damage(0, 0, power);
		if (cast)
		{
			int a = 3 - (p_ptr->pspeed - 100) / 10;
			int r = 3 + randint1(3) + MAX(0, MIN(3, a));

			if (p_ptr->magic_num2[2] > 0)
			{
#ifdef JP
				msg_print("���łɉ䖝�����Ă���B");
#else
				msg_print("You are already patienting.");
#endif
				return NULL;
			}

			p_ptr->magic_num2[1] = 1;
			p_ptr->magic_num2[2] = r;
			p_ptr->magic_num1[2] = 0;
#ifdef JP
			msg_print("�����Ƒς��邱�Ƃɂ����B");
#else
			msg_print("You decide to patient all damages.");
#endif
			add = FALSE;
		}
		if (cont)
		{
			int rad = 2 + (power / 50);

			p_ptr->magic_num2[2]--;

			if ((p_ptr->magic_num2[2] <= 0) || (power >= 200))
			{
#ifdef JP
				msg_print("�䖝�������ꂽ�I");
#else
				msg_print("Time for end of patioence!");
#endif
				if (power)
				{
					project(0, rad, py, px, power, GF_HELL_FIRE,
						(PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
				}
				if (p_ptr->wizard)
				{
#ifdef JP
					msg_format("%d�_�̃_���[�W��Ԃ����B", power);
#else
					msg_format("You return %d damages.", power);
#endif
				}

				/* Reset */
				p_ptr->magic_num2[1] = 0;
				p_ptr->magic_num2[2] = 0;
				p_ptr->magic_num1[2] = 0;
			}
		}
		break;

	/*** 2nd book (8-15) ***/
	case 8:
#ifdef JP
		if (name) return "�X�̊Z";
		if (desc) return "�X�̃I�[����g�ɂ܂Ƃ��A�h��͂��㏸����B";
#else
		if (name) return "Ice armor";
		if (desc) return "Gives fire aura and bonus to AC.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("�̂��X�̊Z�ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by ice armor!");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("�X�̊Z�������������B");
#else
			msg_print("Ice armor disappeared.");
#endif
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�d���̎���";
		if (desc) return "�̗͂⏝�𑽏��񕜂�����B";
#else
		if (name) return "Cure serious wounds";
		if (desc) return "Heals cut and HP more.";
#endif
		if (info) return info_heal(2, 10, 0);
		if (cast)
		{
#ifdef JP
			msg_print("�C�����ǂ��Ȃ��Ă���B");
#else
			msg_print("You feel better and better.");
#endif
		}
		if (cast || cont)
		{
			hp_player(damroll(2, 10));
			set_cut((p_ptr->cut / 2) - 10);
		}
		break;

	case 10:
#ifdef JP
		if (name) return "��i�z��";
		if (desc) return "�����r���𒆎~���邱�ƂȂ��A��̌��ʂ𓾂邱�Ƃ��ł���B";
#else
		if (name) return "Inhail potion";
		if (desc) return "Quaffs a potion without canceling of casting a spell.";
#endif
		if (cast)
		{
			p_ptr->magic_num1[0] |= (1L << HEX_INHAIL);
			do_cmd_quaff_potion();
			p_ptr->magic_num1[0] &= ~(1L << HEX_INHAIL);
			add = FALSE;
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�z����";
		if (desc) return "���E���̃����X�^�[�ɔ���ʂ̐����͋z���̃_���[�W��^����B�^�����_���[�W�̕��A�̗͂��񕜂���B";
#else
		if (name) return "Vampiric mist";
		if (desc) return "Deals few dameges of drain life to all monsters in your sight.";
#endif
		power = (plev / 2) + 5;
		if (info) return info_damage(1, power, 0);
		if (cast || cont)
		{
			project_hack(GF_OLD_DRAIN, randint1(power));
		}
		break;

	case 12:
#ifdef JP
		if (name) return "������";
		if (desc) return "����̍U���͂��グ��B�؂ꖡ�𓾁A�􂢂ɉ����ė^����_���[�W���㏸���A�P�ǂȃ����X�^�[�ɑ΂���_���[�W��2�{�ɂȂ�B";
#else
		if (name) return "Swords to runeswords";
		if (desc) return "Gives vorpal ability to your weapon. Increases damages by your weapon acccording to curse of your weapon.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("���Ȃ��̕��킪�����P�����B");
#else
			if (!empty_hands(FALSE))
				msg_print("Your weapons glow bright black.");
			else
				msg_print("Your weapon glows bright black.");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("����̋P���������������B");
#else
			msg_format("Brightness of weapon%s disappeared.", (empty_hands(FALSE)) ? "" : "s");
#endif
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�����̎�";
		if (desc) return "�U�������ۃ����X�^�[������������B";
#else
		if (name) return "Touch of confusion";
		if (desc) return "Confuses a monster when you attack.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("���Ȃ��̎肪�Ԃ��P���n�߂��B");
#else
			msg_print("Your hands glow bright red.");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("��̋P�����Ȃ��Ȃ����B");
#else
			msg_print("Brightness on your hands disappeard.");
#endif
		}
		break;

	case 14:
#ifdef JP
		if (name) return "���̋���";
		if (desc) return "�p�҂̘r�́A��p���A�ϋv�͂��㏸������B�U���񐔂̏���� 1 ����������B";
#else
		if (name) return "Building up";
		if (desc) return "Attempts to increases your strength, dexterity and constitusion.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("�g�̂������Ȃ����C�������B");
#else
			msg_print("You feel your body is developed more now.");
#endif
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���e���|�[�g���E";
		if (desc) return "���E���̃����X�^�[�̃e���|�[�g��j�Q����o���A�𒣂�B";
#else
		if (name) return "Anti teleport barrier";
		if (desc) return "Obstructs all teleportations by monsters in your sight.";
#endif
		power = plev * 3 / 2;
		if (info) return info_power(power);
		if (cast)
		{
#ifdef JP
			msg_print("�e���|�[�g��h���􂢂��������B");
#else
			msg_print("You feel anyone can not teleport except you.");
#endif
		}
		break;

	/*** 3rd book (16-23) ***/
	case 16:
#ifdef JP
		if (name) return "�Ռ��̃N���[�N";
		if (desc) return "�d�C�̃I�[����g�ɂ܂Ƃ��A�����������Ȃ�B";
#else
		if (name) return "Cloak of shock";
		if (desc) return "Gives lightning aura and a bonus to speed.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("�̂���Ȃ̃I�[���ŕ���ꂽ�B");
#else
			msg_print("You have enveloped by electrical aura!");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("��Ȃ̃I�[���������������B");
#else
			msg_print("Electrical aura disappeared.");
#endif
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�v�����̎���";
		if (desc) return "�̗͂⏝���񕜂�����B";
#else
		if (name) return "Cure critical wounds";
		if (desc) return "Heals cut and HP greatry.";
#endif
		if (info) return info_heal(4, 10, 0);
		if (cast)
		{
#ifdef JP
			msg_print("�C�����ǂ��Ȃ��Ă���B");
#else
			msg_print("You feel better and better.");
#endif
		}
		if (cast || cont)
		{
			hp_player(damroll(4, 10));
			set_stun(0);
			set_cut(0);
			set_poisoned(0);
		}
		break;

	case 18:
#ifdef JP
		if (name) return "���͕���";
		if (desc) return "���@�̓���ɖ��͂��ď[�U����B";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges a magic device.";
#endif
		power = plev * 2;
		if (info) return info_power(power);
		if (cast)
		{
			if (!recharge(power)) return NULL;
			add = FALSE;
		}
		break;

	case 19:
#ifdef JP
		if (name) return "���ҕ���";
		if (desc) return "���̂�h�点�ăy�b�g�ɂ���B";
#else
		if (name) return "Animate Dead";
		if (desc) return "Raises corpses and skeletons from dead.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("���҂ւ̌Ăт������n�߂��B");
#else
			msg_print("You start to call deads.!");
#endif
		}
		if (cast || cont)
		{
			animate_dead(0, py, px);
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�h�����";
		if (desc) return "�������Ă���h��Ɏ􂢂�������B";
#else
		if (name) return "Curse armor";
		if (desc) return "Curse a piece of armour that you wielding.";
#endif
		if (cast)
		{
			int item;
			char *q, *s;
			char o_name[MAX_NLEN];
			object_type *o_ptr;
			u32b f[TR_FLAG_SIZE];

			item_tester_hook = object_is_armour;
#ifdef JP
			q = "�ǂ���􂢂܂����H";
			s = "�h��𑕔����Ă��Ȃ��B";
#else
			q = "Which piece of armour do you curse?";
			s = "You wield no piece of armours.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP))) return FALSE;

			o_ptr = &inventory[item];
			object_desc(o_name, o_ptr, OD_NAME_ONLY);
			object_flags(o_ptr, f);

#ifdef JP
			if (!get_check(format("�{���� %s ���􂢂܂����H", o_name))) return FALSE;
#else
			if (!get_check(format("Do you curse %s, really�H", o_name))) return FALSE;
#endif

			if (!one_in_(3) &&
				(object_is_artifact(o_ptr) || have_flag(f, TR_BLESSED)))
			{
#ifdef JP
				msg_format("%s �͎􂢂𒵂˕Ԃ����B", o_name);
#else
				msg_format("%s resists the effect.", o_name);
#endif
				if (one_in_(3))
				{
					if (o_ptr->to_d > 0)
					{
						o_ptr->to_d -= randint1(3) % 2;
						if (o_ptr->to_d < 0) o_ptr->to_d = 0;
					}
					if (o_ptr->to_h > 0)
					{
						o_ptr->to_h -= randint1(3) % 2;
						if (o_ptr->to_h < 0) o_ptr->to_h = 0;
					}
					if (o_ptr->to_a > 0)
					{
						o_ptr->to_a -= randint1(3) % 2;
						if (o_ptr->to_a < 0) o_ptr->to_a = 0;
					}
#ifdef JP
					msg_format("%s �͗򉻂��Ă��܂����B", o_name);
#else
					msg_format("Your %s was disenchanted!", o_name);
#endif
				}
			}
			else
			{
				int power = 0;
#ifdef JP
				msg_format("���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I", o_name);
#else
				msg_format("A terrible black aura blasts your %s!", o_name);
#endif
				o_ptr->curse_flags |= (TRC_CURSED);

				if (object_is_artifact(o_ptr) || object_is_ego(o_ptr))
				{

					if (one_in_(3)) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
					if (one_in_(666))
					{
						o_ptr->curse_flags |= (TRC_TY_CURSE);
						if (one_in_(666)) o_ptr->curse_flags |= (TRC_PERMA_CURSE);

						add_flag(o_ptr->art_flags, TR_AGGRAVATE);
						add_flag(o_ptr->art_flags, TR_RES_POIS);
						add_flag(o_ptr->art_flags, TR_RES_DARK);
						add_flag(o_ptr->art_flags, TR_RES_NETHER);
#ifdef JP
						msg_print("�����I�����I�����I");
#else
						msg_print("Blood, Blood, Blood!");
#endif
						power = 2;
					}
				}

				o_ptr->curse_flags |= get_curse(power, o_ptr);
			}

			p_ptr->update |= (PU_BONUS);
			add = FALSE;
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�e�̃N���[�N";
		if (desc) return "�e�̃I�[����g�ɂ܂Ƃ��A�G�ɉe�̃_���[�W��^����B";
#else
		if (name) return "Cloak of shadow";
		if (desc) return "Gives aura of shadow.";
#endif
		if (cast)
		{
			object_type *o_ptr = &inventory[INVEN_OUTER];

			if (!o_ptr->k_idx)
			{
#ifdef JP
				msg_print("�N���[�N��g�ɂ��Ă��Ȃ��I");
#else
				msg_print("You don't ware any cloak.");
#endif
				return NULL;
			}
			else if (!object_is_cursed(o_ptr))
			{
#ifdef JP
				msg_print("�N���[�N�͎���Ă��Ȃ��I");
#else
				msg_print("Your cloak is not cursed.");
#endif
				return NULL;
			}
			else
			{
#ifdef JP
				msg_print("�e�̃I�[����g�ɂ܂Ƃ����B");
#else
				msg_print("You have enveloped by shadow aura!");
#endif
			}
		}
		if (cont)
		{
			object_type *o_ptr = &inventory[INVEN_OUTER];

			if ((!o_ptr->k_idx) || (!object_is_cursed(o_ptr)))
			{
				do_spell(REALM_HEX, spell, SPELL_STOP);
				p_ptr->magic_num1[0] &= ~(1L << spell);
				p_ptr->magic_num2[0]--;
				if (!p_ptr->magic_num2[0]) set_action(ACTION_NONE);
			}
		}
		if (stop)
		{
#ifdef JP
			msg_print("�e�̃I�[���������������B");
#else
			msg_print("Shadow aura disappeared.");
#endif
		}
		break;

	case 22:
#ifdef JP
		if (name) return "��ɂ𖂗͂�";
		if (desc) return "���E���̃����X�^�[�ɐ��_�_���[�W�^���A���͂��z�����B";
#else
		if (name) return "Pains to mana";
		if (desc) return "Deals psychic damages to all monsters in sight, and drains some mana.";
#endif
		power = plev * 3 / 2;
		if (info) return info_damage(1, power, 0);
		if (cast || cont)
		{
			project_hack(GF_PSI_DRAIN, randint1(power));
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�ڂɂ͖ڂ�";
		if (desc) return "�Ō��▂�@�Ŏ󂯂��_���[�W���A�U�����̃����X�^�[�ɂ��^����B";
#else
		if (name) return "Eye for an eye";
		if (desc) return "Returns same damage which you got to the monster which damaged you.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("���Q�������~�]�ɂ���ꂽ�B");
#else
			msg_print("You wish strongly you want to revenge anything.");
#endif
		}
		break;

	/*** 4th book (24-31) ***/
	case 24:
#ifdef JP
		if (name) return "�����B���E";
		if (desc) return "���̊K�̑��B���郂���X�^�[�̑��B��j�~����B";
#else
		if (name) return "Anti multiply barrier";
		if (desc) return "Obstructs all multiplying by monsters in entire floor.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("���B��j�~����􂢂��������B");
#else
			msg_print("You feel anyone can not already multiply.");
#endif
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�����͕���";
		if (desc) return "�o���l�����X�ɕ������A���������\�͒l���񕜂�����B";
#else
		if (name) return "Restore life";
		if (desc) return "Restores life energy and status.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("�����͂��߂�n�߂��B");
#else
			msg_print("You feel your life energy starting to return.");
#endif
		}
		if (cast || cont)
		{
			bool flag = FALSE;
			int d = (p_ptr->max_exp - p_ptr->exp);
			int r = (p_ptr->exp / 20);
			int i;

			if (d > 0)
			{
				if (d < r)
					p_ptr->exp = p_ptr->max_exp;
				else
					p_ptr->exp += r;

				/* Check the experience */
				check_experience();

				flag = TRUE;
			}
///mod140104 �p�����[�^����ƕ\�L��ύX ��p�͂ǂ��������\��Ȃ̂Ńp�����[�^�����������ۂ��ƍ폜����
#if 0
			for (i = A_STR; i < 6; i ++)
			{
				if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
				{
					if (p_ptr->stat_cur[i] < 18)
						p_ptr->stat_cur[i]++;
					else
						p_ptr->stat_cur[i] += 10;

					if (p_ptr->stat_cur[i] > p_ptr->stat_max[i])
						p_ptr->stat_cur[i] = p_ptr->stat_max[i];

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					flag = TRUE;
				}
			}
#endif
			if (!flag)
			{
#ifdef JP
				msg_format("%s�̎����̉r������߂��B", do_spell(REALM_HEX, HEX_RESTORE, SPELL_NAME));
#else
				msg_format("Finish casting '%^s'.", do_spell(REALM_HEX, HEX_RESTORE, SPELL_NAME));
#endif
				p_ptr->magic_num1[0] &= ~(1L << HEX_RESTORE);
				if (cont) p_ptr->magic_num2[0]--;
				if (p_ptr->magic_num2) p_ptr->action = ACTION_NONE;

				/* Redraw status */
				p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);
				p_ptr->redraw |= (PR_EXTRA);

				return "";
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "���͋z��";
		if (desc) return "���ꂽ����̎􂢂��z�����Ė��͂��񕜂���B";
#else
		if (name) return "Drain curse power";
		if (desc) return "Drains curse on your weapon and heals SP a little.";
#endif
		if (cast)
		{
			int item;
			char *s, *q;
			u32b f[TR_FLAG_SIZE];
			object_type *o_ptr;

			item_tester_hook = item_tester_hook_cursed;
#ifdef JP
			q = "�ǂ̑����i����z�����܂����H";
			s = "���ꂽ�A�C�e���𑕔����Ă��Ȃ��B";
#else
			q = "Which cursed equipment do you drain mana from?";
			s = "You have no cursed equipment.";
#endif

			if (!get_item(&item, q, s, (USE_EQUIP))) return FALSE;

			o_ptr = &inventory[item];
			object_flags(o_ptr, f);

			p_ptr->csp += (p_ptr->lev / 5) + randint1(p_ptr->lev / 5);
			if (have_flag(f, TR_TY_CURSE) || (o_ptr->curse_flags & TRC_TY_CURSE)) p_ptr->csp += randint1(5);
			if (p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;

			if (o_ptr->curse_flags & TRC_PERMA_CURSE)
			{
				/* Nothing */
			}
			else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				if (one_in_(7))
				{
#ifdef JP
					msg_print("�􂢂�S�ċz��������B");
#else
					msg_print("Heavy curse vanished away.");
#endif
					o_ptr->curse_flags = 0L;
				}
			}
			else if ((o_ptr->curse_flags & (TRC_CURSED)) && one_in_(3))
			{
#ifdef JP
				msg_print("�􂢂�S�ċz��������B");
#else
				msg_print("Curse vanished away.");
#endif
				o_ptr->curse_flags = 0L;
			}

			add = FALSE;
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�z���̐n";
		if (desc) return "�z�������ōU������B";
#else
		if (name) return "Swords to vampires";
		if (desc) return "Gives vampiric ability to your weapon.";
#endif
		if (cast)
		{
#ifdef JP
			msg_print("���Ȃ��̕��킪����~���Ă���B");
#else
			if (!empty_hands(FALSE))
				msg_print("Your weapons want more blood now.");
			else
				msg_print("Your weapon wants more blood now.");
#endif
		}
		if (stop)
		{
#ifdef JP
			msg_print("����̊��]�������������B");
#else
			msg_format("Thirsty of weapon%s disappeared.", (empty_hands(FALSE)) ? "" : "s");
#endif
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�N�O�̌��t";
		if (desc) return "���E���̃����X�^�[��N�O�Ƃ�����B";
#else
		if (name) return "Word of stun";
		if (desc) return "Stuns all monsters in your sight.";
#endif
		power = plev * 4;
		if (info) return info_power(power);
		if (cast || cont)
		{
			stun_monsters(power);
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�e�ړ�";
		if (desc) return "�����X�^�[�ׂ̗̃}�X�ɏu�Ԉړ�����B";
#else
		if (name) return "Moving into shadow";
		if (desc) return "Teleports you close to a monster.";
#endif
		if (cast)
		{
			int i, y, x, dir;
			bool flag;

			for (i = 0; i < 3; i++)
			{
				if (!tgt_pt(&x, &y)) return FALSE;

				flag = FALSE;

				for (dir = 0; dir < 8; dir++)
				{
					int dy = y + ddy_ddd[dir];
					int dx = x + ddx_ddd[dir];
					if (dir == 5) continue;
					if(cave[dy][dx].m_idx) flag = TRUE;
				}

				if (!cave_empty_bold(y, x) || (cave[y][x].info & CAVE_ICKY) ||
					(distance(y, x, py, px) > plev + 2))
				{
#ifdef JP
					msg_print("�����ɂ͈ړ��ł��Ȃ��B");
#else
					msg_print("Can not teleport to there.");
#endif
					continue;
				}
				break;
			}

			if (flag && randint0(plev * plev / 2))
			{
				teleport_player_to(y, x, 0L);
			}
			else
			{
#ifdef JP
				msg_print("�����ƁI");
#else
				msg_print("Oops!");
#endif
				teleport_player(30, 0L);
			}

			add = FALSE;
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�����@���E";
		if (desc) return "���E���̃����X�^�[�̖��@��j�Q����o���A�𒣂�B";
#else
		if (name) return "Anti magic barrier";
		if (desc) return "Obstructs all magic spell of monsters in your sight.";
#endif
		power = plev * 3 / 2;
		if (info) return info_power(power);
		if (cast)
		{
#ifdef JP
			msg_print("���@��h���􂢂��������B");
#else
			msg_print("You feel anyone can not cast spells except you.");
#endif
		}
		break;

	case 31:
#ifdef JP
		if (name) return "���Q�̐鍐";
		if (desc) return "���^�[����ɂ���܂Ŏ󂯂��_���[�W�ɉ������З͂̒n���̍��΂̒e����B";
#else
		if (name) return "Revenge sentence";
		if (desc) return "Fires  a ball of hell fire to try revenging after few turns.";
#endif
		power = p_ptr->magic_num1[2];
		if (info) return info_damage(0, 0, power);
		if (cast)
		{
			int r;
			int a = 3 - (p_ptr->pspeed - 100) / 10;
			r = 1 + randint1(2) + MAX(0, MIN(3, a));

			if (p_ptr->magic_num2[2] > 0)
			{
#ifdef JP
				msg_print("���łɕ��Q�͐鍐�ς݂��B");
#else
				msg_print("You already pronounced your revenge.");
#endif
				return NULL;
			}

			p_ptr->magic_num2[1] = 2;
			p_ptr->magic_num2[2] = r;
#ifdef JP
			msg_format("���Ȃ��͕��Q��鍐�����B���� %d �^�[���B", r);
#else
			msg_format("You pronounce your revenge. %d turns left.", r);
#endif
			add = FALSE;
		}
		if (cont)
		{
			p_ptr->magic_num2[2]--;

			if (p_ptr->magic_num2[2] <= 0)
			{
				int dir;

				if (power)
				{
					command_dir = 0;

					do
					{
#ifdef JP
						msg_print("���Q�̎����I");
#else
						msg_print("Time to revenge!");
#endif
					}
					while (!get_aim_dir(&dir));

					fire_ball(GF_HELL_FIRE, dir, power, 1);

					if (p_ptr->wizard)
					{
#ifdef JP
						msg_format("%d�_�̃_���[�W��Ԃ����B", power);
#else
						msg_format("You return %d damages.", power);
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("���Q����C���������B");
#else
					msg_print("You are not a mood to revenge.");
#endif
				}
				p_ptr->magic_num1[2] = 0;
			}
		}
		break;
	}

	/* start casting */
	if ((cast) && (add))
	{
		/* add spell */
		p_ptr->magic_num1[0] |= 1L << (spell);
		p_ptr->magic_num2[0]++;

		if (p_ptr->action != ACTION_SPELL) set_action(ACTION_SPELL);
	}

	/* Redraw status */
	if (!info)
	{
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);
		p_ptr->redraw |= (PR_EXTRA | PR_HP | PR_MANA);
	}

	return "";
}
#endif


/*:::�u�Q���Z�~�E���E�G���K���X�v��I�����邽�߂�hook*/
static bool item_tester_hook_gelsemium(object_type *o_ptr)
{
	if (o_ptr->tval != TV_MATERIAL) return (FALSE);
	if (o_ptr->sval != SV_MATERIAL_GELSEMIUM) return (FALSE);
	return (TRUE);
}
/*:::����̍ޗ���I�����邽�߂�hook �����ȊO��SOFTDRINK*/
static bool item_tester_hook_make_reisyu(object_type *o_ptr)
{
	if (o_ptr->tval != TV_SOFTDRINK) return (FALSE);
	if (o_ptr->sval == SV_DRINK_SALT_WATER) return (FALSE);
	return (TRUE);
}

/*:::�u����z�΁v��I�����邽�߂�hook*/
static bool item_tester_hook_mercury(object_type *o_ptr)
{
	if (o_ptr->tval != TV_MATERIAL) return (FALSE);
	if (o_ptr->sval != SV_MATERIAL_MERCURY) return (FALSE);
	return (TRUE);
}


static cptr do_new_spell_element(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
	static const char s_random[] = "�����_��";
#endif

	int dir;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�}�W�b�N�E�~�T�C��";
		if (desc) return "�ア���@�̖����B";
#endif
    
		{
			int dice = 3 + ((plev - 1) / 6);
			int sides = 3;
			if (info) return info_damage(dice, sides, 0);
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�M��";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#endif
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if(rad > 8) rad = 8;
			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�T���_�[�E�{���g";
		if (desc) return "�d���̃{���g�������̓r�[������B";
#endif   
		{

			int dice = 7 + (plev - 5) / 4;
			int sides = 5;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;
	case 3:
#ifdef JP
		if (name) return "�A�C�X�E�{���g";
		if (desc) return "��C�̃{���g�������̓r�[������B";
#endif   
		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 6;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_COLD, dir, damroll(dice, sides));
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�A�V�b�h�E�{���g";
		if (desc) return "�_�̃{���g�������̓r�[������B";
#endif   
		{
			int dice = 9 + (plev - 5) / 4;
			int sides = 6;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_ACID, dir, damroll(dice, sides));
			}
		}
		break;
	case 5:
#ifdef JP
		if (name) return "�t�@�C�A�E�{���g";
		if (desc) return "���̃{���g�������̓r�[������B";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 10 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���V";
		if (desc) return "��莞�ԕ��V��ԂɂȂ�B";
#endif
    
		{
			int base = plev;
			int sides = 20 + plev;
			if (info) return info_duration(base, sides);
			if (cast)
			{
				msg_print("���Ȃ��̎���ɕ����Q�������B");
				set_tim_levitation(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���x�Ւf";
		if (desc) return "��莞�ԁA�Ή��Ɨ�C�ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#endif
    
		{
			int base = 20;
			if (info) return info_duration(base, base);
			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�ϓd��";
		if (desc) return "��莞�ԁA�d���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 9:
#ifdef JP		
		if (name) return "�ώ_";
		if (desc) return "��莞�ԁA�_�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�E�B���h�J�b�^�[";
		if (desc) return "���̐n��A���Ŕ�΂��čU������B���x�����オ��Ɛn�̐���������BAC�̍����G�ɂ͌��ʂ������B";
#endif
    
		{
			int count = 3 + plev / 10;
			int dice = 2 + plev / 5;
			int sides = 4;
			int i;

			if(cp_ptr->magicmaster) count += plev / 20;
			if(count<1) count = 1; //paranoia

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				for (i = 0; i < count; i++)
				{
					fire_bolt(GF_WINDCUTTER, dir, damroll(dice, sides));
				}
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�g���l������";
		if (desc) return "�ǂ�n�����Z�˒��̃r�[������B";
#endif
    
		{
			int dice = 1;
			int sides = 1;
			int range = plev / 10 + 2;
	
	
			if (info) return format("�˒��F%d", range);

			if (cast)
			{
				int flg = PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_DISI;
				project_length = range;
				if (!get_aim_dir(&dir)) return NULL;
				project_hook(GF_KILL_WALL, dir, damroll(dice,sides), flg);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�T���_�[�E�{�[��";
		if (desc) return "���̋�����B";
#endif
    
		{
			int dam = plev * 2 + 50;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_ELEC, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�A�C�X�E�{�[��";
		if (desc) return "��C�̋�����B";
#endif
    
		{
			int dam = plev * 2 + 70;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_COLD, dir, dam, rad);
			}
		}
		break;
	case 14:
#ifdef JP
		if (name) return "�A�V�b�h�E�{�[��";
		if (desc) return "�_�̋�����B";
#endif
    
		{
			int dam = plev * 2 + 90;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_ACID, dir, dam, rad);
			}
		}
		break;	
	case 15:
#ifdef JP
		if (name) return "�t�@�C�A�E�{�[��";
		if (desc) return "���̋�����B";
#endif
    
		{
			int dam = plev * 3 + 120;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;
	case 16:
#ifdef JP
		if (name) return "���̊Z";
		if (desc) return "��莞�ԁAAC�㏸�Ɛ��ϐ��𓾂�B";
#endif
    
		{
			int base = 20;
			int sides = 30;
			int tmp;

			if (info) return info_duration(base, base);

			if (cast)
			{
				tmp = randint1(sides);
				msg_print("�����Q�����Ă��Ȃ����񂾁B");
				set_shield(tmp + base, FALSE);
				set_tim_res_water(tmp + base, FALSE);
			}
		}
		break;
	case 17:
#ifdef JP
		if (name) return "�[��������";
		if (desc) return "�אڂ����n�ʂɑ匊���J����B���V���Ă��Ȃ��G������Ɨ����ă_���[�W���󂯂�B�A�C�e���Ȃǂ̂���n�ʂɂ͎g���Ȃ��B";
#else
		if (name) return "Chain Lightning";
		if (desc) return "Fires lightning beams in all directions.";
#endif
    
		{
			int dice = 1;
			int sides = 50 + plev * 2;
			int base = 50 + plev * 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int y, x;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];

				//�����Ȃ����ɂ����J�����Ȃ��B�ł��g���b�v���ɂ͊J������
				if(!in_bounds(y,x) || (!cave_clean_bold(y, x) && !is_trap(cave[y][x].feat))|| cave_have_flag_bold(y, x, FF_SHALLOW))
				{
					msg_print("�����ɂ͌����J�����Ȃ��B");

					return NULL;
				}
				cave_set_feat(y, x, feat_dark_pit);

				//v1.1.24 FLOW�A�b�v�f�[�g�ǉ�
				p_ptr->update |= (PU_FLOW);
				msg_print("�n�ʂɑ匊���J�����I");
				/*:::㩂�����Ώ����B���Ɨn��ȊO�̃A�C�e����u����n�`�Ȃ�[�����ɂ���B���̂Ƃ������Ă��Ȃ������X�^�[��������_���[�W��^����*/
				if (cave[y][x].m_idx)
				{
					int dam = damroll(dice,sides) + base;
					int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_KILL | PROJECT_HIDE;
					monster_type *m_ptr = &m_list[cave[y][x].m_idx];
					char m_name[80];
	
					monster_desc(m_name, m_ptr, 0);

					if(r_info[m_ptr->r_idx].flags7 & RF7_CAN_FLY)
					{
						if (m_ptr->ml) msg_format("%s�͗I�X�ƌ��̏����ł���E�E",m_name);
					}
					else
					{
						if(r_info[m_ptr->r_idx].flags2 & RF2_GIGANTIC)
						{
							if (m_ptr->ml) msg_format("%s�̋��̂��n�����𗧂ĂČ��ɗ������I",m_name);
							dam *= 2;
						}
						else 
						{
							if (m_ptr->ml) msg_format("%s�����ɗ������I",m_name);
						}
						project(0, 0, y, x, dam, GF_MISSILE, flg, -1);
						if(!(r_info[m_ptr->r_idx].flags3 & RF3_NO_STUN))
						{
							if (m_ptr->ml) msg_format("%s�͂����낤�Ƃ����B",m_name);
							(void)set_monster_stunned(cave[y][x].m_idx, MON_STUNNED(m_ptr) + dam / 20);
						}
					}
				}
				p_ptr->redraw |= (PR_MAP);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�G�������^������";
		if (desc) return "1�̂̃G�������^������������B";
#else
		if (name) return "Conjure Elemental";
		if (desc) return "Summons an elemental.";
#endif
    
		{
			if (cast)
			{
				if (!summon_specific(-1, py, px, plev, SUMMON_ELEMENTAL, (PM_ALLOW_GROUP | PM_FORCE_PET)))
				{
#ifdef JP
					msg_print("�G�������^���͌���Ȃ������B");
#else
					msg_print("No Elementals arrive.");
#endif
				}
			}
		}
		break;
	case 19:
#ifdef JP
		if (name) return "���f�ϐ�";
		if (desc) return "��莞�ԁA�_�A�d���A���A��C�ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#endif
    
		{
			int base = plev;
			int dice = 20;

			if (info) return info_duration(base, dice);

			if (cast)
			{
			int time = randint1(dice) + base;
				set_oppose_acid(time, FALSE);
				set_oppose_elec(time, FALSE);
				set_oppose_fire(time, FALSE);
				set_oppose_cold(time, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�X�̑�";
		if (desc) return "���͂ȗ�C�̃r�[������B";
#endif
    
		{
			int dice = plev / 3;
			int sides = 12;
			int base = 120 + plev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_beam(GF_COLD, dir, damroll(dice, sides) + base);
			}
		}
		break;
	case 21:
#ifdef JP
		if (name) return "�_�̉J";
		if (desc) return "���E���S�Ăɑ΂��Ď_�ōU������B";
#else
		if (name) return "Magic Rocket";
		if (desc) return "Fires a magic rocket.";
#endif
    
		{
			int dam = 80 + plev * 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				msg_print("�����������_�ɕ���ꂽ�I");
				project_hack2(GF_ACID, 0,0,dam);

			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "���̗�";
		if (desc) return "����ȉ��̋�����B";
#endif
    
		{
			int dam = plev * 6 + 100;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}

		break;

	case 23:
#ifdef JP
		if (name) return "���̌�";
		if (desc) return "�אڂ����G��̂ɐG��A���͂ȓd������B";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif
    
		{
			int dice = 1;
			int sides = 120 + plev * 4;
			int base = 120 + plev * 4;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int y, x;
				monster_type *m_ptr;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];
				m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int dam = damroll(dice,sides) + base;
					int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
					char m_name[80];	
					monster_desc(m_name, m_ptr, 0);
					msg_format("���Ȃ���%s�ɐG��A����ȓd����H��킹���I",m_name);
					project(0, 0, y, x, dam, GF_ELEC, flg, -1);
					touch_zap_player(m_ptr);
				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "���͏���";
		if (desc) return "�����X�^�[��̂̉����A�����A���G������������B";
#else
		if (name) return "Beam of Gravity";
		if (desc) return "Fires a beam of gravity.";
#endif
    
		{

			if (info) return "�����F1��";

			if (cast)
			{

				int m_idx;

				if (!target_set(TARGET_KILL)) return NULL;
				m_idx = cave[target_row][target_col].m_idx;
				if (!m_idx) break;
				if (!player_has_los_bold(target_row, target_col)) break;
				if (!projectable(py, px, target_row, target_col)) break;
				dispel_monster_status(m_idx);
				break;
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "���q����";
		if (desc) return "����ȕ����̋�����B";
#else
		if (name) return "Disintegrate";
		if (desc) return "Fires a huge ball of disintegration.";
#endif
    
		{
			int dam = plev * 2 + 50;
			int rad = 3 + plev / 40;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir, dam, rad);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�Εǐ���";
		if (desc) return "�����̎��͂ɉԛ���̕ǂ����B";
#else
		if (name) return "Wall of Stone";
		if (desc) return "Creates granite walls in all adjacent squares.";
#endif
    
		{
			if (cast)
			{
				wall_stone();
			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "��Η�x";
		if (desc) return "���E���S�Ăɑ΂��ċɊ��̍U�����s���B";
#endif
    
		{

			int dam = plev * 5 + 30;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				msg_print("��u�A�����̑S�Ă̕������������~�����E�E");
				project_hack2(GF_ICE, 0,0,dam);
			}

		}
		break;

	case 28:
#ifdef JP
		if (name) return "�A�g�~�b�N���C";
		if (desc) return "���ɋ��͂ȉΉ��̃r�[������B";
#endif
    
		{
			int dam = plev * 20;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				msg_print("���Ȃ��͒������̔M����������I");
				fire_spark(GF_FIRE, dir, dam ,2);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�G���N�T�[";
		if (desc) return "���\�̗��𐸐����ĕ��p����B�̗͂ƃo�b�h�X�e�[�^�X���񕜂���B�G�}�Ƃ��āu����z�΁v���K�v�ɂȂ�B";
#endif
    
		{
			int heal = 500 + plev * 10;
			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				cptr q,s;
				int item;

				item_tester_hook = item_tester_hook_mercury;
#ifdef JP
				q = "�ǂ̐G�}���g���܂���? ";
				s = "���Ȃ��͐G�}�ƂȂ鐅��������Ă��Ȃ��B";
#endif
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);	
				}

				msg_print("���Ȃ��͗��𐸐����Ĉ��񂾁B");
				hp_player(heal);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
				set_image(0);
				restore_level();
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				set_alcohol(0);
				set_asthma(0);

				set_broken(-(BROKEN_MAX));

			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�l���f�Ɖu";
		if (desc) return "��莞�ԁA�_�A�d���A�Ή��A��C�̍U����S���󂯕t���Ȃ��Ȃ�B";
#else
		if (name) return "Breathe Logrus";
		if (desc) return "Fires an extremely powerful ball of chaos.";
#endif
    
		{
			int base = plev / 4;
			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = randint1(base) + base;

				set_ele_immune(DEFENSE_4ELEM, time);
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�G�[�e���X�g���C�N";
		if (desc) return "�V��̑�܌��f�ɂ�閳�����U�����s���B";
#endif
    
		{
			int dam = plev * 10;
			int rad = 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISP_ALL, dir, dam, rad);
			}
		}
		break;
	}

	return "";
}



/*:::�J�I�X�̈�V����*/
static cptr do_new_spell_chaos(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
	static const char s_random[] = "�����_��";
#else
	static const char s_dam[] = "dam ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;


	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�}�W�b�N�E�~�T�C��";
		if (desc) return "�ア���@�̖����B";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + ((plev - 1) / 5);
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�g���b�v/�h�A�j��";
		if (desc) return "�אڂ���㩂Ɣ���j�󂷂�B";
#else
		if (name) return "Trap / Door Destruction";
		if (desc) return "Destroys all traps in adjacent squares.";
#endif
    
		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				destroy_doors_touch();
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�M��";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Flash of Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�����̎�";
		if (desc) return "���������������U�����ł���悤�ɂ���B";
#else
		if (name) return "Touch of Confusion";
		if (desc) return "Attempts to confuse the next monster that you hit.";
#endif
    
		{
			if (cast)
			{
				if (!(p_ptr->special_attack & ATTACK_CONFUSE))
				{
#ifdef JP
					msg_print("���Ȃ��̎�͌���n�߂��B");
#else
					msg_print("Your hands start glowing.");
#endif

					p_ptr->special_attack |= ATTACK_CONFUSE;
					p_ptr->redraw |= (PR_STATUS);
				}
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�����y��";
		if (desc) return "���@�̋�����B";
#else
		if (name) return "Mana Burst";
		if (desc) return "Fires a ball of magic.";
#endif
    
		{
			int dice = 3;
			int sides = 5;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			///class ���C�W�n�͈З͂��㏸���閂�@
			if (cp_ptr->magicmaster)
				base = plev + plev / 2;
			else
				base = plev + plev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MISSILE, dir, damroll(dice, sides) + base, rad);

				/*
				 * Shouldn't actually use GF_MANA, as
				 * it will destroy all items on the
				 * floor
				 */
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�t�@�C�A�E�{���g";
		if (desc) return "���̃{���g�������̓r�[������B";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�͂̌�";
		if (desc) return "���������ȕ����̋�����B";
#else
		if (name) return "Fist of Force";
		if (desc) return "Fires a tiny ball of disintegration.";
#endif
    
		{
			int dice = 8 + ((plev - 5) / 4);
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir,
					damroll(dice, sides), 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�e���|�[�g";
		if (desc) return "�������̃e���|�[�g������B";
#else
		if (name) return "Teleport Self";
		if (desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�����_�[";
		if (desc) return "�����X�^�[�Ƀ����_���Ȍ��ʂ�^����B";
#else
		if (name) return "Wonder";
		if (desc) return "Fires something with random effects.";
#endif
    
		{
			if (info) return s_random;

			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;

				cast_wonder(dir);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�J�I�X�E�{���g";
		if (desc) return "�J�I�X�̃{���g�������̓r�[������B";
#else
		if (name) return "Chaos Bolt";
		if (desc) return "Fires a bolt or ball of chaos.";
#endif
    
		{
			int dice = 10 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_CHAOS, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�\�j�b�N�E�u�[��";
		if (desc) return "�����𒆐S�Ƃ��������̋��𔭐�������B";
#else
		if (name) return "Sonic Boom";
		if (desc) return "Generates a ball of sound centered on you.";
#endif
    
		{
			int dam = 60 + plev;
			int rad = plev / 10 + 2;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
#ifdef JP
				msg_print("�h�[���I�������h�ꂽ�I");
#else
				msg_print("BOOM! Shake the room!");
#endif

				project(0, rad, py, px, dam, GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�j�ł̖�";
		if (desc) return "�����Ȗ��͂̃r�[������B";
#else
		if (name) return "Doom Bolt";
		if (desc) return "Fires a beam of pure mana.";
#endif
    
		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_MANA, dir, damroll(dice, sides));
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�t�@�C�A�E�{�[��";
		if (desc) return "���̋�����B";
#else
		if (name) return "Fire Ball";
		if (desc) return "Fires a ball of fire.";
#endif
    
		{
			int dam = plev + 55;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�e���|�[�g�E�A�E�F�C";
		if (desc) return "�����X�^�[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if (name) return "Teleport Other";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�j��̌��t";
		if (desc) return "���ӂ̃A�C�e���A�����X�^�[�A�n�`��j�󂷂�B";
#else
		if (name) return "Word of Destruction";
		if (desc) return "Destroy everything in nearby area.";
#endif
    
		{
			int base = 12;
			int sides = 4;

			if (cast)
			{
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���O���X����";
		if (desc) return "����ȃJ�I�X�̋�����B";
#else
		if (name) return "Invoke Logrus";
		if (desc) return "Fires a huge ball of chaos.";
#endif
    
		{
			int dam = plev * 2 + 99;
			int rad = plev / 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_CHAOS, dir, dam, rad);
			}
		}
		break;

		///mod140211 �J�I�X�̈�̑��ҕϗe�@�J�I�X�ϐ������ɂ͌����Ȃ�����Ƀp���[�O�{�ɂ���
	case 16:
#ifdef JP
		if (name) return "���ҕϗe";
		if (desc) return "�����X�^�[1�̂�ϐg������B��R�����Ɩ����B";
#else
		if (name) return "Polymorph Other";
		if (desc) return "Attempts to polymorph a monster.";
#endif
    
		{
			int power = plev * 3;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				poly_monster(dir, plev);
			}
		}
		break;

		//v1.1.85 �J���W�A���̌��ʂ�ύX...���悤���Ǝv��������߂�
#if 0
	case 17:
		if (name) return "�J���W�A��";
		if (desc) return "�����𒆐S�Ɏ_�����̋���ȃ{�[���𔭐������A����ɒn�`���_�̏��ɕω�������B";

		{

			int dam;

			if (cp_ptr->magicmaster) dam = 225 + plev * 9;
			else dam = 150 + plev * 6;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				msg_print("���͂̕����}���ɗn�����ꂽ�I");
				project(0, 5, py, px, dam, GF_ACID, (PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM),-1);
				project(0, 4, py, px, 5, GF_MAKE_ACID_PUDDLE, (PROJECT_GRID|PROJECT_HIDE), -1);

			}
		}
		break;
#else
	case 17:
		if (name) return "�J���W�A��";
		if (desc) return "���E���̑S�Ă��}���ɗ򉻂����čU������B";
    
		{
			int dam = plev * 2;
			int dice = 1;
			int sides = plev;

			if (info) return info_damage(dice, sides, dam);

			if (cast)
			{
				dam += damroll(dice,sides);
				msg_print("�������̕����n�����ꂽ�I");
				project_hack2(GF_DISENCHANT, dice,sides,dam);

			}
		}
		break;
#endif

	case 18:
#ifdef JP
		if (name) return "���q����";
		if (desc) return "����ȕ����̋�����B";
#else
		if (name) return "Disintegrate";
		if (desc) return "Fires a huge ball of disintegration.";
#endif
    
		{
			int dam = plev + 70;
			int rad = 3 + plev / 40;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISINTEGRATE, dir, dam, rad);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�����ϗe";
		if (desc) return "���݂̊K���č\������B";
#else
		if (name) return "Alter Reality";
		if (desc) return "Recreates current dungeon level.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				alter_reality();
			}
		}
		break;
	case 20:
#ifdef JP
		if (name) return "�v���Y�}��";
		if (desc) return "���͂ȃv���Y�}�̋�����B";
#else
		if (name) return "Plasma Ball";
		if (desc) return "Fires a ball of plasma.";
#endif
    
		{
			int dice = 1;
			int sides = plev * 4;
			int base = plev * 2 + 100;
			int rad = 2 + plev / 40;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_PLASMA, dir, (damroll(dice,sides)+base), rad);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "���ׂ̐n";
		if (desc) return "�������̕����邢�͏㎿�̕���ɑ΂��A�i���I�ɑΒ����̓�����t�^����B";
#else
		if (name) return "Chaos Branding";
		if (desc) return "Makes current weapon a Chaotic weapon.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(7);
			}
		}
		break;
	case 22:
#ifdef JP
		if (name) return "�}�W�b�N�E���P�b�g";
		if (desc) return "���P�b�g�𔭎˂���B";
#else
		if (name) return "Magic Rocket";
		if (desc) return "Fires a magic rocket.";
#endif
    
		{
			int dam = 150 + plev * 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_print("���P�b�g���ˁI");
#else
				msg_print("You launch a rocket!");
#endif

				fire_rocket(GF_ROCKET, dir, dam, rad);
			}
		}
		break;


	case 23:
#ifdef JP
		if (name) return "�͏�̋�";
		if (desc) return "����ȏd�͑����̋�����B����ȓG�Ɍ����₷�����ł���G�ɂ͌��ʂ������B";
#endif
    
		{
			int dice = 1;
			int sides = plev * 4;
			int base = plev*2;
			int rad = 5;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int dam = base + damroll(dice,sides);
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_GRAVITY, dir, dam, rad);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "���ȕϗe";
		if (desc) return "������ϐg�����悤�Ƃ���B";
#else
		if (name) return "Polymorph Self";
		if (desc) return "Polymorphs yourself.";
#endif
    
		{
			if (cast)
			{
#ifdef JP

				if(cp_ptr->flag_only_unique)
				{
					msg_print("�c�c���ȗ\��������B����ς��߂Ă������B");
					return NULL;
				}
				if (!get_check("�ϐg���܂��B��낵���ł����H")) return NULL;
#else
				if (!get_check("You will polymorph yourself. Are you sure? ")) return NULL;
#endif
				set_deity_bias(DBIAS_COSMOS, -5);
				do_poly_self();
			}
		}
		break;
	case 25:
		if (name) return "�X�L�}�`��";
		if (desc) return "�A�C�e��������ɓ]������B������1/13�̊m���ŃA�C�e�������X�g����B";
		{

			if (info) return "";

			if (cast)
			{
				msg_print("����ւ̋��E���q����...");
				item_recall(3);
			}
		}
		break;
/*
	case 25:
#ifdef JP
		if (name) return "�����Q";
		if (desc) return "�����̎��ӂ�覐΂𗎂Ƃ��B";
#else
		if (name) return "Meteor Swarm";
		if (desc) return "Makes meteor balls fall down to nearby random locations.";
#endif
    
		{
			int dam = plev * 2;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				cast_meteor(dam, rad,GF_METEOR);
			}
		}
		break;
*/
	case 26:
#ifdef JP
		if (name) return "��Ԙc��";
		if (desc) return "��Ԃ�c�߂Ď��E���S�ĂɍU������B����ϐ����Ȃ��ꍇ�����������e�����󂯂�B";
#endif
    
		{
			int dice = 1;
			int sides = plev * 10;
			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				msg_print("���͂̋�Ԃ��c�񂾁I");
				project_hack2(GF_DISTORTION, dice, sides, 0);
				if(!p_ptr->resist_time)
				{
					take_hit(DAMAGE_NOESCAPE, randint1(100), "��Ԙc��", -1);
					teleport_player(randint1(50), TELEPORT_PASSIVE);
				}
			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "���O���X�̃u���X";
		if (desc) return "���ɋ��͂ȃJ�I�X�̋�����B";
#else
		if (name) return "Breathe Logrus";
		if (desc) return "Fires an extremely powerful ball of chaos.";
#endif
    
		{
			int dam = p_ptr->chp;
			int rad = 2;
			if(dam > 1600) dam = 1600;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_CHAOS, dir, dam, rad);
			}
		}
		break;
	case 28:
#ifdef JP
		if (name) return "���̗͂�";
		if (desc) return "���ɋ��͂ŋ���ȏ����Ȗ��͂̋�����B";
#else
		if (name) return "Mana Storm";
		if (desc) return "Fires an extremely powerful huge ball of pure mana.";
#endif
    
		{
			int dam = 300 + plev * 5;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MANA, dir, dam, rad);
			}
		}
		break;


	case 29:
#ifdef JP
		if (name) return "���׏���";
		if (desc) return "�����_���ȑ����̎��E���U���𔭐�������B";
#else
		if (name) return "Call Chaos";
		if (desc) return "Generate random kind of balls or beams.";
#endif
    
		{
			if (info) return format("%s1d777", s_dam);

			if (cast)
			{

				call_chaos();
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "��������";
		if (desc) return "�����̎��͂Ɍ������āA���P�b�g�A�����Ȗ��͂̋��A�j�M�̋�����B�������A�ǂɗאڂ��Ďg�p����ƍL�͈͂�j�󂷂�B";
#else
		if (name) return "Call the Void";
		if (desc) return "Fires rockets, mana balls and nuclear waste balls in all directions each unless you are not adjacent to any walls. Otherwise *destroys* huge area.";
#endif
    
		{
			if (info) return format("%s3 * 175", s_dam);

			if (cast)
			{
				call_the_();
			}
		}
		break;



	case 31:
		if (name) return "�I�[�v���p���h��";
		if (desc) return "�ɂ߂ē���Ȃ��Ƃ��N����B�������̌��̒�������ʂ�I�ׂ邪�L�����Z���͂ł��Ȃ��B�㏞�Ƃ��Čo���l��10%���P�v�I�Ɏ�����B�d���l�`���g�����ꍇ�o���l�͌������Ȃ���25%�̊m���ő����i�̂ǂꂩ���j�󂳂��B";
  
		{

			if (info) return "�s��";

			if (cast)
			{
				int i,c,effect_mode;
				cptr effect_desc[10] = {"����S�Đ��ɂ���","�ǂ�S�ĐX�ɂ���","����S�ėn��ɂ���","�����~�߂�(��10�s����)","�S��+���ɂ̑ϐ�+����(100�^�[��)","�t���A�̃����X�^�[��ǂ�����","�ꎞ�I�Ȕ\�͑啝����(100�^�[��)","�i�v�I�Ȕ\�͑���","�G��ق点��","���E���̑��݂�1d7777�_���[�W"};

				int effect_num[3];
				for(i=0;i<3;i++) effect_num[i] = -1;
				effect_num[0] = randint0(10);
				while(1)
				{
					effect_num[1] = randint0(10);
					if(effect_num[0] != effect_num[1]) break;	
				}
				while(1)
				{
					effect_num[2] = randint0(10);
					if(effect_num[0] != effect_num[2] && effect_num[1] != effect_num[2]) break;	
				}
				screen_save();
				msg_print("�N�����������ʂ��L���őI�����Ă��������B");
				for(i=1;i<7;i++)Term_erase(12, i, 255);
				for(i=0;i<3;i++) put_str(format("%c) %s",('a'+i),effect_desc[effect_num[i]]), 3+i , 40);

				while(1)
				{
					c = inkey();
					if(c >= 'a' && c <= 'c') break;
				}
				screen_load();

				effect_mode = effect_num[c - 'a'];

				switch(effect_mode)
				{
				case 0:
				case 1:
				case 2:
				{
					int y,x,feat;
					feature_type *f_ptr;

					if(effect_mode == 0)msg_print("�����Ƌ��Ƀ_���W�������^���ɏP��ꂽ�I");
					if(effect_mode == 1)msg_print("�ӂ蒆����₪�ӂ���悤�ȉ����������B");
					if(effect_mode == 2)msg_print("�n�����Ƌ��Ƀ_���W�������n��ɕ���ꂽ�I");

					for (y = 1; y < cur_hgt - 1; y++)
					{
						for (x = 1; x < cur_wid - 1; x++)
						{
							cave_type *c_ptr = &cave[y][x];
							feat = get_feat_mimic(c_ptr);
							f_ptr = &f_info[feat];

							if (effect_mode == 0 && have_flag(f_ptr->flags, FF_FLOOR))
								cave_set_feat(y, x, feat_deep_water);
							if (effect_mode == 1 && have_flag(f_ptr->flags, FF_HURT_ROCK))
								cave_set_feat(y, x, feat_tree);
							if (effect_mode == 2 && have_flag(f_ptr->flags, FF_FLOOR))
								 cave_set_feat(y, x, feat_deep_lava);
						}
					}

					p_ptr->redraw |= (PR_MAP);
					p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
					handle_stuff();
					break;
				}
				case 3:
					if (world_player)
					{
						msg_print("���Ɏ��͎~�܂��Ă���B");
						return NULL;
					}
					world_player = TRUE;
					msg_print("�����~�܂����B");
					//v1.1.58
					flag_update_floor_music = TRUE;
					msg_print(NULL);
					p_ptr->energy_need -= 2000 ;
					p_ptr->redraw |= (PR_MAP);
					p_ptr->update |= (PU_MONSTERS);
					p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
					handle_stuff();
					break;
				case 4:
				{
					int v = 100;
					hp_player(5000);
					set_stun(0);
					set_cut(0);
					set_image(0);
					(void)set_food(PY_FOOD_MAX-1);
					do_res_stat(A_STR);
					do_res_stat(A_INT);
					do_res_stat(A_WIS);
					do_res_stat(A_DEX);
					do_res_stat(A_CON);
					do_res_stat(A_CHR);
					restore_level();						
					set_fast(v, FALSE);
					set_oppose_acid(v, FALSE);
					set_oppose_elec(v, FALSE);
					set_oppose_fire(v, FALSE);
					set_oppose_cold(v, FALSE);
					set_oppose_pois(v, FALSE);
					set_ultimate_res(v, FALSE);
					set_broken(-(BROKEN_MAX));//v1.1.69
					break;
				}
				case 5:
				{
					msg_print("�Ђǂ��ǓƂɂȂ����C������B");

					//v1.1.32 �ʊ֐��ɒu��������
					mass_genocide_3(0,TRUE,TRUE);
#if 0
					for (i = 1; i < m_max; i++)
					{
						monster_type *m_ptr = &m_list[i];
						monster_race *r_ptr = &r_info[m_ptr->r_idx];

						if (!m_ptr->r_idx) continue;
						if(m_ptr->r_idx == MON_MASTER_KAGUYA) continue;
						if (i == p_ptr->riding) continue;
						if (r_ptr->flags1 & RF1_QUESTOR)
						{
							if(distance(m_ptr->fy,m_ptr->fx,py,px) <= MAX_SIGHT) teleport_away(i,100,TELEPORT_PASSIVE);
							continue;
						}

						if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
						{
							char m_name[80];

							monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
							do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
						}

						/* Delete this monster */
						delete_monster_idx(i);
					}
#endif
					break;
				}
				case 6:
					msg_print("�X�[�p�[�}���ɂȂ����C������I");
					for(i=0;i<6;i++)set_tim_addstat(i,130,100,FALSE);
					break;
				case 7:
					msg_print("�̂���͂����ӂ�o��I");
					for(i=0;i<6;i++)
					{
						do_inc_stat(i);
						do_inc_stat(i);
					}
					break;
				case 8:
					msg_print("�_���W�����Ɉَ��Ȗ��͂��������B");
					p_ptr->silent_floor = 1;//�t���A����o���Ƃ�0�ɒ����B���ꂪ1�̂Ƃ�magic_barrier�Ŗ�������TRUE��Ԃ��Bstupid�łȂ��G�͖��@���g��Ȃ��Ȃ�B

					break;
				case 9:
					msg_print("���͂��������ɖ������ꂽ�B");
					project_hack2(GF_DISP_ALL, 1,7777,0);
					break;


				default:
					msg_print("ERROR:MAHMAN�Œ�`����Ă��Ȃ�effect�l���g��ꂽ");
					return NULL;
				}
				
				if(p_ptr->prace == RACE_ANDROID )
				{
					int slot = INVEN_RARM + randint0(12);

					if(!one_in_(4) || !(inventory[slot].k_idx) || check_invalidate_inventory(slot))
					{
						msg_print("��u�̂��k�����������N����Ȃ������B");

					}
					else
					{
						object_type *o_ptr = &inventory[slot];
						char		o_name[MAX_NLEN];
						object_desc(o_name, o_ptr, OD_OMIT_PREFIX);
						msg_format("%s�͕��X�ɍӂ��ď������I",o_name);
						inven_item_increase(slot, -1);
						inven_item_describe(slot);
						inven_item_optimize(slot);
						calc_android_exp();
					}
				}
				else
				{
					msg_print("�����͂�啝�ɑr�������B");
					/* Lose some experience (permanently) */
					p_ptr->exp -= (p_ptr->exp / 10);
					p_ptr->max_exp -= (p_ptr->max_exp / 10);
					check_experience();
				}


			}
		}
		break;






	}

	return "";
}


/*:::�V�̈�F�\��*/
static cptr do_new_spell_foresee(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int rad= DETECT_RAD_DEFAULT;
	int plev = p_ptr->lev;
	int damage;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�z���T�m";
		if (desc) return "�߂��ɂ�������z����T�m����B";
#else
		if (name) return "Zap";
		if (desc) return "Fires a bolt or beam of lightning.";
#endif
    
		{
			if (info) return info_radius(rad);

			if (cast)
			{
				detect_treasure(rad);
				detect_objects_gold(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�g���b�v���m";
		if (desc) return "�߂��ɂ���㩂����m����B";
#else
		if (name) return "Wizard Lock";
		if (desc) return "Locks a door.";
#endif
    
		{
			if (info) return info_radius(rad);
			if (cast)
			{
				detect_traps(rad, TRUE);

			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�����X�^�[���m";
		if (desc) return "�߂��̑S�Ẵ����X�^�[�����m����B";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif
    
		{
			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
				detect_monsters_invis(rad);
			}
		}
		break;
	case 3:
#ifdef JP
		if (name) return "���|����";
		if (desc) return "���|��Ԃ����Â���B";
#endif
    
		{
			if (cast)
			{
				set_afraid(0);
			}
		}
		break;


	case 4:
#ifdef JP
		if (name) return "�h�A�E�K�i���m";
		if (desc) return "�߂��ɂ���h�A�ƊK�i�����m����B";
#endif
    
		{

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_doors(rad);
				detect_stairs(rad);
			}	
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�A�C�e�����m";
		if (desc) return "�߂��̑S�ẴA�C�e�������m����B";
#endif
		{
			if (info) return info_radius(rad);

			if (cast)
			{
				detect_objects_normal(rad);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���|�̃r�W����";
		if (desc) return "�w�肵����̂̃^�[�Q�b�g�ɋ��낵�����o�������ċ��|������B���j�[�N�����X�^�[�ɂ͌������A�����x���ȓG�ɂ͌����ɂ����B";
#endif    
		{
			int power = plev + 25;
			if (cast)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fear_monster(dir, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�Ӓ�";
		if (desc) return "�A�C�e�������ʂ���B";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif
    
		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

		break;

	case 8:
#ifdef JP
		if (name) return "���ӊ��m";
		if (desc) return "���ӂ̒n�`�����m����B";
#endif
    	{
			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "����";
		if (desc) return "�����X�^�[�̑����A�c��̗́A�ő�̗́A�X�s�[�h�A���̂�m��B";
#else
		if (name) return "Probing";
		if (desc) return "Proves all monsters' alignment, HP, speed and their true character.";
#endif
    
		{
			if (cast)
			{
				probing();
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "���̋P������";
		if (desc) return "���݂̃t���A�ɂ��関����́u���̂���A�C�e���v�����m���A��܂��ȋ�����m��B�A�[�e�B�t�@�N�g�͊��m�ł��Ȃ��B";
#else
		if (name) return "Resist Cold";
		if (desc) return "Gives resistance to cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			if (cast)
			{
				search_specific_object(1);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "���_����";
		if (desc) return "��莞�ԁA�e���p�V�[�\�͂𓾂�B";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;


	case 12:
#ifdef JP
		if (name) return "���o����";
		if (desc) return "���o��Ԃ����Â���B";
#endif
    
		{

			if (cast)
			{
				set_image(0);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "��C���m";
		if (desc) return "���݂���t���A�̕��͋C���@�m����B";
#endif
    
		{
			if (cast)
			{
				if(!dun_level)
				{
					msg_print("���g���Ă��Ӗ��͂Ȃ��B");
					return NULL;
				}
				p_ptr->feeling_turn = 0;
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�j�ł̃r�W����";
		if (desc) return "�^�[�Q�b�g�̐��_�ɔj�œI�ȃC���[�W��A���t���čU������B��R�����Ɩ����B";
#endif
    
		{
			int base = plev * 2;
			int dice = 5 + (plev - 5) / 3;
			int sides = 9;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				damage = damroll(dice,sides) + base;
				if (!get_aim_dir(&dir)) return FALSE;
				fire_ball_hide(GF_BRAIN_SMASH, dir, damage, 0);
				break;
			}
		}
		break;


	case 15:
#ifdef JP
		if (name) return "�S���m";
		if (desc) return "�߂��̑S�Ẵ����X�^�[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if (name) return "Detection";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;


	case 16:
#ifdef JP
		if (name) return "�V���b�t��";
		if (desc) return "�J�[�h�̐肢������B";
#else
		if (name) return "Shuffle";
		if (desc) return "Causes random effects.";
#endif
    
		{
			if (info) return "�����_��";

			if (cast)
			{
				cast_shuffle();
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "���j�[�N�E�����X�^�[���m";
		if (desc) return "���݂̃t���A�ɂ���S�Ắu���ʂȋ��G�v�̖��O��m��B";
#endif
    
		{
			int base = 20;

/*:::�p�����e�B�A��������R�s�[*/
			if (cast)
			{
			int i;
			monster_type *m_ptr;
			monster_race *r_ptr;
#ifdef JP
			msg_print("��ȏꏊ�����̒��ɕ����񂾁D�D�D");
#endif
			/* Process the monsters (backwards) */
				for (i = m_max - 1; i >= 1; i--)
				{
					/* Access the monster */
					m_ptr = &m_list[i];
					/* Ignore "dead" monsters */
					if (!m_ptr->r_idx) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if(r_ptr->flags1 & RF1_UNIQUE)
					{
#ifdef JP
						msg_format("%s�D ",r_name + r_ptr->name);
#endif
					}
				}
				break;
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "*�Ӓ�*";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#endif
    
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "���ȕ���";
		if (desc) return "���݂̎����̏�Ԃ����S�ɒm��B";
#else
		if (name) return "Self Knowledge";
		if (desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif
    
		{
			if (cast)
			{
				self_knowledge();
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�ύ���";
		if (desc) return "��莞�ԁA���ב����U���ɑ΂���ϐ��𓾂�B";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_chaos(randint1(base) + base, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "*�����X�^�[���m*";
		if (desc) return "���݂̃t���A�ɂ���S�Ẵ����X�^�[�����m����B";
#else
		if (name) return "Stone to Mud";
		if (desc) return "Turns one rock square to mud.";
#endif
		{
			if (cast)
			{
				(void)detect_monsters_invis(255);
				(void)detect_monsters_normal(255);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�����\�m";
		if (desc) return "�����߂�����������B�ꎞ�I��AC�啝�㏸�Ɩ������㏸�ƌx���𓾂�B";
#else
		if (name) return "Ray of Light";
		if (desc) return "Fires a beam of light which damages to light-sensitive monsters.";
#endif
    
		{
			int base = 10 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_foresight(randint1(base) + base, FALSE);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�`���̕i���m";
		if (desc) return "���݂̃t���A�ɂ���A�[�e�B�t�@�N�g�����m���A��܂��ȋ�����m��B";
#else
		if (name) return "Resist Cold";
		if (desc) return "Gives resistance to cold. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			if (cast)
			{
				search_specific_object(2);

			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�q���C���̓���";
		if (desc) return "��莞�ԁA�Q�Ă���G���ɂ߂ċN���ɂ����Ȃ�B";
#endif
    
		{
			int base = 25 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_stealth(randint1(base) + base, FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "*�痢��*";
		if (desc) return "�t���A�S�Ă̒n�`�A�A�C�e���A㩂����m����B";
#endif
    
		{
			if (cast)
			{
				wiz_lite(FALSE);
				msg_print("�t���A�̒n�`�ƃA�C�e���������Ƃ����I");
				(void)detect_traps(255,TRUE);

			}
		}
		break;


	case 26:
#ifdef JP
		if (name) return "���ʍ���";
		if (desc) return "�����̎��͂̃_���W������j�󂷂�B�͈͓��̃A�C�e���ƃ����X�^�[�͑S�ď�������B";
#endif
    
		{
			int base = 12;
			int sides = 4;
			if (info) return format("�͈́F%d+1d%d",base,sides);

			if (cast)
			{
				destroy_area(py, px, base + randint1(sides), FALSE,FALSE,FALSE);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�V���h�E�E�V�t�g";
		if (desc) return "���s���E�ɓn�邱�Ƃō�����t���A�����ւ���B�t���A�̍č\���́A�u�قƂ�ǁv�u���ɍs����B";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				if (!get_check("�{���ɕʂ̃t���A�Ɉړ����܂����H")) return NULL;
#endif

				if (p_ptr->inside_arena || ironman_downward)
				{
#ifdef JP
					msg_print("�����N����Ȃ������B");
#endif
				}
				else
				{
					/*:::0�ł͂܂����͂�*/
					p_ptr->alter_reality = 1;
					p_ptr->redraw |= (PR_STATUS);
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�j�ł̐鍐";
		if (desc) return "1�̂̃����X�^�[�ɑ΂��j�œI�ȍЉЂ������炷�B��R�����Ɩ����B���j�[�N�����X�^�[�ɂ͌����Ȃ��B";
#endif
    
		{
			if (info) return format("�����F�Ώ�HP��40%-60%");

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				msg_print("���Ȃ��͔j�œI�ȉ^����a���o�����B");
				/*:::45lev@��55lev�����X�^�[��1/4���x�h������*/
				fire_ball_hide(GF_HAND_DOOM, dir, plev * 3, 0);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�K�^�̃��J�j�Y��";
		if (desc) return "���݂̃t���A�ł����Z���ԍK�^�ɂȂ�B��ɓG�����Ƃ��A�C�e���̎��ɉe������B";
#else
		if (name) return "Detection";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{

			int base = 1 + plev / 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_lucky(10,FALSE);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "���ʏC��";
		if (desc) return "�����̑̂̑S�Ă���Ԃɖ߂��B";
#endif
    
		{
			if (cast)
			{
				hp_player(5000);
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
				set_image(0);
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				set_alcohol(0);
				restore_level();
				dispel_player();
				set_asthma(0);
				set_broken(-(BROKEN_MAX));//v1.1.69

				if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
				{
#ifdef JP
					if(muta_erasable_count()) msg_print("�S�Ă̓ˑR�ψق��������B");
#endif
					p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
					p_ptr->muta1 = p_ptr->muta1_perma;
					p_ptr->muta2 = p_ptr->muta2_perma;
					p_ptr->muta3 = p_ptr->muta3_perma;
//					p_ptr->muta3 = p_ptr->muta4_perma;//v1.1.80 �~�X�C�� 
					p_ptr->muta4 = p_ptr->muta4_perma;
					p_ptr->update |= PU_BONUS;
					p_ptr->bydo = 0;
					handle_stuff();
					mutant_regenerate_mod = calc_mutant_regenerate_mod();
				}
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�A�J�V�b�N���R�[�h����";
		if (desc) return "�ꎞ�I�ɒm�\�ƌ��������E�𒴂��ď㏸����B";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{

			int base = 25 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = randint1(base) + base;
				set_tim_addstat(A_INT,199,time, FALSE);
				set_tim_addstat(A_WIS,199,time, FALSE);
				msg_print("���Ȃ��̓��ɑS�Ă̎���̑S�Ă̒m�������ꂱ��ł����I");			
			}


		}
		break;
	}

	return "";
}



static cptr do_new_spell_enchant(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;
	int dice,sides,base,damage;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "���@�̎{��";
		if (desc) return "���������ł��{������B�{�����ꂽ���͊J���ł��Ȃ����@���󂵂��薂�@�ŊJ���邱�Ƃ͂ł���B";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wizard_lock(dir);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "���@�̜W";
		if (desc) return "���͂̍��߂�ꂽ�����B�N���X�{�E�̖����{�����B�З͖͂�̎��ŕς��B";
#endif
    
		{
			int mul = 2;
			cptr q,s;
			object_type *o_ptr;
			int type;
			int item;

			base = plev / 2 + 5;
			dice = 1;
			sides = 5;

			if (info) return format("�����F%d+%dd%d�`",base,dice,sides);

			if (cast)
			{
				item_tester_tval = TV_BOLT;
#ifdef JP
				q = "�ǂ̖�𔭎˂��܂���? ";
				s = "���Ȃ��̓N���X�{�E�̖�������Ă��Ȃ��B";
#endif
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;
				if (!get_aim_dir(&dir)) return NULL;
				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
					o_ptr = &inventory[item];
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);	
					o_ptr = &o_list[0 - item];
				}
				if(o_ptr->sval == SV_AMMO_NORMAL)
				{
					dice += 2;
					mul = 3;
				}
				if(o_ptr->sval == SV_AMMO_HEAVY)
				{
					dice += 5;
					mul = 4;
				}
				damage = (base + damroll(dice,sides) + o_ptr->to_d) * mul;
				if(o_ptr->name2 == EGO_ARROW_HOLY) type = GF_HOLY_FIRE;
				else if(o_ptr->name2 == EGO_ARROW_ELEC) type = GF_ELEC;
				else if(o_ptr->name2 == EGO_ARROW_FIRE) type = GF_FIRE;
				else if(o_ptr->name2 == EGO_ARROW_COLD) type = GF_COLD;
				else type = GF_ARROW;

				fire_bolt(type, dir, damage);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�񕜗͋���";
		if (desc) return "��莞�ԁA�񕜗͂����������B";
#else
		if (name) return "Regeneration";
		if (desc) return "Gives regeneration ability for a while.";
#endif
    
		{
			int base = MIN(25,plev);

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_regen(base + randint1(base), FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�g���b�v/�h�A�j��";
		if (desc) return "�אڂ���㩂Ɣ���j�󂷂�B";
#else
		if (name) return "Trap / Door Destruction";
		if (desc) return "Destroys all traps in adjacent squares.";
#endif
    
		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				destroy_doors_touch();
			}
		}
		break;


	case 4:
#ifdef JP
		if (name) return "���͋���";
		if (desc) return "��莞�ԁA�ԊO�����͂Ɠ����̎��F�𓾂�B";
#else
		if (name) return "Resist Fire";
		if (desc) return "Gives resistance to fire. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
				set_tim_infra(base + randint1(base), FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�󕠏[��";
		if (desc) return "���͂��h�{�ɕϊ�����B";
#else
		if (name) return "Heroism";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�ϔM�ϊ�";
		if (desc) return "��莞�ԁA�Ή��Ɨ�C�ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�ؗ͑���";
		if (desc) return "��莞�ԁA�r�͂𑝉�������B������̘r�͂�40�𒴂��Ȃ��B";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 15 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_format("���Ȃ��͘r�͂����������B");
				set_tim_addstat(A_STR,1+plev/10,base + randint1(base),FALSE);
			}
		}
		break;

	case 8:

		if (name) return "���@�w�ݒu";
		if (desc) return "�}���p���@�w��ݒu����B���@�w�͔z���Ƃ��Ĉ����A�ړ������ɖ��@�U���݂̂��s���B�K�ړ�����Ə�����B���x��40�ȍ~�ł͈�x�ɕ�������B";
    
		{
			if (cast)
			{
				bool flag = FALSE;
				u32b mode = PM_FORCE_PET | PM_EPHEMERA;
				if(p_ptr->lev > 39) mode |= PM_ALLOW_GROUP;
				if (summon_specific(-1, py, px, p_ptr->lev, SUMMON_MAGICSIGN, mode)) flag = TRUE;

				if(flag) msg_print("���Ȃ��͒��ɖ��@�w��`�����B");
				else msg_print("���@�w��`���̂Ɏ��s�����B");

			}
		}
		break;




	case 9:
#ifdef JP
		if (name) return "�ϓd";
		if (desc) return "��莞�ԁA�d���ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�ώ_";
		if (desc) return "��莞�ԁA�_�ւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
			}
		}
		break;
	case 11:
#ifdef JP
		if (name) return "����m��";
		if (desc) return "����m�����A���|����������B";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_shero(randint1(base) + base, FALSE);
				hp_player(30);
				set_afraid(0);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "���͏[�U";
		if (desc) return "��/���@�_�̏[�U�񐔂𑝂₷���A�[�U���̃��b�h�̏[�U���Ԃ����炷�B";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "��Ӌ}����";
		if (desc) return "��莞�ԉ񕜗͂���������A�C�������g���A����ɓłɑ΂���ϐ��𓾂�B";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif
    
		{
			int base = 15 + plev / 3;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_regen(base + randint1(base), FALSE);
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "���틭��";
		if (desc) return "����̖������C���ƃ_���[�W�C������������B";
#else
		if (name) return "Enchant Weapon";
		if (desc) return "Attempts to increase +to-hit, +to-dam of a weapon.";
#endif
    
		{
			if (cast)
			{
				if (!enchant_spell(randint0(4) + 1, randint0(4) + 1, 0)) return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�h���";
		if (desc) return "�Z�̖h��C������������B";
#else
		if (name) return "Enchant Armor";
		if (desc) return "Attempts to increase +AC of an armor.";
#endif
    
		{
			if (cast)
			{
				if (!enchant_spell(0, 0, randint0(3) + 2)) return NULL;
			}
		}
		break;
	case 16:
		if (name) return "�S�[��������";
		if (desc) return "�S�[��������́`������������B�G�}�Ƃ��ċ��������B";
    
		{
			int lev = p_ptr->lev;
			int charge = lev * lev * 4;
			int i;
			bool flag = FALSE;
			if (info) return format("�������x���F%d �G�}�F��%d",lev,charge);
			if (cast)
			{
				int num = randint1(2) + plev / 15;

				if(p_ptr->au < charge)
				{
					msg_print("���Ȃ��͏[���Ȃ����������Ă��Ȃ��B");
					return NULL;
				}
				p_ptr->au -= charge;
				p_ptr->redraw |= PR_GOLD;

				for(i=0;i<num;i++) if (summon_specific(-1, py, px, lev, SUMMON_GOLEM, (PM_FORCE_PET | PM_ALLOW_GROUP))) flag = TRUE;

				if(flag) msg_print("�ӂ�̒n�ʂ���l�^�̉򂪋N���オ�����B");
				else msg_print("�S�[���������̂Ɏ��s�����B");
			}
		}
		break;


	case 17:
#ifdef JP
		if (name) return "�畆�d��";
		if (desc) return "��莞�ԁAAC���㏸������B";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif
    
		{
			int base = 30;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�S�ϐ�";
		if (desc) return "��莞�ԁA�_�A�d���A���A��C�A�łɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�g�̉���";
		if (desc) return "��莞�ԁA��������B";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif
    
		{
			int base = plev/2;
			int sides = plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�����̃��[��";
		if (desc) return "�����̂��鏰�̏�ɁA�����X�^�[���ʂ�Ɣ������ă_���[�W��^���郋�[����`���B";
#else
		if (name) return "Explosive Rune";
		if (desc) return "Sets a glyph under you. The glyph will explode when a monster moves on it.";
#endif
    
		{
			int dice = 7;
			int sides = 7;
			int base = plev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				explosive_rune();
			}
		}
		break;


	case 21:
#ifdef JP
		if (name) return "�A�C�e���ڍו���";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if (name) return "Knowledge True";
		if (desc) return "*Identifies* an item.";
#endif
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "���@��";
		if (desc) return "��莞�ԁA����ɗ�C�A���A�d���A�_�A�ł̂����ꂩ�̑���������B����������Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Mana Branding";
		if (desc) return "Makes current weapon some elemental branded. You must wield weapons.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_ele_attack()) return NULL;
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "���@�̊Z";
		if (desc) return "��莞�ԕ����h��Ɩ��@�h����㏸�����A����Ƀv���[���[���󂯂�قڑS�Ẵ_���[�W��2/3�Ɍ���������B";
#else
		if (name) return "Magical armor";
		if (desc) return "Gives resistance to magic, bonus to AC, resistance to confusion, blindness, reflection, free action and levitation for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_magicdef(randint1(base) + base, FALSE);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "��������";
		if (desc) return "�A�C�e���ɂ��������ア�􂢂���������B���x��40�ȍ~�ł͋����􂢂������ł���B";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			if (cast)
			{

				if (plev < 40 && remove_curse() || plev > 39 && remove_all_curse())
				{
#ifdef JP
					msg_print("�����i�̎������������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "*���͕t�^*";
		if (desc) return "�w�肵������ɖ��͂����߂Ĕz���Ƃ��Đ�킹��B�\�͕͂���̋����ɂ��ω�����B�|���ꂽ��K�ړ��������������ƌ��ʂ������ĕ���͂��̏�ɗ�����B�|���ꂽ�Ƃ����̕��킪�򉻂���B";
#else
		if (name) return "Telepathy";
		if (desc) return "Gives telepathy for a while.";
#endif
    
		{
			if (cast)
			{
				int         item;
				object_type *o_ptr;
				cptr        q, s;
				int	new_o_idx;
				int i;
				int cnt=0,max;


				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					if (m_ptr->r_idx != MON_TSUKUMO_WEAPON1 ) continue;
					msg_print("���ɍ쐬�ς݂��B");
					return NULL;
				}

				//v1.1.93 ���̕ύX
				item_tester_hook = object_is_melee_weapon_except_strange_kind;

				q = "�ǂ̕���ɖ��͂����߂܂���? ";
				s = "�g�������ȕ��킪�Ȃ��B";

				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (item >= 0) o_ptr = &inventory[item];
				else o_ptr = &o_list[0 - item];
				apply_mon_race_tsukumoweapon(o_ptr,MON_TSUKUMO_WEAPON1);

				new_o_idx = o_pop();

				if (!new_o_idx || !summon_named_creature(0, py, px, MON_TSUKUMO_WEAPON1, PM_EPHEMERA))
				{
					msg_print("���s�����B");
				}
			
				else //�����ɓ��鎞summon_named_creature()��TRUE�ŏI���A�t�r�_�̃p�����[�^�����Ɛ������ς�ł���
				{
					char m_name[80];
					monster_race *r_ptr = &r_info[MON_TSUKUMO_WEAPON1];
					object_copy(&o_list[new_o_idx], o_ptr);
					for (i = 1; i < m_max; i++)
					{
						monster_type *m_ptr = &m_list[i];
						if (m_ptr->r_idx == MON_TSUKUMO_WEAPON1 ) 
						{
							m_ptr->hold_o_idx = new_o_idx;
							break;
						}
					}
					if(i == m_max) msg_print("ERROR:*���͕t�^*�̓��삪��������");
					/*:::�V���ȕt�r�_�����A�C�e���������Ă��鈵���ɂ���*/
					o_list[new_o_idx].held_m_idx = i;
					o_list[new_o_idx].ix = 0;
					o_list[new_o_idx].iy = 0;
					o_list[new_o_idx].number = 1;

					/*:::�v���o����S�ē��đœ|���J�E���g�����Z�b�g*/
					lore_do_probe(MON_TSUKUMO_WEAPON1);
					r_ptr->r_sights = 0;
					r_ptr->r_deaths = 0;
					r_ptr->r_pkills = 0;
					r_ptr->r_akills = 0;
					r_ptr->r_tkills = 0;

					object_desc(m_name,&o_list[new_o_idx],OD_NAME_ONLY);

					msg_format("%s�͒��ɕ����A���Ȃ��̈ӂ̂܂܂ɓ������I",m_name);

					if (item >= 0)
					{
						inven_item_increase(item, -1);
						inven_item_describe(item);
						inven_item_optimize(item);
					}
					else
					{
						floor_item_increase(0 - item, -1);
						floor_item_describe(0 - item);
						floor_item_optimize(0 - item);
					}
				}
			}

		}
		break;

	case 26:
#ifdef JP
		if (name) return "�������͉�";
		if (desc) return "����E�h��ɂ�����ꂽ�����閂�͂����S�ɉ�������B";
#else
		if (name) return "Remove Enchantment";
		if (desc) return "Removes all magics completely from any weapon or armor.";
#endif
    
		{
			if (cast)
			{
				if (!mundane_spell(TRUE)) return NULL;
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "���̋Z";
		if (desc) return "�������̕����邢�͏㎿�̕���ɑ΂��A�i���I�ɉ��炩�̑����������t������B";
#else
		if (name) return "Brand Weapon";
		if (desc) return "Makes current weapon a random ego weapon.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(randint0(16));
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�����ւ̖Ɖu";
		if (desc) return "��莞�ԁA��C�A���A�d���A�_�̂����ꂩ�ɑ΂���Ɖu�𓾂�B";
#else
		if (name) return "Immunity";
		if (desc) return "Gives an immunity to fire, cold, electricity or acid for a while.";
#endif
    
		{
			int base = 13;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_ele_immune(base + randint1(base))) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�������j��";
		if (desc) return "�אڂ����������ɑ΂��ɂ߂ċ��͂ȍU�����s���B�A���f�b�h��f�[�����͑Ώۂɓ���Ȃ��B";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif
    
		{
			int dice = plev;
			int sides = 50;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				int y, x;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];
				m_ptr = &m_list[cave[y][x].m_idx];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int dam = damroll(dice,sides);
					int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_JUMP;
					char m_name[80];	
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);

					if(r_ptr->flags3 & RF3_NONLIVING)
					{
						msg_format("���Ȃ���%s�ɐG��A���������I",m_name);
						project(0, 0, y, x, dam, GF_DISINTEGRATE, flg, -1);
					}
					else
					{
						msg_format("���Ȃ���%s�ɐG�ꂽ���A�ǂ���疳�����ł͂Ȃ��悤���E�E",m_name);
					}
					touch_zap_player(m_ptr);
				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "*���͏[�U*";
		if (desc) return "���ݑ������Ă���S�Ă̑����i�̏[�U�𑁂߂�B�������e�̏[�U�̌��ʂ�1/10�ɂȂ�B";
#else
		if (name) return "Remove All Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			int base = plev * 2;
			if (info) return format("�[�U:%d�^�[����",base);
			if (cast)
			{
				int inven;
				bool flag = FALSE;
				for(inven = INVEN_RARM;inven < INVEN_TOTAL;inven++)
				{
					object_type *o_ptr;
					char desc[80];

					o_ptr = &inventory[inven];
					if(o_ptr->timeout)
					{
						flag = TRUE;
						if(o_ptr->tval == TV_GUN) base *= 100;
						o_ptr->timeout -= base;
						if(o_ptr->timeout < 1)
						{
							o_ptr->timeout = 0;
							object_desc(desc,o_ptr,OD_NAME_ONLY);
							msg_format("%s�͍ď[�U���ꂽ�I",desc);
							p_ptr->window |= (PW_EQUIP);
						}
					}
				}
				if(!flag)
				{
					msg_format("�[�U���K�v�Ȃ��̂�g�ɂ��Ă��Ȃ��B");
					return NULL;
				}
			}
		}
		break;
	case 31:
#ifdef JP
		if (name) return "���l��";
		if (desc) return "�����Z���Ԓ������œ����A�g�̔\�͂�����I�ɏ㏸����B";
#endif
    
		{
			int base = 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int percentage;
				int time = base + randint1(base);
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				set_tim_addstat(A_STR,110,time,FALSE);
				set_tim_addstat(A_DEX,110,time,FALSE);
				set_tim_addstat(A_CON,110,time,FALSE);
				set_lightspeed(time, FALSE);
				set_tim_speedster(time, FALSE);				
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= (PR_HP );
				redraw_stuff();
				msg_print("�X�[�p�[�}���ɂȂ����C������I");
			}
		}
		break;







	}

	return "";
}





static cptr do_new_spell_nature(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
	static const char s_rng[] = "�˒�";
#else
	static const char s_dam[] = "dam ";
	static const char s_rng[] = "rng ";
#endif

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�����X�^�[���m";
		if (desc) return "�߂��̑S�Ă̌����郂���X�^�[�����m����B";
#else
		if (name) return "Detect Creatures";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "���d";
		if (desc) return "�d���̒Z���r�[������B";
#else
		if (name) return "Lightning";
		if (desc) return "Fires a short beam of lightning.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;
			int range = plev / 6 + 2;

			if (info) return format("%s%dd%d %s%d", s_dam, dice, sides, s_rng, range);

			if (cast)
			{
				project_length = range;

				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "㩂Ɣ����m";
		if (desc) return "�߂��̑S�Ă�㩂Ɣ������m����B";
#else
		if (name) return "Detect Doors and Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�H�Ɛ���";
		if (desc) return "�H��������o���B";
#else
		if (name) return "Produce Food";
		if (desc) return "Produces a Ration of Food.";
#endif
    
		{
			if (cast)
			{
				object_type forge, *q_ptr = &forge;

#ifdef JP
				msg_print("�H���𐶐������B");
#else
				msg_print("A food ration is produced.");
#endif

				/* Create the food ration */
				///item �H�Ɛ���
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

				/* Drop the object from heaven */
				drop_near(q_ptr, -1, py, px);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "���̌�";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Daylight";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = (plev / 10) + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);

				if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
				{
#ifdef JP
					msg_print("���̌������Ȃ��̓��̂��ł������I");
#else
					msg_print("The daylight scorches your flesh!");
#endif

#ifdef JP
					take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "���̌�", -1);
#else
					take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "daylight", -1);
#endif
				}
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�����K��";
		if (desc) return "����1�̂𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Animal Taming";
		if (desc) return "Attempts to charm an animal.";
#endif
    
		{
			//v1.1.90 ����
			int power = plev + 25;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				charm_animal(dir, power);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���ւ̑ϐ�";
		if (desc) return "��莞�ԁA��C�A���A�d���ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Environment";
		if (desc) return "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���ƓŎ���";
		if (desc) return "�����S�������A�ł�̂��犮�S�Ɏ�菜���A�̗͂������񕜂�����B";
#else
		if (name) return "Cure Wounds & Poison";
		if (desc) return "Heals all cut and poison status. Heals HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(0);
				set_poisoned(0);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "��Ηn��";
		if (desc) return "�ǂ�n�����ď��ɂ���B";
#else
		if (name) return "Stone to Mud";
		if (desc) return "Turns one rock square to mud.";
#endif
    
		{
			int dice = 1;
			int sides = 30;
			int base = 20;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wall_to_mud(dir, 20 + randint1(30));
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�ϐ�";
		if (desc) return "�ꎞ�I�ɐ��ϐ��𓾂�B";
#else
		if (name) return "Frost Bolt";
		if (desc) return "Fires a bolt or beam of cold.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_water(randint1(base) + base, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "���z����";
		if (desc) return "��������B��������������X�^�[�Ɍ��ʂ�����B";
#else
		if (name) return "Ray of Sunlight";
		if (desc) return "Fires a beam of light which damages to light-sensitive monsters.";
#endif
    
		{
			int dice = 10;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				msg_print("���z���������ꂽ�B");
#else
				msg_print("A line of sunlight appears.");
#endif

				lite_line(dir, damroll(10, 8));
			}
		}
		break;

		/*:::������ߎd�l�ύX�@�p���[�㏸�����j�[�N�ɂ���������s��ǔ����Ȃǈꕔ�����X�^�[�Ɍ����ɂ���*/
	case 11:
#ifdef JP
		if (name) return "�������";
		if (desc) return "���E���̑S�Ẵ����X�^�[������������B��R�����Ɩ����B";
#else
		if (name) return "Entangle";
		if (desc) return "Attempts to slow all monsters in sight.";
#endif
    
		{
			int power = plev * 5 / 2 ;

			if (info) return info_power(power);

			if (cast)
			{
				msg_print("���Ȃ��͒ӂ�؂̍��𑀂����I");
				project_hack(GF_SLOW_TWINE, power);
			}
		}
		break;
	case 12:
#ifdef JP
		if (name) return "���R�̊o��";
		if (desc) return "�u�ԓI�Ɏ��R�ƈ�̉����A�߂��̒n�`�A㩁A�����X�^�[�����m����B";
#else
		if (name) return "Nature Awareness";
		if (desc) return "Maps nearby area. Detects all monsters, traps, doors and stairs.";
#endif
    
		{
			int rad1 = DETECT_RAD_MAP;
			int rad2 = DETECT_RAD_DEFAULT;

			if (info) return info_radius(MAX(rad1, rad2));

			if (cast)
			{
				map_area(rad1);
				detect_traps(rad2, TRUE);
				detect_doors(rad2);
				detect_stairs(rad2);
				detect_monsters_normal(rad2);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "���";
		if (desc) return "�w�肵���ʒu�ɐ^��̐n�������N�����B�h��͂̍����G�ɂ͓�����Ȃ����Ƃ�����B";
#else
		if (name) return "Fire Bolt";
		if (desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int dice = 4 + plev / 6;
			int sides = plev;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if(!fire_ball_jump(GF_WINDCUTTER, dir, damroll(dice, sides),3,NULL)) return NULL;
			}
		}
		break;
	case 14:
#ifdef JP
		if (name) return "�򑐎���";
		if (desc) return "��`�̖򑐂ɂ�鎡�Â��s���BHP�𒆒��x�񕜂����ēłƐ؂菝�������B";
#else
		if (name) return "Herbal Healing";
		if (desc) return "Heals HP greatly. And heals cut, stun and poison completely.";
#endif
    
		{
			int heal = plev * 5;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
			}
		}
		break;


	case 15:
#ifdef JP
		if (name) return "��������";
		if (desc) return "������1�̏�������B";
#else
		if (name) return "Summon Animal";
		if (desc) return "Summons an animal.";
#endif
    
		{
			if (cast)
			{
				if (!(summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, (PM_ALLOW_GROUP | PM_FORCE_PET))))
				{
#ifdef JP
					msg_print("�����͌���Ȃ������B");
#else
					msg_print("No animals arrive.");
#endif
				}
				break;
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "���Ή�";
		if (desc) return "��莞�ԁAAC���㏸������B";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif
    
		{
			int base = 20;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�V���I";
		if (desc) return "�G��̂ɑ΂��Ή���~�点�čU������B�X�̋߂����^�[�Q�b�g�Ƃ��Ė����I�Ɏw�肵���ꍇ�͖؂��|��Ă���u�V��|���v���N����З͂��オ��B";
#else
		if (name) return "Stair Building";
		if (desc) return "Creates a stair which goes down or up.";
#endif
    
		{
			int dice = 3 + plev / 4;
			int sides = 12;
			if (info) return info_damage(dice, sides, 0);
			if (cast)
			{
				bool wood = FALSE;
				int i,xx,yy;
				if (!get_aim_dir(&dir)) return NULL;
				if(dir == 5 && target_okay() && projectable(py,px,target_row,target_col))
				{
					if(cave_have_flag_bold((target_row), (target_col), FF_TREE)) wood = TRUE;
					for(i=0;i<8;i++)
					{
						yy = target_row + ddy_cdd[i];
						xx = target_col + ddx_cdd[i];
						if(!in_bounds(yy,xx)) continue;
						if(cave_have_flag_bold((yy), (xx), FF_TREE)) wood = TRUE;
					}
				}
				if(wood)
				{
					sides *= 2;
					if(!fire_ball_jump(GF_ARROW,dir,damroll(dice,sides),0,"�n�����ƂƂ��ɖ؂���{�|�ꂽ�I")) return NULL;
				}
				else
				{
					msg_print("���I����񂾁I");
					fire_bolt(GF_ARROW,dir,damroll(dice,sides));
				}

			}
		}
		break;


	case 18:
#ifdef JP
		if (name) return "�^�E�ϐ�";
		if (desc) return "��莞�ԁA�_�A�d���A���A��C�A�łɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resistance True";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�X�ёn��";
		if (desc) return "���͂ɖ؂����o���B";
#else
		if (name) return "Forest Creation";
		if (desc) return "Creates trees in all adjacent squares.";
#endif
    
		{
			if (cast)
			{
				tree_creation();
			}
		}
		break;
///mod151024 �����Z�a����g���ɕύX
/*
	case 20:
#ifdef JP
		if (name) return "�����F�a";
		if (desc) return "���E���̑S�Ă̓����𖣗�����B��R�����Ɩ����B";
#else
		if (name) return "Animal Friendship";
		if (desc) return "Attempts to charm all animals in sight.";
#endif
    
		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				charm_animals(power);
			}
		}
		break;
*/
	case 20:
#ifdef JP
		if (name) return "�h�g��";
		if (desc) return "�쐶�̑�h�ɃA�C�e��������܂ŉ^��ł��炤�B���������܂�d�����̂��^�΂��邱�Ƃ͂ł��Ȃ��B";
#else
		if (name) return "Animal Friendship";
		if (desc) return "Attempts to charm all animals in sight.";
#endif
    
		{
			int weight = plev * 2;

			if (info) return info_weight(weight);

			if (cast)
			{
				msg_print("��h���ǂ�����Ƃ��Ȃ����ł����B");
				item_recall(1);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "����";
		if (desc) return "���E���S�Ăɑ΂���C�U�����s���B";
#endif
    
		{
			int dam = 50 + plev;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				msg_print("������������ɕ���ꂽ�I");
				project_hack2(GF_COLD, 0,0,dam);

			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�΂̕�";
		if (desc) return "�����̎��͂ɉԛ���̕ǂ����B";
#else
		if (name) return "Wall of Stone";
		if (desc) return "Creates granite walls in all adjacent squares.";
#endif
    
		{
			if (cast)
			{
				wall_stone();
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "����";
		if (desc) return "���E���̃����_���ȓG�ɑ΂����͂ȗ��𗎂Ƃ��B";
#else
		if (name) return "Protect from Corrosion";
		if (desc) return "Makes an equipment acid-proof.";
#endif
    
		{
			int dam = plev*4 + 160;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{

				if(!fire_random_target(GF_ELEC, dam,4, rad,0))
				{
					msg_print("�����͂��͈͂ɂ͉������Ȃ��悤���B");
					return NULL;
				}
			}
		}
		break;
	case 24:
#ifdef JP
		if (name) return "�d������";
		if (desc) return "�F�D�I�ȗd�����ʂɏ�������B";
#endif
    
		{
			int num;
			if (info) return "";
			if (cast)
			{
				for(num = plev / 15 + 1;num > 0;num--)
				if (!(summon_specific(-1, py, px, plev, SUMMON_FAIRY, (PM_ALLOW_GROUP | PM_FORCE_FRIENDLY))))
				{
#ifdef JP
					msg_print("�d���͌���Ȃ������B");
#else
					msg_print("No animals arrive.");
#endif
				}
				break;
			}
		}
		break;
	case 25:
#ifdef JP
		if (name) return "�n�k";
		if (desc) return "���͂̃_���W������h�炵�A�ǂƏ��������_���ɓ���ς���B";
#else
		if (name) return "Earthquake";
		if (desc) return "Shakes dungeon structure, and results in random swapping of floors and walls.";
#endif
    
		{
			int rad = 10;

			if (info) return info_radius(rad);

			if (cast)
			{
				earthquake(py, px, rad);
			}
		}
		break;


	case 26:
#ifdef JP
		if (name) return "����";
		if (desc) return "����ȗ����𔭐�������B����ȓG����ł���G�ɂ͌��ʂ������B";
#endif
    
		{
			int dam = 100 + plev * 2;
			int rad = plev / 12 + 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				if(!fire_ball_jump(GF_TORNADO, dir, dam, rad,"�������N�������I")) return NULL;
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "����̐n";
		if (desc) return "�������̕����邢�͏㎿�̕���ɑ΂��A�i���I�ɉ��炩�̌��f������t������B";
#else
		if (name) return "Elemental Branding";
		if (desc) return "Makes current weapon fire or frost branded.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(randint0(5));
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�z������";
		if (desc) return "���E����M���ōU�����A����ɍL�͈͂̒n�`�ƃA�C�e���ƃg���b�v�ƃ����X�^�[�����m����B";
#else
		if (name) return "Call Sunlight";
		if (desc) return "Generates ball of light centered on you. Maps and lights whole dungeon level. Knows all objects location.";
#endif
    
		{
			int dam = 150;

			if (info) return info_damage(0, 0, dam/2);

			if (cast)
			{
				project_hack2(GF_LITE, 0, 0, dam);

				map_area(plev);
				detect_all(MIN(plev,50));

				if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
				{
#ifdef JP
					msg_print("���������Ȃ��̓��̂��ł������I");
#else
					msg_print("The sunlight scorches your flesh!");
#endif

#ifdef JP
					take_hit(DAMAGE_NOESCAPE, 50, "����", -1);
#else
					take_hit(DAMAGE_NOESCAPE, 50, "sunlight", -1);
#endif
				}
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "��������";
		if (desc) return "�������������������̐����ōU������B";
#endif
    
		{
			int dam = 200 + plev * 4;
			int rad = 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				cptr msg;
				if (!get_aim_dir(&dir)) return NULL;

				if(dun_level == 0) msg = "���z���̉J�������������I";
				else msg = "�n�������������������I";

				if(!fire_ball_jump(GF_WATER_FLOW, dir, 3, rad,msg)) return NULL;
				fire_ball_jump(GF_WATER, dir, dam, rad,NULL);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "���b����";
		if (desc) return "�i�فE�P���E�����E��T�̂����ꂩ����������B";
#endif
    
		{
			if (info) return "";
			if (cast)
			{
				if (summon_specific(-1, py, px, plev*2, SUMMON_HOLY_ANIMAL, (PM_FORCE_PET)))
				{
					msg_print("���Ȃ��̐��ɉ����ē`���̐��b�����ꂽ�I");
				}
				else
				{
					msg_print("���b�͌���Ȃ������B");
				}
				break;
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�V�ϒn��";
		if (desc) return "�����𒆐S�ɖ��́A�����A���A�d���A�Ή��̋���{�[�������ɔ��������A����ɒn�k���N��������n��ɂ���B";
#else
		if (name) return "Nature's Wrath";
		if (desc) return "Damages all monsters in sight. Makes quake. Generates disintegration ball centered on you.";
#endif
    
		{
			int rad = 6;

			if (cast)
			{

				project(0, rad, py, px, 450, GF_MANA, PROJECT_KILL | PROJECT_ITEM, -1);
				project(0, rad, py, px, 350, GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
				project(0, rad, py, px, 250, GF_WATER, PROJECT_KILL | PROJECT_ITEM, -1);
				project(0, rad, py, px, 200, GF_ELEC, PROJECT_KILL | PROJECT_ITEM, -1);
				project(0, rad, py, px, 150, GF_FIRE, PROJECT_KILL | PROJECT_ITEM, -1);
				earthquake(py, px, rad);
				project(0, rad, py, px, 8, GF_LAVA_FLOW, PROJECT_GRID, -1);

			}
		}
		break;
	}

	return "";
}

/*:::����̈�V�X�y��*/

static cptr do_new_spell_necromancy(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
	static const char s_random[] = "�����_��";
#else
	static const char s_dam[] = "dam ";
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{

	case 0:
#ifdef JP
		if (name) return "����";
		if (desc) return "�􂢂̎D���΂��čU������B���������Ȃ��҂ɂ͌��ʂ��Ȃ��B";
#else
		if (name) return "Malediction";
		if (desc) return "Fires a tiny ball of evil power which hurts good monsters greatly.";
#endif
    
		{
			int dice = 4 + plev / 5;
			int sides = 4;
			int rad = 0;

			if (info) return info_damage(dice, sides, 0);
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DISP_LIVING, dir, damroll(dice, sides), rad);

			}
		}
		break;
	case 1:
#ifdef JP
		if (name) return "���������m";
		if (desc) return "�A���f�b�h�E�f�[�����E�������Ȃǖ��������Ȃ��҂����m����B";
#else
		if (name) return "Detect Unlife";
		if (desc) return "Detects all nonliving monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_nonliving(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�A���f�b�h�ގU";
		if (desc) return "�A���f�b�h��̂����|������B";
#else
		if (name) return "Detect Evil";
		if (desc) return "Detects all evil monsters in your vicinity.";
#endif
    
		{
			int power = 25 + plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_TURN_UNDEAD, dir, power, 0);
			}
		}
		break;

	case 3:
#ifdef JP
		//v1.1.90 �Ö���������ɂ���sleep��stasis�n�ɂ���
		if (name) return "������";
		if (desc) return "1�̂̃����X�^�[�̓������~�߂�B��R�����Ɩ����B";
#else
		if (name) return "Black Sleep";
		if (desc) return "Attempts to sleep a monster.";
#endif
    
		{
			int power = plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				stasis_monster(dir);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�S��";
		if (desc) return "�Ή������̏����ȋʂ���B";
#else
		if (name) return "Stinking Cloud";
		if (desc) return "Fires a ball of poison.";
#endif
    
		{
			int dam = 15 + plev ;
			int rad = 1;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, dam, rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�ϒn��";
		if (desc) return "��莞�ԁA�n�������ւ̑ϐ��𓾂�B";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_nether(randint1(base) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�X�P���g������";
		if (desc) return "���̃����X�^�[��1�̏�������B";
#else
		if (name) return "Horrify";
		if (desc) return "Attempts to scare and stun a monster.";
#endif
    
		{
			int power = plev;


			if (cast)
			{
				int type = SUMMON_SKELETON;
				u32b mode = PM_FORCE_PET;
				if(plev > 34) mode |= PM_ALLOW_GROUP;
				if (summon_specific(-1, py, px, plev , type, mode))
				{
					msg_print("�n�ʂ���[���������o���A���Ȃ��ɏ]�����B");
				}
				else
				{
					msg_print("�n�ʂ������������C�����邪�A�����N����Ȃ������B");

				}
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�A���f�b�h�]��";
		if (desc) return "�A���f�b�h1�̂�z���ɂ���B���s���邱�Ƃ�����B";
#else
		if (name) return "Enslave Undead";
		if (desc) return "Attempts to charm an undead monster.";
#endif
    
		{
			int power = plev + 20;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				control_one_undead(dir, power);
			}
		}
		break;
	case 8:
#ifdef JP
		if (name) return "�n���̖�";
		if (desc) return "�n���̃{���g�������̓r�[������B�A���f�b�h�ɂ͂قڌ������f�[�����ɂ͌��ʂ������B";
#else
		if (name) return "Nether Bolt";
		if (desc) return "Fires a bolt or beam of nether.";
#endif
    
		{
			int dice = 8 + (plev - 5) / 4;
			int sides = 11;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance(), GF_NETHER, dir, damroll(dice, sides));
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "���쏢��";
		if (desc) return "�����̗H��n�����X�^�[���������z���ɂ���B";
#else
		if (name) return "Orb of Entropy";
		if (desc) return "Fires a ball which damages living monsters.";
#endif
    
		{
			int power = plev * 2 / 3 + 1;
			int num = randint1(3) + plev / 10;
			bool msgflag = FALSE;
			int i;


			if (cast)
			{
				int type = SUMMON_GHOST;
				u32b mode = PM_FORCE_PET;
				if(plev > 34) mode |= PM_ALLOW_GROUP;
				for(i=0;i<num;i++) if (summon_specific(-1, py, px, plev , type, mode)) msgflag=TRUE;

				if(msgflag)	msg_print("�H�삽��������A���Ȃ��ɏ]�����B");
				else msg_print("�����������������C�����邪�A�����N����Ȃ������B");
			}
		}
		break;


	case 10:
#ifdef JP
		if (name) return "�O��";
		if (desc) return "��莞�ԁA�������g�Ǝ��͂̓G�̃e���|�[�g��W�Q����B";
#else
		if (name) return "Cloud kill";
		if (desc) return "Generate a ball of poison centered on you.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_nennbaku(randint1(base) + base, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�c���v�O���";
		if (desc) return "�A�C�e���̑�܂��ȉ��l�𔻒肷��B���x��30�ȏ�Œʏ�̊Ӓ�Ɠ������ʂɂȂ�B";
#else
		if (name) return "Genocide One";
		if (desc) return "Attempts to vanish a monster.";
#endif
    
		{
			if (cast)
			{
				if (plev < 30)
				{
					if(!psychometry()) return NULL;
				}
				else
				{
					if (!ident_spell(FALSE)) return NULL;
				}
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "����߈�";
		if (desc) return "�����X�^�[��̂ɉ����߈˂����A���m���ŋ���m��������B��R�����Ɩ����B�d���Ɍ����₷�����ʏ�̐��_�������Ȃ������X�^�[�ɂ͌����Ȃ��B";
#else
		if (name) return "Poison Branding";
		if (desc) return "Makes current weapon poison branded.";
#endif
    
		{
			int power = plev * 2;
			int rad = 0;

			if (cp_ptr->magicmaster) power *= 2;

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POSSESSION, dir, power, rad);
			}
		}
		break;
	case 13:
#ifdef JP
		if (name) return "���ҏ���";
		if (desc) return "1�̂̃A���f�b�h���������z���ɂ���B�������ꂽ�A���f�b�h���G�΂��邱�Ƃ�����B";
#else
		if (name) return "Raise the Dead";
		if (desc) return "Summons an undead monster.";
#endif
    
		{
			if (cast)
			{
				int type;
				bool pet = TRUE;
				u32b mode = 0L;

				if(plev + adj_general[p_ptr->stat_ind[A_WIS]] < randint1(80)) pet = FALSE;

				type = SUMMON_UNDEAD;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

				if (summon_specific((pet ? -1 : 0), py, px, plev*5/4, type, mode))
				{
					if (pet)
					{
#ifdef JP
						msg_print("�n�ʂ��玀�҂������o���A���Ȃ��ɏ]�����B");
#else
						msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("�n�ʂ��玀�҂������o���A���Ȃ��ɏP���|�����Ă����I");
#else
						msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
					}
				}
			}
		}
		break;
	case 15:
#ifdef JP
		if (name) return "�g�t";
		if (desc) return "�t���A�ɂ���A���f�b�h�̔z�����^�[�Q�b�g���ӂɌĂъ񂹂�B";
#else
		if (name) return "Vampiric Drain";
		if (desc) return "Absorbs some HP from a monster and gives them to you. You will also gain nutritional sustenance from this.";
#endif
    
		{
			int num = 1 + plev / 7;

			if(num > 8) num = 8;

			if (info) return format("�ő�F%d��",num);

			if (cast)
			{
				int i;
				int cnt=0;
				int idx[8]; //�ő�8��
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					monster_race *r_ptr = &r_info[m_ptr->r_idx];
					if (!m_ptr->r_idx) continue;
					if (!is_pet(m_ptr) || !(r_ptr->flags3 & RF3_UNDEAD)) continue;
					/*:::�A���f�b�h�̃y�b�g��idx��z��Ɋi�[ �Ăяo����ȏ�̐��̔z�����������₩��K���ɓ���ւ���*/
					///���̂������Əo�Ă���z����������x�Œ艻����Ă��܂���������Ȃ����܂����p��債���e���͂Ȃ����낤�B
					if(cnt<num)idx[cnt++] = i;
					else idx[randint0(num)] = i;
				}
				if(cnt==0)
				{
					msg_format("���Ȃ��ɂ̓A���f�b�h�̔z�������Ȃ��B");
					return NULL;
				}
				if (!get_aim_dir(&dir)) return NULL;

				msg_format("�z���̃A���f�b�h���Ăяo�����I");
				for(i=0;i<cnt&&i<num;i++)
				{
					teleport_monster_to(idx[i], target_row, target_col, 100, TELEPORT_PASSIVE);
				}

			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "���͋z��";
		if (desc) return "�G��̂ɐ��_�U�����s��MP���z������B�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ������B";
#else
		if (name) return "Animate dead";
		if (desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif
    
		{
			int dice= 1;
			int sides= plev * 3;
			int base = plev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_PSI_DRAIN, dir, damroll(dice,sides)+base , 0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "���̌���";
		if (desc) return "�G��̂����͂Ȏ􂢂ōU������B��R�����Ɩ����B���������Ȃ��G�ɂ͌��ʂ��Ȃ��B";
#else
		if (name) return "Genocide";
		if (desc) return "Eliminates an entire class of monster, exhausting you.  Powerful or unique monsters may resist.";
#endif
    
		{
			int dice=15,sides=15;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_CAUSE_4, dir, damroll(dice, sides), 0);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "���_����";
		if (desc) return "��莞�ԁA�e���p�V�[�𓾂�B";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�A���f�b�h�x�z";
		if (desc) return "���E���̃A���f�b�h��z���ɂ��悤�Ǝ��݂�B��R�����Ɩ����B";
#else
		if (name) return "Invoke Spirits";
		if (desc) return "Causes random effects.";
#endif
    
		{
			int power = plev + 20;

			if (info) return info_power(power);

			if (cast)
			{
				charm_undead(power);
			}
		}
		break;


	case 19:
#ifdef JP
		if (name) return "���̌���";
		if (desc) return "���̌�������B";
#else
		if (name) return "Death Ray";
		if (desc) return "Fires a beam of death.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				death_ray(dir, plev);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "����̃I�[��";
		if (desc) return "��ʂ̎����g�ɂ܂Ƃ��B��莞��AC�㏸�ƒn���ϐ��ƌ��f�ϐ��𓾂āA����ɍU�����Ă����G��N�O�E�����E���|������B";
#else
		if (name) return "Vampirism True";
		if (desc) return "Fires 3 bolts. Each of the bolts absorbs some HP from a monster and gives them to you.";
#endif
    
		{
			int base = 20;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				int time = randint1(sides) + base;
				set_tim_sh_death(time,FALSE);
				set_oppose_acid(time, FALSE);
				set_oppose_elec(time, FALSE);
				set_oppose_fire(time, FALSE);
				set_oppose_cold(time, FALSE);
				set_oppose_pois(time, FALSE);
			}
		}
		break;


	case 21:
#ifdef JP
		if (name) return "��E�@�w";
		if (desc) return "���͂Ȏ􂢂̃G�l���M�[���y�􂳂���B���������Ȃ��҂ɂ͌��ʂ��Ȃ��B";
#else
		if (name) return "Nether Ball";
		if (desc) return "Fires a huge ball of nether.";
#endif
    
		{
			int dam = plev * 2 + 100;
			int sides = plev * 4;
			int rad = plev / 20 + 2;

			if (info) return info_damage(1, sides, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				if(!fire_ball_jump(GF_DISP_LIVING, dir, dam + randint1(sides), rad,NULL)) return NULL;
			}
		}
		break;


	case 22:
#ifdef JP
		if (name) return "�z���S�ω�";
		if (desc) return "��莞�ԁA�z���S�ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����ɋz���S�Ƃ��Ă̔\�͂𓾂�B";
#else
		if (name) return "Polymorph Vampire";
		if (desc) return "Mimic a vampire for a while. Loses abilities of original race and gets abilities as a vampire.";
#endif
    
		{
			int base = 10 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_VAMPIRE, FALSE);
			}
		}
		break;
	case 23:
#ifdef JP
		if (name) return "��ʃA���f�b�h����";
		if (desc) return "���͂ȃA���f�b�h�𕡐��������z���ɂ���B�������ꂽ�A���f�b�h���G�΂��邱�Ƃ�����B";
#else
		if (name) return "Darkness Storm";
		if (desc) return "Fires a huge ball of darkness.";
#endif
    
		{
			if (cast)
			{
				int type;
				bool pet =TRUE;
				u32b mode = 0L;
				int num,i;
				bool msgflag=FALSE;

				if(plev + adj_general[p_ptr->stat_ind[A_WIS]] < randint1(120)) pet = FALSE;

				type = SUMMON_HI_UNDEAD;

				mode |= PM_ALLOW_GROUP;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);
				num = 1 + plev / 45 + randint0(1);

				for(i=0;i<num;i++) if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, type, mode) && !msgflag)
				{
					msgflag = TRUE;
#ifdef JP
					msg_print("�₽���������Ȃ��̎���ɐ����n�߂��B����͕��s�L���^��ł���...");
#else
					msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("�Â��̎�����ҋ������Ȃ��Ɏd���邽�ߓy�����S�����I");
#else
						msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("���҂��S�����B�����W���邠�Ȃ��𔱂��邽�߂ɁI");
#else
						msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
					}
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�r������";
		if (desc) return "�h���S���̃A���f�b�h����������B�G�}�Ƃ��ė��̗؂Ȃǂ̕��i�������B";
#else
		if (name) return "Death Ray";
		if (desc) return "Fires a beam of death.";
#endif
    
		{
			u32b summon_mode = 0L;
			int lev = plev ;
			if (info) return format("");
			if (cast)
			{
				int num = 1;
				int cost;

				/*:::���s���͕�V������Ȃ�*/
				if(cast)
				{
					item_tester_hook = item_tester_hook_dragonkind;
					summon_mode |= PM_FORCE_PET;
					if(!select_pay(&cost)) return NULL;

					if(cost <= 0) //�����l�ȃA�C�e�����g���Ǝ��s����
					{
						summon_mode &= ~(PM_FORCE_PET);
						summon_mode |= PM_NO_PET;
					}
					lev += cost / 2000;
					if(lev > 100) lev = 100;
				}

				if (new_summoning(num, py, px, lev, SUMMON_UNDEAD_DRAGON, summon_mode))
				{
					if(summon_mode & PM_NO_PET) msg_print("�G�}�����������炵���B�����P���������Ă����I");
					else     msg_print("�n�̒ꂩ�狿���悤�ȚX�萺�����������E�E");
				}
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�j�ł̎�";
		if (desc) return "�j�ł̎����B�H����������X�^�[�͂��̂Ƃ���HP�̔����O��̃_���[�W���󂯂�B";
#else
		if (name) return "Doom Hand";
		if (desc) return "Attempts to make a monster's HP almost half.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				else msg_print("<�j�ł̎�>��������I");
#else
				else msg_print("You invoke the Hand of Doom!");
#endif

				fire_ball_hide(GF_HAND_DOOM, dir, plev * 2, 0);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�n���̗�";
		if (desc) return "����Ȓn�������̋�����B";
#else
		if (name) return "Nether Storm";
		if (desc) return "Generate a huge ball of nether.";
#endif
    
		{
			int dam = plev * 9;
			int rad = plev / 7;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_NETHER, dir, dam, rad);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "���Q�̌_��";
		if (desc) return "��莞�ԁA�������_���[�W���󂯂��Ƃ��ɍU�����s���������X�^�[�ɑ΂��ē����̃_���[�W��^����B";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 10;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_eyeeye(randint1(base) + base, FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�\�E���X�e�B�[��";
		if (desc) return "�����̂���G���ꌂ�œ|���A����HP���z������B��R�����Ɩ����B�͋����G�⌫���G�ɂ͌����ɂ����B";
#else
		if (name) return "Bloody Curse";
		if (desc) return "Puts blood curse which damages and causes various effects on a monster. You also take damage.";
#endif
    
		{
			int power = plev * 7 / 2;
			int rad = 0;

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_SOULSTEAL, dir, power, rad);
			}
		}
		break;


	case 29:
#ifdef JP
		if (name) return "�H�̉�";
		if (desc) return "��莞�ԁA�ǂ�ʂ蔲���邱�Ƃ��ł��󂯂�_���[�W���y�������H�̂̏�Ԃɕϐg����B";
#else
		if (name) return "Wraithform";
		if (desc) return "Becomes wraith form which gives ability to pass walls and makes all damages half.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_wraith_form(randint1(base) + base, FALSE);
			}
		}
		break;



	case 30:
#ifdef JP
		if (name) return "���̎�";
		if (desc) return "�ɂ߂ċ��͂Ȏ􂢂ɂ��G1�̂ɑ�_���[�W��^���A���X���炩�̒ǉ����ʂ������N�����B�����̂Ȃ��G�ɂ����ʂ�����B30�`100��HP������A�̗͂������Ă���قǈЗ͂����܂�B";
#else
		if (name) return "Bloody Curse";
		if (desc) return "Puts blood curse which damages and causes various effects on a monster. You also take damage.";
#endif
    
		{
			int base = plev * 10;
			int sides = plev * 10;
			int rad = 0;
			int mult = 50 + 300 * (p_ptr->mhp - p_ptr->chp) / p_ptr->mhp;
			base = base * mult / 100;
			sides = sides * mult / 100;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				int dam_add = randint1(sides);
				int uselife = 30 + 70 * dam_add / sides;

				if(uselife > 100) uselife = 100;

				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_BLOOD_CURSE, dir, base + dam_add , rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, uselife, "���̎�", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "Blood curse", -1);
#endif
			}
		}
		break;




	case 31:
#ifdef JP
		if (name) return "�H���̋S�_";
		if (desc) return "���͂ȃA���f�b�h�ł��郊�b�`�ɓ]������B���̕ω��͉i���I�Ȃ��̂ł���B�ꕔ�̎푰��N���X�ł͎g�p�ł��Ȃ��B���̏ꍇ�X�R�A���啝�ɉ�����B�l���ɂ��Ȃ��Ǝg���Ȃ��B";
#else
		if (name) return "Wraithform";
		if (desc) return "Becomes wraith form which gives ability to pass walls and makes all damages half.";
#endif
    
		{

			if (cast)
			{
				if(p_ptr->prace == RACE_LICH)
				{
					msg_print("���Ȃ��͂��łɎ��̌������B");
					return NULL;
				}
				if(RACE_NEVER_CHANGE)
				{
					msg_print("���Ȃ��͓̑̂]���Ƃ����s�ׂɓK���Ȃ��悤���B");
					return NULL;
				}
				if(cp_ptr->flag_only_unique)
				{
					msg_print("�c�c���ȗ\��������B����ς��߂Ă������B");
					return NULL;
				}
				if(dun_level || p_ptr->town_num != TOWN_HITOZATO)
				{
					msg_print("�����Ŕ�p������s���̂͊댯���B");
					return NULL;
				}
				if (!get_check("�]�����s���܂��B��낵���ł����H")) return NULL;
				
				msg_print("���Ȃ��͏����������Ԃ�A���������ވ��̍݌ɂɕ��ꍞ�܂����E�E");
				world_monster = -1;
				do_cmd_redraw();
				Term_xtra(TERM_XTRA_DELAY, 3000);
				teleport_player_to(34,81,TELEPORT_NONMAGICAL);
				world_monster = 0;
				p_ptr->redraw |= PR_MAP;
				redraw_stuff();	
				change_race(RACE_LICH,"");
			}
		}
		break;
	}

	return "";
}




static cptr do_new_spell_life(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�y���̎���";
		if (desc) return "����Ƒ̗͂������񕜂�����B";
#else
		if (name) return "Cure Light Wounds";
		if (desc) return "Heals cut and HP a little.";
#endif
    
		{
			int dice = 2;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_cut(p_ptr->cut - 10);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�j��";
		if (desc) return "��莞�ԁA��������AC�Ɩ��@�h��͂Ƀ{�[�i�X�𓾂�B";
#else
		if (name) return "Bless";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "���";
		if (desc) return "�̓��̓ł���菜���B";
#else
		if (name) return "Cure Poison";
		if (desc) return "Cure poison status.";
#endif
    
		{
			if (cast)
			{
				set_poisoned(0);
			}
		}
		break;


	case 3:
#ifdef JP
		if (name) return "���̏���";
		if (desc) return "�������Ƃ炵�Ă���͈͂������S�̂��i�v�ɖ��邭����B";
#else
		if (name) return "Call Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = plev / 2;
			int rad = plev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�����̊��m";
		if (desc) return "�߂��̖����������X�^�[�����m����B";
#else
		if (name) return "Detect Doors & Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_living(rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�񕜗͋���";
		if (desc) return "��莞�ԁA�񕜗͂����������B";
#else
		if (name) return "Regeneration";
		if (desc) return "Gives regeneration ability for a while.";
#endif
    
		{
			int base = 80;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_regen(base + randint1(base), FALSE);
			}
		}
		break;


	case 6:
#ifdef JP
		if (name) return "�󕠏[��";
		if (desc) return "�����ɂ���B";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif
    
		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "����";
		if (desc) return "�̗͂�啝�ɉ񕜂����A�����ƞN�O��Ԃ��S������B";
#else
		if (name) return "Cure Critical Wounds";
		if (desc) return "Heals cut, stun and HP greatly.";
#endif
    
		{
			int dice = 3 + plev / 3;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "����";
		if (desc) return "�A�C�e���ɂ��������ア�􂢂���������B";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�ϔM�ϊ�";
		if (desc) return "��莞�ԁA�Ή��Ɨ�C�ɑ΂���ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Heat and Cold";
		if (desc) return "Gives resistance to fire and cold. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�^�[���A���f�b�h";
		if (desc) return "���E���̃A���f�b�h�����|������B��R�����Ɩ����B";
#else
		if (name) return "Turn Undead";
		if (desc) return "Attempts to scare undead monsters in sight.";
#endif
    
		{
			if (cast)
			{
				turn_undead();
			}
		}
		break;


	case 11:
#ifdef JP
		if (name) return "�ϒn��";
		if (desc) return "��莞�ԁA�n���ւ̑ϐ��𓾂�B";
#else
		if (name) return "Resist Nether";
		if (desc) return "Gives resistance to nether for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_nether(randint1(base) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�ϓ�";
		if (desc) return "��莞�ԁA�łւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�ӎ�";
		if (desc) return "�A�C�e�������ʂ���B";
#else
		if (name) return "Perception";
		if (desc) return "Identifies an item.";
#endif
    
		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�̗͉�";
		if (desc) return "�ɂ߂ċ��͂ȉ񕜎����ŁA�����ƞN�O��Ԃ��S������B";
#else
		if (name) return "Healing";
		if (desc) return "Much powerful healing magic, and heals cut and stun completely.";
#endif
    
		{
			int heal = 300;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���E�̖��";
		if (desc) return "�����̂��鏰�̏�ɁA�����X�^�[���ʂ蔲�����菢�����ꂽ�肷�邱�Ƃ��ł��Ȃ��Ȃ郋�[����`���B";
#else
		if (name) return "Glyph of Warding";
		if (desc) return "Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.";
#endif
    
		{
			if (cast)
			{
				warding_glyph();
			}
		}
		break;


	case 16:
#ifdef JP
		if (name) return "���_����";
		if (desc) return "��莞�ԁA�e���p�V�[�𓾂�B";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;


	case 17:
#ifdef JP
		if (name) return "���̋���";
		if (desc) return "��莞�ԁA�r�́E��p���E�ϋv�͂��㏸������B�ʏ�̌��E�l�𒴂���B";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 10 + p_ptr->lev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = base + randint1(base);
				set_tim_addstat(A_STR,100 + 1 + p_ptr->lev / 15,time,FALSE);
				set_tim_addstat(A_DEX,100 + 1 + p_ptr->lev / 15,time,FALSE);
				set_tim_addstat(A_CON,100 + 1 + p_ptr->lev / 15,time,FALSE);
				msg_print("���̂��}���Ɋ������������I");

			}
		}
		break;



	case 18:
#ifdef JP
		if (name) return "*����*";
		if (desc) return "�A�C�e���ɂ����������͂Ȏ􂢂���������B";
#else
		if (name) return "Dispel Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("�N���Ɍ�����Ă���悤�ȋC������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;



	case 19:
#ifdef JP
		if (name) return "�f�B�X�y��";
		if (desc) return "�A���f�b�h��̂𒆊m���ňꌂ�œ|���B��R�����Ɩ����B�^�[�Q�b�g���߂��قǌ����₷���B";
#else
		if (name) return "Dispel Undead";
		if (desc) return "Damages all undead monsters in sight.";
#endif
    
		{
			if (cast)
			{
				int y, x;
				int dist;
				int damage;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_aim_dir(&dir)) return NULL;

				y = target_row;
				x = target_col;
				m_ptr = &m_list[target_who];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int power;
					char m_name[80];	
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);
					power = p_ptr->lev * 3 / 2 - m_ptr->cdis * 4;
					if(power<1) power=1;

					///mod140324 �{�Ƃɕ����5%�̐����ۏ؂�K�p����B����قǓK�����߂��Ƃ��ĉ������X�Ƃ����C�����邪�B
					msg_format("�V��������~�蒍�����I");
					if(r_ptr->flags3 & RF3_UNDEAD && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) && ( power / 2 + randint1( power / 2 ) > r_ptr->level || one_in_(20)))
					{
						msg_format("%s�͌��̒��ɏ������B",m_name);
						damage = m_ptr->hp + 1;
						project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_DISP_ALL,PROJECT_KILL,-1);
					}
					else if(r_ptr->flags3 & RF3_UNDEAD)
					{
						msg_format("%s�͉���ɒ�R�����I",m_name);
					}
					else 
					{
						msg_format("%s�ɂ͑S�������Ă��Ȃ��悤���B",m_name);
					}
				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "���Ҏ���";
		if (desc) return "�����̂���^�[�Q�b�g1�̂����Â���B�^�[�Q�b�g�̍ő�HP��10%��200�̑����ق��̐��l���񕜂����N�O�������B";
#else
		if (name) return "Day of the Dove";
		if (desc) return "Attempts to charm all monsters in sight.";
#endif
    
		{
			if (cast)
			{
				int y, x;
				int dist;
				int damage;
				int target_m_idx;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_aim_dir(&dir)) return NULL;
				if(dir == 5 && target_okay())
				{
					y = target_row;
					x = target_col;
					target_m_idx = cave[y][x].m_idx;
				}
				else
				{
					msg_format("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
					return NULL;
				}
				//m_ptr = &m_list[target_who];
				m_ptr = &m_list[target_m_idx];

				if (!los(py,px,m_ptr->fy,m_ptr->fx) )
				{
					msg_format("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
					return NULL;
				}

				else if (m_ptr->r_idx && (m_ptr->ml))
				{
					int heal;
					char m_name[80];	
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);
					heal = m_ptr->maxhp / 10;
					if(heal < 200) heal = 200;
					if(monster_living(r_ptr))
					{
						m_ptr->hp += heal;
						if(m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
						set_monster_stunned(cave[y][x].m_idx,0);
						msg_format("%^s�̏������Â����B", m_name);
						if (p_ptr->health_who == target_m_idx) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == target_m_idx) p_ptr->redraw |= (PR_UHEALTH);
					}
					else
						msg_format("%^s�̏������Â��邱�Ƃ͂ł��Ȃ������B", m_name);

				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}


			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�ύ���";
		if (desc) return "��莞�ԁA���ב����U���ɑ΂���ϐ��𓾂�B";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_chaos(randint1(base) + base, FALSE);
			}
		}
		break;





	case 22:
#ifdef JP
		if (name) return "*�ӎ�*";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if (name) return "Holy Vision";
		if (desc) return "*Identifies* an item.";
#endif
    
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;


	case 23:
#ifdef JP
		if (name) return "���_�h��";
		if (desc) return "��莞�ԁA���|�Ƌ��C�ւ̑ϐ��A���@�h��㏸�𓾂�B";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = randint1(base) + base;
				set_afraid(0);
				set_stun(0);
				set_hero(time,FALSE);
				set_resist_magic(time, FALSE);
				set_tim_res_insanity(time, FALSE);

			}
		}
		break;





	case 24:
#ifdef JP
		if (name) return "���A�N�e�B�u�q�[��";
		if (desc) return "��莞�ԁA�_���[�W���󂯂��Ƃ������I�ɏ��ʂ�HP���񕜂���B�񕜎��Ɍ��ʎ��Ԃ���������B";
#else
		if (name) return "Sterilization";
		if (desc) return "Prevents any breeders on current level from breeding.";
#endif
    
		{
			int base = 25 + plev/2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_reactive_heal(randint1(base) + base, FALSE);
			}
		}
		break;


	case 25:
#ifdef JP
		if (name) return "�A���f�b�h����";
		if (desc) return "�����̎��͂ɂ���A���f�b�h�����݂̊K�����������B��R�����Ɩ����B";
#else
		if (name) return "Annihilate Undead";
		if (desc) return "Eliminates all nearby undead monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif
    
		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide_undead(power, TRUE);
			}
		}
		break;





	case 26:
#ifdef JP
		if (name) return "�S����";
		if (desc) return "���ׂẴX�e�[�^�X�ƌo���l���񕜂���B";
#else
		if (name) return "Restoration";
		if (desc) return "Restores all stats and experience.";
#endif
    
		{
			if (cast)
			{
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				restore_level();
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "���C�t�X�g���[��";
		if (desc) return "���E���̔z�������X�^�[��HP��啝�ɉ񕜂�����B";
#else
		if (name) return "Restoration";
		if (desc) return "Restores all stats and experience.";
#endif
    
		{
			if (cast)
			{
				int i,heal,x,y;
				bool count = FALSE;
				msg_format("�����̃G�l���M�[���ӂ�ɖ�����ꂽ�I");
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					monster_race *r_ptr;
					char m_name[80];	
					r_ptr = &r_info[m_ptr->r_idx];
					if (!m_ptr->r_idx) continue;
					y = m_ptr->fy;
					x = m_ptr->fx;
					if (!player_has_los_bold(y, x) || !projectable(py, px, y, x)) continue;
					if(!is_pet(m_ptr)) continue;
					if(!monster_living(r_ptr))continue;
					monster_desc(m_name, m_ptr, 0);
					r_ptr = &r_info[m_ptr->r_idx];
					heal = m_ptr->maxhp / 5;
					if(heal < 500) heal = 500;
					count = TRUE;
					m_ptr->hp += heal;
					if(m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
					set_monster_stunned(cave[y][x].m_idx,0);
					msg_format("%s�̗͑͂��񕜂����I",m_name);
				}
				if(!count) msg_format("���������������ׂ��z�����߂��ɂ��Ȃ������E�E");

			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�^�E���E";
		if (desc) return "�����̂��鏰�Ǝ���8�}�X�̏��̏�ɁA�����X�^�[���ʂ蔲�����菢�����ꂽ�肷�邱�Ƃ��ł��Ȃ��Ȃ郋�[����`���B";
#else
		if (name) return "Warding True";
		if (desc) return "Creates glyphs in all adjacent squares and under you.";
#endif
    
		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				warding_glyph();
				glyph_creation();
			}
		}
		break;


	case 29:
#ifdef JP
		if (name) return "*�̗͉�*";
		if (desc) return "�ŋ��̎����̖��@�ŁA�����ƞN�O��Ԃ��S������B";
#else
		if (name) return "Healing True";
		if (desc) return "The greatest healing magic. Heals all HP, cut and stun.";
#endif
    
		{
			int heal = 2000;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;


	case 30:
#ifdef JP
		if (name) return "�V��";
		if (desc) return "�S�Ă̓ˑR�ψق������AHP�ő�l���Čv�Z����B";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			if (cast)
			{
			do_cmd_rerate(FALSE);
			get_max_stats();
			p_ptr->update |= PU_BONUS;
			p_ptr->bydo = 0;
			set_alcohol(0);
			set_asthma(0);

			if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3 || p_ptr->muta4)
			{
				///del131214 virtue
				//chg_virtue(V_CHANCE, -5);
				if(muta_erasable_count()) msg_print("�S�Ă̓ˑR�ψق��������B");

				p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = p_ptr->muta4 = 0;
				///mod140324 �V���̖�ł������^�ψق͏����Ȃ�����
				p_ptr->muta1 = p_ptr->muta1_perma;
				p_ptr->muta2 = p_ptr->muta2_perma;
				p_ptr->muta3 = p_ptr->muta3_perma;
				p_ptr->muta4 = p_ptr->muta4_perma;

				p_ptr->update |= PU_BONUS;
				handle_stuff();
				mutant_regenerate_mod = calc_mutant_regenerate_mod();
			}
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "���ɂ̑ϐ�";
		if (desc) return "��莞�ԁA������ϐ���t���AAC�Ɩ��@�h��\�͂��㏸������B";
#else
		if (name) return "Ultimate Resistance";
		if (desc) return "Gives ultimate resistance, bonus to AC and speed.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int v = randint1(base) + base;
				set_fast(v, FALSE);
				set_oppose_acid(v, FALSE);
				set_oppose_elec(v, FALSE);
				set_oppose_fire(v, FALSE);
				set_oppose_cold(v, FALSE);
				set_oppose_pois(v, FALSE);
				set_ultimate_res(v, FALSE);
			}
		}
		break;
	}

	return "";
}







/*:::�V�̈�@�ϗe*/
static cptr do_new_spell_transform(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;
	int dice,sides,base,damage;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "����";
		if (desc) return "�����镔���𖾂邭�Ƃ炷�B";
#endif
    
		{
			if (cast)
			{
				switch(randint1(100))
				{
				case 1:	msg_print("���Ȃ��̗��ڂ͎��F�ɋP�����I");break;
				case 2:	msg_print("���Ȃ��̊z�͂܂΂䂭�������I");break;
				case 3:	msg_print("���Ȃ��̂��Ȃ���ῂ�����������I");break;
				case 4:	msg_print("���Ȃ��̂��̎肪�����ĚX��I");break;
				case 5:	msg_print("���Ȃ��̔��͗d�����C���~�l�[�V���������I");break;
				case 6:	msg_print("���Ȃ��̂��K���u�̂悤�Ɍ������I");break;
				case 7:	msg_print("���Ȃ��̉E���������߂����I");break;
				case 8:	msg_print("���Ȃ��̕@���^���ԂɃs�J�s�J�����ĈÂ��铹���Ƃ炵���I");break;
				default:	msg_print("���Ȃ��̑̂͌���n�߂��B");break;
				}
				lite_area(0, 0);


			}
		}
		break;
	case 1:
#ifdef JP
		if (name) return "�ؓ�����";
		if (desc) return "��莞�ԁA�r�͂�4�|�C���g����������B������̒l��40�𒴂��Ȃ��B";
#else
		if (name) return "Resist Fire";
		if (desc) return "Gives resistance to fire. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_format("�ؓ����c�������B");
				set_tim_addstat(A_STR,4,base + randint1(base),FALSE);
			}
		}
		break;



	case 2:
#ifdef JP
		if (name) return "�b�̎�";
		if (desc) return "�߂��̃����X�^�[�����m����B���x�����オ��Ɗ��m�͈͂��L����B";
#else
		if (name) return "Regeneration";
		if (desc) return "Gives regeneration ability for a while.";
#endif
    
		{
			int rad = plev / 2 + 10;

			if (info) return format("�͈́F%d",rad);

			if (cast)
			{
				if(one_in_(10))
					msg_print("���Ȃ��̓��ɔL���������ăs�N�s�N�������B");
				else 
					msg_print("���Ȃ��͒��o�����������𐟂܂����E�E");
				(void)detect_monsters_normal(rad);
				(void)detect_monsters_invis(rad);
			}
		}
		break;


	case 3:
#ifdef JP
		if (name) return "���͓���";
		if (desc) return "�A�C�e�������͂ɓ�������B�������͒ʏ�̓����Ɠ��l�Ɍv�Z�����B";
#endif
    
		{
			int mult = 2 + plev / 15;

			if (info) return format("�{���F%d",mult);

			if (cast)
			{

				msg_print("�r�ƌ��̋ؓ�������オ�����I");
				if (!do_cmd_throw_aux(mult, FALSE, -1)) return NULL;

			}
		}
		break;
	case 4:
#ifdef JP
		if (name) return "�畆�ϗe";
		if (desc) return "�ꎞ�I�ɉ΂ɑ΂���ϐ��𓾂�B���x�����㏸����Ǝ_�Ɨ�C�ɑ΂���ϐ���������B";
#else
		if (name) return "Trap / Door Destruction";
		if (desc) return "Destroys all traps in adjacent squares.";
#endif
    
		{
			int base = 15 + plev / 3;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				msg_print("�畆�g�D��ϐ��������E�E");
				set_oppose_fire(randint1(base) + base, FALSE);
				if(plev > 19) set_oppose_acid(randint1(base) + base, FALSE);			
				if(plev > 29) set_oppose_cold(randint1(base) + base, FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "���̖ѐj";
		if (desc) return "�^�[�Q�b�g���ӂɌ����A�����̒�З͂ȃ{���g����B";
#else
		if (name) return "Heroism";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{

			int dice = 3 + (plev - 1) / 9;
			int sides = 2;

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_blast(GF_ARROW, dir, dice, sides, 10, 3, 0);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "������";
		if (desc) return "�w���痃�𐶂₵�A��莞�Ԕ�s����B";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print("�w���痃���������I");
				set_tim_levitation(randint1(base) + base, FALSE);
			}
		}
		break;

	case 7:

		if (name) return "�Ζ����o";
		if (desc) return "�^�[�Q�b�g���ӂ̒n�`���u�Ζ��v�ɂ���B�܂��͈͓��̃����X�^�[�̍U���͂�ቺ������B�Ζ��n�`��ł͉Ή��n���̃_���[�W���㏸����B";
		{
			int dam = plev * 3;
			int rad = 1 + plev / 20;

			if (cp_ptr->magicmaster) dam *= 2;
			if (info) return info_power(dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if (!fire_ball_jump(GF_DIG_OIL, dir, dam, rad, "�Ζ��������o�����I")) return NULL;
			}
		}
		break;
		//v1.1.94 ���������Ζ����o

		/*
#ifdef JP
		if (name) return "������";
		if (desc) return "�w��^�[�Q�b�g������������B��R�����Ɩ����B���ł���G�Ȃǈꕔ�̓G�ɂ͌��ʂ������B";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int power = plev * 3 ;
			int rad = 0 + plev / 20;

			if (info) return info_power(power);

			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;
				msg_print("�n�ʂ��h�ꓮ�����˂����B");				
				fire_ball_hide(GF_SLOW_TWINE, dir, power, rad);
			}

		}
		break;
		*/


	case 8:
#ifdef JP
		if (name) return "�z��ϗe";

		if (desc) return "��莞�ԉ񕜗͂���������A����ɓłɑ΂���ϐ��𓾂�B";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			int base = 10 + plev / 2;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				int time = base + randint1(base);
				set_tim_regen(time, FALSE);
				set_oppose_pois(time, FALSE);
			}
		}
		break;
	case 9:
#ifdef JP
		if (name) return "�ڂ���r�[��";
		if (desc) return "�ڂ���r�[������B�r�[���̑����͎푰�ɂ��ς��B";
#else
		if (name) return "Resist Lightning";
		if (desc) return "Gives resistance to electricity. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{

			int dice = 10 + plev  / 5;
			int sides = 7;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				int typ;
				switch(p_ptr->prace)
				{
				case RACE_VAMPIRE:
				case RACE_KAPPA:
				case RACE_NINGYO:
					typ = GF_WATER;break;
				case RACE_YOUKAI:
				case RACE_DAIYOUKAI:
				case RACE_IMP:
					typ = GF_DARK;break;
				case RACE_MAGICIAN:
				case RACE_DEITY:
					typ = GF_MANA;break;
				case RACE_DEATH:
				case RACE_ZOMBIE:
				case RACE_SPECTRE:
				case RACE_LICH:
				case RACE_ANIMAL_GHOST:
					typ = GF_NETHER;break;
				case RACE_ULTIMATE:
				case RACE_FAIRY:
					typ = GF_HOLY_FIRE;break;
				case RACE_NINJA:
				case RACE_ONI:
					typ = GF_FORCE;break;
				case RACE_HOURAI:
					typ = GF_NUKE;break;
				default:
					typ = GF_LITE;break;

				}

				if (!get_aim_dir(&dir)) return NULL;
				 msg_print("�ڂ���r�[����������I");
				fire_beam(typ, dir, damroll(dice, sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�h�A����";
		if (desc) return "�����̎���̏�������h�A�ɕς���B";
#else
		if (name) return "Resist Acid";
		if (desc) return "Gives resistance to acid. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;
			//���x���Ŕ��a������悤�ɂ��悤�Ƃ�����������ƕ֗�������̂Ŏ��d
			//int rad = 1 + plev / 20;

			if (cast)
			{
				door_creation(1);
			}
		}
		break;
	case 11:
#ifdef JP
		if (name) return "�����Đ�";
		if (desc) return "���𖳗���ǂ��Ŏ��Â���BHP��啝�ɉ񕜂��؂菝�������������x����������B";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int heal = plev * 4;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				if(p_ptr->chp < p_ptr->mhp) msg_print("�̂̏��������ɍǂ��ł��������I");
				hp_player(heal);
				set_cut(0);
				set_food(p_ptr->food - 1000);
			}
		}
		break;



	case 12:
#ifdef JP
		if (name) return "�܋���";
		if (desc) return "����̒܂�����Ȑn�ɕω�������BAC��30�㏸�����͂Ȋi���U�����ł���悤�ɂȂ邪���@�̎��s�����㏸����B�܂��E�r�����A���r�����A�O���[�u�͈ꎞ�I�ɖ����������B";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			int base = 30;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				if(p_ptr->riding && !CLASS_RIDING_BACKDANCE)
				{
					msg_print("�n��ł͂��̖��@���g���Ȃ��B");
					return NULL;
				}
				set_clawextend(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�u���X";
		if (desc) return "����HP��1/3�̈З͂̌��f�u���X��f���B�u���X�̑����͐��i�ɂ��ς��B";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif
    
		{
			//�O�̂��ߎ��ۂ̐��i�̐���葽�����Ă���
			int gf_table[] = {GF_FIRE,GF_FIRE,GF_COLD,GF_ACID, GF_ELEC,GF_ACID,GF_FIRE,GF_POIS,GF_ELEC,GF_ACID, GF_COLD, GF_ACID, GF_FIRE, GF_FIRE, GF_FIRE, GF_FIRE, GF_FIRE, GF_FIRE};
			int dam = p_ptr->chp / 3;
			int rad = -2 - plev / 30;

			if(rad < -3) rad = -3;
			if(dam > 1600) dam = 1600;

			if(dam<1) dam=1;
			if (info) return info_damage(0, 0, dam);
			

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP
				
				
				if (gf_table[p_ptr->pseikaku] == GF_FIRE) msg_print("���Ȃ��͉Ή��̃u���X��f�����I");
				if (gf_table[p_ptr->pseikaku] == GF_COLD) msg_print("���Ȃ��͗�C�̃u���X��f�����I");
				if (gf_table[p_ptr->pseikaku] == GF_ELEC) msg_print("���Ȃ��͓d���̃u���X��f�����I");
				if (gf_table[p_ptr->pseikaku] == GF_ACID) msg_print("���Ȃ��͎_�̃u���X��f�����I");
				if (gf_table[p_ptr->pseikaku] == GF_POIS) msg_print("���Ȃ��̓K�X�̃u���X��f�����I");
#else
				else msg_print("You breathe fire.");
#endif
		
				fire_ball(gf_table[p_ptr->pseikaku], dir, dam, rad);
			break;	
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�K�i����";
		if (desc) return "������ꏊ�ɊK�i�����B";
#else
		if (name) return "Enchant Weapon";
		if (desc) return "Attempts to increase +to-hit, +to-dam of a weapon.";
#endif
    
		{
			if (cast)
			{
				stair_creation();
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�L�r";
		if (desc) return "�r��L�΂��ė��ꂽ�ꏊ�̓G�ɍU������B�U���񐔂͋����ɉ�����������B �͓��ƎR���͎˒������Ƀ{�[�i�X�𓾂�B";
#else
		if (name) return "Enchant Armor";
		if (desc) return "Attempts to increase +AC of an armor.";
#endif
    
		{
			int length = 1 + p_ptr->lev / 11;

			if(p_ptr->prace == RACE_KAPPA || p_ptr->prace == RACE_YAMAWARO) length = 2 + p_ptr->lev / 7;

			if (info) return format("�˒��F%d",length);
			if (cast)
			{
				project_length = length;
				if (!get_aim_dir(&dir)) return NULL;
				project_hook(GF_ATTACK, dir, HISSATSU_LONGARM, PROJECT_STOP | PROJECT_KILL);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�X�v�����O�t���A";
		if (desc) return "�w�肵���ꏊ�̒n�ʂ𒵂ˏグ�A�����ɏ���Ă���G�𐁂���΂������_���[�W��^����B�ꕔ�̓G�ɂ͌��ʂ��Ȃ��B�����𐁂���΂����Ƃ��ł���B�A�C�e����������Ԃ����邱�Ƃ�����B";
#else
		if (name) return "Telepathy";
		if (desc) return "Gives telepathy for a while.";
#endif
    
		{
			int dist = 1 + plev / 7;

			if (info) return format("�����F%d",dist);
			if (cast)
			{
				if(!cast_spring_floor(dist)) return NULL;
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�d���Ή�";
		if (desc) return "�ꎞ�I�ɉ�������B";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif
    
		{
			int base = 30;
			int sides = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;
	case 18:
#ifdef JP
		if (name) return "�X���C���ω�";
		if (desc) return "�ꎞ�I�ɃX���C���ɕϐg����B����܂ł̈ꎞ���ʂ͑S�ĉ��������B�_�Ɖu�Ƒ����̑ϐ��𓾂ċz���U�����\�ɂȂ邪�A�ړ����x���ቺ���S�Ă̑��������������ꊪ���A������A���@�A�قƂ�ǂ̓���U�����g�p�s�\�ɂȂ�BU�R�}���h���猳�ɖ߂��B";
#else
		if (name) return "Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int percentage;
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				dispel_player();
				if(!set_mimic(base + randint1(base), MIMIC_SLIME, FALSE)) return NULL;
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();
			}
		}
		break;


	case 19:
#ifdef JP
		if (name) return "*�ؓ�����*";
		if (desc) return "��莞�ԁA�r�͂�啝�ɑ���������B�ʏ�̍ő�l�𒴂���B";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif
    
		{
			int base = plev/2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_format("����ȗ͂��̓����r�ꋶ���C������I");
				set_tim_addstat(A_STR,100 + plev/4 ,base + randint1(base),FALSE);

			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "���̍č\��";
		if (desc) return "HP�Ə������S�ɉ񕜂��邪�����_���Ȕ\�͂��P�v�I�Ɍ������邱�Ƃ�����B��������m����HP�񕜗ʂƍő�HP�̔䗦�ɂ��ō�1/2�܂ŏオ��B";
#else
		if (name) return "Explosive Rune";
		if (desc) return "Sets a glyph under you. The glyph will explode when a monster moves on it.";
#endif
    
		{

			if (cast)
			{
				int chance = 50 - p_ptr->chp * 50 / p_ptr->mhp ;
				if(p_ptr->chp == p_ptr->mhp)
				{
					msg_format("�̂��č\������K�v�͂Ȃ��B");
					return NULL;
				}

				msg_format("���Ȃ��͓��̂��č\�����n�߂��E�E");
				hp_player(5000);
				set_cut(0);
				/*:::oops */
				if(randint0(100) < chance)
				{
					int stat = randint0(6);
					switch(stat)
					{
					case A_STR:
						if(dec_stat(stat,20,TRUE)) msg_format("����������������C������E�E");
						break;
					case A_INT:
						if(dec_stat(stat,20,TRUE)) msg_format("�����]��ᰂ��������C������E�E");
						break;
					case A_WIS:
						if(dec_stat(stat,20,TRUE)) msg_format("�����S�����������Ȃ����C������E�E");
						break;
					case A_DEX:
						if(dec_stat(stat,20,TRUE)) msg_format("�����֐߂��ɂދC������E�E");
						break;
					case A_CON:
						if(dec_stat(stat,20,TRUE)) msg_format("�����ݒ����キ�Ȃ����C������E�E");
						break;
					case A_CHR:
						if(dec_stat(stat,20,TRUE)) msg_format("�����������r�ꂽ�C������E�E");
						break;

					}
	
				}
			}
		}
		break;


	case 21:
#ifdef JP
		if (name) return "*�u���X*";
		if (desc) return "����HP��1/3�̈З͂̏�ʑ����u���X��f���B�����͎푰�ɂ��ς��B";
#else
		if (name) return "Knowledge True";
		if (desc) return "*Identifies* an item.";
#endif
		{
			int dam = p_ptr->chp / 3;
			int rad = -3;
			int typ;
			cptr name;
			if(dam > 500) dam = 500;//v1.1.49 1600����500�ɐ����@�����X�^�[�ϐg���Ă��炱��g���Ə���ɓ͂�����

			if(dam<1) dam=1;
			if (info) return info_damage(0, 0, dam);
			

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
#ifdef JP

				switch(p_ptr->prace)
				{
				case RACE_HUMAN:
				case RACE_SENNIN:
				case RACE_KOBITO:
				case RACE_NINJA:
					typ = GF_FORCE;
					name = "�t�H�[�X��";
					break;
				case RACE_YOUKAI:
				case RACE_HOFGOBLIN:
				case RACE_HALF_YOUKAI:
				case RACE_IMP:
				case RACE_VAMPIRE:
					typ = GF_DARK;
					name = "�Í���";
					break;
				case RACE_FAIRY:
					typ = GF_HOLY_FIRE;
					name = "���Ȃ�";
					break;
				case RACE_KAPPA:
				case RACE_NINGYO:
					typ = GF_WATER;
					name = "�A�N�A";
					break;
				case RACE_KARASU_TENGU:
				case RACE_DEMIGOD:
					typ = GF_TORNADO;
					name = "������";
					break;
				case RACE_HAKUROU_TENGU:
				case RACE_ONI:
				case RACE_WARBEAST:
				case RACE_NYUDOU:
					typ = GF_SOUND;
					name = "�Ռ��g��";
					break;
				case RACE_DEATH:
				case RACE_ZOMBIE:
				case RACE_SPECTRE:
				case RACE_HALF_GHOST:
				case RACE_ANIMAL_GHOST:
					typ = GF_NETHER;
					name = "�n����";
					break;
				case RACE_TSUKUMO:
				case RACE_YAMAWARO:
				case RACE_ULTIMATE:
					typ = GF_SHARDS;
					name = "�j�Ђ�";
					break;

				case RACE_YOUKO:
				case RACE_BAKEDANUKI:
					typ = GF_CHAOS;
					name = "�J�I�X��";
					break;
				case RACE_GOLEM:
					typ = GF_LITE;
					name = "�M����";
					break;

				case RACE_DEITY:
				case RACE_DAIYOUKAI:
					typ = GF_DISINTEGRATE;
					name = "������";
					break;
				case RACE_ZASHIKIWARASHI:
					typ = GF_GRAVITY;
					name = "�d�͂�";
					break;

				default:
					typ = GF_MANA;
					name = "���͂�";
					break;
				}
				
				msg_format("���Ȃ���%s�u���X��f�����I",name);
#endif
		
				fire_ball(typ, dir, dam, rad);
			break;	
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�b�ω�";
		if (desc) return "��莞�ԁA��^�̏b�ɕϐg����B����܂ł̈ꎞ���ʂ͑S�ĉ��������B�ϐg���͉����Ɛg�̔\�͂��啝�ɏ㏸���邪�A�O���ƃA�N�Z�T���ȊO�̕���h��ꎞ�I�ɖ��������ꖂ�@�E���Z�E������E�������g���Ȃ��Ȃ�B";
#else
		if (name) return "Mana Branding";
		if (desc) return "Makes current weapon some elemental branded. You must wield weapons.";
#endif
    
		{
			int base = 25 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int percentage;
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				dispel_player();
				if(!set_mimic(base + randint1(base), MIMIC_BEAST, FALSE)) return NULL;
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();

			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�}�O�}����";
		if (desc) return "�^�[�Q�b�g���ӂ֋���ȉΉ��̃{�[���𔭐������A����ɏ���n��ɂ���B";
#else
		if (name) return "Magical armor";
		if (desc) return "Gives resistance to magic, bonus to AC, resistance to confusion, blindness, reflection, free action and levitation for a while.";
#endif
    
		{
			int dam = (100 + plev) * 2;
			int rad = 3 + plev / 45;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				//�����A����GF_FIRE���悾�Ƃ���Ń^�[�Q�b�g�����񂾂Ƃ��^�[�Q�b�g�������ɂȂ��ā��̑������n��ɂȂ�
				if(!fire_ball_jump(GF_LAVA_FLOW, dir, 3 + randint1(2), rad,NULL)) return NULL;
				fire_ball_jump(GF_FIRE, dir, dam, rad,NULL);
			}
		}
		break;
	case 24:
		if (name) return "�������ω�";
		if (desc) return "�����₷���ƕ]���̌��z���̗L���l�ɕϐg����B�ϐg���͎푰�������l�ԂƂقړ����ɂȂ�A�ړ��\�͂������サ�A���C�V�����Ƃ��Ă������̓��ꖂ�@���g����B���ꖂ�@�̃R�X�g�͂�⍂�߂ňЗ͂��{����藎����B";
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if(p_ptr->pclass == CLASS_MARISA)
				{
					msg_print("���Ȃ��͂��łɖ��������B");
					return NULL;
				}
				if(!set_mimic(base+randint1(base), MIMIC_MARISA, FALSE)) return NULL;

			}
		}
		break;
	case 25:
#ifdef JP
		if (name) return "���[�_�[�Z���X";
		if (desc) return "��莞�ԁA���ӂ̒n�`�E�A�C�e���E�g���b�v�E�����X�^�[��S�Ċ��m��������B";
#else
		if (name) return "Remove Enchantment";
		if (desc) return "Removes all magics completely from any weapon or armor.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_radar_sense(randint1(base) + base, FALSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�n�ʉj�@";
		if (desc) return "��莞�ԁA�n�ʂ�ǂ̒��𐅂̂悤�ɉj�����Ƃ��ł���悤�ɂȂ�B";
#else
		if (name) return "Remove Enchantment";
		if (desc) return "Removes all magics completely from any weapon or armor.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_kabenuke(randint1(base) + base, FALSE);
			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "���ω�";
		if (desc) return "��莞�ԁA���ɕϐg����B�푰�����͈ꎞ�I�ɏ㏑�������B�����U���ƃ{���g�������ɂ����Ȃ�A�؂菝�E���΂��󂯕t���Ȃ��Ȃ�A�B���\�͂��啝�ɏ㏸����B�������g�̔\�͂��啝�ɒቺ�����f�ƑM���Ɨ򉻂̍U���Ɏキ�Ȃ��Ă��܂��B�����i�͖��������ꂸ�A���@��A�C�e���͏]���ʂ�g�p�\�B�R��͕s�B";
#else
		if (name) return "Brand Weapon";
		if (desc) return "Makes current weapon a random ego weapon.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if(!set_mimic(base+randint1(base), MIMIC_MIST, FALSE)) return NULL;
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "���̕ϗe";
		if (desc) return "�����_���ɓ��̂Ɋւ���ˑR�ψق𓾂�B";
#else
		if (name) return "Immunity";
		if (desc) return "Gives an immunity to fire, cold, electricity or acid for a while.";
#endif
    
		{
			//gain_random_mutation()�ŕψٌ��肷�邽�߂̐��l�̃��X�g
			int muta_lis[] = {1,5,8,17,19,24,31,35,38,46,62,65,81,83,85,95,98,100,109,120,123,126,129,136,138,141,143,146,154,157,161,163,165,168,171,173,182,185};

			int attempt = 10;

			if (cast)
			{
				while(( attempt-- > 0) && !gain_random_mutation(muta_lis[randint0(sizeof(muta_lis) / sizeof(int))-1]));
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "���̃~�T�C��";
		if (desc) return "���̂̈ꕔ�𔚔����ɍč\�����Ďˏo���A�j�Б����̍U�����s���B���͂���HP��200�`300�����B";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif

		{
			int dam = 800;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				if(p_ptr->chp < 300 && !get_check_strict("�̗͂����Ȃ�����B�{���Ɏg���܂����H", CHECK_OKAY_CANCEL)) return NULL;

#ifdef JP
				msg_print("���Ȃ��͐��̃~�T�C���𔭎˂����I");
#else
				msg_print("You launch a rocket!");
#endif

				fire_rocket(GF_ROCKET, dir, dam, rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 200+randint0(101), "���d�ȓ��̕ϊ�", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "Blood curse", -1);
#endif
			}
		}
		break;
	case 30:
#ifdef JP
		if (name) return "�h���S���ω�";
		if (desc) return "��莞�ԁA����ȃh���S���ɕϐg����B����܂ł̈ꎞ���ʂ͑S�ĉ��������B�ϐg���͐g�̔\�͂������I�ɏ㏸���u���X��f����悤�ɂȂ邪�A���{���ȊO�̑����i�����������ꖂ�@�E���Z�E������E�����E�򂪈�؎g���Ȃ��Ȃ�B�ϐg���̃��x���E�m�\�E�����ɂ��ϐ��Ɣ\�͒l���ω�����B";
#else
		if (name) return "Remove All Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{

			int base = 50;

			if (info) return format("���ԁF50�^�[��");

			if (cast)
			{
				int percentage;
				p_ptr->mimic_dragon_rank = p_ptr->lev + p_ptr->stat_ind[A_INT] + 3 + p_ptr->stat_ind[A_WIS] + 3;
				if(p_ptr->pclass == CLASS_HIGH_MAGE) p_ptr->mimic_dragon_rank+= 20;

				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				dispel_player();
				if(!set_mimic(base, MIMIC_DRAGON, FALSE)) return NULL;

				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= PR_HP;
				redraw_stuff();

			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�]��";
		if (desc) return "�O�̃����_���Ȏ푰��₩���I�����A���̎푰�ւƕω�����B�S�Ă̓ˑR�ψق������AHP�Ɣ\�͒l�ƌo���l�͍Čv�Z��ɑS�񕜂���B���݌o���l��1�`5%���i�v�Ɏ�����B�ꕔ�̎푰�͎g�p�s�B";
#endif
    
		{
			if (cast)
			{
				int new_race;
				int choices[3];
				int i;
				char c;
				bool flag_untimate;

				if((RACE_NEVER_CHANGE) || p_ptr->prace == RACE_ULTIMATE)
				{
					msg_format("���Ȃ��̑��݂͒ʏ�̗։􂩂�O��Ă���悤���B");
					return NULL;
				}
				if (!get_check("�ʂ̎푰�ւƓ]�����܂��B��낵���ł����H")) return NULL;
				
				/*:::�]����푰�������_���ɑI�肷��B*/
				///mod150406 ���O�ݒ�ɖ�ǐ_�l�ǉ�
				//v1.1.79 �V�푰�u���~��炵�v�ɂȂ邱�Ƃ͂ł��Ȃ�
				do
				{
					new_race = randint0(MAX_RACES);
				}
				while ((new_race == p_ptr->prace) || (new_race == RACE_ANDROID) || (new_race == RACE_STRAYGOD) || (new_race == RACE_ULTIMATE) || (new_race == RACE_ZASHIKIWARASHI)
				|| race_info[new_race].flag_only_unique || race_info[new_race].flag_nofixed || race_info[new_race].flag_special);
				choices[0] = new_race;
				do
				{
					new_race = randint0(MAX_RACES);
				}
				while ((new_race == p_ptr->prace) || (new_race == RACE_ANDROID) || (new_race == RACE_STRAYGOD) || (new_race == RACE_ULTIMATE) || (new_race == RACE_ZASHIKIWARASHI)
				|| race_info[new_race].flag_only_unique || race_info[new_race].flag_nofixed || race_info[new_race].flag_special || (new_race == choices[0]) );
				choices[1] = new_race;
				do
				{
					new_race = randint0(MAX_RACES);
				}
				while ((new_race == p_ptr->prace) || (new_race == RACE_ANDROID) || (new_race == RACE_STRAYGOD) || (new_race == RACE_ULTIMATE) || (new_race == RACE_ZASHIKIWARASHI)
				|| race_info[new_race].flag_only_unique || race_info[new_race].flag_nofixed || race_info[new_race].flag_special || (new_race == choices[0])  || (new_race == choices[1]));
				choices[2] = new_race;

				/*:::���ꏈ���@�Ή��ʂ𑕔������z���S�͋��ɐ����̂ɂȂ��i����͋z���S����Ȃ��Ē��̒j�����j*/
				//����Ƃ��Ή���+�Ԑ΂őS�푰�Ώۂɂ���H
				if(p_ptr->prace == RACE_VAMPIRE && inventory[INVEN_HEAD].name1 == ART_STONEMASK)
				{
					choices[2] = RACE_ULTIMATE;
				}			

				screen_save();
				msg_print("�]���������푰���L���őI�����Ă��������B");
				Term_erase(12, 18, 255);
				Term_erase(12, 17, 255);
				Term_erase(12, 16, 255);
				Term_erase(12, 15, 255);
				Term_erase(12, 14, 255);
				for(i=0;i<3;i++) put_str(format("%c) %s",'a'+i, race_info[choices[i]].title) , 15+i , 40);

				while(1)
				{
					c = inkey();
					if(c >= 'a' && c <= 'c' &&(choices[c-'a']==RACE_TENNIN) 
						&& !get_check_strict("�V�l�ɂȂ�Ɠ�x�ƕϗe�̗d�p���g���܂���B��낵���ł����H", CHECK_OKAY_CANCEL)) continue;

					if(c >= 'a' && c <= 'c') break;
				}
				screen_load();
				//�o���l����
				p_ptr->max_exp -= (p_ptr->max_exp / 100 * randint1(5));
				p_ptr->exp = p_ptr->max_exp;
				//�푰�ύX�@�ψق͂��ׂď����A�i���ψق��Đݒ肳���
				change_race(choices[c-'a'],"");

				hp_player(5000);
				set_stun(0);
				set_cut(0);
				set_poisoned(0);
				set_image(0);
				restore_level();
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				set_alcohol(0);

			}
		}
		break;








	}

	return "";
}






static cptr do_new_spell_darkness(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

#ifdef JP
	static const char s_dam[] = "����:";
#else
	static const char s_dam[] = "dam ";
#endif

	int dir;
	int plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�}�W�b�N�E�~�T�C��";
		if (desc) return "�ア���@�̖����B";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�E�C���m";
		if (desc) return "�߂��̎v�l���邱�Ƃ��ł��郂���X�^�[�����m����B";
#else
		if (name) return "Detect Ferocity";
		if (desc) return "Detects all monsters except mindless in your vicinity.";
#endif
    
		if (cast)
		{
			detect_monsters_mind(DETECT_RAD_DEFAULT);
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�����̏j��";
		if (desc) return "��莞�ԎׂȂ�j�����󂯁A��������AC�Ɩ��@�h��Ƀ{�[�i�X�𓾂�B";
#else
		if (name) return "Evil Bless";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "���L�_";
		if (desc) return "�ł̋�����B";
#else
		if (name) return "Stinking Cloud";
		if (desc) return "Fires a ball of poison.";
#endif
    
		{
			int dam = 10 + plev / 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POIS, dir, dam, rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�ψÍ�";
		if (desc) return "��莞�ԁA�Í��ւ̑ϐ��𓾂�B";
#else
		if (name) return "Resist Nether";
		if (desc) return "Gives resistance to nether for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_dark(randint1(base) + base, FALSE);
			}
		}
		break;

	case 5:

#ifdef JP
		if (name) return "�È�";
		if (desc) return "�����镔�����Â�����B";
#else
		if (name) return "Scare Monster";
		if (desc) return "Attempts to scare a monster.";
#endif
    
		{

			if(info) return format("");
			if (cast)
			{
				unlite_room(py,px);
			}
		}
		break;


	case 6:
#ifdef JP
		if (name) return "�Í��̖�";
		if (desc) return "�Í��̃{���g����B";
#else
		if (name) return "Dark Bolt";
		if (desc) return "Fires a bolt or beam of darkness.";
#endif
    
		{
			int dice = 4 + plev / 5;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt(GF_DARK, dir, damroll(dice, sides));
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�����_��";
		if (desc) return "�אڂ����f�[����1�̂�z���ɂ��悤�Ǝ��݂�B�_��̐����s�����Ɋւ�炸�G���x��*2�̃_���[�W���󂯂�B���j�[�N�ɂ͌����Ȃ��B";
#else
		if (name) return "Summon Manes";
		if (desc) return "Summons a manes.";
#endif
    
		{
			if (cast)
			{
				int y, x;
				monster_race *r_ptr;
				monster_type *m_ptr;
				char m_name[80];
				if (!get_rep_dir2(&dir)) return NULL;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!cave[y][x].m_idx)
				{
					msg_print("�����ɂ͉������Ȃ��B");
					return NULL;
				}
				m_ptr = &m_list[cave[y][x].m_idx];
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(!(r_ptr->flags3 & RF3_DEMON))
				{
					msg_format("%s�͌_��𗝉��ł��Ȃ��悤���B",m_name);
				}
				else 
				{
					msg_format("���Ȃ���%s�Ɍ_��������������E�E",m_name);
					take_hit(DAMAGE_USELIFE,(r_ptr->level)*2,"���d�Ȉ����_��",-1);
					control_one_demon(dir, 50 + plev * 5);
				}
			}
		}
		break;


	case 8:
#ifdef JP
		if (name) return "�ϓ�";
		if (desc) return "��莞�ԁA�łւ̑ϐ��𓾂�B�����ɂ��ϐ��ɗݐς���B";
#else
		if (name) return "Resist Poison";
		if (desc) return "Gives resistance to poison. This resistance can be added to which from equipment for more powerful resistance.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;


	case 9:
#ifdef JP
		if (name) return "�闖�̓�";
		if (desc) return "�������̕���ɑ΂��ꎞ�I�ɓł̑�����t�^����B";
#else
		if (name) return "Vision";
		if (desc) return "Maps nearby area.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{

				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)&& !(p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts))
				{
#ifdef JP
					msg_format("�ł�h��镐��������Ă��Ȃ��B");
#else
		msg_format("You cannot use temporary branding with no weapon.");
#endif
					return FALSE;
				}
				set_ele_attack(ATTACK_POIS, base + randint1(base));

			}
		}
		break;

		break;

	case 10:
#ifdef JP
		if (name) return "�Ǖ�";
		if (desc) return "�w�肵�������X�^�[��̂����݂̊K����Ǖ�����B��R�����Ɩ����B";
#else
		if (name) return "Resist Nether";
		if (desc) return "Gives resistance to nether for a while.";
#endif
    
    
		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_GENOCIDE, dir, power, 0);
			}
		}
		break;


	case 11:
#ifdef JP
		if (name) return "�Í����[�U�[";
		if (desc) return "�Í������̃r�[������B";
#else
		if (name) return "Plasma bolt";
		if (desc) return "Fires a bolt or beam of plasma.";
#endif
    
		{
			int dice = 11 + (plev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_DARK, dir, damroll(dice, sides));
			}
		}
		break;
	case 12:
#ifdef JP
		if (name) return "��������";
		if (desc) return "����1�̂��������Ĕz���ɂ���B�������ꂽ�������G�΂��邱�Ƃ�����A�G�΂���m���͌����Ɩ��͂ɂ��B";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif
    
		{
			if (cast)
			{
				bool pet = TRUE;
				u32b mode = 0L;

				if((adj_general[p_ptr->stat_ind[A_CHR]] + adj_general[p_ptr->stat_ind[A_WIS]]) < randint1(30)) pet = FALSE;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= PM_NO_PET;
				if (!(pet && (plev < 50))) mode |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, plev+randint1(plev/2), SUMMON_DEMON, mode))
				{
#ifdef JP
					msg_print("�����̈��L���[�������B");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("�u���p�ł������܂����A����l�l�v");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("�u�ڂ����҂�A��͓��̉��l�ɂ��炸�I ���O�̍��𒸂����I�v");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("�����͌���Ȃ������B");
#else
					msg_print("No demons arrive.");
#endif
				}
				break;
			}
		}
		break;

		//v1.1.94 �Őj�쐬���Ɏq�̏�...�ɂ��悤���Ǝv����������ς�߂�
		//�h��f�o�t�́u�Z���Ӂv�Ƃ�����Ȃ̂ɂ���
	case 13:
		if (name) return "�Z����";
		if (desc) return "�����X�^�[��̖̂h���(AC)��ቺ������B��R�����Ɩ����B";
		{

			int base = plev * 3;

			if (cp_ptr->magicmaster) base += plev * 2;

			if (info) return info_string_dice("����:",1, base, base);

			if (cast)
			{


				project_length = 1 + p_ptr->lev / 10;
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_DEC_DEF, dir, base+randint1(base), 0);
			}
		}
		break;
		/*
	case 13:
		{
			int dam = plev * 4;

			if (cp_ptr->magicmaster)
				dam = plev * 6;

			if (name) return "�Ɏq�̏�";
			if (desc) return "�ڐG�^�̗אڍU����h���������o���B��x�U����h���Ə��͔j�󂳂�A���̂Ƃ����͂Ȕj�Б����̔������s���B";

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				msg_print("�����ʂ�������ȏ������ꂽ�B");
				p_ptr->special_defense |= (SD_GLASS_SHIELD);
				p_ptr->redraw |= (PR_STATUS);
			}

		}
		break;

		*/

		/*
	case 13:
#ifdef JP
		if (name) return "�Őj�쐬";
		if (desc) return "�G���ꌂ�œ|�����Ƃ����镐��u�Őj�v���쐬����B�ޗ��Ƃ��āu�Q���Z�~�E���E�G���K���X�v��������B";
#else
		if (name) return "Fire Ball";
		if (desc) return "Fires a ball of fire.";
#endif
    
		{

			if (cast)
			{
				cptr q,s;
				int item;
				object_type forge;
				object_type *q_ptr = &forge;

				item_tester_hook = item_tester_hook_gelsemium;
#ifdef JP
				q = "�ǂ̐G�}���g���܂���? ";
				s = "���Ȃ��͐G�}�ƂȂ�ł������Ă��Ȃ��B";
#endif
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);	
				}

				msg_print("���Ȃ��͖ғł̗t��T�d�ɓŐj�Ɏd�グ���E�E");
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_KNIFE,SV_WEAPON_DOKUBARI));
				apply_magic(q_ptr, 1, AM_NO_FIXED_ART | AM_FORCE_NORMAL);
				(void)drop_near(q_ptr, -1, py, px);


			}
		}
		break;
		*/

	case 14:
#ifdef JP
		if (name) return "�z��";
		if (desc) return "�����X�^�[1�̂��琶���͂��z������ĉh�{�ɂ���B";
#else
		if (name) return "Vampiric Drain";
		if (desc) return "Absorbs some HP from a monster and gives them to you. You will also gain nutritional sustenance from this.";
#endif
    
		{
			int dice = 1;
			int sides = plev * 2;
			int base = plev * 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				int dam = base + damroll(dice, sides);

				if (!get_aim_dir(&dir)) return NULL;

				if (drain_life(dir, dam))
				{
	///del131214 virtue
					//chg_virtue(V_SACRIFICE, -1);
					//chg_virtue(V_VITALITY, -1);

					hp_player(dam);
					if(one_in_(10)) set_deity_bias(DBIAS_COSMOS, -1);
					/*
					 * Gain nutritional sustenance:
					 * 150/hp drained
					 *
					 * A Food ration gives 5000
					 * food points (by contrast)
					 * Don't ever get more than
					 * "Full" this way But if we
					 * ARE Gorged, it won't cure
					 * us
					 */
					dam = p_ptr->food + MIN(5000, 100 * dam);

					/* Not gorged already */
					if (p_ptr->food < PY_FOOD_MAX)
						set_food(dam >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dam);
				}
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���E";
		if (desc) return "�t���A�S�̂̎w�肵�������̃����X�^�[�����݂̊K����Ǖ�����B��R�����Ɩ����B";
#else
		if (name) return "Genocide";
		if (desc) return "Eliminates an entire class of monster, exhausting you.  Powerful or unique monsters may resist.";
#endif
    
		{
			int power = plev+50;

			if (info) return info_power(power);

			if (cast)
			{
				symbol_genocide(power, TRUE,0);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�i�C�g���A";
		if (desc) return "�߂��̓G��̂ɑ΂����_�U�����s���B�ʏ�̐��_�������Ȃ��G�ɂ͌��ʂ������B�����Ă���G�ɂ͌��ʂ������A�G���N�����ɂ����B";
#else
		if (name) return "Devilish Eye";
		if (desc) return "Gives telepathy for a while.";
#endif
    
		{
			int dam = plev * 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				project_length = 1 + p_ptr->lev / 15;
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_NIGHTMARE, dir, dam, 0);
			}
		}
		break;
	case 17:

#ifdef JP
		if (name) return "����";
		if (desc) return "����m�����A���|���������A��������B";
#else
		if (name) return "Battle Frenzy";
		if (desc) return "Gives another bonus to hit and HP, immunity to fear for a while. Hastes you. But decreases AC.";
#endif
    
		{
			int b_base = 25;
			int sp_base = plev / 2;
			int sp_sides = 20 + plev / 2;

			if (info) return info_duration(b_base, b_base);

			if (cast)
			{
				set_shero(randint1(25) + 25, FALSE);
				hp_player(30);
				set_afraid(0);
				set_fast(randint1(sp_sides) + sp_base, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�őf�̗�";
		if (desc) return "����ȓł̋�����B";
#else
		if (name) return "Devil Cloak";
		if (desc) return "Removes fear. Gives resistance to fire and cold, and aura of fire. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int dam = plev * 3 + 100;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POIS, dir, dam, rad);
			}
		}

		break;


	case 19:
#ifdef JP
		if (name) return "�w���E�����X";
		if (desc) return "�w���t�@�C�A�����̃r�[������B������G�Ɍ��ʂ�����A�V�g�Ȃǂɂ͂���ɑ傫�ȃ_���[�W��^����B";
#else
		if (name) return "Plasma Ball";
		if (desc) return "Fires a ball of plasma.";
#endif
    
		{
			int dam = plev * 3 + 25;

			if (cp_ptr->magicmaster) dam += plev;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_beam(GF_HELL_FIRE,dir, dam);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�����ω�";
		if (desc) return "��莞�ԁA�����ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����Ɉ����Ƃ��Ă̔\�͂𓾂�B";
#else
		if (name) return "Polymorph Demon";
		if (desc) return "Mimic a demon for a while. Loses abilities of original race and gets abilities as a demon.";
#endif
    
		{
			int base = 10 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_DEMON, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�Í��̗�";
		if (desc) return "����ȈÍ��̋�����B";
#else
		if (name) return "Darkness Storm";
		if (desc) return "Fires a huge ball of darkness.";
#endif
    
		{
			int dam = 100 + plev * 5 / 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DARK, dir, dam, rad);
			}
		}
		break;
	case 22:
#ifdef JP
		if (name) return "�^�E�z��";
		if (desc) return "�����X�^�[1�̂��琶���͂��z���Ƃ�B�z���Ƃ��������͂ɂ���đ̗͂��񕜂���B";
#else
		if (name) return "Vampirism True";
		if (desc) return "Fires 3 bolts. Each of the bolts absorbs some HP from a monster and gives them to you.";
#endif
    
		{
			int dam = 100;

			if (info) return format("%s3*%d", s_dam, dam);

			if (cast)
			{
				int i;

				if (!get_aim_dir(&dir)) return NULL;


				for (i = 0; i < 3; i++)
				{
					if (drain_life(dir, dam))
					{
						if(!i && one_in_(3)) set_deity_bias(DBIAS_COSMOS, -1);
						hp_player(dam);
					}
				}
			}
		}
		break;


	case 23:
#ifdef JP
		if (name) return "�łƂ̗Z�a";
		if (desc) return "��莞�ԈÈłƓ�������B�B���\�͂��㏸���A�����������A�Èłł��������ʂ��A���ڍU�����ɕs�ӑł��ő�_���[�W��^���A�Í��U���ɑ΂���Ɖu�𓾂�悤�ɂȂ�B�������j�ׂƑM���ɋɂ߂Ďキ�Ȃ�B";
#else
		if (name) return "Doom Hand";
		if (desc) return "Attempts to make a monster's HP almost half.";
#endif
    
		{
			int base = 15;
			int sides = plev / 3;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_unite_darkness(base + randint1(sides),FALSE);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�e�̃Q�[�g";
		if (desc) return "�ʒu���w�肵�ăe���|�[�g����B���邭�Ƃ炳�ꂽ�ꏊ���Q���̂���ꏊ�ɂ͏o���Ȃ��B�w�肵���ʒu�ɏo���Ȃ������Ƃ��e���|�[�g�͍s���Ȃ��B";
#else
		if (name) return "Immortal Body";
		if (desc) return "Gives resistance to time for a while.";
#endif
    
		{
			int range = plev ;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("�e�̖傪�J�����B�ړI�n��I��ŉ������B");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(D_DOOR_SHADOW)) return NULL;
			}
		}
		break;
	case 25:
#ifdef JP
		if (name) return "�s�щ�";
		if (desc) return "���̊K�̑��B���郂���X�^�[�����B�ł��Ȃ��Ȃ�B";
#else
		if (name) return "Sterilization";
		if (desc) return "Prevents any breeders on current level from breeding.";
#endif
    
		{
			if (cast)
			{
				msg_print("���Ȃ��̕������􂢂���n�ɖ������E�E");

				num_repro += MAX_REPRO;
			}
		}
		break;


	case 26:
#ifdef JP
		if (name) return "����";
		if (desc) return "��莞�ԁA�������̕��킪�������͂̓G�ɑ傫�ȃ_���[�W��^����悤�ɂȂ�B���x�����㏸����Ƌz�����ʂ��ǉ������B";
#else
		if (name) return "Insanity Circle";
		if (desc) return "Generate balls of chaos, confusion and charm centered on you.";
#endif
    
		{
			int base = 5 + plev / 10;

			if (info) return info_duration(base, base);

			if (cast)
			{

				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM)&& !(p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts))
				{
#ifdef JP
					msg_format("����������Ă��Ȃ��B");
#else
		msg_format("You cannot use temporary branding with no weapon.");
#endif
					return FALSE;
				}
				set_ele_attack(ATTACK_DARK, base + randint1(base));
			}
		}
		break;
	case 27:
#ifdef JP
		if (name) return "�e���v�e�[�V����";
		if (desc) return "��莞�Ԓʏ�̌��E�𒴂��Ė��͂��㏸���A����ɉr�����Ɏ��E���ɂ���G�S�Ă�z���ɂ��悤�Ǝ��݂�B";
#else
		if (name) return "Explode Pets";
		if (desc) return "Makes all pets explode.";
#endif
    
		{
			int base = 15 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{

				msg_print("���Ȃ��͖����̃J���X�}�ɖ�����ꂽ�I");
				set_tim_addstat(A_CHR,120,base + randint1(base),FALSE);
				charm_monsters(plev * 3);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "���Ӗ��E";
		if (desc) return "�����̎��͂ɂ��郂���X�^�[�����݂̊K����Ǖ�����B��R�����Ɩ����B";
#else
		if (name) return "Mass Genocide";
		if (desc) return "Eliminates all nearby monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif
    
		{
			int power = plev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide(power, TRUE);
			}
		}
		break;



	case 29:
#ifdef JP
		if (name) return "��ʈ�������";
		if (desc) return "���͂Ȉ����𕡐��������Ĕz���ɂ���B�������ꂽ�������G�΂��邱�Ƃ�����A�G�΂���m���͌����Ɩ��͂ɂ��B";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif
		{
			if (cast)
			{
				bool pet = TRUE;
				u32b mode = 0L;
				bool summon = FALSE;
				int i;
				if((adj_general[p_ptr->stat_ind[A_CHR]] + adj_general[p_ptr->stat_ind[A_WIS]]) < randint1(90)) pet = FALSE;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= PM_NO_PET;
				if (!(pet && (plev < 50))) mode |= PM_ALLOW_GROUP;

				for(i=3+randint0(3);i>0;i--)
					if (summon_specific((pet ? -1 : 0), py, px, plev+randint1(plev), SUMMON_HI_DEMON, mode))summon = TRUE;

				if(summon)
				{
#ifdef JP
					msg_print("�����̈��L���[�������B");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("�u�u�u���p�ł������܂����A����l�l�v�v�v");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("�u�u�u�ڂ����҂�A��X�͓��̉��l�ɂ��炸�I ���O�̍��𒸂����I�v�v�v");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("�����͌���Ȃ������B");
#else
					msg_print("No demons arrive.");
#endif
				}
				break;
			}
		}
		break;



	case 30:
#ifdef JP
		if (name) return "�n���̍���";
		if (desc) return "�w���t�@�C�A�����̋��͂ȃ{�[������B������G�Ɍ��ʂ�����A�V�g�Ȃǂɂ͂���ɑ傫�ȃ_���[�W��^����B�r�����ɔ�J��20+1d30�̃_���[�W���󂯂�B";
#else
		if (name) return "Hellfire";
		if (desc) return "Fires a powerful ball of evil power. Hurts good monsters greatly.";
#endif
    
		{
			int dam = 666;
			int rad = 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HELL_FIRE, dir, dam, rad);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "�n���̍��΂̎�������������J", -1);
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "the strain of casting Hellfire", -1);
#endif
			}
		}
		break;

	case 31:
#ifdef JP
		if (name) return "�����ω�";
		if (desc) return "�����̉��ɕω�����B�ω����Ă���Ԃ͖{���̎푰�̔\�͂������A����Ɉ����̉��Ƃ��Ă̔\�͂𓾁A�ǂ�j�󂵂Ȃ�������B";
#else
		if (name) return "Polymorph Demonlord";
		if (desc) return "Mimic a demon lord for a while. Loses abilities of original race and gets great abilities as a demon lord. Even hard walls can't stop your walking.";
#endif
    
		{
			int base = 15;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_mimic(base + randint1(base), MIMIC_DEMON_LORD, FALSE);
			}
		}
		break;
	}

	return "";
}





/*:::�V�̈�@����*/
static cptr do_new_spell_summon(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;

	u32b summon_mode = 0L;

#ifdef JP
	static const char s_random[] = "�����_��";
#else
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{

	case 0:
#ifdef JP
		if (name) return "�֏���";
		if (desc) return "�ւ�z���Ƃ��Ĉ�C��������B�㏞�͕s�v�B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev;
			if (info) return format("�������x���F%d",lev);
			if (cast || fail)
			{
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;

				if (new_summoning(1, py, px, lev, SUMMON_SNAKE, summon_mode))
				{
					if(cast) msg_print("���Ȃ��͎ւ����������B");
					else     msg_print("�������ꂽ�ւ����Ȃ��Ɋ��݂��Ă����I");
				}
			}
		}
		break;


	case 1:
#ifdef JP
		if (name) return "�V���[�g�E�e���|�[�g";
		if (desc) return "�ߋ����̃e���|�[�g������B";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;
	case 2:
#ifdef JP
		if (name) return "������";
		if (desc) return "����z���Ƃ��Ĉ�C�`������������B�㏞�͕s�v�B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = 1 + plev / 2;
			int kind;
			if (info) return format("�������x���F%d",lev);
			if (cast || fail)
			{
				int num = 1 + plev / 10;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				if(plev > 19) summon_mode |= PM_ALLOW_GROUP;

				if(one_in_(3)) kind = SUMMON_ANT;
				else kind = SUMMON_SPIDER;

				if (new_summoning(num, py, px, lev, SUMMON_SPIDER, summon_mode))
				{
					if(cast) msg_print("���Ȃ��͒������������B");
					else     msg_print("�������ꂽ�������Ȃ��ɔ�т������Ă����I");
				}
			}
		}
		break;


	case 3:
#ifdef JP
		if (name) return "�T�����}�e���A��";
		if (desc) return "�^�[�Q�b�g�̓���։����𗎂Ƃ��B";
#else
		if (name) return "Reset Recall";
		if (desc) return "Resets the 'deepest' level for recall spell.";
#endif
    
		{
			if (info) return format("�����F�s��");
			if (cast)
			{
				cptr msg;
				int summon = randint1(plev);
				int typ,rad,dam;

				if (!get_aim_dir(&dir)) return NULL;
				if(dir == 5 && target_okay())
				{
					if(target_who == p_ptr->riding)
					{
						msg_print("����͊�Ȃ������߂Ă������B");
						return NULL;
					}
					else if(target_who < 0)
					{
						msg_print("�����ɂ͉������Ȃ��B");
						return NULL;
					}
					else summon_material(target_who);
					///mod150215 �ʃ��[�`���ɕ����Ɨ�����
				}
				else
				{
					msg_format("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
					return NULL;
				}

			}


		}
		break;
	case 4:
#ifdef JP
		if (name) return "�d������";
		if (desc) return "�d����z���Ƃ��Ĉ�́`������������B�㏞�Ƃ��Ă��َq�������B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev * 2 / 3;
			if (info) return format("��{�������x���F%d",lev);
			if (cast || fail)
			{
				int num = 1 + plev / 20;
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::���s���͕�V������Ȃ�*/
				if(cast)
				{
					item_tester_tval = TV_SWEETS;
					if(!select_pay(&cost)) return NULL;
					if(cost > 100) lev += 20;
					if(cost > 1000) lev += 20;
				}
				if (new_summoning(num, py, px, lev, SUMMON_FAIRY, summon_mode))
				{
					if(cast) msg_print("���Ȃ��͗d�������������B");
					else     msg_print("�d�������ꂽ���A�Ђǂ��������Ă��萧��s�\���I");
				}
			}
		}
		break;


	case 5:
#ifdef JP
		if (name) return "�e���|�[�g";
		if (desc) return "�������̃e���|�[�g������B";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 4;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "��������";
		if (desc) return "���R�E�̑��݂�z���Ƃ��Ĉ�́`������������B�㏞�Ƃ��ĐH���������B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev ;
			if (info) return format("��{�������x���F%d",lev);
			if (cast || fail)
			{
				int num = 1 + plev / 15;
				int cost;
				int kind;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::���s���͕�V������Ȃ�*/
				if(cast)
				{
					item_tester_tval = TV_FOOD;
					if(!select_pay(&cost)) return NULL;
					if(cost > 20) lev += 10;
					if(cost > 200) lev += 10;
					if(cost > 2000) lev += 10;
				}

				if(plev > 40 && one_in_(4)) kind = SUMMON_HYDRA;
				else kind = SUMMON_ANIMAL;

				if (new_summoning(num, py, px, lev, kind, summon_mode))
				{
					if(cast) msg_print("���Ȃ��͓��������������B");
					else     msg_print("���������͂��Ȃ������C���f�B�b�V���Ɍ��߂��悤���I");
				}
			}
		}
		break;



	case 7:
#ifdef JP
		if (name) return "�A�C�e���e���|�[�g";
		if (desc) return "���ꂽ�ꏊ�ɂ���A�C�e���������̑����ֈړ�������B";
#else
		if (name) return "Trump Reach";
		if (desc) return "Pulls a distant item close to you.";
#endif
    
		{
			int weight = plev * 30;

			if (info) return info_weight(weight);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fetch(dir, weight, FALSE);
			}
		}
		break;
	case 8:
#ifdef JP
		if (name) return "�e���|�[�g�E�A�E�F�C";
		if (desc) return "�����X�^�[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;
	case 9:
#ifdef JP
		if (name) return "�����X�^�[��̏���";
		if (desc) return "�����_���ȃ����X�^�[��z���Ƃ��Ĉ�̏�������B�㏞�͕s�v�B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev;
			if (info) return format("�������x���F%d",lev);
			if (cast || fail)
			{
				int num = 1;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				if (new_summoning(num, py, px, lev, 0, summon_mode))
				{
					if(cast) msg_print("���Ȃ��̓����X�^�[�����������B");
					else     msg_print("�������ꂽ�҂͂��Ȃ��ɓG�΂����I");
				}
			}
		}
		break;


	case 10:
#ifdef JP
		if (name) return "�e���|�[�g�E���x��";
		if (desc) return "�u���ɏォ���̊K�Ƀe���|�[�g����B";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				if (!get_check("�{���ɑ��̊K�Ƀe���|�[�g���܂����H")) return NULL;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "��������";
		if (desc) return "�f�[������z���Ƃ��Ĉ�̏�������B�㏞�Ƃ��ď������x���Ɠ��ʂ�HP�������B�����Ɏ��s�����Ƃ������̑㏞�͏�����B���̏����̓v���[���[���͂𖳎�����B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev * 3 / 2;
			if (info) return format("�������x���F%d",lev);
			
			if(cast && p_ptr->chp < lev)
			{
				msg_print("���̂��Ȃ��̗̑͂ł͂��̏����͊댯���B");
				return NULL;
			}

			if (cast || fail)
			{
				int num = 1;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::Hack ���i���א��͂��ĂׂȂ����ł����������ł���*/
				ignore_summon_align = TRUE;
				if (new_summoning(num, py, px, lev, SUMMON_DEMON, summon_mode))
				{
					if(cast) msg_print("���Ȃ��͈��������������B");
					else     msg_print("�������ꂽ�����͂��Ȃ��̍���v�������I");

					take_hit(DAMAGE_USELIFE, lev, "���������ɂ��㏞", -1);
				}
				ignore_summon_align = FALSE;

			}
		}
		break;
///mod151024 �Ǖ���]���w�ɕύX
/*
	case 12:
#ifdef JP
		if (name) return "�Ǖ�";
		if (desc) return "�����X�^�[1�̂����݂̊K����Ǖ�����B�o���l��A�C�e���͎�ɓ���Ȃ��B�e���|�[�g�ϐ��𖳎����邪��R�����Ɩ����B";
#else
		if (name) return "Genocide One";
		if (desc) return "Attempts to vanish a monster.";
#endif
    
		{
			int power = 50 + plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_GENOCIDE, dir, power, 0);
			}
		}
		break;
*/
	case 12:
		if (name) return "�]���w";
		if (desc) return "�A�C�e�����ނ�T�����_�܂œ]������B";
		{

			if (info) return "";

			if (cast)
			{
				msg_print("���Ȃ��͓]���̖��@�w��`�����E�E");
				item_recall(2);
			}
		}
		break;
	case 13:
#ifdef JP
		if (name) return "�G�������^������";
		if (desc) return "�G�������^����z���Ƃ��ĕ�����������B�㏞�͕s�v�����A��⃍�b�h�Ȃǂ̖�����������Ƌ��͂ȃG�������^�����o�Ă��₷���B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev ;
			if (info) return format("��{�������x���F%d",lev);
			if (cast || fail)
			{
				int num = 2 + plev / 15;
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::���s���͕�V������Ȃ�*/
				if(cast)
				{
					item_tester_hook = item_tester_hook_staff_rod_wand;
					if(!select_pay(&cost)) cost=0;
					if(cost) lev += 5 + cost / 200;
					if(lev > 120) lev = 120;
				}

				if (new_summoning(num, py, px, lev, SUMMON_ELEMENTAL, summon_mode))
				{
					if(cast) msg_print("���Ȃ��̓G�������^�������������B");
					else     msg_print("�G�������^���͂��Ȃ��Ɏg������邱�Ƃ����ۂ����I");
				}
			}
		}
		break;



	case 14:
#ifdef JP
		if (name) return "�b���ٗp";
		if (desc) return "�l�ԁA���邢�͐l�^�����X�^�[��z���Ƃ��ĕ�����������B�㏞�Ƃ��ċ����x�����B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = MAX(dun_level,plev);
			int charge = lev * lev * 2;
			if (info) return format("�������x���F%d ��V�F��%d",lev,charge);
			if (cast || fail)
			{
				int num = 3 + randint1(3) + plev / 10;
				int cost;
				if(cast)	summon_mode |= (PM_FORCE_PET | PM_ALLOW_GROUP);
				else summon_mode |= PM_NO_PET;

				if(cast)
				{
					if(p_ptr->au < charge)
					{
						msg_print("���Ȃ��͏[���Ȃ����������Ă��Ȃ��B");
						return NULL;
					}
				}

				if (new_summoning(num, py, px, lev, SUMMON_HUMANOID, summon_mode))
				{
					if(cast)
					{
						p_ptr->au -= charge;
						p_ptr->redraw |= PR_GOLD;
						msg_print("���Ȃ��̌��ɗb���������y���Q�����B");
					}
					else     msg_print("�b�������͂������������ɓ]�E�����悤���I");
				}
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�����̔�";
		if (desc) return "�Z�������̎w�肵���ꏊ�Ƀe���|�[�g����B";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif
    
		{
			int range = p_ptr->lev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("�����̔����J�����B�ړI�n��I��ŉ������B");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(D_DOOR_NORMAL)) return NULL;
			}
		}
		break;


	case 16:
#ifdef JP
		if (name) return "�^�E���E�e���|�[�g";
		if (desc) return "�s�������Ƃ̂���X�ֈړ�����B�n��ɂ���Ƃ������g���Ȃ��B";
#else
		if (name) return "Teleport to town";
		if (desc) return "Teleport to a town which you choose in a moment. Can only be used outdoors.";
#endif
    
		{
			if (cast)
			{
				if (!tele_town(TRUE)) return NULL;
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�n�E���h����";
		if (desc) return "�T�E�[�t�@�[�n�E���h�n�����X�^�[��z���Ƃ��Ĉ�O���[�v��������B�㏞�͕s�v�B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev;
			if (info) return format("�������x���F%d + 1d%d",lev,lev/2);
			if (cast || fail)
			{
				int num = 1;
				lev += randint1(lev/2);
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				if (new_summoning(num, py, px, lev, SUMMON_HOUND, summon_mode))
				{
					if(cast) msg_print("���Ȃ��̓n�E���h�����������B");
					else     msg_print("�������ꂽ�n�E���h�͂��Ȃ��ɉ�𔍂����I");
				}
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�ʒu�����e���|�[�g";
		if (desc) return "1�̂̃����X�^�[�ƈʒu����������B";
#else
		if (name) return "Swap Position";
		if (desc) return "Swap positions of you and a monster.";
#endif
    
		{
			if (cast)
			{
				bool result;

				/* HACK -- No range limit */
				project_length = -1;

				result = get_aim_dir(&dir);

				/* Restore range to default */
				project_length = 0;

				if (!result) return NULL;

				teleport_swap(dir);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�_�i����";
		if (desc) return "�_�i��z���Ƃ��Ĉ�̏�������B�㏞�Ƃ��ĕ����������B�����ȕ���ł���قǋ��͂Ȑ_�i���o�₷���B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev ;
			if (info) return format("��{�������x���F%d",lev);
			if (cast || fail)
			{
				int num = 1;
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::���s���͕�V������Ȃ�*/
				if(cast)
				{
					item_tester_hook = object_allow_enchant_weapon;
					if(!select_pay(&cost)) return NULL;
					if(cost <= 0) //�����l�ȃA�C�e�����g���Ǝ��s����
					{
						summon_mode &= ~(PM_FORCE_PET);
						summon_mode |= PM_NO_PET;
					}
					lev += cost / 1000;
					if(lev > 120) lev = 120;
				}

				if (new_summoning(num, py, px, lev, SUMMON_DEITY, summon_mode))
				{
					if(summon_mode & PM_FORCE_PET) msg_print("���Ȃ��͐_�i�����������B");
					else     msg_print("���Ȃ��͐_�̓{��ɐG��Ă��܂����悤���I");
				}
			}
		}
		break;



	case 20:
#ifdef JP
		if (name) return "�_���W�����e���|�[�g";
		if (desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�h���S������";
		if (desc) return "�h���S����z���Ƃ��Ĉ�̏�������B�㏞�͕s�v�����A�h���S���̗؂�܂Ȃǂ̕��i�������Ƌ���������������₷���B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev * 4 / 5 ;
			if (info) return format("��{�������x���F%d",lev);
			if (cast || fail)
			{
				int num = 1;
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::���s���͕�V������Ȃ�*/
				if(cast)
				{
					item_tester_hook = item_tester_hook_dragonkind;
					if(!select_pay(&cost)) cost=0;

					if(cost) lev += 5 + cost / 1000;
					if(lev > 80) lev = 80;
				}

				if (new_summoning(num, py, px, lev, SUMMON_DRAGON, summon_mode))
				{
					if(cast) msg_print("���Ȃ��͗������������B");
					else     msg_print("�������ꂽ���͂��Ȃ����ܖ����悤�Ƃ��Ă���I");
				}
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "���W";
		if (desc) return "�t���A�ɂ��鎩���̔z���������̋߂��ɌĂъ񂹂�B";
#else
		if (name) return "Living Trump";
		if (desc) return "Gives mutation which makes you teleport randomly or makes you able to teleport at will.";
#endif
    
		{
			if (cast)
			{

		{
			int pet_ctr, i;
			u16b *who;
			int max_pet = 0;
			u16b dummy_why;

			/* Allocate the "who" array */
			C_MAKE(who, max_m_idx, u16b);

			/* Process the monsters (backwards) */
			for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				if (is_pet(&m_list[pet_ctr]) && (p_ptr->riding != pet_ctr))
				  who[max_pet++] = pet_ctr;
			}

			/* Select the sort method */
			ang_sort_comp = ang_sort_comp_pet;
			ang_sort_swap = ang_sort_swap_hook;

			ang_sort(who, &dummy_why, max_pet);

			/* Process the monsters (backwards) */
			for (i = 0; i < max_pet; i++)
			{
				pet_ctr = who[i];
				teleport_monster_to(pet_ctr, py, px, 100, TELEPORT_PASSIVE);
			}

			/* Free the "who" array */
			C_KILL(who, max_m_idx, u16b);
		}
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�����X�^�[��������";
		if (desc) return "�����_���ȃ����X�^�[��z���Ƃ��ĕ�����������B�㏞�͕s�v�B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev;
			if (info) return format("�������x���F%d",lev);
			if (cast || fail)
			{
				int num = 3 + lev / 10;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				if (new_summoning(num, py, px, lev, 0, summon_mode))
				{
					if(cast) msg_print("���Ȃ��̓����X�^�[�̌Q������������B");
					else     msg_print("�������ꂽ�҂����͑S�Ă��Ȃ��ɓG�΂����I");
				}
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "����ϐ�";
		if (desc) return "��莞�ԁA����U���ւ̑ϐ��𓾂�B";
#else
		if (name) return "Immortal Body";
		if (desc) return "Gives resistance to time for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_res_time(randint1(base)+base, FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "*�e���|�[�g�E�A�E�F�C*";
		if (desc) return "���E���̑S�Ẵ����X�^�[���e���|�[�g������B��R�����Ɩ����B";
#else
		if (name) return "Banishment";
		if (desc) return "Teleports all monsters in sight away unless resisted.";
#endif
    
		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				banish_monsters(power);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�d�ԏ���";
		if (desc) return "�˒����Z�����^�[�Q�b�g�𐁂���΂��r�[����̍U�����d�|����B�e���|�[�g�ϐ��𖳎����邪����ȓG�͐�����΂Ȃ��B";
#else
		if (name) return "Heal Monster";
		if (desc) return "Heal a monster.";
#endif
    
		{

			int dam = plev * 2 + 100;
			int range = plev /5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				project_length = range;
				if (!get_aim_dir(&dir)) return NULL;

				msg_print("�˔@�A�����ƂƂ��ɓd�Ԃ��ʂ�߂����I");

				fire_beam(GF_TRAIN, dir, dam);
			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "��ʈ�������";
		if (desc) return "���͂ȃf�[������z���Ƃ��ĕ����̏�������B�㏞�Ƃ��ď������x���̎O�{��HP�������B�����Ɏ��s�����Ƃ������̑㏞�͏�����B���̏����̓v���[���[���͂𖳎�����B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev * 3 / 2;
			if (info) return format("�������x���F%d",lev);
			
			if(cast && p_ptr->chp < lev * 3)
			{
				msg_print("���̂��Ȃ��̗̑͂ł͂��̏����͊댯���B");
				return NULL;
			}

			if (cast || fail)
			{
				int num = 1 + plev / 20 + randint0(4);
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::Hack ���i���א��͂��ĂׂȂ����ł����������ł���*/
				ignore_summon_align = TRUE;
				if (new_summoning(num, py, px, lev, SUMMON_HI_DEMON, summon_mode))
				{
					if(cast) msg_print("���Ȃ��͋��͂Ȉ������������������B");
					else     msg_print("���������͂��Ȃ���A��Ēn���ɋA�낤�Ƃ��Ă���I");

					take_hit(DAMAGE_USELIFE, lev*3, "���������ɂ��㏞", -1);
				}
				ignore_summon_align = FALSE;
			}
		}
		break;


	case 28:
#ifdef JP
		if (name) return "���ҏp";
		if (desc) return "��莞�ԁA�G�̏������@��j�Q����B�G�̃��x���������ƌ��ʂ���������Ȃ����Ƃ�����B";
#else
		if (name) return "Trump Meteor";
		if (desc) return "Makes meteor balls fall down to nearby random locations.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_deportation(randint1(base)+base, FALSE);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�T�C�o�[�f�[��������";
		if (desc) return "���͂Ȉ��������z���Ƃ��Ĉ�̏�������B�㏞�͕s�v�B���̏����̓v���[���[���͂𖳎�����B";
#else
		if (name) return "Trump Demon";
		if (desc) return "Summons a demon.";
#endif
		{
			if (info) return format(" ");
			
			if (cast || fail)
			{
				int lev = 77;
				int num = 1;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::Hack ���i���א��͂��ĂׂȂ����ł����������ł���*/
				ignore_summon_align = TRUE;
				if (new_summoning(num, py, px, lev, SUMMON_CYBER, summon_mode))
				{
					if(cast) msg_print("���Ȃ��̓T�C�o�[�f�[���������������B");
					else     msg_print("�T�C�o�[�f�[�����͋��낵���C�������Ȃ��Ɍ������I");
				}
				ignore_summon_align = FALSE;
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�Ñ�h���S������";
		if (desc) return "���͂ȃh���S����z���Ƃ���1�́`������������B�㏞�͕s�v�����A�h���S���̗؂�܂Ȃǂ̕��i�������Ƌ���������������₷���B";
#else
		if (name) return "Trump Spiders";
		if (desc) return "Summons spiders.";
#endif
    
		{
			int lev = plev;
			if (info) return format("��{�������x���F%d",lev);
			if (cast || fail)
			{
				int num = randint1(2);
				int cost;
				if(cast) summon_mode |= PM_FORCE_PET;
				else summon_mode |= PM_NO_PET;
				summon_mode |= PM_ALLOW_GROUP;

				/*:::���s���͕�V������Ȃ�*/
				if(cast)
				{
					item_tester_hook = item_tester_hook_dragonkind;
					if(!select_pay(&cost)) cost=0;
					if(cost)
					{
						lev += 10 + cost / 1000;
						num += randint1(2) + cost / 10000;
					}
					if(lev > 120) 
					{
						lev = 120;
					}
				}

				if (new_summoning(num, py, px, lev, SUMMON_HI_DRAGON, summon_mode))
				{
					if(cast) msg_print("���Ȃ��͌Ñ㗳�����������B");
					else     msg_print("�Ñ�̗��͂��Ȃ��Ɍ����ċ��낵�����K���������I");
				}
			}
		}
		break;


	case 31:
#ifdef JP
//���ɓ|�������̂������ɌĂ񂾂牽���g���u���N����H
		if (name) return "����J��";
		if (desc) return "���z���̖��̂���ҒB��F�D�I�ȏ�Ԃŏ�������B������{����A�����Ȏ��قǑ�l�������������B�r�����s���ɂ͒N������Ȃ��B";
#else
		if (name) return "Trump Ancient Dragon";
		if (desc) return "Summons an ancient dragon.";
#endif
    
		{
			int lev = 99;
			if (info) return format("�������x���F%d",lev);
			if (cast)
			{
				int num = 1;
				int cost;
				summon_mode |= (PM_FORCE_FRIENDLY | PM_ALLOW_UNIQUE);

				item_tester_tval = TV_ALCOHOL;
				if(!select_pay(&cost)) return NULL;
				
				num += cost / 1000;

				if(num > 50) num = 50;

				msg_print("���Ȃ��͉���̊J�Â�錾�����I");
				new_summoning(num, py, px, lev, SUMMON_PHANTASMAGORIA, summon_mode);
			}
		}
		break;
	}

	return "";
}








/*:::�V�̈�@�_��*/
static cptr do_new_spell_mystic(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{

	case 0:
#ifdef JP
		if (name) return "���V";
		if (desc) return "��莞�ԕ��V��ԂɂȂ�B";
#endif
    
		{
			int base = 25 + plev / 2;
			int sides = 25 + plev / 2;
			if (info) return info_duration(base, sides);
			if (cast)
			{
				set_tim_levitation(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�����X�^�[���m";
		if (desc) return "�߂��̑S�Ă̌����郂���X�^�[�����m����B���x��20�ȏ�Ō����Ȃ������X�^�[�����m����B";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
				if(plev > 19) detect_monsters_invis(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�V���[�g�E�e���|�[�g";
		if (desc) return "�ߋ����̃e���|�[�g������B";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;



	case 3:
#ifdef JP
		if (name) return "��������";
		if (desc) return "���鋅�𐶐����A�����镔���𖾂邭����B";
#else
		if (name) return "Light Area";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{

			if (cast)
			{
				msg_print("������ݒu�����B");
				lite_room(py,px);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�Ö��p";
		if (desc) return "�����X�^�[1�̂𖰂点��B���x��30�ȏ�Ŏ��E���S�Ẵ����X�^�[�𖰂点��B��R�����Ɩ����B";
#else
		if (name) return "Sleep Monster";
		if (desc) return "Attempts to sleep a monster.";
#endif
    
		{
			//v1.1.90 ����
			int power = plev + 30;

			if (info) return info_power(power);

			if (cast)
			{
				if(plev < 30)
				{

					if (!get_aim_dir(&dir)) return NULL;
					sleep_monster(dir, plev);
				}
				else sleep_monsters(power);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�e���|�[�g";
		if (desc) return "�������̃e���|�[�g������B";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif
    
		{
			int range = plev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range, 0L);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�n�`���m";
		if (desc) return "���ӂ̒n�`�����m����B";
#else
		if (name) return "Magic Mapping";
		if (desc) return "Maps nearby area.";
#endif
    
		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���͏[�U";
		if (desc) return "��/���@�_�̏[�U�񐔂𑝂₷���A�[�U���̃��b�h�̏[�U���Ԃ����炷�B";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			int power = plev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "��ʓV�̏p";
		if (desc) return "��莞�Ԉړ����x���㏸����B";
#endif
    
		{
			int base = 25 + plev / 2;
			int sides =25 + plev / 2;
			if (info) return info_duration(base, sides);
			if (cast)
			{
				set_tim_speedster(randint1(sides) + base, FALSE);
			}
		}
		break;


	case 9:
#ifdef JP
		if (name) return "�Ӓ�";
		if (desc) return "�A�C�e�������ʂ���B";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif
    
		{
			if (cast)
			{
				if (!ident_spell(FALSE)) return NULL;
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�e���|�[�g�E�����X�^�[";
		if (desc) return "�����X�^�[���e���|�[�g������r�[������B��R�����Ɩ����B";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif
    
		{
			int power = plev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�S���ŋp";
		if (desc) return "��莞�ԑω΂𓾂�B�����i�̑ϐ��ɗݐς���B";
#endif
    
		{
			int base = 15 + plev / 2;
			int sides = 5 + plev / 2;
			if (info) return info_duration(base, sides);
			if (cast)
			{
				msg_print("���_���W�������E�E");
				set_oppose_fire(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�g�̉���";
		if (desc) return "��莞�ԁA���ׂĂ̍s���̑��x���㏸����B";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif
    
		{
			int base = plev;
			int sides = 20 + plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�������";
		if (desc) return "�����邢�̓W���[�X����u����v�ɂ���B";
#else
		if (name) return "Haste Self";
		if (desc) return "Hastes you for a while.";
#endif
    
		{

			if (cast)
			{
				cptr q,s;
				int item;
				object_type forge;
				object_type *q_ptr = &forge;

				item_tester_hook = item_tester_hook_make_reisyu;
#ifdef JP
				q = "�ǂ�����ɂ��܂���? ";
				s = "���Ȃ��͎��ɂ���̂ɓK���Ȉ��ݕ��������Ă��Ȃ��B";
#endif
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);	
				}

				msg_print("���Ȃ��̎�̒��ŕr���F���ȍ��������n�߂��E�E");
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_REISYU));
				apply_magic(q_ptr, 1, AM_NO_FIXED_ART | AM_FORCE_NORMAL);
				(void)drop_near(q_ptr, -1, py, px);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�S���m";
		if (desc) return "�߂��̑S�Ẵ����X�^�[�A㩁A���A�K�i�A����A�����ăA�C�e�������m����B";
#else
		if (name) return "Detection True";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;


	case 15:
#ifdef JP
		if (name) return "�_�i����";
		if (desc) return "�_�i�������݂�z���Ƃ���1�̏�������B�������ꂽ�_�i���G�΂��邱�Ƃ�����A�G�΂���m���͌����Ɩ��͂ɂ��B";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif
    
		{
			if (cast)
			{
				bool pet = TRUE;
				u32b mode = 0L;
				int slev = plev + randint1(plev/2);

				if((adj_general[p_ptr->stat_ind[A_CHR]] + adj_general[p_ptr->stat_ind[A_WIS]]) < randint1(50)) pet = FALSE;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= PM_NO_PET;
				if (!pet) slev += randint1(plev);
				if (!pet) mode |= (PM_ALLOW_UNIQUE);

				if (summon_specific((pet ? -1 : 0), py, px, slev, SUMMON_DEITY, mode))
				{
#ifdef JP
					msg_print("��Ɉٗl�ȋC�z���������E�E�B");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif
					if (pet)
					{
#ifdef JP
						msg_print("�_���������̂����Ȃ�����삷�邽�߂Ɍ��������B");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("�_���������̂����Ȃ��̘����𔱂��邽�߂Ɍ��������I");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("�_�i�͌���Ȃ������B");
#else
					msg_print("No demons arrive.");
#endif
				}
				break;
			}
		}
		break;


	case 16:
#ifdef JP
		if (name) return "�Ŕj";
		if (desc) return "���E���̃����X�^�[�̑����A�c��̗́A�ő�̗́A�X�s�[�h�A���₵���e�̐��̂�m��B";
#else
		if (name) return "Probing";
		if (desc) return "Proves all monsters' alignment, HP, speed and their true character.";
#endif
    
		{
			if (cast)
			{
				probing();
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�򉍂̏p";
		if (desc) return "�s�������Ƃ̂���X�ֈړ�����B�n��ɂ���Ƃ������g���Ȃ��B";
#else
		if (name) return "Teleport to town";
		if (desc) return "Teleport to a town which you choose in a moment. Can only be used outdoors.";
#endif
    
		{
			if (cast)
			{
				if (!tele_town(TRUE)) return NULL;
			}
		}
		break;


	case 18:
#ifdef JP
		if (name) return "���p";
		if (desc) return "���E���̃����X�^�[�������A���|�A�����A�N�O������B";
#else
		if (name) return "Detect items and Treasure";
		if (desc) return "Detects all treasures and items in your vicinity.";
#endif
    
		{
			int power = plev * 3;
			if (info) return info_power(power);

			if (cast)
			{
				slow_monsters(power);
				stun_monsters(power);
				confuse_monsters(power);
				turn_monsters(power);
			}
		}
		break;



	case 19:
#ifdef JP
		if (name) return "*�Ӓ�*";
		if (desc) return "�A�C�e���̎��\�͂����S�ɒm��B";
#else
		if (name) return "Identify True";
		if (desc) return "*Identifies* an item.";
#endif
    
		{
			if (cast)
			{
				if (!identify_fully(FALSE)) return NULL;
			}
		}
		break;


	case 20:
#ifdef JP
		if (name) return "���O�̏p";
		if (desc) return "HP���񕜂��A�����x�𑝉������A�؂菝�E�ŁE�N�O�E���o����������B";
#else
		if (name) return "Self Knowledge";
		if (desc) return "Gives you useful info regarding your current resistances, the powers of your weapon and maximum limits of your stats.";
#endif
    
		{
			int heal = plev * 4;
			if (info) return info_heal(0, 0, heal);
			if (cast)
			{

				hp_player(heal);
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
				set_image(0);
				set_food(p_ptr->food + 2000);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�ǔ���";
		if (desc) return "��莞�ԁA�����������ǂ�ʂ蔲������悤�ɂȂ�B";
#else
		if (name) return "Walk through Wall";
		if (desc) return "Gives ability to pass walls for a while.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_kabenuke(randint1(base) + base, FALSE);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�A�҂̎���";
		if (desc) return "�n��ɂ���Ƃ��̓_���W�����̍Ő[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�����̔�";
		if (desc) return "�Z�������̎w�肵���ꏊ�Ƀe���|�[�g����B";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif
    
		{
			int range = p_ptr->lev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("�����̔����J�����B�ړI�n��I��ŉ������B");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(D_DOOR_NORMAL)) return NULL;
			}
		}
		break;




	case 24:
#ifdef JP
		if (name) return "�~�_�X���̎�";
		if (desc) return "�A�C�e��1������ɕς��A�����Ƃ��ē��肷��B���z�͂��̃A�C�e���̉��l��1/3���x�ƂȂ�B";
#else
		if (name) return "Alchemy";
		if (desc) return "Turns an item into 1/3 of its value in gold.";
#endif
    
		{
			if (cast)
			{
				if (!alchemy()) return NULL;
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�C�z�Ւf";
		if (desc) return "��莞�ԁA���̖͂������G���N���ɂ����Ȃ�B";
#else
		if (name) return "Explosive Rune";
		if (desc) return "Sets a glyph under you. The glyph will explode when a monster moves on it.";
#endif
    
		{
			int base = plev / 2;
			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_stealth(randint1(base) + base, FALSE);
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "���͏���";
		if (desc) return "��莞�Ԙr�͂𑝉�������B�ʏ�̌��E�l�𒴂���B";
#else
		if (name) return "Telekinesis";
		if (desc) return "Pulls a distant item close to you.";
#endif
    
		{
			int base = plev/2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_format("�펯�O��̗͂����Ȃ��̘r�ɏh�����C������B");
				set_tim_addstat(A_STR,102 + plev/10 ,base + randint1(base),FALSE);

			}
		}
		break;


	case 27:
#ifdef JP
		if (name) return "�痢��";
		if (desc) return "���̊K�S�̂��i�v�ɏƂ炵�A�_���W���������ׂẴA�C�e�������m����B����ɁA��莞�ԃe���p�V�[�\�͂𓾂�B";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{

				wiz_lite(FALSE);

				if (!p_ptr->telepathy)
				{
					set_tim_esp(randint1(sides) + base, FALSE);
				}
			}
		}
		break;


	case 28:
#ifdef JP
		if (name) return "��Ԃ�";
		if (desc) return "��莞�ԁA�G�����������{���g�n���@�����m���Œ��˕Ԃ���悤�ɂȂ�B";
#else
		if (name) return "Walk through Wall";
		if (desc) return "Gives ability to pass walls for a while.";
#endif
    
		{
			int base = plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_reflect(randint1(base) + base, FALSE);
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "���p�̏p";
		if (desc) return "�Z���ԁA�������̕���ɗ��͂̔\�͂��t�������B";
#else
		if (name) return "Insanity Circle";
		if (desc) return "Generate balls of chaos, confusion and charm centered on you.";
#endif
    
		{
			int base = 5 + plev / 10;

			if (info) return info_duration(base, base);

			if (cast)
			{

				if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
				{
#ifdef JP
					msg_format("����������Ă��Ȃ��B");
#endif
					return FALSE;
				}
				set_ele_attack(ATTACK_FORCE, base + randint1(base));
			}
		}
		break;
	case 30:
#ifdef JP
		if (name) return "�����̋�";
		if (desc) return "�����Z���ԁA�_���[�W���󂯂Ȃ��Ȃ�o���A�𒣂�B�؂ꂽ�u�Ԃɏ����^�[���������̂Œ��ӁB";
#else
		if (name) return "Globe of Invulnerability";
		if (desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks or duration time is exceeded.";
#endif
    
		{
			int base = 4;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_invuln(randint1(base) + base, FALSE);
			}
		}
		break;


	case 31:
#ifdef JP
		if (name) return "�S�_�ω�";
		if (desc) return "�Z���ԋ��剻����B����܂łɂ������Ă����ꎞ���ʂ͑S�ĉ��������B���剻���͐g�̔\�͂������I�ɏ㏸���邪���@���Ɗ����Ɩ�����g�p�ł��Ȃ��BU�R�}���h�ŋ��剻�������ł���B";
#else
		if (name) return "Globe of Invulnerability";
		if (desc) return "Generates barrier which completely protect you from almost all damages. Takes a few your turns when the barrier breaks or duration time is exceeded.";
#endif
    
		{
			int base = 5 + plev / 10;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int percentage;

				reset_tim_flags();
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				set_mimic(base + randint1(base), MIMIC_GIGANTIC, FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= (PR_HP | PR_STATUS | PR_CUT | PR_STUN );
				redraw_stuff();
				break;
			}
		}
		break;





	}

	return "";
}


/*:::�V�j�ח̈�*/
static cptr do_new_spell_punish(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{
	case 0:
		if (name) return "�������̂܂��Ȃ�";
		if (desc) return "1�̂̃����X�^�[�ɔj�ב����̃_���[�W��^�����|�E�\�͒ቺ������B��R�����Ɩ����B�j�׎�_�łȂ������X�^�[�ɂ͌��ʂ��Ȃ��B";
    
		{
			int dice = 3 + (plev) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_PUNISH_1, dir, damroll(dice, sides), 0);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "���׊��m";
		if (desc) return "�߂��̍��ׂ̐��͂ɑ����郂���X�^�[�����m����B";
#else
		if (name) return "Detect Evil";
		if (desc) return "Detects all evil monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_evil(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�j��";
		if (desc) return "��莞�ԁA��������AC�Ƀ{�[�i�X�𓾂�B";
#endif
    
		{
			int base = 15+plev/5;
			if (info) return info_duration(base, base);
			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 3:
		if (name) return "��D����";
		if (desc) return "�˒��̂��Z���j�ב����̃{���g����B";
    
		{
			int dice = 5 + plev / 10;
			int sides = 8;
			int range = 6 + plev / 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				project_length = range;
				if (!get_aim_dir(&dir)) return NULL;
				msg_print("��D�𓊂����I");
				fire_bolt(GF_HOLY_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�����̎��F";
		if (desc) return "��莞�ԁA�����ȓG��������悤�ɂȂ�B";
#else
		if (name) return "Sense Unseen";
		if (desc) return "Gives see invisible for a while.";
#endif
    
		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;
	case 5:
#ifdef JP
		if (name) return "�m�C���g";
		if (desc) return "��莞�ԁA���|����菜�����|�ϐ��𓾂Ĕ����\�͂������㏸����B";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�X�^�[�_�X�g";
		if (desc) return "�^�[�Q�b�g�t�߂ɑM���̃{���g��A�˂���B";
#else
		if (name) return "Star Dust";
		if (desc) return "Fires many bolts of light near the target.";
#endif
    
		{
			int dice = 3 + (plev - 1) / 9;
			int sides = 2;

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_blast(GF_LITE, dir, dice, sides, 10, 3, 0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "����";
		if (desc) return "�A�C�e���ɂ��������ア�􂢂���������B";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif
    
		{
			if (cast)
			{
				if (remove_curse())
				{
#ifdef JP
					msg_print("�����i�ɂ��������������������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;



	case 8:
#ifdef JP
		if (name) return "����";
		if (desc) return "�̗͂𒆒��x�񕜂����A�؂菝�����Â���B";
#else
		if (name) return "Cure Critical Wounds";
		if (desc) return "Heals cut, stun and HP greatly.";
#endif
    
		{
			int dice = 4 + plev / 4;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}
		break;





	case 9:
#ifdef JP
		if (name) return "�Ј�";
		if (desc) return "�����X�^�[1�̂����|������B��R�����Ɩ����B";
#else
		if (name) return "Scare Monster";
		if (desc) return "Attempts to scare a monster.";
#endif
    
		{
			int power = plev * 3 / 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fear_monster(dir, power);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "���Ȃ����";
		if (desc) return "�j�ב����̃{�[��������čU������B�����@�E���g���Ə����З͂������B";
#else
		if (name) return "Holy Orb";
		if (desc) return "Fires a ball with holy power. Hurts evil monsters greatly, but don't effect good monsters.";
#endif
    
		{
			int dice = 3;
			int sides = 4 + plev / 12;
			int rad = (plev < 30) ? 2 : 3;
			int base;
			if (cp_ptr->magicmaster)
				base = plev * 2;
			else
				base = plev * 3 / 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HOLY_FIRE, dir, damroll(dice, sides) + base, rad);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�ޖ��̎���";
		if (desc) return "���E���̃A���f�b�h�E�����E�d���Ƀ_���[�W��^���N�O�Ƃ�����B";
#else
		if (name) return "Exorcism";
		if (desc) return "Damages all undead and demons in sight, and scares all evil monsters in sight.";
#endif
    
		{
			int dam = plev;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				dispel_undead(dam);
				dispel_demons(dam);
				dispel_kwai(dam);
			}
		}
		break;
	case 12:
#ifdef JP
		if (name) return "�������";
		if (desc) return "�꒼����̑S�Ă�㩂Ɣ���j�󂷂�B";
#else
		if (name) return "Unbarring Ways";
		if (desc) return "Fires a beam which destroy traps and doors.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				destroy_door(dir);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�f�B�X�y��";
		if (desc) return "�A���f�b�h��̂𒆊m���ňꌂ�œ|���B��R�����Ɩ����B�^�[�Q�b�g���߂��قǌ����₷���B";
#else
		if (name) return "Dispel Undead";
		if (desc) return "Damages all undead monsters in sight.";
#endif
    
		{
			if (cast)
			{
				int y, x;
				int dist;
				int damage;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_aim_dir(&dir)) return NULL;

				y = target_row;
				x = target_col;
				m_ptr = &m_list[target_who];

				if (cave[y][x].m_idx && (m_ptr->ml) )
				{
					int power;
					char m_name[80];	
					r_ptr = &r_info[m_ptr->r_idx];
					monster_desc(m_name, m_ptr, 0);
					power = p_ptr->lev * 3 / 2 - m_ptr->cdis * 4;
					if(power<1) power=1;

					///mod140324 �{�Ƃɕ����5%�̐����ۏ؂�K�p����B����قǓK�����߂��Ƃ��ĉ������X�Ƃ����C�����邪�B
					msg_format("�V��������~�蒍�����I");
					if(r_ptr->flags3 & RF3_UNDEAD && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) && ( power / 2 + randint1( power / 2 ) > r_ptr->level || one_in_(20)))
					{
						msg_format("%s�͌��̒��ɏ������B",m_name);
						damage = m_ptr->hp + 1;
						project(0,0,m_ptr->fy,m_ptr->fx,damage,GF_DISP_ALL,PROJECT_KILL,-1);
					}
					else if(r_ptr->flags3 & RF3_UNDEAD)
					{
						msg_format("%s�͉���ɒ�R�����I",m_name);
					}
					else 
					{
						msg_format("%s�ɂ͑S�������Ă��Ȃ��悤���B",m_name);
					}
				}
				else
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}
			}
		}
		break;



		//v1.1.85 �ق��̗����u�V�Ȃ鍌���v�ɖ��̕ύX���A���_����+���a1�d���{�[�����u�����ɂ����B�l�^���Ƃ��Ă�DQ�̃f�C���n�B
	case 14:
#if 0
#ifdef JP
		if (name) return "�ق��̗�";
		if (desc) return "���͂ȓd���̃{���g����B";
#else
		if (name) return "Judgment Thunder";
		if (desc) return "Fires a powerful bolt of lightning.";
#endif

		{
			int dam = plev * 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt(GF_ELEC, dir, dam);
			}
		}
		break;
#else
		if (name) return "�V�Ȃ鍌��";
		if (desc) return "�w�肵���ʒu�ɗ��_���Ăяo���A���͂ȓd�������U�����s���B";
		{
			int dam = plev * 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;

				if (!fire_ball_jump(GF_MAKE_STORM, dir, 2, 1, "���_���Ăяo�����I")) return NULL;
				fire_ball_jump(GF_ELEC, dir, dam, 1, NULL);


			}
		}
		break;

#endif

	case 15:
#ifdef JP
		if (name) return "���Ȃ�䌾�t";
		if (desc) return "���E���̍��ׂ̐��͂ɑ����郂���X�^�[�Ƀ_���[�W��^���A�̗͂��񕜂��A�ŁA���|�A�N�O��ԁA��������S������B";
#else
		if (name) return "Holy Word";
		if (desc) return "Damages all evil monsters in sight, heals HP somewhat, and completely heals poison, fear, stun and cut status.";
#endif
    
		{
			int dam_sides = plev * 6;
			int heal = 100;

#ifdef JP
			if (info) return format("��:1d%d/��%d", dam_sides, heal);
#else
			if (info) return format("dam:d%d/h%d", dam_sides, heal);
#endif

			if (cast)
			{
				dispel_evil(randint1(dam_sides));
				hp_player(heal);
				set_afraid(0);
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
			}
		}
		break;



	case 16:
#ifdef JP
		if (name) return "����";
		if (desc) return "���ׂ̐��͂ɑ����郂���X�^�[�̓������~�߂�B";
#endif
    
		{
			int power = plev * 7 / 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				stasis_evil(dir);
			}
		}
		break;


	case 17:
#ifdef JP
		if (name) return "�Ύ׈����E";
		if (desc) return "���ׂ̐��͂̃����X�^�[�̍U����h���o���A�𒣂�B";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif
    
		{
			int base = 25;
			int sides = 3 * plev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_protevil(randint1(sides) + sides, FALSE);
			}
		}
		break;
	case 18:
#ifdef JP
		if (name) return "�j�ׂ̈�";
		if (desc) return "1�̂̃����X�^�[�ɔj�ב����̑�_���[�W��^�����|�E�N�O�E�\�͒ቺ������B��R�����Ɩ����B�j�׎�_�łȂ������X�^�[�ɂ͌��ʂ��Ȃ��B";
#endif
    
		{
			int dice = 15 + plev / 5;
			int sides = 15;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball_hide(GF_PUNISH_4, dir, damroll(dice, sides), 0);
			}
		}
		break;


	case 19:
#ifdef JP
		if (name) return "*����*";
		if (desc) return "�A�C�e���ɂ����������͂Ȏ􂢂���������B";
#else
		if (name) return "Dispel Curse";
		if (desc) return "Removes normal and heavy curse from equipped items.";
#endif
    
		{
			if (cast)
			{
				if (remove_all_curse())
				{
#ifdef JP
					msg_print("�����i�ɂ��������������������B");
#else
					msg_print("You feel as if someone is watching over you.");
#endif
				}
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "���Ȃ�I�[��";
		if (desc) return "��莞�ԁA�j�ב����̃I�[����g�ɂ܂Ƃ��B";
#else
		if (name) return "Holy Aura";
		if (desc) return "Gives aura of holy power which injures evil monsters which attacked you for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_sh_holy(randint1(base) + base, FALSE);
			}
		}
		break;
	case 21:
#ifdef JP
		if (name) return "���Ȃ��";
		if (desc) return "�j�ב����̃{���g����B���x��45�ȍ~�̓r�[������B";
#else
		if (name) return "Judgment Thunder";
		if (desc) return "Fires a powerful bolt of lightning.";
#endif
    
		{
			int dam = plev * 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				if(plev < 45) fire_bolt(GF_HOLY_FIRE, dir, dam);
				else fire_beam(GF_HOLY_FIRE, dir, dam);
			}
		}
		break;





	case 22:
#ifdef JP
		if (name) return "�׈��Ǖ�";
		if (desc) return "���E���̍��ׂ̐��͂ɑ����郂���X�^�[��S�ăe���|�[�g������B��R�����Ɩ����B";
#else
		if (name) return "Banish Evil";
		if (desc) return "Teleports all evil monsters in sight away unless resisted.";
#endif
    
		{
			int power = 50 + plev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (banish_evil(power))
				{
#ifdef JP
					msg_print("�_���ȗ͂��׈���ł��������I");
#else
					msg_print("The holy power banishes evil!");
#endif

				}
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�M���̗�";
		if (desc) return "����ȑM���̋�����B";
#else
		if (name) return "Star Burst";
		if (desc) return "Fires a huge ball of powerful light.";
#endif
    
		{
			int dam = 100 + plev * 3;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_LITE, dir, dam, rad);
			}
		}
		break;


	case 24:
#ifdef JP
		if (name) return "�V�Ζ�ʖ��~��";
		if (desc) return "�V�Ζ�ʖ�(���܂̂���Ƃ킯�݂̂���)�����̐g�ɍ~�낵�A���͑S�Ă̒n�`�A�A�C�e���A�����X�^�[������������B";
#else
		if (name) return "Armageddon";
		if (desc) return "Destroy everything in nearby area.";
#endif
		{
			int rad = 3 + plev / 4;
			if (info) return format("�͈́F%d",rad);

			if (cast)
			{
				set_kamioroshi(KAMIOROSHI_AMATO, 0);
				destroy_area(py, px, rad, FALSE, TRUE,FALSE);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "�΋ÉW���~��";
		if (desc) return "�΋ÉW��(��������ǂ߂݂̂���)�����̐g�ɍ~�낷�B��莞�ԁA�M���U���ɑ΂��銮�S�ȖƉu�Ɣ��˔\�͂𓾂�B";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 15 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print("�΋ÉW��������A�_�����f�����I");
				set_kamioroshi(KAMIOROSHI_ISHIKORIDOME, randint1(base) + base);
			}
		}
		break;
	case 26:
#ifdef JP
		if (name) return "�эj�����~��";
		if (desc) return "�эj����(���ÂȂ��񂰂�)�����̐g�ɍ~�낷�B��莞�ԁA���ڍU���ɂ���č��ׂ̐��͂̃����X�^�[�ɑ傫�ȃ_���[�W��^������悤�ɂȂ�B";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 10 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print("�ѓꌠ���̌�@�̗͂��g�ɏh�����I");
				set_kamioroshi(KAMIOROSHI_IZUNA, randint1(base) + base);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�ɓ��\���~��";
		if (desc) return "�ɓ��\��(���Â̂�)�����̐g�ɍ~�낵�A���E���S�Ăɑ΂��ċ��͂Ȕj�ב����U�����s���A����Ɏ��ꂽ���������􂷂�B";
#endif
		{
			int dice = 1;
			int sides = plev * 3;
			int base = plev * 3;
			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				set_kamioroshi(KAMIOROSHI_IZUNOME, 0);
				msg_print("��̉��Ƌ��Ƀ_���W�����ɐ���ȋ�C���������E�E");		
				project_hack2(GF_HOLY_FIRE, dice,sides,base);
				if (remove_all_curse())
				{
					msg_print("�����i�ɂ��������������������B");
				}
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "�Z�g�O�_�~��";
		if (desc) return "�㓛�j���i����̂��݂̂��Ɓj�A�����j���i�Ȃ��̂��݂̂��Ɓj�A�ꓛ�j���i�����̂��݂̂��Ɓj�����̐g�ɍ~�낷�B��莞�ԁA�����A�����ړ��\�́A���ϐ��A����U���ϐ��A�x���𓾂�B";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 25 + plev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print("�Z�g�O�_�̉���𓾂��I");
				set_kamioroshi(KAMIOROSHI_SUMIYOSHI, randint1(base) + base);
			}
		}
		break;


	case 29:
#ifdef JP
		if (name) return "���������~��";
		if (desc) return "���������i���������񂰂�j�����̐g�ɍ~�낷�B��莞�ԁA���ɑ΂��銮�S�ȖƉu�Ƌ��͂ȉΉ��̃I�[���𓾂ĉΉ������U�����ł���悤�ɂȂ�B";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 5 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print("�����l�̐_�΂��g�ɏh�����I");
				set_kamioroshi(KAMIOROSHI_ATAGO, randint1(base) + base);
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "�V�F�󔄖��~��";
		if (desc) return "�V�F�󔄖��i���߂̂����߂݂̂��Ɓj�����̐g�ɍ~�낷�B��莞�ԁA�Í��E�n���E�n���̋ƉΑ����̍U���ւ̑ϐ��AAC�㏸�A���@�h��㏸�A���͑啝�㏸�𓾂�B";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 10 + plev / 5;

			if (info) return info_duration(base, base);

			if (cast)
			{
				msg_print("���Ȃ��͌y�₩�ɕ����n�߂��I");
				set_kamioroshi(KAMIOROSHI_AMENOUZUME, randint1(base) + base);
			}
		}
		break;


	case 31:
#ifdef JP
		if (name) return "�V�Ƒ��_�~��";
		if (desc) return "�V�Ƒ��_(���܂Ă炷�����݂���)�����̐g�ɍ~�낵�A���͂̓G�ɑ�Ō���^����B�����̋߂��ɂ��郂���X�^�[�قǌ��ʂ��傫���B�V�F�󔄖����~�낵�Ă���Ƃ��Ɏg���Ƃ���Ɍ��ʂ������B";
#endif
		{

			if (info) return format("");

			if (cast)
			{
				call_amaterasu();
			}
		}
		break;


	}

	return "";
}





/*:::�V�̈�@�I�J���g*/
static cptr do_new_spell_occult(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;
	bool fail = (mode == SPELL_FAIL) ? TRUE : FALSE;

	u32b summon_mode = 0L;

#ifdef JP
	static const char s_random[] = "�����_��";
#else
	static const char s_random[] = "random";
#endif

	int dir;
	int plev = p_ptr->lev;
	if (p_ptr->pclass == CLASS_YATSUHASHI && music_singing(MUSIC_NEW_TSUKUMO_ECHO)) plev = plev * 3 / 2;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK) plev *= 2;

	switch (spell)
	{

	case 0:
		if (name) return "�����{�����Y��";
		if (desc) return "��{�����Y�����삯�čs���ă^�[�Q�b�g�֑̓����肷��B�������Â����␅�n�`�Ȃǂ��z�����Ȃ��B";
    
		{
			int wgt = 200 + plev * 8;

			if (info) return format("�d��:%d.%02d kg",wgt/20,wgt%20);

			if (cast)
			{
				object_type forge;
				object_type *tmp_o_ptr = &forge;
				int ty,tx;

				if (!get_aim_dir(&dir)) return NULL;
				tx = px + 99 * ddx[dir];
				ty = py + 99 * ddy[dir];
				if ((dir == 5) && target_okay())
				{
					tx = target_col;
					ty = target_row;
				}
				//�_�~�[�A�C�e���𐶐����d�ʂ�ݒ肵����ˌ����[�`���֓n��
				msg_print("��{�����Y�����킯�o�����I");
				object_prep(tmp_o_ptr,lookup_kind(TV_MATERIAL,SV_MATERIAL_NINOMIYA));
				tmp_o_ptr->weight = wgt;
				do_cmd_throw_aux2(py,px,ty,tx,2,tmp_o_ptr,6);			

			}
		}
		break;

	case 1:
		if (name) return "�������肳��";
		if (desc) return "$10����ċ߂��̃����X�^�[�����m����B���x��15�ȍ~��㩂����m��$50����A���x��25�ȍ~�̓A�C�e�������m����$100�����B���̖��@�Ɏ��s�����Ƃ��H��n�̃����X�^�[�ɏP����B";
		{
			int rad = DETECT_RAD_DEFAULT;
			int price;
			if (info) return info_radius(rad);

			if (cast)
			{
				if(plev < 15) price = 10;
				else if(plev < 24) price = 50;
				else price = 100;

				if(p_ptr->au < price)
				{
					msg_print("����������Ȃ��B");
					return NULL;
				}
				msg_print("�������肳��Ɏ��͂̏󋵂��������...");
				detect_monsters_normal(rad);
				detect_monsters_invis(rad);
				if(plev > 14) detect_traps(rad,TRUE);
				if(plev > 24) detect_objects_normal(rad);
				p_ptr->au -= price;
				p_ptr->redraw |= PR_GOLD;	

			}
			else if(fail)
			{
				if (summon_specific(0, py, px, MAX(dun_level,10), SUMMON_GHOST, (PM_ALLOW_UNIQUE | PM_NO_PET)))
					msg_print("�ԈႦ�ēG�ΓI�ȗ���Ă�ł��܂����I");

			}
		}
		break;


	case 2:
		if (name) return "�������̍s��";
		if (desc) return "�������̃����^���������ɔR����⋋����B";
    
		{
			if (cast)
			{
				int max_flog = 0;
				object_type * o_ptr = &inventory[INVEN_LITE];

				if(o_ptr->name2 == EGO_LITE_DARKNESS)
				{
					msg_print("�����͈Â��܂܂�...");
					break;
				}
				if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_LANTERN))
				{
					msg_print("�����^���͐��x�܂������A�������Ȍ�������n�߂��B");
					max_flog = FUEL_LAMP;
				}
				else if ((o_ptr->tval == TV_LITE) && (o_ptr->sval == SV_LITE_TORCH))
				{
					msg_print("���������X�ƌ������B");
					max_flog = FUEL_TORCH;
				}
				else
				{
					msg_print("�R�����v������𑕔����Ă��Ȃ��B");
					return NULL;
				}

				o_ptr->xtra4 += (max_flog / 2);
				if (o_ptr->xtra4 >= max_flog) o_ptr->xtra4 = max_flog;
				p_ptr->update |= (PU_TORCH);
			}
		}
		break;
	
	case 3:
		if (name) return "�Z���搶�̏ё���";
		if (desc) return "�אڂ����ǂɃ����X�^�[�u�Z���搶�̏ё���v��z���Ƃ��Đݒu����B";
    
		{
			if (cast)
			{
				int y,x;
				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!cave_have_flag_bold(y,x,FF_HURT_ROCK))
				{
					msg_print("�����ɂ͏���Ȃ��B");
					return NULL;
				}
				if(cave_have_flag_bold(y,x,FF_DOOR))
				{
					msg_print("�O���O�����ď���Ȃ��B");
					return NULL;
				}

				if (cave[y][x].m_idx )
				{
					monster_type *m_ptr = &m_list[cave[y][x].m_idx];
					if(m_ptr->ml) msg_print("�����ɂ̓����X�^�[������I");
					else msg_print("���܂�����Ȃ������B�������ǂ̗l�q�����������B");
					break;
				}
				if(summon_named_creature(0, y, x, MON_PORTRAIT, PM_FORCE_PET))
					msg_print("�ё�����������B");
				else
					msg_print("���s�����B");

			}
		}
		break;


	case 4:
		if (name) return "�u���Ă��@";
		if (desc) return "�A�C�e�����E���Ď����Ă���G�����n�`�ɗאڂ��Ă���Ƃ��ɂ����g���Ȃ��B�G��̂ɐ������̑�_���[�W��^���ăA�C�e���𗎂Ƃ����A���j�[�N�����X�^�[�łȂ��ꍇ���m���ňꌂ�œ|���B";
    
		{
			int i;
			char m_name[80];
			int x,y,tx,ty;
			monster_type *m_ptr;
			monster_race *r_ptr;
			bool flag_water = FALSE;
			int dam = 200 + plev * 4;
			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			if (info) return info_damage(0, 0, dam);
			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
					return NULL;
				}

				y = target_row;
				x = target_col;
				m_ptr = &m_list[cave[y][x].m_idx];

				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);

				if(!m_ptr->hold_o_idx)
				{
					msg_format("%s�͉��������Ă��Ȃ��B",m_name);
					return NULL;
				}

				for (i = 0; i < 8; i++)
				{
					ty = y + ddy_ddd[i];
					tx = x + ddx_ddd[i];
					if(!in_bounds(ty,tx)) continue;
					if(cave_have_flag_bold(ty,tx,FF_WATER)) 
					{
						flag_water = TRUE;
						break;
					}
				}
				if(!flag_water)
				{
					msg_format("%s�̋߂��ɐ��ꂪ�Ȃ��B",m_name);
					return NULL;
				}
				msg_print("�ǂ�����Ƃ��Ȃ��u�u�E���E�āE���`�v�Ɛ�������..");

				if(!(r_ptr->flagsr & (RFR_RES_WATE | RFR_RES_ALL)) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & (RF7_AQUATIC | RF7_UNIQUE2))
				&& randint1((r_ptr->flags2 & (RF2_POWERFUL | RF2_GIGANTIC))?(dam/4):(dam/2)) > r_ptr->level )
				{
					msg_format("���ʂ�������̎肪�L�сA%s���������荞�񂾁I",m_name);
					dam = m_ptr->hp + 1;

					project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_DISP_ALL,flg,-1);
				}
				else
				{
					msg_format("���ʂ�������̎肪�L�сA%s���������荞�񂾁I",m_name);
					project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_WATER,flg,-1);
					if(m_ptr->r_idx) monster_drop_carried_objects(m_ptr);

				}

			}
		}
		break;


	case 5:
		if (name) return "�g�C���̉Ԏq����";
		if (desc) return "�����h�A�Ɏg���Ă����ƁA���̃h�A���J���悤�Ƃ��������X�^�[�Ƀ_���[�W��^���ĞN�O�Ƃ������m���Ő�����΂��B���ʂ����������h�A�͏�����B�v���C���[���J����ƌ��ʂ����������B";
		{
			int dice = 3 + plev / 12;
			int sides = 77;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				int y,x;
				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!is_closed_door(cave[y][x].feat))
				{
					msg_print("�����ɂ͎g���Ȃ��B");
					return NULL;
				}

				if (cave[y][x].m_idx )
				{
					monster_type *m_ptr = &m_list[cave[y][x].m_idx];
					if(m_ptr->ml) msg_print("�����ɂ̓����X�^�[������I");
					else 		msg_print("���s�����B");
					break;
				}

				cave_set_feat(y, x, f_tag_to_index_in_init("HANAKOSAN_DOOR"));
				msg_print("�h�A�ɏp���{���Ē��莆�������B");
			}
		}
		break;


	case 6:
		if (name) return "�ԃ}���g�}���g�̉��l";
		if (desc) return "�אڂ����l�Ԃ������͐l�Ԍ^������̂ɒn�������̃_���[�W��^���AHP��MP�̂ǂ��炩���z������B";
    
		{
			char m_name[80];
			int tx,ty,x,y;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			int dam;
			bool flag_water = FALSE;
			int base = plev;
			int heal;

			int dice = 8;
			int sides = 4 + plev / 3;

			if (info) return info_damage(dice, sides, base);
			if (cast)
			{

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];
				m_ptr = &m_list[cave[y][x].m_idx];
				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}
				r_ptr = &r_info[m_ptr->r_idx];
				if(!(r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN)))
				{
					msg_format("���ʂ��Ȃ������B");
					break;

				}
				monster_desc(m_name, m_ptr, 0);
				dam = damroll(dice,sides)+base;
				heal = dam;

				if(!(cave[y][x].m_idx % 2)) // ���ʂ����������ɂȂ�悤��m_idx�Ŕ���
				{
					msg_format("�Ԃ��}���g��%s���ݍ��񂾁I",m_name);
					project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_NETHER,flg,-1);
					if(r_ptr->flagsr & RFR_RES_NETH) heal /= 3;
					hp_player(heal);
				}
				else
				{
					msg_format("���}���g��%s���ݍ��񂾁I",m_name);
					project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_NETHER,flg,-1);
					heal /= 2;
					if(r_ptr->flagsr & RFR_RES_NETH) heal /= 3;
					player_gain_mana(heal);

				}

			}
		}
		break;


	case 7:
		if (name) return "�����@";
		if (desc) return "�^�[�Q�b�g�ɑ�_���[�W��^���n�k���N�����B�v���C���[���n�k�͈͓̔��ɂ���Ɗ������܂�ă_���[�W���󂯂邱�Ƃ�����B�^�[�Q�b�g�𒆐S�Ƃ����c��3�O���b�h�ɕǂȂǂ�����Ǝg���Ȃ��B�܂����O�ł͎g���Ȃ��B";
    
		{
			int j;
			char m_name[80];
			int tx,ty,x,y;
			monster_type *m_ptr;
			bool flag_ok = TRUE;
			int i;

			int dam = plev * 6;
			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			if (info) return info_damage(0, 0, dam);
			if (cast)
			{
				if(!dun_level || p_ptr->inside_quest == QUEST_CHAOS_WYRM || 
					p_ptr->inside_quest == QUEST_GRAVEDANCER || p_ptr->inside_quest == QUEST_OOGAME
					|| p_ptr->inside_quest == QUEST_KILL_MOKO || p_ptr->inside_quest == QUEST_LIVE
					|| p_ptr->inside_quest == QUEST_KANASUWA || p_ptr->inside_quest == QUEST_KORI )
				{
					msg_print("�����ł͎g���Ȃ��B");
					return NULL;
				}

				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
					return NULL;
				}

				y = target_row;
				x = target_col;
				m_ptr = &m_list[cave[y][x].m_idx];

				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}

				//�ˌ����ʂ�Ȃ��n�`�A�}�b�v�[�A�o��K�i�A�؂��߂��ɂ���Ǝg���Ȃ�
				//v1.1.82 ���������Ԉ���Ă��̂ŏC��
				//v1.1.82a 1.1.11�ł��̌`�ɒ��������̂�Y��Ă��B���Ƃɖ߂��Đ������̕����C������B
				for (i = -1; i < 1; i++) for (j = -1; j < 1; j++)
				{
					ty = y + i;
					tx = x + j;
					if(!in_bounds2(ty,tx) || !cave_have_flag_bold(ty,tx,FF_PROJECT) || cave_have_flag_bold(ty,tx,FF_LESS)) 
					{
						flag_ok = FALSE;
						break;
					}
				}
				if(!flag_ok)
				{
					msg_format("�����ƍL���ꏊ�łȂ��Ǝg���Ȃ��B");
					return NULL;
				}
				monster_desc(m_name, m_ptr, 0);
				ty = m_ptr->fy;
				tx = m_ptr->fx;
				msg_format("����ȑ��������%s�𓥂ݒׂ����I",m_name);
				project(0,0,ty,tx,dam,GF_MISSILE,flg,-1);
				earthquake(ty, tx, 5);

			}
		}
		break;


	case 8:
		if (name) return "�l�ʌ�";
		if (desc) return "�F�D�I�ȃ����X�^�[�u�l�ʌ��v�������B";
		{
			if(info) return "";

			if(cast)
			{

				if(summon_named_creature(0, py, px, MON_MANFACE_DOG, PM_FORCE_FRIENDLY))
				{
					if(one_in_(10)) msg_print("�u�������̂��ƂȂ�Ăق��Ƃ��Ă���您�E�E�v");
					msg_print("��ȕ��e�̌������ꂽ�B");
				}
				else 
					msg_print("��������Ȃ������B");
			}
			break;
		}
	case 9:
		if (name) return "���ԏ�";
		if (desc) return "�w������̕ǂȂǂ̈ړ��s�n�`�̌������ֈړ�����B�ʏ�̃e���|�[�g�ňړ��ł��Ȃ��ꏊ�ɂ͏o���Ȃ��B�}�b�v�[�܂ňړ��悪�Ȃ��ꍇ�͈ړ��Ɏ��s����B���̖��@�͔��e���|�[�g��Ԃł��g�����Ƃ��ł���B";
    
		{

			if (info) return "";

			if (cast)
			{
				int x,y;
				int attempt = 500;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;
				y = py + ddy[dir];
				x = px + ddx[dir];

				if(cave_have_flag_bold(y,x,FF_MOVE) || cave_have_flag_bold(y,x,FF_CAN_FLY))
				{
					msg_format("�����ɂ͕ǂ��Ȃ��B");
					return NULL;
				}
				while(1)
				{
					attempt--;
					y += ddy[dir];
					x += ddx[dir];

					if(y >= cur_hgt || y < 1 || x >= cur_wid || x < 1)
					{
						msg_format("���Ȃ��͕ǂ̌��Ԃɐg�𝀂����񂾁I");
						msg_print("..���������Ԃ̐�͍s���~�܂肾�����B");
						break;
					}
					if(!cave_player_teleportable_bold(y, x, 0L)) continue;
					msg_format("���Ȃ��͕ǂ̌��Ԃɐg�𝀂����񂾁I");
					//�e���|�j�Q�𖳎����邽�߂ɂ�������NONMAGICAL�ɂ���
					teleport_player_to(y,x,TELEPORT_NONMAGICAL);
					break;
				}
			}
		}
		break;


	case 10:
		if (name) return "���􂯏�";
		if (desc) return "���͂̓G�����|�A����������B���͂������قǌ��ʂ��������A�g�p����Ɩ��͂��ꎞ�I�ɒቺ����B";

		{
			int power = plev + (p_ptr->stat_ind[A_CHR] + 3) * 5;
			if(info) return format("����:%d",power);
			if(cast)
			{
				msg_print("���Ȃ��͋��낵��������I��ɂ���...");
				confuse_monsters(power);
				turn_monsters(power);
				do_dec_stat(A_CHR);
			}
		}
		break;



	case 11:
		if (name) return "�^�[�{�o�o�@";
		if (desc) return "�ꎞ�I�ɍ����ړ��\�͂𓾂�B���x��35�ȍ~�͂���ɉ�������B";
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				int time = randint1(base) + base;
				set_tim_speedster(time, FALSE);

				if(plev > 34) set_fast(time, FALSE);
			}
		}
		break;


	case 12:
		if (name) return "�Ԓ��M���~";
		if (desc) return "�����𒆐S�Ƃ����n�������̋��𔭐�������B���̂Ƃ��U�b�N�̒��̈�ԍ����Ȋ��ꕨ���ЂƂj�󂳂�A���̉��i�ɉ����ă_���[�W���オ��B";
		{
			int dam = 100 + plev * 4;
			int rad;
			int slot;
			int value = 0;
			int i;
			bool flag;
			object_type *o_ptr;
			char o_name[MAX_NLEN];

			if (info) return format("����:�ő�%d�{��",dam/2);

			if (cast)
			{


				for (i = 0; i < INVEN_PACK; i++)
				{
					int tmp_value;
					o_ptr = &inventory[i];
					

					if(!o_ptr->k_idx) continue;
					if(o_ptr->tval != TV_POTION && o_ptr->tval != TV_FLASK && o_ptr->tval != TV_COMPOUND) continue;
					if(o_ptr->tval == TV_FLASK && o_ptr->sval != SV_FLASK_OIL) continue;
					if(o_ptr->tval == TV_COMPOUND && o_ptr->sval == SV_COMPOUND_HOURAI) continue;
					tmp_value = object_value_real(o_ptr);
					tmp_value -= tmp_value * o_ptr->discount / 100;

					if(!tmp_value) continue;
					if(tmp_value < value) continue;

					slot = i;
					value = tmp_value;
				}

				if(!value)
				{
					msg_print("�����N����Ȃ������B");
					break;
				}

				o_ptr = &inventory[slot];

				if(value < 200) ; //�{�[�i�X�Ȃ�
				else if(value < 500) dam += 50 + randint1(100);
				else if(value < 3000) dam += dam + 200 + randint1(200);
				else if(value < 10000) dam += dam * 2 + 300 + randint1(300);
				else if(value < 50000) dam += dam * 4 + 400 + randint1(400);
				else  dam += dam * 9 + 1000;

				if(value < 3000) rad = 2;
				else if(value < 10000) rad = 3;
				else if(value < 50000) rad = 4;
				else rad = 6;
				
				if(o_ptr->number > 1) flag = TRUE;
				else flag = FALSE;

				object_desc(o_name,o_ptr,(OD_OMIT_PREFIX | OD_NAME_ONLY));
				inven_item_increase(slot,-1);
				inven_item_optimize(slot);

				if(flag) msg_format("%s�������Ȃ�...",o_name);
				else 	 msg_format("%s���Ȃ�...",o_name);
//portion_smash_effect������H

				project(0,rad,py,px,dam,GF_NETHER,(PROJECT_KILL|PROJECT_JUMP|PROJECT_GRID),-1);

			}
		}
		break;

	case 13:
		{
			int rad = MAX_SIGHT;

			if (name) return "�����[����̓d�b";
			if (desc) return "���͂̃����_���ȃ����X�^�[�ׂ̗Ƀe���|�[�g���A���̂܂ܗאڍU�����d�|����B���_�������Ȃ������X�^�[�͑ΏۂɂȂ�Ȃ��B";
			if (info) return info_radius(rad);

			if(cast)
			{
				monster_type *m_ptr;
				monster_race *r_ptr;
				int i;

				int num=0;
				int tx,ty;

				msg_print("���Ȃ��͓d�b�������n�߂�...");
				for (i = 1; i < m_max; i++)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if (is_pet(m_ptr)) continue;
					if (is_friendly(m_ptr)) continue;
					if(r_ptr->flags2 & (RF2_WEIRD_MIND | RF2_EMPTY_MIND)) continue;
					if(distance(py,px,m_ptr->fy,m_ptr->fx) > rad) continue;
				
					num++;
					if(!one_in_(num)) continue;

					ty = m_ptr->fy;
					tx = m_ptr->fx;

				}
				if(!num)
				{
					msg_print("�N���d�b�ɏo�Ȃ������B");
				}
				else if(p_ptr->anti_tele)
				{
					msg_print("�e���|�[�g�Ɏ��s�����B");
				}
				else
				{
					teleport_player_to(ty,tx,0L);
					if(distance(py,px,ty,tx)==1)
					{
						char m_name[80];
						monster_desc(m_name, &m_list[cave[ty][tx].m_idx], 0);
						msg_format("���Ȃ���%s�̌��ɏo�������I",m_name);
						py_attack(ty,tx,0);
					}
					else
					{
						msg_print("���܂����ɏo���Ȃ������B");
					}
				}
			}
		}
		break;

	case 14:
		if (name) return "���ڂ���";
		if (desc) return "���̃t���A����Ń����X�^�[�w���ڂ��܁x��z���Ƃ��ď�������B���ڂ��܂͋��͂ȍU�����s�����A�߂��ɐl�Ԃ������͐l�^�����̒j������Ɩ��߂𖳎����Ă�������P���ɍs���B";
		if(info) return "";
		if(cast)
		{
			int i;
			monster_type *m_ptr;
			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if(m_ptr->r_idx == MON_HASSYAKUSAMA)
				{
					msg_print("���ڂ��܂͂��łɂ��̃t���A�ɂ���B");
					return NULL;
				}
			}

			if(summon_named_creature(0, py, px, MON_HASSYAKUSAMA, PM_EPHEMERA))
			{

				if(one_in_(7)) msg_print("�u�ۂ��ۂۂ��ۂ��ہE�E�E�v");
				msg_print("�ٗl�Ȃقǂɒ��g�̏��������ꂽ�B");
			}
			else 
				msg_print("��������Ȃ������B");

		}
		break;

	case 15:
		if (name) return "���˂���";
		if (desc) return "���̃t���A����Ń����X�^�[�u���˂��ˁv��z���Ƃ��ď�������B���˂��˂͋߂��̃����X�^�[�ɐ��_�U�����s���A�܂������X�^�[�̎��E���ɂ���Ƃ������E�N�O��������s����j�Q���邱�Ƃ�����B";
		if(info) return "";
		if(cast)
		{
			int i;
			monster_type *m_ptr;
			for (i = 1; i < m_max; i++)
			{
				m_ptr = &m_list[i];
				if (!m_ptr->r_idx) continue;
				if(m_ptr->r_idx == MON_KUNEKUNE)
				{
					msg_print("��x�Ɉ�̂����o���Ȃ��B");
					return NULL;
				}
			}

			if(summon_named_creature(0, py, px, MON_KUNEKUNE, PM_EPHEMERA))
			{
				msg_print("���˂��˂Ɛg���悶���Ȃ��̂����ꂽ..");
			}
			else 
				msg_print("��������Ȃ������B");

		}
		break;


	case 16:
		if (name) return "UFO";
		if (desc) return "�n��ɂ���Ƃ��̓_���W�����̓��B�ςݍŐ[�K�ցA�_���W�����ɂ���Ƃ��͒n��ւƈړ�����B";
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if(one_in_(3))		msg_print("���Ȃ��͐S�̒��ŉF���l�֌Ăт�����...");
				else if(one_in_(2))	msg_print("���Ȃ��͉F���D�Ƃ̃`���l�����O�����݂�...");
				else				msg_print("�s�x���g���[�A�x���g���[...�t");
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 17:
		if (name) return "�l�̔��Ό���";
		if (desc) return "���͂̃����_���Ȑ�����̂̏ꏊ�ɋ��͂ȃv���Y�}�����̋�����������B";
		{
			int tx,ty;
			int rad = 1 + plev / 24;
			monster_type *m_ptr;
			monster_race *r_ptr;
			int i;
			int dice = 12;
			int sides = 25 + plev/2;
			int num = 0;
			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				msg_print("���͂̋�C�������A�p�`�p�`�Ɖ��𗧂Ă��E�E");
				for (i = 1; i < m_max; i++)
				{
					m_ptr = &m_list[i];
					if (!m_ptr->r_idx) continue;
					r_ptr = &r_info[m_ptr->r_idx];
					if (is_pet(m_ptr)) continue;
					if (is_friendly(m_ptr)) continue;
					if (!monster_living(r_ptr)) continue;
					if(!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
					num++;
					if(!one_in_(num)) continue;

					ty = m_ptr->fy;
					tx = m_ptr->fx;
				}
				if(!num)
				{
					msg_print("�����������N����Ȃ������B");
				}
				else
				{
					char m_name[80];
					int dam = damroll(dice,sides);
					monster_desc(m_name, &m_list[cave[ty][tx].m_idx], 0);	
					msg_format("%s�͓ˑRῂ����ɕ�܂�ĔR���オ�����I",m_name);
					project(0, rad, ty, tx, dam, GF_PLASMA, PROJECT_KILL | PROJECT_GRID | PROJECT_JUMP | PROJECT_ITEM, -1);
				}

			}
		}
		break;

	case 18:
		if (name) return "�l�b�V�[";
		if (desc) return "�������̃{���g��u���X������B���������n�`�ɂ��Ȃ��Ǝg�����A�З͎͂��͂̐��n�`�̐��ɂ��ς��B�g���Ɛ��n�`�����オ���Ă����B";
		{

			int power=0, dam;
			int xx,yy;

			if (info) return format("����:�s��");

			if (cast)
			{
				int dist = plev / 5;
				if(!cave_have_flag_bold(py,px,FF_WATER))
				{
					msg_print("�����ɂ͐����Ȃ��B");
					return NULL;
				}
				if (!get_aim_dir(&dir)) return NULL;
				
				for(yy = py - 10;yy <= py + 10;yy++)
				for(xx = px - 10;xx <= px + 10;xx++)
				{
					if(!in_bounds(yy,xx)) continue;
					if(!cave_have_flag_bold(yy,xx,FF_WATER)) continue;
					if(distance(yy,xx,py,px) > dist) continue;
					if(!projectable(yy,xx,py,px)) continue;

					if(cave_have_flag_bold(yy,xx,FF_DEEP)) power += 3;
					else power++;
				}
				//power�̍ő�l��1000���炢
				dam = power;
				if(power < 100)
				{
					msg_print("���ʂ��牽�������Ȑ�����������o���Đ��ʂ𐁂����B");
					fire_bolt(GF_WATER,dir,dam);
				}
				else if(power < 500)
				{
					msg_print("���ʂ����̉���������ău���X��f�����B");
					fire_ball(GF_WATER,dir,dam,-2);
				}
				else
				{
					msg_print("���ʂ������グ�ċ���ȉ��b�����ꌃ����f���o�����I");
					fire_spark(GF_WATER,dir,dam,2);
				}

				for(yy = py - 10;yy <= py + 10;yy++)
				for(xx = px - 10;xx <= px + 10;xx++)
				{
					if(!in_bounds(yy,xx)) continue;
					if(!cave_have_flag_bold(yy,xx,FF_WATER)) continue;
					if(distance(yy,xx,py,px) > dist) continue;
					if(!projectable(yy,xx,py,px)) continue;
	
					//�����͂�鏈��
					if(cave_have_flag_bold(yy,xx,FF_DEEP))
					{
						if(!one_in_(3)) cave_set_feat(yy, xx, feat_shallow_water);
					}
					else if(one_in_(2))
					{
						if(one_in_(3)) 
							cave_set_feat(yy, xx, feat_swamp);
						else if(one_in_(2))
							cave_set_feat(yy, xx, feat_floor);
						else
							cave_set_feat(yy, xx, feat_dirt);
					}
				}

			}
		}
		break;
	case 19:
		if (name) return "���̎�";
		if (desc) return "�^�[�Q�b�g��̂ɖ������̑�_���[�W��^����B�������O��g�����тɑ��Ẩ��O����������B�r���Ɏ��s���������g�����Ƃ݂Ȃ����B";
		{
			int j;
			char m_name[80];
			int x,y;
			bool flag_ok = TRUE;
			monster_type *m_ptr;

			int dam = 512;

			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			if (info) return info_damage(0, 0, dam);
			if (cast)
			{

				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
					return NULL;
				}

				y = target_row;
				x = target_col;
				m_ptr = &m_list[cave[y][x].m_idx];

				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}

				monster_desc(m_name, m_ptr, 0);

				msg_format("���Ȃ��͉��̎��%s������ׂ��悤�ɂƊ�����I",m_name);
				project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_ARROW,flg,-1);
			}

			if(cast || fail)
			{
				p_ptr->monkey_count++;

				if(p_ptr->monkey_count > 2)
				{

					bool stop_ty = FALSE;
					int count = 0;

					msg_print("���̎�͂��Ȃ��ɍЂ��������炵���I");
					do
					{
						stop_ty = activate_ty_curse(stop_ty, &count);
					}
					while (one_in_(6));

					p_ptr->monkey_count = 0;

				}
			}

		}
		break;

	case 20:
		if (name) return "���g���O���[���}��";
		if (desc) return "���͂̋��|��Ԃ̓G�ɑ΂��A������̖������U�����s����B���E���łȂ��Ă��^�[�Q�b�g�ɂȂ�B";
		{
			int dam = 50+plev;
			int num = 2 + plev / 10;
			int i;
			if (info) return "";

			if (cast)
			{
				bool flag = FALSE;
				msg_print("�ΐF�̏��l�������U���Ă�����..");
				for(;num>0;num--)
				{
					for(i=1;i<m_max;i++)
					{
						char m_name[80];
						monster_type *m_ptr = &m_list[i];

						if(!m_ptr->r_idx) continue;
						if(m_ptr->cdis > MAX_SIGHT) continue;
						if(!MON_MONFEAR(m_ptr)) continue;
						if(!is_hostile(m_ptr)) continue;

						monster_desc(m_name, m_ptr, 0);	
						msg_format("�ΐF�̏��l��%s�֏P�����������I",m_name);
						project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_DISP_ALL,(PROJECT_JUMP|PROJECT_KILL),-1);
						flag = TRUE;
						if(p_ptr->is_dead) break;
					}
				}
				if(!flag) msg_print("���������l�����͉��������������B");

			}
		}
		break;

	case 21:
		if (name) return "�h�b�y���Q���K�[";
		if (desc) return "���̃t���A����ŗF�D�I�ȃh�b�y���Q���K�[���߂��Ɍ����B�h�b�y���Q���K�[�̃p�����[�^�͑S�������_���B�h�b�y���Q���K�[�Ƀv���C���[�����ڐG���Ƒ�ςȂ��ƂɂȂ�B�r���Ɏ��s�����ꍇ�G�ΓI�ȃh�b�y���Q���K�[�������B";
		{

			if (info) return "";

			if (cast | fail)
			{
				u32b mode;
				int r_idx;
				int i;
				int ty,tx;
				bool flag1 = TRUE;
				bool flag2 = TRUE;
				bool flag3 = TRUE;
				int attempt;
				//�J���Ă��郉���_�����j�[�N���g��
				msg_print("��������������悤�ȋC�z��������...");
				for(i=1;i<m_max;i++)
				{
					if(m_list[i].r_idx == MON_RANDOM_UNIQUE_1) flag1 = FALSE;
					if(m_list[i].r_idx == MON_RANDOM_UNIQUE_2) flag2 = FALSE;
					if(m_list[i].r_idx == MON_RANDOM_UNIQUE_3) flag3 = FALSE;

					if(r_info[m_list[i].r_idx].flags7 & RF7_DOPPELGANGER)
					{
						flag1 = FALSE;
						flag2 = FALSE;
						flag3 = FALSE;
						break;
					}
				}
				if(flag1)
					r_idx = MON_RANDOM_UNIQUE_1;
				else if(flag2)
					r_idx = MON_RANDOM_UNIQUE_2;
				else if(flag3)
					r_idx = MON_RANDOM_UNIQUE_3;
				else
				{
					msg_print("...�C�̂����������悤���B");
					break;
				}
				if(cast) mode = PM_FORCE_FRIENDLY;
				else mode = PM_NO_PET;
				for(attempt = 1000;attempt > 0;attempt--)
				{
					scatter(&ty,&tx,py,px,MAX_RANGE,0);
					if(distance(ty,tx,py,px) >= 10) break;
				}
				if(!attempt)
				{
					msg_print("...�C�̂����������悤���B");
					break;
				}

				flag_generate_doppelganger = TRUE;

				if(!summon_named_creature(0,ty,tx,r_idx,mode))
				{
					msg_print("...�C�̂����������悤���B");
				}

				flag_generate_doppelganger = FALSE;
			}
		}
		break;

	case 22:
		if (name) return "�����̒j�B";
		if (desc) return "���͂̓G�����X�^�[���t���A���������B���j�[�N�����X�^�[�͏������Ƀe���|�[�g����B�r���Ɏ��s����Ǝ������L����������t���A����Ǖ������B�N�G�X�g�_���W�����ł͎g���Ȃ��B";
		{
			int i;
			int rad = MAX_SIGHT;
			if (info) return info_range(rad);

			if (cast)
			{

				if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle)
				{
					msg_print("�����ł͎g���Ȃ��B");
					return NULL;
				}


				msg_print("�����̒j�B���Q���������U���čs����...");
				//v1.1.32 �ʃ��[�`���ɒu��������
				mass_genocide_3(rad, FALSE, FALSE);
#if 0
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr = &m_list[i];
					monster_race *r_ptr = &r_info[m_ptr->r_idx];

					if (!m_ptr->r_idx) continue;
					if(is_pet(m_ptr)) continue;
					if(is_friendly(m_ptr)) continue;
					if (i == p_ptr->riding) continue;
					if(m_ptr->cdis > rad) continue;
					if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & (RF7_UNIQUE2) )
					{
						teleport_away(i,100,TELEPORT_PASSIVE);
					}
					else
					{

						if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
						{
							char m_name[80];

							monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
							do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
						}
						delete_monster_idx(i);
					}
				}
#endif
			}
			else if(fail)
			{
				if(p_ptr->blind) msg_print("�����̑����Ƌ��Ɉٗl�ȋC�z�ɕ�܂ꂽ�E�E");
				else msg_print("���Ȃ��͍����̒j�B�Ɏ��͂܂ꂽ�I");

				if(lose_all_info()) msg_print("�����̏��u�����ꂽ�B�������ꂽ�̂��L�����B�����E�E");
				teleport_level(0);
			}
		}
		break;



	case 23:
		if (name) return "���ʒ������̏P��";
		if (desc) return "�^�[�Q�b�g�̎��͂ɕ��𑮐��̋������������ė���B��łȂ��Ǝg�p�ł��Ȃ��B";
    
		{
			int dam = plev * 3 ;
			int rad = 3;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				if(is_daytime())
				{
					msg_print("�����N����Ȃ������B");
					break;
				}
				if (!cast_wrath_of_the_god(dam, rad,TRUE)) return NULL;
			}
		}
		break;


	case 24:
		if (name) return "�G�C�{���̖��̎ԗ�";
		if (desc) return "�g�p����Ɓu���z���̊O���痈�������X�^�[�v����F������Ȃ��Ȃ�B�������ҋ@�E�x�e�E���H�E�����ύX�Ȃǂ������s���������艽�炩�̌����Ń_���[�W���󂯂�ƌ��ʂ��؂��B";
		{

			if (info) return "";

			if (cast)
			{
				msg_print("�����̓�������A�يE�̎҂̖ڂ��炠�Ȃ��̎p���B����...");
				p_ptr->special_defense |= (SD_EIBON_WHEEL);
				break_eibon_flag = FALSE;
				p_ptr->redraw |= (PR_STATUS);
			}
		}
		break;

	case 25:
		if (name) return "�X���C�}���̐o";
		if (desc) return "�אڂ���u���z���̊O���痈�������X�^�[�v��̂ɑ΂���_���[�W��^����B���������j�[�N�E�����X�^�[�ɑ΂��Ă͔����̌��ʂ�����������Ȃ��B�A�C�e���u�΍��v���ЂƂ����B";
		{
			int dam;
			int dice;
			int sides;
			int flg = (PROJECT_JUMP|PROJECT_KILL|PROJECT_STOP);
			char m_name[80];
			int x,y;
			monster_type *m_ptr;
			monster_race *r_ptr;

			dice = plev;
			sides = 50;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				cptr q,s;
				int item;

				item_tester_hook = item_tester_hook_ishizakura;
				q = "�ǂ̐G�}���g���܂���? ";
				s = "���Ȃ��͐G�}�ƂȂ�΍��������Ă��Ȃ��B";
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (!get_rep_dir2(&dir)) return NULL;
				if (dir == 5) return NULL;

				y = py + ddy[dir];
				x = px + ddx[dir];
				m_ptr = &m_list[cave[y][x].m_idx];
				if (!cave[y][x].m_idx || !m_ptr->ml)
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}

				//�U�������̑O�ɐG�}�����炷�B
				//�������Ȃ��Ƃ��̍U���Łu���˂�΂̋��v�Ȃǂ�|���Ĕ����ŃA�C�e������ꂽ�ꍇ����A�C�e��������邩������Ȃ�
				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);	
				}

				monster_desc(m_name, m_ptr, 0);
				msg_format("�P������%s���ݍ���...",m_name);

				r_ptr = &r_info[m_ptr->r_idx];
				if(r_ptr->flags3 & RF3_GEN_MASK)
				{
					msg_format("���������ʂ��Ȃ������B");
					break;
				}
				dam = damroll(dice,sides);
				if (r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR) || r_ptr->flags7 & (RF7_UNIQUE2) ) dam /= 2;
				
				project(0,0,m_ptr->fy,m_ptr->fx,dam,GF_DISP_ALL,flg,-1);

			}
		}
		break;


	case 26:

		if (name) return "�يE�Ƃ̐ڐG";
		if (desc) return "�����X�^�[����̔z���Ƃ��ď�������B�r���Ɏ��s�����Ƃ��G�ΓI�ȏ�ԂŌ����B��������Ă��郂���X�^�[�͊K�w�ƒn�`�ɂ���ĕς��B";
    
		{
			if (cast || fail)
			{
				int i,j;
				int lev = dun_level;
				int table_idx = 99;
				u32b mode;

				if(cast) msg_print("���Ȃ��͏����ɋL���ꂽ���ʂȋV��������s�����B");
				//�����Ǝ��͂̃O���b�h���[�v
				for(i=1;i<=9;i++)
				{
					int tx = px+ddx[i];
					int ty = py+ddy[i];

					if(!in_bounds(ty,tx)) continue;
					//�e�O���b�h�ɑ΂��A�N�g�D���t�n�����e�[�u���̊e�s�Ə�����v����
					for(j=0;occult_contact_table[j].r_idx;j++)
					{
						//�e�[�u���̎Ⴂ�s�D��
						if(j >= table_idx) break;
						//featflag1,2,3(not),r_info.level�̏����𖞂����Ώ���idx���L�^
						if(occult_contact_table[j].featflag1 && !cave_have_flag_bold(ty,tx,occult_contact_table[j].featflag1)) continue;
						if(occult_contact_table[j].featflag2 && !cave_have_flag_bold(ty,tx,occult_contact_table[j].featflag2)) continue;
						if(occult_contact_table[j].featflag3 &&  cave_have_flag_bold(ty,tx,occult_contact_table[j].featflag3)) continue;
						if(r_info[occult_contact_table[j].r_idx].level > lev+randint1(10)) continue;

						table_idx = j;
						break;
					}
				}
				if(table_idx == 99)
				{
					msg_print("��������������Ȃ������B");
					break;
				}
				if(cast)
				{
					mode = PM_FORCE_PET;
				}
				else	mode = PM_NO_PET;

				if(summon_named_creature(0,py,px,occult_contact_table[table_idx].r_idx,mode))
				{
					if(cast) 
						msg_format("%s%s������A���Ȃ��ɏ]�����B",occult_contact_table[table_idx].feat_desc,(r_name+r_info[occult_contact_table[table_idx].r_idx].name));
					else 	
						msg_format("%s%s������A���Ȃ��ɏP���|�����Ă����I",occult_contact_table[table_idx].feat_desc,(r_name+r_info[occult_contact_table[table_idx].r_idx].name));
				}
				else
				{
					msg_print("��������������Ȃ������B");
				}


			}
		}
		break;


	case 27:
		if (name) return "�يE�ւ̑ގU";
		if (desc) return "�u���z���̊O���痈�������X�^�[�v��̂��t���A����Ǖ�����B�����X�^�[�̃��x���̔{(���j�[�N�����X�^�[�͎l�{)��MP��ǉ��ŏ���AMP������Ă���ΕK���Ǖ��ɐ�������B�N�G�X�g�_���W�����ł͎g�����A�N�G�X�g�̃^�[�Q�b�g�����X�^�[�ɂ͌��ʂ��Ȃ��BMP������Ȃ������ꍇ�Ǖ��Ɏ��s�����������������󂯂�B";
		{
			monster_type *m_ptr;
			monster_race *r_ptr;
			int j;
			char m_name[80];
			int x,y;
			bool flag_ok = TRUE;

			if (cast)
			{
				int use_mana;

				if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle)
				{
					msg_print("�����ł͎g���Ȃ��B");
					return NULL;
				}


				if (!get_aim_dir(&dir)) return NULL;
				if(dir != 5 || !target_okay() || !projectable(target_row,target_col,py,px))
				{
					msg_print("���E���̃^�[�Q�b�g�𖾎��I�Ɏw�肵�Ȃ��Ƃ����Ȃ��B");
					return NULL;
				}

				y = target_row;
				x = target_col;
				m_ptr = &m_list[cave[y][x].m_idx];

				if (!m_ptr->r_idx || !m_ptr->ml)
				{
					msg_format("�����ɂ͉������Ȃ��B");
					return NULL;
				}
				r_ptr = &r_info[m_ptr->r_idx];
				monster_desc(m_name, m_ptr, 0);
				if(r_ptr->flags3 & RF3_GEN_MASK)
				{
					msg_format("%s�ɂ͌��ʂ��Ȃ������B�����̏Z�l�̂悤���B",m_name);
					break;
				}
				if((r_ptr->flags1 & (RF1_QUESTOR)))
				{
					msg_format("%s�ɂ͌��ʂ��Ȃ������B",m_name);
					break;
				}
				use_mana = r_ptr->level * 2;
				if (r_ptr->flags1 & (RF1_UNIQUE) || r_ptr->flags7 & (RF7_UNIQUE2) ) use_mana *= 2;
				
		
				if(use_mana <= p_ptr->csp)
				{
					msg_format("%s�����������E�ւƑ��҂����B",m_name);
					delete_monster_idx(cave[y][x].m_idx);

				}
				else
				{
					msg_format("%s�͎v���Ă�����苭�傾�I�Ǖ�����O�ɐ��_�͂��g���ʂ����Ă��܂����I",m_name);
					(void)set_monster_csleep(cave[y][x].m_idx, 0);
					set_paralyzed(p_ptr->paralyzed + (use_mana - p_ptr->csp)/10 + randint1(3));
				}

				p_ptr->csp -= use_mana;
				if(p_ptr->csp < 0) p_ptr->csp = 0;


			}
		}
		break;


	case 28:
		if (name) return "�����̖I�����̐���";
		if (desc) return "�u�����̖I�����v�𐻑�����B�A�C�e���u*�[��*�̖�v�u���̒܁v�u�����S�̌���|�v�u�~���N���t���[�c�v�u���̃N�V�i�_�v�̂����ꂩ��������B";
		{

			if (cast)
			{
				cptr q,s;
				int item;
				object_type forge;
				object_type *q_ptr = &forge;

				item_tester_hook = item_tester_hook_make_space_mead;
				q = "�ǂ���ޗ��ɂ��܂���? ";
				s = "���Ȃ��͏����ɋL���ꂽ�ޗ��������Ă��Ȃ��B";
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);	
				}

				msg_print("���Ȃ��͂܂�ŉ����̋V���̂悤�Ȓ�����Ƃ��n�߂��E�E");
				q_ptr = &forge;
				object_prep(q_ptr, lookup_kind(TV_ALCOHOL,SV_ALCOHOL_GOLDEN_MEAD));
				apply_magic(q_ptr, 1, AM_NO_FIXED_ART | AM_FORCE_NORMAL);
				q_ptr->discount = 90;
				(void)drop_near(q_ptr, -1, py, px);
			}
		}
		break;


	case 29:
		if (name) return "�C�V�X�̕���";
		if (desc) return "���͂̕ǂ��i�v�ǂɕω�����B�g�p���Ǝ��s���ɋ��C�ɏP����B���Ƀ����X�^�[������ǂɂ͉����N����Ȃ��B�N�G�X�g�_���W�����ł͎g���Ȃ��B";
		{
			int xx,yy;
			bool flag = FALSE;
			int i;

			if (info) return "";

			if (cast)
			{

				if ((p_ptr->inside_quest && !random_quest_number(dun_level)) || p_ptr->inside_arena || p_ptr->inside_battle)
				{
					msg_print("�����ł͎g���Ȃ��B");
					return NULL;
				}

				for(i=0;i<8;i++)
				{
					yy = py + ddy_cdd[i];
					xx = px + ddx_cdd[i];
					if(!in_bounds(yy,xx)) continue;
					if(!cave_have_flag_bold((yy), (xx), FF_WALL)) continue;
					if(cave_have_flag_bold((yy), (xx), FF_PERMANENT)) continue;
					if(cave[yy][xx].m_idx)continue;
					cave_set_feat(yy, xx, feat_permanent);
					flag = TRUE;
				}
				if(flag) msg_print("���͂̕ǂɋ��͂Ȏ�삪�h�����B");
			}
			if(cast || fail)
			{
				sanity_blast(0,TRUE);
			}

		}
		break;
	case 30:
		if (name) return "���_�̈�";
		if (desc) return "�A�C�e���u�ɜQ�������v���u����v���ЂƂ���A���Ɂu���_�̈�v��ݒu����B��͋߂��̍��ׂ̐��͂̃����X�^�[�ɂ�鏢�����@��j�Q���A���ׂ̐��͂̃����X�^�[����̏ꏊ�ɓ��ݍ��ނ��Ƃ����m���ŖW�Q����B�W�Q�Ɏ��s���Ĉ󂪔j�󂳂�邱�Ƃ����邪�A���̂Ƃ���͔j�ב����̔������N�����B";
		{

			if (info) return "";

			if (cast)
			{
				cptr q,s;
				int item;

				if (!cave_clean_bold(py, px))
				{
					msg_print("�����ł͎g���Ȃ��B");
					return NULL;
				}

				item_tester_hook = item_tester_hook_izanagi_object;
				q = "�ǂ���g���܂���? ";
				s = "���Ȃ��͕K�v�ȕ��i�������Ă��Ȃ��B";
				if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return NULL;

				if (item >= 0)
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}
				else
				{
					floor_item_increase(0 - item, -1);
					floor_item_describe(0 - item);
					floor_item_optimize(0 - item);	
				}

				msg_print("�Â���𔭓������B");
				cave[py][px].info |= CAVE_OBJECT;
				cave[py][px].mimic = feat_elder_sign;
				note_spot(py, px);
				lite_spot(py, px);

			}
		}
		break;

	case 31:
		if (name) return "���O���\�g�[�g�̈ꌂ";
		if (desc) return "�^�[�Q�b�g�̈ʒu�ɔ��a�s��̋��͂Ȉ��ʍ����̋��𔭐�������B�g�p�҂͎����̐��ۂɂ�����炸50�`200�_���[�W�̋��C�U�����󂯂�B�r���Ɏ��s���邩�G�ΓI�ȁw���O���\�g�[�g�x���t���A�ɂ���Ƃ��A�w���O���\�g�[�g�x���v���C���[�ɏP���|�����Ă���B";
		{
			int base = 500;
			int sides = 999;
			int rad;

			if (info) return info_damage(1, sides, base);

			if (cast || fail)
			{
				bool tmp_m_idx = 0;
				int i;

				if(cast) 
				{
					msg_print("���Ȃ��́u�S�Ăɂ��Ĉ�̂��́v�Ƃ̐ڐG�����݂�...");
					project(PROJECT_WHO_CONTACT_YOG,0,py,px,50+randint1(150),GF_COSMIC_HORROR,PROJECT_KILL,-1);
				}
				if(fail)
				{
					msg_print("���Ȃ��́u�S�Ăɂ��Ĉ�̂��́v�̐N�I���󂯂��I");
					project(PROJECT_WHO_CONTACT_YOG,0,py,px,200,GF_COSMIC_HORROR,PROJECT_KILL,-1);
				}

				if(!p_ptr->csp || p_ptr->is_dead || p_ptr->confused || p_ptr->image || p_ptr->paralyzed) fail = TRUE;

				for(i=1;i<m_max;i++) if(m_list[i].r_idx == MON_YOG_SOTHOTH && is_hostile(&m_list[i])) tmp_m_idx = i;

				if(tmp_m_idx)
				{
					teleport_monster_to(tmp_m_idx,py,px,100,0L);
					(void)set_monster_csleep(tmp_m_idx, 0);
					msg_print("�����̂���߂����̂�������֔��ł����I");
				}
				else if(fail)
				{
					summon_named_creature(0,py,px,MON_YOG_SOTHOTH,PM_NO_PET);
				}
				else
				{
					cptr yog_desc;
					int roll = randint1(sides);
					if (!get_aim_dir(&dir)) return NULL;
					rad = roll / 100;

					if(rad < 2) yog_desc = "��ȋ����ォ�痎���Ă����B";
					else if(rad < 5) yog_desc = "����߂�����ȋ����~���Ă����B";
					else yog_desc = "�r�����Ȃ�����ȋ��̂��_���W�����ɂ߂荞�񂾁I";
		
					if(!fire_ball_jump(GF_NEXUS, dir, base+roll, rad, yog_desc)) return NULL;

				}
			}
		}
		break;

	}

	return "";
}














/*
 * Do everything for each spell
 */
/*:::�߂�l:������Z���̕�����ւ̃|�C���^���Ԃ�*/
/*:::���@���r������B���������s������ς݁B�̈悲�Ƃɕ���B*/
///realm ���@�̈悲�ƂɌ��ʔ����֕���
cptr do_spell(int realm, int spell, int mode)
{
	switch (realm)
	{
		/*
	case REALM_LIFE:     return do_life_spell(spell, mode);
	case REALM_SORCERY:  return do_sorcery_spell(spell, mode);
	case REALM_NATURE:   return do_nature_spell(spell, mode);
	case REALM_CHAOS:    return do_chaos_spell(spell, mode);
	case REALM_DEATH:    return do_death_spell(spell, mode);
	case REALM_TRUMP:    return do_trump_spell(spell, mode);
	case REALM_ARCANE:   return do_arcane_spell(spell, mode);
	case REALM_CRAFT:    return do_craft_spell(spell, mode);
	case REALM_DAEMON:   return do_daemon_spell(spell, mode);
	case REALM_CRUSADE:  return do_crusade_spell(spell, mode);
	case REALM_MUSIC:    return do_music_spell(spell, mode);
	case REALM_HISSATSU: return do_hissatsu_spell(spell, mode);
	case REALM_HEX:      return do_hex_spell(spell, mode);
		*/
	case TV_BOOK_ELEMENT:
		return do_new_spell_element(spell,mode);
	case TV_BOOK_CHAOS:
		return do_new_spell_chaos(spell,mode);
	case TV_BOOK_FORESEE:
		return do_new_spell_foresee(spell,mode);
	case TV_BOOK_ENCHANT:
		return do_new_spell_enchant(spell,mode);
	case TV_BOOK_NATURE:
		return do_new_spell_nature(spell,mode);
	case TV_BOOK_NECROMANCY:
		return do_new_spell_necromancy(spell,mode);
	case TV_BOOK_LIFE:
		return do_new_spell_life(spell,mode);
	case TV_BOOK_TRANSFORM:
		return do_new_spell_transform(spell,mode);
	case TV_BOOK_DARKNESS:
		return do_new_spell_darkness(spell,mode);
	case TV_BOOK_SUMMONS:
		return do_new_spell_summon(spell,mode);
	case TV_BOOK_MYSTIC:
		return do_new_spell_mystic(spell,mode);
	case TV_BOOK_PUNISH:
		return do_new_spell_punish(spell,mode);
	case TV_BOOK_MUSIC://�̏��E�p�̓��ꏈ��
		if(p_ptr->pclass != CLASS_BARD) return do_new_spell_music2(spell,mode);
		else
		{
			msg_format("ERROR:���̐E�Ƃ̉̓��Z���o�^����Ă��Ȃ�");
			return NULL;
		}

	case TV_BOOK_HISSATSU:
		return do_hissatsu_spell(spell, mode);
	case TV_BOOK_OCCULT:
		return do_new_spell_occult(spell,mode);

	default:
		msg_print("���̗̈�͖��������B");
		return NULL;

	}

	return NULL;
}
