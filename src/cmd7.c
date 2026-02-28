
#include "angband.h"

/*
 * 追加インベントリなど一部の職業に関係する雑多な処理を記述
 */

/*:::職業ごとの追加インベントリ数を決める*/
/*:::memo：レベルなどパラメータ依存にするときはレベルダウンでインベントリが減らないように考慮したほうがいい。インベントリ使えなくなっても消えはしないはずだが*/
int calc_inven2_num(void)
{

	int num = 0;
	int pc = p_ptr->pclass;

	//薬師　常に10
	if(pc == CLASS_CHEMIST) num = 10;
	//お燐 2-5
	else if(pc == CLASS_ORIN) num = 2 + p_ptr->max_plv / 13;
	//エンジニア 2-10
	else if(pc == CLASS_ENGINEER) num = 2 + p_ptr->max_plv / 6;
	//古道具屋　常に5
	else if(pc == CLASS_SH_DEALER) num = 5;
	//にとり 1-6
	else if(pc == CLASS_NITORI) num = 1 + p_ptr->max_plv / 10;
	//菫子 1-7
	else if(pc == CLASS_SUMIREKO) num = 1 + (p_ptr->max_plv+3) / 8;
	//アリス1-7
	else if(pc == CLASS_ALICE) num = MIN(7, (1 + (p_ptr->max_plv-3) / 7));
	//咲夜4-8
	else if(pc == CLASS_SAKUYA) num = 4 + p_ptr->max_plv / 12;
	//うどんげ(夢) 2-5
	else if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE)) num = 2 + p_ptr->max_plv / 13;
	//女苑　常に8
	else if (pc == CLASS_JYOON) num = JYOON_INVEN2_NUM;
	//カード売人 常に8
	else if (pc == CLASS_CARD_DEALER) num = 8;
	//たかね　常に16
	else if (pc == CLASS_TAKANE) num = 16;
	//山如　常に8
	else if (pc == CLASS_SANNYO) num = 8;
	//ミケ　常に8
	else if (pc == CLASS_MIKE) num = 8;
	//魔理沙専用性格　闇市場調査員 常に8
	else if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA)) num = 8;
	//魅須丸 2-7
	else if (pc == CLASS_MISUMARU)
	{
		num = 1 + p_ptr->lev / 8;

		if (num > INVEN2_MAGATAMA_NUM_MAX) num = INVEN2_MAGATAMA_NUM_MAX;//上限7 ここを変えるとき勾玉発動ルーチンに影響がある

	}


	if(num > INVEN_ADD_MAX) num = INVEN_ADD_MAX;
	//未登録の職業は0が返って外でエラー処理

	return num;
}

#define SI2_MODE_NONE 0
#define SI2_MODE_ENGINEER 1
#define SI2_MODE_ALICE 2
#define SI2_MODE_JYOON	3
#define SI2_MODE_ACTIVATE 4

/*:::追加アイテム欄の一覧表示 display_list_inven2()はコマンドとしての一覧表示でこちらはいろんなコマンドから呼ばれる一時的な一覧表示*/
/*:::mode:SI2_MODE_ENGINEER*/
void show_inven2(int mode)
{
	int i,l,len,col,wid,hgt,cur_col;
	char            tmp_val[80];
	char            o_name[MAX_NLEN];
	int inven2_num = calc_inven2_num();
	int             out_index[INVEN_ADD_MAX];
	byte            out_color[INVEN_ADD_MAX];
	char            out_desc[INVEN_ADD_MAX][MAX_NLEN];
	object_type *o_ptr;

	if(!inven2_num)
	{
		msg_print("ERROR:この職業の追加インベントリ数が定義されていない");
		return ;
	}
	/* Get size */
	Term_get_size(&wid, &hgt);
	col = wid - 30;
	len = wid - col - 1;

	/*:::追加インベントリ表示　常にすべての欄*/
	for (i = 0; i < inven2_num; i++)
	{
		o_ptr = &inven_add[i];

		/* Is this item acceptable? */
		/*:::空欄以外は通るはず*/
		//if (!item_tester_okay(o_ptr)) continue;

		if(!o_ptr->k_idx)
		{
			if(mode == SI2_MODE_ALICE)
			{
				out_color[i] = TERM_L_DARK;
				(void)strcpy(out_desc[i], format("%15s:(未装備)",alice_doll_name[i]));
				l = 13;
			}
			else if (mode == SI2_MODE_JYOON)
			{
				out_color[i] = TERM_L_DARK;
				(void)strcpy(out_desc[i], format("%15s:(未装備)", jyoon_inven2_finger_name[i]));
				l = 13;
			}
			else
			{
				out_color[i] = TERM_L_DARK;
				(void)strcpy(out_desc[i], "(空欄)");
				l = 11;
			}
		}	
		else
		{
			/* Describe the object */
			object_desc(o_name, o_ptr, 0);

			/* Save the object index, color, and description */
			out_color[i] = tval_to_attr[o_ptr->tval % 128];

			/* Grey out charging items */
			if (o_ptr->timeout)
			{
				out_color[i] = TERM_L_DARK;
			}
			//エンジニアはアイテムと一緒に回数を表示し、使用回数切れのアイテムを暗く表示する
			if(mode == SI2_MODE_ENGINEER)
			{
				if(!o_ptr->pval) out_color[i] = TERM_L_DARK;
				else if(o_ptr->xtra4) out_color[i] = TERM_L_DARK;
				else  out_color[i] = TERM_WHITE;
				if(o_ptr->xtra4)
					sprintf(out_desc[i],"%s(故障中)",o_name);
				else
					sprintf(out_desc[i],"%s(%d/%d)",o_name,o_ptr->pval,machine_table[o_ptr->sval].charge);
			}
			//発動モード　発動不可能なアイテムは暗く表示
			else if (mode == SI2_MODE_ACTIVATE)
			{
				if(!item_tester_hook_activate(o_ptr))
					out_color[i] = TERM_L_DARK;
				if(o_ptr->timeout)
					out_color[i] = TERM_L_DARK;

				(void)strcpy(out_desc[i], o_name);
			}
			else if(mode == SI2_MODE_ALICE)
			{
				sprintf(out_desc[i],"%15s:%s",alice_doll_name[i],o_name);
			}
			else if (mode == SI2_MODE_JYOON)
			{
				sprintf(out_desc[i], "%15s:%s", jyoon_inven2_finger_name[i], o_name);
			}

			else (void)strcpy(out_desc[i], o_name);

			/* Find the predicted "line length" */
			l = strlen(out_desc[i]) + 5;

			/* Be sure to account for the weight */
			if (show_weights) l += 9;
		}
	
		/* Maintain the maximum length */
		if (l > len) len = l;
	}

	/* Find the column to start in */
	col = (len > wid - 4) ? 0 : (wid - len - 1);
	/* Output each entry */
	for (i = 0; i < inven2_num; i++)
	{

		/* Get the item */
		o_ptr = &inven_add[i];

		/* Clear the line */
		prt("", i + 1, col ? col - 2 : col);


		/* Prepare an index --(-- */
		sprintf(tmp_val, "%c)", index_to_label(i));


		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, i + 1, col);

		cur_col = col + 3;

		/* Display graphics for object, if desired */
		if (show_item_graph && mode != SI2_MODE_ALICE)
		{
			byte  a = object_attr(o_ptr);
			char c = object_char(o_ptr);

			if(!o_ptr->k_idx)
			{
				a = TERM_DARK;
				c = ' ';
			}
			//ここは空欄の時どう処理される？

#ifdef AMIGA
			if (a & 0x80) a |= 0x40;
#endif

			Term_queue_bigchar(cur_col, i + 1, a, c, 0, 0);
			if (use_bigtile) cur_col++;

			cur_col += 2;
		}


		/* Display the entry itself */
		c_put_str(out_color[i], out_desc[i], i + 1, cur_col);

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt=0;
			if(o_ptr->k_idx) wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			(void)sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
#else
			(void)sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			prt(tmp_val, i + 1, wid - 9);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (i) prt("", i + 1, col ? col - 2 : col);

	/* Save the new column */
	command_gap = col;

	return ;
}


/*:::追加アイテム欄の表示*/
//v1.1.87 アイテムを選択し、Iコマンドと同じ確認をすることにした
void display_list_inven2(void)
{
	int inven2_num = calc_inven2_num();
	int item;
	object_type *o_ptr;

	char c;
	char out_val[160];
	int i;


	screen_save();

	while (1)
	{

		item_tester_full = TRUE;
		if (p_ptr->pclass == CLASS_ALICE)
			show_inven2(SI2_MODE_ALICE);
		else if (p_ptr->pclass == CLASS_JYOON)
			show_inven2(SI2_MODE_JYOON);
		else
			show_inven2(0);
		item_tester_full = FALSE;

		prt("どれを確認しますか？(ESC:終了)", 0, 0);
		c = inkey();
		if (c == ESCAPE) break;

		item = c - 'a';
		if (item < 0 || item >= inven2_num) continue;

		o_ptr = &inven_add[item];

		if (!o_ptr->k_idx)
		{
			msg_format("確認するアイテムがない。");
			continue;
		}
		if (!(o_ptr->ident & IDENT_MENTAL))
		{
			msg_print("このアイテムについて特に知っていることはない。");

		}
		else
		{
			if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL)) msg_print("特に変わったところはないようだ。");

		}



	}

	screen_load();


}


/*:::薬師が薬箱に入れられるアイテムを選定する　薬、素材、キノコのみ*/
static bool item_tester_inven2_chemist(object_type *o_ptr)
{
	if (o_ptr->tval == TV_POTION) return (TRUE);
	if (o_ptr->tval == TV_COMPOUND) return (TRUE);
	if (o_ptr->tval == TV_MATERIAL) return (TRUE);
	if (o_ptr->tval == TV_MUSHROOM) return (TRUE);

	return (FALSE);
}

//v1.1.87 カード売人がカードケースに入れられるアイテム
static bool item_tester_inven2_card_dealer(object_type *o_ptr)
{
	if (o_ptr->tval == TV_ITEMCARD) return (TRUE);
	if (o_ptr->tval == TV_ABILITY_CARD) return (TRUE);

	return (FALSE);
}

//v2.0.4 魅須丸の勾玉選定tester
static bool item_tester_inven2_misumaru(object_type *o_ptr)
{
	if (o_ptr->tval == TV_SPELLCARD && o_ptr->sval == SV_SPELLCARD_MAGATAMA) return (TRUE);

	return (FALSE);
}

/*:::アリスが人形に持たせるもの　武器と盾 反魔法不可*/
static bool item_tester_inven2_alice(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE]; //切れ味
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_NO_MAGIC)) return (FALSE);
	
	if (o_ptr->tval == TV_SHIELD) return (TRUE);
	if ((TV_KNIFE <= o_ptr->tval && o_ptr->tval <= TV_POLEARM)) return (TRUE);

	return (FALSE);
}

//アリスの人形が装備可能な品 ハードコーディングあり
bool check_alice_doll_can_wield(object_type *o_ptr, int doll_num)
{
	switch(doll_num)
	{
	case INVEN_DOLL_SHANHAI:
		if(o_ptr->weight > 50) return FALSE;
		if(o_ptr->tval == TV_KNIFE || o_ptr->tval == TV_SWORD) return TRUE;
		break;		
	case INVEN_DOLL_HORAI:
		if(o_ptr->weight > 70) return FALSE;
		if(o_ptr->tval == TV_KNIFE || o_ptr->tval == TV_SWORD || o_ptr->tval == TV_SPEAR) return TRUE;
		break;	
	case INVEN_DOLL_RUSSIA:
		if(o_ptr->weight > 120) return FALSE;
		if(o_ptr->tval == TV_HAMMER || o_ptr->tval == TV_AXE || o_ptr->tval == TV_SWORD) return TRUE;
		break;	
	case INVEN_DOLL_LONDON:
		if(o_ptr->weight > 160) return FALSE;
		if(o_ptr->tval == TV_SWORD || o_ptr->tval == TV_POLEARM || o_ptr->tval == TV_SPEAR || o_ptr->tval == TV_STICK) return TRUE;
		break;	
	case INVEN_DOLL_ORLEANS:
		if(o_ptr->weight > 180) return FALSE;
		if(o_ptr->tval == TV_SWORD || o_ptr->tval == TV_SHIELD || o_ptr->tval == TV_HAMMER) return TRUE;
		break;	
	case INVEN_DOLL_KYOTO:
		if(o_ptr->weight > 200) return FALSE;
		if(o_ptr->tval != TV_SHIELD) return TRUE;
		break;	
	case INVEN_DOLL_GOLIATH:
		if(o_ptr->weight < 200) return FALSE;
		if(o_ptr->tval != TV_SHIELD) return TRUE;
		//if(o_ptr->tval >= TV_KNIFE && o_ptr->tval <= TV_POLEARM) return TRUE;
		break;	

	default:
		msg_format("ERROR:check_alice_doll_can_wield()に未定義のdoll_num(%d)が送られた",doll_num);
		break;
	}

	return FALSE;
}



/*:::追加インベントリにアイテムを入れる。職によって入れられるアイテムの種類、個数、アイテムをまとめるかどうかなど処理が違う*/
/*:::行動順消費するとき戻り値がTRUE*/
bool put_item_into_inven2(void)
{
	int inven2_num = calc_inven2_num();
	int item, amt, slot2;
	cptr q, s;
	object_type *o_ptr;
	object_type forge,forge2;
	object_type *q_ptr = &forge;
	object_type *q2_ptr = &forge2;
	int pc = p_ptr->pclass;
	char o_name[MAX_NLEN];
	bool success = FALSE;
	bool changed = FALSE;
	int choose_num;

	if(!inven2_num)
	{
		msg_print("ERROR:この職業の追加インベントリ数が定義されていない");
		return FALSE;
	}

	switch(pc)
	{
	case CLASS_CHEMIST:
		item_tester_hook = item_tester_inven2_chemist;
		q = "どれを薬箱に入れますか? ";
		s = "薬箱に入れるべきものがない。";
		break;
	case CLASS_ORIN:
		q = "どれを猫車に入れますか? ";
		s = "猫車に入れるべきものがない。";
		break;
	case CLASS_ENGINEER:
		item_tester_tval = TV_MACHINE;
		q = "どの機械をバックパックに格納しますか? ";
		s = "機械を持っていない。";
		break;
	case CLASS_NITORI:
		item_tester_tval = TV_MACHINE;
		q = "どの機械をリュックに入れますか? ";
		s = "機械を持っていない。";
		break;
	case CLASS_SH_DEALER:
		q = "どれを道具箱に入れますか? ";
		s = "道具箱に入れるべきものがない。";
		break;
	case CLASS_ALICE:
		item_tester_hook = item_tester_inven2_alice;
		q = "どれを人形に持たせますか? ";
		s = "人形に持たせるものがない。";
		break;
	case CLASS_SAKUYA:
		item_tester_tval = TV_KNIFE;
		q = "どの短剣をナイフホルダーに差しますか？ ";
		s = "目ぼしい短剣が見当たらない。";
		break;
	case CLASS_UDONGE:
		q = "どれを薬籠に入れますか? ";
		s = "薬籠に入れるべきものがない。";
		break;
	case CLASS_JYOON:
		item_tester_tval = TV_RING;
		q = "どの指輪をつけますか？ ";
		s = "指輪を持っていない。";
		break;
	case CLASS_TAKANE:
	case CLASS_SANNYO:
	case CLASS_MIKE:
	case CLASS_CARD_DEALER:
		item_tester_hook = item_tester_inven2_card_dealer;

		q = "どのカードをケースに入れますか？ ";
		s = "アビリティカードを持っていない。";
		break;

	case CLASS_MARISA:
		item_tester_hook = item_tester_inven2_card_dealer;
		q = "どのカードを隠し持ちますか？ ";
		s = "アビリティカードを持っていない。";
		break;

	case CLASS_MISUMARU:
		item_tester_hook = item_tester_inven2_misumaru;
		q = "どの勾玉を身につけますか？ ";
		s = "勾玉を持っていない。";
		break;

	default:
		msg_print("ERROR:この職業の追加インベントリ対象アイテムが定義されていない");
		return FALSE;
	}

	//咲夜のみ装備中の品を指定できる
	if(pc == CLASS_SAKUYA)
	{
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR | USE_EQUIP))) return FALSE;
	}
	else
	{
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;
	}

	if (item >= 0)	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];


	
	amt = 1;
	/*:::個数指定　ここで指定されていないアリスやエンジニア、咲夜などは常に一つずつ*/
	if (o_ptr->number > 1 && ( pc == CLASS_CHEMIST || pc == CLASS_ORIN || pc == CLASS_SH_DEALER || pc == CLASS_UDONGE || (CHECK_ABLCARD_DEALER_CLASS)) )
	{
		/*:::数量を入力。アイテム個数以上にはならないよう処理される。*/
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return FALSE;
	}

	//(void)show_inven2();

	//女苑は安物の指輪や呪われた指輪を装備しない
	if (pc == CLASS_JYOON)
	{
		if (object_is_cursed(o_ptr))
		{
			msg_print("あなたはこんなケチのついたものを装備したくない。");
			return FALSE;
		}
		/* アイテム価格制限ボツ
		if(object_is_cheap_to_jyoon(o_ptr))
		{
			msg_print("あなたはこんな安物を装備したくない。");
			return FALSE;
		}
		*/

	}
	//v1.1.53 昨夜の装備品スロットから呪われた装備品を移動できないよう修正
	else if (p_ptr->pclass == CLASS_SAKUYA)
	{
		if (item >= INVEN_RARM && object_is_cursed(o_ptr))
		{
			msg_print("ナイフが呪われていて手から離れない。");
			return FALSE;
		}


	}

	/*:::選択アイテムの実体確保*/
	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;


	/*:::自動的にまとめられるか判定しつつアイテムを自動的に空いてる追加インベントリに入れる職業（薬師、お燐）*/
	/*:::エンジニアは1スロット1つしか入れないがどうせ機械はまとまらないのでこのルーチンのままで問題ないはず*/
	if( pc == CLASS_CHEMIST || pc == CLASS_ORIN || pc == CLASS_ENGINEER || pc == CLASS_NITORI || pc == CLASS_SH_DEALER || pc == CLASS_UDONGE || (CHECK_ABLCARD_DEALER_CLASS) )
	{
		int freespace = 99;
		/*:::まとめられるか判定*/
		for(slot2 = 0;slot2 < inven2_num;slot2++)
		{
			object_type *j_ptr;
			j_ptr = &inven_add[slot2];
			/*:::空きスロットがあればそこを登録しておく*/
			if(!j_ptr->k_idx)
			{
				if(freespace == 99) freespace = slot2;
				 continue;
			}
			//同系統アイテムか、個数合計99以内か、まとめられるアイテムか、を判定している
			if(object_similar(j_ptr,q_ptr))
			{
				/*:::ロッドか魔法棒を分けたときpvalに記録されている充填状況を調整する*/
				distribute_charges(o_ptr, q_ptr, amt);

				/*:::既存のアイテムとまとめられる場合はまとめ処理をし、ループ終了*/
				object_absorb(j_ptr,q_ptr);
				success = TRUE;
				break;

			}
			
		}
		/*:::まとめられなかったが空きスロットがあるときそこに入れる*/
		if(!success && freespace != 99)
		{
			/*:::ロッドか魔法棒を分けたときpvalに記録されている充填状況を調整する*/
			distribute_charges(o_ptr, q_ptr, amt);

			object_wipe(&inven_add[freespace]);
			object_copy(&inven_add[freespace], q_ptr);
			success = TRUE;
		}
	}
	//アリスはスロットを指定する必要があるので別処理
	else if(pc == CLASS_ALICE)
	{
		int wid,hgt;
		int i;
		char o_name_2[MAX_NLEN];
		char tmp_val[7][160];
		int len = 0, col;
		char c;
		
		screen_save();
		msg_print("どの人形に持たせますか？(キャンセル:ESC)");
		Term_get_size(&wid, &hgt);
		for(i=0;i<inven2_num;i++)
		{
			int tmp_len;

			if(inven_add[i].k_idx)
			{
				object_desc(o_name_2, &inven_add[i], 0);
				sprintf(tmp_val[i],"%c) %15s: %s",'a'+i,alice_doll_name[i],o_name_2);
			}
			else
			{
				sprintf(tmp_val[i],"%c) %15s : (未装備)",'a'+i,alice_doll_name[i]);
			}
			tmp_len = strlen(tmp_val[i]);
			if(len < tmp_len) len = tmp_len;
		}

		col = (len > wid - 4) ? 0 : (wid - len - 1);
		for(i=0;i<inven2_num;i++)
		{
			prt("", i + 1, col ? col - 2 : col);

			//装備不可の人形を弾く
			if(!check_alice_doll_can_wield(o_ptr,i)) continue;
			c_put_str(TERM_WHITE, tmp_val[i], i + 1, col);

		}
		while(1)
		{
			c = inkey();
			if(c >= 'a' && c < ('a' + inven2_num) ) 
			{
				choose_num = c - 'a';
				//装備可能な人形の時のみループ脱出
				if(check_alice_doll_can_wield(o_ptr, choose_num)) 
					break;	
			}
			else if(c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
		}
		screen_load();

		//もし既に装備中の人形を選択したら元の装備品を取り出す処理

		if(inven_add[choose_num].k_idx)
		{
			changed = TRUE;
			success = TRUE;
			object_copy(q2_ptr, &inven_add[choose_num]);
			object_wipe(&inven_add[choose_num]);
			object_copy(&inven_add[choose_num], q_ptr);

		}
		else
		{
			object_wipe(&inven_add[choose_num]);
			object_copy(&inven_add[choose_num], q_ptr);
			success = TRUE;
		}
	}
	//咲夜も追加　スロットを常に指定する
	//v1.1.46 女苑も追加
	//v2.0.5 魅須丸も追加しよう
	else if(pc == CLASS_SAKUYA || pc == CLASS_JYOON || pc == CLASS_MISUMARU)
	{
		int wid,hgt;
		int i;
		char o_name_2[MAX_NLEN];
		char tmp_val[8][160]; //ここ7にしてたせいで表示おかしくなってた
		int len = 0, col;
		char c;

		screen_save();
		if(pc == CLASS_SAKUYA)
			msg_print("どこに差しますか？(キャンセル:ESC)");
		else
			msg_print("どこにつけますか？(キャンセル:ESC)");

		Term_get_size(&wid, &hgt);
		for (i = 0; i < inven2_num; i++)
		{
			int tmp_len;

			if (pc == CLASS_SAKUYA || pc == CLASS_MISUMARU)
			{
				if (inven_add[i].k_idx)
				{
					object_desc(o_name_2, &inven_add[i], 0);
					sprintf(tmp_val[i], "%c)  %s", 'a' + i, o_name_2);
				}
				else
				{
					sprintf(tmp_val[i], "%c) (未装備)", 'a' + i);
				}
			}
			else //女苑
			{
				if (inven_add[i].k_idx)
				{
					object_desc(o_name_2, &inven_add[i], 0);
					sprintf(tmp_val[i], "%c) %15s: %s", 'a' + i, jyoon_inven2_finger_name[i], o_name_2);
				}
				else
				{
					sprintf(tmp_val[i], "%c) %15s : (未装備)", 'a' + i, jyoon_inven2_finger_name[i]);
				}
			}

			tmp_len = strlen(tmp_val[i]);
			if(len < tmp_len) len = tmp_len;
		}


		col = (len > wid - 4) ? 0 : (wid - len - 1);
		for(i=0;i<inven2_num;i++)
		{
			prt("", i + 1, col ? col - 2 : col);
			c_put_str(TERM_WHITE, tmp_val[i], i + 1, col);
		}

		while(1)
		{
			c = inkey();
			if(c >= 'a' && c < ('a' + inven2_num) ) 
			{
				choose_num = c - 'a';
				break;	
			}
			else if(c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
		}
		screen_load();


		//v1.1.46 女苑は装備に価格制限が入るので二度と装備できないものを外すときは確認するようにしておく
		/* ...つもりだったがやっぱゲームのテンポが悪くなりそうなのでボツ
		if (p_ptr->pclass == CLASS_JYOON)
		{
			if (inven_add[choose_num].k_idx && object_is_cheap_to_jyoon(&inven_add[choose_num]))
			{
				msg_print("このような安物は二度と身につけることはないだろう。");
				if (!get_check_strict("本当に指輪を付け替えますか？", CHECK_OKAY_CANCEL))
				{
					return FALSE;
				}
			}
		}
		*/

		if(inven_add[choose_num].k_idx)
		{
			changed = TRUE;
			success = TRUE;
			object_copy(q2_ptr, &inven_add[choose_num]);
			object_wipe(&inven_add[choose_num]);
			object_copy(&inven_add[choose_num], q_ptr);
		}
		else
		{
			object_wipe(&inven_add[choose_num]);
			object_copy(&inven_add[choose_num], q_ptr);
			success = TRUE;
		}
	}
	else
	{
		msg_format("ERROR:この職業の追加インベントリスロット選定処理が記述されていない");
		return FALSE;
	}
	

	object_desc(o_name, q_ptr, 0);

	/*:::追加インベントリにアイテムを入れた場合元アイテムの数を減らす*/
	if(success)
	{
		/*:::追加インベントリの重量増加 エンジニアは追加インベントリの重量を無視する（バックパックが浮いてるから）*/
		///mod150805 アリス追加
		if(p_ptr->pclass != CLASS_ALICE && p_ptr->pclass != CLASS_ENGINEER 
			&& p_ptr->pclass != CLASS_NITORI ) p_ptr->total_weight += q_ptr->weight * q_ptr->number;

		/*:::ザックの重量減少はinven_item_increase()内で処理されている*/
		if(item >= 0)
		{
			inven_item_increase(item, -amt);
		//	inven_item_describe(item);
			inven_item_optimize(item);
		}
		else
		{
			floor_item_increase(0 - item, -amt);
		//	floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}	

		if(pc == CLASS_CHEMIST) msg_format("%sを薬箱に入れた。",o_name);
		else if(pc == CLASS_ORIN) msg_format("%sを猫車に入れた。",o_name);
		else if(pc == CLASS_SH_DEALER) msg_format("%sを道具箱に入れた。",o_name);
		else if(pc == CLASS_ENGINEER) msg_format("%sをバックパックに装備した。",o_name);
		else if(pc == CLASS_NITORI) msg_format("%sをリュックに入れた。",o_name);
		else if(pc == CLASS_ALICE) msg_format("%sを%sに持たせた。",o_name,alice_doll_name[choose_num]);
		else if(pc == CLASS_SAKUYA) msg_format("%sをナイフホルダーに差した。",o_name);
		else if (pc == CLASS_UDONGE) msg_format("%sを薬籠に入れた。", o_name);
		else if (pc == CLASS_JYOON) msg_format("%sを装備した。", o_name);
		else if (pc == CLASS_TAKANE) msg_format("%sをケースに収納した。", o_name);
		else if (pc == CLASS_MIKE) msg_format("%sをケースに収納した。", o_name);
		else if (pc == CLASS_SANNYO) msg_format("%sをケースに収納した。", o_name);
		else if (pc == CLASS_CARD_DEALER) msg_format("%sをケースに収納した。", o_name);
		else if (pc == CLASS_MARISA) msg_format("%sをスカートの隠しポケットに入れた。", o_name);
		else if (pc == CLASS_MISUMARU) msg_format("%sで身を飾った。", o_name);

		else msg_format("ERROR:追加インベントリにアイテム入れたときのメッセージがない");

//inven_cntやequip_cntは気にしなくていいんだろうか？アイテムを拾ったりザックをまとめる辺りで何か変なことになるかも。セーブ＆ロードで直るようだが。

		if(changed)
		{
			//Hack - 装備欄はinven_item_optimize()で詰められないのでitem値をそのまま使いサブインベントリにあったアイテムで上書きしても問題ないはず
			if(item > INVEN_PACK)
			{
				object_desc(o_name, q2_ptr, 0);
				msg_format("%sを代わりに構えた。",o_name);
				object_wipe(&inventory[item]);
				object_copy(&inventory[item], q2_ptr);
			}
			else
			{
				inven_carry(q2_ptr);
			}
		}
		///mod160113 装備品を外して追加インベントリに入れ何も出さなかった場合外した装備欄の装備状態を再計算　今のところ咲夜のナイフ収納のみ
		else if(item > INVEN_PACK)
		{
			kamaenaoshi(item);
		}

		//装備変更のとき再計算
		if(item > INVEN_PACK)
		{
			//kamaenaoshiしたら武器が消えた。なぜ？
			//↑kamaenaoshi()は装備を外した時に呼ばれるルーチンだった
			p_ptr->update |= PU_BONUS;
			p_ptr->redraw |= (PR_EQUIPPY);
		}
	
		return TRUE;
	}
	else //追加インベントリが一杯だったときのメッセージ　咲夜やアリスなどは指定したスロットに物があったら入れ替えるのでここには来ないはず
	{
		if(pc == CLASS_CHEMIST) msg_format("薬箱には%sを入れる場所がない。",o_name);
		else if(pc == CLASS_ORIN) msg_format("猫車には%sを入れる隙間がない。",o_name);
		else if(pc == CLASS_SH_DEALER) msg_format("道具箱には%sを入れる隙間がない。",o_name);
		else if(pc == CLASS_ENGINEER) msg_format("バックパックは%sを格納するスペースがない。",o_name);
		else if(pc == CLASS_NITORI) msg_format("もうリュックには機械が入らない。");
		else if (pc == CLASS_UDONGE) msg_format("薬籠には%sを入れる隙間がない。", o_name);
		else if (pc == CLASS_TAKANE) msg_format("ケースには%sを入れる隙間がない。", o_name);
		else if (pc == CLASS_MIKE) msg_format("ケースには%sを入れる隙間がない。", o_name);
		else if (pc == CLASS_SANNYO) msg_format("ケースには%sを入れる隙間がない。", o_name);
		else if (pc == CLASS_CARD_DEALER) msg_format("ケースには%sを入れる隙間がない。", o_name);
		else if (pc == CLASS_MARISA) msg_format("もうスカートの隠しポケットは一杯だ。");
		else if (pc == CLASS_MISUMARU) msg_format("%sをつける場所がない。", o_name);

		else msg_format("ERROR:追加インベントリにアイテム入れる場所がないときのメッセージがない");

		return FALSE;
	}

}


/*:::追加インベントリからアイテムを取り出す*/
bool takeout_inven2(void)
{
	int inven2_num = calc_inven2_num();
	int item, amt;
	cptr q, s;
	char c;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int pc = p_ptr->pclass;
	char o_name[MAX_NLEN];

	if(!inven2_num)
	{
		msg_print("ERROR:この職業の追加インベントリ数が定義されていない");
		return FALSE;
	}
	screen_save();

	if( pc == CLASS_ENGINEER || pc == CLASS_NITORI) (void)show_inven2(SI2_MODE_ENGINEER);
	else if( pc == CLASS_ALICE) (void)show_inven2(SI2_MODE_ALICE);
	else if (pc == CLASS_JYOON) (void)show_inven2(SI2_MODE_JYOON);
	else (void)show_inven2(0);

	if( pc == CLASS_ENGINEER || pc == CLASS_NITORI) prt("どの機械を外しますか？", 0, 0);
	else if( pc == CLASS_ALICE ) prt("どの装備を外しますか？", 0, 0);
	else if( pc == CLASS_SAKUYA ) prt("どの武器を取りますか？", 0, 0);
	else if (pc == CLASS_MISUMARU) prt("どの勾玉を取りますか？", 0, 0);
	else if (pc == CLASS_JYOON) prt("どの指輪を外しますか？", 0, 0);
	else  prt("どのアイテムを取り出しますか？", 0, 0);

	while(1)
	{
		c = inkey();
		if(c >= 'a' && c < ('a' + inven2_num) ) break;
		if(c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
	}
	item = c-'a';
	o_ptr = &inven_add[ item ];

	if(!o_ptr->k_idx)
	{
		if(pc == CLASS_ALICE)
			msg_format("%sは何も持っていない。",alice_doll_name[item]);
		else if(pc == CLASS_JYOON)
			msg_format("そこには何もつけていない。");
		else
			msg_format("そこには何も入っていない。");

		screen_load();
		return FALSE;
	}

	//v1.1.46 女苑は装備に価格制限が入るので二度と装備できないものを外すときは確認するようにしておく
	/* ↑ボツ
	if (p_ptr->pclass == CLASS_JYOON)
	{
		if (object_is_cheap_to_jyoon(o_ptr))
		{
			msg_print("このような安物は二度と身につけることはないだろう。");
			if (!get_check_strict("本当に指輪を外しますか？", CHECK_OKAY_CANCEL))
			{
				screen_load();
				return FALSE;
			}
		}
	}
	*/

	amt = 1;
	if(o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0)
		{
			screen_load();
			return FALSE;
		}
	}

	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;
	distribute_charges(o_ptr, q_ptr, amt);
	object_desc(o_name, q_ptr, 0);

	/*:::追加インベントリの数を減少させ、0になったら初期化する*/
	/*:::inven_item_increase()と似た処理*/

	//v1.1.73 アリス追加
	//重量処理　エンジニア、にとり、アリスは追加インベントリの重量を無視するのでそこから受け取った分は重量増加
	//(ここでアイテムがいっぱいだった場合process_player()のpack_overflow()内でアイテムあふれ処理が行われるのでこの時点ではそのまま渡してよい
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI && p_ptr->pclass != CLASS_ALICE)
		p_ptr->total_weight -= q_ptr->weight * amt; 

	o_ptr->number -= amt;
	if(o_ptr->number <1) object_wipe(o_ptr);

	//if(p_ptr->total_weight < 0) msg_format("ERROR:追加インベントリからアイテムを出したときtotal_weightが負になった");

	if( pc == CLASS_CHEMIST) msg_format("薬箱から%sを取り出した。",o_name);
	else if(pc == CLASS_ORIN) msg_format("%sを猫車から出した。",o_name);
	else if(pc == CLASS_SH_DEALER) msg_format("道具箱から%sを出した。",o_name);
	else if(pc == CLASS_ENGINEER) msg_format("バックパックから%sを外した。",o_name);
	else if(pc == CLASS_NITORI) msg_format("リュックから%sを出した。",o_name);
	else if(pc == CLASS_ALICE) msg_format("%sから%sを受け取った。",alice_doll_name[item],o_name);
	else if(pc == CLASS_SAKUYA) msg_format("ナイフホルダーから%sを抜き取った。",o_name);
	else if (pc == CLASS_UDONGE) msg_format("薬籠から%sを出した。", o_name);
	else if (pc == CLASS_JYOON) msg_format("%sを指から外した。", o_name);
	else if (pc == CLASS_TAKANE) msg_format("%sをケースから出した。", o_name);
	else if (pc == CLASS_MIKE) msg_format("%sをケースから出した。", o_name);
	else if (pc == CLASS_SANNYO) msg_format("%sをケースから出した。", o_name);
	else if (pc == CLASS_CARD_DEALER) msg_format("%sをケースから出した。", o_name);
	else if (pc == CLASS_MARISA) msg_format("スカートの隠しポケットから%sを出した。", o_name);
	else if (pc == CLASS_MISUMARU) msg_format("%sを取り外した。", o_name);
	else msg_format("ERROR:追加インベントリからアイテム出したときのメッセージがない");
	
	(void)inven_carry(q_ptr);

	screen_load();
	return TRUE;

}



//v1.1.46 inven_add[]から装備品を発動する処理。今のところ女苑の指輪専用。
//行動順消費するときTRUEを返す。なお発動コマンドの場合発動成功後にターゲット選択などでキャンセルしても行動順消費する。
bool	activate_inven2(void)
{
	int inven2_num = calc_inven2_num();
	int item, amt;
	cptr q, s;
	char c;
	object_type *o_ptr;
	object_type forge;
	object_type *q_ptr = &forge;
	int pc = p_ptr->pclass;
	char o_name[MAX_NLEN];

	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_ONLY_MELEE)
	{
		msg_print("この姿ではアイテムの発動ができない。");
		return FALSE;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	if (!inven2_num)
	{
		msg_print("ERROR:この職業の追加インベントリ数が定義されていない");
		return FALSE;
	}
	screen_save();

	(void)show_inven2(SI2_MODE_ACTIVATE);

	prt("どのアイテムを始動させますか？", 0, 0);

	while (1)
	{
		c = inkey();
		if (c >= 'a' && c < ('a' + inven2_num)) break;
		if (c == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
	}
	item = c - 'a';
	o_ptr = &inven_add[item];
	screen_load();

	if (!o_ptr->k_idx)
	{
		if (pc == CLASS_JYOON)
			msg_format("そこには何もつけていない。");
		else
			msg_format("そこには何も入っていない。");

		return FALSE;
	}

	if (!item_tester_hook_activate(o_ptr))
	{
		msg_format("それは始動できるアイテムではない。");

		return FALSE;
	}

	/*::: -Hack- 
	* do_cmd_activate_aux()に負のitem値を渡すとinven_add[]からアイテム情報を取得することにする。
	* 他のコマンドの場合itemが負だと床上アイテムに対して処理を行うが、発動コマンドの場合装備中の品しか選択できないので床上アイテムを気にする必要がない。
	* ただしitem=0のときinventory[]の0を読み込まないようさらに1減算する
	*/
	do_cmd_activate_aux( -1 -item);

	return TRUE;

}


/*:::付喪神使いの特技「帰還命令」を使った時の処理 特殊フラグが立っている存在が1/3でザックに帰還する*/
void tsukumo_recall(void)
{
	int i;

	if(p_ptr->pclass != CLASS_TSUKUMO_MASTER) return;

	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		object_type *q_ptr;
		char m_name[80];
		if (m_ptr->r_idx < MON_TSUKUMO_WEAPON1 || m_ptr->r_idx > MON_TSUKUMO_WEAPON5 ) continue;
		if (!(m_ptr->mflag & MFLAG_SPECIAL)) continue;
		if(!one_in_(3)) continue;
		
		q_ptr = &o_list[m_ptr->hold_o_idx];

		if(!q_ptr->k_idx) msg_format("ERROR:帰還命令の出された付喪神IDX:%dがアイテムを持っていない",m_ptr->r_idx);

		object_desc(m_name,q_ptr,OD_NAME_ONLY);
		msg_format("%sが帰還した！",m_name);
		disturb(1,1);

		q_ptr->held_m_idx = 0;
		q_ptr->marked = OM_TOUCHED;
		m_ptr->hold_o_idx = q_ptr->next_o_idx;
		q_ptr->next_o_idx = 0;

		if(inven_carry_okay(q_ptr))
		{
			inven_carry(q_ptr);
		}
		else
		{
			object_type forge;
			object_type *o_ptr = &forge;
			object_copy(o_ptr, q_ptr);
			msg_format("しかしザックに入らず地面に落ちた！",m_name);
			(void)drop_near(o_ptr, -1, py, px);
		}
		delete_monster_idx(i);
	}

}

/*:::付喪神使いの使っている武器が妖器化して能力を得る処理*/
/*:::使用していた武器、この関数が呼ばれた時攻撃していた敵の種族が引数*/
/*:::o_ptr->xtra1が50と100のとき呼ばれる*/
///mod151219 雷鼓追加
void make_evilweapon(object_type *o_ptr, monster_race *r_ptr)
{
	int attempt = 10;
	u32b flgs[TR_FLAG_SIZE];
	int new_flag = -1;
	bool make_1st = FALSE;
	char o_name[MAX_NLEN];

	/*:::Paranoiacs*/
	if(object_is_fixed_artifact(o_ptr)) return; 
	if(!object_is_melee_weapon(o_ptr)) return;
	if(p_ptr->pclass != CLASS_TSUKUMO_MASTER && p_ptr->pclass != CLASS_RAIKO) msg_print("ERROR:付喪神使い以外でmake_evliweapon()が呼ばれている");

	if(o_ptr->xtra1 == 50) make_1st = TRUE;
	object_flags(o_ptr, flgs);
	object_desc(o_name, o_ptr, OD_NAME_ONLY);
	o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER; //鍛冶アイテム扱いにする

	for(attempt=10;attempt;attempt--)
	{
		int select = randint1(7);

		//初回の雷鼓は|電しか付かない
		if(p_ptr->pclass == CLASS_RAIKO && make_1st)
		{
			new_flag = TR_BRAND_ELEC;
			break;
		}

		switch(select)
		{
		case 1:	case 2: //攻撃していた敵に応じてスレイを付与する
//msg_print("chk:Enter:newSlay");
			{
				int cnt=0;


				if(r_ptr->flags3 & RF3_KWAI)
				{
					cnt++;
					if(have_flag(flgs,TR_SLAY_KWAI)) new_flag = TR_KILL_KWAI;
					else new_flag = TR_SLAY_KWAI;
				}
				if(r_ptr->flags3 & RF3_DEITY)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_DEITY)) new_flag = TR_KILL_DEITY;
						else new_flag = TR_SLAY_DEITY;
					}
				}
				if(r_ptr->flags3 & RF3_DRAGON)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_DRAGON)) new_flag = TR_KILL_DRAGON;
						else new_flag = TR_SLAY_DRAGON;
					}
				}
				if(r_ptr->flags3 & RF3_DEMON)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_DEMON)) new_flag = TR_KILL_DEMON;
						else new_flag = TR_SLAY_DEMON;
					}
				}
				if(r_ptr->flags3 & RF3_UNDEAD)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_UNDEAD)) new_flag = TR_KILL_UNDEAD;
						else new_flag = TR_SLAY_UNDEAD;
					}
				}
				if(r_ptr->flags3 & RF3_ANIMAL)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_ANIMAL)) new_flag = TR_KILL_ANIMAL;
						else new_flag = TR_SLAY_ANIMAL;
					}
				}
				if(r_ptr->flags3 & RF3_HUMAN)
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_HUMAN)) new_flag = TR_KILL_HUMAN;
						else new_flag = TR_SLAY_HUMAN;
					}
				}
				if((r_ptr->flags3 & RF3_ANG_CHAOS) && one_in_(2))
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_EVIL)) new_flag = TR_KILL_EVIL;
						else new_flag = TR_SLAY_EVIL;
					}
				}
				if((r_ptr->flags3 & RF3_ANG_COSMOS) && one_in_(2))
				{
					cnt++;
					if(one_in_(cnt))
					{
						if(have_flag(flgs,TR_SLAY_GOOD)) new_flag = TR_KILL_GOOD;
						else new_flag = TR_SLAY_GOOD;
					}
				}
			}
			break;

		case 3: //ランダムにpvalを付与する 魔道具能力は付与されない
