#include "angband.h"
//v1.1.86
//新魔道具「アビリティカード」関係の関数




//アビリティカード生成時にapply_magic()の代わりに呼ぶ。apply_magic()に放り込んでもそこから呼ばれるがランクなどの指定はできない。
//カードのIDXがo_ptr->pvalに記録される。IDXはability_card_list[]の添字に一致する。
//ability_card_list[]にカード名、カードの発動難度、チャージ時間などが設定されている。
//card_idx:生成するカードのidx idxが有効範囲外のときランダム生成
//card_rank_min/max:生成するカードのランクを指定したいとき。0ならランダム生成
//戻り値:生成されたカードのランク(乱数テスト用)
int apply_magic_abilitycard(object_type *o_ptr, int card_idx, int card_rank_min, int card_rank_max)
{

	int i;
	int new_rank = -1, new_idx = -1;
	bool flag_extra_bonus = FALSE;

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_print("ERROR:apply_magic_abilitycard()にTV_ABILITY_CARD以外が渡された");
		return 0;
	}

	//カードidxが指定されているとき
	if (card_idx >= 0 && card_idx < ABILITY_CARD_LIST_LEN)
	{
		new_idx = card_idx;
	}
	else
	{
		int candi_num = 0;

		//指定されていないときカードをランダム生成。まずランクを決める

		if (card_rank_min < 2 && !one_in_(5)) new_rank = 1;//コモン 80%
		else if (card_rank_min < 3 && !one_in_(5)) new_rank = 2;//レア 16%
		else if (card_rank_min < 4 && !one_in_(5)) new_rank = 3;//ユニーク？スーパーレア？ 3.2%
		else new_rank = 4; //レジェンダリーとかUSSRとかそんなの 0.8%

		//weird_luckを通るとランクアップすることにしてみる。グラフィックがあるなら特殊演出を出したいところだ。
		//通常は1/12、白オーラか占い幸運は1/9、両方重複するか幸運のメカニズムで1/7で通る。
		//weird_luck込みの排出率は↓のようになるはず。
		// 1/12 73%/22%/4.3%/1.1%
		// 1/7  69%/30%/5%/1.25%
		if (weird_luck()) new_rank++;

		if (card_rank_max > 0 && new_rank > card_rank_max) new_rank = card_rank_max;

		//ランク4にさらにweird_luck()を通ってランク5になってしまったら4に戻す。
		if (new_rank > 4)
		{
			//箱を開けたときのみ、ランク5になったらボーナスでもう一枚。箱の中から箱が出てこないようにするグローバルフラグを再利用
			if(opening_chest ) flag_extra_bonus = TRUE;
			new_rank = 4;
		}

		//カードリストの中からレアリティが一致したカードをランダムに抽選
		for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
		{
			if (ability_card_list[i].rarity_rank != new_rank) continue;

			//EXTRAモードでは帰還用カードが出ない
			if (EXTRA_MODE && i == ABL_CARD_SANAE1) continue;

			candi_num++;
			if (!one_in_(candi_num)) continue;
			new_idx = i;
		}

		if (new_idx < 0)
		{
			msg_format("ERROR:カードが一枚も選定されなかった(min:%d max:%d)", card_rank_min, card_rank_max);
			return 0;
		}
	}

	//カードの種類をpvalに格納
	o_ptr->pval = new_idx;

	//当たりが出たらもう一枚
	if (flag_extra_bonus) o_ptr->number++;

	//*鑑定*済にする
	object_aware(o_ptr);
	object_known(o_ptr);
	o_ptr->ident |= (IDENT_MENTAL);

	return new_rank;
}


//特定のアビリティカードをザック内に何枚保有しているか確認する。
//職によっては追加インベントリも確認する。
//card_idxはability_card_list[]の添字
//戻り値はカードの合計枚数(複数箇所に持っているときは全ての合計枚数)。仕様上10以上のときも9が返る
int	count_ability_card(int card_idx)
{
	int num = 0;
	int i;
	int inven2_num;

	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:count_ability_card()に不正なidx(%d)が渡された", card_idx);
		return 0;

	}

	//character_ickyがTRUEのときは、店や自宅に置かれたアイテムを調べているものと見なして個数を常に1にする
	//if (character_icky) return 1;
	//v1.1.87 ↑店で買い物とかしたときにcalc_bonuses()が呼ばれてるらしく＠のパラメータが一時的に変になるのでグローバル変数のフラグに置き換えた
	if (hack_flag_store_examine) return 1;


	//ザック内(インベントリ0〜22)をチェック
	for (i = 0; i<INVEN_PACK; i++)
	{
		if (!inventory[i].k_idx) continue;
		if (inventory[i].tval != TV_ABILITY_CARD) continue;
		//カードの種類はpvalやsvalでなくxtra1で管理している
		if (inventory[i].pval == card_idx) num += inventory[i].number;
	}

	inven2_num = calc_inven2_num();

	//v1.1.87 追加インベントリがあればそれもチェック 菫子のは所有といえるかどうかちょっと怪しいがまあ良し
	for (i = 0; i < inven2_num; i++)
	{
		if(!inven_add[i].k_idx) continue;
		if (inven_add[i].tval != TV_ABILITY_CARD) continue;
		if (inven_add[i].pval == card_idx) num += inven_add[i].number;
	}

	//所有型のカードを10枚以上持っても効果は上昇しない
	if (num>9) num = 9;

	return num;

}

