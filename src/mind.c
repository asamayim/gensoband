/* File: mind.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Mindcrafter code */

#include "angband.h"
#include "mindtips.h"

/*:::mind_power�^��mind_type�^�̔z��@mind_type�̓��x���A�R�X�g�A���s���A���O�̏��̍\����*/
/*:::�����ɂ͒��\�͎ҁA�B�C�p�t�A����m�A�E�ҁA���g���̂��ꂼ��̋Z�̊�{��񂪊i�[����Ă���*/
mind_power mind_powers[5] =
{
  {
    {
      /* Level gained,  cost,  %fail,  name */
#ifdef JP
      { 1,   1,  15, "�쎋"},
      { 2,   1,  20, "�_�o�U��"},
      { 3,   2,  25, "�����̏u��"},
      { 7,   6,  35, "����̌��e"},
      { 9,   7,  50, "���_�x�z"},
      { 11,  7,  30, "�O���Ռ��e"},
      { 13, 12,  50, "�Z��"},
      { 15, 12,  60, "�T�C�R���g���["},
      { 18, 10,  45, "���_�g��"},
      { 23, 15,  50, "�A�h���i�����E�h�[�s���O"},
      { 26, 28,  60, "�e���L�l�V�X"},
      { 28, 10,  40, "�T�C�L�b�N�E�h���C��"},
      { 35, 35,  75, "���̌�"},
      { 45,150,  85, "���S�Ȑ��E"},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
#else
      { 1,   1,  15, "Precognition"},
      { 2,   1,  20, "Neural Blast"},
      { 3,   2,  25, "Minor Displacement"},
      { 7,   6,  35, "Major Displacement"},
      { 9,   7,  50, "Domination"},
      { 11,  7,  30, "Pulverise"},
      { 13, 12,  50, "Character Armour"},
      { 15, 12,  60, "Psychometry" },
      { 18, 10,  45, "Mind Wave" },
      { 23, 15,  50, "Adrenaline Channeling"},
      { 26, 28,  60, "Telekinesis"},
      { 28, 10,  40, "Psychic Drain"},
      { 35, 35,  75, "Psycho-Spear"},
      { 45,150,  85, "The World"},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
#endif
      
    }
  },
  
  {
    {
      /* Level gained,  cost,  %fail,  name */
#ifdef JP
      { 1,   1,  15, "����"},
      { 3,   3,  30, "�M��"},
      { 5,   6,  35, "����p"},
      { 8,   5,  40, "�J���n���g"},
      { 10,  7,  45, "�Ζ��@�h��"},
      { 13,  5,  60, "���C"},
      { 17, 17,  50, "�Z���C"},
      { 20, 20,  50, "�Քg"},
      { 23, 18,  55, "�a��"},
      { 25, 30,  70, "���Ă��g��"},
      { 28, 26,  50, "���쏢��"},
      { 32, 35,  65, "�����Ή�"},
      { 38, 42,  75, "���J���n���g"},
      { 44, 50,  80, "�����ړ�"},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
#else
      { 1,   1,  15, "Small Force Ball"},
      { 3,   3,  30, "Flash Light"},
      { 5,   6,  35, "Flying Technique"},
      { 8,   5,  40, "Kamehameha"},
      { 10,  7,  45, "Magic Resistance"},
      { 13,  5,  60, "Improve Force"},
      { 17, 17,  50, "Aura of Force"},
      { 20, 20,  50, "Shock Power"},
      { 23, 18,  55, "Large Force Ball"},
      { 25, 30,  70, "Dispel Magic"},
      { 28, 26,  50, "Summon Ghost"},
      { 32, 35,  65, "Exploding Frame"},
      { 38, 42,  75, "Super Kamehameha"},
      { 44, 50,  80, "Light Speed"},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
#endif
      
    }
  },
  
  {
    {
      /* Level gained,  cost,  %fail,  name */
#ifdef JP
      {  8,  5,  40, "�E�C���m"},
      { 15, 20,   0, "�ˌ�"},
      { 20, 15,   0, "�g���b�v����"},
      { 25, 20,  60, "�n�k"},
      { 30, 80,  75, "�F�E��"},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
#else
      {  8,  5,  40, "Detect Atmosphere of Menace"},
      { 15, 20,   0, "Charge"},
      { 20, 15,   0, "Smash a Trap"},
      { 25, 20,  60, "Quake"},
      { 30, 80,  75, "Massacre"},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
      { 99,  0,   0, ""},
#endif
      
    }
  },

  {
    {
      /* Level gained,  cost,  %fail,  name */
#ifdef JP
      { 1,   1,  15, "�^���̋�"},
      { 1,   2,  40, "������"},
      { 2,   2,  20, "���̂�����"},
      { 3,   2,  20, "�c�񂾋�"},
      { 5,   3,  35, "�M����"},
      { 6,   5,  35, "�f���鋾"},

      { 10,  5,  30, "���o�B��"},
      { 12, 12,  30, "�Ǖ��̋�"},
      { 15, 15,  30, "���ӂ�"},
      { 19, 13,  30, "�Ö���"},
      { 23, 18,  50, "�V�[�J�[���C"},

      { 25, 20,  40, "���̕���"},
      { 27, 30,  60, "�����̏�"},
      { 29, 30,  60, "�X�[�p�[���C"},
      { 31, 35,  60, "���f�̌�"},
      { 33, 50,  80, "���̍�"},

      { 36, 30,  80, "������"},
      { 38, 40,  70, "�A�҂̋�"},
      { 40, 50,  55, "�e���g"},
      { 43, 55,  70, "�������E"},
      { 46, 70,  75, "���t�m�[���̋�"},
#else
      { 1,   1,  15, "Mirror of Seeing"},
      { 1,   2,  40, "Making a Mirror"},
      { 2,   2,  20, "Drip of Light"},
      { 3,   2,  20, "Warped Mirror"},
      { 5,   3,  35, "Mirror of Light"},
      { 6,   5,  35, "Mirror of Wandering"},

      { 10,  5,  30, "Robe of Dust"},
      { 12, 12,  30, "Banishing Mirror"},
      { 15, 15,  30, "Mirror Clashing"},
      { 19, 13,  30, "Mirror Sleeping"},
      { 23, 18,  50, "Seeker Ray"},

      { 25, 20,  40, "Seal of Mirror"},
      { 27, 30,  60, "Shield of Water"},
      { 29, 30,  60, "Super Ray"},
      { 31, 35,  60, "Illusion Light"},
      { 33, 50,  80, "Mirror Shift"},

      { 36, 30,  80, "Mirror Tunnel"},
      { 38, 40,  70, "Mirror of Recall"},
      { 40, 50,  55, "Multi-Shadow"},
      { 43, 55,  70, "Binding Field"},
      { 46, 70,  75, "Mirror of Ruffnor"},
#endif
      
    }
  },
  
  {
    {
      /* Level gained,  cost,  %fail,  name */
#ifdef JP
      {  1,  1,  20, "�ÈŐ���"},
      {  2,  2,  25, "���Ӓ���"},
      {  3,  3,  25, "�t�B��"},
      {  5,  3,  30, "�ς��g"},
      {  7,  8,  35, "�����"},
      {  8, 10,  35, "�ꌂ���E"},
      { 10, 10,  40, "������"},
      { 12, 12,  70, "�Â̌��`"},
      { 15, 10,  50, "���_"},
      { 17, 12,  45, "�Γ�"},
      { 18, 20,  40, "���g"},
      { 20,  5,  50, "�����藠��"},
      { 22, 15,  55, "����"},
      { 25, 32,  60, "����"},
      { 28, 32,  60, "�]�g"},
      { 30, 30,  70, "�����̖��"},
      { 32, 40,  40, "�y��"},
      { 34, 35,  50, "���B��"},
      { 38, 40,  60, "�����Ή�"},
      { 41, 50,  55, "���g"},
      { 99,  0,   0, ""},
#else
      {  1,  1,  20, "Create Darkness"},
      {  2,  2,  25, "Detect Near"},
      {  3,  3,  25, "Hide in Leafs"},
      {  5,  3,  30, "Kawarimi"},
      {  7,  8,  35, "Absconding"},
      {  8, 10,  35, "Hit and Away"},
      { 10, 10,  40, "Bind Monster"},
      { 12, 12,  70, "Ancient Knowledge"},
      { 15, 10,  50, "Floating"},
      { 17, 12,  45, "Hide in Flame"},
      { 18, 20,  40, "Nyusin"},
      { 20,  5,  50, "Syuriken Spreading"},
      { 22, 15,  55, "Chain Hook"},
      { 25, 32,  60, "Smoke Ball"},
      { 28, 32,  60, "Swap Position"},
      { 30, 30,  70, "Glyph of Explosion"},
      { 32, 40,  40, "Hide in Mud"},
      { 34, 35,  50, "Hide in Mist"},
      { 38, 40,  60, "Rengoku-Kaen"},
      { 41, 50,  55, "Bunshin"},
      { 99,  0,   0, ""},
#endif
      
    }
  },
};