//msg_print("chk:Enter:newPval");
			{

				while(1)
				{
					new_flag = TR_STR + randint0(13);
					if(new_flag != TR_MAGIC_MASTERY) break;
				}
			}
			break;

		case 4: case 5: //属性か攻撃能力を付与する
//msg_print("chk:Enter:newBrand");
			{
				while(1)
				{
					new_flag = TR_CHAOTIC + randint0(11);
					if(!object_has_a_blade(o_ptr) && (new_flag == TR_EX_VORPAL || new_flag == TR_VORPAL) ) continue;
					if(object_has_a_blade(o_ptr) && (new_flag == TR_IMPACT) ) continue;
					if(new_flag == TR_VORPAL && have_flag(flgs,TR_VORPAL)) new_flag = TR_EX_VORPAL;
					else if(new_flag == TR_EX_VORPAL && !have_flag(flgs,TR_VORPAL)) new_flag = TR_VORPAL;

					break;
				}
			}
			break;

		case 6://その他の能力をランダムに付与する
//msg_print("chk:Enter:newSkill");
			{
				int chance = randint1(32);
				if(chance < 18) new_flag = TR_ESP_EVIL + randint0(17);
				else if(chance < 26) new_flag = TR_LEVITATION + randint0(8);
				else new_flag = TR_SPEEDSTER + randint0(7);
			}
			break;

		case 7://耐性をランダムに付与する 免疫は付きにくい
//msg_print("chk:Enter:newRes");
			{
				while(1)
				{
					new_flag = TR_IM_ACID + randint0(24);
					if(new_flag >= TR_IM_ACID && new_flag <= TR_IM_COLD && !weird_luck()) continue;
					break;
				}
			}
			break;
		default:
			msg_format("ERROR:make_evilweaponのselect値(%d)がおかしい",select);

		}
		//既に持っている能力に決まったらやり直す
		if(new_flag >= 0 && !have_flag(flgs,new_flag)) break;
		else new_flag = -1;
	}

	if(!attempt) //10回試行して能力が決まらなかったとき代わりにダイスボーナス
	{
		o_ptr->dd += 3;
	}
	else
	{
		//選択された能力を付与
		add_flag(o_ptr->art_flags, new_flag);

		//追加攻撃などのpval調整
		if(new_flag == TR_BLOWS)
		{
			if(make_1st) o_ptr->pval = 1;
			else o_ptr->pval = 2;
		}
		else if(is_pval_flag(new_flag))
		{
			if(have_flag(flgs,TR_BLOWS))
			{
				if(o_ptr->pval > 1) ; //追加攻撃エゴなど既に2以上のpvalのときはpvalを触らない
				else if(make_1st) o_ptr->pval = 1;
				else o_ptr->pval = 2;
			}
			else
			{
				int tmp = 1 + randint1(3);
				o_ptr->pval = MAX(o_ptr->pval, tmp);
			}
		}
		//一度目で追加攻撃が付いて二度目でpval関係ないのが付いた時もpval2になる。追加攻撃や聖戦者エゴで+1攻のときも+2になってしまうがまあいいか。
		else if(have_flag(flgs,TR_BLOWS) && !make_1st && o_ptr->pval == 1)
		{
			o_ptr->pval = 2;
		}
	}
	//雷鼓処理追加　付喪神使いほどには強くならない
	if(p_ptr->pclass == CLASS_RAIKO)
	{
		if(make_1st)
		{
			o_ptr->to_h += randint1(3);
			o_ptr->to_d += randint1(3);
			msg_format("%sの魔力を使いこなせるようになってきた気がする。",o_name);
		}
		else
		{
			o_ptr->dd += 1;
			o_ptr->to_h += randint1(5);
			o_ptr->to_d += randint1(5);
			msg_format("%sの魔力を完全に使いこなせるようになった！",o_name);
		}
	}
	else
	{
		if(make_1st)
		{
			o_ptr->to_h += randint1(5);
			o_ptr->to_d += randint1(5);
			msg_format("モンスターの血を吸い続けた%sは妖器化した！",o_name);
		}
		else
		{
			o_ptr->dd += 2;
			o_ptr->to_h += randint1(5);
			o_ptr->to_d += randint1(5);
			msg_format("%sは更なる力を得た！",o_name);
		}
	}

	p_ptr->window |= (PW_INVEN | PW_EQUIP);
	p_ptr->update |= (PU_BONUS);
	calc_android_exp();

}

/*:::難易度調整チェック　FALSEのときこのモンスターを発生抑止する。check_restでFALSEでRESTRICTするってのも文章がおかしい気がするが。
 *FLAG_50Fフラグの設定された強敵はEASYでは常に抑止されNORMALでは紫を倒すまで抑止される
 */
bool check_rest_f50(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if(!(r_ptr->flags1 & RF1_FLAG_50F)) return TRUE;

	if(difficulty == DIFFICULTY_EASY)
		return FALSE;
	else if(difficulty == DIFFICULTY_NORMAL)
	{
		if(quest[QUEST_YUKARI].status == QUEST_STATUS_FINISHED) return TRUE;
		else return FALSE;
	}
	else return TRUE;

}


/*:::フロアのアイテムを走査し、モードに合致し一番近いものの距離を大まかに知らせる。*/
/*:::mode:1.高級品 2.アーティファクト 3.素材 4.左全てと珍品 5.呪われたアイテム 6.素材とキノコ 7:珍品 8:食べられるもの 9:魔力を帯びたもの*/
void search_specific_object(int mode)
{
	int i;
	int count = 0;
	int temp_dist = 999, temp_dist_new;

	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];
		if (!o_ptr->k_idx) continue;
//object_desc(tmp_str, o_ptr, OD_STORE);
//msg_format("%s..", tmp_str);
		if (o_ptr->held_m_idx) continue;
		if (o_ptr->marked & (OM_TOUCHED))continue;

		if(mode == 1)
		{
			if (!object_is_ego(o_ptr)) continue;
		}
		else if(mode == 2)
		{
			if (!object_is_artifact(o_ptr)) continue;
		}
		else if(mode == 3)
		{
			if (o_ptr->tval != TV_MATERIAL) continue;
			if (o_ptr->sval == SV_MATERIAL_STONE || o_ptr->sval == SV_MATERIAL_SCRAP_IRON) continue;
		}
		else if(mode == 4)
		{
			if (!object_is_ego(o_ptr) && !object_is_artifact(o_ptr) && o_ptr->tval != TV_MATERIAL  && o_ptr->tval != TV_SOUVENIR) continue;
		}
		else if(mode == 5)
		{
			if (!object_is_cursed(o_ptr)) continue;
		}
		else if(mode == 6)
		{
			if (o_ptr->tval != TV_MATERIAL && o_ptr->tval != TV_MUSHROOM) continue;
			if (o_ptr->tval == TV_MATERIAL && (o_ptr->sval == SV_MATERIAL_STONE || o_ptr->sval == SV_MATERIAL_SCRAP_IRON)) continue;
		}
		else if (mode == 7)
		{
			if (o_ptr->tval != TV_SOUVENIR) continue;
		}
		else if (mode == 8)
		{
			if (!object_is_eatable(o_ptr)) continue;
		}
		else if (mode == 9)
		{
			if (!object_is_magic_item(o_ptr)) continue;
		}

		else
		{
			msg_print("ERROR:search_specific_object()に定義されていないmode値が渡された");
			return;
		}

		count++;
		temp_dist_new = distance(py,px,o_ptr->iy,o_ptr->ix);
		if(temp_dist_new < temp_dist)	temp_dist = temp_dist_new;
	}
	if(count < 1)
	{
		if(mode == 1) msg_format("この階には匠の仕事と呼べるものはないようだ。");
		if(mode == 2) msg_format("この階には伝説の宝と呼べるものはないようだ。");
		if(mode == 3) msg_format("この階には素材などはなさそうな感じだ。");
		if(mode == 4) msg_format("しかし興味を惹くものは見当たらなかった。");
		if(mode == 5) msg_format("この階には厄に満ちた物品はないようだ。");
		if(mode == 6) msg_format("この階の植生にはとくに興味を引くものはない。");
		if(mode == 7) msg_format("この階には興味を引くものはなさそうだ。");
		if(mode == 8) msg_format("この辺りには食べられそうなものは見当たらない。");
		if(mode == 9) msg_format("この辺りに魔力の残滓は感知されなかった。");
	}
	else
	{
		char msg_mode[40];
		char msg_dist[16];

		if(mode == 1) my_strcpy(msg_mode,"匠の仕事の痕跡",sizeof(msg_mode)-2);
		if(mode == 2) my_strcpy(msg_mode,"伝説との邂逅",sizeof(msg_mode)-2);
		if(mode == 3) my_strcpy(msg_mode,"特産物の匂い",sizeof(msg_mode)-2);
		if(mode == 4) my_strcpy(msg_mode,"何かの痕跡",sizeof(msg_mode)-2);
		if(mode == 5) my_strcpy(msg_mode,"厄の気配",sizeof(msg_mode)-2);
		if(mode == 6) my_strcpy(msg_mode,"特徴的な痕跡",sizeof(msg_mode)-2);
		if(mode == 7) my_strcpy(msg_mode,"珍しい物品の気配", sizeof(msg_mode) - 2);
		if(mode == 8) my_strcpy(msg_mode,"食べられそうな物", sizeof(msg_mode) - 2);
		if(mode == 9) my_strcpy(msg_mode,"魔力を帯びた物", sizeof(msg_mode) - 2);

		if(temp_dist < 15) my_strcpy(msg_dist,"近い！",sizeof(msg_dist)-2);
		else if(temp_dist < 30) my_strcpy(msg_dist,"やや近い。",sizeof(msg_dist)-2);
		else if(temp_dist < 60) my_strcpy(msg_dist,"やや遠い。",sizeof(msg_dist)-2);
		else if(temp_dist < 100) my_strcpy(msg_dist,"遠い。",sizeof(msg_dist)-2);
		else  my_strcpy(msg_dist,"かなり遠い。",sizeof(msg_dist)-2);

		if(mode == 8 || mode == 9)
			msg_format("蜂たちが%sを見つけた。%s",msg_mode, msg_dist);
		else
			msg_format("%sを感じる。%s", msg_mode, msg_dist);

	}
}

/*:::岩食い　変異だけでなくゴーレムも使うため別関数にした*/
bool eat_rock(void)
{
	int dir;
	int x, y;
	cave_type *c_ptr;
	feature_type *f_ptr, *mimic_f_ptr;

	if (!get_rep_dir2(&dir)) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];
	c_ptr = &cave[y][x];
	f_ptr = &f_info[c_ptr->feat];
	mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];

	mutation_stop_mouth();

	if (!have_flag(mimic_f_ptr->flags, FF_HURT_ROCK))
	{
		msg_print("この地形は食べられない。");
		return FALSE;
	}
	else if (have_flag(f_ptr->flags, FF_PERMANENT))
	{
		msg_format("いてっ！この%sはあなたの歯より硬い！", f_name + mimic_f_ptr->name);
		return FALSE;
	}
	else if (c_ptr->m_idx)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];
		msg_print("何かが邪魔しています！");
		if (!m_ptr->ml || !is_pet(m_ptr))
		{
			py_attack(y, x, 0);
			return TRUE;
		}
		else return FALSE;
	}
	else if (have_flag(f_ptr->flags, FF_TREE))
	{
		msg_print("木の味は好きじゃない！");
		return FALSE;
	}
	else if (have_flag(f_ptr->flags, FF_GLASS))
	{
		msg_print("ガラスの味は好きじゃない！");
		return FALSE;
	}
	else if (have_flag(f_ptr->flags, FF_DOOR) || have_flag(f_ptr->flags, FF_CAN_DIG))
	{
		(void)set_food(p_ptr->food + 3000);
	}
	else if (have_flag(f_ptr->flags, FF_MAY_HAVE_GOLD) || have_flag(f_ptr->flags, FF_HAS_GOLD))
	{
		(void)set_food(p_ptr->food + 5000);
	}
	else
	{
		msg_format("この%sはとてもおいしい！", f_name + mimic_f_ptr->name);
		(void)set_food(p_ptr->food + 10000);
	}

	/* Destroy the wall */
	cave_alter_feat(y, x, FF_HURT_ROCK);
	/* Move the player */
	(void)move_player_effect(y, x, MPE_DONT_PICKUP);

	return TRUE;

}

/*:::＠が赤蛮奇のとき、行動開始時に頭の分身の視界情報を更新*/
/*:::妖夢の分身も同じように処理することにした。*/
void update_minion_sight(void)
{
	int x, y, x2, y2,i,j;
	s16b            feat;
	bool flag = FALSE; //該当配下がいるときマップ再描画するフラグ
	feature_type    *f_ptr;

	if(p_ptr->pclass != CLASS_BANKI && p_ptr->pclass != CLASS_YOUMU) return;

	for(i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		if(!is_pet(m_ptr)) continue;
		if (p_ptr->pclass == CLASS_BANKI)
		{
			if(m_ptr->r_idx != MON_BANKI_HEAD_1 && m_ptr->r_idx != MON_BANKI_HEAD_2) continue;
		}
		else if(p_ptr->pclass == CLASS_YOUMU)
		{
			if(m_ptr->r_idx != MON_YOUMU || !(m_ptr->mflag & MFLAG_EPHEMERA)) continue;
		}
		else msg_format("ERROR:配下視界アップデート処理がおかしい");
		flag = TRUE;


		x2 = m_ptr->fx;
		y2 = m_ptr->fy;
		/*地形感知*/
		for (y = 1; y < cur_hgt - 1; y++)
		{
			for (x = 1; x < cur_wid - 1; x++)
			{
				cave_type *c_ptr = &cave[y][x];
				if(distance(y,x,y2,x2) > MAX_SIGHT) continue;
				if(!los(y,x,y2,x2)) continue;

				/* Memorize terrain of the grid */
				c_ptr->info |= (CAVE_KNOWN);

				/* Feature code (applying "mimic" field) */
				feat = get_feat_mimic(c_ptr);
				f_ptr = &f_info[feat];

				/* All non-walls are "checked" */
				if (!have_flag(f_ptr->flags, FF_WALL))
				{
					/* Memorize normal features */
					if (have_flag(f_ptr->flags, FF_REMEMBER))
					{
						/* Memorize the object */
						c_ptr->info |= (CAVE_MARK);
					}
					/* Memorize known walls */
					for (j = 0; j < 8; j++)
					{
						c_ptr = &cave[y + ddy_ddd[j]][x + ddx_ddd[j]];

						/* Feature code (applying "mimic" field) */
						feat = get_feat_mimic(c_ptr);
						f_ptr = &f_info[feat];

						/* Memorize walls (etc) */
						if (have_flag(f_ptr->flags, FF_REMEMBER))
						{
							/* Memorize the walls */
							c_ptr->info |= (CAVE_MARK);
						}
					}
				}
			}
		}
		/*アイテム感知*/
		for (j = 1; j < o_max; j++)
		{
			object_type *o_ptr = &o_list[j];
			if (!o_ptr->k_idx) continue;
			if (o_ptr->held_m_idx) continue;
			y = o_ptr->iy;
			x = o_ptr->ix;

			if(!los(y,x,y2,x2)) continue;
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
		//	lite_spot(y, x);

		}

	}
	if(flag) prt_map();
	return;
}

/*:::ミスティアの歌で鳥目になっているかどうかの判定*/
///mod150308 にとりの光学迷彩もこれで判定することにした
///mod150801 サニーの幻惑Ⅱも同じ
//v1.1.22 兵士の夜戦技能チェックにも使う
bool check_mon_blind(int m_idx)
{
	monster_type    *m_ptr; 
	monster_race    *r_ptr;
	int chance;

	if(m_idx <= 0)
	{
		msg_print("ERROR:check_mon_blind()に渡されたm_idx値がおかしい");
		return FALSE;
	}
	m_ptr = &m_list[m_idx];
	r_ptr = &r_info[m_ptr->r_idx];

	//視界に全く頼ってなさそうなシンボルはFALSE
	if(my_strchr("bjmsvQV", r_ptr->d_char)) return FALSE;
	if( !isalpha(r_ptr->d_char)) return FALSE;

	//ミスティア夜雀の歌は魅力と部屋の明るさが大きく影響
	if(p_ptr->pclass == CLASS_MYSTIA && music_singing(MUSIC_NEW_MYSTIA_YAZYAKU))
	{
		chance = p_ptr->lev*3 + (p_ptr->skill_stl+10)*4 + adj_general[p_ptr->stat_ind[A_CHR]] * 5;
		if (cave[py][px].info & CAVE_GLOW) chance /= 3;
		else if(p_ptr->cur_lite == 0) chance *= 3;
	}
	else if(p_ptr->pclass == CLASS_SOLDIER)//兵士のノクトビジョン判定　EXTRAモードで光学迷彩とか使ったときもこっちの式になるがまあいいか
	{
		chance = p_ptr->lev*6 + (p_ptr->skill_stl+5)*6;
	}
	else //ほかにとりの光学迷彩、その効果のあるEXTRAアイテムカード、サニー幻惑のとき
	{
		chance = p_ptr->lev*3 + (p_ptr->skill_stl+5)*3;
		if(chance < 50) chance=50;
	}

	if(my_strchr("iwEJLSUWXZ", r_ptr->d_char)) chance /= 3;
	else if(r_ptr->flags3 & (RF3_UNDEAD)) chance /= 3;//v2.1.4 演算子ミス修正
	else if(my_strchr("acfgklqruyACDGIY", r_ptr->d_char)) chance = chance / 2;

	if (r_ptr->flags2 & RF2_SMART) chance /= 2;
	if (p_ptr->cursed & TRC_AGGRAVATE) chance /= 5;
	if (r_ptr->level > p_ptr->lev * 2) chance /= 3;

	return (randint0(chance) > r_ptr->level);

}

//充填アイテム選定のために機械のsvalを保持するグローバル変数を作ってしまおう。externは必要か？
int item_tester_engineer_supply_num = 0;

/*:::エンジニアが消耗品を補充するためのhook グローバル変数使用*/
bool item_tester_hook_engineer_supply(object_type *o_ptr)
{
	if(o_ptr->tval == machine_table[item_tester_engineer_supply_num].tval && o_ptr->sval == machine_table[item_tester_engineer_supply_num].sval) return TRUE;
	else return FALSE;
}


/*:::エンジニアの機械の射撃部分。一斉射撃ルーチンと共用するため分けてある。射撃武器でない場合FALSEが返る*/
bool fire_machine_aux(int sv, int dir)
{

	switch(sv)
	{
	case SV_MACHINE_WATERGUN:
		msg_print("水鉄砲を撃った。");
		fire_bolt(GF_WATER, dir, randint1(10));
		break;
	case SV_MACHINE_FLAME: 
		msg_print("火炎を放射した！");
		fire_ball(GF_FIRE, dir, (25+randint1(50)), -2);
		break;
	case SV_MACHINE_TORPEDO:
		msg_print("魚雷を発射した！");
		fire_rocket(GF_ROCKET, dir, 250, 1);
		break;
	case SV_MACHINE_MOTORGUN:
		msg_print("マシンガンを乱射した！");
		fire_blast(GF_ARROW, dir, 2, 20, 10, 2, 0);
		break;
	case SV_MACHINE_LASERGUN:
		msg_print("レーザーを撃った！");
		fire_beam(GF_LITE, dir, 150);
		break;
	case SV_MACHINE_PLASMAGUN:
		msg_print("プラズマを放った！");
		fire_bolt(GF_PLASMA, dir, 300);
		break;
	case SV_MACHINE_GATLINGGUN:
		msg_print("ガトリング砲が火を噴いた！");
		fire_blast(GF_ARROW, dir, 2, 20, 20 + randint0(11), 3, 0);
		break;
	case SV_MACHINE_CANNON:
		msg_print("轟音と共に砲弾が撃ち出された！");
		fire_ball(GF_ARROW, dir, 400+randint1(200), 2);
		break;
	case SV_MACHINE_ROCKET_6:
		msg_print("ロケット弾が炸裂した！");
		fire_rocket(GF_ROCKET, dir, 400, 1);
		break;
	case SV_MACHINE_NEUTRINOGUN:
		msg_print("不可視の高エネルギー体が放たれた！");
		fire_beam(GF_PSY_SPEAR, dir, 180 + randint1(180));
		break;
	case SV_MACHINE_WAVEMOTION:
		msg_print("高圧縮タキオン粒子がダンジョンを貫いた！");
		fire_spark(GF_DISINTEGRATE,dir,damroll(50,50),2);
		break;
	case SV_MACHINE_N_E_P:
		msg_print("因果律の修正が発動した！");
		fire_spark(GF_N_E_P,dir,9999,2);
		break;

	default:
		return FALSE;
	}



return TRUE;

}
/*:::エンジニア専用　機械を使う mode=0のときはアイテムの説明を見るだけで1のときは弾薬を補充する。 行動順消費しないときFALSEが返る*/
/*:::射撃用機械はfire_machine_aux(slot,dir)に渡す。一斉射撃ルーチンと共用するため*/
bool use_machine(int mode)
{
	int inven2_num = calc_inven2_num();
	int item, amt, i, tmp;
	char c;
	cptr    q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	bool only_browse = FALSE,supply = FALSE, use_machine = FALSE;

	cptr pack = (p_ptr->pclass == CLASS_NITORI)?"リュック":"バックパック";

	if(mode == 0) only_browse = TRUE;
	else if(mode == 1) supply = TRUE;
	else use_machine = TRUE;

	//Paranoia
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI) return FALSE; 

	screen_save();


	item = 0;
#ifdef ALLOW_REPEAT
	if (!repeat_pull(&item))
#endif
	{
		do
		{
			(void)show_inven2(SI2_MODE_ENGINEER);
			if(only_browse) prt("どの機械を確認しますか？", 0, 0);
			else if(supply) prt("どの機械へ補充を行いますか？", 0, 0);
			else prt("どの機械を使いますか？", 0, 0);
			c = inkey();
			if(c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
		}while(c < 'a' || c >= ('a' + inven2_num));
		item = c-'a';
#ifdef ALLOW_REPEAT
		repeat_push(item);
#endif
	}

	o_ptr = &inven_add[ item ];
	if(!o_ptr->k_idx)
	{
		msg_format("そこには何も入っていない。");
		screen_load();
		return FALSE;
	}
	if(only_browse)
	{
		screen_object(o_ptr, SCROBJ_FORCE_DETAIL);
		screen_load();
		return FALSE;
	}
	else if(o_ptr->xtra4)
	{
		msg_format("その機械は今故障中だ。");
		screen_load();
		return FALSE;
	}

	else if(supply && !machine_table[o_ptr->sval].tval)
	{
		msg_format("その機械には補充ができない。");
		screen_load();
		return FALSE;
	}
	else if(supply && machine_table[o_ptr->sval].charge == o_ptr->pval)
	{
		msg_format("その機械はすでに補充が済んでいる。");
		screen_load();
		return FALSE;
	}
	else if(use_machine && !o_ptr->pval)
	{
		switch(o_ptr->sval)
		{
		case SV_MACHINE_WATERGUN:
			msg_print("タンクが空だ。"); break;
		case SV_MACHINE_SHIRIKO:
		case SV_MACHINE_FLAME:
		case SV_MACHINE_J_HAMMER:
		case SV_MACHINE_ROCKETEER:
			msg_print("燃料切れだ！"); break;
		case SV_MACHINE_DRILLARM:
		case SV_MACHINE_RADAR1:
		case SV_MACHINE_RADAR2:
		case SV_MACHINE_SCOUTER:
			msg_print("バッテリー切れだ！"); break;
		case SV_MACHINE_MOTORGUN:
		case SV_MACHINE_GATLINGGUN:
			msg_print("弾切れだ！"); break;
		case SV_MACHINE_CANNON:
			msg_print("まだ次弾を装填していない。"); break;

		default:
			msg_print("エネルギー切れだ！"); break;
		}
		screen_load();
		return FALSE;
	}

	screen_load();

	if(supply) //機械充填時
	{
		feature_type *f_ptr = &f_info[cave[py][px].feat];
		//水鉄砲に水辺で充填する処理
		if( (o_ptr->sval == SV_MACHINE_WATERGUN) && have_flag( f_ptr->flags, FF_WATER))
		{
			msg_print("その辺の水を水鉄砲のタンクにブクブク入れた。");
		}
		else
		{
			item_tester_engineer_supply_num = o_ptr->sval;

			item_tester_hook = item_tester_hook_engineer_supply;

			q = "どれを使って補充しますか？ ";
			s = "この機械の補充用アイテムを持っていない。";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;
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
			switch(o_ptr->sval)
			{
			case SV_MACHINE_WATERGUN:
				msg_print("水鉄砲に水を入れた。"); break;
			case SV_MACHINE_SHIRIKO:
			case SV_MACHINE_FLAME:
			case SV_MACHINE_J_HAMMER:
			case SV_MACHINE_ROCKETEER:
				msg_print("燃料を補給した。"); break;
			case SV_MACHINE_DRILLARM:
			case SV_MACHINE_RADAR1:
			case SV_MACHINE_RADAR2:
			case SV_MACHINE_SCOUTER:
				msg_print("バッテリーを交換した。"); break;
			case SV_MACHINE_MOTORGUN:
			case SV_MACHINE_GATLINGGUN:
				msg_print("弾倉を交換した。"); break;
			case SV_MACHINE_CANNON:
				msg_print("砲弾を込めた。"); break;

			default:
				msg_print("エネルギーパックを取り換えた。"); break;

			}
		}

		o_ptr->pval = machine_table[o_ptr->sval].charge;

	}
	else //機械使用時
	{
		int dir,x,y,v;
		int sv = o_ptr->sval;
		switch(sv)
		{
		case  SV_MACHINE_WATERGUN:
		case  SV_MACHINE_FLAME:
		case  SV_MACHINE_TORPEDO:
		case  SV_MACHINE_MOTORGUN:
		case  SV_MACHINE_LASERGUN:
		case SV_MACHINE_PLASMAGUN:
		case SV_MACHINE_GATLINGGUN:
		case SV_MACHINE_CANNON:
		case SV_MACHINE_ROCKET_6:
		case SV_MACHINE_NEUTRINOGUN:
		case SV_MACHINE_WAVEMOTION:
		case SV_MACHINE_N_E_P:

			if (!get_aim_dir(&dir)) return FALSE;
			fire_machine_aux(sv,dir);

			///mod150712 性格トリガーハッピーの射撃速度ボーナス
			if(p_ptr->pseikaku == SEIKAKU_TRIGGER) 
				new_class_power_change_energy_need = 80;

			break;
		case  SV_MACHINE_SHIRIKO:
			msg_format("%sから大量のアームと奇妙な器具が飛び出し、空中で気味の悪い動きをした・・",pack);
			turn_monsters(50);
			break;
		case  SV_MACHINE_LIGHTBOMB:
			msg_print("閃光弾を放った！");
			lite_area(50, 5);
			confuse_monsters(100);
			break;
		case SV_MACHINE_DRILLARM:
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir == 5) return FALSE;
			msg_format("%sから激しく回転するドリルが飛び出した！",pack);
			y = py + ddy[dir];
			x = px + ddx[dir];
			project(0, 0, y, x, 0, GF_KILL_WALL, PROJECT_GRID, -1);
			project(0, 0, y, x, damroll(10,10), GF_MISSILE, PROJECT_KILL, -1);
			break;
		case  SV_MACHINE_RADAR1:
			detect_monsters_living(DETECT_RAD_DEFAULT);
			break;
		case SV_MACHINE_J_HAMMER:
			if (!get_rep_dir2(&dir)) return FALSE;
			if(dir == 5) return FALSE;
			msg_format("%sから火を噴く巨大なハンマーが飛び出した！",pack);
			y = py + ddy[dir];
			x = px + ddx[dir];
			project(0, 0, y, x, 150 + randint1(150), GF_MISSILE, PROJECT_KILL, -1);
			earthquake(y, x, 5);
			break;
		case SV_MACHINE_ROCKETEER:
			if(!rush_attack3(15,"あなたはジェット噴射で吹き飛んだ！",damroll(10,10))) return FALSE;
			break;
		case SV_MACHINE_SCOUTER:
			scouter_use = TRUE;
			probing(); 
			//Hack - グローバル変数使って高レベルな敵相手だと壊れるようにした
			if(!scouter_use)
			{
				msg_print("スカウターは爆発して壊れた！");
				object_wipe(o_ptr);
				return TRUE;
			}
			scouter_use = FALSE;

			break;
		case SV_MACHINE_BARRIER1:
			msg_print("虹色のバリアを張った！");
			v = randint1(20) + 20;
			set_oppose_acid(v, FALSE);
			set_oppose_elec(v, FALSE);
			set_oppose_fire(v, FALSE);
			set_oppose_cold(v, FALSE);
			set_oppose_pois(v, FALSE);
			break;
		case SV_MACHINE_MEGACLASH:
			msg_print("メガクラッシュを上空へ射出した。");
			if(dun_level) destroy_area(py, px, 20 + randint1(10), FALSE,FALSE,FALSE);
			else if(turn % (TURNS_PER_TICK * TOWN_DAWN) >= 50000)
				msg_print("夜空に流星群のような光の花が咲いた。");
			else 
				msg_print("空に流星群のような光の花が咲いた。");
			break;
		case SV_MACHINE_RADAR2:
			msg_format("%sからセンサが何本も飛び出した。",pack);
			set_radar_sense(20 + randint1(20), FALSE);
			break;
		case SV_MACHINE_TELEPORTER:
				msg_print("テレポーターを起動した・・");
				if (!dimension_door(D_DOOR_MACHINE)) return FALSE;
			break;
		case SV_MACHINE_PHONON_MASER:
			msg_format("%sが開き、振動体が光り始めた・・・",pack);
			project_hack2(GF_SOUND, 0,0,200);
			break;

		case SV_MACHINE_ACCELERATOR:
			msg_print("周囲の動きがゆっくりして見える・・・");
			v = randint1(25) + 25;
			set_fast(v, FALSE);
			break;

		case SV_MACHINE_BARRIER2:
			msg_print("何かに守られている感じがする・・");
			v = randint1(20) + 20;
			set_resist_magic(v, FALSE);
			break;

		case SV_MACHINE_BARRIER3:
			msg_print("斥力フィールドを展開した！");
			v = randint1(20) + 20;
			set_tim_reflect(v, FALSE);
			set_shield(v, FALSE);
			break;

		case SV_MACHINE_MIDAS_HAND:
			if (!alchemy(0)) return FALSE; 
			break;

		case SV_MACHINE_E_CAN:
			msg_print("取っておきのE缶を使った！");
			hp_player(5000);
			set_cut(0);
			break;

		case SV_MACHINE_TWILIGHT_ZONE:
			{
				int percentage;
				percentage = p_ptr->chp * 100 / p_ptr->mhp;
				v = randint1(16) + 16;
				msg_print("発動した・・");
				msg_print(NULL);
				msg_print("ここは不思議空間トワイライトゾーン！"); 
				msg_print("あなたのパワーが三倍になった気がする！"); 
				set_lightspeed(v,FALSE);
				for(i=0;i<6;i++)set_tim_addstat(i,120,v,FALSE);
				p_ptr->chp = p_ptr->mhp * percentage / 100;
				p_ptr->redraw |= (PR_HP );
				redraw_stuff();

			}
			break;

		default:
			msg_format("ERROR:機械(sval:%d)の使用時効果が定義されていない",o_ptr->sval);
			return FALSE;
		}
		/*:::pval(使用回数)を1減少。充填アイテムが設定されていない使い捨て品がpval0になった場合アイテムを消去する。エンジニアなのでバックパックの重量を考慮しない。*/
		o_ptr->pval--;
		if((!o_ptr->pval) && (!machine_table[o_ptr->sval].tval)) object_wipe(o_ptr);
	}



	return TRUE;
}