//カードによるHP,SP上昇の計算式
//use_ability_card_axu()による説明文生成のほかに実際にHP/MPを増やすときにも同じ式を使うのでまとめることにした
int calc_ability_card_add_hp_sp(int card_idx, int card_num)
{
	int bonus = 0;
	int plev = p_ptr->lev;
	switch (card_idx)
	{
		case ABL_CARD_LIFE:
		{
			bonus = plev;

			if (card_num > 1) bonus += plev * (card_num - 1) /10;
			if (card_num == 9) bonus = plev * 2;

		}
		break;
		case ABL_CARD_SPELL:
		{
			bonus = plev / 2;

			if (card_num > 1) bonus += plev * (card_num - 1) / 20;
			if (card_num == 9) bonus = plev;

		}
		break;
		case ABL_CARD_PHOENIX:
		{

			if (card_num == 9) 
				bonus = plev * 8;
			else
				bonus = plev * 3 + (card_num - 1) * plev/2;
		}
		break;
		
		default:
			msg_format("ERROR:calc_ability_card_add_hp_sp()に想定外のカード(idx:%d)が渡された",card_idx);

	}

	return bonus;


}

//背中の扉などカードに関する計算値を返す
//当初防御確率だけだったが別に他のパラメータに使わん理由もない
int calc_ability_card_prob(int card_idx, int card_num)
{
	int result_param = 0;
	switch (card_idx)
	{
	case ABL_CARD_BACKDOOR:
	case ABL_CARD_DANMAKUGHOST:
	case ABL_CARD_BASS_DRUM:
	case ABL_CARD_PSYCHOKINESIS:

	{
		result_param = 25;

		if (card_num > 1) result_param += (card_num-1)*5;

		if (card_num >= 9) result_param = 70;

	}
	break;

	case ABL_CARD_UFO:
	{
		result_param = 10;
		if (card_num > 1) result_param += (card_num - 1) * 2;
		if (card_num >= 9) result_param = 30;

	}
	break;

	case ABL_CARD_KAGUYA:
	{
		result_param = 20;

		if (card_num > 1) result_param += (card_num - 1) * 3;
		if (card_num >= 9) result_param = 50;

	}
	break;
	case ABL_CARD_MAGATAMA:
	{
		result_param = 20;

		if (card_num > 1) result_param += (card_num - 1) * 8;
		if (card_num >= 9) result_param = 100;

	}
	break;
	case ABL_CARD_HANIWA:
	{
		result_param = 20;
		if (card_num > 1) result_param += (card_num - 1) * 3;
		if (card_num >= 9) result_param = 50;

	}
	break;

	case ABL_CARD_JIZO:
		result_param = 50;
		if (card_num > 1) result_param += (card_num - 1) * 10;
		if (card_num >= 9) result_param = 150;

		break;

	case ABL_CARD_PATHE_SPELL:
		result_param = 30;
		if (card_num > 1) result_param += (card_num - 1) * 7;
		if (card_num >= 9) result_param = 100;

		break;

	case ABL_CARD_KAMIYAMA:
		result_param = 50;
		if (card_num > 1) result_param += (card_num - 1) * 10;
		if (card_num >= 9) result_param = 150;

		break;

	case ABL_CARD_EIKI:
		result_param = 100;
		if (card_num > 1) result_param -= (card_num - 1) * 5;
		if (card_num >= 9) result_param = 50;
		break;



	default:
		msg_format("ERROR:calc_ability_card_prob()に想定外のカード(idx:%d)が渡された", card_idx);

	}




	if (p_ptr->wizard && result_param < 100) result_param = 100;

	return result_param;

}



//新魔道具「アビリティカード」のカード発動/説明文生成
//カード選択や成功判定はすでに済んでいる
//カードをIで調べるときはonly_infoをTRUEにしてこれを呼び出すと説明文文字列へのアドレスが返る。
//only_infoでの呼び出しは発動不可能なカードでも呼ばれる。