/*:::���Z�́u���ʁv���̃R�����g�𓾂�B���\�͎ҁA�B�C�p�t�A���g���A�E�҂��Ώ�*/
///class ���Z�R�����g�@���e��do_cmd_mind()�̂ƈ�v���邱��
void mindcraft_info(char *p, int use_mind, int power)
{
#ifdef JP
	cptr s_dam = "����:";
	cptr s_dur = "����:";
	cptr s_range = "�͈�:";
#else
	cptr s_dam = "dam ";
	cptr s_dur = "dur ";
	cptr s_range = "range ";
#endif
  int plev = p_ptr->lev;

  strcpy(p, "");

  switch (use_mind)
    {
    case MIND_MINDCRAFTER:
      switch (power)
	{
	case 0:  break;
	case 1:  sprintf(p, " %s%dd%d", s_dam, 3 + ((plev - 1) / 4), 3 + plev/15); break;
	case 2:  sprintf(p, " %s10", s_range); break;
	case 3:  sprintf(p, " %s%d", s_range, plev * 5);  break;
	case 4:  break;
	case 5: sprintf(p, " %s%dd8", s_dam, 8 + ((plev - 5) / 4));  break;
	case 6:  sprintf(p, " %s%d", s_dur, plev);  break;
	case 7:  break;
	case 8:  sprintf(p, (plev < 25 ? " %s%d" : " %sd%d"), s_dam, (plev < 25 ? plev * 3 / 2 : plev * ((plev - 5) / 10 + 1))); break;
	case 9:  sprintf(p, " %s10+d%d", s_dur, plev * 3 / 2);  break;
#ifdef JP
	case 10: sprintf(p, " �ő�d��:%d.%dkg", lbtokg1(plev * 15),lbtokg2(plev * 15));  break;
#else
	case 10: sprintf(p, " max wgt %d", plev * 15);  break;
#endif
	case 11: sprintf(p, " %s%dd6", s_dam, plev / 2);  break;
	case 12: sprintf(p, " %sd%d+%d", s_dam, plev * 3, plev * 3); break;
#ifdef JP
	case 13: sprintf(p, " �s��:%ld��", (long int)(p_ptr->csp + 100-p_ptr->energy_need - 50)/100); break;
#else
	case 13: sprintf(p, " %ld acts.", (p_ptr->csp + 100-p_ptr->energy_need - 50)/100); break;
#endif
	}
      break;
    case MIND_KI:
      {
	int boost = p_ptr->magic_num1[0];

	if (heavy_armor()) boost /= 2;

	switch (power)
	  {
	  case 0:  sprintf(p, " %s%dd4", s_dam, 3 + ((plev - 1) / 5) + boost / 12); break;
	  case 1:  break;
	  case 2:  sprintf(p, " %s%d+d30", s_dur, 30 + boost / 5); break;
	  case 3:  sprintf(p, " %s%dd5", s_dam, 5 + ((plev - 1) / 5) + boost / 10); break;
	  case 4:  sprintf(p, " %s%d+d20", s_dur, 20 + boost / 5); break;
	  case 5:  break;
	  case 6:  sprintf(p, " %s%d+d%d", s_dur, 15 + boost / 7, plev / 2); break;
	  case 7:  sprintf(p, " %s%dd8", s_dam, 8 + ((plev - 5) / 5) + boost / 12); break;
	  case 8:  sprintf(p, " %s10d6+%d", s_dam, plev * 3 / 2 + boost * 3 / 5); break;
	  case 9:  break;
#ifdef JP
	  case 10: sprintf(p, " �ő�%d��", 1+boost/100); break;
#else
	  case 10: sprintf(p, " max %d", 1+boost/100); break;
#endif
	  case 11: sprintf(p, " %s%d", s_dam, 100 + plev + boost); break;
	  case 12: sprintf(p, " %s%dd15", s_dam, 10 + plev / 2 + boost * 3 / 10); break;
#ifdef JP
	  case 13: sprintf(p, " �s��:%d+d16��", 16+boost/20); break;
#else
	  case 13: sprintf(p, " %d+d16 acts", 16+boost/20); break;
#endif
	  }
	break;
      case MIND_MIRROR_MASTER:
	{
	  switch (power)
	    {
	    case 0:  break;
	    case 1:  break;
	    case 2:  sprintf(p, " %s%dd4", s_dam,  3 + ((plev - 1) / 5) ); break;
	    case 3:  sprintf(p, " %s10", s_range); break;
	    case 4:  break;
	    case 5:  sprintf(p, " %s%d", s_range, plev *5); break;
	    case 6:  sprintf(p, " %s20+d20", s_dur);  break;
	    case 7:  break;
	    case 8:  sprintf(p, " %s%dd8", s_dam, 8+((plev -5)/4) ); break;
	    case 9:  break;
	    case 10: sprintf(p, " %s%dd8", s_dam, 11+(plev-5)/4 ); break;
	    case 11: break;
	    case 12: sprintf(p, " %s20+d20", s_dur);  break;
	    case 13: sprintf(p, " %s150+d%d", s_dam, plev*2 ); break;
	    case 14: break;
	    case 15: break;
	    case 16: sprintf(p, " %s%d", s_range, plev/2 +10); break;
	    case 17: break;
	    case 18: sprintf(p, " %s6+d6", s_dur);  break;
	    case 19: sprintf(p, " %s%d", s_dam, plev*11+5 ); break;
	    case 20: sprintf(p, " %s4+d4", s_dur);  break;
	    }
	  break;
	}
      case MIND_NINJUTSU:
	{
	  switch (power)
	    {
	    case 0:  break;
	    case 1:  break;
	    case 2:  sprintf(p, " %s10", s_range); break;
	    case 3:  break;
	    case 4:  sprintf(p, " %s%d", s_range , plev *5); break;
	    case 5:  sprintf(p, " %s30", s_range); break;
	    case 6:  break;
	    case 7:  break;
	    case 8:  sprintf(p, " %s20+d20", s_dur);  break;
	    case 9:  sprintf(p, " %s%d", s_dam, (50+plev)/2 ); break;
	    case 10: break;
	    case 11: break;
	    case 12: break;
	    case 13: break;
	    case 14: break;
	    case 15: break;
	    case 16: sprintf(p, " %s%d+d%d", s_dur, plev/2, plev/2);  break;
	    case 17: sprintf(p, " %s%d*3", s_dam, (75+plev*2/3)/2 ); break;
	    case 18: sprintf(p, " %s%dd10", s_dam, 6+plev/8 ); break;
	    case 19: sprintf(p, " %s6+d6", s_dur);  break;
	    }
	  break;
	}
      }
    }
}

  /*
 * Allow user to choose a mindcrafter power.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 *
 * nb: This function has a (trivial) display bug which will be obvious
 * when you run it. It's probably easy to fix but I haven't tried,
 * sorry.
 */
