/* File: cmd1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Movement commands (part 1) */

#include "angband.h"
#define MAX_VAMPIRIC_DRAIN 50

//v1.1.94 ���͂����������ǂ����m�F���邽�߂̃O���[�o���t���O
bool gflag_force_effect = FALSE;





//v1.1.94 
//���̓���N���e�B�J���u�K�E�̈ꑾ���v���o�邩�ǂ����̔���
//���𑕔����Ă��邱�Ƃ͊O�Ŕ���ς݂ł��邱��
//r_ptr:�^�[�Q�b�g�푰
//mode:HISSATSU_XXX�Ȃǂ̍U�����[�h
bool check_katana_critical(monster_type *m_ptr, int mode)
{

	int power;
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	int katana_skill_exp = ref_skill_exp(TV_KATANA);

	if (randint1(katana_skill_exp) < 3200) return FALSE;

	//�����Ȃ��G�ɂ͏o�Ȃ�
	if (!m_ptr->ml) return FALSE;

	//lev40:178 lev50:210
	power = p_ptr->lev + katana_skill_exp / 50;

	//�Q�Ă���G��N�O�Ƃ��Ă���G�ɂ͏o�₷��
	if (MON_CSLEEP(m_ptr) || MON_STUNNED(m_ptr)) power += 60;

	//�񓁗��̎��o�Â炢
	if (p_ptr->migite && p_ptr->hidarite) power -= 30;

	//����ȓG�ɂ͏o�Â炢
	if (r_ptr->flags2 & RF2_GIGANTIC) power -= 30;

	//���ʂ̐����̌`�����Ă��Ȃ��G�ɂ͏o�Â炢
	if (r_ptr->flags1 & RF1_SHAPECHANGER || r_ptr->flags2 & RF2_PASS_WALL || !my_strchr("abcdfghklnopqrstuwyzABCDFGHIJKLMOPRSTUVWXYZ", r_ptr->d_char)) 
		power -= 30;

	if (power <= 0) return FALSE;

	switch (mode)
	{
		//�̂Đg�⋏���̌^�̂Ƃ��o�₷��
	case HISSATSU_IAI:
	case HISSATSU_SUTEMI:
	case HISSATSU_SEKIRYUKA:
	case HISSATSU_UNDEAD:
	case HISSATSU_MAJIN: //���ł͎g���Ȃ��������Ȃɂ��d�l�ς����Ƃ��̂��߂�
	case HISSATSU_ATTACK_ONCE:
		power *= 2;
		break;

		//���ʂ̍U���⑮���U���n�͏C���Ȃ�
	case HISSATSU_NONE:
	case HISSATSU_FIRE:
	case HISSATSU_COLD:
	case HISSATSU_POISON:
	case HISSATSU_ELEC:
	case HISSATSU_DRAIN:
	case HISSATSU_DOUBLE:
	case HISSATSU_ZANMA:
	case HISSATSU_QUAKE:
	case HISSATSU_COUNTER:
		break;

		//����������̍U���͏o�Â炢
	case HISSATSU_ISSEN:
	case HISSATSU_2:
	case HISSATSU_NYUSIN:
	case HISSATSU_HARAI:
	case HISSATSU_TRIANGLE:
	case HISSATSU_KAPPA:
		power -= 50;
		break;

		//�ق��L�͈͍U���ȂǓ���ȋZ�ł͏o�Ȃ�
	default:
		return FALSE;
	}

	if (cheat_xtra) msg_format("katana critical:power%d/rlev%d",power,r_ptr->level);

	if (randint1(power) > r_ptr->level) return TRUE;
	else return FALSE;

}



//v1.1.94 �����X�^�[�ɑ΂���s���x��
//add_energy:energy_need�l�ɉ��Z�����l
bool monster_delay(int m_idx, int add_energy)
{

	if (m_idx <= 0 || m_idx >= m_max)
	{
		msg_format("ERROR:monster_delay()�ɓn���ꂽm_idx����������(%d)", m_idx);
		return FALSE;
	}

	//200�ȏ���Z����Ȃ��B���ϓI�ɂ͈�s����100������
	add_energy = (add_energy > 200) ? 200 : (add_energy < 0) ? 0 : add_energy;

	//���łɍs���x�����Ă��郂���X�^�[�͂��̃����X�^�[���Ăэs������܂ōs���x�����󂯂Ȃ�
	if (m_list[m_idx].mflag & MFLAG_ALREADY_DELAYED)
	{
		if (cheat_xtra) msg_format("ALREADY DELAYED - m_idx:%d", m_idx);
		return FALSE;
	}

	if (cheat_xtra) msg_format("DELAY - m_idx:%d energy_need %d -> %d", m_idx, m_list[m_idx].energy_need, m_list[m_idx].energy_need + add_energy);

	m_list[m_idx].energy_need += add_energy;

	m_list[m_idx].mflag |= MFLAG_ALREADY_DELAYED;

	return TRUE;

}







//�ߐڍU���ǉ����ʂ̞N�O�t�^��AC�ቺ�Ȃǂ̔���ƓK�p���s���B
//critical_rank:��S��s�ӑł��Ȃǂ����܂����Ƃ�������@�ő�5
//effect_type: ATTACK_EFFECT_??? �����␇���͏�������₱�����Ȃ�̂ŊO�ŕʂɂ���Ă���
//skill_exp:���̍U���Ɋ֌W����Z�\�̌��ݒl 0~8000
//ref_effect_turns: ���ʂ���������^�[���� 0�Ȃ璆�œK���Ɍ��߂�
void apply_special_effect(int m_idx, int special_effect_type, int critical_rank, int skill_exp, int ref_effect_turns, bool flag_test)
{
	monster_type *m_ptr;
	monster_race *r_ptr;
	int mon_resist = 0;
	int effect_turns = ref_effect_turns;
	char m_name[160];

	bool flag_not_usual_shape = FALSE;

	int power;

	if (m_idx <= 0 || m_idx >= m_max)
	{
		msg_format("ERROR:apply_special_effect()�ɓn���ꂽm_idx����������(%d)", m_idx);
		return;
	}
	if (critical_rank < 0 || critical_rank > 5)
	{
		msg_format("ERROR:apply_special_effect()�ɓn���ꂽcritical_rank����������(%d)", critical_rank);
		return;
	}

	if (!special_effect_type) return;



	m_ptr = &m_list[m_idx];

	if (!m_ptr->r_idx) return;

	r_ptr = &r_info[m_ptr->r_idx];

	if(flag_test) gv_test1++;//�N�O�t�^�Ƃ��̔������`�F�b�N�p �֐��ɓ�������

	//��n���ł̓N���e�B�J���������������Ȃ����Ƃɂ��Ă���
	if (!critical_rank && skill_exp < (4000 + randint1(4000))) return;

	if (r_ptr->flagsr & RFR_RES_ALL) return;

	//�U���^�C�v���Ƃ̃����X�^�[�̒�R�͌v�Z
	mon_resist = 100 + r_ptr->level * 2;
	//v1.1.95 �h��ቺ��ԂȂ��R�͂������邱�Ƃɂ���
	if (MON_DEC_DEF(m_ptr)) mon_resist -= mon_resist / 4;

	if ((r_ptr->flags1 & RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2) mon_resist *= 2;
	if (r_ptr->flags2 & RF2_POWERFUL) mon_resist += 200;

	if (r_ptr->flags1 & RF1_SHAPECHANGER || r_ptr->flags2 & RF2_PASS_WALL || !my_strchr("abcdfghklnopqrstuwyzABCDFGHIJKLMOPRSTUVWXYZ", r_ptr->d_char)) flag_not_usual_shape = TRUE;

	switch (special_effect_type)
	{
	case	ATTACK_EFFECT_STUN:

		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (r_ptr->flags3 & RF3_NO_STUN)  mon_resist += 200;
		if (flag_not_usual_shape) mon_resist += 200;
		if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND) || !monster_living(r_ptr)) mon_resist += 200;

		break;
	case ATTACK_EFFECT_SLOW:

		//���j�[�N�Ɍ����i���͌����Ȃ�
		if ((r_ptr->flags1 & RF1_UNIQUE) || r_ptr->flags7 & RF7_UNIQUE2) return;

		if (r_ptr->flags1 & RF1_NEVER_MOVE) mon_resist += 200;

		if (flag_not_usual_shape) mon_resist += 200;

		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (r_ptr->flagsr & RFR_RES_INER) mon_resist += 200;
		break;

	case ATTACK_EFFECT_DEC_ATK:
		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (r_ptr->flags2 & RF2_REGENERATE) mon_resist += 200;
		if (flag_not_usual_shape) mon_resist += 200;

		break;

	case ATTACK_EFFECT_DEC_DEF:
		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (r_ptr->flags2 & RF2_REGENERATE) mon_resist += 200;
		if (flag_not_usual_shape) mon_resist += 200;
		break;

	case ATTACK_EFFECT_DEC_MAG:
		if (r_ptr->flags2 & RF2_SMART) mon_resist += 200;
		if (r_ptr->flags2 & RF2_REGENERATE) mon_resist += 200;
		if (r_ptr->flags3 & RF3_DEITY) mon_resist += 200;
		break;

	case ATTACK_EFFECT_DEC_ALL:
		if (r_ptr->flags2 & RF2_REGENERATE) mon_resist += 200;
		if (r_ptr->flags3 & RF3_DEITY) mon_resist += 200;
		break;

	case ATTACK_EFFECT_DELAY:
		//�����X�^�[�̍s���܂łɓ��ȏ�̍s���x���͂�����Ȃ�
		if (!flag_test && (m_ptr->mflag & MFLAG_ALREADY_DELAYED)) return;

		if (r_ptr->flags2 & RF2_GIGANTIC) mon_resist += 200;
		if (flag_not_usual_shape) mon_resist += 200;
		break;

	default:
		msg_format("ERROR:����`��special_effect_type(%d)", special_effect_type);
		return;
	}

	//memo
	//���i���N�O�t�^��
	//if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
	//STUN1,2,3�Ƃ��t�^���͓����ŏd�ʂ��S�����֌W
	//resist_stun�l�̓��[���@D��187,�I�x187,����275,J407
	//plev50��D19%,�I�x16%,����12.5%,J9%

	//�i����S�m��(plev=50,plus=45,to_h=100)
	//�d��8  (     2:10% 3:6% 4:4%)
	//�d��12 (     2:4%  3:7% 4:13%)
	//�d��16 (           3:4% 4:18% 5:7%)

	//�����S�m��(plev=50,plus=20,to_h=100)
	//�d��50  12%	(1:6.5% 2:5.5%                        )
	//�d��100 13%	(1:5.5% 2:6.5%                        )
	//�d��200 15%	(1:4.6% 2:6.9% 3:3.5%                 )
	//�d��400 19%	(       2:8.8% 3:5.8% 4:4.4%          )
	//�d��800 27%	(              3:4.1% 4:16.6% 5:6.2%  )
	//�d��1k  31%	(                     4:14.3% 5:16.7% )
	//�d��2k  51%	(                             5:51.0% )


	//�Y���n�����x����30�Ȃ烌�x��60�A40�Ȃ�80�AMAX�Ȃ烌�x��100�����̔���ɂȂ�悤��
	power = MAX((skill_exp / 80), p_ptr->lev);

	//�N���e�B�J���̂Ƃ������X�^�[�̃��W�X�g�l�ቺ
	switch (critical_rank)
	{
	case 1://�艞��
		mon_resist = mon_resist * 75 / 100;
		break;
	case 2://���Ȃ�̎艞��
		mon_resist = mon_resist * 60 / 100;
		break;
	case 3://��S
		mon_resist = mon_resist * 40 / 100;
		break;
	case 4://�ō��̉�S
		mon_resist = mon_resist * 25 / 100;
		break;
	case 5://��ނȂ��ō��̉�S
		mon_resist = mon_resist * 10 / 100;
		break;
	}

	if (cheat_xtra) msg_format("special_effect%d: pow:%d/resist:%d",special_effect_type,power,mon_resist);

	if (flag_test) gv_test3 += MIN(1000, (power * 1000 / mon_resist));//�N�O�t�^�Ƃ��̔������`�F�b�N�p �������̒l�����v���Ă��Ƃŕ��ς��o���@�Ȃ񂩂��̌v�Z�̎d�����������C������

	//����Ɏ��s����ƏI��
	if (power <= randint1(mon_resist)) return;

	if (flag_test) gv_test2++;//�N�O�t�^�Ƃ��̔������`�F�b�N�p �����ʂ�����

	if (effect_turns <= 0) effect_turns = 4 + randint1(4);

	monster_desc(m_name, m_ptr, 0);

	//����ɒʂ����̂Œǉ����ʕt�^����	
	switch (special_effect_type)
	{

	case	ATTACK_EFFECT_STUN:
		if (set_monster_stunned(m_idx, MON_STUNNED(m_ptr) + effect_turns))
			msg_format("%^s�̓t���t���ɂȂ����B", m_name);
		else
			msg_format("%^s�͂���Ƀt���t���ɂȂ����B", m_name);

		break;
	case ATTACK_EFFECT_SLOW:
		if (set_monster_slow(m_idx, MON_SLOW(m_ptr) + effect_turns))
			msg_format("%^s�̓������x���Ȃ����B", m_name);

		break;

	case ATTACK_EFFECT_DEC_ATK:

		if(set_monster_timed_status_add(MTIMED2_DEC_ATK,m_idx,effect_turns))
			msg_format("%^s�̍U���͂����������I", m_name);

		break;

	case ATTACK_EFFECT_DEC_DEF:

		if (set_monster_timed_status_add(MTIMED2_DEC_DEF, m_idx, effect_turns))
			msg_format("%^s�̖h��͂����������I", m_name);

		break;

	case ATTACK_EFFECT_DEC_MAG:

		if (set_monster_timed_status_add(MTIMED2_DEC_MAG, m_idx, effect_turns))
		msg_format("%^s�̖��@�͂����������I", m_name);
		break;

	case ATTACK_EFFECT_DEC_ALL:
	{
		bool flag_ok = FALSE;
		if (set_monster_timed_status_add(MTIMED2_DEC_ATK, m_idx, effect_turns)) flag_ok = TRUE;
		if (set_monster_timed_status_add(MTIMED2_DEC_DEF, m_idx, effect_turns)) flag_ok = TRUE;
		if (set_monster_timed_status_add(MTIMED2_DEC_MAG, m_idx, effect_turns)) flag_ok = TRUE;

		if(flag_ok)
			msg_format("%^s�̑S�\�͂����������I", m_name);

		break;

	}
	case ATTACK_EFFECT_DELAY:

		if (monster_delay(m_idx, 25 + randint1(25)))
			msg_format("%^s�͑̐���������悤���B", m_name);

		break;

	default:
		msg_format("ERROR:����`��special_effect_type(%d)", special_effect_type);
		return;
	}


}


/*
 * Determine if the player "hits" a monster (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
/*:::�ˌ��Ɠ����̖������� 
 *:::chance:do_cmd_fire_aux�ȂǂŌv�Z���������l
 *:::AC:�GAC �X�i�C�p�[�̂Ƃ�������������
 *:::vis m_ptr->ml */
bool test_hit_fire(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	///seikaku �Ȃ܂��ˌ����s
	//if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
	//	if (one_in_(20)) return (FALSE);

	/* Never hit */
	if (chance <= 0) return (FALSE);

	/* Invisible monsters are harder to hit */
	if (!vis) chance = (chance + 1) / 2;

	/* Power competes against armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Determine if the player "hits" a monster (normal combat).
 *
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
 /*:::�ʏ�U����������:::*/
 /*:::chance:�����l �푰�E�Ɛ��i�l+(�A�C�e���C��+�p�����[�^�C��+�j���q�[���[���̑�)*3:::*/
bool test_hit_norm(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	if (cheat_xtra) msg_format("test_hit_norm - chance:%d ac:%d" , chance,ac);

	/* Hack -- Instant miss or hit */
	/*:::����k<10�Ȃ�50%�̊m����True��False��Ԃ��B�����ۏؒl��5%�Ɛ�Ύ��s����5%���s�ōς܂���e�N�j�b�N�炵��:::*/
	if (k < 10) return (k < 5);



	///seikaku �Ȃ܂����̖������s
	//if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
	//	if (one_in_(20)) return (FALSE);

	/* Wimpy attack never hits */
	if (chance <= 0) return (FALSE);

	/* Penalize invisible targets */
	if (!vis) chance = (chance + 1) / 2;

	/* Power must defeat armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level.
 */
/*:::�ˌ��Ɠ����̃N���e�B�J������@���/�������̏d�ʁA�����C���Ɉˑ��@�X�i�C�p�[�̏W���Ƀ{�[�i�X*/
s16b critical_shot(int weight, int plus, int dam)
{
	int i, k;

	/* Extract "shot" power */
	i = ((p_ptr->to_h_b + plus) * 4) + (p_ptr->lev * 2);

	/* Snipers can shot more critically with crossbows */
	if (p_ptr->concent) i += ((i * p_ptr->concent) / 5);
	///class �X�i�C�p�[
	if ((p_ptr->pclass == CLASS_SNIPER) && (p_ptr->tval_ammo == TV_BOLT)) i *= 2;

	//v1.1.74 �Z�C���g�X�^�[�A���[�𔭓����ʂƂ��ĕ���������concent���Ȃ��̂ŃN���e�B�J�����������␳
	if (snipe_type == SP_FINAL)
	{
		i *= 4;
		if (p_ptr->wizard) msg_format("i:%d", i);
	}


	/* Critical hit */
	if (randint1(5000) <= i)
	{
		k = weight * randint1(500);

		if (k < 900)
		{
#ifdef JP
			msg_print("�育�������������I");
#else
			msg_print("It was a good hit!");
#endif

			dam += (dam / 2);
		}
		else if (k < 1350)
		{
#ifdef JP
			msg_print("���Ȃ�̎育�������������I");
#else
			msg_print("It was a great hit!");
#endif

			dam *= 2;
		}
		else
		{
#ifdef JP
			msg_print("��S�̈ꌂ���I");
#else
			msg_print("It was a superb hit!");
#endif

			dam *= 3;
		}
	}

	return (dam);
}



/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 */
/*�N���e�B�J���q�b�g�ɂ��dam�l�␳�v�Z*/
/*weight:����d��
 *plus:����q�b�g�C���l
 *dam:�_�C�X�_���[�W
 *meichuu:���̃q�b�g�C���l
 *mode:���p�Ȃǂ̍U���̂Ƃ�
 *critical_level �N���e�B�J���̂Ƃ��x�����ɂ���Ēl������
 *:::���킪�d�����햽���C���������قǃN���e�B�J���ɂȂ�₷���B���킪�d���قǂ悢�N���e�B�J���ɂȂ�₷���B
 */
s16b critical_norm(int weight, int plus, int dam, s16b meichuu, int mode, int *critical_level)
{
	int i, k;
	int chance = 5000;

	int rank = 0;

	if(p_ptr->pclass == CLASS_NINJA) chance = 4444;
	else if(p_ptr->pclass == CLASS_YUGI) chance = 3500;

	//v1.1.69 �������Z�ɂ��d�ʃ{�[�i�X
	if(mode == HISSATSU_URUMI)
	{
		int old_weight = weight;
		weight = weight * p_ptr->lev * 8 / 50;

	}

	//v1.1.94 �d��2000�������(�L�X����яオ�莞����)
	if (!(p_ptr->pclass == CLASS_KISUME && p_ptr->concent) && weight > 2000) weight = 2000;

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (p_ptr->lev * 3));

	if (cheat_xtra)
		msg_format("(critical_norm) weight:%d plus:%d meichuu:%d i:%d", weight, plus,meichuu,i);

	/* Chance */
	///class �N���e�B�J������@�E�҂ƌ��p�Ƃ̓��ꏈ��
	if ((randint1(chance) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		//v1.1.94
		//k = weight + randint1(650);
		k = randint1(weight) + randint1(650);

		if (mode == HISSATSU_COUNTER_SPEAR) k += randint1(ref_skill_exp(TV_SPEAR) / 10);

		if ((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);
		if(mode == HISSATSU_MAJIN && k < 700) k = 700;

		if (k < 400)
		{
#ifdef JP
			msg_print("�育�������������I");
#else
			msg_print("It was a good hit!");
#endif

			rank = 1;
			dam = 2 * dam + 5;
		}
		else if (k < 700)
		{
#ifdef JP
			msg_print("���Ȃ�̎育�������������I");
#else
			msg_print("It was a great hit!");
#endif

			rank = 2;
			dam = 2 * dam + 10;
		}
		else if (k < 900)
		{
#ifdef JP
			msg_print("��S�̈ꌂ���I");
#else
			msg_print("It was a superb hit!");
#endif
			rank = 3;

			dam = 3 * dam + 15;
		}
		else if (k < 1300)
		{
#ifdef JP
			msg_print("�ō��̉�S�̈ꌂ���I");
#else
			msg_print("It was a *GREAT* hit!");
#endif
			rank = 4;

			dam = 3 * dam + 20;
		}
		else
		{
#ifdef JP
			msg_print("��ނȂ��ō��̉�S�̈ꌂ���I");
#else
			msg_print("It was a *SUPERB* hit!");
#endif
			rank = 5;
			dam = ((7 * dam) / 2) + 25;
		}
	}

	if (*critical_level < rank) *critical_level = rank;

	return (dam);
}


/*:::�X���C�{���K�p�@�ڍז���*/
static int mult_slaying(int mult, const u32b* flgs, const monster_type* m_ptr)
{
	bool yukari = FALSE;
	static const struct slay_table_t {
		int slay_flag;
		u32b affect_race_flag;
		int slay_mult;
		size_t flag_offset;
		size_t r_flag_offset;
	} slay_table[] = {

#define OFFSET(X) offsetof(monster_race, X)
		/*:::���������̍\���悭�������@�K���ɓ���ւ���Α��v���Ǝv����*/
		///sys mon �X���C�t���O�K�p����
		{TR_SLAY_ANIMAL, RF3_ANIMAL, 25, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_ANIMAL, RF3_ANIMAL, 40, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_EVIL,   RF3_ANG_CHAOS,   20, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_EVIL,   RF3_ANG_CHAOS,   35, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_GOOD,   RF3_ANG_COSMOS,   20, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_GOOD,   RF3_ANG_COSMOS,   35, OFFSET(flags3), OFFSET(r_flags3)},

///mod131231 �����X�^�[�t���O�ύX
		{TR_SLAY_HUMAN,  RF3_HUMAN,  25, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_HUMAN,  RF3_HUMAN,  40, OFFSET(flags3), OFFSET(r_flags3)},
//		{TR_SLAY_HUMAN,  RF2_HUMAN,  25, OFFSET(flags2), OFFSET(r_flags2)},
//		{TR_KILL_HUMAN,  RF2_HUMAN,  40, OFFSET(flags2), OFFSET(r_flags2)},
		{TR_SLAY_UNDEAD, RF3_UNDEAD, 30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_UNDEAD, RF3_UNDEAD, 50, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_DEMON,  RF3_DEMON,  30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_DEMON,  RF3_DEMON,  50, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_KWAI,   RF3_KWAI,   30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_KWAI,   RF3_KWAI,   50, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_DEITY,  RF3_DEITY,  30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_DEITY,  RF3_DEITY,  50, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_HUMAN,  RF3_DEMIHUMAN,  20, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_HUMAN,  RF3_DEMIHUMAN,  35, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_SLAY_DRAGON, RF3_DRAGON, 30, OFFSET(flags3), OFFSET(r_flags3)},
		{TR_KILL_DRAGON, RF3_DRAGON, 50, OFFSET(flags3), OFFSET(r_flags3)},
/*:::undef��define�𖳌��ɂ���炵���@OFFSET()�͂��������Ŏg��ꂪ���Ȃ̂ł��������Ɍ��肵�Ă���̂��낤*/
#undef OFFSET
	};
	int i;
	monster_race* r_ptr = &r_info[m_ptr->r_idx];

	//���u�l�ԂƗd���̋��E�v�ɂ��V�t�g
	if(p_ptr->pclass == CLASS_YUKARI && p_ptr->tim_general[2]) yukari = TRUE;

	for (i = 0; i < sizeof(slay_table) / sizeof(slay_table[0]); ++ i)
	{
		const struct slay_table_t* p = &slay_table[i];
		u32b arg = p->affect_race_flag;

		if(yukari) arg <<= 1L;

		if ((have_flag(flgs, p->slay_flag)) &&
//		    (*(u32b*)(((char*)r_ptr) + p->flag_offset) & p->affect_race_flag))
		    (*(u32b*)(((char*)r_ptr) + p->flag_offset) & arg))
		{
			if (is_original_ap_and_seen(m_ptr) && !yukari)
			{
				*(u32b*)(((char*)r_ptr) + p->r_flag_offset) |= p->affect_race_flag;
			}

			mult = MAX(mult, p->slay_mult);
		}
	}

	return mult;
}
/*:::�U�������K�p*/
///system mon HURT_ELEC����������ɂ͂����ɒǉ�����K�v������Ǝv����
///mod131231 �����X�^�[�t���O�ύX ������_��flag3����flagr�Ɉړ��B����������₱�����̂Œ���
static int mult_brand(int mult, const u32b* flgs, const monster_type* m_ptr)
{
	static const struct brand_table_t {
		int brand_flag;
		u32b resist_mask;
		u32b hurt_flag;
	} brand_table[] = {
		{TR_BRAND_ACID, RFR_EFF_IM_ACID_MASK, 0U           },
		{TR_BRAND_ELEC, RFR_EFF_IM_ELEC_MASK, RFR_HURT_ELEC},
		{TR_BRAND_FIRE, RFR_EFF_IM_FIRE_MASK, RFR_HURT_FIRE},
		{TR_BRAND_COLD, RFR_EFF_IM_COLD_MASK, RFR_HURT_COLD},
//		{TR_BRAND_FIRE, RFR_EFF_IM_FIRE_MASK, RF3_HURT_FIRE},
//		{TR_BRAND_COLD, RFR_EFF_IM_COLD_MASK, RF3_HURT_COLD},
		{TR_BRAND_POIS, RFR_EFF_IM_POIS_MASK, 0U           },
	};
	int i;
	monster_race* r_ptr = &r_info[m_ptr->r_idx];

	for (i = 0; i < sizeof(brand_table) / sizeof(brand_table[0]); ++ i)
	{
		const struct brand_table_t* p = &brand_table[i];

		if (have_flag(flgs, p->brand_flag))
		{
			bool brand_hit = FALSE;
			int mult_effect = 10;

			/* Notice immunity */
			if (r_ptr->flagsr & p->resist_mask)
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flagsr |= (r_ptr->flagsr & p->resist_mask);
				}
			}

			/* Otherwise, take the damage */
			//else if (r_ptr->flags3 & p->hurt_flag)
			else if (r_ptr->flagsr & p->hurt_flag)
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flagsr |= p->hurt_flag;
				}
				brand_hit = TRUE;

				//mult = MAX(mult, 50);
				mult_effect = 50;
			}
			else
			{
				brand_hit = TRUE;
				mult_effect = 25;
				//mult = MAX(mult, 25);
			}

			//v1.1.91 �Ζ��n�`��̃_���[�W����
			//mult�l�Ƀ_���[�W�␳�l��K�p���邽�߂�brand_hit�Ƃ����������q�b�g�����t���O��mult_effect�Ƃ������{���݂����Ȑ��l�������
			//���̏����̏ꍇ�A�Ή��u���X�Ƃ���project()�o�R�̍U���͑ϐ������낤���Ȃ��낤���_���[�W�������邪�����̕��푮�������͑ϐ������ɂ����_���[�W�������Ȃ��B
			//�܂����푮���͌��X�����������̂�������ł������B�Ȃ񂩑z������Ɩ��ȊG�ʂ����B
			if (brand_hit && p->brand_flag == TR_BRAND_FIRE && cave_have_flag_bold(m_ptr->fy, m_ptr->fx, FF_OIL_FIELD))
			{
				mult_effect = oil_field_damage_mod(mult_effect, m_ptr->fy, m_ptr->fx);
			}

			if (cheat_xtra) msg_format("MULT:%d", mult_effect);

			if (brand_hit)
			{
				mult = MAX(mult, mult_effect);
			}

		}
	}

	return mult;
}
/*
 * Extract the "total damage" from a given object hitting a given monster.
 *
 * Note that "flasks of oil" do NOT do fire damage, although they
 * certainly could be made to do so.  XXX XXX
 *
 * Note that most brands and slays are x3, except Slay Animal (x2),
 * Slay Evil (x2), and Kill dragon (x5).
 */
/*:::�_�C�X�_���[�W�A������A�����X�^�[���𓾂ăX���C�Ȃǂ��l�����_���[�W�𑝉�������*/
///item mon �X���C�ɂ��_�C�X��������
s16b tot_dam_aux(object_type *o_ptr, int tdam, monster_type *m_ptr, int mode, bool thrown)
{
	int mult = 10;

	u32b flgs[TR_FLAG_SIZE];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Extract the flags */
	object_flags(o_ptr, flgs);
	torch_flags(o_ptr, flgs); /* torches has secret flags */

	if (!thrown)
	{
		/* Magical Swords */
		if (p_ptr->special_attack & (ATTACK_ACID)) add_flag(flgs, TR_BRAND_ACID);
		if (p_ptr->special_attack & (ATTACK_COLD)) add_flag(flgs, TR_BRAND_COLD);
		if (p_ptr->special_attack & (ATTACK_ELEC)) add_flag(flgs, TR_BRAND_ELEC);
		if (p_ptr->special_attack & (ATTACK_FIRE)) add_flag(flgs, TR_BRAND_FIRE);
		if (p_ptr->special_attack & (ATTACK_POIS)) add_flag(flgs, TR_BRAND_POIS);

		///mod150415 ���}���̓ōU��
		if (p_ptr->pclass == CLASS_YAMAME && p_ptr->lev > randint0(100)) add_flag(flgs, TR_BRAND_POIS);

		//v1.1.89 �S�X��
		if (p_ptr->pclass == CLASS_MOMOYO)
		{
			if(p_ptr->lev > 34) add_flag(flgs, TR_BRAND_POIS);
			if (p_ptr->tim_general[2]) add_flag(flgs, TR_SLAY_DRAGON);
		}

		//v1.1.47 �~�X�e�B�A�u�q���[�}���P�[�W�v
		if (p_ptr->pclass == CLASS_MYSTIA && music_singing(MUSIC_NEW_MYSTIA_HUMAN_CAGE))
			add_flag(flgs, TR_SLAY_HUMAN);
		///mod151115 ���̓��ꏈ��
		if(p_ptr->pclass == CLASS_HINA)
		{
			if(p_ptr->magic_num1[0] > 27000) add_flag(flgs, TR_KILL_GOOD);
			else if(p_ptr->magic_num1[0] > 18000) add_flag(flgs, TR_SLAY_GOOD);
			if(p_ptr->magic_num1[0] > 18000) add_flag(flgs, TR_KILL_HUMAN);
			else if(p_ptr->magic_num1[0] > 12000) add_flag(flgs, TR_SLAY_HUMAN);
		}
		if (p_ptr->special_attack & (ATTACK_DARK))
		{
			if(have_flag(flgs,TR_SLAY_GOOD))  add_flag(flgs, TR_KILL_GOOD);
			else add_flag(flgs, TR_SLAY_GOOD);
		}
		if (p_ptr->special_attack & (ATTACK_FORCE))
		{
			add_flag(flgs, TR_FORCE_WEAPON);
		}
		if (p_ptr->kamioroshi & KAMIOROSHI_IZUNA)
			add_flag(flgs, TR_SLAY_EVIL);
		else if (p_ptr->kamioroshi & KAMIOROSHI_ATAGO)
			add_flag(flgs, TR_BRAND_FIRE);

		if(p_ptr->pclass == CLASS_MEDICINE && p_ptr->lev > 34) add_flag(flgs, TR_BRAND_POIS);
	}

	/* Hex - Slay Good (Runesword) */
	if (hex_spelling(HEX_RUNESWORD)) add_flag(flgs, TR_SLAY_GOOD);



	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		///item TVAL weapon
		case TV_BULLET:
		case TV_ARROW:
		case TV_BOLT:
		case TV_KNIFE:
		case TV_SWORD:
		case TV_KATANA:
		case TV_HAMMER:
		case TV_STICK:
		case TV_AXE:
		case TV_SPEAR:
		case TV_POLEARM:		
		case TV_OTHERWEAPON:
		case TV_LITE:
		{
			/* Slaying */
			mult = mult_slaying(mult, flgs, m_ptr);

			/* Elemental Brand */
			mult = mult_brand(mult, flgs, m_ptr);

			/* Hissatsu */
			///class ���p��
			if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_YOUMU)
			{
				mult = mult_hissatsu(mult, flgs, m_ptr, mode);
			}

			/*:::���ꏈ���@�N���m�X�̑劙�͒j�ɑ΂�X�X���C����*/
			if(o_ptr->name1 == ART_KRONOS)
			{
				if((r_ptr->flags1 & RF1_MALE) && mult < 40) mult = 40;
			}

			//v1.1.89 �S�X���̃X�l�[�N�C�[�^�[�ɂ��w�r�X���C
			if (p_ptr->pclass == CLASS_MOMOYO)
			{
				if (p_ptr->tim_general[1] && r_ptr->d_char == 'J' && mult < 30) mult = 30;


			}

			//v1.1.14 �_���l�̓{��@�������U�ɂ��Ă݂�
			if(mode == HISSATSU_GION)
			{
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if((r_ptr->flags1 & RF1_FEMALE) && mult < 30) mult = 30;
			}

			/* Force Weapon */
			///mod150808 �A���X�͖�����
			if ((p_ptr->pclass != CLASS_ALICE) && (p_ptr->pclass != CLASS_SAMURAI) && (p_ptr->pclass != CLASS_MARTIAL_ARTIST) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
			{
				p_ptr->csp -= (1+(o_ptr->dd * o_ptr->ds / 5));
				p_ptr->redraw |= (PR_MANA);
				mult = mult * 3 / 2 + 20;

				gflag_force_effect = TRUE;//���͂����������ǂ����m�F���邽�߂̃t���O
			}




			/*:::���ꏈ��*/
			/* Hack -- The Nothung cause special damage to Fafner */
			///item mon �m�[�g�D���O���t�@�t�i�[�ɑ�_���[�W��^���鏈��
			if ((o_ptr->name1 == ART_NOTHUNG) && (m_ptr->r_idx == MON_FAFNER))
				mult = 150;
			break;
		}
	}
	if (mult > 150) mult = 150;

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Search for hidden things
 */
/*:::���g���b�v�A���g���b�v�A�B���h�A��������*/
static void discover_hidden_things(int y, int x)
{
	s16b this_o_idx, next_o_idx = 0;

	cave_type *c_ptr;

	/* Access the grid */
	c_ptr = &cave[y][x];

	/* Invisible trap */
	if (c_ptr->mimic && is_trap(c_ptr->feat))
	{
		/* Pick a trap */
		disclose_grid(y, x);

		/* Message */
		msg_print(_("�g���b�v�𔭌������B", "You have found a trap."));

		/* Disturb */
		disturb(0, 1);
	}

	/* Secret door */
	if (is_hidden_door(c_ptr))
	{
		/* Message */
		msg_print(_("�B���h�A�𔭌������B", "You have found a secret door."));

		/* Disclose */
		disclose_grid(y, x);

		/* Disturb */
		disturb(0, 0);
	}

	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Skip non-chests */
		if (o_ptr->tval != TV_CHEST) continue;

		/* Skip non-trapped chests */
		/*:::����㩂����邩�ǂ����͔���pval�Ō��܂�炵��*/
		if (!chest_traps[o_ptr->pval]) continue;

		/* Identify once */
		if (!object_is_known(o_ptr))
		{
			/* Message */
			if(o_ptr->pval < 0 && !chest_traps[0-o_ptr->pval]) msg_print("����͊J���Ă����S�������B");
			else msg_print(_("���Ɏd�|����ꂽ�g���b�v�𔭌������I", "You have discovered a trap on the chest!"));

			/* Know the trap */
			object_known(o_ptr);

			/* Notice it */
			disturb(0, 0);
		}
	}
}
/*:::�T���Z�\�ɂ��B���n�`��������*/
void search(void)
{
	int i, chance;


	/* Start with base search ability */
	chance = p_ptr->skill_srh;

	/* Penalize various conditions */
	if (p_ptr->blind || no_lite()) chance = chance / 10;
	if (p_ptr->confused || p_ptr->image) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (i = 0; i < 9; ++ i)
	{
		bool flag_auto_success = FALSE;

		//v1.1.57 �B��ނ͉B���ꂽ�h�A�Ɨ��Ƃ��˂�K�����j��(�K�����^�[��search()���Ă΂��Ƃ͌����Ă��Ȃ�)
		if (p_ptr->pclass == CLASS_OKINA)
		{
			cave_type *c_ptr = &cave[py + ddy_ddd[i]][px + ddx_ddd[i]];
			feature_type *f_ptr = &f_info[c_ptr->feat];
			int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

			if(is_hidden_door(c_ptr)) flag_auto_success = TRUE;
			if(trap_feat_type == TRAP_TRAPDOOR) flag_auto_success = TRUE;
		}

		/* Sometimes, notice things */
		if (flag_auto_success || randint0(100) < chance)
		{
			discover_hidden_things(py + ddy_ddd[i], px + ddx_ddd[i]);
		}
	}
}


/*
 * Helper routine for py_pickup() and py_pickup_floor().
 *
 * Add the given dungeon object to the character's inventory.
 *
 * Delete the object afterwards.
 */
/*:::�A�C�e�������ォ��U�b�N�ɓ���鏈���@�U�b�N�ɋ󂫂�����Ɗm�F�ς݂ł��邱��*/
void py_pickup_aux(int o_idx)
{
	int slot;

#ifdef JP
/*
 * �A�C�e�����E�����ۂɁu�Q�̃P�[�L�������Ă���v
 * "You have two cakes." �ƃA�C�e�����E������̍��v�݂̂̕\�����I���W�i��
 * �����A��a����
 * ����Ƃ����w�E���������̂ŁA�u�`���E�����A�`�������Ă���v�Ƃ����\��
 * �ɂ����Ă���B���̂��߂̔z��B
 */
	char o_name[MAX_NLEN];
	char old_name[MAX_NLEN];
	char kazu_str[80];
	int hirottakazu;
	bool moon_vault_trap = FALSE;
#else
	char o_name[MAX_NLEN];
#endif

	object_type *o_ptr;

	o_ptr = &o_list[o_idx];

	//���s���ۓW�p����A�C�e��
	if(p_ptr->inside_quest == QUEST_MOON_VAULT && o_ptr->marked & OM_SPECIAL_FLAG)
		moon_vault_trap = TRUE;
	//�󕨌Ɂ��Ȃ�������(inven_carry()�̂Ƃ�marked���N���A�����̂�check_find_art_quest_completion()�͎g���Ȃ�)
	else if(p_ptr->inside_quest == QUEST_VAULT && o_ptr->marked & OM_SPECIAL_FLAG)
		complete_quest(QUEST_VAULT);



#ifdef JP
	/* Describe the object */
	object_desc(old_name, o_ptr, OD_NAME_ONLY);
	object_desc_kosuu(kazu_str, o_ptr);
	hirottakazu = o_ptr->number;
#endif
	/* Carry the object */
	/*:::�U�b�N�ɃA�C�e��������*/
	slot = inven_carry(o_ptr);

	/* Get the object again */
	o_ptr = &inventory[slot];

	/* Delete the object */
	/*:::�����̃A�C�e��������*/
	delete_object_idx(o_idx);

		///system ���i�֘A�����Ȃ�
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		bool old_known = identify_item(o_ptr);

		/* Auto-inscription/destroy */
		autopick_alter_item(slot, (bool)(destroy_identify && !old_known));

		/* If it is destroyed, don't pick it up */
		if (o_ptr->marked & OM_AUTODESTROY) return;
	}

	/* Describe the object */
	object_desc(o_name, o_ptr, 0);

	/* Message */

#ifdef JP
///msg131223
#if 0
	if ((o_ptr->name1 == ART_CRIMSON) && (p_ptr->pseikaku == SEIKAKU_COMBAT))
	{
		msg_format("�������āA%s�́w�N�����]���x����ɓ��ꂽ�B", player_name);
		msg_print("���������A�w���ׂ̃T�[�y���g�x�̕����������X�^�[���A");
		msg_format("%s�ɏP��������D�D�D", player_name);
	}
	else
#endif
	{
		if (plain_pickup)
		{
			msg_format("%s(%c)�������Ă���B",o_name, index_to_label(slot));
		}
		else
		{
			if (o_ptr->number > hirottakazu) {
			    msg_format("%s�E���āA%s(%c)�������Ă���B",
			       kazu_str, o_name, index_to_label(slot));
			} else {
				msg_format("%s(%c)���E�����B", o_name, index_to_label(slot));
			}
		}
	}
	strcpy(record_o_name, old_name);
#else
	msg_format("You have %s (%c).", o_name, index_to_label(slot));
	strcpy(record_o_name, o_name);
#endif
	record_turn = turn;



	check_find_art_quest_completion(o_ptr);

	if(moon_vault_trap) activate_moon_vault_trap();


}


/*
 * Player "wants" to pick up an object or gold.
 * Note that we ONLY handle things that can be picked up.
 * See "move_player()" for handling of other things.
 */
/*:::�����A�C�e���ɑ΂��鏈��*/
void carry(bool pickup)
{
	cave_type *c_ptr = &cave[py][px];

	s16b this_o_idx, next_o_idx = 0;

	char	o_name[MAX_NLEN];

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Automatically pickup/destroy/inscribe items */
	autopick_pickup_items(c_ptr);


#ifdef ALLOW_EASY_FLOOR

	if (easy_floor)
	{
		py_pickup_floor(pickup);
		return;
	}

#endif /* ALLOW_EASY_FLOOR */

	/* Scan the pile of objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

/*:::��������`*/
#ifdef ALLOW_EASY_SENSE /* TNB */

		/* Option: Make item sensing easy */
		if (easy_sense)
		{
			/* Sense the object */
			(void)sense_object(o_ptr);
		}

#endif /* ALLOW_EASY_SENSE -- TNB */

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- disturb */
		disturb(0, 0);

		///sys item ���������I�ɏE�������@������P�A�C�e���Ƃ���������Ȃ炱����
		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			int value = (long)o_ptr->pval;

			/* Delete the gold */
			delete_object_idx(this_o_idx);

			/* Message */
#ifdef JP
		msg_format(" $%ld �̉��l������%s���������B",
			   (long)value, o_name);
#else
			msg_format("You collect %ld gold pieces worth of %s.",
				   (long)value, o_name);
#endif


			sound(SOUND_SELL);

			/* Collect the gold */
			p_ptr->au += value;

			/* Redraw gold */
			p_ptr->redraw |= (PR_GOLD);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}

		/* Pick up objects */
		else
		{
			/* Hack - some objects were handled in autopick_pickup_items(). */
			if (o_ptr->marked & OM_NOMSG)
			{
				/* Clear the flag. */
				o_ptr->marked &= ~OM_NOMSG;
			}
			/* Describe the object */
			else if (!pickup)
			{
#ifdef JP
				msg_format("%s������B", o_name);
#else
				msg_format("You see %s.", o_name);
#endif

			}

			/* Note that the pack is too full */
			else if (!inven_carry_okay(o_ptr))
			{
#ifdef JP
				msg_format("�U�b�N�ɂ�%s�����錄�Ԃ��Ȃ��B", o_name);
#else
				msg_format("You have no room for %s.", o_name);
#endif

			}

			/* Pick up the item (if requested and allowed) */
			else
			{
				int okay = TRUE;

				/* Hack -- query every item */
				if (carry_query_flag)
				{
					char out_val[MAX_NLEN+20];
#ifdef JP
					sprintf(out_val, "%s���E���܂���? ", o_name);
#else
					sprintf(out_val, "Pick up %s? ", o_name);
#endif

					okay = get_check(out_val);
				}

				/* Attempt to pick up an object. */
				if (okay)
				{
					/* Pick up the object */
					py_pickup_aux(this_o_idx);
				}
			}
		}
	}
}




/*
* Determine if a trap affects the player.
* Always miss 5% of the time, Always hit 5% of the time.
* Otherwise, match trap power against player armor.
*/
//�g���b�v�̖�������
//v1.1.96 �����̊֐��������ĕ���킵���̂�check_hit()���疼�̕ύX����
static int check_hit_trap(int power)
{
	int k, ac;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- 5% hit, 5% miss */
	if (k < 10) return (k < 5);

	//if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
	//	if (one_in_(20)) return (TRUE);

	/* Paranoia -- No power */
	if (power <= 0) return (FALSE);

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Power competes against Armor */
	if (randint1(power) > ((ac * 3) / 4)) return (TRUE);

	/* Assume miss */
	return (FALSE);
}


//v1.1.97
//����g���b�v�̔����Bhit_trap()������ɕύX����B���project()�o�R�Ł��ɂ������X�^�[�ɂ����ʂ��o��悤�ɂ���B
//��������g���b�v�̍��W���w�肷��B�g���b�v�̏�Ɂ��łȂ������X�^�[�����邱�Ƃ��N�����Ȃ����Ƃ�����B
//GF_ACTIVATE_TRAP�����Ńg���b�v�𔭓�����ꍇproject()���ċA�Ăяo�������̂ŃX�^�b�N�I�[�o�[�t���[�ƒn�`�ω��ƓG���łɒ���
//mpe_mode:MPE_BREAK_TRAP�̂Ƃ�������̃g���b�v�𕲍ӂ��AMPE_ACTIVATE_TRAP�̂Ƃ����V�������Ă����Ƃ��˂ɗ�����
void activate_floor_trap(int y, int x, u32b mpe_mode)
{
	bool flag_break_trap;
	bool trap_player = FALSE;
	bool trap_monster = FALSE;
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;
	char            m_name[80] = "";
	monster_type *m_ptr;
	monster_race *r_ptr;
	int damage;

	flag_break_trap = (mpe_mode & MPE_BREAK_TRAP);


	if (trap_feat_type == NOT_TRAP) return;

	if (world_player || SAKUYA_WORLD)
	{
		msg_print("�g���b�v�͍쓮���Ȃ������B");
		return;
	}

	//�g���b�v�̏�Ɂ�������Ƃ�trap_player��TRUE
	if (player_bold(y, x))
	{
		trap_player = TRUE;

		if (p_ptr->pclass == CLASS_FLAN) flag_break_trap = TRUE;
		if (p_ptr->pseikaku == SEIKAKU_BERSERK && randint1(p_ptr->lev) > 10) flag_break_trap = TRUE;

	}
	//�g���b�v�̏�ɏ�n�ȊO�̃����X�^�[������Ƃ�trap_monster��TRUE
	else if (c_ptr->m_idx)
	{
		trap_monster = TRUE;

		m_ptr = &m_list[c_ptr->m_idx];
		r_ptr = &r_info[m_ptr->r_idx];

		monster_desc(m_name, m_ptr, 0);

	}

	disturb(0, 1);


	switch (trap_feat_type)
	{
		//���Ƃ��ˁ@���̓e���|���x���A�����X�^�[��FLYING�����AGIGANTIC��QUESTOR�͍s���x���A����ȊO�t���A�������
	case TRAP_TRAPDOOR:
	{
		if (trap_player)
		{
			if (!(mpe_mode & MPE_ACTIVATE_TRAP) && p_ptr->levitation)
			{
#ifdef JP
				msg_print("���Ƃ��˂��щz�����B");
#else
				msg_print("You fly over a trap door.");
#endif
				return;
			}
			else
			{
				cptr trap_name;
#ifdef JP
				if(mpe_mode & MPE_ACTIVATE_TRAP)
					msg_print("���Ƃ��˂ɔ�э��񂾁I");
				else
					msg_print("���Ƃ��˂ɗ������I");

#else
				msg_print("You have fallen through a trap door!");
#endif
				sound(SOUND_FALL);
				damage = damroll(2, 8);
#ifdef JP
				trap_name = "���Ƃ���";
#else
				trap_name = "a trap door";
#endif
				take_hit(DAMAGE_NOESCAPE, damage, trap_name, -1);

				/* Still alive and autosave enabled */
				if (autosave_l && (p_ptr->chp >= 0))
					do_cmd_save_game(TRUE);

#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "���Ƃ��˂ɗ�����");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "You have fallen through a trap door!");
#endif
				///mod160813 �N�G�X�g�t���A�Ńg���b�v�h�A�ɗ������Ƃ��̏�����ǉ��@�T�O���p
				if (EXTRA_MODE)
				{

					prepare_change_floor_mode(CFM_DOWN | CFM_RAND_PLACE | CFM_NO_RETURN);
					if (INSIDE_EXTRA_QUEST)
					{
						leave_quest_check();
						p_ptr->inside_quest = 0;
					}
				}
				else
				{
					prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
					if (p_ptr->inside_quest)
					{
						leave_quest_check();
						p_ptr->inside_quest = 0;
					}
				}

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}
		else if (trap_monster)
		{
			msg_print("���Ƃ��˂��J�����I");
			if (r_ptr->flags7 & RF7_CAN_FLY)
			{
				msg_format("%s�͗I�X�Ɨ��Ƃ��˂̏����ł���B", m_name);
			}
			else if (r_ptr->flags1 & RF1_QUESTOR || r_ptr->flags2 & RF2_GIGANTIC)
			{
				msg_format("%s�͗��Ƃ��˂��甲���o�����I", m_name);
				monster_delay(c_ptr->m_idx, randint1(100));
			}
			else
			{
				msg_format("%s�����Ƃ��˂ɗ������I", m_name);
				monster_drop_carried_objects(m_ptr);//�E�����A�C�e���͗��V�ɗ��Ƃ��Ă�������邱�Ƃɂ��Ă���
				delete_monster_idx(c_ptr->m_idx);
			}

		}
		else
		{
			msg_print("���Ƃ��˂��J�����B�������N�����Ƃ��˂̏�ɂ��Ȃ��B");
		}
	}
	break;


	//���Ƃ����̃_���[�W������GF_PIT_FALL�����Ƃ��Ď��������B�����ꖂ�@�́u�[���������v��Ă�̗��Ƃ����Ƃ�����������
	case TRAP_PIT:

	{
		int d = dun_level / 8 + 1;
		damage = damroll(d, d);

		if (trap_player)
		{

			if (!(mpe_mode & MPE_ACTIVATE_TRAP) && p_ptr->levitation)
			{
#ifdef JP
				msg_print("���Ƃ������щz�����B");
#else
				msg_print("You fly over a trap door.");
#endif
				return;

			}
			else
			{
				msg_print("���Ƃ������I");
				project(PROJECT_WHO_TRAP, 0, y, x, damage, GF_PIT_FALL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER), -1);
			}
		}
		else if (trap_monster)
		{
			project(PROJECT_WHO_TRAP, 0, y, x, damage, GF_PIT_FALL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER), -1);
		}
		else
		{
			msg_print("���Ƃ������J�����B�������N�����Ƃ����̏�ɂ��Ȃ��B");
		}

	}
	break;

	//define�̃��x���������̂܂܂Ō��̋@�\�𐅒n�`�����ƐΖ��n�`�����ɕς���
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if (trap_player)
		{
			if (!(mpe_mode & MPE_ACTIVATE_TRAP) && p_ptr->levitation)
			{
				msg_print("�T��̏���щz�����B");
				return;
			}
			else
			{
				if (mpe_mode & MPE_ACTIVATE_TRAP)
					msg_print("���Ȃ��͑����̋T��𓥂݊������I");

				if(trap_feat_type == TRAP_SPIKED_PIT)
					msg_print("���������ꂽ�I���ɐ������܂��Ă���I");
				else
					msg_print("��������Ζ��������o�����I");

			}
		}
		//�n�`�ύX����O�ɂ��̃O���b�h�̃g���b�v�����ł�����
		cave_alter_feat(y, x, FF_HIT_TRAP);

		//�����́�����щz�����Ƃ��ȊO�������ɔ�������悤�ɂ��Ă���B
		//������g���b�v�������X�^�[�����ނ悤�ɉ�������@�\�����������Ƃ��āA
		//��ԃ����X�^�[�����ݓ������Ƃ��ɔ������Ȃ��悤�ݒ肷��K�v������B
		if (trap_feat_type == TRAP_SPIKED_PIT)
			project(PROJECT_WHO_TRAP, (2 + randint1(5) / 4) , y, x, 4, GF_WATER_FLOW, PROJECT_JUMP | PROJECT_GRID, -1);
		else
			project(PROJECT_WHO_TRAP, (3 + randint1(3)/3), y, x, 100+dun_level, GF_DIG_OIL, PROJECT_JUMP | PROJECT_GRID, -1);


		break;

	//�׈��ȃ��[��	�����X�^�[�����񂾂�S�\�͒ቺ+�ړ��֎~�ł������낤
	case TRAP_TY_CURSE:
	{
		if (trap_player)
		{
			int i;
#ifdef JP
			msg_print("�������s�J�b�ƌ������I");
#else
			msg_print("There is a flash of shimmering light!");
#endif
			int num = 2 + randint1(3);
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}

			if (dun_level > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;

				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				} while (one_in_(6));
			}
		}
		else if (trap_monster)
		{
			msg_format("%s�̑����ŉ������s�J�b�ƌ������I", m_name);

			project(0, 0, y, x, 100 + dun_level * 3, GF_DEC_ALL, (PROJECT_JUMP | PROJECT_KILL), -1);
			project(0, 0, y, x, 10 + dun_level / 2, GF_NO_MOVE, (PROJECT_JUMP | PROJECT_KILL), -1);
		}
		else
		{
			msg_print("�������s�J�b�ƌ������B�������􂢂͕s���̂悤���B");
		}


	}
	break;

	//��ȃ��[���@�����X�^�[�����񂾂�AWAY_ALL�ł������낤
	case TRAP_TELEPORT:
	{
		if (trap_player)
		{
#ifdef JP
			msg_print("�e���|�[�g�E�g���b�v�ɂЂ����������I");
#else
			msg_print("You hit a teleport trap!");
#endif
			teleport_player(100, TELEPORT_PASSIVE);
		}
		else if (trap_monster)
		{
			msg_format("%s�̓e���|�[�g�E�g���b�v�ɂЂ����������I", m_name);
			project(0, 0, y, x, 100, GF_AWAY_ALL, (PROJECT_JUMP | PROJECT_KILL), -1);
		}
		else
		{
			msg_print("��ȃ��[�����N�������B�������s���̂悤���B");
		}


	}
	break;

	//���@�P���ɉΉ��{�[�����N�����Ĕ͈͓��S���_���[�W
	case TRAP_FIRE:
	{

#ifdef JP
		msg_print("�n�ʂ��牊�������o�����I");
#else
		msg_print("You are enveloped in flames!");
#endif

		damage = MAX(5, dun_level) * 3;
		project(PROJECT_WHO_TRAP, 2, y, x, damage, GF_FIRE, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

	}
	break;


	case TRAP_ACID:
	{
#ifdef JP
		msg_print("�n�ʂ���_�������o�����I");
#else
		msg_print("You are splashed with acid!");
#endif

		damage = MAX(5, dun_level) * 3;

		project(PROJECT_WHO_TRAP, 2, y, x, damage, GF_ACID, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

	}
	break;

	case TRAP_POISON:
	{
#ifdef JP
		msg_print("�n�ʂ���ΐF�̃K�X�������o�����I");
#else
		msg_print("A pungent green gas surrounds you!");
#endif

		damage = MAX(5, dun_level) * 3;
		project(PROJECT_WHO_TRAP, 3, y, x, damage, GF_POIS, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

	}
	break;


	//�_�[�c�@�����X�^�[�ɂ͔\�͒ቺ�����U��
	case TRAP_SLOW:
	case TRAP_LOSE_STR:
	case TRAP_LOSE_DEX:
	case TRAP_LOSE_CON:
	{
		if (trap_player)
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				damage = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, damage, "�_�[�c���", -1);
#else
				take_hit(DAMAGE_ATTACK, damage, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW())
				{
					if (trap_feat_type == TRAP_SLOW) set_slow(p_ptr->slow + randint0(20) + 20, FALSE);
					if (trap_feat_type == TRAP_LOSE_STR) do_dec_stat(A_STR);
					if (trap_feat_type == TRAP_LOSE_DEX) do_dec_stat(A_DEX);
					if (trap_feat_type == TRAP_LOSE_CON) do_dec_stat(A_CON);
				}
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}

		}
		else if (trap_monster)
		{
			int power = 100 + dun_level * 2;
			msg_format("�����ȃ_�[�c����񂾁I");

			if (trap_feat_type == TRAP_SLOW)
				project(0, 0, y, x, power, GF_OLD_SLOW, (PROJECT_JUMP | PROJECT_KILL), -1);
			if (trap_feat_type == TRAP_LOSE_STR)
				project(0, 0, y, x, power, GF_DEC_ATK, (PROJECT_JUMP | PROJECT_KILL), -1);
			if (trap_feat_type == TRAP_LOSE_DEX)
				project(0, 0, y, x, power, GF_DEC_ATK, (PROJECT_JUMP | PROJECT_KILL), -1);
			if (trap_feat_type == TRAP_LOSE_CON)
				project(0, 0, y, x, power, GF_DEC_DEF, (PROJECT_JUMP | PROJECT_KILL), -1);

		}
		else
		{
			msg_print("�����ȃ_�[�c�����ŏ��Ɏh�������B");
		}

	}
	break;

	//�ӖڃK�X�@�����X�^�[�ɂ͖Ӗڏ�Ԃ��Ȃ��̂ō����Ƃ��Ĉ����B
	//�܂����ɉe�����y�ڂ��Ȃ����������{�[���𔭐������A�����͈͓��ɂ�����Ӗڏ������s���B
	case TRAP_BLIND:
	{
		int power = 150 + dun_level * 3;
#ifdef JP
		msg_print("�n�ʂ��獕���K�X�������o�����I");
#else
		msg_print("A black gas surrounds you!");
#endif
		project(0, 3, y, x, power, GF_OLD_CONF, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);

		if (!p_ptr->resist_blind && distance(y, x, py, px) < 3)
		{
			(void)set_blind(p_ptr->blind + randint0(50) + 25);
		}
	}
	break;
	//�����K�X�@�P���ɍ����{�[���𔭐�������
	case TRAP_CONFUSE:
	{
		int power = 150 + dun_level * 3;
#ifdef JP
		msg_print("�n�ʂ��炫��߂��K�X�������o�����I");
#else
		msg_print("A gas of scintillating colors surrounds you!");
#endif
		project(PROJECT_WHO_TRAP, 3, y, x, power, GF_OLD_CONF, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL | PROJECT_PLAYER), -1);
	}
	break;

	//�����K�X�@����������project_p()�ōs����̂ł����ŏ����Ȃ��čς�
	case TRAP_SLEEP:
	{
		int power = 150 + dun_level * 3;
#ifdef JP
		msg_print("�n�ʂ��甒���K�X�������o�����I");
#else
		msg_print("A gas of scintillating colors surrounds you!");
#endif
		project(PROJECT_WHO_TRAP, 3, y, x, power, GF_OLD_SLEEP, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL | PROJECT_PLAYER), -1);

	}
	break;

	//�R���p�N�g���[��(�g���b�v�ǉ�����)�@��ɉ������悤�Ɠ������ʂł������낤
	case TRAP_TRAPS:
	{
#ifdef JP
		msg_print("�܂΂䂢�M�����������I");
#else
		msg_print("There is a bright flash of light!");
#endif
		/* Make some new traps */
		project(0, 1, y, x, 0, GF_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);

	}
	break;

	//�x��@������STUN�����̃{�[������
	//�Ƃ͂����X�s�[�h�����X�^�[�����邩�痘�p���邱�Ƃ͂Ȃ����낤�B
	//�X�s�[�h�����X�^�[�Ȃ��ŋN���������ɂ���H���ꂾ�Ɠ��Z�̌x��ƕ���킵�����B
	case TRAP_ALARM:
	{
		int power = 150 + dun_level * 3;
#ifdef JP
		msg_print("�������܂��������苿�����I");
#else
		msg_print("An alarm sounds!");
#endif

		aggravate_monsters(0, FALSE);
		project(PROJECT_WHO_TRAP, 1, y, x, power, GF_STUN, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL), -1);

	}
	break;

	//�J��g���b�v
	//�Ƃ肠�����N������ł��������ʂɂ��Ă����B
	//�����X�^�[�ɓ��܂����炻�����^�[�Q�b�g�ɂ���悤�ݒ肷��̂��ʔ������A�n���}�Q�h���ƈႢ�J��ł͂��łɃ����X�^�[�����邩����߂ă^�[�Q�b�g�ݒ肵�����̂�����B
	//�J��g���b�v�Ŕz�u����郂���X�^�[�̐������ɓ���t���O�𗧂ĂĂ����A�g���b�v�𓥂񂾂�^�[�Q�b�g��؂�ւ���H�t���A�ɊJ��g���b�v�����������H
	case TRAP_OPEN:
	{
#ifdef JP
		msg_print("�剹���Ƌ��ɂ܂��̕ǂ����ꂽ�I");
#else
		msg_print("Suddenly, surrounding walls are opened!");
#endif
		(void)project(0, 3, y, x, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
		(void)project(0, 3, y, x - 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
		(void)project(0, 3, y, x + 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
		aggravate_monsters(0, TRUE);

		break;
	}

	//�n���}�Q�h���g���b�v
	//�����X�^�[�ɓ��܂����炻�����^�[�Q�b�g�ɂ���悤�ݒ肵�Ă݂�
	case TRAP_ARMAGEDDON:
	{
		static int levs[10] = { 0, 0, 20, 10, 5, 3, 2, 1, 1, 1 };
		int evil_idx = 0, good_idx = 0;

		int lev;

		if (trap_player)
		{
#ifdef JP
			msg_print("�ˑR�V�E�̐푈�Ɋ������܂ꂽ�I");
#else
			msg_print("Suddenly, you are surrounded by immotal beings!");
#endif
		}
		else if (trap_monster)
		{
			msg_format("%s�͓V�E�̐푈�Ɋ������܂ꂽ�I", m_name);

		}
		else
		{
			msg_print("�ˑR�V�E�̐푈�������N�������I");

		}

		/* Summon Demons and Angels */
		for (lev = dun_level; lev >= 20; lev -= 1 + lev / 16)
		{
			int i;
			int num = levs[MIN(lev / 10, 9)];
			for (i = 0; i < num; i++)
			{
				monster_type *evil_ptr;
				monster_type *good_ptr;

				int x1 = rand_spread(x, 7);
				int y1 = rand_spread(y, 5);

				/* Skip illegal grids */
				if (!in_bounds(y1, x1)) continue;

				/* Require line of projection */
				if (!projectable(py, px, y1, x1)) continue;

				if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_EVIL, (PM_NO_PET)))
				{
					evil_idx = hack_m_idx_ii;
					evil_ptr = &m_list[evil_idx];
				}

				if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_GOOD, (PM_NO_PET)))
				{
					good_idx = hack_m_idx_ii;
					good_ptr = &m_list[good_idx];
				}

				/* Let them fight each other */
				if (evil_idx && good_idx)
				{
					evil_ptr->target_y = good_ptr->fy;
					evil_ptr->target_x = good_ptr->fx;
					good_ptr->target_y = evil_ptr->fy;
					good_ptr->target_x = evil_ptr->fx;
				}
				//�����X�^�[�ɓ��܂����ꍇ�A�������͂łȂ��ق�����W���U�����󂯂邱�Ƃɂ��Ă݂�B�������ނƂ��ɔ�ׂėy���ɑ�_���[�W���󂯂邩������Ȃ�
				if (trap_monster && !(r_ptr->flags3 & RF3_ANG_CHAOS) && evil_idx)
				{
					evil_ptr->target_y = y;
					evil_ptr->target_x = x;
				}
				if (trap_monster && !(r_ptr->flags3 & RF3_ANG_COSMOS) && good_idx)
				{
					good_ptr->target_y = y;
					good_ptr->target_x = x;
				}


			}
		}
	}
	break;

	//�s���j�A�g���b�v�@�N������ł������ł������낤
	case TRAP_PIRANHA:
	{
		int num, i;
#ifdef JP
		msg_print("�ˑR�ǂ��琅�����o�����I�s���j�A������I");
#else
		msg_print("Suddenly, the room is filled with water with piranhas!");
#endif

		//�n�`�ύX����O�ɂ��̃O���b�h�̃g���b�v�����ł�����
		cave_alter_feat(y, x, FF_HIT_TRAP);

		/* Water fills room */
		fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);

		/* Summon Piranhas */
		num = 1 + dun_level / 20;
		for (i = 0; i < num; i++)
		{
			(void)summon_specific(0, y, x, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
		}
	}
	break;


	//�r�[���
	case TRAP_BEAM:
	{
		int tmp_y, tmp_x;
		int i,j, k, dir;
		byte dir_table[8] = { 1,2,3,4,6,7,8,9 };
		byte dir_init = randint0(8);
		bool flag = FALSE;

		for (i = 0; i<8; i++)
		{
			//�����_���ȕ��p�����߂�
			k = dir_init + i;
			if (k >= 8) k -= 8;
			dir = dir_table[k];

			//�^�[�Q�b�g�O���b�h����1�O���b�h�ȏ�J���ĕǂȂǂ�����ʒu��T��
			for (j = 1; j<16; j++)
			{
				tmp_x = x + ddx[dir] * j;
				tmp_y = y + ddy[dir] * j;

				if (!in_bounds(tmp_y, tmp_x)) break;

				if (!cave_have_flag_bold(tmp_y, tmp_x, FF_PROJECT))
				{
					if (j == 1) break;
					else
					{
						tmp_x = x + ddx[dir] * (j - 1);
						tmp_y = y + ddy[dir] * (j - 1);
						flag = TRUE;
						break;
					}
				}

			}
			if (flag) break;
		}

		//���x�ǂ��ꏊ���������ꍇ�������烌�[�U�[����
		if (flag)
		{
			int typ;
			int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU | PROJECT_PLAYER;

			damage = dun_level * 2 + randint1(50);
			if (randint1(dun_level + 50) > 50) typ = GF_NUKE;
			else typ = GF_FIRE;

			//-hack- �ʏ�project()�̓����X�^�[�����̈ʒu���炵�������Ȃ��̂ŃO���[�o���ϐ��g���ė�O�����@��@���z���܂���� :)
			hack_project_start_x = tmp_x;
			hack_project_start_y = tmp_y;

			if (one_in_(7))
				msg_print("*���ɏĂ��܂����[*");
			else if (trap_player)
				msg_print("�ǂ����炩�r�[�������ł����I");
			else
				msg_print("�ǂ���r�[���������o���ꂽ�I");

			(void)project(PROJECT_WHO_TRAP, 0, y, x, damage, typ, flg, -1);
			hack_project_start_x = 0;
			hack_project_start_y = 0;

		}
		else
		{
			msg_print("�����������N����Ȃ������悤���B");
		}

		break;
	}


	//���������������Ă������ۗ��B
	//f_info�̃C���f�b�N�X��255�𒴂���̂łǂ����ɖ�肪�N���邩������Ȃ��B����������������������Ƃ��ɂ��
	/*
	case TRAP_CEILING: //�݂�V��g���b�v ��������150+1d150�̃_���[�W
	{

		msg_print("�����𗧂ĂēV�䂪�����Ă����I");
		project_hack3(PROJECT_WHO_TRAP, y, x, GF_DISP_ALL, 1, 150, 150);

	}
	break;

	case TRAP_OIL: //�Ζ�
	{

#ifdef JP
		msg_print("�n�ʂ���Ζ��������o�����I");
#else
		msg_print("A gas of scintillating colors surrounds you!");
#endif
		project(PROJECT_WHO_TRAP, 6, y, x, 250, GF_DIG_OIL, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

	}
	break;

	case TRAP_SUIKI: //���S�S�_����㩁@���_���[�W+���n�`����+�����Ԉړ��֎~
	{

#ifdef JP
		msg_print("�n�ʂ��璴�����̐��������o�����I");
#else
		msg_print("A gas of scintillating colors surrounds you!");
#endif

		project(PROJECT_WHO_TRAP, 4, y, x, 200, GF_WATER_FLOW, (PROJECT_JUMP | PROJECT_GRID), -1);
		project(PROJECT_WHO_TRAP, 4, y, x, 200, GF_WATER, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);
		project(PROJECT_WHO_TRAP, 1, y, x, 100, GF_NO_MOVE, (PROJECT_JUMP | PROJECT_HIDE | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);
	}
	break;


	//�Ԍ���g���b�v�@��̃t���A�ɔ�΂����
	//�N�G�X�g�_���W������n����K�▲���L�t���A�Ȃǂɐ�������Ȃ��悤����
	case TRAP_GEYSER:
	{

		msg_print("�Ԍ��򂪕����o�����I");
		damage = damroll(10, 10) + dun_level * 2;
		project(PROJECT_WHO_TRAP, 3, y, x, damage, GF_STEAM, (PROJECT_JUMP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_GRID), -1);

		//���͏�̃t���A�ɔ�΂����
		if (trap_player && p_ptr->chp >= 0 && dun_level > 1 && !p_ptr->inside_arena && !(EXTRA_MODE) && !p_ptr->anti_tele)
		{
			do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�Ԍ���ɐ����グ��ꂽ");
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_UP | CFM_RAND_PLACE | CFM_RAND_CONNECT);

			msg_print("���Ȃ��͏�̃t���A�ɐ�����΂��ꂽ�I");

			//�N�G�X�g�t���A�ɂ͐�������Ȃ��悤�ɂ���K�v������
			if (p_ptr->inside_quest)
			{
				leave_quest_check();
				p_ptr->inside_quest = 0;
			}
			p_ptr->leaving = TRUE;

		}
		//�����X�^�[��QUESTOR��GIGANTIC�͍s���x���A����ȊO�̓t���A����Ǖ�
		//���C�{�[���œ|�����Ƃ��͉������Ȃ�
		else if (trap_monster && m_ptr->r_idx)
		{
			if (r_ptr->flags1 & RF1_QUESTOR || r_ptr->flags2 & RF2_GIGANTIC)
			{
				msg_format("%s�͐�����΂���Ȃ������B", m_name);
				monster_delay(c_ptr->m_idx, randint1(100));
			}
			else
			{
				msg_format("%s�͊Ԍ���ɐ�����΂���Ă��̃t���A����������B", m_name);
				monster_drop_carried_objects(m_ptr);//�E�����A�C�e���͗��V�ɗ��Ƃ��Ă�������邱�Ƃɂ��Ă���
				delete_monster_idx(c_ptr->m_idx);
			}

		}

	}
	break;

	//���ʂɒn������낤�B
	//�����X�^�[�u�n���v���폜����H���̏ꍇ�V���̍������l�߂���H��m�كN�G�̒n����u���������肷��K�v������B
	case TRAP_MINE:
	{
	msg_print("�n�����I");

	msg_print("(������)");

	}

	case TRAP_MEGACRASH: //���K�N���b�V���n�� MEGACRASH��MEGACRUSH�̂ǂ������������񂾂낤
	{
		msg_print("����Ȕ������N�������I");
		destroy_area(y, x, 10, FALSE, FALSE, FALSE);
		if (distance(py, px, y, x)<10)
		{
			if (check_activated_nameless_arts(JKF1_DESTRUCT_DEF))
			{
				msg_format("���Ȃ��͔j��̗͂𕨂Ƃ����Ȃ������I");
				break;
			}
			damage = p_ptr->chp / 2;
			if (p_ptr->pclass != CLASS_CLOWNPIECE)
			{
				//v1.1.86 NOESCAPE��ATTACK��
				take_hit(DAMAGE_ATTACK, damage, m_name, -1);
			}
			if (p_ptr->pseikaku != SEIKAKU_BERSERK || one_in_(7))
			{
				msg_format("���Ȃ��͐�����΂��ꂽ�I");
				teleport_player_away(0, 100);
			}
		}
	}
	break;


	//���g���b�v�H㩎d�l�ύXmemo2.txt�Ɍ������e


	//��̎��������܂ꂽ���ɒ����̒���ꂽ覐΂������Ă���Ƃ��H


	*/

	default:
		msg_format("ERROR:activate_trap()�ɂ�trap_feat_type%d�̔������ʂ����o�^", trap_feat_type);
		return;

	}



	//㩔�����̏��ŏ���
	if (is_trap(c_ptr->feat))
	{
		//�g���b�v���ӈړ��Ȃǂ̂Ƃ��g���b�v����
		if (flag_break_trap)
		{
			cave_alter_feat(y, x, FF_DISARM);
#ifdef JP
			msg_print("�g���b�v�𕲍ӂ����B");
#else
			msg_print("You destroyed the trap.");
#endif
			/* Remove "unsafe" flag if player is not blind */
			if (!p_ptr->blind && player_has_los_bold(y, x))
			{
				c_ptr->info &= ~(CAVE_UNSAFE);
				/* Redraw */
				lite_spot(y, x);
			}
		}
		//����ȊO�̔����̂Ƃ���1/6�Ńg���b�v����
		else if (one_in_(6))
		{
			cave_alter_feat(y, x, FF_DISARM);
		}
		//�����łȂ��Ƃ��̓g���b�v���Ƃɐݒ肳�ꂽ�����B
		else
		{
			cave_alter_feat(y, x, FF_HIT_TRAP);
		}
	}


}


//v1.1.96 hit_trap()��activate_floor_trap()�ɒu��������
#if 0

/*
 * Handle player hitting a real trap
 */
/*:::�����g���b�v�ɂ��������ꍇ*/
///mod160813 �T�O���̏����̂���extern����
void hit_trap(bool break_trap)
{
	int i, num, dam;
	int x = px, y = py;

	/* Get the cave grid */
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

#ifdef JP
	cptr name = "�g���b�v";
#else
	cptr name = "a trap";
#endif

	/* Disturb the player */
	disturb(0, 1);

	if(world_player || SAKUYA_WORLD)
	{
		msg_print("�g���b�v�͍쓮���Ȃ������B");
		return;
	}


	cave_alter_feat(y, x, FF_HIT_TRAP);

	if(p_ptr->pclass == CLASS_FLAN) break_trap = TRUE;
	if(p_ptr->pseikaku == SEIKAKU_BERSERK && randint1(p_ptr->lev) > 10 )break_trap = TRUE;

	/* Analyze XXX XXX XXX */
	switch (trap_feat_type)
	{
		case TRAP_TRAPDOOR:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("���Ƃ��˂��щz�����B");
#else
				msg_print("You fly over a trap door.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("���Ƃ��˂ɗ������I");
				///msg131223
				//if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
				//	msg_print("�������`�I");
#else
				msg_print("You have fallen through a trap door!");
#endif

				sound(SOUND_FALL);
				dam = damroll(2, 8);
#ifdef JP
				name = "���Ƃ���";
#else
				name = "a trap door";
#endif

				take_hit(DAMAGE_NOESCAPE, dam, name, -1);

				/* Still alive and autosave enabled */
				if (autosave_l && (p_ptr->chp >= 0))
					do_cmd_save_game(TRUE);

#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "���Ƃ��˂ɗ�����");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "You have fallen through a trap door!");
#endif
				///mod160813 �N�G�X�g�t���A�Ńg���b�v�h�A�ɗ������Ƃ��̏�����ǉ��@�T�O���p
				if(EXTRA_MODE)
				{
					prepare_change_floor_mode(CFM_DOWN | CFM_RAND_PLACE | CFM_NO_RETURN);
					if(INSIDE_EXTRA_QUEST)
					{
						leave_quest_check();
						p_ptr->inside_quest = 0;
					}
				}
				else
				{
					prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);
					if(p_ptr->inside_quest)
					{
						leave_quest_check();
						p_ptr->inside_quest = 0;
					}
				}

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
			break;
		}

		case TRAP_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("���Ƃ������щz�����B");
#else
				msg_print("You fly over a pit trap.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("���Ƃ����ɗ����Ă��܂����I");
#else
				msg_print("You have fallen into a pit!");
#endif

				dam = damroll(2, 6);
#ifdef JP
				name = "���Ƃ���";
#else
				name = "a pit trap";
#endif

				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}
			break;
		}

		case TRAP_SPIKED_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("�g�Q�̂��闎�Ƃ������щz�����B");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("�X�p�C�N���~���ꂽ���Ƃ����ɗ����Ă��܂����I");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
#ifdef JP
				name = "���Ƃ���";
#else
				name = "a pit trap";
#endif

				dam = damroll(2, 6);

				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("�X�p�C�N���h�������I");
#else
					msg_print("You are impaled!");
#endif


#ifdef JP
					name = "�g�Q�̂��闎�Ƃ���";
#else
					name = "a spiked pit";
#endif

					dam = dam * 2;
					(void)set_cut(p_ptr->cut + randint1(dam));
				}

				/* Take the damage */
				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}
			break;
		}

		case TRAP_POISON_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("�g�Q�̂��闎�Ƃ������щz�����B");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
			msg_print("�X�p�C�N���~���ꂽ���Ƃ����ɗ����Ă��܂����I");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
				dam = damroll(2, 6);

#ifdef JP
				name = "���Ƃ���";
#else
				name = "a pit trap";
#endif


				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("�ł�h��ꂽ�X�p�C�N���h�������I");
#else
					msg_print("You are impaled on poisonous spikes!");
#endif


#ifdef JP
					name = "�g�Q�̂��闎�Ƃ���";
#else
					name = "a spiked pit";
#endif


					dam = dam * 2;
					(void)set_cut(p_ptr->cut + randint1(dam));

					if (p_ptr->resist_pois || IS_OPPOSE_POIS())
					{
#ifdef JP
						msg_print("�������ł̉e���͂Ȃ������I");
#else
						msg_print("The poison does not affect you!");
#endif

					}

					else
					{
						dam = dam * 2;
						(void)set_poisoned(p_ptr->poisoned + randint1(dam));
					}
				}

				/* Take the damage */
				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}

			break;
		}

		case TRAP_TY_CURSE:
		{
#ifdef JP
			msg_print("�������s�J�b�ƌ������I");
#else
			msg_print("There is a flash of shimmering light!");
#endif

			num = 2 + randint1(3);
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}

			if (dun_level > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;

				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				}
				while (one_in_(6));
			}
			break;
		}

		case TRAP_TELEPORT:
		{
#ifdef JP
			msg_print("�e���|�[�g�E�g���b�v�ɂЂ����������I");
#else
			msg_print("You hit a teleport trap!");
#endif

			teleport_player(100, TELEPORT_PASSIVE);
			break;
		}

		case TRAP_FIRE:
		{
#ifdef JP
			msg_print("���ɕ�܂ꂽ�I");
#else
			msg_print("You are enveloped in flames!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)fire_dam(dam, "���̃g���b�v", -1);
#else
			(void)fire_dam(dam, "a fire trap", -1);
#endif

			break;
		}

		case TRAP_ACID:
		{
#ifdef JP
			msg_print("�_������������ꂽ�I");
#else
			msg_print("You are splashed with acid!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)acid_dam(dam, "�_�̃g���b�v", -1);
#else
			(void)acid_dam(dam, "an acid trap", -1);
#endif

			break;
		}

		case TRAP_SLOW:
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "�_�[�c���", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)set_slow(p_ptr->slow + randint0(20) + 20, FALSE);
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_STR:
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "�_�[�c���", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_STR);
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_DEX:
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "�_�[�c���", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_DEX);
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_CON:
		{
			if (check_hit_trap(125))
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł��Ďh�������I");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "�_�[�c���", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_CON);
			}
			else
			{
#ifdef JP
				msg_print("�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_BLIND:
		{
#ifdef JP
			msg_print("�����K�X�ɕ�ݍ��܂ꂽ�I");
#else
			msg_print("A black gas surrounds you!");
#endif

			if (!p_ptr->resist_blind)
			{
				(void)set_blind(p_ptr->blind + randint0(50) + 25);
			}
			break;
		}

		case TRAP_CONFUSE:
		{
#ifdef JP
			msg_print("����߂��K�X�ɕ�ݍ��܂ꂽ�I");
#else
			msg_print("A gas of scintillating colors surrounds you!");
#endif

			if (!p_ptr->resist_conf)
			{
				(void)set_confused(p_ptr->confused + randint0(20) + 10);
			}
			break;
		}

		case TRAP_POISON:
		{
#ifdef JP
			msg_print("�h���I�ȗΐF�̃K�X�ɕ�ݍ��܂ꂽ�I");
#else
			msg_print("A pungent green gas surrounds you!");
#endif

			if (!p_ptr->resist_pois && !IS_OPPOSE_POIS())
			{
				(void)set_poisoned(p_ptr->poisoned + randint0(20) + 10);
				if(p_ptr->muta2 & MUT2_ASTHMA) set_asthma(p_ptr->asthma + 3000);
			}
			break;
		}

		case TRAP_SLEEP:
		{
#ifdef JP
			msg_print("��Ȕ������ɕ�܂ꂽ�I");
#else
			msg_print("A strange white mist surrounds you!");
#endif

			if (!p_ptr->free_act)
			{
#ifdef JP
msg_print("���Ȃ��͖���ɏA�����B");
#else
				msg_print("You fall asleep.");
#endif


				if (ironman_nightmare)
				{
#ifdef JP
msg_print("�g�̖т��悾���i�����ɕ����񂾁B");
#else
					msg_print("A horrible vision enters your mind.");
#endif


					/* Pick a nightmare */
					get_mon_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					have_nightmare(get_mon_num(MAX_DEPTH));

					/* Remove the monster restriction */
					get_mon_num_prep(NULL, NULL);
				}
				(void)set_paralyzed(p_ptr->paralyzed + randint0(10) + 5);
			}
			break;
		}

		case TRAP_TRAPS:
		{
#ifdef JP
msg_print("�܂΂䂢�M�����������I");
#else
			msg_print("There is a bright flash of light!");
#endif

			/* Make some new traps */
			project(0, 1, y, x, 0, GF_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);

			break;
		}

		case TRAP_ALARM:
		{
#ifdef JP
			msg_print("�������܂��������苿�����I");
#else
			msg_print("An alarm sounds!");
#endif

			aggravate_monsters(0,TRUE);

			break;
		}

		case TRAP_OPEN:
		{
#ifdef JP
			msg_print("�剹���Ƌ��ɂ܂��̕ǂ����ꂽ�I");
#else
			msg_print("Suddenly, surrounding walls are opened!");
#endif
			(void)project(0, 3, y, x, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x - 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x + 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			aggravate_monsters(0,TRUE);

			break;
		}

		case TRAP_ARMAGEDDON:
		{
			static int levs[10] = {0, 0, 20, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;

			int lev;
#ifdef JP
			msg_print("�ˑR�V�E�̐푈�Ɋ������܂ꂽ�I");
#else
			msg_print("Suddenly, you are surrounded by immotal beings!");
#endif

			/* Summon Demons and Angels */
			for (lev = dun_level; lev >= 20; lev -= 1 + lev/16)
			{
				num = levs[MIN(lev/10, 9)];
				for (i = 0; i < num; i++)
				{
					int x1 = rand_spread(x, 7);
					int y1 = rand_spread(y, 5);

					/* Skip illegal grids */
					if (!in_bounds(y1, x1)) continue;

					/* Require line of projection */
					if (!projectable(py, px, y1, x1)) continue;

					if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_EVIL, (PM_NO_PET)))
						evil_idx = hack_m_idx_ii;

					if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_GOOD, (PM_NO_PET)))
					{
						good_idx = hack_m_idx_ii;
					}

					/* Let them fight each other */
					if (evil_idx && good_idx)
					{
						monster_type *evil_ptr = &m_list[evil_idx];
						monster_type *good_ptr = &m_list[good_idx];
						evil_ptr->target_y = good_ptr->fy;
						evil_ptr->target_x = good_ptr->fx;
						good_ptr->target_y = evil_ptr->fy;
						good_ptr->target_x = evil_ptr->fx;
					}
				}
			}
			break;
		}

		case TRAP_PIRANHA:
		{
#ifdef JP
			msg_print("�ˑR�ǂ��琅�����o�����I�s���j�A������I");
#else
			msg_print("Suddenly, the room is filled with water with piranhas!");
#endif

			/* Water fills room */
			fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);

			/* Summon Piranhas */
			num = 1 + dun_level/20;
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
			}
			break;
		}



		//�r�[���
		case TRAP_BEAM:
		{
			int tmp_y,tmp_x;
			int j,k,dir;
			byte dir_table[8] = {1,2,3,4,6,7,8,9};
			byte dir_init = randint0(8);
			bool flag=FALSE;

			for(i=0;i<8;i++)
			{
				//�����_���ȕ��p�����߂�
				k = dir_init + i;
				if(k>=8) k -= 8;
				dir=dir_table[k];

				//x,y(���̈ʒu�̂͂�)����1�O���b�h�ȏ�J���ĕǂȂǂ�����ʒu��T��
				for(j=1;j<16;j++)
				{
					tmp_x = x + ddx[dir] * j;
					tmp_y = y + ddy[dir] * j;

					if(!in_bounds(tmp_y,tmp_x)) break;

					if(!cave_have_flag_bold(tmp_y,tmp_x,FF_PROJECT))
					{
						if(j==1) break;
						else
						{
							tmp_x = x + ddx[dir] * (j-1);
							tmp_y = y + ddy[dir] * (j-1);
							flag = TRUE;
							break;
						}
					}

				}
				if(flag) break;
			}

			//���x�ǂ��ꏊ���������ꍇ�������灗�̈ʒu�փ��[�U�[����
			if(flag)
			{
				int typ;
				int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU | PROJECT_PLAYER;

				dam = dun_level * 2 + randint1(50);
				if(randint1(dun_level+50) > 50) typ = GF_NUKE;
				else typ = GF_FIRE;

				//-hack- �ʏ�project()�̓����X�^�[�����̈ʒu���炵�������Ȃ��̂ŃO���[�o���ϐ��g���ė�O�����@��@���z���܂���� :)
				hack_project_start_x = tmp_x;
				hack_project_start_y = tmp_y;

				if(one_in_(7))
					msg_print("*���ɏĂ��܂����[*");
				else
					msg_print("�ǂ����炩�r�[�������ł����I");

				(void)project(PROJECT_WHO_TRAP, 0, py, px, dam, typ, flg, -1);
				hack_project_start_x = 0;
				hack_project_start_y = 0;

			}
			else
			{
				msg_print("�����������N����Ȃ������悤���B");
			}
	
			break;
		}




	}

	if (break_trap && is_trap(c_ptr->feat))
	{
		cave_alter_feat(y, x, FF_DISARM);
#ifdef JP
		msg_print("�g���b�v�𕲍ӂ����B");
#else
		msg_print("You destroyed the trap.");
#endif
	}
}

#endif

/*:::�I�[���ɂ��_���[�W���󂯂鏈��*/
///mon �I�[���_���[�W
///mod140211 ���̃\�[�X�t�@�C������g�����߂�static���O�����B
void touch_zap_player(monster_type *m_ptr)
{
	int aura_damage = 0;
	int momoyo_gain_mana = 0;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	//�h�b�y���Q���K�[���ꏈ��
	if(r_ptr->flags7 & RF7_DOPPELGANGER)
	{
		msg_print("���Ȃ��͎����̃h�b�y���Q���K�[�ɐG��Ă��܂����I");
		take_hit(DAMAGE_LOSELIFE,p_ptr->chp,"�h�b�y���Q���K�[�Ƃ̐ڐG",-1);
		set_stun(100+randint0(3));

		return;
	}

	if (r_ptr->flags2 & RF2_AURA_FIRE)
	{
		aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));

		//v1.1.89 �S�X���h���S���C�[�^�[�ɂ��I�[���z��
		if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[2])
		{
			momoyo_gain_mana += aura_damage;

		}
		else if (!p_ptr->immune_fire)
		{
			char aura_dam[80];


			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

#ifdef JP
			msg_print("�ˑR�ƂĂ��M���Ȃ����I");
#else
			msg_print("You are suddenly very hot!");
#endif
			/*
			if (prace_is_(RACE_ENT)) aura_damage += aura_damage / 3;
			if (IS_OPPOSE_FIRE()) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_fire) aura_damage = (aura_damage + 2) / 3;
			*/

			///mod140302 ���f��_�A�ϐ���������
			aura_damage = mod_fire_dam(aura_damage);

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_AURA_FIRE;
			handle_stuff();
		}
	}
	///mod131231 �����X�^�[�t���O�ύX
	if (r_ptr->flags2 & RF2_AURA_COLD)
	//if (r_ptr->flags3 & RF3_AURA_COLD)
	{
		aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));
		//v1.1.89 �S�X���h���S���C�[�^�[�ɂ��I�[���z��
		if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[2])
		{
			momoyo_gain_mana += aura_damage;

		}
		else if (!p_ptr->immune_cold)
		{
			char aura_dam[80];


			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

#ifdef JP
			msg_print("�ˑR�ƂĂ������Ȃ����I");
#else
			msg_print("You are suddenly very cold!");
#endif
			/*
			if (IS_OPPOSE_COLD()) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_cold) aura_damage = (aura_damage + 2) / 3;
			*/
			///mod140302 ���f��_�A�ϐ���������
			aura_damage = mod_cold_dam(aura_damage);

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
///mod131231 �����X�^�[�t���O�ύX
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_AURA_COLD;
			//if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_AURA_COLD;
			handle_stuff();
		}
	}

	if (r_ptr->flags2 & RF2_AURA_ELEC)
	{
		aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));
		//v1.1.89 �S�X���h���S���C�[�^�[�ɂ��I�[���z��
		if (p_ptr->pclass == CLASS_MOMOYO && p_ptr->tim_general[2])
		{
			momoyo_gain_mana += aura_damage;

		}

		else if (!p_ptr->immune_elec)
		{
			char aura_dam[80];


			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

			/*
			if (IS_OPPOSE_ELEC()) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_elec) aura_damage = (aura_damage + 2) / 3;
			*/
			///mod140302 ���f��_�A�ϐ���������
			aura_damage = mod_elec_dam(aura_damage);


#ifdef JP
			msg_print("�d������������I");
#else
			msg_print("You get zapped!");
#endif

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_AURA_ELEC;
			handle_stuff();
		}
	}

	momoyo_gain_mana /= 3;

	if (momoyo_gain_mana && !p_ptr->is_dead)
	{
		msg_print("���Ȃ��͗��т���ꂽ�I�[�����疂�͂�B�������I");
		player_gain_mana(momoyo_gain_mana);

	}


}

/*:::�ψق̕��ʂɂ��U��:::*/
/*:::m_idx:�����X�^�[m_list[]��ID
 *:::attack:���ʂ̎��
 *:::fear �����X�^�[�����|�������ǂ���
 *:::mdeath �����X�^�[�����񂾂��ǂ���
 */
///sys mutation �ψٕ��ʂɂ��U���@�ǉ��i���Ɠ����\��
///mod140323 �ψٕ��ʍU���͕ʃ��[�`���Ŏ������A���܂͂����͎g���Ă��Ȃ�
#if 0
static void natural_attack(s16b m_idx, int attack, bool *fear, bool *mdeath)
{
	int             k, bonus, chance;
	int             n_weight = 0;
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];

	int             dss, ddd;

	cptr            atk_desc;

	/*:::�U�����ʂɂ��_�C�X�Əd�ʂ�����*/
	switch (attack)
	{
		case MUT2_HARDHEAD:
			dss = 3;
			ddd = 7;
			n_weight = 5;
#ifdef JP
			atk_desc = "�Γ�";
#else
			atk_desc = "tail";
#endif

			break;
		case MUT2_HORNS:
			dss = 2;
			ddd = 6;
			n_weight = 15;
#ifdef JP
			atk_desc = "�p";
#else
			atk_desc = "horns";
#endif

			break;
		case MUT2_BIGHORN:
			dss = 4;
			ddd = 6;
			n_weight = 30;
#ifdef JP
			atk_desc = "�傫�Ȋp";
#else
			atk_desc = "beak";
#endif

			break;
		case MUT2_TAIL:
			dss = 1;
			ddd = 4;
			n_weight = 35;
#ifdef JP
			atk_desc = "�K��";
#else
			atk_desc = "trunk";
#endif

			break;
		case MUT2_BIGTAIL:
			dss = 2;
			ddd = 5;
			n_weight = 5;
#ifdef JP
			atk_desc = "�傫�ȐK��";
#else
			atk_desc = "tentacles";
#endif

			break;
		default:
			dss = ddd = n_weight = 1;
#ifdef JP
			atk_desc = "����`�̕���";
#else
			atk_desc = "undefined body part";
#endif

	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);


	/* Calculate the "attack quality" */
	bonus = p_ptr->to_h_m;
	bonus += (p_ptr->lev * 6 / 5);
	chance = (p_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

	/* Test for hit */
	/*:::��������*/
	if ((!(r_ptr->flags2 & RF2_QUANTUM) || !randint0(2)) && test_hit_norm(chance, r_ptr->ac, m_ptr->ml))
	{
		/* Sound */
		sound(SOUND_HIT);

#ifdef JP
		msg_format("%s��%s�ōU�������B", m_name, atk_desc);
#else
		msg_format("You hit %s with your %s.", m_name, atk_desc);
#endif

		/*:::�_�C�X�_���[�W�A�N���e�B�J���_���[�W�v�Z*/
		k = damroll(ddd, dss);
		k = critical_norm(n_weight, bonus, k, (s16b)bonus, 0);

		/* Apply the player damage bonuses */
		k += p_ptr->to_d_m;

		/* No negative damage */
		if (k < 0) k = 0;

		/* Modify the damage */
		k = mon_damage_mod(m_ptr, k, FALSE);
		/*:::�E�B�U�[�h�̓_���[�W��\������*/
		/* Complex message */
		if (p_ptr->wizard)
		{
#ifdef JP
				msg_format("%d/%d �̃_���[�W��^�����B", k, m_ptr->hp);
#else
			msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
#endif

		}

		/* Anger the monster */
		/*:::�G��{�点��*/
		if (k > 0) anger_monster(m_ptr);

		/* Damage, check for fear and mdeath */
		switch (attack)
		{
			case MUT2_HARDHEAD:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_HORNS:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_BIGHORN:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_TAIL:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_BIGTAIL:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			default:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
		}

		touch_zap_player(m_ptr);
	}
	/* Player misses */
	else
	{
		/* Sound */
		sound(SOUND_MISS);

		/* Message */
#ifdef JP
			msg_format("�~�X�I %s�ɂ��킳�ꂽ�B", m_name);
#else
		msg_format("You miss %s.", m_name);
#endif

	}
}
#endif

//�e�������ɓ���i�����s����Ƃ��A�������Ă���e�̎�ނɂ����MELEE_MODE_*��Ԃ�
int select_gun_melee_mode(void)
{
	object_type *o_ptr;
	int melee_mode[2] = {0,0};
	int slot;
	bool flag_ammo = TRUE;
	int timeout_base, timeout_max;
	
	for(slot = INVEN_RARM;slot <= INVEN_LARM;slot++)
	{
		o_ptr = &inventory[slot];
		if(o_ptr->tval != TV_GUN) continue;

		//v1.1.70 �����X���b�g��������
		if (check_invalidate_inventory(slot)) continue;

		switch(o_ptr->sval)
		{
		case SV_FIRE_GUN_LUNATIC:
		case SV_FIRE_GUN_HANDGUN:
		case SV_FIRE_GUN_CRIMSON:
		case SV_FIRE_GUN_MAGNUM:
		case SV_FIRE_GUN_LUNATIC_2:
			melee_mode[slot-INVEN_RARM] =  MELEE_MODE_GUN_HANDGUN;
			timeout_base = calc_gun_timeout(o_ptr) * 1000;
			timeout_max = timeout_base * (calc_gun_load_num(o_ptr) - 1); 
			if(o_ptr->timeout > timeout_max) flag_ammo = FALSE;

			break;
		case SV_FIRE_GUN_HUNTING:
		case SV_FIRE_GUN_MUSKET:
		case SV_FIRE_GUN_SHOTGUN:
		case SV_FIRE_GUN_LASER:
			melee_mode[slot-INVEN_RARM] =  MELEE_MODE_GUN_LONGBARREL;
			break;
		case SV_FIRE_GUN_INFANTRY:
			melee_mode[slot-INVEN_RARM] =  MELEE_MODE_GUN_BAYONET;
			break;
		case SV_FIRE_GUN_ROCKET:
			melee_mode[slot-INVEN_RARM] =  MELEE_MODE_GUN_LAUNCHER;
			break;
		default:
			msg_print("ERROR:select_gun_melee_mode()�ɂ��̏e�̏������o�^����Ă��Ȃ�");
			return 0;
		}
	}
	//������e�Ŋi���Əe�̋Z�\�������Ƃ�����i�����o��悤�ɂ��Ă݂�
	//v1.1.21 �c�e������Ƃ��������Ȃ����Ƃɂ���
	if(melee_mode[0] == MELEE_MODE_GUN_HANDGUN && melee_mode[1] == MELEE_MODE_GUN_HANDGUN
		&& ref_skill_exp(SKILL_MARTIALARTS) >= 4800 && ref_skill_exp(TV_GUN) >= 4800)
	{
		if(flag_ammo) return MELEE_MODE_GUN_TWOHAND;
	}

	if(melee_mode[0] && (!melee_mode[1] || one_in_(2)))
		return melee_mode[0];
	else
		return melee_mode[1];

}

/*:::����ōU�������Ƃ��̃��b�Z�[�W���u�U�������v����ύX����B*/
///pend �n���x�═��_�C�X��mode�ɉ����Ă����Ɣh��ȕ\���ɂ��Ă���������
cptr msg_py_atk(object_type *o_ptr,int mode)
{
	int tv = o_ptr->tval;
	int sv = o_ptr->sval;
	if(!o_ptr->k_idx)
	{
		msg_print("ERROR:����������Ă��Ȃ��̂�msg_py_atk()���Ă΂ꂽ");
		return "";
	}

	if(!ex_attack_msg) return "���U�������B";

	if(tv == TV_KNIFE)
	{
		if(sv == SV_WEAPON_DOKUBARI)
			 return "��ŉt�H��j�Ŏh�����B";
		else
		{
			if(one_in_(2)) return "���a�����B";
			return "��˂����B";
		}
	}

	if(tv == TV_SWORD)
	{
		//�匕
		if(sv == SV_WEAPON_CLAYMORE || sv == SV_WEAPON_GREAT_SWORD || sv == SV_WEAPON_EXECUTIONERS_SWORD)
		{
			if(one_in_(4)) return "���a�����B";
			if(one_in_(3)) return "�֐n��@���t�����B";
			 return "�֐n��U�艺�낵���B";

		}
		//�h�ˌ�
		else if(sv == SV_WEAPON_RAPIER)
		{
			if(one_in_(4)) return "���a�����B";
			return "��˂����B";

		}
		else
		{
			if(one_in_(4)) return "���a�����B";
			if(one_in_(3)) return "�֎a������B";
			if(one_in_(2)) return "��˂����B";
			return "�֎a�肩�������B";
		}


	}
	if(tv == TV_KATANA)
	{
			if(one_in_(6)) return "�֓���U�艺�낵���B";
			if(one_in_(5)) return "���a��􂢂��B";
			if(one_in_(4)) return "���a�����B";
			if(one_in_(3)) return "�֎a������B";
			if(one_in_(2)) return "�֓˂����������B";
			return "�֎a�肩�������B";
	}
	if(tv == TV_HAMMER)
	{
		if(sv == SV_WEAPON_FLAIL || sv == SV_WEAPON_BALL_AND_CHAIN)
		{
			if(one_in_(4)) return "�֓S���𒼌��������I";
			if(one_in_(3)) return "�֕����U��񂵂��B";
			return "���������B";
		}
		else
		{
			if(one_in_(4)) return "���������B";
			if(one_in_(3)) return "�֕����U�艺�낵���B";
			if(one_in_(2)) return "����������B";
			return "��ł����B";
		}
	}
	if(tv == TV_STICK)
	{
		if (o_ptr->name1 == ART_MEGUMU)
		{

			if (one_in_(3)) return "���O�r�łԂ񉣂����I";
			if (one_in_(2)) return "���O�r�łԂ��@�����I";
			return "�ɎO�r��˂��h�����I";

		}
		else
		{
			if (one_in_(4)) return "���������B";
			if (one_in_(3)) return "��ł��������B";
			if (one_in_(2)) return "��˂����B";
			return "��ガ�������B";
		}

	}

	if(tv == TV_AXE)
	{
		if(one_in_(2)) return "��@���a�����B";
		return "�֕����@�������B";
	}

	if(tv == TV_SPEAR)
	{
		if(!one_in_(4)) return "��˂����B";
		return "��ガ�������B";
	}

	if(tv == TV_POLEARM)
	{
		if(sv == SV_WEAPON_WAR_SCYTHE || sv == SV_WEAPON_SCYTHE_OF_SLICING)
		{
			if(one_in_(2)) return "���a��􂢂��B";
			return "��ガ�������B";
		}
		if(sv == SV_WEAPON_GLAIVE || sv == SV_WEAPON_NAGINATA)
		{
			if(one_in_(3)) return "���a�����B";
			if(one_in_(2)) return "�֐n��U�艺�낵���B";
			return "��˂����B";
		}
		if(sv == SV_WEAPON_LUCERNE_HAMMER)
		{
			if(one_in_(3)) return "���������B";
			if(one_in_(2)) return "��˂����B";
			return "�փn���}�[��@���t�����B";
		}
		if(sv == SV_WEAPON_HALBERD)
		{
			if(one_in_(3)) return "��ガ�������B";
			if(one_in_(2)) return "��˂����B";
			return "�֐n��@���t�����B";
		}


	}

	if(tv == TV_OTHERWEAPON)
	{
		if(sv == SV_OTHERWEAPON_FISHING)
		{
			if(one_in_(10)) return "��݂�グ���I";
			if(one_in_(2)) return "�������@�����B";
			return "��ł��������B";
		}
		else if(sv == SV_OTHERWEAPON_NEEDLE)
		{
			if(one_in_(3)) return "���h�����I";
			if(one_in_(2)) return "��˂����I";
			return "�ɐj��˂����Ă��I";
		}
	}

	return "���U�������B";

}


/*:::@�̌��݂̏�Ԃ���f��U���́u���ށv�����肷��*/
//findmode:����U�����[�h ���̂Ƃ�������
//  MELEE_FIND_NOHAND�@�肪�g�����L�b�N�����o�Ȃ��@����U�����̒ǉ��U���A����ɉו��������Ă��鎞�̊i��
//  MELEE_FIND_LONGARM ���ꂽ�}�X�܂ōU���������U���@�ϗe���@�u�L�r�v���͓����C�V�����̒ʔw

///mod160111 v1.0.89�ŗD�揇�ʂȂǌ�����
///mod160325 v1.0.99��EXTRA���[�h�Ή��̂��߂܂�������
int find_martial_arts_method(int findmode)
{
	int plev = p_ptr->lev;
	bool kick = FALSE;

	if(findmode == MELEE_FIND_NOHAND) kick = TRUE;

	//if (cheat_xtra)msg_format("findmode:%d", findmode);

/*�D��1:����ϐg �����i�ɂ�����炸��p�̍U��������*/
	switch(p_ptr->mimic_form)
	{
	case MIMIC_NUE://�ʂ��ϐg
		return MELEE_MODE_NUE;
	case MIMIC_BEAST://����ȏb
		return MELEE_MODE_BEAST;
	case MIMIC_SLIME://�X���C����
		return MELEE_MODE_SLIME;
	case MIMIC_CAT://�L��
		return MELEE_MODE_CLAW;
	case MIMIC_DRAGON://����
		return MELEE_MODE_DRAGON;
	case MIMIC_MIST://����
		return MELEE_MODE_MIST;
	case MIMIC_KOSUZU_HYAKKI://v1.1.37 ����ϐg
		return MELEE_MODE_HYAKKI;
	default:
		break;
	}

	if (SUPER_SHION) return MELEE_MODE_SUPER_SHION;

/*�D��2:����Ȉꎞ����*/
	//�A�C�e���J�[�h�u�Œ܁u���ȂȂ��E�l�S�v�v���L�����V�[�ȊO���g������
	if(findmode == MELEE_FIND_EX_CLAW) return MELEE_MODE_EX_CLAW;

	//�E�Ɓu���m�v�ɂ��X�j�[�L���O�����U��
	if(findmode == MELEE_FIND_SNEAKING_KILL) return MELEE_MODE_SNEAKING;

	//�����l
	if(p_ptr->kamioroshi & KAMIOROSHI_ATAGO) return MELEE_MODE_ATAGO;
	//�����ړ�
	if(p_ptr->lightspeed) return MELEE_MODE_LIGHTSPEED;
	//�فX�_�u���X�R�A���t��
	if(p_ptr->pclass == CLASS_BENBEN && music_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE)) return MELEE_MODE_BENBEN;
	//�܋����@���̂Ƃ�kick����ɂ͂Ȃ�Ȃ��͂�
	if(p_ptr->clawextend) return MELEE_MODE_EX_CLAW;


	//�e�������i��
	//v2.0 ����̍U����i�̂Ƃ��K��=�J�^�̃_���[�W�{�[�i�X�𓾂��Ă��܂��̂ŕs�������Ȃ������ߗD��x�������グ��
	if (!kick && (inventory[INVEN_RARM].tval == TV_GUN || inventory[INVEN_LARM].tval == TV_GUN))
	{
		//v1.1.21 �K��=�J�^�ȊO�̂Ƃ�1/2�ŃL�b�N�U��
		int tmp = select_gun_melee_mode();
		//if (cheat_xtra) msg_format("gun_melee_mode:%d", tmp);
		if (tmp == MELEE_MODE_GUN_TWOHAND || !one_in_(2)) return tmp;
		else kick = TRUE;

	}



	//�L�r�U���@�ϗe���@��͓����C�V�����@�ǉ��i�����o�Ȃ��̂Ńp���`�̂� �ꕔ�N���X�͐�p�U���D��
	if(findmode == MELEE_FIND_LONGARM)
	{

		if(p_ptr->pclass == CLASS_REMY) return MELEE_MODE_REMY_OTHER;
		else if(p_ptr->pclass == CLASS_MINDCRAFTER) return MELEE_MODE_MIND;
		else if(p_ptr->pclass == CLASS_MAMIZOU) return MELEE_MODE_MAMIZOU;
		else if(p_ptr->pclass == CLASS_SUWAKO) return MELEE_MODE_SUWAKO;
		else if(p_ptr->pclass == CLASS_YUKARI) return MELEE_MODE_YUKARI;
		else return MELEE_MODE_LONGARM;
	}
	//v1.1.26 俎q���e�i����Ԃŉ��u�U�����������ɕ��ʂ̊i�����ʂ����o�Ȃ����邽�߂̕���
	if(findmode == MELEE_FIND_SUMIREKO_DISTANT)
	{
		return MELEE_MODE_SUMIREKO;
	}

	//v1.1.59 �D�ɏ���Ă���Ƃ��͏M�ő̓��肷�邱�Ƃɂ���
	if (p_ptr->riding)
	{
		int riding_r_idx = m_list[p_ptr->riding].r_idx;

		if (riding_r_idx == MON_YOUR_BOAT || riding_r_idx == MON_ANCIENT_SHIP)
		{
			return MELEE_MODE_BOAT;
		}

	}

/*�D��3:�ꕔ�̎푰������ψٍU��*/
	//�L�X��
	if(p_ptr->pclass == CLASS_KISUME)
	{
		if(p_ptr->concent == 0) return MELEE_MODE_KISUME1;
		if(p_ptr->concent == 1) return MELEE_MODE_KISUME2;
		if(p_ptr->concent == 2) return MELEE_MODE_KISUME3;
		if(p_ptr->concent == 3) return MELEE_MODE_KISUME4;
		else return MELEE_MODE_KISUME5;
	}


	//v1.1.70 ���剻���͋��͂ȍU��(�K��=�J�^���D��x��������)
	if (p_ptr->mimic_form == MIMIC_GIGANTIC)
	{
		if (kick || one_in_(3))	return MELEE_MODE_GIGA_KICK;
		else			return MELEE_MODE_GIGA_PUNCH;
	}

		//�����ω���1/3�ŋ��͂ȍU��
	if (p_ptr->mimic_form == MIMIC_DEMON_LORD && one_in_(3)) 
		return MELEE_MODE_D_LORD;

	//�z���S(���~���A�͕ʏ���)��1/4�ŋz���U��
	if((prace_is_(RACE_VAMPIRE) || p_ptr->mimic_form == MIMIC_VAMPIRE) && p_ptr->pclass != CLASS_REMY && one_in_(4))
		return MELEE_MODE_VAMP;

	//�푰�j���W���̂Ƃ�1/3
	if(p_ptr->prace == RACE_NINJA && one_in_(3))
	{
		if(kick || one_in_(3)) return MELEE_MODE_KICK_NINJA;
		else return MELEE_MODE_PUNCH_NINJA;
	}
	//�푰���ɐ����̂̂Ƃ�1/3
	else if(p_ptr->prace == RACE_ULTIMATE && one_in_(3))
	{
		if(one_in_(10)) return MELEE_MODE_FISH_TAIL;
		else if(one_in_(9)) return MELEE_MODE_BIGHORN;
		else if(one_in_(8)) return MELEE_MODE_BIGTAIL;
		else if(one_in_(7)) return MELEE_MODE_VAMP;
		else 	return MELEE_MODE_ULTIMATE;
	}

	//�ψ�
	if(p_ptr->muta2 & MUT2_BIGHORN && one_in_(10)) return MELEE_MODE_BIGHORN;
	if(p_ptr->muta2 & MUT2_BIGTAIL && one_in_(8)) return MELEE_MODE_BIGTAIL;
	if(p_ptr->muta2 & MUT2_HORNS && one_in_(14)) return MELEE_MODE_HORNS;
	if(p_ptr->muta2 & MUT2_HARDHEAD && one_in_(7)) return MELEE_MODE_HEADBUTT;

	//v1.1.78 ����
	if (p_ptr->special_attack & ATTACK_SUIKEN)
		return MELEE_MODE_SUIKEN;


/*�D��4:��p�U�����o��E�� */
	///pend �����̐E�Ƃ͎肪�ǂ����ĂĂ��p���`�Ƃ��o�Ă��܂��̂ŉɂ�����Β�������
	switch(p_ptr->pclass)
	{
	case CLASS_MINDCRAFTER:
		return MELEE_MODE_MIND;
	case CLASS_NINJA:
		return MELEE_MODE_NINJA;

	//�i���Ƃ̐�p�U���͑f�莞�̂�
	case CLASS_MARTIAL_ARTIST:
		if((randint0(100) < plev+30) && !kick) return MELEE_MODE_MA;
		else break;
	case CLASS_YUKARI:
		return MELEE_MODE_YUKARI;

	case CLASS_KOISHI:
		return MELEE_MODE_KOISHI;
	//v1.1.23
	case CLASS_NITORI:
		return MELEE_MODE_NITORI;
	case CLASS_CIRNO:
		return MELEE_MODE_CIRNO;
	case CLASS_YOSHIKA:
		return MELEE_MODE_YOSHIKA;
	case CLASS_HINA:
		return MELEE_MODE_HINA;
	case CLASS_MEDICINE:
		return MELEE_MODE_MEDI;
	case CLASS_ICHIRIN:
		return MELEE_MODE_ICHIRIN;
	case CLASS_SUMIREKO:
		return MELEE_MODE_SUMIREKO;
	case CLASS_FUTO:
		return MELEE_MODE_FUTO;
	case CLASS_TOZIKO:
		return MELEE_MODE_TOZIKO;
	case CLASS_YUYUKO:
		return MELEE_MODE_YUYUKO;
	case CLASS_MOKOU:
		return MELEE_MODE_MOKOU;
	case CLASS_MEIRIN:
		return MELEE_MODE_MEIRIN;
	case CLASS_3_FAIRIES:
		if (is_special_seikaku(SEIKAKU_SPECIAL_3_FAIRIES))
			return MELEE_MODE_3_FAIRIES_2; //v1.1.68
		else
			return MELEE_MODE_3_FAIRIES;
	case CLASS_KANAKO:
		return MELEE_MODE_KANAKO;
	case CLASS_RAIKO:
		return MELEE_MODE_RAIKO;
	case CLASS_MARISA:
		return MELEE_MODE_MARISA;
	case CLASS_MAMIZOU:
		return MELEE_MODE_MAMIZOU;

	case CLASS_MURASA:
		{ //���т͎����̂���n�`�ōU�����ς��
			feature_type *f_ptr = &f_info[cave[py][px].feat];
			if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) && !one_in_(3)) return MELEE_MODE_MURASA2;
			else  return MELEE_MODE_MURASA1;
		}
	case CLASS_MIKO:
		return MELEE_MODE_MIKO;
	case CLASS_KOKORO:
		return MELEE_MODE_KOKORO;
	case CLASS_UTSUHO:
		return MELEE_MODE_UTSUHO;
	case CLASS_TENSHI:
		return MELEE_MODE_TENSHI;
	case CLASS_SUWAKO:
		return MELEE_MODE_SUWAKO;
	case CLASS_SHINMYOU_2:
		//�A�C�e���u�V�l�̒O�v�Ŏ푰��l�ɂȂ����炨�o�̊i�����g���Ȃ����Ƃɂ���
		if(p_ptr->prace != RACE_SENNIN)
			return MELEE_MODE_SHINMYOU2;
		else break;
	case CLASS_REIMU: //�얲���ΑK�����N�ōU���̎�ނ��ς��
		if(1 + randint1(10) < osaisen_rank()) return MELEE_MODE_REIMU2;
		else return MELEE_MODE_REIMU1;
	case CLASS_RAN: //���̉�]�U����1/4
		if(one_in_(4)) return MELEE_MODE_RAN;
		else break;
	case CLASS_CHEN: //��̓���U���͔G��Ă��Ȃ����ɂ����o�Ȃ�
		if( !p_ptr->magic_num1[0]) return MELEE_MODE_CHEN;
		else break;
	case CLASS_BYAKUREN: //���@�̓���U���͋������̂�
		if(p_ptr->tim_general[0]) return MELEE_MODE_HIZIRI;
		else break;
	case CLASS_IKU:
		if(!kick) return MELEE_MODE_DRILL;
		else break;

	case CLASS_REMY:
		if(one_in_(3) && !kick) return MELEE_MODE_REMY_HAND;
		else if(one_in_(2)) return MELEE_MODE_REMY_KICK;
		else return MELEE_MODE_REMY_OTHER;
	case CLASS_KASEN:
		if(is_special_seikaku(SEIKAKU_SPECIAL_KASEN))//v1.1.65
			return MELEE_MODE_ONI_KASEN;
		else
			return MELEE_MODE_KASEN;

	case CLASS_UDONGE:
		return MELEE_MODE_UDONGE;

	case CLASS_ORIN:
		//v1.1.77 ���ӂ̓���i��(�L��)�͓��ꐫ�i�łȂ��Ƃ��̂�
		if(one_in_(3)&& !is_special_seikaku(SEIKAKU_SPECIAL_ORIN)) return MELEE_MODE_ORIN;
		else break;

	case CLASS_DOREMY:
		return MELEE_MODE_DOREMY;
		break;

	case CLASS_JYOON:
		if(is_special_seikaku(SEIKAKU_SPECIAL_JYOON))
			return MELEE_MODE_JYOON_3;
		else if (p_ptr->special_attack & ATTACK_WASTE_MONEY)
			return MELEE_MODE_JYOON_2;
		else
			return MELEE_MODE_JYOON_1;




	}




/*�D��5:��ʓI�ȍU����i����I��*/

	if(!kick)//�p���`���L�b�N���g����Ƃ��A�ǂ���ɂȂ邩�I��
	{
		int kick_chance;

		if(p_ptr->pclass == CLASS_SHIZUHA || p_ptr->pclass == CLASS_ALICE || p_ptr->pclass == CLASS_SAKI) //�×t�ƃA���X�̓L�b�N�����o�Ȃ� v1.1.71 ���S�ǉ�
			kick_chance = 100;
		else if(p_ptr->pclass == CLASS_WRIGGLE || p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_KUTAKA)//���O���ƕفX�̓L�b�N���o�₷��
			kick_chance = 70;
		else if(p_ptr->prace == RACE_NINGYO) //�l���̓L�b�N(=�l���̔�)���o�₷��
			kick_chance = 50;
		else
			kick_chance = 35;

		if( randint0(100) < kick_chance ) kick = TRUE;

	}

	if(kick)
	{
		//�l���̔�������Ƃ��R��U���͕K�����U���ɂȂ�
		if(p_ptr->muta3 & MUT3_FISH_TAIL) return MELEE_MODE_FISH_TAIL;
		//�L�����V�[�̏R��U���͑S�����ݕt���ɂȂ�
		if(prace_is_(RACE_ZOMBIE)) return MELEE_MODE_BITE; 

		//v1.1.71 ���S ���͂ȃL�b�N���o�₷��
		if (p_ptr->pclass == CLASS_SAKI)
		{
			if(randint1(60) < p_ptr->lev) 
				return MELEE_MODE_KICK2;
			else
				return MELEE_MODE_KICK1;
		}

		//�i���̋����E�͋��͂ȃL�b�N
		if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 4000 || p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			return MELEE_MODE_KICK2;

		//�S�̏R��͊m���Ŋp�U���ɂȂ�
		if(prace_is_(RACE_ONI) && one_in_(2)) return MELEE_MODE_BIGHORN;

		//���T�V��A�b�l�̏R����m���Ŋ��ݕt���U���ɂȂ�(���܂Ȃ����ȃC���[�W�̃L�����͏��O)
		if(prace_is_(RACE_HAKUROU_TENGU) && one_in_(2)) return MELEE_MODE_BITE;
		if(p_ptr->prace == RACE_WARBEAST && one_in_(2) 
			&& p_ptr->pclass != CLASS_KEINE && p_ptr->pclass != CLASS_ORIN && p_ptr->pclass != CLASS_NAZRIN && p_ptr->pclass != CLASS_SHOU)
			return MELEE_MODE_BITE;

		return MELEE_MODE_KICK1; //���ʂ̏R��
	}
	else
	{

		//�����̃p���`�͋Ղ̒܂ōU��
		if(p_ptr->pclass == CLASS_YATSUHASHI )	return MELEE_MODE_CLAW;

		//�p���`���������~�������ȃC���[�W�̃L����
		if(p_ptr->pclass == CLASS_YAMAME || p_ptr->pclass == CLASS_MYSTIA || p_ptr->pclass == CLASS_KAGEROU || p_ptr->pclass == CLASS_ORIN)
		{
			if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 4000) return MELEE_MODE_EX_CLAW;
			else return MELEE_MODE_CLAW;
		}
		//���܂Ɉ����~�����芚�ݕt�����肵�����ȃL����
		if(p_ptr->pclass == CLASS_RUMIA)
		{
			if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 5000) return MELEE_MODE_EX_CLAW;
			else if(one_in_(3)) return MELEE_MODE_CLAW;
			else if(one_in_(2)) return MELEE_MODE_PUNCH1;
			else return MELEE_MODE_BITE;
		}


		//�i���̋����E�͋��͂ȃp���`
		if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 4000 || p_ptr->pclass == CLASS_MARTIAL_ARTIST)
			return MELEE_MODE_PUNCH2;

		//�L�����V�[���ꏈ��
		if(prace_is_(RACE_ZOMBIE))
		{
			if(randint1(ref_skill_exp(SKILL_MARTIALARTS)) > 4000) return MELEE_MODE_YOSHIKA;
			else if(one_in_(2)) return MELEE_MODE_BITE;
			else return MELEE_MODE_CLAW;
		}

		if(prace_is_(RACE_ONI)) return MELEE_MODE_PUNCH_ONI;
		if(prace_is_(RACE_DAIYOUKAI) && one_in_(2)) return MELEE_MODE_EX_CLAW;
		if(prace_is_(RACE_WARBEAST) && p_ptr->pclass != CLASS_KEINE && one_in_(2)) return MELEE_MODE_CLAW;

		if (prace_is_(RACE_ANIMAL_GHOST) && one_in_(2)) return MELEE_MODE_CLAW;

		return MELEE_MODE_PUNCH1; //���ʂ̉���
	}

}


/*
 * Player attacks a (poor, defenseless) creature        -RAK-
 *
 * If no "weapon" is available, then "punch" the monster one time.
 */
/*:::��ɂ��U�� �f��̎�hand=0�ň�x�Ă΂��
 *:::x,y:�Ώۃ����X�^�[������ʒu
 *:::hand:0�Ȃ�E��A1�Ȃ獶��
 *:::mode:�K�E���ȂǓ���Ȓʏ�U���̂Ƃ��ȊO0*/

///class sys item �i�������@��ɂ��U������
static void py_attack_aux(int y, int x, bool *fear, bool *mdeath, s16b hand, int mode)
{
	int		num = 0, k, bonus, chance;
	//int vir;

	cave_type       *c_ptr = &cave[y][x];

	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	/* Access the weapon */
	object_type     *o_ptr = &inventory[INVEN_RARM + hand];

	char            m_name[80];

	bool            success_hit = FALSE;
	bool            backstab = FALSE;
	bool            sneakingkill = FALSE; //�E�Ɓu���m�v�p�o�b�N�X�^�b�u
	bool            vorpal_cut = FALSE;
	int             chaos_effect = 0;
	bool            stab_fleeing = FALSE;
	bool            fuiuchi = FALSE;
	bool            monk_attack = FALSE;
	bool            do_quake = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE; //�z���ł��邩�@
	bool            can_feed = FALSE;
	int             num_blow;
	int             drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[TR_FLAG_SIZE]; /* A massive hack -- life-draining weapons */ /*���z�������łȂ��؂ꖡ�ɂ��g�����Ƃɂ���*/
	bool            is_human = (r_ptr->d_char == 'p');
	bool            is_lowlevel = (r_ptr->level < (p_ptr->lev - 15));
	bool            zantetsu_mukou, e_j_mukou;

	bool			weapon_invalid = FALSE;

	//���n���x�{�[�i�X�́u�K�E�̈ꑾ���v���o���Ƃ��̃t���O�@������fuiuchi+�E�҃q�b�g�Ɠ���
	bool			flag_katana_critical = FALSE;

	//�񒚌��e�i���@�c�e����
	bool		flag_gun_kata = FALSE;

	//v1.1.42 �h���~�[�E�X�C�[�g�̊i���U���Ń����X�^�[�𖰂点��t���O�J�E���g
	bool		counting_sheep = 0;

	bool	flag_vamp_armour = FALSE; //�h��ɂ��z�������邩�ǂ���

	int			martial_arts_method = 0;//�i���̂Ƃ��Ƀ����_���ɑI�΂��i�����ރ^�C�v MELEE_MODE_***

	//v1.1.94
	int		skill_type = 0;	//�Y������X�L�� ref_skill_exp()�Ɏg��

	int i;

	//v1.1.69 �������Z�����̂Ƃ�����U�������d���[�h�ɂȂ�
	// py_attack()���ɋL�q����ƒǉ��i�����o�Ȃ��Ȃ�̂ł����ɏ�����
	if (p_ptr->pclass == CLASS_URUMI && !mode && p_ptr->tim_general[0] && !p_ptr->do_martialarts)
	{
		mode = HISSATSU_URUMI;
	}

	//v2.0.2�@���Z�u�n���ڂꂷ��܂Ő؂荏�߁v�͐n�̕t��������łȂ��Ɣ������Ȃ�
	if (mode == HISSATSU_HAKOBORE && !object_has_a_blade(o_ptr)) return;

	//v1.1.94 ���ɂ��J�E���^�[�U���͑��łȂ��Ɣ������Ȃ�
	if (mode == HISSATSU_COUNTER_SPEAR && o_ptr->tval != TV_SPEAR) return;

	///mod140914 ���p�Ƃ̕K�E�Z�ł͌����Ă��Ȃ�����̍U���������������
	if((mode == HISSATSU_2 || mode == HISSATSU_ISSEN || mode == HISSATSU_UNDEAD) && o_ptr->tval != TV_KATANA) return;
	if(mode == HISSATSU_MAJIN && o_ptr->tval != TV_AXE) return;
	if((mode == HISSATSU_3WAY || mode == HISSATSU_HAGAN )&& !object_has_a_blade(o_ptr)) return;
	if((mode == HISSATSU_ZANMA || mode == HISSATSU_FUKI || mode == HISSATSU_COLD || mode == HISSATSU_ELEC || mode == HISSATSU_DOUBLE) && o_ptr->tval != TV_KATANA && o_ptr->tval != TV_SWORD) return;
	if(mode == HISSATSU_QUAKE && o_ptr->weight < 100) return;
	if(mode == HISSATSU_3DAN && o_ptr->tval != TV_SPEAR && o_ptr->tval != TV_POLEARM) return;

	//v1.1.14 �_���l���ꏈ��
	if(mode == HISSATSU_GION && o_ptr->tval != TV_KATANA && o_ptr->tval != TV_SWORD) return;

	///mod140216 �i�������ύX
	//if(!o_ptr->k_idx && !p_ptr->riding) monk_attack = TRUE;
	//if(empty_hands(TRUE) & EMPTY_HAND_RARM) monk_attack = TRUE;
	if(p_ptr->do_martialarts) monk_attack = TRUE;

	if(check_invalidate_inventory(INVEN_RARM + hand)) weapon_invalid = TRUE;


	if (check_equip_specific_fixed_art(ART_ECSEDI, TRUE)) flag_vamp_armour = TRUE;

	//v1.1.99 �z���t���O�t���̖h��𒅂��Ă���Ƃ�����ɋz�����ʂ����
	//���Ƃ́��G���[�U�x�g�������������剅��G�S�O���[�u���ǉ������̂ň����𐮗�
	for (i = INVEN_RIBBON; i <= INVEN_FEET; i++)
	{
		object_type *tmp_o_ptr = &inventory[i];
		u32b tmp_flgs[TR_FLAG_SIZE];

		object_flags(tmp_o_ptr, tmp_flgs);

		if (have_flag(tmp_flgs, TR_VAMPIRIC))
		{
			if (cheat_xtra) msg_print("TESTMEG:VAMP ARMOUR");
			flag_vamp_armour = TRUE;
			break;
		}
	}

	//�E�Ƃ��Ƃɕs�ӑł�����Ȃǂ̓��ꏈ��
	switch (p_ptr->pclass)
	{
	case CLASS_KOISHI:
		{
			int tmp = p_ptr->lev * 6;
			if(r_ptr->flags2 & RF2_EMPTY_MIND) tmp = 0;
			else if(r_ptr->flags2 & RF2_WEIRD_MIND) tmp /= 4;
			else if(r_ptr->flags2 & RF2_ELDRITCH_HORROR) tmp /= 2;
			else if(!monster_living(r_ptr)) tmp = tmp * 2 / 3;
			if(randint0(tmp) > r_ptr->level) fuiuchi = TRUE;
	
		}
		break;

	case CLASS_SOLDIER:

		//v1.1.21 ���m(�X�j�[�L���O�Z�\����)�̕s�ӑł����� ���s�����畁�ʂ̃o�b�N�X�^�b�u
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_COMBAT,SS_C_SNEAKING))
		{
			int tmp = r_ptr->level;
			bool flag_fail = FALSE;

			//�Q�Ă�G�łȂ��Ɩ���
			//�O�̂��߂ɉ��炩�̍U�����[�h���͖����ɂ��Ƃ�
			if (MON_CSLEEP(m_ptr) && m_ptr->ml  && !mode)
			{

				if(((r_ptr->flags1 & RF1_UNIQUE) ||(r_ptr->flags7 & RF7_UNIQUE2))) flag_fail = TRUE;
				if(!monster_living(r_ptr)) flag_fail = TRUE;

				if(r_ptr->flags2 & RF2_GIGANTIC) tmp *= 2;
				if(!(r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN))) tmp *= 3;

				if(flag_fail || randint1(tmp) > randint1(p_ptr->lev))
					backstab = TRUE;
				else
					sneakingkill = TRUE;
			}
		}
		//v1.1.22 �Î��Z�\�̂Ƃ��̕s�ӑł�����
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_NOCTO_VISION) && !(cave[py][px].info & CAVE_GLOW) && p_ptr->cur_lite <= 0 )
		{

			if (MON_CSLEEP(m_ptr) && m_ptr->ml)
				backstab = TRUE;
			else if(check_mon_blind(c_ptr->m_idx)) 
				fuiuchi = TRUE;

		}
		//v1.1.22 �����Z�\�̕s�ӑł�����
		if(CHECK_CONCEALMENT)
		{
			if (MON_CSLEEP(m_ptr) && m_ptr->ml)
				backstab = TRUE;
			else if (randint1(r_ptr->level * 2) < (p_ptr->lev + p_ptr->skill_stl * 2))
				fuiuchi = TRUE;
		}

		break;

	/*:::�����ƔE�҂̓���Ō�*/
		///mod141221 �E�ғ��ꔻ�����������
	case CLASS_NINJA:
		if (monk_attack || o_ptr->tval == TV_KNIFE)
		{
			int tmp = p_ptr->lev * 6 + (p_ptr->skill_stl + 10) * 4;
			if (p_ptr->monlite && (mode != HISSATSU_NYUSIN)) tmp /= 3;
			if (p_ptr->cursed & TRC_AGGRAVATE) tmp /= 2;
			if (r_ptr->level > (p_ptr->lev * p_ptr->lev / 20 + 10)) tmp /= 3;
			if (MON_CSLEEP(m_ptr) && m_ptr->ml)
			{
				/* Can't backstab creatures that we can't see, right? */
				backstab = TRUE;
			}
			else if (((p_ptr->special_defense & NINJA_S_STEALTH) || p_ptr->tim_superstealth) && (randint0(tmp) > (r_ptr->level+20)) && m_ptr->ml && !(r_ptr->flagsr & RFR_RES_ALL))
			{
				fuiuchi = TRUE;
			}
			else if (MON_MONFEAR(m_ptr) && m_ptr->ml)
			{
				stab_fleeing = TRUE;
			}
		}
		break;
	case CLASS_CHEN:
			if ( (mode == HISSATSU_NYUSIN && randint1(r_ptr->level * 2) < p_ptr->lev || MON_CSLEEP(m_ptr)) && m_ptr->ml)
			{
				fuiuchi = TRUE;
			}
			break;

		///mod140817 �~�X�e�B�A���ڏ���
	case CLASS_MYSTIA:
		if(check_mon_blind(c_ptr->m_idx)) fuiuchi = TRUE;
		break;

	case CLASS_NUE:
			if ( (p_ptr->cur_lite <= 0 && !(cave[py][px].info & CAVE_GLOW) && randint1(r_ptr->level * 2) < p_ptr->lev || MON_CSLEEP(m_ptr)) && m_ptr->ml)
			{
				fuiuchi = TRUE;
			}
			break;
			//���ۃv���X�e�B���r�[�g���̍U�����x�㏸����
	case CLASS_RAIKO:
		if(music_singing(MUSIC_NEW_RAIKO_PRISTINE) && !hand)
			do_cmd_concentrate(0);
		break;


	/*:::�i���Z�\�҂̊i������*/
	case CLASS_MONK:
	case CLASS_FORCETRAINER:
	//case CLASS_BERSERKER:
		if ((empty_hands(TRUE) & EMPTY_HAND_RARM) && !p_ptr->riding) monk_attack = TRUE;
		break;
	}

	//�E�ƈȊO�ɂ��s�ӑł�����Ȃ�
	if (!fuiuchi && !backstab && !stab_fleeing)
	{
		if (p_ptr->tim_unite_darkness)
		{
			if (!(cave[py][px].info & CAVE_GLOW) && m_ptr->ml && (MON_CSLEEP(m_ptr) || randint1(p_ptr->lev * 3 + (p_ptr->skill_stl + 10) * 4) > r_ptr->level * 2)) fuiuchi = TRUE;
		}

		if (o_ptr->tval == TV_KATANA && check_katana_critical(m_ptr, mode))
		{
			fuiuchi = TRUE;
			flag_katana_critical = TRUE;
		}
		if (mode == HISSATSU_FUIUCHI)
			fuiuchi = TRUE;

		//�f���̉B�`�S�@�B���\�͂ɉ����ĕs�ӑł�����@�u�łƂ̗Z�a�v�Ɠ�������ł�����
		if (mode == HISSATSU_ONGYOU)
		{
			if (m_ptr->ml && (MON_CSLEEP(m_ptr) || randint1(p_ptr->lev * 3 + (p_ptr->skill_stl + 10) * 4) > r_ptr->level * 2)) fuiuchi = TRUE;
		}

	}


	/*:::�f��̂Ƃ��i���Z�\�𓾂�*/
	//if (!o_ptr->k_idx) /* Empty hand */
	if(monk_attack)
	{
		skill_type = SKILL_MARTIALARTS;

		if(p_ptr->pclass == CLASS_MEIRIN)
			do_cmd_concentrate(0);
		///mod131226 skill �Z�\�ƕ���Z�\�̓���
		gain_skill_exp(skill_type, m_ptr);
		/*
		if ((r_ptr->level + 10) > p_ptr->lev)
		{
			if (p_ptr->skill_exp[GINOU_SUDE] < s_info[p_ptr->pclass].s_max[GINOU_SUDE])
			{
				if (p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_BEGINNER)
					p_ptr->skill_exp[GINOU_SUDE] += 40;
				else if ((p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_SKILLED))
					p_ptr->skill_exp[GINOU_SUDE] += 5;
				else if ((p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_EXPERT) && (p_ptr->lev > 19))
					p_ptr->skill_exp[GINOU_SUDE] += 1;
				else if ((p_ptr->lev > 34))
					if (one_in_(3)) p_ptr->skill_exp[GINOU_SUDE] += 1;
				p_ptr->update |= (PU_BONUS);
			}
		}
		*/
	}
	/*:::����̂Ƃ��Ή�����o���l�𓾂�*/
	//v1.1.14 �_���l�������ʂ͏���
	else if (object_is_melee_weapon(o_ptr) && mode != HISSATSU_GION)
	{

		skill_type = o_ptr->tval;

		gain_skill_exp(skill_type,m_ptr);
	/*
		if ((r_ptr->level + 10) > p_ptr->lev)
		{
			int tval = inventory[INVEN_RARM+hand].tval - TV_WEAPON_BEGIN;
			int sval = inventory[INVEN_RARM+hand].sval;
			int now_exp = p_ptr->weapon_exp[tval][sval];
			if (now_exp < s_info[p_ptr->pclass].w_max[tval][sval])
			{
				int amount = 0;
				if (now_exp < WEAPON_EXP_BEGINNER) amount = 80;
				else if (now_exp < WEAPON_EXP_SKILLED) amount = 10;
				else if ((now_exp < WEAPON_EXP_EXPERT) && (p_ptr->lev > 19)) amount = 1;
				else if ((p_ptr->lev > 34) && one_in_(2)) amount = 1;
				p_ptr->weapon_exp[tval][sval] += amount;
				p_ptr->update |= (PU_BONUS);
			}
		}
	*/
		///mod140714 �t�r�_�g�������΂炭������g���Ă���ƕ��킪�d�퉻���鏈��
		///mod151219 ���ےǉ�
		//v1.1.52 �V�A���[�i���ł͗d�퉻���Ȃ��悤�ɂ���
		if((p_ptr->pclass == CLASS_TSUKUMO_MASTER || p_ptr->pclass == CLASS_RAIKO)
			&& !object_is_fixed_artifact(o_ptr) && o_ptr->xtra1 < 100 && !p_ptr->inside_arena)
		{
			if( (r_ptr->level > o_ptr->xtra1 / 2) && (randint0(o_ptr->xtra1) < r_ptr->level) && one_in_(5))
			{
				if(p_ptr->wizard) o_ptr->xtra1 = (o_ptr->xtra1 < 50) ? 50 : 100; 
				else o_ptr->xtra1++; 

				if(o_ptr->xtra1 == 50 || o_ptr->xtra1 == 100) make_evilweapon(o_ptr,r_ptr); 
			}
		}


	}

	/* Disturb the monster */
	(void)set_monster_csleep(c_ptr->m_idx, 0);

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);
	/*:::��������Ȃ�*/
	/* Calculate the "attack quality" */
	bonus = p_ptr->to_h[hand] + o_ptr->to_h;
	chance = (p_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

	if (cheat_xtra)
	{
		msg_format("chance:%d bonus : %d",chance,bonus);

	}
	if (mode == HISSATSU_COUNTER_SPEAR) chance += 60;

	if (mode == HISSATSU_IAI) chance += 60;
	if (mode == HISSATSU_GION) chance += 60;
	if (p_ptr->special_defense & KATA_KOUKIJIN) chance += 150;

	if(mode == HISSATSU_EIGOU)
	{
		chance += 100;
		if(!heavy_armor()) chance *= 3;
	}
	if (p_ptr->sutemi) chance = MAX(chance * 3 / 2, chance + 60);

	//msg_format("chance:%d",chance);

	///sysdel virtue
	//vir = virtue_number(V_VALOUR);
	/*
	if (vir)
	{
		chance += (p_ptr->virtues[vir - 1]/10);
	}
	*/
	///item �a�S����EJ����
	///del131223 �a�S����������
	//zantetsu_mukou = ((o_ptr->name1 == ART_ZANTETSU) && (r_ptr->d_char == 'j'));
	zantetsu_mukou = 0;

	//�V���{���ύXS��c
	e_j_mukou = ((o_ptr->name1 == ART_EXCALIBUR_J) && (r_ptr->d_char == 'c'));

	//��������U���񐔐ݒ�

	//�K�E���A�Őj
	if ((mode == HISSATSU_COUNTER_SPEAR) || (mode == HISSATSU_ATTACK_ONCE) || (mode == HISSATSU_KYUSHO) || (mode == HISSATSU_MINEUCHI) || (mode == HISSATSU_3DAN) || (mode == HISSATSU_IAI))
	{
		num_blow = 1;
	}
	else if (mode == HISSATSU_COLD) num_blow = p_ptr->num_blow[hand]+2;
	/*:::�͓����C�V�����̒ʔw�@�U���񐔂������ɉ�������*/
	else if (mode == HISSATSU_KAPPA)
	{
			num_blow = p_ptr->num_blow[hand] / m_ptr->cdis;
			if(num_blow < 1) num_blow=1;
	}
	/*:::�ψُp�̐L�r�@�����ɉ�����������*/
	else if (mode == HISSATSU_LONGARM)
	{
			num_blow = p_ptr->num_blow[hand] - m_ptr->cdis / 2;
			if(num_blow < 1) num_blow=1;
	}
	//�\�E���X�J���v�`���A�@�G�Ƃ̋����ōU���񐔂�����
	else if(mode == HISSATSU_SOULSCULPTURE)
	{
		if(p_ptr->pclass == CLASS_SAKUYA) num_blow = p_ptr->num_blow[hand] * 7 / (m_ptr->cdis+2);
		else if (p_ptr->pclass == CLASS_NEMUNO)
		{
			if(IS_NEMUNO_IN_SANCTUARY)
				num_blow = p_ptr->num_blow[hand] * 4 / (m_ptr->cdis + 1);
			else
				num_blow = p_ptr->num_blow[hand] * 3 / (m_ptr->cdis + 1);
		}
		else if (p_ptr->pclass == CLASS_KEIKI )
		{
			num_blow = p_ptr->num_blow[hand];
		}
		else if (p_ptr->pclass == CLASS_MEGUMU)
		{
			num_blow = (p_ptr->num_blow[hand]+1)/2;
		}

		else
		{
			msg_print("ERROR:�\�����Ȃ��N���X��GF_SOULSCULPTURE���g��ꂽ");
			num_blow = p_ptr->num_blow[hand];
		}

		if(num_blow < 1) num_blow=1;
	}
	else num_blow = p_ptr->num_blow[hand];


	if(mode == HISSATSU_SHINMYOU) num_blow = num_blow * 3 / 2;
	else if(mode == HISSATSU_UNZAN) num_blow = num_blow * (20+randint0(11)) / 10;
	else if (mode == HISSATSU_HAKOBORE) num_blow *= 2;


	/* Hack -- DOKUBARI always hit once */
	///mod131223 tval
	//if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) num_blow = 1;
	///�ϐg���ɓŐj�������Ă���U����1�ɂȂ�Ȃ��悤�ɂ��Ă���
	if (!weapon_invalid && (o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) num_blow = 1;

	/* Attack once for each legal blow */
	/*:::�U���񐔂��Ƃɔ���*/
	while ((num++ < num_blow) && !p_ptr->is_dead)
	{
		//v1.1.94 
		int		attack_effect_type = ATTACK_EFFECT_NONE; //�N�O�ȂǓG�ɃX�e�[�^�X�ُ��t�^����Ƃ�
		int		critical_rank = 0; //�N���e�B�J����؂ꖡ��s�ӑł��Ȃǂ����܂����Ƃ�������
		int		effect_turns = 0; //�N�O�Ȃǂ̃X�e�[�^�X�ُ��^����^�[����

		bool dokubari = FALSE;
		bool sinker = FALSE;

		int mon_ac = r_ptr->ac;

		//v1.1.94 �����X�^�[�h��͒ቺ����AC25%�J�b�g
		if (MON_DEC_DEF(m_ptr))
		{
			mon_ac = MONSTER_DECREASED_AC(mon_ac);
		}

		///sys item �Őj��������
		///mod131223 tval
		if ((!weapon_invalid && (o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
		{
			int n = 1;
			dokubari = TRUE;

			if (p_ptr->migite && p_ptr->hidarite)
			{
				n *= 2;
			}
			if (mode == HISSATSU_3DAN)
			{
				n *= 2;
			}
			///mod141129 LUNATIC�œŐj������������ɂ���

			if(difficulty == DIFFICULTY_LUNATIC && one_in_(2)) success_hit = one_in_(n*2);
			else success_hit = one_in_(n);
		}
		///class �E�҂̕s�ӑł��͕K������
		//else if ((p_ptr->pclass == CLASS_NINJA) && ((backstab || fuiuchi) && !(r_ptr->flagsr & RFR_RES_ALL))) success_hit = TRUE;
		else if (((backstab || fuiuchi) && !(r_ptr->flagsr & RFR_RES_ALL))) success_hit = TRUE;
		else if(sneakingkill) success_hit = TRUE;
		///�f�P�͍U�����K��
		else if(p_ptr->pclass == CLASS_EIKI) success_hit = TRUE;
		/*:::�ʏ�̖�������*/
		else success_hit = test_hit_norm(chance, mon_ac, m_ptr->ml);

		//v1.1.94
		if (backstab || fuiuchi || stab_fleeing) critical_rank = 3;

		///mod141102 ���Ƃ薽��������␳..���悤���Ǝv�������ǂ�߂�
		//if(p_ptr->pclass == CLASS_SATORI && !p_ptr->blind && !p_ptr->image && p_ptr->confused && m_ptr->ml && !(r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND)) 
		//	&& (randint1(r_ptr->level*3 + 100) < (p_ptr->lev + p_ptr->stat_ind[A_INT]+3) )) success_hit = TRUE;

		/*
		if (mode == HISSATSU_MAJIN)
		{
			if (one_in_(2))
				success_hit = FALSE;
		}
		*/

		/* Test for hit */
		if (success_hit)
		{
			///sys item *�؂ꖡ*���� �t���O�����ɂ���
			//int vorpal_chance = ((o_ptr->name1 == ART_VORPAL_BLADE) || (o_ptr->name1 == ART_CHAINSWORD)) ? 2 : 4;
			int vorpal_chance;

			/* Sound */
			sound(SOUND_HIT);

			if(p_ptr->prace == RACE_STRAYGOD && (backstab || stab_fleeing || fuiuchi || sneakingkill))
			{
				if(fuiuchi)
				{
					if(one_in_(10)) set_deity_bias(DBIAS_COSMOS, -1);
				}
				else
				{
					if(one_in_(3)) set_deity_bias(DBIAS_COSMOS, -1);
					if(one_in_(2)) set_deity_bias(DBIAS_REPUTATION, -1);
				}
			}

			/* Message */
			///sys �U�������b�Z�[�W�@���킲�ƂɕύX���悤���H
			if (p_ptr->pclass == CLASS_KISUME && p_ptr->concent && !monk_attack)
			{
				msg_format("���Ȃ��͕�����\����%s�֌����ė��������I", m_name);
			}
			else if (mode == HISSATSU_GION)
			{
				msg_format("�_���l�̌���%s��؂�􂢂��I", m_name);
			}
			else if (sneakingkill && monk_attack)
			{
				; //���m�̊i���s�ӑł��͂����ł̓��b�Z�[�W���o���Ȃ�
			}
			else if (backstab || sneakingkill)
			{
				msg_format("���Ȃ��͗⍓�ɂ������Ă��閳�͂�%s�֊�P���d�|�����I", m_name);
			}
			else if (fuiuchi)
			{
				if(flag_katana_critical)
					msg_format("%s�ɋC���̈ꌂ��������I", m_name);
				else
					msg_format("�s�ӂ�˂���%s�ɋ���Ȉꌂ�����킹���I", m_name);
			}
			else if (stab_fleeing)
				msg_format("������%s�֔w������P�����������I", m_name);
			//else if (!monk_attack) msg_format("%s���U�������B", m_name);
			else if (!monk_attack)
				msg_format("%s%s", m_name,msg_py_atk(o_ptr,mode));
			//else msg_format("%s���U�������B", m_name);

			//���̖����
			if(p_ptr->pclass == CLASS_HINA) 
				hina_gain_yaku(-(p_ptr->to_dd[hand]) / 2);

			/* Hack -- bare hands do one damage */
			k = 1;

			object_flags(o_ptr, flgs);



			/*:::���ו���̓�����ʌv�Z*/
			/* Select a chaotic effect (50% chance) */
			if ((have_flag(flgs, TR_CHAOTIC)) && one_in_(2) && !weapon_invalid)
			{
				///sysdel virtue
				//if (one_in_(10))
				//chg_virtue(V_CHANCE, 1);

				if (randint1(5) < 3)
				{
					/* Vampiric (20%) */
					chaos_effect = 1;
				}
				else if (one_in_(250))
				{
					/* Quake (0.12%) */
					chaos_effect = 2;
				}
				else if (!one_in_(10))
				{
					/* Confusion (26.892%) */
					chaos_effect = 3;
				}
				else if (one_in_(2))
				{
					/* Teleport away (1.494%) */
					chaos_effect = 4;
				}
				else
				{
					/* Polymorph (1.494%) */
					chaos_effect = 5;
				}
			}

			/* Vampiric drain */
			/*:::�z������*/
			///mod140410 Lv45�ȏ�Ŗ������g���Ƌz���t��*/
			if ((have_flag(flgs, TR_VAMPIRIC) ) || (chaos_effect == 1) || (mode == HISSATSU_DRAIN) || hex_spelling(HEX_VAMP_BLADE) || ((p_ptr->special_attack & (ATTACK_DARK)) && p_ptr->lev > 44))
			{
				/* Only drain "living" monsters */
				if (monster_living(r_ptr))
					can_drain = TRUE;
				else
					can_drain = FALSE;

				if(weapon_invalid) can_drain = FALSE;
			}
			//v1.1.99 �z���h��t���O
			if(monster_living(r_ptr) && flag_vamp_armour)
			{
					can_drain = TRUE;
			}

			//�F������U��
			if(monster_living(r_ptr) && mode == HISSATSU_YOSHIKA)
			{
					can_drain = TRUE;
			}

			//v1.1.46 �؂ꖡ����(vorpal_cut)�����������ɂ����������̕��Ɉڂ���


			/*:::�f��U�����̃��b�Z�[�W�A�З́A���ʍČv�Z*/
			///class �f��U���G�t�F�N�g
			if (monk_attack)
			{
				int mult = 100;
				int special_effect = 0, times = 0, max_times;
				//int stun_effect = 0;
				int min_level = 1;
				//const martial_arts *ma_ptr = &ma_blows[0], *old_ptr = &ma_blows[0];
				const martial_arts_new *ma_ptr = &ma_blows_new[0], *old_ptr = &ma_blows_new[0];
				//int resist_stun = 0;
				int weight = 8;
				int findmode = 0;

				if(mode == HISSATSU_KAPPA || mode == HISSATSU_LONGARM) findmode = MELEE_FIND_LONGARM;
				else if(sneakingkill) findmode = MELEE_FIND_SNEAKING_KILL;
				else if(mode == HISSATSU_YOSHIKA && p_ptr->prace != RACE_ZOMBIE) findmode = MELEE_FIND_EX_CLAW;
				//v1.1.26 俎q���e�i����Ԃŉ��u�U����I�������Ƃ��͕��ʂ̊i�������o�Ȃ��悤�ɂ���
				else if(mode == HISSATSU_NO_AURA && p_ptr->pclass == CLASS_SUMIREKO) findmode = MELEE_FIND_SUMIREKO_DISTANT;

				/*:::�푰�E�ƃ��x���ˑR�ψقȂǂɉ����U�����ނ����肷��*/
				martial_arts_method = find_martial_arts_method(findmode);

				//�K���J�^�t���O
				if(martial_arts_method == MELEE_MODE_GUN_TWOHAND)
					flag_gun_kata = TRUE;

				if(mode == HISSATSU_UNZAN) martial_arts_method = MELEE_MODE_ICHIRIN2;

				/* v1.1.94 �����̏�����apply_special_effect()�Ɉړ�
				if (r_ptr->flags1 & RF1_UNIQUE) resist_stun += 88;
				if (r_ptr->flags2 & RF2_GIGANTIC) resist_stun += 88;
				if (r_ptr->flags3 & RF3_NO_STUN) resist_stun += 66;
				if (r_ptr->flags3 & RF3_NO_CONF) resist_stun += 33;
				if (r_ptr->flags3 & RF3_NO_SLEEP) resist_stun += 33;
				if (r_ptr->flags2 & RF2_POWERFUL) resist_stun += 33;
				if ((r_ptr->flags3 & RF3_UNDEAD) || (r_ptr->flags3 & RF3_NONLIVING))
					resist_stun += 66;
				*/
				/*
				if (p_ptr->special_defense & KAMAE_BYAKKO)
					max_times = (p_ptr->lev < 3 ? 1 : p_ptr->lev / 3);
				else if (p_ptr->special_defense & KAMAE_SUZAKU)
					max_times = 1;
				else if (p_ptr->special_defense & KAMAE_GENBU)
					max_times = 1;
				else
					max_times = (p_ptr->lev < 7 ? 1 : p_ptr->lev / 7);
				*/
				max_times = ref_skill_exp(SKILL_MARTIALARTS) / 2000 + 1;

				/* Attempt 'times' */
				/*:::�f��U���G�t�F�N�g���琔��I�肵��ԍ����x���Ȃ̂�����*/
				for (times = 0; times < max_times; times++)
				{
					if(!martial_arts_method)
					{
						msg_print("ERROR:�f��U�����ނ����܂��Ă��Ȃ�");
						return;
					}


					do
					{
						int mode_pick[50];
						int mode_cnt=0;
						int i;
						for(i = 0;(ma_blows_new[i].method != 0 && mode_cnt < 50);i++)
						{
							if(ma_blows_new[i].method == martial_arts_method)
							{
								mode_pick[mode_cnt++] = i;
							}
						}
						if(!mode_cnt)
						{
							msg_format("ERROR:�w�肳�ꂽ����(%d)�̑f��U��������o�^����Ă��Ȃ�",martial_arts_method);
							return;
						}		

						if(p_ptr->pclass == CLASS_SUMIREKO && mode == HISSATSU_NO_AURA)
						{
							if(mode_cnt < 3)
							{
								msg_print("ERROR:俎q���u�U����₪����Ȃ�");
								return;
							}		
							ma_ptr = &ma_blows_new[ mode_pick[2 + randint0(mode_cnt-2)]];

						}
						else
						{
							//ma_ptr = &ma_blows[randint0(MAX_MA)];
							ma_ptr = &ma_blows_new[ mode_pick[randint0(mode_cnt)]];
							//if ((p_ptr->pclass == CLASS_FORCETRAINER) && (ma_ptr->min_level > 1)) min_level = ma_ptr->min_level + 3;
							//else 
						}
							min_level = ma_ptr->min_level;
					}
					while ((min_level > p_ptr->lev) ||
					       (randint1(p_ptr->lev) < ma_ptr->chance));



					/* keep the highest level attack available we found */
					/*::: Hack - old_ptr�Ƃ̔�r��>���ƁA�f��U����₪���x��1�݂̂̂Ƃ�����ȊO�̕��ނł����邪�I�������̂�>=�ɂ���*/
					//if ((ma_ptr->min_level > old_ptr->min_level) &&
					if ((ma_ptr->min_level >= old_ptr->min_level) &&
					    !p_ptr->stun && !p_ptr->confused)
					{
						old_ptr = ma_ptr;

						if (p_ptr->wizard && cheat_xtra)
						{
#ifdef JP
							msg_print("�U�����đI�����܂����B");
#else
							msg_print("Attack re-selected.");
#endif
						}
					}
					else
					{
						ma_ptr = old_ptr;
					}
				}
				/*:::�i���U��msg*/
				msg_format(ma_ptr->desc, m_name);


				//if (p_ptr->pclass == CLASS_FORCETRAINER) min_level = MAX(1, ma_ptr->min_level - 3);
				//else 
					min_level = ma_ptr->min_level;
					weight = ma_ptr->weight;



				/*:::�i���U���̃_���[�W����*/
				k = damroll(ma_ptr->dd + p_ptr->to_dd[hand], ma_ptr->ds + p_ptr->to_ds[hand]);
				///mod141221 �E�җp��backstab��stab_fleeing��ǉ�
				if (backstab)
				{
					k *= (3 + (p_ptr->lev / 20));
				}
				else if (fuiuchi)
				{
						k = k*(5+(p_ptr->lev*2/25))/2;
				}
				else if (stab_fleeing)
				{
					k = (3 * k) / 2;
				}
				/*:::���ꏈ���@�n���܎E�@*/
				if( martial_arts_method == MELEE_MODE_EX_CLAW && mode == HISSATSU_EXCLAW && !(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_POIS))) 
					if(mult < 200) mult = 200;
				/*:::���ꏈ���@�эj�����~��*/
				if(p_ptr->kamioroshi & KAMIOROSHI_IZUNA && (r_ptr->flags3 & (RF3_ANG_CHAOS)) ) 
					if(mult < 150) mult = 150;

				/*:::���ꏈ���@���}���i��*/
				if( p_ptr->pclass == CLASS_YAMAME && p_ptr->lev > randint0(50) && !(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_POIS))) 
					if(mult < 200) mult = 200;

				//v1.1.89 �S�X��
				if (p_ptr->pclass == CLASS_MOMOYO)
				{
					if(p_ptr->lev > 34 && !(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_POIS)) && mult < 200)
						mult = 200;

					if (p_ptr->tim_general[1] && (r_ptr->d_char == 'J') && mult < 300)//�X�l�[�N�C�[�^�[
						mult = 300;

					if (p_ptr->tim_general[2] && (r_ptr->flags3 & RF3_DRAGON) && mult < 300)//�h���S���C�[�^�[
						mult = 300;
				}

				switch(ma_ptr->effect)
				{
				case MELEE_STUN:
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 3 + randint0(3);
					break;
				case MELEE_STUN2:
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 5 + randint0(5);
					break;
				case MELEE_STUN3:
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 10 + randint0(10);
					break;
				//�얲�i���Ȃ�
				case MELEE_SLAY_EVIL:
					if(r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_KWAI))
					{
						if(mult < 250) mult = 250;
						attack_effect_type = ATTACK_EFFECT_STUN;
						effect_turns = 5 + randint0(5);

					}
					break;
				///mod140217 �ߋ�̉H�ߊi���Ȃǁ@�d���ϐ��̂Ȃ��G�ɃX���C+�N�O
				case MELEE_ELEC:
					if(!(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_ELEC)))
					{
						if(mult<200) mult=200;
						attack_effect_type = ATTACK_EFFECT_STUN;
						effect_turns = 10 + randint0(5);

					}
					break;
				///mod140309 �ؐ�̕�ъi���@�A���f�b�h�X���C+�N�O
				///mod141231 �����X���C���ǉ�
				case MELEE_KASEN:
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 5 + randint0(5);
					if(r_ptr->flags3 & (RF3_UNDEAD) || r_ptr->flags3 & (RF3_ANIMAL))
					{
						if(mult < 300) mult = 300;
						effect_turns = 10 + randint0(10);
					}
					break;
				/*:::Hack �i���z������*/
				case MELEE_VAMP:
					if (monster_living(r_ptr))
					{
						can_drain = TRUE;
						can_feed = TRUE;
					}
					break;
				//�n�����@�_����+�z��
				case MELEE_MELT:
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_ACID))
					{
						if(mult<200) mult=200;
					}
					if (monster_living(r_ptr))
					{
						can_drain = TRUE;
						can_feed = TRUE;
					}
					break;
				//���͂ȉΉ��U��
				case MELEE_ATAGO:
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_FIRE))
					{
						if(r_ptr->flagsr & RFR_HURT_FIRE && mult<500) mult = 500;
						else if(mult<300) mult=300;

						//v1.1.91 �Ζ��n�`�Ή��_���[�W
						if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) mult = oil_field_damage_mod(mult, y, x);

					}
					break;
					//�Ή��U��
				case MELEE_FIRE:
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_FIRE))
					{
						if(r_ptr->flagsr & RFR_HURT_FIRE && mult<300) mult = 300;
						else if(mult<200) mult=200;

						//v1.1.91 �Ζ��n�`�Ή��_���[�W
						if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) mult = oil_field_damage_mod(mult, y, x);

					}
					break;
					//��C�A�X�U��
				case MELEE_COLD:
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_COLD))
					{
						if(r_ptr->flagsr & RFR_HURT_COLD && mult<300) mult = 300;
						else if(mult<200) mult=200;
					}
					break;

				case MELEE_SLOW:
					attack_effect_type = ATTACK_EFFECT_SLOW;
					effect_turns = 5 + randint0(5);

					break;

				case MELEE_POIS: //�őϐ��̂Ȃ��G�ɑ�_���[�W
					if(r_ptr->flagsr & RFR_RES_ALL) k=0;
					else if(!(r_ptr->flagsr & RFR_IM_POIS))
					{
						if(monster_living(r_ptr)) mult = MAX(300,mult);
						else mult = MAX(200,mult);
					}
					break;
				case MELEE_SINK: //���ѐ��v�U��
						if(monster_living(r_ptr) && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (r_ptr->flags1 & RF1_QUESTOR)))
							sinker = TRUE;
						//break���Ȃ�
				case MELEE_WATER:
					if ((r_ptr->flags7 & RF7_AQUATIC) || (r_ptr->flagsr & RFR_RES_WATE))
					{
						mult = 50;
					}
					else
					{
						if (r_ptr->flagsr & RFR_HURT_WATER) mult = 200;
						attack_effect_type = ATTACK_EFFECT_STUN;
						effect_turns = 10 + randint0(10);
					}
					break;
				case MELEE_CONFUSE: //�����t�^
					if(r_ptr->flags3 & RF3_NO_CONF)
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;
					}
					else if(randint0(100) > r_ptr->level)
					{
						msg_format("%^s�͍��������悤���B", m_name);
						(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
					}
					break;

				//v1.1.20�@���ӂ̔L�ԍU���͏d�ʂ��L�Ԃ̒��g�̏d�ʂɂȂ�
				case MELEE_NEKOGURUMA:
					{
						int inven2_num = calc_inven2_num();
						int nekoguruma_weight = 0;
						int i;

						for (i = 0; i < inven2_num; i++)
						{
							object_type *tmp_o_ptr = &inven_add[i];
							if(!tmp_o_ptr->k_idx) continue;

							nekoguruma_weight += tmp_o_ptr->weight * tmp_o_ptr->number;
						}

						// -hack- �d�ʒl�ύX�@���Ƃ�plev���|������Ԃ񌸂炵�Ƃ�
						weight = nekoguruma_weight / p_ptr->lev; 
						if(weight < 1) weight = 1;
						if(weight > 20) weight = 20;
						if(cheat_xtra) msg_format("weight:%d",weight);
						if(nekoguruma_weight > 200)
						{
							attack_effect_type = ATTACK_EFFECT_STUN;
							effect_turns = nekoguruma_weight / 100 + randint1(nekoguruma_weight / 100);

						}
					}
					break;

					//v1.1.42 �h���~�[�����U�� ���̐��E�Ŗ������X���C�A���̐��E�ȊO�ł͐�������t���O�ǉ�
				case MELEE_SLEEP:
					if (IN_DREAM_WORLD)
					{
						mult = 300;
					}
					else
					{
						counting_sheep++;
					}
					break;

				case MELEE_DEC_ATK:
					attack_effect_type = ATTACK_EFFECT_DEC_ATK;
					break;
				case MELEE_DEC_DEF:
					attack_effect_type = ATTACK_EFFECT_DEC_DEF;
					break;
				case MELEE_DEC_MAG:
					attack_effect_type = ATTACK_EFFECT_DEC_MAG;
					break;
				case MELEE_DEC_ALL:
					attack_effect_type = ATTACK_EFFECT_DEC_ALL;
					break;
				case MELEE_DELAY:
					attack_effect_type = ATTACK_EFFECT_DELAY;
					break;

				default:

					//v1.1.47 �~�X�e�B�A�u�q���[�}���P�[�W�v
					if (p_ptr->pclass == CLASS_MYSTIA && music_singing(MUSIC_NEW_MYSTIA_HUMAN_CAGE) && (r_ptr->flags3 & RF3_HUMAN))
					{
						if(mult < 200) mult = 200;
					}

					break;

				}
				k = k * mult / 100;//�i���[���X���C

				/*:::�L�X�����ꏈ�� ����AC�Ȃǂ��_���[�W�ɉ��Z*/
				if(p_ptr->pclass == CLASS_KISUME) k += calc_kisume_bonus();


#if 0
				if (p_ptr->special_attack & ATTACK_SUIKEN) k *= 2;

				if (ma_ptr->effect == MA_KNEE)
				{
					if (r_ptr->flags1 & RF1_MALE)
					{
#ifdef JP
						msg_format("%s�ɋ��I�G�R�������킵���I", m_name);
#else
						msg_format("You hit %s in the groin with your knee!", m_name);
#endif

						sound(SOUND_PAIN);
						special_effect = MA_KNEE;
					}
					else
						msg_format(ma_ptr->desc, m_name);
				}

				else if (ma_ptr->effect == MA_SLOW)
				{
					if (!((r_ptr->flags1 & RF1_NEVER_MOVE) ||
					    my_strchr("~#{}.UjmeEv$,DdsbBFIJQSXclnw!=?", r_ptr->d_char)))
					{
#ifdef JP
						msg_format("%s�̑���Ɋ֐ߏR�������킵���I", m_name);
#else
						msg_format("You kick %s in the ankle.", m_name);
#endif

						special_effect = MA_SLOW;
					}
					else msg_format(ma_ptr->desc, m_name);
				}
				else
				{
					if (ma_ptr->effect)
					{
						stun_effect = (ma_ptr->effect / 2) + randint1(ma_ptr->effect / 2);
					}

					msg_format(ma_ptr->desc, m_name);
				}
#endif

				//if (p_ptr->special_defense & KAMAE_SUZAKU) weight = 4;
				/*:::���C�p�m�ɋC�����܂��Ă�ꍇ�d�ʔ��葝��*/
				if ((p_ptr->pclass == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
				{
					weight += (p_ptr->magic_num1[0]/30);
					if (weight > 20) weight = 20;
				}
				//v1.1.71 ���S�̃L�b�N�ɃN���e�B�J���{�[�i�X
				if (p_ptr->pclass == CLASS_SAKI && (martial_arts_method == MELEE_MODE_KICK1 || martial_arts_method == MELEE_MODE_KICK2))
				{
					weight += 5 + p_ptr->lev / 10;
				}

				/*:::�i���U���̃N���e�B�J������*/

				k = critical_norm(p_ptr->lev * weight, min_level, k, p_ptr->to_h[0], 0, &critical_rank);


				//v1.1.46 �����̓��ꏈ���@���[�h�ɂ���ċ���D�����������肷��
				if (p_ptr->pclass == CLASS_JYOON)
				{
					if (martial_arts_method == MELEE_MODE_JYOON_1)
					{
						int tmp, gain_money;

						tmp = MAX(1, r_ptr->level / 3);

						gain_money = k * tmp + randint1(k * tmp);
						//�|������(���m�ɂ̓_�C�X�l�����œGHP���������Ƃ�)�͎擾���z5�{�ɂ��Ă݂�
						if (k > m_ptr->hp) gain_money = gain_money * 3 + randint1(gain_money * 4);
						//�z������͋���D���Ȃ�
						if (is_pet(m_ptr)) gain_money = 0;
						//�h���b�v�̂Ȃ������X�^�[�͋��������ĂȂ����Ƃɂ��Ă���
						if (!(r_ptr->flags1 & (RF1_DROP_60| RF1_DROP_90 | RF1_DROP_1D2| RF1_DROP_2D2| RF1_DROP_3D2| RF1_DROP_4D2))) 
							gain_money = 0;

						if (gain_money > 0)
						{
							msg_format("$%d�D�����B", gain_money);
							p_ptr->au += gain_money;
							p_ptr->redraw |= PR_GOLD;
						}


					}
					else if (martial_arts_method == MELEE_MODE_JYOON_2)
					{
						int tmp = (p_ptr->lev - 15);
						int waste_money = k * tmp + randint1(k * tmp);

						jyoon_record_money_waste(waste_money);

						if (waste_money > p_ptr->au)
						{

							p_ptr->au = 0;
						}
						else 			
							p_ptr->au -= waste_money;
						p_ptr->redraw |= PR_GOLD;
						//�����Ȃ��Ȃ�������Z�R�}���h�ɂ��U�����[�h���f
						if (!p_ptr->au)
						{
							msg_print("�U�����~�߂��B");
							p_ptr->special_attack &= ~(ATTACK_WASTE_MONEY);
							p_ptr->redraw |= PR_STATUS;

						}


					}
				}

#if 0	//v1.1.94 �i���̏�Ԉُ�t�^������ apply_special_effect()�Ɏ����Ă�����
				if ((special_effect == MA_KNEE) && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
#ifdef JP
					msg_format("%^s�͋�ɂɂ��߂��Ă���I", m_name);
#else
					msg_format("%^s moans in agony!", m_name);
#endif

					stun_effect = 7 + randint1(13);
					resist_stun /= 3;
				}

				else if ((special_effect == MA_SLOW) && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
					if (!(r_ptr->flags1 & RF1_UNIQUE) &&
					    (randint1(p_ptr->lev) > r_ptr->level) &&
					    m_ptr->mspeed > 60)
					{
#ifdef JP
						msg_format("%^s�̑��x���x���Ȃ����B", m_name);
#else
						msg_format("%^s starts limping slower.", m_name);
#endif

						m_ptr->mspeed -= 10;
					}
				}
				if (stun_effect && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
					if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
					{
						if (set_monster_stunned(c_ptr->m_idx, stun_effect + MON_STUNNED(m_ptr)))
						{
#ifdef JP
							msg_format("%^s�̓t���t���ɂȂ����B", m_name);
#else
							msg_format("%^s is stunned.", m_name);
#endif
						}
						else
						{
#ifdef JP
							msg_format("%^s�͂���Ƀt���t���ɂȂ����B", m_name);
#else
							msg_format("%^s is more stunned.", m_name);
#endif
						}
					}
				}
#endif

			//�i���U���̃_�C�X�E�X���C�E�N���e�B�J���ȂǏI��
			}

			/* Handle normal weapon */
			/*:::�ʏ핐��ɂ��U��*/
			else if (o_ptr->k_idx)
			{
				int wgt = o_ptr->weight;

				//����ւ̒ǉ��_�C�X����
				if(o_ptr->name1 == ART_EIKI)
					k = damroll(r_ptr->level / 10 + o_ptr->dd + p_ptr->to_dd[hand], o_ptr->ds + p_ptr->to_ds[hand]);
				else 
					k = damroll(o_ptr->dd + p_ptr->to_dd[hand], o_ptr->ds + p_ptr->to_ds[hand]);

				//tot_dam_aux()�ŗ��͂����������ǂ����m�F���邽�߂̃t���O
				gflag_force_effect = FALSE;

				/*:::�_�C�X�ɃX���C�ȂǓK�p*/
				k = tot_dam_aux(o_ptr, k, m_ptr, mode, FALSE);

				if (gflag_force_effect && critical_rank < 1) critical_rank = 1;
				gflag_force_effect = FALSE;

				if (backstab)
				{
					k *= (3 + (p_ptr->lev / 20));
				}
				else if (fuiuchi)
				{
						k = k*(5+(p_ptr->lev*2/25))/2;
				}
				else if (stab_fleeing)
				{
					k = (3 * k) / 2;
				}

				//if ((p_ptr->impact[hand] && ((k > 50) || one_in_(7))) || (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				if ((p_ptr->impact[hand] && (randint1(1000)<wgt || one_in_(7))) || (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				{
					do_quake = TRUE;
					if (critical_rank < 2) critical_rank = 2;
				}
				//�L�X����яオ��̏d�ʉ��Z
				if(p_ptr->pclass == CLASS_KISUME && p_ptr->concent)
					wgt += p_ptr->concent * (wgt + 100);

				/*:::�N���e�B�J������*/
			//	if ((!(o_ptr->tval == TV_SWORD) || !(o_ptr->sval == SV_DOKUBARI)) && !(mode == HISSATSU_KYUSHO))
				if ((!(o_ptr->tval == TV_KNIFE) || !(o_ptr->sval == SV_WEAPON_DOKUBARI)) && !(mode == HISSATSU_KYUSHO))
					k = critical_norm(wgt * 3 / 2 + p_ptr->lev * 2, o_ptr->to_h * 2, k, p_ptr->to_h[hand], mode,&critical_rank);

				drain_result = k;

				//v1.1.46 �؂ꖡ�������̕����玝���Ă��ē��e�����ڂ����B�a�S���������Ǝ�p�؂ꖡ�����͏���łł͎g��Ȃ��̂ō폜
				if (have_flag(flgs, TR_EX_VORPAL))
				{
					vorpal_chance = 2;
				}
				else if (have_flag(flgs, TR_VORPAL))
				{
					if ((p_ptr->special_attack & ATTACK_VORPAL) && object_has_a_blade(o_ptr))
						vorpal_chance = 2;
					else if (p_ptr->special_attack & ATTACK_UNSUMMON)
						vorpal_chance = 3;
					else
						vorpal_chance = 4;
				}
				else
				{
					if ((p_ptr->special_attack & ATTACK_VORPAL) && object_has_a_blade(o_ptr))
						vorpal_chance = 4;
					else if (p_ptr->special_attack & ATTACK_UNSUMMON)
						vorpal_chance = 4;
					else
						vorpal_chance = 0;
				}

				if(vorpal_chance && (randint1(vorpal_chance * 3 / 2) == 1))
					vorpal_cut = TRUE;
				else 
					vorpal_cut = FALSE;

				if (weapon_invalid) vorpal_cut = FALSE;

				/*:::���H�[�p���q�b�g����*/
				if (vorpal_cut)
				{
					int mult = 2;

					///del msg �`�F�[���\�[
#if 0
					///sydel item �`�F�[���\�[�̃��b�Z�[�W�����@�����Ƃ����@�ς��ɃX�g�u���̃��b�Z�[�W���O�ɓ���悤��
					if ((o_ptr->name1 == ART_CHAINSWORD) && !one_in_(2))
					{
						char chainsword_noise[1024];
#ifdef JP
						if (!get_rnd_line("chainswd_j.txt", 0, chainsword_noise))
#else
						if (!get_rnd_line("chainswd.txt", 0, chainsword_noise))
#endif
						{
							msg_print(chainsword_noise);
						}
					}
#endif
					if (o_ptr->name1 == ART_VORPAL_BLADE)
					{
#ifdef JP
						msg_print("�ڂɂ��~�܂�ʃ��H�[�p���u���[�h�A��B�̑��ƁI");
#else
						msg_print("Your Vorpal Blade goes snicker-snack!");
#endif
					}
					else
					{
#ifdef JP
						msg_format("�n��%s���s���؂�􂢂��I", m_name);
#else
						msg_format("Your weapon cuts deep into %s!", m_name);
#endif
					}

					/* Try to increase the damage */
					while (one_in_(vorpal_chance))
					{
						mult++;
					}

					k *= mult;

					/* Ouch! */
					/*:::�؂ꖡ���b�Z�[�W�@�ύX���悤*/
					if (((r_ptr->flagsr & RFR_RES_ALL) ? k/100 : k) > m_ptr->hp)
					{
#ifdef JP
				//		msg_format("%s��^����ɂ����I", m_name);
#else
						msg_format("You cut %s in half!", m_name);
#endif
					}
					else
					{
						switch (mult)
						{
						case 2: //msg_format("%s���a�����I", m_name); break;
						case 3: msg_format("%s���Ԃ����a�����I", m_name); break;
						case 4: //msg_format("%s�����b�^�a��ɂ����I", m_name); break;
						case 5: msg_format("%s��ő��a��ɂ����I", m_name); break;
						case 6: //msg_format("%s���h�g�ɂ����I", m_name); break;
						case 7: default:msg_format("%s���a���Ďa���Ďa��܂������I", m_name); break;
						//default: msg_format("%s���א؂�ɂ����I", m_name); break;

						}
					}
					drain_result = drain_result * 3 / 2;

					//v1.1.94
					if (critical_rank < mult-1) critical_rank = mult-1;
					if (critical_rank > 5) critical_rank = 5;

				}

				k += o_ptr->to_d;
				drain_result += o_ptr->to_d;
			/*:::����U�����������܂ŁB��������i��/���틤��*/
			}

			//v1.1.94
			if ((p_ptr->special_attack & ATTACK_CHAOS) && !(r_ptr->flagsr & RFR_RES_CHAO))
			{
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 10 + randint0(10);
			}


			//�p���X�B�_�C�X�u�[�X�g
			if(p_ptr->pclass == CLASS_PARSEE && r_ptr->level > p_ptr->lev)
			{
				int k_old = k;

				if(r_ptr->flags1 & RF1_FEMALE) k *= 3;
				else k = k * 3 / 2;

				if(cheat_xtra) msg_format("�p���X�B�̃_�C�X�u�[�X�g:%d��%d",k_old, k);

				if (one_in_(2)) critical_rank++;
				if (critical_rank > 5) critical_rank = 5;

			}

			/* Apply the player damage bonuses */
			k += p_ptr->to_d[hand];
			drain_result += p_ptr->to_d[hand];

			///class ���p�Ƃ̕K�E������
			if ((mode == HISSATSU_SUTEMI) || (mode == HISSATSU_3DAN)) k *= 3; ///mod140914 2��3
			if ((mode == HISSATSU_SEKIRYUKA) && !monster_living(r_ptr)) k = 0;
			if ((mode == HISSATSU_SEKIRYUKA) && !p_ptr->cut) k /= 2;

			/* No negative damage */
			if (k < 0) k = 0;

			if ((mode == HISSATSU_ZANMA) && monster_living(r_ptr))
			{
				k = 0;
			}

			///item �a�S��
			if (zantetsu_mukou)
			{
#ifdef JP
				msg_print("����ȓ�炩�����̂͐؂��I");
#else
				msg_print("You cannot cut such a elastic thing!");
#endif
				k = 0;
				critical_rank = 0;//paranoia
			}

			if (e_j_mukou)
			{
#ifdef JP
				msg_print("�w偂͋�肾�I");
#else
				msg_print("Spiders are difficult for you to deal with!");
#endif
				k /= 2;
			}

			if (mode == HISSATSU_MINEUCHI )
			{
				int tmp = (10 + randint1(15) + p_ptr->lev / 5);

				if(mode == HISSATSU_MINEUCHI) k = 0;
				anger_monster(m_ptr);

				if (!(r_ptr->flags3 & (RF3_NO_STUN)))
				{
					/* Get stunned */
					if (MON_STUNNED(m_ptr))
					{
#ifdef JP
						msg_format("%s�͂Ђǂ������낤�Ƃ����B", m_name);
#else
						msg_format("%s is more dazed.", m_name);
#endif

						tmp /= 2;
					}
					else
					{
#ifdef JP
						msg_format("%s �͂����낤�Ƃ����B", m_name);
#else
						msg_format("%s is dazed.", m_name);
#endif
					}

					/* Apply stun */
					(void)set_monster_stunned(c_ptr->m_idx, MON_STUNNED(m_ptr) + tmp);
				}
				else
				{
#ifdef JP
					msg_format("%s �ɂ͌��ʂ��Ȃ������B", m_name);
#else
					msg_format("%s is not effected.", m_name);
#endif
				}
			}

			/* Modify the damage */
			/*:::�������Ƃ͂��ꃁ�^���̏���*/
			///item ���̑劙 class ����m�@��������������
			///del131223 ���̑劙�폜
			//k = mon_damage_mod(m_ptr, k, (bool)(((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE)) || ((p_ptr->pclass == CLASS_BERSERKER) && one_in_(2))));
			k = mon_damage_mod(m_ptr, k, (bool)((p_ptr->pseikaku == SEIKAKU_BERSERK) && one_in_(2)));

			///item �Őj
			///mod131223 tval
			if ((!weapon_invalid && (o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
			{
				if ((randint1(randint1(r_ptr->level / 7) + 5) == 1) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2))
				{
					k = m_ptr->hp + 1;
#ifdef JP
					msg_format("%s�̋}����˂��h�����I", m_name);
#else
					msg_format("You hit %s on a fatal spot!", m_name);
#endif
					set_deity_bias(DBIAS_REPUTATION, -3);

				}
				else k = 1;
			}
			//v1.1.94 ��������͂��ꃁ�^������̂Ƃ������̃_���[�W�������ʂ͔������Ȃ��悤�ɂ���
			else if (k > 1)
			{
				/*:::�E�҃q�b�g*/
				///class �E�҃q�b�g
				///mod140511 �������������悤�Ȃ��Ƃ���
				if ((p_ptr->pclass == CLASS_KOISHI && fuiuchi) ||(flag_katana_critical) || 
					(p_ptr->pclass == CLASS_NINJA) && (o_ptr->tval == TV_KNIFE || monk_attack) && ((p_ptr->cur_lite <= 0) || one_in_(7)))
				{
					int maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
					//v1.1.94 �����o�₷��
					//if (one_in_(backstab ? 13 : (stab_fleeing || fuiuchi) ? 15 : 27))
					if (one_in_(backstab ? 9 : (stab_fleeing || fuiuchi) ? 12 : 24))
						{
						k *= 5;
						drain_result *= 2;
#ifdef JP
						if (p_ptr->pclass == CLASS_KOISHI)
							msg_format("%s�͑S�����h���Ɉꌂ���󂯂��I", m_name);
						else if (flag_katana_critical)
							msg_print("�K�E�̈ꑾ�����I");
						else if (o_ptr->k_idx)
							msg_format("�n��%s�ɐ[�X�Ɠ˂��h�������I", m_name);
						else
							msg_format("�s���ꌂ��%s�ɓ˂��h�������I", m_name);
#else
						msg_format("You critically injured %s!", m_name);
#endif
					}
					//else if (((m_ptr->hp < maxhp / 2) && one_in_((p_ptr->num_blow[0] + p_ptr->num_blow[1] + 1) * 10)) || ((one_in_(666) || ((backstab || fuiuchi) && one_in_(11))) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2)))
					else if (((m_ptr->hp < maxhp / 2) && one_in_((p_ptr->num_blow[0] + p_ptr->num_blow[1] + 1) * 6)) || ((one_in_(66) || ((backstab || fuiuchi) && one_in_(11))) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2)))
					{
						if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (m_ptr->hp >= maxhp / 2) && !one_in_(4))
						{
							k = MAX(k * 5, m_ptr->hp / 2);
							drain_result *= 2;
#ifdef JP
							if (p_ptr->pclass == CLASS_KOISHI)
								msg_format("%s�͑S�����h���Ɉꌂ���󂯂��I", m_name);
							else if (flag_katana_critical)
								msg_print("�K�E�̈ꑾ�����I");
							else if (o_ptr->k_idx)
								msg_format("�n��%s�̋}�����т����I", m_name);
							else
								msg_format("%s�̋}���֋���Ȉꌂ���������I", m_name);
#else
							msg_format("You fatally injured %s!", m_name);
#endif
						}
						else
						{
							k = m_ptr->hp + 1;
#ifdef JP
							if (p_ptr->pclass == CLASS_KOISHI)
								msg_format("%s�̈ӎ��͓r�؂ꂽ�B", m_name);
							else if (flag_katana_critical)
								msg_format("%s���꓁�̂��ƂɎa�蕚�����I", m_name);
							else if (o_ptr->k_idx)
								msg_format("�n��%s�̋}�����т����I", m_name);
							else
								msg_format("%s�����|�������B", m_name);
#else
							msg_format("You hit %s on a fatal spot!", m_name);
#endif
						}
					}
				}
				//���ѓ���U���@���j�[�N�A�����A���ϐ��̂Ƃ����̃t���O�͗����Ȃ�
				else if (sinker)
				{
					int resist = 100 + r_ptr->level * 6;
					if (r_ptr->flags2 & RF2_POWERFUL) resist = resist * 3 / 2;
					if (r_ptr->flags2 & RF2_GIGANTIC) resist *= 2;
					if (r_ptr->flags7 & RF7_CAN_FLY) resist *= 2;

					if (randint1(resist) < p_ptr->lev)
					{
						k = m_ptr->hp + 1;
						msg_format("%s��M�ꂳ�����B", m_name);

					}
				}

				//v1.1.21 ���m(�X�j�[�L���O���萬��)
				else if (sneakingkill)
				{
					k = m_ptr->hp + 1;
					msg_format("%s�𖳗͉������B", m_name);

				}
				//v1.1.94 �U����̞N�O�t�^�Ȃǂ������ōs��
				else
				{
					if (skill_type == TV_HAMMER) attack_effect_type = ATTACK_EFFECT_STUN;
					if (skill_type == TV_AXE) attack_effect_type = ATTACK_EFFECT_DEC_DEF;
					if (skill_type == TV_POLEARM) attack_effect_type = ATTACK_EFFECT_DELAY;

					apply_special_effect(c_ptr->m_idx, attack_effect_type, critical_rank, ref_skill_exp(skill_type), effect_turns,TRUE);
				}
			}


			if( k > 9999) k = 9999;

			/* Complex message */
			if (p_ptr->wizard || cheat_xtra)
			{
#ifdef JP
				msg_format("%d/%d �̃_���[�W��^�����B", k, m_ptr->hp);
#else
				msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
#endif
			}

			if (k <= 0)
			{
				can_drain = FALSE;
				can_feed = FALSE;
			}

			if (drain_result > m_ptr->hp)
				drain_result = m_ptr->hp;

			/* Damage, check for fear and death */
			if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
			{
				*mdeath = TRUE;
				///class �o�[�T�[�J�[�@�|�����Ƃ��U���񐔂��c���Ă������G�l���M�[������H
				///sys �A���X�̘A���U���Ƃ����̏������p�ŗǂ���ł͂Ȃ��낤��
				/*
				if ((p_ptr->pclass == CLASS_BERSERKER) && energy_use)
				{
					if (p_ptr->migite && p_ptr->hidarite)
					{
						if (hand) energy_use = energy_use*3/5+energy_use*num*2/(p_ptr->num_blow[hand]*5);
						else energy_use = energy_use*num*3/(p_ptr->num_blow[hand]*5);
					}
					else
					{
						energy_use = energy_use*num/p_ptr->num_blow[hand];
					}
				}
				*/
				///item �a�S��
				if ((o_ptr->name1 == ART_ZANTETSU) && is_lowlevel && !weapon_invalid)
#ifdef JP
					msg_print("�܂��܂�ʂ��̂��a���Ă��܂����D�D�D");
#else
					msg_print("Sigh... Another trifling thing I've cut....");
#endif

				///mod140714 �t�r�_�g���̗d�퉻�����@�G��|�������͗d�퉻���₷��
				//v1.1.29 ���ۂ̒ǉ��Ɓ��̗d�퉻�������Ă��̂Œǉ�
				if((p_ptr->pclass == CLASS_TSUKUMO_MASTER || p_ptr->pclass == CLASS_RAIKO) && 
					!object_is_fixed_artifact(o_ptr) && o_ptr->xtra1 < 100)
				{
					if(randint0(o_ptr->xtra1) < (r_ptr->level-o_ptr->xtra1 / 4) || (r_ptr->flags1 & RF1_UNIQUE))
					{
						o_ptr->xtra1++; 
						if((r_ptr->flags1 & RF1_UNIQUE) && (o_ptr->xtra1 % 50)) o_ptr->xtra1++; //���j�[�N��|��������2�|�C���g�{�[�i�X
						if(o_ptr->xtra1 == 50 || o_ptr->xtra1 == 100) make_evilweapon(o_ptr,r_ptr); 
					}
				}

				/*:::�����X�^�[��|�����Ƃ����z���������s����悤�ɂ��邽�߁A����break���z���̌��Ɉړ�*/
				//break;
			}

			/* Are we draining it?  A little note: If the monster is
			dead, the drain does not work... */
			/*:::�z������ �����X�^�[��|�������ɂ������悤�ɂ���*/
			if (can_drain && (drain_result > 0))
			{
				///item �����̓��ꋭ������
				if (o_ptr->name1 == ART_MURAMASA)
				{
					if (is_human)
					{
						int to_h = o_ptr->to_h;
						int to_d = o_ptr->to_d;
						int i, flag;

						flag = 1;
						for (i = 0; i < to_h + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_h++;

						flag = 1;
						for (i = 0; i < to_d + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_d++;

						if (o_ptr->to_h != to_h || o_ptr->to_d != to_d)
						{
#ifdef JP
							msg_print("�d���͌����z���ċ����Ȃ����I");
#else
							msg_print("Muramasa sucked blood, and became more powerful!");
#endif
							o_ptr->to_h = to_h;
							o_ptr->to_d = to_d;
						}
					}
				}
				else
				{
					if (drain_result > 5) /* Did we really hurt it? */
					{
						///mod140412 �G�Ɏ~�߂��h��������HP���啝�ɉ񕜂��邱�Ƃɂ��Ă݂�
						if(*mdeath) drain_heal = drain_result;
						else drain_heal = damroll(2, drain_result / 6);

						if(p_ptr->pclass == CLASS_YOSHIKA) drain_heal *= 2;
						/* Hex */
						if (hex_spelling(HEX_VAMP_BLADE)) drain_heal *= 2;

						if (cheat_xtra)
						{
#ifdef JP
							msg_format("Draining left: %d", drain_left);
#else
							msg_format("Draining left: %d", drain_left);
#endif

						}

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}

							if (drain_msg)
							{
#ifdef JP
								msg_format("%s���琶���͂��z��������I", m_name);
#else
								msg_format("Your weapon drains life from %s!", m_name);
#endif
								if(one_in_(100)) set_deity_bias(DBIAS_COSMOS, -1);

								drain_msg = FALSE;
							}

							/*:::�ˑR�ψق������Ƌz���̉񕜗ʂ�����炵��*/
							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player(drain_heal);
							/* We get to keep some of it! */
						}
					}
				}
				m_ptr->maxhp -= (k+7)/8;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				if (m_ptr->maxhp < 1) m_ptr->maxhp = 1;
				weak = TRUE;
			}

			if (can_feed && (drain_result > 0))
			{
				if(p_ptr->food < PY_FOOD_MAX) set_food(MIN(PY_FOOD_MAX-1,p_ptr->food + drain_result*5));
			}

			can_drain = FALSE;
			can_feed = FALSE;
			drain_result = 0;

			if(*mdeath) break;

			/* Anger the monster */
			///mod140412 ���̓�s�͋z�������̑O�����������Ɏ����Ă���
			if (k > 0) anger_monster(m_ptr);

			//�I�[���_���[�W�@�j���ۓ���U���̓I�[�����󂯂Ȃ�
			//��֊i�����A�\�E���X�J���v�`���A�A�_�u���X�R�A���I�[�����󂯂Ȃ�
			//v1.1.14 �_���l�̌���
			if(mode != HISSATSU_SHINMYOU && 
				mode != HISSATSU_NO_AURA && 
				mode != HISSATSU_GION && 
				mode != HISSATSU_SOULSCULPTURE && 
				mode != HISSATSU_DOUBLESCORE && 
				!(p_ptr->pclass == CLASS_ICHIRIN && p_ptr->do_martialarts ))
				touch_zap_player(m_ptr);


			/* Confusion attack */
			/*:::�����̎菈��*/
			///sys �ʂ��̍����̎菈���Ƃ����̕ӂɏ悹��΂�����
			if ((p_ptr->special_attack & ATTACK_CONFUSE) 
				|| (p_ptr->special_attack & ATTACK_CHAOS) && !(r_ptr->flagsr & RFR_RES_CHAO) && p_ptr->lev > randint1(r_ptr->level * 4) 
				|| (chaos_effect == 3) || (mode == HISSATSU_CONF) || hex_spelling(HEX_CONFUSION))
			{
				bool flg= FALSE;
				/* Cancel glowing hands */
				if (p_ptr->special_attack & ATTACK_CONFUSE)
				{
					flg = TRUE;
					p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
					msg_print("��̋P�����Ȃ��Ȃ����B");
#else
					msg_print("Your hands stop glowing.");
#endif
					p_ptr->redraw |= (PR_STATUS);

				}

				/* Confuse the monster */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;

#ifdef JP
					if(flg) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else if (randint0(100) < r_ptr->level)
				{
#ifdef JP
					if(flg) msg_format("%^s�ɂ͌��ʂ��Ȃ������B", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s�͍��������悤���B", m_name);
#else
					msg_format("%^s appears confused.", m_name);
#endif

					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
			}
//v1.1.20 �c�Ȗ��@���̌��ʕύX
#if 0
			///mod140725 �c�Ȗ��@���̖��E�����@�ʏ�̖��E�ƈႤ����ɂ��邽��player_cast��FALSE�œn���B
			if(p_ptr->special_attack & ATTACK_UNSUMMON)
			{
				if((r_ptr->flagsr & ( RFR_RES_TIME)) && !one_in_(6)) ;
				else if((r_ptr->flagsr & (RFR_RES_TELE)) && !one_in_(3)) ;
				else if(randint1(r_ptr->level * 3 + 100) > p_ptr->lev); 
				else if(genocide_aux(c_ptr->m_idx,0, FALSE, 0,"(���̃��b�Z�[�W�͏o�Ȃ��͂�)"))
				{
						msg_format("%^s�͂��̊K����������B", m_name);
						num = num_blow + 1; /* Can't hit it anymore! */
						*mdeath = TRUE;
				}

			}

			else 
#endif				
			if (chaos_effect == 4)
			{
				bool resists_tele = FALSE;

				if (r_ptr->flagsr & RFR_RES_TELE)
				{
					if (r_ptr->flags1 & RF1_UNIQUE)
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
						msg_format("%^s�ɂ͌��ʂ��Ȃ������B", m_name);
#else
						msg_format("%^s is unaffected!", m_name);
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
						msg_format("%^s�͒�R�͂������Ă���I", m_name);
#else
						msg_format("%^s resists!", m_name);
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
#ifdef JP
					msg_format("%^s�͏������I", m_name);
#else
					msg_format("%^s disappears!", m_name);
#endif

					teleport_away(c_ptr->m_idx, 50, TELEPORT_PASSIVE);
					num = num_blow + 1; /* Can't hit it anymore! */
					*mdeath = TRUE;
				}
			}

			else if ((chaos_effect == 5) && (randint1(90) > r_ptr->level))
			{
				if (!(r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) &&
				    !(r_ptr->flagsr & RFR_EFF_RES_CHAO_MASK))
				{
					if (polymorph_monster(y, x))
					{
#ifdef JP
						msg_format("%^s�͕ω������I", m_name);
#else
						msg_format("%^s changes!", m_name);
#endif

						*fear = FALSE;
						weak = FALSE;
					}
					else
					{
#ifdef JP
						msg_format("%^s�ɂ͌��ʂ��Ȃ������B", m_name);
#else
						msg_format("%^s is unaffected.", m_name);
#endif
					}

					/* Hack -- Get new monster */
					m_ptr = &m_list[c_ptr->m_idx];

					/* Oops, we need a different name... */
					monster_desc(m_name, m_ptr, 0);

					/* Hack -- Get new race */
					r_ptr = &r_info[m_ptr->r_idx];
				}
			}
			///sys item �S�[���f���n���}�[�@�g���W���[�n���^�[�Ƃ��œ������Ƃ��ł��Ă���������
			/*:::�S�[���f���n���}�[�ŉ������Ƃ��A�C�e����D������*/
			else if (o_ptr->name1 == ART_G_HAMMER && !weapon_invalid)
			{
				monster_type *tmp_m_ptr = &m_list[c_ptr->m_idx];

				if (tmp_m_ptr->hold_o_idx && (tmp_m_ptr->r_idx < MON_TSUKUMO_WEAPON1 || tmp_m_ptr->r_idx > MON_TSUKUMO_WEAPON5))
				{
					object_type *q_ptr = &o_list[tmp_m_ptr->hold_o_idx];
					char o_name[MAX_NLEN];

					object_desc(o_name, q_ptr, OD_NAME_ONLY);
					q_ptr->held_m_idx = 0;
					q_ptr->marked = OM_TOUCHED;
					tmp_m_ptr->hold_o_idx = q_ptr->next_o_idx;
					q_ptr->next_o_idx = 0;
#ifdef JP
					msg_format("%s��D�����B", o_name);
#else
					msg_format("You snatched %s.", o_name);
#endif
					inven_carry(q_ptr);
				}
			}

			if((mode == HISSATSU_EIGOU) && !dokubari) num--;
		}

		/* Player misses */
		else
		{
			backstab = FALSE; /* Clumsy! */
			fuiuchi = FALSE; /* Clumsy! */
			///del131223 ���̑劙�폜
#if 0
			/*:::���̑劙*/
			///sysdel item race ���̑劙�����s�����Ƃ��̏���
			if ((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE) && one_in_(3))
			{
				u32b flgs[TR_FLAG_SIZE];

				/* Sound */
				sound(SOUND_HIT);

				/* Message */
#ifdef JP
				msg_format("�~�X�I %s�ɂ��킳�ꂽ�B", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
				/* Message */
#ifdef JP
				msg_print("�U��񂵂��劙���������g�ɕԂ��Ă����I");
#else
				msg_print("Your scythe returns to you!");
#endif

				/* Extract the flags */
				object_flags(o_ptr, flgs);

				k = damroll(o_ptr->dd + p_ptr->to_dd[hand], o_ptr->ds + p_ptr->to_ds[hand]);
				{
					int mult;
					switch (p_ptr->mimic_form)
					{
					case MIMIC_NONE:
						switch (p_ptr->prace)
						{
						//	case RACE_YEEK:
						//	case RACE_KLACKON:
							case RACE_HUMAN:
							//case RACE_AMBERITE:
							case RACE_DUNADAN:
							//case RACE_BARBARIAN:
							case RACE_BEASTMAN:
								mult = 25;break;
						//	case RACE_HALF_ORC:
						//	case RACE_HALF_TROLL:
						//	case RACE_HALF_OGRE:
							//case RACE_HALF_GIANT:
							//case RACE_HALF_TITAN:
							//case RACE_CYCLOPS:
						//	case RACE_IMP:
							case RACE_SKELETON:
							case RACE_ZOMBIE:
							case RACE_VAMPIRE:
							case RACE_SPECTRE:
							//case RACE_DEMON:
						//	case RACE_DRACONIAN:
								mult = 30;break;
							default:
								mult = 10;break;
						}
						break;
					case MIMIC_DEMON:
					case MIMIC_DEMON_LORD:
					case MIMIC_VAMPIRE:
						mult = 30;break;
					default:
						mult = 10;break;
					}

					if (p_ptr->align < 0 && mult < 20)
						mult = 20;
					if (!(p_ptr->resist_acid || IS_OPPOSE_ACID() || p_ptr->immune_acid) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_elec || IS_OPPOSE_ELEC() || p_ptr->immune_elec) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_fire || IS_OPPOSE_FIRE() || p_ptr->immune_fire) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_cold || IS_OPPOSE_COLD() || p_ptr->immune_cold) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_pois || IS_OPPOSE_POIS()) && (mult < 25))
						mult = 25;

					if ((p_ptr->pclass != CLASS_SAMURAI) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (p_ptr->msp / 30)))
					{
						p_ptr->csp -= (1+(p_ptr->msp / 30));
						p_ptr->redraw |= (PR_MANA);
						mult = mult * 3 / 2 + 20;
					}
					k *= mult;
					k /= 10;
				}

				k = critical_norm(o_ptr->weight, o_ptr->to_h, k, p_ptr->to_h[hand], mode);
				if (one_in_(6))
				{
					int mult = 2;
#ifdef JP
					msg_format("�O�b�T���؂�􂩂ꂽ�I");
#else
					msg_format("Your weapon cuts deep into yourself!");
#endif
					/* Try to increase the damage */
					while (one_in_(4))
					{
						mult++;
					}

					k *= mult;
				}
				k += (p_ptr->to_d[hand] + o_ptr->to_d);

				if (k < 0) k = 0;

#ifdef JP
				take_hit(DAMAGE_FORCE, k, "���̑劙", -1);
#else
				take_hit(DAMAGE_FORCE, k, "Death scythe", -1);
#endif

				redraw_stuff();
			}
			else
#endif
			{
				/* Sound */
				sound(SOUND_MISS);

				/* Message */
#ifdef JP
				msg_format("�~�X�I %s�ɂ��킳�ꂽ�B", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
			}

			if(mode == HISSATSU_EIGOU) break;
		}
		backstab = FALSE;
		fuiuchi = FALSE;
		flag_katana_critical = FALSE;

	}	//���U���񐔃��[�v�I��


	//�h���~�[�i���U���Ŗ��点�鏈��
	if (counting_sheep && !(*mdeath))
	{
		int sleep_power = (p_ptr->lev+10) * counting_sheep;

		if (r_ptr->flagsr & RFR_RES_ALL) sleep_power = 0;
		if (r_ptr->flags1 & RF1_QUESTOR) sleep_power = 0;
		if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2)) sleep_power /= 2;
		if ((r_ptr->flags3 & RF3_NO_SLEEP)) sleep_power /= 3;

		if (randint1(sleep_power) > randint1(MAX(r_ptr->level,10)))
		{
			msg_format("%s�͖������B", m_name);
			set_monster_csleep(c_ptr->m_idx, randint1(3));
		}

	}

	//�K���J�^�������Ƃ��c�e����
	if(flag_gun_kata)
	{
		object_type *tmp_o_ptr;
		int i;
		int timeout_base,timeout_max;
		for(i=INVEN_RARM;i<=INVEN_LARM;i++)
		{
			tmp_o_ptr = &inventory[i];

			if(tmp_o_ptr->tval != TV_GUN)
				msg_print("ERROR:�e�𑕔����Ă��Ȃ��̂Ɏc�e�����������Ă΂�Ă���");
			else
			{
				timeout_base = calc_gun_timeout(tmp_o_ptr) * 1000;
				timeout_max = timeout_base * (calc_gun_load_num(tmp_o_ptr) - 1); 
				tmp_o_ptr->timeout += timeout_base;
				p_ptr->window |= PW_EQUIP;

				//�K���J�^�c�e����̂��ߒe�؂�`�F�b�N
				if(o_ptr->timeout > timeout_max)
					p_ptr->update |= PU_BONUS;
			}

		}
	}

	if (weak && !(*mdeath))
	{
#ifdef JP
		msg_format("%s�͎キ�Ȃ����悤���B", m_name);
#else
		msg_format("%^s seems weakened.", m_name);
#endif
	}
	//if (drain_left != MAX_VAMPIRIC_DRAIN)
	//{
		///sysdel virtue
	//	if (one_in_(4))
	//	{
	//		chg_virtue(V_UNLIFE, 1);
	//	}
	//}
	/* Mega-Hack -- apply earthquake brand */
	if (do_quake)
	{
		earthquake(py, px, 10);
		if (!cave[y][x].m_idx) *mdeath = TRUE;
	}
	///mod140828 �t�����̍U���̕���򉻏���
	if(p_ptr->pclass == CLASS_FLAN && o_ptr->k_idx)
	{
		int blast;
		object_flags(o_ptr, flgs);
		if(object_is_artifact(o_ptr) && have_flag(flgs,TR_RES_DISEN)) blast = 0;
		else if(object_is_artifact(o_ptr) || have_flag(flgs,TR_RES_DISEN)) blast = 1;
		else blast = 13;

		if(randint0(169) < blast)
		{
			msg_print("�j��̗͂ɕ��킪�ς����Ȃ��I");
			apply_disenchant(0,INVEN_RARM + hand);

		}

	}
	//�l���m�u�n���ڂꂷ��܂Ő؂荏�߁v�ɂ�镐���
	else if (mode == HISSATSU_HAKOBORE)
	{
		apply_disenchant(0, INVEN_RARM + hand);
	}


}




/*:::�ψٕ��ʂ⏂�Ȃǂɂ��ǉ��i���U���p���[�`���@*/
static void py_attack_aux2(int y, int x, bool *fear, bool *mdeath)
{
	int		num = 0, k, bonus_h, bonus_d , chance;

	cave_type       *c_ptr = &cave[y][x];

	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	object_type     *o_ptr;

	char            m_name[80];

//	bool            success_hit = FALSE;
	bool            monk_attack = FALSE;
	//bool            do_quake = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE;
	bool            can_feed = FALSE;
	int             num_blow;
	int             drain_left = MAX_VAMPIRIC_DRAIN;
	//u32b flgs[TR_FLAG_SIZE]; /* A massive hack -- life-draining weapons */
	//bool            is_human = (r_ptr->d_char == 'p');
	//bool            is_lowlevel = (r_ptr->level < (p_ptr->lev - 15));
	bool flag_gain_exp = FALSE;

	int skill_type;

	//int				martial_arts_method = 0;
	int i;

	int method[10];

	num_blow=0;
	for(i=0;i<10;i++) method[i]=0;


	/*:::�����ɗ���O��py_attack_aux()�Ŋi�������Ƃ�TRUE�ɂȂ�͂� */
	//if(!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM) && empty_hands(TRUE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM)) monk_attack = TRUE;
	if(p_ptr->do_martialarts) monk_attack = TRUE;

	/*:::�܂��U���\�Ȏ�i��method[]�Ɋi�[*/

	/*:::���n���x�ɉ����V�[���h�o�b�V�� �n���x20����o�n�߂�50��100%*/
	//v1.1.94 �񔭂܂ŏo��悤�ɂ��悤
	if(	(!check_invalidate_inventory(INVEN_RARM) &&inventory[INVEN_RARM].tval == TV_SHIELD || !check_invalidate_inventory(INVEN_LARM) &&inventory[INVEN_LARM].tval == TV_SHIELD))
	{
		if(ref_skill_exp(SKILL_SHIELD) > 3200 + randint0(4800))
			method[num_blow++] = MELEE_MODE_SHIELD;
		if (ref_skill_exp(SKILL_SHIELD) > 4800 + randint0(3200))
			method[num_blow++] = MELEE_MODE_SHIELD;

	}
	/*:::����U����������������ɉו��������Ă���Ƃ��A�̏p�n���x�ɉ����ǉ��Ŋi�� �i���n���x��20�ȏォ�Γ��ψُ�����*/
	///mod141116 �L�X���ɂ͕���U�����̒ǉ��i�����Ȃ�
	///mod141221 �E�҂�����
	if(!monk_attack && p_ptr->pclass != CLASS_KISUME && p_ptr->pclass != CLASS_NINJA)
	{
		if(p_ptr->muta2 & MUT2_HARDHEAD && randint0(100) < p_ptr->lev + 25)	method[num_blow++] = MELEE_MODE_HEADBUTT;
		else if(randint0(4800) < ref_skill_exp(SKILL_MARTIALARTS)-3200)
		{
			method[num_blow++] = find_martial_arts_method(MELEE_FIND_NOHAND);
		}
		//�_�𗼎莝�����Ă��Ċi���Z�\�������Ƃ��A����ɒǉ��i�����o��
		///mod150428 ��������C�����A�������d�����񐔏オ��ɂ�������
		//v1.1.94 �i���Z�\�����łȂ��_�Z�\�Ɗi���Z�\�̕��ςŔ��肷�邱�Ƃɂ���
		if(inventory[INVEN_RARM].k_idx && inventory[INVEN_RARM].tval == TV_STICK && p_ptr->ryoute)
		{
			int num_xtra = 0;
			int skill_exp;
			
			skill_exp = (ref_skill_exp(SKILL_MARTIALARTS) + ref_skill_exp(TV_STICK)) / 2;
			if(p_ptr->pclass == CLASS_MARTIAL_ARTIST) num_xtra++;
			if(randint1(skill_exp) > 3000) num_xtra++;
			if(randint1(skill_exp) > 4500) num_xtra++;
			if(randint1(skill_exp) > 6000) num_xtra++;
			if(heavy_armor()) num_xtra /= 2;
			for(;num_xtra>0;num_xtra--) method[num_blow++] = find_martial_arts_method(MELEE_FIND_NOHAND);
		}

	}
	/*:::�p�������Ă���Ƃ����x���ɉ����Ċp�U��*/
	if(p_ptr->muta2 & (MUT2_HORNS) && randint0(100) < p_ptr->lev + 20)	method[num_blow++] = MELEE_MODE_HORNS;	
	if(p_ptr->muta2 & (MUT2_BIGHORN) && randint0(100) < p_ptr->lev + 30)	method[num_blow++] = MELEE_MODE_BIGHORN;	
	/*:::�K���������Ă���Ƃ����x���ɉ����ĐK���U��*/
	if(p_ptr->muta2 & (MUT2_BIGTAIL) && randint0(100) < p_ptr->lev + 40)	method[num_blow++] = MELEE_MODE_BIGTAIL;	
	if(p_ptr->muta3 & (MUT3_FISH_TAIL) && randint0(100) < p_ptr->lev + 50)	method[num_blow++] = MELEE_MODE_FISH_TAIL;

	//�e�������̓���ǉ��i��
	if(!monk_attack && (inventory[INVEN_RARM].tval == TV_GUN || inventory[INVEN_LARM].tval == TV_GUN) 
		&& !check_invalidate_inventory(INVEN_RARM) && randint0(8000) < ref_skill_exp(TV_GUN))
	{
		method[num_blow++] = select_gun_melee_mode();
	}


	if(num_blow==0) return;


	//msg_format("BLOW:%d",num_blow);
	//for(i=0;i<10;i++) 
	//	msg_format("%d:%d",i,method[i]);

	/* Disturb the monster */
	(void)set_monster_csleep(c_ptr->m_idx, 0);

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/*:::�U���񐔂��Ƃɔ��� while����num�𒆂ň����̂ɕs�s���Ȃ̂�for�ɂ���*/
	//while ((num++ < num_blow) && !p_ptr->is_dead)
	for (num=0; num < num_blow && !p_ptr->is_dead; num++ )
	{
		bool sinker = FALSE;
		int mult = 100;
		int mon_ac = r_ptr->ac;

		//v1.1.94 
		int		attack_effect_type = ATTACK_EFFECT_NONE; //�N�O�ȂǓG�ɃX�e�[�^�X�ُ��t�^����Ƃ�
		int		critical_rank = 0; //�N���e�B�J����؂ꖡ��s�ӑł��Ȃǂ����܂����Ƃ�������
		int		effect_turns = 0; //�N�O�Ȃǂ̃X�e�[�^�X�ُ��^����^�[����

		//v1.1.94 �����X�^�[�h��͒ቺ����AC25%�J�b�g
		if (MON_DEC_DEF(m_ptr))
		{
			mon_ac = MONSTER_DECREASED_AC(mon_ac);
		}

		/*:::�o���l�𓾂�@���̏����ȊO�͑S�Ċi��*/
		if (method[num] == MELEE_MODE_SHIELD)
		{
			skill_type = SKILL_SHIELD;
		}
		else if(!flag_gain_exp)
		{
			skill_type = SKILL_MARTIALARTS;
			flag_gain_exp = TRUE; //���i���͈�x�����o�Ȃ�������ȊO�͉��x���o��̂ň�x�����o���l�𓾂��Ȃ��悤�ɂ��Ă���
		}
		gain_skill_exp(skill_type, m_ptr);

		/*:::�����i�Ȃǂ̏C�����Čv�Z����*/
		bonus_h = ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
		bonus_d = ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);

		{
			int tmp =  p_ptr->lev / 2 * ref_skill_exp(skill_type) / 8000;
			if(heavy_armor() && method[num] != MELEE_MODE_SHIELD) tmp /= 2;
			bonus_h += tmp;
			bonus_d += tmp/2;
		}
		if (p_ptr->stun > 50 || p_ptr->drowning){	bonus_h -= 20;	bonus_d -= 20;	}
		else if (p_ptr->stun){	bonus_h -= 5;	bonus_d -= 5;	}
		if (IS_HERO()) bonus_h += 12;
		if (p_ptr->shero) bonus_d += 3+(p_ptr->lev/5);

		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{
			o_ptr = &inventory[i];
			if (!o_ptr->k_idx) continue;
			if(check_invalidate_inventory(i)) continue;
			//����̎E�C�C���͖���
			if(object_is_melee_weapon(o_ptr)) continue;
			if(object_is_shooting_weapon(o_ptr)) continue;
			//���������Ă����������w�ւ̏C���͖����@(����ɏ��Ǝw�ւ𑕔����ď��U������Ƃ�����̎w�ւ̏C�����K�p����邪�܂��ǂ��ł��������낤)
			if(i == INVEN_RIGHT && !(method[num] == MELEE_MODE_SHIELD && inventory[INVEN_RARM].tval == TV_SHIELD)) continue;
			if(i == INVEN_LEFT && !(method[num] == MELEE_MODE_SHIELD && inventory[INVEN_LARM].tval == TV_SHIELD)) continue;
			//�����i�̏C����K�p�@1/2�ɂ��Ă���
			bonus_h += o_ptr->to_h/2;
			bonus_d += o_ptr->to_d/2;
		}
		chance = (p_ptr->skill_thn + (bonus_h * BTH_PLUS_ADJ));

		/* Test for hit */
		if ( test_hit_norm(chance, mon_ac, m_ptr->ml))
		{
			//int resist_stun = 0;
			//int stun_effect = 0;
			int special_effect = 0, times = 0, max_times;
			int min_level = 1;
			const martial_arts_new *ma_ptr = &ma_blows_new[0], *old_ptr = &ma_blows_new[0];
			int weight = 8;

			k = 1;
			/*
			if (r_ptr->flags1 & RF1_UNIQUE) resist_stun += 88;
			if (r_ptr->flags2 & RF2_GIGANTIC) resist_stun += 88;
			if (r_ptr->flags3 & RF3_NO_STUN) resist_stun += 66;
			if (r_ptr->flags3 & RF3_NO_CONF) resist_stun += 33;
			if (r_ptr->flags3 & RF3_NO_SLEEP) resist_stun += 33;
			if (r_ptr->flags2 & RF2_POWERFUL) resist_stun += 33;
			if ((r_ptr->flags3 & RF3_UNDEAD) || (r_ptr->flags3 & RF3_NONLIVING)) resist_stun += 66;
			*/

			max_times = ref_skill_exp(skill_type) / 2000 + 1;

			/*:::�w��U���G�t�F�N�g���琔��I�肵��ԍ����x���Ȃ̂�����*/
			for (times = 0; times < max_times; times++)
			{
				do
				{
					int mode_pick[20];
					int mode_cnt=0;
					for(i = 0;(ma_blows_new[i].method != 0 && mode_cnt < 20);i++)
					{
						if(ma_blows_new[i].method == method[num])
						{
							mode_pick[mode_cnt++] = i;
						}
					}
					if(!mode_cnt)
					{
						msg_format("ERROR:�ǉ��i�������ɂĎw�肳�ꂽ����(%d)�̑f��U��������o�^����Ă��Ȃ�",method[num]);
						return;
					}		
					ma_ptr = &ma_blows_new[ mode_pick[randint0(mode_cnt)]];
					min_level = ma_ptr->min_level;
				}
				while ((min_level > p_ptr->lev) || (randint1(p_ptr->lev) < ma_ptr->chance));

				if ((ma_ptr->min_level >= old_ptr->min_level) && !p_ptr->stun && !p_ptr->confused)	old_ptr = ma_ptr;
				else ma_ptr = old_ptr;
			}
			min_level = ma_ptr->min_level;
			weight = ma_ptr->weight;
			//�V�[���h�o�b�V���̃_���[�W��d�ʂ�F�X�׍H�@����ɏ��������Ă鎞�̌v�Z�������s���R�ɂȂ邪�ʓ|�Ȃ̂Ŗ���
			if(method[num] == MELEE_MODE_SHIELD && inventory[INVEN_RARM].tval == TV_SHIELD)
			{
				bonus_d += (inventory[INVEN_RARM].ac + inventory[INVEN_RARM].to_a)/2;
				weight =  inventory[INVEN_RARM].weight / 10;
			}
			else if(method[num] == MELEE_MODE_SHIELD && inventory[INVEN_LARM].tval == TV_SHIELD)
			{
				bonus_d += (inventory[INVEN_LARM].ac + inventory[INVEN_LARM].to_a)/2;
				weight =  inventory[INVEN_LARM].weight / 10;
			}

			k = damroll(ma_ptr->dd , ma_ptr->ds );

			switch(ma_ptr->effect)
			{
			case MELEE_STUN:
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 3 + randint0(3);
				break;
			case MELEE_STUN2:
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 5 + randint0(5);
				break;
			case MELEE_STUN3:
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 10 + randint0(10);
				break;

			//�얲�i���Ȃ�
			case MELEE_SLAY_EVIL:
				if(r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_KWAI))
				{
					if(mult < 250) mult = 250;
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 5 + randint0(5);

				}
				break;

			case MELEE_ELEC:
				if (!(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_ELEC)))
				{
					if (mult<200) mult = 200;
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 10 + randint0(5);

				}
				break;
			case MELEE_KASEN:
				attack_effect_type = ATTACK_EFFECT_STUN;
				effect_turns = 5 + randint0(5);
				if (r_ptr->flags3 & (RF3_UNDEAD) || r_ptr->flags3 & (RF3_ANIMAL))
				{
					if (mult < 300) mult = 300;
					effect_turns = 10 + randint0(10);
				}
				break;


			case MELEE_VAMP:
				if (monster_living(r_ptr))
				{
					can_drain = TRUE;
					can_feed = TRUE;
				}
				break;

			case MELEE_MELT:
				if(!(r_ptr->flagsr & (RFR_RES_ALL | RFR_IM_ACID)))
					if(mult<200) mult=200;
				if (monster_living(r_ptr))
				{
					can_drain = TRUE;
					can_feed = TRUE;
				}
				break;
			//���͂ȉΉ��U��
			case MELEE_ATAGO:
				if(r_ptr->flagsr & RFR_RES_ALL) k=0;
				else if(!(r_ptr->flagsr & RFR_IM_FIRE))
				{
					if(r_ptr->flagsr & RFR_HURT_FIRE && mult<500) mult = 500;
					else if(mult<300) mult=300;

					//v1.1.91 �Ζ��n�`�Ή��_���[�W
					if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) mult = oil_field_damage_mod(mult, y, x);

				}
				break;
			//�Ή��U��
			case MELEE_FIRE:
				if(r_ptr->flagsr & RFR_RES_ALL) k=0;
				else if(!(r_ptr->flagsr & RFR_IM_FIRE))
				{
					if(r_ptr->flagsr & RFR_HURT_FIRE && mult<300) mult = 300;
					else if(mult<200) mult=200;

					//v1.1.91 �Ζ��n�`�Ή��_���[�W
					if (cave_have_flag_bold(y, x, FF_OIL_FIELD)) mult = oil_field_damage_mod(mult, y, x);

				}
				break;
			//��C�A�X�U��
			case MELEE_COLD:
				if(r_ptr->flagsr & RFR_RES_ALL) k=0;
				else if(!(r_ptr->flagsr & RFR_IM_COLD))
				{
					if(r_ptr->flagsr & RFR_HURT_COLD && mult<300) mult = 300;
					else if(mult<200) mult=200;
				}
				break;
			case MELEE_SLOW:
				attack_effect_type = ATTACK_EFFECT_SLOW;
				effect_turns = 5 + randint0(5);

				break;
			case MELEE_POIS: //�őϐ��̂Ȃ��G�ɑ�_���[�W
				if(r_ptr->flagsr & RFR_RES_ALL) k=0;
				else if(!(r_ptr->flagsr & RFR_IM_POIS))
				{
					if(monster_living(r_ptr)) mult = MAX(300,mult);
					else mult = MAX(200,mult);
				}
				break;
			case MELEE_SINK: //���ѐ��v�U��
					if(monster_living(r_ptr) && !((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (r_ptr->flags1 & RF1_QUESTOR)))
						sinker = TRUE;
					//break���Ȃ�
			case MELEE_WATER:
				if ((r_ptr->flags7 & RF7_AQUATIC) || (r_ptr->flagsr & RFR_RES_WATE))
				{
					mult = 50;
				}
				else
				{
					if (r_ptr->flagsr & RFR_HURT_WATER) mult = 200;
					attack_effect_type = ATTACK_EFFECT_STUN;
					effect_turns = 10 + randint0(10);
				}
				break;
			case MELEE_CONFUSE: //�����t�^
				if(r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;
				}
				else if(randint0(100) > r_ptr->level)
				{
					msg_format("%^s�͍��������悤���B", m_name);
					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
				break;

			//v1.1.20�@���ӂ̔L�ԍU���͏d�ʂ��L�Ԃ̒��g�̏d�ʂɂȂ�
			case MELEE_NEKOGURUMA:
				{
					int inven2_num = calc_inven2_num();
					int nekoguruma_weight = 0;
				//	int i;

					for (i = 0; i < inven2_num; i++)
					{
						object_type *tmp_o_ptr = &inven_add[i];
						if(!tmp_o_ptr->k_idx) continue;

						nekoguruma_weight += tmp_o_ptr->weight * tmp_o_ptr->number;
					}

					// -hack- �d�ʒl�ύX�@���Ƃ�plev���|������Ԃ񌸂炵�Ƃ�
					weight = nekoguruma_weight / p_ptr->lev; 
					if(weight < 1) weight = 1;
					if(weight > 40) weight = 40;
					if(cheat_xtra) msg_format("weight:%d",weight);
					if(nekoguruma_weight > 200)
					{
						attack_effect_type = ATTACK_EFFECT_STUN;
						effect_turns = nekoguruma_weight / 100 + randint1(nekoguruma_weight / 100);

					}
					break;
				}

			case MELEE_DEC_ATK:
				attack_effect_type = ATTACK_EFFECT_DEC_ATK;
				break;
			case MELEE_DEC_DEF:
				attack_effect_type = ATTACK_EFFECT_DEC_DEF;
				break;
			case MELEE_DEC_MAG:
				attack_effect_type = ATTACK_EFFECT_DEC_MAG;
				break;
			case MELEE_DEC_ALL:
				attack_effect_type = ATTACK_EFFECT_DEC_ALL;
				break;
			case MELEE_DELAY:
				attack_effect_type = ATTACK_EFFECT_DELAY;
				break;
			}
			k = k * mult / 100;//�i���[���X���C
			/*:::�U���ɂ�郁�b�Z�[�W*/
			msg_format(ma_ptr->desc, m_name);
			k = critical_norm(p_ptr->lev * weight, min_level, k, bonus_h, 0, &critical_rank);
			k += bonus_d;
			drain_result = k;

			/*
			if (stun_effect && k < m_ptr->hp)
			{
				if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
				{
					if (set_monster_stunned(c_ptr->m_idx, stun_effect + MON_STUNNED(m_ptr)))
					{
#ifdef JP
						msg_format("%^s�̓t���t���ɂȂ����B", m_name);
#else
						msg_format("%^s is stunned.", m_name);
#endif
					}
					else
					{
#ifdef JP
						msg_format("%^s�͂���Ƀt���t���ɂȂ����B", m_name);
#else
						msg_format("%^s is more stunned.", m_name);
#endif
					}
				}
			}
			*/

			/* No negative damage */
			if (k < 0) k = 0;

			/* Modify the damage */
			/*:::�������Ƃ͂��ꃁ�^���̏���*/
			k = mon_damage_mod(m_ptr, k, (bool)(( p_ptr->pseikaku == SEIKAKU_BERSERK) && one_in_(2)));

			if (k > 1)
			{
				//���ѓ���U���@���j�[�N�A�����A���ϐ��̂Ƃ����̃t���O�͗����Ȃ�
				if (sinker)
				{
					int resist = 100 + r_ptr->level * 6;
					if (r_ptr->flags2 & RF2_POWERFUL) resist = resist * 3 / 2;
					if (r_ptr->flags2 & RF2_GIGANTIC) resist *= 2;
					if (r_ptr->flags7 & RF7_CAN_FLY) resist *= 2;

					if (randint1(resist) < p_ptr->lev)
					{
						k = m_ptr->hp + 1;
						msg_format("%s��M�ꂳ�����B", m_name);

					}
				}
				//v1.1.94 �U����̞N�O�t�^�Ȃǂ������ōs��
				else
				{
					apply_special_effect(c_ptr->m_idx, attack_effect_type, critical_rank, ref_skill_exp(skill_type), effect_turns,FALSE);
				}
			}

			/* Complex message */
			if (p_ptr->wizard || cheat_xtra)
			{
#ifdef JP
				msg_format("%d/%d �̃_���[�W��^�����B", k, m_ptr->hp);
#else
				msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
#endif
			}
			if (k <= 0)
			{
				can_drain = FALSE;
				can_feed = FALSE;
			}

			if (drain_result > m_ptr->hp)
				drain_result = m_ptr->hp;
			/* Damage, check for fear and death */
			if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
			{
				*mdeath = TRUE;
				break;
			}

			/*:::�z������*/
			if (can_drain && (drain_result > 0))
			{

					if (drain_result > 5) /* Did we really hurt it? */
					{
						///mod140412 �G�Ɏ~�߂��h��������HP���啝�ɉ񕜂��邱�Ƃɂ��Ă݂�
						if(*mdeath) drain_heal = drain_result;
						else drain_heal = damroll(2, drain_result / 6);

						if (cheat_xtra)
							msg_format("Draining left: %d", drain_left);

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}

							if (drain_msg)
							{
								msg_format("%s���琶���͂��z��������I", m_name);
								drain_msg = FALSE;
							}

							/*:::�ˑR�ψق������Ƌz���̉񕜗ʂ�����炵��*/
							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player(drain_heal);
							/* We get to keep some of it! */
						}
					}
				m_ptr->maxhp -= (k+7)/8;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				if (m_ptr->maxhp < 1) m_ptr->maxhp = 1;
				weak = TRUE;
			}

			if (can_feed && (drain_result > 0))
			{
				if(p_ptr->food < PY_FOOD_MAX) set_food(MIN(PY_FOOD_MAX-1,p_ptr->food + drain_result*5));
			}

			can_drain = FALSE;
			can_feed = FALSE;
			drain_result = 0;

			if(*mdeath) break;

			/* Anger the monster */
			if (k > 0) anger_monster(m_ptr);

			touch_zap_player(m_ptr);

			/* Confusion attack */
			/*:::�����̎菈��*/
			///sys �ʂ��̍����̎菈���Ƃ����̕ӂɏ悹��΂�����
#if 0
			if ((p_ptr->special_attack & ATTACK_CONFUSE) || (chaos_effect == 3) || (mode == HISSATSU_CONF) || hex_spelling(HEX_CONFUSION))
			{
				/* Cancel glowing hands */
				if (p_ptr->special_attack & ATTACK_CONFUSE)
				{
					p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
					msg_print("��̋P�����Ȃ��Ȃ����B");
#else
					msg_print("Your hands stop glowing.");
#endif
					p_ptr->redraw |= (PR_STATUS);

				}

				/* Confuse the monster */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;

#ifdef JP
					msg_format("%^s�ɂ͌��ʂ��Ȃ������B", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else if (randint0(100) < r_ptr->level)
				{
#ifdef JP
					msg_format("%^s�ɂ͌��ʂ��Ȃ������B", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s�͍��������悤���B", m_name);
#else
					msg_format("%^s appears confused.", m_name);
#endif

					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
			}
#endif

		}

		/* Player misses */
		else
		{
			sound(SOUND_MISS);

				/* Message */
#ifdef JP
				msg_format("�~�X�I %s�ɂ��킳�ꂽ�B", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
		}
	}


	if (weak && !(*mdeath))
	{
		msg_format("%s�͎キ�Ȃ����悤���B", m_name);
	}

}


/*:::�A���X��p�@�l�`�ɂ��U�����[�`��*/
static void py_attack_aux_alice(int y, int x, bool *fear, bool *mdeath, int mode)
{
	int		num = 0, k, chance, bonus;

	cave_type       *c_ptr = &cave[y][x];
	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	object_type     *o_ptr;

	char            m_name[80];

	bool            success_hit = FALSE;
	bool            vorpal_cut = FALSE;
	bool            fuiuchi = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE;
	int             drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[TR_FLAG_SIZE]; //�؂ꖡ
	bool            is_human = (r_ptr->d_char == 'p');

	//���n���x�{�[�i�X�́u�K�E�̈ꑾ���v���o���Ƃ��̃t���O
	bool			flag_katana_critical = FALSE;

	int total_blow_num = 0;
	int attack_doll_array[100];
	int i;
	int mon_ac = r_ptr->ac;


	if(p_ptr->pclass != CLASS_ALICE)
	{
		msg_print("ERROR:�N���X���A���X�ȊO�̎�alice_attack_aux()���Ă΂ꂽ"); 
		return;
	}

	//v1.1.94 �����X�^�[�h��͒ቺ����AC25%�J�b�g
	if (MON_DEC_DEF(m_ptr))
	{
		mon_ac = MONSTER_DECREASED_AC(mon_ac);
	}


	/*:::�l�`�̍U���񐔂����߁A���Ԃ������_���ɐݒ肷��*/
	for(i=0;i<100;i++) attack_doll_array[i]=0;
	for(i=0;i<INVEN_DOLL_NUM_MAX;i++)//�S�l�`���[�v
	{
		int j;
		int tmp_blow_num = calc_doll_blow_num(i);//�l�`��̂̍U���񐔌v�Z

		 //v1.1.94 ���ɂ��}��
		if (mode == HISSATSU_COUNTER_SPEAR)
		{
			if (inven_add[i].tval != TV_SPEAR) continue;
			tmp_blow_num = 1;
		}

		for(j=0;j<tmp_blow_num;j++)
		{

			if(!i) 
			{
				attack_doll_array[total_blow_num] = i;//��ԏ�̏�C�l�`�͕��ʂɊi�[
			}
			else//����ȊO�̎��m���œ���ւ���
			{
				if(one_in_(total_blow_num+1)) attack_doll_array[total_blow_num] = i;
				else
				{
					int move = randint0(total_blow_num);	
					attack_doll_array[total_blow_num] = attack_doll_array[move];
					attack_doll_array[move] = i;
				}
			}
			total_blow_num++;
			if(total_blow_num == 100) 
			{msg_print("ERROR:alice_attack_aux()�̍U���񐔂�100�ȏ�"); return; }
		}
	}
	if(!total_blow_num) 
	{msg_print("ERROR:alice_attack_aux()�ōU���񐔂�0"); return;}


//����̓��Z�̂Ƃ��͖����ɂ���H
	if(p_ptr->tim_unite_darkness)
	{
		if(!(cave[py][px].info & CAVE_GLOW)  && m_ptr->ml && (MON_CSLEEP(m_ptr) || randint1(p_ptr->lev * 3 + (p_ptr->skill_stl + 10) * 4) > r_ptr->level * 2)) fuiuchi = TRUE;
	}

	//���̈ꌂ�ڂɕK���ƃ{�[�i�X���悹��
	if(check_katana_critical(m_ptr, mode))
	{
		flag_katana_critical = TRUE;
	}

	(void)set_monster_csleep(c_ptr->m_idx, 0);
	monster_desc(m_name, m_ptr, 0);

	/* Attack once for each legal blow */
	/*:::�U���񐔂��Ƃɔ���*/
	while ((num < total_blow_num) && !p_ptr->is_dead)
	        {
		bool dokubari = FALSE;
		int doll_num = attack_doll_array[num];

		bool flag_katana_critical_done = FALSE;

		//v1.1.94
		int		skill_type = 0;	//�Y������X�L�� ref_skill_exp()�Ɏg��
		int		attack_effect_type = ATTACK_EFFECT_NONE; //�N�O�ȂǓG�ɃX�e�[�^�X�ُ��t�^����Ƃ�
		int		critical_rank = 0; //�N���e�B�J����؂ꖡ��s�ӑł��Ȃǂ����܂����Ƃ�������
		int		effect_turns = 0; //�N�O�Ȃǂ̃X�e�[�^�X�ُ��^����^�[����

		o_ptr =  &inven_add[doll_num];
		if(!object_is_melee_weapon(o_ptr))
		{msg_format("ERROR:����������Ȃ��l�`���Ă΂ꂽ(%d)",doll_num); return;}

		skill_type = o_ptr->tval;

		if (flag_katana_critical && o_ptr->tval == TV_KATANA) flag_katana_critical_done = TRUE;

		//������n���x��1/10���x�ɂ��Ă���
		if(one_in_(10)) gain_skill_exp(skill_type,m_ptr);

		//calc_bonuses()�Ōv�Z���Ă��Ȃ�����n���x�������Ŕ��f
		//v1.1.94 ���̖������ɏn���x�{�[�i�X���f
		if (o_ptr->tval == TV_SWORD)
		{
			bonus = p_ptr->to_h[0] + o_ptr->to_h + (ref_skill_exp(skill_type) - 2500) / 125;
		}
		else
		{
			bonus = p_ptr->to_h[0] + o_ptr->to_h + (ref_skill_exp(skill_type) - WEAPON_EXP_BEGINNER) / 200;
		}


		chance = (p_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

		if (mode == HISSATSU_COUNTER_SPEAR) chance += 60;


		///sys item �Őj��������
		///mod131223 tval
		if ((o_ptr->tval == TV_KNIFE) && (o_ptr->sval == SV_WEAPON_DOKUBARI))
		{
			dokubari = TRUE;
			success_hit = one_in_(3);
		}

		else if ((fuiuchi && !(r_ptr->flagsr & RFR_RES_ALL)))
		{
			success_hit = TRUE;
		}
		else if (flag_katana_critical_done)
		{
			success_hit = TRUE;
		}
		else 
		{
			/*:::�ʏ�̖�������*/
			success_hit = test_hit_norm(chance, mon_ac, m_ptr->ml);
			//�I�[���ɂ�閽��������
			if(r_ptr->flags2 & RF2_AURA_FIRE && one_in_(20)) success_hit = FALSE;
			if(r_ptr->flags2 & RF2_AURA_COLD && one_in_(20)) success_hit = FALSE;
			if(r_ptr->flags2 & RF2_AURA_ELEC && one_in_(20)) success_hit = FALSE;
		}

		/* Test for hit */
		if (success_hit)
		{
			int vorpal_chance;

			//v1.1.94
			if ( fuiuchi ) critical_rank = 3;

			/* Sound */
			sound(SOUND_HIT);

			if (fuiuchi)
				msg_format("%s�͕s�ӂ�˂���%s�ɋ���Ȉꌂ�����킹���I",alice_doll_name[doll_num], m_name);
			else if (flag_katana_critical_done)
				msg_format("%s��%s�ɂ��炭�蔲���p���I�����I", alice_doll_name[doll_num], m_name);
			else if(ex_attack_msg)
				msg_format("%s��%s%s",alice_doll_name[doll_num], m_name, msg_py_atk(o_ptr,mode));
			else 
				msg_format("%s%s", m_name, msg_py_atk(o_ptr,mode));

			k = 1;

			object_flags(o_ptr, flgs);
			//v1.1.46 �A���X��EXTRA���[�h�œu�΂��g�������̏�����ǉ�
			if (have_flag(flgs, TR_EX_VORPAL)) 
				vorpal_chance = 2;
			else if ((p_ptr->special_attack & ATTACK_VORPAL) && object_has_a_blade(o_ptr) && have_flag(flgs, TR_VORPAL))
				vorpal_chance = 2;
			else if ((p_ptr->special_attack & ATTACK_VORPAL) && object_has_a_blade(o_ptr) || have_flag(flgs, TR_VORPAL))
				vorpal_chance = 4;
			else
				vorpal_chance = 0;

			/* Vampiric drain */
			/*:::�z������*/
			//�����g�p������������������

			if ((p_ptr->special_attack & (ATTACK_DARK)) && p_ptr->lev > 44 || o_ptr->name1 == ART_MURAMASA)
			{
				if (monster_living(r_ptr))
					can_drain = TRUE;
				else
					can_drain = FALSE;
			}

			if (vorpal_chance && randint1(vorpal_chance*3/2) == 1 )
				vorpal_cut = TRUE;
			else 
				vorpal_cut = FALSE;

			/*:::����_�C�X(�ǉ��_�C�X�����j*/
			k = damroll(o_ptr->dd , o_ptr->ds );
			/*:::�_�C�X�ɃX���C�ȂǓK�p*/
			k = tot_dam_aux(o_ptr, k, m_ptr, mode, FALSE);

			if (fuiuchi || flag_katana_critical_done)
			{
				k = k*(5+(p_ptr->lev*2/25))/2;
			}
			/* �n�k�͋N����Ȃ�*/

			/*:::�N���e�B�J������*/ 
			if (!dokubari)
			k = critical_norm(o_ptr->weight * 3 / 2 + p_ptr->lev * 2, o_ptr->to_h*2, k, p_ptr->to_h[0], mode, &critical_rank);
			//v1.1.94 �d�ʃA�b�v
			//k = critical_norm(o_ptr->weight, o_ptr->to_h, k, p_ptr->to_h[0], mode, &critical_rank);



			drain_result = k;
			/*:::���H�[�p���q�b�g����*/
			if (vorpal_cut)
			{
				int mult = 2;
				if (o_ptr->name1 == ART_VORPAL_BLADE)
					msg_print("�ڂɂ��~�܂�ʃ��H�[�p���u���[�h�A��B�̑��ƁI");
				else
					msg_format("�n��%s���s���؂�􂢂��I", m_name);
				while (one_in_(vorpal_chance))
				{
					mult++;
				}

				k *= mult;

				switch (mult)
				{
					case 2: //msg_format("%s���a�����I", m_name); break;
					case 3: msg_format("%s���Ԃ����a�����I", m_name); break;
					case 4: //msg_format("%s�����b�^�a��ɂ����I", m_name); break;
					case 5: msg_format("%s��ő��a��ɂ����I", m_name); break;
					case 6: //msg_format("%s���h�g�ɂ����I", m_name); break;
					case 7: default:msg_format("%s���a���Ďa���Ďa��܂������I", m_name); break;
					//default: msg_format("%s���א؂�ɂ����I", m_name); break;
				}
				drain_result = drain_result * 3 / 2;

				//v1.1.94
				if (critical_rank < mult - 1) critical_rank = mult - 1;
				if (critical_rank > 5) critical_rank = 5;

			}

			k += o_ptr->to_d;
			drain_result += o_ptr->to_d;
			k += p_ptr->to_d[0];
			drain_result += p_ptr->to_d[0];

			if (flag_katana_critical_done && weird_luck())
			{
				msg_print("�K�E�̈ꑾ�����I");
				k *= 5;
			}

			/* No negative damage */
			if (k < 0) k = 0;

			/* Modify the damage */
			/*:::�������Ƃ͂��ꃁ�^���̏���*/
			k = mon_damage_mod(m_ptr, k, FALSE);

			///item �Őj
			if (dokubari)
			{
				if ((randint1(randint1(r_ptr->level/7)+5) == 1) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2))
				{
					k = m_ptr->hp + 1;
					msg_format("%s�̋}����˂��h�����I", m_name);
				}
				else k = 1;
			}
			//v1.1.94 �U����̞N�O�t�^�Ȃǂ������ōs��
			else
			{
				if (skill_type == TV_HAMMER) attack_effect_type = ATTACK_EFFECT_STUN;
				if (skill_type == TV_AXE) attack_effect_type = ATTACK_EFFECT_DEC_DEF;
				if (skill_type == TV_POLEARM) attack_effect_type = ATTACK_EFFECT_DELAY;

				apply_special_effect(c_ptr->m_idx, attack_effect_type, critical_rank, ref_skill_exp(skill_type), effect_turns,TRUE);
			}



			if( k > 9999) k = 9999;
			/* Complex message */
			if (p_ptr->wizard || cheat_xtra)
			{
				msg_format("%d/%d �̃_���[�W��^�����B", k, m_ptr->hp);
			}

			if (k <= 0)
			{
				can_drain = FALSE;
			}

			if (drain_result > m_ptr->hp)
				drain_result = m_ptr->hp;

			/* Damage, check for fear and death */
			if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
			{
				*mdeath = TRUE;

				//�G��|�����Ƃ��܂��l�`�̍U���񐔂��c���Ă���΂��̕��s���͂̏������������
				if (energy_use && p_ptr->lev > 24)
				{
					energy_use = 25 + 75 * (num+1) / total_blow_num;
				}
			}

			/*:::�z������ �����X�^�[��|�������ɂ������悤�ɂ���*/
			if (can_drain && (drain_result > 0))
			{
				///item �����̓��ꋭ������
				if (o_ptr->name1 == ART_MURAMASA)
				{
					if (is_human)
					{
						int to_h = o_ptr->to_h;
						int to_d = o_ptr->to_d;
						int flag;

						flag = 1;
						for (i = 0; i < to_h + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_h++;

						flag = 1;
						for (i = 0; i < to_d + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_d++;

						if (o_ptr->to_h != to_h || o_ptr->to_d != to_d)
						{
							msg_print("�d���͌����z���ċ����Ȃ����I");

							o_ptr->to_h = to_h;
							o_ptr->to_d = to_d;
						}
					}
				}
				else
				{
					if (drain_result > 5) /* Did we really hurt it? */
					{
						///mod140412 �G�Ɏ~�߂��h��������HP���啝�ɉ񕜂��邱�Ƃɂ��Ă݂�
						if(*mdeath) drain_heal = drain_result;
						else drain_heal = damroll(2, drain_result / 6);

						if (cheat_xtra)
						{
							msg_format("Draining left: %d", drain_left);
						}

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}
							if (drain_msg)
							{
								msg_format("%s���琶���͂��z��������I", m_name);
								drain_msg = FALSE;
							}

							/*:::�ˑR�ψق������Ƌz���̉񕜗ʂ�����炵��*/
							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player(drain_heal);
						}
					}
				}
				m_ptr->maxhp -= (k+7)/8;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				if (m_ptr->maxhp < 1) m_ptr->maxhp = 1;
				weak = TRUE;
			}

			can_drain = FALSE;
			drain_result = 0;

			if(*mdeath) break;

			/* Anger the monster */
			///mod140412 ���̓�s�͋z�������̑O�����������Ɏ����Ă���
			if (k > 0) anger_monster(m_ptr);
			//�A���X�̐l�`�U���̓I�[���_���[�W���󂯂Ȃ�
			//touch_zap_player(m_ptr);


			/* Confusion attack */
			/*:::�����̎菈��*/
			///sys �ʂ��̍����̎菈���Ƃ����̕ӂɏ悹��΂�����
			if (p_ptr->special_attack & ATTACK_CONFUSE )
			{
				p_ptr->special_attack &= ~(ATTACK_CONFUSE);
				msg_print("��̋P�����Ȃ��Ȃ����B");
				p_ptr->redraw |= (PR_STATUS);

				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;
					msg_format("%^s�ɂ͌��ʂ��Ȃ������B", m_name);

				}
				else if (randint0(100) < r_ptr->level)
				{
					msg_format("%^s�ɂ͌��ʂ��Ȃ������B", m_name);
				}
				else
				{
					msg_format("%^s�͍��������悤���B", m_name);
					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
			}

		}

		/* Player misses */
		else
		{
			fuiuchi = FALSE; /* Clumsy! */
			{
				/* Sound */
				sound(SOUND_MISS);
				if(ex_attack_msg)
					msg_format("%s�̍U����%s�ɂ��킳�ꂽ�B",alice_doll_name[doll_num], m_name);
				else
					msg_format("�~�X�I %s�ɂ��킳�ꂽ�B", m_name);
			}

		}
		fuiuchi = FALSE;
		if(flag_katana_critical_done) flag_katana_critical = FALSE;


		num++;
	}

	if (weak && !(*mdeath))
	{
		msg_format("%s�͎キ�Ȃ����悤���B", m_name);
	}

}




/*:::�w��O���b�h�֍U������Bx,y�ɂ̓����X�^�[�����邱�ƁB*/
bool py_attack(int y, int x, int mode)
{
	bool            fear = FALSE;
	bool            mdeath = FALSE;
	bool            stormbringer = FALSE;

	//�A���X�l�`�U���p�t���O�@�����@���A�ōU���s��
	bool			alice_doll_attack = (p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts);

	cave_type       *c_ptr = &cave[y][x];
	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];

	if(!c_ptr->m_idx)
	{
		msg_print("ERROR:py_attack()�Ŏw��O���b�h�Ƀ����X�^�[�����Ȃ�");
		return FALSE;
	}

	if(m_ptr->r_idx < 1 || m_ptr->r_idx >= max_r_idx)
	{
		msg_format("ERROR:py_attack()�Ŏw��O���b�h�̃����X�^�[��r_idx����������(%d)",m_ptr->r_idx);
		delete_monster_idx(c_ptr->m_idx);
		return FALSE;
	}

	/* Disturb the player */
	disturb(0, 1);

	//v1.1.14 �˕P�̋_���l�����Z�b�g����鏈���B�ʏ��process_player�̃��[�v��ɍs�������b�Z�[�W�Ɉ�a������̂ōŏ��ɂ���Ƃ�
	if(p_ptr->pclass == CLASS_YORIHIME && mode != HISSATSU_GION && (p_ptr->kamioroshi & KAMIOROSHI_GION)) 
		reset_concentration(TRUE);

	if(SAKUYA_WORLD)
	{
		msg_print("���͍U�����ł��Ȃ��B");
		return FALSE;
	}

	//����i���̏���s���͌�������
	if(p_ptr->pclass == CLASS_MEIRIN && p_ptr->do_martialarts)
	{
		energy_use = p_ptr->magic_num1[0];
		if(cheat_xtra) msg_format("Energy_use:%d",energy_use);
	}
	//���ۃv���X�e�B���r�[�g�̏���s���͌�������
	else if(p_ptr->pclass == CLASS_RAIKO && music_singing(MUSIC_NEW_RAIKO_PRISTINE))
	{
		energy_use = 75 - p_ptr->concent * 5;
		if(energy_use < 25) energy_use = 25;

		if(cheat_xtra) msg_format("Energy_use:%d",energy_use);
	}
	else if(mode == HISSATSU_GION || mode == HISSATSU_DOUBLESCORE || mode == HISSATSU_COUNTER_SPEAR)
	{
		; //�J�E���^�[�n�̍U���͍s������Ȃ�
	}
	else
		energy_use = 100;

	if(IS_METAMORPHOSIS && r_info[MON_EXTRA_FIELD].flags1 & RF1_NEVER_BLOW)
	{
		msg_print("���̐g�̂͒��ڍU���Ɍ����Ȃ��悤���B");
		return FALSE;
	}

	/*:::���肪����ȊO�ōǂ����Ă���U���p�ψق������ĂȂ��ꍇ�A�U���ł��Ȃ�*/
	///sys ���肪�ӂ������Ă�ƍU���ł��Ȃ������@�ǉ��i����1�`2�񂭂炢�o�Ă�������������Ȃ�
	if (!p_ptr->migite && !p_ptr->hidarite &&
	    !(p_ptr->muta2 & (MUT2_HORNS | MUT2_BIGHORN | MUT2_HARDHEAD | MUT2_BIGTAIL)))
	{
#ifdef JP
		msg_format("%s�U���ł��Ȃ��B", (empty_hands(FALSE) == EMPTY_HAND_NONE) ? "���肪�ӂ�������" : "");
#else
		msg_print("You cannot do attacking.");
#endif
		return FALSE;
	}

	//��\��o���̉��t���f(�_�u���X�R�A���t���̊i���U������)
	if((p_ptr->pclass == CLASS_BENBEN || p_ptr->pclass == CLASS_YATSUHASHI) && music_singing_any())
	{
		if(music_singing(MUSIC_NEW_TSUKUMO_DOUBLESCORE) && p_ptr->do_martialarts) mode = HISSATSU_DOUBLESCORE;
		else stop_tsukumo_music();
	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);
	/*:::�����X�^�[�������Ă�ꍇ�A�v���o��HP�o�[�̏��X�V�t���O�𗧂Ă�*/
	if (m_ptr->ml)
	{
		/* Auto-Recall if possible and visible */
		if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

		/* Track a new monster */
		health_track(c_ptr->m_idx);
	}

	///del131223 �a�S����������
#if 0
	if ((r_ptr->flags1 & RF1_FEMALE) &&
	    !(p_ptr->stun || p_ptr->confused || p_ptr->image || !m_ptr->ml))
	{
		if ((inventory[INVEN_RARM].name1 == ART_ZANTETSU) || (inventory[INVEN_LARM].name1 == ART_ZANTETSU))
		{
#ifdef JP
			msg_print("�َҁA���Ȃ��͎a��ʁI");
#else
			msg_print("I can not attack women!");
#endif
			return FALSE;
		}
	}
#endif
	if ((d_info[dungeon_type].flags1 & DF1_NO_MELEE) && !alice_doll_attack)
	{
#ifdef JP
		msg_print("�Ȃ����U�����邱�Ƃ��ł��Ȃ��B");
#else
		msg_print("Something prevent you from attacking.");
#endif
		return FALSE;
	}
	if(alice_doll_attack)
	{
		if (d_info[dungeon_type].flags1 & DF1_NO_MAGIC || p_ptr->anti_magic)
		{
			msg_print("�l�`�������Ȃ��I");
			return FALSE;
		}
		if(p_ptr->confused)
		{
			msg_print("�������Ă��Đl�`�𑀂�Ȃ��I");
			return FALSE;
		}

	}

	///sys class item �t�����h���[�A�^�b�N�@�X�g�u��
	/* Stop if friendly */
	if (!is_hostile(m_ptr) && !flag_friendly_attack && 
	    !(p_ptr->stun || p_ptr->confused || p_ptr->image ||
	    p_ptr->shero || !m_ptr->ml))
	{
		if (!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (!check_invalidate_inventory(INVEN_LARM) && inventory[INVEN_LARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (stormbringer)
		{
#ifdef JP
			msg_format("�����n�͋��~��%s���U�������I", m_name);
#else
			msg_format("Your black blade greedily attacks %s!", m_name);
#endif
			///del131223 virtue
			//chg_virtue(V_INDIVIDUALISM, 1);
			//chg_virtue(V_HONOUR, -1);
			//chg_virtue(V_JUSTICE, -1);
			//chg_virtue(V_COMPASSION, -1);
		}
		else if ( p_ptr->pseikaku != SEIKAKU_BERSERK && mode != HISSATSU_EXCLAW && mode != HISSATSU_YOSHIKA )
		{
#ifdef JP
			if (get_check("�{���ɍU�����܂����H"))
#else
			if (get_check("Really hit it? "))
#endif
			{
				///del131223 virtue
				//chg_virtue(V_INDIVIDUALISM, 1);
				//chg_virtue(V_HONOUR, -1);
				//chg_virtue(V_JUSTICE, -1);
				//chg_virtue(V_COMPASSION, -1);
			}
			else
			{
#ifdef JP
				msg_format("%s���U������̂��~�߂��B", m_name);
#else
				msg_format("You stop to avoid hitting %s.", m_name);
#endif
				return FALSE;
			}
		}
	}


	/* Handle player fear */
	if (p_ptr->afraid && mode != HISSATSU_EXCLAW && mode != HISSATSU_YOSHIKA && mode != HISSATSU_GION && !alice_doll_attack && mode != HISSATSU_DOUBLESCORE
		|| p_ptr->pclass == CLASS_KOMACHI && m_ptr->r_idx == MON_EIKI)
	{
		/* Message */
		if (m_ptr->ml)
#ifdef JP
			msg_format("������%s���U���ł��Ȃ��I", m_name);
#else
			msg_format("You are too afraid to attack %s!", m_name);
#endif

		else
#ifdef JP
			msg_print("�������ɂ͉����������̂�����I");
#else
			msg_format ("There is something scary in your way!");
#endif

		/* Disturb the monster */
		(void)set_monster_csleep(c_ptr->m_idx, 0);

		/* Done */
		return FALSE;
	}

	//�h�b�y���Q���K�[���ꏈ��
	if(r_ptr->flags7 & RF7_DOPPELGANGER)
	{
		msg_print("���Ȃ��͎����̃h�b�y���Q���K�[�ɐG��Ă��܂����I");
		take_hit(DAMAGE_LOSELIFE,p_ptr->chp,"�h�b�y���Q���K�[�Ƃ̐ڐG",-1);
		set_stun(100+randint0(3));
		delete_monster_idx(c_ptr->m_idx);
		return TRUE;
	}


	/*:::�G�̐Q���݂��P�����Ƃ��̓�����*/
	///sysdel virtue
#if 0
	if (MON_CSLEEP(m_ptr)) /* It is not honorable etc to attack helpless victims */
	{
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_COMPASSION, -1);
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_HONOUR, -1);
	}
#endif
	/*:::�񓁗��Z�\�o���l�𓾂�*/
	///mod151205 �O����������
	if (p_ptr->migite && p_ptr->hidarite && p_ptr->pclass != CLASS_3_FAIRIES)
	{
		///mod131226 skill �Z�\�ƕ���Z�\�̓���
		gain_skill_exp(SKILL_2WEAPONS,m_ptr);
		/*
		if ((p_ptr->skill_exp[GINOU_NITOURYU] < s_info[p_ptr->pclass].s_max[GINOU_NITOURYU]) && ((p_ptr->skill_exp[GINOU_NITOURYU] - 1000) / 200 < r_ptr->level))
		{
			if (p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_BEGINNER)
				p_ptr->skill_exp[GINOU_NITOURYU] += 80;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_SKILLED)
				p_ptr->skill_exp[GINOU_NITOURYU] += 4;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_EXPERT)
				p_ptr->skill_exp[GINOU_NITOURYU] += 1;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_MASTER)
				if (one_in_(3)) p_ptr->skill_exp[GINOU_NITOURYU] += 1;
			p_ptr->update |= (PU_BONUS);
		}
		*/
	}
	/*:::��n�Z�\�o���l�𓾂�*/
	///sys ��n�o���l����
	/* Gain riding experience */
	if (p_ptr->riding && !CLASS_RIDING_BACKDANCE)
	{
		///mod131226 skill �Z�\�ƕ���Z�\�̓���
		gain_skill_exp(SKILL_RIDING,m_ptr);
#if 0
		int cur = p_ptr->skill_exp[GINOU_RIDING];
		int max = s_info[p_ptr->pclass].s_max[GINOU_RIDING];

		if (cur < max)
		{
			int ridinglevel = r_info[m_list[p_ptr->riding].r_idx].level;
			int targetlevel = r_ptr->level;
			int inc = 0;

			if ((cur / 200 - 5) < targetlevel)
				inc += 1;

			/* Extra experience */
			if ((cur / 100) < ridinglevel)
			{
				if ((cur / 100 + 15) < ridinglevel)
					inc += 1 + (ridinglevel - (cur / 100 + 15));
				else
					inc += 1;
			}

			p_ptr->skill_exp[GINOU_RIDING] = MIN(max, cur + inc);

			p_ptr->update |= (PU_BONUS);
		}
#endif
	}

	riding_t_m_idx = c_ptr->m_idx;

	/*:::�E��A����A�ψٕ��ʂ̏��ɍU���@�^�[�Q�b�g�����񂾂璆�~*/
	/*:::�i���̂Ƃ���migite�̂�TRUE�ɂȂ��Ă�͂�*/
	/*:::���|�g�ύX�ɂ��A�N���X�{�E���e�������Ă���Ƃ���hidarite��TRUE�ɂȂ��Ċi�������邱�ƂɂȂ����B*/
	///mod150808 �A���X���ꏈ���ǉ�

	//�����X�^�[�ϐg���אڍU��
	if(IS_METAMORPHOSIS)
	{
		monplayer_attack_monst(c_ptr->m_idx);
	}
	//�A���X�l�`�אڍU��
	else if(p_ptr->pclass == CLASS_ALICE && !p_ptr->do_martialarts)
	{
		py_attack_aux_alice(y, x, &fear, &mdeath, mode);
	}
	//���\�E���X�J���v�`���A
	else if(mode == HISSATSU_SOULSCULPTURE && p_ptr->pclass == CLASS_SAKUYA)
	{
		if (p_ptr->migite && inventory[INVEN_RARM].tval == TV_KNIFE)	
			py_attack_aux(y, x, &fear, &mdeath, 0, mode);
		if (p_ptr->hidarite && !mdeath && inventory[INVEN_RARM].tval == TV_KNIFE)
			py_attack_aux(y, x, &fear, &mdeath, 1, mode);
	}
	else
	{
		if (p_ptr->migite)	
			py_attack_aux(y, x, &fear, &mdeath, 0, mode);
		//�ǉ������@�j���ۓ���U���͉E�蕪�̂�
		if (p_ptr->hidarite && !mdeath && mode != HISSATSU_SHINMYOU)
			py_attack_aux(y, x, &fear, &mdeath, 1, mode);

	}

	///mod140323 �ǉ��i������ ����U���̎��͏o�Ȃ�
	//�����ɏ����ǉ�������display_player_various()�́u+���v�\���������ɂ��ǉ�����
	if(!mdeath && !(IS_METAMORPHOSIS) && !mode && !(mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_NO_EXATACK) 
		&& p_ptr->pclass != CLASS_3_FAIRIES ) 
		py_attack_aux2(y,x,&fear,&mdeath);

	/* Mutations which yield extra 'natural' attacks */
	///sys �ψٕ��ʍU���@�����̓��e�́���py_attack_aux2()�Ɉڂ���
#if 0
	if (!mdeath)
	{
		if ((p_ptr->muta2 & MUT2_HORNS) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_HORNS, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_BIGHORN) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_BIGHORN, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_HARDHEAD) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_HARDHEAD, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_TAIL) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_TAIL, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_BIGTAIL) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_BIGTAIL, &fear, &mdeath);
	}
#endif
	
	/* Hack -- delay fear messages */
	if (fear && m_ptr->ml && !mdeath)
	{
		/* Sound */
		sound(SOUND_FLEE);

		/* Message */
#ifdef JP
		msg_format("%^s�͋��|���ē����o�����I", m_name);
#else
		msg_format("%^s flees in terror!", m_name);
#endif

	}

	if ((p_ptr->special_defense & KATA_IAI) && ((mode != HISSATSU_IAI) || mdeath))
	{
		set_action(ACTION_NONE);
	}

	return mdeath;
}

/*:::��_����͂��A�p�^�[���̏������Ƃ��̕��s�\����@*/
bool pattern_seq(int c_y, int c_x, int n_y, int n_x)
{
	feature_type *cur_f_ptr = &f_info[cave[c_y][c_x].feat];
	feature_type *new_f_ptr = &f_info[cave[n_y][n_x].feat];
	bool is_pattern_tile_cur = have_flag(cur_f_ptr->flags, FF_PATTERN);
	bool is_pattern_tile_new = have_flag(new_f_ptr->flags, FF_PATTERN);
	int pattern_type_cur, pattern_type_new;

	if (!is_pattern_tile_cur && !is_pattern_tile_new) return TRUE;

	pattern_type_cur = is_pattern_tile_cur ? cur_f_ptr->subtype : NOT_PATTERN_TILE;
	pattern_type_new = is_pattern_tile_new ? new_f_ptr->subtype : NOT_PATTERN_TILE;

	if (pattern_type_new == PATTERN_TILE_START)
	{
		if (!is_pattern_tile_cur && !p_ptr->confused && !p_ptr->stun && !p_ptr->image)
		{
#ifdef JP
			if (get_check("�p�^�[���̏������n�߂�ƁA�S�Ă�����Ȃ���΂Ȃ�܂���B�����ł����H"))
#else
			if (get_check("If you start walking the Pattern, you must walk the whole way. Ok? "))
#endif
				return TRUE;
			else
				return FALSE;
		}
		else
			return TRUE;
	}
	else if ((pattern_type_new == PATTERN_TILE_OLD) ||
		 (pattern_type_new == PATTERN_TILE_END) ||
		 (pattern_type_new == PATTERN_TILE_WRECKED))
	{
		if (is_pattern_tile_cur)
		{
			return TRUE;
		}
		else
		{
#ifdef JP
			msg_print("�p�^�[���̏������ɂ̓X�^�[�g�n�_��������n�߂Ȃ��Ă͂Ȃ�܂���B");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
	}
	else if ((pattern_type_new == PATTERN_TILE_TELEPORT) ||
		 (pattern_type_cur == PATTERN_TILE_TELEPORT))
	{
		return TRUE;
	}
	else if (pattern_type_cur == PATTERN_TILE_START)
	{
		if (is_pattern_tile_new)
			return TRUE;
		else
		{
#ifdef JP
			msg_print("�p�^�[���̏�͐����������ŕ����˂΂Ȃ�܂���B");
#else
			msg_print("You must walk the Pattern in correct order.");
#endif

			return FALSE;
		}
	}
	else if ((pattern_type_cur == PATTERN_TILE_OLD) ||
		 (pattern_type_cur == PATTERN_TILE_END) ||
		 (pattern_type_cur == PATTERN_TILE_WRECKED))
	{
		if (!is_pattern_tile_new)
		{
#ifdef JP
			msg_print("�p�^�[���𓥂݊O���Ă͂����܂���B");
#else
			msg_print("You may not step off from the Pattern.");
#endif

			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		if (!is_pattern_tile_cur)
		{
#ifdef JP
			msg_print("�p�^�[���̏������ɂ̓X�^�[�g�n�_��������n�߂Ȃ��Ă͂Ȃ�܂���B");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
		else
		{
			byte ok_move = PATTERN_TILE_START;
			switch (pattern_type_cur)
			{
				case PATTERN_TILE_1:
					ok_move = PATTERN_TILE_2;
					break;
				case PATTERN_TILE_2:
					ok_move = PATTERN_TILE_3;
					break;
				case PATTERN_TILE_3:
					ok_move = PATTERN_TILE_4;
					break;
				case PATTERN_TILE_4:
					ok_move = PATTERN_TILE_1;
					break;
				default:
					if (p_ptr->wizard)
#ifdef JP
						msg_format("�������ȃp�^�[�����s�A%d�B", pattern_type_cur);
#else
						msg_format("Funny Pattern walking, %d.", pattern_type_cur);
#endif

					return TRUE; /* Goof-up */
			}

			if ((pattern_type_new == ok_move) ||
			    (pattern_type_new == pattern_type_cur))
				return TRUE;
			else
			{
				if (!is_pattern_tile_new)
#ifdef JP
					msg_print("�p�^�[���𓥂݊O���Ă͂����܂���B");
#else
					msg_print("You may not step off from the Pattern.");
#endif
				else
#ifdef JP
					msg_print("�p�^�[���̏�͐����������ŕ����˂΂Ȃ�܂���B");
#else
					msg_print("You must walk the Pattern in correct order.");
#endif

				return FALSE;
			}
		}
	}
}

/*:::���������n�`���ǂ������f����*/
bool player_can_enter(s16b feature, u16b mode)
{
	feature_type *f_ptr = &f_info[feature];

	if (p_ptr->riding) return monster_can_cross_terrain(feature, &r_info[m_list[p_ptr->riding].r_idx], mode | CEM_RIDING);

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && p_ptr->levitation) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && p_ptr->can_swim) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS) && p_ptr->pass_wall) return TRUE;

	if (!have_flag(f_ptr->flags, FF_WALL) && have_flag(f_ptr->flags, FF_CAN_PASS) && CHECK_FAIRY_TYPE == 41) return TRUE;

	//v1.1.57 �B��ނ̓h�A�𔲂�����
	if (p_ptr->pclass == CLASS_OKINA && have_flag(f_ptr->flags, FF_DOOR) ) return TRUE;

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	return TRUE;
}


/*
 * Move the player
 */
/*:::���̏ꏊ��ύX����Bnx,ny:�ړ��� mpe_mode:�ړ����[�h MPE_????�̌`��defines.h�ɒ�`*/
/*:::���̊֐����Ă΂ꂽ���_��nx,ny�͈ړ��\�ł���Ɗm�F����Ă��邱��*/
/*:::�����K�ړ������莀�񂾂肹�����̃O���b�h�ɋ����TRUE��Ԃ�*/
bool move_player_effect(int ny, int nx, u32b mpe_mode)
{
	cave_type *c_ptr = &cave[ny][nx];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	//v1.1.14 �˕P�̋_���l�����Z�b�g����鏈���B�ʏ��process_player�̃��[�v��ɍs�������b�Z�[�W�Ɉ�a������̂ōŏ��ɂ���Ƃ�
	if(p_ptr->pclass == CLASS_YORIHIME && (p_ptr->kamioroshi & KAMIOROSHI_GION) )
	{
		if(py != ny || px != nx) reset_concentration(TRUE);
		else reset_concent = FALSE;
	}


	if (!(mpe_mode & MPE_STAYING))
	{
		int oy = py;
		int ox = px;
		cave_type *oc_ptr = &cave[oy][ox];
		int om_idx = oc_ptr->m_idx; //���������ꏊ�̃����X�^�[(�R�撆�����X�^�[ �ꏏ�Ɉړ�)
		int nm_idx = c_ptr->m_idx; //�s��̃����X�^�[(�ꏊ����ւ�)

		/* Move the player */
		py = ny;
		px = nx;
		/*:::�����X�^�[�Əꏊ����ւ��H*/
		/* Hack -- For moving monster or riding player's moving */
		if (!(mpe_mode & MPE_DONT_SWAP_MON))
		{
			/* Swap two monsters */
			c_ptr->m_idx = om_idx; 
			oc_ptr->m_idx = nm_idx;

			if (om_idx > 0) /* Monster on old spot (or p_ptr->riding) */
			{
				monster_type *om_ptr = &m_list[om_idx];
				om_ptr->fy = ny;
				om_ptr->fx = nx;
				update_mon(om_idx, TRUE);
			}

			if (nm_idx > 0) /* Monster on new spot */
			{
				monster_type *nm_ptr = &m_list[nm_idx];
				nm_ptr->fy = oy;
				nm_ptr->fx = ox;
				update_mon(nm_idx, TRUE);
			}
		}

		/* Redraw old spot */
		lite_spot(oy, ox);

		/* Redraw new spot */
		lite_spot(ny, nx);

		/* Check for new panel (redraw map) */
		verify_panel();

		if (mpe_mode & MPE_FORGET_FLOW)
		{
			forget_flow();

			/* Mega-Hack -- Forget the view */
			p_ptr->update |= (PU_UN_VIEW);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);
		}

		/* Update stuff */
		p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_DISTANCE);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		/* Remove "unsafe" flag */
		if ((!p_ptr->blind && !no_lite()) || !is_trap(c_ptr->feat)) c_ptr->info &= ~(CAVE_UNSAFE);

		/* For get everything when requested hehe I'm *NASTY* */
		/*:::���{�ȂǂŒn�`��Y��鏈���炵��*/
		if (dun_level && (d_info[dungeon_type].flags1 & DF1_FORGET)) wiz_dark();

		/* Handle stuff */
		if (mpe_mode & MPE_HANDLE_STUFF) handle_stuff();





		/*:::�n�`�ɂ��p�����[�^�Ȃǂ��ύX�����Ƃ��̃��b�Z�[�W����*/
		if(c_ptr->feat != oc_ptr->feat)
		{
			bool flag_update = FALSE;
			feature_type *of_ptr = &f_info[oc_ptr->feat];

			p_ptr->redraw |= PR_STATUS | PR_STATE;


			///mod140316 ���n�`�p���[�A�b�v�����Ȃǂ̂��߂�PU_BONUS���X�V����悤�ɂ���

			if(p_ptr->pclass == CLASS_MURASA || p_ptr->pclass == CLASS_WAKASAGI ||  p_ptr->prace == RACE_KAPPA || p_ptr->prace == RACE_YAMAWARO )
			{
				/* �璷�Ȃ̂Ń��b�Z�[�W�폜
				if(have_flag(f_ptr->flags, FF_WATER) && !have_flag(of_ptr->flags, FF_WATER) )
					msg_print("�͂��N���o��C������I");
				if(!have_flag(f_ptr->flags, FF_WATER) && have_flag(of_ptr->flags, FF_WATER) )
					msg_print("�͂�������C������E�E");
				*/
				flag_update = TRUE;
			}
			else if(p_ptr->prace == RACE_NINGYO )
			{
				/*
				if(have_flag(f_ptr->flags, FF_WATER) && !have_flag(of_ptr->flags, FF_WATER) )
					msg_print("���ɓ������B");
				if(!have_flag(f_ptr->flags, FF_WATER) && have_flag(of_ptr->flags, FF_WATER) )
					msg_print("���ɏオ�����B");
				*/
				flag_update = TRUE;
			}
			else if(CHECK_FAIRY_TYPE == 5 || CHECK_FAIRY_TYPE == 27 || CHECK_FAIRY_TYPE == 32 || CHECK_FAIRY_TYPE == 39 ||  CHECK_FAIRY_TYPE == 40 ||  CHECK_FAIRY_TYPE == 43)
			{
				flag_update = TRUE;
			}
			//v1.1.22 ���m�����Z�\ �����n�`����o��Ƃ��̂��Ƃ��l����CHECK_CONCEAL�͎g��Ȃ�
			else if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_MAGICBULLET,SS_M_CONCEALMENT))
			{
				flag_update = TRUE;
			}
			//v1.1.47 ���}���̒w偂̑�
			else if (p_ptr->pclass == CLASS_YAMAME)
			{
				flag_update = TRUE;
			}


			///mod140402 �M�ꏈ��
			if(have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) )
			{

				if(prace_is_(RACE_VAMPIRE) && !p_ptr->levitation && !p_ptr->can_swim)
				{
					msg_print("���Ȃ��̑̂͏u���Ԃɐ���ւƒ���ł������E�E");
					if(p_ptr->wild_mode)
					{
						/*:::-Hack- �S�̃}�b�v�ړ�����energy_use�����{�ɂȂ��ĂĐ��ɓ������u�Ԏ��ɂ��˂Ȃ��̂ł����Œ����Ă���*/
						energy_use = MIN(energy_use,100);
						change_wild_mode();
					}
					set_action(ACTION_NONE);
					flag_update = TRUE;
				}
				else if(!p_ptr->resist_water && !p_ptr->levitation && p_ptr->total_weight > weight_limit() && !p_ptr->can_swim)
				{
					msg_print("�ו����d���ĉj���Ȃ��I�M��Ă���I");
					if(p_ptr->wild_mode)
					{
						energy_use = MIN(energy_use,100);
						change_wild_mode();
					}
					set_action(ACTION_NONE);
					flag_update = TRUE;
				}
			}
			///mod150413 �M�ꏈ���֘A��������C��
		//	else if(!have_flag(f_ptr->flags, FF_WATER) && have_flag(of_ptr->flags, FF_WATER) )
			else if((have_flag(f_ptr->flags, FF_WATER) || have_flag(of_ptr->flags, FF_WATER)) && f_ptr != of_ptr)
			{
					flag_update = TRUE;
			}

			p_ptr->redraw |= (PR_STATE); //�n�`���\���̂��ߊO�ɏo����
			if(flag_update)
			{
				p_ptr->update |= (PU_BONUS);
				handle_stuff();
			}
		}
		///mod150110 �򐅖v
		if(p_ptr->pclass == CLASS_CHEN && !p_ptr->magic_num1[0] &&
			have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) && !p_ptr->levitation)
		{
			msg_print("���ɓ����Ď���������Ă��܂����I");
			p_ptr->magic_num1[0] = 1;
			p_ptr->update |= (PU_BONUS);
			handle_stuff();
		}

		//v1.1.35 �l���m�꒣��
		if((c_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO) != (oc_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO))
		{
			if(c_ptr->cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO)
				msg_print("���Ȃ��͓꒣��ɖ߂����B");
			else
			{
				msg_print("���Ȃ��͓꒣�肩��o���B");
				//v1.1.54 �l���m�����j�Q���Z�̉���
				p_ptr->special_defense &= ~(SD_UNIQUE_CLASS_POWER);
				p_ptr->redraw |= (PR_STATUS);

			}
			disturb(0, 1);
			p_ptr->update |= (PU_BONUS | PU_HP);
			handle_stuff();
		}
		if (WINTER_LETTY1)
		{
			if (!cave_have_flag_bold(oy, ox, FF_COLD_PUDDLE))
			{
				msg_print("�����͊��C�ɖ����Ă���B�͂��N���o��I");

			}

		}
		if (p_ptr->pclass == CLASS_IKU && cave_have_flag_bold(py, px, FF_ELEC_PUDDLE) && !cave_have_flag_bold(oy, ox, FF_ELEC_PUDDLE))
		{
			msg_print("�����ł͓��Z���g�����S�����Ȃ��ς݂������B");

		}


		/*:::���B��*/
		///class �E�҂̒��B��
		if (p_ptr->pclass == CLASS_NINJA)
		{
			if (c_ptr->info & (CAVE_GLOW)) set_superstealth(FALSE);
			else if (p_ptr->cur_lite <= 0) set_superstealth(TRUE);
		}

		///sys ���삯�͐[������n��ł͋@�\���Ȃ��炵��
		if ((p_ptr->action == ACTION_HAYAGAKE) &&
		    (!have_flag(f_ptr->flags, FF_PROJECT) ||
		     (!p_ptr->levitation && have_flag(f_ptr->flags, FF_DEEP))))
		{
#ifdef JP
			msg_print("�����ł͑f���������Ȃ��B");
#else
			msg_print("You cannot run in here.");
#endif
			set_action(ACTION_NONE);
		}
	}

	/*:::�T�������Ȃ�*/
	if (mpe_mode & MPE_ENERGY_USE)
	{
		/*:::�ǌ@��̉�*/
		if (music_singing(MUSIC_WALL))
		{
			(void)project(0, 0, py, px, (60 + p_ptr->lev), GF_DISINTEGRATE,
				PROJECT_KILL | PROJECT_ITEM, -1);

			if (!player_bold(ny, nx) || p_ptr->is_dead || p_ptr->leaving) return FALSE;
		}

		/* Spontaneous Searching */
		///sys �m�o�����@�T���ƈ�{�����悤
		if ((p_ptr->skill_fos >= 50) || (0 == randint0(50 - p_ptr->skill_fos)))
		{
			search();
		}

		/* Continuous Searching */
		if (p_ptr->action == ACTION_SEARCH)
		{
			search();
		}
	}



	/* Handle "objects" */
	/*:::�����̃A�C�e���ɑ΂��鏈��*/
	if (!(mpe_mode & MPE_DONT_PICKUP))
	{
		carry((mpe_mode & MPE_DO_PICKUP) ? TRUE : FALSE);
	}

	///mod140428 @�̈ړ�������уA�C�e���E�����ɑ����A�C�e���ꗗ���X�V
	if(!running)
	{
		p_ptr->window |= PW_FLOOR_ITEM_LIST;
		window_stuff();
	}

	/* Handle "store doors" */
	if (have_flag(f_ptr->flags, FF_STORE))
	{
		/* Disturb */
		disturb(0, 1);

		energy_use = 0;
		/* Hack -- Enter store */
		command_new = SPECIAL_KEY_STORE;
	}

	/* Handle "building doors" -KMW- */
	else if (have_flag(f_ptr->flags, FF_BLDG))
	{
		/* Disturb */
		disturb(0, 1);

		energy_use = 0;
		/* Hack -- Enter building */
		command_new = SPECIAL_KEY_BUILDING;
	}

	/* Handle quest areas -KMW- */
	else if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
	{
		/* Disturb */
		disturb(0, 1);

		energy_use = 0;
		/* Hack -- Enter quest level */
		command_new = SPECIAL_KEY_QUEST;
	}
	/*:::�o�����B�ŃN���A�ɂȂ�^�C�v�̃N�G�X�g�̏ꍇ�H*/
	else if (have_flag(f_ptr->flags, FF_QUEST_EXIT))
	{
		if (quest[p_ptr->inside_quest].type == QUEST_TYPE_FIND_EXIT)
		{
			complete_quest(p_ptr->inside_quest);
		}

		leave_quest_check();

		p_ptr->inside_quest = c_ptr->special;
		dun_level = 0;
		p_ptr->oldpx = 0;
		p_ptr->oldpy = 0;

		p_ptr->leaving = TRUE;
	}

	/* Set off a trap */
	else if (have_flag(f_ptr->flags, FF_HIT_TRAP) && !(mpe_mode & MPE_STAYING))
	{
		/* Disturb */
		disturb(0, 1);

		/* Hidden trap */
		/*:::�B���g���b�v�Ɉ������������ꍇ�g���b�v���������@������ʂ�Ȃ��Ƃ��̓g���b�v�����Ɏ��s�����Ƃ�*/
		if (c_ptr->mimic || have_flag(f_ptr->flags, FF_SECRET))
		{
			/* Message */
#ifdef JP
			msg_print("�g���b�v���I");
#else
			msg_print("You found a trap!");
#endif

			/* Pick a trap */
			disclose_grid(py, px);
		}

		/* Hit the trap */
		//hit_trap((mpe_mode & MPE_BREAK_TRAP) ? TRUE : FALSE);

		activate_floor_trap(py,px,mpe_mode );


		if (!player_bold(ny, nx) || p_ptr->is_dead || p_ptr->leaving) return FALSE;
	}

	/* Warn when leaving trap detected region */
	if (!(mpe_mode & MPE_STAYING) && (disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(c_ptr->info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(c_ptr->info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* ����:���̐�̓g���b�v�̊��m�͈͊O�ł��I *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect) disturb(0, 1);
		}
	}

	return player_bold(ny, nx) && !p_ptr->is_dead && !p_ptr->leaving;
}

/*:::�������Ă����g���b�v�𔻒肷��*/
bool trap_can_be_ignored(int feat)
{
	feature_type *f_ptr = &f_info[feat];

	if (!have_flag(f_ptr->flags, FF_TRAP)) return TRUE;

	switch (f_ptr->subtype)
	{
	case TRAP_TRAPDOOR:
	case TRAP_PIT:
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if (p_ptr->levitation) return TRUE;
		break;
	case TRAP_TELEPORT:
		if (p_ptr->anti_tele) return TRUE;
		break;
	case TRAP_FIRE:
		if (p_ptr->immune_fire) return TRUE;
		break;
	case TRAP_ACID:
		if (p_ptr->immune_acid) return TRUE;
		break;
	case TRAP_BLIND:
		if (p_ptr->resist_blind) return TRUE;
		break;
	case TRAP_CONFUSE:
		if (p_ptr->resist_conf) return TRUE;
		break;
	case TRAP_POISON:
		if (p_ptr->resist_pois) return TRUE;
		break;
	case TRAP_SLEEP:
		if (p_ptr->free_act) return TRUE;
		break;
	}

	return FALSE;
}


/*
 * Determine if a "boundary" grid is "floor mimic"
 */
#define boundary_floor(C, F, MF) \
	((C)->mimic && permanent_wall(F) && \
	 (have_flag((MF)->flags, FF_MOVE) || have_flag((MF)->flags, FF_CAN_FLY)) && \
	 have_flag((MF)->flags, FF_PROJECT) && \
	 !have_flag((MF)->flags, FF_OPEN))

/*
 * Move player in the given direction, with the given "pickup" flag.
 *
 * This routine should (probably) always induce energy expenditure.
 *
 * Note that moving will *always* take a turn, and will *always* hit
 * any monster which might be in the destination grid.  Previously,
 * moving into walls was "free" and did NOT hit invisible monsters.
 */
/*:::�����w�肵�������ֈړ�����B
 *:::�ړ��͏�Ƀ^�[��������A���̃O���b�h�Ƀ����X�^�[���������ɍU������B
 *:::�ǂɌ������Ĉړ�����̂̓^�[����������ǂ̒��̌����Ȃ������X�^�[�͍U�����Ȃ��H
 *:::do_pickup:�A�C�e�����E��
 *:::break_trap:�g���b�v��j�󂷂�i����m�p�H�j
 *:::oktomove:�ړ��ɐ����������ǂ����̃t���O
 */
//v1.1.97 activate_trap�t���O�ǉ��B���ꂪTRUE�̂Ƃ�㩂��������Ȃ������V�������Ă����Ƃ����◎�Ƃ��˂ɗ�����
void move_player(int dir, bool do_pickup, bool break_trap, bool activate_trap)
{
	/* Find the result of moving */
	/*:::�ړ���O���b�h�̈ʒu���v�Z*/
	int y = py + ddy[dir];
	int x = px + ddx[dir];

	/* Examine the destination */
	/*:::�ړ���n�`���𓾂�*/
	cave_type *c_ptr = &cave[y][x];

	feature_type *f_ptr = &f_info[c_ptr->feat];

	monster_type *m_ptr;

	/*:::�R�撆�����X�^�[��monster_type��monster_race�𓾂�*/
	monster_type *riding_m_ptr = &m_list[p_ptr->riding];
	monster_race *riding_r_ptr = &r_info[p_ptr->riding ? riding_m_ptr->r_idx : 0]; /* Paranoia */

	char m_name[80];

	bool p_can_enter = player_can_enter(c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);
	bool p_can_kill_walls = FALSE;
	bool stormbringer = FALSE;

	bool oktomove = TRUE;
	bool do_past = FALSE;




	if(IS_METAMORPHOSIS && !c_ptr->m_idx)
	{
		monster_race *r_ptr = &r_info[MON_EXTRA_FIELD];

		if(r_ptr->flags1 & RF1_NEVER_MOVE)
		{
			msg_print("���Ȃ��͓����Ȃ��B");
			return;
		}
		if(cave_have_flag_bold(y,x,FF_WATER) && cave_have_flag_bold(y,x,FF_DEEP) && !(r_ptr->flags7 & (RF7_CAN_SWIM|RF7_CAN_FLY)))
		{
			msg_print("���Ȃ��͉j���Ȃ��B");
			return;
		}
		if(!cave_have_flag_bold(y,x,FF_WATER) && (r_ptr->flags7 & (RF7_AQUATIC)) && !(r_ptr->flags7 & (RF7_CAN_FLY)))
		{
			msg_print("���Ȃ��͗���œ����Ȃ��B");
			return;
		}
	}

	/* Exit the area */
	/*:::�r��ŗׂ̃}�b�v�Ɉڂ�Ƃ�*/
	if (!dun_level && !p_ptr->wild_mode &&
		((x == 0) || (x == MAX_WID - 1) ||
		 (y == 0) || (y == MAX_HGT - 1)))
	{
		/* Can the player enter the grid? */
			/*:::��������邩�ǂ����m�F����*/
		if (c_ptr->mimic && player_can_enter(c_ptr->mimic, 0))
		{
			/* Hack: move to new area */
			if ((y == 0) && (x == 0))
			{
				p_ptr->wilderness_y--;
				p_ptr->wilderness_x--;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if ((y == 0) && (x == MAX_WID - 1))
			{
				p_ptr->wilderness_y--;
				p_ptr->wilderness_x++;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = 1;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == 0))
			{
				p_ptr->wilderness_y++;
				p_ptr->wilderness_x--;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == MAX_WID - 1))
			{
				p_ptr->wilderness_y++;
				p_ptr->wilderness_x++;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = 1;
				ambush_flag = FALSE;
			}

			else if (y == 0)
			{
				p_ptr->wilderness_y--;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = x;
				ambush_flag = FALSE;
			}

			else if (y == MAX_HGT - 1)
			{
				p_ptr->wilderness_y++;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = x;
				ambush_flag = FALSE;
			}

			else if (x == 0)
			{
				p_ptr->wilderness_x--;
				p_ptr->oldpx = cur_wid - 2;
				p_ptr->oldpy = y;
				ambush_flag = FALSE;
			}

			else if (x == MAX_WID - 1)
			{
				p_ptr->wilderness_x++;
				p_ptr->oldpx = 1;
				p_ptr->oldpy = y;
				ambush_flag = FALSE;
			}
			/*:::�}�b�v����o��Ƃ�leaving*/
			p_ptr->leaving = TRUE;
			energy_use = 100;

			return;
		}

		/* "Blocked" message appears later */
		/* oktomove = FALSE; */
		/*:::�ʂ�Ȃ��Ƃ��̃t���O�H*/
		p_can_enter = FALSE;
	}

	/* Get the monster */
	m_ptr = &m_list[c_ptr->m_idx];

	///item �X�g�[���u�����K�[�������̃t���O
	if (!check_invalidate_inventory(INVEN_RARM) && inventory[INVEN_RARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
	if (!check_invalidate_inventory(INVEN_LARM) && inventory[INVEN_LARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;

	/* Player can not walk through "walls"... */
	/* unless in Shadow Form */
	p_can_kill_walls = p_ptr->kill_wall && have_flag(f_ptr->flags, FF_HURT_DISI) &&
		(!p_can_enter || !have_flag(f_ptr->flags, FF_LOS)) &&
		!have_flag(f_ptr->flags, FF_PERMANENT);

	/* Hack -- attack monsters */
	/*:::�ړ���Ƀ����X�^�[�����āA�u�����Ă���or�����ĂȂ������̃O���b�h�Ɉړ��ł���v�Ƃ��A�U������*/
	if (c_ptr->m_idx && (m_ptr->ml || p_can_enter || p_can_kill_walls))
	{

		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		//���̐��E���ꏈ��
		if(SAKUYA_WORLD)
		{
			monster_desc(m_name, m_ptr, 0);
			msg_format("%^s�̑̂��ז��ł�����ɍs���Ȃ��B", m_name);
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		/* Attack -- only if we can see it OR it is not in a wall */
		/*:::�����X�^�[���F�D�I�Ō����Ă���A���������E���o�E�N�O�E����m���ψقłȂ��A�������̏ꏊ�ɓ����ꍇ�����̂�����*/
		else if (!is_hostile(m_ptr) &&
		    !(p_ptr->confused || p_ptr->image || !m_ptr->ml || p_ptr->stun ||
		    ((p_ptr->muta2 & MUT2_BERS_RAGE) && p_ptr->shero)) &&
		    pattern_seq(py, px, y, x) && (p_can_enter || p_can_kill_walls))
		{
			/* Disturb the monster */
			/*:::�N����*/
			(void)set_monster_csleep(c_ptr->m_idx, 0);

			/* Extract monster name (or "it") */
			monster_desc(m_name, m_ptr, 0);

			if (m_ptr->ml)
			{
				/* Auto-Recall if possible and visible */
				if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

				/* Track a new monster */
				health_track(c_ptr->m_idx);
			}

			/* displace? */
			/*:::�X�g�[���u�����K�[�𑕔����Ă���Ƃ��A�����X�^�[���F�D�I�ł��y�b�g�ł��U������H*/
			///class ����m�����m���ŗF�D�I�ȓG�ɂ��U�����鏈��
			if ((stormbringer && (randint1(1000) > 666)) || ( p_ptr->pseikaku == SEIKAKU_BERSERK))
			{
				py_attack(y, x, 0);
				oktomove = FALSE;
			}
			else if (monster_can_cross_terrain(cave[py][px].feat, r_ptr, 0))
			{
				do_past = TRUE;
			}
			else
			{
#ifdef JP
				msg_format("%^s���ז����I", m_name);
#else
				msg_format("%^s is in your way!", m_name);
#endif

				energy_use = 0;
				oktomove = FALSE;
			}

			/* now continue on to 'movement' */
		}
		else
		{
			py_attack(y, x, 0);
			oktomove = FALSE;
		}
	}

	//v1.1.24 ���ꏈ���@�N�G�X�g�u�}������v�̐��̒��ł͍����Ɉړ��ł��Ȃ�
	if(oktomove && QRKDR)
	{
		if(cave_have_flag_bold(py,px,FF_WATER) && x < px)
		{
			msg_print("�}���ɉ����߂��ꂽ�I");
			oktomove = FALSE;
			disturb(0, 1);
		}

	}

	/*:::�R�掞�̏������܂���*/
	if (oktomove && p_ptr->riding)
	{

		if (riding_r_ptr->flags1 & RF1_NEVER_MOVE)
		{
#ifdef JP
			msg_print("�����Ȃ��I");
#else
			msg_print("Can't move!");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (MON_MONFEAR(riding_m_ptr))
		{

			/* Acquire the monster name */
			monster_desc(m_name, riding_m_ptr, 0);

			/* Dump a message */
#ifdef JP
			msg_format("%s�����|���Ă��Đ���ł��Ȃ��B", m_name);
#else
			msg_format("%^s is too scared to control.", m_name);
#endif
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (p_ptr->riding_ryoute && !CLASS_RIDING_BACKDANCE)
		{
			oktomove = FALSE;
			disturb(0, 1);
		}
		//v1.1.24 �}������N�G�X�g�p���ꏈ��
		else if(QRKDR && have_flag(f_ptr->flags, FF_WATER) && !((riding_r_ptr->flags7 & (RF7_CAN_SWIM | RF7_AQUATIC))))
		{
			monster_desc(m_name, riding_m_ptr, 0);

			msg_format("%s�����ɓ��肽����Ȃ��B", m_name);
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (have_flag(f_ptr->flags, FF_CAN_FLY) && (riding_r_ptr->flags7 & RF7_CAN_FLY))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_CAN_SWIM) && (riding_r_ptr->flags7 & RF7_CAN_SWIM))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_WATER) &&
			!(riding_r_ptr->flags7 & RF7_AQUATIC) &&
			(have_flag(f_ptr->flags, FF_DEEP) || (riding_r_ptr->flags2 & RF2_AURA_FIRE)))
		{
#ifdef JP
			msg_format("%s�̏�ɍs���Ȃ��B", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Can't swim.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (!have_flag(f_ptr->flags, FF_WATER) && (riding_r_ptr->flags7 & RF7_AQUATIC))
		{
#ifdef JP
			msg_format("%s����オ��Ȃ��B", f_name + f_info[get_feat_mimic(&cave[py][px])].name);
#else
			msg_print("Can't land.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (have_flag(f_ptr->flags, FF_LAVA) && !(riding_r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK))
		{
#ifdef JP
			msg_format("%s�̏�ɍs���Ȃ��B", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Too hot to go through.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		//v1.1.85 �_�̏��Ɠł̏����ǉ�
		else if (have_flag(f_ptr->flags, FF_ACID_PUDDLE) && !(riding_r_ptr->flagsr & RFR_EFF_IM_ACID_MASK))
		{
			msg_format("%s�̏�ɍs���Ȃ��B", f_name + f_info[get_feat_mimic(c_ptr)].name);
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}
		else if (have_flag(f_ptr->flags, FF_POISON_PUDDLE) && !(riding_r_ptr->flagsr & RFR_EFF_IM_POIS_MASK))
		{
			msg_format("%s�̏�ɍs���Ȃ��B", f_name + f_info[get_feat_mimic(c_ptr)].name);
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 1);
		}




		if (oktomove && MON_STUNNED(riding_m_ptr) && one_in_(2))
		{
			monster_desc(m_name, riding_m_ptr, 0);
#ifdef JP
			msg_format("%s���N�O�Ƃ��Ă��Ă��܂������Ȃ��I",m_name);
#else
			msg_format("You cannot control stunned %s!",m_name);
#endif
			oktomove = FALSE;
			disturb(0, 1);
		}
	}

	if (!oktomove)
	{
	}

	else if (!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !p_ptr->levitation)
	{

		//v1.1.86 �R���͕��V���Ȃ��Ă��R�����z�����邱�Ƃɂ���
		if (prace_is_(RACE_YAMAWARO) && have_flag(f_ptr->flags, FF_MOUNTAIN))
		{
			;
		}
		else
		{
			msg_format("����΂Ȃ���%s�̏�ɂ͍s���Ȃ��B", f_name + f_info[get_feat_mimic(c_ptr)].name);

			energy_use = 0;
			running = 0;
			oktomove = FALSE;
		}

	}

	/*
	 * Player can move through trees and
	 * has effective -10 speed
	 * Rangers can move without penality
	 */
	/*:::�؂̏�Ō������鏈��*/
	//v1.1.86 �R���͌������Ȃ����Ƃɂ���
	else if (have_flag(f_ptr->flags, FF_TREE) && !p_can_kill_walls && !p_ptr->levitation)
	{
		if ((p_ptr->pclass != CLASS_RANGER && !prace_is_(RACE_YAMAWARO))  && (!p_ptr->riding || !(riding_r_ptr->flags8 & RF8_WILD_WOOD))) energy_use *= 2;
	}

	//v1.1.91 �Ζ��n�`��Ō������鏈��
	else if (have_flag(f_ptr->flags, FF_OIL_FIELD) && !p_ptr->levitation)
	{
		if ((p_ptr->pclass != CLASS_YUMA )) energy_use *= 2;
	}


#ifdef ALLOW_EASY_DISARM /* TNB */

	/* Disarm a visible trap */
	else if ((do_pickup != easy_disarm) && have_flag(f_ptr->flags, FF_DISARM) && !c_ptr->mimic && !activate_trap)
	{
		if (!trap_can_be_ignored(c_ptr->feat))
		{
			(void)do_cmd_disarm_aux(y, x, dir);
			return;
		}
	}

#endif /* ALLOW_EASY_DISARM -- TNB */

	/* Player can not walk through "walls" unless in wraith form...*/
	else if (!p_can_enter && !p_can_kill_walls)
	{
		/* Feature code (applying "mimic" field) */
		s16b feat = get_feat_mimic(c_ptr);
		feature_type *mimic_f_ptr = &f_info[feat];
		cptr name = f_name + mimic_f_ptr->name;

		oktomove = FALSE;

		/* Notice things in the dark */
		if (!(c_ptr->info & CAVE_MARK) && !player_can_see_bold(y, x))
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("����ȏ��ɂ͐i�߂Ȃ��悤���B");
#else
				msg_print("You feel you cannot go any more.");
#endif
			}

			/* Wall (or secret door) */
			else
			{
#ifdef JP
				msg_format("%s���s������͂΂�ł���悤���B", name);
#else
				msg_format("You feel %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				c_ptr->info |= (CAVE_MARK);
				lite_spot(y, x);
			}
		}

		/* Notice things */
		else
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("����ȏ��ɂ͐i�߂Ȃ��B");
#else
				msg_print("You cannot go any more.");
#endif

				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
					energy_use = 0;
			}

			/* Wall (or secret door) */
			else
			{
#ifdef ALLOW_EASY_OPEN
				/* Closed doors */
				if (easy_open && is_closed_door(feat) && easy_open_door(y, x)) return;
#endif /* ALLOW_EASY_OPEN */

#ifdef JP
				msg_format("%s���s������͂΂�ł���B", name);
#else
				msg_format("There is %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				/*
				 * Well, it makes sense that you lose time bumping into
				 * a wall _if_ you are confused, stunned or blind; but
				 * typing mistakes should not cost you a turn...
				 */
				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
					energy_use = 0;
			}
		}

		/* Disturb the player */
		disturb(0, 1);

		/* Sound */
		if (!boundary_floor(c_ptr, f_ptr, mimic_f_ptr)) sound(SOUND_HITWALL);
	}

	/* Normal movement */
	if (oktomove && !pattern_seq(py, px, y, x))
	{
		if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
		{
			energy_use = 0;
		}

		/* To avoid a loop with running */
		disturb(0, 1);

		oktomove = FALSE;
	}

	/* Normal movement */
	if (oktomove)
	{
		u32b mpe_mode = MPE_ENERGY_USE;
		/*:::�x������*/
		//v1.1.97 㩔����ړ��̂Ƃ��x�����o�Ȃ�����B�댯�ȃ����X�^�[�̕��̌x�����o�Ȃ��Ȃ邪�܂�����قǊ�ȋ����ł��Ȃ����낤
		if (p_ptr->warning && !activate_trap)
		{
			if (!process_warning(x, y,FALSE))
			{
				energy_use = 25;
				return;
			}
		}


		//�h�b�y���Q���K�[���ꏈ��
		if(do_past && r_info[m_ptr->r_idx].flags7 & RF7_DOPPELGANGER)
		{
			msg_print("���Ȃ��͎����̃h�b�y���Q���K�[�ɐG��Ă��܂����I");
			take_hit(DAMAGE_LOSELIFE,p_ptr->chp,"�h�b�y���Q���K�[�Ƃ̐ڐG",-1);
			set_stun(100+randint0(3));
			energy_use = 100;
			delete_monster_idx(cave[y][x].m_idx);

			return;
		}

		if (do_past)
		{
#ifdef JP
			msg_format("%s�������ނ����B", m_name);
#else
			msg_format("You push past %s.", m_name);
#endif
		}

		/* Change oldpx and oldpy to place the player well when going back to big mode */
		if (p_ptr->wild_mode)
		{
			if (ddy[dir] > 0)  p_ptr->oldpy = 1;
			if (ddy[dir] < 0)  p_ptr->oldpy = MAX_HGT - 2;
			if (ddy[dir] == 0) p_ptr->oldpy = MAX_HGT / 2;
			if (ddx[dir] > 0)  p_ptr->oldpx = 1;
			if (ddx[dir] < 0)  p_ptr->oldpx = MAX_WID - 2;
			if (ddx[dir] == 0) p_ptr->oldpx = MAX_WID / 2;
		}

		if (p_can_kill_walls)
		{
			cave_alter_feat(y, x, FF_HURT_DISI);

			/* Update some things -- similar to GF_KILL_WALL */
			p_ptr->update |= (PU_FLOW);
		}

		/* Sound */
		/* sound(SOUND_WALK); */

#ifdef ALLOW_EASY_DISARM /* TNB */

		if (do_pickup != always_pickup) mpe_mode |= MPE_DO_PICKUP;

#else /* ALLOW_EASY_DISARM -- TNB */

		if (do_pickup) mpe_mode |= MPE_DO_PICKUP;

#endif /* ALLOW_EASY_DISARM -- TNB */

		if (activate_trap) mpe_mode |= MPE_ACTIVATE_TRAP;
		else if (break_trap) mpe_mode |= MPE_BREAK_TRAP;

		/* Move the player */
		(void)move_player_effect(y, x, mpe_mode);
	}


}


static bool ignore_avoid_run;

/*
 * Hack -- Check for a "known wall" (see below)
 */
/*:::�Ώۂ̃O���b�h���u�L�����Ă���ǁv���ǂ�������*/
static int see_wall(int dir, int y, int x)
{
	cave_type   *c_ptr;

	/* Get the new location */
	/*:::dir�ɓ�����1-9�̐����ɂ��x��y��-1�`+1�̌v�Z������*/
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are not known walls */
	if (!in_bounds2(y, x)) return (FALSE);

	/* Access grid */
	c_ptr = &cave[y][x];

	/* Must be known to the player */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Feature code (applying "mimic" field) */
		s16b         feat = get_feat_mimic(c_ptr);
		feature_type *f_ptr = &f_info[feat];

		/* Wall grids are known walls */
		if (!player_can_enter(feat, 0)) return !have_flag(f_ptr->flags, FF_DOOR);

		/* Don't run on a tree unless explicitly requested */
		if (have_flag(f_ptr->flags, FF_AVOID_RUN) && !ignore_avoid_run)
			return TRUE;

		/* Don't run in a wall */
		if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
			return !have_flag(f_ptr->flags, FF_DOOR);
	}

	return FALSE;
}


/*
 * Hack -- Check for an "unknown corner" (see below)
 */
static int see_nothing(int dir, int y, int x)
{
	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are unknown */
	if (!in_bounds2(y, x)) return (TRUE);

	/* Memorized grids are always known */
	if (cave[y][x].info & (CAVE_MARK)) return (FALSE);

	/* Viewable door/wall grids are known */
	if (player_can_see_bold(y, x)) return (FALSE);

	/* Default */
	return (TRUE);
}





/*
 * The running algorithm:                       -CJS-
 *
 * In the diagrams below, the player has just arrived in the
 * grid marked as '@', and he has just come from a grid marked
 * as 'o', and he is about to enter the grid marked as 'x'.
 *
 * Of course, if the "requested" move was impossible, then you
 * will of course be blocked, and will stop.
 *
 * Overview: You keep moving until something interesting happens.
 * If you are in an enclosed space, you follow corners. This is
 * the usual corridor scheme. If you are in an open space, you go
 * straight, but stop before entering enclosed space. This is
 * analogous to reaching doorways. If you have enclosed space on
 * one side only (that is, running along side a wall) stop if
 * your wall opens out, or your open space closes in. Either case
 * corresponds to a doorway.
 *
 * What happens depends on what you can really SEE. (i.e. if you
 * have no light, then running along a dark corridor is JUST like
 * running in a dark room.) The algorithm works equally well in
 * corridors, rooms, mine tailings, earthquake rubble, etc, etc.
 *
 * These conditions are kept in static memory:
 * find_openarea         You are in the open on at least one
 * side.
 * find_breakleft        You have a wall on the left, and will
 * stop if it opens
 * find_breakright       You have a wall on the right, and will
 * stop if it opens
 *
 * To initialize these conditions, we examine the grids adjacent
 * to the grid marked 'x', two on each side (marked 'L' and 'R').
 * If either one of the two grids on a given side is seen to be
 * closed, then that side is considered to be closed. If both
 * sides are closed, then it is an enclosed (corridor) run.
 *
 * LL           L
 * @x          LxR
 * RR          @R
 *
 * Looking at more than just the immediate squares is
 * significant. Consider the following case. A run along the
 * corridor will stop just before entering the center point,
 * because a choice is clearly established. Running in any of
 * three available directions will be defined as a corridor run.
 * Note that a minor hack is inserted to make the angled corridor
 * entry (with one side blocked near and the other side blocked
 * further away from the runner) work correctly. The runner moves
 * diagonally, but then saves the previous direction as being
 * straight into the gap. Otherwise, the tail end of the other
 * entry would be perceived as an alternative on the next move.
 *
 * #.#
 * ##.##
 * .@x..
 * ##.##
 * #.#
 *
 * Likewise, a run along a wall, and then into a doorway (two
 * runs) will work correctly. A single run rightwards from @ will
 * stop at 1. Another run right and down will enter the corridor
 * and make the corner, stopping at the 2.
 *
 * ##################
 * o@x       1
 * ########### ######
 * #2          #
 * #############
 *
 * After any move, the function area_affect is called to
 * determine the new surroundings, and the direction of
 * subsequent moves. It examines the current player location
 * (at which the runner has just arrived) and the previous
 * direction (from which the runner is considered to have come).
 *
 * Moving one square in some direction places you adjacent to
 * three or five new squares (for straight and diagonal moves
 * respectively) to which you were not previously adjacent,
 * marked as '!' in the diagrams below.
 *
 *   ...!              ...
 *   .o@!  (normal)    .o.!  (diagonal)
 *   ...!  (east)      ..@!  (south east)
 *                      !!!
 *
 * You STOP if any of the new squares are interesting in any way:
 * for example, if they contain visible monsters or treasure.
 *
 * You STOP if any of the newly adjacent squares seem to be open,
 * and you are also looking for a break on that side. (that is,
 * find_openarea AND find_break).
 *
 * You STOP if any of the newly adjacent squares do NOT seem to be
 * open and you are in an open area, and that side was previously
 * entirely open.
 *
 * Corners: If you are not in the open (i.e. you are in a corridor)
 * and there is only one way to go in the new squares, then turn in
 * that direction. If there are more than two new ways to go, STOP.
 * If there are two ways to go, and those ways are separated by a
 * square which does not seem to be open, then STOP.
 *
 * Otherwise, we have a potential corner. There are two new open
 * squares, which are also adjacent. One of the new squares is
 * diagonally located, the other is straight on (as in the diagram).
 * We consider two more squares further out (marked below as ?).
 *
 * We assign "option" to the straight-on grid, and "option2" to the
 * diagonal grid, and "check_dir" to the grid marked 's'.
 *
 * ##s
 * @x?
 * #.?
 *
 * If they are both seen to be closed, then it is seen that no benefit
 * is gained from moving straight. It is a known corner.  To cut the
 * corner, go diagonally, otherwise go straight, but pretend you
 * stepped diagonally into that next location for a full view next
 * time. Conversely, if one of the ? squares is not seen to be closed,
 * then there is a potential choice. We check to see whether it is a
 * potential corner or an intersection/room entrance.  If the square
 * two spaces straight ahead, and the space marked with 's' are both
 * unknown space, then it is a potential corner and enter if
 * find_examine is set, otherwise must stop because it is not a
 * corner. (find_examine option is removed and always is TRUE.)
 */




/*
 * Hack -- allow quick "cycling" through the legal directions
 */
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

/*
 * Hack -- map each direction into the "middle" of the "cycle[]" array
 */
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };

/*
 * The direction we are running
 */
static byte find_current;

/*
 * The direction we came from
 */
static byte find_prevdir;

/*
 * We are looking for open area
 */
static bool find_openarea;

/*
 * We are looking for a break
 */
static bool find_breakright;
static bool find_breakleft;



/*
 * Initialize the running algorithm for a new direction.
 *
 * Diagonal Corridor -- allow diaginal entry into corridors.
 *
 * Blunt Corridor -- If there is a wall two spaces ahead and
 * we seem to be in a corridor, then force a turn into the side
 * corridor, must be moving straight into a corridor here. ???
 *
 * Diagonal Corridor    Blunt Corridor (?)
 *       # #                  #
 *       #x#                 @x#
 *       @p.                  p
 */
/*:::���邽�߂̏������H�ڍז���*/
static void run_init(int dir)
{
	int             row, col, deepleft, deepright;
	int             i, shortleft, shortright;


	/* Save the direction */
	find_current = dir;

	/* Assume running straight */
	find_prevdir = dir;

	/* Assume looking for open area */
	find_openarea = TRUE;

	/* Assume not looking for breaks */
	find_breakright = find_breakleft = FALSE;

	/* Assume no nearby walls */
	deepleft = deepright = FALSE;
	shortright = shortleft = FALSE;

	p_ptr->run_py = py;
	p_ptr->run_px = px;

	/* Find the destination grid */
	row = py + ddy[dir];
	col = px + ddx[dir];

	ignore_avoid_run = cave_have_flag_bold(row, col, FF_AVOID_RUN);

	/* Extract cycle index */
	i = chome[dir];

	/* Check for walls */
	if (see_wall(cycle[i+1], py, px))
	{
		find_breakleft = TRUE;
		shortleft = TRUE;
	}
	else if (see_wall(cycle[i+1], row, col))
	{
		find_breakleft = TRUE;
		deepleft = TRUE;
	}

	/* Check for walls */
	if (see_wall(cycle[i-1], py, px))
	{
		find_breakright = TRUE;
		shortright = TRUE;
	}
	else if (see_wall(cycle[i-1], row, col))
	{
		find_breakright = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if (find_breakleft && find_breakright)
	{
		/* Not looking for open area */
		find_openarea = FALSE;

		/* Hack -- allow angled corridor entry */
		if (dir & 0x01)
		{
			if (deepleft && !deepright)
			{
				find_prevdir = cycle[i - 1];
			}
			else if (deepright && !deepleft)
			{
				find_prevdir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if (see_wall(cycle[i], row, col))
		{
			if (shortleft && !shortright)
			{
				find_prevdir = cycle[i - 2];
			}
			else if (shortright && !shortleft)
			{
				find_prevdir = cycle[i + 2];
			}
		}
	}
}


/*
 * Update the current "run" path
 *
 * Return TRUE if the running should be stopped
 */
/*:::�����Ă�Ƃ��̎��̈���̕��������߁Afind_current�Ɋi�[���Ă���炵���B�ڍז���*/
static bool run_test(void)
{
	int         prev_dir, new_dir, check_dir = 0;
	int         row, col;
	int         i, max, inv;
	int         option = 0, option2 = 0;
	cave_type   *c_ptr;
	s16b        feat;
	feature_type *f_ptr;

	/* Where we came from */
	prev_dir = find_prevdir;


	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(cave[py][px].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[py][px].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* ����:���̐�̓g���b�v�̊��m�͈͊O�ł��I *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		s16b this_o_idx, next_o_idx = 0;

		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = py + ddy[new_dir];
		col = px + ddx[new_dir];

		/* Access grid */
		c_ptr = &cave[row][col];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &f_info[feat];

		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}

		/* Visible objects abort running */
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Visible object */
			if (o_ptr->marked & OM_FOUND) return (TRUE);
		}

		/* Assume unknown */
		inv = TRUE;

		/* Check memorized grids */
		if (c_ptr->info & (CAVE_MARK))
		{
			bool notice = have_flag(f_ptr->flags, FF_NOTICE);

			if (notice && have_flag(f_ptr->flags, FF_MOVE))
			{
				/* Open doors */
				if (find_ignore_doors && have_flag(f_ptr->flags, FF_DOOR) && have_flag(f_ptr->flags, FF_CLOSE))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Stairs */
				else if (find_ignore_stairs && have_flag(f_ptr->flags, FF_STAIRS))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Lava */
				else if (have_flag(f_ptr->flags, FF_LAVA) && (p_ptr->immune_fire || IS_INVULN()))
				{
					/* Ignore */
					notice = FALSE;
				}

				/* Deep water */
				else if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
				         (p_ptr->levitation || p_ptr->can_swim || (p_ptr->total_weight <= weight_limit())))
				{
					/* Ignore */
					notice = FALSE;
				}
				//v1.1.85 �����Ɏ_�̏��ȂǂɖƉu������Ƃ���notice���������������ׂ��Ȃ񂾂낤���ǂ��܂��܂Ƃ܂�񂵑������������g��񂾂낤����p�X


			}

			/* Interesting feature */
			if (notice) return (TRUE);

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors considering mimic */
		if (inv || !see_wall(0, row, col))
		{
			/* Looking for open area */
			if (find_openarea)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if (!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if (option2)
			{
				return (TRUE);
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if (option != cycle[chome[prev_dir] + i - 1])
			{
				return (TRUE);
			}

			/* Two new (adjacent) directions (case 1) */
			else if (new_dir & 0x01)
			{
				check_dir = cycle[chome[prev_dir] + i - 2];
				option2 = new_dir;
			}

			/* Two new (adjacent) directions (case 2) */
			else
			{
				check_dir = cycle[chome[prev_dir] + i + 1];
				option2 = option;
				option = new_dir;
			}
		}

		/* Obstacle, while looking for open area */
		else
		{
			if (find_openarea)
			{
				if (i < 0)
				{
					/* Break to the right */
					find_breakright = TRUE;
				}

				else if (i > 0)
				{
					/* Break to the left */
					find_breakleft = TRUE;
				}
			}
		}
	}

	/* Looking for open area */
	if (find_openarea)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			/* Unknown grid or non-wall */
			if (!see_wall(cycle[chome[prev_dir] + i], py, px))
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			/* Unknown grid or non-wall */
			if (!see_wall(cycle[chome[prev_dir] + i], py, px))
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}
		}
	}

	/* Not looking for open area */
	else
	{
		/* No options */
		if (!option)
		{
			return (TRUE);
		}

		/* One option */
		else if (!option2)
		{
			/* Primary option */
			find_current = option;

			/* No other options */
			find_prevdir = option;
		}

		/* Two options, examining corners */
		else if (!find_cut)
		{
			/* Primary option */
			find_current = option;

			/* Hack -- allow curving */
			find_prevdir = option2;
		}

		/* Two options, pick one */
		else
		{
			/* Get next location */
			row = py + ddy[option];
			col = px + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if (!see_wall(option, row, col) ||
			    !see_wall(check_dir, row, col))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if (see_nothing(option, row, col) &&
				    see_nothing(option2, row, col))
				{
					find_current = option;
					find_prevdir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return (TRUE);
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if (find_cut)
			{
				find_current = option2;
				find_prevdir = option2;
			}

			/* This corner is seen to be enclosed, and we */
			/* deliberately go the long way. */
			else
			{
				find_current = option;
				find_prevdir = option2;
			}
		}
	}

	/* About to hit a known wall, stop */
	if (see_wall(find_current, py, px))
	{
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}



/*
 * Take one step along the current "run" path
 */
/*:::�^����ꂽ�����֑���*/
void run_step(int dir)
{
	/* Start running */
	if (dir)
	{
		/* Ignore AVOID_RUN on a first step */
		ignore_avoid_run = TRUE;

		/* Hack -- do not start silly run */
		if (see_wall(dir, py, px))
		{
			/* Message */
#ifdef JP
			msg_print("���̕����ɂ͑���܂���B");
#else
			msg_print("You cannot run in that direction.");
#endif

			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}

		/* Initialize */
		run_init(dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if (run_test())
		{
			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}
	}

	/* Decrease the run counter */
	if (--running <= 0) return;

	/* Take time */
	energy_use = 100;

	//v3.0.2 ����R�}���h��energy_use�l�ɍ����ړ��Ȃǂ�K�p����
	walk_energy_modify();

	/* Move the player, using the "pickup" flag */
#ifdef ALLOW_EASY_DISARM /* TNB */

	move_player(find_current, FALSE, FALSE,FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

	move_player(find_current, always_pickup, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */

	if (player_bold(p_ptr->run_py, p_ptr->run_px))
	{
		p_ptr->run_py = 0;
		p_ptr->run_px = 0;
		disturb(0, 0);
	}
}


#ifdef TRAVEL
/*
 * Test for traveling
 */
/*:::�g���x���R�}���h�Ői�ޕ��������߂�*/
static int travel_test(int prev_dir)
{
	int new_dir = 0;
	int i, max;
	const cave_type *c_ptr;
	int cost;

	/* Cannot travel when blind */
	if (p_ptr->blind || no_lite())
	{
		msg_print(_("�ڂ������Ȃ��I", "You cannot see!"));
		return (0);
	}

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(cave[py][px].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[py][px].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* ����:���̐�̓g���b�v�̊��m�͈͊O�ł��I *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return (0);
			}
		}
	}

	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		/* New direction */
		int dir = cycle[chome[prev_dir] + i];

		/* New location */
		int row = py + ddy[dir];
		int col = px + ddx[dir];

		/* Access grid */
		c_ptr = &cave[row][col];

		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (0);
		}

	}

	/* Travel cost of current grid */
	cost = travel.cost[py][px];

	/* Determine travel direction */
	for (i = 0; i < 8; ++ i) {
		int dir_cost = travel.cost[py+ddy_ddd[i]][px+ddx_ddd[i]];

		if (dir_cost < cost)
		{
			new_dir = ddd[i];
			cost = dir_cost;
		}
	}

	if (!new_dir) return (0);

	/* Access newly move grid */
	c_ptr = &cave[py+ddy[new_dir]][px+ddx[new_dir]];

	/* Close door abort traveling */
	if (!easy_open && is_closed_door(c_ptr->feat)) return (0);

	/* Visible and unignorable trap abort tarveling */
	if (!c_ptr->mimic && !trap_can_be_ignored(c_ptr->feat)) return (0);

	/* Move new grid */
	return (new_dir);
}


/*
 * Travel command
 */
/*:::�g���x���R�}���h�ɂ��ړ�*/
//travel.run�ɒl�����鎞�Ă΂��
//travel_test()�ŖړI�n�ɋ߂��ׂ̃O���b�h�����肵�����ֈړ���travel.run��1���炷
void travel_step(void)
{
	/* Get travel direction */
	travel.dir = travel_test(travel.dir);

	/* disturb */
	if (!travel.dir)
	{
		if (travel.run == 255)
		{
#ifdef JP
			msg_print("���؂�������܂���I");
#else
			msg_print("No route is found!");
#endif
		}
		disturb(0, 1);
		return;
	}

	energy_use = 100;

	//v2.0.3 �g���x���R�}���h��energy_use�l�ɍ����ړ��Ȃǂ�K�p����
	walk_energy_modify();

	move_player(travel.dir, always_pickup, FALSE,FALSE);

	if ((py == travel.y) && (px == travel.x))
		travel.run = 0;
	else if (travel.run > 0)
		travel.run--;

	/* Travel Delay */
	/*:::Term->xtra_hook���ݒ肳��Ă���΂�������s����*/
	Term_xtra(TERM_XTRA_DELAY, delay_factor);
}
#endif

///mod131226 skill �Z�\�ƕ���Z�\�̓����̂��߂Ɋ֐��ǉ�
///mod140130 ����A�Z�\�o���l�@�v�Z������
/*:::�i���A���A��n�A�񓁁A�����A����o���l*/
/*:::skill_num�ɂ͊i��0,��n1,��2,��3,����4,5-9�\���A����̏ꍇTVAL�l������10(knife)�`20(gun)�ɕϊ������*/
/*:::m_ptr�͂��̊֐����Ă΂��Ƃ��U������/���ꂽ����*/
void gain_skill_exp(int skill_num , monster_type* m_ptr)
{

	int n = skill_num;
	int amount;
	int old_skill_lev, new_skill_lev, max_skill_lev;
	cptr skill_desc;
	monster_race* r_ptr = &r_info[m_ptr->r_idx];
	int ridinglevel = r_info[m_list[p_ptr->riding].r_idx].level;
	int targetlevel = r_ptr->level;
	int skill_max;

	//v1.1.41 ���Ɨ��T�̓���R��͏n���x������Ȃ�
	if (CLASS_RIDING_BACKDANCE && skill_num == SKILL_RIDING) return;


	/*:::����TVAL(23�`)��Ή�����Z�\�l�z�񍀖�(10�`)�ɕϊ�����*/
	if(skill_num >= TV_KNIFE) n = skill_num -= SKILL_WEAPON_SHIFT;

	/*:::�E�Ƃ��Ƃ̂��̋Z�\�̍ő�l�𓾂� �K���l*1600 */
	skill_max = cp_ptr->skill_aptitude[n] * SKILL_LEV_TICK * 10;
	max_skill_lev = cp_ptr->skill_aptitude[n] * 10;
	old_skill_lev = p_ptr->skill_exp[n] / SKILL_LEV_TICK;

	if(n==0) skill_desc = "�i��";
	else if(n==1) skill_desc = "��";
	else if(n==2) skill_desc = "��n";
	else if(n==3) skill_desc = "�񓁗�";
	else if(n==4) skill_desc = "����";
	else if(n==10) skill_desc = "�Z��";
	else if(n==11) skill_desc = "����";
	else if(n==12) skill_desc = "��";
	else if(n==13) skill_desc = "�݊�";
	else if(n==14) skill_desc = "�_";
	else if(n==15) skill_desc = "��";
	else if(n==16) skill_desc = "��";
	else if(n==17) skill_desc = "��������";
	else if(n==18) skill_desc = "�|";
	else if(n==19) skill_desc = "�N���X�{�E";
	else if(n==20) skill_desc = "�e";
	else if(n>=21&&n<=23) return; /*MAGICBOOK/MAGICWEAPON/OTHERWEAPON�ɂ͏n���x���Ȃ�*/
	else{
		msg_format("ERROR:gain_skill_exp()�����ݎg���Ă��Ȃ��Z�\(%d)�ŌĂ΂ꂽ�B",skill_num);
		return;
	}

	//���ɏ���̏ꍇ�I��
	if(p_ptr->skill_exp[n] >= skill_max) return;


	//msg_format("TEST:RIDINGLEV:%d",ridinglevel);

	/*:::������Z�\�o���l���v�Z�@�L�т��낪�傫���Əオ��₷��*/
	if(max_skill_lev - old_skill_lev > 30) amount = 20;
	else if(max_skill_lev - old_skill_lev > 20) amount = 10;
	else if(max_skill_lev - old_skill_lev > 10) amount = 5;
	else amount = 1;
	
	/*:::�Z�\���Ⴂ�Əオ��₷��*/
	if(old_skill_lev < 10) amount += 4;
	else if(old_skill_lev < 20) amount += 3;
	else if(old_skill_lev < 30) amount += 2;
	else if(old_skill_lev < 40) amount += 1;

	//v1.1.94 �e�͏n���x���オ��ɂ����Ƃ悭�����̂ł�����ƒ������悤
	if (skill_num == TV_GUN) amount += randint1(amount);

	/*:::�^�[�Q�b�g�⁗���ア�Əオ��ɂ���*/
	if((targetlevel < old_skill_lev / 4) ) amount /= 10;
	else if((targetlevel < old_skill_lev / 2) ) amount /= 5;
	else if((targetlevel < old_skill_lev) && one_in_(2)) amount /= 2;
	if((p_ptr->lev < old_skill_lev ) && one_in_(2)) amount /= 2;

	/*:::�R�惂���X�^�[���ア�ƋR��Z�\�͏オ��ɂ���*/
	if(n == SKILL_RIDING && (ridinglevel < old_skill_lev) && one_in_(2)) amount /= 2;
	/*:::���͏����オ��ɂ���*/
	if(n == SKILL_SHIELD && one_in_(2)) amount /= 2;

	/*:::�y�b�g�ɂ��Z�\�l�グ��}�~*/
	if(is_pet(m_ptr) && p_ptr->skill_exp[n] > 4000)
	{
		if(one_in_(10)) msg_format("�����z��������ł�%s�̗��K�ɂ͂Ȃ�Ȃ��B",skill_desc);
		amount = 0;
	}
	/*:::��ɑ��B�����X�^�[�ɂ��Z�\�l�グ��}�~*/
	if(r_ptr->r_akills > 100 && targetlevel < old_skill_lev)
	{
		if(one_in_(10)) msg_format("�������̓G������ł�%s�̗��K�ɂ͂Ȃ�Ȃ��B",skill_desc);
		amount = 0;
	}
	//msg_format("GAIN_SKILL:(n:%d)%s:amount:%d",n,skill_desc,amount);
	
	p_ptr->skill_exp[n] += amount;
	/*::: �������*/
	if(p_ptr->skill_exp[n] > skill_max) p_ptr->skill_exp[n] = skill_max;
	new_skill_lev = p_ptr->skill_exp[n] / SKILL_LEV_TICK;
	if(new_skill_lev > old_skill_lev){
		msg_format("%s�̋Z�\���x����%d�ɃA�b�v����!", skill_desc,new_skill_lev);
	}
	p_ptr->update |= (PU_BONUS);

}


/*:::����A�Z�\�̌o���l���Q�Ƃ���*/
s16b ref_skill_exp(int skill_num){
	int n = skill_num;
	if(skill_num >= TV_KNIFE) n = skill_num -= SKILL_WEAPON_SHIFT;

	if(n >= 5 && n <= 9 || n >= 24){
		msg_format("ERROR:ref_skill_exp()�����ݎg���Ă��Ȃ��Z�\(%d)�ŌĂ΂ꂽ�B",skill_num);
	}

	//�B��ށu�����̌|�\�ҁv���s��
	if (p_ptr->pclass == CLASS_OKINA && p_ptr->tim_general[0])
	{
		return ((p_ptr->stat_ind[A_CHR] + 3) * SKILL_LEV_TICK);
	}


	return p_ptr->skill_exp[n];
}

/*:::�q�b�g���A�E�F�C��Ɨ����[�`���ɂ����B�^�[�Q�b�g���I������Ȃ�������FALSE*/
bool hit_and_away(void)
{
	int x, y,dir;

	if (!get_rep_dir(&dir, FALSE)) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];
	if (cave[y][x].m_idx)
	{
		py_attack(y, x, 0);
		//v1.1.30 �I�[���Ȃǂœ|�ꂽ��e���|���Ȃ��悤�ɂ��Ƃ�
		if (p_ptr->is_dead || randint0(p_ptr->skill_dis) < 7)
#ifdef JP
			msg_print("���E���s�I");
#endif
		else 
		{
			msg_print("���Ȃ��͍U���㗣�E�����B");
			teleport_player(30, 0L);
		}
	}
	else
	{
#ifdef JP
		msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
		msg_print("You don't see any monster in this direction");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}