cptr use_ability_card_aux(object_type *o_ptr, bool only_info)
{
	int dir;
	int card_num; //カードのスタックされた枚数　保有カードの効果に影響　10枚以上持ってても最大値は9
	int card_idx;
	int card_charge;
	int plev = p_ptr->lev;
	int chr_adj = adj_general[p_ptr->stat_ind[A_CHR]];

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_format("ERROR:use_ability_card_aux()にアビリティカード以外が渡された(Tval:%d)", o_ptr->tval);
		return NULL;
	}
	card_idx = o_ptr->pval;

	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:use_ability_card()に不正なカードidx(%d)を持ったアビリティカードが渡された", card_idx);
		return NULL;
	}

	card_num = count_ability_card(card_idx);

	if (!ability_card_list[card_idx].activate && !only_info)
	{
		msg_format("ERROR:発動不可能なカードが発動された(idx:%d)", card_idx);
		return NULL;
	}

	//card_charge = ability_card_list[o_ptr->pval].charge_turn;



	switch (card_idx)
	{

	case ABL_CARD_BLANK:
	{
		if (only_info) return format(" このカードはなんの役にも立たない。");

	}
	break;

	case ABL_CARD_LIFE:
	case ABL_CARD_PHOENIX:
	{
		int add_hp;

		add_hp = calc_ability_card_add_hp_sp(card_idx, card_num);

		if (only_info) return format(" このカードを所持していると最大HPが%d増加する。同じ効果のカードを複数種類所持している場合一つだけが有効。", add_hp);

	}
	break;
	case ABL_CARD_SPELL:
	{
		int add_sp;

		add_sp = calc_ability_card_add_hp_sp(card_idx, card_num);

		if (only_info) return format(" このカードを所持していると最大MPが%d増加する。MPの非常に低い一部の職には無効。", add_sp);

	}
	break;
	case ABL_CARD_NAZ:
	{

		if (only_info) return format(" このカードには何の使い道もない。");

	}
	break;
	case ABL_CARD_RINGO:
	{
		int v,base = 10;
		{

			if (only_info) return format(" このカードを発動すると%d+1d%dターン腕力・器用・耐久が3ポイント上昇する。通常の限界値を超えない。",base,base);

			v = base + randint1(base);

			set_tim_addstat(A_STR, 3, v, FALSE);
			set_tim_addstat(A_CON, 3, v, FALSE);
			set_tim_addstat(A_DEX, 3, v, FALSE);

		}
		break;

	}
	case ABL_CARD_ONMYOU1:
	{
		int dam = 20 + plev + chr_adj;

		if (only_info) return format(" このカードを発動すると%dダメージの破邪属性のボールを放つ。", dam);

		if (!get_aim_dir(&dir)) return NULL;

		fire_ball(GF_HOLY_FIRE, dir, dam, 1);

	}
	break;
	case ABL_CARD_ONMYOU2:
	{
		int dam = 40 + plev + chr_adj;

		if (only_info) return format(" このカードを発動すると%dダメージの破邪属性のボルトを放つ。", dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_bolt(GF_HOLY_FIRE, dir, dam);

	}
	break;

	case ABL_CARD_HAKKE1:
	{
		int dam = 20 + plev + chr_adj;

		if (only_info) return format(" このカードを発動すると%dダメージの閃光属性のビームを放つ。", dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_beam(GF_LITE, dir, dam);

	}
	break;

	case ABL_CARD_HAKKE2:
	{
		int dam = 65 + plev * 5 / 2 + chr_adj * 2;

		if (only_info) return format(" このカードを発動すると%dダメージのロケットを放つ。", dam);

		if (!get_aim_dir(&dir)) return NULL;
		fire_rocket(GF_ROCKET, dir, dam, 2);

	}
	break;
	case ABL_CARD_KNIFE1:
	{
		//咲夜特技のページから丸コピーした。こういうのは行儀が悪いんだが
		int tx, ty;
		int quality = plev / 5;
		object_type forge;
		object_type *q_ptr = &forge;
		if (only_info) return format("このカードを発動すると1d3(+%d,+%d)のナイフを生成して投擲する。", quality, quality);

		if (!get_aim_dir(&dir)) return NULL;
		if ((dir == 5) && target_okay())
		{
			tx = target_col;
			ty = target_row;
		}
		else
		{
			tx = px + 99 * ddx[dir];
			ty = py + 99 * ddy[dir];
		}
		object_prep(q_ptr, lookup_kind(TV_KNIFE, SV_WEAPON_KNIFE));
		q_ptr->to_d = quality;
		q_ptr->to_h = quality;
		q_ptr->discount = 99;
		object_known(q_ptr);
		msg_print("あなたはナイフを投げた。");
		do_cmd_throw_aux2(py, px, ty, tx, 1, q_ptr, 0);

		break;
	}
	break;

	case ABL_CARD_KNIFE2:
	{
		int dam = 20 + plev * 2 + chr_adj;

		int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_OPT;

		if (only_info) return format(" このカードを発動すると%dダメージの破片属性ボルトを放つ。このボルトが壁に当たるとそこから一度だけランダムな敵のほうへ飛ぶ。", dam);

		if (!get_aim_dir(&dir)) return NULL;

		project_hook(GF_SHARDS, dir, dam, flg);

	}
	break;

	case ABL_CARD_SANAE1:
	{
		int base = 3;
		if (only_info) return format(" このカードを発動すると%d+1d%dターン後にダンジョンから帰還できる。地上からダンジョンに戻るときには使えない。",base,base);

		if (!dun_level)
		{
			msg_print("地上では使えない。");
			return NULL;
		}

		recall_player(base+randint1(base));

	}
	break;

	case ABL_CARD_EIKI:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info) return format(" このカードを所持していると、麻痺や意識不明になったときに即座に復帰できる可能性がある。ただしその時には有り金の%d%%を奪い取られる。",prob );

	}
	break;

	case ABL_CARD_SANAE2:
	{

		int base = 20;

		if (only_info)	return format(" このカードを発動すると%d+1d%dターン毒に対する一時的な耐性を得る。", base, base);

		set_oppose_pois(base + randint1(base), FALSE);

	}
	break;


	case ABL_CARD_ICE_FAIRY:
	{
		int dam = 10 + plev * 3 + chr_adj * 3;

		if (only_info) return format(" このカードを発動すると隣接%dダメージの巨大な極寒属性のボールを発生させる。", dam );

		project(0, 6, py, px, dam*2, GF_ICE, PROJECT_GRID | PROJECT_JUMP | PROJECT_KILL, -1);


	}
	break;

	case ABL_CARD_TANUKI:
	{
		int power = plev + p_ptr->stat_ind[A_CHR]+3;

		if (only_info) return format(" このカードを発動すると周囲の情報を知ることができる。レベルと魅力が高いほど多くの情報が集まる。");
		msg_print("子狸たちが奔走して情報を集めてきた！");

		map_area(DETECT_RAD_DEFAULT);
		if (power>30) detect_monsters_normal(DETECT_RAD_DEFAULT);
		if (power>40) detect_monsters_invis(DETECT_RAD_DEFAULT);
		if (power>50) detect_objects_gold(DETECT_RAD_DEFAULT);
		if (power>60) detect_objects_normal(DETECT_RAD_DEFAULT);
		if (power>70) detect_traps(DETECT_RAD_DEFAULT,TRUE);
		if (power>80) p_ptr->feeling_turn = 0;
	}
	break;



	case ABL_CARD_QUARTER_GHOST:
	{
		int dice = card_num+1;
		int sides = 9;
		if (card_num >= 9) sides=10;

		if (only_info) return format(" このカードを所持していると敵の隣接攻撃に対して地獄属性で%dd%dダメージの反撃を行う。", dice,sides);


	}
	break;

	case ABL_CARD_BACKDOOR:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" このカードを所持していると敵のボルト・ビーム攻撃を%d%%の確率で無効化する。", prob);
	}
	break;
	case ABL_CARD_DANMAKUGHOST:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" このカードを所持していると敵のボール攻撃を%d%%の確率で無効化する。", prob);
	}
	break;
	case ABL_CARD_BASS_DRUM:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" このカードを所持していると敵のロケット攻撃を%d%%の確率で無効化する。", prob);
	}
	break;
	case ABL_CARD_PSYCHOKINESIS:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" このカードを所持していると敵のブレス攻撃を%d%%の確率で無効化する。", prob);
	}
	break;
	case ABL_CARD_UFO:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);
		if (only_info)return format(" このカードを所持していると敵の投射系攻撃魔法やブレスなどを%d%%の確率で無効化する。", prob);
	}
	break;


	case ABL_CARD_MANEKINEKO:
	{
		if (only_info)return format(" このカードを所持していると店の商品が入れ替わりやすくなり、商品の質が少し良くなる。");
	}
	break;

	case ABL_CARD_YAMAWARO:
	{
		if (only_info)return format(" このカードを所持していると店に新たに並ぶ商品が値引きされやすくなる。");
	}
	break;

	case ABL_CARD_SYUSENDO:
	{
		if (only_info)
		{
			if(card_num == 1)
				return format(" このカードを所持していると泥棒に物を盗まれなくなる。");
			else if (card_num < 9)
				return format(" このカードを所持していると泥棒に物を盗まれなくなる。さらに泥棒が逃げ出すことを%d%%の確率で妨害する。", (card_num-1) * 10);
			else
				return format(" このカードを所持していると泥棒に物を盗まれなくなる。さらに泥棒が逃げ出すことを妨害する。");
		}
	}
	break;

	case ABL_CARD_HANIWA:
	{
		int sav_bonus = calc_ability_card_prob(card_idx, card_num);
		if (only_info)
			return format(" このカードを所持していると魔法防御能力が%d上昇する。",sav_bonus);

	}
	break;

	case ABL_CARD_MAGATAMA:
	{
		int ac_bonus = calc_ability_card_prob(card_idx, card_num);

		if (only_info)
			return format(" このカードを所持しているとACが%d上昇する。",ac_bonus);
	}
	break;

	case ABL_CARD_YACHIE:
	{
		int percen=10;

		if(card_num>1)percen += (card_num-1) * 2;
		if (card_num >= 9) percen = 30;

		if (only_info)
			return format(" このカードを所持していると敵を倒したとき得られる金やアイテムが%d%%増加する。金やアイテムを落とさない敵には効果がない。",percen);

	}
	break;

	case ABL_CARD_SYANHAI:
	{

		if (only_info)	return format(" このカードを発動すると「人形」を一体配下として召喚する。");

		if (summon_named_creature(0, py, px, MON_ALICE_DOLL, PM_FORCE_PET))
			msg_print("人形を放った。");

	}
	break;

	case ABL_CARD_LILYWHITE:
	{
		bool flag = FALSE;
		if (only_info)	return format(" このカードを発動すると敵対的な『リリーホワイト』を召喚する。リリーホワイトは呼ぶたびに強くなる。");

		flag_generate_lilywhite = o_ptr->xtra3+1;
		if (summon_named_creature(0, py, px, MON_RANDOM_UNIQUE_2, (PM_FORCE_ENEMY|PM_NO_ENERGY))) flag=TRUE;
		flag_generate_lilywhite = 0;

		if (flag)
		{
			msg_print("リリーホワイトが襲ってきた！");
			if (o_ptr->xtra3 < 99) o_ptr->xtra3++;
		}
		else
		{
			msg_print("何も起こらなかった。");
		}

	}
	break;

	case ABL_CARD_EIRIN_DRUG:
	{
		int base = 4;

		if (only_info)	return format(" このカードを発動すると%d+1d%dターン無敵化する。MP200を消費する。",base,base);

		if (p_ptr->csp < 200)
		{
			msg_print("MPが足りない。");
			return NULL;
		}
		p_ptr->csp -= 200;
		p_ptr->redraw |= PR_MANA;

		set_invuln(base + randint1(base), FALSE);


	}
	break;
	case ABL_CARD_INABA_LEG:
	{
		int prob = 25;

		if (card_num > 1) prob += (card_num - 1) * 8;
		if (card_num >= 9) prob = 100;

		if (only_info)	return format(" このカードを所持しているとザックの中のアイテムが%d%%の確率で属性攻撃による破壊から守られる。", prob);

	}
	break;

	case ABL_CARD_SAKI:
	{
		int bonus = 10;
		if (card_num > 1) bonus += (card_num - 1) * 2;
		if (card_num >= 9) bonus = 32;

		if (only_info)	return format(" このカードを所持していると隣接攻撃に(+%d,+%d)のボーナスを得る。", bonus,bonus);


	}
	break;

	case ABL_CARD_KOISHI:
	{
		int base = 20;

		if (only_info)	return format(" このカードを発動すると%d+1d%dターン敵に認識されづらくなる。", base, base);

		set_tim_superstealth(base + randint1(20), FALSE, SUPERSTEALTH_TYPE_NORMAL);

	}
	break;

	case ABL_CARD_KISERU:
	{
		int base = 15;
		int v;

		if (only_info)	return format(" このカードを発動すると%d+1d%dターン士気高揚、加速、狂戦士化する。", base, base);

		v = base + randint1(base);
		set_hero(v, FALSE);
		set_fast(v, FALSE);
		set_shero(v, FALSE);
	}
	break;


	case ABL_CARD_JIZO:
	{
		int percen = calc_ability_card_prob(card_idx, card_num);

		if (only_info)	return format(" このカードを所持しているとHPの自然回復量が%d%%増加する。急回復に累積して効果が出る。すでに同様の能力を持っている場合は効果がない。", percen);
	}

		break;

	case ABL_CARD_PATHE_SPELL:
	{
		int percen = calc_ability_card_prob(card_idx, card_num);

		if (only_info)	return format(" このカードを所持しているとMPの自然回復量が%d%%増加する。急回復に累積して効果が出る。すでに同様の能力を持っている場合は効果がない。", percen);
	}
		break;


	case ABL_CARD_AKAGAERU:
	{
		int mult= 75;
		int rad = 1;

		if (card_num > 1) mult += (card_num - 1) * 7;
		if (card_num >= 9)
		{
			rad = 2;
			mult = 150;
		}
		if (only_info) return format("このカードを所持しているとプレイヤーが使用したボルト系攻撃が半径%dのロケットに変化する。威力は%d%%になる。",rad,mult);
	}
	break;

	case ABL_CARD_TENGU_GETA:
	{
		int len = 3 + plev / 7;

		if (only_info) return format("このカードを発動すると%dグリッドを一瞬で移動し、敵に当たったらそのまま通常攻撃を行う。", len);
		if (!rush_attack(NULL, len, 0)) return NULL;
	}
	break;

	case ABL_CARD_MUKADE:
	{
		int border_lis[10] = { 100,95,92,89,86,86,83,80,77,75 };
		int add_lis[10]		= { 1,1,1,1,1,2,2,2,2,3 };

		if (only_info) return format("このカードを所持していると、HPが最大値の%d%%以上のときに限り隣接攻撃回数が%d増える。", border_lis[card_num], add_lis[card_num]);

	}
	break;

	case ABL_CARD_SUKIMA:
	{

		if (only_info) return format("このカードを発動すると隣接した壁などの向こうへ瞬間移動する。マップ端に到達すると反対側へ出る。通常のテレポートで移動できない場所には出られない。");

		if (!wall_through_telepo(0)) return NULL;

	}
	break;

	case ABL_CARD_KODUCHI:
	{

		if (only_info) return format("このカードを発動するとアイテムをひとつ選択して1/3の価値の＄に変えることができる。");

		alchemy();
	}
	break;

	case ABL_CARD_KANAMEISHI:
	{

		int y, x;
		monster_type *m_ptr;
		int damage = plev * 3;

		if (only_info) return format("このカードを発動すると視界内の床一箇所を「岩石」地形に変える。指定位置にモンスターがいた場合%dのダメージを与える。",damage);


		if (!get_aim_dir(&dir)) return NULL;
		if (dir != 5 || !target_okay() || !projectable(target_row, target_col, py, px))
		{
			msg_print("視界内のターゲットを明示的に指定しないといけない。");
			return NULL;
		}
		y = target_row;
		x = target_col;

		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx)
		{
			if (!cave_naked_bold(y, x))
			{
				msg_print("そこには何かがあるようだ。やめておこう。");
				return NULL;
			}
			msg_print("地面に石塊を積み重ねた。");
			cave_set_feat(y, x, f_tag_to_index_in_init("RUBBLE"));
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);

			msg_format("%sの頭上に要石が落ちてきた！", m_name);
			project(0, 0, y, x, damage, GF_ARROW, PROJECT_KILL, -1);
		}

		break;



	}
	break;

	case ABL_CARD_LUNATIC:
	{
		int base = 100 + plev * 6;
		int sides = 100 + chr_adj * 10;

		if (only_info) return format("このカードを発動すると指定したグリッドに%d+1d%dダメージの隕石属性のボールで攻撃する。", base,sides);

		if (!get_aim_dir(&dir)) return NULL;
		if(!fire_ball_jump(GF_METEOR, dir, base + randint1(sides), 3, NULL)) return NULL;

	}
	break;

	case ABL_CARD_MIKO:
	{
		int base = 50 + plev + chr_adj * 5;
		int time = 15 + randint1(15);
		if (only_info) return format("このカードを発動すると視界内すべてに対し%dダメージの閃光属性攻撃を行う。さらに一時的に対邪悪結界と聖なるオーラを得る。", base);

		project_hack2(GF_LITE, 0, 0, base);
		set_protevil(time, FALSE);
		set_tim_sh_holy(time, FALSE);

	}
	break;

	case ABL_CARD_VAMPIRE_FANG:
	{
		int dice = plev+10;
		int sides = 10 + chr_adj / 5;
		int y, x;
		monster_type *m_ptr;

		if (only_info) return format("このカードを発動すると隣接したモンスター一体に%dd%dの無属性ダメージを与え、さらに耐性を無視して朦朧とさせる。", dice,sides);

		if (!get_rep_dir2(&dir)) return NULL;
		if (dir == 5) return NULL;
		y = py + ddy[dir];
		x = px + ddx[dir];
		m_ptr = &m_list[cave[y][x].m_idx];

		if (!m_ptr->r_idx || !m_ptr->ml)
		{
			msg_format("そこには何もいない。");
			return NULL;
		}
		else
		{
			char m_name[80];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
			msg_format("%sへ牙を突き立てた！", m_name);

			project(0, 0, y, x, damroll(dice, sides), GF_MISSILE, PROJECT_KILL | PROJECT_JUMP, -1);
			if (m_ptr->r_idx)
			{
				msg_format("%sはフラフラになった。", m_name);
				(void)set_monster_stunned(cave[y][x].m_idx, MON_STUNNED(m_ptr) + 4 + randint0(4));
			}
		}

	}
	break;

	case ABL_CARD_SUBTERRANEAN:
	{
		int dam = 500 + plev * 10 + chr_adj * 20;

		if (only_info) return format("このカードを発動すると視界内すべてに対し%dダメージの核熱属性攻撃を行う。", dam);

		project_hack2(GF_NUKE, 0, 0, dam);

	}
	break;

	case ABL_CARD_MUGIMESHI:
	{
		int heal = 400;

		if (only_info) return format("このカードを発動するとHPを%d回復し、毒と切り傷を治療し、満腹になり、低下している肉体能力を復活させる。",heal);

		hp_player(400);
		set_poisoned(0);
		set_cut(0);
		set_food(PY_FOOD_MAX - 1);
		do_res_stat(A_STR);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);

	}
	break;

	case ABL_CARD_KAMIYAMA:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info) return format("このカードを持っていると、特定の固定アーティファクトを持つモンスターを倒したときにそのアーティファクトを落とす確率が%d.%02d倍になる。",(prob/100+1),(prob%100));

	}
	break;

	case ABL_CARD_TSUKASA:
	{

		if (only_info) return format("このカードを発動すると最大HPの1/3のHPを消費して同量のMPを回復する。一時的に腕力・器用・耐久が低下することがある。");

		if (p_ptr->pclass == CLASS_CLOWNPIECE)
		{
			msg_print("あなたはこのカードを使えない。");
			return NULL;
		}

		if (p_ptr->chp < p_ptr->mhp/3)
		{
			msg_print("カードを使えない。体力が少なすぎる！");
			return NULL;
		}
		take_hit(DAMAGE_USELIFE, p_ptr->mhp / 3, "管狐のカード", -1);
		player_gain_mana(p_ptr->mhp/3);

		if (one_in_(2)) do_dec_stat(A_STR);
		if (one_in_(2)) do_dec_stat(A_DEX);
		if (one_in_(2)) do_dec_stat(A_CON);

	}
	break;

	case ABL_CARD_KYOUTEN:
	{
		int percen = CALC_ABL_KYOUTEN_RECHARGE_BONUS(card_num);


		if (only_info) return format("このカードを所持していると魔道具の自然充填が%d%%早くなる。",percen);
	}
	break;

	case ABL_CARD_KAGUYA:
	{
		int prob = calc_ability_card_prob(card_idx, card_num);

		if (only_info) return format("このカードを所持していると、敵の攻撃でダメージを受けたときその%d%%のMPを回復する。またMP減少打撃に対して%d%%の耐性を得る。",prob,prob*2);


	}
	break;


	default:
		if (only_info) return "未実装";

		msg_format("ERROR:実装されていないアビリティカードが使われた (idx:%d)", card_idx);
		return NULL;
	}

	return ""; //発動成功するとNULLでない適当な空文字を返す

}