/*:::���Z�̈ꗗ��\�������̒�����I������@���\�͎ҁA�B�C�p�t�A����m�A�E�ҁA���g��*/
/*:::sn:�I�����ꂽ�ԍ�*/
/*:::only_browse:b�R�}���h�Ő�������ǂނƂ�TRUE*/
///class
  static int get_mind_power(int *sn, bool only_browse)
    {
      int             i;
      int             num = 0;
      int             y = 1;
      int             x = 10;
      int             minfail = 0;
      int             plev = p_ptr->lev;
      int             chance = 0;
      int             ask = TRUE;
      char            choice;
      char            out_val[160];
      char            comment[80];
      cptr            p;

      mind_type       spell;
      mind_power      *mind_ptr;
      bool            flag, redraw;
      int             use_mind;
      int menu_line = (use_menu ? 1 : 0);

      switch(p_ptr->pclass)
	{
	case CLASS_MINDCRAFTER:
	case CLASS_SUMIREKO:
	  {
		  /*:::MIND_�͒��\��0�`�E�p4*/
	    use_mind = MIND_MINDCRAFTER;
#ifdef JP
	    p = "���\��";
#else
	    p = "mindcraft";
#endif
	    break;
	  }
	case CLASS_FORCETRAINER:
	  {
	    use_mind = MIND_KI;
#ifdef JP
	    p = "���C�p";
#else
	    p = "Force";
#endif
	    break;
	  }
	  /*
	case CLASS_BERSERKER:
	  {
	    use_mind = MIND_BERSERKER;
#ifdef JP
	    p = "�Z";
#else
	    p = "brutal power";
#endif
	    break;
	  }
	  */
	case CLASS_MIRROR_MASTER:
	  {
	    use_mind = MIND_MIRROR_MASTER;
#ifdef JP
	    p = "�����@";
#else
	    p = "magic";
#endif
	    break;
	  }
	case CLASS_NINJA:
	  {
	    use_mind = MIND_NINJUTSU;
#ifdef JP
	    p = "�E�p";
#else
	    p = "ninjutsu";
#endif
	    break;
	  }
	default:
	  {
	    use_mind = 0;
#ifdef JP
	    p = "���\��";
#else
	    p = "mindcraft";
#endif
	    break;
	  }
	}
	  /*:::�e�E�Ƃ̓��Z�̊�b���ꗗ�������œ���*/
	  /*:::mind_power�^��mind_type�^�̔z��@mind_type�̓��x���A�R�X�g�A���s���A���O�̏��̍\����*/
      mind_ptr = &mind_powers[use_mind];

	/* Assume cancelled */
      *sn = (-1);

#ifdef ALLOW_REPEAT /* TNB */

	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Hack -- If requested INVEN_FORCE(1111), pull again */
		if (*sn == INVEN_FORCE) repeat_pull(sn);

		/* Verify the spell */
		if (mind_ptr->info[*sn].min_lev <= plev)
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

      /* Nothing chosen yet */
      flag = FALSE;

      /* No redraw yet */
      redraw = FALSE;

      for (i = 0; i < MAX_MIND_POWERS; i++)
	{
	  if (mind_ptr->info[i].min_lev <= plev)
	    {
	      num++;
	    }
	}

      /* Build a prompt (accept all spells) */
      if (only_browse)
	{
#ifdef JP
	  (void) strnfmt(out_val, 78, "(%^s %c-%c, '*'�ňꗗ, ESC) �ǂ�%s�ɂ��Ēm��܂����H",
#else
	  (void) strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
#endif
				       p, I2A(0), I2A(num - 1), p);
	  }
	else
	  {
#ifdef JP
(void) strnfmt(out_val, 78, "(%^s %c-%c, '*'�ňꗗ, ESC) �ǂ�%s���g���܂����H",
#else
		(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
#endif
		p, I2A(0), I2A(num - 1), p);
	}

	if (use_menu && !only_browse) screen_save();
	/* Get a spell from the user */

	choice= (always_show_list || use_menu) ? ESCAPE:1 ;
	while (!flag)
	{
		if(choice==ESCAPE) choice = ' '; 
		else if( !get_com(out_val, &choice, TRUE) )break;
		/*:::���j���[�\���p*/
		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					if (!only_browse) screen_load();
					return (FALSE);
				}

				case '8':
				case 'k':
				case 'K':
				{
					menu_line += (num - 1);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					menu_line++;
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				case '\n':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
			if (menu_line > num) menu_line -= num;
		}
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				char psi_desc[80];
				bool has_weapon[2];

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!only_browse && !use_menu) screen_save();

				/* Display a list of spells */
				prt("", y, x);
#ifdef JP
put_str("���O", y, x + 5);
#else
				put_str("Name", y, x + 5);
#endif

#ifdef JP
put_str(format("Lv   %s   ���� ����", ((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU)) ? "HP" : "MP"), y, x + 35);
#else
put_str(format("Lv   %s   Fail Info", ((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU)) ? "HP" : "MP"), y, x + 35);
#endif
				has_weapon[0] = buki_motteruka(INVEN_RARM);
				has_weapon[1] = buki_motteruka(INVEN_LARM);

				/* Dump the spells */
				/*:::�Z�Ȃǂ̈ꗗ��\��*/
				for (i = 0; i < MAX_MIND_POWERS; i++)
				{
					int mana_cost;

					/* Access the spell */
					spell = mind_ptr->info[i];
					/*:::���x�����s�����Ă���Z�͕\�����Ȃ�*/
					if (spell.min_lev > plev)   break;

					chance = spell.fail;

					mana_cost = spell.mana_cost;
					/*:::���s���v�Z�@���s���ݒ�0�̋Z�̓p�X*/
					
					if (chance)
					{

						/* Reduce failure rate by "effective" level adjustment */
						chance -= 3 * (plev - spell.min_lev);

						/* Reduce failure rate by INT/WIS adjustment */
						/*:::player_magic.spell_stat�ɋL���ꂽ�p�����[�^�ɂ�鎸�s������*/
						chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]] - 1);

						if (use_mind == MIND_KI)
						{
							if (heavy_armor()) chance += 20;
							if (p_ptr->icky_wield[0]) chance += 20;
							else if (has_weapon[0]) chance += 10;
							if (p_ptr->icky_wield[1]) chance += 20;
							else if (has_weapon[1]) chance += 10;
							if (i == 5)
							{
								int j;
								for (j = 0; j < p_ptr->magic_num1[0] / 50; j++)
									mana_cost += (j+1) * 3 / 2;
							}
						}

						/* Not enough mana to cast */
						if ((use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU) && (mana_cost > p_ptr->csp))
						{
							chance += 5 * (mana_cost - p_ptr->csp);
						}

						chance += p_ptr->to_m_chance;

						/* Extract the minimum failure rate */
						minfail = adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]];

						/* Minimum failure rate */
						if (chance < minfail) chance = minfail;

						/* Stunning makes spells harder */
						if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
						else if (p_ptr->stun) chance += 15;

						if (use_mind == MIND_KI)
						{
							if (heavy_armor()) chance += 5;
							if (p_ptr->icky_wield[0]) chance += 5;
							if (p_ptr->icky_wield[1]) chance += 5;
						}
						/* Always a 5 percent chance of working */
						if (chance > 95) chance = 95;
					}

					/* Get info */
					/*:::���ʗ��̃R�����g��\��*/
					mindcraft_info(comment, use_mind, i);
					/*:::�J�[�\���܂��̓A���t�@�x�b�g�̕\��*/
					if (use_menu)
					{
#ifdef JP
						if (i == (menu_line-1)) strcpy(psi_desc, "  �t ");
#else
						if (i == (menu_line-1)) strcpy(psi_desc, "  >  ");
#endif
						else strcpy(psi_desc, "     ");
					}
					else
						sprintf(psi_desc, "  %c) ",I2A(i));
					/* Dump the spell --(-- */
					strcat(psi_desc,
					       format("%-30s%2d %4d%s %3d%%%s",
						      spell.name, spell.min_lev, mana_cost,
#ifdef JP
						      (((use_mind == MIND_MINDCRAFTER) && (i == 13)) ? "�`" : "  "), 
#else
						      (((use_mind == MIND_MINDCRAFTER) && (i == 13)) ? "~ " : "  "), 
#endif
						      chance, comment));
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			/* Hide the list */
			else if (!only_browse)
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
			/* Note verify */
			ask = isupper(choice);

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = mind_ptr->info[i];

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
#ifdef JP
(void) strnfmt(tmp_val, 78, "%s���g���܂����H", spell.name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", spell.name);
#endif


			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw && !only_browse) screen_load();

	/* Show choices */
	p_ptr->window |= (PW_SPELL);

	/* Window stuff */
	window_stuff();

	/* Abort if needed */
	if (!flag) return (FALSE);

	/* Save the choice */
	(*sn) = i;