/*:::エンジニアの一斉射撃コマンド 行動順消費するときTRUE*/
bool engineer_triggerhappy(void)
{
	int fire_cnt = 0;
	int inven2_num = calc_inven2_num();
	int dir, i;
	object_type *o_ptr;

	if(p_ptr->pclass != CLASS_ENGINEER)
	{
		msg_print("ERROR:非対応の職でtriggerhappy()が呼ばれた");
		return FALSE;
	}

	if (!get_aim_dir(&dir)) return FALSE;
	screen_save();
	(void)show_inven2(SI2_MODE_ENGINEER);
	if (!get_check_strict("一斉射撃しますか？", CHECK_DEFAULT_Y))
	{
		screen_load();
		return FALSE;
	}
	screen_load();

	msg_print("バックパックの全武装が展開された！");

	for(i=0;i<inven2_num;i++)
	{
		o_ptr = &inven_add[i];

		if(!o_ptr->k_idx) continue; //空欄
		if(!o_ptr->pval) continue; //弾切れ
		if(o_ptr->xtra4) continue; //故障中
		if(o_ptr->tval != TV_MACHINE) msg_print("ERROR:バックパックに機械以外が入った状態でengineer_triggerhappy()が呼ばれた");

		/*:::射撃しない機械はFALSEが返るので射撃した武器のみ使用回数減少やカウント処理*/
		if(fire_machine_aux(o_ptr->sval, dir))
		{
			fire_cnt++;
			o_ptr->pval--;
			//補充不可の機械が回数0になったら消滅　エンジニアは重量を考慮しない
			if((!o_ptr->pval) && (!machine_table[o_ptr->sval].tval)) object_wipe(o_ptr); 
		}
		if(dir == 5 && !target_okay()) break; //ターゲットが倒れたら終わり
	}

	if(!fire_cnt)
	{
		msg_print("しかし射撃できる機械を装備していなかった・・");
	}
	else
	{
		//energy_useを再設定する変数
		new_class_power_change_energy_need = 100 + fire_cnt * 25; 
		///mod150712 トリガーハッピーの射撃ボーナス
		if(p_ptr->pseikaku == SEIKAKU_TRIGGER) 
			new_class_power_change_energy_need = new_class_power_change_energy_need * 4 / 5;

	}

	return TRUE;

}



/*:::エンジニアの機械が水や電撃で一時使用不能になる処理。判定が通るとxtra4が加算され、0に戻るまでは使用・取り外し・補給不可*/
/*:::typ=0で呼ぶと回復処理。全ての装備中の機械のxtra4がdam(正) 値分減算される。時間経過、宿屋で呼ばれる*/
void engineer_malfunction(int typ, int dam)
{
	int inven2_num = calc_inven2_num();
	int dir, i;
	int chance;
	object_type *o_ptr;
	char o_name[MAX_NLEN];

	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI) return;
	if(dam <= 0) return;
	if(typ == GF_WATER && (p_ptr->prace == RACE_KAPPA  )) return; //河童の機械は防水　v1.1.86 山童は外した
	if(typ == GF_ELEC && (p_ptr->immune_elec || p_ptr->resist_elec && IS_OPPOSE_ELEC() )) return; //電撃の免疫か二重耐性があると故障を防げる

	for(i=0;i<inven2_num;i++)
	{
		o_ptr = &inven_add[i];
		if(!o_ptr->k_idx) continue;
		if(o_ptr->tval != TV_MACHINE)
		{
			msg_format("ERROR:機械以外(Tval:%d)に対しengineer_malfunction()が呼ばれている",o_ptr->tval);
			return;

		}
		//不調からの回復処理
		if(!typ)
		{
			if( o_ptr->xtra4)
			{
				o_ptr->xtra4 -= dam;
				if(o_ptr->xtra4 < 0) o_ptr->xtra4 = 0;
				if(!o_ptr->xtra4)
				{
					object_desc(o_name, o_ptr, 0);
					msg_format("%sは機能を回復した！",o_name);
				}
			}
		}
		else
		{
			if(typ == GF_WATER) chance = dam * machine_table[o_ptr->sval].hate_water / 100;
			else if(typ == GF_ELEC) chance = dam * machine_table[o_ptr->sval].hate_elec / 100;
			else
			{
				msg_format("ERROR:未定義の属性でengineer_malfunction()が呼ばれた");
				return;
			}
			if(randint0(100) < chance)
			{
				object_desc(o_name, o_ptr, 0);
				if(!o_ptr->xtra4) msg_format("%sが故障した！",o_name);
				else  msg_format("%sの調子がさらにおかしくなったようだ・・",o_name);
				o_ptr->xtra4 += chance;
				if(o_ptr->xtra4 > 10000) o_ptr->xtra4 = 10000;
			}
		}
	}

}



/*:::エンジニアギルドで消耗品を購入するときのための価格係数を変更する。ゲームスタート時,6:00,18:00に呼ばれる。*/
/*:::p_ptr->magic_num1[0]～[19]に対し、-50～+50の値を格納する。河童の里ではこの範囲で常に-10される。*/
void engineer_guild_price_change(void)
{
	int i, tmp;
	if(p_ptr->pclass != CLASS_ENGINEER && p_ptr->pclass != CLASS_NITORI) return;

	for(i=0;i<20;i++) 
	{
		/*:::3/7で変化なし、3/7で前の値へ±5され、1/7で全く新しい値になる。新しい値は中心に偏り、安くはなりにくい。*/
		if(one_in_(7))
		{
			tmp = damroll(2,50) - 50;
			if(tmp<0) tmp = 0;
			else if(one_in_(3)) tmp = 0 - tmp;
			p_ptr->magic_num1[i] = tmp;
		}
		else if(one_in_(2))
		{
			tmp = p_ptr->magic_num1[i] + randint0(11) - 5;
			if(tmp < -50) tmp = -50;
			if(tmp > 50) tmp = 50;
			p_ptr->magic_num1[i] = tmp;
		}

	}
}

















/*:::突進ルーチン3 敵を弾き飛ばしてダメージを与えつつ移動するが巨大な敵には止められる*/
/*:::length:射程 msg:発動時のセリフ dam:ダメージ*/
bool rush_attack3(int length ,cptr msg, int dam)
{
	int dir;
	int tx, ty;
	u16b path_g[32];
	int path_n, i;
	bool moved = FALSE;

	project_length = length;
	if (!get_aim_dir(&dir)) return FALSE;
	tx = px + project_length * ddx[dir];
	ty = py + project_length * ddy[dir];
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	if (in_bounds(ty, tx)) msg_format("%s",msg);

	path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_THRU | PROJECT_KILL);
	project_length = 0;
	if (!path_n) return FALSE;
	ty = py;
	tx = px;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];
		bool stop = FALSE;

		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);
	
		if (cave[ny][nx].m_idx)
		{
			m_ptr = &m_list[cave[ny][nx].m_idx];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			if(d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
				msg_format("なぜか%sの直前で足が止まった。",m_name);
				break;
			}						
			else if(r_ptr->flags2 & RF2_GIGANTIC)
			{			
				msg_format("%sに突進を止められた！",m_name);
				(void)project(0, 0, ny, nx, p_ptr->lev * 3, GF_MISSILE, PROJECT_HIDE|PROJECT_KILL, -1);
				break;
			}
			else 
			{
				msg_format("%sを弾き飛ばした！",m_name);
				(void)project(0, 0, ny, nx, dam, GF_MISSILE, PROJECT_HIDE|PROJECT_KILL, -1);
				if(cave[ny][nx].m_idx) teleport_away(cave[ny][nx].m_idx, 5,TELEPORT_PASSIVE);
			}
		}
		if (cave_empty_bold(ny, nx) && player_can_enter(cave[ny][nx].feat, 0))
		{
			ty = ny;
			tx = nx;
			if (!player_bold(ty, tx)) teleport_player_to(ty, tx, TELEPORT_NONMAGICAL);
		}
		else 
		{
			msg_print("ここには入れない。");
			break;
		}

	}
	return TRUE;
}



/*:::キスメ落下格闘攻撃のボーナス値*/
int calc_kisume_bonus(void)
{
	object_type *o_ptr = &inventory[INVEN_BODY];
	int bonus=0;

	if(!character_generated) return 0;
	if(p_ptr->pclass != CLASS_KISUME){msg_print("ERROR:calc_kisume_bonus()がキスメ以外で呼ばれた"); return 0;}
	if(o_ptr->tval != TV_ARMOR || o_ptr->sval != SV_ARMOR_TUB){msg_print("ERROR:calc_kisume_bonus()にて桶以外の防具が検出された"); return 0;}

	bonus += (o_ptr->ac + o_ptr->to_a) * 2 + o_ptr->to_d * 4;

	if(p_ptr->concent) bonus *= (1 + p_ptr->concent);

	return bonus;
	
}

/*:::集中度加算　キスメなど　行動順消費しない時FALSE*/
bool do_cmd_concentrate(int mode)
{
	int max;

	if (!(CLASS_USE_CONCENT)) msg_print("ERROR:CLASS_USE_CONCENTにない職業でdo_cmd_concentrate()が呼ばれている");

	p_ptr->concent++;
	switch(p_ptr->pclass)
	{
	case CLASS_KISUME:
		max = 1 + p_ptr->lev / 15;
		if(p_ptr->concent > max)
		{
			p_ptr->concent = max;
			msg_format("あなたは標的を待ち構えている・・(高度：%d)", p_ptr->concent);
		}
		else 
			msg_format("あなたは上昇していった・・(高度：%d)", p_ptr->concent);
		break;
	case CLASS_YUGI:
		if(p_ptr->concent == 1)
			msg_print("一歩目を踏み出した。空気が震えた。");
		else
			msg_print("二歩目を踏み出した。地面が揺れた。");

		break;
	case CLASS_SANAE:
		max = MAX(p_ptr->lev, 5);
		if(mode == p_ptr->magic_num1[0])
		{
			if(p_ptr->concent > max) p_ptr->concent = max;
			msg_format("詠唱を続けた・・");
			break;			
		}
		else
		{
			p_ptr->concent = 1;
			p_ptr->magic_num1[0] = mode;
			if(mode == CONCENT_SOMA) 
				msg_format("あなたは秘術の詠唱を開始した・・");
			else if(mode == CONCENT_KANA) 
				msg_format("あなたは乾の詠唱を開始した・・");
			else if(mode == CONCENT_SUWA) 
				msg_format("あなたは坤の詠唱を開始した・・");
		}
		break;
	case CLASS_MEIRIN:
		max = 50;
		if(mode == 1) p_ptr->concent += 4;
		if(p_ptr->concent > max) p_ptr->concent = max;
		break;

	case CLASS_BENBEN:
	case CLASS_YATSUHASHI:
			msg_print("清浄な琴と琵琶の音が辺りを包んだ・・");
		break;
	case CLASS_RAIKO:
		if(p_ptr->concent < 5)
			msg_print("リズムに乗って素早く攻撃した。");
		else if(p_ptr->concent < 10)
			msg_print("熱狂的な連撃を放った！");
		else
			msg_print("あなたのビートは最高潮だ！");

		break;
	case CLASS_YORIHIME:
		msg_print("祇園様の剣を地面に突き立てた。地面から無数の刃が生えてきた！");
		break;

	case CLASS_SOLDIER:
		max = 50;
		if(p_ptr->concent > max) 
		{
			p_ptr->concent = max;
			msg_print("あなたは限界までの集中を維持した..");
		}
		else
		{
			msg_print("あなたは集中して狙いを定めた..");

		}
		break;
	case CLASS_RESEARCHER:
		max = 1;
		if (p_ptr->concent > max) p_ptr->concent = max;

		p_ptr->magic_num1[0] = mode; //気合ため/魔力集中のどちらを使ってるかをmagic_num1[0]で管理

		if (mode == CONCENT_KANA)
		{
			p_ptr->concent = max;
			msg_print("あなたは大きく息を吸い込んだ！");
		}
		else
		{
			msg_print("あなたは魔力を集中した..");
		}
		break;

	case CLASS_ENOKO:
		max = 1;
		if (p_ptr->concent > max)
		{
			p_ptr->concent = max;
			msg_print("あなたは攻撃の機会を窺っている...");
		}
		else
		{
			msg_print("あなたは物陰に隠れて身を低くした。");
		}
		break;

	default:
		msg_print("ERROR:この職業のdo_cmd_concentrate()の処理が記述されていない");
		return FALSE;
	}

	reset_concent = FALSE;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);
	p_ptr->window |= (PW_EQUIP);
	p_ptr->redraw |= (PR_STATUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	return (TRUE);
}


/*:::はたての念写Ⅱ　キーワードを入力しそれに合致したものの周囲を撮影する*/
bool hatate_psychography(void)
{
	char temp[80] = "";
	int xx;
	char temp2[80];
	int i;
	int tx = -1, ty = -1;
	bool success = FALSE;
	int range = 7;

	if (!get_string("キーワードを入力：",temp, 70)) return FALSE;

	//フロアのモンスターを走査しキーワードに一致すれば座標を格納
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		m_ptr = &m_list[i];
		if(!m_ptr->r_idx) continue;
		r_ptr = &r_info[m_ptr->r_idx];

		for (xx = 0; temp[xx] && xx < 80; xx++)
		{
			if (iskanji(temp[xx]))
			{
				xx++;
				continue;
			}
			if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
		}
  
		strcpy(temp2, r_name + r_ptr->E_name);
		for (xx = 0; temp2[xx] && xx < 80; xx++)if (isupper(temp2[xx])) temp2[xx] = tolower(temp2[xx]);
		if (my_strstr(temp2, temp) || my_strstr(r_name + r_ptr->name, temp))
		{
			success = TRUE;
			ty = m_ptr->fy;
			tx = m_ptr->fx;
			break;
		}
	}
	//モンスターに該当がなければフロアのアイテムで同じようにする
	if(!success)
	{
		char o_name[MAX_NLEN];
		for (i = 1; i < o_max; i++)
		{
			object_type *o_ptr = &o_list[i];

			if (!o_ptr->k_idx) continue;
			for (xx = 0; temp[xx] && xx < 80; xx++)
			{
				if (iskanji(temp[xx]))
				{
					xx++;
					continue;
				}
				if (isupper(temp[xx])) temp[xx] = tolower(temp[xx]);
			}
			object_desc(o_name, o_ptr, (OD_NAME_ONLY | OD_STORE));
			if (my_strstr(o_name, temp))
			{
				success = TRUE;

				if(o_ptr->held_m_idx)
				{
					ty = m_list[o_ptr->held_m_idx].fy;
					tx = m_list[o_ptr->held_m_idx].fx;
				}
				else
				{
					ty = o_ptr->iy;
					tx = o_ptr->ix;
				}
				break;
			}
		}
	}


	if(!success)
	{
		msg_print("検索結果・・該当なし");
	}
	else if(!in_bounds2(ty,tx))
	{
		msg_print("ERROR:hatate_psychography()のtxとtyがおかしい");
		return FALSE;
	}
	//発見。該当箇所周辺の地形、アイテム、敵を感知する
	else
	{
		int x,y;
		cave_type       *c_ptr;
		s16b            feat;
		feature_type    *f_ptr;

		if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;
		//地形
		for (y = 1; y < cur_hgt - 1; y++)
		for (x = 1; x < cur_wid - 1; x++)
		{
			if (distance(ty, tx, y, x) > range) continue;
			c_ptr = &cave[y][x];
			c_ptr->info |= (CAVE_KNOWN);

			feat = get_feat_mimic(c_ptr);
			f_ptr = &f_info[feat];

			if (!have_flag(f_ptr->flags, FF_WALL))
			{
				if (have_flag(f_ptr->flags, FF_REMEMBER)) c_ptr->info |= (CAVE_MARK);
				for (i = 0; i < 8; i++)
				{
					c_ptr = &cave[y + ddy_ddd[i]][x + ddx_ddd[i]];
					feat = get_feat_mimic(c_ptr);
					f_ptr = &f_info[feat];
					if (have_flag(f_ptr->flags, FF_REMEMBER)) c_ptr->info |= (CAVE_MARK);
				}
			}
		}

		//アイテム、金
		for (i = 1; i < o_max; i++)
		{
			object_type *o_ptr = &o_list[i];
			if (!o_ptr->k_idx) continue;
			if (o_ptr->held_m_idx) continue;
			y = o_ptr->iy;
			x = o_ptr->ix;
			if (distance(ty, tx, y, x) > range) continue;
			o_ptr->marked |= OM_FOUND;
			lite_spot(y, x);
		}
		//モンスター
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			if (!m_ptr->r_idx) continue;
			y = m_ptr->fy;
			x = m_ptr->fx;

			if (distance(ty, tx, y, x) > range) continue;

			if (!(r_ptr->flags2 & RF2_INVISIBLE) || p_ptr->see_inv)
			{
				repair_monsters = TRUE;
				m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);
				update_mon(i, FALSE);
			}
		}

		msg_format("「%s」を念写した！",temp);
		p_ptr->redraw |= (PR_MAP);
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	}

	return TRUE;
}



/*:::こころ特殊技により特殊装備「○○の面」を作成する*/
void make_mask(monster_type *m_ptr)
{
	object_type forge;
	object_type *o_ptr = &forge;
	monster_race *r_ptr;
	char m_name[80];
	int gain_skill[5] = {0,0,0,0,0};
	int gain_skill_cnt = 0;
	int i;

	if(p_ptr->pclass != CLASS_KOKORO) { msg_print("ERROR:こころ以外でmake_mask()が呼ばれた"); return;}
	monster_desc(m_name, m_ptr, 0);
	r_ptr = &r_info[m_ptr->r_idx];

	if(r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND) )
	{
		msg_format("%sは感情を持たないようだ。",m_name);
		return;
	}

	object_prep(o_ptr, (int)lookup_kind(TV_MASK, 0));
	/*:::モンスターの能力をobject_typeの各変数へ格納する。
	 *:::ランダムユニークに対応するためモンスター名やレベル、スキルなど全てここで格納しないといけない。
	 *レベル→discount
	 *HP→xtra5(ddとdsは投げたときの処理が面倒)
	 *AC→1/10にし面のACへ加算
	 *モンスター名→art_name
	 */
	o_ptr->number = 1;
	o_ptr->discount = r_ptr->level;
	o_ptr->xtra5 = MIN(30000,(r_ptr->hdice * r_ptr->hside));
	o_ptr->to_a = MIN(99,(r_ptr->ac / 10));
	o_ptr->art_name = quark_add(format("%sの",m_name));

	/*:::モンスターの耐性をart_flagsにコピー*/
	/*:::Hack - POWERFULフラグをTR120として扱う*/
	/*:::フラグそのまま実装だとGHBとかの面が馬鹿強いのでレベルでランダム判定にする*/
	if( r_ptr->flagsr & (RFR_IM_ACID) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_RES_ACID);
	if( r_ptr->flagsr & (RFR_IM_ELEC) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_RES_ELEC);
	if( r_ptr->flagsr & (RFR_IM_FIRE) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_RES_FIRE);
	if( r_ptr->flagsr & (RFR_IM_COLD) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_RES_COLD);
	if( r_ptr->flagsr & (RFR_IM_POIS) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_POIS);

	if( r_ptr->flagsr & (RFR_RES_LITE) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_LITE);
	if( r_ptr->flagsr & (RFR_RES_DARK) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_DARK);
	if( r_ptr->flagsr & (RFR_RES_NETH) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_NETHER);
	if( r_ptr->flagsr & (RFR_RES_WATE) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_WATER);
	if( r_ptr->flagsr & (RFR_RES_SHAR) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_SHARDS);
	if( r_ptr->flagsr & (RFR_RES_SOUN) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_SOUND);
	if( r_ptr->flagsr & (RFR_RES_CHAO) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_CHAOS);
	if( r_ptr->flagsr & (RFR_RES_HOLY) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_HOLY);
	if( r_ptr->flagsr & (RFR_RES_DISE) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_DISEN);
	if( r_ptr->flagsr & (RFR_RES_TIME) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_TIME);

	if( r_ptr->flags3 & (RF3_NO_FEAR) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_FEAR);
	if( r_ptr->flags3 & (RF3_NO_CONF) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_RES_CONF);
	if( r_ptr->flags3 & (RF3_NO_SLEEP) && randint1(100) < r_ptr->level ) add_flag(o_ptr->art_flags, TR_FREE_ACT);
	if( r_ptr->flags2 & (RF2_POWERFUL) && randint1(100) < r_ptr->level  ) add_flag(o_ptr->art_flags, TR_SPECIAL_KOKORO);

	/*:::モンスターのスキルから最大5つランダムに選定してxtra1-4へ格納*/
	for(i=0;i<128;i++)
	{
		int shift = i % 32;
		u32b rflg;

		if(i < 32) rflg = r_ptr->flags4;
		else if(i < 64) rflg = r_ptr->flags5;
		else if(i < 96) rflg = r_ptr->flags6;
		else rflg = r_ptr->flags9;

		//持ってない技はcontinue;
		if(!(rflg & (1L << shift))) continue;
		//特別な行動など＠が使えない技はcontinue;
		if(!monspell_list2[i+1].level) continue;


		if(gain_skill_cnt < 5) gain_skill[gain_skill_cnt] = i+1;
		else
		{
			int check = randint0(gain_skill_cnt+1);
			if(check < 5) gain_skill[check] = i+1;
		}
		gain_skill_cnt++;
	}

	o_ptr->xtra1 = gain_skill[0];
	o_ptr->xtra2 = gain_skill[1];
	o_ptr->xtra3 = gain_skill[2];
	//xtra4はs16bなので二つ格納する
	o_ptr->xtra4 = gain_skill[3] + gain_skill[4] * 200; 

	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	(void)drop_near(o_ptr, -1, py, px);

	return;

}

/*:::永続変異を除く変異の数を数える*/
int muta_erasable_count(void)
{
	int bits = 0;
	bits += count_bits(p_ptr->muta1 & ~(p_ptr->muta1_perma));
	bits += count_bits(p_ptr->muta2 & ~(p_ptr->muta2_perma));
	bits += count_bits(p_ptr->muta3 & ~(p_ptr->muta3_perma));
	bits += count_bits(p_ptr->muta4 & ~(p_ptr->muta4_perma));

	return (bits);
}

/*:::現在の霊夢のパワーアップ度合いを得る*/
int osaisen_rank(void)
{
	if(p_ptr->pclass != CLASS_REIMU)
	{
		//msg_print("ERROR:霊夢以外でosaisen_rank()が呼ばれた");
		return 0;
	}
	if(osaisen >= OSAISEN_MAX) return 9;
	if(osaisen >= 50000000) return 8;
	if(osaisen >= 10000000) return 7;
	if(osaisen >= 5000000) return 6;
	if(osaisen >= 1000000) return 5;
	if(osaisen >= 500000) return 4;
	if(osaisen >= 100000) return 3;
	if(osaisen >= 50000) return 2;
	if(osaisen >= 10000) return 1;
	else return 0;

}

/*:::霊夢がお賽銭を得る*/
void gain_osaisen(int amount)
{
	int old_rank = osaisen_rank();
	int new_rank;
	if(p_ptr->pclass != CLASS_REIMU) return;
	if(osaisen >= OSAISEN_MAX) return;
	osaisen += amount;
	if(osaisen > OSAISEN_MAX) osaisen = OSAISEN_MAX;

//msg_format("Osaisen:%d(%d)",amount,osaisen);
	new_rank = osaisen_rank();
	if(old_rank != new_rank)
	{
		msg_print("何だかやる気が出てきた！");
		p_ptr->update |= (PU_BONUS);
	
	}
}


/*:::お賽銭を入れる。*/
void throw_osaisen(void)
{
	s32b amount;
	char out_val[160];
	cptr p;
	if(p_ptr->pclass == CLASS_REIMU)
	{
		msg_print("ERROR:霊夢でthrow_osaisen()が呼ばれた");
		return ;
	}
	//ウィザードコマンドやいかさま中は賽銭を入れられない 
	if (p_ptr->noscore || p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		msg_print("無尽蔵の金を投じようとしたが、なぜか賽銭箱がどこにも見当たらない。");
		return ;
	}
	//v2.0.3 龍専用性格
	if (is_special_seikaku(SEIKAKU_SPECIAL_MEGUMU))
	{
		msg_print("この資金は天狗社会のために使われなければならない。");
		return;
	}

	if(quest[QUEST_CHAOS_WYRM].status != QUEST_STATUS_FINISHED && !EXTRA_MODE && 
		!(p_ptr->pclass == CLASS_3_FAIRIES || p_ptr->pclass == CLASS_SUNNY || p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_STAR))
	{
		msg_print("悪戯者の妖精たちが賽銭箱の横から興味津々の様子でこちらを見ている。後にしておこう。");
		return ;
	}
	if (osaisen >= OSAISEN_MAX && !EXTRA_MODE)
	{
		msg_print("信じ難いことに、賽銭箱は既に一杯だ。");
		return ;
	}


	if (p_ptr->au < 1)
	{
		msg_print("あなたは無一文だ。");
		return ;
	}

	strcpy(out_val, "");	
	if (get_string("お賽銭をいくら入れますか？", out_val, 32))
	{
		for (p = out_val; *p == ' '; p++);
		amount = atol(p);
		if(amount > p_ptr->au)
		{
			msg_print("そんなにお金を持っていない。");
			return ;
		}
		else if(amount < 0)
		{
			msg_print("賽銭泥棒をするつもりはない。");
			return;
		}
		else if(amount == 0)
		{
			msg_print("やはりやめておいた。");
			return;
		}

		if(osaisen + amount > OSAISEN_MAX && !EXTRA_MODE)
		{
			amount = OSAISEN_MAX - osaisen;
			msg_print("賽銭箱に入りきらないので入れられるだけ入れた。");

		}
		else
		{
			msg_print("お賽銭を入れた。");
		}

		osaisen += amount;
		if(osaisen > OSAISEN_MAX) osaisen = OSAISEN_MAX; //Paranoia
		p_ptr->au -= amount; 
		p_ptr->redraw |= (PR_GOLD);

		//Hack - Extraモードでは建物パラメータに賽銭金額が記録され、その後のトレードに影響を与える
		if(EXTRA_MODE)
		{
			if(ex_buildings_param[f_info[cave[py][px].feat].subtype] != 255)
			{
				int tmp = ex_buildings_param[f_info[cave[py][px].feat].subtype];
				tmp += amount / (dun_level * dun_level / 5 + 100);
				if(tmp > 100) tmp = 100;
				ex_buildings_param[f_info[cave[py][px].feat].subtype] = (byte)tmp;
			}
		}

	}
	else
		msg_print("やはりやめておいた。");

}

/*:::賽銭ランクを確認する　霊夢専用*/
void check_osaisen(void)
{
	if(p_ptr->pclass != CLASS_REIMU)
	{
		msg_print("ERROR:霊夢以外でcheck_osaisen()が呼ばれた");
		return ;
	}

	prt("賽銭箱を確認した・・", 7, 5); 
	(void)inkey();
	switch(osaisen_rank())
	{
		case 0:
		prt("ほとんど空っぽだ。", 8, 5); 
		prt("心まで寒々しくなるようだ・・", 9, 5); 
		break;
		case 1:
		prt("何とも侘しいお賽銭の入りだ・・", 8, 5); 
		break;
		case 2:
		prt("少しは入っている。", 8, 5); 
		break;
		case 3:
		prt("お賽銭はそれなりに入っている。", 8, 5); 
		break;
		case 4:
		prt("お賽銭の入りは上々だ。", 8, 5); 
		break;
		case 5:
		prt("沢山のお賽銭が入っている。", 8, 5); 
		prt("神社の好評が誇らしい。", 9, 5); 
		break;
		case 6:
		prt("びっくりするほどお賽銭が入っている。", 8, 5); 
		prt("見ているだけで心が温まるようだ。", 9, 5); 
		break;
		case 7:
		prt("信じられないほどのお賽銭が入っている。", 8, 5); 
		prt("人々の感謝と期待を受けて身が引き締まる思いだ。", 9, 5); 
		break;
		case 8:
		prt("お賽銭が一杯に詰まっている。", 8, 5); 
		prt("身も心も喜びにはち切れそうだ！", 9, 5); 
		break;
		case 9:
		prt("賽銭箱がお賽銭に埋まっている！", 8, 5); 
		prt("夢にも想像しなかった光景が目の前にある！", 9, 5); 
		break;
	}

}


/*:::魔理沙が魔法作成用の魔力を得る。Maripo交換や魔力抽出時に呼ばれる。*/
//職業魔理沙のときは抽出量を倍にする？自宅抽出は倍で特殊スキルでダンジョン内で抽出したら通常？
//multは抽出量係数(単位は%)
//魔力はmarisa_magic_power[8]に格納されるが、魔理沙プレイ中にチートオプションをONにしたら代わりにmagic_num[30-37]に格納
void marisa_gain_power(object_type *o_ptr, int mult)
{
	int type;
	int tv = o_ptr->tval;
	int sv = o_ptr->sval;
	bool flag = FALSE;
	bool cheat_gain = FALSE;

	///mod160103 いかさま以外のチートモード魔理沙は別領域に魔力を得る
	if (p_ptr->pclass == CLASS_MARISA && p_ptr->noscore)
	{
		cheat_gain = TRUE;
	}
	//ほか、他キャラデバッグモードや性格いかさまのときは値を得られない。
	else if (p_ptr->noscore || p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		if(p_ptr->wizard || cheat_xtra) msg_print("デバッグモードやいかさまの時は魔理沙用魔力を得られない");
		return;
	}
	for(type=0;type<8;type++)
	{
		int gain = 0;

		switch(type)
		{
			case MARISA_POWER_FIRE:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_STUPIDITY) gain = 3;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_WEAKNESS) gain = 4;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_BLINDNESS) gain = 10;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_RED) gain = 5;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_GARNET) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HEMATITE) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONSCALE) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_RUBY) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) gain = 100;
				if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_PHOENIX_FEATHER) gain = 1000;
				if (tv == TV_STONE_INCIDENT && sv == 0) gain = 200;
				if (tv == TV_STONE_INCIDENT && sv == 1) gain = 200;
				break;

			case MARISA_POWER_COLD:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_RESTORE_STR) gain = 10;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_WHITE) gain = 5;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_AQUAMARINE) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_PERIDOT) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONSCALE) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ICESCALE) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_SAPPHIRE) gain = 300;
				if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) gain = 1000;
				if (tv == TV_STONE_INCIDENT && sv == 2) gain = 200;
				if (tv == TV_STONE_INCIDENT && sv == 3) gain = 200;
				break;
			case MARISA_POWER_ELEC:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_PARALYSIS) gain = 4;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_DISEASE) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_POISON) gain = 10;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_RESTORE_CON) gain = 10;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_BLUE) gain = 5;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_AMETHYST) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_OPAL) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HEMATITE) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONSCALE) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_EMERALD) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) gain = 100;
				if (tv == TV_STONE_INCIDENT && sv == 4) gain = 200;
				if (tv == TV_STONE_INCIDENT && sv == 5) gain = 200;
				break;
			case MARISA_POWER_LITE:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_NAIVETY) gain = 4;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CONFUSION) gain = 2;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_PARANOIA) gain = 8;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIKARIGOKE) gain = 10;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_TOPAZ) gain = 15;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MITHRIL) gain = 50;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_TAKAKUSAGORI) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_EMERALD) gain = 200;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HOPE_FRAGMENT) gain = 300;
				if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_PHOENIX_FEATHER) gain = 500;
				if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_MOON_ORB) gain = 200;
				break;
			case MARISA_POWER_DARK:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_SICKNESS) gain = 6;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_POISON) gain = 3;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_UNHEALTH) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_BLACK) gain = 5;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_GREEN) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_CONFUSION) gain = 8;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MOONSTONE) gain = 10;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_COAL) gain = 10;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_LAPISLAZULI) gain = 15;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MAGNETITE) gain = 30;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ISHIZAKURA) gain = 20;
				if (tv == TV_MATERIAL && sv == SV_MATERIAL_SKULL) gain = 25;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_NIGHTMARE_FRAGMENT) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_SAPPHIRE) gain = 200;
				if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_KODOKUZARA) gain = 500;

				break;
			case MARISA_POWER_ILLU:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_BLINDNESS) gain = 3;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_HALLUCINATION) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_BERSERK) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_GRAY) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_L_BLUE) gain = 15;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_TAKAKUSAGORI) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIKARIGOKE) gain = 10;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ISHIZAKURA) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ARSENIC) gain = 50;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_GELSEMIUM) gain = 50;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ICESCALE) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MYSTERIUM) gain = 500;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_NIGHTMARE_FRAGMENT) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_RUBY) gain = 200;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIHIIROKANE) gain = 1000;
				if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) gain = 300;
				if (tv == TV_SWEETS && sv == SV_SWEETS_HONEY) gain = 2;
				if (tv == TV_MATERIAL && sv == SV_MATERIAL_SAZAREISHI) gain = 300;
				if (tv == TV_STONE_INCIDENT && sv == 6) gain = 200;
				if (tv == TV_STONE_INCIDENT && sv == 7) gain = 200;

				break;
			case MARISA_POWER_STAR:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_PARANOIA) gain = 2;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_CURE_SERIOUS) gain = 4;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MANA) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_CLEAR) gain = 5;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_STONE) gain = 1; //あとで3で割る
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_ADAMANTITE) gain = 30;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DIAMOND) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HOPE_FRAGMENT) gain = 300;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_METEORICIRON) gain = 1000;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MYSTERIUM) gain = 500;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIHIIROKANE) gain = 1000;
				if (tv == TV_SWEETS && sv == SV_SWEETS_HONEY) gain = 2;
				break;
			case MARISA_POWER_SUPER:
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_RESTORING) gain = 8;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_GAUDY) gain = 4;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MERCURY) gain = 20;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_BROKEN_NEEDLE) gain = 30;
				if(tv == TV_MUSHROOM && sv == SV_MUSHROOM_MON_SUPER) gain = 500;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DRAGONNAIL) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_IZANAGIOBJECT) gain = 100;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_DIAMOND) gain = 200;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_MYSTERIUM) gain = 500;
				if(tv == TV_MATERIAL && sv == SV_MATERIAL_HIHIIROKANE) gain = 1000;
				if(tv == TV_SOUVENIR && sv == SV_SOUVENIR_PHOENIX_FEATHER) gain = 500;
				if (tv == TV_MUSHROOM && sv == SV_MUSHROOM_PUFFBALL) gain = 10;
				if (tv == TV_SOUVENIR && sv == SV_SOUVENIR_ELDER_THINGS_CRYSTAL) gain = 300;
				if (tv == TV_MATERIAL && sv == SV_MATERIAL_RYUUZYU) gain = 100;
				if (tv == TV_SWEETS && sv == SV_SWEETS_HONEY) gain = 2;
				break;
			default:
			msg_print("ERROR:marisa_gain_power()");
			return;
		}

		if(tv == TV_MATERIAL && sv == SV_MATERIAL_SCRAP_IRON)
		{
			if(one_in_(3)) gain = randint1(10);
			while(one_in_(3)) gain *= 2;
		}
		gain *= o_ptr->number;
		if(tv == TV_MATERIAL && sv == SV_MATERIAL_STONE) gain /= 3;
		gain = gain * mult / 100;
		if(!gain) continue;
		flag = TRUE;

		//職業魔理沙の時メッセージ出す
		if(p_ptr->pclass == CLASS_MARISA) 
		{
			msg_format("%sの魔力を%d抽出した！",marisa_essence_name[type],gain);
			msg_print(NULL);
		}

		//魔力を得る処理　魔理沙チートオプションONのときmagic_num[30-37]を使う
		if(cheat_gain)
		{
			p_ptr->magic_num1[30+type] += gain;
			if(p_ptr->magic_num1[30+type] > MARISA_POWER_MAX) p_ptr->magic_num1[30+type] = MARISA_POWER_MAX;

		}
		else
		{
			marisa_magic_power[type] += gain;
			if(marisa_magic_power[type] > MARISA_POWER_MAX) marisa_magic_power[type] = MARISA_POWER_MAX;
		}
	}

	if(p_ptr->pclass == CLASS_MARISA && !flag) msg_print("何も得られなかった・・");
}