//アビリティカードを使用する。カードはすでに選択されている。成功判定、チャージ減少処理を行い実行部分に渡す。
//行動順消費しないときのみFALSEを返す。
//v1.1.94 発動成功後にターゲットキャンセルした場合行動力を消費しないものとする
bool use_ability_card(object_type *o_ptr)
{
	ability_card_type *ac_ptr;

	int chance = p_ptr->skill_dev;
	int fail;
	bool success;

	if (world_player || SAKUYA_WORLD)
	{
		if (flush_failure) flush();
		msg_print("止まった時の中ではうまく働かないようだ。");

		sound(SOUND_FAIL);
		return FALSE;
	}

	if (o_ptr->tval != TV_ABILITY_CARD)
	{
		msg_format("ERROR:use_ability_card()にtval%dのアイテムが渡された", o_ptr->tval);
		return FALSE;
	}

	if (o_ptr->pval < 0 || o_ptr->pval >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:use_ability_card()に不正なカードidx(%d)を持ったアビリティカードが渡された", o_ptr->pval);
		return FALSE;
	}

	ac_ptr = &ability_card_list[o_ptr->pval];


	//難易度計算　ロッドと同じ計算を使う
	if (p_ptr->confused) chance /= 2;
	fail = ac_ptr->difficulty + 5;
	if (chance > fail) fail -= (chance - fail) * 2;
	else chance -= (fail - chance) * 2;
	if (fail < USE_DEVICE) fail = USE_DEVICE;
	if (chance < USE_DEVICE) chance = USE_DEVICE;


	//成功判定　ロッドと同じ計算を使う
	//カード売人(仮)のみ性格狂気でもカードを使えるようにしてもいい
	if (p_ptr->pseikaku == SEIKAKU_BERSERK) success = FALSE;
	else if (chance > fail)
	{
		if (randint0(chance * 2) < fail) success = FALSE;
		else success = TRUE;
	}
	else
	{
		if (randint0(fail * 2) < chance) success = TRUE;
		else success = FALSE;
	}

	if (p_ptr->pclass == CLASS_HIGH_MAGE && p_ptr->lev > 39 && p_ptr->realm1 == TV_BOOK_ENCHANT) success = TRUE;

	if (!success)
	{
		if (flush_failure) flush();
		msg_print("カードの発動に失敗した。");

		sound(SOUND_FAIL);
		return TRUE;
	}

	if (o_ptr->timeout > ac_ptr->charge_turn * (o_ptr->number - 1))
	{
		if (flush_failure) flush();
		msg_print("このカードはまだチャージが終わっていない。");

		return FALSE;
	}

	if ((o_ptr->timeout + ac_ptr->charge_turn) > 32767)
	{
		msg_format("ERROR:timeout値がオーバーフローする。");
		return FALSE;
	}

	/* Sound */
	sound(SOUND_ZAP);


	//(未鑑定ロッドの判明に相当する処理はない。カードは生成時に常に*鑑定*されているため)

	
	//msg_format("あなたは『%s』のアビリティカードを発動した！", ac_ptr->card_name);
	//↑メッセージが冗長なので消す

	//実行部分。ターゲットキャンセルなどしたらNULLが返り行動消費しない
	if (!use_ability_card_aux(o_ptr, FALSE)) return FALSE;

	//チャージタイム加算
	o_ptr->timeout += ac_ptr->charge_turn;
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	return TRUE;

}