#ifdef ALLOW_REPEAT /* TNB */

	repeat_push(*sn);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
/*:::���\�͎҂̋Z�g�p ��������ς�*/
///class
static bool cast_mindcrafter_spell(int spell)
{
	int             b = 0;
	int             dir;
	int             plev = p_ptr->lev;

	/* spell code */
	switch (spell)
	{
		/*:::�쎋*/
	case 0:   /* Precog */
		if (plev > 44)
		{
			///virtue
			chg_virtue(V_KNOWLEDGE, 1);
			chg_virtue(V_ENLIGHTEN, 1);
			/*:::�痢��*/
			wiz_lite(FALSE);
		}
		else if (plev > 19)
			map_area(DETECT_RAD_MAP);

		if (plev < 30)
		{
			/*:::b�ɂ͉����댯�Ȃ��̂����������Ƃ�1������炵��*/
			b = detect_monsters_normal(DETECT_RAD_DEFAULT);
			if (plev > 14) b |= detect_monsters_invis(DETECT_RAD_DEFAULT);
			if (plev > 4)  {
				b |= detect_traps(DETECT_RAD_DEFAULT, TRUE);
				b |= detect_doors(DETECT_RAD_DEFAULT);
			}
		}
		else
		{
			/*:::�S���m*/
			b = detect_all(DETECT_RAD_DEFAULT);
		}
		if(p_ptr->pclass == CLASS_MINDCRAFTER)
		{
			if ((plev > 24) && (plev < 40))	set_tim_esp(plev, FALSE);
		}
		else
		{
			if ((plev > 24))	set_tim_esp(plev, FALSE);
		}

#ifdef JP
if (!b) msg_print("���S�ȋC������B");
#else
		if (!b) msg_print("You feel safe.");
#endif

		break;
	case 1:
		/* Mindblast */
		if (!get_aim_dir(&dir)) return FALSE;

		if (randint1(100) < plev * 2)
			fire_beam(GF_PSI, dir, damroll(3 + ((plev - 1) / 4), (3 + plev / 15)));
		else
			fire_ball(GF_PSI, dir, damroll(3 + ((plev - 1) / 4), (3 + plev / 15)), 0);
		break;
	case 2:
		/* Minor displace */
		teleport_player(10, 0L);
		break;
	case 3:
		/* Major displace */
		teleport_player(plev * 5, 0L);
		break;
	case 4:
		/* Domination */
		if (plev < 30)
		{
			if (!get_aim_dir(&dir)) return FALSE;

			fire_ball(GF_DOMINATION, dir, plev, 0);
		}
		else
		{
			charm_monsters(plev * 2);
		}
		break;
	case 5:
		/* Fist of Force  ---  not 'true' TK  */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_TELEKINESIS, dir, damroll(8 + ((plev - 5) / 4), 8),
			(plev > 20 ? (plev - 20) / 8 + 1 : 0));
		break;
	case 6:
		/* Character Armour */
		set_shield(plev, FALSE);
		if (plev > 14) set_oppose_acid(plev, FALSE);
		if (plev > 19) set_oppose_fire(plev, FALSE);
		if (plev > 24) set_oppose_cold(plev, FALSE);
		if (plev > 29) set_oppose_elec(plev, FALSE);
		if (plev > 34) set_oppose_pois(plev, FALSE);
		break;
	case 7:
		/* Psychometry */
		if (plev < 25)
			return psychometry();
		else
			return ident_spell(FALSE);
	case 8:
		/* Mindwave */
#ifdef JP
msg_print("���_��P���Ȃ���g���𔭐��������I");
#else
		msg_print("Mind-warping forces emanate from your brain!");
#endif

		if (plev < 25)
			project(0, 2 + plev / 10, py, px,
			(plev * 3), GF_PSI, PROJECT_KILL, -1);
		else
			/*:::���E�����_�U��*/
			(void)mindblast_monsters(randint1(plev * ((plev - 5) / 10 + 1)));
		break;
	case 9:
		/* Adrenaline */
		set_afraid(0);
		set_stun(0);

		/*
		 * Only heal when Adrenalin Channeling is not active. We check
		 * that by checking if the player isn't fast and 'heroed' atm.
		 */
		if (!IS_FAST() || !IS_HERO())
		{
			hp_player(plev);
		}

		b = 10 + randint1((plev * 3) / 2);
		set_hero(b, FALSE);
		/* Haste */
		(void)set_fast(b, FALSE);
		break;
	case 10:
		/* Telekinesis */
		if (!get_aim_dir(&dir)) return FALSE;

		fetch(dir, plev * 15, FALSE);

		break;
	case 11:
		/* Psychic Drain */
		if (!get_aim_dir(&dir)) return FALSE;

		b = damroll(plev / 2, 6);

		/* This is always a radius-0 ball now */
		if (fire_ball(GF_PSI_DRAIN, dir, b, 0))
			p_ptr->energy_need += randint1(150);
		break;
	case 12:
		/* psycho-spear */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_beam(GF_PSY_SPEAR, dir, randint1(plev*3)+plev*3);
		break;
		/*:::�U�E���[���h*/
	case 13:
	{
		if (world_player)
		{
#ifdef JP
			msg_print("���Ɏ��͎~�܂��Ă���B");
#else
			msg_print("Time is already stopped.");
#endif
			return (FALSE);
		}
		world_player = TRUE;
#ifdef JP
		msg_print("�u����I�v");
#else
		msg_print("You yell 'Time!'");
#endif
		msg_print(NULL);
		//v1.1.58
		flag_update_floor_music = TRUE;
		/* Hack */
		p_ptr->energy_need -= 1000 + (100 + p_ptr->csp - 50)*TURNS_PER_TICK/10;

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Update monsters */
		p_ptr->update |= (PU_MONSTERS);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		handle_stuff();
		break;
	}
	default:
#ifdef JP
msg_print("�ȂɁH");
#else
		msg_print("Zap?");
#endif

	}

	return TRUE;
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'ForceTrainer'.
 */