/*:::ぬえが正体を見破られている状態かどうかチェックする。*/
/*:::フロアを移動した直後、delete_monster_idx()時、変身魔法解除時に呼ばれる。ループ注意*/
void check_nue_undefined(void)
{
	int i;
	bool flag_undefined = TRUE;

	if(p_ptr->pclass != CLASS_NUE) return;
	//if(p_ptr->special_defense & NUE_UNDEFINED) return;
	//正体不明以外の変身中には関係ない
	if(p_ptr->mimic_form && p_ptr->mimic_form != MIMIC_NUE) return;

	if (cheat_xtra) msg_print("nue_check");

	//ぬえの正体を見破っている敵がいるか
	for (i = 1; i < m_max; i++)
	{
		if(!m_list[i].r_idx) continue;
		if(m_list[i].mflag & MFLAG_SPECIAL)
		{
			flag_undefined = FALSE;
			break;
		}
	}

	/*:::ぬえは変身してないが正体を見破っている敵がいないので変身処理*/
	if(flag_undefined && !(p_ptr->special_defense & NUE_UNDEFINED))
	{
		msg_print("ここにあなたの正体を知っている者はいない。");
		p_ptr->special_defense |= NUE_UNDEFINED;
		set_mimic(1,MIMIC_NUE,FALSE);
	}
	//変身中だがフロアに正体を見破っているものがいる時
	else if(!flag_undefined && (p_ptr->special_defense & NUE_UNDEFINED))
	{
		msg_print("ここにはあなたの正体を知っている者がいる！");
		p_ptr->special_defense &= ~(NUE_UNDEFINED);
		set_mimic(0,0,FALSE);

	}

}


/*:::ぬえが特定の敵の行動により正体を見破られたかどうかチェックする*/
//chance_mult:行動による見破りやすさ倍率　閃光属性3倍など 負の場合必ず見破る
void check_nue_revealed(int m_idx, int chance_mult)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr;
	int chance=0;
	int power = 0;
	char m_name[120];

	if(!m_ptr->r_idx) return;
	r_ptr = &r_info[m_ptr->r_idx];	
	//ぬえ限定
	if(p_ptr->pclass != CLASS_NUE) return;

	//v2.05 los()では距離を判定していないらしく遠くの敵にも判定が発生するので距離に制限をつける
	if (m_ptr->cdis > MAX_RANGE) return;

	//正体不明以外の変身中には見破られない
	if(p_ptr->tim_mimic && p_ptr->mimic_form != MIMIC_NUE) return;

	//すでに見破られた敵には関係ない
	if(m_ptr->mflag & MFLAG_SPECIAL) return;

	if(r_ptr->flags2 & (RF2_STUPID | RF2_EMPTY_MIND | RF2_WEIRD_MIND)) return;
	if(r_ptr->flags3 & RF3_NONLIVING) return;
	if(!is_hostile(m_ptr)) return;

	if(!los(py,px,m_ptr->fy,m_ptr->fx)) return;


	if(chance_mult >= 0)
	{
		chance = 20 + r_ptr->level * 2;

		if (r_ptr->flags2 & RF2_SMART_EX) chance *= 10;
		else if (r_ptr->flags2 & RF2_SMART) chance *= 3;
		if(is_gen_unique(m_ptr->r_idx)) chance *= 4;
		else if(r_ptr->flags1 & RF1_UNIQUE || r_ptr->flags7 & RF7_UNIQUE2) chance *= 2;

		if(m_ptr->cdis < 2) chance *= 2;

		if(!p_ptr->tim_mimic) chance *= 5;
		if (cave[py][px].info & CAVE_GLOW) chance *= 5;
		chance = chance * chance_mult / 100;

		power = (50 + p_ptr->lev * 3) * (30 + adj_general[p_ptr->stat_ind[A_CHR]] + adj_general[p_ptr->stat_ind[A_INT]]);
	}

	//見破られなかったら終了
	if(chance_mult >= 0 && chance < randint1(power)) return;

	monster_desc(m_name, m_ptr, 0);
	if(p_ptr->special_defense & NUE_UNDEFINED)
	{
		msg_format("%sに正体を見破られてしまった！",m_name);
		m_ptr->mflag |= MFLAG_SPECIAL;
		p_ptr->special_defense &= ~(NUE_UNDEFINED);
		if(p_ptr->mimic_form) set_mimic(0,0,FALSE);
	}
	else
	{
		msg_format("%sにも正体がばれてしまった！",m_name);
		m_ptr->mflag |= MFLAG_SPECIAL;
	}
}


void restore_seija_item(void)
{
	int i;
	bool flag_mdv = FALSE;
	bool flag_mdv2 = FALSE;

	if(p_ptr->pclass != CLASS_SEIJA && p_ptr->pclass != CLASS_MAGIC_EATER) return;

	for (i = 0; i < EATER_EXT*2; i++)
	{
		if(p_ptr->magic_num1[i] != p_ptr->magic_num2[i]*EATER_CHARGE)
		{	
			flag_mdv = TRUE;
			p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
		}
	}
	for (; i < EATER_EXT*3; i++)
	{
		if(p_ptr->magic_num1[i])
		{

			p_ptr->magic_num1[i] = 0;
			flag_mdv = TRUE;
		}
	}
	if(p_ptr->pclass == CLASS_MAGIC_EATER)
	{
		if(flag_mdv) msg_print("あなたは魔道具のメンテナンスをした。");
		return;
	}
	for( i = SEIJA_ITEM_START; i <= SEIJA_ITEM_END; i++)
	{
		if(p_ptr->magic_num1[i] != p_ptr->magic_num2[i])
		{
			p_ptr->magic_num1[i] = p_ptr->magic_num2[i];
			flag_mdv2 = TRUE;
		}

	}

	if(flag_mdv) msg_print("あなたは隠し持った魔道具のメンテナンスをした。");
	if(flag_mdv2) msg_format("%s不思議道具のメンテナンスをした。",flag_mdv?"さらに":"あなたは");

}


/*:::充填中の装備品を選択*/
bool item_tester_hook_recharging_equipment(object_type *o_ptr)
{
	if(!o_ptr->k_idx) return FALSE;
	if(!item_tester_hook_wear(o_ptr)) return FALSE;
	if(o_ptr->timeout) return TRUE;
	return (FALSE);
}

/*:::装備品を選択して充填する*/
bool do_cmd_recharge_equipment(int power)
{
	object_type *o_ptr;
	int item;
	char o_name[MAX_NLEN];
	cptr q, s;

	item_tester_hook = item_tester_hook_recharging_equipment;
	q = "どれを充填しますか? ";
	s = "充填中の装備品がない。";

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) return FALSE;

	if (item >= 0) 	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	if(o_ptr->tval == TV_GUN) power *= 100;

	msg_format("%sに魔力を注入した・・",o_name);
	
	o_ptr->timeout -= power;
	if(o_ptr->timeout <= 0)
	{
		o_ptr->timeout = 0;
		msg_format("充填が完了した。");
		p_ptr->window |= (PW_EQUIP);
	}

	return TRUE;
}

//正邪、魔道具使い専用の取り込み済み魔道具の充填特技ルーチン
//行動順消費するときTRUE
bool recharge_magic_eater(int power)
{
	int plev = p_ptr->lev;
	int item;
	int k_idx;
	int item_level;

	if(p_ptr->pclass != CLASS_SEIJA && p_ptr->pclass != CLASS_MAGIC_EATER)
		{ msg_print("ERROR:recharge_magic_eater()が未対応の職で呼ばれた"); return FALSE;}

	item = select_magic_eater(FALSE, TRUE);
	if (item == -1) return FALSE;
	if(item < EATER_EXT ) //杖
	{
		if(p_ptr->magic_num1[item] == p_ptr->magic_num2[item]*EATER_CHARGE)
		{
			msg_print("その杖は既に充填されている。");
			return FALSE;
		}

		k_idx = lookup_kind(TV_STAFF,item);
		item_level = k_info[k_idx].level;
		if(cheat_xtra) msg_format("power:%d level:%d",power,item_level);
		power /= (10+item_level);
		power = power * MIN(50,p_ptr->magic_num2[item]) / 10;
		if(item == SV_STAFF_THE_MAGI) power /= 10;//賢者の杖特殊処理
		p_ptr->magic_num1[item] += power;
		msg_print("杖に魔力を注ぎ込んだ・・");
		if(p_ptr->magic_num1[item] >= p_ptr->magic_num2[item]*EATER_CHARGE)
		{
			p_ptr->magic_num1[item] = p_ptr->magic_num2[item]*EATER_CHARGE;
			msg_print("充填が完了した。");

		}
		if(cheat_xtra) msg_format("num:%d",p_ptr->magic_num1[item] / 65535);

	}
	else if(item < EATER_EXT * 2 ) //魔法棒
	{
		if(p_ptr->magic_num1[item] == p_ptr->magic_num2[item]*EATER_CHARGE)
		{
			msg_print("その魔法棒は既に充填されている。");
			return FALSE;
		}
		k_idx = lookup_kind(TV_WAND,item - EATER_EXT);
		item_level = k_info[k_idx].level;

		if(cheat_xtra) msg_format("power:%d level:%d",power,item_level);

		power /= (10+item_level);
		power = power * MIN(50,p_ptr->magic_num2[item]) / 10;
		p_ptr->magic_num1[item] += power;
		msg_print("魔法棒に魔力を注ぎ込んだ・・");
		if(p_ptr->magic_num1[item] >= p_ptr->magic_num2[item]*EATER_CHARGE)
		{
			p_ptr->magic_num1[item] = p_ptr->magic_num2[item]*EATER_CHARGE;
			msg_print("充填が完了した。");

		}
		if(cheat_xtra) msg_format("num:%d",p_ptr->magic_num1[item]/ 65535);
	}
	else if(item < EATER_EXT * 3) //ロッド
	{
		if(!p_ptr->magic_num1[item])
		{
			msg_print("そのロッドは既に充填されている。");
			return FALSE;
		}
		k_idx = lookup_kind(TV_ROD,item - EATER_EXT*2);

		item_level = k_info[k_idx].level;
		if(cheat_xtra) msg_format("power:%d level:%d",power,item_level);
		power /= (10 + item_level);
		power = power * MIN(50,p_ptr->magic_num2[item]*5) / 10;
		p_ptr->magic_num1[item] -= (power / k_info[k_idx].pval);
		msg_print("ロッドに魔力を注ぎ込んだ・・");
		if(p_ptr->magic_num1[item] <= 0)
		{
			p_ptr->magic_num1[item] = 0;
			msg_print("充填が完了した。");

		}

		if(cheat_xtra) msg_format("num:%d",p_ptr->magic_num1[item]);

	}
	else
	{
		msg_format("ERROR:select_magic_eater()で想定外の値(%d)が返された",item);
		return FALSE;
	}

	return TRUE;
}

//天子の要石が地震を吸収できるかのチェック TRUEが返ると地震が起こらない
bool kanameishi_check(int add)
{
	if(p_ptr->pclass != CLASS_TENSHI)
	{
		return FALSE;
	}
	//要石設置されていない
	if(!p_ptr->magic_num1[0] || !p_ptr->magic_num1[1]) return FALSE;
	if(!add) return FALSE;
	//クエストダンジョンではカウントが進まない
	if(p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)) return FALSE;

	if(EXTRA_QUEST_FLOOR) return FALSE;

	p_ptr->magic_num1[2] += add;
	if(cheat_xtra) msg_format("地震:%d",p_ptr->magic_num1[2]);

	if(p_ptr->magic_num1[2] + randint1(50) < p_ptr->lev * 5)
	{
		msg_print("地中の要石が地震を封じ込めた。");
	}
	else
	{
		msg_print("地中の要石が地震に耐え切れず壊れた！");
		kanameishi_break();
	}
	return TRUE;


}

//天子の要石が壊れるか解除されて必要なら地震を起こす処理
//このダメージで地震打撃の敵が倒れた場合の処理は問題ないか？注意
void kanameishi_break()
{
	int cx;
	int cy;
	int power;

	if(p_ptr->pclass != CLASS_TENSHI)
	{
		msg_format("ERROR:天子以外でkanameishi_break()が呼ばれた");
		return;
	}
	if(p_ptr->is_dead) return;

	if(!p_ptr->magic_num1[0] || !p_ptr->magic_num1[1])
	{
		msg_format("ERROR:要石が設置されていないのにkanameish_break()が呼ばれた");
		return;
	}
	cx = p_ptr->magic_num1[1];
	cy = p_ptr->magic_num1[0];
	power = p_ptr->magic_num1[2] * 2;

	//座標とカウントをリセット
	p_ptr->magic_num1[0] = 0;
	p_ptr->magic_num1[1] = 0;
	p_ptr->magic_num1[2] = 0;

	//要石が一度でも地震を吸収している場合は地震が起こる
	if(power)
	{
		int             i, t, y, x, yy, xx, dy, dx;

		int r = power / 8;
		int             sn = 0, sy = 0, sx = 0;
		bool            map[128][128];
		int damage;
		cave_type       *c_ptr;

		if(r > 60) r = 60;

		msg_print("押さえつけられていた地震が解放された！");

		for (y = 0; y < 128; y++)for (x = 0; x < 128; x++)map[y][x] = FALSE;


		/* Check around the epicenter */
		if(dun_level) for (dy = -r; dy <= r; dy++)
		{
			for (dx = -r; dx <= r; dx++)
			{
				yy = cy + dy;
				xx = cx + dx;

				if (!in_bounds(yy, xx)) continue;
				if (distance(cy, cx, yy, xx) > r) continue;

				c_ptr = &cave[yy][xx];
				///vault内には効果がない
				if(c_ptr->info & CAVE_ICKY) continue;

				/* Lose room and vault */
				c_ptr->info &= ~(CAVE_ROOM | CAVE_UNSAFE);
				/* Lose light and knowledge */
				c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK | CAVE_KNOWN);
				/* Skip most grids */
				if (randint0(100) < 60) continue;
				if (player_bold(yy, xx)) continue;
				map[64+yy-cy][64+xx-cx] = TRUE;
			}
		}
		/* Examine the quaked region */
		for (dy = -r; dy <= r; dy++)
		{
			for (dx = -r; dx <= r; dx++)
			{
				yy = cy + dy;
				xx = cx + dx;

				if (!in_bounds(yy, xx)) continue;
				if (distance(cy, cx, yy, xx) > r) continue;

				/* Access the grid */
				c_ptr = &cave[yy][xx];

				if (c_ptr->m_idx == p_ptr->riding) continue;
				///vault内には効果がない
				if(c_ptr->info & CAVE_ICKY) continue;
				/* Process monsters */
				if (c_ptr->m_idx)
				{
					monster_type *m_ptr = &m_list[c_ptr->m_idx];
					monster_race *r_ptr = &r_info[m_ptr->r_idx];


					///天子の地震ダメージ
					if((r_ptr->flags7 & RF7_CAN_FLY)
					|| (r_ptr->flags2 & (RF2_PASS_WALL | RF2_KILL_WALL))
					|| (r_ptr->flagsr & RFR_RES_ALL))
					{/*何もしない*/}
					else
					{
						//地震ダメージ　倒れたらcontinue
						if(project_m(0, 0, yy, xx, power + randint1(power), GF_ARROW, 0, TRUE)) continue;
					}
					if(!dun_level) continue;

					//ここから先崩れない地形は関係ない
					if (!map[64+yy-cy][64+xx-cx]) continue;

					/* Quest monsters */
					if (r_ptr->flags1 & RF1_QUESTOR)
					{
						map[64+yy-cy][64+xx-cx] = FALSE;
						continue;
					}

					/* Most monsters cannot co-exist with rock */
					if (!(r_ptr->flags2 & (RF2_KILL_WALL)) &&
					    !(r_ptr->flags2 & (RF2_PASS_WALL)))
					{
						char m_name[80];

						/* Assume not safe */
						sn = 0;

					/* Monster can move to escape the wall */
					if (!(r_ptr->flags1 & (RF1_NEVER_MOVE)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Access the grid */
							y = yy + ddy_ddd[i];
							x = xx + ddx_ddd[i];

							/* Skip non-empty grids */
							if (!cave_empty_bold(y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if (is_glyph_grid(&cave[y][x])) continue;
							if (is_explosive_rune_grid(&cave[y][x])) continue;

							/* ... nor on the Pattern */
							if (pattern_tile(y, x)) continue;

							/* Important -- Skip "quake" grids */
							if (map[64+y-cy][64+x-cx]) continue;

							if (cave[y][x].m_idx) continue;
							if (player_bold(y, x)) continue;

							/* Count "safe" grids */
							sn++;

							/* Randomize choice */
							if (randint0(sn) > 0) continue;

							/* Save the safe grid */
							sy = y; sx = x;
						}
					}

					/* Describe the monster */
					monster_desc(m_name, m_ptr, 0);

					/* Scream in pain */
#ifdef JP
					if (!ignore_unview || is_seen(m_ptr)) msg_format("%^sは悲鳴を上げた！", m_name);
#else
					if (!ignore_unview || is_seen(m_ptr)) msg_format("%^s wails out in pain!", m_name);
#endif

					/* Take damage from the quake */
					damage = (sn ? damroll(4, 8) : (m_ptr->hp + 1));

					/* Monster is certainly awake */
					(void)set_monster_csleep(c_ptr->m_idx, 0);

					/* Apply damage directly */
					m_ptr->hp -= damage;

					/* Delete (not kill) "dead" monsters */
					if (m_ptr->hp < 0)
					{
						/* Message */
#ifdef JP
						if (!ignore_unview || is_seen(m_ptr)) msg_format("%^sは岩石に埋もれてしまった！", m_name);
#else
						if (!ignore_unview || is_seen(m_ptr)) msg_format("%^s is embedded in the rock!", m_name);
#endif

						if (c_ptr->m_idx)
						{
							if (record_named_pet && is_pet(&m_list[c_ptr->m_idx]) && m_list[c_ptr->m_idx].nickname)
							{
								char m2_name[80];

								monster_desc(m2_name, m_ptr, MD_INDEF_VISIBLE);
								do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_EARTHQUAKE, m2_name);
							}
						}

						/* Delete the monster */
						delete_monster(yy, xx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if (sn)
					{
						int m_idx = cave[yy][xx].m_idx;

						/* Update the old location */
						cave[yy][xx].m_idx = 0;

						/* Update the new location */
						cave[sy][sx].m_idx = m_idx;

						/* Move the monster */
						m_ptr->fy = sy;
						m_ptr->fx = sx;

						/* Update the monster (new location) */
						update_mon(m_idx, TRUE);

						/* Redraw the old grid */
						lite_spot(yy, xx);

						/* Redraw the new grid */
						lite_spot(sy, sx);
					}
				}
			}
		}
	}
	if(!dun_level) return;

	/* Lose monster light */
	clear_mon_lite();

	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[64+yy-cy][64+xx-cx]) continue;

			/* Access the cave grid */
			c_ptr = &cave[yy][xx];

			/* Paranoia -- never affect player */
/*			if (player_bold(yy, xx)) continue; */

			/* Destroy location (if valid) */
			if (cave_valid_bold(yy, xx))
			{
				/* Delete objects */
				delete_object(yy, xx);

				/* Wall (or floor) type */
				t = cave_have_flag_bold(yy, xx, FF_PROJECT) ? randint0(100) : 200;

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					cave_set_feat(yy, xx, feat_granite);
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					cave_set_feat(yy, xx, feat_quartz_vein);
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					cave_set_feat(yy, xx, feat_magma_vein);
				}

				/* Floor */
				else
				{
					/* Create floor */
					cave_set_feat(yy, xx, floor_type[randint0(100)]);
				}
			}
		}
	}


	/* Process "re-glowing" */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if (!in_bounds(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			if (is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
			else if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS))
			{
				int ii, yyy, xxx;
				cave_type *cc_ptr;

				for (ii = 0; ii < 9; ii++)
				{
					yyy = yy + ddy_ddd[ii];
					xxx = xx + ddx_ddd[ii];
					if (!in_bounds2(yyy, xxx)) continue;
					cc_ptr = &cave[yyy][xxx];
					if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
					{
						c_ptr->info |= CAVE_GLOW;
						break;
					}
				}
			}
		}
	}


	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_MONSTERS);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);






	}



}



/*:::投擲ルーチン改変 投擲開始位置、ターゲット、投げるアイテムが既に決まっている*/
//mult:投擲パワー 基本1
//y,x: 投擲開始座標
//ty,tx: 投擲ターゲット(dirを送ると複数回処理のとき面倒)
//o_ptr:投げるアイテム この中では数量減少、削除処理は行わない 数量は常に1であること
//トールハンマーなども戻ってこない
//投擲がされなかったときFALSE
//うどんげメディスンチェスト投擲を除き投げたアイテムの重量減少処理は行われない
//モード 0:何もなし 1:咲夜ナイフ投擲(必ず消える) 2:咲夜ナイフ全投擲(投擲の指輪の効果半減)
//モード 3:デフレーションワールド(倍率大幅増、特殊表示、必ず壊れるなど)
//モード4:紫の「無限の超高速飛行体」投擲の指輪の影響を受けない
//5:ハルコンネン2発動用ダミーアイテム 必中
//6:二宮金次郎像
//7:ユイマン特技「ディアジェノサイダー」専用　投擲の指輪の効果半減、常に「投擲に向いた武器」扱い、動物スレイ、射程最大
bool do_cmd_throw_aux2(int y, int x, int ty, int tx, int mult, object_type *o_ptr, int mode)
{
	int item;
	int i, j, prev_y, prev_x;
	int ny[19], nx[19];
	int chance, tdam, tdis;
	int mul, div, dd, ds;
	int cur_dis, visible;

	object_type forge;
	object_type *q_ptr;

	bool hit_body = FALSE;
	bool hit_wall = FALSE;
	bool suitable_item = FALSE;
	bool return_when_thrown = FALSE;

	char o_name[MAX_NLEN];

	int msec = delay_factor * delay_factor * delay_factor;

	u32b flgs[TR_FLAG_SIZE];
	cptr q, s;
	bool do_drop = TRUE;


	q_ptr = &forge;
	object_copy(q_ptr, o_ptr);
	object_flags(q_ptr, flgs);
	torch_flags(q_ptr, flgs);
	if(have_flag(flgs, TR_THROW)) suitable_item = TRUE;
	/* Description */
	object_desc(o_name, q_ptr, OD_OMIT_PREFIX);

	if(o_ptr->number != 1)
	{
		msg_format("ERROR:throw_aux2()でアイテム%sの個数が1以外",o_name);
		return FALSE;
	}

	if (mode == 7) suitable_item = TRUE;

	/*:::強力投擲の指輪の効果*/
	if(p_ptr->mighty_throw)
	{
		//咲夜のナイフ全部投げは強力投擲の効果が薄い
		if(mode == 2 || mode==7) mult += 1;
		//菫子の投擲は指輪の効果を得られない
		else if ( p_ptr->pclass != CLASS_SUMIREKO) ;
		else if ( mode == 4 || mode == 5 || mode == 6) ;
		else 
			mult += 2;

	}

	/* Extract a "distance multiplier" */
	mul = 10 + 2 * (mult - 1);

	/*:::最低重量10に固定　重量10が1ポンド(0.5kg)扱い*/
	div = (((q_ptr->weight * 2) > 10) ? (q_ptr->weight * 2) : 10);
	if (have_flag(flgs, TR_THROW)) div /= 2;

	/* Hack -- Distance -- Reward strength, penalize weight */
	tdis = (adj_str_blow[p_ptr->stat_ind[A_STR]] + 20) * mul / div;

	/* Max distance of 10-18 */
	if (tdis > mul) tdis = mul;

	//菫子は重量を無視
	//デフレーションワールドも同じく
	if(p_ptr->pclass == CLASS_SUMIREKO || mode == 3||mode==7) tdis = 18;
	else if(mode == 5 || mode == 6) tdis = 18;
	else if(tdis > 18) tdis = 18;
	else if(tdis < 1) tdis = 1;

	if ( have_flag(flgs, TR_BOOMERANG))
		return_when_thrown = TRUE;

	//行動力消費処理はしない

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Chance of hitting */
	/*:::投擲向きの武器は命中判定に武器自身の命中率が加わる*/
//菫子の命中率は知能依存にしようか?
	if (suitable_item) chance = ((p_ptr->skill_tht) +
		((p_ptr->to_h_b + q_ptr->to_h) * BTH_PLUS_ADJ));
	else chance = (p_ptr->skill_tht + (p_ptr->to_h_b * BTH_PLUS_ADJ));

	//投擲スキルによる命中補正 最大二倍
	if(suitable_item) chance += chance * ref_skill_exp(SKILL_THROWING) / 8000;
	else chance += chance * ref_skill_exp(SKILL_THROWING) / 16000;

	if(mode == 3) chance *= 3;

	//恐怖状態だと命中率減少
	if(p_ptr->afraid) chance = chance * 2 / 3;

	/* Save the old location */
	prev_y = y;
	prev_x = x;

	//「咲夜の世界」
	if(SAKUYA_WORLD)
	{
		sakuya_time_stop(FALSE);
	}

	/* Travel until stopped */
	/*:::投げたものが飛ぶ処理*/
	for (cur_dis = 0; cur_dis <= tdis; )
	{
		/* Hack -- Stop at the target */
		if ((y == ty) && (x == tx)) break;

		/* Calculate the new location (see "project()") */
		ny[cur_dis] = y;
		nx[cur_dis] = x;
		mmove2(&ny[cur_dis], &nx[cur_dis], py, px, ty, tx);

		/* Stopped by walls/doors */
		if (!cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_PROJECT))
		{
			hit_wall = TRUE;
			/*:::壁に当たって、投げたのが薬、人形、あるいは壁の中にモンスターがいた場合ループを抜ける*/
			if ((q_ptr->tval == TV_FIGURINE) || object_is_potion(q_ptr) || !cave[ny[cur_dis]][nx[cur_dis]].m_idx) break;
		}

		//二宮金次郎像特殊処理
		if(mode == 6 && !cave[ny[cur_dis]][nx[cur_dis]].m_idx)
		{
			feature_type *f_ptr = &f_info[cave[ny[cur_dis]][nx[cur_dis]].feat];
			if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_LAVA))
			{
				msg_format("%sは溶岩に沈んでいった...",o_name);
				return TRUE;
			}
			if (cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_ACID_PUDDLE))
			{
				msg_format("%sは酸の沼に沈んでいった...", o_name);
				return TRUE;
			}
			if (cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_POISON_PUDDLE))
			{
				msg_format("%sは毒の沼に沈んでいった...", o_name);
				return TRUE;
			}

			if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_WATER))
			{
				msg_format("%sは水中に沈んでいった...",o_name);
				return TRUE;
			}
			if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_STAIRS))
			{
				if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_MORE))				
					msg_format("%sは階段を駆け降りていった...",o_name);
				else
					msg_format("%sは階段を駆け上がっていった...",o_name);
				return TRUE;
			}
			if(cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_CAN_FLY) && !cave_have_flag_bold(ny[cur_dis], nx[cur_dis], FF_MOVE))
			{
				msg_format("%sは深い穴に落ちていった...",o_name);
				return TRUE;
			}
			if(have_flag(f_ptr->flags,FF_TRAP))
			{
				disclose_grid(ny[cur_dis], nx[cur_dis]);
				if(f_ptr->subtype == TRAP_TRAPDOOR)
				{
					msg_format("%sは落とし戸に落ちていった...",o_name);
					return TRUE;
				}
				else if(f_ptr->subtype == TRAP_PIT || f_ptr->subtype == TRAP_SPIKED_PIT || f_ptr->subtype == TRAP_POISON_PIT)
				{
					msg_format("%sは落とし穴に落ちた！",o_name);
					return TRUE;
				}
				else if(f_ptr->subtype == TRAP_TELEPORT)
				{
					msg_format("%sはどこかに消えた。",o_name);
					return TRUE;
				}
				else if(f_ptr->subtype == TRAP_BLIND || f_ptr->subtype == TRAP_CONFUSE || f_ptr->subtype == TRAP_POISON || f_ptr->subtype == TRAP_SLEEP)
				{
					msg_format("%sにガスが吹きつけられた。",o_name);
				}
				else
				{
					msg_format("%sの足元でカチリと音がしたが何も起こらなかった。",o_name);
				}
			}



		}

		/* The player can see the (on screen) missile */
		if (panel_contains(ny[cur_dis], nx[cur_dis]) && player_can_see_bold(ny[cur_dis], nx[cur_dis]))
		{
			char c = object_char(q_ptr);
			byte a = object_attr(q_ptr);

			/* Draw, Hilite, Fresh, Pause, Erase */
			print_rel(c, a, ny[cur_dis], nx[cur_dis]);
			move_cursor_relative(ny[cur_dis], nx[cur_dis]);
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, msec);
			if(mode != 3) 
			{
				lite_spot(ny[cur_dis], nx[cur_dis]);
				Term_fresh();
			}
		}

		/* The player cannot see the missile */
		else
		{
			/* Pause anyway, for consistancy */
			Term_xtra(TERM_XTRA_DELAY, msec);
		}

		/* Save the old location */
		prev_y = y;
		prev_x = x;

		/* Save the new location */
		x = nx[cur_dis];
		y = ny[cur_dis];

		/* Advance the distance */
		cur_dis++;

		/* Monster here, Try to hit it */
		if (cave[y][x].m_idx)
		{
			cave_type *c_ptr = &cave[y][x];

			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			int mon_ac = r_ptr->ac;

			//v1.1.94 モンスター防御力低下中はAC25%カット
			if (MON_DEC_DEF(m_ptr))
			{
				mon_ac = MONSTER_DECREASED_AC(mon_ac);
			}

			visible = m_ptr->ml;
			hit_body = TRUE;

			/* Did we hit it (penalize range) */
			//ハルコンネン2と二宮金次郎像は必中
			if (test_hit_fire(chance - cur_dis, r_ptr->ac, m_ptr->ml) || mode == 5 || mode == 6)
			{
				bool fear = FALSE;

				if (!visible)
				{
					msg_format("%sが敵を捕捉した。", o_name);
				}
				else
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, 0);
					if(mode == 3)
						msg_format("無数の%sが%sに向けて収縮した！", o_name, m_name);
					else
						msg_format("%sが%sに命中した。", o_name, m_name);
					if (m_ptr->ml)
					{
						if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);
						health_track(c_ptr->m_idx);
					}
				}
				//v1.1.27 銃をぶん投げたときは別処理にする
				if(q_ptr->tval == TV_GUN)
				{
					gun_throw_effect(y,x,q_ptr);
					//投擲と銃の両方の経験値を得ることにしておく
					if(mode != 5 && mode != 6)
					{
						gain_skill_exp(SKILL_THROWING, m_ptr);
						gain_skill_exp(TV_GUN, m_ptr);
					}
				}
				else
				{
					int tmp_mode = 0;

					//「ディアジェノサイダー」のとき動物スレイを追加するためのパラメータ
					if (mode == 7) tmp_mode = HISSATSU_SLAY_ANIMAL;

					dd = q_ptr->dd;
					ds = q_ptr->ds;
					torch_dice(q_ptr, &dd, &ds);


					if(suitable_item && object_is_artifact(q_ptr)) dd *= 2; //投擲向きのアーティファクトを投げた時のボーナス追加
				
					tdam = damroll(dd, ds);
					tdam = tot_dam_aux(q_ptr, tdam, m_ptr, tmp_mode, TRUE);
					tdam = critical_shot(q_ptr->weight, q_ptr->to_h, tdam);
					if (q_ptr->to_d > 0)
						tdam += q_ptr->to_d;
					else
						tdam += -q_ptr->to_d;

					if (suitable_item)
					{
						tdam *= (2+mult);
						tdam += p_ptr->to_d_m;
					}
					else
					{
						tdam *= mult;
					}

					if (tdam < 0) tdam = 0;

					///mod150620 ダメージに重量ボーナスがつくようにしてみた
					///mod160625 ちょっとダイス修正
					tdam += damroll(3, MAX(q_ptr->weight/10,1));

					tdam = mon_damage_mod(m_ptr, tdam, FALSE);

					if (p_ptr->wizard)
						msg_format("%d/%dのダメージを与えた。",  tdam, m_ptr->hp);

					//投擲スキルを得る
					if(mode != 5 && mode != 6) gain_skill_exp(SKILL_THROWING, m_ptr);

					if (mon_take_hit(c_ptr->m_idx, tdam, &fear, extract_note_dies(real_r_ptr(m_ptr))))
					{
						/* Dead monster */
					}

					/* No death */
					else
					{
						message_pain(c_ptr->m_idx, tdam);
						if ((tdam > 0) && !object_is_potion(q_ptr))
							anger_monster(m_ptr);

						//v2.0.5 投擲にターゲット変更(反撃召喚など用)が設定されていなかった
						set_target(m_ptr, py, px);


						if (fear && m_ptr->ml && mode != 5)
						{
							char m_name[80];
							sound(SOUND_FLEE);
							monster_desc(m_name, m_ptr, 0);
							msg_format("%^sは恐怖して逃げ出した！", m_name);

						}
					}
				}
			}

			break;
		}
	}
	if(mode == 3) 
	{
		p_ptr->redraw |= PR_MAP;
		redraw_stuff();
	}

	if (hit_body) torch_lost_fuel(q_ptr);

	/* Chance of breakage (during attacks) */
	j = (hit_body ? breakage_chance(q_ptr) : 0);

	//破壊率設定
	if(mode == 1 || mode == 5 || mode == 6) j=100;

	if ((q_ptr->tval == TV_FIGURINE) && !(p_ptr->inside_arena))
	{
		j = 100;

		if (!(summon_named_creature(0, y, x, q_ptr->pval,  !(object_is_cursed(q_ptr)) ? PM_FORCE_PET : 0L)))
			msg_print("人形は捻じ曲がり砕け散ってしまった！");
		else if (object_is_cursed(q_ptr))
			msg_print("これはあまり良くない気がする。");
	}

	//v1.1.43 オニフスベは破裂して混乱ボール発生
	if (q_ptr->tval == TV_MUSHROOM && q_ptr->sval == SV_MUSHROOM_PUFFBALL)
	{
		if (hit_body || hit_wall)
		{
			msg_format("%sは破裂した！", o_name);
			project(0, 3, y, x, damroll(8, 12), GF_CONFUSION, (PROJECT_JUMP | PROJECT_KILL), -1);
			do_drop = FALSE;
		}
		else j = 0;
	}

	//うどんげメディスンチェスト投擲
	if (q_ptr->tval == TV_COMPOUND && q_ptr->sval == SV_COMPOUND_MEDICINE_CHEST)
	{
		int potion_count = 0;
		int rad;

		do_drop = FALSE;

		//追加インベントリループ まずインベントリ内の薬の数を数える
		for (i = 0; i<INVEN_ADD_MAX; i++)
		{
			object_type *tmp_o_ptr = &inven_add[i];
			if (object_is_potion(tmp_o_ptr)) potion_count += tmp_o_ptr->number;

		}

		//薬の数に応じて薬のボールの散らばり具合を設定
		if (potion_count < 3) rad = 0;
		else if (potion_count < 20) rad = 1;
		else if (potion_count < 50) rad = 2;
		else if (potion_count < 100) rad = 3;
		else  rad = 4;

		//追加インベントリループ インベントリ内のアイテムを割ったり落としたりする
		for (i = 0; i<INVEN_ADD_MAX; i++)
		{
			int k;

			object_type *tmp_o_ptr = &inven_add[i];

			if (!tmp_o_ptr->k_idx) continue;

			//同じダミーアイテムが中に入っていた場合何もしない。ウィザードコマンドでわざとやらないと起こり得ないが無限ループ防止に一応入れとく
			if (tmp_o_ptr->tval == TV_COMPOUND && tmp_o_ptr->sval == SV_COMPOUND_MEDICINE_CHEST) continue;

			//薬なら割れる
			if (object_is_potion(tmp_o_ptr))
			{

				int count = tmp_o_ptr->number;
				
				//中の薬が一つずつ割れる処理
				for (k = 0; k<count; k++)
				{
					int cx, cy;

					scatter(&cy, &cx, y, x, rad, 0);
					//薬の爆発処理。*爆発*の薬はちょっと強すぎるので普通の爆発にする
					if (tmp_o_ptr->tval == TV_COMPOUND && tmp_o_ptr->sval == SV_COMPOUND_STAR_DETORNATION)
						project(0, 7, cy, cx, (800 + randint1(800)), GF_NUKE, (PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID), -1);
					else
						potion_smash_effect(0, cy, cx, tmp_o_ptr->k_idx);
				}

			}
			//薬以外はそのまま地面に落ちる
			else
			{
				drop_near(tmp_o_ptr, 0, y, x);

			}
			//重量減少
			p_ptr->total_weight -= tmp_o_ptr->weight * tmp_o_ptr->number; 

			//インベントリのアイテムを消す
			object_wipe(&inven_add[i]);
		}
		

	}

	else if (object_is_potion(q_ptr))
	{
		if (hit_body || hit_wall || (randint1(100) < j))
		{
			if(mode != 5) msg_format("%sは砕け散った！", o_name);

			if (potion_smash_effect(0, y, x, q_ptr->k_idx))
			{
				monster_type *m_ptr = &m_list[cave[y][x].m_idx];

				/* ToDo (Robert): fix the invulnerability */
				if (cave[y][x].m_idx &&
				    is_friendly(&m_list[cave[y][x].m_idx]) &&
				    !MON_INVULNER(m_ptr) &&
					m_ptr->r_idx != MON_MASTER_KAGUYA					
					)
				{
					char m_name[80];
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
					msg_format("%sは怒った！", m_name);
					set_hostile(&m_list[cave[y][x].m_idx]);
				}
			}
			do_drop = FALSE;
		}
		else
		{
			j = 0;
		}
	}
	/* Drop (or break) near that location */
	if (mode == 3)
	{
		msg_format("%sは消滅した。", o_name);
	}
	else if (do_drop)
	{
		if (cave_have_flag_bold(y, x, FF_PROJECT))
		{
			/* Drop (or break) near that location */
			(void)drop_near(q_ptr, j, y, x);
		}
		else
		{
			/* Drop (or break) near that location */
			(void)drop_near(q_ptr, j, prev_y, prev_x);
		}
	}

	return TRUE;
}