//アビリティカードの「高騰度」を設定する。
//ゲーム開始直後を含む6:00と18 : 00に呼ばれる。
//宿に連続で泊まったら宿を出る瞬間に一度だけしか呼ばれないが多分実用上の問題はないだろう
void set_abilitycard_price_rate()
{

	int mod = 0;
	int prev_rate = p_ptr->abilitycard_price_rate;

	//ゲーム開始から半日経過ごとにカウント
	int half_days = turn / ((TURNS_PER_TICK * TOWN_DAWN) / 2);

	//EXTRAモードでは一律階層/2
	if (EXTRA_MODE)
	{
		p_ptr->abilitycard_price_rate = (dun_level + 1) / 2;
		return;
	}

	//高騰度10からゲーム開始
	if (!half_days)
	{
		mod = 10;
	}
	//3日目晩までは+1
	else if (half_days < 6)
	{
		mod = 1;
	}
	//7日目晩までは+1+1d2 
	else if (half_days < 14)
	{
		mod = 1+randint1(2);
	}
	//14日目晩までは+15%〜20%(14日終了時点で300くらいになる。その頃にはガチャを引いたほうが割がいいはず)
	else if (half_days < 28)
	{
		mod = prev_rate * 14 / 100 + randint1(prev_rate * 6 / 100);
	}
	//その後は高騰度300〜600くらいの範囲内で乱高下。
	else
	{
		mod = damroll(3, 100) - 150;

		if ((prev_rate + mod) < 300)
		{
			if (mod < 0 && !one_in_(3)) mod *= -1; //あまり下には張り付きにくい
			//else mod = prev_rate - 300; 派手に計算間違ってた
			else mod = 300 - prev_rate;
		}
		else if ((prev_rate + mod) > 600)
		{
			mod = 600 - prev_rate;
		}

	}



	p_ptr->abilitycard_price_rate += mod;

	if (p_ptr->abilitycard_price_rate < 0)
	{
		p_ptr->abilitycard_price_rate = 300;
		msg_print("ERROR:abilitycard_price_rateがマイナスになった");

	}

	if (CHECK_ABLCARD_DEALER_CLASS && cheat_xtra) 
		msg_format("testmsg:アビリティカード高騰度更新:%d(%d)", p_ptr->abilitycard_price_rate, mod);


}