/*:::�B�C�p�t�̋Z�@magic_num1[0]�ɋC�̗��܂����ʂ��i�[���Ă���*/
static bool cast_force_spell(int spell)
{
	int             dir;
	int             plev = p_ptr->lev;
	int             boost = p_ptr->magic_num1[0];

	if (heavy_armor()) boost /= 2;

	/* spell code */
	switch (spell)
	{
	case 0:
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(GF_MISSILE, dir, damroll(3 + ((plev - 1) / 5) + boost / 12, 4), 0);
		break;
	case 1:
		(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
		break;
	case 2:
		set_tim_levitation(randint1(30) + 30 + boost / 5, FALSE);
		break;
	case 3:
		project_length = plev / 8 + 3;
		if (!get_aim_dir(&dir)) return FALSE;

		fire_beam(GF_MISSILE, dir, damroll(5 + ((plev - 1) / 5) + boost / 10, 5));
		break;
	case 4:
		set_resist_magic(randint1(20) + 20 + boost / 5, FALSE);
		break;
	case 5:
#ifdef JP
		msg_print("�C��������B");
#else
		msg_print("You improved the Force.");
#endif
		p_ptr->magic_num1[0] += (70 + plev);
		p_ptr->update |= (PU_BONUS);
		if (randint1(p_ptr->magic_num1[0]) > (plev * 4 + 120))
		{
#ifdef JP
			msg_print("�C���\�������I");
#else
			msg_print("The Force exploded!");
#endif
			fire_ball(GF_MANA, 0, p_ptr->magic_num1[0] / 2, 10);
#ifdef JP
			take_hit(DAMAGE_LOSELIFE, p_ptr->magic_num1[0] / 2, "�C�̖\��", -1);
#else
			take_hit(DAMAGE_LOSELIFE, p_ptr->magic_num1[0] / 2, "Explosion of the Force", -1);
#endif
		}
		else return TRUE;
		break;
	case 6:
		/*:::�Z���C*/
		set_tim_sh_touki(randint1(plev / 2) + 15 + boost / 7, FALSE);
		break;
		/*:::�Քg�@�G�𐁂���΂�����*/
	case 7:
	{
		int y, x, dam;
		project_length = 1;
		if (!get_aim_dir(&dir)) return FALSE;

		y = py + ddy[dir];
		x = px + ddx[dir];
		dam = damroll(8 + ((plev - 5) / 4) + boost / 12, 8);
		fire_beam(GF_MISSILE, dir, dam);
		if (cave[y][x].m_idx)
		{
			int i;
			int ty = y, tx = x;
			int oy = y, ox = x;
			int m_idx = cave[y][x].m_idx;
			monster_type *m_ptr = &m_list[m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			char m_name[80];

			monster_desc(m_name, m_ptr, 0);

			if (randint1(r_ptr->level * 3 / 2) > randint0(dam / 2) + dam/2)
			{
#ifdef JP
				msg_format("%s�͔�΂���Ȃ������B", m_name);
#else
				msg_format("%^s was not blown away.", m_name);
#endif
			}
			else
			{
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

					if (r_ptr->flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
						p_ptr->update |= (PU_MON_LITE);
				}
			}
		}
		break;
	}
	case 8:
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(GF_MISSILE, dir, damroll(10, 6) + plev * 3 / 2 + boost * 3 / 5, (plev < 30) ? 2 : 3);
		break;
	case 9:
	{
		int m_idx;

		if (!target_set(TARGET_KILL)) return FALSE;
		m_idx = cave[target_row][target_col].m_idx;
		if (!m_idx) break;
		if (!player_has_los_bold(target_row, target_col)) break;
		if (!projectable(py, px, target_row, target_col)) break;
		dispel_monster_status(m_idx);
		break;
	}
	case 10:
	{
		int i;
		bool success = FALSE;

		for (i = 0; i < 1 + boost/100; i++)
			if (summon_specific(-1, py, px, plev, SUMMON_PHANTOM, PM_FORCE_PET))
				success = TRUE;
		if (success)
		{
#ifdef JP
msg_print("��p�ł������܂����A���l�l�H");
#else
			msg_print("'Your wish, master?'");
#endif
		}
		else
		{
#ifdef JP
			msg_print("��������Ȃ������B");
#else
			msg_print("Nothing happen.");
#endif
		}
		break;
	}
	case 11:
		fire_ball(GF_FIRE, 0, 200 + (2 * plev) + boost * 2, 10);
		break;
	case 12:
		if (!get_aim_dir(&dir)) return FALSE;

		fire_beam(GF_MANA, dir, damroll(10 + (plev / 2) + boost * 3 / 10, 15));
		break;
	case 13:
		set_lightspeed(randint1(16) + 16 + boost / 20, FALSE);
		break;
	default:
#ifdef JP
msg_print("�ȂɁH");
#else
		msg_print("Zap?");
#endif

	}
	p_ptr->magic_num1[0] = 0;
	p_ptr->update |= (PU_BONUS);

	return TRUE;
}

/* by henkma */
/* calculate mirrors */
static int number_of_mirrors( void )
{
  int x,y;
  int val=0;
  for( x=0 ; x < cur_wid ; x++ ){
    for( y=0 ; y < cur_hgt ; y++ ){
      if (is_mirror_grid(&cave[y][x])) val++;
    }
  }
  return val;
}

static bool cast_mirror_spell(int spell)
{
	int             dir;
	int             plev = p_ptr->lev;
	int		tmp;
	int		x,y;

	/* spell code */
	switch (spell)
	{
	/* mirror of seeing */
	case 0:
	  tmp = is_mirror_grid(&cave[py][px]) ? 4 : 0;
	  if( plev + tmp > 4)detect_monsters_normal(DETECT_RAD_DEFAULT);
	  if( plev + tmp > 18 )detect_monsters_invis(DETECT_RAD_DEFAULT);
	  if( plev + tmp > 28 )set_tim_esp(plev,FALSE);
	  if( plev + tmp > 38 )map_area(DETECT_RAD_MAP);
	  if( tmp == 0 && plev < 5 ){
#ifdef JP
	    msg_print("�����Ȃ��ďW���ł��Ȃ������I");
#else
	    msg_print("You need a mirror to concentrate!");
#endif
	  }
	  break;
	/* drip of light */
	case 1:
	  if( number_of_mirrors() < 4 + plev/10 ){
	    place_mirror();
	  }
	  else {
#ifdef JP
msg_format("����ȏ㋾�͐���ł��Ȃ��I");
#else
msg_format("There are too many mirrors to control!");
#endif
	  }
	  break;
	case 2:
	  if (!get_aim_dir(&dir)) return FALSE;
	  if ( plev > 9 && is_mirror_grid(&cave[py][px]) ) {
	    fire_beam(GF_LITE, dir,damroll(3+((plev-1)/5),4));
	  }
	  else {
	    fire_bolt(GF_LITE, dir,damroll(3+((plev-1)/5),4));
	  }
	  break;
	/* warped mirror */
	case 3:
	  teleport_player(10, 0L);
	  break;
	/* mirror of light */
	case 4:
	  (void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
	  break;
	/* mirror of wandering */
	case 5:
	  teleport_player(plev * 5, 0L);
	  break;
	/* robe of dust */
	case 6:
	  set_dustrobe(20+randint1(20),FALSE);
	  break;
	/* banishing mirror */
	case 7:
	  if (!get_aim_dir(&dir)) return FALSE;
	  (void)fire_beam(GF_AWAY_ALL, dir , plev);
	  break;
	/* mirror clashing */
	case 8:
	  if (!get_aim_dir(&dir)) return FALSE;
	  fire_ball(GF_SHARDS, dir, damroll(8 + ((plev - 5) / 4), 8),
		    (plev > 20 ? (plev - 20) / 8 + 1 : 0));
	  break;
	/* mirror sleeping */
	case 9:
	  for(x=0;x<cur_wid;x++){
	    for(y=0;y<cur_hgt;y++){
	      if (is_mirror_grid(&cave[y][x])) {
		project(0,2,y,x,plev,GF_OLD_SLEEP,(PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP|PROJECT_NO_HANGEKI),-1);
	      }
	    }
	  }
	  break;
	/* seeker ray */
	case 10:
	  if (!get_aim_dir(&dir)) return FALSE;
	  fire_beam(GF_SEEKER,dir, damroll(11+(plev-5)/4,8));
	  break;
	/* seal of mirror */
	case 11:
	  seal_of_mirror(plev*4+100);
	  break;
	/* shield of water */
	case 12:
	  tmp = 20+randint1(20);
	  set_shield(tmp, FALSE);
	  if( plev > 31 )set_tim_reflect(tmp, FALSE);
	  if( plev > 39 )set_resist_magic(tmp,FALSE);
	  break;
	/* super ray */
	case 13:
	  if (!get_aim_dir(&dir)) return FALSE;
	  fire_beam(GF_SUPER_RAY,dir, 150+randint1(2*plev));
	  break;
	/* illusion light */
	case 14:
	  tmp = is_mirror_grid(&cave[py][px]) ? 4 : 3;
	  slow_monsters(plev);
	  stun_monsters(plev*tmp);
	  confuse_monsters(plev*tmp);
	  turn_monsters(plev*tmp);
	  stun_monsters(plev*tmp);
	  stasis_monsters(plev*tmp);
	  break;
	/* mirror shift */
	case 15:
	  if( !is_mirror_grid(&cave[py][px]) ){
#ifdef JP
		msg_print("���̍��̏ꏊ���킩��Ȃ��I");
#else
		msg_print("You cannot find out where is the world of mirror!");
#endif
		break;
	  }
	  alter_reality();
	  break;
	/* mirror tunnel */
	case 16:
#ifdef JP
	  msg_print("���̐��E��ʂ蔲���c  ");
#else
	  msg_print("Go through the world of mirror...");
#endif
	  return mirror_tunnel();

	/* mirror of recall */
	case 17:
		return word_of_recall();
	/* multi-shadow */
	case 18:
	  set_multishadow(6+randint1(6),FALSE);
	  break;
	/* binding field */
	case 19:
#ifdef JP
	  if( !binding_field(plev*11+5) )msg_print("�K���ȋ���I�ׂȂ������I");
#else
	  if( !binding_field(plev*11+5) )msg_print("You were not able to choose suitable mirrors!");
#endif
	  break;
	/* mirror of Ruffnor */
	case 20:
	  (void)set_invuln(randint1(4)+4,FALSE);
	  break;
	default:
#ifdef JP
msg_print("�ȂɁH");
#else
		msg_print("Zap?");
#endif

	}
	p_ptr->magic_num1[0] = 0;

	return TRUE;
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'berserker'.
 */
/*:::����m�̋Z*/
static bool cast_berserk_spell(int spell)
{
	int y, x;
	int dir;

	/* spell code */
	switch (spell)
	{
	case 0:
		/*:::�E�C���m*/
		detect_monsters_mind(DETECT_RAD_DEFAULT);
		break;
		/*:::�ˌ�*/
	case 1:
	{
		if (p_ptr->riding)
		{
#ifdef JP
			msg_print("��n���ɂ͖������B");
#else
			msg_print("You cannot do it when riding.");
#endif
			return FALSE;
		}

		if (!get_rep_dir2(&dir)) return FALSE;

		if (dir == 5) return FALSE;
		y = py + ddy[dir];
		x = px + ddx[dir];

		if (!cave[y][x].m_idx)
		{
#ifdef JP
			msg_print("���̕����ɂ̓����X�^�[�͂��܂���B");
#else
			msg_print("There is no monster.");
#endif
			return FALSE;
		}
/*:::�U����A�^�[�Q�b�g�����X�^�[�̋��ꏊ�����̐N���ł���n�`�Ńg���b�v���Ȃ��A
 *:::���^�[�Q�b�g�����X�^�[�̈�}�X�������̐N���ł���n�`�Ńg���b�v�������X�^�[���Ȃ��ꍇ�A
 *:::���̈ړ��������s��*/
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
		break;
	}
	/*:::�g���b�v����*/
	case 2:
	{
		if (!get_rep_dir2(&dir)) return FALSE;
		y = py + ddy[dir];
		x = px + ddx[dir];
		move_player(dir, easy_disarm, TRUE,FALSE);
		break;
	}
	/*:::�n�k*/
	case 3:
		earthquake(py, px, 8+randint0(5));
		break;
	/*:::�F�E���@����8�O���b�h�ɍU������*/
	case 4:
	{
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
			/*:::�s���̃����X�^�[���U�����邪�A�ǂ̒��ɂ���s���̃����X�^�[�͍U�����Ȃ�*/
			if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
				py_attack(y, x, 0);
		}
		break;
	}
	default:
#ifdef JP
msg_print("�ȂɁH");
#else
		msg_print("Zap?");
#endif

	}
	return TRUE;
}



/*
 * do_cmd_cast calls this function if the player's class
 * is 'ninja'.
 */
static bool cast_ninja_spell(int spell)
{
	int x, y;
	int dir;
	int plev = p_ptr->lev;

	/* spell code */
	switch (spell)
	{
	case 0:
		(void)unlite_area(0, 3);
		break;
	case 1:
		if (plev > 44)
		{
			wiz_lite(TRUE);
		}
		detect_monsters_normal(DETECT_RAD_DEFAULT);
		if (plev > 4)
		{
			detect_traps(DETECT_RAD_DEFAULT, TRUE);
			detect_doors(DETECT_RAD_DEFAULT);
			detect_stairs(DETECT_RAD_DEFAULT);
		}
		if (plev > 14)
		{
			detect_objects_normal(DETECT_RAD_DEFAULT);
		}
		break;
	case 2:
	{
		teleport_player(10, 0L);
		break;
	}
	case 3:
	{
		if (!(p_ptr->special_defense & NINJA_KAWARIMI))
		{
#ifdef JP
			msg_print("�G�̍U���ɑ΂��ĕq���ɂȂ����B");
#else
			msg_print("You are now prepare to evade any attacks.");
#endif

			p_ptr->special_defense |= NINJA_KAWARIMI;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	}
	case 4:
	{
		teleport_player(p_ptr->lev * 5, 0L);
		break;
	}
	case 5:
		/*:::�ꌂ���E�@������������͉����Z�\�ɏ�����*/
	{
		if (!get_rep_dir(&dir, FALSE)) return FALSE;
		y = py + ddy[dir];
		x = px + ddx[dir];
		if (cave[y][x].m_idx)
		{
			py_attack(y, x, 0);
			if (randint0(p_ptr->skill_dis) < 7)
#ifdef JP
msg_print("���܂��������Ȃ������B");
#else
				msg_print("You failed to run away.");
#endif
			else
			{
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
		}
		break;
	}
	case 6:
	{
		/*:::������@���ۂ̏����͐��������Ȃ̂ōU��������N����*/
		if (!get_aim_dir(&dir)) return FALSE;
		(void)stasis_monster(dir);
		break;
	}
	case 7:
		return ident_spell(FALSE);
	case 8:
		set_tim_levitation(randint1(20) + 20, FALSE);
		break;
	case 9:
		fire_ball(GF_FIRE, 0, 50+plev, plev/10+2);
		teleport_player(30, 0L);
		set_oppose_fire(plev, FALSE);
		break;
	case 10:
		/*:::���g*/
		return rush_attack(NULL, 0,0);
		/*:::�����藠��*/
	case 11:
	msg_print("�����т��Ȃ��Ȃ����B�i���̐��E����j");
	break;
#if 0
	{
		int i;
		for (i = 0; i < 8; i++)
		{
			int slot;

			for (slot = 0; slot < INVEN_PACK; slot++)
			{
				if (inventory[slot].tval == TV_SPIKE) break;
			}
			if (slot == INVEN_PACK)
			{
#ifdef JP
				if (!i) msg_print("�����т������Ă��Ȃ��B");
				else msg_print("�����т��Ȃ��Ȃ����B");
#else
				if (!i) msg_print("You have no Iron Spikes.");
				else msg_print("You have no more Iron Spikes.");
#endif
				return FALSE;
			}

			/* Gives a multiplier of 2 at first, up to 3 at 40th */
			/*:::�����̋L�q�́H�{�����x�����オ��ق�mult���オ��͂��������H*/
			do_cmd_throw_aux(1, FALSE, slot);

			energy_use = 100;
		}
		break;
	}
#endif
	/*:::����*/
	case 12:
	{
		monster_type *m_ptr;
		int m_idx;
		char m_name[80];
		int i;
		int path_n;
		u16b path_g[512];
		int ty,tx;

		if (!target_set(TARGET_KILL)) return FALSE;
		m_idx = cave[target_row][target_col].m_idx;
		if (!m_idx) break;
		if (m_idx == p_ptr->riding) break;
		if (!player_has_los_bold(target_row, target_col)) break;
		if (!projectable(py, px, target_row, target_col)) break;
		m_ptr = &m_list[m_idx];
		monster_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("%s�������߂����B", m_name);
#else
		msg_format("You pull back %s.", m_name);
#endif

		path_n = project_path(path_g, MAX_RANGE, target_row, target_col, py, px, 0);
		ty = target_row, tx = target_col;
		for (i = 1; i < path_n; i++)
		{
			int ny = GRID_Y(path_g[i]);
			int nx = GRID_X(path_g[i]);
			cave_type *c_ptr = &cave[ny][nx];

			if (in_bounds(ny, nx) && cave_empty_bold(ny, nx) &&
			    !(c_ptr->info & CAVE_OBJECT) &&
				!pattern_tile(ny, nx))
			{
				ty = ny;
				tx = nx;
			}
		}
		/* Update the old location */
		cave[target_row][target_col].m_idx = 0;

		/* Update the new location */
		cave[ty][tx].m_idx = m_idx;

		/* Move the monster */
		m_ptr->fy = ty;
		m_ptr->fx = tx;

		/* Wake the monster up */
		(void)set_monster_csleep(m_idx, 0);

		/* Update the monster (new location) */
		update_mon(m_idx, TRUE);

		/* Redraw the old grid */
		lite_spot(target_row, target_col);

		/* Redraw the new grid */
		lite_spot(ty, tx);

		if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
			p_ptr->update |= (PU_MON_LITE);

		if (m_ptr->ml)
		{
			/* Auto-Recall if possible and visible */
			if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

			/* Track a new monster */
			health_track(m_idx);
		}

		break;
	}
	case 13:
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(GF_OLD_CONF, dir, plev*3, 3);
		break;
	case 14:
		/*:::�]�g*/
		project_length = -1;
		if (!get_aim_dir(&dir))
		{
			project_length = 0;
			return FALSE;
		}
		project_length = 0;

		(void)teleport_swap(dir);
		break;
	case 15:
		explosive_rune();
		break;
	case 16:
		(void)set_kabenuke(randint1(plev/2) + plev/2, FALSE);
		set_oppose_acid(plev, FALSE);
		break;
	case 17:
		fire_ball(GF_POIS, 0, 75+plev*2/3, plev/5+2);
		fire_ball(GF_OLD_DRAIN, 0, 75+plev*2/3, plev/5+2);
		fire_ball(GF_CONFUSION, 0, 75+plev*2/3, plev/5+2);
		teleport_player(30, 0L);
		break;
	case 18:
	{
		int k;
		int num = damroll(3, 9);

		for (k = 0; k < num; k++)
		{
			int typ = one_in_(2) ? GF_FIRE : one_in_(3) ? GF_NETHER : GF_PLASMA;
			int attempts = 1000;

			while (attempts--)
			{
				scatter(&y, &x, py, px, 4, 0);

				if (!player_bold(y, x)) break;
			}
			project(0, 0, y, x, damroll(6 + plev / 8, 10), typ,
				(PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
		}
		break;
	}
	case 19:
		set_multishadow(6+randint1(6), FALSE);
		break;
	default:
#ifdef JP
msg_print("�ȂɁH");
#else
		msg_print("Zap?");
#endif

	}
	return TRUE;
}



/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
/*:::�\�͂��g���B���\�͎ҁA�B�C�p�t�A����m�A���g���A�E��*/
///class
void do_cmd_mind(void)
{
	int             n = 0,  b = 0;
	int             chance;
	int             minfail = 0;
	int             plev = p_ptr->lev;
	int             old_csp = p_ptr->csp;
	mind_type       spell;
	bool            cast;
	int             use_mind, mana_cost;
#ifdef JP
	cptr            p;
#endif
	bool		on_mirror = FALSE;

	/* not if confused */
	if (p_ptr->confused)
	{
#ifdef JP
msg_print("�������Ă��ďW���ł��Ȃ��I");
#else
		msg_print("You are too confused!");
#endif

		return;
	}

	/* get power */
	/*:::�ꗗ����Z��I�����Z�ԍ���n�Ɋi�[*/
	if (!get_mind_power(&n, FALSE)) return;

#ifdef JP
	switch(p_ptr->pclass)
	{
		case CLASS_MINDCRAFTER:
		case CLASS_SUMIREKO:
			use_mind = MIND_MINDCRAFTER;p = "���_";break;
		case CLASS_FORCETRAINER:          use_mind = MIND_KI;p = "�C";break;
		//case CLASS_BERSERKER:   use_mind = MIND_BERSERKER;p = "�{��";break;
		case CLASS_MIRROR_MASTER:   use_mind = MIND_MIRROR_MASTER;p = "�����@";break;
		case CLASS_NINJA:       use_mind = MIND_NINJUTSU;p = "���_";break;
		default:                use_mind = 0;p = "���\��";break;
	}
#else
	switch(p_ptr->pclass)
	{
		case CLASS_MINDCRAFTER: use_mind = MIND_MINDCRAFTER;break;
		case CLASS_FORCETRAINER:          use_mind = MIND_KI;break;
		case CLASS_BERSERKER:   use_mind = MIND_BERSERKER;break;
		case CLASS_MIRROR_MASTER:   use_mind = MIND_MIRROR_MASTER;break;
		case CLASS_NINJA:       use_mind = MIND_NINJUTSU;break;
		default:                use_mind = 0;break;
	}
#endif
	/*:::spell�ɋZ�����i�[*/
	spell = mind_powers[use_mind].info[n];

	/* Spell failure chance */
	chance = spell.fail;
	/*:::���s���v�Z*/
	mana_cost = spell.mana_cost;
	if (use_mind == MIND_KI)
	{
		if (heavy_armor()) chance += 20;
		if (p_ptr->icky_wield[0]) chance += 20;
		else if (buki_motteruka(INVEN_RARM)) chance += 10;
		if (p_ptr->icky_wield[1]) chance += 20;
		else if (buki_motteruka(INVEN_LARM)) chance += 10;
		if (n == 5)
		{
			int j;
			for (j = 0; j < p_ptr->magic_num1[0] / 50; j++)
				mana_cost += (j+1) * 3 / 2;
		}
	}

	/* Verify "dangerous" spells */
	if ((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU))
	{
		if (mana_cost > p_ptr->chp)
		{
#ifdef JP
msg_print("�g�o������܂���B");
#else
			msg_print("You do not have enough hp to use this power.");
#endif
			return;
		}
	}
	else if (mana_cost > p_ptr->csp)
	{
		/* Warning */
#ifdef JP
msg_print("�l�o������܂���B");
#else
		msg_print("You do not have enough mana to use this power.");
#endif


		if (!over_exert) return;

		/* Verify */
#ifdef JP
if (!get_check("����ł����킵�܂���? ")) return;
#else
		if (!get_check("Attempt it anyway? ")) return;
#endif

	}
	/*:::���s���̂Ȃ��Z�i�ꕔ����m�j����������Ɏ��s���v�Z*/
	if (chance)
	{
		/* Reduce failure rate by "effective" level adjustment */
		chance -= 3 * (plev - spell.min_lev);

		chance += p_ptr->to_m_chance;

		/* Reduce failure rate by INT/WIS adjustment */
		chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[cp_ptr->spell_stat]] - 1);

		/* Not enough mana to cast */
		if ((mana_cost > p_ptr->csp) && (use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU))
		{
			chance += 5 * (mana_cost - p_ptr->csp);
		}

		/* Extract the minimum failure rate */
		minfail = adj_mag_fail[p_ptr->stat_ind[cp_ptr->spell_stat]];

		/* Minimum failure rate */
		if (chance < minfail) chance = minfail;

		/* Stunning makes spells harder */
		if (p_ptr->stun > 50 || p_ptr->drowning) chance += 25;
		else if (p_ptr->stun) chance += 15;

		if (use_mind == MIND_KI)
		{
			if (heavy_armor()) chance += 5;
			if (p_ptr->icky_wield[0]) chance += 5;
			if (p_ptr->icky_wield[1]) chance += 5;
		}
	}

	/* Always a 5 percent chance of working */
	if (chance > 95) chance = 95;

	if(p_ptr->pseikaku == SEIKAKU_BERSERK && chance < 30) chance = 30;
	/*:::���s���v�Z�I��*/

	/* Failed spell */
	if (randint0(100) < chance)
	{
		if (flush_failure) flush();
#ifdef JP
msg_format("%s�̏W���Ɏ��s�����I",p);
#else
		msg_format("You failed to concentrate hard enough!");
#endif

		sound(SOUND_FAIL);

		if ((use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU))
		{
			if ((use_mind == MIND_KI) && (n != 5) && p_ptr->magic_num1[0])
			{
#ifdef JP
				msg_print("�C���U���Ă��܂����D�D�D");
#else
				msg_print("Your improved Force has gone away...");
#endif
				p_ptr->magic_num1[0] = 0;
			}

			if (randint1(100) < (chance / 2))
			{
				/* Backfire */
			  b = randint1(100);

			  if( use_mind == MIND_MINDCRAFTER ){
				if (b < 5)
				{
#ifdef JP
msg_print("�Ȃ�Ă������I���̒����^�����ɂȂ����I");
#else
					msg_print("Oh, no! Your mind has gone blank!");
#endif

					lose_all_info();
				}
				else if (b < 15)
				{
#ifdef JP
msg_print("��Ȍ��i���ڂ̑O�ŗx���Ă���...");
#else
					msg_print("Weird visions seem to dance before your eyes...");
#endif

					set_image(p_ptr->image + 5 + randint1(10));
				}
				else if (b < 45)
				{
#ifdef JP
msg_print("���Ȃ��̓��͍��������I");
#else
					msg_print("Your brain is addled!");
#endif

					set_confused(p_ptr->confused + randint1(8));
				}
				else if (b < 90)
				{
					set_stun(p_ptr->stun + randint1(8));
				}
				else
				{
					/* Mana storm */
#ifdef JP
msg_format("%s�̗͂�����ł��Ȃ��×��ƂȂ��ĉ�����ꂽ�I", p);
#else
					msg_print("Your mind unleashes its power in an uncontrollable storm!");
#endif

					project(PROJECT_WHO_UNCTRL_POWER, 2 + plev / 10, py, px, plev * 2,
						GF_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
					p_ptr->csp = MAX(0, p_ptr->csp - plev * MAX(1, plev / 10));
				}
			  }
			  if( use_mind == MIND_MIRROR_MASTER ){
				if (b < 51)
				{
				  /* Nothing has happen */
				}
				else if (b < 81)
				{
#ifdef JP
msg_print("���̐��E�̊����󂯂��I");
#else
					msg_print("Weird visions seem to dance before your eyes...");
#endif
					teleport_player(10, TELEPORT_PASSIVE);
				}
				else if (b < 96)
				{
#ifdef JP
msg_print("�܂��̂��̂��L���L���P���Ă���I");
#else
					msg_print("Your brain is addled!");
#endif

					set_image(p_ptr->image + 5 + randint1(10));
				}
				else
				{
					/* Mana storm */
#ifdef JP
msg_format("%s�̗͂�����ł��Ȃ��×��ƂȂ��ĉ�����ꂽ�I", p);
#else
					msg_print("Your mind unleashes its power in an uncontrollable storm!");
#endif

					project(PROJECT_WHO_UNCTRL_POWER, 2 + plev / 10, py, px, plev * 2,
						GF_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
					p_ptr->csp = MAX(0, p_ptr->csp - plev * MAX(1, plev / 10));
				}
			  }
			}
		}
	}
	/*:::���s�y�i���e�B�����I��*/

	/*:::���������ꍇ*/
	else
	{
		sound(SOUND_ZAP);

		switch(use_mind)
		{
		case MIND_MINDCRAFTER:
			/* Cast the spell */
			cast = cast_mindcrafter_spell(n);
			break;
		case MIND_KI:
			/* Cast the spell */
			cast = cast_force_spell(n);
			break;
		case MIND_BERSERKER:
			/* Cast the spell */
			cast = cast_berserk_spell(n);
			break;
		case MIND_MIRROR_MASTER:
			/* Cast the spell */
			if( is_mirror_grid(&cave[py][px]) )on_mirror = TRUE;
			cast = cast_mirror_spell(n);
			break;
		case MIND_NINJUTSU:
			/* Cast the spell */
			cast = cast_ninja_spell(n);
			break;
		default:
#ifdef JP
			msg_format("��̔\��:%d, %d",use_mind, n);
#else
			msg_format("Mystery power:%d, %d",use_mind, n);
#endif
			return;
		}

		if (!cast) return;
	}


	/* Take a turn */
	energy_use = 100;
	/* teleport from mirror costs small energy */
	if( on_mirror && p_ptr->pclass == CLASS_MIRROR_MASTER )
	{
	  if( n==3 || n==5 || n==7 || n==16 )energy_use = 50;
	}

	if ((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU))
	{
#ifdef JP
		take_hit(DAMAGE_USELIFE, mana_cost, "�ߓx�̏W��", -1);
#else
		take_hit(DAMAGE_USELIFE, mana_cost, "concentrating too hard", -1);
#endif
		/* Redraw hp */
		p_ptr->redraw |= (PR_HP);
	}

	/* Sufficient mana */
	else if (mana_cost <= old_csp)
	{
		/* Use some mana */
		p_ptr->csp -= mana_cost;

		/* Limit */
		if (p_ptr->csp < 0) p_ptr->csp = 0;
		/*:::���Ԓ�~�͑SMP�g��*/
		if ((use_mind == MIND_MINDCRAFTER) && (n == 13))
		{
			/* No mana left */
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
		}
	}

	/* Over-exert the player */
	else
	{
		int oops = mana_cost - old_csp;

		/* No mana left */
		if ((p_ptr->csp - mana_cost) < 0) p_ptr->csp_frac = 0;
		p_ptr->csp = MAX(0, p_ptr->csp - mana_cost);

		/* Message */
#ifdef JP
msg_format("%s���W���������ċC�������Ă��܂����I",p);
#else
		msg_print("You faint from the effort!");
#endif


		/* Hack -- Bypass free action */
		(void)set_paralyzed(p_ptr->paralyzed + randint1(5 * oops + 1));

		/* Damage WIS (possibly permanently) */
		if (randint0(100) < 50)
		{
			bool perm = (randint0(100) < 25);

			/* Message */
#ifdef JP
msg_print("�����̐��_���U�����Ă��܂����I");
#else
			msg_print("You have damaged your mind!");
#endif


			/* Reduce constitution */
			(void)dec_stat(A_WIS, 15 + randint1(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
/*:::���Z�̐�����ǂށ@���\�͎ҁA�B�C�p�t�A����m�A�E�ҁA���g��*/
///class ���Z�̐�����ǂ�
void do_cmd_mind_browse(void)
{
	int n = 0;
	int j, line;
	char temp[62*5];
	int use_mind = 0;

	if (p_ptr->pclass == CLASS_MINDCRAFTER || p_ptr->pclass == CLASS_SUMIREKO) use_mind = MIND_MINDCRAFTER;
	else if (p_ptr->pclass == CLASS_FORCETRAINER) use_mind = MIND_KI;
	//else if (p_ptr->pclass == CLASS_BERSERKER) use_mind = MIND_BERSERKER;
	else if (p_ptr->pclass == CLASS_NINJA) use_mind = MIND_NINJUTSU;
	else if (p_ptr->pclass == CLASS_MIRROR_MASTER)
	  use_mind = MIND_MIRROR_MASTER;

	screen_save();

	while(1)
	{
		/* get power */
		if (!get_mind_power(&n, TRUE))
		{
			screen_load();
			return;
		}

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(12, 21, 255);
		Term_erase(12, 20, 255);
		Term_erase(12, 19, 255);
		Term_erase(12, 18, 255);
		Term_erase(12, 17, 255);
		Term_erase(12, 16, 255);

		roff_to_buf(mind_tips[use_mind][n], 62, temp, sizeof(temp));
		for(j=0, line = 17;temp[j];j+=(1+strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
		/*:::�Ȃ����g���ƔE�҂������̃��b�Z�[�W���o���H�����Z�̎�ނ�����������\��������ꗗ�̉��̂ق����B��邹���炵��*/
		switch (use_mind)
		{
		case MIND_MIRROR_MASTER:
		case MIND_NINJUTSU:
#ifdef JP
		  prt("�����L�[�������ĉ������B",0,0);
#else
		  prt("Hit any key.",0,0);
#endif
		  (void)inkey();
		}
	}
}