//雲山が一輪を守る判定
bool unzan_guard(void)
{
	int chance = 15 + p_ptr->lev / 5;
	if(p_ptr->pclass != CLASS_ICHIRIN)	return FALSE;
//最終技の発動中は効果が出ない？
	if(p_ptr->chp < p_ptr->mhp / 2) chance += 50 * (p_ptr->mhp - p_ptr->chp)  / p_ptr->mhp ;

	if(p_ptr->afraid || p_ptr->paralyzed || p_ptr->confused || p_ptr->alcohol >= DRANK_3) chance += 30;

	if(chance > 80) chance = 80;

	if(inventory[INVEN_RARM].k_idx) chance /= 2;
	if(inventory[INVEN_LARM].k_idx) chance /= 2;

	if(randint1(100) < chance)
	{
		msg_print("雲山があなたを守った！");
		return TRUE;
	}
	else	return FALSE;
}
/*:::妹紅リザレクション*/
void mokou_resurrection(void)
{
		int heal_mult = MIN(100,p_ptr->csp * 2);
		int dam = (p_ptr->lev * 8 + adj_general[p_ptr->stat_ind[A_CHR]] * 10 + 300);

		msg_format("『 -リザレクション- 』");
		flag_mokou_resurrection = FALSE;
		p_ptr->magic_num1[0] += 1; //復活回数カウント

		set_poisoned(0);
		set_confused(0);
		set_paralyzed(0);
		set_blind(0);
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

		//死亡時リザレクションはMPを消費する
		if(p_ptr->chp < 0)
		{
			p_ptr->csp -= 50;
			if(p_ptr->csp < 0) p_ptr->csp = 0;
		}
		p_ptr->chp = MAX(p_ptr->chp,(p_ptr->mhp * heal_mult / 100));
		//msg_format("mult:%d hp:%d mp:%d",heal_mult,p_ptr->chp,p_ptr->csp);
		project(0,(1 + p_ptr->lev / 16),py,px,dam,GF_FIRE, (PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID),-1);
		inven_damage(set_fire_destroy, 5);
		p_ptr->redraw |= (PR_MANA | PR_HP);
		handle_stuff();

}

/*:::現在の神奈子のパワーアップ度合いを得る*/
//p_ptr->magic_num1[0]を信仰値に使う。
//p_ptr->magic_num2[0,1]をボス打倒フラグに使う。
int kanako_rank(void)
{
	if(p_ptr->pclass != CLASS_KANAKO)
	{
		msg_print("ERROR:神奈子以外でkanako_rank()が呼ばれた");
		return 0;
	}

	if(p_ptr->magic_num1[0] >= 15000) return 7;
	if(p_ptr->magic_num1[0] >= 8000) return 6;
	if(p_ptr->magic_num1[0] >= 4500) return 5;
	if(p_ptr->magic_num1[0] >= 2500) return 4;
	if(p_ptr->magic_num1[0] >= 1200) return 3;
	if(p_ptr->magic_num1[0] >= 500) return 2;
	if(p_ptr->magic_num1[0] >= 200) return 1;
	else return 0;
}

/*:::現在の神奈子のパワーアップ度合いのコメントを得る*/
cptr kanako_comment(void)
{
	if(p_ptr->pclass != CLASS_KANAKO)
	{
		msg_print("ERROR:神奈子以外でkanako_comment()が呼ばれた");
		return "";
	}
	switch(kanako_rank())
	{
	case 0:
		return "ほとんど信仰されていない・・";
	case 1:
		return "ちょっと信仰が集まってきた気がする。";
	case 2:
		return "少しは信仰されるようになってきたようだ。";
	case 3:
		return "山だけでなく人里でも名が売れてきたらしい。";
	case 4:
		return "様々な人妖からの信仰が集まってくるのを感じる・・";
	case 5:
		return "もう幻想郷であなたの名を知らぬものはいない。";
	case 6:
		return "あなたは今や幻想郷で最も信仰を集めている神様だ。";
	case 7:
		return "あなたはついに幻想郷の信仰全てを手中にした！";
	default:
		return format("ERROR:神奈子メッセージ(rank:%d)が登録されていない",kanako_rank());

	}
}


void kanako_get_point(monster_type *m_ptr)	
	{
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		int kanako_old_rank = kanako_rank();
		int get_point = 0;
		int need_level_list[8] = {0, 10, 20, 30, 40, 50, 60, 0}; //各ランクで信仰値を得るために最低限必要な敵レベル
		byte kanako_old_beat1 = p_ptr->magic_num2[0];
		byte kanako_old_beat2 = p_ptr->magic_num2[1];

		if(m_ptr->r_idx < 1)
			{msg_print("ERROR:kanako_get_point()のr_idxがおかしい"); return; }

		if(p_ptr->pclass != CLASS_KANAKO)
			{msg_print("ERROR:神奈子以外でkanako_get_point()が呼ばれた"); return; }

		if(!(r_ptr->flags1 & RF1_UNIQUE || (r_ptr->flags7 & RF7_UNIQUE2) && (r_ptr->flags7 & RF7_VARIABLE)))
			return;


		if(m_ptr->r_idx == MON_BYAKUREN)
		{
			get_point += 300;
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_HIZIRIN;
		}
		if(m_ptr->r_idx == MON_MIKO)
		{
			get_point += 200; //*2
		 	p_ptr->magic_num2[0] |= KANAKO_BEAT_0_MIMIMI;
		}
		if(m_ptr->r_idx == MON_REIMU && r_ptr->level > 89)
		{
			 if(!(p_ptr->magic_num2[0] & KANAKO_BEAT_0_REIMU)) get_point += 500;
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_REIMU;
		}
		if(m_ptr->r_idx == MON_G_CTHULHU)
		{
			get_point += 300; //*2
			p_ptr->magic_num2[0] |= KANAKO_BEAT_0_CTHULHU;
		}

		if(m_ptr->r_idx == MON_TAISAI)
		{
			get_point += 250; //*2
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_OBERON;
		}
		if(m_ptr->r_idx == MON_AZATHOTH)
		{
			get_point += 500; //*2
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_AZAT;
		}
		if(m_ptr->r_idx == MON_MORGOTH)
		{
			get_point += 400; //*2
			 p_ptr->magic_num2[0] |= KANAKO_BEAT_0_MORGOTH;
		}
		if(m_ptr->r_idx == MON_SERPENT) 
		{
			get_point += 3000;
			p_ptr->magic_num2[0] |= KANAKO_BEAT_0_J;
		}

		if(kanako_old_rank == 7) 
		{
			get_point = 0;
		}
		else if(r_ptr->level < need_level_list[kanako_old_rank])
		{
			 if(!get_point) msg_print("もうこのレベルの敵を倒しても信仰を得られないようだ。");
		}
		else
		{
			get_point += r_ptr->level;
		}
		if(r_ptr->flags3 & RF3_DEITY) get_point *= 2;

		p_ptr->magic_num1[0] += get_point;
		if(cheat_xtra && get_point) msg_format("神奈子信仰値%d(+%d)",p_ptr->magic_num1[0],get_point);

		if(kanako_old_beat1 != p_ptr->magic_num2[0] || kanako_old_beat2 != p_ptr->magic_num2[1] ||  kanako_old_rank != kanako_rank())
		{
			msg_print("信仰が増えて力が増した気がする！");
			p_ptr->update |= (PU_BONUS | PU_HP);
			p_ptr->redraw |= (PR_STATS | PU_HP | PU_MANA);
		}

	}


///mod150719 布都の皿割り処理
bool futo_break_plate(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	if(!is_plate_grid(c_ptr))
	{
		return FALSE;
	}

	c_ptr->info &= ~(CAVE_MARK);
	c_ptr->info &= ~(CAVE_OBJECT);
	c_ptr->mimic = 0;
	note_spot(y, x);
	lite_spot(y, x);
	return TRUE;

}

///mod150719 布都一部特技のターゲット自動選定処理
//視界内に皿があれば一番近い皿を、なければ一番近い敵をtargetにセット、それもなければFALSE
bool futo_determine_target(int cy, int cx, int *ty, int *tx)
{
	int x, y, i;
	bool flag_ok = FALSE;
	int dist = 99;
	monster_type *m_ptr;


	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(distance(cy,cx,y,x) > MAX_RANGE) continue;
			if(!projectable(cy,cx,y,x)) continue;
			if(!is_plate_grid(&cave[y][x])) continue;
			if(distance(cy,cx,y,x) > dist) continue;

			dist = distance(cy,cx,y,x);
			flag_ok = TRUE;
			*ty = y;
			*tx = x;
		}
	}

	if(flag_ok) return TRUE;

	for (i = 1; i < m_max; i++)
	{
		m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (is_pet(m_ptr)) continue;
		if (is_friendly(m_ptr)) continue;
		if (!m_ptr->ml) continue;
		if (!projectable(cy, cx, m_ptr->fy,m_ptr->fx)) continue;
		if (!los(cy, cx, m_ptr->fy,m_ptr->fx)) continue;
		if (distance(cy,cx, m_ptr->fy,m_ptr->fx) > dist) continue;
		dist = distance(cy,cx, m_ptr->fy,m_ptr->fx);
		flag_ok = TRUE;
		*ty = m_ptr->fy;
		*tx = m_ptr->fx;

	}

	if(flag_ok) return TRUE;

	return FALSE;

}
//布都用　皿を配置できる場所
bool futo_can_place_plate(int y, int x)
{
	if(!cave_have_flag_bold((y), (x), FF_MOVE) && !cave_have_flag_bold((y), (x), FF_CAN_FLY) || cave_have_flag_bold((y), (x), FF_TRAP) || cave[y][x].info & CAVE_OBJECT)
		return FALSE;
	else
		return TRUE;

}



///mod150801 スターサファイアの爆弾カウントダウンと爆発
//v1.1.14 爆弾の爆発+誘爆処理
//project()の再帰のし過ぎと思われるフリーズが起こったので作り直した。
//爆弾のカウントダウン値がマイナスになると呼ばれる。
//爆弾の誘爆はカウント加算でなく必ず起こることにした。
//x,y:爆発する爆弾がある座標

void bomb_count(int y, int x, int num)
{
	cave_type       *c_ptr;
	//爆発する爆弾の場所を格納　処理は[0]から始まる
	byte explode_array_x[MAX_HGT * MAX_WID];
	byte explode_array_y[MAX_HGT * MAX_WID];
	//爆発する爆弾数合計
	s16b explode_num_total = 1;
	//爆発する爆弾のうち、誘爆判定が済んだ数
	s16b explode_num_induce_check = 0;

	bool flag_seen = FALSE;
	int xx,yy,cx,cy,i;
	int rad = 3; //爆弾の誘爆判定半径

	c_ptr = &cave[y][x];
	if (!((c_ptr->info & CAVE_OBJECT) && have_flag(f_info[c_ptr->mimic].flags, FF_BOMB)))
	{
		return;
	}
	//v1.1.14 カウントダウンがすでに済んでいる爆弾には何もしない。すでに誘爆リストに入っているはず。
	if(c_ptr->special <= 0) return;

	//カウントダウンを進め、0以下になったら先へ
	c_ptr->special += num;
	if(c_ptr->special > 999) c_ptr->special = 999;

	if(c_ptr->special > 0) return;

	if(player_can_see_bold(y,x)) flag_seen = TRUE;

	explode_array_x[0] = x;
	explode_array_y[0] = y;

	//誘爆判定未処理の爆弾ごとにループ
	while(1)
	{
		int rad_tmp;
		cx = explode_array_x[explode_num_induce_check];
		cy = explode_array_y[explode_num_induce_check];

		for(rad_tmp=1;rad_tmp <= rad;rad_tmp++)
		{
			for(xx = cx - rad_tmp;xx <= cx + rad_tmp; xx++)
			{
				for(yy = cy - rad_tmp;yy <= cy + rad_tmp; yy++)
				{
					//中心から誘爆判定するため少々奇妙なループをする
					if(cx == xx && cy == yy) continue;
					if(!in_bounds(yy,xx)) continue;
					if(rad_tmp > 1 && distance(yy,xx,cy,cx) != rad_tmp) continue;

					c_ptr = &cave[yy][xx];
					//今チェックしている爆弾から射線が通る場所に爆弾があれば爆風が届いて誘爆
					if(!(c_ptr->info & CAVE_OBJECT) || !have_flag(f_info[c_ptr->mimic].flags, FF_BOMB))continue;
					if(rad_tmp > 1 && !projectable(yy,xx,cx,cy)) continue;
					//カウントダウンが終わっている爆弾は対象外。すでに爆発リストに入っているはずなので。
					if(c_ptr->special <= 0) continue;

					//爆発リストに追加しカウントダウンを0にして爆発個数を加算
					c_ptr->special = 0;
					explode_array_x[explode_num_total] = xx;
					explode_array_y[explode_num_total] = yy;
					explode_num_total++;

					//プレイヤーから見えているフラグ
					if(!flag_seen && player_can_see_bold(yy,xx)) flag_seen = TRUE;
				}
			}
		}

		//誘爆判定が誘爆含む全ての爆弾で済んだらループを抜ける
		explode_num_induce_check++;
		if(explode_num_total == explode_num_induce_check) break;
	}

	disturb(1,1);
	//msg
	if(flag_seen)
	{
		if(explode_num_total < 4) msg_print("爆弾が爆発した！");
		else if(explode_num_total < 10) msg_print("いくつもの爆弾が誘爆した！");
		else  msg_print("爆弾の山が大爆発した！");
	}
	//ルナチャが能力を使っているときは爆音が聞こえない
	else if(!((p_ptr->pclass == CLASS_LUNAR || p_ptr->pclass == CLASS_3_FAIRIES) && (p_ptr->tim_general[0] || p_ptr->tim_general[1])))
	{
		if(explode_num_total < 2) msg_print("爆音が聞こえた。");
		else if(explode_num_total < 10) msg_print("いくつもの爆音が響いた！");
		else  msg_print("巨大な爆音が大地を揺るがした！");
	}
	else
	{
		msg_print("ダンジョンの空気が震えた気がする...");
	}

	//リストに登録した爆弾の爆発処理
	for(i=0;i<explode_num_total;i++)
	{
		cx = explode_array_x[i];
		cy = explode_array_y[i];

		if(!in_bounds(cy,cx) || !(cave[cy][cx].info & CAVE_OBJECT) || !have_flag(f_info[cave[cy][cx].mimic].flags, FF_BOMB))
		{
			msg_format("ERROR:爆弾爆発処理 array[%d],(%d,%d)がおかしい",i,cx,cy);
			return;
		}
		c_ptr = &cave[cy][cx];
		c_ptr->info &= ~(CAVE_MARK);
		c_ptr->info &= ~(CAVE_OBJECT);
		c_ptr->mimic = 0;
		c_ptr->special = 0;
		note_spot(cy, cx);
		lite_spot(cy, cx);

		//v1.1.21 兵士
		if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_ENGINEER,SS_E_POWERUP_BOMB))
			project(PROJECT_WHO_EXPLODE_BOMB, 4, cy, cx, 150 + p_ptr->lev * 6,GF_ROCKET, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
		else
			project(PROJECT_WHO_EXPLODE_BOMB, 3, cy, cx, 100 + p_ptr->lev * 4,GF_ROCKET, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
	}





}

/*:::アリスの人形の攻撃回数を計算する。最低値は常に1で実際にその人形を使えるレベルでない場合は0。*/
/*:::インベントリ処理にはmax_levを使うため、レベルアップで人形が増えたあとレベルダウンしても普通に攻撃可能。*/
int calc_doll_blow_num(int doll_num)
{
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	int add_blows = 0;
	int add_blows_tmp = 0;
	int i;
	int num;
	int doll_total_num;
	int id;

	if(p_ptr->pclass != CLASS_ALICE)
	{
		msg_format("ERROR:アリス以外でcalc_doll_blow_num()が呼ばれた");
		return 0;
	}

	doll_total_num = calc_inven2_num();	

	/*:::まだ使えない人形は0*/
	if(doll_total_num < doll_num + 1) return 0;

	o_ptr = &inven_add[doll_num];
	/*:::該当の人形が武器を装備していない場合0*/
	if(!o_ptr->k_idx) return 0; 
	if(!object_is_melee_weapon(o_ptr)) return 0;
	//毒針は常に1回攻撃
	if(o_ptr->tval == TV_KNIFE && o_ptr->sval == SV_WEAPON_DOKUBARI) return 1;

	/*:::該当の人形が装備している武器の追加攻撃適用*/
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_BLOWS)) add_blows += o_ptr->pval;
	
	/*:::アリスの防具とアクセサリの追加攻撃を適用 pval+1につき二体、上海から順番に増える*/
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];
		if (!o_ptr->k_idx) continue;
		/*:::変身で無効化されている装備部位を弾く。 関係ないはずだが一応*/
		if(check_invalidate_inventory(i)) continue;
		object_flags(o_ptr, flgs);
		if (have_flag(flgs, TR_BLOWS)) add_blows_tmp += o_ptr->pval * 2;
	}
	add_blows += add_blows_tmp / doll_total_num + ((add_blows_tmp % doll_total_num) > doll_num)?1:0;


	/*:::(INT+DEX値を攻撃回数計算基礎値にする*/
	id = p_ptr->stat_ind[A_INT] + 3 + p_ptr->stat_ind[A_DEX] + 3;
	switch(doll_num)
	{
	//上海 id80で4,100で6
	case INVEN_DOLL_SHANHAI:
		if(id > 80) num = (4 + (id-80) / 8);
		else num = (id+10) / 20;
		break;
	//蓬莱 id80で4,100で6 少し上海より上がるのが遅い
	case INVEN_DOLL_HORAI:
		if(id > 80) num = (4 + (id-83) / 8);
		else num = (id+5) / 20;
		break;

	//ロシア id80で3,100で4
	case INVEN_DOLL_RUSSIA:
		if(id > 80) num = (3 + id / 85);
		else num = (id+15) / 25;
		break;
	//ロンドン id80で3,100で4 フランスより遅い
	case INVEN_DOLL_LONDON:
		if(id > 80) num = (3 + id / 90);
		else num = (id+10)/25;
		break;
	//オルレアン id80で4,100で6 京より遅い
	case INVEN_DOLL_ORLEANS:
		if(id > 80) num = (3 + id / 95);
		else num = (id+5) / 25;
		break;
		//京 id80で3,100で4 ロンドンより遅い
	case INVEN_DOLL_KYOTO:
		if(id > 80) num = (3 + id / 100);
		else num = (id) / 25;
		break;
	//ゴリアテ id80で2,100で4
	case INVEN_DOLL_GOLIATH:
		if(id > 80) num = 2 + (id-80) / 10;
		else num = (id) / 35;
		break;
	default:
		msg_format("ERROR:calc_doll_blow_num()において未定義の人形(%d)が呼ばれた",doll_num);
		return 0;
	}
	num += add_blows;
	//重量超過ペナルティ
	if(heavy_armor()) num = MAX(1,num * 3 / 4 );
	//片腕装備ペナルティ
	if(inventory[INVEN_LARM].k_idx) num = MAX(1,num * 3 / 4 );
	if(num < 1) return 1;

	return num;

}


///mod150811 魔理沙
/*:::魔理沙による素材からの魔力エッセンス抽出 家で抽出しないときは量が少し減る　行動順消費するときTRUE*/
bool marisa_extract_material(bool in_home)
{

	int choice;
	int item, item_pos;
	int amt;
	s32b price, value, dummy;
	object_type forge;
	object_type *q_ptr;
	object_type *o_ptr;
	cptr q, s;
	char o_name[MAX_NLEN];
	int i, base_point, total_point;


	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print("今は闇市場の調査中だ。アビリティカードを探しに行こう。");
		return FALSE;
	}

	q = "何から魔力を抽出しようか？";
	s = "魔法の材料になりそうなものを持っていない。";

	item_tester_hook = marisa_will_buy;
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

	if (item >= 0) 	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];

	amt = 1;
	if (o_ptr->number > 1)
	{
		amt = get_quantity(NULL, o_ptr->number);
		if (amt <= 0) return FALSE;
	}
	q_ptr = &forge;
	object_copy(q_ptr, o_ptr);
	q_ptr->number = amt;

	object_desc(o_name, q_ptr, 0);

	if(!get_check(format("%sから抽出しますか？",o_name))) return FALSE;

	marisa_gain_power(q_ptr,(in_home)?100:(p_ptr->lev + 30));

	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
	return TRUE;

}

/*:::魔理沙の魔法の効果、必要エッセンスを確認する*/
bool check_marisa_recipe(void)
{
	int num;
	char buf[1024];
	int i;


	if (is_special_seikaku(SEIKAKU_SPECIAL_MARISA))
	{
		msg_print("今は闇市場の調査中だ。アビリティカードを探しに行こう。");
		return FALSE;
	}

	while(1)
	{
		int j;
		cptr info_ptr;
		num = choose_marisa_magic(CMM_MODE_CHECK);
		if(num < 0) return FALSE;

		screen_save();

		//建物内かそうでないかで画面クリアする領域を変えておく。上のscreen_save()内でcharacter_icky++されるのでTRUE/FALSEでは判定できない。
		if(character_icky > 1)
			clear_bldg(4,22);
		else
			for(i=4;i<22;i++) Term_erase(17, i, 255);

		prt(format("%s%s",marisa_magic_table[num].name,marisa_magic_table[num].is_drug?"生成":""),4,18);
		sprintf(buf,"%s",marisa_magic_table[num].info);
		prt(format("%s",use_marisa_magic(num,TRUE)), 5, 18);

		//rumor_newと同じ\\による自動改行
		for(i=0;i < sizeof(buf)-2;i++)
		{
			if(buf[i] == '\0')	break; 
			/*:::一部漢字が改行扱いされてしまうのを抑止*/
			else if(iskanji(buf[i]))
			{
				i++;
				continue;
			}
			else if(buf[i] == '\\') buf[i] = '\0';
		}
		buf[i] = '\0';
		buf[i+1] = '\0';
		info_ptr = buf;
		for (i = 0; i< 4; i++)
		{
			if(!info_ptr[0]) break;
			prt(info_ptr, 7+i, 18);
			info_ptr += strlen(info_ptr) + 1;
		}
		prt("必要な魔力:", 12, 18);
		j=13;
		for(i=0;i<8;i++)
		{
			if(!marisa_magic_table[num].use_magic_power[i]) continue;
			prt(format("%sの魔力(%d)",marisa_essence_name[i],marisa_magic_table[num].use_magic_power[i]),j++,18);
		}


		prt("何かキーを押してください", 0, 0);
		(void)inkey();
		prt("", 0, 0);

		screen_load();
	}


	return TRUE;
}


void msg_marisa_magic(int num)
{
	if(marisa_magic_table[num].is_drug)
	{
		if(one_in_(4)) msg_format("ポケットから%sを取り出して飲んだ。",marisa_magic_table[num].name);
		else if(one_in_(3)) msg_format("帽子の中から%sを取り出して飲んだ。",marisa_magic_table[num].name);
		else if(one_in_(2)) msg_format("懐から%sを取り出して飲んだ。",marisa_magic_table[num].name);
		else msg_format("スカートの中から%sを取り出して飲んだ。",marisa_magic_table[num].name);
	}
	else 
		msg_format("%sを放った！",marisa_magic_table[num].name);
}


/*:::魔理沙の魔法実行部*/
/*:::numはmarisa_magic_table[]の添字と一致すること*/
cptr use_marisa_magic(int num, bool only_info)
{
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int i;
	int plev = p_ptr->lev;
	int dir;
	
	switch(num)
	{
		case 0: //八卦ファイア 500-750
		{
			int dam = 300 + plev * 6 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_ball(GF_FIRE, dir, dam, -2);
		}
		break;
		case 1: //ポラリスユニーク 320-400
		{
			int dam = 200 + plev * 3 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_rocket(GF_TELEKINESIS, dir, dam, 2);
		}
		break;

		case 2: //ナロースパーク 450-700
		{
			int dam = 300 + plev * 5 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_beam(GF_ELEC, dir, dam);
		}
		break;

	/*
		case 2: //ミリ秒パルサー 250-1000?
		{
			int x, y;
			char m_name[160];
			int dam = 30 + MAX(p_ptr->to_d[0],p_ptr->to_d[1]);
			int num = 3 + plev / 12;
			if(only_info) return format("損傷:%d * %d",dam, num);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
//トラップ・ドア・敵・パターン・vault内で動作確認
			msg_marisa_magic(num);
			for(i=0;i<num;i++)
			{
				y = py + ddy[dir];
				x = px + ddx[dir];
				if(!in_bounds(y,x))
				{
					break;
				}
				else if(cave[y][x].m_idx)
				{
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
					msg_format("%sに体当たりした！",m_name);
					project(0, 0, y, x, dam, GF_DISP_ALL , (PROJECT_JUMP | PROJECT_KILL), -1);
				}
				else if(player_can_enter(cave[y][x].feat, 0))
				{
					teleport_player_to(y, x, TELEPORT_NONMAGICAL);
				}
				else break;
			}
		}
		break;
*/
		case 3: //ステラミサイル 400-650
		{
			int dam = 200 + plev * 6 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_ball_jump(GF_METEOR, dir, dam, 1, NULL);
		}
		break;

		case 4: //ウィッチレイライン 300-400
		{
			int len = 10 + plev / 10;
			int damage = 150 + plev * 5;
			if(only_info) return format("射程:%d 損傷:%d",len,damage);
			if (!rush_attack2(len,GF_MANA,damage,0)) return NULL;
			break;			
		}
		break;

		case 5: //丹
		{
			int heal = 300;
			
			if(only_info) return format("回復:%d",heal);
			msg_marisa_magic(num);
			hp_player(heal);
			set_cut(0);	
			set_poisoned(0);
		}
		break;
		case 6: //ルミネスストライク 200-400
		{
			int sides = 50 + chr_adj * 5;
			int base = 100 + plev * 2 ;

			if(only_info) return format("損傷:%d+1d%d",base,sides);
			if (!get_aim_dir(&dir)) return NULL;
			msg_marisa_magic(num);
			fire_rocket(GF_LITE,dir,base + randint1(sides),1);

			break;
		}

		case 7: //エスケープベロシティ300-400
		{
			int dam = 350 + plev * 6 + chr_adj * 8;
			if(only_info) return format("損傷:%d",dam/2);	
			msg_marisa_magic(num);
			project(0,1,py,px,dam,GF_MANA,(PROJECT_KILL|PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM),-1);
			teleport_player(20,TELEPORT_NONMAGICAL);
			break;
		}

		case 8: //コールドインフェルノ 500-750
		{
			int dam = 300 + plev * 6 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_ball(GF_COLD, dir, dam, -2);
		}
		break;

		case 9: //デビルダムトーチ 600-800
		{
			int len = 5 + plev / 16;
			int damage = 400 + plev * 5 + chr_adj * 5;
			if(only_info) return format("射程:%d 損傷:%d",len,damage);
	
			project_length = len;
			if (!get_aim_dir(&dir)) return NULL;
			msg_marisa_magic(num);
			if(!fire_ball_jump(GF_NETHER,dir,damage,2,NULL)) return NULL;			
			break;			
		}
		break;

		case 10: //マジックドレインミサイル 300-500
		{
			int dam = 200 + plev * 3 + chr_adj * 5;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			drain_life(dir,dam);
		}
		break;

		case 11: //パワフルドラッグ 400-600
		{
			int len = 5 + plev / 16;
			int damage = 250 + plev * 4 + chr_adj * 5;
			if(only_info) return format("射程:%d 損傷:%d",len,damage);
			project_length = len;

			if (!get_aim_dir(&dir)) return NULL;
			msg_marisa_magic(num);
			if(!fire_ball_jump(GF_ACID,dir,damage,3,NULL)) return NULL;			
			break;	
		}
		break;



		case 12: //ドラゴンメテオ 1200-1700
		{
			int len = 2 + plev / 24;
			int dice = 35 + p_ptr->lev / 2;
			int sides = 40 + chr_adj / 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);
	
			if (!get_aim_dir(&dir)) return NULL;
			msg_marisa_magic(num);
			if(!fire_ball_jump(GF_NUKE,dir,damroll(dice,sides),4,NULL)) return NULL;			
			break;	
		}
		break;
		case 13: //オープンユニバース650-1000
		{
			int dam = 400 + plev * 6 + chr_adj * 10;
			if(only_info) return format("損傷:%d",dam);

			msg_marisa_magic(num);
			project_hack2(GF_METEOR,0,0,dam);
			break;
		}
		case 14: //マジックアブソーバー general[0]のカウンタを使う
		{
			int time = 10 + plev / 5;
			if(only_info) return format("期間:%d",time);
			msg_marisa_magic(num);
			set_tim_general(time,FALSE,0,0);

			break;
		}
		case 15: //イベントホライズン 1000-1500
		{
			int dam = 1500 + plev * 20 + chr_adj * 50 / 3;
			int rad = 3 + plev / 9;
			if(only_info) return format("半径:%d 損傷:～%d",rad,dam/2);	
			msg_marisa_magic(num);
			project(0,rad,py,px,dam,GF_GRAVITY,(PROJECT_KILL|PROJECT_JUMP|PROJECT_GRID|PROJECT_ITEM),-1);
			break;
		}
		case 16: //魔力丹
		{
			int mana = 9999;
			
			if(only_info) return format("");
			msg_marisa_magic(num);
			p_ptr->csp += mana;
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= (PR_MANA);
		}
		break;
		case 17: //強力丹
		{
			int heal = 9999;
			
			if(only_info) return format("");
			msg_marisa_magic(num);
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
		}
		break;
		case 18: //スターダストレヴァリエ1000-2000?
		{
			int rad = 2 + plev / 20;
			int damage = 170 + plev * 2 + chr_adj;
			if(only_info) return format("損傷:%d*(10+1d10)",damage);
			msg_marisa_magic(num);
			cast_meteor(damage, rad, GF_LITE);

			break;
		}


		case 19: //オーレリーズソーラーシステム
		{
			int time = 15 + plev / 2;
			if(only_info) return format("期間:%d",time);
			msg_marisa_magic(num);
			set_tim_sh_fire(time,FALSE);
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);

			break;
		}


		case 20: //ビッグクランチ
		{
			int rad = plev;
			if(only_info) return format("範囲:%d",rad);

			msg_marisa_magic(num);
			destroy_area(py,px,rad,FALSE,FALSE,FALSE);
		}
		break;

		case 21: //ダークスパーク 600-1000
		{
			int dice = 25 + p_ptr->lev / 2;
			int sides = 25 + chr_adj / 2;
			if(only_info) return format("損傷:%dd%d",dice,sides);
			
			if (!get_aim_dir(&dir)) return NULL;
			fire_spark(GF_DARK,dir, damroll(dice,sides),1);

		}
		break;

		case 22: //オカルティション
		{
			int power = 200 + plev * 4;
			if(only_info) return format("効力:%d",power);
			mass_genocide(power,TRUE);

			break;
		}
		case 23: //ディープエコロジカルボム 1200-1600
		{
			int rad = 4 + plev / 16;
			int tx=0,ty=0;
			int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP;
			int len = 6 + plev / 16;
			int damage = 500 + plev * 16 + chr_adj * 10;
			if(only_info) return format("射程:%d 半径:%d 損傷:%d",len,rad, damage);
			project_length = len;

			if (!get_aim_dir(&dir)) return NULL;

			if ((dir == 5) && target_okay())
			{
				flg &= ~(PROJECT_STOP);
				tx = target_col;
				ty = target_row;
			}
			if(dir != 5 || (dir == 5) && !target_okay() || !in_bounds(ty,tx) || !projectable(py,px,ty,tx))
			{
				msg_print("視界内の一点を狙わないといけない。");
				return NULL;
			}

			if(distance(py,px,ty,tx) <= rad && !get_check_strict("ターゲットが近すぎる。本当に使いますか？", CHECK_OKAY_CANCEL)) return NULL;

			msg_marisa_magic(num);
			project(PROJECT_WHO_DEEPECO_BOMB, 5, ty, tx, damage, GF_CHAOS, flg, -1);


			break;	
		}
		break;

		case 24: //ファイナルスパーク 1600固定
		{
			int dam = 1600;
			if(only_info) return format("損傷:%d",dam);

			if (!get_aim_dir(&dir)) return FALSE;
			msg_marisa_magic(num);
			fire_spark(GF_DISINTEGRATE,dir, dam,2);

		}
		break;

		case 25: //オーレリーズユニバース
		{
			int time = 14 + plev / 3;
			if(only_info) return format("期間:%d",time);
			msg_marisa_magic(num);
			set_oppose_acid(time, FALSE);
			set_oppose_elec(time, FALSE);
			set_oppose_fire(time, FALSE);
			set_oppose_cold(time, FALSE);
			set_oppose_pois(time, FALSE);
			set_ultimate_res(time, FALSE);
			set_magicdef(time,FALSE);


			break;
		}

		case 26: //アルティメットショートウェーブ 1500
		{
			int y,x;
			int typ = GF_PLASMA;
			int dice = plev;
			int sides = 20 + chr_adj;
			if(only_info) return format("損傷:%dd%d",dice,sides);

			msg_marisa_magic(num);
			for (y = 1; y < cur_hgt-1; y++)	for (x = 1; x < cur_wid-1; x++)
			{
				if(cave_have_flag_bold(y,x,FF_PERMANENT)) continue;
				print_rel('#', (0x30 + spell_color(typ)), y, x);
			}
			Term_fresh();
			Term_xtra(TERM_XTRA_DELAY, delay_factor * delay_factor * delay_factor * 2);

			for (y = 1; y < cur_hgt-1; y++)	for (x = 1; x < cur_wid-1; x++)
			{
				int dam = damroll(dice,sides);
				if(cave_have_flag_bold(y,x,FF_PERMANENT)) continue;
				project_f(0,0,y,x,dam,typ);
				project_o(0,0,y,x,dam,typ);
				project_m(0,0,y,x,dam,typ,(PROJECT_KILL),FALSE);

			}
//			project_hack2(GF_PLASMA,dice,sides,0);
			break;
		}

		case 27: //ブレイジングスター 3200
		{
			int tx, ty;
			int dam = 3200;
			int flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_THRU | PROJECT_FAST | PROJECT_MASTERSPARK | PROJECT_DISI | PROJECT_LONG_RANGE|PROJECT_MOVE);
			if(only_info) return format("損傷:%d",dam);

			if (!get_rep_dir2(&dir)) return NULL;
			if (dir == 5) return NULL;
			msg_marisa_magic(num);

			tx = px + 255 * ddx[dir];
			ty = py + 255 * ddy[dir];
			project(0, 2, ty, tx, dam, GF_BLAZINGSTAR, flg, -1);

		}
		break;


		default:
		if(only_info) return format("未実装");
		msg_format("ERROR:num%dの魔法処理が未実装",num);
		return NULL;
	}
	return "";
}