//カード販売所のリストを作る
//ゲーム開始直後を含む6:00と18:00に呼ばれる
//リストはp_ptr->magic_num2[0-9]に格納
//↑職によっては使用箇所をずらす
void make_ability_card_store_list(void)
{
	object_type forge;
	object_type *o_ptr = &forge;
	int i;

	//カード売人系のクラスのみ
	if (!CHECK_ABLCARD_DEALER_CLASS) return;

	for (i = 0; i < 10; i++)
	{
		int tmp;

		//ダミーカード生成 ＠のレベルが高いほど高ランクのものが出やすい
		object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
		tmp = randint1(p_ptr->lev);
		if(tmp >= 45 )
			apply_magic_abilitycard(o_ptr, -1,4, 0);
		else if(tmp >= 30)
			apply_magic_abilitycard(o_ptr, -1, 3, 0);
		else if (tmp >= 15)
			apply_magic_abilitycard(o_ptr, -1, 2, 0);
		else
			apply_magic_abilitycard(o_ptr, -1, 0, 0);


		//カードの種別(pval値)を記録
		p_ptr->magic_num2[i+ ABLCARD_MAGICNUM_SHIFT] = o_ptr->pval;

	}


}

//アビリティカードの販売価格を計算
int calc_ability_card_price(int card_idx)
{
	int price;
	if (card_idx < 0 || card_idx >= ABILITY_CARD_LIST_LEN)
	{
		msg_format("ERROR:calc_ability_card_price()に渡されたcard_idx(%d)が範囲外", card_idx);
		return 0;
	}

	price = 300 + p_ptr->abilitycard_price_rate * ability_card_list[card_idx].trade_value;

	if (ability_card_list[card_idx].rarity_rank == 4) price *= 30;
	else if (ability_card_list[card_idx].rarity_rank == 3) price *= 10;
	else if (ability_card_list[card_idx].rarity_rank == 2) price *= 3;

	return price;

}