//月都万象展の罠で出てくる敵のIDXと座標
int moon_vault_mon_list[][3] =
{
	//idx,y,x
	{MON_KAGUYA,5,1},
	{MON_EIRIN,5,3},
	{MON_UDONGE,3,5},
	{MON_TEWI,7,5},

	{MON_YOUKAI_RABBIT2,3,7},
	{MON_YOUKAI_RABBIT2,4,7},
	{MON_YOUKAI_RABBIT2,5,7},
	{MON_YOUKAI_RABBIT2,6,7},
	{MON_YOUKAI_RABBIT2,7,7},
	{MON_YOUKAI_RABBIT2,3,8},
	{MON_YOUKAI_RABBIT2,4,8},
	{MON_YOUKAI_RABBIT2,5,8},
	{MON_YOUKAI_RABBIT2,6,8},
	{MON_YOUKAI_RABBIT2,7,8},

	{MON_FAIRY_EX,3,12},
	{MON_FAIRY_EX,4,12},
	{MON_FAIRY_EX,5,12},
	{MON_FAIRY_EX,6,12},
	{MON_FAIRY_EX,7,12},
	{MON_FAIRY_EX,3,13},
	{MON_FAIRY_EX,4,13},
	{MON_FAIRY_EX,5,13},
	{MON_FAIRY_EX,6,13},
	{MON_FAIRY_EX,7,13},

	{MON_GETTOMECA,5,85},



	{0,0,0}

};

//魔理沙が月都万象展で起動する罠
void activate_moon_vault_trap(void)
{
	int i, x, y;

	if(p_ptr->pclass != CLASS_MARISA || p_ptr->inside_quest != QUEST_MOON_VAULT)
	{
		msg_print("activate_moon_vault_trap()が予期しない呼ばれ方をした");
		return;
	}
	if(quest[QUEST_MOON_VAULT].flags & QUEST_FLAG_SPECIAL)
		return;
	quest[QUEST_MOON_VAULT].flags |= QUEST_FLAG_SPECIAL;

	msg_format("突如、館中に警報音が鳴り響いた！");
	world_monster = -1;
	do_cmd_redraw();

	/*モンスター全抹殺*/
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		if (!m_ptr->r_idx) continue;
		if (i == p_ptr->riding) continue;

		if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
		{
			char m_name[80];

			monster_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_MOVED, m_name);
		}
		delete_monster_idx(i);
	}
	/*一定範囲の岩や森、ルーンなど全破壊*/
	for(y=3;y<8;y++)for(x=2;x<87;x++)
	{
		remove_mirror(y,x);
		cave_set_feat(y,x,feat_floor);
	}
	/*:::モンスター配置*/
	for(i=0;;i++)
	{
		int r_idx = moon_vault_mon_list[i][0];
		y = moon_vault_mon_list[i][1];
		x = moon_vault_mon_list[i][2];
		if(!r_idx) break;

		if (r_info[r_idx].flags1 & RF1_UNIQUE)
		{
			r_info[r_idx].cur_num = 0;
			r_info[r_idx].max_num = 1;
		}
		place_monster_aux(0, y, x, r_idx, (PM_NO_KAGE));
	}
	
	Term_xtra(TERM_XTRA_DELAY, 1000);
	world_monster = 0;
	p_ptr->redraw |= PR_MAP;
	redraw_stuff();
	msg_format("辺りの雰囲気が変わった気がする・・");

}



/*:::プリズムリバー専用 建物内、もしくはキャラメイク時にどのキャラになるか選択する*/
int select_prism_class(void)
{

	int n,choose=0;
	char c;
	bool selected = FALSE;
	cptr namelist[3] = {"ルナサ","メルラン","リリカ"};

	screen_save();	

	put_str("誰と交代しますか？", 10, 10);

	while(1)
	{
		for(n=0;n<3;n++)
		{
			if(p_ptr->pclass == (CLASS_LUNASA+n)) continue;
			else put_str( format("%c)%s",'a'+n,namelist[n]), 12 + n, 10);
		}
		c = inkey();

		if (c == 'a')
		{
			if(p_ptr->pclass == CLASS_LUNASA) continue;
			choose = CLASS_LUNASA;
			break;
		}
		else if (c == 'b')
		{
			if(p_ptr->pclass == CLASS_MERLIN) continue;
			choose = CLASS_MERLIN;
			break;
		}
		else if (c == 'c')
		{
			if(p_ptr->pclass == CLASS_LYRICA) continue;
			choose = CLASS_LYRICA;
			break;
		}
		else if(c == ESCAPE)
		{
			choose = p_ptr->pclass;				
			break;
		}
	}

	screen_load();
	return choose;
}

/*::: Mega Hack - プリズムリバー姉妹専用転職コマンド*/		
void  prism_change_class(bool urgency)
{
	int new_class, i;


	if(!CLASS_IS_PRISM_SISTERS)
	{
		msg_print("ERROR:prism_change_class()が三姉妹以外で呼ばれた");
		return;
	}

	if(urgency)
	{
		do
		{
			new_class = CLASS_LUNASA + randint0(3);
		}while(p_ptr->pclass == new_class);
	}
	else
	{
		new_class = select_prism_class();
		if(p_ptr->pclass == new_class) return;
	}

	if(urgency)
		msg_format("あなたのピンチに%sが助けに来た！",new_class < p_ptr->pclass?"姉":"妹");

	msg_format("あなたは%sと交代した。",new_class < p_ptr->pclass?"姉":"妹");

	//magic_num1/2の[0～1]は歌系特技に使う 念のため[2]も空けとく

	//各キャラの技能熟練度をp_ptr->magic_num1[3-66]に保持する
	for(i=0;i<SKILL_EXP_MAX;i++)
	{
		int tmp_idx1 = 3 + i + SKILL_EXP_MAX * (p_ptr->pclass - CLASS_LUNASA);
		int tmp_idx2 = 3 + i + SKILL_EXP_MAX * (new_class - CLASS_LUNASA);

		if (tmp_idx1 < 0 || tmp_idx2 < 0)
		{
			msg_print("ERROR! : prism_change_class()");
			return;
		}

		p_ptr->magic_num1[tmp_idx1] = p_ptr->skill_exp[i];
		p_ptr->skill_exp[i] = p_ptr->magic_num1[tmp_idx2];
	}

	//突然変異も保持 magic_num2[3-74]
	for(i=0;i<4;i++)
	{
		p_ptr->magic_num2[3+i      + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta1>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 4  + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta2>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 8  + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta3>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 12 + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta1_perma>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 16 + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta2_perma>>(i*8) & (0xFF);
		p_ptr->magic_num2[3+i + 20 + 24 * (p_ptr->pclass - CLASS_LUNASA)] = p_ptr->muta3_perma>>(i*8) & (0xFF);
	}
		p_ptr->muta1 = 0L;
		p_ptr->muta2 = 0L;
		p_ptr->muta3 = 0L;
		p_ptr->muta1_perma = 0L;
		p_ptr->muta2_perma = 0L;
		p_ptr->muta3_perma = 0L;
	for(i=0;i<4;i++)
	{
		p_ptr->muta1       |= (u32b)(p_ptr->magic_num2[3+i      + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta2       |= (u32b)(p_ptr->magic_num2[3+i +  4 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta3       |= (u32b)(p_ptr->magic_num2[3+i +  8 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta1_perma |= (u32b)(p_ptr->magic_num2[3+i + 12 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta2_perma |= (u32b)(p_ptr->magic_num2[3+i + 16 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
		p_ptr->muta3_perma |= (u32b)(p_ptr->magic_num2[3+i + 20 + 24 * (new_class - CLASS_LUNASA)]) <<(i*8);
	}

	p_ptr->magic_num2[75] = 1; 
	//一度でも交代したらnum2[75]にフラグON

	//パラメータ回復
	for(i=0;i<6;i++) p_ptr->stat_cur[i] = p_ptr->stat_max[i];

	p_ptr->pclass = new_class;
	cp_ptr = &class_info[p_ptr->pclass];


	reset_tim_flags();
	set_food(PY_FOOD_FULL - 1);
	p_ptr->exp = p_ptr->max_exp;
	check_experience();

	p_ptr->update |= PU_BONUS | PU_HP | PU_MANA | PU_SPELLS;
	update_stuff();

	p_ptr->chp = p_ptr->mhp;
	p_ptr->chp_frac = 0;
	p_ptr->csp = p_ptr->msp;
	p_ptr->csp_frac = 0;

	//名前、職業表記書き換え
	strcpy(player_class_name, class_info[p_ptr->pclass].f_title);
	if(p_ptr->pclass == CLASS_LUNASA)
		strcpy(player_name, "ルナサ・プリズムリバー");
	else if(p_ptr->pclass == CLASS_MERLIN)
		strcpy(player_name, "メルラン・プリズムリバー");
	else
		strcpy(player_name, "リリカ・プリズムリバー");


}

/*:::別のモンスターに変身する。ドレミー、マミゾウの特技*/
//v1.1.47紫苑も追加 timeが負で呼ばれた場合は時間制限を無視するフラグが立つことにした
void metamorphose_to_monster(int r_idx , int time)
{
	int percen;
	monster_race *r_ptr = &r_info[r_idx];



	//paranoia
	if(r_info[r_idx].flagsr & RFR_RES_ALL)
	{
		msg_print("変身に失敗した。");
		return;
	}

	//v1.1.47 確認やめた
	//if(!get_check_strict("変身しますか？", CHECK_OKAY_CANCEL)) return;


	//変身中のモンスターの名前を専用領域に格納　ランダムユニーク考慮
	//変身中のモンスターidxをp_ptr->metamor_r_idxに格納するようにしたが、ランダムユニークを考慮すると依然これは必要
	if(IS_RANDOM_UNIQUE_IDX(r_idx))
	{
		sprintf(extra_mon_name,"%s",random_unique_name[r_idx - MON_RANDOM_UNIQUE_1]);
	}
	else
	{
		sprintf(extra_mon_name,"%s",r_name + r_ptr->name);
	}

	//モンスターの情報を全て得る
	lore_do_probe(r_idx);

	(void)COPY(&r_info[MON_EXTRA_FIELD],r_ptr,monster_race);

	percen = p_ptr->chp * 100 / p_ptr->mhp;
	//特殊フラグを立てる
	p_ptr->special_defense |= SD_METAMORPHOSIS;
	//v1.1.47 timeが負で呼ばれた場合時間制限を無視するフラグを立てる
	if (time < 0)
	{
		time = -time;
		p_ptr->special_flags |= SPF_IGNORE_METAMOR_TIME;

	}

	if(r_ptr->flags2 & RF2_GIGANTIC)
	{
		if(!set_mimic(time,MIMIC_METAMORPHOSE_GIGANTIC,FALSE)) return;
		msg_print("この姿では魔法書や魔道具、さらに巻物が使えない。");
	}
	else if(is_gen_unique(r_idx) || my_strchr("hknopstuzAFGKLOPSTUVWY", r_ptr->d_char) || r_ptr->flags3 & (RF3_HUMAN | RF3_DEMIHUMAN))
	{
		if(!set_mimic(time,MIMIC_METAMORPHOSE_NORMAL,FALSE)) return;
	}
	else
	{
		if(!set_mimic(time,MIMIC_METAMORPHOSE_MONST,FALSE)) return;
		msg_print("この姿では魔法書や魔道具、さらに巻物が使えない。");
	}

	p_ptr->chp = p_ptr->mhp * percen / 100;

	p_ptr->metamor_r_idx = r_idx; //v1.1.85 変身中のモンスターのr_idxを保持する
	

	p_ptr->update |= (PU_HP | PU_BONUS | PU_MANA | PU_TORCH);
	p_ptr->redraw |= (PR_HP | PR_MANA | PR_STATS | PR_ARMOR);
	update_stuff();




}

///mod150922
/*:::本当に今更だが、MPを回復するルーチンを独立する。
 *気が向いたら過去の記述も置き換えよう。剣術家などMP上限突破する職に注意。
 *既にMPが最大の場合FALSEを返す
 *v2.0.7 得られるMPが0のときにもFALSE
 */
bool player_gain_mana(int mana)
{

	if(p_ptr->csp >= p_ptr->msp) return FALSE;

	if (!mana) return FALSE;

	p_ptr->csp += mana;
	if(p_ptr->csp >= p_ptr->msp)
	{
		p_ptr->csp = p_ptr->msp;
		p_ptr->csp_frac = 0;

	}
	p_ptr->redraw |= (PR_MANA);
	return TRUE;
}




/*:::雛の厄増減処理　厄の保持にはmagic_num1[0]を使う。*/
void hina_gain_yaku(int yaku)
{
	int old_rank,new_rank;

	if(p_ptr->pclass != CLASS_HINA) return;
	if(!yaku) return;

	if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT1) old_rank = 0;
	else if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT2) old_rank = 1;
	else if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT3) old_rank = 2;
	else old_rank = 3;

	p_ptr->magic_num1[0] += yaku;
	if(p_ptr->magic_num1[0] < 0) p_ptr->magic_num1[0] = 0;
	if(p_ptr->magic_num1[0] > 99999) p_ptr->magic_num1[0] = 99999;

	if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT1) new_rank = 0;
	else if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT2) new_rank = 1;
	else if(p_ptr->magic_num1[0] < HINA_YAKU_LIMIT3) new_rank = 2;
	else new_rank = 3;


	if(old_rank != new_rank)
	{
		if(new_rank == 0)
		{
			if(!p_ptr->magic_num1[0])
				msg_print("厄が全て落ちた。");
			else
				msg_print("この程度の厄なら軽く抑え込める。");
		}
		else if(new_rank == 1 && old_rank > new_rank)
		{
			msg_print("厄が減って扱いやすくなった。");
		}
		else if(new_rank == 1 && old_rank < new_rank)
		{
			msg_print("少し厄が溜まってきたようだ。");
		}
		else if(new_rank == 2 && old_rank > new_rank)
		{
			msg_print("厄の負担が減ったがまだ持て余し気味だ。");
		}
		else if(new_rank == 2 && old_rank < new_rank)
		{
			msg_print("厄が重い。持て余した厄があふれ始めた…");
		}
		else if(new_rank == 3)
		{
			msg_print("大量の厄を扱いきれない！このままでは大変なことになる！");
		}
	}

	p_ptr->update |= PU_BONUS;
	p_ptr->redraw |= PR_ARMOR;
}

//咲夜の世界　open=TRUEで開始
void sakuya_time_stop(bool open)
{
	if(open && !SAKUYA_WORLD)
	{
		msg_print("あなたは時間の流れから外れた。");
		p_ptr->special_defense |= SD_SAKUYA_WORLD;
	}
	else if(!open && SAKUYA_WORLD)
	{
		msg_print("再び時間の流れに乗った。");
		p_ptr->special_defense &= ~(SD_SAKUYA_WORLD);
	}
	else return;

	p_ptr->redraw |= (PR_MAP | PR_STATUS);
	p_ptr->update |= (PU_MONSTERS);
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
	handle_stuff();

}



//警告2　警告に従ってキャンセルする場合FALSEが返る　警告に至る条件は外で処理する
bool process_warning2(void)
{
	object_type *o_ptr = choose_warning_item();
	char o_name[MAX_NLEN];

	//警告状態でないとき何もしない
	if(!p_ptr->warning) return TRUE;

	if (o_ptr)
	{
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
		msg_format("%sが鋭く震えた！", o_name);
		}
	else
	{
		if(p_ptr->pclass == CLASS_WRIGGLE)
			msg_format("周囲を飛び回る蟲が警告を発した！");
		else if(p_ptr->pclass == CLASS_ICHIRIN)
			msg_format("雲山が警告の意思を発した！");
		else if (p_ptr->pclass == CLASS_KUTAKA)
			msg_format("あなたの頭上のヒヨコが鋭く鳴いた！");
		else
			msg_format("あなたの第六感が警告を発した！");
	}
	disturb(0, 1);


	return get_check("本当に続けますか？");
}




//雛が呪いフラグ付きのアイテムを装備して戦っていると呪いが解ける処理
//通常の解呪可能な呪いでなく、art_flag,a_info,e_infoのいずれかにフラグがある呪い、もしくはマイナスpval,ac,to_d,to_hが対象
//経験値得るたびに呼ばれるのでちょっと要らん計算多すぎるかも。ゲームが重くなるようなら先に計算値入れて減算する仕組みにしよう
void hina_yakuotoshi(int gain_exp)
{
	int need_exp;
	int i,j;
	char o_name[MAX_NLEN];
	u32b flgs[TR_FLAG_SIZE];
	int tmp_xtra5;

	if(p_ptr->pclass != CLASS_HINA) return;
	if(gain_exp < 10) return;
	
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		need_exp = 0;
		if (!o_ptr->k_idx) continue;
		if(o_ptr->xtra3 == SPECIAL_ESSENCE_OTHER) continue;//すでに反転処理されたものはもう処理しない
		if(o_ptr->tval == TV_CAPTURE) continue;

		object_flags(o_ptr, flgs);
		//厄が落ちるのに必要な経験値(/1000)を算出 xtra5が16bitなので30000を超えないこと
		if(object_is_ego(o_ptr) && o_ptr->name2 == EGO_WEAPON_DEMONLORD) need_exp += 500; //邪神エゴ 50万
		else if(object_is_ego(o_ptr) && e_info[o_ptr->name2].rating == 0) need_exp += 100; //ほか呪いエゴ 10万
		//呪いフラグ　解呪で取れない永続的なもののみが対象
		if(o_ptr->curse_flags & TRC_PERMA_CURSE) need_exp += 1000;//永遠の呪い100万
		if(have_flag(flgs,TR_AGGRAVATE)) need_exp += 500;//反感50万
		if(have_flag(flgs,TR_TY_CURSE)) need_exp += 500;//太古の呪い50万
		if(have_flag(flgs,TR_DRAIN_EXP)) need_exp += 300;//経験値吸収30万
		if(have_flag(flgs,TR_ADD_L_CURSE)) need_exp += 50;//弱い呪いを増やす 5万
		if(have_flag(flgs,TR_ADD_H_CURSE)) need_exp += 300;//強い呪いを増やす 30万
		if(have_flag(flgs,TR_NO_TELE)) need_exp += 100;//反テレポ 10万
		if(have_flag(flgs,TR_NO_MAGIC)) need_exp += 100; //反魔法 10万

		//マイナスpval
		if(o_ptr->pval < 0)
		{
			if(have_flag(flgs,TR_BLOWS)) need_exp += -(o_ptr->pval) * -(o_ptr->pval) * 50; //高マイナス修正の追加攻撃に大幅ペナルティ
			if(have_flag(flgs,TR_SPEED)) need_exp += -(o_ptr->pval) * -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_MAGIC_MASTERY)) need_exp += -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_STR)) need_exp += -(o_ptr->pval) * 15;
			if(have_flag(flgs,TR_INT)) need_exp += -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_WIS)) need_exp += -(o_ptr->pval) * 20;
			if(have_flag(flgs,TR_CON)) need_exp += -(o_ptr->pval) * 20;
			if(have_flag(flgs,TR_DEX)) need_exp += -(o_ptr->pval) * 15;
			if(have_flag(flgs,TR_CHR)) need_exp += -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_STEALTH)) need_exp += -(o_ptr->pval) * 10;
			if(have_flag(flgs,TR_SEARCH)) need_exp += -(o_ptr->pval) * 10;

			if (have_flag(flgs, TR_DISARM)) need_exp += -(o_ptr->pval) * 10;
			if (have_flag(flgs, TR_SAVING)) need_exp += -(o_ptr->pval) * 10;

		}

		//マイナス修正値 1ごとに2000
		if(o_ptr->ac + o_ptr->to_a < 0) need_exp += -(o_ptr->to_a) * 2;
		if(o_ptr->to_d < 0) need_exp += -(o_ptr->to_d) * 2;
		if(o_ptr->to_h < 0 && o_ptr->tval != TV_ARMOR && o_ptr->tval != TV_DRAG_ARMOR) need_exp += -(o_ptr->to_h) * 2; //鎧の命中マイナスは計算しない

		if(!need_exp) continue;

		if(object_is_artifact(o_ptr))
		{
			if(o_ptr->name1 == ART_POWER) need_exp = 10000;//一つの指輪は1000万固定
			else if(o_ptr->name1 == ART_TWILIGHT) need_exp = 5000;//ほか一部アーティファクトに大幅ペナルティ
			else if(o_ptr->name1 == ART_THANOS) need_exp = 4000;
			else if(o_ptr->name1 == ART_ANUBIS) need_exp = 3000;
			else if(o_ptr->name1 == ART_HAKABUSA) need_exp = 3000;
			else if(o_ptr->name1 == ART_STORMBRINGER) need_exp = 3000;
			else if(o_ptr->name1 == ART_BERUTHIEL) need_exp = 3000;
			else need_exp += 500; //ほかアーティファクト+50万
		}

		if(need_exp > 10000) need_exp = 10000;//1000万を超えない

		//xtra5に経験値を加算　16bitなのでオーバーフロー対策しとく
		tmp_xtra5 = o_ptr->xtra5;
		tmp_xtra5 += gain_exp / 1000;
		if(randint1(1000) < (gain_exp % 1000)) tmp_xtra5++;
		if(tmp_xtra5 > 32000) tmp_xtra5 = 32000;
		o_ptr->xtra5 = tmp_xtra5;

		if(p_ptr->wizard) msg_format("INVEN:%d %d/%d",i,tmp_xtra5,need_exp);

		if(o_ptr->xtra5 > need_exp)
		{
			bool flag_bonus = FALSE;
			object_desc(o_name,o_ptr,OD_OMIT_INSCRIPTION);
			msg_format("%sの厄が全て落ちた！",o_name);

			o_ptr->xtra3 = SPECIAL_ESSENCE_OTHER; //鍛冶アイテム扱いにする
			if(o_ptr->curse_flags & TRC_PERMA_CURSE) flag_bonus = TRUE;
			o_ptr->curse_flags = 0L; //永遠の呪いも消える
			//パラメータ反転
			if(o_ptr->to_a < 0) o_ptr->to_a = -(o_ptr->to_a);
			if(o_ptr->to_d < 0) o_ptr->to_d = -(o_ptr->to_d);
			if(o_ptr->to_h < 0) o_ptr->to_h = -(o_ptr->to_h);
			if(o_ptr->pval < 0) o_ptr->pval = -(o_ptr->pval);


			add_flag(o_ptr->art_flags, TR_IGNORE_FIRE);
			add_flag(o_ptr->art_flags, TR_IGNORE_ACID);
			add_flag(o_ptr->art_flags, TR_IGNORE_ELEC);
			add_flag(o_ptr->art_flags, TR_IGNORE_COLD);
			//永遠の呪いを解いたときボーナス
			if(flag_bonus)
			{
				if(object_is_melee_weapon(o_ptr))
				{
					gain_random_slay_2(o_ptr);
					o_ptr->dd += 1;
				}
				else
				{
					one_high_resistance(o_ptr);
					one_ability(o_ptr);

				}
			}

			p_ptr->update |= PU_BONUS;
			p_ptr->window |= (PW_INVEN | PW_EQUIP);

		}
	}

}


//特定の★を所持/装備しているかどうかをチェックする。三月精に対応するためスロットを限定せずにおく
//v1.1.59 装備していなくても所持していればOKにし、装備限定にするためのフラグをつけた
bool check_equip_specific_fixed_art(int a_idx, bool only_equip)
{
	object_type *o_ptr;
	int i;

	if (only_equip) i = INVEN_RARM;
	else i = 0;

	for(;i<INVEN_TOTAL;i++)
	{
		o_ptr = &inventory[i];
		if(!o_ptr->k_idx) continue;
		if(check_invalidate_inventory(i)) continue;
		if(o_ptr->name1 == a_idx) return TRUE;
	}

	return FALSE;
}

//モンスターをランクアップさせる。薬師の栄養剤、純狐の純化、ブリーダーの特訓？
//rankの数だけランクアップする。0なら最大まで。
//v1.1.15 ランクアップしたらTRUEを返すようにした
bool rankup_monster(int m_idx, int rank)
{
	monster_type *m_ptr;
	monster_race *r_ptr;
	int i;
	int perc;
	char m_name_old[80],m_name_new[80];

	if (m_idx <= 0) return FALSE;
	m_ptr = &m_list[m_idx];
	r_ptr = &r_info[m_ptr->r_idx];
	monster_desc(m_name_old, m_ptr, 0);
	perc = m_ptr->hp * 100 / m_ptr->maxhp;

	if (m_ptr->mflag2 & MFLAG2_CHAMELEON) return FALSE;
	if (r_ptr->flags7 & RF7_TANUKI) return FALSE;

	//ランクアップ処理　monster_gain_exp()からコピー
	for(i=0;(rank<=0||i<rank) && r_ptr->next_exp;i++)
	{
		byte old_sub_align = m_ptr->sub_align;

		real_r_ptr(m_ptr)->cur_num--;
		m_ptr->r_idx = r_ptr->next_r_idx;
		real_r_ptr(m_ptr)->cur_num++;
		m_ptr->ap_r_idx = m_ptr->r_idx;
		r_ptr = &r_info[m_ptr->r_idx];
		if (r_ptr->flags1 & RF1_FORCE_MAXHP)
			m_ptr->max_maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
		else
			m_ptr->max_maxhp = damroll(r_ptr->hdice, r_ptr->hside);
		if (ironman_nightmare)
		{
			u32b hp = m_ptr->max_maxhp * 2L;
			m_ptr->max_maxhp = (s16b)MIN(30000, hp);
		}
		else if(difficulty == DIFFICULTY_EASY) m_ptr->max_maxhp = MAX(1,m_ptr->max_maxhp/2);
		else if(difficulty == DIFFICULTY_NORMAL) m_ptr->max_maxhp = MAX(1,m_ptr->max_maxhp/2 + m_ptr->max_maxhp/4);
		m_ptr->maxhp = m_ptr->max_maxhp;
		//v1.1.64 ランクアップ時にちょっとHP回復
		if (m_ptr->hp < m_ptr->maxhp)
			m_ptr->hp += (m_ptr->maxhp - m_ptr->hp) / (2 + randint0(4));

		m_ptr->mspeed = get_mspeed(r_ptr);
		if (!is_pet(m_ptr) && !(r_ptr->flags3 & (RF3_ANG_CHAOS | RF3_ANG_COSMOS)))
			m_ptr->sub_align = old_sub_align;
		else
		{
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
			if (r_ptr->flags3 & RF3_ANG_CHAOS) m_ptr->sub_align |= SUB_ALIGN_EVIL;
			if (r_ptr->flags3 & RF3_ANG_COSMOS) m_ptr->sub_align |= SUB_ALIGN_GOOD;
		}

		m_ptr->exp = 0;
	}

	if(!i) return FALSE; //ランクアップしなかった場合終了

	m_ptr->hp = m_ptr->maxhp * perc / 100;//HP調整
	(void)set_monster_csleep(m_idx, 0);//起こす
	msg_format("%sはパワーアップした！", m_name_old);
	update_mon(m_idx, FALSE);
	lite_spot(m_ptr->fy, m_ptr->fx);
	if (m_idx == p_ptr->riding) p_ptr->update |= PU_BONUS;

	return TRUE;

}






//薬師が調剤秘伝書を読むコマンドの実体部 do_cmd_browse()とcheck_drug_recipe()から呼ばれる
void check_drug_recipe_aux(object_type *o_ptr)
{
	int i,j;
	object_type forge;
	object_type *q_ptr = &forge;
	char temp[70 * 20];
	char *t;
	int shift = o_ptr->sval * 8;

	if(o_ptr->tval != TV_BOOK_MEDICINE || o_ptr->sval > 3)
	{
		msg_print("ERROR:check_drug_recipe()に渡されたアイテムがおかしい");
		return;
	}

	screen_save();

	//建物内と外で画面クリアの範囲を変える
	if(character_icky > 1)	clear_bldg(4,22);
	else		for(i=4;i<22;i++) Term_erase(17, i, 255);

	while(1)
	{
		char c;
		char o_name[MAX_NLEN];
		int idx;

		if(character_icky > 1)	clear_bldg(4,22);
		else		for(i=4;i<22;i++) Term_erase(17, i, 255);

		prt("どの薬について確認しますか？」(ESC:キャンセル)",4,20);
		for(i=0;i<8;i++)
		{
			char sym = 'a'+i;
			object_prep(q_ptr, lookup_kind(drug_recipe_table[shift+i].tval,drug_recipe_table[shift+i].sval ));
			object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,format("%c)%s", sym,o_name),7+i,20);
		}

		c = inkey();
		if(c == ESCAPE) break;
		if(c < 'a' || c >= 'a'+8) continue;

		if(character_icky > 1)	clear_bldg(4,22);
		else		for(i=4;i<22;i++) Term_erase(17, i, 255);

		idx = shift+(c-'a');
		//完成ベースアイテムとそのk_info説明文を表示
		object_prep(q_ptr, lookup_kind(drug_recipe_table[idx].tval,drug_recipe_table[idx].sval ));
		object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
		roff_to_buf((k_text + k_info[q_ptr->k_idx].text),77 - 15, temp, sizeof(temp));

		c_put_str(TERM_WHITE,o_name,5,20);
		t = temp;
		for (i = 0; i < 4; i++)
		{
			char buf[160];
			if (t[0] == 0) break;
			else
			{
				strcpy(buf, t);
				c_put_str(TERM_WHITE,buf,7+i,20);
				t += strlen(t) + 1;
			}
		}

/* なんで↓はバグるのかわからない
		for (j = 0; temp[j]; j += 1 + strlen(&temp[j]))
		{
			msg_format("%s",temp[j]);
			//c_put_str(TERM_WHITE,&temp[j],7+j,20);
		}
*/
		c_put_str(TERM_WHITE,"材料例:",12,20);
		if(drug_recipe_table[idx].mat1_tv)
		{
			object_prep(q_ptr, lookup_kind(drug_recipe_table[idx].mat1_tv,drug_recipe_table[idx].mat1_sv ));
			object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,o_name,13,20);
		}
		if(drug_recipe_table[idx].mat2_tv)
		{
			object_prep(q_ptr, lookup_kind(drug_recipe_table[idx].mat2_tv,drug_recipe_table[idx].mat2_sv ));
			object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,o_name,14,20);
		}
		if(drug_recipe_table[idx].mat3_tv)
		{
			object_prep(q_ptr, lookup_kind(drug_recipe_table[idx].mat3_tv,drug_recipe_table[idx].mat3_sv ));
			object_desc(o_name, q_ptr, (OD_NO_FLAVOR | OD_STORE | OD_NO_PLURAL | OD_NAME_ONLY));
			c_put_str(TERM_WHITE,o_name,15,20);
		}



		prt("何かキーを押してください", 0, 0);
		(void)inkey();
		prt("", 0, 0);


	}
	screen_load();
}