//隣接したモンスター一体を指定し、そのモンスターにちなんだカードがあれば買い取る。
//買えるのはカード一種類につき一枚まで。p_ptr->magic_num1[]に買取済みのカードを記録する。
//行動順消費するときTRUE
bool	buy_abilitycard_from_mon(void)
{
	int i;
	int price = 0;
	int dir;
	monster_type *m_ptr;
	int y, x;
	char m_name[160];

	object_type forge;
	object_type *o_ptr = &forge;


	if (!CHECK_ABLCARD_DEALER_CLASS)
	{
		msg_print("ERROR:カード売人系以外のクラスでbuy_abilitycard_from_mon()が呼ばれた");
		return FALSE;
	}
	if (inventory[INVEN_PACK - 1].k_idx)
	{
		msg_print("今は荷物が一杯だ。");
		return FALSE;
	}

	//隣接モンスター指定
	if (!get_rep_dir2(&dir)) return FALSE;
	if (dir == 5) return FALSE;

	y = py + ddy[dir];
	x = px + ddx[dir];
	m_ptr = &m_list[cave[y][x].m_idx];

	if (!cave[y][x].m_idx || !m_ptr->ml)
	{
		msg_format("そこには何もいない。");
		return FALSE;
	}

	//起こす
	set_monster_csleep(cave[y][x].m_idx, 0);

	monster_desc(m_name, m_ptr, 0);
	if (p_ptr->inside_arena)
	{
		msg_format("夢の世界の%sは好戦的だ！交渉どころではない！", m_name);
		return TRUE;
	}
	msg_format("あなたは%sにカードの買取りを持ち掛けた...", m_name);

	//カードリストループ。購入可能なカードがあればiにability_card_list[]のインデックスが入ってループから出る
	for (i = 0; i<ABILITY_CARD_LIST_LEN; i++)
	{
		//カードリストに登録された特定モンスターのみ有効
		if (ability_card_list[i].r_idx != m_ptr->r_idx) continue;

		//EXTRAモードでは帰還用カードが出ない
		if (EXTRA_MODE && i == ABL_CARD_SANAE1) continue;

		//売却済みフラグが立っているカードはもう出ない。符号付き変数を無理やりビットフラグに使うのはなんか不安なので下位16bitだけフラグに使う。
		if ((p_ptr->magic_num1[(i / 16)+ ABLCARD_MAGICNUM_SHIFT] >> (i % 16)) & 1L) continue;

		break;
	}

	if (i >= ABILITY_CARD_LIST_LEN)
	{
		msg_print("カードを持っていないようだ。");
		return TRUE;
	}

	//カードを持っているとき、価格を決定して確認を取る
	//価格は典から買う2/3
	price = calc_ability_card_price(i) * 2 / 3;

	//v1.1.93 ミケが特技でフロアに呼んだ客は価格を吹っかけてくる ランダムに二倍から三倍
	if (p_ptr->pclass == CLASS_MIKE && m_ptr->mflag & MFLAG_SPECIAL)
	{
		if (cheat_xtra) msg_print("(価格上昇)");
		price = price * (200 + m_ptr->mon_random_number % 100) / 100;

	}

	if (!get_check_strict(format("「%s」のカードを$%dで売ってくれるようだ。買いますか？", ability_card_list[i].card_name, price), CHECK_DEFAULT_Y))
	{
		msg_print("交渉は決裂した！");
		return TRUE;
	}

	if (p_ptr->au < price)
	{
		msg_print("お金が足りない！");
		return TRUE;
	}

	//所持金を減らしてカード入手
	p_ptr->au -= price;
	p_ptr->redraw |= (PR_GOLD);
	object_prep(o_ptr, lookup_kind(TV_ABILITY_CARD, SV_ABILITY_CARD));
	apply_magic_abilitycard(o_ptr, i, 0, 0);
	inven_carry(o_ptr);
	msg_format("%sからカードを買い取った！", m_name);

	//入手済みフラグを立てる
	p_ptr->magic_num1[(i / 16)+ ABLCARD_MAGICNUM_SHIFT] |= ((1L) << (i % 16));

	//売買が成立したときモンスターを友好的にする。ただしクエストダンジョン内やクエスト討伐対象やダンジョンボスの場合敵対したまま
	//↑護衛持ちモンスターが売買成立した途端配下からタコ殴りにされるのがなんか変なので中止
	//if (!(r_info[m_ptr->r_idx].flags1 & RF1_QUESTOR) && !p_ptr->inside_quest)
	//{
	//		set_friendly(m_ptr);
	//}

	return TRUE;
}