//薬師レシピ確認　browseコマンドを経由しない時
void check_drug_recipe(void)
{
	int		item;

	object_type	*o_ptr;

	cptr q, s;

	if (!CHECK_MAKEDRUG_CLASS)
	{
		msg_print("あなたは調剤レシピを読めない。");
		return;
	}
	if (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & (MIMIC_NO_CAST | MIMIC_GIGANTIC))
	{
		msg_print("今は本を読めない。");
		return ;
	}
	item_tester_tval = TV_BOOK_MEDICINE;

	q = "どの調剤レシピを確認しますか? ";
	s = "調剤レシピがない。";

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR)))
	{
		return;
	}
		
	if (item >= 0)	o_ptr = &inventory[item];
	else			o_ptr = &o_list[0 - item];

	check_drug_recipe_aux(o_ptr);

}





/*:::金属製の装備品のときTRUE 小傘の武器強化*/
bool object_is_metal(object_type *o_ptr)
{
	int sv = o_ptr->sval;

	//未鑑定品はfalseにする？

	switch(o_ptr->tval)
	{
	//矢は「鋼鉄の」のみOK
	case TV_ARROW:
	case TV_BOLT:
		if(sv == SV_AMMO_NORMAL) return TRUE;
		break;
	//針追加
	case TV_OTHERWEAPON:
		if(sv == SV_OTHERWEAPON_NEEDLE) return TRUE;
		break;
	//折れた針
	case TV_MATERIAL:
		if(sv == SV_MATERIAL_BROKEN_NEEDLE) return TRUE;
		break;

	case TV_KNIFE:
		if(sv != SV_WEAPON_DRAGON_DAGGER && sv != SV_WEAPON_DOKUBARI) return TRUE;
		break;
	case TV_SWORD:
	case TV_KATANA:
	case TV_AXE:
	case TV_SPEAR:
	case TV_POLEARM:
		return TRUE;
	case TV_HAMMER:
		if(sv != SV_WEAPON_KINE) return TRUE;
		break;
	case TV_STICK:
		if(sv == SV_WEAPON_SYAKUJYOU || sv == SV_WEAPON_IRONSTICK || sv == SV_WEAPON_GROND) return TRUE;
		break;
	case TV_SHIELD:
		if(sv == SV_SMALL_METAL_SHIELD || sv == SV_LARGE_METAL_SHIELD || sv == SV_KNIGHT_SHIELD || sv == SV_MIRROR_SHIELD) return TRUE;
		break;
	case TV_ARMOR:
		if(sv == SV_ARMOR_CHAIN || sv == SV_ARMOR_BRIGANDINE || sv == SV_ARMOR_PARTIAL_PLATE || sv == SV_ARMOR_OOYOROI
		|| sv == SV_ARMOR_FULL_PLATE || sv == SV_ARMOR_MITHRIL_CHAIN || sv == SV_ARMOR_MITHRIL_PLATE) return TRUE;
		break;
	case TV_HEAD:
		if(sv == SV_HEAD_METALCAP || sv == SV_HEAD_STARHELM || sv == SV_HEAD_STEELHELM || sv == SV_HEAD_GOLDCROWN
		|| sv == SV_HEAD_JEWELCROWN || sv == SV_HEAD_IRONCROWN) return TRUE;
		break;
	case TV_GLOVES:
		if(sv == SV_HAND_ARMGUARDS || sv == SV_HAND_GAUNTLETS) return TRUE;
		break;
	case TV_BOOTS:
		if(sv == SV_LEG_METALSHODBOOTS || sv == SV_LEG_SABATON) return TRUE;
		break;

	}
	return FALSE;

}


//小傘鍛冶能力
bool kogasa_smith_aux(object_type *o_ptr)
{
	int new_val = 5 + p_ptr->lev / 5;

	if(o_ptr->tval == TV_MATERIAL && o_ptr->sval == SV_MATERIAL_BROKEN_NEEDLE)
	{
		int new_num = damroll((2*o_ptr->number),6) + 8 * o_ptr->number;
		if(new_num > 99) new_num = 99;

		object_wipe(o_ptr);
		object_prep(o_ptr, lookup_kind(TV_OTHERWEAPON, SV_OTHERWEAPON_NEEDLE));	
		o_ptr->number = new_num;
		o_ptr->name2 = EGO_ARROW_HOLY;
		o_ptr->to_h = new_val;
		o_ptr->to_d = new_val;
		identify_item(o_ptr);
	}
	else if(object_is_weapon_ammo(o_ptr))
	{
		if(o_ptr->to_h < new_val) o_ptr->to_h = new_val;
		if(o_ptr->to_d < new_val) o_ptr->to_d = new_val;
	}
	else if(object_is_armour(o_ptr))
	{
		if(o_ptr->to_a < new_val) o_ptr->to_a = new_val;
	}
	else
	{
		msg_print("ERROR:kogasa_smith_aux()に想定しないアイテムが渡された");
		return FALSE;
	}

	if (object_is_cursed(o_ptr) && !(o_ptr->curse_flags & TRC_PERMA_CURSE) && !(o_ptr->curse_flags & TRC_HEAVY_CURSE) )
	{
		o_ptr->curse_flags = 0L;
		o_ptr->ident |= (IDENT_SENSE);
		o_ptr->feeling = FEEL_NONE;
	}
	return TRUE;
}



//アーティファクトを消費し光の剣属性の大型ビームかボールを放つ。行動順消費しないときFALSE
bool final_strike(int item_idx, bool remain_broken_weapon)
{
	object_type *o_ptr;
	int item, dam, dir;
	char o_name[MAX_NLEN];

	cptr q = "アイテムを選択してください: ";
	cptr s = "適したアイテムがありません。";

	//Paranoia
	if (item_idx < 0) return FALSE;

	if(item_idx) 
		item = item_idx;
	else
	{
		item_tester_hook = object_is_artifact;
		if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return FALSE;
		if( !get_check_strict("よろしいですか? ", CHECK_NO_HISTORY)) return FALSE;	
	}

	if (item >= 0)	o_ptr = &inventory[item];
	else		o_ptr = &o_list[0 - item];

	object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	dam =  object_value_real(o_ptr) / (15+randint1(15));

	if(dam > 9999) dam = 9999;

	if(dam > 0)
	{
		msg_format("%sは眩い光を発した！",o_name);
		if(object_is_melee_weapon(o_ptr))
		{
			(void)get_hack_dir(&dir);	
			fire_spark(GF_PSY_SPEAR,dir,dam,1);
		}
		else
		{
			project(0,6,py,px,dam*2,GF_PSY_SPEAR,(PROJECT_JUMP|PROJECT_KILL|PROJECT_GRID|PROJECT_ITEM),-1);
		}
	}
	else
	{
		msg_format("%sは粉々に砕けて消えた！",o_name);
	}	

	//武器(☆のみ)を折る
	if(object_is_melee_weapon(o_ptr) && remain_broken_weapon && object_is_random_artifact(o_ptr))
	{
		int new_tv = o_ptr->tval;
		int new_sv = o_ptr->sval;

		if(o_ptr->tval == TV_SWORD) new_sv = SV_WEAPON_BROKEN_SWORD;
		else if(o_ptr->tval == TV_KNIFE) new_sv = SV_WEAPON_BROKEN_DAGGER;
		else if(o_ptr->tval == TV_KATANA) new_sv = SV_WEAPON_BROKEN_KATANA;
		else
		{
			new_tv = TV_STICK;
			new_sv = SV_WEAPON_BROKEN_STICK;
		}

		//武器のパラメータ変更など
		o_ptr->k_idx = lookup_kind(new_tv,new_sv);
		//v1.1.12 TvalとSvalが更新されてなかったので追加
		o_ptr->tval = new_tv;
		o_ptr->sval = new_sv;

		o_ptr->dd = randint1(o_ptr->dd / 2 + 1);
		o_ptr->ds = randint0(o_ptr->ds) +1 ;
		if(o_ptr->ds < k_info[o_ptr->k_idx].ds) o_ptr->ds = k_info[o_ptr->k_idx].ds;
		o_ptr->to_a /= 3;
		o_ptr->to_d /= 2;
		o_ptr->to_h /= 2;
		o_ptr->weight = k_info[o_ptr->k_idx].weight;
		if(o_ptr->pval > 1 && !one_in_(4)) o_ptr->pval -= 1;

		//発動インデックスをクリア もしいずれ★を追加するなら発動インデックスの扱いが違うので特殊処理が必要だろう
		o_ptr->xtra2 = 0; 
		remove_flag(o_ptr->art_flags,TR_ACTIVATE);
	}
	else if (item >= 0)
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

	if (item >= INVEN_RARM)
	{
		kamaenaoshi(item);
		calc_android_exp();
	}
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
	p_ptr->update |= (PU_BONUS);
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	return TRUE;

}


/*:::装備中の銃の射撃発動成功率を計算する*/
int	calc_gun_fire_chance(int slot)
{
	int chance,lev;
	object_type *o_ptr;

	if(slot != INVEN_RARM && slot != INVEN_LARM)
	{ msg_print("ERROR:calc_gun_fire_cance()が手に持ってない物に対して呼ばれた");return 0;}
	o_ptr = &inventory[slot];
	if(o_ptr->tval != TV_GUN)
	{ msg_print("ERROR:calc_gun_fire_cance()が銃以外に対して呼ばれた");return 0;}

	if(p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->realm1 == TV_BOOK_ENCHANT && p_ptr->lev > 39) return 100;

	if (object_is_fixed_artifact(o_ptr))
	{	//最大100
		lev = a_info[o_ptr->name1].level;
	}
	else if (object_is_random_artifact(o_ptr))
	{	//最大127　攻撃力から大まかにレベル計算
		int need_turn = gun_base_param_table[o_ptr->sval].charge_turn;
		lev = (o_ptr->dd * o_ptr->ds + o_ptr->to_h + o_ptr->to_d)/ need_turn;
		switch(o_ptr->xtra1)
		{
			case GF_FIRE: case GF_ELEC: case GF_ACID:case GF_COLD:case GF_POIS:
			case GF_PLASMA:
				lev /= 2;break;
			case GF_NETHER: case GF_POLLUTE: case GF_NUKE: case GF_CHAOS:
				lev = lev * 2/ 3;break;
		}
		if(o_ptr->xtra4 == GUN_FIRE_MODE_BEAM || o_ptr->xtra4 == GUN_FIRE_MODE_SPARK ||
			o_ptr->xtra4 == GUN_FIRE_MODE_BREATH ||o_ptr->xtra4 == GUN_FIRE_MODE_ROCKET)
			lev += 10;
		if(lev < k_info[o_ptr->k_idx].level) lev = k_info[o_ptr->k_idx].level;
		if(lev > 80) lev = 80 + (lev - 80) / 2;
	}
	else if(object_is_ego(o_ptr))
	{	//最大で基本レベル+25 ロケランで90
		lev = k_info[o_ptr->k_idx].level;
		if(lev > 50) lev = 50 + (lev-50)/2;
		lev += e_info[o_ptr->name2].level;
	}
	else
	{	//基本レベル
		lev = k_info[o_ptr->k_idx].level;
		if(lev > 50) lev = 50 + (lev-50)/2;
	}

	if(lev > 127) lev = 127;
	if(lev < 5) lev = 5;

	//魔道具か射撃の技能の高い方を使う。そこにp_ptr->to_h_bも加算する。　
	//to_h_bは(祝福+10,ヒーロー+12,器用さ40で+16,さらに防具や装飾品の命中修正など)*3
	//Lev1で50-100程度、Lev50で200-300,魔道具使い400,トリガーハッピー射手450くらい?
	chance = MAX(p_ptr->skill_dev,p_ptr->skill_thb) + p_ptr->to_h_b * BTH_PLUS_ADJ;

	chance += 50 + p_ptr->lev * 2;
	//技能値補正 0-200
	chance += ref_skill_exp(TV_GUN) / 32;
	//命中修正値
	chance += o_ptr->to_h * BTH_PLUS_ADJ;

	//腕防具ペナルティ
	if(p_ptr->cumber_glove)
	{
		chance -= chance * (100 - p_ptr->stat_ind[A_DEX] * 2) / 100;
	}
	//二丁拳銃ペナルティ 三妖精には関係ない
	if(inventory[INVEN_RARM].tval == TV_GUN && inventory[INVEN_LARM].tval == TV_GUN && p_ptr->pclass != CLASS_3_FAIRIES)
	{
		u32b flgs[TR_FLAG_SIZE];
		int i;
		bool flag_genzi = FALSE;
		//源氏装備フラグチェック
		object_flags(&inventory[INVEN_HANDS], flgs);
		if (have_flag(flgs, TR_GENZI))
		{
			flag_genzi = TRUE;
		}
		//ヤクザ天狗の銃の二丁拳銃ボーナス
		//v1.1.21 兵士「ウェポンマスタリー」を習得したときもここに来るようにした
		if((inventory[INVEN_RARM].name1 == ART_YAKUZAGUN_1 && inventory[INVEN_LARM].name1 == ART_YAKUZAGUN_2)
			|| (inventory[INVEN_RARM].name1 == ART_YAKUZAGUN_2 && inventory[INVEN_LARM].name1 == ART_YAKUZAGUN_1)
			|| HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_WEAPON_MASTERY)			)
		{
			if(!flag_genzi) chance = chance * 3 / 4;
		}
		else
		{
			if(flag_genzi) chance = chance * 3 / 4;
			else chance /= 2;
		}

	}
	//銃を持たない手に武器や盾や妖魔本などを装備している時のペナルティ　三妖精には関係ない
	else if((slot == INVEN_RARM && inventory[INVEN_LARM].k_idx || slot == INVEN_LARM && inventory[INVEN_RARM].k_idx) && p_ptr->pclass != CLASS_3_FAIRIES)
	{
		chance = chance * 4 / 5;
	}
	//やる人いないと思うが騎乗中ペナルティもつけとこう
	if(p_ptr->riding )
	{
		chance = chance * (ref_skill_exp(SKILL_RIDING)+2000) / 10000;
	}

	if (p_ptr->confused) chance /= 2;
	else if (p_ptr->shero && p_ptr->pseikaku != SEIKAKU_BERSERK) chance /= 2;
	if(p_ptr->afraid) chance = chance * 2 / 3;
	if(p_ptr->stun) chance = chance * 2 / 3;

	//補正式
	chance = chance * 5 / lev + chance / 5 - lev * 2 / 3;


	if(p_ptr->pclass == CLASS_SOLDIER && p_ptr->concent)
	{
			chance += p_ptr->concent;
	}

//	msg_format("slot:%d lev:%d chance:%d",slot,lev,chance);

	if(p_ptr->special_attack & ATTACK_HEAD_SHOT)	chance = chance * 3 / 4;

	if(special_shooting_mode == SSM_SEIRAN4) chance = chance * 4 / 5;
	//if(special_shooting_mode == SSM_HEAD_SHOT) chance = chance * 4 / 5;
	if (special_shooting_mode == SSM_RAPID_FIRE)
	{
		//v1.1.44 うどんげルナティックダブルはむしろ命中率が上がることにする。
		if (is_special_seikaku(SEIKAKU_SPECIAL_UDONGE))
		{
			chance += chance / 5;
		}
		else
		{
			chance = chance * 7 / 10;
		}
	}


	if(chance > 100) chance = 100;
	if(chance < USE_DEVICE) chance = USE_DEVICE;

	return chance;

}

///mod160508 銃の充填時間にかかる倍率(%)を計算する
int calc_gun_charge_mult(int slot)
{
	int charge_bonus = 0;
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	int mult;

//v1.1.21 兵士「メンテナンス」技能により両手に持たない銃にも充填適用する
//	if(slot != INVEN_RARM && slot != INVEN_LARM)
//	{ msg_print("ERROR:calc_gun_charge_bonus()が手に持ってない物に対して呼ばれた");return 0;}
	o_ptr = &inventory[slot];
	if(o_ptr->tval != TV_GUN)
	{ msg_print("ERROR:calc_gun_charge_bonus()が銃以外に対して呼ばれた");return 1;}


	//充填短縮ボーナス計算
	charge_bonus = ref_skill_exp(TV_GUN) / 2000;//熟練度ボーナス最大+4
	if(p_ptr->pseikaku == SEIKAKU_TRIGGER) charge_bonus++; //トリガーハッピー
	object_flags(o_ptr, flgs);
	if(have_flag(flgs, TR_XTRA_SHOTS)) charge_bonus++; //銃の高速射撃

	//三月精の追加射撃の指輪の処理　装備箇所に関わらずカウントされるが銃1つ当たり+1まで
	if(p_ptr->pclass == CLASS_3_FAIRIES)
	{
		int i,cnt=0;

		for(i=INVEN_RIBBON;i<INVEN_TOTAL;i++)
		{
			if(inventory[i].tval == TV_RING && inventory[i].sval == SV_RING_SHOTS) cnt++;
		}
		if(inventory[INVEN_RARM].tval == TV_GUN && inventory[INVEN_LARM].tval == TV_GUN)
		{
			if(slot == INVEN_RARM && cnt > 0) charge_bonus++;
			if(slot == INVEN_LARM && cnt > 1) charge_bonus++;
		}
		else if(cnt > 0) charge_bonus++;

	}
	//通常の追加射撃の指輪の処理
	else
	{
		//右手の銃のとき、左指の指輪は左手に何も持っていない時のみ有効
		if(slot == INVEN_RARM)
		{
			if(inventory[INVEN_RIGHT].sval == SV_RING_SHOTS) charge_bonus++;
			if(inventory[INVEN_LEFT].sval == SV_RING_SHOTS && !inventory[INVEN_LARM].k_idx) charge_bonus++;
		}
		else if(slot == INVEN_LARM)
		{
			if(inventory[INVEN_LEFT].sval == SV_RING_SHOTS) charge_bonus++;
			if(inventory[INVEN_RIGHT].sval == SV_RING_SHOTS && !inventory[INVEN_RARM].k_idx) charge_bonus++;
		}
	}

	//速射ボーナス値1ごとに20%充填短縮  100→80→64→51..
	mult = 100;
	while(charge_bonus)
	{
		mult = mult * 80 / 100;
		charge_bonus--;
	}
	//ワンマンアーミーはチャージ2倍
	if(HAVE_SOLDIER_SKILL(SOLDIER_SKILL_SHOOT,SS_S_ONE_MAN_ARMY)) 
		mult /= 2;

	return mult;

}

//銃の装弾数　基本値はgun_base_param_table[]で★はハードコーディングで上書き
int calc_gun_load_num(object_type *o_ptr)
{
	if(o_ptr->tval != TV_GUN)
	{
		msg_format("ERROR:calc_gun_load_num()に銃以外のアイテムが渡された");
		return 1;
	}

	switch(o_ptr->name1)
	{
	case ART_3DPRINTER_GUN:
		return 1;
	case ART_YAKUZAGUN_1:
	case ART_YAKUZAGUN_2:
		return 4;
	case ART_BFG9000:
		return 1;
	default:
		return gun_base_param_table[o_ptr->sval].bullets;
	}

}

//特定のアイテムを持っているかどうか確認する。本当に今更だが。
//v1.1.69 持っているときインベントリidxを、持っていないとき-1を返すことにした
int	check_have_specific_item(int tval, int sval)
{
	int i;
	object_type *o_ptr;

	for(i=0;i<INVEN_PACK;i++)
	{
		o_ptr = &inventory[i];
		if(!o_ptr->k_idx) continue;
		if(o_ptr->tval == tval && o_ptr->sval == sval) return i;
	}

	return -1;

}


//v1.1.15
/*::: Mega Hack - ヘカーティアの体変更コマンド*/		
//magic_num2[0]:今の体の番号(異界0,地球1,月2)を格納
//magic_num2[1]:追加インベントリ前半(inven_add[0-11])に入っているのはどの体の装備品なのか格納
//magic_num2[4-75]:突然変異を保持

//magic_num1[0-2]それぞれの体のHP残量(%*100)を記録
//magic_num1[3-5]それぞれの体のMP残量(%*100)を記録
//magic_num1[6-8]それぞれの体の性格値を記録
//magic_num1[9-11]それぞれの体の最大MPを記録

/*:::ヘカーティア専用 どの身体と交代するか選択する*/
//HPとMP残量を比率で表示
static int select_hecatia_body(void)
{

	int n,choose=0;
	char c;
	bool selected = FALSE;
	int i;
	cptr namelist[3] = {"異界","地球","月"};

	screen_save();	


	while(1)
	{
		for(i=9;i<16;i++) Term_erase(17, i, 255);
		put_str("どの体と交代しますか？", 10, 30);

		for(n=0;n<3;n++)
		{
			//今の体は選択できない
			if(n == p_ptr->magic_num2[0]) continue;
			else put_str( format("%c)%4sの体(HP:%.1f%% MP:%.1f%%)",'a'+n,namelist[n],((float)p_ptr->magic_num1[0+n]/100.0),((float)p_ptr->magic_num1[3+n]/100.0)), 12 + n, 30);
		}
		c = inkey();

		if (c == 'a')
		{
			if(p_ptr->magic_num2[0] == HECATE_BODY_OTHER) continue;
			choose = HECATE_BODY_OTHER;
			break;
		}
		else if (c == 'b')
		{
			if(p_ptr->magic_num2[0] == HECATE_BODY_EARTH) continue;
			choose = HECATE_BODY_EARTH;
			break;
		}
		else if (c == 'c')
		{
			if(p_ptr->magic_num2[0] == HECATE_BODY_MOON) continue;
			choose = HECATE_BODY_MOON;
			break;
		}
		else if(c == ESCAPE)
		{
			choose = p_ptr->magic_num2[0];				
			break;
		}
	}

	screen_load();
	return choose;
}

//ヘカーティアの体交代コマンド　行動順消費した時TRUE
bool  hecatia_change_body(void)
{
	int old_body, new_body, i;
	int shift;
	int hp_perc, sp_perc;
	if(p_ptr->pclass != CLASS_HECATIA)
	{
		msg_print("ERROR:hecatia_change_body()がヘカーティア以外で呼ばれた");
		return FALSE;
	}

	old_body = p_ptr->magic_num2[0];
	new_body = select_hecatia_body();
	if( old_body == new_body) return FALSE;


	if(new_body == HECATE_BODY_OTHER)
		msg_format("あなたは異界の体と交代した。");
	else if(new_body == HECATE_BODY_EARTH)
		msg_format("あなたは地球の体と交代した。");
	else
		msg_format("あなたは月の体と交代した。");

	//MP消費　特技終了時新しい体でも消費する 消費魔力減少などがこちらには効かないがまあいいか
	p_ptr->csp -= 30;
	if(p_ptr->csp < 0) p_ptr->csp = 0; //paranoia


	//新しい身体の装備品が追加インベントリの前半後半どちら側に入っているか読みだして配列添え字シフト量を得る
	if(p_ptr->magic_num2[1] == new_body)
	{
		shift=0;
		p_ptr->magic_num2[1] = old_body;
	}
	else 
		shift=12;

	//追加インベントリに保管された装備品と今の装備品を入れ替える
	for(i=INVEN_RARM;i<=INVEN_FEET;i++)
	{
		object_type forge;
		object_type *o_ptr_swap = &forge;
		object_type *o_ptr_old;
		object_type *o_ptr_new;

		o_ptr_old = &inventory[i];
		o_ptr_new = &inven_add[i-INVEN_RARM+shift];

		object_copy(o_ptr_swap,o_ptr_old);
		object_copy(o_ptr_old,o_ptr_new);
		object_copy(o_ptr_new,o_ptr_swap);
	}

	//突然変異も保持 magic_num2[4-75]
	for(i=0;i<4;i++)
	{
		p_ptr->magic_num2[4+i      + 24 * (old_body)] = p_ptr->muta1>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 4  + 24 * (old_body)] = p_ptr->muta2>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 8  + 24 * (old_body)] = p_ptr->muta3>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 12 + 24 * (old_body)] = p_ptr->muta1_perma>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 16 + 24 * (old_body)] = p_ptr->muta2_perma>>(i*8) & (0xFF);
		p_ptr->magic_num2[4+i + 20 + 24 * (old_body)] = p_ptr->muta3_perma>>(i*8) & (0xFF);
	}
	p_ptr->muta1 = 0L;
	p_ptr->muta2 = 0L;
	p_ptr->muta3 = 0L;
	p_ptr->muta1_perma = 0L;
	p_ptr->muta2_perma = 0L;
	p_ptr->muta3_perma = 0L;
	for(i=0;i<4;i++)
	{
		p_ptr->muta1       |= (u32b)(p_ptr->magic_num2[4+i      + 24 * (new_body)]) <<(i*8);
		p_ptr->muta2       |= (u32b)(p_ptr->magic_num2[4+i +  4 + 24 * (new_body)]) <<(i*8);
		p_ptr->muta3       |= (u32b)(p_ptr->magic_num2[4+i +  8 + 24 * (new_body)]) <<(i*8);
		p_ptr->muta1_perma |= (u32b)(p_ptr->magic_num2[4+i + 12 + 24 * (new_body)]) <<(i*8);
		p_ptr->muta2_perma |= (u32b)(p_ptr->magic_num2[4+i + 16 + 24 * (new_body)]) <<(i*8);
		p_ptr->muta3_perma |= (u32b)(p_ptr->magic_num2[4+i + 20 + 24 * (new_body)]) <<(i*8);
	}

	//パラメータ回復
	reset_tim_flags();
	for(i=0;i<6;i++) p_ptr->stat_cur[i] = p_ptr->stat_max[i];
	p_ptr->exp = p_ptr->max_exp;
	check_experience();

	//満腹度減ってれば回復
	if(p_ptr->food < 5000) p_ptr->food = 5000;

	//元の体の最大MPを記録
	p_ptr->magic_num1[9+old_body] = p_ptr->msp;

	//今のHPとMPの割合(0.01%単位)を計算し保持、さらに新しい身体の割合値を持ってくる
	hp_perc = p_ptr->chp * 10000 / p_ptr->mhp;
	sp_perc = p_ptr->csp * 10000 / p_ptr->msp;
	p_ptr->magic_num1[0+old_body] = hp_perc;
	p_ptr->magic_num1[3+old_body] = sp_perc;
	hp_perc = p_ptr->magic_num1[0+new_body];
	sp_perc = p_ptr->magic_num1[3+new_body];



	//性格変更処理
	p_ptr->pseikaku = p_ptr->magic_num1[6+new_body];
	ap_ptr = &seikaku_info[p_ptr->pseikaku];
	//「今動かしてる体」値を変更
	p_ptr->magic_num2[0] = new_body;

	p_ptr->update |= PU_BONUS | PU_HP | PU_MANA | PU_SPELLS;
	update_stuff();

	//新たな体のHPとMPを計算する
	p_ptr->chp = p_ptr->mhp * hp_perc / 10000;
	p_ptr->csp = p_ptr->msp * sp_perc / 10000;
	p_ptr->chp_frac = 0;
	p_ptr->csp_frac = 0;


	//再描画
	do_cmd_redraw();

	return TRUE;
}

//v1.1.15
/*:::ヘカーティアの地球と月の体の性格(p_ptr->magic_num1[6,7,8])を設定する*/
void get_hecatia_seikaku(void)
{
	int i,j;

	//普通の性格リスト
	byte seikaku_list[HECATE_SEIKAKU_CHOOSE] = 
		{SEIKAKU_FUTUU,SEIKAKU_CHIKARA,SEIKAKU_KIREMONO,SEIKAKU_INOCHI,SEIKAKU_SUBASI,SEIKAKU_SHIAWASE,
		SEIKAKU_TRIGGER,SEIKAKU_ELEGANT,SEIKAKU_GAMAN,SEIKAKU_LUCKY};
	byte flag_need_choose = FALSE;

	p_ptr->magic_num1[6] = p_ptr->pseikaku;//異界の体の性格値格納部

	for(i=0;i<HECATE_SEIKAKU_CHOOSE;i++)
	{
		if(p_ptr->pseikaku == seikaku_list[i]) flag_need_choose = TRUE;
	}

	if(flag_need_choose)
	{
		char c;

		screen_save();	
		//異界の性格が普通のものだと他の体の性格も選ぶ
		for(j=0;j<2;j++)
		{
			
			while(1)
			{
				for(i=9;i<HECATE_SEIKAKU_CHOOSE+14;i++) Term_erase(17, i, 255);
				if(j==0) put_str("あなたの「地球」の体はどんな性格ですか？", 10, 30);
				else put_str("あなたの「月」の体はどんな性格ですか？", 10, 30);

				for(i=0;i<HECATE_SEIKAKU_CHOOSE;i++)
				{
					put_str( format("%c)%20s",'a'+i,seikaku_info[seikaku_list[i]].f_title),12+i,25);
				}
				c = inkey();

				if(c >= 'a' && c < 'a'+HECATE_SEIKAKU_CHOOSE)
				{
					if(j==0) p_ptr->magic_num1[7] = seikaku_list[c - 'a'];
					else p_ptr->magic_num1[8] = seikaku_list[c - 'a'];
					break;
				}
			}

		}

		//性格に「ラッキーガール」が混ざっていると白いオーラ永続変異を設定　ハードコーディング
		if(p_ptr->magic_num1[7] == SEIKAKU_LUCKY)
		{
				p_ptr->magic_num2[39] = 0x80;
				p_ptr->magic_num2[51] = 0x80;
		}

		if(p_ptr->magic_num1[8] == SEIKAKU_LUCKY)
		{
				p_ptr->magic_num2[63] = 0x80;
				p_ptr->magic_num2[75] = 0x80;
		}


		screen_load();
	}
	else
	{
		//異界の性格がそれ以外だと強制的に同じ性格にする
		p_ptr->magic_num1[7] = p_ptr->pseikaku;
		p_ptr->magic_num1[8] = p_ptr->pseikaku;

	}


}


//v1.1.21
/*:::兵士の取得技能を確認する*/
void check_soldier_skill(void)
{

	char buf[1024];
	int i;
	int total_skill_num = 0;
	cptr info_ptr;

	if(p_ptr->pclass != CLASS_SOLDIER){msg_print("ERROR:check_soldier_skill()が未対応のクラスで呼ばれた"); return;}

	for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++) 
		total_skill_num += p_ptr->magic_num2[i];

	if(!total_skill_num)
	{
		msg_print("まだ何の技能も得ていない。");
		return;
	}

	screen_save();

	while(1)
	{
		int line_num = 0;
		int j;
		char c;
		int s_array=0,num=0;

		for(i=0;i<total_skill_num + 5;i++) Term_erase(17, i, 255);

		prt("どの技能の説明を見ますか？(ESC:終了)",2,18);
		//技能一覧を表示
		for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++)
		{
			for(j=1;j<=p_ptr->magic_num2[i];j++)
			{
				sprintf(buf,"  %c) %s",'a' + line_num, soldier_skill_table[i][j].name);
				prt(buf,4+line_num,18);
				line_num++;
			}
		}

		while(1)
		{
			c = inkey();
			if(c == ESCAPE || c >= 'a' && c < 'a'+total_skill_num) break;
		}

		if(c == ESCAPE) break;

		num = c - 'a' + 1;
		
		//選択した文字が技能テーブルのどれに当たるかarrayとnumを計算
		for(s_array = 0; p_ptr->magic_num2[s_array] < num;s_array++) 
			num -= p_ptr->magic_num2[s_array];


		//説明文表示
		//画面クリア
		for(i=0;i<total_skill_num + 5;i++) Term_erase(17, i, 255);
		//技能タイトル表示
		sprintf(buf,("[%s]"),soldier_skill_table[s_array][num].name);
		prt(buf,3,18);
		//説明文表示
		sprintf(buf,"%s",soldier_skill_table[s_array][num].info);
		//rumor_newと同じ\\による自動改行
		for(i=0;i < sizeof(buf)-2;i++)
		{
			if(buf[i] == '\0')	break; 
			/*:::一部漢字が改行扱いされてしまうのを抑止*/
			else if(iskanji(buf[i]))
			{
				i++;
				continue;
			}
			else if(buf[i] == '\\') buf[i] = '\0';
		}
		buf[i] = '\0';
		buf[i+1] = '\0';
		info_ptr = buf;
		for (i = 0; i< 4; i++)
		{
			if(!info_ptr[0]) break;
			prt(info_ptr, 4+i, 18);
			info_ptr += strlen(info_ptr) + 1;
		}

		prt("何かキーを押してください", 0, 0);
		(void)inkey();
		prt("", 0, 0);

	}

	screen_load();
	return;

}

//v1.1.21
//兵士が技能ポイントを得る。level_rewardルーチンから呼ばれる。
//p_ptr->magic_num2[10]をポイント保存領域に使う
void get_soldier_skillpoint(void)
{
	if(p_ptr->pclass != CLASS_SOLDIER){msg_print("ERROR:get_soldier_skillpoint()が未対応のクラスで呼ばれた"); return;}

	//特定レベルでのみポイントを得る
	switch(p_ptr->lev)
	{
		case 1:	case 5:	case 10:case 15:case 20:case 24:case 28:
		case 32:case 35:case 38:case 41:case 44:case 46:case 48:
		case 50:
			msg_print("技能習得ポイントを得た！");
			p_ptr->magic_num2[10]++;
			break;

		default:
			break;
	}

	//v1.1.40 妖怪人形兵士はレベル1のポイントを得る前に初期装備品でレベル9になるのでレベル1のポイントを得られないというバグへの弥縫策
	if (p_ptr->prace == RACE_ANDROID && p_ptr->lev == 5)
	{
		p_ptr->magic_num2[10]++;

	}



}

//v1.1.21
//兵士がポイントを消費して技能を一つ習得する
//p_ptr->magic_num2[0-3]を技能レベルの保持に使う。
bool gain_soldier_skill(void)
{

	char buf[1024];
	int i;
	int s_array=0,num=0;
	cptr info_ptr;
	int skillpoint = p_ptr->magic_num2[10];

	//デバッグ時用フリーズ避け
	for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++) num += p_ptr->magic_num2[i];
	if(num >= SOLDIER_SKILL_ARRAY_MAX * SOLDIER_SKILL_NUM_MAX){msg_print("ERROR:既にすべての技能を習得済みだ"); return FALSE;}
	num=0;

	
	screen_save();

	while(1)
	{
		int j;
		char c;

		for(i=0;i<10;i++) Term_erase(17, i, 255);

		prt(format("どの技能を習得しますか？(ESC:キャンセル)"),2,18);
		prt(format("(残り技能習得ポイント:%d)",skillpoint),3,28);
		//技能一覧を表示
		for(i=0;i<SOLDIER_SKILL_ARRAY_MAX;i++)
		{
			j = p_ptr->magic_num2[i]+1;
			if(j > SOLDIER_SKILL_NUM_MAX)
			{
				sprintf(buf,"---習得完了---");
			}
			else if(soldier_skill_table[i][j].lev > p_ptr->lev)
			{
				sprintf(buf,"(今のレベルでは習得できない)");
			}
			else
			{
				if(j == SOLDIER_SKILL_NUM_MAX)
					sprintf(buf,"  %c) %s(2ポイント必要)",('a' + i), soldier_skill_table[i][j].name);
				else
					sprintf(buf,"  %c) %s",('a' + i), soldier_skill_table[i][j].name);
			}
			prt(buf,4+i,18);
		}

		while(1)
		{
			c = inkey();

			if(c == ESCAPE)
			{
				screen_load();
				return FALSE;
			}

			if(c >= 'a' && c < 'a'+SOLDIER_SKILL_ARRAY_MAX)
			{
				s_array = c - 'a';
				num = p_ptr->magic_num2[s_array]+1;
				if(num > SOLDIER_SKILL_NUM_MAX) continue; //習得完了のところのキーを押したとき
				else if(!skillpoint) continue;
				else if(skillpoint < 2 && num == SOLDIER_SKILL_NUM_MAX) continue;
				else if(soldier_skill_table[s_array][num].lev > p_ptr->lev) continue;
				break;
			}
		}

		//説明文表示
		//画面クリア
		for(i=0;i<10;i++) Term_erase(17, i, 255);
		//名前
		sprintf(buf,("[%s]"),soldier_skill_table[s_array][num].name);
		prt(buf,3,18);
		//説明文表示
		sprintf(buf,"%s",soldier_skill_table[s_array][num].info);
		//rumor_newと同じ\\による自動改行
		for(i=0;i < sizeof(buf)-2;i++)
		{
			if(buf[i] == '\0')	break; 
			/*:::一部漢字が改行扱いされてしまうのを抑止*/
			else if(iskanji(buf[i]))
			{
				i++;
				continue;
			}
			else if(buf[i] == '\\') buf[i] = '\0';
		}
		buf[i] = '\0';
		buf[i+1] = '\0';
		info_ptr = buf;
		for (i = 0; i< 4; i++)
		{
			if(!info_ptr[0]) break;
			prt(info_ptr, 4+i, 18);
			info_ptr += strlen(info_ptr) + 1;
		}

		if(get_check_strict("この技能を習得しますか？", CHECK_OKAY_CANCEL)) break;

	}
	msg_format("「%s」の技能を習得した！",soldier_skill_table[s_array][num].name);
	//習得レベルアップ
	p_ptr->magic_num2[s_array]++;
	//スキルポイント減少
	if(num == SOLDIER_SKILL_NUM_MAX)
		p_ptr->magic_num2[10] -= 2;
	else
		p_ptr->magic_num2[10] -= 1;


	screen_load();
	p_ptr->update |= (PU_BONUS|PU_HP|PU_MANA);
	return TRUE;

}

//v1.1.21 ポータルを通り抜ける
void pass_through_portal(void)
{
	bool flag_ok = FALSE;
	int x,y;
	if(!cave_have_flag_bold(py,px,FF_PORTAL)) return;

	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(cave_have_flag_bold(y,x,FF_PORTAL))
			{
				if(!player_bold(y,x))
				{
					//＠がいる以外のポータルがあったらループを抜ける
					flag_ok = TRUE;
					break;
				}
			}
		}
		if(flag_ok) break;
	}
	//行先がないとき終了　energy消費なしのはず
	if(!flag_ok)
	{
		msg_print("まだ行き先のポータルがない。");
		return;
	}

	if(!get_check_strict("もう片方のポータルに移動しますか？", CHECK_OKAY_CANCEL)) return;

	//反対側のポータルに移動。もしそこにモンスターがいたら入れ替わるはず
	move_player_effect(y,x,MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);
	energy_use = 100;



}


//v1.1.22兵士の銃弾変更
//SSM_**の番号を返す
//残弾減少やコスト処理は
int soldier_change_bullet(void)
{
	int i;
	int num_list[100];
	int num,total_num;
	bool flag_only_info = FALSE;
	bool flag_choose = FALSE;
	char buf[1000];

	//画面クリア
	screen_save();


	//ALLOW_REPEATリピート処理
	if (repeat_pull(&num))
	{
		flag_choose = TRUE;
	}

	while(!flag_choose)
	{
		int j;
		char c;
		num=0;
		total_num=0;

		for(i=0;i<21;i++) Term_erase(17, i, 255);

		if(flag_only_info)
			prt(format("どの銃弾の説明を見ますか？(ESC:キャンセル ?:確認を終える)"),2,18);
		else
			prt(format("どの銃弾を使用しますか？(ESC:キャンセル ?:銃弾の効果を確認する)"),2,18);

			prt(format("　弾種　　　　　　　　 MP 残弾　　弾種　　　　　　　　 MP 残弾"),4,18);

		//銃弾一覧を表示
		for(i=1;soldier_bullet_table[i].cost >= 0;i++)
		{
			int col,row;
			byte char_color;
			bool use_ammo = FALSE;
			bool flag_unable = FALSE;
			//魔弾技能レベルが設定されていない、または足りないとき除外
			if(!soldier_bullet_table[i].magic_bullet_lev)
				continue;
			if(soldier_bullet_table[i].magic_bullet_lev > p_ptr->magic_num2[SOLDIER_SKILL_MAGICBULLET])
				continue;
			//候補数カウント
			total_num++;
			//リストに銃弾indexを記録
			num_list[total_num] = i;

			if(soldier_bullet_table[i].magic_bullet_lev == SS_M_MATERIAL_BULLET)
				use_ammo = TRUE;

			//残弾不足、コスト不足のとき使用不可フラグ
			if(soldier_bullet_table[i].magic_bullet_lev == SS_M_MATERIAL_BULLET && !p_ptr->magic_num1[i])
				flag_unable = TRUE;
			else if(soldier_bullet_table[i].cost > p_ptr->csp)
				flag_unable = TRUE;

			if(use_ammo)
				sprintf(buf,"%c)%-20s %2d  %d",('a'+total_num-1),soldier_bullet_table[i].name,soldier_bullet_table[i].cost,
				p_ptr->magic_num1[i]);
			else
				sprintf(buf,"%c)%-20s %2d  --",('a'+total_num-1),soldier_bullet_table[i].name,soldier_bullet_table[i].cost);

			//使用不能の弾丸は灰字
			if(flag_unable) char_color = TERM_L_DARK;
			else char_color = TERM_WHITE;

			col = (total_num-1) % 2;
			row = (total_num-1) / 2;

			c_put_str(char_color,buf,row+5,18+col*32);

		}

		while(1)
		{
			c = inkey();

			if(c == '?')//?で情報確認フラグ反転
			{
				for(i=21;i<25;i++) Term_erase(17, i, 255);
				flag_only_info = !flag_only_info;
				break;
			}

			if(c == ESCAPE)
			{
				screen_load();
				return 0;
			}
			if(c >= 'a' && c <= ('a'+total_num-1))
			{
				num = num_list[c - 'a' + 1]; //numをリストから設定(num_listは1から格納)

				break;
			}
		}

		if(!num) continue;//?を押したとき

		//説明文を表示し再びループの最初へ
		if(flag_only_info)
		{
			cptr info_ptr;
			for(i=21;i<28;i++) Term_erase(17, i, 255);
			sprintf(buf,("[%s]"),soldier_bullet_table[num].name);
			prt(buf,21,18);
			//説明文表示
			sprintf(buf,"%s",soldier_bullet_table[num].info);
			//rumor_newと同じ\\による自動改行
			for(i=0;i < sizeof(buf)-2;i++)
			{
				if(buf[i] == '\0')	break; 
				/*:::一部漢字が改行扱いされてしまうのを抑止*/
				else if(iskanji(buf[i]))
				{
					i++;
					continue;
				}
				else if(buf[i] == '\\') buf[i] = '\0';
			}
			buf[i] = '\0';
			buf[i+1] = '\0';
			info_ptr = buf;
			for (i = 0; i< 4; i++)
			{
				if(!info_ptr[0]) break;
				prt(info_ptr, 22+i, 18);
				info_ptr += strlen(info_ptr) + 1;
			}
		}
		//選択が済んだらループから出る
		else
		{
			repeat_push(num);
			flag_choose = TRUE;
		}
	}

	screen_load();

	//弾切れやMP不足処理。二丁拳銃のためdo_cmd_fire_bun_aux()でも行う。やや冗長だが銃弾選択→ターゲット→弾切れmsgみたいなのもちょっと変なので
	if(soldier_bullet_table[num].magic_bullet_lev == SS_M_MATERIAL_BULLET && !p_ptr->magic_num1[num])
	{
		msg_print("その銃弾を持っていない。");
		return 0;
	}
	else if(soldier_bullet_table[num].cost > p_ptr->csp)
	{
		msg_print("MPが足りない。");
		return 0;
	}

	//num(p_ptr->magic_num1[]添字、SSM_***に等しい)を返して終了
	return num;
}


//v1.1.27 銃をブン投げてモンスターに命中したときに起こる効果。銃の威力に等しいボールが発生し、銃の残弾が1減る。
void gun_throw_effect(int y,int x,object_type *q_ptr)
{
	int dam,typ,mode,rad;

	int timeout_base,timeout_max;
	const activation_type* const act_ptr = find_activation_info(q_ptr);

	if(q_ptr->tval != TV_GUN ) return;

	timeout_base = calc_gun_timeout(q_ptr) * 1000;
	timeout_max = timeout_base * (calc_gun_load_num(q_ptr) - 1); 
	//timeout値がmaxを超えていると残弾0で何も起こらない
	if(q_ptr->timeout > timeout_max) return;

	//do_cmd_fire_gun_aux()みたいなハードコーディングをこちらにも追加。違法建築がまた一つ。
	switch(act_ptr->index)
	{
	case ACT_CRIMSON:
		rad = 3;
		dam = p_ptr->lev * p_ptr->lev * 6 / 50;
		typ = GF_ROCKET;
		if(one_in_(3)) msg_print("「上から来るぞ、気をつけろ！」");
		break;

	case ACT_BFG9000:
		rad = 5;
		dam = 800 + damroll(8,100);
		typ = GF_DISP_ALL;
		break;
	case ACT_BAROQUE:
		rad = 4;
		dam = 500;
		typ = GF_GENOCIDE;
		if(one_in_(3)) msg_print("《おわあ》");
		break;
	case ACT_DER_FREISCHUTZ:
		dam = 100;
		while(!one_in_(7)) dam += 100;
		rad = 1;
		typ = GF_SHARDS;
		break;

	case ACT_HARKONNEN2:
		rad = 7;
		dam = 1600;
		typ = GF_NUKE;
		break;

	case ACT_YAKUZA_GUN:
		//銃にロックがかかっていて撃てない。それを言うならヤクザ天狗でないのに持ってれば撃てるのかという話になるが
		msg_print("「セーフティ入りな。本人認証できませんドスエ」");
		return;

	//基本的な銃
	case ACT_GUN_VARIABLE:
		{
			int dice = q_ptr->dd;
			int sides = q_ptr->ds;
			int base = q_ptr->to_d;
			typ = q_ptr->xtra1;
			mode = q_ptr->xtra4;

			dam = base;
			if(dice && sides) dam += damroll(dice,sides);

			switch(mode)
			{
			case  GUN_FIRE_MODE_BALL:
			case  GUN_FIRE_MODE_ROCKET:
			case  GUN_FIRE_MODE_BREATH:
			case  GUN_FIRE_MODE_SPARK:
				rad = 2 + p_ptr->lev / 15;
				break;
			default:
				rad = 0;
				break;
			}
			break;
		}
	default:
		msg_print("ERROR:gun_throw_effect()にてこの銃の射撃基礎パラメータ処理がされていない");
		return;
	}

	//爆発発生処理
	project(0, rad, y, x, dam, typ,(PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);

	//残弾消費
	if(q_ptr->timeout + timeout_base > 32000L)
	{
		msg_print("ERROR:この銃のタイムアウト値が32000を超えた");
		return;
	}
	q_ptr->timeout += timeout_base;



}

//v1.1.27 新システム　結界ガード
//set:＠による操作
//guard_break:set=FALSEのときガードブレイクならTRUE,麻痺などによる強制解除はFALSE 
bool set_mana_shield(bool set, bool guard_break)
{

	bool notice = FALSE;

	if (p_ptr->is_dead) return FALSE;

	if (set)
	{
		if(!cp_ptr->magicmaster)
		{
			msg_print("あなたは結界の張り方を心得ていない。");
			return FALSE;
		}

		if ((p_ptr->special_defense & SD_MANA_SHIELD))
		{
			msg_print("あなたは防御を解いた。");
			p_ptr->special_defense &= ~(SD_MANA_SHIELD);
		}
		else
		{
			if(p_ptr->confused || p_ptr->stun > 50)
			{
				msg_print("結界に集中できない！");
				return FALSE;
			}
			if(p_ptr->anti_magic || dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
			{
				msg_print("結界を張れない。");
				return FALSE;
			}

			msg_print("あなたは結界を張って身構えた。");
			p_ptr->special_defense |= SD_MANA_SHIELD;
		}

		notice = TRUE;
	}
	//攻撃を受けて解除されるとき朦朧
	else
	{
		if (p_ptr->special_defense & SD_MANA_SHIELD)
		{
			if(guard_break)
			{
				msg_print("ガードブレイクされた！");
				set_stun(p_ptr->stun + 30 + randint1(30));
			}
			else
			{
				msg_print("結界が解けた！");
			}

			notice = TRUE;
			/* Use the value */
			p_ptr->special_defense &= ~(SD_MANA_SHIELD);
		}
	}

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Result */
	return (TRUE);
}



///v1.1.27 結界ガードによるダメージ軽減とMP減少
void check_mana_shield(int *dam, int damage_type)
{
	int reduce_dam, use_mana;

	int shield_power;

	if (!(p_ptr->special_defense & SD_MANA_SHIELD)) return;

	//一部のダメージは非対象
	//DAMAGE_NOESCAPEはどう扱った物か。落馬やオーラや罠や*破壊*は軽減できていい気がするが爆発の薬を飲んだり溺れるのは軽減されたら変な気がする。とりあえず軽減可能に。
	if(damage_type == DAMAGE_USELIFE || damage_type == DAMAGE_LOSELIFE || damage_type == DAMAGE_GENO)
		return;

	count_damage_guard_break += *dam;

	if(damage_type == DAMAGE_FORCE)
	{
		msg_print("結界を貫通された！");
		reduce_dam = 0;
		use_mana = 0;
	}
	else
	{
		//軽減ダメージ計算
		reduce_dam = (*dam + 1) / 2;

		//消費MP計算 消費魔力減少のとき-25%
		use_mana = reduce_dam;
		if (p_ptr->dec_mana) use_mana = use_mana * 3 / 4;

		//MPが足りないぶんはHPダメージに逆流(消費魔力減少の効果は発揮されたまま)
		if (p_ptr->csp < use_mana)
		{
			reduce_dam -= use_mana - p_ptr->csp;
			use_mana = p_ptr->csp;
			//ガードブレイク
			set_mana_shield(FALSE, TRUE);
		}

	}

	shield_power = MIN(p_ptr->msp, 500);
	if (p_ptr->easy_spell) shield_power += shield_power / 2;

	//次の行動までに最大MP以上のダメージを食らったらガードブレイク
	if(count_damage_guard_break > shield_power)
		set_mana_shield(FALSE,TRUE);

	//paranoia
	if (reduce_dam < 0) reduce_dam = 0;

	//ダメージとMP減少
	*dam -= reduce_dam;
	p_ptr->csp -= use_mana;
	p_ptr->redraw |= PR_MANA;

}






//ネムノ縄張り作成1グリッドチェック 
//temp_x[],temp_y[],temp_n[]を使う。

static bool make_nemuno_sanctuary_aux2(byte y, byte x)
{
	cave_type *c_ptr = &cave[y][x];

	//画面外は無視
	if (!in_bounds2(y, x))
		return TRUE;

	//ROOM外は非対象
	if (!(c_ptr->info & CAVE_ROOM))
		return TRUE;

	//壁やドアは非対象 木や岩石を入れるためにTUNNELもチェック
	if (cave_have_flag_bold(y, x, FF_WALL)
		|| cave_have_flag_bold(y, x, FF_DOOR)
		|| cave_have_flag_bold(y, x, FF_TUNNEL)
		)
		return TRUE;

	//すでにチェックした場所は何もしない。同じところをループしないように。
	if(c_ptr->info & CAVE_TEMP)
		return TRUE;

	//モンスターが居たらFALSEを返す。縄張り判定に失敗する。
	if(c_ptr->m_idx && is_hostile(&m_list[c_ptr->m_idx]))
		return FALSE;

	if (temp_n == TEMP_MAX) return TRUE;

	/* Mark the grid as "seen" */
	c_ptr->info |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;

	//守りのルーンで四角く囲えばROOMでなくても縄張りにできると面白いかも

	return TRUE;

}

//v1.1.35 ネムノが縄張りを作る。lite_room()を参考にした。
//v2.0.16 「縄張り」を「聖域」に変えた。外來韋編での記述より。
void make_nemuno_sanctuary(void)
{
	bool flag_success = TRUE;
	int i;
	byte x,y;

	if(p_ptr->pclass != CLASS_NEMUNO)
	{
		msg_print("ERROR:ネムノ以外でmake_nemuno_sanctuary()が呼ばれた");
		return;

	}

	if(!(cave[py][px].info & CAVE_ROOM))
	{
		msg_print("部屋でない場所を聖域にすることはできない。");
		return;
	}

	//v1.1.36 temp_nカウントとCAVE_TEMPフラグをリセットする
	//壁を超えて複数部屋にわたり縄張りが生成されるという報告を受けたので、ひょっとしてtemp_nがリセットされていない処理がどこかにあるのではないかと思い追加しておく
	for (i = 0; i < temp_n; i++)
	{
		y = temp_y[i];
		x = temp_x[i];
		cave[y][x].info &= ~(CAVE_TEMP);
	}
	temp_n = 0;



	if(!make_nemuno_sanctuary_aux2(py,px)) flag_success = FALSE;

	//隣のグリッドをチェックしながら縄張りにできるかどうかの判定を続ける
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		//壁やドアは縄張りになるが、そこから先までは縄張り判定が続かないことにする。
		//やっぱり縄張りにもならないことにした。穴熊防止
		//if(cave_have_flag_bold(y,x,FF_WALL)
		//	|| cave_have_flag_bold(y,x,FF_DOOR))
		//	continue;

		/* Spread adjacent */
		if(!make_nemuno_sanctuary_aux2(y + 1, x)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y - 1, x)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y , x + 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y , x - 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y + 1, x + 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y - 1, x - 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y - 1, x + 1)) flag_success = FALSE;
		if(!make_nemuno_sanctuary_aux2(y + 1, x - 1)) flag_success = FALSE;

		if(!flag_success)
			break;

		if (temp_n == TEMP_MAX)
			break;

	}

	if(cheat_xtra)
		msg_format("認識グリッド数:%d",temp_n);

	if(!flag_success)
	{
		msg_print("この部屋には敵対的な気配がする！ここを聖域にすることはできない。");
	}
	else if(temp_n < 4)
	{
		msg_print("ここは狭すぎて聖域にできない。");
	}
	else
	{
		//古い縄張りを放棄
		erase_nemuno_sanctuary(FALSE,TRUE);

		//縄張りフラグ付与処理
		for (i = 0; i < temp_n; i++)
		{
			y = temp_y[i];
			x = temp_x[i];
			cave[y][x].cave_xtra_flag |= CAVE_XTRA_FLAG_NEMUNO;
			lite_spot(y,x);
		}

		msg_print("あなたはこの部屋を不可侵の聖域に定めた！");
		p_ptr->update |= (PU_BONUS | PU_HP);

	}

	//temp_nカウントとCAVE_TEMPフラグをリセットする
	for (i = 0; i < temp_n; i++)
	{
		y = temp_y[i];
		x = temp_x[i];
		cave[y][x].info &= ~(CAVE_TEMP);
	}
	temp_n = 0;


}

//v1.1.35 ネムノが縄張りを放棄する処理
void erase_nemuno_sanctuary(bool force,bool redraw)
{
	int x,y;
	bool flag_erase = FALSE;

	if(p_ptr->pclass != CLASS_NEMUNO)
	{
		msg_print("ERROR:ネムノ以外でerase_nemuno_sanctuary()が呼ばれた");
		return;
	}

	//古い縄張りフラグを消去
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if(cave[y][x].cave_xtra_flag & CAVE_XTRA_FLAG_NEMUNO)
			{
				flag_erase = TRUE;
				cave[y][x].cave_xtra_flag &=  ~(CAVE_XTRA_FLAG_NEMUNO);
				if(redraw) lite_spot(y,x);
			}
		}
	}
	if(flag_erase)
	{
		if(force)
		{
			msg_print("聖域を奪われてしまった！");
		}
		else
		{
			msg_print("あなたは古い聖域を放棄した。");
		}
	}
	//v1.1.54 ネムノ召喚阻害特技の解除
	p_ptr->special_defense &= ~(SD_UNIQUE_CLASS_POWER);
	p_ptr->redraw |= (PR_STATUS);


}



//v1.1.46 女苑が浪費した金額をp_ptr->magic_num1[0][1]に記録する。$999,999,999,999が上限
//v1.1.92 専用性格のときには地形変化して石油地形にしたグリッド数をカウントすることにする
void	jyoon_record_money_waste(int sum)
{
	int dig1, dig2;

	if (p_ptr->pclass != CLASS_JYOON) return;

	dig1 = p_ptr->magic_num1[0];
	dig2 = p_ptr->magic_num1[1];

	dig1 += sum % 1000000;

	if (dig1 > 1000000)
	{
		dig1 -= 1000000;
		dig2 += 1;
	}
	dig2 += sum / 1000000;

	if (dig2 > 1000000)
	{
		dig1 = 999999;
		dig2 = 999999;
	}
	p_ptr->magic_num1[0] = dig1;
	p_ptr->magic_num1[1] = dig2;


}


//v1.1.48 紫苑が装備品に差し押さえを食らう。
//process_worldから50ゲームターンごとに呼ばれ、o_ptr->xtra5でカウントし、100になったら没収。約12時間。
//装備を外したらカウントが進まなくなるがインベントリや家など全部のカウントをチェックしたり装備外し即回収にしたりするのも面倒なので放置
void shion_seizure(int slot)
{
	char o_name[MAX_NLEN];
	object_type *o_ptr = &inventory[slot];
	int value;
	u32b flgs[TR_FLAG_SIZE];

	if (p_ptr->pclass != CLASS_SHION) return;
	if (!o_ptr->k_idx) return;


	object_flags(o_ptr, flgs);

	//借金がなくなったらカウントストップ
	if (p_ptr->au >= 0)
	{
		o_ptr->xtra5 = 0;
		return;
	}

	//呪われたアイテムはパス　TR_のフラグは別処理らしいので記述追加
	if (object_is_cursed(o_ptr) || have_flag(flgs, TR_TY_CURSE) || have_flag(flgs, TR_AGGRAVATE)
		|| have_flag(flgs, TR_ADD_L_CURSE) || have_flag(flgs, TR_ADD_H_CURSE)) return;

	value = object_value_real(o_ptr);
	if ( value < 1) return;


	if (!o_ptr->xtra5)
	{
		object_desc(o_name, o_ptr, 0);
		msg_format("%sは差し押さえを受けた！",o_name);
		p_ptr->window |= PW_EQUIP;
		disturb(1, 0);
	}

	o_ptr->xtra5++;

	//憑依中は回収を受けない
	if (check_invalidate_inventory(slot)) return;

	if (o_ptr->xtra5 >= 100)
	{
		object_desc(o_name, o_ptr, 0);
		msg_format("%sが回収されてしまった！", o_name);

		inven_item_increase(slot, -1);
		inven_item_describe(slot);
		inven_item_optimize(slot);

		p_ptr->au += value / 2;
		p_ptr->update |= PU_BONUS;
		p_ptr->redraw |= (PR_EQUIPPY | PR_GOLD);
		p_ptr->window |= PW_EQUIP;
		disturb(1, 0);

	}


}


//v1.1.48 紫苑の物乞いコマンド　行動順消費するときにTRUEを返す
bool shion_begging(void)
{
	int y, x;
	int dir;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];
	int power = 20 + chr_adj;

	monster_type *m_ptr;
	monster_race *r_ptr;

	if (p_ptr->inside_quest || p_ptr->inside_arena)
	{
		msg_print("ダンジョンの雰囲気は殺伐としている。食物をねだるどころではないようだ。");
		return FALSE;
	}

	if (p_ptr->food < PY_FOOD_ALERT) power *= 2;
	if (p_ptr->food < PY_FOOD_WEAK) power *= 2;
	if (p_ptr->food < PY_FOOD_STARVE) power *= 2;

	if (!get_rep_dir2(&dir)) return FALSE;
	if (dir == 5) return FALSE;

	y = py + ddy[dir];
	x = px + ddx[dir];
	m_ptr = &m_list[cave[y][x].m_idx];

	if (cave[y][x].m_idx && (m_ptr->ml))
	{
		char m_name[80];
		r_ptr = &r_info[m_ptr->r_idx];
		monster_desc(m_name, m_ptr, 0);


		//起こす
		set_monster_csleep(cave[y][x].m_idx, 0);

		if (is_pet(m_ptr))
		{
			msg_format("あなたの配下はみな一文無しだ。");
			return TRUE;
		}

		if (p_ptr->food < PY_FOOD_WEAK)
		{
			if (one_in_(2))
				msg_format("あなたは恥も外聞もなく%sに縋りついた！", m_name);
			else
				msg_format("あなたは形振り構わず%sに泣きついた！", m_name);
		}
		else
		{
			if (one_in_(4))
				msg_format("「何か恵んでよー。ひもじいよー。」", m_name);
			else if (one_in_(3))
				msg_format("あなたは%sに食物をねだった。", m_name);
			else if (one_in_(2))
				msg_format("あなたは欠けた鉢を抱えて%sを物欲しげに見つめた…", m_name);
			else
				msg_format("あなたは%sに食べ物をせがんだ。", m_name);
		}

		if (r_ptr->flags2 & (RF2_EMPTY_MIND | RF2_WEIRD_MIND) || r_ptr->flagsr & RFR_RES_ALL)
		{
			msg_format("%sは無反応だ。", m_name);
			return TRUE;
		}

		if (r_ptr->flags1 & RF1_QUESTOR)
		{
			msg_format("%sとは完全に敵対している。決着をつけるしかないようだ。", m_name);
			return TRUE;
		}

		if (m_ptr->mflag & MFLAG_SPECIAL)
		{
			msg_format("%sはもうあなたに取り合ってくれない。", m_name);
			return TRUE;
		}

		if (r_ptr->flags3 & RF3_GEN_MASK)
		{
			if (r_ptr->flags3 & (RF3_GEN_HUMAN)) power = power * 3 / 2;
			else if (r_ptr->flags3 & RF3_GEN_KWAI) power /= 2;
			else if (r_ptr->flags3 & RF3_GEN_MOON) power /= 3;
			else if (r_ptr->flags3 & RF3_GEN_WARLIKE) power /= 2;
		}
		else if (r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD)) power /= 3;
		else if (r_ptr->flags3 & RF3_ANG_CHAOS) power /= 2;
		else if (r_ptr->flags3 & RF3_HUMAN) power = power * 3 / 2;

		if (p_ptr->cursed & TRC_AGGRAVATE) power /= 2;
		else if (is_friendly(m_ptr)) power *= 2;

		//天子は公式でつるんでるので仲良くしとく
		if (m_ptr->r_idx == MON_TENSHI) power = 100;

		if (randint1(100) < power) //成功
		{
			object_type forge, *q_ptr = &forge;

			if (m_ptr->r_idx == MON_TENSHI)
			{
				msg_format("%sは天界の桃をわけてくれた。", m_name);
			}
			else if(is_gen_unique(m_ptr->r_idx) || (r_ptr->flags3 & RF3_HUMAN)|| (r_ptr->flags3 & RF3_DEMIHUMAN)
				|| my_strchr("hknoptuzAFKLOPSTUVWY", r_ptr->d_char))
			{
				if (power > 50)
					msg_format("%sは気遣わしげに何かを差し出してきた。", m_name);
				else if (power > 30)
					msg_format("%sは呆れた顔をしつつも何かを分けてくれた。", m_name);
				else if (power > 15)
					msg_format("%sはいかにも嫌々そうに何かを恵んでくれた。", m_name);
				else
					msg_format("%sはあなたを嘲笑しつつ何かを足元に放った。", m_name);

			}
			else
			{
				msg_format("%sはどこからかアイテムを出した。", m_name);
			}


			if (m_ptr->r_idx == MON_MINORIKO)
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEET_POTATO));
			else if (m_ptr->r_idx == MON_SANAE)
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_MIRACLE_FRUIT));
			else if (m_ptr->r_idx == MON_RINGO)
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_DANGO));
			else if (m_ptr->r_idx == MON_TENSHI)
				object_prep(q_ptr, lookup_kind(TV_SWEETS, SV_SWEETS_PEACH));
			else if (r_ptr->d_char == 'K')
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_CUCUMBER));

			else if (is_friendly(m_ptr) && weird_luck())
				object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_CURING));
			else if( (r_ptr->flags2 & RF2_SMART) && weird_luck())
				object_prep(q_ptr, lookup_kind(TV_STAFF, SV_STAFF_IDENTIFY));
			else if (power > 50 && weird_luck())
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_SENTAN));
			else if (power < 30 && weird_luck())
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_SLIMEMOLD));
			else if (one_in_(6))
				object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH));
			else if (one_in_(3))
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_VENISON));
			else
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

			apply_magic(q_ptr, dun_level, AM_NO_FIXED_ART);
			q_ptr->discount = 99;
			drop_near(q_ptr, -1, y, x);

			msg_format("%sは去っていった。", m_name);

			check_quest_completion(m_ptr);
			delete_monster_idx(cave[y][x].m_idx);

		}
		else  //失敗
		{
			//サグメは無口
			if (m_ptr->r_idx == MON_SAGUME)
			{
				msg_format("%sはあなたを蔑むように睨んだ。", m_name);
			}

			else if (is_gen_unique(m_ptr->r_idx) || (r_ptr->flags3 & RF3_HUMAN) || (r_ptr->flags3 & RF3_DEMIHUMAN)
				|| my_strchr("hknoptuzAFKLOPSTUVWY", r_ptr->d_char))
			{
				if (power > 50)
					msg_format("%sは済まなそうに首を振った。", m_name);
				else if (power > 30)
					msg_format("%sはあなたの訴えを黙殺した。", m_name);
				else if (power > 15)
					msg_format("%sはあなたに施す気などさらさらないようだ。", m_name);
				else
					msg_format("%sはあなたに侮蔑的な言葉を浴びせた。", m_name);

			}
			else if (r_ptr->flags2 & RF2_SMART)
			{
				if (power > 50)
					msg_format("%sは何もくれなかった。", m_name);
				else
					msg_format("%sは威圧的に拒絶した。", m_name);
			}
			else
			{
				if (power > 30)
					msg_format("%sはあなたの訴えを理解できないようだ。", m_name);
				else
					msg_format("%sはあなたのことを餌だとしか思っていない！", m_name);

			}

			//さらに判定を失敗すると、特殊フラグを立ててもう応じてくれなくする
			if (randint1(50) > power || one_in_(6))
			{
				msg_format("%sはもう耳を貸してくれそうにない！", m_name);
				m_ptr->mflag |= MFLAG_SPECIAL;

			}
		}

	}
	else
	{
		msg_format("そこには何もいない。");
		return FALSE;
	}
	return TRUE;

}

//v1.1.76 幽々子の「西行寺無余涅槃」効果
//使用時とターン経過時(process_upkeep_with_speed)に呼ばれる
void yuyuko_nehan(void)
{
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	if (one_in_(3))
	{
		msg_print("冷たく澄んだ光が放たれた！");
		deathray_monsters();
		project_hack(GF_HAND_DOOM, plev * 5 + chr_adj * 5);
	}

	if (one_in_(2))
	{
		msg_print("桜の花弁が降り注いだ！");
		project_hack(GF_REDEYE, plev * 10 + chr_adj * 10);
		confuse_monsters(plev * 5);
		stun_monsters(plev * 5);
		turn_monsters(plev * 5);

	}

	if (one_in_(2))
	{
		msg_print("無数の蝶が辺りを埋め尽くした！");
		project_hack2(GF_DISP_ALL, plev, chr_adj+10 , 0);
	}

	msg_print("死の力が周囲を包み込んだ...");
	project_hack(GF_NETHER, damroll(plev, chr_adj + 20));


}


//魔力不足ペナルティによる気絶処理
//魔法使用(要オプション設定)か反獄王の酒でここに来る
//sleep_turn_base:気絶ターン数　基本的にMP不足分
void process_over_exert(int sleep_turn_base)
{
	int sleep_turn;

	p_ptr->csp = 0;
	p_ptr->csp_frac = 0;
	p_ptr->redraw |= PR_MANA;

	if (sleep_turn_base < 1) return;

	if (RACE_NOT_MANA_DRIVEN)
	{
		sleep_turn = randint1(sleep_turn_base / 5 + 1);
	}
	else
	{
		sleep_turn = (sleep_turn_base + 1) / 2 + randint1(sleep_turn_base);
	}


	if (sleep_turn < 5 && !one_in_(6))
	{
		set_stun(sleep_turn * 20);
	}
	else
	{
		if (RACE_NOT_MANA_DRIVEN)
			msg_print("あなたは急激な魔力喪失によって気絶した！");
		else
			msg_print("あなたは魔力欠乏のため昏睡状態に陥った！");

		//耐性を無視して麻痺
		(void)set_paralyzed(p_ptr->paralyzed + sleep_turn);
	}

}





//ユイマン特技「ディアジェノサイダー」
//矢を指定し、ターゲットを視界内でランダムに選定し、指定回数ぶんその矢を投擲(throw_aux2)する
//行動順消費時TRUE
bool deer_genocider(int arrow_num)
{

	int item;
	object_type* o_ptr;
	cptr q, s;
	int num = arrow_num;
	int used_num;

	if (p_ptr->image || p_ptr->blind)
	{
		msg_print("今はこの特技を使えない。");
		return FALSE;
	}

	//投げる矢を選択
	q = "どれを投げますか? ";
	s = "矢を持っていない。";
	item_tester_hook = object_is_ammo;
	if (!get_item(&item, q, s, (USE_INVEN)))return FALSE;
	if (item >= 0)	o_ptr = &inventory[item];
	else return FALSE;

	if (o_ptr->number < num) num = o_ptr->number;
	msg_format("あなたは矢を%s掴んだ！",num > 1 ? "まとめて":"");

	//有効な矢の本数ぶんループ
	for (used_num = 0; used_num < num; used_num++)
	{
		int i,target_who_tmp=0;
		monster_type* m_ptr;

		object_type forge;
		object_type *tmp_o_ptr = &forge;

		object_copy(tmp_o_ptr, o_ptr);
		tmp_o_ptr->number = 1;

		//視界内のターゲットからランダムに選定
		int tmp_idx_cnt = 0;
		for (i = 1; i < m_max; i++)
		{
			m_ptr = &m_list[i];

			if (!m_ptr->r_idx) continue;
			if (is_pet(m_ptr)) continue;
			if (is_friendly(m_ptr)) continue;
			if (!m_ptr->ml) continue;
			if (!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
			//if (range && m_ptr->cdis > range) continue;

			tmp_idx_cnt++;
			if (one_in_(tmp_idx_cnt)) target_who_tmp = i;
		}
		if (!tmp_idx_cnt) break;

		target_who = target_who_tmp;
		m_ptr = &m_list[target_who];
		target_row = m_ptr->fy;
		target_col = m_ptr->fx;

		//投擲処理　
		do_cmd_throw_aux2(py, px, target_row, target_col, 1, tmp_o_ptr, 7);

	}

	if (!used_num)
	{
		msg_print("しかし周囲に標的がいなかった。");
		return TRUE;
	}

	//矢の減少処理
	inven_item_increase(item, -used_num);
	inven_item_describe(item);
	inven_item_optimize(item);

	return TRUE;

}




